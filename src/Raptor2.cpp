//============================================================================
// Name        	: Raptor2.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2021-24 Katana Financial
// Date			: April 27, 2023 4:49:28 PM
//============================================================================

#include <Raptor2.hpp>
#include <akl/PositionStore.hpp>
#include <exchsupport/cfe/mktdata/CfeMktDataHandler.hpp>
#include <exchsupport/eqt/mktdata/EquityMktData.hpp>

Raptor2::Raptor2(const string& settingsfile)://_qOrdsSend(512),_qHdgs(512),_qOrdHdgCombo(512),
	_MEASURE(true)
{
	_logsvc = new LogSvc("Raptor2", "Raptor");

	MenuDisplay();

	//this is legacy. might be removed.
	manageLinuxSignals();

	StartServices(settingsfile);
}

Raptor2::~Raptor2()
{}

void Raptor2::WaitForShutdown()
{
	waitUntilEnterKey();
	Stop();
}

void Raptor2::LogMe(std::string szMsg, LogLevel::Enum level , bool Persist)
{
//	LWT_LOG_ME(szMsg, "RAPTOR", iColor);
//	return;

	LogMsg msg = {};
	msg.msg = szMsg;
	msg.loglevel = level;
	_logsvc->LogMe(msg);
}

void Raptor2::StartServices(const string& settingsfile)
{
	_CBCNT = 0;

	_SETTINGSFILE = settingsfile;
	Settings settings = Settings(settingsfile);

	_SOURCE = settings.getString("Source");
	_ORG = settings.getString("Org");
	
	_SERVICES = {};
	_SOURCE = settings.getString("Source");

	LogMe("LOADING CME SECMASTER...");
	//CmeSecDef x = CmeSecMaster::instance().getSecDef(0);

	LogMe("GENERATING INITIAL ORDER NUMBERING QUEUES...");
	bool y = OrderNums::instance().isLoaded();

	LogMe("ORD ID COUNT=" + std::to_string(OrderNums::instance().OrdIdSize()));
	LogMe("REQ ID COUNT=" + std::to_string(OrderNums::instance().ReqIdSize()));

	_SERVICES.exchanges = settings.getBoolean("Services.Exchanges");
	_SERVICES.orders = settings.getBoolean("Services.Orders");
	_SERVICES.algos = settings.getBoolean("Services.Algos");

	if (_SERVICES.orders)
	{
		LogMe("Starting ZeroMQ Order Handler Service [OK]" + _SOURCE, LogLevel::OK);
		StartZmqOrderServer();
	}
	else
		LogMe("ORDERS SERVICE SETTING DISABLED!", LogLevel::ERROR);


	if (_SERVICES.algos)
	{
		LogMe("Starting Algo Service Broker [OK]", LogLevel::OK);
		_algos = new AlgoServiceBrokerV3(this);

		const tbb::concurrent_unordered_map<uint64_t, KTN::Order> &apiOrders = OrderRecoveryService::instance().GetApiOrders();
		for (tbb::concurrent_unordered_map<uint64_t, KTN::Order>::const_iterator it = apiOrders.begin(); it != apiOrders.end(); ++it)
		{
			KTN::Order ord = it->second;
			if (ord.OrdAlgoTrig != KOrderAlgoTrigger::ORDERBOOK)
			{
				OrderPod pod = {};
				OrderUtils::Transpose(pod, ord, ord.ordernum);
				_algos->onRecoveredOrder(pod);
			}

		}
	}
	else
		LogMe("ALGOS SERVICE SETTING DISABLED!", LogLevel::ERROR);

		
	if (_SERVICES.exchanges)
	{
		int raptor_queue_affinity;
		if (settings.hasKey("RaptorQueueAffinity"))
		{
			raptor_queue_affinity = settings.getInteger("RaptorQueueAffinity");
		}
		else
		{
			raptor_queue_affinity = 2;
		}
		if (settings.hasKey("SpinRaptorQueueThread"))
		{
			_spinRaptorThread.store(settings.getBoolean("SpinRaptorQueueThread"), std::memory_order_release);
		}
		_exchanges = new ExchangeManager(qExchOrdsToProcess, _SETTINGSFILE);
		CreateAndRun("RaptorQ",raptor_queue_affinity);
	}
	else
		LogMe("EXCHANGES SERVICE SETTING DISABLED!", LogLevel::ERROR);


	//KT01_LOG_OK("RAPTOR2", "STARTING CFE MARKET DATA HANDLER");
	//(new CfeMktDataHandler())->Start();
}

void Raptor2::Run()
{
	if (_spinRaptorThread.load(std::memory_order_acquire))
	{
		runRaptorQueueThread<true>();
	}
	else
	{
		runRaptorQueueThread<false>();
	}

	//_ACTIVE.store(true, std::memory_order_relaxed);

	// while(_ACTIVE.load(std::memory_order_acquire) == true)
	// {
	// 	while(!qExchOrdsToProcess.empty())
	// 	{
	// 		KTN::OrderPod ord = {};
	// 		while(qExchOrdsToProcess.try_pop(ord))
	// 		{
	// 			onExchOrdEvent(ord);
	// 		}
	// 	}

	// 	while (_EVENT_WAITING.load(std::memory_order_acquire) == true)
	// 	{
	// 		while(!_qAlgoCmds.empty())
	// 		{
	// 			Instruction inst = {};
	// 			if (_qAlgoCmds.try_pop(inst))
	// 			{
	// 				_exchanges->onCommand(inst);
	// 			}
	// 		}

	// 		_EVENT_WAITING.store(false, std::memory_order_relaxed);
	// 	}

	// 	std::this_thread::sleep_for(std::chrono::microseconds(1));
	// }

}

void Raptor2::StartExchanges()
{
	//Deprecated. We now start exchanges directly above.
}


void Raptor2::Stop()
{
	LogMe("STOPPING ALGOS!",LogLevel::ERROR);

	if (_SERVICES.algos)
	{
		usleep(500000);
		_algos->StopAllAlgos();
		usleep(500000);
	}

	LogMe("STOPPING ORDERS!",LogLevel::ERROR);
	if (_SERVICES.orders)
		_ordzmq->Stop();

	if (_SERVICES.exchanges)
	{
		Instruction inst = {};
		inst.command = ExchCmd::TERMINATE;
		_exchanges->onCommand(inst);
	}

	akl::PositionStore::instance().Stop();
	akl::StrategyManager::instance().Stop();
	KTN::notify::NotifierRest::instance().Stop();
	usleep(5'000'000);
	KT01::Core::Log::instance().shutdown();
	_exit(EXIT_SUCCESS);
}

void Raptor2::MenuDisplay()
{
	menu = new Menu(*this, GetMenuChoices(),"Menu"),
	menu->DisplayAndWait();
}

//Note: I moved this from external text file so we can just update this map
//when we update the menu choices and not worry about external file syncs.
std::map<int, std::string> Raptor2::GetMenuChoices() {
	return {
		{1, "Print Orders"},
		{2, "Run Tester (DEBUG)"},
		{33, "Reset Risk & Positions"},
		{4, "Clear All Orders & Reset Algos"},
		{5, "Cancel All Open Orders"},
		//{555, "Cancel All Orders for All Algos"},
		{6, "Execute Exchange Command"},
		//{666, "Execute Exchange Command (Alt)"},
		{7, "Print Algos"},

		{11, "Print Latencies"},
		{12, "Clear Latencies"},
		{22, "Print Message Ratios"},
		{19, "Stop All Algos"},
		{23, "Clear Stops on MM2"},
		{59, "Disable Slack"},
		{60, "Enable Slack"},
		{71, "Send Command to Algo"},
		{72, "Send Command to Algo"},
		{85, "Send Command to Algo"},
		{92, "CME iLink3 Session Status"},
		{93, "CME iLink3 Party Definition Request"},
		{94, "CME iLink3 Mass Order Status Request"},
		{95, "Toggle Measurement"},
		{96, "CME iLink3 Party List Request"},
		{97, "CME iLink3 Retransmit Request"},
		{200, "Start EQT Market Data Feed"},
		{210, "Force TCP Reconnect (CFE)"},
		{211, "Reset Reconnect Tracking (CFE)"},
		{350, "OSE Query Settlement (RQ62)"},
		{999, "Exit Program"}
	};
}

void Raptor2::onMenuChoice(int iKey)
{
	//MenuChoice(iKey);
	//_qMenuChoice.push(iKey);
	MenuChoice(iKey);
	_EVENT_WAITING.store(true, std::memory_order_relaxed);
}

void Raptor2::MenuChoice(int choice)//, ExchangeManager* exchanges)
{

	switch (choice)
	{
		case 1:
			PrintOrders();
			break;
		case 2:
#ifdef DEBUG_TESTING
			_ordzmqV2->Tester();
#endif
			break;
		case 33:
		{
			LogMe("RESETTING RISK & POSITIONS!!!", LogLevel::WARN);
			if (_SERVICES.algos)
				_algos->ResetPositions();
		}
			break;
		case 4:
		{
			LogMe("CLEARING ALL ORDERS FROM LOCAL BOOK AND RESETTING ALGO POSITIONS!", LogLevel::WARN);
			if (_SERVICES.orders) _ordzmq->Clear();
			if (_SERVICES.algos) _algos->ResetPositions();

			if (_SERVICES.exchanges)
			{
				Instruction inst = {};
				 inst.command = ExchCmd::CLEAR;
				 _exchanges->onCommand(inst);
			}
		}break;

		case 5:
		{
			if (!_SERVICES.exchanges) break;

			 if (_SERVICES.algos) {
				 LogMe("STOPPING ALL ALGOS!!",LogLevel::WARN);
				 _algos->StopAllAlgos();
			 }

			 LogMe("MASS CXL: CANCELING ALL OPEN ORDERS ON ALL SEGS!!!", LogLevel::WARN);

			 Instruction cmd = {};
			 cmd.command = ExchCmd::CXL_ALL_SEGS;
			 ExchCommand(cmd);


		}break;

		case 555:
		{
			//if an algo has a specific
			if (!_SERVICES.exchanges) break;

			 if (_SERVICES.algos) {
				 LogMe("SENDING CXL ALL ORDS TO **ALL*** ALGOS!!",LogLevel::WARN);
				 Instruction inst = {};
				 inst.command = ExchCmd::ALGO_CXL_ALL;
				 inst.cmd_value = choice;
				 _algos->onAlgoCommand(inst);

			 }
		}break;

		case 6:
		case 666:
		{
			if (!_SERVICES.exchanges) break;
			Instruction inst = {};
			inst.command = ExchCmd::MENU_CHOICE;
			inst.cmd_value = choice;
			_exchanges->onCommand(inst);
			usleep(1000);
		}break;

		case 7:
		{
			if (_SERVICES.algos) _algos->Print();
//			Instruction inst = {};
//			inst.command = ExchCmd::SESS_STATUS;
//			exchanges.Get()->Command(inst);
//			usleep(1000);
		}break;

		case 8:
		{
			if (!_SERVICES.exchanges) break;
			KT01_LOG_WARN(__CLASS__, "SENDING RISK RESET TO EXCHANGES");
			Instruction inst = {};
			inst.command = ExchCmd::RISK_RESET;
			inst.cmd_value = choice;
			_exchanges->onCommand(inst);
		}break;


		case 11:
			if (_SERVICES.exchanges) PrintLatencies(false);
			_trk.printMeasurements();
			break;
		case 12:
			if (!_SERVICES.exchanges) break;
			LogMe("CLEARING LATENCIES!!!!", LogLevel::WARN);
			PrintLatencies(true);
			_trk.clear();
			_trk.printMeasurements();
			break;
		case 22:
			PrintMsgRatios();
			break;

		case 19:
			if (_SERVICES.algos) _algos->StopAllAlgos();
			break;

		case 23:
			LogMe("SENDING CLEAR STOPS TO MM2",LogLevel::WARN);
			if (_SERVICES.algos) _algos->SendCommand(choice);
			break;

		case 59:
			LogMe("DISABLING SLACK!!!", LogLevel::ERROR);
			_SLACKER.Enable(false);
			if (_SERVICES.algos) _algos->SendCommand(choice);
			break;
		case 60:
			LogMe("ENABLING SLACK!!!", LogLevel::INFO);
			_SLACKER.Enable(true);
			if (_SERVICES.algos) _algos->SendCommand(choice);
			break;

		case 71:
		case 72:
		case 85:
			if (_SERVICES.algos) _algos->SendCommand(choice);
			break;

		case 92:
		{
			if (!_SERVICES.exchanges) break;
			LogMe("CME iLink3 Session Status", LogLevel::WARN);

			Instruction inst = {};
			inst.command = ExchCmd::SESS_STATUS;
			_exchanges->onCommand(inst);
			usleep(1000);

		}break;
		case 93:
		{
			if (!_SERVICES.exchanges) break;
			LogMe("CME iLink3 SENDING PARTY DEF REQUEST", LogLevel::WARN);

			Instruction inst = {};
			inst.command = ExchCmd::PARTY_DET_REQ;
			_exchanges->onCommand(inst);
			usleep(1000);

		}break;

		case 94:
		{
			if (!_SERVICES.exchanges) break;
			LogMe("CME iLink3 SENDING MASS ORDER STATUS REQUEST", LogLevel::WARN);

			Instruction inst = {};
			inst.command = ExchCmd::MASS_ORDER_STATUS_REQ;
			_exchanges->onCommand(inst);
			usleep(1000);

		}break;

		case 95:{
			if (!_SERVICES.exchanges) break;
			LogMe("COMMAND: MEASURE TOGGLE", LogLevel::WARN);

			Instruction inst = {};
			inst.command = ExchCmd::MEASURE;
			_exchanges->onCommand(inst);

			_MEASURE = !_MEASURE;

		}break;
		case 96:
		{
			if (!_SERVICES.exchanges) break;
			LogMe("CME iLink3 SENDING PARTY **LIST** REQUEST", LogLevel::WARN);

			Instruction inst = {};
			inst.command = ExchCmd::PARTY_LIST_REQ;
			_exchanges->onCommand(inst);
			usleep(1000);

		}break;

		case 97:
		{
			if (!_SERVICES.exchanges) break;
			LogMe("CME iLink3 SENDING RETRANSMIT REQUEST", LogLevel::WARN);

			Instruction inst = {};
			inst.command = ExchCmd::RETRANS_REQ;
			_exchanges->onCommand(inst);
			usleep(1000);

		}break;

		case 200:
		{
			LogMe("Starting EQT Market Data Feed...", LogLevel::INFO);
			(new EquityMktData())->StartAsync();
		}break;

		case 210:
		{
			if (!_SERVICES.exchanges) break;
			LogMe("FORCE TCP RECONNECT REQUEST", LogLevel::WARN);

			Instruction inst = {};
			inst.command = ExchCmd::TCP_RECONNECT;
			_exchanges->onCommand(inst);
			usleep(1000);
		}break;

		case 211:
		{
			if (!_SERVICES.exchanges) break;
			LogMe("RESET RECONNECT TRACKING REQUEST", LogLevel::WARN);

			Instruction inst = {};
			inst.command = ExchCmd::RESET_RECONNECT_TRACKING;
			_exchanges->onCommand(inst);
			usleep(1000);
		}break;

		case 350:
		{
			if (!_SERVICES.exchanges) break;
			LogMe("OSE QUERY SETTLEMENT (RQ62)", LogLevel::INFO);

			Instruction inst = {};
			inst.command = ExchCmd::QUERY_SETTLEMENT;
			_exchanges->onCommand(inst);
			usleep(1000);
		}break;

		case 999:
			LogMe("RECEIVED TERMINATE MENU COMMAND");
			Stop();
			break;

		default:
			if (choice > 1000)
			{
				if (!_SERVICES.exchanges) break;
					Instruction inst = {};
					inst.command = ExchCmd::MENU_CHOICE;
					inst.cmd_value = choice;
					_exchanges->onCommand(inst);
					usleep(1000);
			}
			break;

	}
}

void Raptor2::SendCommandToSessions(int choice)
{
	ostringstream oss;
	oss << "SENDING COMMAND " << choice << " TO SESSIONS";
	LogMe(oss.str());

	Instruction inst = {};
	inst.command = ExchCmd::MENU_CHOICE;
	inst.cmd_value = choice;
	_exchanges->onCommand(inst);
}

void Raptor2::PrintOrders()
{
	if (_SERVICES.algos)
		_algos->Print();

	if (_SERVICES.orders)
		_ordzmq->Print();
}

void Raptor2::PrintLatencies(bool clear)
{
	if (!_SERVICES.exchanges)
		return;

	Instruction inst = {};
	inst.command = ExchCmd::PRINT_LATENCIES;
	inst.cmd_value = (clear) ? 12 : 11;
	_exchanges->onCommand(inst);
}

void Raptor2::PrintMsgRatios()
{
	if (!_SERVICES.exchanges) return;

	Instruction inst = {};
	inst.command = ExchCmd::PRINT_MSG_RATIOS;
	_exchanges->onCommand(inst);
}


void Raptor2::StartZmqOrderServer()
{
	_ordzmq = new OrderHandlerZmqV6(*this);
}

void Raptor2::AlgoJson(string json, ExchCmd::Enum cmd)
{
	LogMe("**** GOT ALGO JSON FOR INPROC HANDLER ***", LogLevel::WARN);
	Instruction inst = {};
	inst.command = cmd;
	inst.cmd_json = json;
	_qAlgoCmds.push(inst);

	_EVENT_WAITING.store(true, std::memory_order_relaxed);
}

void Raptor2::ExchCommand(Instruction cmd)
{
	 _exchanges->onCommand(cmd);
}

void Raptor2::Send(KTN::OrderPod& ord, int action)
{
	//handles a redirect order
	if (ord.callbackid < 0)
		ord.callbackid = _ORDER_CB_ID;
	
	_reqid_to_callbackid[ord.orderReqId] = ord.callbackid;

	Instruction inst;
	inst.command = ExchCmd::ORD_SEND;
	inst.ord = ord;
	inst.isorder = true;
	_exchanges->onCommand(inst);
}

void Raptor2::RedirectOrderCallback(KTN::Order& ord, int newcallbackid, uint64_t orderReqId)
{
	if (newcallbackid == -1)
		newcallbackid = _ORDER_CB_ID;

	_reqid_to_callbackid[orderReqId] = newcallbackid;

	ord.callbackid = newcallbackid;
	ord.orderReqId = orderReqId;
	_ordzmq->RegisterOrderWithOrderbook(ord);
}

void Raptor2::OrderRecovered(KTN::OrderPod& ord, std::string guid, std::string tag)
{
	//We don't use this anymore. The OrderRecoveryService will handle this.
	//LogMe("Algoid " + guid + ": Order recovered for non-running algo. ExchOrdId=" + std::to_string(ord.cmeid) + ". Adding to suspense queue.", LogLevel::INFO)
	//_qSuspenseOrds[guid].push(ord);
}

void Raptor2::GetManualOrdersBySecId(std::vector<KTN::OrderPod> &ords, int32_t secId)
{
	_ordzmq->GetOrdersBySecId(ords, secId);
}

void Raptor2::MoveOrderToStrategy(const KTN::OrderPod &ord)
{
	if (ord.callbackid >= 0 && ord.callbackid < _CBCNT && _cbs[ord.callbackid] != nullptr)
	{
		_reqid_to_callbackid[ord.orderReqId] = ord.callbackid;
		// TODO remove order from OrderManager in ZMQ Order Handler?
	}
	else
	{
		KT01_LOG_WARN(__CLASS__, "Attempting to move ordser {} to strategy with invalid callback ID: {}",
			ord.exchordid, ord.callbackid);
	}
}

void Raptor2::SendOrderWithHedge(OrderWithHedge & oh)
{
	Instruction inst;
	inst.command = ExchCmd::ORD_SEND_WITH_HEDGE;
	inst.ord = oh.ord;
	inst.hdgs = oh.hdgs;
	inst.reqid = oh.ord.orderReqId;
	inst.isorder = true;
	_exchanges->onCommand(inst);

}

void Raptor2::SendHedgeInstruction(uint64_t reqid, const vector<SpreadHedgeV3> & hdg)
{
	Instruction inst;
	inst.command = ExchCmd::ORD_SEND_HDG_INSTRUCT;
	inst.hdgs = hdg;
	inst.reqid = reqid;
	inst.isorder = true;
	_exchanges->onCommand(inst);
}

int Raptor2::RegisterCallback(SessionCallback * cb)
{
	if (_CBCNT + 1 >= _MAXCBS)
	{
		ostringstream oss;
		oss << "ERROR: MAXIMUM CALLBACKS REACHED FOR THIS INSTANCE: " << _CBCNT << " MAX:" << _MAXCBS;
		LogMe(oss.str(), LogLevel::ERROR);
		return -1;
	}

	int index = _CBCNT;
	_cbs[_CBCNT] = cb;
	_CBCNT ++;

	string guid = cb->getGuid();

	_guid_to_callbackid[guid] = index;

	if (guid == "ORDERBOOK")
	{
		// This is a special case for the OrderBook. We register this internally for ease of access.
		_guid_to_callbackid["ORDBOOK"] = index;
		_ORDER_CB_ID = index;
		_ORDER_CB_GUID = guid;
		ostringstream oss;
		oss << "ZMQ Order Server Registered OK. Index=" << index << " Total=" << _CBCNT << " GUID=" << guid;
		LogMe(oss.str());
	}
	//this is now handled by AlgoServiceBrokerV3 and OrderRecoveryService
	// else
	// {
	// 	// This is a regular algo callback. We need to check if we have any orders in suspense for this GUID.
	// 	// If so, we need to process them.
	// 	ostringstream oss;
	// 	oss << "Session Callback Registered OK. Index=" << index << " Total=" << _CBCNT << " GUID=" << guid;
	// 	LogMe(oss.str());
	// 	CheckRecoveredOrders(guid);
	// }
	   
    return index;
}


void Raptor2::onExchOrdEvent(KTN::OrderPod& ord)
{
	//Check if this order has a callback mapped to it.
	auto it = _reqid_to_callbackid.find(ord.orderReqId);
	if (it != _reqid_to_callbackid.end())
		ord.callbackid = it->second;
	else
	{
		LOGINFO("[Raptor2::onExchOrdEvent] No callbackid found for orderReqId {}", ord.orderReqId);

		// If we don't have a callback, we need to check if this is a recovered order.
		// If it is, we need to check if the order is in the OrderRecoveryService.
		KTN::Order apiord;
		if (OrderRecoveryService::instance().GetOrder(ord.exchordid, apiord))
		{
			// We have a recovered order. We need to set the callback ID to the one we have in the map.
			if (_guid_to_callbackid.find(apiord.guid) != _guid_to_callbackid.end())
				ord.callbackid = _guid_to_callbackid[apiord.guid];
			else
			{
				if (apiord.OrdAlgoTrig == KOrderAlgoTrigger::ORDERBOOK)
					ord.callbackid = _ORDER_CB_ID;
				else
				{
					// We don't have a callback for this order. We need to set the callback ID to the -1 to make hasCallback go to false.
					ord.callbackid = -1;
				}
			}
			
			_reqid_to_callbackid[ord.orderReqId] = ord.callbackid;
		}
		else
			ord.callbackid = -1;
	}

	// cout << "[Raptor DEBUG] onExchOrdEvent: CALLBACKID=" << (int)ord.callbackid
	// 		<< " OrderReqID=" << ord.orderReqId
	// 		<< " ExchOrdId=" << ord.cmeid
	// 		<< " Status=" << KOrderStatus::toString(ord.OrdStatus)
	// 		<< endl;

	//If we don't have a callback, we need to check if this is a recovered order.
	//If it is, we need to check if the order is in the OrderRecoveryService.

	//we probably don't need this anymore
    const bool hasCallback = CallbackExists(ord.callbackid);

    if (!hasCallback)
    {
        KT01_LOG_HIGHLIGHT(__CLASS__,
            "[Recovered] ALGO OrderPod received with no valid callback. exchid=" + std::to_string(ord.exchordid) +
            " status=" + KOrderStatus::toString(ord.OrdStatus) +". WARNING: ASSUMING THIS IS AN ALGO ORDER");

		KT01_LOG_HIGHLIGHT(__CLASS__, "INCOMING FROM EXCH: " + OrderUtils::Print(ord));
        
        OrderRecoveryService::instance().UpdateOrder(ord);

		//protection on double counting fills in possRetransmit
		if (ord.possRetransmit)
		{
			ord.lastpx = 0;
			ord.lastqty = 0;
		}

		// Do NOT change callbackid — preserve true ownership and just in case order was updated, we can send the updated order
		// to the algos
        _algos->onRecoveredOrder(ord);
        return;  // Do NOT route anywhere
    }

    // Valid callback → route normally
    routeToCallback(ord.callbackid, ord);
}


void Raptor2::routeToCallback(int index, KTN::OrderPod& ord)
{
    switch (ord.OrdStatus)
    {
        case KOrderStatus::NEW:               _cbs[index]->minOrderAck(ord); break;
        case KOrderStatus::MODIFIED:          _cbs[index]->minOrderModify(ord); break;
        case KOrderStatus::CANCELED:          _cbs[index]->minOrderCancel(ord); break;
        case KOrderStatus::FILLED:
        case KOrderStatus::PARTIALLY_FILLED:  _cbs[index]->minOrderExecution(ord, true); break;
        case KOrderStatus::REJECTED:          _cbs[index]->minOrderReject(ord, "EXECUTION ORDER REJECTED"); break;
        case KOrderStatus::CXL_REPL_REJECT:   _cbs[index]->minOrderCancelReplaceReject(ord, "CXL_REPL_REJECT"); break;
        case KOrderStatus::CXL_REJECT:        _cbs[index]->minOrderCancelReject(ord, "CXL_REJECT"); break;

        case KOrderStatus::HEDGE_SENT:
            ord.OrdStatus = KOrderStatus::sent_new;
            _cbs[index]->onHedgeOrderSent(ord.ordernum, ord.orderReqId, ord.secid, ord.OrdSide, ord.quantity, ord.price, ord.instindex);
            break;

        case KOrderStatus::PASSOUT_SENT:
            ord.OrdStatus = KOrderStatus::sent_new;
            _cbs[index]->onOrderSent(ord.ordernum, ord.orderReqId, ord.secid, ord.OrdSide, ord.quantity, ord.price, ord.OrdAlgoTrig);
            break;

        default: break;
    }
}


