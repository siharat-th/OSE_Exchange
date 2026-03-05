#pragma once

#include "ProrataSideQuoter.hpp"

namespace akl::stacker
{

struct ProrataTarget;

class ProrataQuoter
{
public:
	ProrataQuoter(
		const QuoteInstrumentSettings &quoteSettings,
		const Time &time,
		IExecutionInterface &execution);

	bool Update(const std::optional<ProrataTarget> &bidTarget, const std::optional<ProrataTarget> &askTarget, const bool doRequote);

	bool RemoveOrder(Order *o, const bool cancelAck);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void Cancel(const Price px);

	void LogOrders() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const std::optional<ProrataTarget> &Target() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void GetOrders(std::vector<Order *> &orders) const;

	Order *GetOrder(KTN::ORD::KOrderSide::Enum side, uint64_t orderReqId);
	bool AbandonOrder(KTN::ORD::KOrderSide::Enum side, Order *o);

	void ReleaseAllOrders();

	bool HasOrder(const Order *o) const;

private:
	ProrataQuoter(const ProrataQuoter &) = delete;
	const ProrataQuoter &operator=(const ProrataQuoter &) = delete;
	ProrataQuoter(const ProrataQuoter &&) = delete;
	const ProrataQuoter &operator=(const ProrataQuoter &&) = delete;

	ProrataSideQuoter<KTN::ORD::KOrderSide::Enum::BUY> bidQuoter;
	ProrataSideQuoter<KTN::ORD::KOrderSide::Enum::SELL> askQuoter;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	ProrataSideQuoter<SIDE> &getSideQuoter();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const ProrataSideQuoter<SIDE> &getSideQuoter() const;
};

}

#include "ProrataQuoter.ipp"