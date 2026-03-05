/*
 * AlgoServiceBrokerV3.cpp
 *
 *  Created on: Apr 24, 2021
 *  ReCreated on Apr 29, 2023. Ass.
 *      Author: sgaer
 *
 *
 *  *******VERSION 3**********
 *  *******VERSION 3**********
 *  This version will simply be a receiver of zmq messages, and then forward
 *  them to the respective session handler.  The algos are being moved from the
 *  main thread (current) to inside the critical/hot path of the session.
 *
 *  There, AlgoFactory will create, launch and manage the algos
 *
 *  Also added ability to recover orders on a per algo basis.
 *
 *  Also added ability to recover orders on a per algo basis.
 *
 */

#include <Algos/AlgoServiceBrokerV3.hpp>
#include <akl/ProrataProfileManager.hpp>
#include <akl/PositionStore.hpp>
#include <akl/StrategyManager.hpp>

namespace KTN
{

	AlgoServiceBrokerV3::AlgoServiceBrokerV3(IExchangeSender *sess) : _sess(sess)
	{
		string settingsfile = "Settings.txt";
		Settings settings_ = Settings(settingsfile);

		std::string ArgoServer = settings_.getString("ArgoServer");
		int ArgoPort = settings_.getInteger("ArgoPort");
		_SOURCE = settings_.getString("Source");
		_cpuCount = settings_.getInteger("Affinity");
		int cpu = settings_.getInteger("AlgoAffinity");
		_DEBUG_ME = settings_.getBoolean("Debug");
		_ARCHETYPE = settings_.getString("AlgoArchetype");

		// Load verbose logging flag from DebugSettings.txt
		Settings dbgsett;
		dbgsett.Load("DebugSettings.txt", "DBG");
		LOAD_TYPED_SETTING_OR_DEFAULT(dbgsett, "Debug.AlgoServiceBrokerV3.Verbose", getBoolean, _verboseLog, true);
		LOGINFO("AlgoServiceBrokerV3 | verboseLog={}", _verboseLog);

		initZmq(ArgoServer, ArgoPort);

		_CURRENT_CORE = cpu;
		_STARTING_CORE = cpu;

		const auto processor_count = std::thread::hardware_concurrency();
		_CPUS = processor_count;

		if (_CURRENT_CORE >= _CPUS - 1)
		{
			_STARTING_CORE = 1;
			_CURRENT_CORE = _STARTING_CORE;
		}

		ostringstream oss;
		oss << "ALGO SERVICE V3 STARTING CORE=" << _CURRENT_CORE << " TOTAL CORES=" << _CPUS;
		LogMe(oss.str(), LogLevel::INFO);

		akl::ProrataProfileManager::instance().Init(_SOURCE);
		akl::PositionStore::instance().Init();
		akl::StrategyManager::instance().Init();

		CreateAndRun("AlgoSvcV3", _STARTING_CORE);
	}

	AlgoServiceBrokerV3::~AlgoServiceBrokerV3()
	{
	}

	void AlgoServiceBrokerV3::LogMe(std::string szMsg, LogLevel::Enum level)
	{
		switch(level)
		{
			case LogLevel::INFO:
				KT01_LOG_INFO(__CLASS__, szMsg);
				break;
			case LogLevel::OK:
				KT01_LOG_OK(__CLASS__, szMsg);
				break;
			case LogLevel::HIGHLIGHT:
				KT01_LOG_HIGHLIGHT(__CLASS__, szMsg);
				break;
			case LogLevel::ERROR:
				KT01_LOG_ERROR(__CLASS__, szMsg);
				break;
			case LogLevel::WARN:
				KT01_LOG_WARN(__CLASS__, szMsg);
				break;
			case LogLevel::DEBUG:
				KT01_LOG_DEBUG(__CLASS__, szMsg);
				break;
			default:
				KT01_LOG_INFO(__CLASS__, szMsg);
				break;
		}

	}

	void AlgoServiceBrokerV3::StopAllAlgos()
	{
		for (auto kv : _algos)
		{
			Instruction inst = {};
			inst.command = ExchCmd::ALGO_STOP;
			kv.second->Enable(false);
		}
	}

	void AlgoServiceBrokerV3::Print()
	{
		for (auto kv : _algos)
		{
			Instruction inst = {};
			inst.command = ExchCmd::ALGO_PRINT;
			kv.second->Print();
		}
	}

	void AlgoServiceBrokerV3::initZmq(std::string ArgoServer, int ArgoPort)
	{
		_zmq = new ZmqSubscriberV5(this, ArgoServer, ArgoPort,
								   "AlgoZmq", "Settings.txt", "Algos.Topics");
	}

	void AlgoServiceBrokerV3::Run()
	{
		LogMe("****STARTING ALGO SVC BROKER THREAD", LogLevel::WARN);

		_ACTIVE.store(true, std::memory_order_relaxed);

		while (_ACTIVE.load(std::memory_order_acquire) == true)
		{

			if (_EVENT_WAITING.load(std::memory_order_acquire))
			{
				if (!_qcmds.empty())
				{
					Instruction cmd = {};
					while (_qcmds.try_pop(cmd))
						onAlgoCommand(cmd);
				}

				_EVENT_WAITING.store(false, std::memory_order_relaxed);
			}

			akl::stacker::Timestamp now = akl::stacker::Time::ClockType::now();
			if (now >= lastHeartbeatCheckTime + HEARTBEAT_CHECK_INTERVAL)
			{
				checkHeartbeats(now);
				lastHeartbeatCheckTime = now;
			}

			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
	}

	void AlgoServiceBrokerV3::onPubMsg(std::string szTopic, std::string json)
	{
		JsonHeader hdr = JsonOrderDecoders::CheckJsonHeader(json);

		if (hdr.isEmpty)
		{
			LogMe("JSON HEADER RETURNS 'EMPTY'; INVALID FORMAT: TOPIC=" + szTopic, LogLevel::ERROR);
			return;
		}

		if (hdr.source.compare(_SOURCE) != 0 && hdr.source.compare("*") != 0)
		{
			// LogMe("SOURCE OF HEADER [" + hdr.source + "] DOESN'T MATCH CURRENT SOURCE OF " + _SOURCE, LogLevel::ERROR);
			return;
		}

		if (_verboseLog) {
			LogMe("GOT ALGO ZMQ: TOPIC=" + hdr.topic, LogLevel::INFO);
		}

		// HACK FOR NOW-- ONLY USING CME:
		hdr.destination = "cme";

		if (hdr.topic.compare("ALGOPARAMS") == 0)
		{
			Instruction inst = {};
			inst.cmd_json = json;
			inst.command = ExchCmd::ALGO_PARAMS;

			_qcmds.push(inst);
			_EVENT_WAITING.store(true, std::memory_order_relaxed);
			return;
		}

		if (hdr.topic == "ALGOENGINE")
		{
			LogMe("GOT ALGO ENGINE MESSAGE", LogLevel::WARN);
			LogMe(json, LogLevel::WARN);

			Instruction inst = {};
			inst.cmd_json = json;
			inst.command = ExchCmd::ALGO_ENGINE;

			_qcmds.push(inst);
			_EVENT_WAITING.store(true, std::memory_order_relaxed);
		}

		if (hdr.topic == "ALGOORDER")
		{
			KT01_LOG_INFO(__CLASS__, "GOT ALGO ORDER COMMAND MESSAGE: {}", json);
			Instruction inst = {};
			inst.cmd_json = json;
			inst.command = ExchCmd::ALGO_ORDER;
			_qcmds.push(inst);
			_EVENT_WAITING.store(true, std::memory_order_relaxed);
		}

		if (hdr.topic == "ALGOMESSAGE")
		{
			Instruction inst = {};
			inst.cmd_json = json;
			inst.command = ExchCmd::ALGO_MESSAGE;

			_qcmds.push(inst);
			_EVENT_WAITING.store(true, std::memory_order_relaxed);
			return;
		}

		if (hdr.topic == "ALGOKILLSWITCH")
		{
			Instruction inst = {};
			inst.cmd_json = json;
			inst.command = ExchCmd::ALGO_KILLSWITCH;

			_qcmds.push(inst);
			_EVENT_WAITING.store(true, std::memory_order_relaxed);
			return;
		}
	}

	void AlgoServiceBrokerV3::onAlgoCommand(const Instruction &cmd)
	{
		switch (cmd.command)
		{
		case ExchCmd::ALGO_PARAMS:
			HandleAlgoParams(cmd.cmd_json);
			break;

		case ExchCmd::ALGO_ORDER:
			HandleAlgoOrder(cmd.cmd_json);
			break;

		case ExchCmd::ALGO_ENGINE:
		{
			LogMe("GOT ALGO ENGINE CMD!");
			EngineCommand eng = AlgoFactory::ParseEngineCommand(cmd.cmd_json);

			_algos[eng.guid]->onEngineJson(eng);

			break;
		}
		case ExchCmd::ALGO_STOP:
		{
			if (_algos.size() == 0)
				break;
			LogMe("STOPPING ALL ALGOS!!!");
			for (auto kv : _algos)
				kv.second->Enable(false);
		}
		break;

		case ExchCmd::ALGO_PRINT:
		{
			if (_algos.size() == 0)
				break;
			LogMe("ALGO PRINT COMMAND RECEIVED");
			for (auto kv : _algos)
				kv.second->Print();
		}
		break;

		case ExchCmd::ALGO_CXL_ALL:
		{
			if (_algos.size() == 0)
				break;
			LogMe("ALGO CXL ALL ORDERS CMD RECEIVED");
			for (auto kv : _algos)
				kv.second->CancelAllOrders();
		}
		break;

		case ExchCmd::ALGO_MESSAGE:
		{
			onAlgoMessage(cmd);
		}
		break;

		case ExchCmd::ALGO_KILLSWITCH:
		{
			LogMe("GOT ALGO KILLSWITCH CMD: " + cmd.cmd_json);

			const std::optional<bool> killswitchState = AlgoFactory::ParseKillswitchCommand(cmd.cmd_json);
			if (killswitchState.has_value())
			{
				if (_algos.size() > 0)
				{
					if (*killswitchState)
					{
						for (auto kv : _algos)
						{
							kv.second->DisableRisk();
						}
						for (auto kv : _algos)
						{
							kv.second->TryStop();
						}

						Instruction cmd = {};
						cmd.command = ExchCmd::CXL_ALL_SEGS;
						this->_sess->ExchCommand(cmd);


						std::ostringstream ss;
						ss << "KILLSWITCH ENABLED FOR " << _SOURCE << ". All resting orders deleted. No strategy orders allowed. Only manual orders until killswitch is disabled";
						KTN::notify::Notification n = {};
						n.exch = "ALGO";
						n.source = _SOURCE;
						n.org = "AKL";
						n.level = MsgLevel::ERROR;
						n.imsg = MsgType::ALERT;
						n.text = ss.str();
						KTN::notify::NotifierRest::instance().Notify(n);
					}
					for (auto kv : _algos)
					{
						kv.second->onKillswitchUpdate(*killswitchState);
					}
					if (*killswitchState)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(250));
						Instruction cmd = {};
						cmd.command = ExchCmd::CXL_ALL_SEGS;
						this->_sess->ExchCommand(cmd);
					}
					else
					{
						std::ostringstream ss;
						ss << "KILLSWITCH DISABLED FOR " << _SOURCE << ". All orders allowed";
						KTN::notify::Notification n = {};
						n.exch = "ALGO";
						n.source = _SOURCE;
						n.org = "AKL";
						n.level = MsgLevel::INFO;
						n.imsg = MsgType::NOTIFY;
						n.text = ss.str();
						KTN::notify::NotifierRest::instance().Notify(n);
					}
				}
				else
				{
					if (*killswitchState)
					{
						Instruction cmd = {};
						cmd.command = ExchCmd::CXL_ALL_SEGS;
						this->_sess->ExchCommand(cmd);

						std::ostringstream ss;
						ss << "KILLSWITCH ENABLED FOR " << _SOURCE << ". All resting orders deleted. No strategy orders allowed. Only manual orders until killswitch is disabled";
						KTN::notify::Notification n = {};
						n.exch = "ALGO";
						n.source = _SOURCE;
						n.org = "AKL";
						n.level = MsgLevel::ERROR;
						n.imsg = MsgType::ALERT;
						n.text = ss.str();
						KTN::notify::NotifierRest::instance().Notify(n);
					}
					else
					{
						std::ostringstream ss;
						ss << "KILLSWITCH DISABLED FOR " << _SOURCE << ". All orders allowed";
						KTN::notify::Notification n = {};
						n.exch = "ALGO";
						n.source = _SOURCE;
						n.org = "AKL";
						n.level = MsgLevel::INFO;
						n.imsg = MsgType::NOTIFY;
						n.text = ss.str();
						KTN::notify::NotifierRest::instance().Notify(n);
					}
				}
				killswitchEnabled = *killswitchState;
				LogMe("KILLSWITCH STATE IS NOW: " + std::to_string(killswitchEnabled), LogLevel::INFO);
			}
		}
		break;
		default:
			break;
		}
	}

	void AlgoServiceBrokerV3::HandleAlgoParams(string json)
	{
		//string json = cmd.cmd_json;
		AlgoHeader hdr = AlgoFactory::ParseAlgoHeader(json);
		// LogMe("V3 GOT ALGO PARAM JSON: " + cmd.cmd_json);

		string archetype = hdr.archetype;

		if (_algos.find(hdr.guid) == _algos.end())
		{
			if (killswitchEnabled)
			{
				KTN::notify::Notification n = {};
				n.exch = "ALGO";
				n.source = _SOURCE;
				n.org = "AKL"; // TODO?
				n.level = MsgLevel::ERROR;
				n.imsg = MsgType::ALERT;
				n.text = "Can't launch new strategy when killswitch is enabled";
				KTN::notify::NotifierRest::instance().Notify(n);

				LogMe("Can't launch new strategy when killswitch is enabled", LogLevel::ERROR);
			}
			else if (hdr.enabled)
			{
				// only on launch- we check to see if we've recovered orders...
				vector<KTN::OrderPod> recoveredOrders = GetRecoveredOrdersForAlgo(hdr.guid);
				LogMe("[INFO] Recovering orders for GUID: " + hdr.guid + ". Count=" + std::to_string(recoveredOrders.size()));
				
				int core = Core();
				AlgoBaseV3 *result = AlgoFactory::LaunchAlgo(json, core, _sess, recoveredOrders);

				// Check if the result is not NULL
				if (result != NULL)
				{
					LogMe("LAUNCHING ALGO!!", LogLevel::INFO);
					_algos[hdr.guid] = result;
				}
				else 
				{
					// Log an error message if the result is NULL
					LogMe("ALGO " + archetype + " NOT FOUND IN ALGO MASTER SERVICE", LogLevel::ERROR);

					std::ostringstream ss;
					ss << "Algo type " << archetype << " not found";

					KTN::notify::Notification n = {};
					n.exch = "ALGO";
					n.source = _SOURCE;
					n.org = "AKL"; // TODO?
					n.level = MsgLevel::ERROR;
					n.imsg = MsgType::ALERT;
					n.text = ss.str();
					KTN::notify::NotifierRest::instance().Notify(n);

					return;
				}

				_algos[hdr.guid]->onLaunch(json);

				// We now will check to see if orders exist in the suspense queue that
				// have been recovered.
			}
			else
				LogMe("ALGO IS IN DISABLED STATE AND NOT YET LOADED/STARTED. NOT PROCEEDING", LogLevel::ERROR);
		}
		else
			_algos[hdr.guid]->onJson(json);
	}

	void AlgoServiceBrokerV3::HandleAlgoOrder(string json)
	{
			KTN::Order ord = AlgoFactory::ParseAlgoOrder(json);

			if (_algos.find(ord.algoid) != _algos.end())
			{
				_algos[ord.algoid]->OrderReceived(ord, -1);
			}
			else
			{
				LogMe("Received order request of type " + ord.action + " for non-running algo: " + ord.algoid, LogLevel::WARN);
				// If the algo is NOT running, and we receive an order command, we need to
				// determine what to do.  Generally- I think we should just cancel it.
				string action = ord.action;

				KT01_LOG_HIGHLIGHT(__CLASS__, "ACTION=" + action + ": NON RUNNING ALGO ORDER REQUEST. CANCELING THIS ORDER.");

				HandleAlgoCancelForNonRunningAlgo(ord);
			}
	}

	void AlgoServiceBrokerV3::HandleAlgoCancelForNonRunningAlgo(KTN::Order &ord)
	{
		// This function will handle the cancel for an order that belongs to a non-running algo.
		KT01_LOG_HIGHLIGHT(__CLASS__, "Handling cancel for non-running algo order: " + OrderUtils::Print(ord));

		KTN::Order existingOrder;
		if (OrderRecoveryService::instance().GetOrder(ord.exchordid, existingOrder))
		{
			KTN::OrderPod neword;
			string newordid = OrderNums::instance().NextClOrdId();
			OrderUtils::Transpose(neword, existingOrder, newordid.c_str()); // Transpose the existing order details to the new order pod.

			//set reqid callback to the orderbook callback so we can ensure processing
			//neword.orderReqId =  OrderNums::instance().NextReqId(); // Generate a new request ID for this order. 0 is used as a placeholder for callbackid since it's not relevant here.
			neword.callbackid = -1;
			neword.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;
			neword.isManual = true;
			neword.stratid = 0;
			neword.OrdStatus = KOrderStatus::sent_cxl;
			neword.OrdAction = KOrderAction::ACTION_CXL;
			OrderUtils::fastCopy(neword.origordernum, existingOrder.ordernum, sizeof(neword.origordernum));

			neword.secid = existingOrder.secid;
			neword.hasMeta = false; // Reset the hasMeta flag since we are transferring ownership of the order.	

			LogMe("[INFO] Redirecting Order Callback for Order " + std::to_string(ord.exchordid) + " to OrderBook. SECID=" + std::to_string(neword.secid));
			//note: -1 tells the session callback to use the orderbook callback
			_sess->RedirectOrderCallback(existingOrder, -1, neword.orderReqId);

			_sess->Send(neword, KOrderAction::ACTION_CXL); // Send the cancel request to the exchange.

			LogMe("ALGO ORDER CANCELED FOR ALGO " + ord.algoid + "***THIS IS BY DESIGN. MODIFICATIONS TO ALGO ORDERS WHILE ALGO IS NOT RUNNING WILL RESULT IN ORDER CANCELATION.", LogLevel::ERROR);
			
			// You can also remove it from the API orders if needed.
			OrderRecoveryService::instance().RemoveOrder(existingOrder.exchordid);
			removeSuspenseOrder(existingOrder.algoid, existingOrder.exchordid); // Remove the order from the suspense map.
		}
		else
		{
			KT01_LOG_ERROR(__CLASS__, "Order with exchordid=" + std::to_string(ord.exchordid) + " not found in API orders.");
		}
	}

	void AlgoServiceBrokerV3::onRecoveredOrder(KTN::OrderPod &ord) {

		KTN::Order existingOrder;
		if (OrderRecoveryService::instance().GetOrder(ord.exchordid, existingOrder))
		{
			_mapSuspenseOrds[existingOrder.algoid][existingOrder.exchordid] = ord;
				LogMe("Algoid " + existingOrder.algoid + ": Order recovered for non-running algo. ExchOrdId=" 
					+ std::to_string(ord.exchordid) + ". Adding to suspense map.");
		}
		else
		{
			KT01_LOG_ERROR(__CLASS__, "Order with exchordid=" + std::to_string(ord.exchordid) + " not found in API orders.");
		}
	}

	void AlgoServiceBrokerV3::removeSuspenseOrder(const std::string& guid, int exchordid) {
		auto it = _mapSuspenseOrds.find(guid);
		if (it != _mapSuspenseOrds.end())
		{
			auto& ordersMap = it->second;
			ordersMap.erase(exchordid);
			LogMe("OK: Removed order from suspense map: " + std::to_string(exchordid), LogLevel::OK);
		}
		else
		{
			KT01_LOG_ERROR(__CLASS__, "Order with exchordid=" + std::to_string(exchordid) + " not found in suspense map. Algo ID " + guid + " not found.");
		}
	}

	void AlgoServiceBrokerV3::onAlgoMessage(const Instruction &cmd)
	{
		rapidjson::Document document;
		rapidjson::ParseResult result = document.Parse(cmd.cmd_json.c_str());

		if (!result)
		{
			std::string error = std::string("JSON parse error: ") + rapidjson::GetParseError_En(result.Code()) +
								" at offset " + std::to_string(result.Offset());
			LOGERROR("[AlgoServiceBrokerV3] {}", error);
			return;
		}

		int messageTypeInt = 0;
		std::string payload = "";

		for (rapidjson::GenericValue<rapidjson::UTF8<>>::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
		{
			if (itr->HasMember("MessageType") && itr->operator[]("MessageType").IsInt())
			{
				messageTypeInt = (*itr)["MessageType"].GetInt();
			}
			if (itr->HasMember("Payload"))
			{
				payload = (*itr)["Payload"].GetString();
				if (_verboseLog) {
					LOGINFO("[AlgoServiceBrokerV3] payload: {}", payload);
				}
			}
		}

		const stacker::MessageType messageType = static_cast<stacker::MessageType>(messageTypeInt);

		switch (messageType)
		{
			case stacker::MessageType::HEARTBEAT:
				onClientHeartbeat(cmd, payload);
				break;
			case stacker::MessageType::PRORATA_PROFILE_UPDATE:
			case stacker::MessageType::PRORATA_PROFILE_DELETE:
				akl::ProrataProfileManager::instance().OnAlgoMessage(cmd.cmd_json);
				if (_algos.size() == 0)
				{
					return;
				}
				for (auto kv : _algos)
				{
					kv.second->onAlgoMessage(cmd.cmd_json);
				}
				break;
			default:
				if (_algos.size() == 0)
				{
					return;
				}
				for (auto kv : _algos)
				{
					kv.second->onAlgoMessage(cmd.cmd_json);
				}
				break;
		}
	}

	void AlgoServiceBrokerV3::onClientHeartbeat(const Instruction &cmd, const std::string &payload)
	{
		akl::stacker::HeartbeatMessage msg = akl::stacker::HeartbeatMessage::DeserializeFromString(payload);
		if (!msg.clientId.empty())
		{
			HeartbeatTimeMapType::iterator it = heartbeatTimeMap.find(msg.clientId);
			if (it != heartbeatTimeMap.end())
			{
				it->second = akl::stacker::Time::ClockType::now();
			}
			else
			{
				heartbeatTimeMap[msg.clientId] = akl::stacker::Time::ClockType::now();
				for (auto kv : _algos)
				{
					kv.second->onClientConnected(msg.clientId);
				}
				LOGINFO("[AlgoServiceBrokerV3] Received heartbeat for new client ID {}", msg.clientId);
			}
		}
		else
		{
			LOGWARN("[AlgoServiceBrokerV3] Received HeartbeatMessage with empty clientId");
		}
	}

	void AlgoServiceBrokerV3::checkHeartbeats(const akl::stacker::Time::TimestampType now)
	{
		HeartbeatTimeMapType::iterator it = heartbeatTimeMap.begin();
		while (it != heartbeatTimeMap.end())
		{
			if (now >= it->second + HEARTBEAT_TIMEOUT)
			{
				for (auto kv : _algos)
				{
					kv.second->onClientDisconnected(it->first);
				}
				LOGWARN("[AlgoServiceBrokerV3] Detected missed heartbeats for client {}", it->first);
				it = heartbeatTimeMap.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

} /* namespace KTN */
