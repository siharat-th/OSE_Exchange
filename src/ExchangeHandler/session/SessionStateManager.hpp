//============================================================================
// Name        	: SessionStateManager.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 15, 2023 3:53:14 PM
//============================================================================

#ifndef SRC_EXCHHANDLER_SessionStateManager_HPP_
#define SRC_EXCHHANDLER_SessionStateManager_HPP_

#pragma once

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <stdint.h>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include <tbb/concurrent_queue.h>

#include <KT01/Core/Settings.hpp>
using namespace std;

#include <KT01/Core/Log.hpp>
#include <KT01/Core/DateTimeHelpers.hpp>
using namespace KTN::Core;

#include <ExchangeHandler/session/SessSvc.hpp>

#include <ExchangeHandler/session/HeartbeatMonitor.hpp>
#include <ExchangeHandler/session/SessionEnums.hpp>
using namespace KTN::Session;

#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
using namespace KTN::NET;

// #include <exchsupport/cme/il3/IL3Helpers.hpp>
// #include <exchsupport/cme/il3/IL3Messages2.hpp>
// using namespace KTN::CME::IL3;


namespace KTN::Session
{
	/**
	 * @struct Gap
	 * @brief Represents a gap in the sequence of messages
	 */
	struct Gap {
		std::atomic<uint32_t> start;
		std::atomic<uint32_t> end;
		std::atomic<uint16_t> count;
		std::atomic<uint64_t> uuid;

		const int MAX_BATCH_SIZE = 2500;

		std::string print() {
			ostringstream oss;
			oss << "[Gap] Start=" << start.load() << " End=" << end.load()
						<< " Count=" << count.load() << " UUID=" << uuid.load();

			return oss.str();
		}
	};


	/**
	 * @struct CheckSequenceResult
	 * @brief Represents the result returned from CehckIncomingSequenceOK
	 */
	struct CheckSequenceResult
	{
		enum class ResultType { OK, Queued, Retransmit, ProcessThenRetransmit, Process, Fatal, Ignore, ProcessPrevUUID };
		ResultType type;
		uint32_t startSeq;
		uint32_t numMsgs;
		std::vector<RecoveredMessage> messages;
	};


/**
 * @class SessionStateManager
 * @brief Manages the state of a session
 */
class SessionStateManager {
public:
	/**
  * @brief Default constructor
  */
	SessionStateManager();

	/**
  * @brief Destructor
  */
	virtual ~SessionStateManager();

	/**
  * @brief Initializes the session state manager
  * @param sessionid The session ID
  * @param firmid The firm ID
  * @param segid The segment ID
  * @param hbsecs The heartbeat interval in seconds
  * @param hbbuffms The heartbeat buffer in milliseconds
  */
	void Init(string sessionid, string firmid, int segid, int hbsecs, int hbbuffms,  string statefiledir,
			bool debugGaps, bool debugSess, string exchname, string source, string org);

	/**
  * @brief Stops the session state manager
  */
	void Stop();

	/**
  * @brief Updates the session state manager
  */
	void Update();
	/**
  * @brief Handles the state change of the session
  * @param newState The new state of the session
  */
	void onStateChange(EnumSessionState::Enum newState);


	void PrintStatus();


	void GapDetected(uint32_t expected_seq, uint32_t incoming_seq, uint64_t uuid);


	/**
	 * @brief Checks the incoming sequence number of a message
	 * @param msg The incoming message
	 * @return The result of the sequence check
	 */
	CheckSequenceResult CheckIncomingSequenceOK(const KTNBuf8t& msg);

	/**
	 * @brief Queues a message for processing during retranmission
	 * @param msg The message to queue
	 */
	void QueueMessage(const KTNBuf8t& msg);

	/**
	 * @brief Drains the queue of messages
	 * @return The result of the sequence check
	 */
	CheckSequenceResult drain();

	void ResetSessionSeqNums()
	{
		_SEQ_INa.store(1, std::memory_order_relaxed);
		_SEQ_OUTa.store(1, std::memory_order_relaxed);
		Update();
	}

	void ResetSessionSeqNumsToZero()
	{
		_SEQ_INa.store(0, std::memory_order_relaxed);
		_SEQ_OUTa.store(0, std::memory_order_relaxed);
		Update();
	}

	void UpdateCurrentSessionValues(bool negotiated, uint64_t uuid, uint64_t prevuuid)
	{
		_NEGOTIATEDa.store(negotiated, std::memory_order_relaxed);
		_UUID = uuid;
		_PREVUUID = prevuuid;

		cout << "[UpdateCurrentSessionValues DEBUG] negotiated=" << negotiated
				<< " UUID=" << _UUID << " PREV_UUID=" << _PREVUUID << endl;

		Update();
	}
	

	//sets previous session values
	void UpdatePreviousSessionValues(uint64_t uuid, uint64_t prevuuid)
	{
		uint32_t seqin = SeqIn();
		uint32_t seqout = SeqOut();

		_PREVSEQ_INa.store(seqin, std::memory_order_relaxed);
		_PREVSEQ_OUTa.store(seqout, std::memory_order_relaxed);
		_UUID = uuid;
		_PREVUUID = prevuuid;

		Update();
	}



	/**
  * @brief Increments the outgoing sequence number by a specified number of sequences
  * @param numSeqs The number of sequences to increment by
  * @return The new outgoing sequence number
  */
	inline uint64_t IncSeqOut(int numseqs=1)
	{
		_SEQ_OUTa++;
		return _SEQ_OUTa.load(std::memory_order_relaxed);;
	}

	inline const string& Firmid() const {
		return _FIRMID;
	}

	inline void SetFirmid(const string &firmid) {
		_FIRMID = firmid;
		//Update();
	}

	inline uint64_t PrevUUID() const {
		return _PREVUUID;
	}

	inline void SetPrevUUID(uint64_t prevuuid) {
		_PREVUUID = prevuuid;
		_msg.previousuuid = _PREVUUID;
		//Update();
	}

	inline int Segid() const {
		return _SEGID;
	}

	inline void SetSegid(int segid) {
		_SEGID = segid;
		//Update();
	}


	//*** CURRENT SEQNUMS *******

	inline uint64_t SeqIn() const {
		return _SEQ_INa.load(std::memory_order_relaxed);
	}

	inline void SetSeqIn(uint32_t seqIn, bool persist=true) {
		_SEQ_INa.store(seqIn, std::memory_order_relaxed);
		//if (persist)
		Update();
	}

	inline uint64_t SeqOut() const {
		return _SEQ_OUTa.load(std::memory_order_relaxed);
	}

	inline uint64_t SeqOutAndAdvance() {
		uint64_t seq = _SEQ_OUTa.load(std::memory_order_relaxed);
		_SEQ_OUTa ++;
		return seq;
	}

	inline void SetSeqOut(uint32_t seqOut, bool persist) {
		_SEQ_OUTa.store(seqOut, std::memory_order_relaxed);
		if (persist)
			Update();
	}


	//***** PREVIOUS SEQNUMS ******
	inline uint64_t PrevSeqIn() const {
		return _PREVSEQ_INa.load(std::memory_order_relaxed);
	}

	inline void SetPrevSeqIn(uint32_t seqIn) {
	    _PREVSEQ_INa.store(seqIn, std::memory_order_relaxed);
		Update();
	}

	inline uint64_t PrevSeqOut() const {
		return _PREVSEQ_OUTa.load(std::memory_order_relaxed);
	}

	inline void SetPrevSeqOut(uint32_t seqOut) {
		_PREVSEQ_OUTa.store(seqOut, std::memory_order_relaxed);
		Update();
	}


	//***** SET NEGOTIATED STATE ******
	inline bool Negotiated() const {
		return _NEGOTIATEDa.load(std::memory_order_relaxed);
	}


	inline const string& Sessid() const {
		return _SESSID;
	}

	inline void SetSessid(const string &sessid) {
		_SESSID = sessid;
		//Update();
	}

	inline uint64_t Uuid() const {
		return _UUID;
	}

	inline void SetUuid(uint64_t uuid) {
		_UUID = uuid;
		_msg.uuid = _UUID;
		//Update();
	}

	inline bool HbTriggered()
	{
		return _hb.IsFlagSet();
	}

	inline void HbSent()
	{
		_HB_COUNT++;
		_hb.Reset();
	}

	inline uint64_t LastHb() const
	{
		return _LAST_HB;
	}

	//LEGACY....
	inline void SetLastHb(uint64_t hb)
	{
		_LAST_HB = hb;
		_HB_COUNT++;
	}

	inline uint64_t HbCount() const
	{
		return _HB_COUNT;
	}

	const bool Connected() const {
		return _CONNECTEDa.load(std::memory_order_relaxed);
	}


	inline uint64_t NewReqId()
	{
		_LAST_REQID ++;
		//Update();
		return _LAST_REQID;
	}

	inline uint64_t LastReqId()
	{
		return _LAST_REQID;
	}

	inline void SetReqId(uint64_t id)
	{
		_LAST_REQID = id;
	}

	inline void SetReplayComplete(bool complete)
	{
		_REPLAY_COMPLETE.store(complete, std::memory_order_relaxed);
	}
	
	inline bool ReplayComplete()
	{
		return _REPLAY_COMPLETE.load(std::memory_order_relaxed);
	}

	inline const string& GetFilename() const {
		return _FILENAME;
	}



private:
	/**
  * @brief Logs a message
  * @param szMsg The message to log
  * @param iColor The color of the log message
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

	//cme specific?
	inline uint32_t extractSeqNum(const void* buffer) {
	    // Cast the buffer to a pointer to uint32_t and dereference it.
	    // Only do this if you know the buffer is correctly aligned!
	    return *reinterpret_cast<const uint32_t*>(buffer);
	}

public:
	EnumSessionState::Enum CurrentState;

private:
	SessSvc* _log;
	string _FILENAME;
	string _NAME;
	HeartbeatMonitor _hb;
	SessStateMsg _msg;
	std::atomic<bool> _CONNECTEDa;
	std::atomic<bool> _NEGOTIATEDa;
	string _FIRMID;
	string _SESSID;
	int _SEGID;
	bool _DBG_GAP;
	bool _DBG_SESS;

	string _EXCHNAME;
	string _SOURCE;
	string _ORG;

	std::atomic<bool> _REPLAY_COMPLETE;

	uint64_t _UUID;
	std::atomic<uint32_t> _SEQ_INa;
	std::atomic<uint32_t> _SEQ_OUTa;

	uint64_t _PREVUUID;
	std::atomic<uint32_t> _PREVSEQ_INa;
	std::atomic<uint32_t> _PREVSEQ_OUTa;

	uint64_t _LAST_HB;
	uint64_t _HB_COUNT;
	uint64_t _LAST_REQID;



	//Gap Detection and Processing
	Gap gapInfo;
	tbb::concurrent_queue<RecoveredMessage> _qRecovIn;

	bool initialized { false };
};

}

#endif /* SRC_EXCHSUPPORT_CME_IL3_SESSIONSTATEMANAGER_HPP_ */
