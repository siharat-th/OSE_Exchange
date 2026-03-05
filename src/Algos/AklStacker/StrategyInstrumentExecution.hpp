#pragma once

#include "risk/RateLimiter.hpp"
#include "risk/InstrumentRisk.hpp"
#include "StrategyNotifier.hpp"

#include <Orders/OrderPod.hpp>

#include <list>

class IExchangeSender;
class OrderManagerV5;
class QuoterV3;

namespace akl::stacker
{

struct InstrumentRiskSettings;
class RateLimitSettings;
class AklStacker;

class StrategyInstrumentExecution
{
public:
	StrategyInstrumentExecution(
		AklStacker &strategy,
		QuoterV3 &quoter,
		const Time &time,
		IExchangeSender &orderSender,
		OrderManagerV5 &orderManager,
		const InstrumentRiskSettings &riskSettings,
		const RateLimitSettings &rateLimitSettings,
		const char *strategyId,
		StrategyNotifier &notifier/*,
		stacker::Strategy &strategy*/);

	void Create(Order *o, const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px);
	void Submit(Order *o);
	void Modify(Order *o, const Price px);
	void Modify(Order *o, const Quantity qty);
	void Cancel(Order *o);

	void Process(const Timestamp now);
	void ProcessCallbacks();

	// void UpdateRisk(Order *o, const ttsdk::ExecutionReportPtr &execRpt);
	// void OnReject(Order *o, const ttsdk::RejectResponsePtr &rejResp);
	// void OnSendFailed(Order *o, const ttsdk::OrderProfile &profile);

	void OnAck(Order *o);
	void OnModAck(Order *o, const KTN::OrderPod &ko);
	void OnCancelAck(Order *o, const KTN::OrderPod &ko);
	void OnFill(Order *o, const KTN::OrderPod &ko);
	void OnSubmitReject(Order *o, const KTN::OrderPod &ko);
	void OnCancelReplaceReject(Order *o, const KTN::OrderPod &ko);

	inline InstrumentRisk &GetRisk() { return risk; }
	inline const InstrumentRisk &GetRisk() const { return risk; }

	inline RateLimiter &GetRateLimiter() { return rateLimiter; }
	inline const RateLimiter &GetRateLimiter() const { return rateLimiter; }
	inline bool SubmitQueueFull() const { return rateLimiter.SubmitQueueFull(); }
	inline bool SubmitQueueHasRoom(const int numOrders) const { return rateLimiter.SubmitQueueSize() + numOrders - rateLimiter.TransactionsLeft() <= rateLimiter.SubmitQueueCapacity(); }

	void OnStrategyPause();
	void CancelQueuedActions();

	void onKillswitchUpdate(const bool state);

	uint64_t ReceiveTime = 0;

private:
	StrategyInstrumentExecution(const StrategyInstrumentExecution &) = delete;
	StrategyInstrumentExecution &operator=(const StrategyInstrumentExecution &) = delete;
	StrategyInstrumentExecution(const StrategyInstrumentExecution &&) = delete;
	StrategyInstrumentExecution &operator=(const StrategyInstrumentExecution &&) = delete;

	AklStacker &strategy;
	StrategyNotifier &notifier;
	QuoterV3 &quoter;
	const Time &time;
	IExchangeSender &orderSender;
	OrderManagerV5 &orderManager;
	const InstrumentRiskSettings &riskSettings;
	const std::string symbol;
	const KTN::ORD::KOrderExchange::Enum exchange;
	//stacker::Strategy &strategy;

	RateLimiter rateLimiter;
	InstrumentRisk risk;

	void internalSubmit(Order *o);
	void internalModify(Order *o, const Price px);
	void internalModify(Order *o, const Quantity qty);
	void internalCancel(Order *o);

	using OrderListType = std::vector<Order *>;
	OrderListType ordersCanceledInternally;
	OrderListType ordersRejectedInternally;

	std::list<Order *> ordersToCancelOnAck;

	struct QueuedModify
	{
		Order *o;
		std::optional<Price> px;
		std::optional<Quantity> qty;
	};
	std::list<QueuedModify> ordersToModifyOnAck;
};

}
