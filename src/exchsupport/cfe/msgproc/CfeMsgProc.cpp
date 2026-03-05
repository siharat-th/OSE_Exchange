//============================================================================
// Name        	: CfeMsgProc.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Dec 10, 2024 10:45:48 AM
//============================================================================

#include "CfeMsgProc.hpp"
#include <exchsupport/cfe/spin/SharedSpinClient.hpp>

using namespace KTN::CFE::Pitch;

CfeMsgProc::CfeMsgProc()
{
	_secmaster = SECMASTER_CFE;
	LoadDebugSettings();

	_SEQA = 0;
	_SEQ_FIRST = 0;

	timeval curTime;
	gettimeofday(&curTime, nullptr);
	_TIME = static_cast<uint32_t>(curTime.tv_sec);

	_TRADEDATE = 0;

	_lob2.reserve(16);

	ChangeState(OrderBookState::NoState);

	Spin::SharedSpinClient::instance().Start();
	Spin::SharedSpinClient::instance().Register(this);
}

CfeMsgProc::~CfeMsgProc() {
}

void CfeMsgProc::Subscribe(KTN::AlgoMktDataCallback *subscriber)
{
	callback = subscriber;
}

void CfeMsgProc::AddSymbol(const std::string &symbol, int index, uint32_t secId)
{
	for (size_t i = 0; i < _lob2.size(); i++)
	{
		if (_lob2[i].SecId() == secId)
		{
			KT01_LOG_WARN(__CLASS__, "Symbol " + symbol + " with secId=" + std::to_string(secId) + " already exists in LOB vector at index " + std::to_string(i));
			return;
		}
	}

	_lob2.emplace_back(LimitOrderBook { });
	_lob2.back().init(secId, Base63Codec::encode_to_string(secId), symbol);
}

void CfeMsgProc::LogMe(std::string szMsg)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void CfeMsgProc::LoadDebugSettings()
{
	Settings dbgsett;
	dbgsett.Load("DebugSettings.txt","DBG");

	//dbg.DebugSecid = dbgsett.getInteger("Debug.Secid");
	dbg.DebugSymbol = dbgsett.getString("Debug.Symbol");
	dbg.DebugSpreadSymbol = dbgsett.getString("Debug.SpreadSymbol");
	dbg.DebugCertMode = dbgsett.getBoolean("Debug.CertMode");
	dbg.DebugInstDefs = dbgsett.getBoolean("Debug.InstDefs");
	dbg.DebugBytes = dbgsett.getBoolean("Debug.Bytes");
	dbg.DebugMarketSnapshotMsg = dbgsett.getBoolean("Debug.MarketSnapshotMsg");
	dbg.DebugPriceLimitsMsg = dbgsett.getBoolean("Debug.PriceLimitsMsg");
	LOAD_TYPED_SETTING_OR_DEFAULT(dbgsett, "Debug.OnPacketEnd", getBoolean, dbg.DebugOnPacketEnd, true);

	KT01_LOG_WARN(__CLASS__, "DEBUG SYMBOL=" + dbg.DebugSymbol);
	KT01_LOG_WARN(__CLASS__, "DEBUG SPREAD SYMBOL=" + dbg.DebugSpreadSymbol);
	KT01_LOG_WARN(__CLASS__, "DEBUG CERT MODE=" + std::to_string(dbg.DebugCertMode));
	KT01_LOG_WARN(__CLASS__, "DEBUG INST DEFS=" + std::to_string(dbg.DebugInstDefs));
	KT01_LOG_WARN(__CLASS__, "DEBUG BYTES=" + std::to_string(dbg.DebugBytes));

	auto def = _secmaster->getSecDef(dbg.DebugSymbol);
	dbg.DebugSecid = def.secid;

	def = _secmaster->getSecDef(dbg.DebugSpreadSymbol);
	dbg.DebugSpreadSecid = def.secid;

	if (def.secid > 0)
		KT01_LOG_WARN(__CLASS__, "DBG SYMBOL=" + dbg.DebugSymbol + " SECID=" + std::to_string(def.secid));
	else
		KT01_LOG_ERROR(__CLASS__,"!!! NO DEBUG SECID FOUND FOR DEBUG SYMBOL=" + dbg.DebugSymbol);
}

void CfeMsgProc::Stop()
{

}

void CfeMsgProc::PushBookUpdate(const LimitOrderBook &book, uint16_t index)
{
	DepthBook bk;
	book.to_depthbook(bk);
	bk.Index = index;
	callback->onMktDataBidAsk(bk);
}

void CfeMsgProc::Process(char* buf, int len)
{
	MsgHandler(buf, len);
}

void CfeMsgProc::ProcessSuspenseQueue(uint32_t seqnum)
{
	ChangeState(OrderBookState::Dequeuing);

	ostringstream oss;
	oss << "[ProcessSuspenseQueue DEBUG] PROCESSING SUSPENSE QUEUE: START SEQNUM=" << seqnum;
	LogMe(oss.str());

	while (!_qsuspense.empty())
	{
		std::pair<char*, int> msg;
		if (_qsuspense.try_pop(msg))
		{
			auto hdr = reinterpret_cast<SequencedUnitHeader *>(msg.first);
			LOGINFO("[ProcessSuspenseQueue DEBUG] PROCESSING SUSPENDED SEQ={} COUNT={}", static_cast<uint32_t>(hdr->HdrSequence), static_cast<int>(hdr->HdrCount));
			ProcessBuffer(msg.first, msg.second, true);  // skip_spun_messages = true
			delete[] msg.first;
		}
	}

	LogMe("PROCESS SUSPENSE QUEUE OK: DONE");
	ChangeState(OrderBookState::Current);
}


void CfeMsgProc::MsgHandler(char * buf, int len)//, uint64_t recvTime )
{
	// if (dbg.DebugBytes)
	// 	cout << "[CfeMsgProc DEBUG] MsgHandler Got Bytes: buf=" << (void*)buf << " len=" << len << endl;
	

	auto hdr = reinterpret_cast<SequencedUnitHeader *>(buf);

	// if (dbg.DebugBytes)
	// {
	// 	cout << "[MsgHandler DEBUG] MSG LEN=" << len << " SEQ=" << _SEQA  << " HDR SEQ=" << hdr->HdrSequence
	// 		<< " UNIT=" << (int)hdr->HdrUnit << " COUNT=" << (int)hdr->HdrCount 
	// 		<< " HDR LEN=" << hdr->HdrLength << endl;

	// 	//NetHelpers::hexdump(buf, len);
	// }

	// if (hdr->HdrCount == 0)
	// 	return;

	
	//check sequence but don't advance buffer. Once we process, we'll start advancing and counting bytes
	if (likely(hdr->HdrSequence > 0))
	{
		if (nextExpectedSeq != hdr->HdrSequence)
		{
			LOGWARN("[CfeMsgProc] Expected seq {} but received {}", nextExpectedSeq, static_cast<unsigned int>(hdr->HdrSequence));

			if (hdr->HdrSequence < nextExpectedSeq)
			{
				LOGWARN("[CfeMsgProc] Potential reset detected: expected={} got={}", nextExpectedSeq, static_cast<uint32_t>(hdr->HdrSequence));
			}
			else if (_STATE == OrderBookState::Current)
			{
				LOGWARN("[CfeMsgProc] GAP DETECTED while Current: expected={} got={} (dropped {})",
					nextExpectedSeq,
					static_cast<unsigned int>(hdr->HdrSequence),
					static_cast<unsigned int>(hdr->HdrSequence) - nextExpectedSeq);
				gapSeq = hdr->HdrSequence;
				BeginRecovery();
			}
			else if (_STATE != OrderBookState::NoState)
			{
				gapSeq = hdr->HdrSequence;
				spinRequested = false;
				LOGWARN("[CfeMsgProc] Additional gap during recovery at seq={}, raising _gapSeq", gapSeq.load());
			}
		}
		nextExpectedSeq = hdr->HdrSequence + hdr->HdrCount;
		_SEQA = hdr->HdrSequence;

		if (unlikely(_SEQ_FIRST == 0))
		{
			//this is the first packet we are processing. We can set the first sequence
			_SEQ_FIRST = hdr->HdrSequence;
			LOGWARN("[CfeMsgProc] Setting _SEQ_FIRST to {}", _SEQ_FIRST.load(std::memory_order_relaxed));
		}

		if (unlikely(_SEQ_FIRST > 0 && _STATE == OrderBookState::NoState))
		{
			const uint32_t currentSpinSeq = Spin::SharedSpinClient::instance().CurrentSpinImageSeq();
			if (currentSpinSeq >= _SEQ_FIRST - 1)
			{
				Spin::SharedSpinClient::instance().SendSpinRequest(currentSpinSeq);
				LOGWARN("[CfeMsgProc] SPIN REQUEST SENT FOR SEQ < {}. SPIN IMAGE={}", _SEQ_FIRST.load(std::memory_order_relaxed), currentSpinSeq);
				ChangeState(OrderBookState::SpinRequested);
			}
		}
		
		if (unlikely(_STATE != OrderBookState::Current))
		{
			if (hdr->HdrCount > 0)
			{
				//we need to make copies to avoid pointer reuse:
				char * buf_copy = new char[len];
				std::memcpy(buf_copy, buf, len);

				_qsuspense.push(std::make_pair(buf_copy, len));

				LOGWARN("[CfeMsgProc] SUSPENDING SEQ={} COUNT={} END SEQ={} STATE={} TOTAL SUS={}",
					_SEQA.load(std::memory_order_relaxed),
					(int)hdr->HdrCount,
					(_SEQA + hdr->HdrCount -1),
					toString(GetState()),
					_qsuspense.unsafe_size()
				);

				_SEQA += hdr->HdrCount -1;
				hasSuspendedMessages = true;

				return;
			}
		}
	}

	//if we get here, we are processing normally. I'm separating this so we can efficiently
	//process buffers once we are released from sping
	if (likely(hdr->HdrCount > 0))
	{
		if (likely(hdr->HdrSequence > 0))
		{
			if (unlikely(hasSuspendedMessages))
			{
				// Adding this due to a possible race condition.
				// It may be possible for the spin thread to set state to current after the code above already checked it,
				// potentially leaving suspended messages unprocessed.
				while (!_qsuspense.empty())
				{
					std::pair<char*, int> msg;
					if (_qsuspense.try_pop(msg))
					{
						auto hdr = reinterpret_cast<SequencedUnitHeader *>(msg.first);
						LOGINFO("[MsgHandler DEBUG] PROCESSING SUSPENDED SEQ={} COUNT={}", static_cast<uint32_t>(hdr->HdrSequence), static_cast<int>(hdr->HdrCount));
						ProcessBuffer(msg.first, msg.second, false);
						delete[] msg.first;
					}
				}
				hasSuspendedMessages = false;
			}

			ProcessBuffer(buf, len);
			OnPacketEnd();
		}
		else
		{
			ProcessBuffer(buf, len);
		}
	}
	else if (dirty && likely(hdr->HdrSequence > 0))
	{
		for (size_t i = 0; i < _lob2.size(); i++)
		{
			LimitOrderBook &book = _lob2[i];
			if (book.IsDirty())
			{
				PushBookUpdate(book, static_cast<uint16_t>(i));
				book.ClearDirty();
			}
		}
		dirty = false;
	}
}

void CfeMsgProc::ProcessBuffer(char * buf, int len, bool skip_spun_messages)
{
	int ilooper = 0;
	int bytesread = 0;

	auto hdr = reinterpret_cast<SequencedUnitHeader *>(buf);
	if (hdr->HdrCount == 0) return;

	uint32_t seq = hdr->HdrSequence;  // local tracking

	bytesread += sizeof(SequencedUnitHeader);
	buf += sizeof(SequencedUnitHeader);

	while (bytesread < hdr->HdrLength)
	{
		auto msghdr = reinterpret_cast<KTN::CFE::Pitch::MsgHeader*>(buf);
		int iMsgLen = msghdr->Length;
		int iTemplateid = static_cast<int>(msghdr->MsgType);

		if (unlikely(dbg.DebugBytes))
		{
			ostringstream oss;
			oss << "[ProcessBuffer DEBUG] SEQ=" << _SEQA << " LOOP=" << ilooper
				<< " MSG LEN=" << iMsgLen << " MSG TYPE=" << std::hex << iTemplateid
				<< " BYTES=" << bytesread << "/" << hdr->HdrLength;
			LogMe(oss.str());
		}

		if (skip_spun_messages) {
			ostringstream oss;
			oss << "[ProcessBuffer WARNING] Processing Local SEQ=" << seq << " (skipping spun messages). SEQA=" << _SEQA;
			LogMe(oss.str());
		}

		// Only update _SEQA if this was live feed (not suspense replay)
		if (!skip_spun_messages && ilooper > 0) {
			// Increment _SEQA only after the first loop
			_SEQA++;
		}

		if (unlikely(skip_spun_messages && seq <= _SEQ_SPIN_LAST && GetState() == OrderBookState::Dequeuing)) {
			ostringstream oss;
			oss << "[ProcessBuffer INFO] SKIPPING SEQ=" << seq << " (already in spin). SEQA=" << _SEQA;
			LogMe(oss.str());
		} else {
			ProcessMessage(iTemplateid, buf, iMsgLen);
		}
		
		seq++;

		bytesread += iMsgLen;
		buf += iMsgLen;
		ilooper++;
	}
}


int CfeMsgProc::ProcessMessage(int iTemplateid, char* buf, int len)
{
	int bytesread = 0;

	switch (iTemplateid)
	{
	case MsgType::EnumTimeMsg:
		bytesread += CfeMsgProc::ProcessTimeMsg(buf, len);
		break;
	case MsgType::EnumAddOrderShortMsg:
		bytesread += CfeMsgProc::ProcessAddOrderShortMsg(buf, len);
		break;
	case MsgType::EnumOrderExecutedMsg:
		bytesread += CfeMsgProc::ProcessOrderExecutedMsg(buf, len);
		break;
	case MsgType::EnumReduceSizeShortMsg:
		bytesread += CfeMsgProc::ProcessReduceSizeShortMsg(buf, len);
		break;
	case MsgType::EnumModifyOrderShortMsg:
		bytesread += CfeMsgProc::ProcessModifyOrderShortMsg(buf, len);
		break;
	case MsgType::EnumDeleteOrderMsg:
		bytesread += CfeMsgProc::ProcessDeleteOrderMsg(buf, len);
		break;
	case MsgType::EnumTradeLongMsg:
		bytesread += CfeMsgProc::ProcessTradeLongMsg(buf, len);
		break;
	case MsgType::EnumTradeShortMsg:
		bytesread += CfeMsgProc::ProcessTradeShortMsg(buf, len);
		break;
	case MsgType::EnumTradeBreakMsg:
		bytesread += CfeMsgProc::ProcessTradeBreakMsg(buf, len);
		break;
	case MsgType::EnumEndSessionMsg:
		bytesread += CfeMsgProc::ProcessEndSessionMsg(buf, len);
		break;
	case MsgType::EnumTradingStatusMsg:
		bytesread += CfeMsgProc::ProcessTradingStatusMsg(buf, len);
		break;
	case MsgType::EnumUnitClearMsg:
		bytesread += CfeMsgProc::ProcessUnitClearMsg(buf, len);
		break;
	case MsgType::EnumTimeReferenceMsg:
		bytesread += CfeMsgProc::ProcessTimeReferenceMsg(buf, len);
		break;
	case MsgType::EnumSingleSideUpdateShortMsg:
		bytesread += CfeMsgProc::ProcessSingleSideUpdateShortMsg(buf, len);
		break;
	case MsgType::EnumTwoSideUpdateShortMsg:
		bytesread += CfeMsgProc::ProcessTwoSideUpdateShortMsg(buf, len);
		break;
	case MsgType::EnumTOPTradeMsg:
		bytesread += CfeMsgProc::ProcessTOPTradeMsg(buf, len);
		break;
	case MsgType::EnumSettlementMsg:
		bytesread += CfeMsgProc::ProcessSettlementMsg(buf, len);
		break;
	case MsgType::EnumEODSummaryMsg:
		bytesread += CfeMsgProc::ProcessEODSummaryMsg(buf, len);
		break;
	case MsgType::EnumFuturesInstrumentDefinitionMsg:
		//bytesread += CfeMsgProc::ProcessFuturesInstrumentDefinitionMsg(buf, len);
		bytesread += len;
		break;
	case MsgType::EnumTransactionBeginMsg:
		bytesread += CfeMsgProc::ProcessTransactionBeginMsg(buf, len);
		break;
	case MsgType::EnumTransactionEndMsg:
		bytesread += CfeMsgProc::ProcessTransactionEndMsg(buf, len);
		break;
	case MsgType::EnumPriceLimitsMsg:
		bytesread += CfeMsgProc::ProcessPriceLimitsMsg(buf, len);
		break;
	case MsgType::EnumMarketSnapshotShortMsg:
		bytesread += CfeMsgProc::ProcessMarketSnapshotShortMsg(buf, len);
		break;
	case MsgType::EnumOpenInterestMsg:
		bytesread += len;
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

int CfeMsgProc::ProcessTimeMsg(const char* buf, size_t len) {
	const TimeMessage* msg = reinterpret_cast<const TimeMessage*>(buf);
	if (unlikely(dbg.DebugCertMode))
	{
		LOGINFO("[CfeMsgProc] [TimeMsg DEBUG] Time={} EpochTime={} SEQ={}",
			msg->Time,
			msg->EpochTime,
			_SEQA.load(std::memory_order_relaxed));
	}
	_TIME = msg->EpochTime;
	return sizeof(TimeMessage);
}


int CfeMsgProc::ProcessAddOrderShortMsg(const char* buf, size_t len) {
	const AddOrderShortMsg* msg = reinterpret_cast<const AddOrderShortMsg*>(buf);

	const int secid = Base63Codec::decode(msg->Symbol);

	if (unlikely(dbg.DebugCertMode))
	{
		LOGINFO("[CfeMsgProc] [AddOrderShortMsg] DEBUG | SEQ={} | Symbol={} | Side={} | Price={} | Qty={} | OrderID={} | SecID={}",
			_SEQA.load(std::memory_order_relaxed),
			std::string(msg->Symbol, 6),
			msg->Side,
			msg->Price,
			msg->Quantity,
			msg->OrderId,
			secid);
	}

	for (size_t i = 0; i < _lob2.size(); i++)
	{
		if (_lob2[i].SecId() == secid)
		{
			MdOrder ord = {};
			ord.id = msg->OrderId;
			ord.ns = msg->TimeOffset;
			ord.price = msg->Price;
			ord.quantity = msg->Quantity;
			ord.seqnum = _SEQA;
			ord.base63_secid = std::string(msg->Symbol, 6);
			ord.secid = secid;
			bool isbid = (msg->Side == 'B' || msg->Side == 'b');

			_orderIdToSecId[ord.id] = secid;

			_lob2[i].add_order(ord, isbid);
			dirty = true;
			return sizeof(AddOrderShortMsg);
		}
	}

	return sizeof(AddOrderShortMsg);
}

int CfeMsgProc::ProcessOrderExecutedMsg(const char* buf, size_t len) {
	const OrderExecutedMsg* msg = reinterpret_cast<const OrderExecutedMsg*>(buf); 
	if (unlikely(dbg.DebugCertMode))
	{
		const int secid = _orderIdToSecId[msg->OrderId];
		string symbol = _secmaster->getSymbol(secid);
		int16_t price = std::numeric_limits<int16_t>::max();
		for (size_t i = 0; i < _lob2.size(); i++)
		{
			if (_lob2[i].SecId() == secid)
			{
				auto book = _lob2[i].find_book(msg->OrderId);
				if (book)
				{
					price = (*book)[msg->OrderId].price;
				}
			}
		}
		LOGINFO("[CfeMsgProc] [OrderExecutedMsg DEBUG] SEQ={} | Symbol={} | Price={} | Qty={} | OrderID={}",
			_SEQA.load(std::memory_order_relaxed),
			symbol,
			price,
			msg->ExecQuantity,
			msg->OrderId);
	}

	std::unordered_map<uint64_t, uint32_t>::const_iterator it = _orderIdToSecId.find(msg->OrderId);

	if (it == _orderIdToSecId.end())
	{
		return sizeof(OrderExecutedMsg);
	}

	const int secid = it->second;

	for (size_t i = 0; i < _lob2.size(); i++)
	{
		if (_lob2[i].SecId() == secid)
		{
			_lob2[i].execute_order(msg->OrderId, msg->ExecQuantity);
			dirty = true;
			return sizeof(OrderExecutedMsg);
		}
	}

	return sizeof(OrderExecutedMsg);
}

int CfeMsgProc::ProcessReduceSizeShortMsg(const char* buf, size_t len) {
	const ReduceSizeShortMsg* msg = reinterpret_cast<const ReduceSizeShortMsg*>(buf);
	if (unlikely(dbg.DebugCertMode))
	{
		LOGINFO("[CfeMsgProc] [ReduceSizeShortMsg DEBUG] SEQ={} | OrderID={} | CxlQty={}",
			_SEQA.load(std::memory_order_relaxed),
			msg->OrderId,
			msg->CxlQuantity);
	}
	
	std::unordered_map<uint64_t, uint32_t>::const_iterator it = _orderIdToSecId.find(msg->OrderId);

	if (it == _orderIdToSecId.end())
	{
		return sizeof(ReduceSizeShortMsg);
	}

	const int secid = it->second;

	for (size_t i = 0; i < _lob2.size(); i++)
	{
		if (_lob2[i].SecId() == secid)
		{
			_lob2[i].reduce_order(msg->OrderId, msg->CxlQuantity);
			dirty = true;
			return sizeof(ReduceSizeShortMsg);
		}
	}

	return sizeof(ReduceSizeShortMsg);
}

int CfeMsgProc::ProcessModifyOrderShortMsg(const char* buf, size_t len) {
	const ModifyOrderShortMsg* msg = reinterpret_cast<const ModifyOrderShortMsg*>(buf);
	if (unlikely(dbg.DebugCertMode))
	{
		LOGINFO("[CfeMsgProc] [ModifyOrderShortMsg DEBUG] SEQ={} | OrderID={} | Qty={} | Price={}",
			_SEQA.load(std::memory_order_relaxed),
			msg->OrderId,
			msg->Quantity,
			msg->Price);
	}
	
	std::unordered_map<uint64_t, uint32_t>::const_iterator it = _orderIdToSecId.find(msg->OrderId);

	if (it == _orderIdToSecId.end())
	{
		return sizeof(ModifyOrderShortMsg);
	}

	const int secid = it->second;

	for (size_t i = 0; i < _lob2.size(); i++)
	{
		if (_lob2[i].SecId() == secid)
		{
			MdOrder ord = {};
			ord.id = msg->OrderId;
			ord.ns = msg->TimeOffset;
			ord.quantity = msg->Quantity;
			ord.price = msg->Price;
			ord.seqnum = _SEQA;

			_lob2[i].update_order(ord);
			dirty = true;
			return sizeof(ModifyOrderShortMsg);
		}
	}

	return sizeof(ModifyOrderShortMsg);
}

int CfeMsgProc::ProcessDeleteOrderMsg(const char* buf, size_t len) {
	const DeleteOrderMsg* msg = reinterpret_cast<const DeleteOrderMsg*>(buf);
	if (unlikely(dbg.DebugCertMode))
	{
		LOGINFO("[CfeMsgProc] [DeleteOrderMsg DEBUG] SEQ={} | OrderID={}",
			_SEQA.load(std::memory_order_relaxed),
			msg->OrderId);
	}
	
	std::unordered_map<uint64_t, uint32_t>::const_iterator it = _orderIdToSecId.find(msg->OrderId);
	if (it == _orderIdToSecId.end())
	{
		return sizeof(DeleteOrderMsg);
	}
	const int secid = it->second;

	for (size_t i = 0; i < _lob2.size(); i++)
	{
		if (_lob2[i].SecId() == secid)
		{
			_lob2[i].delete_order(msg->OrderId);
			dirty = true;
			_orderIdToSecId.erase(it);
			return sizeof(DeleteOrderMsg);
		}
	}
	
	return sizeof(DeleteOrderMsg);
}

int CfeMsgProc::ProcessTradeLongMsg(const char* buf, size_t len) {
	const TradeLongMsg* msg = reinterpret_cast<const TradeLongMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		int secid = Base63Codec::decode(msg->Symbol);
		string symbol = _secmaster->getSymbol(secid);
		int64_t price = msg->Price;
		ostringstream oss;
		oss << "[TradeLongMsg DEBUG] SEQ=" << _SEQA << " Symbol=" << symbol << " Price=" << price;
		LogMe(oss.str());
	}
	return sizeof(TradeLongMsg);
}

int CfeMsgProc::ProcessTradeShortMsg(const char* buf, size_t len) {
	const TradeShortMsg* msg = reinterpret_cast<const TradeShortMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		int secid = Base63Codec::decode(msg->Symbol);
		string symbol = _secmaster->getSymbol(secid);
		int16_t price = msg->Price;
		ostringstream oss;
		oss << "[TradeShortMsg DEBUG] SEQ=" << _SEQA << " Symbol=" << symbol << " Price=" << price << " Qty=" << msg->Quantity;
		LogMe(oss.str());
	}

	return sizeof(TradeShortMsg);
}

int CfeMsgProc::ProcessTradeBreakMsg(const char* buf, size_t len) {
	const TradeBreakMsg* msg = reinterpret_cast<const TradeBreakMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		ostringstream oss;
		oss << "[TradeBreakMsg DEBUG] SEQ=" << _SEQA;
		LogMe(oss.str());
	}
	return sizeof(TradeBreakMsg);
}

int CfeMsgProc::ProcessEndSessionMsg(const char* buf, size_t len) {
	const EndSessionMsg* msg = reinterpret_cast<const EndSessionMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		ostringstream oss;
		oss << "[EndSessionMsg DEBUG] SEQ=" << _SEQA;
		LogMe(oss.str());
	}
	return sizeof(EndSessionMsg);
}

int CfeMsgProc::ProcessTradingStatusMsg(const char* buf, size_t len) {
	const TradingStatusMsg* msg = reinterpret_cast<const TradingStatusMsg*>(buf);
	if (unlikely(dbg.DebugCertMode))
	{
		char symbol[7];
		std::memcpy(symbol, msg->Symbol, 6);
		symbol[6] = '\0';
		LOGINFO("[CfeMsgProc] [TradingStatusMsg DEBUG] SEQ={} SYMBOL={} STATUS={}",
			_SEQA.load(std::memory_order_relaxed), symbol, msg->StatusInd);
	}

	// Check for valid _TIME
	if (_TIME == 0) {
		LogMe("WARNING: _TIME not initialized, using current time");
		timeval curTime;
		gettimeofday(&curTime, nullptr);
		_TIME = static_cast<uint32_t>(curTime.tv_sec);
	}

	const int secid = Base63Codec::decode(msg->Symbol);

	for (size_t i = 0; i < _lob2.size(); i++)
	{
		if (_lob2[i].SecId() == secid)
		{
			_lob2[i].update_trading_status(msg->StatusInd);
				callback->onMktDataSecurityStatus(
				Base63Codec::decode(msg->Symbol),
				_lob2[i].SecurityStatus(),
				KT01::DataStructures::MarketDepth::BookSecurityTradingEvent::NoEvent,
				0 // TODO
			);
		}
	}

	return sizeof(TradingStatusMsg);
}

int CfeMsgProc::ProcessUnitClearMsg(const char* buf, size_t len) {
	const UnitClearMsg* msg = reinterpret_cast<const UnitClearMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		ostringstream oss;
		oss << "[UnitClearMsg DEBUG] SEQ=" << _SEQA;
		LogMe(oss.str());
	}
	return sizeof(UnitClearMsg);
}

void CfeMsgProc::onNewTradeDate(uint32_t tradedate) {
	ostringstream oss;
	oss << "onNewTradeDate: " << tradedate << " (previous: " << _TRADEDATE << ")";
	LogMe(oss.str());
	_TRADEDATE = tradedate;
}

int CfeMsgProc::ProcessTimeReferenceMsg(const char* buf, size_t len) {
	const TimeReferenceMsg* msg = reinterpret_cast<const TimeReferenceMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		ostringstream oss;
		oss << "[TimeReferenceMsg DEBUG] SEQ=" << _SEQA;
		LogMe(oss.str());
	}

	if (msg->TradeDate != _TRADEDATE) {
		onNewTradeDate(msg->TradeDate);
	}

	return sizeof(TimeReferenceMsg);
}


int CfeMsgProc::ProcessSingleSideUpdateShortMsg(const char* buf, size_t len) {
	const SingleSideUpdateShortMsg* msg = reinterpret_cast<const SingleSideUpdateShortMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		ostringstream oss;
		oss << "[SingleSideUpdateShortMsg DEBUG] SEQ=" << _SEQA;
		LogMe(oss.str());
	}
	return sizeof(SingleSideUpdateShortMsg);
}


int CfeMsgProc::ProcessTwoSideUpdateShortMsg(const char* buf, size_t len) {
	const TwoSideUpdateShortMsg* msg = reinterpret_cast<const TwoSideUpdateShortMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		ostringstream oss;
		oss << "[TwoSideUpdateShortMsg DEBUG] SEQ=" << _SEQA;
		LogMe(oss.str());
	}
	return sizeof(TwoSideUpdateShortMsg);
}

int CfeMsgProc::ProcessTOPTradeMsg(const char* buf, size_t len) {
	const TOPTradeMsg* msg = reinterpret_cast<const TOPTradeMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		int secid = Base63Codec::decode(msg->Symbol);
		string symbol = _secmaster->getSymbol(secid);
		int16_t price = msg->Price;
		ostringstream oss;
		oss << "[TOPTradeMsg DEBUG] SEQ=" << _SEQA << " Symbol=" << symbol << " Price=" << price << " Qty=" << msg->Quantity;
		LogMe(oss.str());
	}

	return sizeof(TOPTradeMsg);
}

int CfeMsgProc::ProcessSettlementMsg(const char* buf, size_t len) {
	const SettlementMsg* msg = reinterpret_cast<const SettlementMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		ostringstream oss;
		oss << "[SettlementMsg DEBUG] SEQ=" << _SEQA;
		LogMe(oss.str());
	}
	return sizeof(SettlementMsg);
}

int CfeMsgProc::ProcessEODSummaryMsg(const char* buf, size_t len) {
	const EndOfDaySummaryMsg* msg = reinterpret_cast<const EndOfDaySummaryMsg*>(buf);
	if (unlikely(dbg.DebugCertMode)) {
		ostringstream oss;
		oss << "[EODSummaryMsg DEBUG] SEQ=" << _SEQA;
		LogMe(oss.str());
	}

	return sizeof(EndOfDaySummaryMsg);
}

// int CfeMsgProc::ProcessFuturesInstrumentDefinitionMsg(const char* buf, size_t len) {
// 	const FuturesInstrumentDefinitionMsg* msg = reinterpret_cast<const FuturesInstrumentDefinitionMsg*>(buf);
// 	if (dbg.DebugInstDefs) {
// 		ostringstream oss;
// 		oss << "[FuturesInstrumentDefinitionMsg DEBUG] LegCount=" << (int)msg->LegCount
// 		<< " LegOffset=" << (int)msg->LegOffset << " SEQ=" << _SEQA;
// 		LogMe(oss.str());
// 	}

// 	int bytesread = sizeof(FuturesInstrumentDefinitionMsg);

// 	if (msg->LegOffset > 0)
// 	{
// 		buf += sizeof(FuturesInstrumentDefinitionMsg);

// 		for (int i = 0; i < msg->LegCount; i++)
// 		{
// 			const FuturesLeg* leg = reinterpret_cast<const FuturesLeg*>(buf);
// 			if (dbg.DebugInstDefs) {
// 				ostringstream oss;
// 				oss << "[FuturesLeg DEBUG] LegRatio=" << leg->LegRatio << " LegSymbol=" << std::string(leg->LegSymbol);
// 				LogMe(oss.str());
// 			}
			
// 			buf += sizeof(FuturesLeg);
// 			bytesread += sizeof(FuturesLeg);
// 		}
// 	}

// 	return bytesread;
// }

int CfeMsgProc::ProcessTransactionBeginMsg(const char* buf, size_t len) {
	const TransactionBeginMsg* msg = reinterpret_cast<const TransactionBeginMsg*>(buf);
	if (unlikely(dbg.DebugCertMode))
	{
		LOGINFO("[CfeMsgProc] [TransactionBeginMsg DEBUG] SEQ={} | TimeOffset={}",
			_SEQA.load(std::memory_order_relaxed),
			msg->TimeOffset);
	}
	return sizeof(TransactionBeginMsg);
}

int CfeMsgProc::ProcessTransactionEndMsg(const char* buf, size_t len) {
	const TransactionEndMsg* msg = reinterpret_cast<const TransactionEndMsg*>(buf);
	if (unlikely(dbg.DebugCertMode))
	{
		LOGINFO("[CfeMsgProc] [TransactionEndMsg DEBUG] SEQ={} | TimeOffset={}",
			_SEQA.load(std::memory_order_relaxed),
			msg->TimeOffset);
	}
	return sizeof(TransactionEndMsg);
}

int CfeMsgProc::ProcessPriceLimitsMsg(const char* buf, size_t len) {
	const PriceLimitsMsg* msg = reinterpret_cast<const PriceLimitsMsg*>(buf);
	if(unlikely(dbg.DebugPriceLimitsMsg)){
		ostringstream oss;
		oss << "[PriceLimitsMsg DEBUG] SEQ=" << _SEQA;
		LogMe(oss.str());
	}
	return sizeof(PriceLimitsMsg);
}

int CfeMsgProc::ProcessMarketSnapshotShortMsg(const char* buf, size_t len) {
	const MarketSnapshotShortMsg* msg = reinterpret_cast<const MarketSnapshotShortMsg*>(buf);
	if (unlikely(dbg.DebugMarketSnapshotMsg)) {
		ostringstream oss;
		oss << "[MarketSnapshotShortMsg DEBUG] SEQ=" << _SEQA;
		LogMe(oss.str());
	}
	return sizeof(MarketSnapshotShortMsg);
}

void CfeMsgProc::OnPacketEnd()
{
	if (unlikely(dbg.DebugCertMode))
	{
		int dirtyCount = 0;
		for (size_t i = 0; i < _lob2.size(); i++)
		{
			if (_lob2[i].IsDirty())
			{
				++dirtyCount;
			}
		}
		LOGINFO("[CfeMsgProc] OnPacketEnd called. Dirty count={}", dirtyCount);
	}
	if (dirty)
	{
		for (size_t i = 0; i < _lob2.size(); i++)
		{
			if (_lob2[i].IsDirty())
			{
				PushBookUpdate(_lob2[i], static_cast<uint16_t>(i));
				_lob2[i].ClearDirty();
			}
		}
		dirty = false;
	}
}

void CfeMsgProc::BeginRecovery()
{
	LOGWARN("[CfeMsgProc] BEGINNING GAP RECOVERY — clearing books and requesting spin");

	spinRequested = false;

	for (size_t i = 0; i < _lob2.size(); i++)
	{
		_lob2[i].clear();
	}
	_orderIdToSecId.clear();

	// TODO: Notify the strategy that books are stale?

	// Drain any leftover suspense queue from a previous cycle
	while (!_qsuspense.empty())
	{
		std::pair<char*, int> msg;
		if (_qsuspense.try_pop(msg))
			delete[] msg.first;
	}
	hasSuspendedMessages = false;

	ChangeState(OrderBookState::SpinRequested);
	Spin::SharedSpinClient &sharedSpin = Spin::SharedSpinClient::instance();
	sharedSpin.Start();
	sharedSpin.SendSpinRequest(gapSeq);
	spinRequested = true;
	LOGWARN("[CfeMsgProc] GAP RECOVERY: spin requested for seq={}", gapSeq.load());
}