//============================================================================
// Name        	: HeartbeatMonitor.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 21, 2023 1:15:52 PM
//============================================================================

#ifndef SRC_CORE_HEARTBEATMONITOR_HPP_
#define SRC_CORE_HEARTBEATMONITOR_HPP_

#include <iostream>
#include <pthread.h>
#include <atomic>
#include <unistd.h>
#include <sstream>

#include <KT01/Core/NamedThread.hpp>
#include <KT01/Core/Macro.hpp>

using namespace std;


namespace KTN{ namespace Session {

/**
 * @brief The HeartbeatMonitor class is responsible for monitoring the heartbeat of a system.
 */
class HeartbeatMonitor : public NamedThread
{
public:
	/**
  * @brief Constructs a new HeartbeatMonitor object.
  */
	HeartbeatMonitor();

	/**
  * @brief Destroys the HeartbeatMonitor object.
  */
	virtual ~HeartbeatMonitor();

	/**
  * @brief Starts the heartbeat monitoring with the specified parameters.
  * @param seconds The interval in seconds between each heartbeat.
  * @param segid The segment ID of the system.
  * @param bufferms The buffer time in milliseconds.
  */
	void Start(int seconds, int segid, int bufferms);

	/**
  * @brief Stops the heartbeat monitoring.
  */
	void Stop();

	/**
  * @brief Resets the heartbeat monitoring.
  */
	void Reset();

	/**
  * @brief Checks if the flag is set.
  * @return True if the flag is set, false otherwise.
  */
	inline bool IsFlagSet() {
		return atomic_flag_.load();
	}

	/**
  * @brief Resets the flag.
  * @return True if the flag was reset, false otherwise.
  */
	inline bool ResetFlag() {
		return reset_flag_.load();
	}

private:
    int heartbeat_seconds_;
    int segid_;
    int buffer_ms_;
    std::atomic<bool> atomic_flag_;
    std::atomic<bool> reset_flag_;
    pthread_t monitor_thread_;
    std::atomic<bool> _ACTIVE;
    uint64_t last_sent_time_;

    /**
     * @brief The Run method that runs the heartbeat monitoring logic.
     */
    void Run() override;

    /**
     * @brief Gets the current time in microseconds since the epoch.
     * @return The current time in microseconds since the epoch.
     */
    static uint64_t GetMicrosecondsSinceEpoch() {
					struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		return static_cast<uint64_t>(ts.tv_sec) * 1000000ULL + static_cast<uint64_t>(ts.tv_nsec) / 1000ULL;

//		uint64_t timestamp;
//		uint32_t low, high;
//		asm volatile("rdtsc" : "=a"(low), "=d"(high));
//		timestamp = static_cast<uint64_t>(low) | (static_cast<uint64_t>(high) << 32);
//		//MICROSECONDS
//		timestamp = timestamp / 1000UL;
//		return timestamp;

    }
};

}}

#endif /* SRC_CORE_HEARTBEATMONITOR_HPP_ */
