//============================================================================
// Name        	: CfeGT4.cpp
// Author      	: Samuel Gaer
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Nov 5, 2021 6:24:37 PM
//============================================================================

#include <exchsupport/cfe/CfeGT4.hpp>
#include "CfeGT4.hpp"
#include <KT01/SecDefs/CfeSecMaster.hpp>
#include <exchsupport/cfe/session/CfeResetSession.hpp>
using namespace KTN::CFE;
using namespace KTN::CFE::Boe3;

#include <exchsupport/cme/il3/IL3Messages2.hpp>
#include <exchsupport/cme/il3/IL3MetaData.hpp>

#include <Orders/OrderRecoveryService.hpp>

/**
 *
 * @param qords
 * @param settingsfile
 * @param source
 */
CfeGT4::CfeGT4(tbb::concurrent_queue<KTN::OrderPod> &qords, const std::string &settingsfile, const std::string &source)
	: ExchangeBase2(settingsfile, source, qords, "CfeGT4")
{
	if (!KT01::SECDEF::CFE::CfeSecMaster::instance().IsLoaded())
		LogMe("LOADING CFE SECMASTER...");

	_sett.Load(settingsfile);
	_sessfact.Init(settingsfile);
	_fastfact.Init(settingsfile);

	_State.ORG = _sett.Org;
	_State.SOURCE = source;
	_State.EXCHNAME = _sett.ExchName;
	_logsvc->ChangeName(_sett.ExchName, _sett.ExchName);

	LogMe("LOG SVC CHANGED NAME TO " + _sett.ExchName + " SRC=" + source);

	_State.HEARTBEAT_SECS = _sett.SessionSettings.HeartbeatSecs;
	
	_State.EVENTPOLLMAX = _sett.PollerMax;
	
	_State.USE_PRIMARY = _sett.SessionSettings.UsePrimary;
	_State.MEASURE = _sett.MeasureLatency;

	Settings mainsett = Settings("Settings.txt");
	_State.TCP_VERSION = mainsett.getInteger("TcpVersion");
	if (_State.TCP_VERSION == 0)
	{
		KT01_LOG_ERROR(__CLASS__, "TCP VERSION NOT SET, DEFAULTING TO 1");
		_State.TCP_VERSION = 1;
	}
	_State.ALGO_AFFINITY = mainsett.getInteger("AlgoAffinity");
	_State.logToScreen = _sett.LogMessagesToScreen;

	_State.USE_AUDIT = _sett.UseAudit;
	if (_State.USE_AUDIT)
	{
		int auditcore = _sett.AuditCore;
		string dir = _sett.AuditDir;
		StartAuditTrail(dir, _sett.SessionSettings.SessionId, _sett.SessionSettings.SubId, auditcore);
	}

	_State.STATEFILE_DIR = _sett.StateFileDir;
	string szRetrNewUuidEnable = (_State.RetransOnNewUUID) ? "ENABLED" : "DISABLED";
	
	_State.DebugRecvBytes = _sett.DebugRecvBytes;
	_State.DebugSession = _sett.DebugSession;
	_State.DebugAppMsgs = _sett.DebugAppMsgs;
	_State.DebugSessMsgs = _sett.DebusSessMsgs;
	_State.DebugGapDetection = _sett.DebugGapDetection;


	Display();

	usleep(10000);

	_procthread = std::thread(&CfeGT4::ProcessingThread, this);

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

CfeGT4::~CfeGT4()
{
	_State.ACTIVE.store(false, std::memory_order_relaxed);
	_PROC_THREAD_ACTIVE.store(false, std::memory_order_relaxed);
}

void CfeGT4::StartAuditTrail(std::string dir, std::string sessid, std::string firmid, int core)
{
	LogMe("OK Initializeing audit trail writer. Dir=" + dir);
	auto processor = std::make_unique<CfeAuditProcessor>();
	_audit = std::make_unique<AuditTrailWriter>(std::move(processor), _KtnBufs);

	_audit->Start(dir, sessid, firmid, core);
	int segid = 1;
	_audit->InitFileForSeg(segid);
}


void CfeGT4::Stop()
{
	LogMe("GOT STOP REQUEST");

	Instruction cmd = {};
	cmd.command = ExchCmd::TERMINATE;

	_qCmds.push(cmd);
	_State.EVENT_WAITING.store(true, std::memory_order_relaxed);
}

void CfeGT4::Start()
{
	tbb::concurrent_unordered_map<uint64_t, KTN::Order> &apiorders = OrderRecoveryService::instance().GetApiOrders();

	for (tbb::concurrent_unordered_map<uint64_t, KTN::Order>::const_iterator it = apiorders.begin(); it != apiorders.end(); ++it)
	{
		const KTN::Order &ord = it->second;
		clordIdToReqIdMap[ord.ordernum] = { ord.orderReqId, ord.exchordid, static_cast<uint64_t>(ord.secid)};
		LOGINFO("[CfeGT4] Start: Mapping clOrdId {} to reqId {}", ord.ordernum, ord.orderReqId);
	}

	_State.CHECK_EVENTS_TRIG = _State.EVENTPOLLMAX * 2;
	_State.CHECK_EVENT_LOOP_COUNT = 0;
	_State.LOOPER = 0;
	_State.LOOP_TRIG = _State.EVENTPOLLMAX;
	_State.ACTIVE.store(true, std::memory_order_relaxed);

	cout << "[POLLERS DEBUG] EventPollerMax=" << _State.EVENTPOLLMAX
		 << " CHECK_EVENTS_TRIG=" << _State.CHECK_EVENTS_TRIG
		 << " LOOP_TRIG=" << _State.LOOP_TRIG
		 << endl;

	StartSessionManager(_CX);
	StartTcpConnection(_CX, _State.USE_PRIMARY);
}

void CfeGT4::StartSessionManager(ConnState &conn)
{
	conn.sessmgr.CurrentState = EnumSessionState::Disconnected;
	conn.segid = 1;
	conn.index = 0;

	conn.sessmgr.CurrentState = EnumSessionState::StartingLoginProcess;
	conn.sessmgr.Init(_sett.SessionSettings.SessionId, _sett.SessionSettings.EFID, 1, _State.HEARTBEAT_SECS, 100,
					 _State.STATEFILE_DIR, _State.DebugGapDetection, _State.DebugSession,
					 _State.EXCHNAME, _State.SOURCE, _State.ORG);

	//cfe special case b/c there's no "negotiated" state like cme
	
}

void CfeGT4::StartTcpConnection(ConnState &conn, bool primary)
{
	//Instantiate the TCP handler
	LogMe("Tcp Factory Instanatiation: TYPE=" + std::to_string(_State.TCP_VERSION));
	conn.tcpHandler = KT01::NET::TCP::TcpHandlerFactory::Create("Settings.txt", this);
	
	string whichconn = (primary) ? "PRIMARY" : "SECONDARY";
	LogMe("STARTING " + whichconn + " TCP CONNECTION");

	if (conn.sessmgr.CurrentState != EnumSessionState::StartingLoginProcess)
	{
		ostringstream oss;
		oss << "CANNOT CREATE TCP CONNECTION. STATE=" << EnumSessionState::toString(conn.sessmgr.CurrentState);
		KT01_LOG_ERROR(__CLASS__, oss.str());
		conn.sessmgr.CurrentState = EnumSessionState::Disconnected;
		conn.sessmgr.Stop();
		return;
	}

	//bool res = false;
	int port = _sett.SessionSettings.Port;

	try
	{
		ServiceDescriptor svc;
		svc.address = (primary) ? _sett.SessionSettings.Host : _sett.SessionSettings.SecondaryHost;
		svc.port = port;
		svc.iSource = 0;
		svc.index = conn.index;
		svc.segid = conn.segid;

		int res = conn.tcpHandler->Init(svc, _sett.UseFeed_A);

		if (res) {
			// Enable auto-reconnect: 15 seconds (initial wait time, will use exponential backoff)
			conn.tcpHandler->SetReconnectConfig(true, 15000);
			LogMe("TCP auto-reconnect enabled (initial wait: 15s, exponential backoff)");
		} else {
			KT01_LOG_ERROR(__CLASS__, "TCP Init failed");
		}
	}
	catch (const std::exception &ex)
	{
		ostringstream oss;
		oss << ex.what();
		KT01_LOG_ERROR(__CLASS__, "Cannot connect to the primary host: " + oss.str());
	}
}

void CfeGT4::ProcessingThread()
{
	_PROC_THREAD_ACTIVE.store(true, std::memory_order_relaxed);

	while (_PROC_THREAD_ACTIVE.load(std::memory_order_relaxed) == true)
	{
		//cout << "[POLLERS DEBUG] ProcessingThread: " << _State.CHECK_EVENT_LOOP_COUNT << " vs " << _State.CHECK_EVENTS_TRIG << endl;
		if (_State.CHECK_EVENT_LOOP_COUNT >= _State.CHECK_EVENTS_TRIG)
		//if (_State.EVENT_WAITING.load(std::memory_order_relaxed) == true)
		{
			CheckEvents(_CX);
			_State.CHECK_EVENT_LOOP_COUNT = 0;
		}

		if (!_qHdgProc.empty())
		{
			std::vector<std::unique_ptr<KTNBuf>> vbufs;
			KTNBuf buf;
			while (_qHdgProc.try_pop(buf))
			{
				auto bufPtr = std::make_unique<KTNBuf>(buf);
				vbufs.push_back(std::move(bufPtr)); // Move unique_ptr into vector
			}

			ProcessHdgBuf(vbufs);
		}

		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
}

void CfeGT4::ClearMaps()
{
	LogMe(_State.EXCHNAME + "CLEARING MAPS!");
	_msgratio.clear();
	_msgevents.clear();
	_hedgeOrds.clear();
	_hedgeMap.clear();
}

void CfeGT4::SendHeartbeatMsg(ConnState &conn)
{
	int segid = conn.segid;
	if (UNLIKELY(!ConnState::OkToSend(conn.sessmgr.CurrentState)))
	{
		ostringstream oss;
		oss << "CANNOT SEND SEQUENCE MSG FOR SEG " << segid
			<< ". CONNECTION STATE=" << EnumSessionState::toString(conn.sessmgr.CurrentState);
		KT01_LOG_ERROR(__CLASS__, oss.str());
		setLastSentTime(conn);
		return;
	}

	if (_State.DebugSession)
		cout << "[SendSeq] Sending Heartbeat: STATE=" << EnumSessionState::toString(conn.sessmgr.CurrentState) << endl;

	//uint64_t seq = conn.sessmgr.SeqOut();
	KTNBuf msg = _sessfact.HeartbeatEncode();

	conn.tcpHandler->Send(msg.buffer, msg.buffer_length, false);

	setLastSentTime(conn);

	//conn.sessmgr.Update();
}

void CfeGT4::SendLogoutMsg(ConnState &conn)
{
	if (UNLIKELY(!ConnState::OkToSend(conn.sessmgr.CurrentState)))
	{
		ostringstream oss;
		oss << "CANNOT SEND LOGOUT. CONNECTION STATE=" << EnumSessionState::toString(conn.sessmgr.CurrentState);
		KT01_LOG_ERROR(__CLASS__, oss.str());

		return;
	}

	conn.sessmgr.Stop();
	conn.sessmgr.CurrentState = EnumSessionState::Terminating;

	KT01_LOG_INFO(__CLASS__, "Sending Logout Message");

	KTNBuf msg = _sessfact.LogoutMsgEncode();

	conn.tcpHandler->Send(msg.buffer, msg.buffer_length, false);
}

uint16_t CfeGT4::onRecv(byte_ptr buf, int len, int segid, int index)
{
	const uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	int offset = 0;

	//cfe plug- segid = 1 to match matching unit.
	segid = 1;

	if (_State.DebugRecvBytes)
	{
		cout <<"[WARN]-----------BEGINNING-----------------" << endl;
		cout << "[onRecv DEBUG] SEGID=" << segid
			 << " RECV LEN=" << len
			 << " OFFSET=" << offset
			 << endl;
	}
	// Process each complete SOFH message in the buffer
	while (offset < len)
	{
		// Make sure we have at least a complete SOFH header.
		if (len - offset < MsgSizes::HEADER)
			break; // Not enough data

		// Get the SOFH header. Assume sofh->msgSize_ includes the header.
		auto sofh = reinterpret_cast<KTN::CFE::Boe3::MessageHeaderMsg *>(buf + offset);
		if (sofh->MessageLength + 2 > (len - offset))
			break; // Incomplete SOFH message; wait for more data

		// Calculate end of the current message.
		const int messageEnd = offset + sofh->MessageLength + 2;

		// Log overall SOFH info.
		if (_State.DebugRecvBytes)
			cout << "[onRecv DEBUG] *** MSG SIZE=" << sofh->MessageLength + 2 << " TOTAL_LEN=" << len << endl;

		// Move past the SOFH header- we don't need it here since we include headers in all msgs
		//offset += MsgSizes::HEADER;

		uint16_t templateId = static_cast<uint16_t>(sofh->MessageType);
		uint16_t msgBytes = sofh->MessageLength + 2; // initial message bytes to advance

		// Calculate remaining bytes in this message.
		const int sbeRemaining = messageEnd - offset;

		if (_State.DebugRecvBytes)
		{
			cout << "[onRecv DEBUG] ***GOT TEMPLATEID=" << templateId
				 << " MSGSIZE=" << sofh->MessageLength + 2
				 << " REMAIN=" << sbeRemaining
				 << " BYTESREAD=" << ((sofh->MessageLength + 2) - sbeRemaining)
				 << " TOTAL SIZE=" << len
				 << " OFFSET=" << offset
				 << endl;
		}

		// Prepare the application buffer.
		KTNBuf8t appbuf = {};
		appbuf.buffer = buf + offset;
		// (Copying the remaining bytes—adjust the length if you only need the message body)
		memcpy(appbuf.buf, appbuf.buffer, sbeRemaining);
		appbuf.msgtype = templateId;
		appbuf.buffer_length = sbeRemaining;
		appbuf.segid = segid;
		appbuf.index = index;
		appbuf.block_length = sofh->MessageLength + 2;
		appbuf.msg_length = sbeRemaining;
		appbuf.seqnum = sofh->SeqNum;
		appbuf.recvTime = timestamp;

		bool save = false;
		if (UNLIKELY(appbuf.msgtype <= MsgTypeRange::SESSION_MSG_MAX))
		{
			msgBytes = appbuf.block_length;
			ProcessSessionMessage(_CX, appbuf);
		}
		else
		{
			// check seqnum here?

			msgBytes = ProcessApplicationMessage(_CX, appbuf);
			save = true;
		}

		if (_State.USE_AUDIT && save)
		{
			if (_State.MEASURE)
				_trk.startMeasurement("auditIn");

			// _audit->Write(std::move(appbuf));

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

	if (_State.DebugRecvBytes)
	{
		cout <<"[WARN]-----------END: " << offset << "-----------------" << endl;
	}

	// Return the total number of bytes processed.
	return static_cast<uint16_t>(offset);
}

bool CfeGT4::checkSessionIsConnected(ConnState &conn, int segid)
{
	if (!conn.sessmgr.Connected())
	{
		return false;
	}

	return true;
}

/**
 * Poll the Tcp connection for events
 * Note: We've offloaded some event checking etc to the ProcessingThread function
 */
void CfeGT4::Poll()
{
	bool checkhb = false;
	if (_State.LOOPER > _State.LOOP_TRIG)
	{
		checkhb = true;
		_State.LOOPER = 0;
	}

	Dispatch(_CX, checkhb);

	_State.CHECK_EVENT_LOOP_COUNT++;
	_State.LOOPER++;
}

void CfeGT4::SessionInit(ConnState &conn, int index)
{
	conn.sessmgr.CurrentState = EnumSessionState::Disconnected;
	conn.segid = 1;
	conn.index = index;
	// conn.tcpHandler->Subscribe(this);
}

void CfeGT4::SessionLogin(ConnState &conn)
{
	ostringstream oseg;
	oseg << "SEGID=" << conn.segid;

	if (_State.DebugSession)
	{
		cout << "[Login] Sending Login. SeqOut=" << conn.sessmgr.SeqOut()
			<< " SeqIn=" << conn.sessmgr.SeqIn()
			 << " SEQID=" << conn.segid 
			 << endl;
	}

	onStateChange(EnumSessionState::AwaitEstablishmentAck, conn);
	conn.sessmgr.SetReplayComplete(false);

	//NOTE: on the login, they are expecting us to send the seqin (Unit Sequence). Ok. 
	KTNBuf msg = _sessfact.LoginMsgEncode(conn.sessmgr.SeqIn(), 1, _State.DebugSessMsgs);
	conn.tcpHandler->Send(msg.buffer, msg.buffer_length, false);

	setLastSentTime(conn);
}

void CfeGT4::Dispatch(ConnState &conn, bool checkHb)
{
	// Always call Poll() first to allow reconnect logic to work
	if (conn.tcpHandler != nullptr) {
		conn.tcpHandler->Poll();
	}

	// Check if sequence reset is needed
	if (conn.needs_reset_reconnect) {
		ostringstream oss;
		oss << "Performing session reset (attempt #" << (conn.reset_retry_count + 1)
		    << "). Backoff: " << conn.reset_backoff_ms << "ms";
		KT01_LOG_WARN(__CLASS__, oss.str());

		// Adding this as I think there's a potential race condition here.
		// During the maintenance window, we reconnect to CFE, which queues an update of the state file.
		// We then send a login and get rejected due to our sequence number being too high, which sets the needs_reset_reconnect flag.
		// We disconnect and immediately call reset_session.
		// However, there may still be a queued state file update from the previous reconnect attempt, which may overwrite this.
		// The sequence of events is significantly faster in production (around 600us from the reconnect to the session reset),
		// making this a lot more likely. Production instance kept trying to log in with the old sequence number on 02/10/2026,
		// until I manually edited the sequence numbers in the file.
		conn.sessmgr.ResetSessionSeqNumsToZero();

		// Get the state filename from SessionStateManager
		string state_filename = conn.sessmgr.GetFilename();

		// Call reset_session to reset the .STATE file
		if (reset_session("False", "config/cfe/cfeBoe3.conf", state_filename, false)) {
			LogMe("Session reset successful. Reconnecting...");

			// Apply exponential backoff before reconnecting
			usleep(conn.reset_backoff_ms * 1000);

			// Stop current connection
			conn.tcpHandler->Stop();

			// Wait a moment
			usleep(1000000); // 1 second

			// Reconnect
			conn.is_reconnecting = true;
			StartSessionManager(conn);
			StartTcpConnection(conn, _State.USE_PRIMARY);
			conn.is_reconnecting = false;

			// Clear the flag and update backoff
			conn.needs_reset_reconnect = false;
			conn.reset_retry_count++;

			// Exponential backoff: double the time, cap at 5 minutes (300000ms)
			conn.reset_backoff_ms = std::min(conn.reset_backoff_ms * 2, 300000);

			LogMe("Reconnection attempt completed. Will validate sequences on login.");
		} else {
			KT01_LOG_ERROR(__CLASS__, "Session reset failed. Will retry.");
			// Keep the flag set so we retry next time through Dispatch()
		}

		return;
	}

	// Skip state checks if reconnecting
	if (conn.is_reconnecting) {
		return;
	}

	// Check if reconnect just happened (state=Disconnected but FD > 0)
	// Use static variables to track state transitions and prevent loop
	static EnumSessionState::Enum last_state = EnumSessionState::Disconnected;
	static int last_fd = -1;

	int current_fd = conn.tcpHandler->Fd();
	bool reconnect_just_happened = (current_fd > 0 && last_fd <= 0 &&
	                                conn.sessmgr.CurrentState == EnumSessionState::Disconnected);

	if (reconnect_just_happened) {
		conn.reconnect_attempt++;

		// Send reconnect attempt notification
		// Send on attempts: 1, 5, 10, 20, 40, 80, 120, 160, then every 40
		bool should_notify = (conn.reconnect_attempt == 1) ||
		                     (conn.reconnect_attempt == 5) ||
		                     (conn.reconnect_attempt == 10) ||
		                     (conn.reconnect_attempt == 20) ||
		                     (conn.reconnect_attempt >= 40 && (conn.reconnect_attempt % 40 == 0));

		if (should_notify) {
			ostringstream notify_oss;
			notify_oss << "SEG " << conn.segid << ": Reconnect attempt #" << conn.reconnect_attempt;
			KTN::notify::NotifierRest::NotifyInfo(_State.EXCHNAME, _State.SOURCE, _State.ORG, notify_oss.str());
		}

		ostringstream reconnect_log;
		reconnect_log << "Reconnect detected (FD changed from " << last_fd << " to " << current_fd
		              << ", attempt #" << conn.reconnect_attempt
		              << "). Starting login process for SEG " << std::to_string(conn.segid);
		LogMe(reconnect_log.str());

		StartSessionManager(conn);  // Change state to StartingLoginProcess
		// Don't return - continue to login section below
	} else if (conn.sessmgr.CurrentState == EnumSessionState::Disconnected && current_fd <= 0) {
		// No FD and disconnected - wait for reconnect
		last_state = conn.sessmgr.CurrentState;
		last_fd = current_fd;
		return;
	}

	// Update tracking variables
	last_state = conn.sessmgr.CurrentState;
	last_fd = current_fd;

	if (conn.tcpHandler->Fd() <= 0)
	{
		KT01_LOG_ERROR(__CLASS__, "CANNOT SEND MESSAGE. TCP HANDLER FD <= 0");
		onStateChange(EnumSessionState::Disconnected, conn);
		return;
	}

	if (UNLIKELY(conn.sessmgr.CurrentState != EnumSessionState::Established))
	{
		if (conn.sessmgr.CurrentState == EnumSessionState::StartingLoginProcess)
		{
			EFVITcpV5 *efviHandler = dynamic_cast<EFVITcpV5 *>(conn.tcpHandler);
			if (efviHandler == nullptr || efviHandler->isConnectionEstablished())
			{
				LogMe("SENDING LOGIN FOR SEG " + std::to_string(conn.segid) + " FD=" + std::to_string(conn.tcpHandler->Fd()));
				SessionLogin(conn);
			}
		}
	}

	if (checkHb && ConnState::OkToSend(conn.sessmgr.CurrentState))
		CheckForHeartbeat(conn);
}

/***
 * Note: I'm moving the hb here, as we don't need it in the session manager.
 * We can then re-do the sess mgr to be mo better and mo simpler
 */

void CfeGT4::setLastSentTime(ConnState &conn)
{
	conn.sessmgr.HbSent();
}

bool CfeGT4::CheckForHeartbeat(ConnState &conn)
{
	if (conn.sessmgr.HbTriggered())
	{
		SendHeartbeatMsg(conn);
		return true;
	}

	return false;
}

void CfeGT4::CheckEvents(ConnState &conn)
{
	if (_State.EVENT_WAITING.load(std::memory_order_relaxed) == false)
		return;


	while (!_qCmds.empty())
	{
		Instruction cmd = {};
		_qCmds.try_pop(cmd);

		switch (cmd.command)
		{

		case ExchCmd::MENU_CHOICE:
		{
			if (cmd.cmd_value > 1000)
			{
				int newseq = cmd.cmd_value - 1000;
				LogMe("DANGER! GOT COMMAND > 1000 - SEQUENCE CHANGE FOR TESTING ONLY: SEQ=" + std::to_string(newseq));
				conn.sessmgr.SetSeqIn(newseq);
			}
		}
		break;

		case ExchCmd::ALGO_PARAMS:
		case ExchCmd::ALGO_ORDER:
			AlgoJson(cmd.cmd_json, cmd.command);
			break;

		case ExchCmd::SESS_STATUS:
			PrintSessionMap(conn);
			break;

		case ExchCmd::CXL_ALL_SEGS:
		{
			KT01_LOG_WARN(__CLASS__, "GOT CXL ALL SEGS COMMAND!!!!");
			SendMassCxl(conn,  KOrderAction::ACTION_CXL_ALL,"VX","FBN");
		}
		break;

		case ExchCmd::RISK_RESET:
		{
			KT01_LOG_WARN(__CLASS__, "GOT RISK RESET COMMAND!!!!");
			SendRiskReset(conn);
		}
		break;

		case ExchCmd::CXL_ALL_INSTRUCTIONS:
		{
			KT01_LOG_WARN(__CLASS__, "GOT CXL ALL WITH INSTRUCTIONS COMMAND: " + cmd.cmd_text);
			SendMassCxl(conn,  KOrderAction::ACTION_CXL_ALL, "VX", cmd.cmd_text);
		}
		break;

		case ExchCmd::TERMINATE:
		{
			KT01_LOG_WARN(__CLASS__, "GOT LOGOUT/TERMINATE MESSAGE IN Q");

			if (conn.tcpHandler->Fd() > 0)
			{
				KT01_LOG_WARN(__CLASS__, "SENDING LOGOUT MESSAGE");
				// send s shutdown....
				SendLogoutMsg(conn);
				conn.sessmgr.CurrentState = EnumSessionState::Terminating;
				usleep(100000);
			}
		}
		break;
		case ExchCmd::PRINT_LATENCIES:
		{
			bool clear = (cmd.cmd_value == 12) ? true : false;
			PrintLatencies(clear);
		}
		break;

		case ExchCmd::TCP_RECONNECT:
		{
			if (_CX.tcpHandler != nullptr) {
				LogMe("Executing TCP reconnect...");

				// Set reconnecting flag to prevent state changes
				_CX.is_reconnecting = true;

				// Stop current connection
				_CX.tcpHandler->Stop();

				// Wait 1 second
				usleep(1000000);

				// Start new connection (StartSessionManager sets state to StartingLoginProcess)
				StartSessionManager(_CX);
				StartTcpConnection(_CX, _State.USE_PRIMARY);

				// Clear reconnecting flag
				_CX.is_reconnecting = false;

				LogMe("TCP reconnect completed");
			} else {
				KT01_LOG_ERROR(__CLASS__, "Cannot reconnect: tcpHandler is null");
			}
		}
		break;

		case ExchCmd::RESET_RECONNECT_TRACKING:
		{
			LogMe("Resetting reconnect tracking...");

			// Reset reconnect tracking to initial state
			_CX.reconnect_attempt = 0;
			_CX.reconnect_backoff_ms = 15000;
			_CX.has_notified_disconnect = false;

			// Also reset sequence reset tracking
			_CX.reset_retry_count = 0;
			_CX.reset_backoff_ms = 30000;

			// Reset TCP handler wait time to initial 15s
			if (_CX.tcpHandler != nullptr) {
				_CX.tcpHandler->SetReconnectWaitTime(15000);
			}

			LogMe("Reconnect tracking reset: attempt=0, backoff=15s, notifications enabled");
		}
		break;
		}
	}

	_State.EVENT_WAITING.store(false, std::memory_order_relaxed);
}

void CfeGT4::Reset()
{
}

void CfeGT4::PrintLatencies(bool clear)
{
	LogMe("--- LATENCY STATS ---");

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

void CfeGT4::Command(Instruction cmd)
{
	LogMe("*** [OK] " + _State.EXCHNAME + " SESS GOT GENERIC COMMAND REQUEST: " + ExchCmd::toString(cmd.command));

	switch (cmd.command)
	{
	case ExchCmd::SESS_STATUS:
		_qCmds.push(cmd);
		_State.EVENT_WAITING.store(true, std::memory_order_relaxed);
		break;
	case ExchCmd::MEASURE:
	{
		if (_State.MEASURE)
		{
			LogMe("DISABLING LATENCY MEASURING");
			_State.MEASURE = false;
		}
		else
		{
			LogMe("ENABLING LATENCY MEASURING");
			_State.MEASURE = true;
		}
	} break;

	case ExchCmd::TCP_RECONNECT:
	{
		LogMe("TCP RECONNECT REQUEST - Closing and reconnecting TCP connection");
		_qCmds.push(cmd);
		_State.EVENT_WAITING.store(true, std::memory_order_relaxed);
	} break;

	default:
		cout << "[ExchCmd DEBUG] default switch: " << ExchCmd::toString(cmd.command) << ". EVENT_WAITING=True" << endl;
		_qCmds.push(cmd);
		_State.EVENT_WAITING.store(true, std::memory_order_relaxed);
	}
}

void CfeGT4::PrintSessionMap(ConnState &conn)
{
	conn.sessmgr.PrintStatus();
}

void CfeGT4::SendMassAction(KTN::Order &ord)
{
	// NOTE: At this point, we just cancel all orders for all segements. This can change later  but for now...
	if (ord.massscope == KTN::CME::IL3::EnumMassActionScope::EnumMassActionScope_MarketSegmentID)
	{
		Instruction cmd = {};
		cmd.command = ExchCmd::CXL_ALL_SEGS;
		cmd.cmd_value = KTN::CME::IL3::EnumMassActionScope::EnumMassActionScope_MarketSegmentID;
		_qCmds.push(cmd);
		_State.EVENT_WAITING.store(true, std::memory_order_relaxed);
	}
}

void CfeGT4::SendWarm(int index)
{
	//_CX.tcpHandler->SendWarm();
}

void CfeGT4::Send(KTN::OrderPod &ord, int action)
{
	if (UNLIKELY(!ConnState::OkToSend(_CX.sessmgr.CurrentState)))
	{
		LOGERROR("[CfeGT4] ORD {} SEND ERROR: SEGMENT {} CONNECTION IS STATE={}!",
			OrderUtils::toString(ord.ordernum),
			static_cast<uint16_t>(ord.mktsegid),
			EnumSessionState::toString(_CX.sessmgr.CurrentState));

		Notification n = {};
		n.exch = _State.EXCHNAME;
		n.source = _State.SOURCE;
		n.org = _State.ORG;
		n.level = MsgLevel::ERROR;
		n.imsg = MsgType::ALERT;
		n.text = fmt::format("Failed to send order {}: {} {}@{}. Connection state is {}",
			KT01::SECDEF::CFE::CfeSecMaster::instance().getSymbol(ord.secid),
			KTN::ORD::KOrderSide::toString(ord.OrdSide),
			ord.quantity,
			ord.price.AsUnshifted(),
			EnumSessionState::toString(_CX.sessmgr.CurrentState));
		NotifierRest::instance().Notify(n);

		return;
	}

	uint64_t seq = _CX.sessmgr.SeqOutAndAdvance();

	// because we don't have the OrderRequestId like CME does, we need to store this info
	//_clordid_to_callback.insert(ord.ordernum, ord.callbackid);
	//SetOrderCallback(ord.ordernum, ord.callbackid);
	clordIdToReqIdMap[ord.ordernum] = { ord.orderReqId, ord.exchordid, ord.secid};
	KTNBuf *msg = _KtnBufs.get();

	LOGINFO("[CFE Send OK] SEND ORDER: {}", OrderUtils::Print(ord));

	switch (ord.OrdAction)
	{
	case KOrderAction::ACTION_NEW:
	{
		msg->msgtype = AppMsgType::NewOrderV2;
		if (_State.MEASURE)
			_trk.startMeasurement("newFact");
		if (UNLIKELY(ord.isManual))
		{
			LOGINFO("[CFE Send] ord.isManual=True");

			const char *oeoid = nullptr;
			for (const CfeBoe3Settings::Tag50Pair &pair : _sett.Tag50List)
			{
				if (strcmp(pair.user.c_str(), ord.text) == 0)
				{
					oeoid = pair.tag50.c_str();
					break;
				}
			}

			_fastfact.NewOrderEncodeMan(ord, seq, oeoid, *msg, _State.logToScreen);
		}
		else
		{
			msg->buffer_length = _fastfact.NewOrderEncodeAuto(ord.ordernum, ord.quantity, ord.price, ord.secid,
															  ord.OrdSide, ord.OrdTif,	 seq, msg->buffer);
		}
		if (_State.MEASURE)
			_trk.stopMeasurement("newFact");
	}
	break;

	case KOrderAction::ACTION_MOD:
	{
		//ord.quantity = CfeOrderModDelta(ord);

		msg->msgtype = AppMsgType::ModifyOrder;
		if (_State.MEASURE)
			_trk.startMeasurement("modFact");
		if (UNLIKELY(ord.isManual))
		{
			const char *oeoid = nullptr;
			for (const CfeBoe3Settings::Tag50Pair &pair : _sett.Tag50List)
			{
				if (strcmp(pair.user.c_str(), ord.text) == 0)
				{
					oeoid = pair.tag50.c_str();
					break;
				}
			}

			_fastfact.ModifyOrderEncodeMan(ord.leavesqty + ord.fillqty, ord.price, ord.ordernum, ord.origordernum,
										   seq, ord.OrdType, oeoid, *msg);
		}
		else
		{
			msg->buffer_length = _fastfact.ModifyOrderEncodeAuto(ord.quantity, ord.price, ord.ordernum,
																 ord.origordernum, seq, msg->buffer);
		}
		if (_State.MEASURE)
			_trk.stopMeasurement("modFact");
	}
	break;

	case KOrderAction::ACTION_CXL:
	{
		msg->msgtype = AppMsgType::CancelOrder;

		if (_State.MEASURE)
			_trk.startMeasurement("cxlFact");
		if (UNLIKELY(ord.isManual))
		{
			const char *oeoid = nullptr;
			for (const CfeBoe3Settings::Tag50Pair &pair : _sett.Tag50List)
			{
				if (strcmp(pair.user.c_str(), ord.text) == 0)
				{
					oeoid = pair.tag50.c_str();
					break;
				}
			}

			_fastfact.CancelOrderEncodeMan(ord.origordernum, seq, oeoid, *msg);
		}
		else
		{
			_fastfact.CancelOrderEncodeAuto(ord.origordernum, seq, *msg);
		}
		if (_State.MEASURE)
			_trk.stopMeasurement("cxlFact");
	}
	break;

	default:
		KT01_LOG_ERROR(__CLASS__, "ORDER NOT HANDLED. INVALID SEND ACTION!!!");
		KT01_LOG_ERROR(__CLASS__, OrderUtils::Print(ord));
		return;
	}

	msg->audit_trail = true;
	msg->segid = _CX.segid;

	if (_State.MEASURE)
		_trk.startMeasurement("sendGT4");
	int res =_CX.tcpHandler->SendFastPath(msg->buffer, msg->buffer_length);
	const uint64_t sendTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	if (_State.MEASURE)
		_trk.stopMeasurement("sendGT4");

	LOGWARN("[CfeGT4] | Sent order {} | recvTime={} | qWrite={} | sendTime={} | recvToQ={} | qToSend={} | total={}",
		ord.orderReqId,
		ord.recvTime,
		ord.queueWriteTime,
		sendTime,
		ord.queueWriteTime - ord.recvTime,
		sendTime - ord.queueWriteTime,
		sendTime - ord.recvTime
	);

	if (_State.DebugRecvBytes)
	{
		LOGINFO("[CfeGT4 DEBUG] SEND: {} MSGTYPE={} MSGSIZE={} SEQ={} SEGID={} RES={}",
			KOrderAction::toString(ord.OrdAction),
			msg->msgtype,
			msg->buffer_length,
			seq,
			static_cast<uint16_t>(_CX.segid),
			res);
		NetHelpers::hexdump(msg->buffer, msg->buffer_length);
	}

	if (_State.USE_AUDIT)
	{
		msg->customerOrderTime = sendTime;
		if (_State.MEASURE)
			_trk.startMeasurement("auditOut");
		_audit->Write(msg);
		//_audit->Write(msg->buffer, msgtype, _CX.segid);
		if (_State.MEASURE)
			_trk.stopMeasurement("auditOut");
	}

	//_KtnBufs.put(msg);
}

void CfeGT4::SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3> &hdg)
{
	if (_State.MEASURE)
		_trk.startMeasurement("HdgMsgMapIns");
	_hedgeMap[ordereqid] = hdg;
	//_hedgeMap.insert(ordereqid, hdg);
	if (_State.MEASURE)
		_trk.stopMeasurement("HdgMsgMapIns");
}

bool CfeGT4::HedgeComplex(uint64_t ordreqid, int lastqty, int64_t lastpx, uint8_t cbid)
{
	_sprd_hdgs = _hedgeMap.find(ordreqid);
	if (_sprd_hdgs.size() == 0)
		return false;

	int i = 0;
	for (SpreadHedgeV3 &hdg : _sprd_hdgs)
	{
		if (LIKELY(hdg.ratio == 1))
			hdg.qty = lastqty;
		else
		{
			float newqty = hdg.ratio * lastqty;
			hdg.qty = static_cast<int>(newqty);
		}

		if (hdg.cbid > 0)
			cbid = hdg.cbid;

		std::pair<uint64_t, char *> idpair;
		_qHdgClOrdIDs.try_dequeue(idpair);

		i++;
		// here, we shortcut this because:
		//  1. the ordreqid is coming from the fill, and
		//  2. each order_request_id is incremented by 25 for this purpose
		//  3. the existing ordreqid that's passed in from the fill is already encoded
		//  with the cbid, so no need to do this again.
		//uint64_t newreqid = ordreqid + i;

		uint64_t seq = _CX.sessmgr.SeqOutAndAdvance();

		KTNBuf *msg = _KtnBufs.get();
		msg->buffer_length = _fastfact.NewOrderEncodeAuto(idpair.second, hdg.qty,
														  hdg.price, hdg.secid, hdg.OrdSide, KOrderTif::Enum::DAY, seq, msg->buffer);
		msg->audit_trail = true;
		msg->index = hdg.instindex;
		msg->segid = hdg.mktsegid;

		// if (_State.MEASURE) _trk.startMeasurement("sendGT4");
		_CX.tcpHandler->SendFastPath(msg->buffer, msg->buffer_length);
		// if (_State.MEASURE) _trk.stopMeasurement("sendGT4");

		_hdgbufs.push_back(msg);
	}

	for (auto kbuf : _hdgbufs)
	{
		_qHdgProc.push(*kbuf);
	}

	_sprd_hdgs.clear();
	_hdgbufs.clear();

	return true;
}

void CfeGT4::ProcessHdgBuf(std::vector<std::unique_ptr<KTNBuf>> &vbufs)
{
	// int cnt = 0;
	// for (const auto &kbuf : vbufs) // Use const auto& to avoid copying unique_ptr
	// {
	// 	if (_State.USE_AUDIT)
	// 	{
	// 		if (_State.MEASURE)
	// 			_trk.startMeasurement("auditOut");
	// 		_audit->Write(kbuf->buffer, IL3::META::NewOrderSingleMeta::id, kbuf->segid);
	// 		if (_State.MEASURE)
	// 			_trk.stopMeasurement("auditOut");
	// 	}

	// 	NewOrderSingleMsg *msg = reinterpret_cast<NewOrderSingleMsg *>(kbuf->buffer);

	// 	KOrderSide::Enum side = (msg->Side == EnumSideReq::EnumSideReq_Buy)
	// 								? KOrderSide::BUY
	// 								: KOrderSide::SELL;

	//	akl::Price price = CFEConverter::FromWire(msg->Price);

	// 	int8_t cbid = GetOrderCallback(msg->ClOrdID);

	// 	KTN::OrderPod ord = {};
	// 	OrderUtils::fastCopy(ord.ordernum, msg->ClOrdID, sizeof(ord.ordernum));
	// 	ord.orderReqId = msg->OrderRequestID;
	// 	ord.callbackid = cbid;
	// 	ord.secid = msg->SecurityID;
	// 	ord.OrdAction = KOrderAction::ACTION_NEW;
	// 	ord.mktsegid = kbuf->segid;
	// 	ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_HEDGE;
	// 	ord.OrdStatus = KOrderStatus::HEDGE_SENT;
	// 	ord.quantity = msg->OrderQty;
	// 	ord.price = price;
	// 	ord.OrdSide = side;
	// 	ord.leavesqty = msg->OrderQty;

	// 	if (cbid > 0)
	// 		_qOrdsProc.push(ord);

	// 	//_OrdPodPool.put(ord);
	// 	//_KtnBufs.put(kbuf);

	// 	if (_State.MEASURE)
	// 	{
	// 		ostringstream oss;
	// 		oss << "**HDG SENT " << ord.ordernum << " SIDE=" << KOrderSide::toString(ord.OrdSide)
	// 			<< " " << " PX=" << ord.price
	// 			<< " QTY=" << ord.quantity //<< " SEG=" << (int)ord.mktsegid
	// 			<< " SECID=" << ord.secid;
	// 		//<< " REQID=" << ord.orderReqId << " CLORDID=" << ord.ordernum;
	// 		LogMe(oss.str());
	// 	}
	// }
}

void CfeGT4::SendMassCxl(ConnState &conn, KOrderAction::Enum action, string root, string inst)
{
	uint64_t seq = conn.sessmgr.SeqOutAndAdvance();

	if (_State.DebugAppMsgs)
	{
		cout << "[SendMassCxl] SeqOut=" << seq << " SeqIn=" << conn.sessmgr.SeqIn() << endl;
	}

	if (UNLIKELY(!ConnState::OkToSend(conn.sessmgr.CurrentState)))
	{
		ostringstream oss;
		oss << "HANDLER IS NOT CONNECTED! CANNOT SEND!";
		KT01_LOG_ERROR(__CLASS__, oss.str());
		return;
	}

	KTNBuf msg = CFE::Boe3::AppMessageGen::MassCancelEncode(_sett.SessionSettings.EFID, _sett.Tag50_Manual, EnumManualOrderIndicator::MANUAL,
															OrderNums::instance().NextClOrdId().c_str(), seq, root, inst);

	msg.msgtype = AppMsgType::MassCancelOrder;
	msg.seqnum = seq;
	msg.audit_trail = true;
	msg.segid = conn.segid;

	if (_State.MEASURE)
		_trk.startMeasurement("sendGT4");
	conn.tcpHandler->SendFastPath(msg.buffer, msg.buffer_length);
	if (_State.MEASURE)
		_trk.stopMeasurement("sendGT4");

	ostringstream oss;
	oss << "OK: SENDING MASS CXL FOR ROOT=" << root << " INST=" << inst
		<< " ACTION=" << KOrderAction::toString(action);
	LogMe(oss.str());

	conn.sessmgr.Update();

	if (_State.USE_AUDIT)
	{
		// Create a copy for audit trail
		KTNBuf *msg2 = _KtnBufs.get();
		memcpy(msg2->buffer, msg.buffer, msg.buffer_length);
		msg2->buffer_length = msg.buffer_length;
		msg2->dir = -1;
		msg2->msgtype = msg.msgtype;
		msg2->segid = msg.segid;
		msg2->seqnum = seq;

		// Capture current time for MassCancel (no OrderPod available)
		auto now = std::chrono::high_resolution_clock::now();
		uint64_t currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
			now.time_since_epoch()).count();
		msg2->customerOrderTime = currentTime;

		if (_State.MEASURE)
			_trk.startMeasurement("auditOut");
		_audit->Write(msg2);
		//_audit->Write(msg.buffer, AppMsgType::MassCancelOrder, 1);
		if (_State.MEASURE)
			_trk.stopMeasurement("auditOut");
	}
}

void CfeGT4::SendRiskReset(ConnState &conn)
{
	uint64_t seq = conn.sessmgr.SeqOutAndAdvance();

	if (_State.DebugAppMsgs)
	{
		cout << "[SendMassCxl] SeqOut=" << seq << " SeqIn=" << conn.sessmgr.SeqIn() << endl;
	}

	if (UNLIKELY(!ConnState::OkToSend(conn.sessmgr.CurrentState)))
	{
		ostringstream oss;
		oss << "HANDLER IS NOT CONNECTED! CANNOT SEND!";
		KT01_LOG_ERROR(__CLASS__, oss.str());
		return;
	}

	KTNBuf msg = CFE::Boe3::AppMessageGen::RiskResetEncode(_sett.SessionSettings.EFID, OrderNums::instance().NextClOrdId().c_str(), seq);

	msg.msgtype = AppMsgType::ResetRisk;
	msg.seqnum = seq;
	msg.audit_trail = true;

	if (_State.MEASURE)
		_trk.startMeasurement("sendGT4");
	conn.tcpHandler->SendFastPath(msg.buffer, msg.buffer_length);
	if (_State.MEASURE)
		_trk.stopMeasurement("sendGT4");

	ostringstream oss;
	oss << "OK: SENDING RISK RESET";
	LogMe(oss.str());

	conn.sessmgr.Update();

	if (_State.USE_AUDIT)
	{
		// Create a copy for audit trail
		KTNBuf *msg2 = _KtnBufs.get();
		memcpy(msg2->buffer, msg.buffer, msg.buffer_length);
		msg2->buffer_length = msg.buffer_length;
		msg2->dir = -1;
		msg2->msgtype = msg.msgtype;
		msg2->segid = msg.segid;
		msg2->seqnum = seq;

		// Capture current time for ResetRisk (no OrderPod available)
		auto now = std::chrono::high_resolution_clock::now();
		uint64_t currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
			now.time_since_epoch()).count();
		msg2->customerOrderTime = currentTime;

		if (_State.MEASURE)
			_trk.startMeasurement("auditOut");
		_audit->Write(msg2);
		//_audit->Write(msg.buffer, AppMsgType::ResetRisk, 1);
		if (_State.MEASURE)
			_trk.stopMeasurement("auditOut");
	}
}

//*********** KTN MSG PROCESSING *****************************************

void CfeGT4::onStateChange(EnumSessionState::Enum newState, ConnState &conn)
{
	if (newState == EnumSessionState::Disconnected)
	{
		// Send disconnect notification only on first disconnect
		if (!conn.has_notified_disconnect)
		{
			ostringstream oss;
			oss << "SEG " << conn.segid << ": SESSION DISCONNECTED";
			KT01_LOG_ERROR(__CLASS__, oss.str());

			// Send notification
			KTN::notify::NotifierRest::NotifyError(_State.EXCHNAME, _State.SOURCE, _State.ORG, oss.str());

			conn.has_notified_disconnect = true;  // Mark as notified
		}

		// Calculate exponential backoff wait time for reconnect (do this on EVERY disconnect)
		// 15s, 30s, 60s, 120s, 240s, 300s (cap at 5 minutes)
		int wait_times_ms[] = {15000, 30000, 60000, 120000, 240000, 300000};
		int attempt_index = std::min(conn.reconnect_attempt, 5);
		conn.reconnect_backoff_ms = wait_times_ms[attempt_index];

		// Update StandardTcpV1 wait time BEFORE it starts waiting
		conn.tcpHandler->SetReconnectWaitTime(conn.reconnect_backoff_ms);

		ostringstream backoff_log;
		backoff_log << "SEG " << conn.segid << ": Disconnect detected. Next reconnect will wait "
		            << (conn.reconnect_backoff_ms / 1000) << "s (attempt #"
		            << (conn.reconnect_attempt + 1) << ")";
		LogMe(backoff_log.str());
	}

	conn.sessmgr.onStateChange(newState);
}

void CfeGT4::ProcessSessionMessage(ConnState &conn, KTNBuf8t &msg)
{
	switch (msg.msgtype)
	{
	case Boe3::ServerHeartbeat:
		onHeartbeatMsg(conn, msg.buffer, msg.msg_length, msg.segid);
		break;

	case Boe3::LoginResponse:
		onLoginReponseMsg(conn, msg.buffer, msg.msg_length, msg.segid);
		break;

	case Boe3::LogoutResponse:
		onLogoutResponseMsg(conn, msg.buffer, msg.msg_length, msg.segid);
		break;

	case Boe3::ReplayComplete:
		onReplayCompleteMsg(conn, msg.buffer, msg.msg_length, msg.segid);
		break;

	default:
		KT01_LOG_ERROR(__CLASS__, "SESS MSG PROC: TEMPLATE " + std::to_string(msg.msgtype) + " NOT HANDLED!");
		break;
	}
}

void KTN::CFE::CfeGT4::onLoginReponseMsg(ConnState &conn, byte_ptr buf, int iMsgLen, int segid)
{
	auto msg = reinterpret_cast<LoginResponseMsg *>(buf);
	buf += sizeof(LoginResponseMsg);

	ostringstream oss;
	oss << "LOGON RESPONSE: " << toString(msg->LoginResponseStatus) << " TEXT=" << msg->LoginResponseText << " LAST SEQ OUT (ClientSeqNum)=" << msg->ClientSeqNum
		<< " LAST SEQ IN (UnitSequence)=" << msg->UnitSequence << " UNIT NBR=" << (int)msg->UnitNumber;
	LogMe(oss.str());

	if (msg->LoginResponseStatus == EnumLoginResponseStatus::Accepted)
	{
		//short note on this:
		// We are receiving the last seq out from the server, which is the last seqnum that we sent.
		// We need to set the seqout to this value + 1, as the server will be sending us the next message

		// Check for sequence number mismatch
		uint32_t file_seqin = conn.sessmgr.SeqIn();
		uint32_t server_seqin = msg->UnitSequence;

		if (file_seqin > server_seqin) {
			ostringstream warn_oss;
			warn_oss << "SEQUENCE MISMATCH DETECTED: File SeqIn=" << file_seqin
			         << " > Server UnitSequence=" << server_seqin
			         << ". Session needs reset. Will reset and reconnect.";
			KT01_LOG_WARN(__CLASS__, warn_oss.str());

			// Set flag to trigger reset in Dispatch()
			conn.needs_reset_reconnect = true;

			// Don't establish session - let Dispatch() handle reset
			return;
		}

		conn.sessmgr.SetSeqIn(msg->UnitSequence, true);
		conn.sessmgr.SetSeqOut(msg->ClientSeqNum + 1, true);
		onStateChange(EnumSessionState::Established, conn);

		LogMe("OK LOGON SUCCESSFUL!");

		// Send login success notification
		ostringstream notify_oss;
		notify_oss << "SEG " << conn.segid << ": LOGIN SUCCESSFUL";
		KTN::notify::NotifierRest::NotifyInfo(_State.EXCHNAME, _State.SOURCE, _State.ORG, notify_oss.str());

		// Reset reconnect tracking - allow disconnect notification on next disconnect
		conn.has_notified_disconnect = false;
		conn.reconnect_attempt = 0;
		conn.reconnect_backoff_ms = 15000;  // Reset to 15s

		// Also reset sequence reset tracking
		conn.reset_retry_count = 0;
		conn.reset_backoff_ms = 30000;  // Reset to 30s
	}
	else
	{
		// Check if login rejected due to sequence ahead
		if (msg->LoginResponseStatus == EnumLoginResponseStatus::Sequence_Ahead) {
			ostringstream warn_oss;
			warn_oss << "LOGIN REJECTED - SEQUENCE AHEAD. Server rejected our sequence numbers. "
			         << "Will reset session and reconnect.";
			KT01_LOG_WARN(__CLASS__, warn_oss.str());

			// Set flag to trigger reset in Dispatch()
			conn.needs_reset_reconnect = true;

			// Set state to disconnected but let Dispatch() handle reset
			onStateChange(EnumSessionState::Disconnected, conn);
			return;
		}

		onStateChange(EnumSessionState::Disconnected, conn);
		ostringstream oss;
		oss << "LOGON REJECTED: " << toString(msg->LoginResponseStatus) << " TEXT=" << msg->LoginResponseText;
		KT01_LOG_ERROR(__CLASS__, oss.str());
	}
}

void KTN::CFE::CfeGT4::onLogoutResponseMsg(ConnState &conn, byte_ptr buf, int iMsgLen, int segid)
{
	onStateChange(EnumSessionState::Disconnected, conn);

	auto msg = reinterpret_cast<LogoutResponseMsg *>(buf);
	buf += sizeof(LogoutResponseMsg);

	ostringstream oss;
	oss << "LOGOUT RESPONSE: " << toString(msg->LogoutReason) << " TEXT=" << msg->LogoutReasonText;
	KT01_LOG_WARN(__CLASS__, oss.str());
}
void KTN::CFE::CfeGT4::onReplayCompleteMsg(ConnState &conn, byte_ptr buf, int iMsgLen, int segid)
{
	//auto msg = reinterpret_cast<ReplayCompleteMsg *>(buf);
	buf += sizeof(ReplayCompleteMsg);

	ostringstream oss;
	oss << "[OK] REPLAY COMPLETE!";
	LogMe(oss.str());

	conn.sessmgr.SetReplayComplete(true);

	onStateChange(EnumSessionState::Established, conn);
}
void KTN::CFE::CfeGT4::onHeartbeatMsg(ConnState &conn, byte_ptr buf, int iMsgLen, int segid)
{
	//auto msg = reinterpret_cast<ServerHeartbeatMsg *>(buf);
	buf += sizeof(ServerHeartbeatMsg);

	// if (_State.DebugSessMsgs)
	//cout << "[onHeartbeatMsg DEBUG] Server Heartbeat received." << endl;

	//SendHeartbeatMsg(conn);
}

int CfeGT4::ProcessApplicationMessage(ConnState &conn, KTNBuf8t &msg, bool ignore_seqs)
{
	int msgbytes = msg.buffer_length;

	if (_State.DebugAppMsgs)
		cout << "[ProcessApp] seqnum=" << msg.seqnum << " totalen[including groups]=" << msg.buffer_length << " type=" << msg.msgtype << endl;

	msg.buffer = msg.buf;
	msg.segid = conn.segid;

	if (ignore_seqs || msg.seqnum == 0)
	{
		if (_State.DebugAppMsgs)
			cout << "[ProcessApp] SeqNum=0. UNSEQUENCED MESSAGE. IGNORING SEQNUM" << endl;
	}
	else if (!CheckIncomingSeq(conn, msg))
	{
		msgbytes = msg.msg_length; // IL3Helpers::GetLiveBytes(msg);
		if (_State.DebugAppMsgs)
			cout << "[ProcessAppMsg DEBUG] Returning MsgLen=" << msgbytes << endl;
		return msgbytes;
	}

	

	switch (msg.msgtype)
	{
	// application messages:
	// Order Manager:
	case OrderAckMsg::id:
		onExecRptNew(msg.buffer, msg.msg_length, msg.segid, msg.recvTime);
		break;
	case OrderExecutionMsg::id:
		onExecRptTrade(msg.buffer, msg.msg_length, msg.segid, msg.recvTime);
		break;
	case OrderCancelledMsg::id:
		onExecRptCancel(msg.buffer, msg.msg_length, msg.segid, msg.recvTime);
		break;
	case OrderModifiedMsg::id:
		onExecRptModify(msg.buffer, msg.msg_length, msg.segid, msg.recvTime);
		break;
	case OrderRejectMsg::id:
		onExecRptReject(msg.buffer, msg.msg_length, msg.segid, msg.recvTime);
		break;
	case CancelRejectMsg::id:
		onOrderCancelReject(msg.buffer, msg.msg_length, msg.segid, msg.recvTime);
		break;
	case ModifyRejectedMsg::id:
		onOrderCancelReplaceReject(msg.buffer, msg.msg_length, msg.segid, msg.recvTime);
		break;
	case TradeCancelCorrectMsg::id:
		onTradeCancelCorrect(msg.buffer, msg.msg_length, msg.segid);
		break;
	case ResetRiskAckMsg::id:
		onRiskResetAck(msg.buffer, msg.msg_length, msg.segid);
		break;

	case MassCancelAckMsg::id:
		// NOTE FOR TESTING: THIS IS AN UNSEQUENCED MESSAGE
		onMassCancelAck(msg.buffer, msg.msg_length, msg.segid);
		break;
	case MassCancelRejectMsg::id:
		onMassCancelReject(msg.buffer, msg.msg_length, msg.segid);
		break;
	case TASRestatementMsg::id:
		onTASRestatement(msg.buffer, msg.msg_length, msg.segid);
		break;
	}

	return msgbytes;
}

bool CfeGT4::CheckIncomingSeq(ConnState &conn, KTNBuf8t &msg)
{
	uint32_t next_expected_seq = conn.sessmgr.SeqIn() + 1;

	if (LIKELY(conn.sessmgr.CurrentState == EnumSessionState::Established))
	{
		// std::cout << "[Incoming] Processing message seq " << msg.seqnum << " expected=" << next_expected_seq << std::endl;
		if (LIKELY(msg.seqnum == next_expected_seq))
		{
			if (_State.DebugSession)
				std::cout << "[OK] GT4 Processing message seq " << msg.seqnum << " expected=" << next_expected_seq << std::endl;
			// In-order: process immediately.
			conn.sessmgr.SetSeqIn(msg.seqnum, true);
			return true;
		}
	}

	cout << "[CheckIncomingSeq WARNING] SEQS DO NOT MATCH! seqnum=" << msg.seqnum << " expected=" << next_expected_seq << endl;

	if (_State.DebugSession)
		std::cout << "[GT4 Incoming] Processing message seq " << msg.seqnum << " expected=" << next_expected_seq << std::endl;

	if (conn.sessmgr.ReplayComplete() == false)
	{
		ostringstream oss;
		oss << "CheckIncomingSeq: Replay is not complete! SEQNUM=" << msg.seqnum << ". Messages are being replayed!";
		KT01_LOG_WARN(__CLASS__, oss.str());
	}

	return true;

	// CheckSequenceResult result = conn.sessmgr.CheckIncomingSequenceOK(msg);

	// if (LIKELY(result.type == CheckSequenceResult::ResultType::OK))
	// {
	// 	return true;
	// }

	// if (result.type == CheckSequenceResult::ResultType::Ignore)
	// {
	// 	KT01_LOG_ERROR(__CLASS__, "RESULT: IGNORE- BAD DATA! " + std::to_string(msg.seqnum));
	// 	return false;
	// }

	// if (result.type == CheckSequenceResult::ResultType::Retransmit)
	// {
	// 	KT01_LOG_ERROR(__CLASS__, "[ERROR] RESULT: RETRANSMIT NEEDED- HOW DO WE DO THIS IN CFE????");
	// 	//SendRetransmitRequestMsg(conn, result.startSeq, result.numMsgs);
	// 	return false;
	// }

	// if (result.type == CheckSequenceResult::ResultType::Fatal)
	// {
	// 	ostringstream oss;
	// 	oss << "RESULT: FATAL- SEQNUM IS LESS THAN EXPECTED: " << msg.seqnum
	// 		<< " Expected=" << next_expected_seq << " UUID=" << conn.sessmgr.Uuid()
	// 		<< " PrevUUID=" << conn.sessmgr.PrevUUID() << " PrevSeqIn=" << conn.sessmgr.PrevSeqIn();
	// 		KT01_LOG_ERROR(__CLASS__, oss.str());
	// 	SendLogoutMsg(conn);
	// 	return false;
	// }

	// if (result.type == CheckSequenceResult::ResultType::Queued)
	// {
	// 	LogMe("OK: QUEUED MESSAGE " + std::to_string(msg.seqnum));
	// 	return false;
	// }

	//return false;
}

void CfeGT4::onExecRptNew(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime)
{
	auto msg = reinterpret_cast<OrderAckMsg::BlockData *>(buf);

	bool possretransmit = false;
	if (UNLIKELY(_CX.sessmgr.ReplayComplete() == false))
	{
		possretransmit = true;
		ostringstream oss;
		oss << "onExecRptNew: ReplayComplete = FALSE. SEQNUM=" << msg->SeqNum << ". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str());
	}

	if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;


	// returns 255 (uint8_t NULL) if not found...
	//int8_t cbid = GetOrderCallback(msg->ClOrdID);// _clordid_to_callback.find(msg->ClOrdID);
	//_exchid_to_ordreqid[msg->OrderID] = Base63Codec::decode(msg->ClOrdID);
	

	KTN::OrderPod *ord = _OrdPodPool.get();
	//ord->callbackid = cbid;
	ord->OrdStatus = KOrderStatus::NEW;
	ord->OrdState = KOrderState::WORKING;
	ord->OrdExch = KOrderExchange::CFE;

	ord->orderReqId = Base63Codec::decode(msg->ClOrdID);

	OrderMapType::iterator it = clordIdToReqIdMap.find(msg->ClOrdID);
	if (it != clordIdToReqIdMap.end())
	{
		it->second.exchId = msg->OrderID;
	}

	//now, we are tracking the orderReqId for the exchange order ID, since we can't rely on clordid since it changes
	//SetOrderReqIdForExchExchId(msg->OrderID, ord->orderReqId);
	
	ord->secid = Base63Codec::decode(msg->SecId);
	ord->exchordid = msg->OrderID;
	OrderUtils::fastCopy(ord->ordernum, msg->ClOrdID, sizeof(ord->ordernum));
	ord->OrdSide = (msg->Side == EnumOrderSide::BUY) ? KOrderSide::BUY : KOrderSide::SELL;
	
	ord->quantity = msg->OrderQty;

	ord->leavesqty = msg->LeavesQty;
	ord->fillqty = msg->OrderQty > msg->LeavesQty ? (msg->OrderQty - msg->LeavesQty) : 0;
	ord->timestamp = msg->TransactTime;
	ord->recvTime = recvTime;

	ord->possRetransmit = possretransmit;

	ord->price = CFEConverter::FromWire(msg->Price);

	ord->OrdOpenClose = KOrderOpenCloseInd::NONE;

	if (msg->OpenClose == EnumOrderOpenClose::CLOSE	)
		ord->OrdOpenClose = KOrderOpenCloseInd::CLOSE;
	else if (msg->OpenClose == EnumOrderOpenClose::OPEN)
		ord->OrdOpenClose = KOrderOpenCloseInd::OPEN;
	
		
	if (msg->OrdType == CFE::Boe3::EnumOrderType::StopLimit)
		ord->stopprice = CFEConverter::FromWire(msg->StopPx);


	if (UNLIKELY(_State.logToScreen))
	{
		cout << "[CfeGT4] onExecRptNew: orderReqId=" << ord->orderReqId << "  secid=" << ord->secid << " price=" << ord->price << endl;
	}

	ord->queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(*ord);

	_OrdPodPool.put(ord);
}

void CfeGT4::onExecRptModify(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime)
{
	auto msg = reinterpret_cast<OrderModifiedMsg::BlockData *>(buf);

	bool possretransmit = false;
	if (UNLIKELY(_CX.sessmgr.ReplayComplete() == false))
	{
		possretransmit = true;
		ostringstream oss;
		oss << "onExecRptModify: ReplayComplete = FALSE. SEQNUM=" << msg->SeqNum << ". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str());
	}

	if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	//int8_t cbid = GetOrderCallback(msg->ClOrdID);// _clordid_to_callback.find(msg->ClOrdID);

	KTN::OrderPod *ord = _OrdPodPool.get();
	//ord->callbackid = cbid;

	if (msg->LeavesQty > 0)
	{
		ord->OrdStatus = KOrderStatus::MODIFIED;
		ord->OrdState = KOrderState::WORKING;
	}
	else
	{
		ord->OrdStatus = KOrderStatus::CANCELED;
		ord->OrdState = KOrderState::COMPLETE;
	}
	
	ord->OrdExch = KOrderExchange::CFE;

	OrderMapType::iterator it = clordIdToReqIdMap.find(msg->ClOrdID);
	if (it != clordIdToReqIdMap.end())
	{
		ord->orderReqId = it->second.orderReqId;
		it->second.exchId = msg->OrderID;
	}
	else
	{
		ord->orderReqId = 0;
	}
	//clordIdToReqIdMap.erase(msg->OrigClOrdID);

	//ord->orderReqId = GetOrderReqIdFromExchId(msg->OrderID);//  Base63Codec::decode(msg->ClOrdID);
	OrderUtils::fastCopy(ord->ordernum, msg->ClOrdID, sizeof(ord->ordernum));
	ord->exchordid = msg->OrderID;
	ord->timestamp = msg->TransactTime;
	ord->recvTime = recvTime;

	ord->quantity = msg->OrderQty;
	ord->leavesqty = msg->LeavesQty;

	ord->price = CFEConverter::FromWire(msg->Price);
	ord->possRetransmit = possretransmit;

	if (msg->OrdType == Boe3::EnumOrderType::StopLimit)
		ord->stopprice = CFEConverter::FromWire(msg->StopPx);


	if (UNLIKELY(_State.logToScreen))
	{
		cout << "-------------------------------------------" << endl;
		cout << "[onExecRptModify] ClordID=" << msg->ClOrdID << "  OrigClordID=" << msg->OrigClOrdID
			<< " NEW ORD QTY=" << msg->OrderQty << "  NEW LEAVES=" << msg->LeavesQty << " NEW PX=" << msg->Price << endl;
		cout << "-----------------------------------------" << endl;
	}

	ord->queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(*ord);
	_OrdPodPool.put(ord);
}

void CfeGT4::onExecRptCancel(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime)
{
	auto msg = reinterpret_cast<OrderCancelledMsg::BlockData *>(buf);
	bool possretransmit = false;

	if (UNLIKELY(_CX.sessmgr.ReplayComplete() == false))
	{
		possretransmit = true;
		ostringstream oss;
		oss << "onExecRptCancel: ReplayComplete = FALSE. SEQNUM=" << msg->SeqNum << ". This may be a retransmission. Not Incementing Sequences.";
		KT01_LOG_WARN(__CLASS__, oss.str());

		cout << "[WARN] " << oss.str() << endl;
	}

	if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	//int8_t cbid = GetOrderCallback(msg->ClOrdID);// _clordid_to_callback.find(msg->ClOrdID);
	
	KTN::OrderPod *ord = _OrdPodPool.get();
	
	fastCopy2(ord->ordernum, msg->ClOrdID, sizeof(msg->ClOrdID));
	//ord->orderReqId = 0;//Base63Codec::decode(msg->ClOrdID);

	OrderMapType::const_iterator it = clordIdToReqIdMap.find(msg->ClOrdID);
	if (it != clordIdToReqIdMap.end())
	{
		ord->orderReqId = it->second.orderReqId;
		ord->exchordid = it->second.exchId;
		ord->secid = it->second.secId;
		//clordIdToReqIdMap.erase(it);
	}
	else
	{
		ord->orderReqId = 0;
	}

	//ord->callbackid = cbid;
	ord->OrdStatus = KOrderStatus::CANCELED;
	ord->OrdState = KOrderState::COMPLETE;
	ord->OrdExch = KOrderExchange::CFE;
	
	ord->timestamp = msg->TransactTime;
	ord->recvTime = recvTime;
	ord->leavesqty = 0;

	ord->possRetransmit = possretransmit;
	if (possretransmit)
		ord->OrdState = KOrderState::STATUS_UPDATE;

	ord->queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(*ord);

	if (msg->CancelReason == EnumOrderReasonCode::WouldWash)
	{
		Notification n = {};
		n.exch = _State.EXCHNAME;
		n.source = _State.SOURCE;
		n.org = _State.ORG;
		n.level = MsgLevel::INFO;
		n.imsg = MsgType::NOTIFY;
		n.text = fmt::format("{} order canceled for wash trade prevention.  ClOrdId: {}, EOID: {}",
			KT01::SECDEF::CFE::CfeSecMaster::instance().getSymbol(ord->secid),
			msg->ClOrdID,
			ord->exchordid);
		NotifierRest::instance().Notify(n);
	}

	_OrdPodPool.put(ord);
}

void CfeGT4::onExecRptTrade(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime)
{
	auto msg = reinterpret_cast<OrderExecutionMsg::BlockData *>(buf);

	bool possretransmit = false;
	if (UNLIKELY(_CX.sessmgr.ReplayComplete() == false))
	{
		possretransmit = true;
		ostringstream oss;
		oss << "onExecRptTrade: ReplayComplete = FALSE. SEQNUM=" << msg->SeqNum << ". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str());
	}

	if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	//int8_t cbid = GetOrderCallback(msg->ClOrdID);// _clordid_to_callback.find(msg->ClOrdID);
	//uint64_t orderreqid = 0;//Base63Codec::decode(msg->ClOrdID);

	//bool senthedge = HedgeComplex(orderreqid, msg->LastShares, msg->LastPx, cbid);
	
	KTN::OrderPod *ord = _OrdPodPool.get();

	//auto ord = new KTN::OrderPod();

	OrderMapType::const_iterator it = clordIdToReqIdMap.find(msg->ClOrdID);
	if (it != clordIdToReqIdMap.end())
	{
		ord->orderReqId = it->second.orderReqId;
		ord->exchordid = it->second.exchId;
		ord->secid = it->second.secId;
	}
	else
	{
		ord->orderReqId = 0;
		ord->exchordid = 0;
	}

	//ord->orderReqId = orderreqid;
	ord->OrdExch = KOrderExchange::CFE;

	//ord->callbackid = cbid;
	ord->timestamp = msg->TransactTime;
	ord->recvTime = recvTime;
	ord->secid = Base63Codec::decode(msg->SecId);

	ord->lastpx = CFEConverter::FromWire(msg->LastPx);
	ord->lastqty = msg->LastShares;
	OrderUtils::toChar(ord->execid, msg->ExecId, sizeof(ord->execid));
	OrderUtils::fastCopy(ord->ordernum, msg->ClOrdID, sizeof(ord->ordernum));
	
	uint32_t lvs = 0;
	KOrderStatus::Enum status = KOrderStatus::FILLED;
	KOrderState::Enum state = KOrderState::COMPLETE;

	if (msg->LeavesQty > 0)
	{
		lvs = msg->LeavesQty;
		status = KOrderStatus::PARTIALLY_FILLED;
		state = KOrderState::WORKING;
	}

	ord->leavesqty = lvs;
	ord->OrdStatus = status;
	ord->OrdState = state;

	// ord->exchordid = msg->OrderID;

	switch (msg->MultilegReportingType)
	{
	case EnumMultilegReportType::OUTRIGHT:
		ord->OrdFillType = KOrderFillType::OUTRIGHT_FILL;
		break;
	case EnumMultilegReportType::SPREAD_LEG:
		ord->OrdFillType = KOrderFillType::SPREAD_LEG_FILL;
		break;
	case EnumMultilegReportType::SPREAD:
		ord->OrdFillType = KOrderFillType::SPREAD_FILL;
		break;
	default:
		ord->OrdFillType = KOrderFillType::OUTRIGHT_FILL;
		break;
	}

	ord->OrdSide = (msg->Side == EnumOrderSide::BUY) ? KOrderSide::BUY : KOrderSide::SELL;
	ord->possRetransmit = possretransmit;

	// if (senthedge)
	// 	ord->OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;

	ostringstream oss;
	oss << "[OK] onExecRptTrade FILL: " << msg->ClOrdID << " REQID=" << ord->orderReqId// << " CBID=" << (int)cbid
		<< " FILLYTPE=" << KOrderFillType::toString(ord->OrdFillType)	
		<< " SEC=" << msg->SecId << " SIDE=" << (int)msg->Side
		<< " LASTQTY=" << msg->LastShares 
		<< " LASTPX=" << msg->LastPx << " LVSQTY=" << msg->LeavesQty
		<< " ORDSTATUS=" << KOrderStatus::toString(ord->OrdStatus)
		<< " ORDSTATE=" << KOrderState::toString(ord->OrdState);
	LogMe(oss.str());

	ord->tradeDate = msg->TradeDate;
	ord->queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(*ord);

	if (UNLIKELY(_State.DebugAppMsgs))
	{
		ostringstream oss;
		oss << "[OK] onExecRptTrade FILL: " << msg->ClOrdID << " REQID=" << ord->orderReqId// << " CBID=" << (int)cbid
			<< " FILLYTPE=" << KOrderFillType::toString(ord->OrdFillType)
			<< " SEC=" << msg->SecId << " SIDE=" << (int)msg->Side
			<< " LASTQTY=" << msg->LastShares
			<< " LASTPX=" << msg->LastPx << " LVSQTY=" << msg->LeavesQty
			<< " ORDSTATUS=" << KOrderStatus::toString(ord->OrdStatus)
			<< " ORDSTATE=" << KOrderState::toString(ord->OrdState);
		LogMe(oss.str());
	}

	_OrdPodPool.put(ord);
}

void CfeGT4::onExecRptReject(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime)
{
	auto msg = reinterpret_cast<OrderRejectMsg::BlockData *>(buf);

	if (UNLIKELY(_State.logToScreen))
	{
		cout << "[onExecRptReject ERROR] seqnum=" << msg->SeqNum
			<< "|transtime=" << msg->TransactTime
			<< "|clordid=" << msg->ClOrdID
			<< "|rejectreason=" << toString(msg->OrderRejectReason)
			<< "|text=" << msg->Text;
	}

	//int8_t cbid = _clordid_to_callback.find(msg->ClOrdID);

	uint64_t orderreqid = Base63Codec::decode(msg->ClOrdID);
	
	bool possretransmit = false;

	if (UNLIKELY(_CX.sessmgr.ReplayComplete() == false))
	{
		possretransmit = true;
		ostringstream oss;
		oss << "onExecRptReject: ReplayComplete = FALSE. SEQNUM=" << msg->SeqNum << ". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str());
	}

	if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	KTN::OrderPod ord = {};
	ord.OrdStatus = KOrderStatus::REJECTED;
	ord.OrdState = KOrderState::COMPLETE;
	ord.OrdExch = KOrderExchange::CFE;
	ord.OrdSide = KOrderSide::UNKNOWN;
	ord.OrdAction = KOrderAction::ACTION_REJ;
	ord.orderReqId = orderreqid;

	OrderUtils::fastCopy(ord.ordernum, msg->ClOrdID, sizeof(ord.ordernum));
	// ord.exchordid = msg->OrderID;

	//ord.callbackid = cbid;
	ord.possRetransmit = possretransmit;

	ord.timestamp = msg->TransactTime;
	ord.recvTime = recvTime;

	ord.leavesqty = 0;
	ord.rejcode = msg->OrderRejectReason;
	OrderUtils::fastCopy(ord.text, msg->Text, sizeof(ord.text));

	OrderMapType::const_iterator it = clordIdToReqIdMap.find(msg->ClOrdID);
	if (it != clordIdToReqIdMap.end())
	{
		ord.secid = it->second.secId;
	}
	else
	{
		ord.secid = 0;
	}

	ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(ord);

	Notification n = {};
	n.exch = _State.EXCHNAME;
	n.source = _State.SOURCE;
	n.org = _State.ORG;
	n.level = MsgLevel::ERROR;
	n.imsg = MsgType::ALERT;
	n.text = fmt::format("{} order rejected. Reason={}; Text={}; ClOrdId={}",
		KT01::SECDEF::CFE::CfeSecMaster::instance().getSymbol(ord.secid),
		toString(msg->OrderRejectReason),
		msg->Text,
		msg->ClOrdID);

	NotifierRest::instance().Notify(n);

	LOGERROR("[CfeGT4] onExecRptReject: {}", n.text);
}

// NOTE FOR TESTING: THIS IS AN UNSEQUENCED MESSAGE
void CfeGT4::onMassCancelAck(byte_ptr buf, int iMsgLen, int segid)
{
	auto msg = reinterpret_cast<MassCancelAckMsg::BlockData *>(buf);

	if (_State.DebugAppMsgs)
	{
		ostringstream oss;
		oss << "[onMassCancelAck DEBUG] seqnum=" << msg->SeqNum
			<< "|transtime=" << msg->TransactTime
			<< "|masscancelid=" << msg->MassCancelId
			<< "|cancelledordcount=" << msg->CancelledOrderCount
			<< "|rectimestamp=" << msg->RequestReceivedTime;

		cout << oss.str() << endl;
	}

	if (UNLIKELY(_State.logToScreen))
	{
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;
	}

	LogMe("[OK] onMassCancelAck: " + std::to_string(msg->CancelledOrderCount) + " orders cancelled.");
}

void CfeGT4::onMassCancelReject(byte_ptr buf, int iMsgLen, int segid)
{
	auto msg = reinterpret_cast<MassCancelRejectMsg::BlockData *>(buf);

	if (_State.DebugAppMsgs)
	{
		ostringstream oss;
		oss << "[onMassCancelREJECT DEBUG]  seqnum=" << msg->SeqNum
			<< "|transtime=" << msg->TransactTime
			<< "|masscancelid=" << msg->MassCancelId
			<< "|rejectreason=" << toString(msg->MassCancelRejectReason)
			<< "|text=" << msg->Text;

		cout << oss.str() << endl;
	}

	if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	KT01_LOG_ERROR(__CLASS__, "onMassCancelReject: " + toString(msg->MassCancelRejectReason) + ".  TEXT=" + std::string(msg->Text));
}

void CfeGT4::onOrderCancelReject(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime)
{
	auto msg = reinterpret_cast<CancelRejectMsg::BlockData *>(buf);

	//int8_t cbid = _clordid_to_callback.find(msg->ClOrdID);
	//uint64_t orderreqid;// = Base63Codec::decode(msg->ClOrdID);

	KTN::OrderPod ord = {};

	OrderMapType::const_iterator it = clordIdToReqIdMap.find(msg->ClOrdID);
	if (it != clordIdToReqIdMap.end())
	{
		ord.orderReqId = it->second.orderReqId;
		ord.exchordid = it->second.exchId;
		ord.secid = it->second.secId;
	}
	else
	{
		ord.orderReqId = 0;
		ord.exchordid = 0;
	}

	bool possretransmit = false;

	if (UNLIKELY(_CX.sessmgr.ReplayComplete() == false))
	{
		possretransmit = true;
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum << ". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str());
	}

	if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	string reason = msg->Text;

	if (reason.length() == 0)
		reason = "CXL REJECT TEXT NOT PRESENT!";

	OrderUtils::fastCopy(ord.ordernum, msg->ClOrdID, strlen(ord.ordernum));

	// ord.exchordid = msg->OrderID;
	// OrderUtils::fastCopy(ord.execid, msg->ExecID, strlen(ord.execid));

	//ord.callbackid = cbid;
	ord.possRetransmit = possretransmit;

	ord.timestamp = msg->TransactTime;
	ord.recvTime = recvTime;
	ord.fillqty = 0;
	ord.leavesqty = 0;

	ord.OrdState = KOrderState::COMPLETE;
	ord.OrdStatus = KOrderStatus::CXL_REJECT;
	ord.OrdAction = KOrderAction::ACTION_CXL_REJ;

	OrderUtils::fastCopy(ord.text, reason.c_str(), sizeof(ord.text));
	ord.rejcode = msg->CancelRejectReason;

	ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(ord);

	const std::string  msg_str = fmt::format("{} cancel rejected. Reason={}; Text={}; ClOrdId={}",
		KT01::SECDEF::CFE::CfeSecMaster::instance().getSymbol(ord.secid),
		toString(msg->CancelRejectReason),
		msg->Text,
		msg->ClOrdID);
	LOGERROR("[CfeGT4] {}", msg_str);

	Notification n = {};
	n.exch = _State.EXCHNAME;
	n.source = _State.SOURCE;
	n.org = _State.ORG;
	n.level = MsgLevel::ERROR;
	n.imsg = MsgType::ALERT;
	n.text = msg_str;
	NotifierRest::instance().Notify(n);
}

void CfeGT4::onOrderCancelReplaceReject(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime)
{
	auto msg = reinterpret_cast<ModifyRejectedMsg::BlockData *>(buf);

	//int8_t cbid = _clordid_to_callback.find(msg->ClOrdID);
	bool possretransmit = false;

	if (UNLIKELY(_CX.sessmgr.ReplayComplete() == false))
	{
		ostringstream oss;
		oss << "onExecRptNew: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum << ". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str());
	}

	if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	KTN::OrderPod ord = {};

	OrderMapType::const_iterator it = clordIdToReqIdMap.find(msg->ClOrdID);
	if (it != clordIdToReqIdMap.end())
	{
		ord.orderReqId = it->second.orderReqId;
		ord.exchordid = it->second.exchId;
		ord.secid = it->second.secId;
	}
	else
	{
		ord.orderReqId = 0;
		ord.exchordid = 0;
		ord.secid = 0;
	}

	std::string reason = msg->Text;
	if (reason.length() == 0)
		reason = "CXL REPL REJECT TEXT NOT PRESENT!";

	OrderUtils::fastCopy(ord.ordernum, msg->ClOrdID, strlen(msg->ClOrdID));

	//ord.orderReqId = orderreqid;
	// ord.exchordid = msg->OrderID;
	//ord.callbackid = cbid;
	ord.timestamp = msg->TransactTime;
	ord.recvTime = recvTime;
	ord.fillqty = 0;
	ord.leavesqty = 0;
	ord.possRetransmit = possretransmit;
	ord.rejcode = msg->ModifyRejectReason;
	OrderUtils::fastCopy(ord.text, reason.c_str(), sizeof(ord.text));

	ord.OrdState = KOrderState::COMPLETE;
	ord.OrdStatus = KOrderStatus::CXL_REPL_REJECT;
	ord.OrdAction = KOrderAction::ACTION_CXL_REPL_REJ;

	string txt = msg->Text;

	if (txt.length() == 0)
	{
		txt = "ERROR: REJECT TEXT NOT PRESENT!";
	}

	ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	_qOrdsProc.push(ord);

	const std::string  msg_str = fmt::format("{} modify rejected. Reason={}; Text={}; ClOrdId={}",
		KT01::SECDEF::CFE::CfeSecMaster::instance().getSymbol(ord.secid),
		toString(msg->ModifyRejectReason),
		msg->Text,
		msg->ClOrdID);
	LOGERROR("[CfeGT4] {}", msg_str);

	Notification n = {};
	n.exch = _State.EXCHNAME;
	n.source = _State.SOURCE;
	n.org = _State.ORG;
	n.level = MsgLevel::ERROR;
	n.imsg = MsgType::ALERT;
	n.text = msg_str;
	NotifierRest::instance().Notify(n);
}

void CfeGT4::onRiskResetAck(byte_ptr buf, int iMsgLen, int segid)
{
	auto msg = reinterpret_cast<ResetRiskAckMsg::BlockData *>(buf);

	ostringstream oss;
	oss << "[INFO] onRiskReset: seqnum=" << msg->SeqNum
		<< "|riskresetid=" << msg->RiskStatusId
		<< "|riskresetresult=" << msg->RiskResetResult;

		cout << oss.str() << endl;

		//if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

}

void CfeGT4::onTradeCancelCorrect(byte_ptr buf, int iMsgLen, int segid)
{
	auto msg = reinterpret_cast<TradeCancelCorrectMsg::BlockData *>(buf);

	ostringstream oss;
	oss << "[INFO] onTradeCancelCorrect: seqnum=" << msg->SeqNum
		<< "|transtime=" << msg->TransactTime
		<< "|clordid=" << msg->ClOrdID
		<< "|execrefid=" << msg->ExecRefId
		<< "|side=" << msg->Side
		<< "|baseliq=" << msg->BaseLiquidityIndicator
		<< "|clearfirm=" << msg->ClearingFirm
		<< "|clearacct=" << msg->ClearingAccount
		<< "|lastshares=" << msg->LastShares
		<< "|lastpx=" << msg->LastPx
		<< "|correctedpx=" << msg->CorrectedPrice
		<< "|origtime=" << msg->OrigTime
		<< "|symbol=" << msg->Symbol
		<< "|capacity=" << msg->Capacity
		<< "|matdate=" << msg->MaturityDate
		<< "|openclose=" << msg->OpenClose
		<< "|cmta=" << msg->CMTANumber;

	cout << oss.str() << endl;

	//if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;
}

void CfeGT4::onTASRestatement(byte_ptr buf, int iMsgLen, int segid)
{
	auto msg = reinterpret_cast<TASRestatementMsg::BlockData *>(buf);

	//int8_t cbid = _clordid_to_callback.find(msg->ClOrdID);
	uint64_t orderreqid = Base63Codec::decode(msg->ClOrdID);
	bool possretransmit = false;

	if (UNLIKELY(_CX.sessmgr.ReplayComplete() == false))
	{
		ostringstream oss;
		oss << "onTASRestatement: PossRetransFlag = TRUE. SEQNUM=" << msg->SeqNum << ". This may be a retransmission. Not Incementing Sequences.";
		LogMe(oss.str());
	}

	if (UNLIKELY(_State.logToScreen))
		cout << Boe3::MessagePrinter::generateJsonString(*msg) << "\n---------------" << endl;

	KTN::OrderPod *ord = new OrderPod();

	ord->orderReqId = orderreqid;

	//ord->callbackid = cbid;
	ord->timestamp = msg->TransactTime;
	Base63Codec::decode(msg->SecId);

	ord->lastpx = CFEConverter::FromWire(msg->LastPx);
	ord->lastqty = msg->LastShares;
	OrderUtils::toChar(ord->execid, msg->ExecId, sizeof(ord->execid));
	OrderUtils::fastCopy(ord->ordernum, msg->ClOrdID, sizeof(ord->ordernum));
	// ord->exchordid = msg->OrderID;

	switch (msg->MultilegReportingType)
	{
	case EnumMultilegReportType::OUTRIGHT:
		ord->OrdFillType = KOrderFillType::OUTRIGHT_FILL;
		break;
	case EnumMultilegReportType::SPREAD_LEG:
		ord->OrdFillType = KOrderFillType::SPREAD_LEG_FILL;
		break;
	case EnumMultilegReportType::SPREAD:
		ord->OrdFillType = KOrderFillType::SPREAD_FILL;
		break;
	default:
		ord->OrdFillType = KOrderFillType::OUTRIGHT_FILL;
		break;
	}

	ord->OrdSide = (msg->Side == EnumOrderSide::BUY) ? KOrderSide::BUY : KOrderSide::SELL;
	ord->possRetransmit = possretransmit;

	LogMe("[OK] onTASRestatement: Secid=" + std::string(msg->SecId, 6) + " LastPx=" + std::to_string(msg->LastPx) + " LastShares=" + std::to_string(msg->LastShares) + " ClOrdID=" + std::string(msg->ClOrdID));

	ord->tradeDate = msg->TradeDate;
	// unsure if we push this
	_qOrdsProc.push(*ord);
}

void CfeGT4::Display()
{

	if (_State.MEASURE)
		Banner("MEASURING LATENCY!!!!");
}

void CfeGT4::Banner(string msg)
{
	for (int i = 0; i < 3; i++)
		LogMe("*********************************************");
	// for (int i = 0; i < 5; i++)
	LogMe("*******       " + msg + "      *********");
	for (int i = 0; i < 3; i++)
		LogMe("*********************************************");
}
