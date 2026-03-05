//============================================================================
// Name        	: CmeGT5.cpp
// Author      	: Samuel Gaer
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Nov 5, 2021 6:24:37 PM
//============================================================================

#include <exchsupport/cme/CmeGT5.hpp>
#include "CmeGT5.hpp"

using namespace KTN::CME;

/**
 *
 * @param qords
 * @param settingsfile
 * @param source
 */
CmeGT5::CmeGT5(tbb::concurrent_queue<KTN::OrderPod>& qords, const std::string& settingsfile, const std::string& source)
	:ExchangeBase2(settingsfile, source, qords, "CmeGT5"),
	_qOrdsProc(qords),
	_qCmds(32),
	_qAlgoCmds(32),
	_OrdPodPool(512),
	_KtnBufs(256, 4096),
	//_qRecvBufs(2048),
	_qHdgClOrdIds(10000)
	//_HdgBufs(512)
{
	//CmeSecMaster is a singleton, so if not loaded, it loads on first
	//call/instantiation
	if (!CmeSecMaster::instance().IsLoaded())
			LogMe("LOADING CME SECMASTER...");

	_sett.Load(settingsfile);
	_sessfact.Init(settingsfile);
	_fastfact.Init(settingsfile);

	Settings cme_config_settings = Settings(settingsfile);

	_State.ORG = _sett.Org;
	_State.SOURCE = source;
	_State.EXCHNAME = _sett.ExchName;
	_logsvc->ChangeName(_sett.ExchName, _sett.ExchName);

	LogMe("LOG SVC CHANGED NAME TO " + _sett.ExchName + " SRC=" + source);

	_State.KEEPALIVE = _sett.KeepAliveSecs;
	_State.KEEPALIVE_BUFFERMS = cme_config_settings.getInteger("IL3.KeepAliveBufferMS");
	_State.EVENTPOLLMAX =  cme_config_settings.getLong("IL3.PollerMax");

	_State.RetransOnNewUUID = cme_config_settings.getBoolean("IL3.RetransOnNewUUID");

	//PRIMARY CONNECTION SWITCH
	_State.USE_PRIMARY = cme_config_settings.getBoolean("IL3.UsePrimary");

	_State.MEASURE = cme_config_settings.getBoolean("IL3.MeasureLatency");
	_State.processingThreadAffinity = cme_config_settings.getInteger("IL3.ProcessingThreadAffinity");
	_State.SpinProcessingThread = cme_config_settings.getBooleanSafe("IL3.SpinProcessingThread");

	Settings mainsett = Settings("Settings.txt");
	_State.ALGO_AFFINITY = mainsett.getInteger("AlgoAffinity");
	_State.logToScreen = cme_config_settings.getBoolean("IL3.LogMessagesToScreen");

	vector<string> segs;
	string segline = cme_config_settings.getString("IL3.SEGLIST");

	_SEGS.clear();
	segs = StringSplitter::Split(segline, ",");
	for (auto seg : segs)
		_SEGS.push_back(stoi(seg));

	_msgw.Load();
	
	_State.USE_AUDIT = cme_config_settings.getBoolean("IL3.UseAuditTrail");
	if (_State.USE_AUDIT)
	{
		int auditcore = cme_config_settings.getInteger("IL3.AuditCore");
		string dir = cme_config_settings.getString("IL3.AuditTrailDirectory");
		StartAuditTrail(cme_config_settings.getString("IL3.AuditTrailDirectory"), _sett.SessionId, _sett.FirmId, auditcore);
	}

	_State.STATEFILE_DIR = cme_config_settings.getString("IL3.StateFileDirectory");
	_State.STATEFILE_DIR = cme_config_settings.getString("IL3.StateFileDirectory");
	string szRetrNewUuidEnable =  (_State.RetransOnNewUUID) ? "ENABLED" : "DISABLED";
	LogMe("Retransmit gap requests from previous UUID is " + szRetrNewUuidEnable);

	//Debug switches. Add more if you like
	_State.DebugHmac = cme_config_settings.getBoolean("IL3.DebugHmac");
	_State.DebugRecvBytes = cme_config_settings.getBoolean("IL3.DebugRecvBytes");
	_State.DebugSession = cme_config_settings.getBoolean("IL3.DebugSession");
	_State.DebugAppMsgs = cme_config_settings.getBoolean("IL3.DebugAppMsgs");
	_State.DebugSessMsgs = cme_config_settings.getBoolean("IL3.DebugSessMsgs");
	_State.DebugGapDetection = cme_config_settings.getBoolean("IL3.DebugGapDetection");

	//hedge clordids- we pre-generate pairs of order req id's and clordids
	for(int i = 0; i < 10000; i++)
	{
		uint64_t newreqid =  OrderNums::instance().NextReqId();
		char* newclordid = new char[20];
		OrderNums::instance().NextClOrdId(newclordid, 20);

		_qHdgClOrdIds.enqueue(std::make_pair(newreqid, newclordid));
	}

	Display();

	usleep(10000);

	_procthread = std::thread(&CmeGT5::ProcessingThread, this);

	Notification n = {};
	n.exch = _State.EXCHNAME;
	n.source = _State.SOURCE;
	n.org = _State.ORG;
	n.level = MsgLevel::INFO;
	n.imsg = MsgType::NOTICE;
	n.text = "STARTING EXCHANGE HANDLER: " + _State.EXCHNAME + " SOURCE=" + _State.SOURCE;

	NotifierRest::instance().Notify(n);

	Start();
}

CmeGT5::~CmeGT5() {
	_State.ACTIVE.store(false, std::memory_order_relaxed);
	_PROC_THREAD_ACTIVE.store(false, std::memory_order_relaxed);
}

void CmeGT5::StartAuditTrail(std::string dir, std::string sessid, std::string firmid, int core)
{
	
	LogMe("[INFO] Initializeing audit trail writer. Dir=" + dir);
	auto processor = std::make_unique<CmeAuditProcessor>();
    _audit = std::make_unique<AuditTrailWriter>(std::move(processor), _KtnBufs);

	_audit->Start(dir,_sett.SessionId, _sett.FirmId, core);
	for(int segid : _SEGS)
		_audit->InitFileForSeg(segid);
}

void CmeGT5::ProcessingThread()
{
	const char *threadName = "cme-proc";

	pthread_setname_np(pthread_self(), threadName);
	pthread_t current_thread = pthread_self();

	int core = _State.processingThreadAffinity;
	cpu_set_t cpus;
	CPU_ZERO(&cpus);
	CPU_SET(core, &cpus);
	int irc = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpus);
	if (irc != 0)
	{
		KT01_LOG_ERROR(__CLASS__, "Error setting affinity to {} on CME ProcessingThread {}: {}", core, threadName, irc);
	}

	if (_State.SpinProcessingThread)
	{
		runProcessingThreadLoop<true>();
	}
	else
	{
		runProcessingThreadLoop<false>();
	}

	// _PROC_THREAD_ACTIVE.store(true, std::memory_order_relaxed);

	// LogMe("OK: STARTING PROCESSING THREAD", LogLevel::OK);

	// std::vector<uint8_t> payload;

	// while(_PROC_THREAD_ACTIVE.load(std::memory_order_relaxed) == true)
	// {
	// 	if (!_qRecvBufs.empty())
    //     {
    //         KTNBuf8t ktnbuf;
    //         while (_qRecvBufs.try_pop(ktnbuf))
    //         {
	// 			const uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
	// 				std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	// 			KT01_LOG_INFO(__CLASS__, "Processing message recvTime={} current={} diff={} size={} segid={} index={}",
	// 				ktnbuf.recvTime, timestamp, timestamp - ktnbuf.recvTime, ktnbuf.buffer_length, ktnbuf.segid, ktnbuf.index);

    //             std::vector<uint8_t> &tcpRemainder = _tcpRemainders[ktnbuf.index];
    //             if (!tcpRemainder.empty()) {
    //                 payload.insert(payload.end(), tcpRemainder.begin(), tcpRemainder.end());
    //                 payload.insert(payload.end(), ktnbuf.buffer, ktnbuf.buffer + ktnbuf.buffer_length);
    //             } else {
    //                 payload.assign(ktnbuf.buffer, ktnbuf.buffer + ktnbuf.buffer_length);
    //             }

    //             int consumed = ProcessRecvBuffer(payload.data(), payload.size(), ktnbuf.segid, ktnbuf.index, ktnbuf.recvTime);

    //             if (consumed < static_cast<int>(payload.size())) {
    //                 tcpRemainder.assign(payload.begin() + consumed, payload.end());
    //             } else {
    //                 tcpRemainder.clear();
    //             }

	// 			payload.clear();
    //             // No manual delete needed — destructor handles it
    //         }
    //     }		


	// 	// if(!_qHdgProc.empty()){
	// 	// 	std::vector<std::unique_ptr<KTNBuf>> vbufs;
	// 	// 	KTNBuf buf;
	// 	// 	while (_qHdgProc.try_pop(buf)) {
	// 	// 		//vbufs.push_back(std::make_unique<KTNBuf>(buf));

	// 	// 		 auto bufPtr = std::make_unique<KTNBuf>(buf);
	// 	// 		//_KtnBufs.put(bufPtr.get()); // Use the raw pointer for _KtnBufs.put
	// 	// 		vbufs.push_back(std::move(bufPtr)); // Move unique_ptr into vector
	// 	// 	}

	// 	// 	ProcessHdgBuf(vbufs);
	// 	// }

	// 	if (_State.CHECK_EVENT_LOOP_COUNT >= _State.CHECK_EVENTS_TRIG){
	// 		CheckEvents(_CX);
	// 		_State.CHECK_EVENT_LOOP_COUNT = 0;
	// 	}

	// 	std::this_thread::sleep_for(std::chrono::microseconds(1));
	// }
}

void CmeGT5::Stop()
{
	LogMe("GOT STOP REQUEST", LogLevel::WARN);

//	string failover = (_sett.FaultTolerance) ? "ENABLED" : "DISABLED";
//	ostringstream oss;
//
//	oss << "FAILOVER IS " << failover;
//	LogMe(oss.str(),LogLevel::INFO);



	Instruction cmd = {};
	cmd.command = ExchCmd::TERMINATE;

	_qCmds.push(cmd);
	_State.EVENT_WAITING.store(true, std::memory_order_relaxed);
}

void CmeGT5::ClearMaps()
{
	LogMe(_State.EXCHNAME + "CLEARING MAPS!", LogLevel::WARN);
	_msgratio.clear();
	_msgevents.clear();
	_hedgeOrds.clear();
	_hedgeMap.clear();
}

void CmeGT5::Start()
{
	_State.CHECK_EVENTS_TRIG = _State.EVENTPOLLMAX * 2;
	_State.CHECK_EVENT_LOOP_COUNT = 0;
	_State.LOOPER = 0;
	_State.LOOP_TRIG = _State.EVENTPOLLMAX;

	_State.SEGCNT = _SEGS.size();

	if (_State.SEGCNT > _MAX_SEGS)
	{
		KT01_LOG_ERROR(__CLASS__, "TOO MANY SEGMENTS DEFINED IN SETTINGS FILE. MAX={} SEGMENTS={}",
			_MAX_SEGS, _State.SEGCNT);
		_State.SEGCNT = _MAX_SEGS;
	}

	LogMe("STARTING " + std::to_string(_SEGS.size()) + " MSGW CONNECTIONS",LogLevel::INFO);
	for(int i = 0; i < _State.SEGCNT; i++)
	{
		_CX[i].sessmgr.CurrentState = EnumSessionState::Disconnected;
		_CX[i].segid = _SEGS[i];
		_CX[i].index = i;
		_CX[i].tcpHandler = new EFVITcpV5(this);


		if(_CX[i].sessmgr.CurrentState != EnumSessionState::Disconnected)
		{
			KT01_LOG_ERROR(__CLASS__, "SEGID " + std::to_string(_CX[i].segid)
					+ ": CANNOT LOGIN UNLESS SESSION STATE IS DISCONNECTED");
			continue;
		}

		if (_State.DebugSession)
			cout << "[Login] Segment " << _CX[i].segid <<" STARTING LOGIN PROCESS: INIT" << endl;

		_CX[i].sessmgr.CurrentState = EnumSessionState::StartingLoginProcess;
		_CX[i].sessmgr.Init(_sett.SessionId, _sett.FirmId, _CX[i].segid, _State.KEEPALIVE, _State.KEEPALIVE_BUFFERMS,
				_State.STATEFILE_DIR, _State.DebugGapDetection, _State.DebugSessMsgs,
				_State.EXCHNAME, _State.SOURCE, _State.ORG);

		usleep(10000);
		StartTcpConnection(_CX[i], _State.USE_PRIMARY);
		usleep(1000);
	}

	_State.ACTIVE.store(true, std::memory_order_relaxed);
	_State.ALGOS_IN_USE.store(true, std::memory_order_relaxed);
}

bool CmeGT5::StartTcpConnection(ConnState& conn, bool primary)
{
	ostringstream oseg;
	oseg << "SEGID=" << conn.segid;
	int seg =  conn.segid;
	int port = _sett.Port;

	ostringstream oss;
	oss << "Attempting Session Login: SEG " << seg;
	LogMe(oss.str(),LogLevel::INFO);



	int segid = conn.segid;
	Msgw gw = _msgw.getMsgw(segid);

	if (gw.segid == 0)
	{
		ostringstream err;
		err << "MSGW DEF FOR SEGID " << segid << " NOT FOUND";
		LogMe(err.str());
		return false;
	}

	string whichconn = (primary) ? "PRIMARY" : "SECONDARY";
	LogMe("STARTING " + whichconn + " TCP CONNECTION TO " + oseg.str());

    if (conn.sessmgr.CurrentState != EnumSessionState::StartingLoginProcess)
    {
    	ostringstream oss;
    	oss << "ERROR: CANNOT CREATE TCP CONNECTION. SEG=" << segid << " STATE=" << EnumSessionState::toString(conn.sessmgr.CurrentState);
    	KT01_LOG_ERROR(__CLASS__, oss.str());
    	conn.sessmgr.CurrentState = EnumSessionState::Disconnected;
    	conn.sessmgr.Stop();
    	return false;
    }

    bool res = false;

	try
	{
		ServiceDescriptor svc;
		svc.address = (primary) ? gw.primip : gw.secip;
		svc.port = port;
		svc.iSource = gw.segid;
		svc.index = conn.index;
		svc.segid = conn.segid;

	    res = conn.tcpHandler->Init(svc, _sett.UseFeed_A);

	}
	catch (const std::exception& ex)
	{
		ostringstream oss;
		oss << ex.what();
		KT01_LOG_ERROR(__CLASS__, "Cannot connect to the primary host: " + oss.str());
	}

	return res;
}



void CmeGT5::SendSequenceMsg(ConnState& conn, EnumKeepAliveLapsed lapsed)
{
	int segid = conn.segid;
	if (UNLIKELY(!ConnState::OkToSend(conn.sessmgr.CurrentState)))
	{
		ostringstream oss;
		oss << "CANNOT SEND SEQUENCE MSG FOR SEG " << segid
				<< ". CONNECTION STATE=" << EnumSessionState::toString(conn.sessmgr.CurrentState);
		LogMe(oss.str(),LogLevel::WARN);
		setLastSentTime(conn);
		return;
	}

	if (lapsed == EnumKeepAliveLapsed::EnumKeepAliveLapsed_Lapsed)
	{
		ostringstream oss;
		LogMe("SendSequenceMsg: SENDING SEQUENCE MESSAGE: LAPSE=TRUE", LogLevel::INFO);
	}

	if (_State.DebugSessMsgs)
		cout << "[SendSeq] Sending Sequence: Lapse=" << (int)lapsed << " STATE=" << EnumSessionState::toString(conn.sessmgr.CurrentState) << endl;

	uint64_t seq = conn.sessmgr.SeqOut();
	EnumFTI fti = (_State.USE_PRIMARY) ? EnumFTI::EnumFTI_Primary : EnumFTI::EnumFTI_Backup;
	KTN_MSG msg = _sessfact.SequenceEncode(conn.sessmgr.Uuid(), seq, fti, lapsed);

	conn.tcpHandler->Send(msg.buffer, msg.buffer_length, false);

	setLastSentTime(conn);

	conn.sessmgr.Update();

}

void CmeGT5::SendTerminateMsg(ConnState& conn, string reason, int errorcode=0)
{
	int segid = conn.segid;
	if (UNLIKELY(!ConnState::OkToSend(conn.sessmgr.CurrentState)))
	{
		ostringstream oss;
		oss << "CANNOT SEND TERMINATE FOR SEG " << segid << ". CONNECTION STATE=" << EnumSessionState::toString(conn.sessmgr.CurrentState);
		LogMe(oss.str(),LogLevel::WARN);

		return;
	}

	conn.sessmgr.Stop();
	conn.sessmgr.CurrentState = EnumSessionState::Terminating;

	ostringstream oseg;
	oseg << "SENDING TERMINATE FOR SESSION ON SEGID=" << segid << " CODE=" << errorcode << " REASON=" << reason;
	LogMe(oseg.str());

	KTNBuf msg = _sessfact.TerminateEncode(conn.sessmgr.Uuid(), reason, errorcode);

	conn.tcpHandler->Send(msg.buffer, msg.buffer_length, false);
}

void CmeGT5::SendRetransmitRequestMsg(ConnState& conn, int fromseq, int msgcount, uint64_t lastuuid)
{
	int segid = conn.segid;
	if (UNLIKELY(!ConnState::OkToSend(conn.sessmgr.CurrentState)))
	{
		ostringstream oss;
		oss << "SEND CANNOT SEND RETRANS REQUEST FOR SEG " << segid << ". CONNECTION STATE=" << EnumSessionState::toString(conn.sessmgr.CurrentState);
		KT01_LOG_ERROR(__CLASS__, oss.str());

		return;
	}

	if (msgcount > 2500)
	{
		ostringstream oss;
		oss << "RETRANSMIT: Message count cannot exceed 2500.  Changing to 0, which tells CME to send all from Seq=" << fromseq;
		LogMe(oss.str(), LogLevel::WARN);

		msgcount = 0;
	}

	ostringstream oss;
	oss << "SEGMENT " << conn.segid << " SENDING RETRANSMIT REQUEST: START_SEQ=" << fromseq << " NUM_MSGS=" << msgcount << " LASTUUID=" << lastuuid;
	LogMe(oss.str(), LogLevel::WARN);

	if (conn.sessmgr.CurrentState != EnumSessionState::WaitingForRetransmission)
		onStateChange(EnumSessionState::WaitingForRetransmission, conn);


	KTNBuf msg = _sessfact.RetransRequestEncode(conn.sessmgr.Uuid(), fromseq, msgcount, lastuuid);

	conn.tcpHandler->Send(msg.buffer, msg.buffer_length, false);

	setLastSentTime(conn);

}

uint16_t CmeGT5::onRecv(byte_ptr buf, int len, int segid, int index)
{
	const uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	KTNBuf8t ktbuf = {};
	ktbuf.buffer = new uint8_t[len];

	const uint64_t afterPop = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	 __builtin_memcpy(ktbuf.buffer, buf, len);
	ktbuf.buffer_length = len;
	ktbuf.segid = segid;
	ktbuf.index = index;
	ktbuf.recvTime = timestamp;
	ktbuf.owns_buffer = true;

	_qRecvBufs.push(std::move(ktbuf));

	const uint64_t afterpush = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	LOGINFO("[CmeGT5] onRecv: segid={} index={} len={} recvTime={} recvToPop={} popToPush={}",
		segid, index, len, timestamp, afterPop - timestamp, afterpush - afterPop);
	
	return len;
}

void CmeGT5::onConnectionClosed(int segid, int index)
{
	std::ostringstream ss;
	ss << "Segment " << segid << " disconnected";

	Notification n = {};
	n.exch = _State.EXCHNAME;
	n.source = _State.SOURCE;
	n.org = _State.ORG;
	n.level = MsgLevel::ERROR;
	n.imsg = MsgType::ALERT;
	n.text = ss.str();

	NotifierRest::instance().Notify(n);
}

uint16_t CmeGT5::ProcessRecvBuffer(byte_ptr buf, int len, int segid, int index, const uint64_t recvTime)
{
    int offset = 0;
	int frames = 0;

    // Process each complete SOFH message in the buffer
    while (offset < len)
    {
		frames ++;
        // Make sure we have at least a complete SOFH header.
        if (len - offset < IL3Helpers::SOFH_SIZE)
            break;  // Not enough data

        // Get the SOFH header. Assume sofh->msgSize_ includes the header.
        auto sofh = reinterpret_cast<SOFH*>(buf + offset);
        if (sofh->msgSize_ > (len - offset))
		{
			if (_State.DebugRecvBytes)
			{
				ostringstream oss;
				oss << "[onRecv] INCOMPLETE SOFH MSG SIZE=" << sofh->msgSize_ << " LEN-OFFSET=" 
					<< (len-offset) << " OFFSET=" << offset << " TOTAL_LEN=" << len << " FRAMES=" << frames;	
				KT01_LOG_WARN("CmeGT5", oss.str());
			}
			break;  // Incomplete SOFH message; wait for more data
		}

        // Calculate end of the current message.
        const int messageEnd = offset + sofh->msgSize_;

        // Log overall SOFH info.
        if (_State.DebugRecvBytes){
			ostringstream oss;
			KT01_LOG_WARN("CmeGT5", "-------------------------------------------------------------------");
        	oss	<< "[onRecv] *** MSG SIZE=" << sofh->msgSize_ << " TOTAL_LEN=" << len << " LOOP=" << frames;
			KT01_LOG_WARN("CmeGT5", oss.str());
		}

        // Move past the SOFH header.
        offset += IL3Helpers::SOFH_SIZE;

        // Process all SBE messages within this SOFH message.
        while (offset < messageEnd)
        {
            // Ensure we have a complete SBE header.
            if (messageEnd - offset < IL3Helpers::SBE_HEADER_SIZE)
                break;

            auto msghdr = reinterpret_cast<SBEHeader*>(buf + offset);
            offset += IL3Helpers::SBE_HEADER_SIZE;

            uint16_t blockLen  = msghdr->blockLength_;
            uint16_t templateId = static_cast<uint16_t>(msghdr->templateId_);
            uint16_t msgBytes = blockLen;  // initial message bytes to advance

            // Calculate remaining bytes in this SOFH message.
            const int sbeRemaining = messageEnd - offset;

            if (_State.DebugRecvBytes)
            {
				ostringstream oss;
				oss << "[onRecv] PROCESS MESSAGE: TEMPLATEID=" << templateId
					 << " BLOCKLEN=" << blockLen
					 << " MSGSIZE=" << sofh->msgSize_
					 << " REMAIN=" << sbeRemaining
					 << " BYTESREAD=" << (sofh->msgSize_ - sbeRemaining)
					 << " TOTAL SIZE=" << len
					 << " OFFSET=" << offset
					 << " FRAME=" << frames;
				KT01_LOG_WARN("CmeGT5", oss.str());
            }

            // Prepare the application buffer.
            // KTNBuf8t appbuf = {};
            // appbuf.buffer = buf + offset;
            // // (Copying the remaining bytes—adjust the length if you only need the message body)
            // memcpy(appbuf.buf, appbuf.buffer, sbeRemaining);

			//fix 5/29/2025
			// std::cout << "[DEBUG] First 16 bytes @ offset: ";
			// for (int i = 0; i < 16; ++i) std::cout << std::hex << (int)*(buf + offset + i) << " ";
			// std::cout << std::dec << std::endl;

			KTNBuf8t appbuf(buf + offset, sbeRemaining);  // ← will use buf or heap as needed

			appbuf.msgtype        = templateId;
			appbuf.segid          = segid;
			appbuf.index          = index;
			appbuf.block_length   = blockLen;
			appbuf.msg_length     = sbeRemaining;
			appbuf.recvTime       = recvTime;

            // appbuf.msgtype        = templateId;
            // appbuf.buffer_length  = sbeRemaining;
            // appbuf.segid          = segid;
            // appbuf.index          = index;
            // appbuf.block_length   = blockLen;
            // appbuf.msg_length 	  = sbeRemaining;


            bool save = false;
            if (UNLIKELY(appbuf.msgtype <= IL3Helpers::MAX_SESS_MSG_ID))
            {
                if (templateId == NegotiationResponseMsg::id)
                {
                    // For this special case adjust the lengths.
                    blockLen += IL3Helpers::DATA_FIELD_LEN;
                    appbuf.buffer_length += IL3Helpers::DATA_FIELD_LEN;
                }
                msgBytes = blockLen;
                ProcessSessionMessage(_CX[index], appbuf);
            }
            else
            {
                msgBytes = ProcessApplicationMessage(_CX[index], appbuf);
                save = true;
            }

            if (_State.USE_AUDIT && save)
            {
                if (_State.MEASURE)
                    _trk.startMeasurement("auditIn");
                // Use BufferPool - no hidden memcpy in queue
                KTNBuf *auditbuf = _KtnBufs.get();
                memcpy(auditbuf->buf, appbuf.buf, appbuf.buffer_length);
                auditbuf->buffer = auditbuf->buf;
                auditbuf->buffer_length = appbuf.buffer_length;
                auditbuf->msgtype = appbuf.msgtype;
                auditbuf->segid = appbuf.segid;
                auditbuf->index = appbuf.index;
                auditbuf->seqnum = appbuf.seqnum;
                auditbuf->dir = 1;  // incoming message
                auditbuf->customerOrderTime = 0;  // not used for incoming ACKs
                _audit->Write(auditbuf);
                if (_State.MEASURE)
                    _trk.stopMeasurement("auditIn");
            }
            // Move past the SBE message.
            offset += msgBytes;
        }
    }
	
	if (_State.DebugRecvBytes)
	{
		ostringstream oss;
		oss << "onRecv: RETURNING " << offset << " bytes. Frames=" << frames << "\n"
			<<"-------------------------------------------------------------------";
		KT01_LOG_WARN("CmeGT5", oss.str());
	}
    // Return the total number of bytes processed.
    return static_cast<uint16_t>(offset);
}

int CmeGT5::SkipGroupAndAdvance(byte_ptr& buf, int msgbytes)
{
	auto nofills = reinterpret_cast<IL3::groupSize*>(buf);

	buf += sizeof(IL3::groupSize);
	msgbytes += sizeof(IL3::groupSize);

	buf += (nofills->blockLength * nofills->numInGroup);
	msgbytes += (nofills->blockLength * nofills->numInGroup);

	return msgbytes;
}

void CmeGT5::SendPartyDetailsDefRequest(ConnState& conn)
{
	//TODO: Make this a constant
	int segid=12;

	LogMe("SENDING PARTY DETAILS REQ...", LogLevel::INFO);

	ostringstream oseg;
	oseg << "SEGID=" << segid;

	LogMe("SENDING PTY DETAILS DEF REQ FOR " + oseg.str() + " SESSION");

	uint64_t seq = conn.sessmgr.SeqOut();

	KTNBuf msg = AppMessageGen::PartyDetailsDefEncode(_sett, seq);

	conn.tcpHandler->Send(msg.buffer, msg.buffer_length, false);
	conn.sessmgr.IncSeqOut(1);

	setLastSentTime(conn);

	msg.dir = -1;
	msg.segid = segid;
	msg.msgtype = IL3::META::PartyDetailsDefinitionRequestMeta::id;
	msg.seqnum = seq;
	msg.audit_trail = true;

	if (_State.USE_AUDIT){
		KTNBuf *msg2 = _KtnBufs.get();
		memcpy(msg2->buffer, msg.buffer, msg.buffer_length);
		msg2->buffer_length = msg.buffer_length;
		msg2->dir = -1;
		msg2->msgtype = IL3::META::PartyDetailsDefinitionRequestMeta::id;
		msg2->segid = segid;
		msg2->seqnum = seq;
		msg2->audit_trail = true;

		//_audit->Write(msg.buffer, IL3::META::PartyDetailsDefinitionRequestMeta::id, segid);
		_audit->Write(msg2);
	}
}

void CmeGT5::SendPartyDetailsListRequest(ConnState& conn)
{
	//TODO: Make this a constant
	int segid=12;

	LogMe("SENDING PARTY **LIST** REQ...", LogLevel::INFO);

	ostringstream oseg;
	oseg << "SEGID=" << segid;

	LogMe("SENDING PTY DETAILS **LIST** REQ FOR " + oseg.str() + " SESSION");

	uint64_t seq = conn.sessmgr.SeqOut(); //conn.sessmgr.IncSeqOut(1);

	KTNBuf msg = AppMessageGen::PartyDetailsListEncode(_sett, seq);

	//cout << "PTY DETAILS DEF REQ MSG LEN=" << msg.buffer_length << " PARTY ID=" << _sett.PartyListId << endl;

	conn.tcpHandler->Send(msg.buffer, msg.buffer_length, false);
	conn.sessmgr.IncSeqOut(1);

	setLastSentTime(conn);

	msg.dir = -1;
	msg.segid = segid;
	msg.msgtype = IL3::META::PartyDetailsDefinitionRequestMeta::id;
	msg.seqnum = seq;
	msg.audit_trail = true;

	if (_State.USE_AUDIT){
		KTNBuf *msg2 = _KtnBufs.get();
		memcpy(msg2->buffer, msg.buffer, msg.buffer_length);
		msg2->buffer_length = msg.buffer_length;
		msg2->dir = -1;
		msg2->msgtype = IL3::META::PartyDetailsDefinitionRequestMeta::id;
		msg2->segid = segid;
		msg2->seqnum = seq;
		msg2->audit_trail = true;

		//_audit->Write(msg.buffer, IL3::META::PartyDetailsDefinitionRequestMeta::id, segid);
		_audit->Write(msg2);
	}
}

bool CmeGT5::checkSessionIsConnected(ConnState& conn, int segid)
{
    if ( !conn.sessmgr.Connected()){
        return false;
    }

    return true;
}


/**
 * Poll the Tcp connection for events
 * Note: We've offloaded some event checking etc to the ProcessingThread function
 */
void CmeGT5::Poll()
{
	bool checkhb = false;
	if (_State.LOOPER > _State.LOOP_TRIG)
	{
		checkhb = true;
		_State.LOOPER = 0;
	}

	for(int i = 0; i < _State.SEGCNT; i++){
		Dispatch(_CX[i], checkhb);
	}

	_State.CHECK_EVENT_LOOP_COUNT++;
	_State.LOOPER++;
}

void CmeGT5::SessionInit(ConnState& conn, int index)
{
	conn.sessmgr.CurrentState = EnumSessionState::Disconnected;
	conn.segid = _SEGS[index];
	conn.index = index;
	//conn.tcpHandler->Subscribe(this);
}

void CmeGT5::SessionLogin(ConnState& conn)
{
	ostringstream oseg;
	oseg << "SEGID=" << conn.segid;

	if (_State.DebugSession)
	{
		cout << "[Login] Login for segid=" << (int)conn.segid << ". Negotiated=" << conn.sessmgr.Negotiated() << endl;
	}

	//Note- we now detect the Negotiated flag to determine whether to
	//goto Negotiate or Establish....
	//if (conn.sessmgr.Uuid() == 0)
	if (!conn.sessmgr.Negotiated())
	{
		LogMe("***NEW SESSION NEGOTIATION REQUIRED FOR " + oseg.str(),LogLevel::WARN);
		//3
		NegotiateSession(conn);
	}
	else
	{
		LogMe("SENDING ESTABLISH MSG FOR EXISTING UUID " + oseg.str(),LogLevel::WARN);
		//4
		EstablishSession(conn);
	}
}

void CmeGT5::Dispatch(ConnState& conn, bool checkHb)
{
	if (conn.sessmgr.CurrentState == EnumSessionState::Disconnected)
		return;

	if (conn.tcpHandler->Fd() <= 0)
	{
		onStateChange(EnumSessionState::Disconnected, conn);
		return;
	}


	conn.tcpHandler->Poll();

	if (UNLIKELY(conn.sessmgr.CurrentState != EnumSessionState::Established))
	{
		if (conn.sessmgr.CurrentState == EnumSessionState::StartingLoginProcess){
			EFVITcpV5 *efviHandler = dynamic_cast<EFVITcpV5 *>(conn.tcpHandler);
			if (efviHandler == nullptr || efviHandler->isConnectionEstablished())
			{
				LogMe("SENDING LOGIN FOR SEG " + std::to_string(conn.segid) + " FD=" + std::to_string(conn.tcpHandler->Fd()));

				SessionLogin(conn);
			}
		}

		if (conn.sessmgr.CurrentState == EnumSessionState::Negotiated){
			LogMe("OK: SEG " + std::to_string(conn.segid) + " NEGOTIATED=TRUE", LogLevel::OK);
			LogMe("NOW SENDING ESTABLISH FOR SEG " + std::to_string(conn.segid));
			EstablishSession(conn);
		}
	}

	if (checkHb && ConnState::OkToSend(conn.sessmgr.CurrentState))
		CheckForHeartbeat(conn);

}

/***
 * Note: I'm moving the hb here, as we don't need it in the session manager.
 * We can then re-do the sess mgr to be mo better and mo simpler
 */

void CmeGT5::setLastSentTime(ConnState& conn)
{
	conn.sessmgr.HbSent();
}

bool CmeGT5::CheckForHeartbeat(ConnState& conn)
{
	if(conn.sessmgr.HbTriggered())
	{
		SendSequenceMsg(conn, EnumKeepAliveLapsed::EnumKeepAliveLapsed_NotLapsed);
		return true;
	}

	return false;
}

void CmeGT5::CheckEvents(ConnState (&conns)[_MAX_SEGS])
{
	if (_State.EVENT_WAITING.load(std::memory_order_relaxed) == false)
		return;

	LogMe("CHECK EVENTS- EVENT WAITING = TRUE!");

	while(!_qCmds.empty())
	{
		Instruction cmd = {};
		_qCmds.try_pop(cmd);

		switch(cmd.command)
		{

		case ExchCmd::MENU_CHOICE:
		{
			if (cmd.cmd_value > 1000)
			{

				int newseq = cmd.cmd_value - 1000;
				LogMe("DANGER! GOT COMMAND > 1000 - SEQUENCE CHANGE FOR TESTING ONLY: SEQ=" + std::to_string(newseq), LogLevel::WARN);
				for(size_t i = 0; i < _SEGS.size(); i++)
					conns[i].sessmgr.SetSeqIn(newseq);
			}
		}break;

		case ExchCmd::ALGO_PARAMS:
		case ExchCmd::ALGO_ORDER:
			AlgoJson(cmd.cmd_json, cmd.command);
			break;

		case ExchCmd::SESS_STATUS:
			PrintSessionMap(conns);
			break;

		case ExchCmd::PARTY_DET_REQ:
			{
				LogMe("GOT PARTY DETAILS REQ COMMAND");
				for(size_t i = 0; i < _SEGS.size(); i++){
					if (conns[i].segid != 12)
					{
						LogMe("CANNOT SEND PARTY DETAILS FOR SEG "
								+ std::to_string(conns[i].segid),LogLevel::WARN);
						break;
					}
					SendPartyDetailsDefRequest(conns[i]);
				}

			}break;

		case ExchCmd::PARTY_LIST_REQ:
		{
			LogMe("GOT PARTY **LIST** REQ COMMAND");
			for(size_t i = 0; i < _SEGS.size(); i++){
				if (conns[i].segid != 12)
				{
					LogMe("CANNOT SEND PARTY LIST REQ FOR SEG "
							+ std::to_string(conns[i].segid),LogLevel::WARN);
					break;
				}
				SendPartyDetailsListRequest(conns[i]);
			}

		}break;

		case ExchCmd::RETRANS_REQ:
		{
			LogMe("GOT **RETRANSMIT** REQ COMMAND. SENDING FROM SEQIN");
			for(size_t i = 0; i < _SEGS.size(); i++){
				ostringstream oss;
				oss << "RETRANS: SEG=" << conns[i].segid << " SEQIN=" << conns[i].sessmgr.SeqIn();
				LogMe(oss.str());
				SendRetransmitRequestMsg(conns[i], conns[i].sessmgr.SeqIn(), 0);
			}

		}break;

		case ExchCmd::MASS_ORDER_STATUS_REQ:
		{
			LogMe("GOT MASS ORDER STATUS REQ COMMAND");
			for(size_t i = 0; i < _SEGS.size(); i++){
				//ConnState& conn, EnumMassStatusReqTyp reqtype, int segid
				SendMassOrderStatusReq(conns[i],EnumMassStatusReqTyp::EnumMassStatusReqTyp_MarketSegment, conns[i].segid);
			}
		} break;
		case ExchCmd::CXL_ALL_SEGS:
			{
				LogMe("GOT CXL ALL SEGS COMMAND!!!!", LogLevel::WARN);
				for (size_t i = 0; i < _SEGS.size(); i++)
				{
					ostringstream oss;
					oss << "SESS THREAD SENDING MASS CXL FOR SEG " << _SEGS[i];
					LogMe(oss.str(), LogLevel::INFO);
					KTN::Order ord = {};
					ord.OrdAction = KOrderAction::ACTION_CXL_ALL;
					ord.massscope = EnumMassActionScope::EnumMassActionScope_MarketSegmentID;
					ord.mktsegid = _SEGS[i];
					SendMassCxl(conns[i], ord, _SEGS[i]);
				}
			}break;

		case ExchCmd::CXL_ALL_SYMB:
			{
				LogMe("GOT CXL ALL FOR SYMBOL " + cmd.cmd_symbol +" COMMAND!!!!", LogLevel::WARN);
				//int segid = CmeSecMaster::instance().getSegmentIdForProduct(cmd.cmd_symbol);
				auto def = CmeSecMaster::instance().getSecDef(cmd.cmd_symbol);
				int segid = def.msgw;
				LogMe("SEG ID RETURN VALUE FOR " + cmd.cmd_symbol + " Def: " + def.symbol + "=" + std::to_string(segid), LogLevel::WARN);
				for (size_t i = 0; i < _SEGS.size(); i++)
				{
					if (_SEGS[i] == segid)
					{
						ostringstream oss;
						oss << "SESS THREAD SENDING MASS CXL FOR SYMBOL=" << cmd.cmd_symbol << " SEG=" << segid;
						LogMe(oss.str(), LogLevel::INFO);
						KTN::Order ord = {};
						ord.OrdAction = KOrderAction::ACTION_CXL_ALL;
						ord.massscope = EnumMassActionScope::EnumMassActionScope_Instrument;
						strcpy(ord.symbol, cmd.cmd_symbol.c_str());
						ord.mktsegid = segid;
						SendMassCxl(conns[i], ord, _SEGS[i]);
					}
				}

			}break;

		case ExchCmd::TERMINATE:
			{
				LogMe("GOT TERMINATE MESSAGE IN Q", LogLevel::WARN);
				for (size_t i = 0; i < _SEGS.size(); i++)
				{
					//if the seg is has an FD > 0, it's ok
					if (conns[i].tcpHandler->Fd() > 0)
					{
						ostringstream oss;
						oss << "TERMINATING SESSION ON SEGID=" << i;
						KT01_LOG_WARN(__CLASS__, oss.str());
						//send s shutdown....
						SendTerminateMsg(conns[i], "USER TERMINATES SESSION", 0);
						conns[i].sessmgr.CurrentState = EnumSessionState::Terminating;
						usleep(100000);
					}
				}
			}break;
		case ExchCmd::PRINT_LATENCIES:
		{
			bool clear = (cmd.cmd_value == 12) ? true : false;
			PrintLatencies(clear);
		}break;
		}
	}

	_State.EVENT_WAITING.store(false, std::memory_order_relaxed);
}


void CmeGT5::Reset()
{
}

void CmeGT5::PrintLatencies(bool clear)
{
	LogMe("--- LATENCY STATS ---",LogLevel::INFO);

	if (_State.MEASURE)
	{
		_State.MEASURE = false;
		_trk.printMeasurements();
		_State.MEASURE = true;
	}
	else
		LogMe("  ->MEASURE IS NOT ENABLED.");

	if (clear)
		_trk.clear();
}

void CmeGT5::Command(Instruction cmd)
{
	LogMe("*** " + _State.EXCHNAME + " SESS GOT GENERIC COMMAND REQUEST: "
				+ ExchCmd::toString(cmd.command), LogLevel::INFO);

	switch(cmd.command)
	{
	case ExchCmd::PARTY_DET_REQ:
	case ExchCmd::PARTY_LIST_REQ:
	case ExchCmd::RETRANS_REQ:
	case ExchCmd::MASS_ORDER_STATUS_REQ:
	case ExchCmd::SESS_STATUS:
		_qCmds.push(cmd);
		_State.EVENT_WAITING.store(true, std::memory_order_relaxed);
		break;
	case ExchCmd::MEASURE:
	{
		if (_State.MEASURE)
		{
			LogMe("DISABLING LATENCY MEASURING", LogLevel::WARN);
			_State.MEASURE = false;
		}
		else
		{
			LogMe("ENABLING LATENCY MEASURING", LogLevel::WARN);
			_State.MEASURE = true;
		}
	}break;
	default:
		_qCmds.push(cmd);
		_State.EVENT_WAITING.store(true, std::memory_order_relaxed);
	}
}

void CmeGT5::PrintSessionMap(ConnState (&conns)[_MAX_SEGS])
{
	LogMe("*** IL3 SESSION STATE MAP***", LogLevel::INFO);
	for(size_t i = 0; i < _SEGS.size(); i++)
		conns[i].sessmgr.PrintStatus();
}


void CmeGT5::SendMassAction(KTN::Order& ord)
{
	//NOTE: At this point, we just cancel all orders for all segements. This can change later  but for now...
	if (ord.massscope == EnumMassActionScope::EnumMassActionScope_MarketSegmentID)
	{
		Instruction cmd = {};
		cmd.command = ExchCmd::CXL_ALL_SEGS;
		cmd.cmd_value = EnumMassActionScope::EnumMassActionScope_MarketSegmentID;
		_qCmds.push(cmd);
		_State.EVENT_WAITING.store(true, std::memory_order_relaxed);
	}
}

void CmeGT5::SendWarm(int index)
{
	//_CX[index].tcpHandler->SendWarm();
	//_CX[index].tcpHandler->SendWarm();
}


void CmeGT5::Send(KTN::OrderPod& ord, int action)
{
	int index = GetConnIndex(_CX, ord.mktsegid);

	if (UNLIKELY(index < 0))
	{
		KT01_LOG_ERROR(__CLASS__, "ORD " + OrderUtils::toString(ord.ordernum) + " SEND ERROR: SEGMENT " + std::to_string(ord.mktsegid) + " NOT FOUND IN ACTIVE CONNECTIONS!");

		std::ostringstream ss;
		ss << "Failed to send order " << ord.ordernum << ". Market segment " << static_cast<uint16_t>(ord.mktsegid) << " is not configured";

		Notification n = {};
		n.exch = _State.EXCHNAME;
		n.source = _State.SOURCE;
		n.org = _State.ORG;
		n.level = MsgLevel::ERROR;
		n.imsg = MsgType::ALERT;
		n.text = ss.str();
		NotifierRest::instance().Notify(n);

		return;
	}

	if (UNLIKELY(!ConnState::OkToSend(_CX[index].sessmgr.CurrentState)))
	{
		KT01_LOG_ERROR(__CLASS__, "ORD " + OrderUtils::toString(ord.ordernum) + " SEND ERROR: SEGMENT " + std::to_string(ord.mktsegid)
				+ " CONNECTION IS STATE=" + EnumSessionState::toString(_CX[index].sessmgr.CurrentState) + " !");

		std::ostringstream ss;
		ss << "Failed to send order "
			<< ord.ordernum
			<< ". Market segment "
			<< static_cast<uint16_t>(ord.mktsegid)
			<< " has state " << EnumSessionState::toString(_CX[index].sessmgr.CurrentState);

		Notification n = {};
		n.exch = _State.EXCHNAME;
		n.source = _State.SOURCE;
		n.org = _State.ORG;
		n.level = MsgLevel::ERROR;
		n.imsg = MsgType::ALERT;
		n.text = ss.str();
		NotifierRest::instance().Notify(n);

		return;
	}

	uint64_t seq = _CX[index].sessmgr.SeqOutAndAdvance();
	uint16_t msgtype = 0;

	KTNBuf* msg = _KtnBufs.get();

	switch (ord.OrdAction)
	{
	case KOrderAction::ACTION_NEW:
	{
		msgtype = IL3::META::NewOrderSingleMeta::id;
		if (_State.MEASURE)
		{
			_trk.startMeasurement("newFact");
		}
		if (UNLIKELY(ord.isManual))
		{
			_fastfact.NewOrderEncodeMan(ord, ord.orderReqId, seq, *msg, _State.logToScreen);
		}
		else
		{
			msg->buffer_length = _fastfact.NewOrderEncodeAutoV42(
				ord.ordernum,
				ord.quantity,
				ord.price,
				ord.secid,
				ord.OrdSide,
				ord.OrdTif,
				ord.orderReqId,
				seq,
				msg->buffer);
		}
		if (_State.MEASURE)
		{
			_trk.stopMeasurement("newFact");
		}
		break;
	}

	case KOrderAction::ACTION_MOD:
	{
		msgtype = IL3::META::OrderCancelReplaceRequestMeta::id;
		if (_State.MEASURE)
		{
			_trk.startMeasurement("modFact");
		}
		if (UNLIKELY(ord.isManual))
		{
			_fastfact.ModifyOrderEncodeMan(ord, seq, *msg);
		}
		else
		{
			msg->buffer_length = _fastfact.ModifyOrderEncodeAutoV4(ord, seq, msg->buffer);
		}
		if (_State.MEASURE)
		{
			_trk.stopMeasurement("modFact");
		}
		break;
	}

	case KOrderAction::ACTION_CXL:
	{
		msgtype = IL3::META::OrderCancelRequestMeta::id;

		if (_State.MEASURE)
		{
			_trk.startMeasurement("cxlFact");
		}
		if (UNLIKELY(ord.isManual))
		{
			_fastfact.CancelOrderEncodeMan(ord, seq, *msg);
		}
		else
		{
			_fastfact.CancelOrderEncodeAuto(ord, seq, *msg);
		}
		if (_State.MEASURE)
		{
			_trk.stopMeasurement("cxlFact");
		}
		break;
	}

	default:
		KT01_LOG_ERROR(__CLASS__, "ORDER NOT HANDLED. INVALID SEND ACTION!!!");
		KT01_LOG_ERROR(__CLASS__, OrderUtils::Print(ord));
		return;
	}

	msg->audit_trail = true;

	if (_State.MEASURE) _trk.startMeasurement("sendGT5");
	_CX[index].tcpHandler->SendFastPath(msg->buffer, msg->buffer_length);
	const uint64_t sendTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_CX[index].AddOrderRecord(ord, seq);
	if (_State.MEASURE) _trk.stopMeasurement("sendGT5");

	LOGWARN("[CmeGT5] | Sent order {} | recvTime={} | qWrite={} | sendTime={} | recvToQ={} | qToSend={} | total={}",
		ord.orderReqId,
		ord.recvTime,
		ord.queueWriteTime,
		sendTime,
		ord.queueWriteTime - ord.recvTime,
		sendTime - ord.queueWriteTime,
		sendTime - ord.recvTime
	);

	if (_State.USE_AUDIT){
		if (_State.MEASURE) _trk.startMeasurement("auditOut");
		//_audit->Write(msg->buffer, msgtype, ord.mktsegid);
		msg->msgtype = msgtype;
		msg->segid = ord.mktsegid;
		msg->index = index;
		_audit->Write(msg);
		if (_State.MEASURE) _trk.stopMeasurement("auditOut");
	}
	else {
		// If audit is used, the buffer will be given back to the pool after it's logged in the audit file
		_KtnBufs.put(msg);
	}
}


void CmeGT5::SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3> & hdg)
{
	if (_State.MEASURE) _trk.startMeasurement("HdgMsgMapIns");
		_hedgeMap[ordereqid] = hdg;
		//_hedgeMap.insert(ordereqid, hdg);
	if (_State.MEASURE) _trk.stopMeasurement("HdgMsgMapIns");


#ifdef ALGO_TEST
	ostringstream oss;
	oss << "HEDGE INSERTING REQ ID=" << ordereqid << " LEN=" << _hedgeMap.size() ;
	LogMe(oss.str());
#endif
}


bool CmeGT5::HedgeComplex(uint64_t ordreqid, int lastqty, int64_t lastpx, uint8_t cbid)
{
	_sprd_hdgs = _hedgeMap.find(ordreqid);
	if (_sprd_hdgs.size() == 0)
		return false;


	int i = 0;
	for(SpreadHedgeV3& hdg : _sprd_hdgs)
	{
		if (LIKELY(hdg.ratio == 1))
			hdg.qty = lastqty;
		else
		{
			float newqty  = hdg.ratio * lastqty;
			hdg.qty = static_cast<int>(newqty);
		}

		
		cbid = hdg.cbid;

		std::pair<uint64_t, char*> idpair;
		_qHdgClOrdIDs.try_dequeue(idpair);

		i++;
		//here, we shortcut this because:
		// 1. the ordreqid is coming from the fill, and
		// 2. each order_request_id is incremented by 25 for this purpose
		// 3. the existing ordreqid that's passed in from the fill is already encoded
		// with the cbid, so no need to do this again.
		uint64_t newreqid = ordreqid + i;

		int index = GetConnIndex(_CX, hdg.mktsegid);

		uint64_t seq = _CX[index].sessmgr.SeqOutAndAdvance();

		KTNBuf* msg = _KtnBufs.get();
		msg->buffer_length = _fastfact.NewOrderEncodeAutoV42(idpair.second, hdg.qty,
										hdg.price, hdg.secid, hdg.OrdSide, KOrderTif::Enum::DAY, newreqid, seq, msg->buffer);
		msg->audit_trail = true;
		msg->index = hdg.instindex;
		msg->segid = hdg.mktsegid;

		_CX[index].tcpHandler->SendFastPath(msg->buffer, msg->buffer_length);

		_hdgbufs.push_back(msg);
	}

	for(auto kbuf : _hdgbufs)
	{
		_qHdgProc.push(*kbuf);
	}

	_sprd_hdgs.clear();
	_hdgbufs.clear();

	return true;
}

void CmeGT5::ProcessHdgBuf(std::vector<std::unique_ptr<KTNBuf>>& vbufs)
{
	LogMe("PROCESSING HEDGE BUFFERS: " + std::to_string(vbufs.size()));

	for (const auto& kbuf : vbufs) // Use const auto& to avoid copying unique_ptr
	{
		if (_State.USE_AUDIT) {
			if (_State.MEASURE) _trk.startMeasurement("auditOut");
			//_audit->Write(kbuf->buffer, IL3::META::NewOrderSingleMeta::id, kbuf->segid); // Not currently using this function
			if (_State.MEASURE) _trk.stopMeasurement("auditOut");
		}

		NewOrderSingleMsg* msg = reinterpret_cast<NewOrderSingleMsg*>(kbuf->buffer);

		KOrderSide::Enum side = (msg->Side == EnumSideReq::EnumSideReq_Buy)
				? KOrderSide::BUY : KOrderSide::SELL;

		int32_t price = PriceConverters::FromMantissa9(msg->Price);


		KTN::OrderPod ord = {};
		OrderUtils::fastCopy(ord.ordernum, msg->ClOrdID, sizeof(ord.ordernum));
		ord.orderReqId = msg->OrderRequestID;
		ord.secid = msg->SecurityID;
		ord.OrdAction = KOrderAction::ACTION_NEW;
		ord.mktsegid = kbuf->segid;
		ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_HEDGE;
		ord.OrdStatus = KOrderStatus::HEDGE_SENT;
		ord.quantity = msg->OrderQty;
		ord.price = price;
		ord.OrdSide = side;
		ord.leavesqty = msg->OrderQty;

		_qOrdsProc.push(ord);

		if (_State.MEASURE)
		{
			ostringstream oss;
			oss << "**HDG SENT " << ord.ordernum << " SIDE=" << KOrderSide::toString(ord.OrdSide)
					<<" " << " PX=" << ord.price
					<< " QTY=" << ord.quantity //<< " SEG=" << (int)ord.mktsegid
					<<" SECID=" << ord.secid;
					//<< " REQID=" << ord.orderReqId << " CLORDID=" << ord.ordernum;
			LogMe(oss.str(),LogLevel::INFO);
		}

	}
}


void CmeGT5::SendMassCxl(ConnState& conn, KTN::Order ord, int segid)
{
	EnumMassActionScope scope;

	int id = 0;

	if (ord.massscope == EnumMassActionScope::EnumMassActionScope_Instrument){
		scope = EnumMassActionScope::EnumMassActionScope_Instrument;
		ord.exchange = "CME";
		ord.partydetails = _sett.PartyListId;

		CmeSecDef def = CmeSecMaster::instance().getSecDef(ord.symbol);
		ord.secid = def.secid;
		ord.product = def.product;
		ord.OrdProdType = def.prodtype;
		ord.mktsegid = def.msgw;

		id = def.secid;
	}

	if (ord.massscope == EnumMassActionScope::EnumMassActionScope_MarketSegmentID)
	{
		scope = EnumMassActionScope::EnumMassActionScope_MarketSegmentID;
		id = ord.mktsegid;
		LogMe("GOT MKT SEGMENT MASS CXL FOR SEGID=" + std::to_string(segid));
	}


	uint64_t reqid = conn.sessmgr.NewReqId();
	uint64_t seq =  conn.sessmgr.SeqOutAndAdvance();

	if (UNLIKELY(!ConnState::OkToSend(conn.sessmgr.CurrentState))){
		ostringstream oss;
		oss << "SEGMENT ID " << segid << " IS NOT CONNECTED! CANNOT SEND!";
		KT01_LOG_ERROR(__CLASS__, oss.str());
		return;
	}

	//KTNBuf* msg = new KTNBuf();
	//_appfact.MassActionEncode(msg, scope, id, reqid, _sett.PartyListId, seq);
	KTNBuf msg = AppMessageGen::MassActionEncode(_sett, scope, id, reqid, seq);
	msg.audit_trail = true;

	if (_State.MEASURE) _trk.startMeasurement("sendGT5");
		//_qBufs.push(msg);
		conn.tcpHandler->SendFastPath(msg.buffer, msg.buffer_length);
	if (_State.MEASURE) _trk.stopMeasurement("sendGT5");


	ostringstream oss;
	oss << "SENDING MASS CXL FOR SEG " << segid;
	LogMe(oss.str(), LogLevel::WARN);


	conn.sessmgr.Update();

	if (_State.USE_AUDIT){
		KTNBuf *msg2 = _KtnBufs.get();
		memcpy(msg2->buffer, msg.buffer, msg.buffer_length);
		msg2->buffer_length = msg.buffer_length;
		msg2->dir = -1;
		msg2->msgtype = IL3::META::OrderMassActionRequestMeta::id;
		msg2->segid = segid;
		msg2->seqnum = seq;
		if (_State.MEASURE) _trk.startMeasurement("auditOut");
		_audit->Write(msg2);
		if (_State.MEASURE) _trk.stopMeasurement("auditOut");
	}
}

void CmeGT5::SendMassOrderStatusReq(ConnState& conn, EnumMassStatusReqTyp reqtype, int segid)
{
	EnumMassStatusReqTyp scope = reqtype;

	int id = segid;

	//for now, let's just support this
	if (reqtype != EnumMassStatusReqTyp::EnumMassStatusReqTyp_MarketSegment){
		KT01_LOG_ERROR(__CLASS__, "UNSUPPORTED MASS STATUS REQ TYPE. ONLY MKT SEGMENT LEVEL REQUESTS SUPPORTED AT THIS TIME.");
		return;
	}

	uint64_t reqid = conn.sessmgr.NewReqId();
	uint64_t seq =  conn.sessmgr.SeqOutAndAdvance();

	if (UNLIKELY(!ConnState::OkToSend(conn.sessmgr.CurrentState))){
		ostringstream oss;
		oss << "[ERROR] SEGMENT ID " << segid << " IS NOT CONNECTED! CANNOT SEND!";
		KT01_LOG_ERROR(__CLASS__, oss.str());
		return;
	}


	KTNBuf msg = AppMessageGen::MassOrdStatusEncode(_sett, scope, id, reqid, seq);
	msg.audit_trail = true;

	ostringstream oss;
	oss << "Sending MassOrderStatusRequest for Seg=" << segid << " MsgLen=" << msg.buffer_length;
	LogMe(oss.str());

	conn.tcpHandler->SendFastPath(msg.buffer, msg.buffer_length);


	conn.sessmgr.Update();

	if (_State.USE_AUDIT){
		KTNBuf *msg2 = _KtnBufs.get();
		memcpy(msg2->buffer, msg.buffer, msg.buffer_length);
		msg2->buffer_length = msg.buffer_length;
		msg2->dir = -1;
		msg2->msgtype = IL3::META::OrderMassStatusRequestMeta::id;
		msg2->segid = segid;
		msg2->seqnum = seq;

		if (_State.MEASURE) _trk.startMeasurement("auditOut");
		_audit->Write(msg2);
		if (_State.MEASURE) _trk.stopMeasurement("auditOut");
	}
}

//*********** KTN MSG PROCESSING *****************************************

void CmeGT5::onStateChange(EnumSessionState::Enum newState, ConnState& conn)
{
	conn.sessmgr.onStateChange(newState);
}

void CmeGT5::ProcessSessionMessage(ConnState& conn, KTNBuf8t &msg)
{
	switch (msg.msgtype)
	{
		case NegotiationResponseMsg::id:
			onNegotiateResponseMsg(conn, msg.buffer, msg.buffer_length, msg.segid);
			break;
		case NegotiationRejectMsg::id:
			onNegotiateRejectMsg(conn, msg.buffer, msg.buffer_length, msg.segid);
			break;
		case EstablishmentAckMsg::id:
			onEstablishAckMsg(conn, msg.buffer, msg.buffer_length, msg.segid);
			break;
		case EstablishmentRejectMsg::id:
			onEstablishRejectMsg(conn, msg.buffer, msg.buffer_length, msg.segid);
			break;
		case SequenceRecvMsg::id:
			onSequenceMsg(conn, msg.buffer, msg.buffer_length, msg.segid);
			break;
		case TerminateRecvMsg::id:
			onTerminateMsg(conn, msg.buffer, msg.buffer_length, msg.segid);
			break;
		case RetransmissionMsg::id:
			onRetransmissionMsg(conn, msg.buffer, msg.buffer_length, msg.segid);
			break;
		case RetransmitRejectMsg::id:
			onRetransmitRejectMsg(conn, msg.buffer, msg.buffer_length, msg.segid);
			break;
		case NotAppliedMsg::id:
			onNotAppliedMsg(conn, msg.buffer, msg.buffer_length, msg.segid);
			break;
		default:
			KT01_LOG_ERROR(__CLASS__, "SESS MSG PROC: TEMPLATE {} NOT HANDLED!", msg.msgtype);
			break;
	}

}

void CmeGT5::NegotiateSession(ConnState& conn)
{
//	//Ok. Some explanation here
//	/*
//	 * If the session is already negotiated, that means we are NOT receiving a new UUID
//	 * If the session is NOT negotiated, we WILL BE receiving a new UUID
//	 *
//	 * If we DON'T receive a new UUID, we don't need to update the stored values, as they should already be there
//	 * If we DO receive a NEW UUID, we will want to store the OLD UUID's seqnums
//	 */

	conn.sessmgr.ResetSessionSeqNums();

	int segid = conn.segid;
	onStateChange(EnumSessionState::AwaitNegotiationResponse, conn);

	KTN_MSG msg = _sessfact.NegotiateEncode(_State.DebugHmac);
	conn.tcpHandler->Send(msg.buffer, msg.buffer_length, false);

	LogMe("NEGOTITATING SESION. SEGID=" + std::to_string(segid) + ". NEGOTIATION MESSAGE SENT.");
	setLastSentTime(conn);
}

void CmeGT5::EstablishSession(ConnState& conn)
{
	onStateChange(EnumSessionState::AwaitEstablishmentAck, conn);

	KTNBuf msg = _sessfact.EstablishEncode(conn.sessmgr.Uuid(), conn.sessmgr.SeqOut(), _State.DebugHmac);
	conn.tcpHandler->Send(msg.buffer, msg.buffer_length, false);

	setLastSentTime(conn);
}

void CmeGT5::onNegotiateResponseMsg(ConnState& conn, byte_ptr buf, int iMsgLen,
		int segid) 
{

	ostringstream oseg;
	oseg << "SEGID=" << segid;

	auto msg = reinterpret_cast<NegotiationResponseMsg::BlockData*>(buf);
    buf += NegotiationResponseMsg::blockLength;

	if (_State.DebugSessMsgs)
	{
		cout << "[onNegotiateResponseMsg DEBUG] SEGID=" << segid
				<< " UUID=" << msg->UUID 
				<< " PREV_UUID=" << msg->PreviousUUID
				<< " PREV_SEQ=" << msg->PreviousSeqNo
				<< endl;
	}

    ostringstream oss;
    oss << "SEG " << segid << " NEGOTIATE RESPONSE MSG: UUID=" << msg->UUID
    		<< " PREV_SEQ=" << msg->PreviousSeqNo << " PREV_UUID=" << msg->PreviousUUID;
    LogMe(oss.str(), LogLevel::OK);

    if (CheckFaultTolerance(msg->FaultToleranceIndicator, segid, "onNegotiateResponseMsg"))
    {
    	string fti = (msg->FaultToleranceIndicator == EnumFTI::EnumFTI_Primary) ? "PRIMARY" : "BACKUP";
    	LogMe("Fault Tolerance Check OK: FTI=" + oss.str(), LogLevel::INFO);
    }

	// uint32_t seqin = conn.sessmgr.SeqIn();
	// uint32_t seqout = conn.sessmgr.SeqOut();

	// conn.sessmgr.SetPrevSeqIn(seqin);
	// conn.sessmgr.SetPrevSeqOut(seqout);
	// LogMe("SETTING PREV_SEQ_IN FOR SESS=" + std::to_string(conn.sessmgr.PrevSeqIn()));

	//conn.sessmgr.SetSeqOut(1, false);
    //conn.sessmgr.SetNegotiated(true);
	conn.sessmgr.UpdateCurrentSessionValues(true, msg->UUID, msg->PreviousUUID);
    // conn.sessmgr.SetPrevuuid(msg->PreviousUUID);
    // conn.sessmgr.Update();

    onStateChange(EnumSessionState::Negotiated, conn);
}

void CmeGT5::onNegotiateRejectMsg(ConnState& conn, byte_ptr buf, int iMsgLen,
		int segid)
{
	auto msg = reinterpret_cast<NegotiationRejectMsg::BlockData*>(buf);
    buf += NegotiationRejectMsg::blockLength;

    string szreason = ErrorCodes::getSessionErrorMessage(msg->ErrorCodes);

    ostringstream oss;
    oss << "SEG " << segid << " NEGOTIATE **REJECT** MSG: UUID=" << msg->UUID
    		<< " CODE=" << msg->ErrorCodes
			<< " REASON=" << msg->Reason
    		<< " TEXT=" << szreason;

	KT01_LOG_ERROR(__CLASS__, oss.str());

    //falsify base case
	CheckFaultTolerance(msg->FaultToleranceIndicator, segid, "onNegotiateReject");


    //int index = GetConnIndex(segid);
    conn.tcpHandler->Stop();

    onStateChange(EnumSessionState::Disconnected, conn);
    usleep(100000);
    conn.sessmgr.Stop();
}

void CmeGT5::onEstablishAckMsg(ConnState& conn, byte_ptr buf, int iMsgLen,
		int segid) {

	auto msg = reinterpret_cast<EstablishmentAckMsg::BlockData*>(buf);
    buf += EstablishmentAckMsg::blockLength;


    onStateChange(EnumSessionState::Established, conn);

    ostringstream oss;
    oss << "OK SEG " << segid << " ESTABLISH ACK MSG: UUID=" << msg->UUID << " PREVIOUS_UUID=" << msg->PreviousUUID
			<< " NEXT_SEQ_IN=" << msg->NextSeqNo << " PREV_SEQ_IN=" << msg->PreviousSeqNo;
    LogMe(oss.str(),LogLevel::OK);

	if (CheckFaultTolerance(msg->FaultToleranceIndicator, segid, "onEstablishAckMsg"))
	{
		string fti = (msg->FaultToleranceIndicator == EnumFTI::EnumFTI_Primary) ? "PRIMARY" : "BACKUP";
		LogMe("Fault Tolerance Check [OK]: FTI=" + fti, LogLevel::OK);
	}

	if (_State.DebugAppMsgs)
		cout << "[onEstablish DEBUG] msg->PreviousSeqNo=" << msg->PreviousSeqNo
			<< " msg->NextSeqNo=" << msg->NextSeqNo
			<< " SessMgr SeqIn=" << conn.sessmgr.SeqIn()
			<< " SessMgr PrevSeqIn=" << conn.sessmgr.PrevSeqIn() 
			<< " msg->UUID=" << msg->UUID << " SessMgr UUID=" << conn.sessmgr.Uuid()
			<< " msg->PrevUUID=" << msg->PreviousUUID << " SessMgr=" << conn.sessmgr.PrevUUID() << endl;

	conn.sessmgr.UpdatePreviousSessionValues(msg->UUID, msg->PreviousUUID);


	bool sess_ok = false;
	//the 2 scenarios are: Either the UUID = last or it doesn't.
	if (msg->UUID == msg->PreviousUUID)
	{
		LogMe("MIDWEEK SESSION: UUIDS ARE EQUAL. UUID=" + std::to_string(msg->UUID)
				+ " PREV_UUID=" + std::to_string(msg->PreviousUUID)
				+ " PREV_SEQ=" + std::to_string(msg->PreviousSeqNo), LogLevel::INFO);

		if (msg->PreviousSeqNo == conn.sessmgr.SeqIn())
		{
			if (_State.DebugAppMsgs)
				cout << "[onEstablish DEBUG] Connection OK. PreviousSeqNo=" << msg->PreviousSeqNo
					<< " SessMgr SeqIn=" << conn.sessmgr.SeqIn() 
					<< " UUID=" << msg->UUID << " SessMgr UUID=" << conn.sessmgr.Uuid() << endl;

			sess_ok = true;
		}
		else
		{

			cout << "[onEstablish WARNING] Sequences are not as expected. PreviousSeqNo=" << msg->PreviousSeqNo
			<< " SessMgr SeqIn=" << conn.sessmgr.SeqIn() << endl;

			if (msg->PreviousSeqNo > conn.sessmgr.PrevSeqIn())
			{
				//Found a gap from a previously negotiated UUID
				int diff = msg->PreviousSeqNo - conn.sessmgr.SeqIn();

				/*
						Note: we put in the flag here to catch the case when we reset like above to 0, expecting the
					next current sequence = 1
					When this occurs, and we have a newly negotiated session, we have moved the last SAVED seq in,
					in this case 0, to the PrevSeqIn() value in the sessmgr.  So, in that case the PrevSeqIn will ALWAYS
					be greater than the one we saved (0), causing an artificial gap.
					TODO: In later versions, we should correct this behavior to check for NextSeqs all time, or some other way
				*/
				if (diff > 0 && conn.sessmgr.SeqIn() > 0)// conn.sessmgr.PrevSeqIn() > 0)
				{
					LogMe("GAP DETECTED: Sequence mismatch for Current UUID. WE KNOW SEQIN="
							+ std::to_string(conn.sessmgr.SeqIn()) + ". EXCHANGE KNOWS PREVSEQIN=" + std::to_string(msg->PreviousSeqNo), LogLevel::HIGHLIGHT);

					//we have to adjust to what would be expected, not what was received
					//if the previous expected seqnum we know is 1 but they know 2,
					//in current session terms, our expected values are those numbers + 1...
					uint32_t exchExpected = msg->NextSeqNo;
					uint32_t ourExpected = conn.sessmgr.SeqIn() + 1;
					//Register the gap
					conn.sessmgr.GapDetected(ourExpected, exchExpected, msg->UUID);

					//Send the retrans
					SendRetransmitRequestMsg(conn, ourExpected, diff, msg->UUID);
				}
				
			}
			else {
                cout << "[onEstablish] ERROR FATAL: Mid-week same UUID, received PreviousSeqNo (" 
                     << msg->PreviousSeqNo << ") less than expected (" << conn.sessmgr.SeqIn() << ")." << endl;
					 std::exit(1);
                // TODO: Handle error.
            }

		}
	}
	else
	{
		LogMe("NEW SESSION: UUIDS ARE NOT EQUAL. UUID=" + std::to_string(msg->UUID)
				+ " PREV_UUID=" + std::to_string(msg->PreviousUUID)
				+ " PREV_SEQ=" + std::to_string(msg->PreviousSeqNo), LogLevel::HIGHLIGHT);
		//UUids are not equal, indicating a new session has been negotiated and is being established.  
		if (_State.DebugSessMsgs)
			cout << "[onEstablish WARN] UUIDs are NOT equal. msg->UUID=" 
					<< msg->UUID << " msg->PreviousUUID=" << msg->PreviousUUID
					<< " msg->NextSeqNo=" << msg->NextSeqNo
					<< " msg->PreviousSeqNo=" << msg->PreviousSeqNo
					<< " SessMgr SeqIn=" << conn.sessmgr.SeqIn() 
					<< " SessMgr PrevSeqIn=" << conn.sessmgr.PrevSeqIn() 
					<< " SessMgr UUID=" << conn.sessmgr.Uuid()
					<< " SessMgr PrevUUID=" << conn.sessmgr.PrevUUID() << endl;
		
		
		//conn.sessmgr.UpdatePreviousSessionValues(msg->PreviousUUID, msg->PreviousSeqNo);
		if (msg->NextSeqNo == 1 && msg->PreviousUUID != 0)
		{
			//this is the CME definition of having a new session that needs to send a retrans
			conn.sessmgr.SetSeqIn(0, true);
			sess_ok = true;
		}
		else if (msg->NextSeqNo == 1 && msg->PreviousUUID == 0)
		{
			//this is the CME definition of having a new session that needs to send a retrans
			conn.sessmgr.SetSeqIn(0, true);

			if (msg->PreviousSeqNo > conn.sessmgr.PrevSeqIn())
			{
				//Found a gap from a previously negotiated UUID
				int diff = msg->PreviousSeqNo - conn.sessmgr.SeqIn();//  conn.sessmgr.PrevSeqIn();

				/*
						Note: we put in the flag here to catch the case when we reset like above to 0, expecting the
					next current sequence = 1
					When this occurs, and we have a newly negotiated session, we have moved the last SAVED seq in,
					in this case 0, to the PrevSeqIn() value in the sessmgr.  So, in that case the PrevSeqIn will ALWAYS
					be greater than the one we saved (0), causing an artificial gap.
					TODO: In later versions, we should correct this behavior to check for NextSeqs all time, or some other way
				*/
				if (diff > 0 && conn.sessmgr.SeqIn() > 0)// conn.sessmgr.PrevSeqIn() > 0)
				{
					LogMe("GAP DETECTED: Sequence mismatch for Current UUID. WE KNOW SEQIN="
							+ std::to_string(conn.sessmgr.SeqIn()) + ". EXCHANGE KNOWS PREVSEQIN=" + std::to_string(msg->PreviousSeqNo), LogLevel::HIGHLIGHT);

					//we have to adjust to what would be expected, not what was received
					//if the previous expected seqnum we know is 1 but they know 2,
					//in current session terms, our expected values are those numbers + 1...
					uint32_t exchExpected = msg->NextSeqNo;
					uint32_t ourExpected = conn.sessmgr.SeqIn() + 1;
					//Register the gap
					conn.sessmgr.GapDetected(ourExpected, exchExpected, msg->UUID);

					//Send the retrans
					SendRetransmitRequestMsg(conn, ourExpected, diff, msg->UUID);
				}
				else
				{
					sess_ok = true;
				}
			}
			else
			{
				sess_ok = true;
			}
		}
		else
		{
			//this is an error situation
			cout << "[onEstablish] FATAL: New Session Negotiated but SeqIn value is incorrect: PrevSeqNo=" 
					<< msg->PreviousSeqNo << " NextSeqNo=" << msg->NextSeqNo
					<< " UUID=" << msg->UUID <<" PrevUUID=" << msg->PreviousUUID << endl;

			std::exit(1);
		}
		
	}


	if (sess_ok && conn.segid != 12)
	{
		//Next, assuming we have established the connection, let's check and see if we have any working orders:
		LogMe("onEstablish OK: Sending Order Status Request");
		SendMassOrderStatusReq(conn,EnumMassStatusReqTyp::EnumMassStatusReqTyp_MarketSegment, conn.segid);
	}
	

    Notification n = {};
	n.exch = _State.EXCHNAME;
	n.source = _State.SOURCE;
	n.org = _State.ORG;
	n.level = MsgLevel::INFO;
	n.imsg = MsgType::NOTICE;
	n.text = oss.str();

	NotifierRest::instance().Notify(n);
}

void CmeGT5::onEstablishRejectMsg(ConnState& conn, byte_ptr buf, int iMsgLen,
		int segid) {

	auto msg = reinterpret_cast<EstablishmentRejectMsg::BlockData*>(buf);
    buf += EstablishmentRejectMsg::blockLength;

    string errtext = ErrorCodes::getSessionErrorMessage(msg->ErrorCodes);

    ostringstream oss;
    oss << "SEG " << segid << " ESTABLISH ** REJECT MSG: ERR=" << msg->ErrorCodes
    		<<" REASON=" << msg->Reason
    		<< " NEXTSEQ=" << msg->NextSeqNo << " ERROR=" << errtext;
    KT01_LOG_ERROR(__CLASS__, oss.str());

    //int index = GetConnIndex(segid);
    conn.sessmgr.SetSeqIn(msg->NextSeqNo);

    onStateChange(EnumSessionState::Disconnected, conn);
    usleep(100000);
    conn.sessmgr.Stop();
}

void CmeGT5::onSequenceMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid) {

	auto msg = reinterpret_cast<SequenceRecvMsg::BlockData*>(buf);

	if (_State.DebugSessMsgs)
		cout << "[onSeqMsg] SeqMsg received. Lapsed=" << (int)msg->KeepAliveIntervalLapsed << " NEXT=" << msg->NextSeqNo << endl;


	//falsify base case
	if (!CheckFaultTolerance(msg->FaultToleranceIndicator, segid, "onSequenceMsg"))
	{
		SendTerminateMsg(conn, "User Terminates Session: Fault Tolerance Changed", 0);
		conn.sessmgr.CurrentState = EnumSessionState::Terminating;
		return;
	}

	if (msg->KeepAliveIntervalLapsed == EnumKeepAliveLapsed::EnumKeepAliveLapsed_Lapsed)
	{
		LogMe("SENDING SEQUENCE MSG IN RESPONSE TO LAPSE NOTIFICATION. WTF?", LogLevel::WARN);
		SendSequenceMsg(conn, msg->KeepAliveIntervalLapsed);
	}
}

void CmeGT5::onTerminateMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid) {

	auto msg = reinterpret_cast<TerminateRecvMsg::BlockData*>(buf);
	std::string text = ErrorCodes::getTerminateMessage(msg->ErrorCodes);

	ostringstream oss;
    oss << "SEG " << segid << " TERMINATE MSG: ERR=" << msg->ErrorCodes << " REASON=" << text;
    KT01_LOG_WARN(__CLASS__, oss.str());

    //INVALID NEXT SEQ
    if (msg->ErrorCodes == 10)
    {
    	LogMe("*** INCREASING MSG SEQ OUT ON ERROR!!!",LogLevel::WARN);
    	conn.sessmgr.IncSeqOut(10);
    	conn.sessmgr.Update();
    }

    //int index = GetConnIndex(segid);
    onStateChange(EnumSessionState::Disconnected, conn);

    for(int i = 0; i < _State.SEGCNT; i++){
		conn.sessmgr.Update();
		usleep(100);
	}
}

void CmeGT5::onRetransmissionMsg(ConnState& conn, byte_ptr buf, int iMsgLen,
		int segid) {

	auto msg = reinterpret_cast<RetransmissionMsg::BlockData*>(buf);
    buf += RetransmissionMsg::blockLength;

    ostringstream oss;
    oss << "SEG " << segid << " RETRANSMISSION RESPONSE FROM=" << msg->FromSeqNo
    			<< " MSGCOUNT=" << msg->MsgCount << " UUID=" << msg->UUID << " LASTUUID=" << msg->LastUUID
				<< "\n-----------------------------------------------------";

    LogMe(oss.str(),LogLevel::WARN);

    //conn.sessmgr.SetRecoveryParams(msg->FromSeqNo, msg->MsgCount);
}

void CmeGT5::onRetransmitRejectMsg(ConnState& conn, byte_ptr buf, int iMsgLen,
		int segid) {
	auto msg = reinterpret_cast<RetransmitRejectMsg::BlockData*>(buf);
    buf += RetransmitRejectMsg::blockLength;

    string errmsg = IL3::ErrorCodes::getRetransmitRejectMessage(msg->ErrorCodes);

    ostringstream oss;
    oss << "SEG " << segid << " RETRANSMISSION REJECT MSG ERR=[" << msg->ErrorCodes << "]:" << errmsg << " REASON=" << msg->Reason
    		<< " UUID=" << msg->UUID << " LASTUUID=" << msg->LastUUID << " SPLITMSG=" << (int)msg->SplitMsg;
	KT01_LOG_ERROR(__CLASS__, oss.str());

    //for now, we put the handler back into a state where it can process messages.
    //TODO: ALERT THE OPERATOR TO TAKE ACTION IF NEEDED
    if (conn.sessmgr.CurrentState == EnumSessionState::WaitingForRetransmission)
    	onStateChange(EnumSessionState::Established, conn);
}

void CmeGT5::onNotAppliedMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid) {
	auto msg = reinterpret_cast<NotAppliedMsg::BlockData*>(buf);
    buf += NotAppliedMsg::blockLength;

    ostringstream oss;
    oss << "SEG " << segid << " NOT APPLIED MSG:  FROM=" << msg->FromSeqNo << " MSGCOUNT=" << msg->MsgCount;
    LogMe(oss.str(),LogLevel::WARN);

    struct timespec now;
	const uint64_t now_ns = ({
		clock_gettime(CLOCK_REALTIME, &now);
		now.tv_sec * INT64_C(1000000000) + now.tv_nsec;
	  });

	//int index = GetConnIndex(segid);
	conn.sessmgr.SetLastHb(now_ns);

    uint32_t exch_knows_seq = msg->FromSeqNo + msg->MsgCount -1;

    conn.sessmgr.SetSeqOut(exch_knows_seq,true);

    oss.clear();
    oss.str("");
    oss << segid << ": onNotApplied NEW SEQ_OUT=" << conn.sessmgr.SeqOut();
    LogMe(oss.str(),LogLevel::WARN);
    //_hlog->Persist(oss.str());

    LogMe(std::to_string(segid) + " SENDING SEQUENCE MSG IN RESPONSE TO NOT APPLIED", LogLevel::WARN);
    //_hlog->Persist("SENDING SEQUENCE MSG IN RESPONSE TO NOT APPLIED");
    SendSequenceMsg(conn, EnumKeepAliveLapsed::EnumKeepAliveLapsed_NotLapsed);
}

int CmeGT5::ProcessApplicationMessage(ConnState& conn, KTNBuf8t &msg, bool ignore_seqs)
{
	int msgbytes = msg.buffer_length;
	uint32_t seqnum = IL3Helpers::extractSeqNum(msg.buffer);

	if (_State.DebugAppMsgs)
		cout << "[ProcessApp] seqnum=" << seqnum << " totalen[including groups]=" << msg.buffer_length << " type=" << msg.msgtype << endl;

	//memcpy(msg.buf, msg.buffer, msg.buffer_length);

	msg.segid = conn.segid;
	msg.seqnum = seqnum;

	if (ignore_seqs)
	{
		 if (_State.DebugAppMsgs)
			 cout << "[ProcessApp] IGNORES_SEQS=TRUE. Processing previous UUID Gap!" << endl;
	}
	else if (!CheckIncomingSeq(conn, msg))
	{
		msgbytes = msg.msg_length;// IL3Helpers::GetLiveBytes(msg);
		if (_State.DebugAppMsgs)
			cout << "[LiveMsgBytes] Returning MsgLen=" << msgbytes << endl;
		return msgbytes;
	}	

	switch (msg.msgtype)
	{
		case ExecutionReportNewMsg::id:
			 onExecRptNew(msg.buffer, msg.buffer_length, msg.segid, msg.recvTime);
			 break;

		 case ExecutionReportModifyMsg::id:
			 onExecRptModify(msg.buffer, msg.buffer_length, msg.segid, msg.recvTime);
			 break;
		 case ExecutionReportCancelMsg::id:
			 onExecRptCancel(msg.buffer, msg.buffer_length, msg.segid, msg.recvTime);
			 break;

		 case ExecutionReportRejectMsg::id:
			 onExecRptReject(msg.buffer, msg.buffer_length, msg.segid, msg.recvTime);
			 break;

		 case ExecutionReportTradeOutrightMsg::id:
			 msgbytes = onExecRptTradeOutright(msg.buffer, msg.buffer_length, msg.segid, msg.index, msg.recvTime);
			 break;
		 case ExecutionReportTradeSpreadMsg::id:
			 msgbytes = onExecRptTradeSpread(msg.buffer, msg.buffer_length, msg.segid, msg.index, msg.recvTime);
			 break;
		 case ExecutionReportTradeSpreadLegMsg::id:
			 msgbytes = onExecRptTradeSpreadLeg(msg.buffer, msg.buffer_length, msg.segid, msg.index, msg.recvTime);
			 break;

		 case PartyDetailsDefinitionRequestAckMsg::id:
		 {
			// msgbytes = PartyDetailsDefinitionRequestAckMsg::blockLength;
			// msgbytes += GROUP_SIZE_LEN;
			// msgbytes += GROUP_SIZE_LEN;

			// msgbytes += 23; // first group: NoPartyDetails; always 1 group
			// msgbytes += 2; // second groupt: NoTrdRegPublications; always 1 group
			msgbytes = onPartyDetailsDefinitionRequestAck(conn, msg.buffer, msg.buffer_length, msg.segid);
		 } break;

		 case PartyDetailsListReportMsg::id:
			 msgbytes = onPartyDetailsListReport(conn, msg.buffer, msg.buffer_length, msg.segid);
			 break;

		 case OrderMassActionReportMsg::id:
		 {
			msgbytes = onOrderMassActionReport(msg.buffer, msg.buffer_length, msg.segid);
		 } break;

		 case BusinessRejectMsg::id:
			 onBusinessReject(conn, msg.buffer, msg.buffer_length, msg.segid);
			 break;

		 case OrderCancelReplaceRejectMsg::id:
			 onOrderCancelReplaceReject(msg.buffer, msg.buffer_length, msg.segid, msg.recvTime);
			 break;

		 case OrderCancelRejectMsg::id:
			 onOrderCancelReject(msg.buffer, msg.buffer_length, msg.segid, msg.recvTime);
			 break;

		 case ExecutionReportTradeAddendumOutrightMsg::id:
			 onTradeAddendumOutright(conn, msg.buffer, msg.buffer_length, msg.segid);
			 break;

		 case ExecutionReportTradeAddendumSpreadMsg::id:
			 onTradeAddendumSpread(conn, msg.buffer, msg.buffer_length, msg.segid);
			 break;

		 case ExecutionReportTradeAddendumSpreadLegMsg::id:
			 onTradeAddendumSpreadLeg(conn, msg.buffer, msg.buffer_length, msg.segid);
			 break;

		 case ExecutionReportEliminationMsg::id:
			 onExecRptElimination(msg.buffer, msg.buffer_length, msg.segid, msg.recvTime);
			 break;
		 case ExecutionReportStatusMsg::id:
			 onExecRptStatus(conn, msg.buffer, msg.buffer_length, msg.segid);
			 break;
		 case ExecutionAckMsg::id:
			 onExecutionAck(conn, msg.buffer, msg.buffer_length, msg.segid);
		 	 break;
		 default:
			 KT01_LOG_ERROR(__CLASS__, "APP MSG PROC: TEMPLATE {} NOT HANDLED!", msg.msgtype);
			 break;
	}

	return msgbytes;
}

bool CmeGT5::CheckIncomingSeq(ConnState& conn, KTNBuf8t &msg)
{
	uint32_t next_expected_seq = conn.sessmgr.SeqIn() + 1;

	LOGINFO("[Incoming DEBUG] Processing message seq {} expected={}", msg.seqnum, next_expected_seq);
	//std::cout << "[Incoming DEBUG] Processing message seq " << msg.seqnum << " expected=" << next_expected_seq << std::endl;

	if (LIKELY(conn.sessmgr.CurrentState == EnumSessionState::Established))
	{
		//std::cout << "[Incoming] Processing message seq " << msg.seqnum << " expected=" << next_expected_seq << std::endl;
		if (LIKELY(msg.seqnum == next_expected_seq))
		{
			if (_State.DebugSession)
				std::cout << "[OK] GT5 Processing message seq " << msg.seqnum << " expected=" << next_expected_seq << std::endl;
			// In-order: process immediately.
			conn.sessmgr.SetSeqIn(msg.seqnum, true);
			return true;
		}
	}

	cout <<"STATE=" << EnumSessionState::toString(conn.sessmgr.CurrentState) << " NEXT=" << next_expected_seq << " SEQ=" << msg.seqnum << endl;
	//uint32_t cast = IL3Helpers::extractSeqNum(msg.buffer);

	if (_State.DebugSession)
		std::cout << "[GT5 Incoming] Processing message seq " << msg.seqnum << " expected=" << next_expected_seq << std::endl;

	CheckSequenceResult result = conn.sessmgr.CheckIncomingSequenceOK(msg);

	if (LIKELY(result.type == CheckSequenceResult::ResultType::OK))
	{
		return true;
	}

	if (result.type == CheckSequenceResult::ResultType::Ignore)
	{
		LogMe("RESULT: IGNORE- BAD DATA! " + std::to_string(msg.seqnum), LogLevel::WARN);
		return false;
	}

	if (result.type == CheckSequenceResult::ResultType::Retransmit)
	{
		LogMe("RESULT: RETRANSMIT REQUESTED", LogLevel::WARN);
		SendRetransmitRequestMsg(conn, result.startSeq, result.numMsgs);
		return false;
	}

	if (result.type == CheckSequenceResult::ResultType::Fatal)
	{
		ostringstream oss;
		oss << "RESULT: FATAL- SEQNUM IS LESS THAN EXPECTED: " << msg.seqnum
				<< " Expected=" << next_expected_seq << " UUID=" << conn.sessmgr.Uuid()
				<< " PrevUUID=" << conn.sessmgr.PrevUUID() << " PrevSeqIn=" << conn.sessmgr.PrevSeqIn();
		KT01_LOG_ERROR(__CLASS__, oss.str());
		SendTerminateMsg(conn, "InvalidLastSeqNo: Lower Than Expected", 12 );
		return false;
	}

	if (result.type == CheckSequenceResult::ResultType::Queued)
	{
		LogMe("RESULT: QUEUED MESSAGE " + std::to_string(msg.seqnum), LogLevel::WARN);
		return false;
	}

	if (result.type == CheckSequenceResult::ResultType::ProcessThenRetransmit) {
		LogMe("------------------------------------------", LogLevel::INFO);
		LogMe("RESULT: PROCESS THEN RETRANSMIT", LogLevel::INFO);
		LogMe(" 1. --> PROCESSING MESSAGES", LogLevel::INFO);
	
		onStateChange(EnumSessionState::Established, conn);
	
		for (RecoveredMessage& r : result.messages)
		{
			KTNBuf8t msg(r.payload.data(), r.payload.size());
			msg.seqnum = r.seqnum;
			msg.msgtype = r.msgtype;
			msg.msg_length = r.msg_length;
			msg.poss_dupe = r.poss_dupe;
			msg.prev_uuid_dupe = r.prev_uuid_dupe;

			uint32_t cast = IL3Helpers::extractSeqNum(msg.buffer);
			std::cout << "[Reprocess] sending message " << msg.seqnum << " for processing. Cast=" << cast << std::endl;

			ProcessApplicationMessage(conn, msg);
		}
	
		LogMe(" 2. --> RESULT: NOW SENDING RETRANSMIT MESSAGE...", LogLevel::WARN);
		onStateChange(EnumSessionState::WaitingForRetransmission, conn);
		usleep(100);
		SendRetransmitRequestMsg(conn, result.startSeq, result.numMsgs);
	
		return false;
	}
	

	if (result.type == CheckSequenceResult::ResultType::Process) {
		LogMe("OK RESULT: PROCESSING QUEUED MESSAGES: " + std::to_string(result.messages.size()), LogLevel::OK);
		onStateChange(EnumSessionState::Established, conn);
	
		for (RecoveredMessage& r : result.messages)
		{
			KTNBuf8t msg(r.payload.data(), r.payload.size());
			msg.seqnum = r.seqnum;
			msg.msgtype = r.msgtype;
			msg.msg_length = r.msg_length;
			msg.poss_dupe = r.poss_dupe;
			msg.prev_uuid_dupe = r.prev_uuid_dupe;
	
			uint32_t cast = IL3Helpers::extractSeqNum(msg.buffer);
			if (_State.DebugGapDetection) {
				std::cout << "[Reprocess] sending message " << msg.seqnum
						  << " for processing. Cast=" << cast
						  << " Len=" << msg.msg_length
						  << " Type=" << msg.msgtype << std::endl;
			}
	
			ProcessApplicationMessage(conn, msg);
		}

		if (conn.segid != 12)
		{
			LogMe("Retransmission complete and onEstablish OK: Sending Order Status Request");
			SendMassOrderStatusReq(conn, EnumMassStatusReqTyp::EnumMassStatusReqTyp_MarketSegment, conn.segid);
		}
	
		return false;
	}

	
	if (result.type == CheckSequenceResult::ResultType::ProcessPrevUUID) {
		LogMe("RESULT: PROCESSING PREV UUID MESSAGES: " + std::to_string(result.messages.size()), LogLevel::INFO);
		onStateChange(EnumSessionState::Established, conn);
	
		for (RecoveredMessage& r : result.messages)
		{
			KTNBuf8t msg(r.payload.data(), r.payload.size());
			msg.seqnum = r.seqnum;
			msg.msgtype = r.msgtype;
			msg.msg_length = r.msg_length;
			msg.poss_dupe = true;
			msg.prev_uuid_dupe = true;

			uint32_t cast = IL3Helpers::extractSeqNum(msg.buffer);
			if (_State.DebugGapDetection) {
				std::cout << "[Repro PrevUUID] sending prev uuid message " << msg.seqnum
						  << " for processing. Cast=" << cast
						  << " Len=" << msg.msg_length << std::endl;
			}
	
			ProcessApplicationMessage(conn, msg, true);  // skip seq check
		}
	
		return false;
	}
	

	return false;
}

void CmeGT5::onExecRptNew(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime) {
	auto msg = reinterpret_cast<ExecutionReportNewMsg::BlockData*>(buf);

	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

   
	if (_State.DebugAppMsgs)
		cout << "[onExecRptNew WARNING] cbid=dynamic  OrderRequestID=" << msg->OrderRequestID << " CMEID=" << msg->OrderID << " Side=" << msg->Side << " Qty=" << msg->OrderQty << " Price=" << msg->Price.mantissa << " OrdType=" << (int)msg->OrdType << endl;

	KTN::OrderPod* ord = _OrdPodPool.get();
	ord->OrdStatus = KOrderStatus::NEW;
	ord->OrdState = KOrderState::WORKING;
	ord->OrdExch = KOrderExchange::CME;
	ord->OrdType = ConvertOrderType(msg->OrdType); 
	ord->orderReqId = msg->OrderRequestID;
	ord->mktsegid = segid;
	ord->secid = msg->SecurityID;

	ord->exchordid = msg->OrderID;
	ord->OrdSide = (msg->Side == EnumSideReq::EnumSideReq_Buy) ? KOrderSide::BUY : KOrderSide::SELL;
	ord->quantity = msg->OrderQty;
	ord->dispqty = msg->DisplayQty;
	ord->leavesqty = ord->quantity;
	ord->timestamp = msg->TransactTime;
	ord->recvTime = recvTime;
	ord->possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;


	ord->price = PriceConverters::FromMantissa9ToPrice(msg->Price.mantissa);

	if (msg->OrdType == EnumOrderType::EnumOrderType_StopLimit )
		ord->stopprice = PriceConverters::FromMantissa9ToPrice(msg->StopPx.mantissa);

	ord->queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(*ord);

	_OrdPodPool.put(ord);

	_exchid_to_ordreqid[msg->OrderID] = msg->OrderRequestID;

	#if defined(ELIM_TESTING)
	{
		for(int i = 0; i < 5; i++)	
			cout << "**** DEBUG ***** [onExecRptNew DEBUG] TESTING EXEC RPT ELIMINATION" << endl;
		
		auto [testbuf, len] = TestFactory::BuildExecRptElimination(msg, segid);
		onExecRptElimination(testbuf, len, segid);
		delete[] testbuf;
	}
#endif

	

}

void CmeGT5::onExecRptModify(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime)
{
	auto msg = reinterpret_cast<ExecutionReportModifyMsg::BlockData*>(buf);
	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptModify: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	KTN::OrderPod* ord = _OrdPodPool.get();
	ord->OrdStatus = KOrderStatus::MODIFIED;
	ord->OrdState = KOrderState::WORKING;
	ord->OrdExch = KOrderExchange::CME;
	ord->OrdExch = KOrderExchange::CME;
	ord->OrdSide = (msg->Side == EnumSideReq::EnumSideReq_Buy) ? KOrderSide::BUY : KOrderSide::SELL;
	ord->orderReqId = msg->OrderRequestID;
	ord->exchordid = msg->OrderID;
	ord->timestamp = msg->TransactTime;
	ord->recvTime = recvTime;
	ord->secid = msg->SecurityID;

	ord->quantity = msg->OrderQty;
	ord->dispqty = msg->DisplayQty;
	ord->fillqty = msg->CumQty;
	ord->leavesqty = msg->LeavesQty;
	ord->price = PriceConverters::FromMantissa9ToPrice(msg->Price.mantissa);
	ord->possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;


	if (msg->OrdType == EnumOrderType::EnumOrderType_StopLimit )
			ord->stopprice = PriceConverters::FromMantissa9ToPrice(msg->StopPx.mantissa);

	ord->queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(*ord);
	_OrdPodPool.put(ord);


#if  defined(DEBUG_TESTING) || defined(TAS_TEST)
	LogMe("\n\n****IL3 MOD RECEIVED: " + OrderUtils::Print(ord) + "\n", LogLevel::WARNING);
#endif

}

void CmeGT5::onExecRptCancel(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime) {

	auto msg = reinterpret_cast<ExecutionReportCancelMsg::BlockData*>(buf);
	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptCancel: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	
	if (_State.DebugAppMsgs)
		cout << "[onExecRptCancel][INFO] OrderRequestID=" << msg->OrderRequestID << " CMEID=" << msg->OrderID << " Side=" << (int)msg->Side << " Qty=" << msg->OrderQty << " Price=" << msg->Price.mantissa << endl;

	KTN::OrderPod *ord = _OrdPodPool.get();
	ord->OrdStatus = KOrderStatus::CANCELED;
	ord->OrdState = KOrderState::COMPLETE;
	ord->OrdExch = KOrderExchange::CME;
	ord->OrdExch = KOrderExchange::CME;
	ord->OrdSide = (msg->Side == EnumSideReq::EnumSideReq_Buy) ? KOrderSide::BUY : KOrderSide::SELL;
	ord->orderReqId = msg->OrderRequestID;
	ord->mktsegid = segid;
	ord->exchordid = msg->OrderID;
	ord->timestamp = msg->TransactTime;
	ord->recvTime = recvTime;
	ord->leavesqty = 0;
	ord->fillqty = msg->CumQty;
	ord->possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;

	if (msg->OrdType == EnumOrderType::EnumOrderType_StopLimit )
		ord->stopprice = PriceConverters::FromMantissa9ToPrice(msg->StopPx.mantissa);

	ord->secid = msg->SecurityID;
	ord->queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(*ord);

	_OrdPodPool.put(ord);
}

int CmeGT5::onExecRptTradeOutright(byte_ptr buf, int iMsgLen, int segid, int index, uint64_t recvTime)
{
	auto msg = reinterpret_cast<ExecutionReportTradeOutrightMsg::BlockData*>(buf);
	buf += ExecutionReportTradeOutrightMsg::blockLength;
	int msgbytes = ExecutionReportTradeOutrightMsg::blockLength;

	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << __func__ << ": PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;


	bool senthedge = HedgeComplex(msg->OrderRequestID, msg->LastQty, msg->LastPx.mantissa, -1);

	//Note: This is pure convenience. Remove this for quicker reporting.
	//This adds ~ 5-15us, so it's not insignificant
	if (_State.MEASURE){
		ostringstream oss;
		oss << "FILL: " << msg->ClOrdID << " REQID=" << msg->OrderRequestID
				<< " SEC=" << msg->SecurityID << " QTY=" << msg->LastQty << " SIDE=" << (int)msg->Side
				//<< " SEQ=" << msg->SeqNum << "SEGID=" << segid;
				<< " PX=" << msg->LastPx.mantissa;
		LogMe(oss.str(), LogLevel::OK);
	}

	// uint32_t lvs = 0;
	// KOrderStatus::Enum status = KOrderStatus::FILLED;
	// KOrderState::Enum state = KOrderState::COMPLETE;

	// if (msg->OrdStatus == EnumOrdStatusTrd::EnumOrdStatusTrd_PartiallyFilled)
	// {
	// 	lvs = msg->LeavesQty;
	// 	status = KOrderStatus::PARTIALLY_FILLED;
	// 	state = KOrderState::WORKING;
	// }

	//2 groups we want to skip - I leave this here for safety so
	//once we're done with the msg pointer, we can advance the buffer
	int nbrgrps = 2;
	for (int i = 0; i < nbrgrps; i++)
		msgbytes = SkipGroupAndAdvance(buf, msgbytes);


	KTN::OrderPod *ord = _OrdPodPool.get();

	ord->orderReqId = msg->OrderRequestID;
	ord->OrdExch = KOrderExchange::CME;
	ord->OrdExch = KOrderExchange::CME;
	ord->mktsegid = segid;
	ord->exchordid = msg->OrderID;
	ord->timestamp = msg->TransactTime;
	ord->recvTime = recvTime;
	ord->secid = msg->SecurityID;

	ord->lastpx = PriceConverters::FromMantissa9ToPrice(msg->LastPx.mantissa);
	ord->lastqty = msg->LastQty;
	ord->fillqty = msg->CumQty;
	ord->quantity = msg->OrderQty;
	OrderUtils::fastCopy(ord->execid, msg->ExecID, sizeof(ord->execid));
	OrderUtils::fastCopy(ord->ordernum, msg->ClOrdID, sizeof(ord->ordernum));
	ord->OrdFillType = KOrderFillType::OUTRIGHT_FILL;
	ord->OrdSide = (msg->Side == EnumSideReq::EnumSideReq_Buy) ? KOrderSide::BUY : KOrderSide::SELL;
	ord->possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;

	if (senthedge)
		ord->OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;

	if (msg->OrdStatus == EnumOrdStatusTrd::EnumOrdStatusTrd_Filled)
	{
		ord->leavesqty = 0;
		ord->OrdStatus = KOrderStatus::FILLED;
		ord->OrdState = KOrderState::COMPLETE;
	}
	if (msg->OrdStatus == EnumOrdStatusTrd::EnumOrdStatusTrd_PartiallyFilled)
	{
		ord->leavesqty = msg->LeavesQty;
		ord->OrdStatus = KOrderStatus::PARTIALLY_FILLED;
		ord->OrdState = KOrderState::WORKING;
	}

	ord->queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(*ord);
	_OrdPodPool.put(ord);

	return msgbytes;
}

int CmeGT5::onExecRptTradeSpread(byte_ptr buf, int iMsgLen, int segid, int index, uint64_t recvTime)
{
	auto msg = reinterpret_cast<ExecutionReportTradeSpreadMsg::BlockData*>(buf);
	buf += ExecutionReportTradeSpreadMsg::blockLength;
	int msgbytes = ExecutionReportTradeSpreadMsg::blockLength;

	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << __func__ << ": PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	bool senthedge = HedgeComplex(msg->OrderRequestID, msg->LastQty, msg->LastPx.mantissa, -1);

	//Note: This is pure convenience. Remove this for quicker reporting.
	//This adds ~ 5-15us, so it's not insignificant
	if (_State.MEASURE){
		ostringstream oss;
		oss << "SPRDFILL: SECID=" << msg->SecurityID << " QTY=" << msg->LastQty << " SIDE=" << (int)msg->Side
				<< " PX=" << msg->LastPx.mantissa;
		LogMe(oss.str(), LogLevel::OK);
	}

	int nbrgrps = 3;
	for (int i = 0; i < nbrgrps; i++)
		msgbytes = SkipGroupAndAdvance(buf, msgbytes);

	KTN::OrderPod *ord = _OrdPodPool.get();

	ord->orderReqId = msg->OrderRequestID;
	ord->OrdExch = KOrderExchange::CME;
	ord->OrdExch = KOrderExchange::CME;
	ord->mktsegid = segid;
	ord->exchordid = msg->OrderID;
	ord->timestamp = msg->TransactTime;
	ord->recvTime = recvTime;
	ord->secid = msg->SecurityID;

	ord->lastpx = PriceConverters::FromMantissa9ToPrice(msg->LastPx.mantissa);
	ord->lastqty = msg->LastQty;
	ord->fillqty = msg->CumQty;
	ord->quantity = msg->OrderQty;
	OrderUtils::fastCopy(ord->execid, msg->ExecID, sizeof(ord->execid));
	OrderUtils::fastCopy(ord->ordernum, msg->ClOrdID, sizeof(ord->ordernum));
	ord->OrdFillType = KOrderFillType::SPREAD_FILL;
	ord->OrdSide = (msg->Side == EnumSideReq::EnumSideReq_Buy) ? KOrderSide::BUY : KOrderSide::SELL;
	ord->possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;

	if (senthedge)
		ord->OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;

	if (msg->OrdStatus == EnumOrdStatusTrd::EnumOrdStatusTrd_Filled)
	{
		ord->leavesqty = 0;
		ord->OrdStatus = KOrderStatus::FILLED;
		ord->OrdState = KOrderState::COMPLETE;
	}
	if (msg->OrdStatus == EnumOrdStatusTrd::EnumOrdStatusTrd_PartiallyFilled)
	{
		ord->leavesqty = msg->LeavesQty;
		ord->OrdStatus = KOrderStatus::PARTIALLY_FILLED;
		ord->OrdState = KOrderState::WORKING;
	}

	ord->queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(*ord);
	_OrdPodPool.put(ord);

	return msgbytes;
}



int CmeGT5::onExecRptTradeSpreadLeg(byte_ptr buf, int iMsgLen, int segid, int index, uint64_t recvTime)
{
	auto msg = reinterpret_cast<ExecutionReportTradeSpreadLegMsg::BlockData*>(buf);
	buf += ExecutionReportTradeSpreadLegMsg::blockLength;
	int msgbytes = ExecutionReportTradeSpreadLegMsg::blockLength;

	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << __func__ << ": PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	// KOrderSide::Enum side;
	// if(msg->Side == EnumSideReq::EnumSideReq_Buy)
	// 	side = KOrderSide::BUY;
	// else
	//     side = KOrderSide::SELL;

	int nbrgrps = 2;
	for (int i = 0; i < nbrgrps; i++)
		msgbytes = SkipGroupAndAdvance(buf, msgbytes);


	uint64_t reqid = 0;
	if (_exchid_to_ordreqid.find(msg->OrderID) != _exchid_to_ordreqid.end())
		reqid = _exchid_to_ordreqid[msg->OrderID];

	KTN::OrderPod *ord = _OrdPodPool.get();
	ord->OrdExch = KOrderExchange::CME;
	ord->mktsegid = segid;
	ord->exchordid = msg->OrderID;
	ord->timestamp = msg->TransactTime;
	ord->recvTime = recvTime;
	ord->secid = msg->SecurityID;
	ord->possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;

	ord->lastpx = PriceConverters::FromMantissa9ToPrice(msg->LastPx.mantissa);
	ord->lastqty = msg->LastQty;
	ord->fillqty = msg->CumQty;
	
	OrderUtils::fastCopy(ord->execid, msg->ExecID, sizeof(ord->execid));
	ord->OrdFillType = KOrderFillType::SPREAD_LEG_FILL;

	ord->leavesqty = 0;
	ord->OrdStatus = KOrderStatus::FILLED;
	ord->OrdState = KOrderState::COMPLETE;
	ord->OrdExch = KOrderExchange::CME;

	ord->orderReqId = reqid;

	if(msg->Side == EnumSideReq::EnumSideReq_Buy)
		ord->OrdSide = KOrderSide::BUY;
	else
		ord->OrdSide = KOrderSide::SELL;

	ord->queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(*ord);
	_OrdPodPool.put(ord);

    return msgbytes;
}

void CmeGT5::onExecRptReject(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime) {
	auto msg = reinterpret_cast<ExecutionReportRejectMsg::BlockData*>(buf);
	buf += ExecutionReportRejectMsg::blockLength;
	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		KT01_LOG_WARN(__CLASS__, oss.str());
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	KTN::OrderPod ord = {};
	ord.OrdStatus = KOrderStatus::REJECTED;
	ord.OrdState = KOrderState::COMPLETE;
	ord.OrdExch = KOrderExchange::CME;
	ord.OrdSide = KOrderSide::UNKNOWN;
	ord.OrdAction = KOrderAction::ACTION_REJ;
	ord.orderReqId = msg->OrderRequestID;
	ord.mktsegid = segid;
	ord.exchordid = msg->OrderID;
	ord.possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;

	ord.timestamp = msg->TransactTime;
	ord.recvTime = recvTime;
	ord.secid = msg->SecurityID;

	ord.quantity = msg->OrderQty;
	ord.price = PriceConverters::FromMantissa9ToPrice(msg->Price.mantissa);
	ord.leavesqty = 0;
	ord.rejreason = msg->OrdRejReason;

	ostringstream xxx;
	xxx << "Execution Report Reject:" << msg->OrdRejReason << " TEXT=" << msg->Text;
	KT01_LOG_ERROR(__CLASS__, xxx.str());

	Notification n = {};
	n.exch = _State.EXCHNAME;
	n.source = _State.SOURCE;
	n.org = _State.ORG;
	n.level = MsgLevel::ERROR;
	n.imsg = MsgType::ALERT;
	n.text = xxx.str();

	NotifierRest::instance().Notify(n);

	ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(ord);
}

void CmeGT5::onExecRptElimination(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime) {
	auto msg = reinterpret_cast<ExecutionReportEliminationMsg::BlockData*>(buf);
	buf += ExecutionReportEliminationMsg::blockLength;

	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;


	ostringstream oss;
	oss << "SEG " << segid << " EXEC RPT ELIMINATION: " << msg->ClOrdID << " SECID=" << msg->SecurityID
		<<	" ORDERID=" << msg->OrderID << " LVS=" << msg->DisplayQty <<  " SEQNUM=" << msg->SeqNum;
	LogMe(oss.str(),LogLevel::WARN);

	//cout << "[onExecRptElimination DEBUG] " << oss.str() << endl;

	KTN::OrderPod ord = {};
	ord.OrdStatus = KOrderStatus::CANCELED;
	ord.OrdState = KOrderState::COMPLETE;
	ord.OrdExch = KOrderExchange::CME;
	ord.OrdSide = KOrderSide::UNKNOWN;
	ord.orderReqId = msg->OrderRequestID;
	ord.mktsegid = segid;
	ord.exchordid = msg->OrderID;
	ord.possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;

	ord.timestamp = msg->TransactTime;
	ord.recvTime = recvTime;
	ord.secid = msg->SecurityID;
	ord.leavesqty = 0;

	ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(ord);
}

int CmeGT5::onOrderMassActionReport(byte_ptr buf, int iMsgLen, int segid)
{
	auto msg = reinterpret_cast<OrderMassActionReportMsg::BlockData*>(buf);
	buf += OrderMassActionReportMsg::blockLength;

	int msgbytes = OrderMassActionReportMsg::blockLength;


	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << __func__ << ": PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	ostringstream oss;
	oss << "SEG " << segid << " MASS CXL REPORTS " << msg->TotalAffectedOrders << " TOTAL AFFECTED ORDERS | MSGLEN=" << iMsgLen ;
	LogMe(oss.str(),LogLevel::WARN);

	auto noords = reinterpret_cast<IL3::groupSize*>(buf);
	buf += sizeof(IL3::groupSize);
	msgbytes += sizeof(IL3::groupSize);

	//for msgprint/audit...
	std::vector<GrpAffectedOrdersMassActionReport::BlockData> affectedOrders;

	if (_State.DebugAppMsgs)
		cout << "[onOrderMassActionReport]NO AFF ORDS IN GRP=" << (int)noords->numInGroup << " BLOCK LEN=" << noords->blockLength
			<< " BYTES_READ=" << msgbytes << "/" << iMsgLen <<  endl;

	for (size_t i = 0; i < noords->numInGroup; i++ )
	{
		int blocklen = 32;
		auto afford = reinterpret_cast<GrpAffectedOrdersMassActionReport::BlockData*>(buf);
		buf += blocklen; //noords->blockLength;
		msgbytes += blocklen;// noords->blockLength;

		GrpAffectedOrdersMassActionReport::BlockData ords = *afford;
		affectedOrders.push_back(ords);

		if (!(_State.DebugAppMsgs))
		{
			ostringstream oxx;
			oxx << "onOrderMassActionReport " << (int) i << ": " << afford->OrigCIOrdID << " ExchOrdId=" << afford->AffectedOrderID
					<< " CxlQty=" << afford->CxlQuantity;
			LogMe(oxx.str());
		}

		string ordernum = afford->OrigCIOrdID;
		KTN::OrderPod ord = {};

		ord.orderReqId  = 0;
		ord.callbackid = 0;
		
		if (_exchid_to_ordreqid.find(afford->AffectedOrderID) != _exchid_to_ordreqid.end())
		{
			ord.orderReqId = _exchid_to_ordreqid[afford->AffectedOrderID];
		}
		else
		{
			if (_State.DebugAppMsgs)
				cout << "[onOrderMassActionReport] ExchOrdID "
					+ std::to_string(afford->AffectedOrderID) + " not found in local cme_to_callback map!" << endl;
			
			ord.orderReqId = 0;
			ord.callbackid = -1;
		}
	
		ord.OrdStatus = KOrderStatus::CANCELED;
		ord.OrdState = KOrderState::COMPLETE;
		ord.OrdExch = KOrderExchange::CME;
		ord.mktsegid = segid;
		ord.exchordid = afford->AffectedOrderID;
		ord.timestamp = msg->TransactTime;
		ord.possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;

		//secid?
		ord.leavesqty = 0;

		// if (_State.DebugAppMsgs)
		// 	cout << "[onOrderMassActionReport] ordernum=" << ordernum << " cbid=" << (int)ord.callbackid << " ordreqid=" << ord.orderReqId << endl;

		// if (_State.DebugAppMsgs)
		// 	cout << "[onOrderMassActionReport] ordernum=" << ordernum << " cbid=" << (int)ord.callbackid << " ordreqid=" << ord.orderReqId << endl;

		ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		_qOrdsProc.push(ord);
	}

		if (UNLIKELY(_State.logToScreen)){
			cout << MessagePrinter::generateJsonString(*msg, affectedOrders) << "\n---------------" << endl;
		}

	if (_State.DebugAppMsgs)
		cout << "[onOrderMass] returning bytes=" << msgbytes << " msg_len=" << iMsgLen << endl;
	return msgbytes;
}

void CmeGT5::onOrderCancelReject(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime) {
	auto msg = reinterpret_cast<OrderCancelRejectMsg::BlockData*>(buf);
	buf += OrderCancelRejectMsg::blockLength;
	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;


	string reason = msg->Text;

	if (reason.length() == 0)
		reason = "CXL REJECT TEXT NOT PRESENT!";

	ostringstream oss;
	oss << "SEG " << segid << " OrderCancelRejectMsg: " << msg->ClOrdID
			<< " REASON=" << msg->CxlRejReason
			<< ": " << reason << " SEQ=" << msg->SeqNum;
	KT01_LOG_ERROR(__CLASS__, oss.str());


	KTN::OrderPod ord = {};
	OrderUtils::fastCopy(ord.ordernum, msg->ClOrdID, strlen(ord.ordernum));
	OrderUtils::fastCopy(ord.execid, msg->ExecID, strlen(ord.execid));
	ord.orderReqId = msg->OrderRequestID;
	ord.exchordid = msg->OrderID;

	ord.possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;

	ord.timestamp = msg->TransactTime;
	ord.recvTime = recvTime;
	ord.fillqty = 0;
	ord.leavesqty = 0;

	ord.OrdState = KOrderState::COMPLETE;
	ord.OrdExch = KOrderExchange::CME;
	ord.OrdStatus = KOrderStatus::CXL_REJECT;
	ord.OrdAction = KOrderAction::ACTION_CXL_REJ;

	string txt = msg->Text;

	if (txt.length() == 0)
		txt = "ERROR: REJECT TEXT NOT PRESENT!";
	

	Notification n = {};
	n.exch = _State.EXCHNAME;
	n.source = _State.SOURCE;
	n.org = _State.ORG;
	n.level = MsgLevel::ERROR;
	n.imsg = MsgType::ALERT;
	n.text = oss.str();
	NotifierRest::instance().Notify(n);

	ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(ord);
}

void CmeGT5::onOrderCancelReplaceReject(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime) {
	auto msg = reinterpret_cast<OrderCancelReplaceRejectMsg::BlockData*>(buf);
	buf += OrderCancelReplaceRejectMsg::blockLength;
	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	//fyi, a cxl rej will likely NOT have an orderid since you're prob trying to cxl and order not in book...

	string reason = msg->Text;
	if (reason.length() == 0)
	{
		reason = "CXL REPL REJECT TEXT NOT PRESENT!";
	}

	ostringstream oss;
	oss << "SEG " << segid << " OrderCancelReplaceRejectMsg: " << msg->ClOrdID << " REASON=" << msg->CxlRejReason
			<< ": " << reason << " SEQ=" << msg->SeqNum << " ORDREQ=" << msg->OrderRequestID;
	KT01_LOG_ERROR(__CLASS__, oss.str());

	KTN::OrderPod ord = {};

	OrderUtils::fastCopy(ord.ordernum, msg->ClOrdID, strlen(msg->ClOrdID));

	ord.orderReqId = msg->OrderRequestID;
	ord.exchordid = msg->OrderID;
	ord.timestamp = msg->TransactTime;
	ord.recvTime = recvTime;
	ord.fillqty = 0;
	ord.leavesqty = 0;
	ord.possRetransmit = (msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True) ? true : false;

	ord.OrdState = KOrderState::COMPLETE;
	ord.OrdExch = KOrderExchange::CME;
	ord.OrdStatus = KOrderStatus::CXL_REPL_REJECT;
	ord.OrdAction = KOrderAction::ACTION_CXL_REPL_REJ;

	string txt = msg->Text;

	if (txt.length() == 0)
	{
		txt = "ERROR: REJECT TEXT NOT PRESENT!";
	}

	Notification n = {};
	n.exch = _State.EXCHNAME;
	n.source = _State.SOURCE;
	n.org = _State.ORG;
	n.level = MsgLevel::ERROR;
	n.imsg = MsgType::ALERT;
	n.text = oss.str();

	NotifierRest::instance().Notify(n);

	ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(ord);
}


void CmeGT5::onBusinessReject(ConnState& conn, byte_ptr buf, int iMsgLen, int segid)
{
	auto msg = reinterpret_cast<BusinessRejectMsg::BlockData*>(buf);
	buf += BusinessRejectMsg::blockLength;
	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
		//cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	string reason = ErrorCodes::getBusRejectErrorMessage(msg->BusinessRejectReason);

	ostringstream oss;
	oss << "SEG " << segid << " BusinessRejectMsg: " << msg->Text
			<< " ErrText=" << reason
			<< " RefMsgType=" << msg->RefMsgType
			<< " RefSeqNum=" << msg->RefSeqNum
			<< " RefTagID=" << msg->RefTagID;

	KT01_LOG_ERROR(__CLASS__, oss.str());

	ConnState::OrderSeqRecord entry = conn.GetOrderRecord(msg->RefSeqNum);
	if (LIKELY(entry.second == msg->RefSeqNum))
	{
		KTN::OrderPod ord = entry.first;

		switch (ord.OrdAction)
		{
			case KOrderAction::ACTION_NEW:
				ord.OrdAction = KOrderAction::ACTION_REJ;
				ord.OrdState = KOrderState::COMPLETE;
				ord.OrdStatus = KOrderStatus::REJECTED;
				ord.leavesqty = 0;
				break;
			case KOrderAction::ACTION_CXL:
				ord.OrdAction = KOrderAction::ACTION_CXL_REJ;
				ord.OrdStatus = KOrderStatus::CXL_REJECT;
				break;
			case KOrderAction::ACTION_MOD:
				ord.OrdAction = KOrderAction::ACTION_CXL_REPL_REJ;
				ord.OrdStatus = KOrderStatus::CXL_REPL_REJECT;
				break;
			default:
				LOGERROR("[CmeGT5] onBusinessReject: Unexpected order action in seq num {}. OrdAction={}", static_cast<uint32_t>(msg->RefSeqNum), static_cast<int>(ord.OrdAction));
				ord.OrdAction = KOrderAction::UNKNOWN;
				break;
		}

		ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		_qOrdsProc.push(ord);
	}
	else
	{
		LOGERROR("[CmeGT5] onBusinessReject: Failed to find order for seq num {}", static_cast<uint32_t>(msg->RefSeqNum));
	}

	Notification n = {};
	n.exch = _State.EXCHNAME;
	n.source = _State.SOURCE;
	n.org = _State.ORG;
	n.level = MsgLevel::ERROR;
	n.imsg = MsgType::ALERT;
	n.text = oss.str();

	NotifierRest::instance().Notify(n);
}


int CmeGT5::onPartyDetailsDefinitionRequestAck(ConnState& conn, byte_ptr buf, int iMsgLen, int segid)
{
	auto msg = reinterpret_cast<PartyDetailsDefinitionRequestAckMsg::BlockData*>(buf);

	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	buf += PartyDetailsDefinitionRequestAckMsg::blockLength;
	int msgSize = PartyDetailsDefinitionRequestAckMsg::blockLength;

	const IL3::groupSize *partyDetailsGrpSize = reinterpret_cast<const IL3::groupSize *>(buf);
	buf += sizeof(IL3::groupSize);
	msgSize += sizeof(IL3::groupSize);
	buf += partyDetailsGrpSize->blockLength * partyDetailsGrpSize->numInGroup;
	msgSize += partyDetailsGrpSize->blockLength * partyDetailsGrpSize->numInGroup;

	const IL3::groupSize *trdRegPublicationGroups = reinterpret_cast<const IL3::groupSize *>(buf);
	msgSize += sizeof(IL3::groupSize);
	msgSize += trdRegPublicationGroups->blockLength * trdRegPublicationGroups->numInGroup;

	ostringstream oss;
	oss << "GOT onPartyDetailsDefinitionRequestAck FOR PARTY ID=" << msg->PartyDetailsListReqID
			<< " STATUS=" << (int)msg->PartyDetailRequestStatus;
	LogMe(oss.str(),LogLevel::INFO);

	return msgSize;
}

int CmeGT5::onPartyDetailsListReport(ConnState& conn, byte_ptr buf, int iMsgLen, int segid) {
	auto msg = reinterpret_cast<PartyDetailsListReportMsg::BlockData*>(buf);
	buf += PartyDetailsListReportMsg::blockLength;
	int msgbytes = PartyDetailsListReportMsg::blockLength;

	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;


	std::string output = MessagePrinter::generateJsonString(*msg);
	cout << output << endl;

	auto noPartyDets = reinterpret_cast<IL3::groupSize*>(buf);
	buf += sizeof(IL3::groupSize);
	msgbytes += sizeof(IL3::groupSize);

	cout << "PARTY DETAILS [" << (int) noPartyDets->numInGroup << "]:"<< endl;
	for (int i = 0; i < noPartyDets->numInGroup; i++)
	{
		auto details = reinterpret_cast<GrpPartyDetailsListReport::BlockData*>(buf);

		buf += sizeof(GrpPartyDetailsListReport::BlockData);// noPartyDets->blockLength;
		msgbytes += sizeof(GrpPartyDetailsListReport::BlockData);//noPartyDets->blockLength;

		ostringstream oss;
		oss << i <<"] ID=" << details->PartyDetailID
				<< " ROLE=" << (int)details->PartyDetailRole;
		cout << oss.str() << endl;
	}

	//NoTrdRegPublications: always 1
	int nbrgrps = 1;
	for (int i = 0; i < nbrgrps; i++)
		msgbytes = SkipGroupAndAdvance(buf, msgbytes);

	return msgbytes;
}

void CmeGT5::onExecRptStatus(ConnState& conn, byte_ptr buf, int iMsgLen, int segid) {
	auto msg = reinterpret_cast<ExecutionReportStatusMsg::BlockData*>(buf);
	buf += ExecutionReportStatusMsg::blockLength;

	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "[onExecRptStatus]: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen || _State.DebugAppMsgs))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	if (msg->OrderID == 0)
	{
		ostringstream oss;
		oss.clear();
		oss.str("");
		oss << "SEG " << segid << " RETURNED NO ORDERS FOR ORDER STATUS REQUEST";
		LogMe(oss.str(),LogLevel::INFO);
		return;
	}

	string status = IL3Helpers::toString(msg->OrdStatus);

	//add this to ensure that orders are mapped to their reqid- especially for mass cancels!
	_exchid_to_ordreqid[msg->OrderID] = msg->OrderRequestID;

	KTN::OrderPod ord = {};
	//for execrpt status we need to set possretrans to false to prevent double counting of fills
	ord.possRetransmit = false;
	ord.OrdStatus = IL3Helpers::translate(msg->OrdStatus);
	ord.OrdState = KOrderState::STATUS_UPDATE;
	ord.OrdExch = KOrderExchange::CME;
	ord.orderReqId = msg->OrderRequestID;
	ord.mktsegid = segid;
	ord.secid = msg->SecurityID;
	ord.timestamp = msg->TransactTime;

	ord.fillqty = msg->CumQty;

	OrderUtils::fastCopy(ord.ordernum, msg->ClOrdID, sizeof(ord.ordernum));

	ord.exchordid = msg->OrderID;
	ord.OrdSide = (msg->Side == EnumSideReq::EnumSideReq_Buy) ? KOrderSide::BUY : KOrderSide::SELL;
	
	ord.quantity = msg->OrderQty;
	ord.dispqty = msg->DisplayQty;
	ord.leavesqty = msg->LeavesQty;
	ord.minqty = msg->MinQty;

	ord.timestamp = msg->TransactTime;
	ord.secid = msg->SecurityID;

	ord.isManual = (msg->ManualOrderIndicator == EnumManualOrdIndReq::EnumManualOrdIndReq_Manual) ? true : false;	
	
	ord.price = PriceConverters::FromMantissa9ToPrice(msg->Price.mantissa);
	ord.stopprice = PriceConverters::FromMantissa9ToPrice(msg->StopPx.mantissa);

	if (msg->OrdType == EnumOrderType::EnumOrderType_StopLimit )
		ord.stopprice = PriceConverters::FromMantissa9ToPrice(msg->StopPx.mantissa);

	if (_State.DebugAppMsgs)
	{
		cout << "[onExecRptStatus DEBUG] ORDER:" << OrderUtils::Print(ord) << endl;
	}

	ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(ord);
}

void CmeGT5::onExecutionAck(ConnState& conn, byte_ptr buf, int iMsgLen, int segid) {
	ostringstream oss;
	oss << segid << ": onExecutionAck: GOT " << iMsgLen << " BYTES";
	LogMe(oss.str(), LogLevel::WARN);
	if (UNLIKELY(_State.logToScreen))
	{
		auto msg = reinterpret_cast<ExecutionAckMsg::BlockData*>(buf);
		buf += ExecutionAckMsg::blockLength;

		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;
	}
}

void CmeGT5::onTradeAddendumOutright(ConnState& conn, byte_ptr buf, int iMsgLen, int segid) {
	auto msg = reinterpret_cast<ExecutionReportTradeAddendumOutrightMsg::BlockData*>(buf);
	buf += ExecutionReportTradeAddendumOutrightMsg::blockLength;

	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

    cout << "SEG " << segid << " onExecRptTradeAddendumOutright: " << msg->ClOrdID
			<< " SYMBOL=" << msg->SecurityID << " SEQNUM=" << msg->SeqNum << endl;

	std::string sz = IL3::MessagePrinter::generateJsonString(*msg);
	LogMe(sz, LogLevel::INFO);

}

void CmeGT5::onTradeAddendumSpread(ConnState& conn, byte_ptr buf, int iMsgLen, int segid) {
	auto msg = reinterpret_cast<ExecutionReportTradeAddendumSpreadMsg::BlockData*>(buf);
		buf += ExecutionReportTradeAddendumSpreadMsg::blockLength;

		//int msgbytes = ExecutionReportTradeAddendumSpreadMsg::blockLength;

		if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
		{
			ostringstream oss;
			oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
			LogMe(oss.str(), LogLevel::INFO);
		}

		if (UNLIKELY(_State.logToScreen))
			cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;



		std::string sz = IL3::MessagePrinter::generateJsonString(*msg);
		LogMe(sz, LogLevel::INFO);
}

void CmeGT5::onTradeAddendumSpreadLeg(ConnState& conn, byte_ptr buf, int iMsgLen,
		int segid)
{
	auto msg = reinterpret_cast<ExecutionReportTradeAddendumSpreadLegMsg::BlockData*>(buf);
	buf += ExecutionReportTradeAddendumSpreadLegMsg::blockLength;

	//int msgbytes = ExecutionReportTradeAddendumSpreadLegMsg::blockLength;

	if (UNLIKELY(msg->PossRetransFlag == EnumBooleanFlag::EnumBooleanFlag_True))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum <<". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str(), LogLevel::INFO);
	}

	if (UNLIKELY(_State.logToScreen))
		cout << MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;



	std::string sz = IL3::MessagePrinter::generateJsonString(*msg);
	LogMe(sz, LogLevel::INFO);
}


void CmeGT5::Display()
{

	if (_State.MEASURE) Banner("MEASURING LATENCY!!!!");
}

void CmeGT5::Banner(string msg)
{
	for (int i = 0; i < 3; i++)
		LogMe("*********************************************", LogLevel::WARN);
	//for (int i = 0; i < 5; i++)
		LogMe("*******       " + msg + "      *********", LogLevel::INFO);
	for (int i = 0; i < 3; i++)
		LogMe("*********************************************", LogLevel::WARN);
}
