#pragma once

#include "stack/Stack.hpp"
#include "stack/MStack.hpp"
#include "stack/ProrataQuoter.hpp"
#include "pricing/PricingModel.hpp"
#include "pricing/ProrataPricingModel.hpp"
#include "pricing/SpreaderPricingModel.hpp"
#include "PositionManager.hpp"
#include "PayupCalculator.hpp"
#include "IExecutionInterface.hpp"
#include "StrategyInstrumentExecution.hpp"
#include "HedgingModel.hpp"
#include "StopHandler.hpp"
#include "Order.hpp"
#include "MemoryPool.hpp"
#include "messages/StackerPriceAbandonMessage.hpp"
#include "messages/StackerPriceAdoptMessage.hpp"
#include "messages/StackerPositionUpdateMessage.hpp"
#include "messages/StopUpdateMessage.hpp"
#include "messages/ProrataProfileUpdateMessage.hpp"
#include "messages/StackerProrataOrderRequestMessage.hpp"
#include "messages/StackerProrataOrderAdoptMessage.hpp"
#include "messages/StackerProrataOrderAbandonMessage.hpp"
#include "messages/StackerSpreaderUpdateMessage.hpp"
#include "messages/HeartbeatMessage.hpp"

class QuoterV3;

namespace KTN
{
	struct OrderPod;
}

namespace KTN::Core
{
	template<typename T>
	class ObjectPool;
}

namespace akl
{
	class Book;
}

namespace akl::stacker
{

struct AklStackerParams;
class AklStacker;
class StrategyNotifier;
struct StackInfoMessage;

class Instrument : public IExecutionInterface
{
public:
	Instrument(
		AklStacker &strategy,
		QuoterV3 &quoter,
		KTN::Core::ObjectPool<KTN::OrderPod> &orderPool,
		IExchangeSender &orderSender,
		StrategyNotifier &notifier,
		const AklStackerParams &settings,
		const Book &book,
		const Price tickIncrement);

	void OnMarketStateChange(
		const KT01::DataStructures::MarketDepth::BookSecurityStatus oldState,
		const KT01::DataStructures::MarketDepth::BookSecurityStatus newState);
	bool OnTrade(
		const KTN::ORD::KOrderSide::Enum aggressingSide,
		const Quantity qty,
		const Price px);
	bool Recalculate(
		const bool doRequote,
		const std::optional<FillInfo> fillInfo = std::nullopt,
		const std::optional<CanceledHedgeInfo> canceledHedgeInfo = std::nullopt);
	bool CanAddOrderToStack(
		const KTN::ORD::KOrderSide::Enum side,
		const uint64_t reqId,
		const Price px,
		const bool mstackAdoptAllowed = true,
		bool stack1Adopt = true,
		bool stack2Adopt = false) const;
	bool AddOrderToStack(Order *o, const bool mstackAdoptAllowed = true, bool stack1Adopt = true, bool stack2Adopt = false);
	void ClearPosition();

	void UpdateStackQuantities();
	void AddRecoveredOrders(const std::vector<KTN::OrderPod> &recoveredOrders);

	virtual Order *CreateOrder(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px) override final;
	virtual bool CanSubmit(const Order *o, bool canCross = false, bool canBypassWait = false, int numOrders = 1) const override final;
	virtual bool CanModify(const Order *o, const Price px, bool canCross = false) const override final;
	virtual void Submit(Order *o) override final;
	virtual void Modify(Order *o, const Price px) override final;
	virtual void Modify(Order *o, const Quantity qty) override final;
	virtual void Cancel(Order *o) override final;
	virtual Quantity TakeDynamicOrders(const KTN::ORD::KOrderSide::Enum side, const Price px, FixedSizeVector<Order *, 10> &orders) override final;
	virtual Quantity TakeMOrders(const KTN::ORD::KOrderSide::Enum side, const Price px, const int stackIndex, FixedSizeVector<Order *, 10> &orders, bool &ordersRemaining) override final;

	void OnAck(KTN::OrderPod &ord);
	void OnModAck(KTN::OrderPod &ord);
	void OnFill(KTN::OrderPod &ord);
	void OnCancelAck(KTN::OrderPod &ord);
	void OnSubmitReject(KTN::OrderPod &ord);
	void OnCancelReject(KTN::OrderPod &ord);
	void OnCancelReplaceReject(KTN::OrderPod &ord);

	void OnInternalCancel(Order *o);
	void OnInternalReject(Order *o);

	
	const Price TickIncrement;
	inline PricingModel &GetPricingModel(int index = 0) { return pricingModel[index]; }
	inline const PricingModel &GetPricingModel(int index = 0) const { return pricingModel[index]; }
	inline const ProrataPricingModel &GetProrataPricingModel() const { return prorataPricingModel; }
	inline SpreaderPricingModel &GetSpreaderPricingModel() { return spreaderPricingModel; }
	inline const SpreaderPricingModel &GetSpreaderPricingModel() const { return spreaderPricingModel; }
	inline Quantity GetPosition() const { return pm.Position(); }
	inline const PositionManager &GetPositionManager() const { return pm; }
	inline Stack &GetStack(const int idx) { return stack[idx]; }
	inline const Stack &GetStack(const int idx) const { return stack[idx]; }
	inline MStack &GetMStack(const int idx) { return mstack[idx]; }
	inline const MStack &GetMStack(const int idx) const { return mstack[idx]; }
	inline HedgingModel &GetHedgingModel() { return hm; }
	inline const HedgingModel &GetHedgingModel() const { return hm; }
	inline InstrumentRisk &GetRisk() { return strategyExecution.GetRisk(); }
	inline const InstrumentRisk &GetRisk() const { return strategyExecution.GetRisk(); }
	inline const StrategyInstrumentExecution &GetStrategyExecution() const { return strategyExecution; }
	inline StrategyInstrumentExecution &GetStrategyExecution() { return strategyExecution; }
	inline const StopHandler &GetStopHandler() const { return sh; }
	inline StopHandler &GetStopHandler() { return sh; }
	inline const ProrataQuoter &GetProrataQuoter() const { return prorataQuoter; }

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline const std::optional<ProrataTarget> &ProrataTarget() const
	{
		return prorataQuoter.Target<SIDE>();
	}

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline void CancelSide(const Price px, const bool scratchImmediately)
	{
		prorataQuoter.Cancel<SIDE>(px);
		stack[0].Cancel<SIDE>(px, scratchImmediately);
		stack[1].Cancel<SIDE>(px, scratchImmediately);
		//dynamicStack.Cancel<Side::BID>(px, scratchImmediately);
		mstack[0].Cancel<SIDE>(px, scratchImmediately);
		mstack[1].Cancel<SIDE>(px, scratchImmediately);
	}

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline Quantity GetPayup() const
	{
		return pc.GetPayup<SIDE>();
	}

	inline const Order *GetOrder(const uint64_t orderReqId) const
	{
		OrderMapType::const_iterator it = orderMap.find(orderReqId);
		if (it != orderMap.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}

	inline Order *GetOrder(const uint64_t orderReqId)
	{
		OrderMapType::const_iterator it = orderMap.find(orderReqId);
		if (it != orderMap.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}

	void Start();
	void Stop();
	void RepricePositions();
	void AddPosition(const Quantity pos);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	std::optional<Price> GetBestQuotePrice() const;

	void Handle(const StackerPriceAbandonMessage &msg);
	void Handle(const StackerPriceAdoptMessage &msg);
	void Handle(const StackerPositionUpdateMessage &msg);
	void Handle(const StopUpdateMessage &msg);
	void Handle(const std::vector<StackerProrataOrderAdoptMessage> &pendingAdopts);
	void Handle(const std::vector<StackerProrataOrderAbandonMessage> &pendingAbandons);
	void Handle(const StackerSpreaderUpdateMessage &msg);

	void GetStackInfo(StackInfoMessage &msg, const int index) const;
	void QueueStackInfoUpdate();
	void QueuePositionUpdate();
	
	int Adopt(bool repricePositions, const AdoptLevelInfo *levels = nullptr, const size_t count = 0, const int stackIndex = 0);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void OnProrataProfileUpdate(const ProrataProfileUpdateMessage &msg);
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void ClearProrataProfile();
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void OnAltProrataProfileUpdate(const ProrataProfileUpdateMessage &msg);
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void ClearAltProrataProfile();

	void GetProrataOrders(std::vector<Order *> &orders) const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void DisableAdditionalHedge();

	bool CheckTimers(const Timestamp now);
	void CheckOrderPool();

	void SetHasSpreader(const bool val);

	const std::string Symbol;

private:
	Instrument(const Instrument &) = delete;
	Instrument &operator=(const Instrument &) = delete;
	Instrument(const Instrument &&) = delete;
	Instrument &operator=(const Instrument &&) = delete;

	AklStacker &strategy;
	QuoterV3 &quoter;
	const Time &time;
	IExchangeSender &orderSender;
	const AklStackerParams &settings;
	const Book &book;

	MemoryPool<Order> orderPool;
	PositionManager pm;
	PayupCalculator pc;
	PricingModel pricingModel[2];
	Stack stack[2];
	MStack mstack[2];
	ProrataPricingModel prorataPricingModel;
	ProrataQuoter prorataQuoter;
	SpreaderPricingModel spreaderPricingModel;
	StrategyInstrumentExecution strategyExecution;
	StopHandler sh;
	HedgingModel hm;

	StrategyNotifier &notifier;

	int numRejects[2] { 0, 0 };
	Timestamp lastRejectTime[2] { Time::Min(), Time::Min() };
	static constexpr int MAX_SUBMIT_REJECTS { 3 };
	static constexpr int REJECT_WAIT_INT { 3 };
	static constexpr Duration REJECT_WAIT = Seconds(REJECT_WAIT_INT);

	void removeOrder(Order *o, const bool wasCancel = false, const bool wasFill = false);

	using OrderMapType = std::unordered_map<uint64_t, Order *>;
	OrderMapType orderMap;
	OrderMapType doneOrderMap;

	uint64_t strategyOrderId { 1 };

	bool disableAdditionalHedge[2] { false, false };
	bool hasSpreader;
};

}

#include "Instrument.ipp"
