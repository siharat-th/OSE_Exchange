//============================================================================
// Name        : SqlUtils.hpp
// Copyright   : Copyright (C) 2017-2024 Katana Financial
//============================================================================

#ifndef SRC_DATABASEPROCESSORS_SQLUTILS_HPP_
#define SRC_DATABASEPROCESSORS_SQLUTILS_HPP_

#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <sys/time.h>

using namespace std;

class SqlUtils {
public:
	static std::string vector_join(std::vector<std::string>& v, std::string token) {
		ostringstream result;
		for (auto ii = v.begin(); ii != v.end(); ++ii) {
			result << *ii;
			if (ii + 1 != v.end()) result << token;
		}
		return result.str();
	}

	static std::string GetTimeMillis()
	{
		timeval curTime;
		gettimeofday(&curTime, NULL);
		int micro = curTime.tv_usec;

		char buffer[80];
		strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", gmtime(&curTime.tv_sec));

		char currentTime[84] = "";
		sprintf(currentTime, "%s.%06d", buffer, micro);

		return currentTime;
	}
};

#endif /* SRC_DATABASEPROCESSORS_SQLUTILS_HPP_ */
