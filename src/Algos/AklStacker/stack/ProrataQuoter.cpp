#include "ProrataQuoter.hpp"

namespace akl::stacker
{

ProrataQuoter::ProrataQuoter(
	const QuoteInstrumentSettings &quoteSettings,
	const Time &time,
	IExecutionInterface &execution)
: bidQuoter(quoteSettings, time, execution)
, askQuoter(quoteSettings, time, execution)
{

}

bool ProrataQuoter::Update(const std::optional<ProrataTarget> &bidTarget, const std::optional<ProrataTarget> &askTarget, const bool doRequote)
{
	bool result = bidQuoter.Update(bidTarget, doRequote);
	result |= askQuoter.Update(askTarget, doRequote);
	return result;
}

bool ProrataQuoter::RemoveOrder(Order *o, const bool cancelAck)
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bidQuoter.RemoveOrder(o, cancelAck);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return askQuoter.RemoveOrder(o, cancelAck);
		default:
			LOGWARN("Can't remove prorata order {} due to unknown side {}",
				o->orderReqId, static_cast<int>(o->side));
			return false;
	}
}

void ProrataQuoter::LogOrders() const
{
	bidQuoter.LogOrders();
	askQuoter.LogOrders();
}

Order *ProrataQuoter::GetOrder(KTN::ORD::KOrderSide::Enum side, uint64_t orderReqId)
{
	switch (side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bidQuoter.GetOrder(orderReqId);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return askQuoter.GetOrder(orderReqId);
		default:
		// TODO log
			return nullptr;
	}
}

bool ProrataQuoter::AbandonOrder(KTN::ORD::KOrderSide::Enum side, Order *o)
{
	switch (side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bidQuoter.AbandonOrder(o);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return askQuoter.AbandonOrder(o);
		default:
		// TODO log
			return false;
	}
}

void ProrataQuoter::ReleaseAllOrders()
{
	bidQuoter.ReleaseAllOrders();
	askQuoter.ReleaseAllOrders();
}

bool ProrataQuoter::HasOrder(const Order *o) const
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bidQuoter.HasOrder(o);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return askQuoter.HasOrder(o);
		default:
			return false;
	}
}

template<>
ProrataSideQuoter<KTN::ORD::KOrderSide::Enum::BUY> &ProrataQuoter::getSideQuoter<KTN::ORD::KOrderSide::Enum::BUY>()
{
	return bidQuoter;
}

template<>
ProrataSideQuoter<KTN::ORD::KOrderSide::Enum::SELL> &ProrataQuoter::getSideQuoter<KTN::ORD::KOrderSide::Enum::SELL>()
{
	return askQuoter;
}

template<>
const ProrataSideQuoter<KTN::ORD::KOrderSide::Enum::BUY> &ProrataQuoter::getSideQuoter<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return bidQuoter;
}

template<>
const ProrataSideQuoter<KTN::ORD::KOrderSide::Enum::SELL> &ProrataQuoter::getSideQuoter<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return askQuoter;
}

}
