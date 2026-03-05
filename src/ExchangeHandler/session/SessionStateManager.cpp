//============================================================================
// Name        	: SessionStateManager.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 15, 2023 3:53:14 PM
//============================================================================

#include <ExchangeHandler/session/SessionStateManager.hpp>
#include <exchsupport/cme/il3/IL3Helpers.hpp>
#include <Notifications/NotifierRest.hpp>

using namespace KTN::Session;

SessionStateManager::SessionStateManager()
	:CurrentState(EnumSessionState::Disconnected),_NAME("SessMgr-X"),_DBG_GAP(false),_UUID(0),_PREVUUID(0),
	 _LAST_HB(0), _HB_COUNT(0),_LAST_REQID(0)
{
	_SEQ_INa.store(0, std::memory_order_relaxed);
	_SEQ_OUTa.store(0, std::memory_order_relaxed);
}

SessionStateManager::~SessionStateManager() {

}


void SessionStateManager::LogMe(std::string szMsg, LogLevel::Enum level)
{
	switch(level)
	{
		case LogLevel::ERROR:
			KT01_LOG_ERROR(_NAME, szMsg);
			break;
		case LogLevel::WARN:
			KT01_LOG_WARN(_NAME, szMsg);
			break;
		case LogLevel::INFO:
			KT01_LOG_INFO(_NAME, szMsg);
			break;
		case LogLevel::DEBUG:
			KT01_LOG_DEBUG(_NAME, szMsg);
			break;
		case LogLevel::OK:
			KT01_LOG_OK(_NAME, szMsg);
			break;
		case LogLevel::HIGHLIGHT:
			KT01_LOG_HIGHLIGHT(_NAME, szMsg);
			break;
		default:
			KT01_LOG_INFO(_NAME, szMsg);
			break;
	}
}

void SessionStateManager::PrintStatus()
{
	string status = (CurrentState == EnumSessionState::Established ) ? "OK" : "WARNING";
	LogLevel::Enum level = (CurrentState == EnumSessionState::Established ) ? LogLevel::OK : LogLevel::ERROR;
	if (CurrentState == EnumSessionState::Disconnected) level = LogLevel::ERROR;
	ostringstream oss;
	oss << "[" << status << "] Segment " << Segid() << ": "
			<< EnumSessionState::toString(CurrentState)
			<< " SeqIn=" << SeqIn() << " SeqOut=" << SeqOut()
			<< " UUID=" << _UUID << " PREV_UUID=" << _PREVUUID;

	LogMe(oss.str(),level);
}

void SessionStateManager::Init(string sessionid, string firmid, int segid, int hbsecs, int hbbuffms,  string statefiledir,
		bool debugGaps, bool debugSess, string exchname, string source, string org)
{
	_NAME = sessionid +"-" + std::to_string(segid) + "-mgr";

	_DBG_GAP = debugGaps;
	_DBG_SESS = debugSess;

	_FIRMID = firmid;
	_SESSID = sessionid;
	_SEGID =segid;

	_EXCHNAME = exchname;
	_SOURCE = source;
	_ORG = org;

	string dir = "./" + statefiledir + "/";

	ostringstream oss;
	oss << _SESSID <<"-" << _FIRMID << "-" << _SEGID << ".STATE";

	string filename = dir + oss.str();
	_FILENAME = oss.str();

	if (_DBG_SESS)
		cout << "[Sess " << _SEGID << "]: FILENAME=" << _FILENAME << endl;

	ifstream f(filename.c_str());

	if (f.good())
	{
		Settings sett = Settings(filename);

		string firmid = sett.getString("FirmID");
		string sessid = sett.getString("SessID");
		int segid = sett.getInteger("SegID");

		if (firmid != _FIRMID)
		{
			LogMe("ERROR: FIRM ID IN STATE FILE " + filename + " NOT MATCHING FIRM ID: " + _FIRMID, LogLevel::ERROR);
			return;
		}

		if (sessid != _SESSID)
		{
			LogMe("ERROR: SESSION ID IN STATE FILE " + filename + " NOT MATCHING SESS ID: " + _SESSID, LogLevel::ERROR);
			return;
		}

		if (segid != _SEGID)
		{
			ostringstream oss;
			 oss << segid;
			LogMe("ERROR: SEG ID IN STATE FILE " + filename + " NOT MATCHING SEG ID: " + oss.str(), LogLevel::ERROR);
			return;
		}

		_UUID = sett.getLong("UUID");
		uint32_t seqin = sett.getInteger("MsgSeqIn");
		uint32_t seqout = sett.getInteger("MsgSeqOut");
		_SEQ_INa.store(seqin, std::memory_order_relaxed);
		_SEQ_OUTa.store(seqout, std::memory_order_relaxed);


		bool neg = sett.getBoolean("Negotiated");
		_NEGOTIATEDa.store(neg, std::memory_order_relaxed);

		//Ok. Some explanation here
		/*
		 * If the session is already negotiated, that means we are NOT receiving a new UUID
		 * If the session is NOT negotiated, we WILL BE receiving a new UUID
		 *
		 * If we DON'T receive a new UUID, we don't need to update the stored values, as they should already be there
		 * If we DO receive a NEW UUID, we will want to store the OLD UUID's seqnums
		 */
		_PREVUUID = sett.getLong("PreviousUUID");

		uint32_t prevseqin = sett.getInteger("PrevMsgSeqIn");
		uint32_t prevseqout = sett.getInteger("PrevMsgSeqOut");

		_PREVSEQ_INa.store(prevseqin, std::memory_order_relaxed);
		_PREVSEQ_OUTa.store(prevseqout, std::memory_order_relaxed);

		_LAST_REQID = sett.getLong("LastReqId");

		if (_DBG_SESS)
			cout << "[Sess " << _SEGID << "]: Negotiated=" << ((neg == true)? "TRUE" : "FALSE")
				<< " UUID=" << _UUID << " PrevUUID=" << _PREVUUID << " SEQ_IN=" << seqin << " PREV_SEQ_IN=" << prevseqin << endl;


	}
	else
	{
		LogMe("CREATING NEW STATE FILE. NO EXSITING FILE FOUND FOR " + filename);
	}
	
	if (_LAST_REQID == 0)
		_LAST_REQID = (segid * 1000000);

	//legacy
	if (_LAST_REQID < 1000000)
		_LAST_REQID = (segid * 1000000);

	if (!initialized)
	{
		_log = new SessSvc(_SESSID, _FIRMID, _SEGID, 0, _DBG_SESS);

		_msg = {};
		_msg.firmid = _FIRMID;
		_msg.segid = _SEGID;
		_msg.sessid = _SESSID;
		_msg.uuid = _UUID;
		_msg.previousuuid = _PREVUUID;
		_msg.lastreqid = _LAST_REQID;
		_msg.sessioncreated = DateTimeHelpers::CurrentUTC();

		_hb.Start(hbsecs, segid, hbbuffms);

		initialized = true;
	}

	Update();
}

void SessionStateManager::Stop()
{
	Update();
	usleep(10000);
	_hb.Stop();
	onStateChange(EnumSessionState::Disconnected);
}

void SessionStateManager::Update()
{
	// these are all set elsewhere so saving some cycles
//	_msg.firmid = _FIRMID;
//	_msg.segid = _SEGID;
//	_msg.sessid = _SESSID;
	_msg.uuid = _UUID;
	_msg.previousuuid = _PREVUUID;

	_msg.seqin = _SEQ_INa.load(std::memory_order_relaxed);
	_msg.seqout = _SEQ_OUTa.load(std::memory_order_relaxed);
	_msg.prevseqin = _PREVSEQ_INa.load(std::memory_order_relaxed);
	_msg.prevseqout = _PREVSEQ_OUTa.load(std::memory_order_relaxed);
	_msg.lastreqid = _LAST_REQID;

	_msg.negotiated = _NEGOTIATEDa.load(std::memory_order_relaxed);

	if (_DBG_SESS)
		cout << "[Update " << _FILENAME << "]: Negotiated=" << ((_msg.negotiated == true)? "TRUE" : "FALSE")
			<< " UUID=" << _msg.uuid << " PrevUUID=" << _msg.previousuuid << " SEQ_IN=" << _msg.seqin
			<< " PREV_SEQ_IN=" << _msg.prevseqin << endl;



	_log->Persist(_msg);
}


void SessionStateManager::onStateChange(EnumSessionState::Enum newState)
{
	EnumSessionState::Enum prev = CurrentState;
	CurrentState = newState;

	if (newState == EnumSessionState::Established)
		_CONNECTEDa.store(true, std::memory_order_relaxed);
	else
		_CONNECTEDa.store(false, std::memory_order_relaxed);

	if (newState == EnumSessionState::Disconnected)
	{
		ostringstream oss;
		oss << "SEG " << Segid()  <<"!!SESSION STATE: DISCONNECTED!!";
		LogMe(oss.str(), LogLevel::ERROR);

		// NOTE: Notification now handled in CfeGT4::onStateChange() to control frequency
		// KTN::notify::NotifierRest::NotifyError(_EXCHNAME, _SOURCE, _ORG, oss.str());
	}

	if (newState == prev)
	{
		LogMe("STATE HAS NOT CHANGED BUT RECEIVED STATE CHANGE MSG");
		return;
	}

	LogLevel::Enum level = LogLevel::WARN;

	if (newState == EnumSessionState::Disconnected)
		level = LogLevel::ERROR;
	if (newState == EnumSessionState::Established)
		level = LogLevel::INFO;

	if (newState == EnumSessionState::WaitingForRetransmission)
		level = LogLevel::INFO;

	ostringstream oss;
	oss << "SEG " << Segid() << ": SESSION STATE CHANGED TO "
			<< EnumSessionState::toString(newState);
			//<<" FROM " << EnumSessionState::toString(prev);
	LogMe(oss.str(), level);

	if (_DBG_SESS)
		cout << oss.str() << endl;

}

/********** GAP DETECTION & PROCESSING ***************/

void SessionStateManager::GapDetected(uint32_t expected_seq, uint32_t incoming_seq, uint64_t uuid)
{
	gapInfo.start.store(expected_seq);
	gapInfo.count.store(incoming_seq - expected_seq);
	gapInfo.end.store(incoming_seq - 1);
	gapInfo.uuid.store(uuid);

	ostringstream oss;

	oss << "Start=" << gapInfo.start.load() << " End=" << gapInfo.end.load()
							<< " Count=" << gapInfo.count.load() << " UUID=" << gapInfo.uuid.load();
	if (_DBG_GAP)
		cout << "[Gap Detected] " << oss.str() << endl;
	else
	{
		LogMe("GAP DETECTED: " + oss.str(),LogLevel::HIGHLIGHT);
	}

	onStateChange(EnumSessionState::WaitingForRetransmission);
}


/*
	NOTE: This function is SPECIFIC for CME iLInk3.  
	It will not work for other exchanges, as you see the cast for finding the seqnum
	is specific to the iLink3 protocol.

	TODO: Make this more generic, or at least make it a template function
*/

// Called for each arriving message.
    // Note: This function only queues messages (or processes them immediately if in order)
    // and never calls drain() directly. (if we were threading the drain function- it's a thought.
CheckSequenceResult SessionStateManager::CheckIncomingSequenceOK(const KTNBuf8t& msg) {

	CheckSequenceResult result = {};

	uint32_t cast = extractSeqNum(msg.buffer);
	if (_DBG_GAP)
		cout << "[CheckIncoming] message " << msg.seqnum << " received. cast=" << cast << endl;

	if (msg.seqnum != cast)
	{
		if (_DBG_GAP)
			cout << "[Error-Ignore] message " << msg.seqnum << " sequence does not match cast=" << cast << " len=" << msg.buffer_length << std::endl;
		result.type = CheckSequenceResult::ResultType::Ignore;
		return result;
	}

	uint32_t currExpected = SeqIn() + 1;

	if (CurrentState == EnumSessionState::Established) {


		if (msg.seqnum > currExpected) {
			if (_DBG_GAP)
				std::cout << "[Gap detected] expected " << currExpected
						<< " but received " << msg.seqnum << "\n";

			// Set gap range and change state: missing messages from expectedSeq up to msg.seq - 1.
			GapDetected(currExpected, msg.seqnum, _UUID);

			// Queue the out-of-order message.
			QueueMessage(msg);

			result.type = CheckSequenceResult::ResultType::Retransmit;
			result.startSeq = gapInfo.start.load();
			result.numMsgs = gapInfo.count.load();

			return result;
		}
		else { // msg.seq < currExpected: duplicate/late.
			if (_DBG_GAP)
				std::cout << "[Fatal] message seq " << msg.seqnum
						<< " is LESS THAN expected seq " << currExpected << "\n";

			result.type = CheckSequenceResult::ResultType::Fatal;
			return result;
		}
	}
	else { // state == WaitingForRetransmission.
		// Always queue messages in this state.
		QueueMessage(std::move(msg));
		// If this message is the highest in the current gap, set the flag
		// so that the background thread will drain the queue.
		if (msg.seqnum == gapInfo.end.load()) {
			if (_DBG_GAP)
				std::cout << "[Flag] Highest message received: " << msg.seqnum << ". Flagging for drain.\n";
			return drain();
		}

		//so this is an edge case.  if we have a gap that's > max batch size (2500),
		//we need to continue queueing this and send a new retrans
		//[of course, if we missed 2500 messages, I'm thinking we have bigger problems, but hey]
		if (msg.seqnum == gapInfo.start.load() + gapInfo.MAX_BATCH_SIZE-1)
		{
			//queue this one: 12771  (10272 + 2500)
			QueueMessage(std::move(msg));

			//start = this seq + 1: 12772
			//end = same end seq as curent gap: 12822
			uint32_t newstart = msg.seqnum + 1;
			//uint32_t expected_end_seq = gapInfo.end + 1;  //we add 1 because gap detect subtracts 1.
			uint32_t current_count = gapInfo.count.load(std::memory_order_relaxed);
			current_count -= gapInfo.MAX_BATCH_SIZE;

			//or:
			gapInfo.start.store(newstart);
			//kee the end
			gapInfo.count.store(current_count);

			if (_DBG_GAP)
				cout << "[Gap Processing] ***Edge Case: Maximum count of " << gapInfo.MAX_BATCH_SIZE << " received with seq=" << msg.seqnum
					<< ". Current gap is modified: new start=" << gapInfo.start.load() << " end (unmodified)=" << gapInfo.end.load()
					<< " new count (msgs remaining to process)=" << gapInfo.count.load() << ". Sending Retransmit."<< endl;

			result.type = CheckSequenceResult::ResultType::Retransmit;
			result.startSeq = gapInfo.start.load();
			result.numMsgs = gapInfo.count.load();

			return result;

		}

		//If we get here then we didn't drain, meaning we are still waiting for retransmission
		//messages to come in. So we return a queued result.
		result.type = CheckSequenceResult::ResultType::Queued;
		return result;
	}

}


void SessionStateManager::QueueMessage(const KTNBuf8t& msg) {
    RecoveredMessage r;
    r.seqnum = msg.seqnum;
    r.msgtype = msg.msgtype;
    r.msg_length = msg.msg_length;
    r.payload.assign(msg.buffer, msg.buffer + msg.buffer_length);

    _qRecovIn.push(std::move(r));
}


// Drain method:
// Pulls all queued messages, sorts them, processes contiguous messages from the
// current expected sequence (marking those processed from the queue with [Q]),
// and stops immediately when a gap is encountered.
// Any unprocessed messages are requeued.
CheckSequenceResult SessionStateManager::drain() {
    CheckSequenceResult result;
    std::vector<RecoveredMessage> msgs;

    RecoveredMessage rmsg;
    while (_qRecovIn.try_pop(rmsg)) {
        if (_DBG_GAP) {
            uint32_t cast = CME::IL3::IL3Helpers::extractSeqNum(rmsg.payload.data());
            std::cout << "[Dequeue] seq=" << rmsg.seqnum << " cast=" << cast << std::endl;
        }
        msgs.push_back(std::move(rmsg));
    }

    if (msgs.empty()) {
        if (_DBG_GAP)
            std::cout << "[Drain] No messages to drain.\n";
        result.type = CheckSequenceResult::ResultType::Queued;
        return result;
    }

    if (gapInfo.uuid.load(std::memory_order_acquire) != _UUID) {
        if (_DBG_GAP)
            std::cout << "[Drain] Previous UUID Gap. Sending right away" << std::endl;

        result.type = CheckSequenceResult::ResultType::ProcessPrevUUID;

        for (const auto& m : msgs) {
            if (_DBG_GAP) {
                uint32_t cast = CME::IL3::IL3Helpers::extractSeqNum(m.payload.data());
                std::cout << "[Drain] PrevUUID Push Back message " << m.seqnum << " cast=" << cast << std::endl;
            }

            RecoveredMessage r = m;
            r.prev_uuid_dupe = true;
            r.poss_dupe = true;
            result.messages.push_back(std::move(r));
        }

        return result;
    }

    std::sort(msgs.begin(), msgs.end(), [](const RecoveredMessage& a, const RecoveredMessage& b) {
        return a.seqnum < b.seqnum;
    });

    uint32_t localExpected = SeqIn() + 1;
    size_t i = 0;

    for (; i < msgs.size(); ++i) {
        if (msgs[i].seqnum == localExpected) {
            uint32_t cast = CME::IL3::IL3Helpers::extractSeqNum(msgs[i].payload.data());
            if (_DBG_GAP)
                std::cout << "[Drain] Push Back message " << msgs[i].seqnum << " cast=" << cast << std::endl;

            result.type = CheckSequenceResult::ResultType::Process;
            result.messages.push_back(std::move(msgs[i]));
            localExpected++;
        } else if (msgs[i].seqnum < localExpected) {
            // duplicate
        } else {
            if (_DBG_GAP)
                std::cout << "[Drain] Gap encountered: expected " << localExpected
                          << " but found " << msgs[i].seqnum << "\n";
            break;
        }
    }

    if (_DBG_GAP)
        std::cout << "[Drain] Requeueing " << (msgs.size() - i) << " unprocessed messages.\n";

    for (size_t j = i; j < msgs.size(); ++j) {
        _qRecovIn.push(std::move(msgs[j]));  // still RecoveredMessage now
    }

    if (i < msgs.size()) {
        GapDetected(localExpected, msgs[i].seqnum, _UUID);
        if (_DBG_GAP)
            std::cout << "[Drain] Issuing new retransmit request for missing range "
                      << gapInfo.start << " to " << gapInfo.end
                      << " Count=" << gapInfo.count << "\n";

        onStateChange(EnumSessionState::WaitingForRetransmission);
        result.type = CheckSequenceResult::ResultType::ProcessThenRetransmit;
        result.startSeq = localExpected;
        result.numMsgs = gapInfo.count;
        return result;
    } else {
        onStateChange(EnumSessionState::Established);
        if (_DBG_GAP)
            std::cout << "[Drain complete] New expectedSeq is " << localExpected << "\n";

        result.type = result.messages.empty()
            ? CheckSequenceResult::ResultType::OK
            : CheckSequenceResult::ResultType::Process;

        return result;
    }
}




