//============================================================================
// Name        	: CfeMsgProc.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Dec 10, 2024 10:45:48 AM
//============================================================================

#ifndef SRC_DATAHANDLERS_CFEMSGPROC_HPP_
#define SRC_DATAHANDLERS_CFEMSGPROC_HPP_

#pragma once
#include <cassert>
#include <cstdio>
#include <memory>

#include <unordered_map>
#include <set>

#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_queue.h>

#include <KT01/Core/Log.hpp>
#include <KT01/Core/NamedThread.hpp>


#include <Settings/DataProcSettings.hpp>

// book building
#include <KT01/DataStructures/DepthBook.hpp>
#include <KT01/DataStructures/OrderBookEnums.hpp>
#include <KT01/DataStructures/LimitOrderBook.hpp>
#include <KT01/DataStructures/MDOrder.hpp>

#include <KT01/SecDefs/CfeSecMaster.hpp>

#include <exchsupport/cfe/pitch/PitchMessages.hpp>
#include <exchsupport/cfe/pitch/PitchMsgTypes.hpp>

#include <exchsupport/cfe/spin/SpinClient.hpp>
#include <exchsupport/cfe/spin/SharedSpinClient.hpp>
#include <exchsupport/cfe/spin/SpinCallback.hpp>

#include <AlgoSupport/AlgoMktDataCallback.hpp>


namespace KTN{
namespace CFE{
namespace Pitch{


struct DebugState
{
	bool DebugEnabled;
	int DebugSecid;
	int DebugSpreadSecid;
	string DebugSymbol;
	string DebugSpreadSymbol;
	bool DebugCertMode;
	bool DebugInstDefs;
	bool DebugBytes;
	bool DebugMarketSnapshotMsg;
	bool DebugPriceLimitsMsg;
	bool DebugOnPacketEnd;
};


class CfeMsgProc : public Spin::SpinCallback
{
public:
	CfeMsgProc();
	virtual ~CfeMsgProc();
	void Stop();

	void Process(char* buf, int len);
    
    void MenuChoice(int choice);

    //callback for spin client
	void onSpinImageAvailable(uint32_t sequence) override
	{
		if (GetState() != OrderBookState::Current && GetState() != OrderBookState::SpinRequested)
		{
			LogMe("SPIN IMAGE AVAILABLE: SEQ=" + std::to_string(sequence));
		}
	}
	void onSpinStart(uint32_t startseq, uint32_t endseq, uint32_t count) override
	{
		LogMe("SPIN START: SEQ=" + std::to_string(startseq) + " ENDSEQ=" + std::to_string(endseq) + " COUNT=" + std::to_string(count));
		ChangeState(OrderBookState::Recovering);
	}
	void onSpinEnd(uint32_t seqnum, uint32_t total_processed) override
	{
		_SEQ_SPIN_LAST = seqnum;
		LogMe("SPIN END: SEQ=" + std::to_string(seqnum) + " PROCESSED=" + std::to_string(total_processed));

		if (seqnum < gapSeq.load())
		{
			// This spin doesn't cover our latest gap — need another spin
			LOGWARN("[CfeMsgProc] Spin seq={} doesn't cover gap at {} — will request another", seqnum, gapSeq.load());
			spinRequested = false;
			ChangeState(OrderBookState::SpinRequested);
			return; // Don't transition to BuildingBooks
		}

		ChangeState(OrderBookState::BuildingBooks);
	}
	void onBooksAvailable(vector<LimitOrderBook> & books) override
	{
		LogMe("SPIN BOOKS AVAILABLE: COUNT=" + std::to_string(books.size()));

		for (auto & book : books)
		{
			string symb = _secmaster->getSymbol(book.SecId());

			for (size_t i = 0; i < _lob2.size(); i++)
			{
				if (_lob2[i].SecId() == book.SecId())
				{
					_lob2[i] = book;
					_lob2[i].SetDirty();
					for(auto & order : book.Bids())
						_orderIdToSecId[order.second.id] = order.second.secid;
					for(auto & order : book.Asks())
						_orderIdToSecId[order.second.id] = order.second.secid;
					dirty = true;
					break;
				}
			}
		}

		LogMe("ORDERIDS MAPPED TO SECIDS=" + std::to_string(_orderIdToSecId.size()));
		ProcessSuspenseQueue(_SEQ_SPIN_LAST);
	}

	//orderbook state menthods
	void ChangeState(OrderBookState state)
	{
		KT01_LOG_WARN(__CLASS__, "Orderbook State changed from " + toString(_STATE) + " to " + toString(state));
		_STATE = state;
	}

	OrderBookState GetState()
	{
		return _STATE;
	}

	void SetTradeDate(uint32_t tradeDate) {
		_TRADEDATE = tradeDate;
	}

	uint32_t GetTradeDate() const {
		return _TRADEDATE;
	}

	// convert CFE TradeDate(YYYYMMDD) to days since epoch
	uint32_t GetDaysSinceEpoch(uint32_t tradeDate) const {
		// Check cache first
		static std::unordered_map<uint32_t, uint32_t> dateCache;
		auto it = dateCache.find(tradeDate);
		if (it != dateCache.end()) {
			return it->second;
		}

		// Extract year, month, day from YYYYMMDD format
		uint32_t year = tradeDate / 10000;
		uint32_t month = (tradeDate / 100) % 100;
		uint32_t day = tradeDate % 100;

		// Validate input
		if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31) {
			return 0; // Return 0 for invalid dates
		}

		// Manual calculation to avoid timezone issues
		auto isLeapYear = [](uint32_t y) -> bool {
			return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
			};

		// Days in each month (non-leap year)
		const uint32_t daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		uint32_t totalDays = 0;

		// Add days for complete years since 1970
		for (uint32_t y = 1970; y < year; ++y) {
			totalDays += isLeapYear(y) ? 366 : 365;
		}

		// Add days for complete months in the current year
		for (uint32_t m = 1; m < month; ++m) {
			totalDays += daysInMonth[m - 1];
			// Add extra day for February in leap years
			if (m == 2 && isLeapYear(year)) {
				totalDays += 1;
			}
		}

		// Add remaining days
		totalDays += (day - 1); // -1 because we want days since start of epoch

		// Cache the result
		dateCache[tradeDate] = totalDays;

		return totalDays;
	}

	void Subscribe(KTN::AlgoMktDataCallback *subscriber);
	void AddSymbol(const std::string &symbol, int index, uint32_t secId);

	bool DropNextPacket { false };

private:
	void LogMe(std::string szMsg);

    void LoadDebugSettings();

	void PushBookUpdate(const LimitOrderBook &book, uint16_t index);

	void MsgHandler(char * buf, int len);
	void ProcessBuffer(char * buf, int len, bool skip_spun_messages = false);

    int ProcessMessage(int iTemplateid, char *buf, int iMsgLen);

	void ProcessSuspenseQueue(uint32_t start_seq);
	
	int ProcessTimeMsg(const char* buf, size_t len);
	int ProcessAddOrderLongMsg(const char* buf, size_t len);
	int ProcessAddOrderShortMsg(const char* buf, size_t len);
	int ProcessOrderExecutedMsg(const char* buf, size_t len);
	int ProcessOrderExecutedPriceSizeMsg(const char* buf, size_t len);
	int ProcessReduceSizeLongMsg(const char* buf, size_t len);
	int ProcessReduceSizeShortMsg(const char* buf, size_t len);
	int ProcessModifyOrderLongMsg(const char* buf, size_t len);
	int ProcessModifyOrderShortMsg(const char* buf, size_t len);
	int ProcessDeleteOrderMsg(const char* buf, size_t len);
	int ProcessTradeLongMsg(const char* buf, size_t len);
	int ProcessTradeShortMsg(const char* buf, size_t len);
	int ProcessTradeBreakMsg(const char* buf, size_t len);
	int ProcessEndSessionMsg(const char* buf, size_t len);
	int ProcessTradingStatusMsg(const char* buf, size_t len);
	int ProcessUnitClearMsg(const char* buf, size_t len);
	int ProcessTimeReferenceMsg(const char* buf, size_t len);
	int ProcessMarketSnapshotShortMsg(const char* buf, size_t len);
	int ProcessMarketSnapshotLongMsg(const char* buf, size_t len);
	int ProcessSingleSideUpdateShortMsg(const char* buf, size_t len);
	int ProcessSingleSideUpdateLongMsg(const char* buf, size_t len);
	int ProcessTwoSideUpdateShortMsg(const char* buf, size_t len);
	int ProcessTwoSideUpdateLongMsg(const char* buf, size_t len);
	int ProcessTOPTradeMsg(const char* buf, size_t len);
	int ProcessSettlementMsg(const char* buf, size_t len);
	int ProcessEODSummaryMsg(const char* buf, size_t len);
	//int ProcessFuturesInstrumentDefinitionMsg(const char* buf, size_t len);
	int ProcessTransactionBeginMsg(const char* buf, size_t len);
	int ProcessTransactionEndMsg(const char* buf, size_t len);
	int ProcessPriceLimitsMsg(const char* buf, size_t len);

	void OnPacketEnd();

	void BeginRecovery();

	void onNewTradeDate(uint32_t tradedate);

	inline uint64_t GetTimeFromTimeOffset(uint32_t timeoffset)
	{
		//timeoffset is in nanoseconds from last time msg
		//_TIME is in seconds from epoch
		uint64_t t = ((uint64_t)_TIME * 1000000000) + (uint64_t)timeoffset;
		return t;
	}

private:
	OrderBookState _STATE;
	DebugState dbg;

	std::shared_ptr<KT01::SECDEF::CFE::CfeSecMaster> _secmaster;

	tbb::concurrent_queue<std::pair<char*, int>> _qsuspense;
	std::vector<LimitOrderBook> _lob2;
	std::unordered_map<uint64_t, uint32_t> _orderIdToSecId; 
	bool dirty { false };
	bool hasSuspendedMessages { false };

	//Sequencing
	std::atomic<uint32_t> _SEQA;
	//this is the lowest sequence we can send a sping request for... we will have all seqs after
	std::atomic<uint32_t> _SEQ_FIRST;
	std::atomic<uint32_t> _SEQ_SPIN_LAST;
	uint32_t nextExpectedSeq { 0 };

	uint32_t _TIME;
	uint32_t _TRADEDATE;

	KTN::AlgoMktDataCallback *callback { nullptr };

	std::atomic_uint32_t gapSeq { 0 };
	bool spinRequested { false };

private:
	CfeMsgProc(const CfeMsgProc&) = delete;
	CfeMsgProc& operator=(const CfeMsgProc&) = delete;

};

//============================================================================
} // namespace Pitch
} // namespace CFE
} // namespace KTN

#endif /* SRC_DATAHANDLERS_CfeMsgProc_HPP_ */
