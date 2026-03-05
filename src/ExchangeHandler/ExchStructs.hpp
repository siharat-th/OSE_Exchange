//============================================================================
// Name        	: ExchStructs.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Jan 31, 2024 10:20:23 AM
//============================================================================

#ifndef SRC_EXCHANGEHANDLERS2_EXCHSTRUCTS_HPP_
#define SRC_EXCHANGEHANDLERS2_EXCHSTRUCTS_HPP_

#include <string>
#include <atomic>

using namespace std;

struct ExchState
{
	bool MEASURE;
	int ALGO_AFFINITY;
	int processingThreadAffinity;
	std::string FILENAME;
	std::string SOURCE;
	std::string EXCHNAME;
	std::string KTNACCT;
	std::string ORG;
	std::string STATEFILE_DIR;

	std::atomic<bool> EVENT_WAITING;
	std::atomic<bool> ALGO_WAITING;
	std::atomic<bool> ACTIVE;
	std::atomic<bool> ALGOS_IN_USE;

	bool USE_PRIMARY;
	uint64_t EVENTPOLLMAX;
	bool USE_AUDIT;
	bool logToScreen;
	int KEEPALIVE;
	int KEEPALIVE_BUFFERMS;
	int HEARTBEAT_SECS;


	bool RetransOnNewUUID;

	bool DebugHmac;
	bool DebugRecvBytes;
	bool DebugSession;
	bool DebugAppMsgs;
	bool DebugSessMsgs;
	bool DebugGapDetection;
	bool SpinProcessingThread;

	//tcp handler version
	int TCP_VERSION;

	//for polling:
	uint64_t CHECK_EVENTS_TRIG;
	uint64_t CHECK_EVENT_LOOP_COUNT;
	uint64_t LOOPER;
	uint64_t LOOP_TRIG;
	int SEGCNT;
};

struct msgrate
{
	long sent;
	long volume;
};

struct msgevents
{
	long msgtype;
	long count;
};







#endif /* SRC_EXCHANGEHANDLERS2_EXCHSTRUCTS_HPP_ */
