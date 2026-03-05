#include "AklStacker.hpp"
#include "Instrument.hpp"
#include "Helpers.hpp"
#include "PriceConverter.hpp"
#include "messages/MessageType.hpp"

#include <KT01/Core/Settings.hpp>
#include <akl/BranchPrediction.hpp>
#include <akl/ProrataProfileManager.hpp>
#include <akl/PositionStore.hpp>
#include <rapidjson/error/en.h>

#include "messages/StackerAdoptMessage.hpp"
#include "messages/StopUpdateMessage.hpp"
#include "messages/ProrataProfileDeleteMessage.hpp"
#include "messages/StackerProrataOrderRequestMessage.hpp"
#include "messages/StackerProrataOrderAdoptMessage.hpp"
#include "messages/StackerProrataOrderAbandonMessage.hpp"
#include "messages/StackerProrataOrderResponseMessage.hpp"
#include "messages/HeartbeatMessage.hpp"
#include "messages/StrategyStatusMessage.hpp"
#include "messages/StackerSpreaderUpdateMessage.hpp"
#include "Formatters.hpp"

#include <exchsupport/cfe/mktdata/CfeMktDataHandler.hpp>

namespace akl::stacker
{

AklStacker::AklStacker(AlgoInitializer &params)
	: AlgoBaseV3(params)
	, orderPool(1000) // TODO figure out how many we need
	, publisher("AklStackerPublisher", 10, params.guid)
{
	LOGINFO("AklStacker::AklStacker");

	// Load verbose logging flag from DebugSettings.txt
	Settings dbgsett;
	dbgsett.Load("DebugSettings.txt", "DBG");
	LOAD_TYPED_SETTING_OR_DEFAULT(dbgsett, "Debug.AklStacker.Verbose", getBoolean, _verboseLog, true);
	LOGINFO("AklStacker {} | verboseLog={}", StrategyId(), _verboseLog);

	LOGINFO("AklStacker::AklStacker logging recovered orders");
	for (const KTN::OrderPod &ord : params.orders)
	{
		LOGINFO("AklStacker::AklStacker recovered order: {}", ord);
	}
	LOGINFO("AklStacker::AklStacker finished logging recovered orders");
	recoveredOrders = params.orders;

	if (!recoveredOrders.empty())
	{
		for (OrderPod &ord : recoveredOrders)
		{
			ord.callbackid = _EXCH_CALLBACK_ID;
			_Ords.minOrderAddInState(ord);
			_ordersender->MoveOrderToStrategy(ord);
		}
	}

	uint64_t startingReqId = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	quoter.SetStartingReqId(startingReqId);
	LOGINFO("AklStacker {} using {} as starting reqId", StrategyId(), startingReqId);
}

AklStacker::~AklStacker()
{

}

void AklStacker::onLaunch(std::string json)
{
	LOGINFO("AklStacker::onLaunch: {}", json);

	if (unlikely(_LEGS.size() == 0))
	{
		LOGERROR("No legs defined for AklStacker strategy");
		std::ostringstream ss;
		ss << "AklStacker " << StrategyId() << " failed to launch: No instruments configured";
		Notify(ss.str(), KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
		return;
	}
	else
	{
		if (_LEGS.size() > 2)
		{
			LOGWARN("More than two legs not supported for AklStacker strategy");
		}

		std::lock_guard<std::mutex> lock(mutex);

		symbol = _LEGS[0].symbol;
		book[0].SetSymbol(symbol);
		if (_LEGS[0].exchEnum == KTN::ORD::KOrderExchange::Enum::CME)
		{
			StrategyManager::instance().AddStacker(this);

			const CmeSecDef def = CmeSecMaster::instance().getSecDef(_LEGS[0].symbol);
			if (def.secid != 0)
			{
				secId = def.secid;
				RegisterCmeSecId(_LEGS[0]);

				priceConverter = new PriceConverter(def);
			}
			else
			{
				LOGERROR("AklStacker {} | Failed to initialize: can't find symbol {}", StrategyId(), _LEGS[0].symbol);
				std::ostringstream ss;
				ss << "AklStacker " << StrategyId() << " failed to launch: Can't find symbol " << _LEGS[0].symbol;
				Notify(ss.str(), MsgLevel::Enum::ERROR, MsgType::Enum::ALERT);
				return;
			}
		}
		else if (_LEGS[0].exchEnum == KTN::ORD::KOrderExchange::Enum::CFE)
		{
			StrategyManager::instance().AddStacker(this);

			const CfeSecDef def = CfeSecMaster::instance().getSecDef(_LEGS[0].symbol);
			if (def.secid != 0)
			{
				secId = def.secid;
				quoteExchange = KTN::ORD::KOrderExchange::Enum::CFE;
				RegisterCfeSecId(_LEGS[0]);
				LOGINFO("AklStacker {} | Registered CFE secId {} for symbol {}", StrategyId(), secId, _LEGS[0].symbol);

				priceConverter = new PriceConverter(def);
			}
			else
			{
				LOGERROR("AklStacker {} | Failed to initialize: can't find symbol {}", StrategyId(), _LEGS[0].symbol);
				std::ostringstream ss;
				ss << "AklStacker " << StrategyId() << " failed to launch: Can't find symbol " << _LEGS[0].symbol;
				Notify(ss.str(), MsgLevel::Enum::ERROR, MsgType::Enum::ALERT);
				return;
			}
		}
		else
		{
			LOGERROR("Only CME and CFE legs supported for AklStacker strategy");
			std::ostringstream ss;
			ss << "AklStacker " << StrategyId() << " failed to launch: only CME and CFE instruments currently supported";
			Notify(ss.str(), MsgLevel::Enum::ERROR, MsgType::Enum::ALERT);
			return;
		}

		{
			settings = AklStackerParams::parse(json, *priceConverter);
			UpdateMetaParams(settings.meta);
			// Alternative prorata profiles should default to empty
			settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile = "";
			settings.quoteInstrumentSettings.askSettings.alternateProrataProfile = "";

			quoter.Construct(_EXCH_CALLBACK_ID, _LEGS[0], _PARAMS);

			notifier = new StrategyNotifier(settings.meta.Source, "AKL"); // TODO do we need org to be configurable?

			const Price tickIncrement = Price::FromUnshifted(_LEGS[0].mintick); // TODO take a closer look at this
			LOGINFO("AklStacker {} | Tick increment for quote symbol {} is {} (shifted: {})", StrategyId(), symbol, _LEGS[0].mintick, tickIncrement.AsShifted());
			book[0].TickIncrement = tickIncrement;
			instr = new Instrument(*this, quoter, orderPool, *_ordersender, *notifier, settings, book[0], tickIncrement);
			initStacks();
			instr->AddRecoveredOrders(recoveredOrders);
			recoveredOrders.clear();

			initProrataProfiles();
			initProrataProfile<KTN::ORD::KOrderSide::Enum::BUY>();
			initProrataProfile<KTN::ORD::KOrderSide::Enum::SELL>();

			settingsUpdateNeeded.store(true, std::memory_order_release);
		}

		if (_LEGS.size() > 1)
		{
			spreaderSymbol = _LEGS[1].symbol;
			book[1].SetSymbol(spreaderSymbol);
			if (_LEGS[1].iscme)
			{
				const CmeSecDef def = CmeSecMaster::instance().getSecDef(_LEGS[1].symbol);
				if (def.secid != 0)
				{
					spreaderSecId = def.secid;
					RegisterCmeSecId(_LEGS[1]);
					const Price tickIncrement = Price::FromUnshifted(_LEGS[1].mintick); // TODO take a closer look at this
					book[1].TickIncrement = tickIncrement;
					instr->SetHasSpreader(true);
					instr->GetSpreaderPricingModel().RandomizeTicks();
				}
				else
				{
					LOGERROR("AklStacker {} | Failed to initialize spreader instrument: can't find symbol {}", StrategyId(), _LEGS[1].symbol);
					std::ostringstream ss;
					ss << "AklStacker " << StrategyId() << " failed to find spreader symbol " << _LEGS[1].symbol;
					Notify(ss.str(), MsgLevel::Enum::ERROR, MsgType::Enum::ALERT);
				}
			}
			else
			{
				LOGERROR("Non-CME legs not supported for spreader symbol in AklStacker strategy");
				std::ostringstream ss;
				ss << "AklStacker " << StrategyId() << " does not support non-CME instruments for spreader";
				Notify(ss.str(), MsgLevel::Enum::ERROR, MsgType::Enum::ALERT);
			}
		}
		else
		{
			settings.spreaderPricingSettings.bidEnabled[0] = false;
			settings.spreaderPricingSettings.bidEnabled[1] = false;
			settings.spreaderPricingSettings.bidEnabled[2] = false;
			settings.spreaderPricingSettings.askEnabled[0] = false;
			settings.spreaderPricingSettings.askEnabled[1] = false;
			settings.spreaderPricingSettings.askEnabled[2] = false;
		}

		_mdh->SetHandleTrades(true);
		StartMarketDataHandler();
	}

#if !AKL_TEST_MODE		
	statusThread = std::thread(&AklStacker::runStatusThread, this);
#endif

	if (settings.meta.Enabled != 0)
	{
		_ENABLED = true;
	}
	else
	{
		_ENABLED = false;
	}
	_PARAMS.Enabled = settings.meta.Enabled;

	// if (settings.meta.Enabled != 0)
	// {
	// 	isRunning = true;
	// 	std::cout << "AklStacker is running" << std::endl;
	// }
	// else
	// {
	// 	isRunning = false;
	// 	std::cout << "AklStacker is not running" << std::endl;
	// }

	publisher.SetSource(settings.meta.Source);
#if !AKL_TEST_MODE
	publisher.Start();
#endif

	//Notify("Started AklStacker for " + symbol, MsgLevel::Enum::INFO, MsgType::Enum::NOTIFY);
}

void AklStacker::onJson(std::string json)
{
	if (unlikely(!IsReady()))
	{
		LOGWARN("AklStacker::onJson: not ready");
		return;
	}

	settings.reset = false;

	const int oldEnabled = settings.meta.Enabled;
	const int oldBidLength = settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length;
	const int oldAskLength = settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length;
	const bool oldSecondStackEnabled = settings.quoteInstrumentSettings.secondStackEnabled;
	//const bool oldShowProrata = settings.quoteInstrumentSettings.showProrata;
	//const bool oldShowAltManualAdj = settings.quoteInstrumentSettings.showAltManualAdj;
	const int oldBidLength2 = settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length;
	const int oldAskLength2 = settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length;

	const std::string oldBidProrataProfile = settings.quoteInstrumentSettings.bidSettings.prorataProfile;
	const std::string oldAskProrataProfile = settings.quoteInstrumentSettings.askSettings.prorataProfile;
	const std::string oldBidAltProrataProfile = settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile;
	const std::string oldAskAltProrataProfile = settings.quoteInstrumentSettings.askSettings.alternateProrataProfile;

	const Quantity prevMaxOrderQty = settings.quoteInstrumentSettings.riskSettings.maxOrderQty;

	std::lock_guard<std::mutex> lock(mutex);

	const AklStackerParams::ParseResult result = settings.update(json, *priceConverter);

	if (settings.meta.Enabled != oldEnabled)
	{
		if (settings.meta.Enabled != 0)
		{
			_ENABLED = true;
		}
		else
		{
			_ENABLED = false;
		}
		_PARAMS.Enabled = settings.meta.Enabled;
	}

	instr->UpdateStackQuantities();
	if (spreaderSecId > 0)
	{
		instr->GetSpreaderPricingModel().RandomizeTicks();
	}
	else
	{
		// Make sure spreader is disabled if we don't have a spreader instrument
		settings.spreaderPricingSettings.bidEnabled[0] = false;
		settings.spreaderPricingSettings.bidEnabled[1] = false;
		settings.spreaderPricingSettings.bidEnabled[2] = false;
		settings.spreaderPricingSettings.askEnabled[0] = false;
		settings.spreaderPricingSettings.askEnabled[1] = false;
		settings.spreaderPricingSettings.askEnabled[2] = false;
	}

	if (oldBidLength != settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length)
	{
		instr->GetStack(0).GetSide<KTN::ORD::KOrderSide::Enum::BUY>().UpdateLength(settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length);
		LOGINFO("AklStacker {} | index=0 | stackBidLength changed from {} to {}",
			StrategyId(), oldBidLength, settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length);
	}
	if (oldAskLength != settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length)
	{
		instr->GetStack(0).GetSide<KTN::ORD::KOrderSide::Enum::SELL>().UpdateLength(settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length);
		LOGINFO("AklStacker {} | index=0 | stackAskLength changed from {} to {}",
			StrategyId(), oldAskLength, settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length);
	}
	if (oldSecondStackEnabled && !settings.quoteInstrumentSettings.secondStackEnabled)
	{
		if (instr->GetStack(1).HasOrders())
		{
			LOGWARN("Rejecting turning off of second stack, because it currently has orders");
			settings.quoteInstrumentSettings.secondStackEnabled = oldSecondStackEnabled;
			// const char *msg = "Rejected turning off of second stack, because it currently has orders";
			// SendText(msg);
			// QueueAudit(msg);
			// TODO send audit message and update parameters on UI
		}
	}
	if (oldBidLength2 != settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length)
	{
		instr->GetStack(1).GetSide<KTN::ORD::KOrderSide::Enum::BUY>().UpdateLength(settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length);
		LOGINFO("AklStacker {} | index=1 | stackBidLength changed from {} to {}",
			StrategyId(), oldBidLength2, settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length);
	}
	if (oldAskLength2 != settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length)
	{
		instr->GetStack(1).GetSide<KTN::ORD::KOrderSide::Enum::SELL>().UpdateLength(settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length);
		LOGINFO("AklStacker {} | index=1 | stackAskLength changed from {} to {}",
			StrategyId(), oldAskLength2, settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length);
	}
	if (oldBidProrataProfile != settings.quoteInstrumentSettings.bidSettings.prorataProfile)
	{
		ProrataProfileMapType::const_iterator it = prorataMap.find(settings.quoteInstrumentSettings.bidSettings.prorataProfile);
		if (likely(it != prorataMap.end()))
		{
			instr->OnProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::BUY>(it->second);
		}
		else
		{
			instr->ClearProrataProfile<KTN::ORD::KOrderSide::Enum::BUY>();
			LOGWARN("AklStacker {} | Failed to find bid prorataprofile {}. Clearing.",
				StrategyId(), settings.quoteInstrumentSettings.bidSettings.prorataProfile);
		}
		LOGINFO("AklStacker {} | bidProrataProfile changed from {} to {}",
			StrategyId(), oldBidProrataProfile, settings.quoteInstrumentSettings.bidSettings.prorataProfile);
	}
	if (oldAskProrataProfile != settings.quoteInstrumentSettings.askSettings.prorataProfile)
	{
		ProrataProfileMapType::const_iterator it = prorataMap.find(settings.quoteInstrumentSettings.askSettings.prorataProfile);
		if (likely(it != prorataMap.end()))
		{
			instr->OnProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::SELL>(it->second);
		}
		else
		{
			instr->ClearProrataProfile<KTN::ORD::KOrderSide::Enum::SELL>();
			LOGWARN("AklStacker {} | Failed to find ask prorataprofile {}. Clearing.",
				StrategyId(), settings.quoteInstrumentSettings.askSettings.prorataProfile);
		}
		LOGINFO("AklStacker {} | index=0 | stackAskProrataProfile changed from {} to {}",
			StrategyId(), oldAskProrataProfile, settings.quoteInstrumentSettings.askSettings.prorataProfile);
	}
	if (oldBidAltProrataProfile != settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile)
	{
		ProrataProfileMapType::const_iterator it = prorataMap.find(settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile);
		if (likely(it != prorataMap.end()))
		{
			instr->OnAltProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::BUY>(it->second);
		}
		else
		{
			instr->ClearAltProrataProfile<KTN::ORD::KOrderSide::Enum::BUY>();
			LOGWARN("AklStacker {} | Failed to find bid alt prorata profile {}. Clearing.",
				StrategyId(), settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile);
		}
		LOGINFO("AklStacker {} | bidAltProrataProfile changed from {} to {}",
			StrategyId(), oldBidAltProrataProfile, settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile);
	}
	if (oldAskAltProrataProfile != settings.quoteInstrumentSettings.askSettings.alternateProrataProfile)
	{
		ProrataProfileMapType::const_iterator it = prorataMap.find(settings.quoteInstrumentSettings.askSettings.alternateProrataProfile);
		if (likely(it != prorataMap.end()))
		{
			instr->OnAltProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::SELL>(it->second);
		}
		else
		{
			instr->ClearAltProrataProfile<KTN::ORD::KOrderSide::Enum::SELL>();
			LOGWARN("AklStacker {} | Failed to find ask alt prorata profile {}. Clearing.",
				StrategyId(), settings.quoteInstrumentSettings.askSettings.alternateProrataProfile);
		}
		LOGINFO("AklStacker {} | askAltProrataProfile changed from {} to {}",
			StrategyId(), oldAskAltProrataProfile, settings.quoteInstrumentSettings.askSettings.alternateProrataProfile);
	}
	if (settings.quoteInstrumentSettings.riskSettings.maxOrderQty < prevMaxOrderQty)
	{
		instr->GetStopHandler().CheckAllStopsForNumberOfOrders();
	}

	instr->Recalculate(false);
	settings.reset = false;

	if (result.priceBandsChanged)
	{
		instr->GetStack(0).HandlePriceBandUpdate();
		instr->GetStack(1).HandlePriceBandUpdate();
	}

	// if (!oldEnabled && settings.meta.Enabled)
	// {
	// 	isRunning = true;
	// 	doStart();
	// 	std::cout << "AklStacker is now running" << std::endl;
	// }
	// else if (oldEnabled && !settings.meta.Enabled)
	// {
	// 	isRunning = false;
	// 	doStop();
	// 	std::cout << "AklStacker is now stopped" << std::endl;
	// }

	LogVerbose("AklStacker::onJson | {} | {}", StrategyId(), json);

	settingsUpdateNeeded.store(true, std::memory_order_release);
}

void AklStacker::onCalculate()
{
	std::ostringstream ss;
	ss << std::this_thread::get_id();
	LogVerbose("AklStacker::onCalculate | {}", ss.str());

}

void AklStacker::Enable(bool enable)
{
	std::ostringstream ss;
	ss << std::this_thread::get_id();
	LOGINFO("AklStacker::Enable: {} | {}", ss.str(), enable);
	
}

void AklStacker::onTrade(int index, double price, double size, int aggressor)
{
	std::ostringstream ss;
	ss << std::this_thread::get_id();
	LOGINFO("AklStacker::onTrade | {} | index={}, price={}, size={}, aggressor={}", ss.str(), index, price, size, aggressor);
	AlgoBaseV3::onTrade(index, price, size, aggressor);
}

void AklStacker::onMktData(int index)
{
	std::ostringstream ss;
	ss << std::this_thread::get_id();
	LogVerbose("AklStacker::onMktData | {} | {}", ss.str(), index);
	AlgoBaseV3::onMktData(index);
}

void AklStacker::onHeartbeat(int msgseqnum)
{
	std::ostringstream ss;
	ss << std::this_thread::get_id();
	LogVerbose("AklStacker::onHeartbeat | {} | {}", ss.str(), msgseqnum);
	AlgoBaseV3::onHeartbeat(msgseqnum);
}

void AklStacker::onSessionStatus(std::string exch, int mktsegid, int protocol, std::string desc, std::string state)
{
	std::ostringstream ss;
	ss << std::this_thread::get_id();
	LOGINFO("AklStacker::onSessionStatus | {} | exch={}, mktsegid={}, protocol={}, desc={}, state={}",
		ss.str(), exch, mktsegid, protocol, desc, state);
	AlgoBaseV3::onSessionStatus(exch, mktsegid, protocol, desc, state);
}

void AklStacker::minOrderAck(KTN::OrderPod &ord)
{
	_Ords.minOrderAck(ord);

	time.SetTime();

	LOGINFO("AklStacker::minOrderAck | {} | {}", StrategyId(), ord);

	if (likely(IsReady()))
	{
		std::lock_guard<std::mutex> lock(mutex);
		instr->OnAck(ord);
	}
	else
	{
		LOGWARN("AklStacker::minOrderAck | {} | Not ready", StrategyId());
	}
}

void AklStacker::minOrderCancel(KTN::OrderPod &ord)
{
	_Ords.minOrderCancel(ord);

	time.SetTime();

	LOGINFO("AklStacker::minOrderCancel | {} | {}", StrategyId(),ord);

	if (likely(IsReady()))
	{
		std::lock_guard<std::mutex> lock(mutex);
		instr->OnCancelAck(ord);
	}
	else
	{
		LOGWARN("AklStacker::minOrderCancel | {} | Not ready", StrategyId());
	}
}

void AklStacker::minOrderModify(KTN::OrderPod &ord)
{
	_Ords.minOrderModify(ord);

	time.SetTime();

	LOGINFO("AklStacker::minOrderModify | {} | {}", StrategyId(), ord);

	if (likely(IsReady()))
	{
		std::lock_guard<std::mutex> lock(mutex);
		instr->OnModAck(ord);
	}
	else
	{
		LOGWARN("AklStacker::minOrderModify | {} | Not ready", StrategyId());
	}
}

void AklStacker::minOrderExecution(KTN::OrderPod &ord, bool hedgeSent)
{
	const uint64_t strategyTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	if (ord.OrdFillType == KOrderFillType::SPREAD_LEG_FILL)
	{
		_Ords.minOrderExecSpreadLeg(ord);
		return;
	}

	_Ords.minOrderExecution(ord, hedgeSent);

	time.SetTime();

	if (likely(IsReady()))
	{
		std::lock_guard<std::mutex> lock(mutex);
		instr->GetStrategyExecution().ReceiveTime = ord.recvTime;
		instr->OnFill(ord);
		instr->GetStrategyExecution().ReceiveTime = 0;
	}

	QueuePositionUpdate();

	LOGINFO("AklStacker::minOrderExecution | {} | ord={}, hedgeSent={} | recvTime={} |  queueWrite={} | stratTime={}",
		StrategyId(), ord, hedgeSent, ord.recvTime, ord.queueWriteTime, strategyTime);

}

void AklStacker::minOrderReject(KTN::OrderPod &ord, const std::string &text)
{
	_Ords.minOrderReject(ord, text);

	time.SetTime();

	LOGINFO("AklStacker::minOrderReject | {} | ord={}, text={}", StrategyId(), ord, text);

	if (likely(IsReady()))
	{
		std::lock_guard<std::mutex> lock(mutex);
		instr->OnSubmitReject(ord);
	}
	else
	{
		LOGWARN("AklStacker::minOrderReject | {} | Not ready", StrategyId());
	}
}

void AklStacker::minOrderCancelReject(KTN::OrderPod &ord, const std::string &text)
{
	const Order *o = nullptr;

	if (likely(IsReady()))
	{
		std::lock_guard<std::mutex> lock(mutex);
		instr->OnCancelReject(ord);
		o = instr->GetOrder(ord.orderReqId);
	}

	if (o != nullptr && o->IsWorking())
	{
		ord.OrdState = KTN::ORD::KOrderState::Enum::WORKING;
		ord.quantity = o->ackedQuantity.value();
		ord.leavesqty = o->WorkingQuantity().value();
		ord.price = o->ackedPrice;
		ord.secid = secId;
		ord.OrdSide = o->side;

		ord.OrdStatus = KTN::ORD::KOrderStatus::Enum::MODIFIED; // HACK: I think UI will only update order if statues is NEW, MODIFIED, or PARTIAL
		ord.OrdAlgoTrig = KTN::ORD::KOrderAlgoTrigger::Enum::ALGO_LEG;
	}

	_Ords.minOrderCancelReject(ord, text);

	time.SetTime();

	LOGINFO("AklStacker::minOrderCancelReject | {} | ord={}, text={}", StrategyId(), ord, text);
}

void AklStacker::minOrderCancelReplaceReject(KTN::OrderPod &ord, const std::string &text)
{
	const Order *o = nullptr;

	if (likely(IsReady()))
	{
		std::lock_guard<std::mutex> lock(mutex);
		instr->OnCancelReplaceReject(ord);
		o = instr->GetOrder(ord.orderReqId);
	}

	if (o != nullptr && o->IsWorking())
	{
		ord.OrdState = KTN::ORD::KOrderState::Enum::WORKING;
		ord.quantity = o->ackedQuantity.value();
		ord.leavesqty = o->WorkingQuantity().value();
		ord.price = o->ackedPrice;
		ord.secid = secId;
		ord.OrdSide = o->side;

		ord.OrdStatus = KTN::ORD::KOrderStatus::Enum::MODIFIED; // HACK: I think UI will only update order if statues is NEW, MODIFIED, or PARTIAL
		ord.OrdAlgoTrig = KTN::ORD::KOrderAlgoTrigger::Enum::ALGO_LEG;
	}

	_Ords.minOrderCancelReplaceReject(ord, text);

	time.SetTime();

	LOGINFO("AklStacker::minOrderCancelReplaceReject | {} | ord={}, text={}", StrategyId(), ord, text);
}

void AklStacker::onHedgeOrderSent(
	char *clordid,
	uint64_t reqid,
	int32_t secid,
	KOrderSide::Enum side,
	uint32_t qty,
	int32_t price9,
	uint16_t instindex)
{
	LOGINFO("AklStacker::onHedgeOrderSent: clordid={}, reqid={}, secid={}, side={}, qty={}, price9={}, instindex={}", clordid, reqid, secid, KOrderSide::toString(side), qty, price9, instindex);
	AlgoBaseV3::onHedgeOrderSent(clordid, reqid, secid, side, qty, price9, instindex);
}

void AklStacker::onOrderSent(
	char *clordid,
	uint64_t reqid,
	int32_t secid,
	KOrderSide::Enum side,
	uint32_t qty,
	int32_t price,
	KOrderAlgoTrigger::Enum trig)
{
	LOGINFO("AklStacker::onOrderSent: clordid={}, reqid={}, secid={}, side={}, qty={}, price={}, trig={}", clordid, reqid, secid, KOrderSide::toString(side), qty, price, KOrderAlgoTrigger::toString(trig));
	AlgoBaseV3::onOrderSent(clordid, reqid, secid, side, qty, price, trig);
}

void AklStacker::ResetPosition()
{
	LOGINFO("AklStacker::ResetPosition | {}", StrategyId());
	AlgoBaseV3::ResetPosition();
}

void AklStacker::ToggleCommand(int id)
{
	LOGINFO("AklStacker::ToggleCommand: {} | {}", StrategyId(), id);
	AlgoBaseV3::ToggleCommand(id);
}

void AklStacker::Shutdown()
{
	LOGINFO("AklStacker::Shutdown | {}", StrategyId());
	AlgoBaseV3::Shutdown();
#if !AKL_TEST_MODE
	statusThreadRunning.store(false, std::memory_order_release);
	statusThread.join();
#endif
}

void AklStacker::Restart()
{
	LOGINFO("AklStacker::Restart");
	AlgoBaseV3::Restart();
}

void AklStacker::CancelAllOrders()
{
	LOGINFO("AklStacker::CancelAllOrders | {}", StrategyId());
	AlgoBaseV3::CancelAllOrders();
}

void AklStacker::OrderReceived(KTN::Order &incoming, int index)
{
	LOGINFO("AklStacker::OrderReceived: index={} | order={}", index, incoming);

	if (incoming.action == "NEW")
	{
		LOGINFO("AklStacker does not accept new orders"); // TODO check this
	}
	else if (incoming.action == "MOD")
	{
		if (likely(IsReady()))
		{
			std::lock_guard<std::mutex> lock(mutex);
			KTN::OrderPod ord;
			if (_Ords.GetByExchId(ord, incoming.exchordid))
			{
				Order *o = instr->GetOrder(ord.orderReqId);
				if (likely(o))
				{
					const Quantity incomingQty = Quantity(incoming.leavesqty) + Quantity(incoming.fillqty);
					if (o->targetQuantity != incomingQty && incomingQty != Quantity(0))
					{
						instr->Modify(o, incomingQty);
					}
					else
					{
						const Price incomingPx = Price::FromUnshifted(incoming.price);
						if (incomingPx != o->price)
						{
							o->price = incomingPx;
							instr->Modify(o, incomingPx);
							// TODO do I want to allow the user to modify price of strategy orders?
						}
						else
						{
							LOGWARN("AklStacker {} found no change in order", StrategyId());
						}
					}
				}
				else
				{
					LOGWARN("AklStacker {} failed to find internal order for reqId {}", StrategyId(), ord.orderReqId);
				}
			}
			else
			{
				LOGWARN("AklStracker {} failed to find OrderPod for exchId {}", StrategyId(), incoming.exchordid);
			}
		}
		else
		{
			LOGWARN("AklStacker {} is not initialized", StrategyId());
		}
	}
	else if (incoming.action == "CXL")
	{
		if (likely(IsReady()))
		{
			std::lock_guard<std::mutex> lock(mutex);
			KTN::OrderPod ord;
			if (_Ords.GetByExchId(ord, incoming.exchordid))
			{
				Order *o = instr->GetOrder(ord.orderReqId);
				if (likely(o))
				{
					instr->Cancel(o);
				}
				else
				{
					LOGWARN("AklStacker {} failed to find internal order for reqId {}", StrategyId(), ord.orderReqId);
				}
			}
			else
			{
				LOGWARN("AklStracker {} failed to find OrderPod for exchId {}", StrategyId(), incoming.exchordid);
			}
		}
		else
		{
			LOGWARN("AklStacker {} is not initialized", StrategyId());
		}
	}
	else
	{
		LOGWARN("AklStacker::OrderReceived: unknown action {}", incoming.action);
	}
}

void AklStacker::OrderMinUpdate(KTN::OrderPod &ord)
{
	LOGINFO("AklStacker::OrderMinUpdate: {}", ord);
	AlgoBaseV3::OrderMinUpdate(ord);
}

void AklStacker::onMktDataBidAsk(const KT01::DataStructures::MarketDepth::DepthBook &md)
{
#if !AKL_TEST_MODE
	time.SetTime();
#endif

	if (unlikely(md.Index != 0 && md.Index != 1))
	{
		if (_verboseLog) {
			LOGWARN("AklStacker::onMktDataBidAsk | {} | Received market data for unknown index {} | secId={}", StrategyId(), md.Index, md.SecId);
		}
		return;
	}

	if (likely(IsReady()))
	{
		const KT01::DataStructures::MarketDepth::BookSecurityStatus oldSecStatus = book[md.Index].MarketState();
		bool result = false;
		{
			std::lock_guard<std::mutex> lock(mutex);
			book[md.Index].Process(md);
			instr->GetStrategyExecution().ReceiveTime = md.recvTime;

			if (unlikely(book[md.Index].IsPreopen()))
			{
				if (book[md.Index].HasBid() && book[md.Index].HasAsk() && book[md.Index].BestBid() >= book[md.Index].BestAsk())
				{
					book[md.Index].CleanCross();
				}
			}

			if (md.Index == 0 && oldSecStatus != book[md.Index].MarketState())
			{
				instr->OnMarketStateChange(oldSecStatus, book[md.Index].MarketState());
			}

			result = Recalculate();
		}

		if (unlikely(oldSecStatus != book[md.Index].MarketState()))
		{
			LOGINFO("AklStacker::onMktDataBidAsk: Market state for {} changed from {} to {}",
				book[md.Index].Symbol(),
				static_cast<uint16_t>(oldSecStatus),
				static_cast<uint16_t>(book[md.Index].MarketState()));
			result = true;
		}

		if (result)
		{
			if (book[md.Index].IsOpen() || book[md.Index].IsPreopen())
			{
				const bool validBook = book[md.Index].ValidateBook();
				if (unlikely(!validBook))
				{
					LOGWARN("AklStacker {} | Potentially invalid book", StrategyId());
				}
				book[md.Index].LogDepth();
			}
		}
		else
		{
			if (book[md.Index].IsOpen() || book[md.Index].IsPreopen())
			{
				const bool validBook = book[md.Index].ValidateBook();
				if (unlikely(!validBook) && time.Now() >= lastInvalidBookLog + invalidBookLogInterval)
				{
					LOGWARN("AklStacker {} | Potentially invalid book", StrategyId());
					book[md.Index].LogDepth();
					lastInvalidBookLog = time.Now();
				}
			}
		}

		book[md.Index].UpdatePrevious();
		instr->GetStrategyExecution().ReceiveTime = 0;
	}
	else
	{
		std::lock_guard<std::mutex> lock(mutex);
		book[md.Index].Process(md);
	}
}

void AklStacker::onMktDataTrade(const KT01::DataStructures::MarketDepth::DepthBook &md)
{
#if !AKL_TEST_MODE
	time.SetTime();
#endif

	if (unlikely(md.Index != 0 && md.Index != 1))
	{
		if (_verboseLog) {
			LOGWARN("AklStacker::onMktDataTrade | {} | Received trade for unknown index {} | secId={}", StrategyId(), md.Index, md.SecId);
		}
		return;
	}

	// Only process trades for the quote product
	if (md.Index == 0)
	{
		const KTN::ORD::KOrderSide::Enum aggressingSide = CmeSideToKtnSide(static_cast<KTN::Mdp3::EnumAggressorSide>(md.LastAgressor));
		const Quantity tradeQty = Quantity(md.LastSize);
		const Price tradePx = Price::FromUnshifted(md.Last);

		if (likely(IsReady()))
		{
			bool result = false;
			{
				std::lock_guard<std::mutex> lock(mutex);
				instr->GetStrategyExecution().ReceiveTime = md.recvTime;

				book[md.Index].Process(aggressingSide, tradeQty, tradePx, md.RptSeq, md.recvTime);

				if (md.Index == 0)
				{
					result = instr->OnTrade(aggressingSide, tradeQty, tradePx);
				}

				book[md.Index].UpdatePrevious();
				instr->GetStrategyExecution().ReceiveTime = 0;
			}

			if (result)
			{
				book[md.Index].LogDepth();
			}
		}
		else
		{
			book[md.Index].Process(aggressingSide, tradeQty, tradePx, md.RptSeq, md.recvTime);
		}
	}
}

void AklStacker::onMktDataSecurityStatus(
	int secid,
	KT01::DataStructures::MarketDepth::BookSecurityStatus secstatus,
	KT01::DataStructures::MarketDepth::BookSecurityTradingEvent event,
	const uint64_t recvTime)
{
#if !AKL_TEST_MODE
	time.SetTime();
#endif

	if (secid == _LEGS[0].secid)
	{
		const KT01::DataStructures::MarketDepth::BookSecurityStatus oldSecStatus = book[0].MarketState();
		const bool oldNoCancel = book[0].NoCancel();
		bool result = false;
		{
			std::lock_guard<std::mutex> lock(mutex);
			book[0].Process(secstatus, event, recvTime);

			if (oldSecStatus != book[0].MarketState())
			{
				if (likely(IsReady()))
				{
					instr->OnMarketStateChange(oldSecStatus, book[0].MarketState());
				}
			}

			result = Recalculate();
		}
		if (oldSecStatus != book[0].MarketState() || oldNoCancel != book[0].NoCancel())
		{
			LOGINFO("AklStacker::onMktDataSecurityStatus: Market state changed from {} to {} | NoCancel changed from {} to {}",
				static_cast<uint16_t>(oldSecStatus),
				static_cast<uint16_t>(book[0].MarketState()),
				static_cast<int>(oldNoCancel),
				static_cast<int>(book[0].NoCancel()));
		}
		if (result)
		{
			book[0].LogDepth();
		}
	}
	else if (spreaderSecId != 0 && secid == spreaderSecId)
	{
		const KT01::DataStructures::MarketDepth::BookSecurityStatus oldSecStatus = book[1].MarketState();
		const bool oldNoCancel = book[1].NoCancel();
		std::lock_guard<std::mutex> lock(mutex);
		book[1].Process(secstatus, event, recvTime);
		if (oldSecStatus != book[1].MarketState() || oldNoCancel != book[1].NoCancel())
		{
			LOGINFO("AklStacker::onMktDataSecurityStatus: Market state changed from {} to {} for second leg | NoCancel changed from {} to {}",
				static_cast<uint16_t>(oldSecStatus),
				static_cast<uint16_t>(book[1].MarketState()),
				static_cast<int>(oldNoCancel),
				static_cast<int>(book[1].NoCancel()));
		}
	}
	else
	{
		LOGWARN("AklStacker::onMktDataSecurityStatus: secid mismatch {} {} {}", _LEGS[0].secid, secid, spreaderSecId);
	}
}

void AklStacker::onAlgoMessage(const std::string &json)
{
	rapidjson::Document document;
	rapidjson::ParseResult result = document.Parse(json.c_str());

	if (!result)
	{
		std::string error = std::string("JSON parse error: ") + rapidjson::GetParseError_En(result.Code()) +
							" at offset " + std::to_string(result.Offset());
		LOGERROR("AklStacker::onAlgoMessage: {}", error);
	}

	int messageTypeInt = 0;
	std::string payload = "";
	bool hasId = true;

	for (rapidjson::GenericValue<rapidjson::UTF8<>>::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
	{
		if (itr->HasMember("Payload"))
		{
			payload = (*itr)["Payload"].GetString();
		}
		if (itr->HasMember("MessageType") && itr->operator[]("MessageType").IsInt())
		{
			messageTypeInt = (*itr)["MessageType"].GetInt();
		}

		rapidjson::Document payloadDoc;
		result = payloadDoc.Parse((*itr)["Payload"].GetString());
		if (!result)
		{
			std::string error = std::string("JSON parse error: ") + rapidjson::GetParseError_En(result.Code()) +
								" at offset " + std::to_string(result.Offset());
			LOGERROR("AklStacker::onAlgoMessage: {}", error);
		}

		if (!payloadDoc.HasMember("id"))
		{
			hasId = false;
		}
		else if (!payloadDoc["id"].IsString())
		{
			hasId = false;
		}
		else if (std::string(payloadDoc["id"].GetString()) != StrategyId())
		{
			LOGINFO("AklStacker::onAlgoMessage: Ignoring message bewcause payload id {} does not match algo id {}",
				payloadDoc["id"].GetString(), StrategyId());
			return;
		}
	}

	const MessageType messageType = static_cast<MessageType>(messageTypeInt);
	if (!hasId)
	{
		switch (messageType)
		{
			case MessageType::PRORATA_PROFILE_DELETE:
			case MessageType::PRORATA_PROFILE_UPDATE:
			case MessageType::HEARTBEAT:
				break;
			default:
				LOGINFO("AklStacker::onAlgoMessage {}: Ignoring message with invalid ID", StrategyId());
				return;
		}
	}

	switch (static_cast<MessageType>(messageType))
	{
		case MessageType::STRATEGY_STATUS:
			if (likely(IsReady()))
			{
				StrategyStatusMessage msg = StrategyStatusMessage::DeserializeFromString(payload);
				bool queueUpdate = false;
				const Status oldStatus = status;
				{
					std::lock_guard<std::mutex> lock(mutex);

					processStatusUpdate(msg.status);
					switch (oldStatus)
					{
						case Status::PAUSED:
						{
							if (status == Status::RUNNING)
							{
								doStart();
								queueUpdate = true;
							}
							break;
						}
						case  Status::RUNNING:
						{
							if (status == Status::PAUSED)
							{
								doStop();
								queueUpdate = true;
							}
							break;
						}
						default:
						{
							if (status == Status::RUNNING)
							{
								doStart();
								queueUpdate = true;
							}
							else if (status == Status::PAUSED)
							{
								doStop();
								queueUpdate = true;
							}
							break;
						}
					}
					queueUpdate |= msg.status != status;
				}
				if (queueUpdate)
				{
					QueueStrategyStatusUpdate();
				}
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		case MessageType::STACKER_REQUOTE:
			if (likely(IsReady()))
			{
				std::lock_guard<std::mutex> lock(mutex);
				Recalculate(true);
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			
			break;
		case MessageType::STACKER_ADOPT:
			if (likely(IsReady()))
			{
				StackerAdoptMessage msg = StackerAdoptMessage::DeserializeFromString(payload);
				if (msg.stackIndex == 0 || msg.stackIndex == 1)
				{
					std::lock_guard<std::mutex> lock(mutex);
					instr->Adopt(true, nullptr, 0, msg.stackIndex);
				}
				else
				{
					LOGWARN("AklStacker::onAlgoMessage: invalid stack index {}", msg.stackIndex);
				}
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		case MessageType::STACKER_PRICE_ABANDON:
			if (likely(IsReady()))
			{
				StackerPriceAbandonMessage msg = StackerPriceAbandonMessage::DeserializeFromString(payload);
				{
					std::lock_guard<std::mutex> lock(mutex);
					instr->Handle(msg);
				}
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		case MessageType::STACKER_PRICE_ADOPT:
			if (likely(IsReady()))
			{
				StackerPriceAdoptMessage msg = StackerPriceAdoptMessage::DeserializeFromString(payload);
				{
					std::lock_guard<std::mutex> lock(mutex);
					instr->Handle(msg);
				}
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		case MessageType::STACKER_POSITION_UPDATE: // TODO queue the position up and process it during the JSON update, so that they're not processed as separate updates
			if (likely(IsReady()))
			{
				StackerPositionUpdateMessage msg = StackerPositionUpdateMessage::DeserializeFromString(payload);
				{
					std::lock_guard<std::mutex> lock(mutex);
					instr->Handle(msg);
				}
				QueuePositionUpdate();
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		case MessageType::STACKER_CLEAR_POSITION:
			if (likely(IsReady()))
			{
				{
					std::lock_guard<std::mutex> lock(mutex);
					instr->ClearPosition();
					instr->Recalculate(false);
				}
				QueuePositionUpdate();
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		case MessageType::STOP_UPDATE:
			if (likely(IsReady()))
			{
				StopUpdateMessage msg = StopUpdateMessage::DeserializeFromString(payload);
				{
					std::lock_guard<std::mutex> lock(mutex);
					instr->Handle(msg);
					instr->Recalculate(false);
				}
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		case MessageType::STOP_SUBSCRIBE:
			if (likely(IsReady()))
			{
				std::lock_guard<std::mutex> lock(mutex);
				instr->GetStopHandler().HandleStopSubscribe();
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		case MessageType::STACKER_PRORATA_ORDER_REQ:
			if (likely(IsReady()))
			{
				QueueProrataOrderUpdate();
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		case MessageType::STACKER_PRORATA_ORDER_ADOPT:
		{
			StackerProrataOrderAdoptMessage msg = StackerProrataOrderAdoptMessage::DeserializeFromString(payload);
			if (msg.index == 0)
			{
				pendingProrataAdoptList.clear();
			}
			pendingProrataAdoptList.push_back(msg);
			if (static_cast<int>(pendingProrataAdoptList.size()) == msg.count)
			{
				if (likely(IsReady()))
				{
					{
						std::lock_guard<std::mutex> lock(mutex);
						instr->Handle(pendingProrataAdoptList);
					}
					pendingProrataAdoptList.clear();
				}
				else
				{
					LOGWARN("AklStacker::onAlgoMessage: not ready");
				}
			}
			break;
		}
		case MessageType::STACKER_PRORATA_ORDER_ABANDON:
		{
			StackerProrataOrderAbandonMessage msg = StackerProrataOrderAbandonMessage::DeserializeFromString(payload);
			if (msg.index == 0)
			{
				pendingProrataAbandonList.clear();
			}
			pendingProrataAbandonList.push_back(msg);
			if (static_cast<int>(pendingProrataAbandonList.size()) == msg.count)
			{
				if (likely(IsReady()))
				{
					{
						std::lock_guard<std::mutex> lock(mutex);
						instr->Handle(pendingProrataAbandonList);
					}
					pendingProrataAbandonList.clear();
				}
				else
				{
					LOGWARN("AklStacker::onAlgoMessage: not ready");
				}
			}
			break;
		}
		case MessageType::PRORATA_PROFILE_UPDATE:
		{
			ProrataProfileUpdateMessage msg = ProrataProfileUpdateMessage::DeserializeFromString(payload);
			if (likely(msg.name[0] != '\0'))
			{
				{
					std::lock_guard<std::mutex> lock(mutex);
					prorataMap[msg.name] = msg;
				}

				if (likely(IsReady()))
				{
					bool doRecalc = false;
					if (msg.name == settings.quoteInstrumentSettings.bidSettings.prorataProfile)
					{
						{
							std::lock_guard<std::mutex> lock(mutex);
							instr->OnProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::BUY>(msg);
						}
						LOGINFO("AklStacker {} | Updating bid prorata profile {}", StrategyId(), msg.name);
						doRecalc = true;
					}
					if (msg.name == settings.quoteInstrumentSettings.askSettings.prorataProfile)
					{
						{
							std::lock_guard<std::mutex> lock(mutex);
							instr->OnProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::SELL>(msg);
						}
						LOGINFO("AklStacker {} | Updating ask prorata profile {}", StrategyId(), msg.name);
						doRecalc = true;
					}
					if (msg.name == settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile)
					{
						{
							std::lock_guard<std::mutex> lock(mutex);
							instr->OnAltProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::BUY>(msg);
						}
						LOGINFO("AklStacker {} | Updating bid alt prorata profile {}", StrategyId(), msg.name);
						doRecalc = true;
					}
					if (msg.name == settings.quoteInstrumentSettings.askSettings.alternateProrataProfile)
					{
						{
							std::lock_guard<std::mutex> lock(mutex);
							instr->OnAltProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::SELL>(msg);
						}
						LOGINFO("AklStacker {} | Updating ask alt prorata profile {}", StrategyId(), msg.name);
						doRecalc = true; // TODO is a recalc needed for alt profile updates?
					}
					if (doRecalc)
					{
						std::lock_guard<std::mutex> lock(mutex);
						instr->Recalculate(false);
					}
				}
				LOGINFO("AklStacker {} | Received update for prorata profile {}", StrategyId(), msg.name);
			}
			else
			{
				LOGWARN("AklStacker {} received prorata profile with blank name",	StrategyId());
			}
			break;
		}
		case MessageType::PRORATA_PROFILE_DELETE:
		{
			ProrataProfileDeleteMessage msg = ProrataProfileDeleteMessage::DeserializeFromString(payload);
			if (likely(msg.name[0] != '\0'))
			{
				prorataMap.erase(msg.name);
				if (likely(IsReady()))
				{
					bool doRecalc = false;
					if (msg.name == settings.quoteInstrumentSettings.bidSettings.prorataProfile)
					{
						{
							std::lock_guard<std::mutex> lock(mutex);
							instr->ClearProrataProfile<KTN::ORD::KOrderSide::Enum::BUY>();
							settings.quoteInstrumentSettings.bidSettings.prorataProfile.clear();
						}
						LOGINFO("AklStacker {} | Clearing bid prorata profile {}", StrategyId(), msg.name);
						doRecalc = true;
					}
					if (msg.name == settings.quoteInstrumentSettings.askSettings.prorataProfile)
					{
						{
							std::lock_guard<std::mutex> lock(mutex);
							instr->ClearProrataProfile<KTN::ORD::KOrderSide::Enum::SELL>();
							settings.quoteInstrumentSettings.askSettings.prorataProfile.clear();
						}
						LOGINFO("AklStacker {} | Clearing ask prorata profile {}", StrategyId(), msg.name);
						doRecalc = true;
					}
					if (msg.name == settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile)
					{
						{
							std::lock_guard<std::mutex> lock(mutex);
							instr->ClearAltProrataProfile<KTN::ORD::KOrderSide::Enum::BUY>();
							settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile.clear();
						}
						LOGINFO("AklStacker {} | Clearing bid alt prorata profile {}", StrategyId(), msg.name);
						doRecalc = true;
					}
					if (msg.name == settings.quoteInstrumentSettings.askSettings.alternateProrataProfile)
					{
						{
							std::lock_guard<std::mutex> lock(mutex);
							instr->ClearAltProrataProfile<KTN::ORD::KOrderSide::Enum::SELL>();
							settings.quoteInstrumentSettings.askSettings.alternateProrataProfile.clear();
						}
						LOGINFO("AklStacker {} | Clearing ask alt prorata profile {}", StrategyId(), msg.name);
						doRecalc = true;// TODO is a recalc needed for alt profile updates?
					}
					if (doRecalc)
					{
						std::lock_guard<std::mutex> lock(mutex);
						instr->Recalculate(false);
						QueueSettingsUpdate();
					}
				}
				LOGINFO("AklStacker {} | Received delete for prorata profile {}", StrategyId(), msg.name);
			}
			else
			{
				LOGWARN("AklStacker {} received prorata profile with blank name", StrategyId());
			}
			break;
		}
		case MessageType::STACKER_PRORATA_CONFIG:
		{
			if (likely(IsReady()))
			{
				StackerProrataConfigMessage msg = StackerProrataConfigMessage::DeserializeFromString(payload);
				if (settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile != msg.alternateBidProrataProfile
					|| settings.quoteInstrumentSettings.askSettings.alternateProrataProfile != msg.alternateAskProrataProfile)
				{
					std::lock_guard<std::mutex> lock(mutex);
					if (settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile != msg.alternateBidProrataProfile)
					{
						ProrataProfileMapType::const_iterator it = prorataMap.find(msg.alternateBidProrataProfile);
						if (likely(it != prorataMap.end()))
						{
							instr->OnAltProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::BUY>(it->second);
						}
						else
						{
							instr->ClearAltProrataProfile<KTN::ORD::KOrderSide::Enum::BUY>();
							LOGWARN("AklStacker {} | Failed to find bid alt prorata profile {}. Clearing.",
								StrategyId(), msg.alternateBidProrataProfile);
						}
						LOGINFO("AklStacker {} | bidAltProrataProfile changed from {} to {}",
							StrategyId(), settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile, msg.alternateBidProrataProfile);
						settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile = msg.alternateBidProrataProfile;
					}
					if (settings.quoteInstrumentSettings.askSettings.alternateProrataProfile != msg.alternateAskProrataProfile)
					{
						ProrataProfileMapType::const_iterator it = prorataMap.find(msg.alternateAskProrataProfile);
						if (likely(it != prorataMap.end()))
						{
							instr->OnAltProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::SELL>(it->second);
						}
						else
						{
							instr->ClearAltProrataProfile<KTN::ORD::KOrderSide::Enum::SELL>();
							LOGWARN("AklStacker {} | Failed to find ask alt prorata profile {}. Clearing.",
								StrategyId(), msg.alternateAskProrataProfile);
						}
						LOGINFO("AklStacker {} | askAltProrataProfile changed from {} to {}",
							StrategyId(), settings.quoteInstrumentSettings.askSettings.alternateProrataProfile, msg.alternateAskProrataProfile);
						settings.quoteInstrumentSettings.askSettings.alternateProrataProfile = msg.alternateAskProrataProfile;
					}
					QueueSettingsUpdate();
				}
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		}
		case MessageType::STRATEGY_CONFIG_REQ:
			if (likely(IsReady()))
			{
				QueueStrategyStatusUpdate();
				QueueStackTargetsUpdate();
				QueuePositionUpdate();
				QueueProrataSettingsUpdate();
				QueueStackInfoUpdate();
				QueueSpreaderStrikeUpdate();
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		case MessageType::STACKER_SPREADER_UPDATE:
			if (likely(IsReady()))
			{
				StackerSpreaderUpdateMessage msg = StackerSpreaderUpdateMessage::DeserializeFromString(payload);
				{
					std::lock_guard<std::mutex> lock(mutex);
					instr->Handle(msg); // Update message to UI is queued inside here, if a strike changed
					instr->Recalculate(false);
				}
			}
			else
			{
				LOGWARN("AklStacker::onAlgoMessage: not ready");
			}
			break;
		default:
			LOGWARN("AklStacker::onAlgoMessage: Unknown message type {}", messageTypeInt);
			break;
	}
}


void AklStacker::onClientConnected(const std::string &clientId)
{
	if (likely(IsReady()))
	{
		QueueStrategyStatusUpdate();
		QueueStackTargetsUpdate();
		QueuePositionUpdate();
		QueueProrataSettingsUpdate();
		QueueSpreaderStrikeUpdate();
		QueueStackInfoUpdate();
		LOGINFO("AklStacker {} | Queueing updates for client {}", StrategyId(), clientId);
	}
}

void AklStacker::onClientDisconnected(const std::string &clientId)
{
	if (likely(IsReady()))
	{
		instr->GetStopHandler().HandleClientDisconnect(clientId);
		LOGINFO("AklStacker {} | Handling client disconnect for {}", StrategyId(), clientId);
	}
}

void AklStacker::onKillswitchUpdate(const bool state)
{
	if (likely(IsReady()))
	{
		instr->GetStrategyExecution().onKillswitchUpdate(state);
		if (state)
		{
			if (IsRunning())
			{
				processStatusUpdate(Status::PAUSED);
				doStop();
				QueueStrategyStatusUpdate();
				LOGINFO("AklStacker::onKillswitchUpdate: Killswitch ON, pausing strategy");

				std::ostringstream ss;
				ss << "Strategy " << settings.meta.UniqueId << " for " << symbol << " has been paused due to killswitch";
				notifier->Notify(ss.str(), KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
			}
			instr->GetStopHandler().CancelAll();
		}
	}
	else
	{
		LOGWARN("AklStacker::onKillswitchUpdate: strategy not initialized");
	}
}

void AklStacker::DisableRisk()
{
	if (likely(IsReady()))
	{
		instr->GetStrategyExecution().onKillswitchUpdate(true);
	}
}

void AklStacker::TryStop()
{
	if (likely(IsReady()))
	{
		if (IsRunning())
		{
			if (mutex.try_lock())
			{
				processStatusUpdate(Status::PAUSED);
				doStop();
				QueueStrategyStatusUpdate();
				mutex.unlock();

				std::ostringstream ss;
				ss << "Strategy " << StrategyId() << " for " << symbol << " has been paused due to killswitch";
				notifier->Notify(ss.str(), KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
			}
			else
			{
				LOGWARN("AklStacker::TryStop: Failed to acquire mutex for strategy {}", StrategyId());
			}
		}
		else
		{
			LOGWARN("AklStacker::TryStop: Strategy {} is not running", StrategyId());
		}
	}
	else
	{
		LOGERROR("AklStacker::TryStop: strategy not initialized");
	}
}

bool AklStacker::Recalculate(const bool requote)
{
	if (likely(IsReady()))
	{
		return instr->Recalculate(requote);
	}
	return false;
}

void AklStacker::OnInternalCancel(Order *o)
{
	if (likely(IsReady()))
	{
		instr->OnInternalCancel(o);
	}
	else
	{
		LOGWARN("AklStacker::OnInternalCancel: not ready");
	}
}

void AklStacker::OnInternalReject(Order *o)
{
	if (likely(IsReady()))
	{
		instr->OnInternalReject(o);
	}
	else
	{
		LOGWARN("AklStacker::OnInternalReject: not ready");
	}
}

void AklStacker::PublishStackInfo()
{
	StackInfoMessage msg;
	msg.id = settings.meta.UniqueId;
	msg.index = 0;

	StackInfoMessage msg2;
	msg2.id = settings.meta.UniqueId;
	msg2.index = 1;

	{
		std::lock_guard<std::mutex> lock(mutex);
		instr->GetStackInfo(msg, 0);

		if (settings.quoteInstrumentSettings.secondStackEnabled)
		{
			instr->GetStackInfo(msg2, 1);
		}
	}

	Publish(msg);
	if (settings.quoteInstrumentSettings.secondStackEnabled)
	{
		Publish(msg2);
	}
}

void AklStacker::PublishStackTargets()
{
	StackTargetsMessage msg;
	msg.id = settings.meta.UniqueId;

	{
		std::lock_guard<std::mutex> lock(mutex);
		msg.askTargetPrice = instr->GetPricingModel().AskLean() ? instr->GetPricingModel().AskLean()->AsShifted() : Price::Max().AsShifted();
		msg.bidTargetPrice = instr->GetPricingModel().BidLean() ? instr->GetPricingModel().BidLean()->AsShifted() : Price::Min().AsShifted();
		msg.askTargetPrice2 = instr->GetPricingModel(1).AskLean() ? instr->GetPricingModel(1).AskLean()->AsShifted() : Price::Max().AsShifted();
		msg.bidTargetPrice2 = instr->GetPricingModel(1).BidLean() ? instr->GetPricingModel(1).BidLean()->AsShifted() : Price::Min().AsShifted();
	}

	Publish(msg);
}

void AklStacker::PublishRiskInfo()
{
	StackerRiskInfoMessage msg;
	msg.id = settings.meta.UniqueId;

	const InstrumentRisk &risk = instr->GetRisk();

	{
		std::lock_guard<std::mutex> lock(mutex);
		msg.internalRisk.bidWorking = risk.GetWorkingQty<KTN::ORD::KOrderSide::Enum::BUY>().value();
		msg.internalRisk.askWorking = risk.GetWorkingQty<KTN::ORD::KOrderSide::Enum::SELL>().value();
		msg.internalRisk.bidFilled = risk.GetFilledQty<KTN::ORD::KOrderSide::Enum::BUY>().value();
		msg.internalRisk.askFilled = risk.GetFilledQty<KTN::ORD::KOrderSide::Enum::SELL>().value();
		msg.internalRisk.bidPositionLimit = risk.GetPositionLimit().value();
		msg.internalRisk.askPositionLimit = risk.GetPositionLimit().value();
	}

	Publish(msg);
}

void AklStacker::PublishPosition()
{
	StackerPositionUpdateMessage msg;
	msg.id = settings.meta.UniqueId;

	{
		std::lock_guard<std::mutex> lock(mutex);
		msg.position = instr->GetPosition().value();
	}

	Publish(msg);
}

void AklStacker::PublishSettings()
{

	AklStackerParams settingsCopy;
	{
		std::lock_guard<std::mutex> lock(mutex);
		settingsCopy = settings;
	}
	Publish(StackerConfigMessage::FromParams(settingsCopy));
	Publish(StackerPriceBandsMessage::FromParams(settingsCopy));
	Publish(StackerProrataConfigMessage::FromParams(settingsCopy));
	Publish(StopSettingsMessage::FromParams(settingsCopy));
	Publish(settingsCopy);
}

void AklStacker::PublishProrataSettings()
{
	StackerProrataConfigMessage msg;

	{
		std::lock_guard<std::mutex> lock(mutex);
		msg = StackerProrataConfigMessage::FromParams(settings);
	}

	Publish(msg);
}

void AklStacker::PublishStatus()
{
	Publish(StrategyStatusMessage { settings.meta.UniqueId, status });
}

void AklStacker::PublishProrataOrder(const uint64_t reqId, const Quantity workingQty, const int piq, const KTN::ORD::KOrderSide::Enum side, const int8_t index, const int count)
{
	StackerProrataOrderResponseMessage msg;
	msg.id = settings.meta.UniqueId;
	msg.oid = reqId;
	msg.workingQuantity = workingQty.value();
	msg.piq = piq;
	msg.side = side;
	msg.index = index;
	msg.count = count;
	Publish(msg);
}

void AklStacker::PublishSpreaderUpdate()
{
	StackerSpreaderUpdateMessage msg;
	msg.id = settings.meta.UniqueId;

	{
		std::lock_guard<std::mutex> lock(mutex);
		const SpreaderPricingModel &spreaderModel = instr->GetSpreaderPricingModel();
		msg.askStrike = spreaderModel.HasStrikePrice<KTN::ORD::KOrderSide::Enum::SELL>() ? spreaderModel.StrikePrice<KTN::ORD::KOrderSide::Enum::SELL>()->AsShifted() : Price::Max().AsShifted();
		msg.bidStrike = spreaderModel.HasStrikePrice<KTN::ORD::KOrderSide::Enum::BUY>() ? spreaderModel.StrikePrice<KTN::ORD::KOrderSide::Enum::BUY>()->AsShifted() : Price::Min().AsShifted();
	}

	Publish(msg);
}

void AklStacker::OnInternalProrataProfileUpdate(const ProrataProfileUpdateMessage &msg)
{
	ProrataProfileMapType::iterator it = prorataMap.find(msg.name);
	if (likely(it != prorataMap.end()))
	{
		it->second = msg;
		pendingInternalProrataProfileUpdates.push(msg);
		//ProrataProfileManager::instance().OnInternalProfileUpdate(msg);
	}
	else
	{
		LOGWARN("Failed to find prorata profile {} for internal update", msg.name);
	}
}

std::optional<ProrataProfileUpdateMessage> AklStacker::GetProrataProfile(const std::string &name) const
{
	ProrataProfileMapType::const_iterator it = prorataMap.find(name);
	if (it != prorataMap.end())
	{
		return it->second;
	}
	else
	{
		return std::nullopt;
	}
}

void AklStacker::ProcessProrataProfileUpdateFromOtherStrategy(const ProrataProfileUpdateMessage &msg)
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		prorataMap[msg.name] = msg;
	}

	if (likely(IsReady()))
	{
		if (msg.name == settings.quoteInstrumentSettings.bidSettings.prorataProfile)
		{
			{
				std::lock_guard<std::mutex> lock(mutex);
				instr->OnProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::BUY>(msg);
			}
			LOGINFO("AklStacker {} | Updating bid prorata profile {}", StrategyId(), msg.name);
		}
		if (msg.name == settings.quoteInstrumentSettings.askSettings.prorataProfile)
		{
			{
				std::lock_guard<std::mutex> lock(mutex);
				instr->OnProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::SELL>(msg);
			}
			LOGINFO("AklStacker {} | Updating ask prorata profile {}", StrategyId(), msg.name);
		}
		if (msg.name == settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile)
		{
			{
				std::lock_guard<std::mutex> lock(mutex);
				instr->OnAltProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::BUY>(msg);
			}
			LOGINFO("AklStacker {} | Updating bid alt prorata profile {}", StrategyId(), msg.name);
		}
		if (msg.name == settings.quoteInstrumentSettings.askSettings.alternateProrataProfile)
		{
			{
				std::lock_guard<std::mutex> lock(mutex);
				instr->OnAltProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::SELL>(msg);
			}
			LOGINFO("AklStacker {} | Updating ask alt prorata profile {}", StrategyId(), msg.name);
		}
		// The only change should be additional hedge, so this shouldn't need a recalculation
	}
}

void AklStacker::runStatusThread()
{
	//int secs = 2 * 1000 * 1000;
	//_STATUS_ACTIVE.store(true, std::memory_order_relaxed);
#if !AKL_TEST_MODE
	statusThreadRunning.store(true, std::memory_order_relaxed);
	while(statusThreadRunning.load(std::memory_order_relaxed))//_STATUS_ACTIVE.load(std::memory_order_relaxed) == true)
#endif
	{
		if (likely(IsReady()))
		{
			time.SetTime();

			const Timestamp now = time.Now();

			if (now >= lastReportingTime + reportingInterval)
			{
				ostringstream oss;
				//for (int i = 0; i < _cp.LegCount; i++)
				{
					//instrument& leg = _cp.Legs[i];
					//DepthBook md = GetQuote(leg);

					//int bq = _Ords.GetSideExposure(leg.secid, KOrderSide::BUY, KOrderAlgoTrigger::ALGO_LEG);
					//int sq = _Ords.GetSideExposure(leg.secid, KOrderSide::SELL, KOrderAlgoTrigger::ALGO_LEG);

					oss << symbol << ": TOT=" << 0 <<" OPEN B=" << 0 << " S=" << 0;

				}
				AddReportingMsg(oss.str(), "INFO");
				lastReportingTime = now;
			}

			checkTimers(now);
		}

#if !AKL_TEST_MODE
		usleep(25000);
#endif
	}
}

void AklStacker::checkTimers(const Timestamp now)
{
	if (now >= lastTimerCheckTime + timerCheckInterval)
	{
		if (likely(IsReady()))
		{
			std::lock_guard<std::mutex> lock(mutex);
			const bool result = instr->CheckTimers(now);
			if (result)
			{
				instr->Recalculate(false);
			}
		}
		lastTimerCheckTime = now;
	}
	if (now >= lastStrategyStatusUpdate + strategyStatusUpdateInterval)
	{
		if (strategyStatusUpdateNeeded.load(std::memory_order_acquire))
		{
			PublishStatus();
			strategyStatusUpdateNeeded.store(false, std::memory_order_release);
		}
		lastStrategyStatusUpdate = now;
	}
	if (stackInfoUpdateNeeded && now >= lastStackInfoSendTime + stackInfoInterval)
	{
		PublishStackInfo();
		lastStackInfoSendTime = now;
		stackInfoUpdateNeeded = false;
	}
	if (stackTargetUpdateNeeded && now >= lastStackTargetSendTime + stackTargetInterval)
	{
		PublishStackTargets();
		lastStackTargetSendTime = now;
		stackTargetUpdateNeeded = false;
	}
	if (now >= lastRiskInfoSendTime + riskInfoInterval)
	{
		PublishRiskInfo();
		lastRiskInfoSendTime = now;
	}
	if (now >= lastSettingsUpdateTime + settingsUpdateInterval)
	{
		if (settingsUpdateNeeded.load(std::memory_order_acquire))
		{
			settingsUpdateNeeded.store(false, std::memory_order_release);
			PublishSettings();
			lastSettingsUpdateTime = now;
			LOGINFO("AklStacker::runStatusThread: sending settings update");
			prorataSettingsUpdateNeeded.store(false, std::memory_order_release);
			lastProrataSettingsUpdateTime = now;
		}
	}
	if (now > lastProrataSettingsUpdateTime + prorataSettingsUpdateInterval)
	{
		if (!settingsUpdateNeeded.load(std::memory_order_acquire) && prorataSettingsUpdateNeeded.load(std::memory_order_acquire))
		{
			prorataSettingsUpdateNeeded.store(false, std::memory_order_release);
			PublishProrataSettings();
			lastProrataSettingsUpdateTime = now;
			LOGINFO("AklStacker::runStatusThread: sending prorata settings update");
		}
	}
	if (now >= lastPositionSendTime + positionUpdateInterval)
	{
		if (positionUpdateNeeded.load(std::memory_order_acquire))
		{
			positionUpdateNeeded.store(false, std::memory_order_release);
			PublishPosition();
			lastPositionSendTime = now;
			LOGINFO("AklStacker::runStatusThread: sending position update");
			const InstrumentRisk &risk = instr->GetRisk();

			Quantity bidFilled;
			Quantity askFilled;
			{
				std::lock_guard<std::mutex> lock(mutex);
				bidFilled = risk.GetFilledQty<KTN::ORD::KOrderSide::Enum::BUY>();
				askFilled = risk.GetFilledQty<KTN::ORD::KOrderSide::Enum::SELL>();
			}

			PositionStore::instance().Push(StrategyId(), bidFilled.value(), askFilled.value());
		}
	}
	if (now >= lastSpreaderUpdateTime + spreaderUpdateInterval)
	{
		if (spreaderUpdateNeeded.load(std::memory_order_acquire))
		{
			spreaderUpdateNeeded.store(false, std::memory_order_release);
			PublishSpreaderUpdate();
			lastSpreaderUpdateTime = now;
			LOGINFO("AklStacker::runStatusThread: sending spreader update");
		}
	}
	if (now >= lastProrataOrderUpdate + prorataOrderUpdateInterval)
	{
		if (prorataOrderUpdateNeeded.load(std::memory_order_acquire))
		{
			prorataOrderUpdateNeeded.store(false, std::memory_order_release);
			std::vector<Order *> prorataOrders;

			{
				std::lock_guard<std::mutex> lock(mutex);
				instr->GetProrataOrders(prorataOrders);
			}

			if (!prorataOrders.empty())
			{
				std::sort(prorataOrders.begin(), prorataOrders.end(), [](const Order *a, const Order *b)
				{
					return a->priorityTransactTime < b->priorityTransactTime;
				});
				const int count = static_cast<int>(prorataOrders.size());
				int bidPriority = 1;
				int askPriority = 1;
				for (int i = 0; i < count; ++i)
				{
					const Order *o = prorataOrders[i];
					switch (o->side)
					{
						case KTN::ORD::KOrderSide::Enum::BUY:
							PublishProrataOrder(o->cmeId, o->WorkingQuantity(), bidPriority++, o->side, i, count);
							break;
						case KTN::ORD::KOrderSide::Enum::SELL:
							PublishProrataOrder(o->cmeId, o->WorkingQuantity(), askPriority++, o->side, i, count);
							break;
						default:
							PublishProrataOrder(o->cmeId, o->WorkingQuantity(), std::numeric_limits<int>::max(), o->side, i, count);
							break;
					}
				}
			}
			else
			{
				PublishProrataOrder(0, Quantity(0), std::numeric_limits<int>::max(), KTN::ORD::KOrderSide::Enum::BUY, 0, 0);
			}

			lastProrataOrderUpdate = now;
			LOGINFO("AklStacker::runStatusThread: sending prorata order update");
		}
	}

	instr->CheckOrderPool();
	if (now >= lastQuoterCheckTime + quoterCheckInterval)
	{
		if (quoter.GetOrderQueueSize() < 10000)
		{
			quoter.FillQueue();
		}
		lastQuoterCheckTime = now;
	}
}

void AklStacker::doStart()
{
	if (likely(IsReady()))
	{
		instr->Start();
		QueueStackInfoUpdate();
	}
}

void AklStacker::doStop()
{
	if (likely(IsReady()))
	{
		instr->Stop();
		QueueStackInfoUpdate();
	}
}

void AklStacker::processStatusUpdate(const int newStatus)
{
	switch (newStatus)
	{
		case 0:
			if (status != Status::PAUSED)
			{
				status = Status::PAUSED;
				LOGINFO("AklStacker {} | Switched to PAUSED", StrategyId());
			}
			else
			{
				LOGINFO("AklStacker {} | Ignoring PAUSED status as it is already PAUSED", StrategyId());
			}
			break;
		case 1:
			if (status != Status::RUNNING)
			{
				if (!instr->GetRisk().IsKillswitchEnabled())
				{
					status = Status::RUNNING;
					LOGINFO("AklStacker {} | Switched to RUNNING", StrategyId());
				}
				else
				{
					LOGWARN("AklStacker {} | Ignoring RUNNING status as killswitch is enabled", StrategyId());
					std::ostringstream ss;
					ss << "Strategy " << StrategyId() << " for " << symbol << " cannot be started due to killswitch";
					notifier->Notify(ss.str(), KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
				}
			}
			else
			{
				LOGINFO("AklStacker {} | Ignoring RUNNING status as it is already PAUSED", StrategyId());
			}
			break;
		default:
			LOGINFO("AklStacker {} | Ignoring unknown status {} | Current status: {}", StrategyId(), newStatus, static_cast<int32_t>(status));
			break;
	}
}

void AklStacker::processBookUpdate(const KT01::DataStructures::MarketDepth::DepthBook &md, Book &book) const
{

}

template<>
void AklStacker::setProrataProfile<KTN::ORD::KOrderSide::Enum::BUY>(const ProrataProfileUpdateMessage &msg)
{
	instr->OnProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::BUY>(msg);
}

template<>
void AklStacker::setProrataProfile<KTN::ORD::KOrderSide::Enum::SELL>(const ProrataProfileUpdateMessage &msg)
{
	instr->OnProrataProfileUpdate<KTN::ORD::KOrderSide::Enum::SELL>(msg);
}

void AklStacker::initProrataProfiles()
{
	std::vector<stacker::ProrataProfileUpdateMessage> profiles;
	akl::ProrataProfileManager::instance().GetProfiles(profiles);
	if (!profiles.empty())
	{
		for (const ProrataProfileUpdateMessage &profile : profiles)
		{
			prorataMap[profile.name] = profile;
		}
	}
	LOGINFO("AklStacker {} | Loaded {} pro-rata profiles", StrategyId(), prorataMap.size());
}

void AklStacker::initStacks()
{
	instr->UpdateStackQuantities();
	instr->GetStack(0).GetSide<KTN::ORD::KOrderSide::Enum::BUY>().UpdateLength(settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length);
	instr->GetStack(0).GetSide<KTN::ORD::KOrderSide::Enum::SELL>().UpdateLength(settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length);
	instr->GetStack(1).GetSide<KTN::ORD::KOrderSide::Enum::BUY>().UpdateLength(settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length);
	instr->GetStack(1).GetSide<KTN::ORD::KOrderSide::Enum::SELL>().UpdateLength(settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length);
}

}

