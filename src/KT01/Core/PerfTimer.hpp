#ifndef PERFTIMER_H
#define PERFTIMER_H
#pragma once
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <iomanip>
using namespace std;

#include <KT01/Core/Time.hpp>
using namespace KT01::Core;


/**
 * @class PerfTimer
 * @brief High-resolution performance counter.
 */
class PerfTimer
{
public:

    /**
     * @brief Constructs a PerfTimer object with the given name.
     * @param name The name of the PerfTimer.
     */
    PerfTimer(const std::string& name): name_(name), start_(0), end_(0), _started(false)
    {
        //from Time.hpp
        cpuspeed_ = GetCPUClockSpeed();
        obs_.reserve(10000);
    }

    /**
     * @brief Clears the measurements.
     */
    void Clear()
    {
        obs_.clear();
    }

    /**
     * @brief Starts the timer.
     * @note If the timer is already started, an error message will be printed.
     */
    void start()
    {
        if(_started)
        {
            cout <<"ERROR " << name_ << ": Timer start() called without corresponding stop()" << endl;
            return;
        }
        start_ = rdtsc();
        _started = true;
    }

    /**
     * @brief Stops the timer and records the elapsed time.
     */
    void stop()
    {
        end_ =   rdtsc();
        _started = false;
        obs_.push_back(end_ - start_);
    }

    /**
     * @brief Gets the current timestamp.
     * @return The current timestamp.
     */
    uint64_t Current()
    {
        return rdtsc();
    }

    /**
     * @brief Prints the measurements.
     * @return A string representation of the measurements.
     */
    std::string Print()
    {
        if (obs_.size() == 0)
        {
            string res =  name_ + ": No Measurements Found." ;
            return res;
        }
        //cout << "cpu=" << cpuspeed_ << endl;
        double cpumult = 1/cpuspeed_;

        double sum = 0;
        double max = -1;
        double min = 99999999;

        std::vector<uint64_t> sorted_measurements(obs_);
        std::sort(sorted_measurements.begin(), sorted_measurements.end());

        //lose the max:
        if (sorted_measurements.size() > 2)
            sorted_measurements.pop_back();

        double median = 0.0;
        if (sorted_measurements.size() % 2 == 0) {
          median = (sorted_measurements[sorted_measurements.size() / 2 - 1] +
                    sorted_measurements[sorted_measurements.size() / 2]) / 2.0;
        } else {
          median = sorted_measurements[sorted_measurements.size() / 2];
        }

        //new max.
        max = sorted_measurements[sorted_measurements.size()-1];
        min = sorted_measurements[0];

        int count = sorted_measurements.size();

        for (const auto& tick : sorted_measurements)
            sum += tick;

        double average = sum / count;

        average *= cpumult;
        max *= cpumult;
        min *= cpumult;
        median *= cpumult;

        ostringstream oss;

        int name_width = 15;
        int col = 8;
        oss  << std::setw(name_width) << name_ << " | "
            << std::setw(col) << average << " | "
            << std::setw(col) << median << " | "
            << std::setw(col) << count << " | "
            << std::setw(col) << min << " | "
            << std::setw(col) << max;

        return oss.str();
    }

private:
    std::string name_;
    uint64_t start_;
    uint64_t end_;
    std::vector<uint64_t> obs_;
    bool _started;
    double cpuspeed_;
};

#endif /*PERFORMANCECOUNTER_H*/
