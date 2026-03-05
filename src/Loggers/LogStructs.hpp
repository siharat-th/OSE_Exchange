//============================================================================
// Name        	: LogStructs.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Dec 24, 2023 9:51:15 AM
//============================================================================

#ifndef SRC_LOGGERS_LOGSTRUCTS_HPP_
#define SRC_LOGGERS_LOGSTRUCTS_HPP_
#include <iostream>
#include <string>
#include <sstream>

#include <Loggers/LogEnums.hpp>

using namespace std;

/**
 * @brief Struct representing a log message.
 */
struct LogMsg
{
	string topic; /**< The topic of the log message. */
	string msg; /**< The content of the log message. */
	//int color; /**< The color associated with the log message. */
	LogLevel::Enum loglevel; /**< The log level of the message. */
	bool persist; /**< Flag indicating if the log message should be persisted. */
};

#endif /* SRC_LOGGERS_LOGSTRUCTS_HPP_ */
