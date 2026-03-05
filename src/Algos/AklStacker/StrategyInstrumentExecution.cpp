#include "StrategyInstrumentExecution.hpp"
#include "Time.hpp"

#include <ExchangeHandler/IExchangeSender.hpp>
#include <Orders/OrderManagerV5.hpp>
#include "AklStacker.hpp"

namespace akl::stacker
{

StrategyInstrumentExecution::StrategyInstrumentExecution(
	AklStacker &strategy,
	QuoterV3 &quoter,
	const Time &time,
	IExchangeSender &orderSender,
	OrderManagerV5 &orderManager,
	const InstrumentRiskSettings &riskSettings,
	const RateLimitSettings &rateLimitSettings,
	const char *strategyId,
	StrategyNotifier &notifier/*,
	stacker::Strategy &strategy*/)
: strategy(strategy)
, notifier(notifier)
, quoter(quoter)
, time(time)
, orderSender(orderSender)
, orderManager(orderManager)
, riskSettings(riskSettings)
, symbol(strategy.GetSymbol())
, exchange(strategy.GetQuoteExchange())
, rateLimiter(rateLimitSettings)
, risk(strategyId, riskSettings)
{

}

void StrategyInstrumentExecution::Create(Order *o, const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px)
{
	quoter.NextNew(o->orderPod, o->side, 0, static_cast<uint32_t>(o->targetQuantity.value()), o->price.AsShifted()); // TODO this takes an int price

	o->orderReqId = o->orderPod.orderReqId;

	o->orderPod.price = o->price;
	o->orderPod.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;
	o->orderPod.OrdState = KOrderState::INFLIGHT;
	o->orderPod.callbackid = strategy.ExchangeCallbackId();
	o->orderPod.OrdExch = exchange;
	o->orderPod.OrdType = KTN::ORD::KOrderType::Enum::LIMIT;

	o->orderPod.OrdTif = strategy.GetSettings().quoteInstrumentSettings.timeInForce == static_cast<int>(KTN::ORD::KOrderTif::Enum::GTC)
		? KTN::ORD::KOrderTif::Enum::GTC
		: KTN::ORD::KOrderTif::Enum::DAY;

	o->tif = o->orderPod.OrdTif;
}

void StrategyInstrumentExecution::Submit(Order *o)
{
	o->ackedQuantity = o->targetQuantity;
	o->ackedPrice = o->price;
	const Timestamp now = time.Now();
	const RateLimiter::Result result = rateLimiter.TrySubmit(now, o);
	if (result == RateLimiter::Result::ALLOWED)
	{
		internalSubmit(o);
	}
	else if (result == RateLimiter::Result::REJECTED)
	{
		ordersRejectedInternally.push_back(o);
		LOGWARN("Rate limiter rejected order {} {} {} {}@{}",
			o->orderReqId,
			o->strategyOrderId,
			KTN::ORD::KOrderSide::toString(o->side),
			o->targetQuantity.value(),
			o->price.AsShifted());
	}
	else if (result == RateLimiter::Result::QUEUED)
	{
		LOGWARN("Queued submit for order {} {} {} {}@{}",
			o->orderReqId,
			o->strategyOrderId,
			KTN::ORD::KOrderSide::toString(o->side),
			o->targetQuantity.value(),
			o->price.AsShifted());
	}
}

void StrategyInstrumentExecution::Modify(Order *o, const Price px)
{
	const Timestamp now = time.Now();
	const RateLimiter::Result result = rateLimiter.TryModify(now, o, px);
	if (result == RateLimiter::ALLOWED)
	{
		internalModify(o, px);
	}
}

void StrategyInstrumentExecution::Modify(Order *o, const Quantity qty)
{
	const Timestamp now = time.Now();
	const RateLimiter::Result result = rateLimiter.TryModify(now, o, qty);
	if (result == RateLimiter::Result::ALLOWED)
	{
		internalModify(o, qty);
	}
}

void StrategyInstrumentExecution::Cancel(Order *o)
{
	const Timestamp now = time.Now();
	o->cancelTime = now;
	const RateLimiter::Result result = rateLimiter.TryCancel(now, o);
	if (result == RateLimiter::Result::ALLOWED)
	{
		internalCancel(o);
	}
	else if (result == RateLimiter::Result::UPDATED)
	{
		ordersCanceledInternally.push_back(o);
	}
}

void StrategyInstrumentExecution::Process(const Timestamp now)
{
	rateLimiter.CheckTransactions(now);

	while (rateLimiter.TransactionsLeft() > 0)
	{
		RateLimiter::OrderAction action = rateLimiter.GetNextAction(now);
		if (action.o != nullptr && action.o && action.type != RateLimiter::ActionType::NONE)
		{
			switch (action.type)
			{
				case RateLimiter::ActionType::SUBMIT:
					internalSubmit(action.o);
					break;
				case RateLimiter::ActionType::PRICE_MODIFY:
					if (action.px)
					{
						internalModify(action.o, *action.px);
					}
					else
					{
						LOGWARN("Action of type PRICE_MODIFY for order {} has null price", action.o->orderReqId);
					}
					break;
				case RateLimiter::ActionType::QUANTITY_MODIFY:
					if (action.qty)
					{
						internalModify(action.o, *action.qty);
					}
					else
					{
						LOGWARN("Action of type QUANTITY_MODIFY for order {} has null quantity", action.o->orderReqId);
					}
					break;
				case RateLimiter::ActionType::CANCEL:
					internalCancel(action.o);
					break;
				default:
					break;
			}
		}
		else
		{
			break;
		}
	}
}

void StrategyInstrumentExecution::ProcessCallbacks()
{
	for (OrderListType::iterator it = ordersCanceledInternally.begin(); it != ordersCanceledInternally.end(); ++it)
	{
		Order *o = *it;
		strategy.OnInternalCancel(o);
	}
	ordersCanceledInternally.clear();

	for (OrderListType::iterator it = ordersRejectedInternally.begin(); it != ordersRejectedInternally.end(); ++it)
	{
		Order *o = *it;
		strategy.OnInternalReject(o);
	}
	ordersRejectedInternally.clear();
}

void StrategyInstrumentExecution::OnAck(Order *o)
{
	std::list<Order *>::iterator cancelIt = std::find(ordersToCancelOnAck.begin(), ordersToCancelOnAck.end(), o);
	std::list<QueuedModify>::iterator modifyIt = std::find_if(
		ordersToModifyOnAck.begin(),
		ordersToModifyOnAck.end(),
		[o](const QueuedModify &qm) { return qm.o == o; });
	if (cancelIt != ordersToCancelOnAck.end())
	{
		ordersToCancelOnAck.erase(cancelIt);
		if (modifyIt != ordersToModifyOnAck.end())
		{
			ordersToModifyOnAck.erase(modifyIt);
		}
		Cancel(o);
	}
	else if (modifyIt != ordersToModifyOnAck.end())
	{
		QueuedModify qm = *modifyIt;
		ordersToModifyOnAck.erase(modifyIt);
		o->pendingModify.store(false, std::memory_order_release);
		if (qm.px)
		{
			Modify(o, *qm.px);
		}
		else if (qm.qty)
		{
			Modify(o, *qm.qty);
		}
	}
	else if (o->cancelQueued.load(std::memory_order_acquire))
	{
		// Do nothing here.
		// Cancel will be sent from the rate limiter.
		LOGINFO("OnAck: Order {} had cancel queued, waiting for rate limiter to send cancel", o->orderReqId);
	}
	else if (o->pendingCancel.load(std::memory_order_acquire))
	{
		LOGINFO("OnAck: Failed to find order {} in pending cancels waiting for ack", o->orderReqId);
		internalCancel(o);
	}
}

void StrategyInstrumentExecution::OnModAck(Order *o, const KTN::OrderPod &ko)
{
	risk.OnModAck(o, ko);
}

void StrategyInstrumentExecution::OnCancelAck(Order *o, const KTN::OrderPod &ko)
{
	risk.OnCancelAck(o, ko);
}

void StrategyInstrumentExecution::OnFill(Order *o, const KTN::OrderPod &ko)
{
	risk.OnFill(o, ko);
}

void StrategyInstrumentExecution::OnSubmitReject(Order *o, const KTN::OrderPod &ko)
{
	risk.OnSubmitReject(o, ko);
}

void StrategyInstrumentExecution::OnCancelReplaceReject(Order *o, const KTN::OrderPod &ko)
{
	risk.OnModReject(o, ko);
}

// void StrategyInstrumentExecution::UpdateRisk(Order *o, const ttsdk::ExecutionReportPtr &execRpt)
// {
// 	risk.OnExecutionReport(o, execRpt);
// }

// void StrategyInstrumentExecution::OnReject(Order *o, const ttsdk::RejectResponsePtr &rejResp)
// {
// 	risk.OnReject(o, rejResp);
// }

// void StrategyInstrumentExecution::OnSendFailed(Order *o, const ttsdk::OrderProfile &profile)
// {
// 	risk.OnSendFailed(o, profile);
// }

void StrategyInstrumentExecution::OnStrategyPause()
{
	//rateLimiter.OnStrategyPause(ordersCanceledInternally);
}

void StrategyInstrumentExecution::CancelQueuedActions()
{
	//rateLimiter.CancelAll(ordersCanceledInternally);
}

void StrategyInstrumentExecution::onKillswitchUpdate(const bool state)
{
	risk.OnKillswitchUpdate(state);
	if (state)
	{
		rateLimiter.CancelAll(ordersCanceledInternally);
	}
}

void StrategyInstrumentExecution::internalSubmit(Order *o)
{
	const RiskStatus riskStatus = risk.TrySubmit(o);
	if (riskStatus == RiskStatus::RISK_OK)
	{
		orderManager.onSentOrderFast(o->orderPod);
		o->orderPod.recvTime = ReceiveTime;
		o->orderPod.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		orderSender.Send(o->orderPod, KTN::ORD::KOrderAction::Enum::ACTION_NEW);
		o->submitted = true;

		LOGINFO("Submit | {} | {} | {} {}@{}",
			o->orderReqId,
			o->orderPod.ordernum,
			KTN::ORD::KOrderSide::toString(o->side),
			o->targetQuantity.value(),
			o->price.AsShifted());
	}
	else
	{
		LOGWARN("Strategy {} failed to submit order {} due to risk error: {}",
			risk.StrategyId.c_str(), o->orderReqId, static_cast<int16_t>(riskStatus));
		risk.Log();
		ordersRejectedInternally.push_back(o);

		if (riskStatus == RiskStatus::RISK_CLIP_EXCEEDED)
		{
			const std::string str = fmt::format("Submit failed: {} order quantity {} exceeded internal max order size of {}", symbol, o->targetQuantity.value(), riskSettings.maxOrderQty.value());
			LOGWARN(str.c_str());
			notifier.Notify(str, KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
		}
		else if (riskStatus == RiskStatus::RISK_POSITION_EXCEEDED)
		{
			const std::string str = fmt::format("Submit failed: {} {} position limit exceeded. Qty={} Working={} Pos={} Limit={}",
				symbol,
				KTN::ORD::KOrderSide::toString(o->side),
				o->targetQuantity.value(),
				risk.GetWorkingQty(o->side).value(),
				risk.GetPosition().value(),
				riskSettings.maxPosition.value());
			LOGWARN(str.c_str());
			notifier.Notify(str, KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
		}
		else
		{
			const std::string str = fmt::format("Submit failed in {} due to unknown risk error", symbol);
			LOGWARN(str.c_str());
			notifier.Notify(str, KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
		}
	}
}

void StrategyInstrumentExecution::internalModify(Order *o, const Price px)
{
	if (likely(o->cmeId > 0))
	{
		const RiskStatus riskStatus = risk.TryModify(o, px);
		if (riskStatus == RiskStatus::RISK_OK)
		{
			KTN::OrderPod ko = { };

			quoter.NextMod(ko, o->side, o->orderReqId, o->cmeId, static_cast<uint32_t>(o->targetQuantity.value()), px.AsShifted());
			ko.price = px;
			// Need to set leaves and fill qtys, as these are stored in OrderManagerV5, and used if this transaction is rejected
			ko.leavesqty = o->WorkingQuantity() > Quantity(0) ? o->WorkingQuantity().value() : 0;
			ko.fillqty = o->filledQuantity > Quantity(0) ? o->filledQuantity.value() : 0;
			ko.OrdTif = o->tif;
			OrderUtils::fastCopy(ko.origordernum, o->orderPod.origordernum, sizeof(ko.origordernum));
			OrderUtils::fastCopy(o->orderPod.origordernum, ko.ordernum, sizeof(o->orderPod.origordernum));

			orderManager.onSentOrderMod(ko);
			ko.recvTime = ReceiveTime;
			ko.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			orderSender.Send(ko, KTN::ORD::KOrderAction::Enum::ACTION_MOD);

			LOGINFO("Modify | {} | {} | {} {}@{}",
				o->orderReqId,
				ko.ordernum,
				KTN::ORD::KOrderSide::toString(o->side),
				o->targetQuantity.value(),
				o->price.AsShifted());
		}
		else
		{
			LOGWARN("Strategy {} failed to modify order {} due to risk error: %{}", strategy.StrategyId(), o->orderReqId, static_cast<int16_t>(riskStatus));
			risk.Log();

			if (riskStatus == RiskStatus::RISK_KILLSWITCH_ENABLED)
			{
				const std::string str = fmt::format("Modify failed in {} due to killswitch enabled", symbol);
				LOGWARN(str.c_str());
				notifier.Notify(str, KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
			}
			else
			{
				const std::string str = fmt::format("Modify failed in {} due to unknown risk error", symbol);
				LOGWARN(str.c_str());
				notifier.Notify(str, KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
			}
		}
	}
	else
	{
		o->pendingModify.store(true, std::memory_order_release);

		std::list<QueuedModify>::iterator modifyIt = std::find_if(
			ordersToModifyOnAck.begin(),
			ordersToModifyOnAck.end(),
			[o](const QueuedModify &qm) { return qm.o == o; });
		if (modifyIt != ordersToModifyOnAck.end())
		{
			ordersToModifyOnAck.erase(modifyIt);
		}

		ordersToModifyOnAck.push_back({o, px, std::nullopt});
		LOGWARN("Queueing modify for order {} because it isn't acked", o->orderReqId);
	}
}

void StrategyInstrumentExecution::internalModify(Order *o, const Quantity qty)
{
	if (likely(o->cmeId > 0))
	{
		const RiskStatus riskStatus = risk.TryModify(o, qty);
		if (riskStatus == RiskStatus::RISK_OK)
		{
			o->targetQuantity = qty;
			KTN::OrderPod ko = { };
			quoter.NextMod(ko, o->side, o->orderReqId, o->cmeId, static_cast<uint32_t>(qty.value()), o->price.AsShifted());
			ko.price = o->price;
			// Need to set leaves and fill qtys, as these are stored in OrderManagerV5, and used if this transaction is rejected
			ko.leavesqty = o->WorkingQuantity() > Quantity(0) ? o->WorkingQuantity().value() : 0;
			ko.fillqty = o->filledQuantity > Quantity(0) ? o->filledQuantity.value() : 0;
			ko.OrdTif = o->tif;
			OrderUtils::fastCopy(ko.origordernum, o->orderPod.origordernum, sizeof(ko.origordernum));
			OrderUtils::fastCopy(o->orderPod.origordernum, ko.ordernum, sizeof(o->orderPod.origordernum));

			orderManager.onSentOrderMod(ko);
			ko.recvTime = ReceiveTime;
			ko.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			orderSender.Send(ko, KTN::ORD::KOrderAction::Enum::ACTION_MOD);

			LOGINFO("Modify | {} | {} | {} {}@{}",
				o->orderReqId,
				ko.ordernum,
				KTN::ORD::KOrderSide::toString(o->side),
				o->targetQuantity.value(),
				o->price.AsShifted());
		}
		else
		{
			LOGWARN("Strategy {} failed to modify order {} due to risk error: %{}", strategy.StrategyId(), o->orderReqId, static_cast<int16_t>(riskStatus));
			risk.Log();

			if (riskStatus == RiskStatus::RISK_CLIP_EXCEEDED)
			{
				const std::string str = fmt::format("Modify failed: {} order quantity {} exceeded internal max order size of {}", symbol, qty.value(), riskSettings.maxOrderQty.value());
				LOGWARN(str.c_str());
				notifier.Notify(str, KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
			}
			else if (riskStatus == RiskStatus::RISK_POSITION_EXCEEDED)
			{
				const std::string str = fmt::format("Modify failed: {} position limit exceeded. Qty={} Working={} Pos={} Limit={}", symbol, (qty - o->targetQuantity).value(), risk.GetWorkingQty(o->side).value(), risk.GetPosition().value(), riskSettings.maxPosition.value());
				LOGWARN(str.c_str());
				notifier.Notify(str, KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
			}
			else if (riskStatus == RiskStatus::RISK_KILLSWITCH_ENABLED)
			{
				const std::string str = fmt::format("Modify failed in {} due to killswitch enabled", symbol);
				LOGWARN(str.c_str());
				notifier.Notify(str, KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
			}
			else
			{
				const std::string str = fmt::format("Modify failed in {} due to unknown risk error", symbol);
				LOGWARN(str.c_str());
				notifier.Notify(str, KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
			}
		}
	}
	else
	{
		o->pendingModify.store(true, std::memory_order_release);

		std::list<QueuedModify>::iterator modifyIt = std::find_if(
			ordersToModifyOnAck.begin(),
			ordersToModifyOnAck.end(),
			[o](const QueuedModify &qm) { return qm.o == o; });
		if (modifyIt != ordersToModifyOnAck.end())
		{
			ordersToModifyOnAck.erase(modifyIt);
		}

		ordersToModifyOnAck.push_back({o, std::nullopt, qty});
		LOGWARN("Queueing qty modify for order {} because it isn't acked", o->orderReqId);
	}
}

void StrategyInstrumentExecution::internalCancel(Order *o)
{
	if (likely(o->cmeId != 0))
	{
		KTN::OrderPod ko = { };

		quoter.NextMod(ko, o->side, o->orderReqId, o->cmeId, static_cast<uint32_t>(o->targetQuantity.value()), o->price.AsShifted());
		ko.price = o->price;
		ko.OrdAction = KOrderAction::ACTION_CXL;
		ko.OrdStatus = KOrderStatus::sent_cxl;
		// Need to set leaves and fill qtys, as these are stored in OrderManagerV5, and used if this transaction is rejected
		ko.leavesqty = o->WorkingQuantity() > Quantity(0) ? o->WorkingQuantity().value() : 0;
		ko.fillqty = o->filledQuantity > Quantity(0) ? o->filledQuantity.value() : 0;
		ko.OrdTif = o->tif;
		OrderUtils::fastCopy(ko.origordernum, o->orderPod.origordernum, sizeof(ko.origordernum));

		orderManager.onSentOrderCxl(ko);
		ko.recvTime = ReceiveTime;
		ko.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		orderSender.Send(ko, KTN::ORD::KOrderAction::Enum::ACTION_CXL);

		LOGINFO("Cancel | {} | {} | {} {}@{}",
			o->orderReqId,
			ko.ordernum,
			KTN::ORD::KOrderSide::toString(o->side),
			o->targetQuantity.value(),
			o->price.AsShifted());
	}
	else
	{
		ordersToCancelOnAck.push_back(o);
		LOGWARN("Queueing cancel for order {} because it isn't acked", o->orderReqId);
	}
}

}
