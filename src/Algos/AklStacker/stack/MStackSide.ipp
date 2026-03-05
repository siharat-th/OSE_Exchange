#pragma once

#include "MStackSide.hpp"
#include "../IExecutionInterface.hpp"
#include <AlgoParams/AklStackerParams.hpp>
#include "../Order.hpp"

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
MStackSide<SIDE>::MStackSide(
	const QuoteInstrumentSettings &quoteSettings,
	const StackSideSettings &sideSettings,
	IExecutionInterface &execution,
	const Price tickIncrement,
	const int stackIndex)
: quoteSettings(quoteSettings)
, sideSettings(sideSettings)
, execution(execution)
, tickIncrement(tickIncrement)
, stackIndex(stackIndex)
{

}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool MStackSide<SIDE>::Update(const std::optional<Price> &mPx, const std::optional<Price> &stackPx)
{
	bool result = false;

	if (mPx)
	{
		//const StackSideSettings &sideSettings = stackSettings.GetStackSideSettings<SIDE>();
		typename BookType::iterator it = orderBook.begin();
		while (it != orderBook.end()
			&& (SideTraits<SIDE>::IsBetter(it->first, *mPx)
				|| (sideSettings.outsidePrice && SideTraits<SIDE>::IsWorse(it->first, *sideSettings.outsidePrice))))
		{
			LevelType &level = it->second;
			if (likely(!level.empty()))
			{
				for (typename LevelType::iterator lit = level.begin(); lit != level.end(); ++lit)
				{
					Order *o = *lit;
					if (!o->pendingCancel)
					{
						execution.Cancel(o);
						result = true;
					}
				}
				++it;
			}
			else
			{
				it = orderBook.erase(it);
			}
		}
	}
	else
	{
		typename BookType::iterator it = orderBook.begin();
		while (it != orderBook.end())
		{
			LevelType &level = it->second;
			if (likely(!level.empty()))
			{
				for (typename LevelType::iterator lit = level.begin(); lit != level.end(); ++lit)
				{
					Order *o = *lit;
					if (!o->pendingCancel)
					{
						execution.Cancel(o);
						result = true;
					}
				}
				++it;
			}
			else
			{
				it = orderBook.erase(it);
			}
		}
	}
	this->mPx = mPx;
	this->stackPx = stackPx;
	return result;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool MStackSide<SIDE>::Remove(Order *o)
{
	typename BookType::iterator it = orderBook.find(o->price);
	if (likely(it != orderBook.end()))
	{
		LevelType &level = it->second;
		for (typename LevelType::iterator lit = level.begin(); lit != level.end(); ++lit)
		{
			if (*lit == o)
			{
				level.erase(lit);
				if (level.empty())
				{
					orderBook.erase(it);
				}
				return true;
			}
		}
	}
	return false;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void MStackSide<SIDE>::Cancel(const Price px, const bool scratchImmediately)
{
	typename BookType::iterator it = orderBook.begin();
	while (it != orderBook.end() && !SideTraits<SIDE>::IsWorse(it->first, px))
	{
		LevelType &level = it->second;
		if (likely(!level.empty()))
		{
			for (typename LevelType::iterator lit = level.begin(); lit != level.end(); ++lit)
			{
				Order *o = *lit;
				if (!o->pendingCancel)
				{
					o->scratchFillsImmediately = scratchImmediately;
					execution.Cancel(o);
				}
			}
			++it;
		}
		else
		{
			it = orderBook.erase(it);
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool MStackSide<SIDE>::HasOrder(const Order *o) const
{
	typename BookType::const_iterator it = orderBook.find(o->price);
	if (likely(it != orderBook.cend()))
	{
		const LevelType &level = it->second;
		for (typename LevelType::const_iterator lit = level.cbegin(); lit != level.cend(); ++lit)
		{
			if (*lit == o)
			{
				return true;
			}
		}
	}
	return false;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool MStackSide<SIDE>::CanAddOrder(const uint64_t reqId, const Price px) const
{
	//const StackSideSettings &sideSettings = stackSettings.GetStackSideSettings<SIDE>();
	if (sideSettings.length)
	{
		if (mPx)
		{
			if (SideTraits<SIDE>::IsBetter(px, *mPx))
			{
				LOGINFO("Can't add order {} to MStack because order price {} is better than MStack price {}",
					reqId, px.AsShifted(), mPx->AsShifted());
				return false;
			}
			else if (stackPx && !SideTraits<SIDE>::IsBetter(px, *stackPx))
			{
				LOGINFO("Can't add order {} to MStack because order price {} is not better than stack price {}",
					reqId, px.AsShifted(), stackPx->AsShifted());
				return false;
			}
			else if (sideSettings.insidePrice && SideTraits<SIDE>::IsBetter(px, *sideSettings.insidePrice))
			{
				LOGINFO("Can't add order {} to MStack because order price {} is better than inside price {}",
					reqId, px.AsShifted(), sideSettings.insidePrice->AsShifted());
				return false;
			}
			else if (sideSettings.outsidePrice && SideTraits<SIDE>::IsWorse(px, *sideSettings.outsidePrice))
			{
				LOGINFO("Can't add order {} to MStack because order price {} is worse than outside price {}",
					reqId, px.AsShifted(), sideSettings.outsidePrice->AsShifted());
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			LOGINFO("Can't add order {} to stack because stack has no price", reqId);
			return false;
		}
	}

	return false;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool MStackSide<SIDE>::AddOrder(Order *o)
{
	//const StackSideSettings &sideSettings = stackSettings.GetStackSideSettings<SIDE>();
	if (sideSettings.length)
	{
		if (mPx)
		{
			if (SideTraits<SIDE>::IsBetter(o->price, *mPx))
			{
				LOGINFO("Can't add order {} to MStack because order price {} is better than MStack price {}",
					o->orderReqId, o->price.AsShifted(), mPx->AsShifted());
				return false;
			}
			else if (stackPx && !SideTraits<SIDE>::IsBetter(o->price, *stackPx))
			{
				LOGINFO("Can't add order {} to MStack because order price {} is not better than stack price {}",
					o->orderReqId, o->price.AsShifted(), stackPx->AsShifted());
				return false;
			}
			// TODO move the price band checks to the instrument level, since they're done in both stacks?
			else if (sideSettings.insidePrice && SideTraits<SIDE>::IsBetter(o->price, *sideSettings.insidePrice))
			{
				LOGINFO("Can't add order {} to MStack because order price {} is better than inside price {}",
				 	o->orderReqId, o->price.AsShifted(), sideSettings.insidePrice->AsShifted());
				return false;
			}
			else if (sideSettings.outsidePrice && SideTraits<SIDE>::IsWorse(o->price, *sideSettings.outsidePrice))
			{
				LOGINFO("Can't add order {} to MStack because order price {} is worse than outside price {}",
				 	o->orderReqId, o->price.AsShifted(), sideSettings.outsidePrice->AsShifted());
				return false;
			}
			else if (unlikely(o->submitQueued))
			{
				LOGINFO("Can't add order {} to MStack because it is queued for submission", o->orderReqId);
				return false;
			}
			else
			{
				o->type = Order::Type::STACK;
				o->stackIndex = stackIndex;
				o->wasAbandoned = false;
				orderBook[o->price].push_back(o);
				LOGINFO("Added order {} to MStack | px={} | index={}", o->orderReqId, o->price.AsShifted(), stackIndex);
				return true;
			}
		}
		else
		{
			LOGINFO("Can't add order {} to stack because stack has no price", o->orderReqId);
			return false;
		}
	}

	return false;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
Quantity MStackSide<SIDE>::GiveUpOrders(const Price px, FixedSizeVector<Order *, 10> &orders, bool &ordersRemaining)
{
	typename BookType::iterator it = orderBook.find(px);
	if (likely(it != orderBook.end()))
	{
		Quantity sum { 0 };
		LevelType &level = it->second;
		while (!level.empty() && !orders.Full())
		{
			sum += level.front()->WorkingQuantity();
			orders.PushBack(level.front());
			level.pop_front();
		}
		ordersRemaining = !level.empty();
		return sum;
	}
	else
	{
		return Quantity { 0 };
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
int MStackSide<SIDE>::AbandonPrice(const Price px)
{
	typename BookType::iterator it = orderBook.find(px);
	if (likely(it != orderBook.end()))
	{
		int count = 0;
		LevelType &level = it->second;
		while (!level.empty())
		{
			Order *o = level.front();
			o->type = Order::Type::NONE;
			o->wasAbandoned = true;
			LOGINFO("MStack abandoned order {}", o->orderReqId);
			++count;
			level.pop_front();
		}
		orderBook.erase(it);
		return count;
	}
	else
	{
		return 0;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
int MStackSide<SIDE>::ReleaseAllOrders()
{
	int count = 0;

	while (!orderBook.empty())
	{
		typename BookType::iterator it = orderBook.begin();
		LevelType &level = it->second;
		while (!level.empty())
		{
			Order *o = level.front();
			o->type = Order::Type::NONE;
			o->wasAbandoned = true;
			++count;
			LOGINFO("MStack abandoned order {}", o->orderReqId);
			level.pop_front();
		}
		orderBook.erase(it);
	}

	return count;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void MStackSide<SIDE>::GetStackInfo(int64_t &stackPrice, int32_t *qtys, int &index, const int size) const
{
	if (stackPrice != SideTraits<SIDE>::INVALID_PRICE.AsShifted())
	{
		for (typename BookType::const_iterator it = orderBook.cbegin(); it != orderBook.cend(); ++it)
		{
			const Price px = it->first;
			Quantity levelQty { 0 };
			const LevelType &level = it->second;
			for (LevelType::const_iterator lit = level.cbegin(); lit != level.cend(); ++lit)
			{
				levelQty += (*lit)->WorkingQuantity();
			}
			if (levelQty > Quantity(0))
			{
				index = SideTraits<SIDE>::DeltaIn(Price::FromShifted(stackPrice), px) / tickIncrement;
				if (index < size)
				{
					if (index >= 0)
					{
						qtys[index] += levelQty.value();
					}
				}
				else
				{
					break;
				}
			}
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool MStackSide<SIDE>::HasOrders(const Price px) const
{
	typename BookType::const_iterator it = orderBook.find(px);
	return it != orderBook.cend() && !it->second.empty();
}

}
