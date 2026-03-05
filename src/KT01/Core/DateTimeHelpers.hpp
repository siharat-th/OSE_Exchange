/*
 * DateTimeHelpers.hpp
 *
 *  Created on: Feb 18, 2025
 *      Author: rocky
 */

#ifndef SRC_CORE_DATETIMEHELPERS_HPP_
#define SRC_CORE_DATETIMEHELPERS_HPP_

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

namespace KTN{
namespace Core{

class DateTimeHelpers {
public:
	static string CurrentUTC()
	{
		using namespace std::chrono;

		// Get current time_point from system_clock
		auto now = system_clock::now();

		// Get duration since epoch
		auto duration_since_epoch = now.time_since_epoch();

		// Extract seconds and remaining nanoseconds using modulo
		std::chrono::seconds sec = duration_cast<std::chrono::seconds>(duration_since_epoch);
		auto nsec = duration_cast<nanoseconds>(duration_since_epoch % std::chrono::seconds(1)).count();

		// Convert to time_t for calendar time
		std::time_t now_time_t = system_clock::to_time_t(now);
		std::tm utc_tm;

		gmtime_r(&now_time_t, &utc_tm); // POSIX version

		// Format the date and time
		std::ostringstream oss;
		oss << std::put_time(&utc_tm, "%Y%m%d-%H:%M:%S")
			<< '.' << std::setw(9) << std::setfill('0') << nsec;

		return oss.str();
	}
};



}
}


#endif /* SRC_CORE_DATETIMEHELPERS_HPP_ */
