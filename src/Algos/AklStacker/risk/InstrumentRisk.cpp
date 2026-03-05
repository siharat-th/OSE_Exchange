#include "InstrumentRisk.hpp"
#include <KT01/Core/Log.hpp>
#include <akl/BranchPrediction.hpp>
#include <akl/PositionStore.hpp>

namespace akl::stacker
{

InstrumentRisk::InstrumentRisk(const char *strategyId, const InstrumentRiskSettings &settings)
: StrategyId(strategyId)
, settings(settings)
{
	const std::optional<PositionStore::PositionInfo> positionInfo = akl::PositionStore::instance().GetPositionInfo(StrategyId);
	if (positionInfo)
	{
		if (positionInfo->bidFills > 0)
		{
			AddFilledQty<KTN::ORD::KOrderSide::Enum::BUY>(Quantity(positionInfo->bidFills));
			LOGINFO("AklStacker {} risk adding {} to bid fill quantity", StrategyId, positionInfo->bidFills);
		}
		if (positionInfo->askFills > 0)
		{
			AddFilledQty<KTN::ORD::KOrderSide::Enum::SELL>(Quantity(positionInfo->askFills));
			LOGINFO("AklStacker {} risk adding {} to ask fill quantity", StrategyId, positionInfo->askFills);
		}
	}
}

void InstrumentRisk::AddWorking(Order *o)
{
	// if (isStrategyOrder(o))
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
				workingQty[0] += o->WorkingQuantity();
				break;
			case KTN::ORD::KOrderSide::Enum::SELL:
				workingQty[1] += o->WorkingQuantity();
				break;
			default:
				break;
		}
		// if (o->order->GetCurrentState())
		// {
		// 	o->lastRiskQuantity = convert<Quantity>(o->order->GetCurrentState()->GetOrderQty());
		// }
		// else
		{
			o->lastRiskQuantity = o->ackedQuantity; // TODO double check this
		}
		Log("Risk Recover");
	}
}

RiskStatus InstrumentRisk::TrySubmit(Order *o)
{
	if (likely(!killswitchEnabled.load(std::memory_order_relaxed)))
	{
		if (o->riskPreallocated)
		{
			return RiskStatus::RISK_OK;
		}
		else// if (isStrategyOrder(o))
		{
			switch (o->side)
			{
				case KTN::ORD::KOrderSide::Enum::BUY:
					return trySubmit<KTN::ORD::KOrderSide::Enum::BUY>(o);
				case KTN::ORD::KOrderSide::Enum::SELL:
					return trySubmit<KTN::ORD::KOrderSide::Enum::SELL>(o);
				default:
					//LOGWARN("Risk Submit %s: Unknown side %hd", StrategyId.c_str(), static_cast<int16_t>(o->side));
					return RiskStatus::RISK_UNKNOWN_ERROR;
			}
		}
		// else
		// {
		// 	return RiskStatus::RISK_UNKNOWN_ERROR;
		// }
	}
	else
	{
		return RiskStatus::RISK_KILLSWITCH_ENABLED;
	}
}

RiskStatus InstrumentRisk::TryModify(Order *o, const Quantity newQty)
{
	//if (isStrategyOrder(o))
	{
		if (newQty > o->lastRiskQuantity)
		{
			if (likely(!killswitchEnabled.load(std::memory_order_relaxed)))
			{
				switch (o->side)
				{
					case KTN::ORD::KOrderSide::Enum::BUY:
					{
						const RiskStatus riskStatus = tryModifyUp<KTN::ORD::KOrderSide::Enum::BUY>(o, newQty);
						if (riskStatus == RiskStatus::RISK_OK)
						{
							// TODO
							//o->lastIncreaseRequstId = o->profile.request_id + 1;
							++o->numPendingMods;
							return RiskStatus::RISK_OK;
						}
						else
						{
							return riskStatus;
						}
					}
					case KTN::ORD::KOrderSide::Enum::SELL:
					{
						const RiskStatus riskStatus = tryModifyUp<KTN::ORD::KOrderSide::Enum::SELL>(o, newQty);
						if (riskStatus == RiskStatus::RISK_OK)
						{
							// TODO
							//o->lastIncreaseRequstId = o->profile.request_id + 1;
							++o->numPendingMods;
							return RiskStatus::RISK_OK;
						}
						else
						{
							return riskStatus;
						}
					}
					default:
						LOGWARN("Risk Modify {}: Unknown side {}", StrategyId.c_str(), static_cast<int16_t>(o->side));
						return RiskStatus::RISK_UNKNOWN_ERROR;
				}
			}
			else
			{
				return RiskStatus::RISK_KILLSWITCH_ENABLED;
			}

		}
		else
		{
			if (newQty > o->ackedQuantity || newQty > o->targetQuantity)
			{
				// TODO
				//o->lastIncreaseRequstId = o->profile.request_id + 1;
			}
			++o->numPendingMods;
			return RiskStatus::RISK_OK;
		}
	}
//	else
	{
//		return RiskStatus::RISK_OK; // TODO check clip here?
	}
}

RiskStatus InstrumentRisk::TryModify(Order *o, const Price px)
{
	if (unlikely(killswitchEnabled.load(std::memory_order_relaxed)))
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
				if (px > o->ackedPrice)
				{
					return RiskStatus::RISK_KILLSWITCH_ENABLED;
				}
				break;
			case KTN::ORD::KOrderSide::Enum::SELL:
				if (px < o->ackedPrice)
				{
					return RiskStatus::RISK_KILLSWITCH_ENABLED;
				}
				break;
			default:
				return RiskStatus::RISK_UNKNOWN_ERROR;
		}
	}
	++o->numPendingMods;
	return RiskStatus::RISK_OK;
}

void InstrumentRisk::OnModAck(Order *o, const KTN::OrderPod &ko)
{
	if (likely(o->numPendingMods > 0))
	{
		--o->numPendingMods;
	}
	else
	{
		LOGWARN("[InstrumentRisk::OnModAck] numPendingMods is {} for order {}", static_cast<int16_t>(o->numPendingMods), o->orderReqId);
		o->numPendingMods = 0;
	}
	if (o->numPendingMods == 0)
	{
		const Quantity orderQty = Quantity(ko.quantity);
		if (orderQty < o->lastRiskQuantity)
		{
			const Quantity qtyToRemove = o->lastRiskQuantity - orderQty;
			switch (o->side)
			{
				case KTN::ORD::KOrderSide::Enum::BUY:
					if (workingQty[0] >= qtyToRemove)
					{
						workingQty[0] -= qtyToRemove;
					}
					else
					{
						workingQty[0] = Quantity(0);
						LOGWARN("Risk Modified {}: Removed quantity greater than working quantity", StrategyId.c_str());
					}
					break;
				case KTN::ORD::KOrderSide::Enum::SELL:
					if (workingQty[1] >= qtyToRemove)
					{
						workingQty[1] -= qtyToRemove;
					}
					else
					{
						workingQty[1] = Quantity(0);
						LOGWARN("Risk Modified {}: Removed quantity greater than working quantity", StrategyId.c_str());
					}
					break;
				default:
					LOGWARN("Risk Modified {}: Unknown side {}", StrategyId.c_str(), static_cast<int16_t>(o->side));
					break;
			}
			o->lastRiskQuantity = orderQty;
		}
	}
	Log("Risk Modified");
}

void InstrumentRisk::OnCancelAck(Order *o, const KTN::OrderPod &ko)
{
	const Quantity canceledQty = o->lastRiskQuantity - o->filledQuantity;//Quantity(ko.quantity) - Quantity(ko.fillqty);

	if (likely(canceledQty > Quantity(0)))
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
				if (workingQty[0] >= canceledQty)
				{
					workingQty[0] -= canceledQty;
				}
				else
				{
					workingQty[0] = Quantity(0);
					LOGWARN("Risk Canceled {}: Canceled quantity greater than working quantity", StrategyId.c_str());
				}
				break;
			case KTN::ORD::KOrderSide::Enum::SELL:
				if (workingQty[1] >= canceledQty)
				{
					workingQty[1] -= canceledQty;
				}
				else
				{
					workingQty[1] = Quantity(0);
					LOGWARN("Risk Canceled {}: Canceled quantity greater than working quantity", StrategyId.c_str());
				}
				break;
			default:
				LOGWARN("Risk Canceled {}: Unknown side {}", StrategyId.c_str(), static_cast<int16_t>(o->side));
				break;
		}
	}

	Log("Risk Canceled");
}

void InstrumentRisk::OnFill(Order *o, const KTN::OrderPod &ko)
{
	const Quantity lastFillQty = Quantity(ko.lastqty);

	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			if (workingQty[0] >= lastFillQty)
			{
				workingQty[0] -= lastFillQty;
			}
			else
			{
				workingQty[0] = Quantity(0);
				LOGWARN("Risk Fill {}: Fill quantity greater than working quantity", StrategyId.c_str());
			}
			filledQty[0] += lastFillQty;
			break;
		case KTN::ORD::KOrderSide::Enum::SELL:
			if (workingQty[1] >= lastFillQty)
			{
				workingQty[1] -= lastFillQty;
			}
			else
			{
				workingQty[1] = Quantity(0);
				LOGWARN("Risk Fill {}: Fill quantity greater than working quantity", StrategyId.c_str());
			}
			filledQty[1] += lastFillQty;
			break;
		default:
			LOGWARN("Risk Fill {}: Unknown side {}", StrategyId.c_str(), static_cast<int16_t>(o->side));
			break;
	}
	Log("Risk Fill");
}

void InstrumentRisk::OnSubmitReject(Order *o, const KTN::OrderPod &ko)
{
	// TODO
	//const Quantity canceledQty = Quantity(ko.quantity);// - convert<Quantity>(execRpt->GetCumQty());
	const Quantity riskQty = o->lastRiskQuantity;//canceledQty;//execRpt->GetRequestId() < o->lastIncreaseRequstId ? o->lastRiskQty : canceledQty;

	if (likely(riskQty > Quantity(0)))
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
				if (workingQty[0] >= riskQty)
				{
					workingQty[0] -= riskQty;
				}
				else
				{
					workingQty[0] = Quantity(0);
					LOGWARN("Risk Submit Reject {}: Rejected quantity greater than working quantity", StrategyId.c_str());
				}
				break;
			case KTN::ORD::KOrderSide::Enum::SELL:
				if (workingQty[1] >= riskQty)
				{
					workingQty[1] -= riskQty;
				}
				else
				{
					workingQty[1] = Quantity(0);
					LOGWARN("Risk Submit Reject {}: Rejected quantity greater than working quantity", StrategyId.c_str());
				}
				break;
			default:
				LOGWARN("Risk Submit Reject {}: Unknown side %hd", StrategyId.c_str(), static_cast<int16_t>(o->side));
				break;
		}
	}
	Log("Risk Submit Rejected");
}

void InstrumentRisk::OnModReject(Order *o, const KTN::OrderPod &ko)
{
	if (likely(o->numPendingMods > 0))
	{
		--o->numPendingMods;
	}
	else
	{
		LOGWARN("[InstrumentRisk::OnModReject] numPendingMods is {} for order {}", static_cast<int16_t>(o->numPendingMods), o->orderReqId);
		o->numPendingMods = 0;
	}
	if (o->numPendingMods == 0)
	{
		const Quantity orderQty = o->ackedQuantity; // quantity is not set on OrderPod for modify rejects, get it from the strategy order instead
		if (orderQty < o->lastRiskQuantity)
		{
			const Quantity qtyToRemove = o->lastRiskQuantity - orderQty;
			switch (o->side)
			{
				case KTN::ORD::KOrderSide::Enum::BUY:
					if (workingQty[0] >= qtyToRemove)
					{
						workingQty[0] -= qtyToRemove;
					}
					else
					{
						workingQty[0] = Quantity(0);
						LOGWARN("Risk Mod Reject {}: Removed quantity greater than working quantity", StrategyId.c_str());
					}
					break;
				case KTN::ORD::KOrderSide::Enum::SELL:
					if (workingQty[1] >= qtyToRemove)
					{
						workingQty[1] -= qtyToRemove;
					}
					else
					{
						workingQty[1] = Quantity(0);
						LOGWARN("Risk Mod Reject {}: Removed quantity greater than working quantity", StrategyId.c_str());
					}
					break;
				default:
					LOGWARN("Risk Mod Reject {}: Unknown side {}", StrategyId.c_str(), static_cast<int16_t>(o->side));
					break;
			}
			o->lastRiskQuantity = orderQty;
		}
	}
	Log("Risk Mod Reject");
}

// void InstrumentRisk::OnExecutionReport(OrderType *o, const ttsdk::ExecutionReportPtr &execRpt)
// {
// 	if (isStrategyOrder(o))
// 	{
// 		if (execRpt->IsFill())
// 		{
// 			const Quantity lastFillQty = convert<Quantity>(execRpt->GetLastFillQty());

// 			switch (o->side)
// 			{
// 				case Side::BID:
// 					if (workingQty[0] >= lastFillQty)
// 					{
// 						workingQty[0] -= lastFillQty;
// 					}
// 					else
// 					{
// 						workingQty[0] = Quantity(0);
// 						LOGWARN("Risk Fill %s: Fill quantity greater than working quantity", StrategyId.c_str());
// 					}
// 					filledQty[0] += lastFillQty;
// 					break;
// 				case Side::ASK:
// 					if (workingQty[1] >= lastFillQty)
// 					{
// 						workingQty[1] -= lastFillQty;
// 					}
// 					else
// 					{
// 						workingQty[1] = Quantity(0);
// 						LOGWARN("Risk Fill %s: Fill quantity greater than working quantity", StrategyId.c_str());
// 					}
// 					filledQty[1] += lastFillQty;
// 					break;
// 				default:
// 					LOGWARN("Risk Fill %s: Unknown side %hd", StrategyId.c_str(), static_cast<int16_t>(o->side));
// 					break;
// 			}
// 			Log("Risk Fill");
// 		}
// 		else if (execRpt->IsDelete())
// 		{
// 			const Quantity canceledQty = convert<Quantity>(execRpt->GetOrderQty()) - convert<Quantity>(execRpt->GetCumQty());

// 			if (likely(canceledQty > Quantity(0)))
// 			{
// 				switch (o->side)
// 				{
// 					case Side::BID:
// 						if (workingQty[0] >= canceledQty)
// 						{
// 							workingQty[0] -= canceledQty;
// 						}
// 						else
// 						{
// 							workingQty[0] = Quantity(0);
// 							LOGWARN("Risk Canceled %s: Canceled quantity greater than working quantity", StrategyId.c_str());
// 						}
// 						break;
// 					case Side::ASK:
// 						if (workingQty[1] >= canceledQty)
// 						{
// 							workingQty[1] -= canceledQty;
// 						}
// 						else
// 						{
// 							workingQty[1] = Quantity(0);
// 							LOGWARN("Risk Canceled %s: Canceled quantity greater than working quantity", StrategyId.c_str());
// 						}
// 						break;
// 					default:
// 						LOGWARN("Risk Canceled %s: Unknown side %hd", StrategyId.c_str(), static_cast<int16_t>(o->side));
// 						break;
// 				}
// 			}

// 			Log("Risk Canceled");
// 		}
// 		else if (execRpt->IsReject())
// 		{
// 			if (!execRpt->IsWorking())
// 			{
// 				const Quantity canceledQty = convert<Quantity>(execRpt->GetOrderQty()) - convert<Quantity>(execRpt->GetCumQty());
// 				const Quantity riskQty = execRpt->GetRequestId() < o->lastIncreaseRequstId ? o->lastRiskQty : canceledQty;

// 				if (likely(riskQty > Quantity(0)))
// 				{
// 					switch (o->side)
// 					{
// 						case Side::BID:
// 							if (workingQty[0] >= riskQty)
// 							{
// 								workingQty[0] -= riskQty;
// 							}
// 							else
// 							{
// 								workingQty[0] = Quantity(0);
// 								LOGWARN("Risk Reject %s: Rejected quantity greater than working quantity", StrategyId.c_str());
// 							}
// 							break;
// 						case Side::ASK:
// 							if (workingQty[1] >= riskQty)
// 							{
// 								workingQty[1] -= riskQty;
// 							}
// 							else
// 							{
// 								workingQty[1] = Quantity(0);
// 								LOGWARN("Risk Reject %s: Rejected quantity greater than working quantity", StrategyId.c_str());
// 							}
// 							break;
// 						default:
// 							LOGWARN("Risk Reject %s: Unknown side %hd", StrategyId.c_str(), static_cast<int16_t>(o->side));
// 							break;
// 					}
// 				}
// 				Log("Risk Rejected");
// 			}
// 		}
// 		else if (execRpt->IsWorking() && execRpt->GetExecType() == ttsdk::ExecType::Replaced)
// 		{
// 			if (execRpt->GetRequestId() >= o->lastIncreaseRequstId)
// 			{
// 				const Quantity orderQty = convert<Quantity>(execRpt->GetOrderQty());
// 				if (orderQty < o->lastRiskQty)
// 				{
// 					const Quantity qtyToRemove = o->lastRiskQty - orderQty;
// 					switch (o->side)
// 					{
// 						case Side::BID:
// 							if (workingQty[0] >= qtyToRemove)
// 							{
// 								workingQty[0] -= qtyToRemove;
// 							}
// 							else
// 							{
// 								workingQty[0] = Quantity(0);
// 								LOGWARN("Risk Modified %s: Removed quantity greater than working quantity", StrategyId.c_str());
// 							}
// 							break;
// 						case Side::ASK:
// 							if (workingQty[1] >= qtyToRemove)
// 							{
// 								workingQty[1] -= qtyToRemove;
// 							}
// 							else
// 							{
// 								workingQty[1] = Quantity(0);
// 								LOGWARN("Risk Modified %s: Removed quantity greater than working quantity", StrategyId.c_str());
// 							}
// 							break;
// 						default:
// 							LOGWARN("Risk Modified %s: Unknown side %hd", StrategyId.c_str(), static_cast<int16_t>(o->side));
// 							break;
// 					}
// 					o->lastRiskQty = orderQty;
// 				}
// 			}
// 			Log("Risk Modified");
// 		}

// 		o->lastProcessedRequestId = std::max(execRpt->GetRequestId(), o->lastProcessedRequestId);
// 	}
// }

// void InstrumentRisk::OnReject(OrderType *o, const ttsdk::RejectResponsePtr &rejResp)
// {
// 	if (isStrategyOrder(o))
// 	{
// 		if (rejResp->GetRequestId() >= o->lastIncreaseRequstId)
// 		{
// 			const ttsdk::ExecutionReportPtr execRpt = o->order->GetCurrentState();
// 			if (execRpt)
// 			{
// 				if (execRpt->IsWorking())
// 				{
// 					const Quantity orderQty = convert<Quantity>(execRpt->GetOrderQty());
// 					if (orderQty < o->lastRiskQty)
// 					{
// 						const Quantity rejectedQty = o->lastRiskQty - orderQty;
// 						switch (o->side)
// 						{
// 							case Side::BID:
// 								if (workingQty[0] >= rejectedQty)
// 								{
// 									workingQty[0] -= rejectedQty;
// 								}
// 								else
// 								{
// 									workingQty[0] = Quantity(0);
// 									LOGWARN("Risk Rejected %s: Removed quantity greater than working quantity", StrategyId.c_str());
// 								}
// 								break;
// 							case Side::ASK:
// 								if (workingQty[1] >= rejectedQty)
// 								{
// 									workingQty[1] -= rejectedQty;
// 								}
// 								else
// 								{
// 									workingQty[1] = Quantity(0);
// 									LOGWARN("Risk Rejected %s: Removed quantity greater than working quantity", StrategyId.c_str());
// 								}
// 								break;
// 							default:
// 								LOGWARN("Risk Rejected %s: Unknown side %hd", StrategyId.c_str(), static_cast<int16_t>(o->side));
// 								break;
// 						}
// 						o->lastRiskQty = orderQty;
// 					}
// 				}
// 			}
// 			Log("Risk Rejected");
// 		}
// 		o->lastProcessedRequestId = std::max(rejResp->GetRequestId(), o->lastProcessedRequestId);
// 	}
// }

// void InstrumentRisk::OnSendFailed(OrderType *o, const ttsdk::OrderProfile &profile)
// {
// 	if (isStrategyOrder(o))
// 	{
// 		if (profile.request_id == 1 && (!o->order->GetCurrentState() || !o->order->GetCurrentState()->IsWorking()))
// 		{
// 			const Quantity failedQty = std::max(o->lastRiskQty, o->targetQuantity);
// 			switch (o->side)
// 			{
// 				case Side::BID:
// 				{
// 					if (likely(workingQty[0] >= failedQty))
// 					{
// 						workingQty[0] -= failedQty;
// 					}
// 					else
// 					{
// 						workingQty[0] = Quantity(0);
// 						LOGWARN("Risk OnSendFailed %s: Failed quantity greater than working quantity", StrategyId.c_str());
// 					}
// 					break;
// 				}
// 				case Side::ASK:
// 				{
// 					if (likely(workingQty[1] >= failedQty))
// 					{
// 						workingQty[1] -= failedQty;
// 					}
// 					else
// 					{
// 						workingQty[1] = Quantity(0);
// 						LOGWARN("Risk OnSendFailed %s: Failed quantity greater than working quantity", StrategyId.c_str());
// 					}
// 					break;
// 				}
// 				default:
// 					LOGWARN("Risk OnSendFailed %s: Unknown side %hd", StrategyId.c_str(), static_cast<int16_t>(o->side));
// 					break;
// 			}
// 		}
// 		else
// 		{
// 			ttsdk::ExecutionReportPtr execRpt = o->order->GetCurrentState();
// 			if (execRpt && execRpt->IsWorking())
// 			{
// 				if (execRpt->GetRequestId() >= o->lastIncreaseRequstId)
// 				{
// 					const Quantity orderQty = convert<Quantity>(execRpt->GetOrderQty());
// 					if (orderQty < o->lastRiskQty)
// 					{
// 						const Quantity qtyToSubtract = o->lastRiskQty - orderQty;
// 						switch (o->side)
// 						{
// 							case Side::BID:
// 								if (likely(workingQty[0] >= qtyToSubtract))
// 								{
// 									workingQty[0] -= o->lastRiskQty - orderQty;
// 								}
// 								else
// 								{
// 									workingQty[0] = Quantity(0);
// 									LOGWARN("Risk OnSendFailed %s: Mod down quantity greater than working quantity", StrategyId.c_str());
// 								}
// 								break;
// 							case Side::ASK:
// 								if (likely(workingQty[1] >= qtyToSubtract))
// 								{
// 									workingQty[1] -= o->lastRiskQty - orderQty;
// 								}
// 								else
// 								{
// 									workingQty[1] = Quantity(0);
// 									LOGWARN("Risk OnSendFailed %s: Mod down quantity greater than working quantity", StrategyId.c_str());
// 								}
// 								break;
// 							default:
// 								LOGWARN("Risk Modified %s: Unknown side %hd", StrategyId.c_str(), static_cast<int16_t>(o->side));
// 								break;
// 						}
// 						o->lastRiskQty = orderQty;
// 					}
// 				}
// 			}
// 		}
// 		Log("Risk OnSendFailed");
// 	}
// }

void InstrumentRisk::Remove(Order *o)
{
	// if (isStrategyOrder(o))
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
			{
				if (likely(workingQty[0] >= o->lastRiskQuantity))
				{
					workingQty[0] -= o->lastRiskQuantity;
				}
				else
				{
					workingQty[0] = Quantity(0);
					LOGWARN("Risk Remove {}: Order quantity greater than working quantity", StrategyId.c_str());
				}
				break;
			}
			case KTN::ORD::KOrderSide::Enum::SELL:
			{
				if (likely(workingQty[1] >= o->lastRiskQuantity))
				{
					workingQty[1] -= o->lastRiskQuantity;
				}
				else
				{
					workingQty[1] = Quantity(0);
					LOGWARN("Risk Remove {}: Order quantity greater than working quantity", StrategyId.c_str());
				}
				break;
			}
			default:
				LOGWARN("Risk Remove {}: Unknown side %hd", StrategyId.c_str(), static_cast<int16_t>(o->side));
				break;
		}
		o->lastRiskQuantity = Quantity(0);
		o->riskPreallocated = false;
		Log("Risk Remove");
	}
}

void InstrumentRisk::Log()
{
	LOGINFO("Bid: [ working={} filled={} ] | Ask: [ working={} filled={} ]",
		workingQty[0].value(), filledQty[0].value(), workingQty[1].value(), filledQty[1].value());
}

void InstrumentRisk::Log(const char *header)
{
	LOGINFO("{} {} | Bid: [ working={} filled={} ] | Ask: [ working={} filled={} ]",
		header, StrategyId.c_str(), workingQty[0].value(), filledQty[0].value(), workingQty[1].value(), filledQty[1].value());
}

// bool InstrumentRisk::isStrategyOrder(const OrderType *o) const
// {
// 	return o != nullptr
// 		&& o->order != nullptr
// 		&& (!o->order->GetCurrentState() || strcmp(o->order->GetCurrentState()->GetParentOrderId(), StrategyId.c_str()) == 0);
// }

}
