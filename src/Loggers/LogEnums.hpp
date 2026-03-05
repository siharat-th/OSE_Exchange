//============================================================================
// Name        	: LogEnums.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Dec 24, 2023 9:45:58 AM
//============================================================================

#ifndef SRC_LOGGERS_LOGENUMS_HPP_
#define SRC_LOGGERS_LOGENUMS_HPP_

#pragma once

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

/**
 * @brief The LogLevel struct represents the different levels of log messages.
 */
struct LogLevel
{
	enum Enum : uint8_t
	{
     TRACE = 0, /**< Trace level log message */
     DEBUG = 1, /**< Debug level log message */
     INFO = 2, /**< Info level log message */
     WARN = 3, /**< Warning level log message */
     ERROR = 4, /**< Error level log message */
     CRITICAL = 5, /**< Critical level log message */
	 OK = 6,
	 HIGHLIGHT = 7
	};

	/**
  * @brief Converts the log level enum value to its corresponding string representation.
  * @param level The log level enum value.
  * @return The string representation of the log level.
  */
	static std::string toString(Enum level)
	{
		switch(level) {
			case LogLevel::TRACE:
				return "TRACE";
			case LogLevel::DEBUG:
				return "DEBUG";
			case LogLevel::INFO:
				return "INFO";
			case LogLevel::WARN:
				return "WARNING";
			case LogLevel::ERROR:
				return "ERROR";
			case LogLevel::CRITICAL:
				return "CRITICAL";
			case LogLevel::OK:
				return "OK";
			default:
				return "INFO";
		}
	}
};

#endif /* SRC_LOGGERS_LOGENUMS_HPP_ */
