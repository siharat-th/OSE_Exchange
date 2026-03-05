#include "MStack.hpp"
#include "../messages/StackInfoMessage.hpp"

namespace akl::stacker
{

MStack::MStack(
	const QuoteInstrumentSettings &quoteSettings,
	const StackSideSettings &bidSettings,
	const StackSideSettings &askSettings,
	IExecutionInterface &instr,
	const Price tickIncrement,
	const int stackIndex)
: bids(quoteSettings, bidSettings, instr, tickIncrement, stackIndex)
, asks(quoteSettings, askSettings, instr, tickIncrement, stackIndex)
{

}

bool MStack::Update(const std::optional<Price> &mBidPx, const std::optional<Price> &mAskPx, const std::optional<Price> &bidPx, const std::optional<Price> &askPx)
{
	bool result = bids.Update(mBidPx, bidPx);
	result |= asks.Update(mAskPx, askPx);
	return result;
}

bool MStack::Remove(Order *o)
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bids.Remove(o);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return asks.Remove(o);
		default:
			// TODO log?
			return false;
	}
}

bool MStack::HasOrder(const Order *o) const
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bids.HasOrder(o);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return asks.HasOrder(o);
		default:
			// TODO log?
			return false;
	}
}

bool MStack::CanAddOrder(const KTN::ORD::KOrderSide::Enum side, const uint64_t reqId, const Price px) const
{
	switch (side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bids.CanAddOrder(reqId, px);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return asks.CanAddOrder(reqId, px);
		default:
			// TODO log?
			return false;
	}
}

bool MStack::AddOrder(Order *o)
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bids.AddOrder(o);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return asks.AddOrder(o);
		default:
			// TODO log?
			return false;
	}
}

int MStack::ReleaseAllOrders()
{
	int count = bids.ReleaseAllOrders();
	count += asks.ReleaseAllOrders();
	return count;
}

void MStack::GetStackInfo(StackInfoMessage &msg, int &bidIndex, int &askIndex) const
{
	bids.GetStackInfo(msg.stackBidPrice, msg.bidStackQty, bidIndex, StackInfoMessage::SIZE);
	asks.GetStackInfo(msg.stackAskPrice, msg.askStackQty, askIndex, StackInfoMessage::SIZE);
}

template<>
MStackSide<KTN::ORD::KOrderSide::Enum::BUY> &MStack::getSide<KTN::ORD::KOrderSide::Enum::BUY>()
{
	return bids;
}

template<>
MStackSide<KTN::ORD::KOrderSide::Enum::SELL> &MStack::getSide<KTN::ORD::KOrderSide::Enum::SELL>()
{
	return asks;
}

template<>
const MStackSide<KTN::ORD::KOrderSide::Enum::BUY> &MStack::getSide<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return bids;
}

template<>
const MStackSide<KTN::ORD::KOrderSide::Enum::SELL> &MStack::getSide<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return asks;
}

}
