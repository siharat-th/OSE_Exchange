#include "HedgingModel.hpp"
#include "Instrument.hpp"
#include "StrategyNotifier.hpp"
#include <akl/BranchPrediction.hpp>

namespace akl::stacker
{

template<>
void HedgingModel::disableAdditionalHedge<KTN::ORD::KOrderSide::Enum::BUY>()
{
	instr.DisableAdditionalHedge<KTN::ORD::KOrderSide::Enum::BUY>();
}


template<>
void HedgingModel::disableAdditionalHedge<KTN::ORD::KOrderSide::Enum::SELL>()
{
	instr.DisableAdditionalHedge<KTN::ORD::KOrderSide::Enum::SELL>();
}

HedgingModel::HedgingModel(
	const QuoteInstrumentSettings &settings,
	const Book &book,
	Instrument &instr,
	StrategyNotifier &notifier,
	PositionManager &pm,
	const Time &time,
	StopHandler &stopHandler)
: settings(settings)
, book(book)
, instr(instr)
, notifier(notifier)
, symbol(instr.Symbol)
, tickIncrement(instr.TickIncrement)
, pm(pm)
, time(time)
, stopHandler(stopHandler)
{

}

bool HedgingModel::HasOrders() const
{
	return map.HasOrders();
}

bool HedgingModel::HasOrder(const Order *o) const
{
	return map.HasOrder(o);
}

bool HedgingModel::MaintainExistingOrders(const KTN::ORD::KOrderSide::Enum side)
{
	OrderListType ordersToCancel;
	OrderListType ordersGivenToStack;
	OrderReduceListType ordersToReduce;
	Quantity pendingHedge { 0 };

	if (side == KTN::ORD::KOrderSide::Enum::BUY)
	{
		if (book.HasBid())
		{
			maintainOrders<KTN::ORD::KOrderSide::Enum::BUY>(
				book.BestAsk(),
				ordersToCancel,
				ordersGivenToStack,
				ordersToReduce,
				pendingHedge);
		}
	}
	else if (side == KTN::ORD::KOrderSide::Enum::SELL)
	{
		if (book.HasAsk())
		{
			maintainOrders<KTN::ORD::KOrderSide::Enum::SELL>(
				book.BestBid(),
				ordersToCancel,
				ordersGivenToStack,
				ordersToReduce,
				pendingHedge);
		}
	}

	if (book.IsOpen())
	{
		for (OrderListType::iterator it = ordersToCancel.begin(); it != ordersToCancel.end(); ++it)
		{
			Order *o = *it;
			cancel(o);
		}
		for (OrderReduceListType::iterator it = ordersToReduce.begin(); it != ordersToReduce.end(); ++it)
		{
			modify(it->o, it->newQty + it->o->filledQuantity);
		}
	}
	else
	{
		for (OrderListType::iterator it = ordersToCancel.begin(); it != ordersToCancel.end(); ++it)
		{
			Order *o = *it;
			RemoveOrder(o);
			o->type = Order::Type::NONE;
		}
	}

	for (OrderListType::iterator it = ordersGivenToStack.begin(); it != ordersGivenToStack.end(); ++it)
	{
		Order *o = *it;
		RemoveOrder(o);
	}
	if (!ordersGivenToStack.empty())
	{
		instr.QueueStackInfoUpdate();
	}
	return !ordersToCancel.empty() || !ordersToReduce.empty() || !ordersGivenToStack.empty();
}

bool HedgingModel::HandleTrade(const Timestamp &now, const KTN::ORD::KOrderSide::Enum aggressingSide, const Quantity tradeQty, const Price tradePx)
{
	bool result = false;

	if (settings.hedgeOnLargeTrade)
	{
		const std::optional<ProrataTarget> &bidTarget = instr.ProrataTarget<KTN::ORD::KOrderSide::Enum::BUY>();
		const std::optional<ProrataTarget> &askTarget = instr.ProrataTarget<KTN::ORD::KOrderSide::Enum::SELL>();
		tradeTracker.SetTargets(bidTarget, askTarget);
		tradeTracker.HandleTrade(book, now, aggressingSide, tradeQty, tradePx);

		if (bidTarget)
		{
			const Quantity tradeQty = tradeTracker.GetBidTrades();
			const std::optional<Price> tradePx = tradeTracker.GetBidTradePrice();
			if (tradePx && *tradePx == bidTarget->price && bidTarget->ltpQuantity > Quantity(0) && tradeQty >= bidTarget->ltpQuantity)
			{
				instr.CancelSide<KTN::ORD::KOrderSide::Enum::BUY>(bidTarget->price, true);
				const Quantity position = instr.GetPosition();
				const Quantity additionalHedge = bidTarget->addHedgeQuantity;
				const StopInfo *stopInfo = stopHandler.GetStop<KTN::ORD::KOrderSide::Enum::SELL>(bidTarget->price);
				if (position > Quantity(0) || additionalHedge > Quantity(0) || stopInfo != nullptr)
				{
					canHedge[1] = true;
					doOffset<KTN::ORD::KOrderSide::Enum::BUY>(bidTarget->price, bidTarget->price, stopInfo != nullptr && stopInfo->sendOnTrade, additionalHedge);
				}
				if (additionalHedge > Quantity(0))
				{
					disableAdditionalHedge<KTN::ORD::KOrderSide::Enum::BUY>();
				}
				LOGWARN("Canceling bid side due to large trade: {}@{}", tradeQty.value(), bidTarget->price.AsShifted());

				std::ostringstream ss;
				ss << "Large trade on BID in " << symbol << ". Total size: " << tradeQty;
				notifier.Notify(ss.str(), KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
				result = true;
			}
		}
		if (!result && askTarget)
		{
			const Quantity tradeQty = tradeTracker.GetAskTrades();
			const std::optional<Price> tradePx = tradeTracker.GetAskTradePrice();
			if (tradePx && *tradePx == askTarget->price && askTarget->ltpQuantity > Quantity(0) && tradeQty >= askTarget->ltpQuantity)
			{
				instr.CancelSide<KTN::ORD::KOrderSide::Enum::SELL>(askTarget->price, true);
				const Quantity position = instr.GetPosition();
				const Quantity additionalHedge = askTarget->addHedgeQuantity;
				const StopInfo *stopInfo = stopHandler.GetStop<KTN::ORD::KOrderSide::Enum::BUY>(askTarget->price);
				if (position < Quantity(0) || additionalHedge > Quantity(0) || stopInfo != nullptr)
				{
					canHedge[0] = true;
					doOffset<KTN::ORD::KOrderSide::Enum::SELL>(askTarget->price, askTarget->price, stopInfo != nullptr && stopInfo->sendOnTrade, additionalHedge);
				}
				if (additionalHedge > Quantity(0))
				{
					disableAdditionalHedge<KTN::ORD::KOrderSide::Enum::SELL>();
				}
				LOGWARN("Canceling ask side due to large trade: {}@{}", tradeQty.value(), askTarget->price.AsShifted());

				std::ostringstream ss;
				ss << "Large trade on ASK in " << symbol << ". Total size: " << tradeQty;
				notifier.Notify(ss.str(), KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
				result = true;
			}
		}
	}
	return result;
}

bool HedgingModel::HandleOffset(const std::optional<FillInfo> &fillInfo, const std::optional<CanceledHedgeInfo> &canceledHedgeInfo)
{
	bool result = false;
	const Quantity pos = instr.GetPosition();

	if (likely(!settings.noPayup))
	{
		recalculate<KTN::ORD::KOrderSide::Enum::BUY>();
		recalculate<KTN::ORD::KOrderSide::Enum::SELL>();

		if (pos > Quantity(0))
		{
			if (pos > oldPosition)
			{
				canHedge[1] = true;
			}
			result |= handleOffset<KTN::ORD::KOrderSide::Enum::BUY>(fillInfo, canceledHedgeInfo);
			result |= checkStop<KTN::ORD::KOrderSide::Enum::SELL>();
			result |= checkMissedStops<KTN::ORD::KOrderSide::Enum::BUY>();
			result |= checkMissedStops<KTN::ORD::KOrderSide::Enum::SELL>();
			result |= MaintainExistingOrders(KTN::ORD::KOrderSide::Enum::SELL);
		}
		else if (pos < Quantity(0))
		{
			if (pos < oldPosition)
			{
				canHedge[0] = true;
			}
			result |= handleOffset<KTN::ORD::KOrderSide::Enum::SELL>(fillInfo, canceledHedgeInfo);
			result |= checkStop<KTN::ORD::KOrderSide::Enum::BUY>();
			result |= checkMissedStops<KTN::ORD::KOrderSide::Enum::BUY>();
			result |= checkMissedStops<KTN::ORD::KOrderSide::Enum::SELL>();
			result |= MaintainExistingOrders(KTN::ORD::KOrderSide::Enum::BUY);
		}
		else
		{
			result |= checkStop<KTN::ORD::KOrderSide::Enum::BUY>();
			result |= checkStop<KTN::ORD::KOrderSide::Enum::SELL>();
			result |= checkMissedStops<KTN::ORD::KOrderSide::Enum::BUY>();
			result |= checkMissedStops<KTN::ORD::KOrderSide::Enum::SELL>();
			result |= MaintainExistingOrders(KTN::ORD::KOrderSide::Enum::BUY);
			result |= MaintainExistingOrders(KTN::ORD::KOrderSide::Enum::SELL);
		}
	}

	oldPosition = pos;
	return result;
}

bool HedgingModel::RemoveOrder(Order *o)
{
	const bool result = map.RemoveOrder(o);
	if (likely(result))
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
				canHedge[0] |= book.HasAsk() && o->price < book.BestAsk();
				break;
			case KTN::ORD::KOrderSide::Enum::SELL:
				canHedge[1] |= book.HasBid() && o->price > book.BestBid();
				break;
			default:
				break;
		}
	}

	return result;
}

void HedgingModel::ReleaseAllOrders()
{
	map.ReleaseOrders();
}

void HedgingModel::CancelAllOrders()
{
	cancelSideOrders<KTN::ORD::KOrderSide::Enum::BUY>();
	cancelSideOrders<KTN::ORD::KOrderSide::Enum::SELL>();
}

size_t HedgingModel::Adopt(std::list<Order *> &adoptedOrders, const AdoptLevelInfo *levels, const int count, const int stackIndex)
{
	const size_t bidCount = adopt<KTN::ORD::KOrderSide::Enum::BUY>(adoptedOrders, levels, count, stackIndex);
	const size_t askCount = adopt<KTN::ORD::KOrderSide::Enum::SELL>(adoptedOrders, levels, count, stackIndex);
	return bidCount + askCount;
}

bool HedgingModel::CheckStops()
{
	bool result = checkStop<KTN::ORD::KOrderSide::Enum::BUY>();
	result |= checkStop<KTN::ORD::KOrderSide::Enum::SELL>();
	result |= checkMissedStops<KTN::ORD::KOrderSide::Enum::BUY>();
	result |= checkMissedStops<KTN::ORD::KOrderSide::Enum::SELL>();
	return result;
}

bool HedgingModel::CheckTimers(const Timestamp now)
{
	bool result = false;

	if (lastTickUp[0] != Time::Min())
	{
		const Duration timeout = Millis(settings.bidSettings.timeoutCover);
		if (now.time_since_epoch().count() >= lastTickUp[0].time_since_epoch().count() + timeout.count())
		{
			lastTickUp[0] = Time::Min();
			result = true;
		}
	}
	if (lastTickUp[1] != Time::Min())
	{
		const Duration timeout = Millis(settings.askSettings.timeoutCover);
		if (now >= lastTickUp[1] + timeout)
		{
			lastTickUp[1] = Time::Min();
			result = true;
		}
	}
	return result;
}

bool HedgingModel::sendOffset(
	const KTN::ORD::KOrderSide::Enum side,
	const Quantity qty,
	const Price px,
	const int sideIndex,
	const Quantity additionalHedge,
	const StopInfo *stopInfo)
{
	bool result = false;
	const Quantity maxOrderQty = settings.riskSettings.maxOrderQty;
	if (maxOrderQty > Quantity(0))
	{
		Quantity remaining = qty;
		while (remaining > Quantity(0))
		{
			const Quantity orderQty = std::min(remaining, maxOrderQty);
			Order *o = createOrder(side, orderQty, px, Order::Type::HEDGE);
			if (likely(o != nullptr && instr.CanSubmit(o, true, false, ordersToSend.Size() + 1)))
			{
				insertByQuantity(o);
			}
			remaining -= orderQty;
		}
		remaining = additionalHedge;
		while (remaining > Quantity(0))
		{
			const Quantity orderQty = std::min(remaining, maxOrderQty);
			Order *o = createOrder(side, orderQty, px, Order::Type::NONE);
			if (likely(o != nullptr && instr.CanSubmit(o, true, false, ordersToSend.Size() + 1)))
			{
				insertByQuantity(o);
			}
			remaining -= orderQty;
		}

		if (stopInfo != nullptr)
		{
			for (int i = 0; i < stopInfo->orders.Size(); ++i)
			{
				Order *o = stopInfo->orders[i];
				if (instr.CanSubmit(o, true, true, ordersToSend.Size() + 1))
				{
					insertByQuantity(o);
				}
			}
		}

		for (int i = 0; i < ordersToSend.Size(); ++i)
		{
			Order *o = ordersToSend[i];
			if (instr.CanSubmit(o, true, o->type == Order::Type::STOP))
			{
				instr.Submit(o);
				result = true;
			}
		}
		for (int i = 0; i < ordersToSend.Size(); ++i)
		{
			Order *o = ordersToSend[i];
			if (o->type == Order::Type::HEDGE)
			{
				map.AddOrder(o);
				canHedge[sideIndex] = false;
			}
		}

		ordersToSend.Clear();
	}
	return result;
}


bool HedgingModel::sendStop(StopInfo::OrderListType &orders)
{
	bool result = false;

	for (int i = 0; i < orders.Size(); ++i)
	{
		Order *o = orders[i];
		if (instr.CanSubmit(o, true, true))
		{
			instr.Submit(o);
			result = true;
		}
	}

	return result;
}

void HedgingModel::modify(Order *o, const Quantity qty)
{
	instr.Modify(o, qty);
}

void HedgingModel::cancel(Order *o)
{
	instr.Cancel(o);
}

bool HedgingModel::giveOrderToStack(Order *o, const bool stack1Adopt, const bool stack2Adopt)
{
	return instr.AddOrderToStack(o, true, stack1Adopt, stack2Adopt);
}

void HedgingModel::queuePositionUpdate()
{
	instr.QueuePositionUpdate();
}

template<>
const std::optional<ProrataTarget> &HedgingModel::getTarget<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return instr.ProrataTarget<KTN::ORD::KOrderSide::Enum::BUY>();
}

template<>
const std::optional<ProrataTarget> &HedgingModel::getTarget<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return instr.ProrataTarget<KTN::ORD::KOrderSide::Enum::SELL>();
}

template<>
std::optional<Price> HedgingModel::getStackTarget<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return instr.GetPricingModel().BidLean();
}

template<>
std::optional<Price> HedgingModel::getStackTarget<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return instr.GetPricingModel().AskLean();
}

template<>
void HedgingModel::cancelQuoteOrders<KTN::ORD::KOrderSide::Enum::BUY>(const Price px, bool scratchImmediately)
{
	instr.CancelSide<KTN::ORD::KOrderSide::Enum::BUY>(px, scratchImmediately);
}

template<>
void HedgingModel::cancelQuoteOrders<KTN::ORD::KOrderSide::Enum::SELL>(const Price px, bool scratchImmediately)
{
	instr.CancelSide<KTN::ORD::KOrderSide::Enum::SELL>(px, scratchImmediately);
}

void HedgingModel::queueStackInfoUpdate()
{
	instr.QueueStackInfoUpdate();
}

// Side is in terms of position
template<>
Quantity HedgingModel::getPayup<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return instr.GetPayup<KTN::ORD::KOrderSide::Enum::BUY>();
}

// Side is in terms of position
template<>
Quantity HedgingModel::getPayup<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return instr.GetPayup<KTN::ORD::KOrderSide::Enum::SELL>();
}

Order *HedgingModel::createOrder(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px, const Order::Type type)
{
	Order *o = instr.CreateOrder(side, qty, px);
	if (likely(o != nullptr))
	{
		o->targetQuantity = qty;
		o->type = type;
		return o;
	}
	else
	{
		return nullptr;
	}
}

void HedgingModel::insertByQuantity(Order *o)
{
	for (int i = 0; i < ordersToSend.Size(); ++i)
	{
		if (o->targetQuantity > ordersToSend[i]->targetQuantity)
		{
			if (unlikely(!ordersToSend.InsertAt(i, o)))
			{
				LOGWARN("Failed to insert order {} at index {}", o->orderReqId, i);
			}
			return;
		}
	}
	if (unlikely(!ordersToSend.PushBack(o)))
	{
		LOGWARN("Failed to push back order {}", o->orderReqId);
	}
}

}
