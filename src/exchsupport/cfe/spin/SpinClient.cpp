//============================================================================
// Name        	: SpinClient.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Dec 10, 2024 10:45:48 AM
//============================================================================

#include <exchsupport/cfe/spin/SpinClient.hpp>
#include <exchsupport/cfe/msgproc/CfeMsgProc.hpp>
#include "SpinClient.hpp"


using namespace KTN::CFE::Pitch;
using namespace KTN::CFE::Spin;
using namespace std;

SpinClient::SpinClient(SpinCallback* callback) : consumerThread([this] { this->ConsumerFunction(); }),
			_spinCallback(*callback),
			_SEQA(0), _msgcount(0),_SPIN_END_SEQ(0), _SPIN_START_SEQ(0),
			_SPIN_ORDERS_PROCESSED(0), _SPIN_ORDERS_COUNT(0),
			_spinImageCnt(0), _Connected(false)
	
{
	_secmaster = SECMASTER_CFE;
	_settings.Init();
	
	LoadDebugSettings();

	KT01_LOG_OK(__CLASS__, "Spin Client Created");
	_spinthread = std::thread(&SpinClient::SpinThread, this);

	//create the consumer thread
	int cpu_core = sched_getcpu();
	if (cpu_core == -1) {
		KT01_LOG_ERROR(__CLASS__, "Error in getting CPU core.");
	} else {
	   KT01_LOG_WARN(__CLASS__, "Spin Client Running on CPU core: " + std::to_string(cpu_core));
	}
	consumerThread.CreateAndRun("spin-consumer", cpu_core);
}


SpinClient::~SpinClient() {
}

void SpinClient::LogMe(std::string szMsg)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void SpinClient::LoadDebugSettings()
{
	Settings dbgsett;
	dbgsett.Load("DebugSettings.txt","DBG");

	//dbg.DebugSecid = dbgsett.getInteger("Debug.Secid");
	dbg.DebugSpin = dbgsett.getBoolean("Debug.Spin");
	dbg.DebugSpinBooks = dbgsett.getBoolean("Debug.SpinBooks");
	dbg.DebugSpinImage = dbgsett.getBoolean("Debug.SpinImage");
	dbg.DebugMbo = dbgsett.getBoolean("Debug.Mbo");
	dbg.DebugSymbol = dbgsett.getString("Debug.Symbol");
	dbg.DebugSpreadSymbol = dbgsett.getString("Debug.SpreadSymbol");
	dbg.DebugCertMode = dbgsett.getBoolean("Debug.CertMode");
	dbg.DebugInstDefs = dbgsett.getBoolean("Debug.InstDefs");

	dbg.DebugSpin = dbgsett.getBoolean("Debug.Spin");

	KT01_LOG_WARN(__CLASS__, "DEBUG SPIN=" + std::to_string(dbg.DebugSpin));
	KT01_LOG_WARN(__CLASS__, "DEBUG SYMBOL=" + dbg.DebugSymbol);
	KT01_LOG_WARN(__CLASS__, "DEBUG SPREAD SYMBOL=" + dbg.DebugSpreadSymbol);
	KT01_LOG_WARN(__CLASS__, "DEBUG CERT MODE=" + std::to_string(dbg.DebugSpin));
	KT01_LOG_WARN(__CLASS__, "DEBUG MBP=" + std::to_string(dbg.DebugMbp));
	KT01_LOG_WARN(__CLASS__, "DEBUG MBO=" + std::to_string(dbg.DebugMbo));
	KT01_LOG_WARN(__CLASS__, "DEBUG INST DEFS=" + std::to_string(dbg.DebugInstDefs));

	auto def = _secmaster->getSecDef(dbg.DebugSymbol);
	dbg.DebugSecid = def.secid;

	def = _secmaster->getSecDef(dbg.DebugSpreadSymbol);
	dbg.DebugSpreadSecid = def.secid;

	if (def.secid > 0)
		KT01_LOG_WARN(__CLASS__, "DBG SYMBOL=" + dbg.DebugSymbol + " SECID=" + std::to_string(def.secid));
	else
		KT01_LOG_ERROR(__CLASS__,"!!! NO DEBUG SECID FOUND FOR DEBUG SYMBOL=" + dbg.DebugSymbol);
}

void SpinClient::Start()
{
	if (!_connConfig.loaded)
	{
		Settings settings;
		settings.Load("DataSettings.txt", "DataSettings");
		string dir = settings.getString("Cfe.ConfigDir");
		if (!dir.empty() && dir.back() != '/')
			dir += '/';

		string spinfile = settings.getString("Cfe.SpinConfigFile");
		string datacenter = settings.getString("Cfe.SpinDataCenter");
		string spin_unit = settings.getString("Cfe.SpinUnit");
		_connConfig.interfaceName = settings.getString("Cfe.SpinIntfName");
		_connConfig.interfaceIp = NetHelpers::getIPAddress(_connConfig.interfaceName);

		string env_datacenter = settings.getString("Cfe.DataCenter");
		_DATA_CENTER = env_datacenter;

		CfeSpinConfig cfg;
		cfg.Load(dir + spinfile);

		SpinSettings st = cfg.GetChannel(datacenter, spin_unit);
		_connConfig.ip = st.ip;
		_connConfig.port = st.port;
		_connConfig.username = st.username;
		_connConfig.password = st.password;
		_connConfig.subid = st.subid;
		_connConfig.loaded = true;
	}

	Connect();
}

bool SpinClient::Connect()
{
	if (_Connected.load(std::memory_order_relaxed))
	{
		LOGINFO("[SpinClient] Already connected to spin server");
		return true;
	}

	if (!_connConfig.loaded)
	{
		LOGERROR("[SpinClient] Connection config not loaded, cannot connect to spin server");
		return false;
	}

	//Instantiate the TCP handler
	
	LOGINFO("Tcp Factory Instanatiation: TYPE=1 (static choice); Local Interface={} Local IP={}", _connConfig.interfaceName, _connConfig.interfaceIp);

	// if (_tcpHandler != nullptr)
	// {
	// 	LOGWARN("[SpinClient] TCP handler already exists, stopping existing handler before creating new one");
	// 	_tcpHandler->Stop();
	// 	delete _tcpHandler;
	// 	_tcpHandler = nullptr;
	// }

	_tcpHandler = KT01::NET::TCP::TcpHandlerFactory::Create(this);
	_tcpRemainder.clear();
	std::pair<byte_ptr, int> stale;
	while (_qspin.try_pop(stale))
		delete[] stale.first;

	// if (_tcpHandler == nullptr)
	// {
	// 	_tcpHandler = KT01::NET::TCP::TcpHandlerFactory::Create(this);
	// }

	ServiceDescriptor svc;
	svc.address = _connConfig.ip;
	svc.port = _connConfig.port;
	svc.iSource = 0;
	svc.index = 0;
	svc.segid = 0;
	
	bool res = false;
	res = _tcpHandler->Init(svc, true);

	if (res)
	{
		_SEQ_OUT = 0;
		SendLogin(_connConfig.username, _connConfig.password, _connConfig.subid);
		LOGINFO("[SpinClient] OK CFE SPIN TCP CONNECTED TO {}:{}", _connConfig.ip, _connConfig.port);
		_Connected = true;
		return true;
	}
	else
	{
		LOGERROR("[SpinClient] CFE SPIN TCP NOT CONNECTED TO {}:{}", _connConfig.ip, _connConfig.port);
		return false;
	}
}

void SpinClient::Disconnect()
{
	if (!_Connected.load(std::memory_order_relaxed))
	{
		LOGINFO("[SpinClient] Already disconnected from spin server");
		return;
	}

	LOGINFO("[SpinClient] Disconnecting from spin server");
	_Connected = false;

	if (_tcpHandler != nullptr)
	{
		_tcpHandler->Stop();
		// delete _tcpHandler;
		// _tcpHandler = nullptr;
	}

	_SPINNING = false;
	_CURRENT_SPIN_SEQ = 0;

	LOGINFO("[SpinClient] Disconnected from spin server");
}

void SpinClient::Stop()
{
	Disconnect();

	_bActive = false;
	_SPIN_THREAD_ACTIVE = false;
}

void SpinClient::Send(char* buffer, size_t len)
{
	//cout << "[DEBUG] SpinClient::Send: LEN=" << len << endl;
	//NetHelpers::hexdump(buffer, len);
	//_qsend.push(std::make_pair(buffer, len));
	int resbytes = _tcpHandler->SendNormal(buffer, len);
	//cout << "[DEBUG] SPIN LOGIN SEND RESULT=" << resbytes << endl;
}

void SpinClient::SendLogin(string username, string password, string subid)
{
	//Login to the spin server
	//NOTE: on the login, they are expecting us to send the seqin (Unit Sequence). Ok. 
	// after connect() + handle_einprogress + SO_ERROR checks...
	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	LoginMsg msg;
	msg.Construct(username, password, subid);
	msg.MsgType = 0x01;
	msg.hdr.HdrSequence = 1;
	_SEQ_OUT = 1;

	Send(reinterpret_cast<char *>(&msg), sizeof(LoginMsg));
}

void SpinClient::SendHeartbeat()
{
	SequencedUnitHeader msg;
	msg.HdrCount = 0;
	msg.HdrLength = sizeof(SequencedUnitHeader);
	msg.HdrUnit = 0;
	msg.HdrSequence = _SEQ_OUT;

	/*int isent =*/ _tcpHandler->SendNormal(reinterpret_cast<char *>(&msg), sizeof(SequencedUnitHeader));
}

void SpinClient::SendSpinRequest(uint32_t seq)
{
	_SPIN_ORDERS_COUNT = 0;
	_SPIN_ORDERS_PROCESSED = 0;

	//send a spin request

	SpinRequestMsg msg;
	msg.Construct(seq);
	msg.hdr.HdrSequence = ++_SEQ_OUT;

	//NetHelpers::hexdump(buffer, sizeof(SpinRequestMsg));

	/*int isent =*/ _tcpHandler->SendNormal(reinterpret_cast<char *>(&msg), sizeof(SpinRequestMsg));
	//cout << "[DEBUG] SPIN REQUEST SEND RESULT=" << isent << endl;
}

void SpinClient::SpinThread()
{
	pthread_t current_thread = pthread_self();
	const int irc = pthread_setname_np(current_thread, "exh-spinthread");
	if (irc != 0)
	{
		LogMe("WARNING: Failed to set thread name, continuing anyway");
	}

	bool gotconection = false;
	LogMe("OK: SPIN POLLING THREAD STARTING");
	_SPIN_THREAD_ACTIVE = true;

	auto last_send_time = std::chrono::steady_clock::now();
	
	while (_SPIN_THREAD_ACTIVE)
	{
		if (_Connected.load(std::memory_order_relaxed) == false)
		{
			usleep(1000);
			continue;
		}
		if (!gotconection)
		{
			gotconection = true;
			LogMe("OK: SPIN POLLING THREAD CONNECTED");
		}

		if (!_qsend.empty())
		{
			std::pair<char*, int> msg;
			while (_qsend.try_pop(msg))
			{
				int isent = _tcpHandler->SendNormal(msg.first, msg.second);
				if (dbg.DebugSpin)
					LogMe("[SPIN DEBUG] SEND RESULT=" + std::to_string(isent));
			}
		}
		else
		{
			// Every 1 second, send a hb
			auto now = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::seconds>(now - last_send_time).count() >= 1)
			{
				if (_tcpHandler != nullptr &&_tcpHandler->Fd() > 0)
				{
					SendHeartbeat();
				}
				last_send_time = now;
			}
		}

		if (_tcpHandler != nullptr && _tcpHandler->Fd() > 0)
		{
			_tcpHandler->Poll();
		}
	}

	KT01_LOG_HIGHLIGHT(__CLASS__, "SPIN POLLING THREAD EXITING");
}

uint16_t SpinClient::onRecv(uint8_t* data, int len, int segid, int index)
{
	//note- this is to prevent memory reuse/shared ownership
	byte_ptr copy = new uint8_t[len];
	std::memcpy(copy, data, len);
	_qspin.push(std::make_pair(copy, len));
	LOGINFO("[SpinClient] Received message from spin server: LEN={} SEQ={}", len, _SEQA.load());

	return len;
}

void SpinClient::ConsumerFunction()
{
	_SPIN_THREAD_ACTIVE = true;
	while (_SPIN_THREAD_ACTIVE)
	{
		if (_qspin.empty())
		{
			if (!_Connected.load(std::memory_order_relaxed))
			{
				std::this_thread::sleep_for(std::chrono::microseconds(10));
			}
			continue;
		}

		std::pair<byte_ptr, int> msg;
		while (_qspin.try_pop(msg))
		{
			const int len = msg.second;
			std::vector<uint8_t> payload;

			// Combine remainder with new data if any
			if (!_tcpRemainder.empty()) {
				payload.insert(payload.end(), _tcpRemainder.begin(), _tcpRemainder.end());
				payload.insert(payload.end(), msg.first, msg.first + len);
			} else {
				payload.assign(msg.first, msg.first + len);
			}

			int consumed = ProcessBuffer(payload.data(), payload.size());

			if (dbg.DebugSpin)
			{
				LogMe("[SPIN WARNING] SPIN CLIENT ON RECV: LEN=" + std::to_string(len) + " CONSUMED=" + std::to_string(consumed) + " DIFF=" + std::to_string(len-consumed));
			}

			// store leftover
			if (consumed < (int)payload.size()) {
				_tcpRemainder.assign(payload.begin() + consumed, payload.end());
			} else {
				_tcpRemainder.clear();
			}

			delete[] msg.first; // Free the memory allocated for the message
		}
	}

	// KT01_LOG_ERROR(__CLASS__, "CONSUMER POLLING THREAD EXITING");
	// std::exit(1);
}

void SpinClient::MsgHandler(byte_ptr data, int len)//, uint64_t recvTime )
{

	auto hdr = reinterpret_cast<SequencedUnitHeader *>(data);

	//cout << "[WARNING] ********* BEGINNING OF CYCLE ***************" << endl;

	// cout << "[MsgHandler DEBUG] MSG LEN=" << len << " SEQ=" << _SEQA  << " HDR SEQ=" << hdr->HdrSequence
	// 	<< " UNIT=" << (int)hdr->HdrUnit << " COUNT=" << (int)hdr->HdrCount << endl;
	

	if (hdr->HdrCount == 0)
	{
		LogMe("[MsgHandler DEBUG] Got Heartbeat. Sending HB back");
		SendHeartbeat();
	}
	
	//if we get here, we are processing normally. I'm separating this so we can efficiently
	//process buffers once we are released from sping
	ProcessBuffer(data, len);

	//cout << "[WARNING] **************  END OF CYCLE *********************" << endl;
}

int SpinClient::ProcessBuffer(byte_ptr orig_data, int len)
{
	byte_ptr data = new uint8_t[len];
	std::memcpy(data, orig_data, len);

	int bytesread = 0;
	int totalbytes = 0;
	int frames = 0;

	// cout << "-----------------------------------------------------" << endl;
	// cout << "[OK Process Buffer] INCOMING LEN=" << len << endl;

	while (totalbytes < len)
	{
		auto hdr = reinterpret_cast<SequencedUnitHeader *>(data);

		if (hdr->HdrCount == 0)
		{
			LogMe("[MsgHandler DEBUG] Got Heartbeat. Sending HB back");
			SendHeartbeat();
		}

		// if (hdr->HdrCount == 0)
		// 	return;
		frames ++;

		// cout << "[WARNING] ----------------------------------------------" << endl;
		// cout << "[WARNING] FRAME " << frames << " INCOMING LEN=" << len << " HDR LEN=" 
		// << hdr->HdrLength << " CNT=" << (int)hdr->HdrCount  << " TOTAL=" << totalbytes << "/" << len << endl;
		
		if (totalbytes + hdr->HdrLength > len)
		{
			// ostringstream oss;
			// oss << "ProcessBuffer: PACKET LEN=" << len << " vs  TOTAL READ + THIS PACKET LEN="
			// << totalbytes + hdr->HdrLength << " --> EXCEEDS REMAINING: " << len - totalbytes ;
			// KT01_LOG_WARN(__CLASS__, oss.str());

			//sends back
			return totalbytes;
		}

		bytesread = sizeof(SequencedUnitHeader);
		data += sizeof(SequencedUnitHeader);

		int ilooper = 0;
		
		while (bytesread < hdr->HdrLength)
		{
			auto msghdr = reinterpret_cast<KTN::CFE::Pitch::MsgHeader*>(data);
			
			int iMsgLen = msghdr->Length;
			int iTemplateid = msghdr->MsgType;// static_cast<int>(msghdr->MsgType);

			msgcnt();

			if (iMsgLen == 0)
			{
				LogMe("[SPIN ERROR] MSG LEN=0");
				std::exit(1);
			}

			int res = ProcessMessage(iTemplateid, data, iMsgLen);

			bytesread += res;
			ilooper ++;

			if (dbg.DebugSpin)
			{
				LogMe(" --> SEQ=" + std::to_string(_SEQA) + " LOOP=" + std::to_string(ilooper) + "/" + std::to_string((int)hdr->HdrCount) + " RES=" + std::to_string(res) + " MSG LEN=" + std::to_string(iMsgLen) + " BYTES=" + std::to_string(bytesread) + "/" + std::to_string(hdr->HdrLength));
			}

			if (res != iMsgLen)
			{
				KT01_LOG_WARN(__CLASS__,"Message length=" + std::to_string(iMsgLen) + " != bytes read=" + std::to_string(res));
				res = iMsgLen;
			}

			//move the buffer pointer to the next message
			data += res;


			if (ilooper >= hdr->HdrCount)
				break;
		}

		totalbytes += bytesread;
	}

	// cout << "[OK] SPIN MSG HANDLER: TOTAL BYTES PROCESSED=" << totalbytes << endl;
	// cout << "-----------------------------------------------------" << endl;

	return totalbytes;
}

int SpinClient::ProcessMessage(int iTemplateid, byte_ptr buf, int len)
{
	int bytesread = 0;

	switch (iTemplateid)
	{
	case Spin::MsgType::EnumSpinLoginResponse:
		bytesread += SpinClient::ProcessLoginResponseMsg(buf, len);
		break;
	case Spin::MsgType::EnumSpinImageAvailable:
		bytesread += SpinClient::ProcessSpinImageAvailableMsg(buf, len);
		break;
	case Spin::MsgType::EnumSpinResponse:
		bytesread += SpinClient::ProcessSpinResponseMsg(buf, len);
		break;
	case Spin::MsgType::EnumSpinFinished:
		bytesread += SpinClient::ProcessSpinFinishedMsg(buf, len);
		break;
	
	case Pitch::MsgType::EnumTimeMsg:
		bytesread += SpinClient::ProcessTimeMsg(buf, len);
		break;
	case Pitch::MsgType::EnumAddOrderShortMsg:
		bytesread += SpinClient::ProcessAddOrderShortMsg(buf, len);
		break;

	case Pitch::MsgType::EnumTradingStatusMsg:
		bytesread += SpinClient::ProcessTradingStatusMsg(buf, len);
		break;

	case Pitch::MsgType::EnumTimeReferenceMsg:
		bytesread += SpinClient::ProcessTimeReferenceMsg(buf, len);
		break;

	case Pitch::MsgType::EnumSettlementMsg:
		bytesread += SpinClient::ProcessSettlementMsg(buf, len);
		break;

	case Pitch::MsgType::EnumFuturesInstrumentDefinitionMsg:
		bytesread += len;//SpinClient::ProcessFuturesInstrumentDefinitionMsg(buf, len);
		break;

	case Pitch::MsgType::EnumPriceLimitsMsg:
		bytesread += SpinClient::ProcessPriceLimitsMsg(buf, len);
		break;

	case Pitch::MsgType::EnumFuturesVarianceSymbolMappingMsg:
		bytesread += 40;
		break;
	case 0x00:
		bytesread += len;
		break;
	default:
		bytesread += len;
		ostringstream oss;
		oss << "[ERROR] Unknown message type: 0x" << std::hex << iTemplateid;
		KT01_LOG_ERROR("CFE_MSGPROC", oss.str());
		break;
	}

	return bytesread;
}

//spin messsages
int SpinClient::ProcessLoginResponseMsg(byte_ptr data, size_t len) {
	const LoginResponseMsg* msg = reinterpret_cast<const LoginResponseMsg*>(data);
	if (dbg.DebugSpin) {
		LogMe("[LoginResponseMsg DEBUG] GOT " + std::to_string(len) + " bytes");
	}

	LogMe("[LoginResponseMsg DEBUG] Length=" + std::to_string((int)msg->Length) + " MsgType=" + std::to_string((int)msg->MsgType) + " Status=" + std::to_string(msg->Status));

	if (msg->Status == 'A')// Spin::EnumLoginStatus::EnumLoginAccepted)
		LogMe("SPIN LOGIN RESPONSE: " + std::string(1, msg->Status));// toString(msg->Status));
	else
		KT01_LOG_ERROR(__CLASS__, "SPIN LOGIN RESPONSE: " + std::string(1, msg->Status));// toString(msg->Status));


	return sizeof(LoginResponseMsg);
}

int SpinClient::ProcessSpinImageAvailableMsg(byte_ptr data, size_t len) {
	const SpinImageAvailableMsg* msg = reinterpret_cast<const SpinImageAvailableMsg*>(data);
	if (dbg.DebugSpin) {
		LogMe("[SpinImageAvailableMsg DEBUG] Sequence=" + std::to_string(msg->Sequence));
	}
	_spinImageCnt ++;
	_CURRENT_SPIN_SEQ = msg->Sequence;

	_spinCallback.onSpinImageAvailable(msg->Sequence);

	if (dbg.DebugSpinImage)
	{
		if (_spinImageCnt == _spinDbgImageCnt)
		{
			LogMe("DEBUG SENDING SPIN REQUEST AFTER " + std::to_string(_spinImageCnt) + " IMAGES. SEQ=" + std::to_string(msg->Sequence));
			SendSpinRequest(msg->Sequence);	
		}
	}

	return sizeof(SpinImageAvailableMsg);
}

int SpinClient::ProcessSpinResponseMsg(byte_ptr data, size_t len) {
	const SpinResponseMsg* msg = reinterpret_cast<const SpinResponseMsg*>(data);
	if (dbg.DebugSpin) {
		LogMe("[SpinResponseMsg DEBUG] OK SPIN RESPONSE RECEIVED FOR Sequence=" + std::to_string(msg->Sequence) + " Count=" + std::to_string(msg->OrderCount) + "  Len=" + std::to_string(len) + " Sizeof=" + std::to_string(sizeof(SpinResponseMsg)));
	}

	_SPINNING = false;
	string status = toString(msg->Status);
	if (msg->Status != Spin::EnumSpinStatus::EnumSpinAccepted)
	{
		KT01_LOG_ERROR(__CLASS__, "SPIN REJECTED: " + status);
		return sizeof(SpinResponseMsg);
	}

	_lob.clear();
	_lob_temp.clear();

	_SPIN_ORDERS_COUNT = msg->OrderCount;
	_SPIN_START_SEQ = msg->Sequence - msg->OrderCount + 1;
	_SPIN_END_SEQ = msg->Sequence;

	//SEQ A can be set here
	_SEQA = _SPIN_START_SEQ;

	
	LogMe("OK! SPIN ACCEPTED: " + status + ". SPNNING=TRUE");
	_SPINNING = true;
	_spinCallback.onSpinStart(_SPIN_START_SEQ, _SPIN_END_SEQ, msg->OrderCount);

	LogMe("SPIN RESPONSE: SEQ=" + std::to_string(msg->Sequence) + " COUNT=" + std::to_string(_SPIN_ORDERS_COUNT));
	LogMe("SPIN RESPONSE: START SEQ=" + std::to_string(_SPIN_START_SEQ) + " END SEQ=" + std::to_string(_SPIN_END_SEQ));	

	return sizeof(SpinResponseMsg);
}

int SpinClient::ProcessSpinFinishedMsg(byte_ptr data, size_t len) {
	const SpinFinishedMsg* msg = reinterpret_cast<const SpinFinishedMsg*>(data);
	if (dbg.DebugSpin) {
		LogMe("[SpinFinishedMsg DEBUG] Sequence=" + std::to_string(msg->Sequence));
	}

	LogMe("SPIN FINISHED: NUMBER ADD_ORDER PROCESSED=" + std::to_string(_SPIN_ORDERS_PROCESSED) + " EXPECTED=" + std::to_string(_SPIN_ORDERS_COUNT));

	// for (auto kv : _lob_temp)
	// {
	// 	auto lob = kv.second;

	// 	int secid = Base63Codec::decode(kv.first.c_str());
	// 	string symb = _secmaster->getSymbol(secid);
	// 	cout << "[DEBUG] LOB CFE BASE63=" << kv.first << " secid=" << secid << " SYMB=" << symb << " BIDS=" << kv.second.count(true)
	// 	<< " ASKS=" << kv.second.count(false) << endl;
	// }

	for (auto kv : _lob_temp)
	{
		auto lob = kv.second;
		int secid = Base63Codec::decode(kv.first.c_str());
		string symb = _secmaster->getSymbol(secid);
		if (symb == dbg.DebugSymbol)
		{
			LogMe("[DEBUG] LOB CFE BASE63=" + kv.first + " secid=" + std::to_string(secid) + " SYMB=" + symb + " BIDS=" + std::to_string(kv.second.count(true)) + " ASKS=" + std::to_string(kv.second.count(false)));

			kv.second.print_order_book();
		}	
	}

	_SPINNING = false;
	_spinCallback.onSpinEnd(msg->Sequence, _SPIN_ORDERS_PROCESSED);

	LogMe("POKING BOOKS CALLBACK: BOOK POPULATED=" + std::to_string(_lob_temp.size()));
	std::vector<LimitOrderBook> books;
	books.reserve(_lob_temp.size());

	for (const auto& pair : _lob_temp) {
		books.push_back(pair.second);
	}

	_spinCallback.onBooksAvailable(books);

	// if (dbg.DebugInstDefs)
	// {
	// 	string filename = (_DATA_CENTER == "CERT") ?  "cfe-cert.csv" : "cfe-prod.csv";
	// 	LogMe("DATA CENTER=" + _DATA_CENTER + " FILENAME=" + filename);
		
	// 	LogMe("SAVING SECDEF TO FILE " + filename + " COUNT=" + std::to_string(_secdef_map.size()) + " SECDEFS");
		
	// 	//SecId63,Symbol,Product,ExpireDate,ContractDate,Description,TickSize,ContractSize,TestSymbol,LegSymbol,LegRatio,LegSide,Complex,secid,legsecid
	// 	ofstream outfile(filename, ios::out | ios::trunc);

	// 	// Write header
	// 	outfile << "SecId63,Symbol,Product,ExpireDate,ContractDate,Description,TickSize,ContractSize,TestSymbol,LegSymbol,LegRatio,LegSide,Complex,secid,legsecid" << endl;

	// 	for (const auto& kv : _secdef_map)
	// 	{
	// 		auto def = kv.second;
	// 		string complex = (def.prodtype == KOrderProdType::SPREAD) ? "True" : "False";

	// 		string ticksize = "nickel";
	// 		if (def.mintick == 1)
	// 			ticksize = "penny";
	// 		else if (def.mintick == 5)
	// 			ticksize = "nickel";
	// 		else if ((def.mintick == 1) && (def.prodtype = KOrderProdType::SPREAD))
	// 			ticksize = "cmplxpenny";


	// 		outfile << def.b63_secid << "," 
	// 		<< def.symbol << "," 
	// 		<< def.product << "," 
	// 		<< 0 << "," 
	// 		<< 0 << "," 
	// 		<< "CERT VX DEF"<< ","
	// 		<< ticksize << ","
	// 		<< def.contract_size << ","
	// 		<< "False" << ","
	// 		<< "" << ","
	// 		<< 0 << ","
	// 		<< "" << ","
	// 		<< complex << ","
	// 		<< def.secid << ","
	// 		<< 0  << endl;
	// 	}

	// 	outfile.close();
	// }

	// Close the spin process properly
	KT01_LOG_HIGHLIGHT(__CLASS__, "SPIN PROCESS FINISHED - Stopping SpinClient");
	//Stop();

	return sizeof(SpinFinishedMsg);
}



int SpinClient::ProcessTimeMsg(byte_ptr data, size_t len) {
	const TimeMessage* msg = reinterpret_cast<const TimeMessage*>(data);
	if (dbg.DebugSpin) {
		LogMe("[SPIN-TimeMsg DEBUG] Time=" + std::to_string(msg->Time) + " EpochTime=" + std::to_string(msg->EpochTime));
	}
	
	return sizeof(TimeMessage);
}


int SpinClient::ProcessAddOrderShortMsg(byte_ptr data, size_t len) {
	const AddOrderShortMsg* msg = reinterpret_cast<const AddOrderShortMsg*>(data);
	// if (dbg.DebugSpinBooks) {
	// 	std::cout << "[AddOrderShortMsg DEBUG]" << std::endl;
	// }
	_SPIN_ORDERS_PROCESSED ++;

	/*
	uint64_t	OrderId;
	char		Side;
	uint16_t	Quantity;
	char		Symbol[6];
//	char		Symbol[6];
	int16_t		Price;
	*/
	

	
	_SEQA ++;
	
	int secid = Base63Codec::decode(msg->Symbol);
	MdOrder ord = {};
	ord.id = msg->OrderId;
	ord.ns = msg->TimeOffset;
	ord.price = msg->Price;
	ord.quantity = msg->Quantity;
	ord.seqnum = _SEQA;
	ord.base63_secid = std::string(msg->Symbol, 6);
	ord.secid = secid;
	

	bool isbid = (msg->Side == 'B' || msg->Side == 'b');

	// cout << "[DEBUG] SPIN ADD ORDER: SECID=" << secid << " SYMBOL=" << std::string(msg->Symbol, 6)
	// << " SIDE=" << msg->Side << " PRICE=" << msg->Price
	// << " QUANTITY=" << msg->Quantity << endl;

	
	
	_lob[secid].add_order(ord, isbid);
	_lob_temp[ord.base63_secid].add_order(ord, isbid);

	if (dbg.DebugSpin)
	{
		LOGINFO("[SpinClient] [AddOrderShortMsg] DEBUG | SEQ={} | Symbol={} | Side={} | Price={} | Qty={} | OrderID={} | SecID={}",
			_SEQA.load(std::memory_order_relaxed),
			std::string(msg->Symbol, 6),
			msg->Side,
			msg->Price,
			msg->Quantity,
			msg->OrderId,
			secid);
	}

	return sizeof(AddOrderShortMsg);
}


int SpinClient::ProcessTradingStatusMsg(byte_ptr data, size_t len) {
	const TradingStatusMsg* msg = reinterpret_cast<const TradingStatusMsg*>(data);
	if (dbg.DebugSpin)
	{
		LOGINFO("[SPIN-TradingStatusMsg DEBUG] Symbol={} SecID={} Status={}", std::string(msg->Symbol, 6), Base63Codec::decode(msg->Symbol), msg->StatusInd);
		//LogMe("[SPIN-TradingStatusMsg DEBUG]");
	}
	
	const std::string base63_secid = std::string(msg->Symbol, 6);
	_lob_temp[base63_secid].update_trading_status(msg->StatusInd);

	return sizeof(TradingStatusMsg);
}

int SpinClient::ProcessTimeReferenceMsg(byte_ptr data, size_t len) {
	// const TimeReferenceMsg* msg = reinterpret_cast<const TimeReferenceMsg*>(data);
	// if (dbg.DebugSpin) {
	// 	LogMe("[SPIN-TimeReferenceMsg DEBUG]");
	// }

	// CfeMsgProc* cfeMsgProc = static_cast<CfeMsgProc*>(&_spinCallback);
	// // Set the trade date to CfeMsgProc instance if not already set
	// if (cfeMsgProc->GetTradeDate() == 0) {
	// 	cfeMsgProc->SetTradeDate(msg->TradeDate);
	// 	if (dbg.DebugSpin) {
	// 		LogMe("[SPIN-TimeReferenceMsg DEBUG] Set _TRADEDATE=" + std::to_string(msg->TradeDate));
	// 		uint32_t daysSinceEpoch = cfeMsgProc->GetDaysSinceEpoch(msg->TradeDate);
	// 		KT01_LOG_OK(__CLASS__, "Days Since Epoch = " + std::to_string(daysSinceEpoch));
	// 	}
	// }
	// else {
	// 	LogMe("[SPIN-TimeReferenceMsg DEBUG] TradeDate already set to " + std::to_string(cfeMsgProc->GetTradeDate()));
	// }

	return sizeof(TimeReferenceMsg);
}

int SpinClient::ProcessSettlementMsg(byte_ptr data, size_t len) {
	const SettlementMsg* msg = reinterpret_cast<const SettlementMsg*>(data);
	if (dbg.DebugSpin) {
		LogMe("[SPIN-SettlementMsg DEBUG]");
	}

	return sizeof(SettlementMsg);
}


// int SpinClient::ProcessFuturesInstrumentDefinitionMsg(byte_ptr data, size_t len) {
// 	const FuturesInstrumentDefinitionMsg* msg = reinterpret_cast<const FuturesInstrumentDefinitionMsg*>(data);
// 	if (dbg.DebugInstDefs) {
// 		LogMe("[SPIN-FuturesInstrumentDefinitionMsg DEBUG] LegCount=" + std::to_string((int)msg->LegCount) + " LegOffset=" + std::to_string((int)msg->LegOffset));
// 	}

// 	int bytesread = sizeof(FuturesInstrumentDefinitionMsg);

// 	KT01::SECDEF::CFE::CfeSecDef def;
// 	def.b63_secid = std::string(msg->Symbol, 6);
// 	def.secid = Base63Codec::decode(msg->Symbol);

// 	//default here:
// 	def.symbol = KTN::CFE::Helpers::CfeHelpers::StripRightSpaces(std::string(msg->Symbol, 6));

// 	def.contract_size = msg->ContractSize;
// 	def.mintick = msg->PriceIncrement;
// 	def.prodtype = (msg->LegCount == 0) ? KOrderProdType::FUTURE : KOrderProdType::SPREAD;
// 	def.product = "VX";

// 	if (def.prodtype == KOrderProdType::FUTURE)
// 	{
// 		def.symbol = KTN::CFE::Helpers::CfeHelpers::CfeSymbolFromRaw(msg->ReportSymbol, 6, msg->ContractDate);
// 		_secmaster->Add(def);
// 		_secdef_map[def.symbol] = def;
// 	}
	

// 	if (dbg.DebugInstDefs)
// 		LogMe("[SPIN-FuturesInstrumentDefinitionMsg DEBUG] SECID=" + std::to_string(def.secid) + " PRODTYPE=" + KOrderProdType::toString(def.prodtype) + " SYMBOL=" + def.symbol);
	

// 	if (msg->LegCount > 0 && msg->LegOffset == 45)
// 	{
// 		data += sizeof(FuturesInstrumentDefinitionMsg);
// 		vector<std::string> legs;
// 		for (int i = 0; i < msg->LegCount; i++)
// 		{
// 			const FuturesLeg* leg = reinterpret_cast<const FuturesLeg*>(data);
// 			if (dbg.DebugInstDefs)
// 				LogMe("[SPIN-FuturesLeg DEBUG] LegRatio=" + std::to_string(leg->LegRatio) + " LegSymbol=" + std::string(leg->LegSymbol, 6));
			
// 			int32_t secid = Base63Codec::decode(std::string(leg->LegSymbol, 6).c_str());
// 			std::string symb = _secmaster->getSymbol(secid);
// 			legs.push_back(symb);
			
// 			data += sizeof(FuturesLeg);
// 			bytesread += sizeof(FuturesLeg);
// 		}

// 		if (legs.size() == 2)
// 		{
// 			def.symbol = legs[0] + "-" + legs[1];
// 		}
// 		_secmaster->Add(def);
// 		_secdef_map[def.symbol] = def;
// 	}

// 	return bytesread;
// }

int SpinClient::ProcessPriceLimitsMsg(byte_ptr data, size_t len) {
	const PriceLimitsMsg* msg = reinterpret_cast<const PriceLimitsMsg*>(data);
	if (dbg.DebugSpin) {
		LogMe("SPIN-[PriceLimitsMsg DEBUG]");
	}

	return sizeof(PriceLimitsMsg);
}

