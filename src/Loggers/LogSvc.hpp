//============================================================================
// Name        	: LogSvc.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Feb 3, 2023 1:50:50 PM
//============================================================================

#ifndef SRC_LOGGERS_LOGSVC_HPP_
#define SRC_LOGGERS_LOGSVC_HPP_

#include <KT01/Core/Log.hpp>
#include <Loggers/LogStructs.hpp>

#include <Maps/SPSCRingBuffer.hpp>
using namespace KTN::Core;

#include <KT01/Core/NamedThread.hpp>

#include <tbb/concurrent_queue.h>

/**
 * @class LogSvc
 * @brief The LogSvc class is responsible for logging messages. This is a thread-safe class 
 * that offloads messages to a ring buffer and then logs them to the console or another dstination
 */
class LogSvc : public NamedThread
{
public:
	/**
  * @brief Constructs a LogSvc object with the specified owner and source.
  * @param owner The owner of the LogSvc object.
  * @param source The source of the LogSvc object.
  */
	LogSvc(const string& owner, const string& source);

	/**
  * @brief Destroys the LogSvc object.
  */
	virtual ~LogSvc();

	/**
  * @brief Changes the name and source of the LogSvc object.
  * @param newname The new name of the LogSvc object.
  * @param source The new source of the LogSvc object.
  */
	void ChangeName(const string& newname, const string& source)
	{
		_OWNER = newname;
		_SOURCE = source;
	}

	/**
  * @brief Starts the logging service.
  */
	void Start();

	void Stop();

	/**
  * @brief Logs the specified LogMsg object.
  * @param msg The LogMsg object to be logged.
  */
	void LogMe(const LogMsg& msg);

	/**
  * @brief Logs the specified string into a queue of strings for logmsgs to be processed.
  * @param msg The LogMsg object to be logged.
  */
	void LogMe(const string& msg, LogLevel::Enum level = LogLevel::INFO);

private:
	/**
  * @brief The main execution function of the LogSvc object.
  */
	void Run() override;

	/**
  * @brief Starts the logging thread.
  */
	void StartLoggingThread();

	SPSCRingBuffer<LogMsg> _LOGMSGS;
	tbb::concurrent_queue<std::pair<string, LogLevel::Enum>> _LOGPAIRS;
	std::atomic<bool> _RPT_ACTIVE;

	string _OWNER;
	string _SOURCE;
};

#endif /* SRC_LOGGERS_LOGSVC_HPP_ */
