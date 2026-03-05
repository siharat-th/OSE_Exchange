//============================================================================
// Name        	: LogSvc.cpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Feb 3, 2023 1:50:50 PM
//============================================================================

#include <Loggers/LogSvc.hpp>

LogSvc::LogSvc(const string& owner, const string& source)
	:_LOGMSGS(1000),_RPT_ACTIVE(false),_OWNER(owner),_SOURCE(source)
{
	ChangeName(owner, source);
	StartLoggingThread();
}

LogSvc::~LogSvc() {
	_RPT_ACTIVE = false;
}

void LogSvc::Start()
{

}

void LogSvc::Stop()
{
	_RPT_ACTIVE = false;
}

void LogSvc::LogMe(const LogMsg& logmsg)
{
	_LOGMSGS.enqueue(logmsg);
}

void LogSvc::LogMe(const string& msg, LogLevel::Enum loglevel)
{
	_LOGPAIRS.push(std::make_pair(msg, loglevel));
}

void LogSvc::StartLoggingThread()
{
	LogMsg entry = {};
	entry.msg = "CREATING LOGGING SERVICE THREAD: OWNER=" + _OWNER + " SOURCE=" + _SOURCE;
	entry.loglevel = LogLevel::OK;
	LogMe(entry);

	CreateAndRun("logsvc", 1);
}

void LogSvc::Run()
{
	

	//sleep for 500 millis
	int isleepus = 5000;//500 * 1000;

	_RPT_ACTIVE.store(true);
	while(_RPT_ACTIVE.load(std::memory_order_relaxed))
	{
		if(_LOGMSGS.size_approx() > 0)
		{
			string owner = _OWNER;
			LogMsg logmsg = {};
			while(_LOGMSGS.try_dequeue(logmsg)){
				if (logmsg.msg.length() == 0)
					continue;
			
				switch(logmsg.loglevel)
				{
					case LogLevel::ERROR:
						KT01_LOG_ERROR(owner, logmsg.msg);
						break;
					case LogLevel::WARN:
						KT01_LOG_WARN(owner, logmsg.msg);
						break;
					case LogLevel::INFO:
						KT01_LOG_INFO(owner, logmsg.msg);
						break;
					case LogLevel::DEBUG:
						KT01_LOG_DEBUG(owner, logmsg.msg);
						break;
					case LogLevel::OK:
						KT01_LOG_OK(owner, logmsg.msg);
						break;
					case LogLevel::HIGHLIGHT:
						KT01_LOG_HIGHLIGHT(owner, logmsg.msg);
						break;
					default:
						KT01_LOG_INFO(owner, logmsg.msg);
						break;
				}
			}

		}

		if(!_LOGPAIRS.empty())
		{
			string owner = _OWNER;
			std::pair<std::string, LogLevel::Enum> msg;
			while(_LOGPAIRS.try_pop(msg))
			{
				LogMsg logmsg = {};
				logmsg.msg = msg.first;
				logmsg.loglevel = msg.second;

				switch(logmsg.loglevel)
				{
					case LogLevel::ERROR:
						KT01_LOG_ERROR(owner, logmsg.msg);
						break;
					case LogLevel::WARN:
						KT01_LOG_WARN(owner, logmsg.msg);
						break;
					case LogLevel::INFO:
						KT01_LOG_INFO(owner, logmsg.msg);
						break;
					case LogLevel::DEBUG:
						KT01_LOG_DEBUG(owner, logmsg.msg);
						break;
					case LogLevel::OK:
						KT01_LOG_OK(owner, logmsg.msg);
						break;
					case LogLevel::HIGHLIGHT:
						KT01_LOG_HIGHLIGHT(owner, logmsg.msg);
						break;
					default:
						KT01_LOG_INFO(owner, logmsg.msg);
						break;
				}
			}

		}
		usleep(isleepus);
	}

}

