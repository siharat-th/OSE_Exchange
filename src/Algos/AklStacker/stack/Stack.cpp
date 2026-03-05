#include "Stack.hpp"
#include "../messages/StackInfoMessage.hpp"

#include <iostream>

namespace akl::stacker
{

Stack::Stack(
	const QuoteInstrumentSettings &settings,
	const StackSideSettings &bidSettings,
	const StackSideSettings &askSettings,
	IExecutionInterface &execution,
	const Time &time,
	const Price tickIncrement,
	const int8_t stackIndex)
: bids(settings, bidSettings, execution, time, tickIncrement, stackIndex)
, asks(settings, askSettings, execution, time, tickIncrement, stackIndex)
{

}

void Stack::SetDefaultStackQuantity(const Quantity bidStackQty, const Quantity askStackQty)
{
	bids.SetDefaultStackQuantity(bidStackQty);
	asks.SetDefaultStackQuantity(askStackQty);
}

bool Stack::Update(const std::optional<Price> bidPx, const std::optional<Price> askPx, const bool doRequote)
{
	bool result = bids.Update(bidPx, doRequote);
	result |= asks.Update(askPx, doRequote);
	return result;
}

void Stack::SendQueuedSubmits()
{
	bids.SendQueuedSubmits();
	asks.SendQueuedSubmits();
}

bool Stack::HasOrders() const
{
	return bids.HasOrders() || asks.HasOrders();
}

bool Stack::HasOrder(const Order *o) const
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::BUY:
			return bids.HasOrder(o);
		case KTN::ORD::KOrderSide::SELL:
			return asks.HasOrder(o);
		default:
			LOGWARN("Can't check if order {} is in stack due to unknown side | internalSide={}", o->orderReqId, static_cast<int>(o->side));
			return false;
	}
}

bool Stack::CanAddOrder(const KTN::ORD::KOrderSide::Enum side, const uint64_t reqId, const Price px) const
{
	switch (side)
	{
		case KTN::ORD::KOrderSide::BUY:
			return bids.CanAddOrder(reqId, px);
		case KTN::ORD::KOrderSide::SELL:
			return asks.CanAddOrder(reqId, px);
		default:
			LOGWARN("Can't check if order {} can be added to stack due to unknown side | internalSide={}", reqId, static_cast<int>(side));
			return false;
	}
}

bool Stack::AddOrder(Order *o)
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::BUY:
			return bids.AddOrder(o);
			case KTN::ORD::KOrderSide::SELL:
			return asks.AddOrder(o);
		default:
			LOGWARN("Can't check if order {} is in stack due to unknown side | internalSide={}", o->orderReqId, static_cast<int>(o->side));
			return false;
	}
}

bool Stack::RemoveOrder(Order *o, const bool wasFill)
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::BUY:
			return bids.RemoveOrder(o, wasFill);
			case KTN::ORD::KOrderSide::SELL:
			return asks.RemoveOrder(o, wasFill);
		default:
			LOGWARN("Can't check if order {} is in stack due to unknown side | internalSide={}", o->orderReqId, static_cast<int>(o->side));
			return false;
	}
}

void Stack::ReleaseAllOrders()
{
	bids.ReleaseAllOrders();
	asks.ReleaseAllOrders();
}

void Stack::CancelAllOrders()
{
	bids.CancelAllOrders();
	asks.CancelAllOrders();
}

void Stack::GetStackInfo(StackInfoMessage &msg, int &bidIndex, int &askIndex) const
{
	bids.GetStackInfo(msg.stackBidPrice, msg.bidStackQty, bidIndex, StackInfoMessage::SIZE);
	asks.GetStackInfo(msg.stackAskPrice, msg.askStackQty, askIndex, StackInfoMessage::SIZE);
}

bool Stack::CheckRequote(const Timestamp now)
{
	bool result = bids.CheckRequote(now);
	result |= asks.CheckRequote(now);
	return result;
}

void Stack::HandlePriceBandUpdate()
{
	bids.HandlePriceBandsUpdate();
	asks.HandlePriceBandsUpdate();
}

template<>
StackSide<KTN::ORD::KOrderSide::BUY> &Stack::GetSide<KTN::ORD::KOrderSide::BUY>()
{
	return bids;
}

template<>
StackSide<KTN::ORD::KOrderSide::SELL> &Stack::GetSide<KTN::ORD::KOrderSide::SELL>()
{
	return asks;
}

template<>
const StackSide<KTN::ORD::KOrderSide::BUY> &Stack::GetSide<KTN::ORD::KOrderSide::BUY>() const
{
	return bids;
}

template<>
const StackSide<KTN::ORD::KOrderSide::SELL> &Stack::GetSide<KTN::ORD::KOrderSide::SELL>() const
{
	return asks;
}

}
