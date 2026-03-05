#include "Instrument.hpp"
#include "AklStacker.hpp"
#include "OrderHelpers.hpp"
#include "StrategyNotifier.hpp"

#include "messages/StackInfoMessage.hpp"

namespace akl::stacker
{

Instrument::Instrument(
	AklStacker &strategy,
	QuoterV3 &quoter,
	KTN::Core::ObjectPool<KTN::OrderPod> &orderPool,
	IExchangeSender &orderSender,
	StrategyNotifier &notifier,
	const AklStackerParams &settings,
	const Book &book,
	const Price tickIncrement)
: TickIncrement(tickIncrement)
, Symbol(strategy.GetSymbol())
, strategy(strategy)
, quoter(quoter)
, time(strategy.GetTime())
, orderSender(orderSender)
, settings(settings)
, book(book)
, orderPool(1024, 32)
, pm(tickIncrement)
, pc(book, settings.quoteInstrumentSettings, pricingModel[0], pm)
, pricingModel{{book, pm, time, sh, spreaderPricingModel, settings.quoteInstrumentSettings, 0, tickIncrement}, {book, pm, time, sh, spreaderPricingModel, settings.quoteInstrumentSettings, 1, tickIncrement}}
, stack
	{
		{
			settings.quoteInstrumentSettings,
			settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0],
			settings.quoteInstrumentSettings.askSettings.stackSideSettings[0],
			*this,
			strategy.GetTime(),
			tickIncrement,
			0
		},
		{
			settings.quoteInstrumentSettings,
			settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1],
			settings.quoteInstrumentSettings.askSettings.stackSideSettings[1],
			*this,
			strategy.GetTime(),
			tickIncrement,
			1
		}
	}
, mstack
	{
		{
			settings.quoteInstrumentSettings,
			settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0],
			settings.quoteInstrumentSettings.askSettings.stackSideSettings[0],
			*this,
			tickIncrement,
			0
		},
		{
			settings.quoteInstrumentSettings,
			settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1],
			settings.quoteInstrumentSettings.askSettings.stackSideSettings[1],
			*this,
			tickIncrement,
			1
		}
	}
, prorataPricingModel(book, settings.quoteInstrumentSettings, spreaderPricingModel, strategy.GetTime(), strategy.StrategyId())
, prorataQuoter(settings.quoteInstrumentSettings, strategy.GetTime(), *this)
, spreaderPricingModel(settings.spreaderPricingSettings, book, strategy.GetBook(1), tickIncrement, notifier)
, strategyExecution(
	strategy,
	quoter,
	strategy.GetTime(),
	orderSender,
	strategy.GetOrderManager(),
	settings.quoteInstrumentSettings.riskSettings,
	settings.quoteInstrumentSettings.rateLimitSettings,
	strategy.StrategyId().c_str(),
	notifier)
, sh(strategy, *this, settings, book, strategy.GetTime(), notifier)
, hm(settings.quoteInstrumentSettings, book, *this, notifier, pm, strategy.GetTime(), sh)
, notifier(notifier)
{

}

void Instrument::OnMarketStateChange(
	const KT01::DataStructures::MarketDepth::BookSecurityStatus oldState,
	const KT01::DataStructures::MarketDepth::BookSecurityStatus newState)
{
	if (oldState == KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade)
	{
		if (newState == KT01::DataStructures::MarketDepth::BookSecurityStatus::Close
			|| newState == KT01::DataStructures::MarketDepth::BookSecurityStatus::TradingHalt)
		{
			sh.CancelAll();
		}
		if (newState != KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade)
		{
			strategyExecution.CancelQueuedActions();
			strategyExecution.ProcessCallbacks();
		}
	}
}

bool Instrument::OnTrade(
	const KTN::ORD::KOrderSide::Enum aggressingSide,
	const Quantity qty,
	const Price px)
{
	bool result = hm.HandleTrade(time.Now(), aggressingSide, qty, px);
	result |= Recalculate(false);
	return result;
}

bool Instrument::Recalculate(
	const bool doRequote,
	const std::optional<FillInfo> fillInfo,
	const std::optional<CanceledHedgeInfo> canceledHedgeInfo)
{
	bool spreaderStrikeChanged = false;
	bool priceTargetsChanged = false;
	bool result = false;
	bool stackMoved = false;
	bool prorataOrdersUpdated = false;

	if (strategy.IsRunning())
	{
		if (hasSpreader)
		{
			spreaderStrikeChanged = spreaderPricingModel.Recalculate();
		}

		priceTargetsChanged = pricingModel[0].Recalculate(
			strategy.IsRunning(),
			stack[0].StackPrice<KTN::ORD::KOrderSide::Enum::BUY>(),
			stack[0].StackPrice<KTN::ORD::KOrderSide::Enum::SELL>());
		priceTargetsChanged |= pricingModel[1].Recalculate(
			strategy.IsRunning(),
			stack[1].StackPrice<KTN::ORD::KOrderSide::Enum::BUY>(),
			stack[1].StackPrice<KTN::ORD::KOrderSide::Enum::BUY>());
		pc.Recalculate();

		const std::optional<Price> bestQuotePx[2] { GetBestQuotePrice<KTN::ORD::KOrderSide::Enum::BUY>(), GetBestQuotePrice<KTN::ORD::KOrderSide::Enum::SELL>() };

		result = priceTargetsChanged;
		result |= prorataPricingModel.Recalculate(bestQuotePx[0], bestQuotePx[1]);

		const bool canHedge = book.IsOpen();
		const bool canQuote = canHedge || (settings.quoteInstrumentSettings.quoteInPreopen && book.IsPreopen() && !book.NoCancel());
		if (likely(canQuote))
		{
			stackMoved = stack[0].Update(pricingModel[0].BidLean(), pricingModel[0].AskLean(), doRequote); // TODO handle requote
			if (settings.quoteInstrumentSettings.secondStackEnabled)
			{
				stackMoved |= stack[1].Update(pricingModel[1].BidLean(), pricingModel[1].AskLean(), doRequote); // TODO handle requote
			}
			stackMoved |= mstack[0].Update(pricingModel[0].MaintainBid(), pricingModel[0].MaintainAsk(), pricingModel[0].BidLean(), pricingModel[0].AskLean());
			if (settings.quoteInstrumentSettings.secondStackEnabled)
			{
				stackMoved |= mstack[1].Update(pricingModel[1].MaintainBid(), pricingModel[1].MaintainAsk(), pricingModel[1].BidLean(), pricingModel[1].AskLean());
			}

			prorataOrdersUpdated |= prorataQuoter.Update(prorataPricingModel.BidTarget(), prorataPricingModel.AskTarget(), doRequote);
		}
		else if (doRequote)
		{
			LOGWARN("Strategy {} received requote but canQuote is false | marketState={}", strategy.StrategyId(), static_cast<int>(book.MarketState()));
		}
		if (likely(canHedge))
		{
			result |= hm.HandleOffset(fillInfo, canceledHedgeInfo);
		}

		if (likely(canQuote))
		{
			stack[0].SendQueuedSubmits();
			if (settings.quoteInstrumentSettings.secondStackEnabled)
			{
				stack[1].SendQueuedSubmits();
			}
			stackMoved |= stack[0].CheckRequote(time.Now());
			if (settings.quoteInstrumentSettings.secondStackEnabled)
			{
				stackMoved |= stack[1].CheckRequote(time.Now());
			}
		}

		if (likely(canHedge))
		{
			strategyExecution.Process(time.Now());
		}
		else
		{
			sh.CancelMissedStops();
		}
	}
	else
	{
		if (likely(book.IsOpen()))
		{
			result |= hm.CheckStops();
			strategyExecution.Process(time.Now());
		}
		else
		{
			sh.CancelMissedStops();
		}

		if (hasSpreader)
		{
			spreaderStrikeChanged = spreaderPricingModel.Recalculate();
		}

		priceTargetsChanged = pricingModel[0].Recalculate(
			strategy.IsRunning(),
			stack[0].StackPrice<KTN::ORD::KOrderSide::Enum::BUY>(),
			stack[0].StackPrice<KTN::ORD::KOrderSide::Enum::SELL>());
		priceTargetsChanged |= pricingModel[1].Recalculate(
			strategy.IsRunning(),
			stack[1].StackPrice<KTN::ORD::KOrderSide::Enum::BUY>(),
			stack[1].StackPrice<KTN::ORD::KOrderSide::Enum::BUY>());
		pc.Recalculate();

		const std::optional<Price> bestQuotePx[2] { GetBestQuotePrice<KTN::ORD::KOrderSide::Enum::BUY>(), GetBestQuotePrice<KTN::ORD::KOrderSide::Enum::SELL>() };

		result = priceTargetsChanged;
		result |= prorataPricingModel.Recalculate(bestQuotePx[0], bestQuotePx[1]);

		if (unlikely(doRequote))
		{
			LOGWARN("Strategy {} received requote but is not running", strategy.StrategyId());
		}
	}

	if (book.HasBid() && book.HasPreviousBid())
	{
		if (book.BestBid() > book.PreviousBestBid())
		{
			sh.HandleTickIn<KTN::ORD::KOrderSide::Enum::SELL>();
		}
	}
	if (book.HasAsk() && book.HasPreviousAsk())
	{
		if (book.BestAsk() < book.PreviousBestAsk())
		{
			sh.HandleTickIn<KTN::ORD::KOrderSide::Enum::BUY>();
		}
	}

	if (stackMoved)
	{
		strategy.QueueStackInfoUpdate();
	}
	if (priceTargetsChanged)
	{
		strategy.QueueStackTargetsUpdate();
	}
	if (spreaderStrikeChanged)
	{
		strategy.QueueSpreaderStrikeUpdate();
	}
	if (disableAdditionalHedge[0])
	{
		if (prorataPricingModel.DisableAdditionalHedge<KTN::ORD::KOrderSide::Enum::BUY>())
		{
			const std::optional<ProrataProfileUpdateMessage> &profile = prorataPricingModel.GetProfile<KTN::ORD::KOrderSide::Enum::BUY>();
			if (profile)
			{
				strategy.OnInternalProrataProfileUpdate(*profile);
				const std::optional<ProrataProfileUpdateMessage> &oppProfile = prorataPricingModel.GetProfile<KTN::ORD::KOrderSide::Enum::SELL>();
				if (oppProfile && oppProfile->name == profile->name)
				{
					prorataPricingModel.DisableAdditionalHedge<KTN::ORD::KOrderSide::Enum::SELL>();
				}
			}
		}
		disableAdditionalHedge[0] = false;
	}
	if (disableAdditionalHedge[1])
	{
		if (prorataPricingModel.DisableAdditionalHedge<KTN::ORD::KOrderSide::Enum::SELL>())
		{
			const std::optional<ProrataProfileUpdateMessage> &profile = prorataPricingModel.GetProfile<KTN::ORD::KOrderSide::Enum::SELL>();
			if (profile)
			{
				strategy.OnInternalProrataProfileUpdate(*profile);
				const std::optional<ProrataProfileUpdateMessage> &oppProfile = prorataPricingModel.GetProfile<KTN::ORD::KOrderSide::Enum::BUY>();
				if (oppProfile && oppProfile->name == profile->name)
				{
					prorataPricingModel.DisableAdditionalHedge<KTN::ORD::KOrderSide::Enum::BUY>();
				}
			}
		}
		disableAdditionalHedge[1] = false;
	}
	bool sendProrataConfigUpdate = false;
	if (prorataPricingModel.SwitchedToAlternateProfile[0])
	{
		sendProrataConfigUpdate = true;
		prorataPricingModel.SwitchedToAlternateProfile[0] = false;
		strategy.GetSettings().quoteInstrumentSettings.bidSettings.prorataProfile = strategy.GetSettings().quoteInstrumentSettings.bidSettings.alternateProrataProfile;
		strategy.GetSettings().quoteInstrumentSettings.bidSettings.alternateProrataProfile.clear();
	}
	if (prorataPricingModel.SwitchedToAlternateProfile[1])
	{
		sendProrataConfigUpdate = true;
		prorataPricingModel.SwitchedToAlternateProfile[1] = false;
		strategy.GetSettings().quoteInstrumentSettings.askSettings.prorataProfile = strategy.GetSettings().quoteInstrumentSettings.askSettings.alternateProrataProfile;
		strategy.GetSettings().quoteInstrumentSettings.askSettings.alternateProrataProfile.clear();
	}
	if (sendProrataConfigUpdate)
	{
		strategy.QueueSettingsUpdate();
	}

	strategyExecution.ProcessCallbacks();

	if (spreaderStrikeChanged)
	{
		spreaderPricingModel.LogStrikes();
	}
	if (spreaderPricingModel.TicksChanged)
	{
		spreaderPricingModel.TicksChanged = false;
		spreaderPricingModel.LogTicks();
	}

	return result || stackMoved || prorataOrdersUpdated;
}

bool Instrument::CanAddOrderToStack(
		const KTN::ORD::KOrderSide::Enum side,
		const uint64_t reqId,
		const Price px,
		const bool mstackAdoptAllowed,
		bool stack1Adopt,
		bool stack2Adopt) const
{
	switch (side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
		case KTN::ORD::KOrderSide::Enum::SELL:
			if (stack1Adopt && stack[0].CanAddOrder(side, reqId, px))
			{
				return true;
			}
			else if (stack1Adopt && mstackAdoptAllowed && mstack[0].CanAddOrder(side, reqId, px))
			{
				return true;
			}
			else if (settings.quoteInstrumentSettings.secondStackEnabled && stack2Adopt)
			{
				if (stack[1].CanAddOrder(side, reqId, px))
				{
					return true;
				}
				else if (mstackAdoptAllowed && mstack[1].CanAddOrder(side, reqId, px))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		default:
			LOGWARN("AklStacker::CanAddOrderToStack: Invalid order side {}", static_cast<int16_t>(side));
			return false;
	}
}

bool Instrument::AddOrderToStack(Order *o, const bool mstackAdoptAllowed, bool stack1Adopt, bool stack2Adopt)
{
	if (likely(!o->submitQueued))
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
			case KTN::ORD::KOrderSide::Enum::SELL:
				if (stack1Adopt && stack[0].AddOrder(o))
				{
					return true;
				}
				else if (stack1Adopt && mstackAdoptAllowed && mstack[0].AddOrder(o))
				{
					return true;
				}
				else if (settings.quoteInstrumentSettings.secondStackEnabled && stack2Adopt)
				{
					if (stack[1].AddOrder(o))
					{
						return true;
					}
					else if (mstackAdoptAllowed && mstack[1].AddOrder(o))
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			default:
				LOGWARN("AklStacker::AddOrderToStack: Invalid order side {}", static_cast<int16_t>(o->side));
				return false;
		}
	}
	else
	{
		return false;
	}
}

void Instrument::ClearPosition()
{
	const Quantity prevPosition = GetPosition();
	pm.ClearPosition();
	bool result = false;
	if (prevPosition > Quantity(0))
	{
		result |= hm.MaintainExistingOrders(KTN::ORD::KOrderSide::Enum::BUY);
	}
	else if (prevPosition < Quantity(0))
	{
		result |= hm.MaintainExistingOrders(KTN::ORD::KOrderSide::Enum::SELL);
	}
	if (result)
	{
		strategy.QueueStackInfoUpdate();
	}
	LOGINFO("Strategy {} cleared position", strategy.StrategyId());
}

void Instrument::UpdateStackQuantities()
{
	if (settings.quoteInstrumentSettings.dynamicStack)
	{
		// Quantity bidQty { 0 };
		// const std::optional<messaging::DynamicProfileMessage> &bidProfile = dynamicPricingModel.GetBidProfile();
		// if (bidProfile)
		// {
		// 	for (int i = 0; i < bidProfile->levelCount; ++i)
		// 	{
		// 		bidQty += Quantity(bidProfile->orderQuantity[i]);
		// 	}
		// }
		// else
		// {
		// 	bidQty = settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty;
		// }

		// Quantity askQty { 0 };
		// const std::optional<messaging::DynamicProfileMessage> &askProfile = dynamicPricingModel.GetAskProfile();
		// if (askProfile)
		// {
		// 	for (int i = 0; i < askProfile->levelCount; ++i)
		// 	{
		// 		askQty += Quantity(askProfile->orderQuantity[i]);
		// 	}
		// }
		// else
		// {
		// 	askQty = settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty;
		// }

		// stack.SetDefaultStackQuantity(
		// 	std::min(bidQty, settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty),
		// 	std::min(askQty, settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty));
	}
	else
	{
		stack[0].SetDefaultStackQuantity(
			settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty,
			settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty);
		stack[1].SetDefaultStackQuantity(
			settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty,
			settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty);
	}

	// stack2.SetDefaultStackQuantity(
	// 	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty,
	// 	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty);
}

void Instrument::AddRecoveredOrders(const std::vector<KTN::OrderPod> &recoveredOrders)
{
	for (const KTN::OrderPod &o : recoveredOrders)
	{
		if (o.OrdState != KTN::ORD::KOrderState::COMPLETE)
		{
			if (o.OrdState != KTN::ORD::KOrderState::WORKING)
			{
				LOGWARN("AklStacker {} | Recovered order {} has state {} ({}) rather than WORKING",
					strategy.StrategyId(), o.orderReqId, KTN::ORD::KOrderState::toString(o.OrdState), static_cast<int>(o.OrdState));
			}

			Order *order = CreateOrder(o.OrdSide, Quantity(o.quantity), o.price);
			order->orderReqId = o.orderReqId;
			order->cmeId = o.exchordid;
			order->ackedPrice = o.price;
			order->ackedQuantity = order->targetQuantity;
			order->filledQuantity = Quantity(o.fillqty);
			order->type = Order::Type::NONE;
			order->submitQueued = false;
			order->submitted = true;
			order->tif = o.OrdTif;
			OrderUtils::fastCopy(order->orderPod.origordernum, o.ordernum, sizeof(order->orderPod.origordernum));

			std::optional<akl::StrategyOrderInfo> info = akl::StrategyManager::instance().GetOrderInfo(order->cmeId);
			if (info)
			{
				order->stackIndex = info->stackIndex;
				LOGINFO("AklStacker {} | Setting stackIndex for order with cmeId {} to {}",
					strategy.StrategyId(), order->cmeId, static_cast<int16_t>(info->stackIndex));
			}
			else
			{
				order->stackIndex = 0;
				LOGWARN("AklStacker {} | Failed to find order info for cmeId {} | Defaulting stackIndex to 0",
					strategy.StrategyId(), order->cmeId);
			}


			orderMap[order->orderReqId] = order;

			strategyExecution.GetRisk().AddWorking(order);
		}
		else
		{
			LOGWARN("AklStacker {} | Ignoring recovered order {} becase state is COMPLETE",
				strategy.StrategyId(), o.orderReqId);
		}

		// uint64_t orderReqId = o.orderReqId;
		// Order *order = CreateOrder(o.OrdSide, Quantity(o.quantity), o.price);
		// logger.LogFmt("Changing orderReqId from {} to {}", orderReqId, order->orderPod.orderReqId);
		// order->orderReqId = order->orderPod.orderReqId;//o.orderReqId;
		// order->cmeId = o.cmeid;
		// order->ackedPrice = o.price;
		// order->ackedQuantity = order->targetQuantity;
		// order->type = Order::Type::NONE;
		// orderMap[order->orderReqId] = order;

		// strategy.GetOrderManager().onSentOrderMod(order->orderPod);
		// Modify(order, o.price);
	}
}

Order *Instrument::CreateOrder(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px)
{
	Order *o = orderPool.Get();
	if (likely(o != nullptr))
	{
		o->side = side;
		o->targetQuantity = qty;
		o->price = px;
		o->strategyOrderId = strategyOrderId++;
		strategyExecution.Create(o, side, qty, px);
		// if (likely(qty >= Quantity(0)))
		// {
		// 	KTN::OrderPod *o = orderPool.get();
		// 	if (likely(o != nullptr))
		// 	{
		// 		quoter.NextNew(*o, side, 0, static_cast<uint32_t>(qty.value()), px.AsShifted()); // TODO this takes an int price
		// 		o->price = px;
		// 		o->fillqty = 0; // TODO
		// 		o->OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;
		// 		o->OrdState = KOrderState::INFLIGHT;
		// 		o->callbackid = strategy.ExchangeCallbackId();
		// 		o->OrdExch = KTN::ORD::KOrderExchange::CME;
		// 		o->OrdTif = KTN::ORD::KOrderTif::Enum::DAY; // TODO make configurable
		// 		o->OrdType = KTN::ORD::KOrderType::Enum::LIMIT;
		// 		return o;
		// 	}
		// }

		return o;
	}
	else
	{
		LOGWARN("AklStacker {} | Failed to create order", strategy.StrategyId());
		return nullptr;
	}
}

bool Instrument::CanSubmit(const Order *o, bool canCross, bool canBypassWait, int numOrders) const
{
	bool result = true;
	result &= o->targetQuantity > Quantity(0);
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			result &= canCross || !book.HasAsk() || o->price < book.BestAsk();
			result &= canBypassWait || numRejects[0] < MAX_SUBMIT_REJECTS || time.Now() >= lastRejectTime[0] + REJECT_WAIT;
			result &= strategyExecution.SubmitQueueHasRoom(numOrders);
			break;
		case KTN::ORD::KOrderSide::Enum::SELL:
			result &= canCross || !book.HasBid() || o->price > book.BestBid();
			result &= canBypassWait || numRejects[1] < MAX_SUBMIT_REJECTS || time.Now() >= lastRejectTime[1] + REJECT_WAIT;
			result &= strategyExecution.SubmitQueueHasRoom(numOrders);
			break;
		default:
			result = false;
			break;
	}

	return result;
}

bool Instrument::CanModify(const Order* o, const Price px, bool canCross) const
{
	bool result = true;
	if (!canCross)
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
				result &= !book.HasAsk() || px < book.BestAsk();
				break;
			case KTN::ORD::KOrderSide::Enum::SELL:
				result &= !book.HasBid() || px > book.BestBid();
				break;
			default:
				result = false;
				break;
		}
	}
	return result;
}

void Instrument::Submit(Order *o)
{
	strategyExecution.Submit(o);

	orderMap[o->orderReqId] = o; // TODO move this
}

void Instrument::Modify(Order *o, const Price px)
{
	strategyExecution.Modify(o, px);
}

void Instrument::Modify(Order *o, const Quantity qty)
{
	strategyExecution.Modify(o, qty);
}

void Instrument::Cancel(Order *o)
{
	if (!o->pendingCancel.load(std::memory_order_relaxed))
	{
		// const std::string id = quoter.NextId();
		// OrderUtils::fastCopy(o->ordernum, id.c_str(), sizeof(o->ordernum));
		// o->OrdAction = KTN::ORD::KOrderAction::Enum::ACTION_CXL;
		strategyExecution.Cancel(o);
		o->pendingCancel.store(true, std::memory_order_release);
	}
}

Quantity Instrument::TakeDynamicOrders(const KTN::ORD::KOrderSide::Enum side, const Price px, FixedSizeVector<Order *, 10> &orders)
{
	// TODO
	// return strategyExecution.TakeDynamicOrders(side, px, orders);
	return Quantity(0);
}

Quantity Instrument::TakeMOrders(
	const KTN::ORD::KOrderSide::Enum side,
	const Price px,
	const int stackIndex,
	FixedSizeVector<Order *, 10> &orders,
	bool &ordersRemaining)
{
	switch (side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
		{
			if (stackIndex == 0)
			{
				const Quantity takenQty = mstack[0].GiveUpOrders<KTN::ORD::KOrderSide::Enum::BUY>(px, orders, ordersRemaining);
				return takenQty;
			}
			else if (stackIndex == 1)
			{
				const Quantity takenQty = mstack[1].GiveUpOrders<KTN::ORD::KOrderSide::Enum::BUY>(px, orders, ordersRemaining);
				return takenQty;
			}
			else
			{
				ordersRemaining = false;
				return Quantity(0);
			}
		}
		case KTN::ORD::KOrderSide::Enum::SELL:
		{
			if (stackIndex == 0)
			{
				const Quantity takenQty = mstack[0].GiveUpOrders<KTN::ORD::KOrderSide::Enum::SELL>(px, orders, ordersRemaining);
				return takenQty;
			}
			else if (stackIndex == 1)
			{
				const Quantity takenQty = mstack[1].GiveUpOrders<KTN::ORD::KOrderSide::Enum::SELL>(px, orders, ordersRemaining);
				return takenQty;
			}
			else
			{
				ordersRemaining = false;
				return Quantity(0);
			}
		}
		default:
			ordersRemaining = false;
			return Quantity(0);
	}
}

void Instrument::OnAck(KTN::OrderPod &ord)
{
	OrderMapType::iterator it = orderMap.find(ord.orderReqId);
	if (unlikely(it == orderMap.end()))
	{
		LOGERROR("AklStacker {} | OnAck: Order with orderReqId {} not found in orderMap", strategy.StrategyId(), ord.orderReqId);
		return;
	}
	else if (unlikely(it->second == nullptr))
	{
		LOGERROR("AklStacker {} | OnAck: Order with orderReqId {} found in orderMap but is nullptr", strategy.StrategyId(), ord.orderReqId);
		return;
	}

	Order *o = it->second;
	o->ackedQuantity = Quantity(ord.quantity);
	o->ackedPrice = Price(ord.price);
	o->cmeId = ord.exchordid;
	o->priorityTransactTime = ord.timestamp;
	OrderUtils::fastCopy(o->orderPod.origordernum, ord.ordernum, sizeof(ord.ordernum));

	if (o->pendingCancel.load(std::memory_order_acquire) || o->pendingModify.load(std::memory_order_acquire))
	{
		strategyExecution.OnAck(o);
	}

	strategy.pendingOrderUpdates.push(akl::OrderUpdate { o->cmeId, o->stackIndex, false });
}

void Instrument::OnFill(KTN::OrderPod &ko)
{
	OrderMapType::iterator it = orderMap.find(ko.orderReqId);
	if (unlikely(it == orderMap.end()))
	{
		LOGERROR("AklStacker {} | OnFill: Order with orderReqId {} not found in orderMap", strategy.StrategyId(), ko.orderReqId);
		return;
	}
	else if (unlikely(it->second == nullptr))
	{
		LOGERROR("AklStacker {} | OnFill: Order with orderReqId {} found in orderMap but is nullptr", strategy.StrategyId(), ko.orderReqId);
		return;
	}

	Order *o = it->second;

	strategyExecution.OnFill(o, ko);

	//o->filledQuantity = Quantity(ko.fillqty); // TODO check this
	o->filledQuantity += Quantity(ko.lastqty); // CFE does not give cumulative fillqty in fills

	const Quantity fillQty { static_cast<int>(ko.lastqty) };
	const Price fillPx { ko.lastpx };

	if (unlikely(!strategy.IsRunning()))
	{

	}
	else if (unlikely(o->type == Order::Type::NONE || o->type == Order::Type::STOP))
	{
		
	}
	else
	{
		// TODO pro-rata LTP calculations
		
		Quantity bookQty { 0 };
		Duration window { 0 };
		bool positionMagnitudeIncreased = false;

		switch (o->side)
		{
		case KTN::ORD::KOrderSide::Enum::BUY:
		{
			pm.Add(fillQty, fillPx);
			positionMagnitudeIncreased = pm.Position() > Quantity(0);
			if (book.HasUnadjustedBid() && fillPx == book.UnadjustedBestBid())
			{
				bookQty = book.UnadjustedBestBidQuantity();
			}
			const std::optional<stacker::ProrataTarget> &bidTarget = ProrataTarget<KTN::ORD::KOrderSide::Enum::BUY>();
			if (bidTarget)
			{
				window = bidTarget->window;
			}
			const std::pair<Price, Timestamp> lastStop = sh.LastTriggeredStopAndTime<KTN::ORD::KOrderSide::Enum::SELL>();
			o->scratchFillsImmediately |= o->pendingCancel
				&& lastStop.second != Time::Min()
				&& o->cancelTime <= lastStop.second
				&& o->price >= lastStop.first
				&& (o->type == Order::Type::PRORATA || o->type == Order::Type::STACK);

			break;
		}
		case KTN::ORD::KOrderSide::Enum::SELL:
		{
			pm.Add(-fillQty, fillPx);
			positionMagnitudeIncreased = pm.Position() < Quantity(0);

			if (book.HasUnadjustedAsk() && fillPx == book.UnadjustedBestAsk())
			{
				bookQty = book.UnadjustedBestAskQuantity();
			}
			const std::optional<stacker::ProrataTarget> &askTarget = ProrataTarget<KTN::ORD::KOrderSide::Enum::SELL>();
			if (askTarget)
			{
				window = askTarget->window;
			}
			const std::pair<Price, Timestamp> lastStop = sh.LastTriggeredStopAndTime<KTN::ORD::KOrderSide::Enum::BUY>();
			o->scratchFillsImmediately |= o->pendingCancel
				&& lastStop.second != Time::Min()
				&& o->cancelTime <= lastStop.second
				&& o->price <= lastStop.first
				&& (o->type == Order::Type::PRORATA || o->type == Order::Type::STACK);
			
			break;
		}
		default:
			LOGERROR("AklStacker {} | OnFill: Unexpected order side {}", strategy.StrategyId(), static_cast<int16_t>(o->side));
			break;
		}

		pc.OnFill(o, fillPx);

		if (!IsWorking(o))
		{
			removeOrder(o, false, true);

			if (o->type == Order::Type::STACK)
			{
				switch (o->side)
				{
					case KTN::ORD::KOrderSide::Enum::BUY:
					{
						if (o->stackIndex == 0)
						{
							if (settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing && !stack[0].HasOrders<KTN::ORD::KOrderSide::Enum::BUY>(fillPx))
							{
								const std::optional<Price> bestPriceWithOrders = stack[0].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::BUY>();
								if (!bestPriceWithOrders || bestPriceWithOrders < fillPx)
								{
									pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(fillPx - TickIncrement);
									//strategy.QueueStackTargetsUpdate();
								}
							}
						}
						else if (o->stackIndex == 1)
						{
							if (settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing && !stack[1].HasOrders<KTN::ORD::KOrderSide::Enum::BUY>(fillPx))
							{
								const std::optional<Price> bestPriceWithOrders = stack[1].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::BUY>();
								if (!bestPriceWithOrders || bestPriceWithOrders < fillPx)
								{
									pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(fillPx - TickIncrement);
									//strategy.QueueStackTargetsUpdate();
								}
							}
						}

						break;
					}
					case KTN::ORD::KOrderSide::Enum::SELL:
					{
						if (o->stackIndex == 0)
						{
							if (settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing && !stack[0].HasOrders<KTN::ORD::KOrderSide::Enum::SELL>(fillPx))
							{
								const std::optional<Price> bestPriceWithOrders = stack[0].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::SELL>();
								if (!bestPriceWithOrders || bestPriceWithOrders > fillPx)
								{
									pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(fillPx + TickIncrement);
									//strategy.QueueStackTargetsUpdate();
								}
							}
						}
						else if (o->stackIndex == 1)
						{
							if (settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing && !stack[1].HasOrders<KTN::ORD::KOrderSide::Enum::SELL>(fillPx))
							{
								const std::optional<Price> bestPriceWithOrders = stack[1].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::SELL>();
								if (!bestPriceWithOrders || bestPriceWithOrders > fillPx)
								{
									pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(fillPx + TickIncrement);
									//strategy.QueueStackTargetsUpdate();
								}
							}
						}
						break;
					}
					default:
						break;
				}
			}
		}

		if (settings.quoteInstrumentSettings.hedgeOnLargeFill)
		{
			o->tradeTracker.ClearOldTrades(time.Now(), window);
			if (o->type == Order::Type::PRORATA && bookQty > Quantity(0) && settings.quoteInstrumentSettings.prorataAllocation > 0.0)
			{
				const Quantity prevWorkingQty = Quantity(ko.leavesqty) + fillQty;
				if (prevWorkingQty > Quantity(0))
				{
					const double percentFilled = static_cast<double>(fillQty) / static_cast<double>(prevWorkingQty);
					const double percentFilledOfBook = std::min(percentFilled / settings.quoteInstrumentSettings.prorataAllocation, 1.0);
					const Quantity estimatedTrade = Quantity(percentFilledOfBook * static_cast<double>(bookQty));
					if (estimatedTrade > Quantity(0))
					{
						o->tradeTracker.AddTrade(time.Now(), estimatedTrade, fillPx);
					}
				}
			}
		}

		const Quantity totalTradeQty = o->tradeTracker.GetTotalTradeQuantity();

		Recalculate(
			false,
			FillInfo { fillPx, fillQty, o->side, o->scratchFillsImmediately && positionMagnitudeIncreased, totalTradeQty },
			std::nullopt);

		// TODO if hedge on large fill

		if (o->type == Order::Type::STACK)
		{
			strategy.QueueStackInfoUpdate();
		}
	}

	if (!IsWorking(o))
	{
		doneOrderMap[o->orderReqId] = o;
		orderMap.erase(o->orderReqId);
	}
}

void Instrument::OnModAck(KTN::OrderPod &ord)
{
	OrderMapType::iterator it = orderMap.find(ord.orderReqId);
	if (unlikely(it == orderMap.end()))
	{
		LOGERROR("AklStacker {} | OnModAck: Order with orderReqId {} not found in orderMap", strategy.StrategyId(), ord.orderReqId);
		return;
	}
	else if (unlikely(it->second == nullptr))
	{
		LOGERROR("AklStacker {} | OnModAck: Order with orderReqId {} found in orderMap but is nullptr", strategy.StrategyId(), ord.orderReqId);
		return;
	}

	Order *o = it->second;
	if (Quantity(ord.quantity) > o->ackedQuantity)
	{
		o->priorityTransactTime = ord.timestamp;
	}
	o->ackedQuantity = Quantity(ord.quantity);
	o->ackedPrice = ord.price;
	strategyExecution.OnModAck(o, ord);

	if (o->type == Order::Type::PRORATA) // TODO do we need to recalculate for other types?
	{
		Recalculate(false);
	}
	else if (o->type == Order::Type::STACK)
	{
		strategy.QueueStackInfoUpdate();
	}
}

void Instrument::OnCancelAck(KTN::OrderPod &ko)
{
	OrderMapType::iterator it = orderMap.find(ko.orderReqId);
	if (unlikely(it == orderMap.end()))
	{
		LOGERROR("AklStacker {} | OnCancelAck: Order with orderReqId {} not found in orderMap", strategy.StrategyId(), ko.orderReqId);
		return;
	}
	else if (unlikely(it->second == nullptr))
	{
		LOGERROR("AklStacker {} | OnCancelAck: Order with orderReqId {} found in orderMap but is nullptr", strategy.StrategyId(), ko.orderReqId);
		return;
	}

	Order *o = it->second;
	strategyExecution.OnCancelAck(o, ko);

	removeOrder(o, true, false); // TODO need to check if strategy initiated cancel for wasCancel to be true?

	if (o->type == Order::Type::STACK)
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
			{
				if (o->stackIndex == 0)
				{
					if (settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing)
					{
						const std::optional<Price> bestPriceWithOrders = stack[0].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::BUY>();
						if (bestPriceWithOrders)
						{
							if (SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::IsBetter(o->price, *bestPriceWithOrders))
							{
								pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(*bestPriceWithOrders);
								//strategy.QueueStackTargetsUpdate();
							}
						}
						else
						{
							pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::PriceOut(o->price, TickIncrement));
							//strategy.QueueStackTargetsUpdate();
						}
					}
				}
				else if (o->stackIndex == 1)
				{
					if (settings.quoteInstrumentSettings.secondStackEnabled && settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing)
					{
						const std::optional<Price> bestPriceWithOrders = stack[1].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::BUY>();
						if (bestPriceWithOrders)
						{
							if (SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::IsBetter(o->price, *bestPriceWithOrders))
							{
								pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(*bestPriceWithOrders);
								//strategy.QueueStackTargetsUpdate();
							}
						}
						else
						{
							pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::PriceOut(o->price, TickIncrement));
							//strategy.QueueStackTargetsUpdate();
						}
					}
				}

				break;
			}
			case KTN::ORD::KOrderSide::Enum::SELL:
			{
				if (o->stackIndex == 0)
				{
					if (settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing)
					{
						const std::optional<Price> bestPriceWithOrders = stack[0].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::SELL>();
						if (bestPriceWithOrders)
						{
							if (SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::IsBetter(o->price, *bestPriceWithOrders))
							{
								pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(*bestPriceWithOrders);
								//strategy.QueueStackTargetsUpdate();
							}
						}
						else
						{
							pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::PriceOut(o->price, TickIncrement));
							//strategy.QueueStackTargetsUpdate();
						}
					}
				}
				else if (o->stackIndex == 1)
				{
					if (settings.quoteInstrumentSettings.secondStackEnabled && settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing)
					{
						const std::optional<Price> bestPriceWithOrders = stack[1].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::SELL>();
						if (bestPriceWithOrders)
						{
							if (SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::IsBetter(o->price, *bestPriceWithOrders))
							{
								pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(*bestPriceWithOrders);
								//strategy.QueueStackTargetsUpdate();
							}
						}
						else
						{
							pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::PriceOut(o->price, TickIncrement));
							//strategy.QueueStackTargetsUpdate();
						}
					}
				}
				break;
			}
			default:
				break;
		}
	}

	if (o->type == Order::Type::HEDGE)
	{
		Recalculate(false, std::nullopt, CanceledHedgeInfo { o->price, o->side });
	}
	else
	{
		Recalculate(false);
	}

	doneOrderMap[o->orderReqId] = o;
	orderMap.erase(o->orderReqId);
}

void Instrument::OnSubmitReject(KTN::OrderPod &ord)
{
	OrderMapType::iterator it = orderMap.find(ord.orderReqId);
	if (unlikely(it == orderMap.end()))
	{
		LOGERROR("AklStacker {} | OnSubmitReject: Order with orderReqId {} not found in orderMap", strategy.StrategyId(), ord.orderReqId);
		return;
	}
	else if (unlikely(it->second == nullptr))
	{
		LOGERROR("AklStacker {} | OnSubmitReject: Order with orderReqId {} found in orderMap but is nullptr", strategy.StrategyId(), ord.orderReqId);
		return;
	}
	Order *o = it->second;

	strategyExecution.OnSubmitReject(o, ord);

	removeOrder(o, false, false); // TODO need to check if strategy initiated cancel for wasCancel to be true?

	if (o->type == Order::Type::STACK)
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
			{
				if (o->stackIndex == 0)
				{
					if (settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing)
					{
						const std::optional<Price> bestPriceWithOrders = stack[0].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::BUY>();
						if (bestPriceWithOrders)
						{
							if (SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::IsBetter(o->price, *bestPriceWithOrders))
							{
								pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(*bestPriceWithOrders);
								//strategy.QueueStackTargetsUpdate();
							}
						}
						else
						{
							pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::PriceOut(o->price, TickIncrement));
							//strategy.QueueStackTargetsUpdate();
						}
					}
				}
				else if (o->stackIndex == 1)
				{
					if (settings.quoteInstrumentSettings.secondStackEnabled && settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing)
					{
						const std::optional<Price> bestPriceWithOrders = stack[1].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::BUY>();
						if (bestPriceWithOrders)
						{
							if (SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::IsBetter(o->price, *bestPriceWithOrders))
							{
								pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(*bestPriceWithOrders);
								//strategy.QueueStackTargetsUpdate();
							}
						}
						else
						{
							pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::PriceOut(o->price, TickIncrement));
							//strategy.QueueStackTargetsUpdate();
						}
					}
				}

				break;
			}
			case KTN::ORD::KOrderSide::Enum::SELL:
			{
				if (o->stackIndex == 0)
				{
					if (settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing)
					{
						const std::optional<Price> bestPriceWithOrders = stack[0].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::SELL>();
						if (bestPriceWithOrders)
						{
							if (SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::IsBetter(o->price, *bestPriceWithOrders))
							{
								pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(*bestPriceWithOrders);
								//strategy.QueueStackTargetsUpdate();
							}
						}
						else
						{
							pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::PriceOut(o->price, TickIncrement));
							//strategy.QueueStackTargetsUpdate();
						}
					}
				}
				else if (o->stackIndex == 1)
				{
					if (settings.quoteInstrumentSettings.secondStackEnabled && settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing)
					{
						const std::optional<Price> bestPriceWithOrders = stack[1].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::SELL>();
						if (bestPriceWithOrders)
						{
							if (SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::IsBetter(o->price, *bestPriceWithOrders))
							{
								pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(*bestPriceWithOrders);
								//strategy.QueueStackTargetsUpdate();
							}
						}
						else
						{
							pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::PriceOut(o->price, TickIncrement));
							//strategy.QueueStackTargetsUpdate();
						}
					}
				}
				break;
			}
			default:
				break;
		}
	}

	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
		{
			++numRejects[0];
			lastRejectTime[0] = time.Now();
			if (numRejects[0] >= MAX_SUBMIT_REJECTS)
			{
				std::ostringstream ss;
				ss << "Strategy " << strategy.StrategyId() << " received " << MAX_SUBMIT_REJECTS
					<< " bid rejects; stopping new bid orders for " << REJECT_WAIT_INT << " seconds";
				const std::string str = ss.str();
				//SendAudit(str.c_str());
				LOGINFO("{}", str);
			}
			break;
		}
		case KTN::ORD::KOrderSide::Enum::SELL:
			++numRejects[1];
			lastRejectTime[1] = time.Now();
			if (numRejects[1] >= MAX_SUBMIT_REJECTS)
			{
				std::ostringstream ss;
				ss << "Strategy " << strategy.StrategyId() << " received " << MAX_SUBMIT_REJECTS
					<< " ask rejects; stopping new ask orders for " << REJECT_WAIT_INT << " seconds";
				const std::string str = ss.str();
				//SendAudit(str.c_str());
				LOGINFO("{}", str);
			}
			break;
		default:
			break;
	}

	Recalculate(false);

	doneOrderMap[o->orderReqId] = o;
	orderMap.erase(o->orderReqId);
}

void Instrument::OnCancelReject(KTN::OrderPod &ord)
{
	OrderMapType::iterator it = orderMap.find(ord.orderReqId);
	if (likely(it != orderMap.end()))
	{
		Order *o = it->second;
		if (o->pendingCancel.load(std::memory_order_acquire))
		{
			o->pendingCancel.store(false, std::memory_order_release);
		}
		else
		{
			LOGWARN("Received cancel reject for order {} with no pending cancel", ord.orderReqId);
		}
	}
	else
	{
		LOGWARN("Order {} not found in order map", ord.orderReqId);
	}
}

void Instrument::OnCancelReplaceReject(KTN::OrderPod &ord)
{
	OrderMapType::iterator it = orderMap.find(ord.orderReqId);
	if (likely(it != orderMap.end()))
	{
		Order *o = it->second;
		strategyExecution.OnCancelReplaceReject(o, ord);
		if (o->numPendingMods == 0)
		{
			o->targetQuantity = o->ackedQuantity;
			o->price = o->ackedPrice;
		}
	}
	else
	{
		LOGWARN("Order {} not found in order map", ord.orderReqId);
	}
}

void Instrument::OnInternalCancel(Order *o)
{
	removeOrder(o);
	OrderMapType::iterator it = orderMap.find(o->orderReqId);
	if (likely(it != orderMap.end()))
	{
		doneOrderMap[o->orderReqId] = o;
		orderMap.erase(it);
	}
	else
	{
		LOGWARN("Order {} not found in order map", o->orderReqId);
	}
}

void Instrument::OnInternalReject(Order *o)
{
	removeOrder(o);
	OrderMapType::iterator it = orderMap.find(o->orderReqId);
	if (likely(it != orderMap.end()))
	{
		doneOrderMap[o->orderReqId] = o;
		orderMap.erase(it);
	}
	else
	{
		LOGWARN("Order {} not found in order map", o->orderReqId);
	}
}

void Instrument::Start()
{
	Recalculate(false);
	if (!orderMap.empty())
	{
		int bidCount = 0;
		int askCount = 0;
		int bidExtra = 0;
		int askExtra = 0;

		std::optional<Price> bestBidOrderPx[2] { std::nullopt, std::nullopt };
		std::optional<Price> bestAskOrderPx[2] { std::nullopt, std::nullopt };
		if (!settings.quoteInstrumentSettings.secondStackEnabled)
		{
			for (OrderMapType::iterator it = orderMap.begin(); it != orderMap.end(); ++it)
			{
				Order *o = it->second;
				switch (o->side)
				{
				case KTN::ORD::KOrderSide::Enum::BUY:
					bestBidOrderPx[0] = bestBidOrderPx[0] ? std::max(*bestBidOrderPx[0], o->price) : o->price;
					break;
				case KTN::ORD::KOrderSide::Enum::SELL:
					bestAskOrderPx[0] = bestAskOrderPx[0] ? std::min(*bestAskOrderPx[0], o->price) : o->price;
					break;
				default:
					// Log?
					// Do nothing else
					break;
				}
			}
		}
		else
		{
			for (OrderMapType::iterator it = orderMap.begin(); it != orderMap.end(); ++it)
			{
				Order *o = it->second;
				int stackIndex;
				if (o->stackIndex == 0)
				{
					stackIndex = 0;
				}
				else if (o->stackIndex == 1)
				{
					stackIndex = 1;
				}
				else
				{
					LOGWARN("Order {} has invalid stack index {}", o->orderReqId, static_cast<int16_t>(o->stackIndex));
					stackIndex = 0;
					o->stackIndex = 0;
				}

				switch (o->side)
				{
				case KTN::ORD::KOrderSide::Enum::BUY:
					bestBidOrderPx[stackIndex] = bestBidOrderPx[stackIndex] ? std::max(*bestBidOrderPx[stackIndex], o->price) : o->price;
					break;
				case KTN::ORD::KOrderSide::Enum::SELL:
					bestAskOrderPx[stackIndex] = bestAskOrderPx[stackIndex] ? std::min(*bestAskOrderPx[stackIndex], o->price) : o->price;
					break;
				default:
					// Log?
					// Do nothing else
					break;
				}
			}
		}

		const bool useMarketPrices = book.IsOpen() || (book.IsPreopen() && settings.quoteInstrumentSettings.quoteInPreopen);
		std::optional<Price> bidTarget = useMarketPrices && pricingModel[0].BidLean() ? pricingModel[0].BidLean() : bestBidOrderPx[0];
		std::optional<Price> askTarget = useMarketPrices && pricingModel[0].AskLean() ? pricingModel[0].AskLean() : bestAskOrderPx[0];
		std::optional<Price> bidTarget2 = useMarketPrices && pricingModel[1].BidLean() ? pricingModel[1].BidLean() : bestBidOrderPx[1];
		std::optional<Price> askTarget2 = useMarketPrices && pricingModel[1].AskLean() ? pricingModel[1].AskLean() : bestAskOrderPx[1];

		if (settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing)
		{
			if (bidTarget && bestBidOrderPx[0])
			{
				if (*bidTarget > *bestBidOrderPx[0])
				{
					bidTarget = bestBidOrderPx[0];
				}
			}
		}
		if (settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing)
		{
			if (bidTarget2 && bestBidOrderPx[1])
			{
				if (*bidTarget2 > *bestBidOrderPx[1])
				{
					bidTarget2 = bestBidOrderPx[1];
				}
			}
		}
		if (settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing)
		{
			if (askTarget && bestAskOrderPx[0])
			{
				if (*askTarget < *bestAskOrderPx[0])
				{
					askTarget = bestAskOrderPx[0];
				}
			}
		}
		if (settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing)
		{
			if (askTarget2 && bestAskOrderPx[1])
			{
				if (*askTarget2 < *bestAskOrderPx[1])
				{
					askTarget2 = bestAskOrderPx[1];
				}
			}
		}

		stack[0].Update(bidTarget, askTarget, false);
		stack[1].Update(bidTarget2, askTarget2, false);
		stack[0].SendQueuedSubmits();
		stack[1].SendQueuedSubmits();

		std::list<Order *> ordersToCancel;

		for (OrderMapType::iterator it = orderMap.begin(); it != orderMap.end(); ++it)
		{
			Order *o = it->second;
			const int stackIndex = settings.quoteInstrumentSettings.secondStackEnabled ? o->stackIndex : 0;
			const bool added = AddOrderToStack(o, false, stackIndex == 0, stackIndex == 1);
			if (added)
			{
				switch (o->side)
				{
				case KTN::ORD::KOrderSide::Enum::BUY:
					++bidCount;
					break;
				case KTN::ORD::KOrderSide::Enum::SELL:
					++askCount;
					break;
				default:
					// Log?
					// Do nothing else
					break;
				}
				//o->strategyOrderId = OrderId();
			}
			else
			{
				switch (o->side)
				{
				case KTN::ORD::KOrderSide::Enum::BUY:
					++bidExtra;
					break;
				case KTN::ORD::KOrderSide::Enum::SELL:
					++askExtra;
					break;
				default:
					// Log?
					// Do nothing else
					break;
				}
				ordersToCancel.push_back(o);
			}
		}

		std::ostringstream ss;
		if (bidExtra == 0 && askExtra == 0)
		{
			ss << "Strategy " << strategy.StrategyId() << " for " << Symbol << " adopted " << bidCount << " bids and " << askCount << " asks";
		}
		else
		{
			ss << "Strategy " << strategy.StrategyId() << " for " << Symbol << " adopted " << bidCount << " bids and " << askCount << " asks, and wasn't able to adopt " << bidExtra << " bids and " << askExtra << " asks";
		}

		if (settings.quoteInstrumentSettings.cancelUnadoptedOrders)
		{
			if (book.IsOpen() || book.IsPreopen())
			{
				for (Order *o : ordersToCancel)
				{
					Cancel(o);
				}
			}
			else
			{
				if (!ordersToCancel.empty())
				{
					LOGWARN("Strategy {} not canceling {} extra orders due to market state {}",
						strategy.StrategyId(),
						ordersToCancel.size(),
						static_cast<int>(book.MarketState()));
				}
			}
		}

		const std::string str = ss.str();
		notifier.Notify(str, KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
		LOGINFO("{}", str);
	}
	else
	{
		std::ostringstream ss;
		ss << "Strategy " << strategy.StrategyId() << " for " << Symbol << " found no orders to adopt";
		const std::string str = ss.str();
		notifier.Notify(str, KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
		LOGINFO("{}", str);
	}
}

void Instrument::Stop()
{
	strategyExecution.OnStrategyPause();
	strategyExecution.ProcessCallbacks();
	// TODO dynamic stack
	mstack[0].ReleaseAllOrders();
	mstack[1].ReleaseAllOrders();
	stack[0].ReleaseAllOrders();
	stack[1].ReleaseAllOrders();
	prorataQuoter.ReleaseAllOrders();
	hm.ReleaseAllOrders();
	pc.Clear();
}

void Instrument::RepricePositions()
{
	const Quantity position = pm.Position();
	if (position > Quantity(0))
	{
		if (unlikely(!book.IsOpen()))
		{
			LOGWARN("Strategy {} can't reprice long positions due to market not open", strategy.StrategyId());
		}
		else if (unlikely(!book.HasBid()))
		{
			LOGWARN("Strategy {} can't reprice long positions due to no bid price", strategy.StrategyId());
		}
		else
		{
			pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(book.BestBid());
			LOGINFO("Strategy {} repricing long positions to {}", strategy.StrategyId(), book.BestBid().AsShifted());
		}
	}
	else if (position < Quantity(0))
	{
		if (unlikely(!book.IsOpen()))
		{
			LOGWARN("Strategy {} can't reprice short positions due to market not open", strategy.StrategyId());
		}
		else if (unlikely(!book.HasAsk()))
		{
			LOGWARN("Strategy {} can't reprice short positions due to no ask price", strategy.StrategyId());
		}
		else
		{
			pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(book.BestAsk());
			LOGINFO("Strategy {} repricing short positions to {}", strategy.StrategyId(), book.BestAsk().AsShifted());
		}
	}
}

void Instrument::AddPosition(const Quantity pos)
{
	if (pos > Quantity(0))
	{
		const Price px = book.HasBid() ? book.BestBid() : Price::Max();
		pm.Add(Quantity(pos), px);
		LOGINFO("Strategy {} added position {}@{}", strategy.StrategyId(), pos.value(), px.AsShifted());
	}
	else if (pos < Quantity(0))
	{
		const Price px = book.HasAsk() ? book.BestAsk() : Price::Max();
		pm.Add(Quantity(pos), px);
		LOGINFO("Strategy {} added position {}@{}", strategy.StrategyId(), pos.value(), px.AsShifted());
	}
}

void Instrument::Handle(const StackerPriceAbandonMessage &msg)
{
	int numAbandoned = 0;

	for (size_t i = 0; i < msg.levels.size(); ++i)
	{
		const Price px = msg.levels[i].price;
		switch (msg.levels[i].side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
			{
				if (msg.levels[i].index == 0)
				{
					numAbandoned += stack[0].AbandonPrice<KTN::ORD::KOrderSide::Enum::BUY>(px);
					numAbandoned += mstack[0].AbandonPrice<KTN::ORD::KOrderSide::Enum::BUY>(px);
					//numAbandoned += dynamicStack.AbandonPrice<Side::BID>(px);
				}
				else if (msg.levels[i].index == 1)
				{
					numAbandoned += stack[1].AbandonPrice<KTN::ORD::KOrderSide::Enum::BUY>(px);
					numAbandoned += mstack[1].AbandonPrice<KTN::ORD::KOrderSide::Enum::BUY>(px);
				}
				else
				{
					LOGWARN("Unknown stack index {} at index {} in StackerPriceAbandonMessage", static_cast<int16_t>(msg.levels[i].index), i);
				}
				break;
			}
			case KTN::ORD::KOrderSide::Enum::SELL:
			{
				if (msg.levels[i].index == 0)
				{
					numAbandoned += stack[0].AbandonPrice<KTN::ORD::KOrderSide::Enum::SELL>(px);
					numAbandoned += mstack[0].AbandonPrice<KTN::ORD::KOrderSide::Enum::SELL>(px);
					//numAbandoned += dynamicStack.AbandonPrice<Side::ASK>(px);
				}
				else if (msg.levels[i].index == 1)
				{
					numAbandoned += stack[1].AbandonPrice<KTN::ORD::KOrderSide::Enum::SELL>(px);
					numAbandoned += mstack[1].AbandonPrice<KTN::ORD::KOrderSide::Enum::SELL>(px);
				}
				else
				{
					LOGWARN("Unknown stack index {} at index {} in StackerPriceAbandonMessage", static_cast<int16_t>(msg.levels[i].index), i);
				}
				break;
			}
			default:
			{
				LOGWARN("Unknown side {} in StackerPriceAbandonMessage", static_cast<uint16_t>(msg.levels[i].side));
				break;
			}
		}
	}

	if (numAbandoned > 0)
	{
		strategy.QueueStackInfoUpdate();
	}

	if (settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing)
	{
		const std::optional<Price> bestPriceWithOrders = stack[0].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::BUY>();
		if (bestPriceWithOrders)
		{
			for (size_t i = 0; i < msg.levels.size(); ++i)
			{
				if (msg.levels[i].side == KTN::ORD::KOrderSide::Enum::BUY && msg.levels[i].index == 0)
				{
					const Price px = msg.levels[i].price;
					if (SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::IsBetter(px, *bestPriceWithOrders))
					{
						pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(*bestPriceWithOrders);
						strategy.QueueStackTargetsUpdate();
						break;
					}
				}
			}
		}
		else
		{
			std::optional<Price> worstPx { std::nullopt };
			for (size_t i = 0; i < msg.levels.size(); ++i)
			{
				if (msg.levels[i].side == KTN::ORD::KOrderSide::Enum::BUY && msg.levels[i].index == 0)
				{
					const Price px = msg.levels[i].price;
					if (!worstPx)
					{
						worstPx = px;
					}
					else
					{
						worstPx = SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::PickWorst(*worstPx, px);
					}
				}
			}
			if (worstPx)
			{
				pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::PriceOut(*worstPx, TickIncrement));
				strategy.QueueStackTargetsUpdate();
			}
		}
	}
	if (settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing)
	{
		const std::optional<Price> bestPriceWithOrders = stack[0].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::SELL>();
		if (bestPriceWithOrders)
		{
			for (size_t i = 0; i < msg.levels.size(); ++i)
			{
				if (msg.levels[i].side == KTN::ORD::KOrderSide::Enum::SELL && msg.levels[i].index == 0)
				{
					const Price px = msg.levels[i].price;
					if (SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::IsBetter(px, *bestPriceWithOrders))
					{
						pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(*bestPriceWithOrders);
						strategy.QueueStackTargetsUpdate();
						break;
					}
				}
			}
		}
		else
		{
			std::optional<Price> worstPx { std::nullopt };
			for (size_t i = 0; i < msg.levels.size(); ++i)
			{
				if (msg.levels[i].side == KTN::ORD::KOrderSide::Enum::SELL && msg.levels[i].index == 0)
				{
					const Price px = msg.levels[i].price;
					if (!worstPx)
					{
						worstPx = px;
					}
					else
					{
						worstPx = SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::PickWorst(*worstPx, px);
					}
				}
			}
			if (worstPx)
			{
				pricingModel[0].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::PriceOut(*worstPx, TickIncrement));
				strategy.QueueStackTargetsUpdate();
			}
		}
	}
	if (settings.quoteInstrumentSettings.secondStackEnabled)
	{
		if (settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing)
		{
			const std::optional<Price> bestPriceWithOrders = stack[1].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::BUY>();
			if (bestPriceWithOrders)
			{
				for (size_t i = 0; i < msg.levels.size(); ++i)
				{
					if (msg.levels[i].side == KTN::ORD::KOrderSide::Enum::BUY && msg.levels[i].index == 1)
					{
						const Price px = msg.levels[i].price;
						if (SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::IsBetter(px, *bestPriceWithOrders))
						{
							pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(*bestPriceWithOrders);
							strategy.QueueStackTargetsUpdate();
							break;
						}
					}
				}
			}
			else
			{
				std::optional<Price> worstPx { std::nullopt };
				for (size_t i = 0; i < msg.levels.size(); ++i)
				{
					if (msg.levels[i].side == KTN::ORD::KOrderSide::Enum::BUY && msg.levels[i].index == 1)
					{
						const Price px = msg.levels[i].price;
						if (!worstPx)
						{
							worstPx = px;
						}
						else
						{
							worstPx = SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::PickWorst(*worstPx, px);
						}
					}
				}
				if (worstPx)
				{
					pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::BUY>(SideTraits<KTN::ORD::KOrderSide::Enum::BUY>::PriceOut(*worstPx, TickIncrement));
					strategy.QueueStackTargetsUpdate();
				}
			}
		}
		if (settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing)
		{
			const std::optional<Price> bestPriceWithOrders = stack[1].BestPriceWithOrders<KTN::ORD::KOrderSide::Enum::SELL>();
			if (bestPriceWithOrders)
			{
				for (size_t i = 0; i < msg.levels.size(); ++i)
				{
					if (msg.levels[i].side == KTN::ORD::KOrderSide::Enum::SELL && msg.levels[i].index == 1)
					{
						const Price px = msg.levels[i].price;
						if (SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::IsBetter(px, *bestPriceWithOrders))
						{
							pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(*bestPriceWithOrders);
							strategy.QueueStackTargetsUpdate();
							break;
						}
					}
				}
			}
			else
			{
				std::optional<Price> worstPx { std::nullopt };
				for (size_t i = 0; i < msg.levels.size(); ++i)
				{
					if (msg.levels[i].side == KTN::ORD::KOrderSide::Enum::SELL && msg.levels[i].index == 1)
					{
						const Price px = msg.levels[i].price;
						if (!worstPx)
						{
							worstPx = px;
						}
						else
						{
							worstPx = SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::PickWorst(*worstPx, px);
						}
					}
				}
				if (worstPx)
				{
					pricingModel[1].TickBack<KTN::ORD::KOrderSide::Enum::SELL>(SideTraits<KTN::ORD::KOrderSide::Enum::SELL>::PriceOut(*worstPx, TickIncrement));
					strategy.QueueStackTargetsUpdate();
				}
			}
		}
	}

	LOGINFO("Strategy {} abandoned {} orders", strategy.StrategyId(), numAbandoned);
}

void Instrument::Handle(const StackerPriceAdoptMessage &msg)
{
	std::vector<AdoptLevelInfo> levels;
	levels.reserve(msg.levels.size());

	for (size_t i = 0; i < msg.levels.size(); ++i)
	{
		const Price px = msg.levels[i].price;
		const KTN::ORD::KOrderSide::Enum side = msg.levels[i].side;
		if (side != KTN::ORD::KOrderSide::Enum::BUY && side != KTN::ORD::KOrderSide::Enum::SELL)
		{
			LOGWARN("Unknown side {} in StackerPriceAdoptMessage at index %d", static_cast<uint16_t>(side), i);
			continue;
		}

		const int index = msg.levels[i].index;
		if (index != 0 && index != 1)
		{
			LOGWARN("Unknown index %hu in StackerPriceAdoptMessage at index %d", static_cast<uint16_t>(index), i);
			continue;
		}

		bool found = false;
		for (size_t i = 0; i < levels.size(); ++i)
		{
			if (levels[i].px == px && levels[i].side == side)
			{
				levels[i].stackAdoptAllowed[index] = true;
				found = true;
				break;
			}
		}

		if (!found)
		{
			AdoptLevelInfo ali;
			ali.px = px;
			ali.side = side;
			ali.stackAdoptAllowed[index] = true;
			levels.push_back(ali);
		}
	}

	if (levels.size() > 0)
	{
		Adopt(false, &levels[0], levels.size());
	}
}

void Instrument::Handle(const StackerPositionUpdateMessage &msg)
{
	bool result = false;
	const Quantity prevPosition = GetPosition();
	AddPosition(Quantity(msg.position));
	const Quantity newPosition = GetPosition();
	if (prevPosition > Quantity(0))
	{
		if (newPosition < prevPosition)
		{
			result |= hm.MaintainExistingOrders(KTN::ORD::KOrderSide::Enum::BUY);
		}
	}
	else if (prevPosition < Quantity(0))
	{
		if (newPosition > prevPosition)
		{
			result |= hm.MaintainExistingOrders(KTN::ORD::KOrderSide::Enum::SELL);
		}
	}
	if (result)
	{
		// TODO
		//QueueStackInfoUpdate();
	}
}

void Instrument::Handle(const StopUpdateMessage &msg)
{
	sh.Handle(0, msg);
}

void Instrument::Handle(const std::vector<StackerProrataOrderAdoptMessage> &pendingAdopts)
{
	int numAdopted = 0;
	for (const StackerProrataOrderAdoptMessage &msg : pendingAdopts)
	{
		if (likely(msg.stackIndex == 0 || msg.stackIndex == 1))
		{
			Order *o = prorataQuoter.GetOrder(msg.side, msg.oid);
			if (o != nullptr)
			{
				if (AddOrderToStack(o, true, msg.stackIndex == 0, msg.stackIndex == 1))
				{
					prorataQuoter.RemoveOrder(o, false);
					++numAdopted;
					if (o->cmeId > 0)
					{
						strategy.pendingOrderUpdates.push(akl::OrderUpdate {o->cmeId, o->stackIndex, true });
					}
				}
				else
				{
					LOGWARN("Can't add prorata order {} to stack", msg.oid);
				}
			}
			else
			{
				LOGWARN("Order {} not found in prorata quoter", msg.oid);
			}
		}
		else
		{
			LOGWARN("Unknown stack index {} in StackerProrataOrderAdoptMessage", static_cast<int16_t>(msg.stackIndex));
		}
	}

	if (numAdopted > 0)
	{
		strategy.QueueStackInfoUpdate();
		strategy.QueueProrataOrderUpdate();
		LOGINFO("AklStacker {} | Adopted {} pro-rata orders", strategy.StrategyId(), numAdopted);
		std::ostringstream ss;
		ss << "Strategy " << strategy.StrategyId() << " for " << Symbol << " adopted " << numAdopted << " pro-rata orders";
		const std::string str = ss.str();
		notifier.Notify(str, KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
	}
	else
	{
		LOGINFO("AklStacker {} | Failed to adopt any pro-rata orders", strategy.StrategyId());
		std::ostringstream ss;
		ss << "Strategy " << strategy.StrategyId() << " for " << Symbol << " failed to adopt any pro-rata orders";
		const std::string str = ss.str();
		notifier.Notify(str, KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
	}
}

void Instrument::Handle(const std::vector<StackerProrataOrderAbandonMessage> &pendingAbandons)
{
	int numAbandoned = 0;
	for (const StackerProrataOrderAbandonMessage &msg : pendingAbandons)
	{
		Order *o = prorataQuoter.GetOrder(msg.side, msg.oid);
		if (o != nullptr)
		{
			prorataQuoter.AbandonOrder(msg.side, o);
			++numAbandoned;
		}
	}

	if (numAbandoned > 0)
	{
		strategy.QueueProrataOrderUpdate();
		LOGINFO("AklStacker {} | Abandoned {} pro-rata orders", strategy.StrategyId(), numAbandoned);
		std::ostringstream ss;
		ss << "Strategy " << strategy.StrategyId() << " for " << Symbol << " abandoned " << numAbandoned << " pro-rata orders";
		const std::string str = ss.str();
		notifier.Notify(str, KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
	}
	else
	{
		LOGINFO("AklStacker {} | Failed to abandon any pro-rata orders", strategy.StrategyId());
		std::ostringstream ss;
		ss << "Strategy " << strategy.StrategyId() << " for " << Symbol << " failed to abandon any pro-rata orders";
		const std::string str = ss.str();
		notifier.Notify(str, KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
	}
}

void Instrument::Handle(const StackerSpreaderUpdateMessage &msg)
{
	const std::optional<Price> askStrike = isPriceValid(msg.askStrike) ? std::optional(Price::FromShifted(msg.askStrike)) : std::nullopt;
	const std::optional<Price> bidStrike = isPriceValid(msg.bidStrike) ? std::optional(Price::FromShifted(msg.bidStrike)) : std::nullopt;

	const bool result = spreaderPricingModel.OverrideStrikePrices(bidStrike, askStrike);
	if (result)
	{
		strategy.QueueSpreaderStrikeUpdate();
	}
}

void Instrument::GetStackInfo(StackInfoMessage &msg, const int index) const
{
	const std::optional<Price> qPx[2] = { stack[index].StackPrice<KTN::ORD::KOrderSide::Enum::BUY>(), stack[index].StackPrice<KTN::ORD::KOrderSide::Enum::SELL>() };
	//const std::optional<Price> dPx[2] = { dynamicStack.BestStackPrice<Side::BID>(), dynamicStack.BestStackPrice<Side::ASK>() };
	const std::optional<Price> mPx[2] = { mstack[index].BestStackPrice<KTN::ORD::KOrderSide::Enum::BUY>(), mstack[index].BestStackPrice<KTN::ORD::KOrderSide::Enum::SELL>() };

	msg.stackAskPrice = Price::Max().AsShifted();
	if (mPx[1] && mPx[1]->AsShifted() < msg.stackAskPrice)
	{
		msg.stackAskPrice = mPx[1]->AsShifted();
	}
	// if (dPx[1] && dPx[1]->AsShifted() < msg.stackAskPrice)
	// {
	// 	msg.stackAskPrice = dPx[1]->AsShifted();
	// }
	if (qPx[1] && qPx[1]->AsShifted() < msg.stackAskPrice)
	{
		msg.stackAskPrice = qPx[1]->AsShifted();
	}
	msg.stackBidPrice = Price::Min().AsShifted();
	if (mPx[0] && mPx[0]->AsShifted() > msg.stackBidPrice)
	{
		msg.stackBidPrice = mPx[0]->AsShifted();
	}
	// if (dPx[0] && dPx[0]->AsShifted() > msg.stackBidPrice)
	// {
	// 	msg.stackBidPrice = dPx[0]->AsShifted();
	// }
	if (qPx[0] && qPx[0]->AsShifted() > msg.stackBidPrice)
	{
		msg.stackBidPrice = qPx[0]->AsShifted();
	}

	int bidIndex = 0;
	int askIndex = 0;

	//dynamicStack.GetStackInfo(msg, bidIndex, askIndex);
	mstack[index].GetStackInfo(msg, bidIndex, askIndex);
	stack[index].GetStackInfo(msg, bidIndex, askIndex);
}

void Instrument::QueueStackInfoUpdate()
{
	strategy.QueueStackInfoUpdate();
}

void Instrument::QueuePositionUpdate()
{
	strategy.QueuePositionUpdate();
}

int Instrument::Adopt(bool repricePositions, const AdoptLevelInfo *levels, const size_t count, const int stackIndex)
{
	if (likely(strategy.IsRunning()))
	{
		if (repricePositions)
		{
			if (likely(strategy.IsReady()))
			{
				RepricePositions();
			}
		}

		std::list<Order *> adoptedOrders;
		std::list<Order *> ordersToCancel;

		for (OrderMapType::iterator it = orderMap.begin(); it != orderMap.end(); ++it)
		{
			Order *o = it->second;
			if (!o->submitted)
			{
				LOGINFO("Not adopting order {} because it is not submitted", o->orderReqId);
			}
			else if (!o->IsWorking())
			{
				LOGINFO("Not adopting order {} because it is not working", o->orderReqId);
			}
			else if (o->type == Order::Type::NONE || o->type == Order::Type::STOP)
			{
				bool matchesPx = true;
				if (levels != nullptr && count > 0)
				{
					matchesPx = false;
					for (size_t i = 0; i < count; ++i)
					{
						const AdoptLevelInfo &ali = levels[i];
						if (o->side == ali.side && o->price == ali.px)
						{
							matchesPx = true;
						}
					}
				}

				if (!matchesPx)
				{
					LOGINFO("Not adopting order {} because price {} is not in list", o->orderReqId, o->price.AsShifted());
				}
				else
				{
					adoptedOrders.push_back(o);
				}
			}
		}

		int adoptedOrderCount = 0;
		if (levels != nullptr)
		{
			for (Order *&o : adoptedOrders)
			{
				const AdoptLevelInfo *infoForPrice = nullptr;
				for (size_t i = 0; i < count; ++i)
				{
					if (levels[i].px == o->price && levels[i].side == o->side)
					{
						infoForPrice = &levels[i];
						break;
					}
				}
				if (infoForPrice != nullptr)
				{
					const int8_t previousStackIndex = o->stackIndex;
					if (AddOrderToStack(o, true, infoForPrice->stackAdoptAllowed[0], infoForPrice->stackAdoptAllowed[1]))
					{
						++adoptedOrderCount;
						if (o->stackIndex != previousStackIndex && o->cmeId > 0)
						{
							strategy.pendingOrderUpdates.push(akl::OrderUpdate { o->cmeId, o->stackIndex, false });
						}
					}
					else
					{
						ordersToCancel.push_back(o);
					}
				}
				else
				{
					//LOGWARN("Failed to find AdoptLevelInfo for order %s", o->order->GetOrderId());
				}
			}
		}
		else
		{
			for (Order *&o : adoptedOrders)
			{
				const int8_t previousStackIndex = o->stackIndex;
				if (AddOrderToStack(o, true, stackIndex != 1, stackIndex == 1))
				{
					++adoptedOrderCount;
					if (o->stackIndex != previousStackIndex && o->cmeId > 0)
					{
						strategy.pendingOrderUpdates.push(akl::OrderUpdate { o->cmeId, o->stackIndex, false });
					}
				}
				else
				{
					ordersToCancel.push_back(o);
				}
			}
		}

		std::list<Order *> adoptedHedgeOrders;
		adoptedOrderCount += hm.Adopt(adoptedHedgeOrders, levels, count, stackIndex);

		std::vector<KTN::OrderPod> manualOrders;
		orderSender.GetManualOrdersBySecId(manualOrders, strategy.GetSecId());
		if (!manualOrders.empty())
		{
			std::vector<KTN::OrderPod> manualOrdersToAdopt;
			for (const KTN::OrderPod &op : manualOrders)
			{
				if (op.OrdState == KTN::ORD::KOrderState::COMPLETE)
				{
					LOGINFO("AklStacker {} | Ignoring manual order {} because state is {} ({})",
						strategy.StrategyId(), op.orderReqId, KTN::ORD::KOrderState::toString(op.OrdState), static_cast<int>(op.OrdState));
				}
				else if (orderMap.find(op.orderReqId) != orderMap.end())
				{
					LOGINFO("AklStacker {} | Ignoring manual order {} because reqId already exists in orderMap",
						strategy.StrategyId(), op.orderReqId);
				}
				else if (doneOrderMap.find(op.orderReqId) != doneOrderMap.end())
				{

				}
				else if (op.OrdState != KTN::ORD::KOrderState::WORKING)
				{
					LOGINFO("AklStacker {} | Ignoring manual order {} because state is {} ({}) rather than WORKING",
						strategy.StrategyId(), op.orderReqId, KTN::ORD::KOrderState::toString(op.OrdState), static_cast<int>(op.OrdState));
				}
				else
				{
					if (levels != nullptr && count > 0)
					{
						for (size_t i = 0; i < count; ++i)
						{
							const AdoptLevelInfo &ali = levels[i];
							if (op.OrdSide == ali.side && op.price == ali.px)
							{
								if (CanAddOrderToStack(op.OrdSide, op.orderReqId, op.price))
								{
									manualOrdersToAdopt.push_back(op);
									break;
								}
							}

						}
					}
					else
					{
						if (CanAddOrderToStack(op.OrdSide, op.orderReqId, op.price))
						{
							manualOrdersToAdopt.push_back(op);
						}
					}
				}
			}

			for (const KTN::OrderPod &o : manualOrdersToAdopt)
			{
				if (o.OrdState != KTN::ORD::KOrderState::WORKING)
				{
					LOGINFO("AklStacker {} | Manual order {} has state {} ({}) rather than WORKING",
						strategy.StrategyId(), o.orderReqId, KTN::ORD::KOrderState::toString(o.OrdState), static_cast<int>(o.OrdState));
				}

				Order *order = CreateOrder(o.OrdSide, Quantity(o.quantity), o.price);
				order->orderReqId = o.orderReqId;
				order->cmeId = o.exchordid;
				order->ackedPrice = o.price;
				order->ackedQuantity = order->targetQuantity;
				order->filledQuantity = Quantity(o.fillqty);
				order->type = Order::Type::NONE;
				order->submitted = true;
				order->tif = o.OrdTif;

				order->stackIndex = 0;
				orderMap[order->orderReqId] = order;

				KTN::OrderPod newO;
				quoter.NextNew(newO, o.OrdSide, o.exchordid, o.quantity, o.price);
				newO.price = o.price;
				newO.stopprice = o.stopprice;
				newO.lastpx = o.lastpx;
				newO.timestamp = o.timestamp;
				newO.orderReqId = o.orderReqId;
				newO.exchordid = o.exchordid;
				newO.leavesqty = o.leavesqty;
				newO.fillqty = o.fillqty;
				newO.lastqty = 0;
				newO.dispqty = o.dispqty;
				newO.minqty = o.minqty;
				newO.mktsegid = o.mktsegid;
				newO.instindex = 0; // ??
				newO.OrdExch = o.OrdExch;
				newO.OrdType = o.OrdType;
				newO.OrdTif = o.OrdTif;
				newO.OrdAction = o.OrdAction;
				newO.OrdSide = o.OrdSide;
				newO.OrdStatus = o.OrdStatus;
				newO.OrdState = o.OrdState;
				newO.OrdFillType = o.OrdFillType;
				OrderUtils::fastCopy(newO.ordernum, o.ordernum, sizeof(newO.ordernum));

				OrderUtils::fastCopy(order->orderPod.origordernum, o.ordernum, sizeof(order->orderPod.origordernum));

				if (AddOrderToStack(order, true, stackIndex != 1, stackIndex == 1))
				{
					strategy.GetOrderManager().onSentOrderFast(newO);
					strategy.GetOrderManager().minOrderAddInState(newO);
					strategy.GetOrderManager().minOrderUpdate(newO, newO.OrdStatus, newO.OrdState);
					orderSender.MoveOrderToStrategy(newO);

					LOGINFO("AklStacker {} | Adopted manual order reqId={} clOrdId={} exchordid={}",
						strategy.StrategyId(), o.orderReqId, o.ordernum, o.exchordid);

					++adoptedOrderCount;

					strategy.pendingOrderUpdates.push(akl::OrderUpdate { order->cmeId, order->stackIndex, false });
				}
				else
				{
					LOGWARN("AklStacker {} | Failed to adopt manual order reqId={} clOrdId={} exchordid={}",
						strategy.StrategyId(), o.orderReqId, o.ordernum, o.exchordid);
				}

			}

			// KTN::OrderPod o = manualOrders.front();
			// if (o.OrdState != KTN::ORD::KOrderState::COMPLETE)
			// {
			// 	if (o.OrdState != KTN::ORD::KOrderState::WORKING)
			// 	{
			// 		logger.LogFmt("AklStacker {} | Manual order {} has state {} ({}) rather than WORKING",
			// 			strategy.StrategyId(), o.orderReqId, KTN::ORD::KOrderState::toString(o.OrdState), static_cast<int>(o.OrdState));
			// 	}

			// 	Order *order = CreateOrder(o.OrdSide, Quantity(o.quantity), o.price);
			// 	order->orderReqId = o.orderReqId;
			// 	order->cmeId = o.cmeid;
			// 	order->ackedPrice = o.price;
			// 	order->ackedQuantity = order->targetQuantity;
			// 	order->filledQuantity = Quantity(o.fillqty);
			// 	order->type = Order::Type::NONE;

			// 	order->stackIndex = 0;


			// 	orderMap[order->orderReqId] = order;

			// 	//strategyExecution.GetRisk().AddWorking(order);

			// 	KTN::OrderPod newO;
			// 	quoter.NextNew(newO, o.OrdSide, o.cmeid, o.quantity, o.price);
			// 	newO.stopprice = o.stopprice;
			// 	newO.lastpx = o.lastpx;
			// 	newO.timestamp = o.timestamp;
			// 	newO.orderReqId = o.orderReqId;
			// 	newO.cmeid = o.cmeid;
			// 	newO.leavesqty = o.leavesqty;
			// 	newO.fillqty = o.fillqty;
			// 	newO.lastqty = o.lastqty;
			// 	newO.dispqty = o.dispqty;
			// 	newO.minqty = o.minqty;
			// 	newO.mktsegid = o.mktsegid;
			// 	newO.instindex = 0; // ??
			// 	newO.OrdExch = o.OrdExch;
			// 	newO.OrdType = o.OrdType;
			// 	newO.OrdTif = o.OrdTif;
			// 	newO.OrdAction = o.OrdAction;
			// 	newO.OrdSide = o.OrdSide;
			// 	newO.OrdStatus = o.OrdStatus;
			// 	newO.OrdState = o.OrdState;
			// 	newO.OrdFillType = o.OrdFillType;
			// 	strategy.GetOrderManager().onSentOrderFast(newO);
			// 	strategy.GetOrderManager().minOrderUpdate(newO, newO.OrdStatus, newO.OrdState);

			// 	KT01_LOG_INFO(__CLASS__, "AklStacker {} | Adopting manual order reqId={} clOrdId={} cmeId={}",
			// 		strategy.StrategyId(), o.orderReqId, o.ordernum, o.cmeid);
			// }
			// else
			// {
			// 	logger.LogFmt("AklStacker {} | Ignoring recovered order {} becase state is COMPLETE",
			// 		strategy.StrategyId(), o.orderReqId);
			// }
		}

		if (settings.quoteInstrumentSettings.cancelUnadoptedOrders)
		{
			for (Order *&o : ordersToCancel)
			{
				Cancel(o);
			}
		}

		if (adoptedOrderCount > 0)
		{
			std::ostringstream ss;
			ss << "Strategy " << strategy.StrategyId() << " adopted " << adoptedOrderCount << " orders";
			const std::string str = ss.str();
			notifier.Notify(str, KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
			LOGINFO("{}", str);
			QueueStackInfoUpdate();
		}
		else
		{
			std::ostringstream ss;
			ss << "Strategy " << strategy.StrategyId() << " found no orders to adopt";
			const std::string str = ss.str();
			notifier.Notify(str, KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
			LOGINFO("{}", str);
		}

		return count;
	}
	else
	{
		return 0;
	}
}

void Instrument::GetProrataOrders(std::vector<Order *> &orders) const
{
	prorataQuoter.GetOrders<KTN::ORD::KOrderSide::Enum::BUY>(orders);
	prorataQuoter.GetOrders<KTN::ORD::KOrderSide::Enum::SELL>(orders);
}

bool Instrument::CheckTimers(const Timestamp now)
{
	bool result = pricingModel[0].CheckTimers(now);
	result |= pricingModel[1].CheckTimers(now);
	result |= prorataPricingModel.CheckTimers(now);
	//result |= dynamicPricingModel.CheckTimers(now);
	result |= hm.CheckTimers(now);
	result |= sh.CheckTimers(now);
	strategyExecution.Process(now);
	return result;
}

void Instrument::CheckOrderPool()
{
	const int size = orderPool.Size();
	if (size < 512)
	{
		orderPool.Refresh();
		LOGINFO("AklStacker {} | Refreshing order pool", strategy.StrategyId());
	}
}

void Instrument::SetHasSpreader(const bool val)
{
	hasSpreader = val;
	pricingModel[0].SetHasSpreader(val);
	pricingModel[1].SetHasSpreader(val);
	prorataPricingModel.SetHasSpreader(val);
}

void Instrument::removeOrder(Order *o, const bool wasCancel, const bool wasFill)
{
	switch (o->type)
	{
		case Order::Type::NONE:
		case Order::Type::STOP:
			// Do nothing
			break;
		case Order::Type::STACK:
			if (o->stackIndex == 0)
			{
				if (stack[0].RemoveOrder(o, wasFill))
				{
					LOGINFO("Successfully removed done stack order {}", o->orderReqId);
				}
				else if (mstack[0].Remove(o))
				{
					LOGINFO("Successfully removed done mstack order {}", o->orderReqId);
				}
				else if (stack[1].RemoveOrder(o, wasFill))
				{
					LOGINFO("Successfully removed done stack2 order {} with incorrect stack index", o->orderReqId);
				}
				else if (mstack[1].Remove(o))
				{
					LOGINFO("Successfully removed done mstack2 order {} with incorrect stack index", o->orderReqId);
				}
				else
				{
					LOGINFO("Failed to remove done stack order {}", o->orderReqId);
				}
			}
			else if (o->stackIndex == 1)
			{
				if (stack[1].RemoveOrder(o, wasFill))
				{
					LOGINFO("Successfully removed done stack2 order {}", o->orderReqId);
				}
				else if (mstack[1].Remove(o))
				{
					LOGINFO("Successfully removed done mstack2 order {}", o->orderReqId);
				}
				else if (stack[0].RemoveOrder(o, wasFill))
				{
					LOGINFO("Successfully removed done stack order {} with incorrect stack index", o->orderReqId);
				}
				else if (mstack[0].Remove(o))
				{
					LOGINFO("Successfully removed done mstack order {} with incorrect stack index", o->orderReqId);
				}
				else
				{
					LOGINFO("Failed to remove done stack order {}", o->orderReqId);
				}
			}
			else
			{
				if (stack[0].RemoveOrder(o, wasFill))
				{
					LOGINFO("Successfully removed done stack order {}", o->orderReqId);
				}
				else if (mstack[0].Remove(o))
				{
					LOGINFO("Successfully removed done mstack order {}", o->orderReqId);
				}
				else if (stack[1].RemoveOrder(o, wasFill))
				{
					LOGINFO("Successfully removed done stack2 order {}", o->orderReqId);
				}
				else if (mstack[1].Remove(o))
				{
					LOGINFO("Successfully removed done mstack2 order {}", o->orderReqId);
				}
				else
				{
					LOGINFO("Failed to remove done stack order {}", o->orderReqId);
				}
				LOGINFO("Order has bad stack index {}", static_cast<int16_t>(o->stackIndex));
			}
			strategy.QueueStackInfoUpdate();
			break;
		case Order::Type::HEDGE:
			if (hm.RemoveOrder(o))
			{
				LOGINFO("Successfully removed done hedge order {}", o->orderReqId);
			}
			else
			{
				LOGINFO("Failed to remove done hedge order {}", o->orderReqId);
			}
			break;
		case Order::Type::PRORATA:
			if (prorataQuoter.RemoveOrder(o, wasCancel))
			{
				LOGINFO("Successfully removed done prorata order {}", o->orderReqId);
			}
			else
			{
				LOGINFO("Failed to remove done prorata order {}", o->orderReqId);
			}
			break;
	}

	if (o->cmeId > 0 )
	{
		strategy.pendingOrderUpdates.push(akl::OrderUpdate { o->cmeId, o->stackIndex, true });
	}
}

}
