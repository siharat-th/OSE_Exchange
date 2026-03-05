/*
 * AlgoServiceBrokerV3.hpp
 *
 *  Created on: Jan 15 2024
 *      Author: sgaer
 */

#ifndef ALGOSERVICEBROKERV3_HPP_
#define ALGOSERVICEBROKERV3_HPP_

#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
using namespace rapidjson;

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <algorithm>

#include <KT01/Core/Log.hpp>
#include <KT01/Core/PerformanceCounter.h>
#include <KT01/Core/NamedThread.hpp>

#include <ExchangeHandler/session/SessionCallback.hpp>

#include <Orders/IdGen.h>
#include <Orders/Order.hpp>
#include <Orders/OrderUtils.hpp>
#include <Orders/OrderRecoveryService.hpp>

#include <Json/JsonOrderDecoders.hpp>

#include <ZMQ/ZmqSubscriberV5.hpp>

#include <Algos/AlgoBaseV3.hpp>
#include <Algos/algo_includes.hpp>

#include <tbb/spin_mutex.h>
using namespace tbb;

#include <bits/stdc++.h>

#include <ExchangeHandler/IExchangeSender.hpp>
#include <KT01/SecDefs/CmeSecMasterV2.hpp>
using namespace KT01::SECDEF::CME;

#include <AlgoParams/LegParamParser.hpp>
#include <AlgoParams/OrderParser.hpp>
#include <AlgoEngines/engine_structs.hpp>
#include <Algos/AlgoFactory.hpp>

using namespace std;

#include <Algos/AklStacker/Time.hpp>


namespace KTN {

/**
 * @brief The AlgoServiceBrokerV3 class routes messages to and from child instances of AlgoBaseV3.
 * 
 * This class provides functionality for initializing ZeroMQ, printing information, stopping all algorithms,
 * resetting positions, sending commands, and handling algorithm commands and orders.
 */
class AlgoServiceBrokerV3 : public IZmqSubscriber, public NamedThread
{
public:
    /**
     * @brief Constructs an instance of AlgoServiceBrokerV3 with the given exchange sender.
     * 
     * @param sess The exchange sender to be used by the broker.
     */
    explicit AlgoServiceBrokerV3(IExchangeSender* sess);

    /**
     * @brief Destroys the AlgoServiceBrokerV3 instance.
     */
    virtual ~AlgoServiceBrokerV3();

    /**
     * @brief Initializes ZeroMQ with the specified server and port.
     * 
     * @param ArgoServer The server address for ZeroMQ.
     * @param ArgoPort The port number for ZeroMQ.
     */
    void initZmq(std::string ArgoServer, int ArgoPort);

    /**
     * @brief Prints information about the broker.
     */
    void Print();

    /**
     * @brief Stops all running algorithms.
     */
    void StopAllAlgos();

    /**
     * @brief Resets all positions.
     */
    void ResetPositions() {}

    /**
     * @brief Resets the position for a single algorithm with the specified unique ID.
     * 
     * @param uniqueid The unique ID of the algorithm to reset.
     */
    void ResetPositionSingle(string uniqueid) {}

    /**
     * @brief Sends a command to the broker with the specified ID.
     * 
     * @param id The ID of the command to send.
     */
    void SendCommand(int id) {}

    /**
     * @brief Handles an algorithm command.
     * 
     * @param cmd The instruction representing the algorithm command.
     */
    void onAlgoCommand(const Instruction& cmd);

    void HandleAlgoParams(string json);

    void HandleAlgoOrder(string json);

    //order recovery methods for non-running algos:
    void HandleAlgoCancelForNonRunningAlgo(KTN::Order &ord);

    void onRecoveredOrder(KTN::OrderPod& ord);

    void removeSuspenseOrder(const std::string &guid, int cmeid);

    
    /**
     * @brief Handles an algorithm order.
     * 
     * @param cmd The instruction representing the algorithm order.
     */
    void onAlgoOrder(const Instruction& cmd);

private:
    /**
     * @brief Logs a message with the specified color.
     * 
     * @param szMsg The message to log.
     * @param iColor The color of the log message.
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

    /**
     * @brief Handles a published message from ZeroMQ.
     * 
     * @param szTopic The topic of the published message.
     * @param szMsg The content of the published message.
     */
    void onPubMsg(std::string szTopic, std::string szMsg);

    /**
     * @brief Handles a JSON message.
     * 
     * @param json The JSON message to handle.
     */
    void onJson(std::string json);

    /**
     * @brief Handles an engine JSON message.
     * 
     * @param json The engine JSON message to handle.
     */
    void onEngineJson(std::string json);

    /**
     * @brief Handles an order JSON message.
     * 
     * @param json The order JSON message to handle.
     */
    void onOrderJson(std::string json);

    /**
     * @brief Launches an algorithm with the specified archetype, GUID, and JSON.
     * 
     * @param archetype The archetype of the algorithm to launch.
     * @param guid The GUID of the algorithm to launch.
     * @param json The JSON configuration of the algorithm to launch.
     * @return true if the algorithm was successfully launched, false otherwise.
     */
    bool LaunchAlgo(std::string archetype, std::string guid, std::string json);

    /**
     * @brief Runs the broker thread.
     */
    void Run() override;

    /**
     * @brief Gets the current core number for running algorithms.
     * 
     * @return The current core number.
     */
    	int Core()
	{
		const auto processor_count = std::thread::hardware_concurrency();
		_CPUS = processor_count;

		_CURRENT_CORE ++;

		if (_CURRENT_CORE > _CPUS-1)
		{
			_CURRENT_CORE = _STARTING_CORE;
		}

		ostringstream oss;
		oss <<"ALGO CORE=" << _CURRENT_CORE << " MAX=" << _CPUS;
		LogMe(oss.str(),LogLevel::INFO);

		return _CURRENT_CORE;
	}

    inline int IncrementCore(int step)
	{
		int newcore = _CURRENT_CORE + step;

		if (newcore >= _CPUS)
			newcore = _STARTING_CORE;

		_CURRENT_CORE = newcore;

		LogMe("SETTING ALGO CORE TO " + std::to_string(_CURRENT_CORE),LogLevel::INFO);

		return _CURRENT_CORE;
	};

    inline std::vector<KTN::OrderPod> GetRecoveredOrdersForAlgo(const std::string& guid) {
        std::vector<KTN::OrderPod> orders;
    
        auto it = _mapSuspenseOrds.find(guid);
        if (it != _mapSuspenseOrds.end()) {
            for (const auto& kv : it->second) {
                KTN::OrderPod ord = kv.second;
                ord.hasMeta = false; // Reset the hasMeta flag b/c we are effectively transferring ownership
                orders.push_back(ord);
            }
            // Clear the map for this algo after retrieving all orders
            it->second.clear();
        }
    
        return orders;
    }

	void onAlgoMessage(const Instruction &cmd);
	void onClientHeartbeat(const Instruction &cmd, const std::string &payload);
	void checkHeartbeats(const akl::stacker::Time::TimestampType now);

private:
    ZmqSubscriberV5*  _zmq;
    IExchangeSender* _sess;
    std::unordered_map<std::string, AlgoBaseV3*> _algos;
    tbb::concurrent_queue<Instruction> _qcmds;
    std::atomic<bool> _EVENT_WAITING;
    std::atomic<bool> _ACTIVE;
    std::set<string> _launched;
    int _STARTING_CORE;
    int _CURRENT_CORE;
    int _CPUS;
    std::string _SOURCE;
    std::string _ARCHETYPE;
    int _cpuCount;
    bool _DEBUG_ME;
    bool _verboseLog { false };
    bool killswitchEnabled { false };

    //We prefer this to a queue for the use case of when we cancel an order and the algo is not running. We can just pop the order off the map.
    std::unordered_map<std::string, std::unordered_map<int, KTN::OrderPod>> _mapSuspenseOrds;

	using HeartbeatTimeMapType = std::unordered_map<std::string, akl::stacker::Timestamp>;
	HeartbeatTimeMapType heartbeatTimeMap;
	static constexpr akl::stacker::Duration HEARTBEAT_TIMEOUT = akl::stacker::Seconds(5);
	akl::stacker::Timestamp lastHeartbeatCheckTime { akl::stacker::Time::Min() };
	static constexpr akl::stacker::Duration HEARTBEAT_CHECK_INTERVAL = akl::stacker::Millis(250);

};

} /* namespace KTN */

#endif /* ALGOSERVICEBROKERV3_HPP_ */
