//============================================================================
// Name        	: PLMonitor.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 5, 2023 9:39:06 AM
//============================================================================

#ifndef SRC_ALGOENGINES_PLMONITOR_HPP_
#define SRC_ALGOENGINES_PLMONITOR_HPP_
#pragma once

#include <memory>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <tbb/concurrent_unordered_map.h>
using namespace tbb;



#include <AlgoSupport/algo_structs.hpp>
#include <AlgoEngines/engine_structs.hpp>
#include <KT01/Core/Log.hpp>

#include <KT01/DataStructures/DepthBook.hpp>
using namespace KT01::DataStructures::MarketDepth;

#include <AlgoSupport/AlgoPLCallback.hpp>
#include <AlgoParams/RiskBase.hpp>
#include <AlgoParams/Position.hpp>

#include <tbb/concurrent_queue.h>
using namespace tbb;

#include <Reporters/AlgoPLReporter.hpp>
using namespace KTN::REPORTER;

/**
 * @class PLMonitor
 * @brief Class responsible for monitoring and managing profit and loss calculations for positions.
 */
class PLMonitor {
public:
	/**
  * @brief Constructor for PLMonitor class.
  * @param sender The AlgoPLCallback object used for sending profit and loss updates.
  * @param guid The unique identifier for the PLMonitor instance.
  */
	PLMonitor(AlgoPLCallback& sender, string guid);

	/**
  * @brief Destructor for PLMonitor class.
  */
	virtual ~PLMonitor();

	/**
  * @brief Starts the PLMonitor.
  */
	void Start();

	/**
  * @brief Stops the PLMonitor.
  */
	void Stop();

	/**
  * @brief Registers a position for profit and loss monitoring.
  * @param pos The Position object to register.
  */
	void Register(Position& pos);

	/**
  * @brief Deregisters a position from profit and loss monitoring.
  * @param pos The Position object to deregister.
  */
	void DeRegister(Position& pos);

	/**
  * @brief Adjusts the risk for the positions.
  * @param cmd The EngineCommand object containing the risk adjustment command.
  */
	void AdjustRisk(EngineCommand cmd);

	/**
  * @brief Handles market data updates.
  * @param md The algomd object containing the market data.
  */
	void onMarketData(const DepthBook& md);

	/**
  * @brief Checks if the PLMonitor is running.
  * @return True if the PLMonitor is running, false otherwise.
  */
	inline bool Running()
	{
		return _ACTIVE.load(std::memory_order_relaxed);
	}

	/**
  * @brief Prints the current positions.
  */
	void Print();

private:
	/**
  * @brief Logs a message.
  * @param szMsg The message to log.
  * @param iColor The color of the log message.
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

	/**
  * @brief Calculation thread function.
  */
	void calculation_thread();

	/**
  * @brief Prepares a position for profit and loss calculation.
  * @param pos The Position object to prepare.
  * @param isnew Flag indicating if the position is new.
  */
	void Prepare(Position& pos, bool isnew);

	/**
  * @brief Adjusts a position based on the given EngineCommand.
  * @param pos The Position object to adjust.
  * @param cmd The EngineCommand object containing the adjustment command.
  */
	void Adjust(Position& pos, EngineCommand& cmd);

	/**
  * @brief Prints the positions.
  * @param vpos The vector of positions to print.
  */
	void print_positions(vector<Position> vpos);

	/**
  * @brief Checks if market data for a symbol is available.
  * @param sym The algomd object representing the symbol.
  * @return True if market data is available, false otherwise.
  */
	bool GotMktData(DepthBook& sym);

	/**
  * @brief Checks if the current time is within the CME trading hours.
  * @return True if the current time is within the CME trading hours, false otherwise.
  */
	inline bool isTimeInCmeRange() {
     auto now = std::chrono::system_clock::now();
     auto currentTime = std::chrono::system_clock::to_time_t(now);

     std::tm* localTime = std::localtime(&currentTime);
     int hour = localTime->tm_hour;

     // Check if the time is between 17:00:00 and 18:00:00
     if (hour >= 17 && hour < 18) {
         return false;
     } else {
         return true;
     }
	}

	/**
  * @brief Gets the current time as a string.
  * @return The current time as a string in the format HH:mm:ss.
  */
	std::string getCurrentTime() {
     std::time_t currentTime = std::time(nullptr);
     std::tm* localTime = std::localtime(&currentTime);

     char buffer[9];  // Buffer for HH:mm:ss + null terminator

     std::strftime(buffer, sizeof(buffer), "%H:%M:%S", localTime);

     return std::string(buffer);
	}

	/**
  * @brief Converts minutes to microseconds.
  * @param minutes The number of minutes to convert.
  * @return The equivalent number of microseconds.
  */
	uint64_t minutesToMicroseconds(int minutes) {
     return static_cast<uint64_t>(minutes) * 60 * 1000000;
	}

	/**
  * @brief Gets the current time in microseconds.
  * @return The current time in microseconds.
  */
	uint64_t getCurrentTimeInMicroseconds() {
     auto now = std::chrono::high_resolution_clock::now();
     auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
     return duration.count();
	}

	/**
  * @brief Checks if the current time has exceeded the specified start time plus the specified time to live.
  * @param startTimeMicroseconds The start time in microseconds.
  * @param timeToLiveMicroseconds The time to live in microseconds.
  * @return True if the current time has exceeded the specified time, false otherwise.
  */
	bool isTimeExceeded(uint64_t startTimeMicroseconds, uint64_t timeToLiveMicroseconds) {
     uint64_t currentTimeMicroseconds = getCurrentTimeInMicroseconds();
     return currentTimeMicroseconds > (startTimeMicroseconds + timeToLiveMicroseconds);
	}

	/**
  * @brief Gets the time to live in microseconds.
  * @param startTimeMicroseconds The start time in microseconds.
  * @param timeToLiveMicroseconds The time to live in microseconds.
  * @return The time to live in microseconds.
  */
	int64_t getTimeToLive(uint64_t startTimeMicroseconds, uint64_t timeToLiveMicroseconds) {
		uint64_t currentTimeMicroseconds = getCurrentTimeInMicroseconds();
		int64_t ttl = (startTimeMicroseconds + timeToLiveMicroseconds) - currentTimeMicroseconds;
		return ttl;
	}

private:
	AlgoPLCallback& _cb;
	std::thread _calcthread;

	std::unique_ptr<AlgoPLReporter> _rpt;

	tbb::concurrent_queue<Position> _qpos;
	tbb::concurrent_queue<EngineCommand> _qengine;
	tbb::concurrent_queue<DepthBook> _qmd;

	std::atomic<bool> _ACTIVE;
	std::atomic<bool> _PRINTFLAG;

	string _GUID;
	static constexpr int _SLEEPUS = 10000;

};

#endif /* SRC_ALGOENGINES_PLMONITOR_HPP_ */
