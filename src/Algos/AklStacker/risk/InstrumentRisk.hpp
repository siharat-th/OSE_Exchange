#pragma once

#include <akl/SideTraits.hpp>
#include <akl/Quantity.hpp>

#include <Orders/OrderEnumsV2.hpp>

#include <AlgoParams/AklStackerParams.hpp>

#include <atomic>

namespace akl::stacker
{

struct Order;

enum RiskStatus : int8_t
{
	RISK_OK = 0,
	RISK_CLIP_EXCEEDED = 1,
	RISK_POSITION_EXCEEDED = 2,
	RISK_KILLSWITCH_ENABLED = 4,
	RISK_UNKNOWN_ERROR = 8
};

inline constexpr const char *toString(const RiskStatus r)
{
	switch (r)
	{
		case RISK_OK:
			return "RISK_OK";
		case RISK_CLIP_EXCEEDED:
			return "CLIP_LIMIT_EXCEEDED";
		case RISK_POSITION_EXCEEDED:
			return "POSITION_LIMIT_EXCEEDED";
		case RISK_KILLSWITCH_ENABLED:
			return "KILLSWITCH_ENABLED";
		default:
			return "UNKNOWN_RISK_ERROR";
	}
}

class InstrumentRisk
{
public:
	InstrumentRisk(const char *strategyId, const InstrumentRiskSettings &settings);

	void AddWorking(Order *o);

	RiskStatus TrySubmit(Order *o);
	RiskStatus TryModify(Order *o, const Quantity newQty);
	RiskStatus TryModify(Order *o, const Price px);

	void OnModAck(Order *o, const KTN::OrderPod &ko);
	void OnCancelAck(Order *o, const KTN::OrderPod &ko);
	void OnFill(Order *o, const KTN::OrderPod &ko);
	void OnSubmitReject(Order *o, const KTN::OrderPod &ko);
	void OnModReject(Order *o, const KTN::OrderPod &ko);
	// void OnExecutionReport(Order *o, const ttsdk::ExecutionReportPtr &execRpt);
	// void OnReject(Order *o, const ttsdk::RejectResponsePtr &rejResp);
	// void OnSendFailed(Order *o, const ttsdk::OrderProfile &profile);
	void Remove(Order *o);

	void Log();
	void Log(const char *header);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline Quantity GetAvailableQuantity() const
	{
		constexpr int INDEX = SideTraits<SIDE>::INDEX;
		const Quantity netSidePosition = filledQty[INDEX] - filledQty[INDEX ^ 1];
		return std::max(settings.maxPosition - netSidePosition - workingQty[INDEX], Quantity(0));
	}

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline Quantity GetWorkingQty() const { return workingQty[SideTraits<SIDE>::INDEX]; }

	inline Quantity GetWorkingQty(const KTN::ORD::KOrderSide::Enum side)
	{
		switch (side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
				return workingQty[0];
			case KTN::ORD::KOrderSide::Enum::SELL:
				return workingQty[1];
			default:
				return Quantity(0);
		}
	}

	inline Quantity GetFilledQty(const KTN::ORD::KOrderSide::Enum side)
	{
		switch (side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
				return filledQty[0];
			case KTN::ORD::KOrderSide::Enum::SELL:
				return filledQty[1];
			default:
				return Quantity(0);
		}
	}

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline Quantity GetFilledQty() const { return filledQty[SideTraits<SIDE>::INDEX]; }

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline void AddFilledQty(const Quantity qty) { filledQty[SideTraits<SIDE>::INDEX] += qty; }

	inline Quantity GetPosition() const { return filledQty[0] - filledQty[1]; }
	inline Quantity GetPositionLimit() const { return settings.maxPosition; }

	const std::string StrategyId;

	inline void OnKillswitchUpdate(const bool state)
	{
		killswitchEnabled.store(state, std::memory_order_release);
	}

	inline bool IsKillswitchEnabled() const
	{
		return killswitchEnabled.load(std::memory_order_relaxed);
	}

private:
	InstrumentRisk(const InstrumentRisk &) = delete;
	InstrumentRisk &operator=(const InstrumentRisk &) = delete;
	InstrumentRisk(const InstrumentRisk &&) = delete;
	InstrumentRisk &operator=(const InstrumentRisk &&) = delete;

	const InstrumentRiskSettings &settings;
	Quantity workingQty[2] { Quantity(0), Quantity(0) };
	Quantity filledQty[2] { Quantity(0), Quantity(0) };

	template<KTN::ORD::KOrderSide::Enum SIDE>
	RiskStatus trySubmit(Order *o);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	RiskStatus tryModifyUp(Order *o, const Quantity newOrderQty);

	bool isStrategyOrder(const Order *o) const;

	std::atomic_bool killswitchEnabled { false };
};

}

#include "InstrumentRisk.ipp"
