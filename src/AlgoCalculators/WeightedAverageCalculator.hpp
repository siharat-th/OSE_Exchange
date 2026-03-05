//============================================================================
// Name        	: WeightedAverage.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 1, 2023 10:21:50 AM
//============================================================================

#ifndef SRC_ALGOCALCULATORS_WEIGHTEDAVERAGECALCULATOR_HPP_
#define SRC_ALGOCALCULATORS_WEIGHTEDAVERAGECALCULATOR_HPP_

#include <string>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <iostream>
#include <unordered_map>

#include <KT01/Core/Log.hpp>
#include <Loggers/LogEnums.hpp>

#include <AlgoCalculators/TimerCallback.hpp>



using namespace std;

/**
 * @class WeightedAverageCalculator
 * @brief Calculates the weighted average based on price and size.
 */
class WeightedAverageCalculator {
public:
    /**
     * @brief Constructs a WeightedAverageCalculator object.
     * @param cb The TimerCallback object.
     */
    WeightedAverageCalculator(TimerCallback & cb);

    /**
     * @brief Performs a test.
     */
    void Test();

    /**
     * @brief Starts the calculation.
     */
    void Start();

    /**
     * @brief Stops the calculation.
     */
    void Stop();

    /**
     * @brief Sets the start and end time for the calculation.
     * @param startTime The start time in string format.
     * @param endTime The end time in string format.
     * @return True if the start and end time are successfully set, false otherwise.
     */
    bool setStartEndTime(const std::string& startTime, const std::string& endTime);

    /**
     * @brief Calculates the weighted average based on the given index, price, and size.
     * @param index The index.
     * @param price The price.
     * @param size The size.
     */
    void calculate(int index, int64_t price, int32_t size);

    /**
     * @brief Gets the current time.
     * @return The current time in string format.
     */
    std::string getCurrentTime();

    /**
     * @brief Gets the weighted average based on the given index.
     * @param index The index.
     * @return The weighted average.
     */
    double getWavg(int index);

    /**
     * @brief Gets the volume based on the given index.
     * @param index The index.
     * @return The volume.
     */
    double getVolume(int index);

    /**
     * @brief Gets the count based on the given index.
     * @param index The index.
     * @return The count.
     */
    double getCount(int index);

    /**
     * @brief Checks if the timer is triggered.
     * @return True if the timer is triggered, false otherwise.
     */
    bool Triggered();

    /**
     * @brief Checks if the calculation is started.
     * @return True if the calculation is started, false otherwise.
     */
    bool Started();

private:
    /**
     * @brief Logs the given message.
     * @param szMsg The message to log.
     * @param iColor The color of the log message.
     */
    void LogMe(std::string szMsg, LogLevel::Enum level = LogLevel::Enum::INFO);

    /**
     * @brief Formats the given time.
     * @param currentTime The current time.
     * @return The formatted time in string format.
     */
    std::string formatTime(std::time_t currentTime);

    /**
     * @brief The timer thread function.
     * @param arg The argument.
     */
    static void *timer_thread(void * arg);

    /**
     * @brief The timer loop function.
     */
    void TimerLoop();

    /**
     * @brief Checks if the current time has exceeded the end time.
     * @param endTime The end time.
     * @return True if the current time has exceeded the end time, false otherwise.
     */
    bool hasCurrentTimeExceededChatGPTSucks(const std::chrono::high_resolution_clock::time_point& endTime);

    /**
     * @brief Parses the given time string and returns a high_resolution_clock::time_point.
     * @param timeString The time string to parse.
     * @return The high_resolution_clock::time_point.
     */
    inline std::chrono::high_resolution_clock::time_point parseTimeString(const std::string& timeString) {
        std::tm tm = {};
        std::istringstream ss(timeString);
        ss >> std::get_time(&tm, "%H:%M:%S");

        // Convert hours, minutes, seconds to nanoseconds
        auto nanoseconds = (tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec) * 1'000'000'000LL;

        // Create a high_resolution_clock::duration with the calculated nanoseconds
        std::chrono::high_resolution_clock::duration duration(nanoseconds);

        // Create a high_resolution_clock::time_point using the calculated duration
        std::chrono::high_resolution_clock::time_point timePoint(duration);

        return timePoint;
    }

private:
    struct wavelet
	{
					double wavg;
					double wsum;
					int volume;
					int count;
	};

    TimerCallback& _cb;
    volatile bool _bActive;
    volatile bool _bTimerTriggered;
    std::unordered_map<int, wavelet> _waves;

    std::chrono::high_resolution_clock::time_point EndTime;
    std::chrono::high_resolution_clock::time_point StartTime;

    string _sttime;
    string _endtime;
};

#endif /* SRC_ALGOCALCULATORS_WEIGHTEDAVERAGECALCULATOR_HPP_ */
