#pragma once

#include "InstrumentRisk.hpp"

#include <akl/BranchPrediction.hpp>
#include <akl/SideTraits.hpp>

#include "../Order.hpp"

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
RiskStatus InstrumentRisk::trySubmit(Order *o)
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;

	if (unlikely(o->targetQuantity > settings.maxOrderQty))
	{
		return RiskStatus::RISK_CLIP_EXCEEDED;
	}

	const Quantity netSidePosition = filledQty[INDEX] - filledQty[INDEX ^ 1];
	if (unlikely(workingQty[INDEX] + netSidePosition + o->targetQuantity > settings.maxPosition))
	{
		return RiskStatus::RISK_POSITION_EXCEEDED;
	}

	workingQty[INDEX] += o->targetQuantity;
	o->lastRiskQuantity = o->targetQuantity;
	Log("Risk Submit");
	return RiskStatus::RISK_OK;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
RiskStatus InstrumentRisk::tryModifyUp(Order *o, const Quantity newOrderQty)
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;

	if (unlikely(newOrderQty > settings.maxOrderQty)) // TODO check how this works with fills?
	{
		return RiskStatus::RISK_CLIP_EXCEEDED;
	}

	// const Quantity netSidePosition = filledQty[INDEX] - filledQty[INDEX ^ 1];
	// TODO
	// const Quantity additionalQty = o->strategyInfo.lastProcessedRequestId >= o->strategyInfo.lastIncreaseRequstId
	// 	? std::max(newOrderQty - o->strategyInfo.targetQuantity, Quantity(0))
	// 	: std::max(newOrderQty - o->strategyInfo.lastRiskQty, Quantity(0));
	// if (unlikely(workingQty[INDEX] + netSidePosition + additionalQty > settings.maxPosition))
	// {
	// 	return RiskStatus::RISK_POSITION_EXCEEDED;
	// }

	if (newOrderQty > o->lastRiskQuantity)
	{
		workingQty[INDEX] += newOrderQty - o->lastRiskQuantity;
		o->lastRiskQuantity = newOrderQty;
	}
	else
	{
		LOGWARN("Risk Modify {}: New quantity is not greater than last risk quantity", StrategyId.c_str());
	}

	Log("Risk Modify");
	return RiskStatus::RISK_OK;
}

}
