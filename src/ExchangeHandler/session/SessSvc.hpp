//============================================================================
// Name        	: LogSvc.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Feb 3, 2023 1:50:50 PM
//============================================================================

#ifndef SRC_EXCHANGEHANDLER_SESSION_SESSSVC_HPP_
#define SRC_EXCHANGEHANDLER_SESSION_SESSSVC_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h> // for usleep
#include <string>
#include <sstream>
#include <chrono>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

using namespace std;

#include <KT01/Core/NamedThread.hpp>
#include <KT01/Core/Log.hpp>
#include <tbb/concurrent_queue.h>


//FilePartCount=
//FirmID=KNR
//ID=BGL
//InSeqNum=1286
//Negotiated=true
//OutSeqNum=1029
//PreviousSeqNum=0
//PreviousUUID=0
//SessionCreationTime=20230615-14:58:33.317675017
//UUID=1686841113317894

struct SessStateMsg
{
	string sessid;
	string firmid;

	int segid;
	uint32_t seqin;
	uint32_t seqout;
	uint32_t prevseqin;
	uint32_t prevseqout;

	string sessioncreated;
	string updatetime;

	bool negotiated;

	uint64_t uuid;
	uint64_t previousuuid;

	uint64_t lastreqid;
};

/**
 * @brief The SessSvc class represents a CME session service.We use this service for 
 * persisting session state.
 */
class SessSvc : public NamedThread {
public:
	/**
  * @brief Constructs a SessSvc object.
  * @param sesid The session ID.
  * @param firmid The firm ID.
  * @param segid The segment ID.
  * @param keepalive The keep-alive value.
  */
	SessSvc(string sesid, string firmid, int segid, int keepalive, bool dbg);

	/**
  * @brief Destroys the SessSvc object.
  */
	virtual ~SessSvc();

	/**
  * @brief Starts the session service.
  */
	void Start();

	/**
  * @brief Persists the session state message.
  * @param msg The session state message to persist.
  */
	void Persist(const SessStateMsg& msg);

private:
	/**
  * @brief Logs a message.
  * @param szMsg The message to log.
  * @param iColor The color of the log message.
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

	/**
  * @brief Starts the logging thread.
  */
	void StartLoggingThread();

	/**
  * @brief The logging thread function.
  * @param arg The argument passed to the thread function.
  */
	//static void *logging_thread(void * arg);

	void Run() override;

private:
	tbb::concurrent_queue<SessStateMsg> _MSGS;
	std::atomic<bool> _ACTIVE;
	string _FILENAME;
	string _DIR;
	int _keepalive;
	string _sessid;
	string _firmid;
	int _segid;
	bool _DBG;
};

#endif /* SRC_LOGGERS_LOGSVC_HPP_ */
