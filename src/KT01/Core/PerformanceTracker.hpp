//============================================================================
// Name        	: PerformanceTracker.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 3, 2023 9:22:04 AM
//============================================================================

#ifndef SRC_CORE_PERFORMANCETRACKER_HPP_
#define SRC_CORE_PERFORMANCETRACKER_HPP_

#include <unordered_map>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

#include <KT01/Core/Log.hpp>

#include <KT01/Core/PerfTimer.hpp>

/**
 * @class PerformanceTracker
 * @brief Class for tracking performance measurements.
 */
class PerformanceTracker {
 public:
  /**
   * @brief Start a new performance measurement with the given name.
   * @param name The name of the performance measurement.
   */
  void startMeasurement(const std::string& name) {
		auto result = _mapPerf.emplace(name, name);
		result.first->second.start();
  }

  /**
   * @brief Stop the performance measurement with the given name.
   * @param name The name of the performance measurement.
   */
  void stopMeasurement(const std::string& name) {
    auto it = _mapPerf.find(name);
    if (it != _mapPerf.end()) {
      it->second.stop();
    }
  }

  /**
   * @brief Print the stats for all measurements.
   */
  void printMeasurements()  {
   if (_mapPerf.size() == 0)
    return;

	Log("**** LATENCY RESULTS *****");

	ostringstream oss;
	oss << std::left << std::setw(15) << "Name" << " | "
     << std::setw(8) << "Average" << " | "
     << std::setw(8) << "Median" << " | "
     << std::setw(8) << "Count" << " | "
     << std::setw(8) << "Min" << " | "
     << std::setw(8) << "Max";
	Log(oss.str());

	oss.str("");
	oss.clear();
	oss << std::setw(15) << "---------------" << " | "
      << std::setw(8) << "--------" << " | "
      << std::setw(8) << "--------"  << " | "
      << std::setw(8) << "--------"  << " | "
      << std::setw(8) << "--------"  << " | "
      << std::setw(8) << "--------" ;
	Log(oss.str());


    for ( auto& entry : _mapPerf) {
      Log(entry.second.Print());
    }

    Log(oss.str());
  }

  /**
   * @brief Clear all performance measurements.
   */
  void clear()
  {
   _mapPerf.clear();
  }

 private:
  /**
   * @brief Log a message with the specified color.
   * @param msg The message to log.
   * @param color The color of the log message.
   */
  void Log(string msg)
  {
    KT01_LOG_INFO(__CLASS__, msg);
  }

 private:
  std::unordered_map<std::string, PerfTimer> _mapPerf;
};


#endif /* SRC_CORE_PERFORMANCETRACKER_HPP_ */
