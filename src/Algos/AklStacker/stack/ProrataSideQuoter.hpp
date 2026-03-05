#pragma once

#include <Orders/OrderEnumsV2.hpp>
#include <akl/Price.hpp>
#include "../Order.hpp"

#include <optional>
#include <vector>

#include "../pricing/ProrataTarget.hpp"
#include <AlgoParams/AklStackerParams.hpp>

namespace akl::stacker
{

class IExecutionInterface;
struct ProrataTarget;
class Time;

template<KTN::ORD::KOrderSide::Enum SIDE>
class ProrataSideQuoter
{
public:
	ProrataSideQuoter(
		const QuoteInstrumentSettings &quoteSettings,
		const Time &time,
		IExecutionInterface &execution);

	bool HasOrders() const;

	bool Update(const std::optional<ProrataTarget> &target, const bool doRequote);
	void Cancel(bool scratchFillsImmediately);
	void Cancel(const Price px);
	bool RemoveOrder(Order *o, const bool cancelAck);

	void LogOrders() const;

	inline const std::optional<ProrataTarget> &Target() const { return currentTarget; }

	void GetOrders(std::vector<Order *> &orders) const;

	Order *GetOrder(const uint64_t orderReqId);
	bool AbandonOrder(Order *o);

	void ReleaseAllOrders();

	bool HasOrder(const Order *o) const;

private:
	ProrataSideQuoter(const ProrataSideQuoter &) = delete;
	ProrataSideQuoter &operator=(const ProrataSideQuoter &) = delete;
	ProrataSideQuoter(const ProrataSideQuoter &&) = delete;
	ProrataSideQuoter &operator=(const ProrataSideQuoter &&) = delete;

	static constexpr int MAX_NUM_ORDERS { 10 };

	const QuoteInstrumentSettings &quoteSettings;
	const Time &time;
	IExecutionInterface &execution;

	std::optional<Price> currentPx { std::nullopt };
	std::optional<Price> lastQuotePx { std::nullopt };

	struct ProrataOrder
	{
		Order *o { nullptr };
		Timestamp timeToClear { Time::Max() };
		int8_t numRequotes { QuoteInstrumentSettings::MAX_REQUOTE_COUNT };
		bool allowRequote { false };
		bool strategyCancel { false };
	};

	using OrderListType = std::vector<ProrataOrder>;
	OrderListType workingOrders;
	OrderListType cachedOrders;

	std::optional<ProrataTarget> currentTarget { std::nullopt };
	bool manualCancel { false };

	bool updateOrders(const ProrataTarget &target);
	bool cancel(const bool strategyCancel, bool scratchFillsImmediately = false);
	void popNullOrders();
	bool canRequote(const ProrataOrder &po) const;
};

}

#include "ProrataSideQuoter.ipp"