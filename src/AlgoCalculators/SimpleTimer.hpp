//============================================================================
// Name        	: SimpleIntervalTimer.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Aug 1, 2023 10:21:50 AM
//============================================================================

#ifndef SRC_ALGOCALCULATORS_SIMPLETIMER_HPP_
#define SRC_ALGOCALCULATORS_SIMPLETIMER_HPP_

#include <string>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <iostream>

#include <KT01/Core/Log.hpp>
#include <KT01/Core/NamedThread.hpp>

#include <AlgoCalculators/TimerCallback.hpp>



using namespace std;

namespace KTN{
namespace ALGO{

/**
 * @brief The SimpleTimer class represents a simple timer that can be started, stopped, and paused.
 */
class SimpleTimer : public NamedThread
{
public:
	/**
  * @brief Constructs a SimpleTimer object with the given TimerCallback.
  * @param cb The TimerCallback object to be associated with the timer.
  */
	SimpleTimer(TimerCallback & cb);

	/**
  * @brief Destroys the SimpleTimer object.
  */
	~SimpleTimer();

	/**
  * @brief Pauses or resumes the timer.
  * @param paused If true, the timer will be paused. If false, the timer will be resumed.
  */
    void Pause(bool paused);

    /**
     * @brief Starts the timer with the specified start and end times.
     * @param startTime The start time of the timer in string format.
     * @param endTime The end time of the timer in string format.
     */
    void Start(const std::string& startTime, const std::string& endTime);

    /**
     * @brief Stops the timer.
     */
    void Stop();

    /**
     * @brief Checks if the timer has been triggered.
     * @return True if the timer has been triggered, false otherwise.
     */
    inline bool Triggered()
    {
					bool trig = _TRIGGERED.load(std::memory_order_relaxed);
					return trig;
    }

    /**
     * @brief Checks if the timer has been started.
     * @return True if the timer has been started, false otherwise.
     */
    bool Started()
    {
					bool started = _ACTIVE.load(std::memory_order_relaxed);
					return started;
    }

    /**
     * @brief Checks if the timer has expired.
     * @return True if the timer has expired, false otherwise.
     */
    bool Expired()
    {
					bool expired = _EXPIRED.load(std::memory_order_relaxed);
					return expired;
    }

private:
    /**
     * @brief Logs a message with the specified color.
     * @param szMsg The message to be logged.
     * @param iColor The color of the log message (default is LOG_WHITE).
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

    /**
     * @brief Formats the given time in string format.
     * @param currentTime The time to be formatted.
     * @return The formatted time as a string.
     */
    std::string formatTime(std::time_t currentTime);

    /**
     * @brief Runs the timer.
     */
    void Run();

    /**
     * @brief Gets the current time in string format.
     * @return The current time as a string.
     */
    std::string getCurrentTime();

    /**
     * @brief Validates the start and end times of the timer.
     * @param startTime The start time of the timer in string format.
     * @param endTime The end time of the timer in string format.
     * @return True if the start and end times are valid, false otherwise.
     */
    bool Validate(const std::string& startTime, const std::string& endTime);

    /**
     * @brief Converts the given time string to microseconds since epoch.
     * @param timeStr The time string to be converted.
     * @return The time in microseconds since epoch.
     */
    std::chrono::microseconds MicrosecondsSinceEpoch(const std::string& timeStr);

    /**
     * @brief Waits for the start time of the timer.
     * @param startTime The start time of the timer in string format.
     * @param endTime The end time of the timer in string format.
     */
    void waitForStart(const std::string& startTime, const std::string& endTime);

private:
    TimerCallback& _cb;
    std::atomic<bool> _ACTIVE;
    std::atomic<bool> _EXPIRED;
    std::atomic<bool> _TRIGGERED;
    std::atomic<bool> _PAUSED;

    std::string _StartTime;
    std::string _EndTime;

    uint64_t _LAST_SENT;

};

}
}
#endif /* SRC_ALGOCALCULATORS_SIMPLETIMER_HPP_ */
