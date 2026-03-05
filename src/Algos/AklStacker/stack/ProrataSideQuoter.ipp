#pragma once

#include "ProrataSideQuoter.hpp"
#include "../pricing/ProrataPricingModel.hpp"
#include "../Time.hpp"
#include "../IExecutionInterface.hpp"

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
ProrataSideQuoter<SIDE>::ProrataSideQuoter(
	const QuoteInstrumentSettings &quoteSettings,
	const Time &time,
	IExecutionInterface &execution)
: quoteSettings(quoteSettings)
, time(time)
, execution(execution)
, cachedOrders(MAX_NUM_ORDERS)
{
	workingOrders.reserve(MAX_NUM_ORDERS);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool ProrataSideQuoter<SIDE>::HasOrders() const
{
	return !workingOrders.empty();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool ProrataSideQuoter<SIDE>::Update(const std::optional<ProrataTarget> &target, const bool doRequote)
{
	bool result = false;

	if (doRequote)
	{
		manualCancel = false;
		for (ProrataOrder &po : workingOrders)
		{
			if (po.o == nullptr)
			{
				po.allowRequote = true;
			}
			po.numRequotes = 0;
		}
		for (ProrataOrder &po : cachedOrders)
		{
			if (po.o == nullptr)
			{
				po.allowRequote = true;
			}
			po.numRequotes = 0;
		}
	}

	if (target)
	{
		if (currentPx)
		{
			if (*currentPx != target->price)
			{
				popNullOrders();
				if (!HasOrders())
				{
					currentPx = target->price;
				}
			}
		}
		else
		{
			currentPx = target->price;
		}

		if (*currentPx == target->price)
		{
			if (target->quoteQuantities.Size() > 0)
			{
				result = updateOrders(*target);
				currentTarget = target;
				lastQuotePx = *currentPx;
			}
			else
			{
				result = cancel(true);
			}
		}
		else
		{
			result = cancel(true);
		}
	}
	else
	{
		if (currentPx)
		{
			popNullOrders();
			if (HasOrders())
			{
				result = cancel(true);
			}
			else
			{
				currentPx.reset();
				currentTarget.reset();
			}
		}
	}
	
	return result;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataSideQuoter<SIDE>::Cancel(bool scratchFillsImmediately)
{
	cancel(true, scratchFillsImmediately);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataSideQuoter<SIDE>::Cancel(const Price px)
{
	if (currentPx && *currentPx == px)
	{
		Cancel(true);
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool ProrataSideQuoter<SIDE>::RemoveOrder(Order *o, const bool cancelAck)
{
	typename OrderListType::iterator it = std::find_if(workingOrders.begin(), workingOrders.end(), [o](const ProrataOrder &po) { return po.o == o; });
	if (it != workingOrders.end())
	{
		it->o = nullptr;
		it->timeToClear = Time::Max();
		if (cancelAck)
		{
			if (!it->strategyCancel)
			{
				manualCancel = true;
			}
			else if (!manualCancel && !o->scratchFillsImmediately)
			{
				it->allowRequote = true;
			}
		}
		popNullOrders();
		if (workingOrders.empty())
		{
			currentTarget.reset();
		}
		return true;
	}
	else
	{
		return false;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataSideQuoter<SIDE>::LogOrders() const
{
	const std::string sideString = KTN::ORD::KOrderSide::toString(SIDE);
	if (workingOrders.empty())
	{
		LOGINFO("{} side has no prorata orders", sideString);
	}
	else
	{
		for (size_t i = 0; i < workingOrders.size(); ++i)
		{
			const ProrataOrder &po = workingOrders[i];
			LOGINFO("{} prorata order {}: oid={} | allowRequote=%{} | strategyCancel=%{}",
				sideString,
				static_cast<int>(i),
				po.o != nullptr ? po.o->orderReqId : std::numeric_limits<uint64_t>::max(),
				static_cast<uint16_t>(po.allowRequote),
				static_cast<uint16_t>(po.strategyCancel));
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataSideQuoter<SIDE>::GetOrders(std::vector<Order *> &orders) const
{
	for (typename OrderListType::const_iterator it = workingOrders.cbegin(); it != workingOrders.cend(); ++it)
	{
		const ProrataOrder &po = *it;
		if (po.o != nullptr)
		{
			orders.push_back(po.o);
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
Order *ProrataSideQuoter<SIDE>::GetOrder(const uint64_t orderReqId)
{
	for (typename OrderListType::iterator it = workingOrders.begin(); it != workingOrders.end(); ++it)
	{
		ProrataOrder &po = *it;
		if (po.o != nullptr && po.o->cmeId == orderReqId)
		{
			return po.o;
		}
	}
	return nullptr;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool ProrataSideQuoter<SIDE>::AbandonOrder(Order *o)
{
	for (typename OrderListType::iterator it = workingOrders.begin(); it != workingOrders.end(); ++it)
	{
		ProrataOrder &po = *it;
		if (po.o == o)
		{
			po.o->type = Order::Type::NONE;
			po.o = nullptr;
			po.allowRequote = false;
			po.strategyCancel = false;
			popNullOrders();
			return true;
		}
	}
	return false;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataSideQuoter<SIDE>::ReleaseAllOrders()
{
	for (typename OrderListType::iterator it = workingOrders.begin(); it != workingOrders.end(); ++it)
	{
		ProrataOrder &po = *it;
		if (po.o != nullptr)
		{
			po.o->type = Order::Type::NONE;
			po.o = nullptr;
			po.allowRequote = false;
			po.strategyCancel = false;
		}
		po.numRequotes = QuoteInstrumentSettings::MAX_REQUOTE_COUNT;
	}

	for (typename OrderListType::iterator it = cachedOrders.begin(); it != cachedOrders.end(); ++it)
	{
		ProrataOrder &po = *it;
		po.numRequotes = QuoteInstrumentSettings::MAX_REQUOTE_COUNT;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool ProrataSideQuoter<SIDE>::HasOrder(const Order *o) const
{
	for (typename OrderListType::const_iterator it = workingOrders.cbegin(); it != workingOrders.cend(); ++it)
	{
		const ProrataOrder &po = *it;
		if (po.o == o)
		{
			return true;
		}
	}
	return false;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool ProrataSideQuoter<SIDE>::updateOrders(const ProrataTarget &target)
{
	if (lastQuotePx && *lastQuotePx != target.price)
	{
		if (quoteSettings.prorataRequoteCount > 0 && !manualCancel)
		{
			for (ProrataOrder &po : workingOrders)
			{
				if (po.o == nullptr && !po.allowRequote && po.numRequotes < quoteSettings.prorataRequoteCount)
				{
					po.allowRequote = true;
					++po.numRequotes;
				}
			}
			for (ProrataOrder &po : cachedOrders)
			{
				if (po.o == nullptr && !po.allowRequote && po.numRequotes < quoteSettings.prorataRequoteCount)
				{
					po.allowRequote = true;
					++po.numRequotes;
				}
			}
		}
	}
	lastQuotePx = target.price;

	bool result = false;

	Quantity totalTargetQty { 0 };
	for (int i = 0; i < target.quoteQuantities.Size(); ++i)
	{
		totalTargetQty += std::min(target.quoteQuantities[i], quoteSettings.riskSettings.maxOrderQty);
	}

	Quantity currentQty { 0 };
	for (typename OrderListType::iterator it = workingOrders.begin(); it != workingOrders.end(); ++it)
	{
		ProrataOrder &po = *it;
		if (po.o != nullptr)
		{
			if (po.o->IsWorking())
			{
				currentQty += std::max(po.o->targetQuantity, po.o->ackedQuantity);
			}
			else
			{
				if (po.timeToClear == Time::Max())
				{
					po.timeToClear = time.Now() + Seconds(2);
				}
				else if (time.Now() >= po.timeToClear)
				{
					LOGWARN("{} prorata quoter cleared order {} because it is not working",
						KTN::ORD::KOrderSide::toString(SIDE), po.o->orderReqId);
					po.o = nullptr;
					po.timeToClear = Time::Max();
				}
			}
		}
	}

	int orderIndex { 0 };
	int targetIndex { 0 };
	Quantity additional { 0 };
	while (orderIndex < static_cast<int>(workingOrders.size()) && targetIndex < target.quoteQuantities.Size())
	{
		ProrataOrder &po = workingOrders[orderIndex];
		const Quantity targetQty = std::min(target.quoteQuantities[targetIndex], quoteSettings.riskSettings.maxOrderQty);
		if (po.o != nullptr)
		{
			if (!po.o->pendingCancel)
			{
				if (po.o->targetQuantity < targetQty)
				{
					const Quantity additionalOrderQty = targetQty - po.o->targetQuantity;
					if (currentQty + additional + additionalOrderQty <= totalTargetQty)
					{
						execution.Modify(po.o, targetQty);
						result = true;
						additional += additionalOrderQty;
					}
				}
				else if (po.o->targetQuantity > targetQty)
				{
					if (targetQty > Quantity(0))
					{
						execution.Modify(po.o, targetQty);
						result = true;
					}
					else
					{
						if (!po.o->pendingCancel)
						{
							execution.Cancel(po.o);
							po.strategyCancel = true;
							result = true;
						}
					}
				}
			}
		}
		else if (canRequote(po))
		{
			if (targetQty > Quantity(0) && currentQty + additional + targetQty <= totalTargetQty)
			{
				Order *o = execution.CreateOrder(SIDE, targetQty, *currentPx);
				if (likely(o != nullptr))
				{
					o->type = Order::Type::PRORATA;
					if (execution.CanSubmit(o))
					{
						execution.Submit(o);
						po.o = o;
						po.allowRequote = false;
						po.strategyCancel = false;
						po.timeToClear = Time::Max();
						result = true;
					}
				}
			}
			additional += targetQty;
		}
		++orderIndex;
		++targetIndex;
	}

	while (orderIndex < static_cast<int>(workingOrders.size()))
	{
		ProrataOrder &po = workingOrders[orderIndex];
		if (po.o != nullptr)
		{
			if (!po.o->pendingCancel)
			{
				execution.Cancel(po.o);
				po.strategyCancel = true;
				result = true;
			}
		}
		++orderIndex;
	}

	while (targetIndex < target.quoteQuantities.Size())
	{
		if (!cachedOrders.empty())
		{
			ProrataOrder po = cachedOrders.back();
			cachedOrders.pop_back();
			if (canRequote(po))
			{
				const Quantity targetQty = std::min(target.quoteQuantities[targetIndex], quoteSettings.riskSettings.maxOrderQty);
				if (targetQty > Quantity(0) && currentQty + additional + targetQty <= totalTargetQty)
				{
					Order *o = execution.CreateOrder(SIDE, targetQty, *currentPx);
					if (likely(o != nullptr))
					{
						o->type = Order::Type::PRORATA;
						if (execution.CanSubmit(o))
						{
							execution.Submit(o);
							po.o = o;
							po.allowRequote = false;
							po.strategyCancel = false;
							po.timeToClear = Time::Max();
							result = true;
						}
					}
					additional += targetQty;
				}

			}
			workingOrders.push_back(po);
		}
		++targetIndex;
	}

	return result;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool ProrataSideQuoter<SIDE>::cancel(const bool strategyCancel, bool scratchFillsImmediately)
{
	bool result = false;
	for (typename OrderListType::iterator it = workingOrders.begin(); it != workingOrders.end(); ++it)
	{
		ProrataOrder &po = *it;
		if (po.o != nullptr)
		{
			if (!po.o->pendingCancel)
			{
				execution.Cancel(po.o);
			}
			po.o->scratchFillsImmediately |= scratchFillsImmediately;
			po.strategyCancel = strategyCancel;
			result = true;
		}
	}
	if (scratchFillsImmediately)
	{
		for (typename OrderListType::iterator it = cachedOrders.begin(); it != cachedOrders.end(); ++it)
		{
			it->allowRequote = false;
		}
	}
	return result;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataSideQuoter<SIDE>::popNullOrders()
{
	while (!workingOrders.empty() && workingOrders.back().o == nullptr)
	{
		ProrataOrder &po = workingOrders.back();
		cachedOrders.push_back(po);
		workingOrders.pop_back();
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool ProrataSideQuoter<SIDE>::canRequote(const ProrataOrder &po) const
{
	return po.allowRequote && quoteSettings.newOrders && !manualCancel;
}

}
