//============================================================================
// Name        	: OmnetWorker.cpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
//============================================================================

#include "OmnetWorker.hpp"
#include <KT01/Core/Log.hpp>
#include <KT01/SecDefs/OseSecMaster.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <cstring>

using namespace KT01::SECDEF::OSE;

#undef __CLASS__
#define __CLASS__ "OmnetWorker"

// Helper: swap host-order int64_t to network order (and vice versa — symmetric)
static inline void PutInt64(int64_t& dest, const int64_t& src)
{
	swapint64_t(&dest, const_cast<int64_t*>(&src));
}

namespace KTN::OSE {

OmnetWorker::OmnetWorker(SPSCRingBuffer<KTN::OrderPod>& orderQueue,
                           SPSCRingBuffer<KTN::OrderPod>& responseQueue,
                           const OseSessionSettings& sett)
	: _orderQueue(orderQueue)
	, _responseQueue(responseQueue)
	, _sett(sett)
	, _active(false)
	, _readyToTrade(false)
{
}

OmnetWorker::~OmnetWorker()
{
	Stop();
}

bool OmnetWorker::Start()
{
	// Login to OMnet Gateway
	if (!_session.Login(_sett, _sett.ForceLogin))
	{
		KT01_LOG_ERROR(__CLASS__, "Failed to login to OMnet Gateway");
		return false;
	}

	// Start worker thread
	_active.store(true, std::memory_order_release);
	_thread = std::thread(&OmnetWorker::Run, this);

	KT01_LOG_INFO(__CLASS__, "Worker thread started");
	return true;
}

void OmnetWorker::Stop()
{
	_active.store(false, std::memory_order_release);

	if (_thread.joinable())
		_thread.join();

	_session.Logout();
	KT01_LOG_INFO(__CLASS__, "Worker thread stopped");
}

void OmnetWorker::Run()
{
	KTN::OrderPod ord;

	while (_active.load(std::memory_order_acquire))
	{
		if (_orderQueue.dequeue(ord))
		{
			ProcessOrder(ord);
		}
		else
		{
			std::this_thread::yield();
		}
	}
}

void OmnetWorker::ProcessOrder(KTN::OrderPod& ord)
{
	char txbuf[MAX_REQUEST_SIZE];
	memset(txbuf, 0, sizeof(txbuf));
	int txlen = 0;

	// Lookup series from secid (orderbook_id, stored as uint64_t in OrderPod)
	const series_t* series = FindSeries(static_cast<uint32_t>(ord.secid));
	if (!series && ord.OrdAction != KOrderAction::ACTION_CXL)
	{
		KT01_LOG_ERROR(__CLASS__, "Series not found for secid=" + std::to_string(ord.secid));
		ord.OrdStatus = KOrderStatus::REJECTED;
		strncpy(ord.text, "Series not found", sizeof(ord.text) - 1);
		_responseQueue.enqueue(ord);
		return;
	}

	switch (ord.OrdAction)
	{
	case KOrderAction::ACTION_NEW:
		txlen = BuildMO31(ord, txbuf);
		if (txlen > 0 && series)
		{
			hv_order_trans_t* trans = (hv_order_trans_t*)txbuf;
			memcpy(&trans->series, series, sizeof(series_t));
			// Convert back to network byte order for sending
			PUTSHORT(trans->series.commodity_n, trans->series.commodity_n);
			PUTSHORT(trans->series.expiration_date_n, trans->series.expiration_date_n);
			PUTLONG(trans->series.strike_price_i, trans->series.strike_price_i);
		}
		break;
	case KOrderAction::ACTION_MOD:
		txlen = BuildMO33(ord, txbuf);
		if (txlen > 0 && series)
		{
			hv_alter_trans_t* trans = (hv_alter_trans_t*)txbuf;
			memcpy(&trans->series, series, sizeof(series_t));
			PUTSHORT(trans->series.commodity_n, trans->series.commodity_n);
			PUTSHORT(trans->series.expiration_date_n, trans->series.expiration_date_n);
			PUTLONG(trans->series.strike_price_i, trans->series.strike_price_i);
		}
		break;
	case KOrderAction::ACTION_CXL:
		txlen = BuildMO4(ord, txbuf);
		if (txlen > 0 && series)
		{
			delete_trans_t* trans = (delete_trans_t*)txbuf;
			memcpy(&trans->series, series, sizeof(series_t));
			PUTSHORT(trans->series.commodity_n, trans->series.commodity_n);
			PUTSHORT(trans->series.expiration_date_n, trans->series.expiration_date_n);
			PUTLONG(trans->series.strike_price_i, trans->series.strike_price_i);
		}
		break;
	default:
		KT01_LOG_ERROR(__CLASS__, "Unknown order action: " + std::to_string((int)ord.OrdAction));
		ord.OrdStatus = KOrderStatus::REJECTED;
		_responseQueue.enqueue(ord);
		return;
	}

	if (txlen <= 0)
	{
		KT01_LOG_ERROR(__CLASS__, "Failed to build transaction message");
		ord.OrdStatus = KOrderStatus::REJECTED;
		_responseQueue.enqueue(ord);
		return;
	}

	// Send blocking transaction
	uint32 txid = 0;
	uint32 ordid = 0;

	int status = _session.SendTransaction(txbuf, txlen, _session.GetFacilityEP0(), &txid, &ordid);

	if (status == OMNIAPI_SUCCESS)
	{
		if (_sett.DebugAppMsgs)
		{
			KT01_LOG_INFO(__CLASS__, "TX sent ok: action=" + std::to_string((int)ord.OrdAction) +
			              " txid=" + std::to_string(txid));
		}
		// Response will come via BO5 broadcast (handled by BdxThread)
	}
	else
	{
		KT01_LOG_ERROR(__CLASS__, "TX failed: status=" + std::to_string(status));
		ord.OrdStatus = KOrderStatus::REJECTED;
		_responseQueue.enqueue(ord);
	}
}

bool OmnetWorker::QuerySeries()
{
	// Series mapping is now done in QueryInstruments (DQ3) which returns
	// series_t + name_s for each instrument. DQ22 is not supported on J-GATE.
	KT01_LOG_INFO(__CLASS__, "QuerySeries: series mapping deferred to DQ3");
	_seriesCache.clear();
	_secidToSeriesIdx.clear();
	return true;
}

bool OmnetWorker::QueryInstruments()
{
	KT01_LOG_INFO(__CLASS__, "Querying instruments (DQ3) and building series cache...");

	_seriesCache.clear();
	_secidToSeriesIdx.clear();

	query_instrument_t query;
	memset(&query, 0, sizeof(query));
	query.transaction_type.central_module_c = 'D';
	query.transaction_type.server_type_c = 'Q';
	PUTSHORT(query.transaction_type.transaction_number_n, 3);

	char answer[sizeof(answer_instrument_t)];
	answer_instrument_t* ans = (answer_instrument_t*)answer;

	uint16_t segment = 1;
	int totalInstruments = 0;
	int mappedCount = 0;

	while (true)
	{
		PUTSHORT(query.segment_number_n, segment);
		size_t ansLen = sizeof(answer);

		int status = _session.SendQuery(&query, sizeof(query), answer, ansLen,
		                                _session.GetFacilityEP0());

		if (status != OMNIAPI_SUCCESS)
		{
			KT01_LOG_ERROR(__CLASS__, "DQ3 failed: status=" + std::to_string(status));
			return false;
		}

		uint16_t items;
		PUTSHORT(items, ans->items_n);

		if (items == 0)
			break;

		totalInstruments += items;

		// Process each instrument item — build series cache
		for (uint16_t i = 0; i < items; ++i)
		{
			// Extract name (trim trailing spaces)
			char name[33];
			memcpy(name, ans->item[i].name_s, 32);
			name[32] = '\0';
			for (int j = 31; j >= 0 && name[j] == ' '; --j)
				name[j] = '\0';

			// Convert series fields to host byte order
			series_t hostSeries = ans->item[i].series;
			PUTSHORT(hostSeries.commodity_n, hostSeries.commodity_n);
			PUTSHORT(hostSeries.expiration_date_n, hostSeries.expiration_date_n);
			PUTLONG(hostSeries.strike_price_i, hostSeries.strike_price_i);

			// Look up orderbook_id from OseSecMaster by instrument name
			OseSecDef def = OseSecMaster::instance().getSecDef(std::string(name));
			uint32_t obid = def.orderbook_id;

			if (obid != 0)
			{
				// Build seriesKey (same formula as BDX thread uses for BO5 lookup)
				uint64_t seriesKey = ((uint64_t)hostSeries.country_c << 56) |
				                     ((uint64_t)hostSeries.market_c << 48) |
				                     ((uint64_t)hostSeries.instrument_group_c << 40) |
				                     ((uint64_t)hostSeries.modifier_c << 32) |
				                     ((uint64_t)hostSeries.commodity_n << 16) |
				                     ((uint64_t)hostSeries.expiration_date_n);

				// Register in OseSecMaster for BDX thread lookups
				OseSecMaster::instance().RegisterSeriesKey(seriesKey, obid);

				// Add to local series cache (stored in host byte order)
				SeriesInfo si;
				si.series = hostSeries;
				memcpy(si.name, name, 32);

				size_t idx = _seriesCache.size();
				_seriesCache.push_back(si);
				_secidToSeriesIdx[obid] = idx;

				++mappedCount;
			}

			if (_sett.DebugAppMsgs)
			{
				if (obid != 0)
				{
					KT01_LOG_INFO(__CLASS__, "DQ3 mapped: '" + std::string(name) +
					              "' → obid=" + std::to_string(obid) +
					              " commodity=" + std::to_string(hostSeries.commodity_n) +
					              " expiry=" + std::to_string(hostSeries.expiration_date_n));
				}
				else
				{
					KT01_LOG_INFO(__CLASS__, "DQ3 unmapped: '" + std::string(name) +
					              "' commodity=" + std::to_string(hostSeries.commodity_n) +
					              " expiry=" + std::to_string(hostSeries.expiration_date_n));
				}
			}
		}

		if (_sett.DebugAppMsgs)
		{
			KT01_LOG_INFO(__CLASS__, "DQ3 segment " + std::to_string(segment) +
			              ": " + std::to_string(items) + " instruments");
		}

		// Last segment if fewer than 100 items
		if (items < 100)
			break;

		++segment;
	}

	KT01_LOG_INFO(__CLASS__, "DQ3 complete: " + std::to_string(totalInstruments) +
	              " instruments, " + std::to_string(mappedCount) + " mapped to SecMaster");
	return true;
}

const series_t* OmnetWorker::FindSeries(uint32_t orderbook_id) const
{
	auto it = _secidToSeriesIdx.find(orderbook_id);
	if (it != _secidToSeriesIdx.end())
		return &_seriesCache[it->second].series;
	return nullptr;
}

const series_t* OmnetWorker::FindSeriesByName(const char* name) const
{
	for (const auto& si : _seriesCache)
	{
		if (strncmp(si.name, name, 32) == 0)
			return &si.series;
	}
	return nullptr;
}

void OmnetWorker::PopulateSeries(KTN::OrderPod& ord, const series_t* series)
{
	if (!series)
	{
		KT01_LOG_ERROR(__CLASS__, "Series not found for secid=" + std::to_string(ord.secid));
		return;
	}
}

bool OmnetWorker::SendReadyToTrade()
{
	// Build UI1 (application_status) transaction
	application_status_t ui1;
	memset(&ui1, 0, sizeof(ui1));

	// Set transaction type: UI1
	ui1.transaction_type.central_module_c = 'U';
	ui1.transaction_type.server_type_c = 'I';
	PUTSHORT(ui1.transaction_type.transaction_number_n, 1);

	// Set application status = 1 (ready to trade)
	PUTLONG(ui1.application_status_i, 1);

	int status = _session.SendTransaction(&ui1, sizeof(ui1), _session.GetFacilityEP0());
	if (status == OMNIAPI_SUCCESS)
	{
		_readyToTrade.store(true, std::memory_order_release);
		KT01_LOG_INFO(__CLASS__, "UI1 Ready-to-Trade sent successfully");
		return true;
	}

	KT01_LOG_ERROR(__CLASS__, "UI1 Ready-to-Trade failed: " + std::to_string(status));
	return false;
}

int OmnetWorker::BuildMO31(const KTN::OrderPod& ord, void* buf)
{
	hv_order_trans_t* trans = (hv_order_trans_t*)buf;
	memset(trans, 0, sizeof(hv_order_trans_t));

	// Transaction type: MO31
	trans->transaction_type.central_module_c = 'M';
	trans->transaction_type.server_type_c = 'O';
	PUTSHORT(trans->transaction_type.transaction_number_n, 31);

	// Order variable fields
	int64_t qty = ord.quantity;
	PutInt64(trans->order_var.mp_quantity_i, qty);

	int32_t price = static_cast<int32_t>(ord.price.AsShifted());
	PUTLONG(trans->order_var.premium_i, price);

	// Side: OMnet uses 1=Buy, 2=Sell
	trans->order_var.bid_or_ask_c = (ord.OrdSide == KOrderSide::BUY) ? 1 : 2;

	// Block number (always 1 for single orders)
	uint32_t block = 1;
	PUTLONG(trans->order_var.block_n, block);

	// Open/close: 0 = not specified
	trans->order_var.open_close_req_c = 0;

	// Time validity
	uint16_t tif = 0x0100; // Default: Day
	switch (ord.OrdTif)
	{
	case KOrderTif::DAY: tif = 0x0100; break;
	case KOrderTif::GTC: tif = 0x0200; break;
	case KOrderTif::IOC: tif = 0x0300; break;
	case KOrderTif::FOK: tif = 0x0400; break;
	default: tif = 0x0100; break;
	}
	PUTSHORT(trans->order_var.time_validity_n, tif);

	// Order type: 1=Limit, 2=Market
	trans->order_var.order_type_c = (ord.OrdType == KOrderType::MARKET) ? 2 : 1;

	// Total volume (same as mp_quantity for new orders)
	PutInt64(trans->total_volume_i, qty);

	// Customer info (ex_client) — store our order request ID for correlation
	snprintf(trans->order_var.ex_client_s, sizeof(trans->order_var.ex_client_s),
	         "%lu", (unsigned long)ord.orderReqId);

	if (_sett.DebugAppMsgs)
	{
		KT01_LOG_INFO(__CLASS__, "MO31: side=" + std::to_string(trans->order_var.bid_or_ask_c) +
		              " price=" + std::to_string(price) + " qty=" + std::to_string(qty) +
		              " tif=0x" + std::to_string(tif) + " type=" + std::to_string(trans->order_var.order_type_c));
	}

	return sizeof(hv_order_trans_t);
}

int OmnetWorker::BuildMO33(const KTN::OrderPod& ord, void* buf)
{
	hv_alter_trans_t* trans = (hv_alter_trans_t*)buf;
	memset(trans, 0, sizeof(hv_alter_trans_t));

	// Transaction type: MO33
	trans->transaction_type.central_module_c = 'M';
	trans->transaction_type.server_type_c = 'O';
	PUTSHORT(trans->transaction_type.transaction_number_n, 33);

	// Order number — the exchange order ID to alter
	memcpy(&trans->order_number_u, &ord.exchordid, sizeof(quad_word));

	// Updated order variable fields
	int64_t qty = ord.quantity;
	PutInt64(trans->order_var.mp_quantity_i, qty);

	int32_t price = static_cast<int32_t>(ord.price.AsShifted());
	PUTLONG(trans->order_var.premium_i, price);

	// Side
	trans->order_var.bid_or_ask_c = (ord.OrdSide == KOrderSide::BUY) ? 1 : 2;

	// Block number
	uint32_t block = 1;
	PUTLONG(trans->order_var.block_n, block);

	// Open/close
	trans->order_var.open_close_req_c = 0;

	// Time validity
	uint16_t tif = 0x0100;
	switch (ord.OrdTif)
	{
	case KOrderTif::DAY: tif = 0x0100; break;
	case KOrderTif::GTC: tif = 0x0200; break;
	case KOrderTif::IOC: tif = 0x0300; break;
	case KOrderTif::FOK: tif = 0x0400; break;
	default: tif = 0x0100; break;
	}
	PUTSHORT(trans->order_var.time_validity_n, tif);

	// Order type
	trans->order_var.order_type_c = (ord.OrdType == KOrderType::MARKET) ? 2 : 1;

	// Total volume
	PutInt64(trans->total_volume_i, qty);

	// Delta quantity: 0 = absolute quantity, 1 = delta
	trans->delta_quantity_c = 0;

	// Balance quantity (remaining after alter)
	PutInt64(trans->balance_quantity_i, qty);

	// Customer info
	snprintf(trans->order_var.ex_client_s, sizeof(trans->order_var.ex_client_s),
	         "%lu", (unsigned long)ord.orderReqId);

	if (_sett.DebugAppMsgs)
	{
		KT01_LOG_INFO(__CLASS__, "MO33: side=" + std::to_string(trans->order_var.bid_or_ask_c) +
		              " price=" + std::to_string(price) + " qty=" + std::to_string(qty));
	}

	return sizeof(hv_alter_trans_t);
}

int OmnetWorker::BuildMO4(const KTN::OrderPod& ord, void* buf)
{
	delete_trans_t* trans = (delete_trans_t*)buf;
	memset(trans, 0, sizeof(delete_trans_t));

	// Transaction type: MO4
	trans->transaction_type.central_module_c = 'M';
	trans->transaction_type.server_type_c = 'O';
	PUTSHORT(trans->transaction_type.transaction_number_n, 4);

	// Order number — the exchange order ID to delete
	memcpy(&trans->order_number_u, &ord.exchordid, sizeof(quad_word));

	// Side: 0=both (mass cancel), 1=buy, 2=sell
	trans->bid_or_ask_c = (ord.OrdSide == KOrderSide::BUY) ? 1 : 2;

	// Customer info
	snprintf(trans->customer_info_s, sizeof(trans->customer_info_s),
	         "%lu", (unsigned long)ord.orderReqId);

	if (_sett.DebugAppMsgs)
	{
		KT01_LOG_INFO(__CLASS__, "MO4: side=" + std::to_string(trans->bid_or_ask_c) +
		              " exchordid=" + std::to_string(ord.exchordid));
	}

	return sizeof(delete_trans_t);
}

} // namespace KTN::OSE
