//============================================================================
// Name        	: SimpleIntervalTimer.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Aug 1, 2023 10:21:50 AM
//============================================================================

#ifndef SRC_ALGOCALCULATORS_INTERVALTIMER_HPP_
#define SRC_ALGOCALCULATORS_INTERVALTIMER_HPP_

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
 * @brief The IntervalTimer class represents a timer that triggers at regular intervals.
 */
class IntervalTimer : public NamedThread
{
public:
	/**
  * @brief Constructs an IntervalTimer object.
  * @param cb The TimerCallback object to be called when the timer triggers.
  */
	IntervalTimer(TimerCallback & cb);

	/**
  * @brief Destroys the IntervalTimer object.
  */
	~IntervalTimer();

	/**
  * @brief Test function for the IntervalTimer.
  */
    void Test();

    /**
     * @brief Pauses or resumes the timer.
     * @param paused True to pause the timer, false to resume.
     */
    void Pause(bool paused);

    /**
     * @brief Starts the timer with the specified parameters.
     * @param startTime The start time of the timer in string format.
     * @param endTime The end time of the timer in string format.
     * @param intervalsecs The interval in seconds at which the timer triggers.
     * @param maxworksecs The maximum duration in seconds for the timer to perform its work.
     */
    void Start(const std::string& startTime, const std::string& endTime, int intervalsecs, int maxworksecs);

    /**
     * @brief Stops the timer.
     */
    void Stop();

    /**
     * @brief Updates the interval at which the timer triggers.
     * @param intervalsecs The new interval in seconds.
     */
    void UpdateInterval(int intervalsecs);

    /**
     * @brief Resets the timer interval.
     */
    inline void ResetInterval()
    {
					_RESET_INTERVAL.store(true, std::memory_order_acquire);
    }

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
     * @brief Checks if the timer has started.
     * @return True if the timer has started, false otherwise.
     */
    bool Started()
    {
					bool started = _ACTIVE.load(std::memory_order_relaxed);
					return started;
    }

private:
    /**
     * @brief Logs a message with the specified color.
     * @param szMsg The message to log.
     * @param iColor The color of the log message.
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

    /**
     * @brief Formats the given time in string format.
     * @param currentTime The time to format.
     * @return The formatted time string.
     */
    std::string formatTime(std::time_t currentTime);

    /**
     * @brief Runs the timer.
     */
    void Run();

    /**
     * @brief Gets the current time in string format.
     * @return The current time string.
     */
    std::string getCurrentTime();

    /**
     * @brief Validates the start time, end time, interval, and maximum work duration.
     * @param startTime The start time of the timer in string format.
     * @param endTime The end time of the timer in string format.
     * @param intervalms The interval in milliseconds at which the timer triggers.
     * @param maxworkms The maximum duration in milliseconds for the timer to perform its work.
     * @return True if the parameters are valid, false otherwise.
     */
    bool Validate(const std::string& startTime, const std::string& endTime, int intervalms, int maxworkms);

    /**
     * @brief Converts the given time string to microseconds since epoch.
     * @param timeStr The time string to convert.
     * @return The microseconds since epoch.
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
    std::atomic<bool> _TRIGGERED;
    std::atomic<bool> _PAUSED;
    std::atomic<bool> _RESET_INTERVAL;

    std::string _StartTime;
    std::string _EndTime;
    std::atomic<uint64_t> _TimerIntervalUS;
    int _TimerMaxWorkMS;
    uint64_t _LAST_SENT;

};

}
}
#endif /* SRC_ALGOCALCULATORS_INTERVALTIMER_HPP_ */
