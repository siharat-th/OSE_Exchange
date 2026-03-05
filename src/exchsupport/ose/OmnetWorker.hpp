//============================================================================
// Name        	: OmnetWorker.hpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
// Description 	: Worker thread for blocking OMnet tx_ex calls
//============================================================================

#ifndef SRC_EXCHSUPPORT_OSE_OMNETWORKER_HPP_
#define SRC_EXCHSUPPORT_OSE_OMNETWORKER_HPP_

#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <Maps/SPSCRingBuffer.hpp>
#include <Orders/OrderPod.hpp>
#include "OmnetSession.hpp"
#include "settings/OseSessionSettings.hpp"

namespace KTN::OSE {

using KTN::Core::SPSCRingBuffer;
using namespace KTN::ORD;

// DQ2 query/answer structs (not in omex_ose.h — defined per OMnet convention)
#pragma pack(push, 1)
struct query_series_ose_t
{
	transaction_type_t transaction_type;
	series_t series;
	uint16_t segment_number_n;
	char filler_2_s[2];
};

struct answer_series_item_t
{
	series_t series;
	char ins_id_s[32];
};

struct answer_series_ose_t
{
	transaction_type_t transaction_type;
	uint16_t segment_number_n;
	uint16_t items_n;
	answer_series_item_t item[1]; // variable-length array
};

// DQ3 query/answer structs
struct answer_instrument_item_t
{
	series_t series;
	char name_s[32];
	// Additional fields may follow — we only use series + name
};

struct answer_instrument_ose_t
{
	transaction_type_t transaction_type;
	uint16_t segment_number_n;
	uint16_t items_n;
	answer_instrument_item_t item[1]; // variable-length array
};
#pragma pack(pop)

// Named struct IDs for DQ124 VIM parsing
static constexpr uint16_t NS_DELTA_HEADER          = 37001;
static constexpr uint16_t NS_REMOVE                = 37002;
static constexpr uint16_t NS_INST_SERIES_BASIC     = 37301;
static constexpr uint16_t NS_INST_SERIES_BASIC_SINGLE = 37302;
static constexpr uint16_t NS_INST_SERIES_ID        = 37310;

// Series info for cache
struct SeriesInfo
{
	series_t series;
	char name[32];
};

class OmnetWorker
{
public:
	OmnetWorker(SPSCRingBuffer<KTN::OrderPod>& orderQueue,
	            SPSCRingBuffer<KTN::OrderPod>& responseQueue,
	            const OseSessionSettings& sett);
	~OmnetWorker();

	bool Start();
	void Stop();
	bool IsReady() const { return _session.IsLoggedIn() && _readyToTrade; }

	// Pre-trade setup (called from OseGT after login)
	bool QuerySeries();
	bool QueryInstruments();
	bool QueryInstrumentSeries();  // DQ124 — actual tradeable series with orderbook_id
	bool SendReadyToTrade();

	OmnetSession& GetSession() { return _session; }

	// Series cache lookup (secid = orderbook_id from ITCH/SecMaster)
	const series_t* FindSeries(uint32_t orderbook_id) const;
	const series_t* FindSeriesByName(const char* name) const;

private:
	OmnetSession _session;
	SPSCRingBuffer<KTN::OrderPod>& _orderQueue;
	SPSCRingBuffer<KTN::OrderPod>& _responseQueue;
	const OseSessionSettings& _sett;
	std::thread _thread;
	std::atomic<bool> _active;
	std::atomic<bool> _readyToTrade;
	std::atomic<bool> _setupDone;
	std::atomic<bool> _setupOk;

	// Series cache (populated by DQ124, keyed by orderbook_id)
	std::vector<SeriesInfo> _seriesCache;
	std::unordered_map<uint32_t, size_t> _secidToSeriesIdx; // orderbook_id → index in _seriesCache

	void Run();
	void ProcessOrder(KTN::OrderPod& ord);
	void PopulateSeries(KTN::OrderPod& ord, const series_t* series);
	bool Reconnect();
	int BuildMO31(const KTN::OrderPod& ord, void* buf);    // New order
	int BuildMO33(const KTN::OrderPod& ord, void* buf);    // Alter
	int BuildMO4(const KTN::OrderPod& ord, void* buf);     // Delete
};

} // namespace KTN::OSE

#endif /* SRC_EXCHSUPPORT_OSE_OMNETWORKER_HPP_ */
