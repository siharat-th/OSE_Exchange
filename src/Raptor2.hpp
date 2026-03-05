//============================================================================
// Name        	: Raptor2.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2021-24 Katana Financial
// Date			: April 27, 2023 4:49:28 PM
//============================================================================

#ifndef RAPTOR_HPP_
#define RAPTOR_HPP_

#pragma once

#include <ExchangeHandler/ExchangeManager.hpp>
#include <ExchangeHandler/IExchangeSender.hpp>
#include <ExchangeHandler/session/SessionCallback.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>
using namespace KTN::CME::IL3;

#include <Orders/Order.hpp>
#include <Orders/OrderHandlerZmqV6.hpp>
#include <Orders/OrderRecoveryService.hpp>
#include <Orders/IdGen.h>

#include <Loggers/LogSvc.hpp>

#include <Menu/Menu.hpp>

#include <Algos/AlgoServiceBrokerV3.hpp>
#include <Algos/AlgoFactory.hpp>
#include <KT01/Core/Settings.hpp>
#include <KT01/Core/Helpers.h>
#include <KT01/Core/NamedThread.hpp>
#include <Notifications/Slacker.hpp>

#include <KT01/SecDefs/CmeSecMasterV2.hpp>
using namespace KT01::SECDEF::CME;

#include <Reporters/StatusReporter.hpp>
using namespace KTN::REPORTER;

#include <Orders/OrderNums.hpp>



using namespace KTN;
using namespace KTN::notify;

/**
 * @class Raptor2
 * @brief Represents the Raptor2 Main Exchange Handler class that extends NamedThread, MenuCallback, and IExchangeSender.
 */
class Raptor2 : public NamedThread, public MenuCallback, public IExchangeSender
{
public:
	/**
  * @brief Constructs a Raptor2 object with the specified settings file.
  * @param settingsfile The path to the settings file.
  */
	Raptor2(const string& settingsfile);

	/**
  * @brief Destroys the Raptor2 object.
  */
	virtual ~Raptor2();

	void WaitForShutdown();

	/**
  * @brief Stops the Raptor2 object.
  */
	void Stop();

	/**
  * @brief Runs the Raptor2 object.
  */
	void Run() override;

	/**
  * @brief Registers a session callback to the Raptor2 object.
  * @param cb The session callback to subscribe.
  * @return The subscription ID.
  */
    int RegisterCallback(SessionCallback *cb);

    //void CheckRecoveredOrders(std::string &guid);

    /**
  * @brief Sends an order with the specified action to the Raptor2 object.
  * @param order The order to send.
  * @param action The action to perform on the order.
  */
	void Send(KTN::OrderPod& order, int action);


	/**
  * @brief Redirects the order callback with the specified order and new callback ID.
  * @param ord The order to redirect.
  * @param newcallbackid The new callback ID to set.
  */
	void RedirectOrderCallback(KTN::Order& ord, int newcallbackid, uint64_t orderReqId);

	/**
  * @brief Sends a hedge instruction with the specified order request ID and hedge to the Raptor2 object.
  * @param ordereqid The order request ID.
  * @param hdg The hedge to send.
  */
	void SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3>& hdg);

	/**
  * @brief Sends an order with hedge to the Raptor2 object.
  * @param oh The order with hedge to send.
  */
	void SendOrderWithHedge(OrderWithHedge& oh);


	/**
  * @brief Sends an algorithm JSON command with the specified JSON and command to the Raptor2 object.
  * @param json The JSON command.
  * @param cmd The command to perform.
  */
	void AlgoJson(std::string json, ExchCmd::Enum cmd);


	/**
  * @brief Sends a generic exchange Command Instruction; usually mass order or action related
  * @param inst Instruction object
	 */
	void ExchCommand(Instruction inst);

	/**
  * @brief Handles the order recovered event with the specified order.
  * @param ord The order to recover.
  * @param guid The algo GUID.
  * @param tag The order tag.
  */
	void OrderRecovered(KTN::OrderPod& ord, std::string guid, std::string tag);

	virtual void GetManualOrdersBySecId( std::vector<KTN::OrderPod> &ords, int32_t secId) override;
	virtual void MoveOrderToStrategy(const KTN::OrderPod &ord) override;

private:
	/**
  * @brief Logs the specified message with the specified color and persistence flag.
  * @param szMsg The message to log.
  * @param iColor The color of the message.
  * @param Persist The persistence flag.
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO, bool Persist = false);

    
    /**
  * @brief Starts up the Raptor2 object with the specified settings file.
  * @param settingsfile The path to the settings file.
  */
 	void StartServices(const string &settingsfile);


	/**
  * @brief Starts the exchanges.
  */
	void StartExchanges();

	/**
  * @brief Starts the ZMQ order server with the specified Argo server, ports, source, key, account, and affinity.
  * @param ArgoServer The Argo server.
  * @param ArgoPort The Argo port.
  * @param ArgoPushPort The Argo push port.
  * @param source The source.
  * @param key The key.
  * @param account The account.
  * @param affinity The affinity.
  */
	void StartZmqOrderServer();

	/**
  * @brief Tests the order.
  */
	void TestOrder();

	/**
  * @brief Displays the menu.
  */
	void MenuDisplay();

	/**
  * @brief Handles the menu choice with the specified choice.
  * @param choice The menu choice.
  */
	void MenuChoice(int choice);


	/**
  * @brief Constructs a map to display menu choices. Updated by the programmer and then compiled.
  * @returns map of menu choices and descriptions.
  */
	std::map<int, std::string> GetMenuChoices();


	/**
  * @brief Prints the menu.
  */
	void PrintMenu();

	/**
  * @brief Sends the specified choice command to the sessions.
  * @param choice The choice command to send.
  */
	void SendCommandToSessions(int choice);

	/**
  * @brief Handles the menu choice event with the specified key.
  * @param iKey The menu choice event key.
  */
	virtual void onMenuChoice(int iKey);

	/**
  * @brief Prints the orders.
  */
	void PrintOrders();

	/**
  * @brief Prints the latencies with the specified clear flag.
  * @param clear The clear flag.
  */
	void PrintLatencies(bool clear);

	/**
  * @brief Prints the message ratios.
  */
	void PrintMsgRatios();

	/**
  * @brief Handles the exchange order event with the specified order.
  * @param ord The exchange order event.
  */
	void onExchOrdEvent(KTN::OrderPod& ord);

	/**
  * @brief Handles the order event with the specified order.
  * @param index The index of the callback.
  * @param ord The order event.
  */
    void routeToCallback(int index, KTN::OrderPod &ord);

    inline int getCbidFromGuid(std::string& guid)
	{
		for(int i = 0; i < _CBCNT; i++)
		{
			if (_cbs[i]->getGuid() == guid)
				return i;
		}

		cout << "[getCbidFromGuid DEBUG] GUID NOT FOUND: " << guid << endl;
		return -1;
	}

	inline bool CallbackExists(int8_t callbackid) {
		return callbackid >= 0 && callbackid < static_cast<int8_t>(_cbs.size()) && _cbs[callbackid] != nullptr;
	}
	
	
	
	

private:
	std::string _ORG;
	std::string _SOURCE;
	std::string _SETTINGSFILE;

	bool _MEASURE;

	int _MAXCBS = 20;
	int _CBCNT;
	int _ORDER_CB_ID;
	string _ORDER_CB_GUID;
	std::array<SessionCallback*, 20> _cbs = {};  // All elements are set to nullptr

	Menu* menu;

	struct ServiceState
	{
		bool algos;
		bool orders;
		bool exchanges;
	};
	ServiceState _SERVICES;

	LogSvc* _logsvc;
	//Settings settings_;

	AlgoServiceBrokerV3* _algos;
	OrderHandlerZmqV6* _ordzmq;

	Slacker _SLACKER;

	ExchangeManager* _exchanges;
	tbb::concurrent_queue<KTN::OrderPod> qExchOrdsToProcess;

	//std::unordered_map<std::string, tbb::concurrent_queue<KTN::OrderPod> > _qSuspenseOrds;

	//std::unordered_map<std::string, int8_t> _clordid_to_callbackid;
	//std::unordered_map<std::string, int8_t> _guid_to_callbackid;
	tbb::concurrent_unordered_map<std::string, int8_t> _guid_to_callbackid;
	//std::unordered_map<uint64_t, int8_t> _reqid_to_callbackid;
	tbb::concurrent_unordered_map<uint64_t, int8_t> _reqid_to_callbackid;

	tbb::concurrent_queue<Instruction> _qCmds;
	tbb::concurrent_queue<Instruction> _qAlgoCmds;
	tbb::concurrent_queue<int> _qMenuChoice;

	//SPSCRingBuffer<KTN::OrderPod> _qOrdsSend;
	//SPSCRingBuffer<std::pair<uint64_t, vector<SpreadHedgeV3>>> _qHdgs;
	//SPSCRingBuffer<OrderWithHedge> _qOrdHdgCombo;

	std::atomic<bool> _EVENT_WAITING;
	std::atomic<bool> _ALGO_WAITING;

	std::atomic<bool> _ACTIVE;
	std::atomic<bool> _ALGOS_IN_USE;

	std::atomic<bool> _spinRaptorThread { false };

	template<bool SPIN>
	void runRaptorQueueThread()
	{
		_ACTIVE.store(true, std::memory_order_relaxed);

		while(_ACTIVE.load(std::memory_order_acquire) == true)
		{
			while(!qExchOrdsToProcess.empty())
			{
				KTN::OrderPod ord = {};
				while(qExchOrdsToProcess.try_pop(ord))
				{
					onExchOrdEvent(ord);
				}
			}

			while (_EVENT_WAITING.load(std::memory_order_acquire) == true)
			{
				while(!_qAlgoCmds.empty())
				{
					Instruction inst = {};
					if (_qAlgoCmds.try_pop(inst))
					{
						_exchanges->onCommand(inst);
					}
				}

				_EVENT_WAITING.store(false, std::memory_order_relaxed);
			}

			if constexpr (!SPIN)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}
		}
	}

	PerformanceTracker _trk;
};

#endif
