//============================================================================
// Name        	: OmnetWorker.cpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
//============================================================================

#include "OmnetWorker.hpp"
#include <KT01/Core/Log.hpp>
#include <KT01/SecDefs/OseSecMaster.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <Notifications/NotifierRest.hpp>
#include <akl/Price.hpp>
#include <akl/PriceConverters.hpp>
#include <cstring>
#include <cinttypes>
#include <set>

using namespace KT01::SECDEF::OSE;

#undef __CLASS__
#define __CLASS__ "OmnetWorker"

// Helper: swap host-order int64_t to network order (and vice versa — symmetric)
static inline void PutInt64(int64_t& dest, const int64_t& src)
{
	swapint64_t(&dest, const_cast<int64_t*>(&src));
}

// Helper: hex dump for debug logging
static std::string HexDump(const char* data, size_t len, size_t maxBytes = 64)
{
	std::string out;
	size_t n = std::min(len, maxBytes);
	char buf[4];
	for (size_t i = 0; i < n; ++i)
	{
		snprintf(buf, sizeof(buf), "%02X ", (unsigned char)data[i]);
		out += buf;
	}
	if (len > maxBytes) out += "...";
	return out;
}

namespace KTN::OSE {

OmnetWorker::OmnetWorker(int workerId,
                           SPSCRingBuffer<KTN::OrderPod>& orderQueue,
                           SPSCRingBuffer<KTN::OrderPod>& responseQueue,
                           const OseSessionSettings& sett,
                           const OseSessionSettings::SessionCreds& creds,
                           SettlementCache& settlCache,
                           std::atomic<bool>& settlementReady,
                           std::atomic<bool>& settlementQueryReq)
	: _workerId(workerId)
	, _logTag("OmnetWorker[" + std::to_string(workerId) + "]")
	, _orderQueue(orderQueue)
	, _responseQueue(responseQueue)
	, _sett(sett)
	, _creds(creds)
	, _active(false)
	, _readyToTrade(false)
	, _settlCache(settlCache)
	, _settlementReady(settlementReady)
	, _settlementQueryReq(settlementQueryReq)
{
}

OmnetWorker::~OmnetWorker()
{
	Stop();
}

bool OmnetWorker::Start()
{
	// Start worker thread — all OMnet API calls happen on this thread
	_active.store(true, std::memory_order_release);
	_setupDone.store(false, std::memory_order_release);
	_setupOk.store(false, std::memory_order_release);
	_thread = std::thread(&OmnetWorker::Run, this);

	// Wait for setup to complete (login, queries, subscribe, UI1)
	while (!_setupDone.load(std::memory_order_acquire))
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	if (!_setupOk.load(std::memory_order_acquire))
	{
		KT01_LOG_ERROR(_logTag, "Worker setup failed");
		return false;
	}

	KT01_LOG_INFO(_logTag, "Worker thread started and ready — user=" + _creds.LoginId);
	return true;
}

void OmnetWorker::Stop()
{
	_active.store(false, std::memory_order_release);

	if (_thread.joinable())
		_thread.join();

	_session.Logout();
	KT01_LOG_INFO(_logTag, "Worker thread stopped");
}

void OmnetWorker::Run()
{
	// Phase 1: Setup — all OMnet API calls on this thread
	if (!_session.Login(_creds, _sett.ForceLogin))
	{
		KT01_LOG_ERROR(_logTag, "Failed to login to OMnet Gateway");
		KTN::notify::NotifierRest::NotifyError(_sett.ExchName, _sett.Source, _sett.Org,
			"Worker #" + std::to_string(_workerId + 1) + " login failed");
		_setupOk.store(false, std::memory_order_release);
		_setupDone.store(true, std::memory_order_release);
		return;
	}

	// Pre-trade queries — only Worker[0] does DQ3/DQ124 (others reuse via SecMaster singleton)
	if (_workerId == 0)
	{
		QueryInstruments();
		QueryInstrumentSeries();
	}
	else
	{
		KT01_LOG_INFO(_logTag, "Skipping DQ3/DQ124 — reusing series from Worker[0]");
	}

	// Subscribe to broadcasts (required before sending orders)
	_session.SubscribeAll();

	// Ready-to-trade
	SendReadyToTrade();

	// Signal setup complete
	_setupOk.store(true, std::memory_order_release);
	_setupDone.store(true, std::memory_order_release);

	KT01_LOG_INFO(_logTag, "Setup complete, entering order processing loop");
	KTN::notify::NotifierRest::NotifyInfo(_sett.ExchName, _sett.Source, _sett.Org,
		"Worker #" + std::to_string(_workerId + 1) + " LOGIN SUCCESSFUL");

	// Phase 2: Order processing loop
	KTN::OrderPod ord;
	auto lastKeepalive = std::chrono::steady_clock::now();
	constexpr auto KEEPALIVE_INTERVAL = std::chrono::seconds(30);

	while (_active.load(std::memory_order_acquire))
	{
		if (_orderQueue.dequeue(ord))
		{
			ProcessOrder(ord);
			lastKeepalive = std::chrono::steady_clock::now(); // TX counts as activity
		}
		else
		{
			// Check session health
			if (!_session.IsLoggedIn())
			{
				KT01_LOG_ERROR(_logTag, "Session lost, attempting reconnect...");
				// CFE-style notification throttle: notify on attempts 1,5,10,20,40,80,120, then every 40
				bool shouldNotify = (_reconnectAttempt == 0 || _reconnectAttempt == 4 ||
				                     _reconnectAttempt == 9 || _reconnectAttempt == 19 ||
				                     _reconnectAttempt == 39 || _reconnectAttempt == 79 ||
				                     _reconnectAttempt == 119 ||
				                     (_reconnectAttempt > 119 && (_reconnectAttempt - 119) % 40 == 0));
				if (shouldNotify)
				{
					KTN::notify::NotifierRest::NotifyError(_sett.ExchName, _sett.Source, _sett.Org,
						"Worker #" + std::to_string(_workerId + 1) + " session lost, reconnect attempt " + std::to_string(_reconnectAttempt + 1));
				}
				Reconnect();  // backoff is inside Reconnect()
				lastKeepalive = std::chrono::steady_clock::now();
				continue;
			}

			// Settlement queries only on Worker[0]
			if (_workerId == 0)
			{
				// Auto-trigger RQ62 after BDX sees BI7 info_type=100
				if (_settlementReady.load(std::memory_order_acquire))
				{
					KT01_LOG_INFO(_logTag, "BI7 settlement signal received — auto-querying RQ62");
					_settlementReady.store(false, std::memory_order_release);
					QuerySettlement();
				}

				// Manual query from menu (option 350)
				if (_settlementQueryReq.load(std::memory_order_acquire))
				{
					KT01_LOG_INFO(_logTag, "Manual settlement query requested (menu)");
					_settlementQueryReq.store(false, std::memory_order_release);
					QuerySettlement();
				}
			}

			// Periodic keepalive — prevent gateway timeout when idle
			auto now = std::chrono::steady_clock::now();
			if (now - lastKeepalive >= KEEPALIVE_INTERVAL)
			{
				int32 txstatus = 0;
				uint32 len = sizeof(int32);
				int32 dummy = 0;
				int32 rc = omniapi_get_info_ex(_session.GetHandle(), &txstatus,
				                               (uint32)OMNI_INFTYP_TXTIMEOUT, &len, (void*)&dummy);
				if (rc != OMNIAPI_SUCCESS)
				{
					KT01_LOG_ERROR(_logTag, "Keepalive failed: " + std::to_string(rc) +
					               " — session may be dead");
					// Fatal errors: mark session dead so reconnect triggers on next iteration
					if (rc == OMNIAPI_INTFAILURE || rc == OMNIAPI_NOT_LOGGED_IN || rc == OMNIAPI_NOTCONNECTED)
						_session.MarkDead();
				}
				lastKeepalive = now;
			}

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
		KT01_LOG_ERROR(_logTag, "Series not found for secid=" + std::to_string(ord.secid));
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

			if (_sett.DebugAppMsgs)
			{
				KT01_LOG_INFO(_logTag, "MO31 series: country=" + std::to_string(trans->series.country_c) +
				              " market=" + std::to_string(trans->series.market_c) +
				              " ig=" + std::to_string(trans->series.instrument_group_c) +
				              " mod=" + std::to_string(trans->series.modifier_c) +
				              " commodity=" + std::to_string(trans->series.commodity_n) +
				              " expiry=" + std::to_string(trans->series.expiration_date_n) +
				              " strike=" + std::to_string(trans->series.strike_price_i));
				KT01_LOG_INFO(_logTag, "MO31 series hex: " + HexDump((const char*)&trans->series, sizeof(series_t), 16));
			}
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
		KT01_LOG_ERROR(_logTag, "Unknown order action: " + std::to_string((int)ord.OrdAction));
		ord.OrdStatus = KOrderStatus::REJECTED;
		_responseQueue.enqueue(ord);
		return;
	}

	if (txlen <= 0)
	{
		KT01_LOG_ERROR(_logTag, "Failed to build transaction message");
		ord.OrdStatus = KOrderStatus::REJECTED;
		_responseQueue.enqueue(ord);
		return;
	}

	// Debug: log struct size, EP0, and hex dump of first bytes
	if (_sett.DebugAppMsgs)
	{
		KT01_LOG_INFO(_logTag, "TX: len=" + std::to_string(txlen) +
		              " sizeof(hv_order_trans_t)=" + std::to_string(sizeof(hv_order_trans_t)) +
		              " EP0=" + std::to_string(_session.GetFacilityEP0()));
		KT01_LOG_INFO(_logTag, "TX hex: " + HexDump(txbuf, txlen, 80));
	}

	// Send blocking transaction
	uint32 txid = 0;
	quad_word ordidQw;
	memset(&ordidQw, 0, sizeof(quad_word));
	int32 txstatus = 0;

	int status = _session.SendTransaction(txbuf, txlen, _session.GetFacilityEP0(),
	                                       &txid, &ordidQw, &txstatus);

	if (status == OMNIAPI_SUCCESS)
	{
		KT01_LOG_INFO(_logTag, "TX sent ok: action=" + std::to_string((int)ord.OrdAction) +
		              " txid=" + std::to_string(txid) +
		              " ordid=" + HexDump(ordidQw.quad_word, 8, 8) +
		              " txstatus=" + std::to_string(txstatus));

		// For MO31 (NEW): ordid from tx_ex IS the order_number
		// For MO33 (MOD): ordid from tx_ex is just txid (NOT new order_number!)
		//   OMnet keeps the same order_number after alter — no update needed
		// For MO4 (CXL): exchordid already set correctly
		if (ord.OrdAction == KOrderAction::ACTION_NEW)
			memcpy(&ord.exchordid, &ordidQw, sizeof(quad_word));

		// Fill txstatus (2,3,6) only applies to MO31 (NEW) — defer to BO5/BD6 for real execid
		// For MO33/MO4: txstatus = number of contracts before change, NOT a fill indicator
		bool isFillTx = (ord.OrdAction == KOrderAction::ACTION_NEW) &&
		                (txstatus == 2 || txstatus == 3 || txstatus == 6);

		// For MO33 (MOD) and MO4 (CXL): txstatus = number of contracts before change
		// For MO31 (NEW): txstatus has specific meanings (1=FOK reject, 2=filled, 4=in book, etc.)
		if (ord.OrdAction == KOrderAction::ACTION_MOD)
		{
			// MO33: txstatus >= 1 means success (N contracts before change)
			// txstatus == 0 means order not found (should not happen if bid_or_ask set)
			if (txstatus > 0)
			{
				ord.OrdStatus = KOrderStatus::MODIFIED;
				ord.OrdState = KOrderState::WORKING;
				ord.leavesqty = ord.quantity;
			}
			else
			{
				KT01_LOG_WARN(_logTag, "MO33 txstatus=0 — order not found, no modification");
				ord.OrdStatus = KOrderStatus::REJECTED;
				ord.OrdState = KOrderState::COMPLETE;
				snprintf(ord.text, sizeof(ord.text) - 1, "MO33 order not found (txstatus=0)");
			}
		}
		else if (ord.OrdAction == KOrderAction::ACTION_CXL)
		{
			// MO4: txstatus = contracts before delete (0 = success for single delete)
			ord.OrdStatus = KOrderStatus::CANCELED;
			ord.OrdState = KOrderState::COMPLETE;
		}
		else
		{
			// MO31 (NEW): txstatus has specific meanings
			switch (txstatus)
			{
			case 2: // Whole order traded (filled) — wait for BO5/BD6
			case 3: // Partially traded, nothing in book (FAK) — wait for BO5/BD6
			case 6: // Partially traded, rest in orderbook — wait for BO5/BD6
				KT01_LOG_INFO(_logTag, "Fill txstatus=" + std::to_string(txstatus) +
				              " — deferring to BO5/BD6 broadcast");
				isFillTx = true;
				break;
			case 4: // Whole order placed in orderbook
				ord.OrdStatus = KOrderStatus::NEW;
				ord.OrdState = KOrderState::WORKING;
				ord.leavesqty = ord.quantity;
				break;
			case 1: // FOK rejected (no fill, no book)
			case 17: // circuit breaker
			case 19: // circuit breaker partial
				ord.OrdStatus = KOrderStatus::CANCELED;
				ord.OrdState = KOrderState::COMPLETE;
				break;
			default:
				ord.OrdStatus = KOrderStatus::NEW;
				ord.OrdState = KOrderState::WORKING;
				break;
			}
		}
		if (!isFillTx)
			_responseQueue.enqueue(ord);
		// Fills come from BO5/BD6 broadcasts via BDX thread
	}
	else
	{
		KT01_LOG_ERROR(_logTag, "TX failed: status=" + std::to_string(status) +
		               " txstatus=" + std::to_string(txstatus));

		// Session dead (-2001 or -2008) — attempt reconnect and retry once
		if (status == OMNIAPI_INTFAILURE || status == OMNIAPI_NOT_LOGGED_IN || status == OMNIAPI_NOTCONNECTED)
		{
			KT01_LOG_INFO(_logTag, "Session lost (status=" + std::to_string(status) +
			              "), attempting reconnect and retry...");
			if (Reconnect())
			{
				// Re-lookup series (cache was rebuilt)
				series = FindSeries(static_cast<uint32_t>(ord.secid));
				if (series && ord.OrdAction == KOrderAction::ACTION_NEW)
				{
					hv_order_trans_t* trans = (hv_order_trans_t*)txbuf;
					memcpy(&trans->series, series, sizeof(series_t));
					PUTSHORT(trans->series.commodity_n, trans->series.commodity_n);
					PUTSHORT(trans->series.expiration_date_n, trans->series.expiration_date_n);
					PUTLONG(trans->series.strike_price_i, trans->series.strike_price_i);
				}

				txstatus = 0;
				txid = 0;
				memset(&ordidQw, 0, sizeof(quad_word));
				int retry = _session.SendTransaction(txbuf, txlen, _session.GetFacilityEP0(),
				                                      &txid, &ordidQw, &txstatus);
				if (retry == OMNIAPI_SUCCESS)
				{
					KT01_LOG_INFO(_logTag, "Retry TX ok: txid=" + std::to_string(txid) +
					              " ordid=" + HexDump(ordidQw.quad_word, 8, 8) + " txstatus=" + std::to_string(txstatus));
					if (ord.OrdAction != KOrderAction::ACTION_CXL)
						memcpy(&ord.exchordid, &ordidQw, sizeof(quad_word));
					// Same txstatus handling as above
					switch (txstatus)
					{
					case 2:
						ord.OrdStatus = KOrderStatus::FILLED; ord.OrdState = KOrderState::COMPLETE;
						ord.lastqty = ord.quantity; ord.leavesqty = 0; ord.fillqty = ord.quantity;
						ord.lastpx = ord.price;
						{ uint64_t s = _fillSeq.fetch_add(1, std::memory_order_relaxed);
						  snprintf(ord.execid, sizeof(ord.execid), "OSE-%" PRIu64 "-%" PRIu64, ord.exchordid, s); }
						break;
					case 4: ord.OrdStatus = KOrderStatus::NEW; ord.OrdState = KOrderState::WORKING; break;
					case 6:
						ord.OrdStatus = KOrderStatus::PARTIALLY_FILLED; ord.OrdState = KOrderState::WORKING;
						ord.lastpx = ord.price;
						{ uint64_t s = _fillSeq.fetch_add(1, std::memory_order_relaxed);
						  snprintf(ord.execid, sizeof(ord.execid), "OSE-%" PRIu64 "-%" PRIu64, ord.exchordid, s); }
						break;
					case 1: case 17: case 19: ord.OrdStatus = KOrderStatus::CANCELED; ord.OrdState = KOrderState::COMPLETE; break;
					default:
						if (ord.OrdAction == KOrderAction::ACTION_CXL) { ord.OrdStatus = KOrderStatus::CANCELED; ord.OrdState = KOrderState::COMPLETE; }
						else if (ord.OrdAction == KOrderAction::ACTION_MOD) { ord.OrdStatus = KOrderStatus::MODIFIED; ord.OrdState = KOrderState::WORKING; }
						else { ord.OrdStatus = KOrderStatus::NEW; ord.OrdState = KOrderState::WORKING; }
						break;
					}
					_responseQueue.enqueue(ord);
					return;
				}
				KT01_LOG_ERROR(_logTag, "Retry TX also failed: status=" + std::to_string(retry));
			}
		}

		ord.OrdStatus = KOrderStatus::REJECTED;
		ord.OrdState = KOrderState::COMPLETE;
		memset(ord.text, 0, sizeof(ord.text));
		snprintf(ord.text, sizeof(ord.text) - 1, "omniapi_tx_ex=%d txstatus=%d", status, txstatus);
		_responseQueue.enqueue(ord);
	}
}

bool OmnetWorker::QuerySeries()
{
	// Series mapping is now done in QueryInstruments (DQ3) which returns
	// series_t + name_s for each instrument. DQ22 is not supported on J-GATE.
	KT01_LOG_INFO(_logTag, "QuerySeries: series mapping deferred to DQ3");
	_seriesCache.clear();
	_secidToSeriesIdx.clear();
	return true;
}

bool OmnetWorker::QueryInstruments()
{
	KT01_LOG_INFO(_logTag, "Querying instruments (DQ3) and building series cache...");

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
			KT01_LOG_ERROR(_logTag, "DQ3 failed: status=" + std::to_string(status));
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
					KT01_LOG_INFO(_logTag, "DQ3 mapped: '" + std::string(name) +
					              "' → obid=" + std::to_string(obid) +
					              " commodity=" + std::to_string(hostSeries.commodity_n) +
					              " expiry=" + std::to_string(hostSeries.expiration_date_n));
				}
				else if (_sett.DebugAppMsgs)
				{
					KT01_LOG_INFO(_logTag, "DQ3 unmapped: '" + std::string(name) +
					              "' commodity=" + std::to_string(hostSeries.commodity_n) +
					              " expiry=" + std::to_string(hostSeries.expiration_date_n));
				}
			}
		}

		if (_sett.DebugAppMsgs)
		{
			KT01_LOG_INFO(_logTag, "DQ3 segment " + std::to_string(segment) +
			              ": " + std::to_string(items) + " instruments");
		}

		// Last segment if fewer than 100 items
		if (items < 100)
			break;

		++segment;
	}

	KT01_LOG_INFO(_logTag, "DQ3 complete: " + std::to_string(totalInstruments) +
	              " instruments, " + std::to_string(mappedCount) + " mapped to SecMaster");
	return true;
}

bool OmnetWorker::QueryInstrumentSeries()
{
	KT01_LOG_INFO(_logTag, "Querying instrument series (DQ124) -- building series cache...");

	_seriesCache.clear();
	_secidToSeriesIdx.clear();

	// Build DQ124 query
	query_delta_t query;
	memset(&query, 0, sizeof(query));
	query.transaction_type.central_module_c = 'D';
	query.transaction_type.server_type_c = 'Q';
	PUTSHORT(query.transaction_type.transaction_number_n, 124);

	// country_c = 96 (Japan Exchange Group), rest = 0 (all markets/instruments)
	query.series.country_c = 96;

	// Full answer: download_ref_number = -1 (NO_VALUE)
	int64_t noValue = -1;
	PutInt64(query.download_ref_number_q, noValue);

	// full_answer_timestamp = 0 (first-time full download)
	query.full_answer_timestamp.tv_sec = 0;
	query.full_answer_timestamp.tv_nsec = 0;

	// Answer buffer -- OMnet API max is MAX_RESPONSE_SIZE (64000)
	static constexpr size_t ANS_BUF_SIZE = MAX_RESPONSE_SIZE;
	std::vector<char> ansBuf(ANS_BUF_SIZE);

	uint16_t segment = 1;
	int totalSeries = 0;
	int mappedCount = 0;

	while (true)
	{
		PUTSHORT(query.segment_number_n, segment);
		size_t ansLen = ANS_BUF_SIZE;

		int status = _session.SendQuery(&query, sizeof(query), ansBuf.data(), ansLen,
		                                _session.GetFacilityEP0());

		if (status != OMNIAPI_SUCCESS)
		{
			KT01_LOG_ERROR(_logTag, "DQ124 failed: status=" + std::to_string(status) +
			               " segment=" + std::to_string(segment));
			return false;
		}

		const char* base = ansBuf.data();
		const char* end = base + ansLen;

		if (ansLen < sizeof(answer_segment_hdr_t))
		{
			KT01_LOG_ERROR(_logTag, "DA124 answer too small: " + std::to_string(ansLen));
			return false;
		}

		const answer_segment_hdr_t* hdr = (const answer_segment_hdr_t*)base;
		uint16_t itemCount;
		PUTSHORT(itemCount, hdr->items_n);
		uint16_t msgSize;
		PUTSHORT(msgSize, hdr->size_n);
		uint16_t ansSegment;
		PUTSHORT(ansSegment, hdr->segment_number_n);

		if (_sett.DebugAppMsgs)
		KT01_LOG_INFO(_logTag, "DA124 seg=" + std::to_string(ansSegment) +
		              " items=" + std::to_string(itemCount) +
		              " size=" + std::to_string(msgSize) +
		              " ansLen=" + std::to_string(ansLen));

		const char* ptr = base + sizeof(answer_segment_hdr_t);

		// VIM layout: each item has an item_hdr_t wrapper
		//   item[0] = delta_header (skip)
		//   item[1..N] = series (each contains sub_items: 37301, 37302, 37310, etc.)
		// item_hdr_t.size_n includes the item_hdr itself (4 bytes)
		// sub_item_hdr_t.size_n includes the sub_item_hdr itself (4 bytes)

		for (uint16_t item = 0; item < itemCount && ptr + sizeof(item_hdr_t) <= end; ++item)
		{
			const item_hdr_t* ihdr = (const item_hdr_t*)ptr;
			uint16_t iSize;
			PUTSHORT(iSize, ihdr->size_n);

			if (iSize < sizeof(item_hdr_t) || ptr + iSize > end)
				break;

			const char* itemEnd = ptr + iSize;
			ptr = itemEnd; // advance to next item

			if (item == 0)
			{
				if (segment == 1)
				{
					uint16_t iItems;
					PUTSHORT(iItems, ihdr->items_n);
					KT01_LOG_INFO(_logTag, "DA124 item[0] (delta): items_n=" +
					              std::to_string(iItems) + " size_n=" + std::to_string(iSize));
				}
				continue; // skip delta_header item
			}

			// Debug first few items of first segment
			if (segment == 1 && item <= 2)
			{
				uint16_t iItems;
				PUTSHORT(iItems, ihdr->items_n);
				KT01_LOG_INFO(_logTag, "DA124 item[" + std::to_string(item) +
				              "]: items_n=" + std::to_string(iItems) +
				              " size_n=" + std::to_string(iSize) +
				              " offset=" + std::to_string((const char*)ihdr - base));
			}

			// Parse sub_items within this series item
			series_t curSeries = {};
			char curInsId[33] = {};
			int32_t curObid = 0;
			bool curHasSeries = false;
			bool curHasObid = false;

			const char* subPtr = ((const char*)ihdr) + sizeof(item_hdr_t);
			while (subPtr + sizeof(sub_item_hdr_t) <= itemEnd)
			{
				const sub_item_hdr_t* subHdr = (const sub_item_hdr_t*)subPtr;
				uint16_t nsId;
				PUTSHORT(nsId, subHdr->named_struct_n);
				uint16_t nsSize;
				PUTSHORT(nsSize, subHdr->size_n);

				// size_n includes the sub_item_hdr_t itself (4 bytes)
				if (nsSize < sizeof(sub_item_hdr_t) || subPtr + nsSize > itemEnd)
					break;

				const char* nsData = subPtr + sizeof(sub_item_hdr_t);
				uint16_t dataSize = nsSize - sizeof(sub_item_hdr_t);

				subPtr += nsSize;

				switch (nsId)
				{
				case NS_INST_SERIES_BASIC: // 37301
				{
					if (dataSize >= sizeof(ns_inst_series_basic_t))
					{
						const ns_inst_series_basic_t* basic = (const ns_inst_series_basic_t*)nsData;
						curSeries = basic->series;
						PUTSHORT(curSeries.commodity_n, curSeries.commodity_n);
						PUTSHORT(curSeries.expiration_date_n, curSeries.expiration_date_n);
						PUTLONG(curSeries.strike_price_i, curSeries.strike_price_i);

						memcpy(curInsId, basic->ins_id_s, 32);
						curInsId[32] = '\0';
						for (int j = 31; j >= 0 && curInsId[j] == ' '; --j)
							curInsId[j] = '\0';

						curHasSeries = true;
					}
					break;
				}
				case NS_INST_SERIES_ID: // 37310 — orderbook_id
				{
					if (dataSize >= sizeof(ns_inst_series_id_t))
					{
						const ns_inst_series_id_t* sid = (const ns_inst_series_id_t*)nsData;
						PUTLONG(curObid, sid->orderbook_id_i);
						curHasObid = true;
					}
					break;
				}
				default:
					break;
				}
			}

			// Finalize this series — only keep if in SecMaster (secdef CSV)
			if (curHasSeries && curHasObid && curObid > 0)
			{
				uint32_t obid = static_cast<uint32_t>(curObid);

				uint64_t seriesKey = ((uint64_t)curSeries.country_c << 56) |
				                     ((uint64_t)curSeries.market_c << 48) |
				                     ((uint64_t)curSeries.instrument_group_c << 40) |
				                     ((uint64_t)curSeries.modifier_c << 32) |
				                     ((uint64_t)curSeries.commodity_n << 16) |
				                     ((uint64_t)curSeries.expiration_date_n);

				// Always register seriesKey → obid (needed for BO5/EB10 lookups)
				OseSecMaster::instance().RegisterSeriesKey(seriesKey, obid);

				// Only cache series that are in our secdef
				if (OseSecMaster::instance().Contains(obid))
				{
					SeriesInfo si;
					si.series = curSeries;
					memcpy(si.name, curInsId, 32);

					_seriesCache.push_back(si);
					_secidToSeriesIdx[obid] = _seriesCache.size() - 1;
					++mappedCount;
				}
			}

			if (totalSeries < 10 && curHasSeries)
			{
				KT01_LOG_INFO(_logTag, "DQ124[" + std::to_string(totalSeries) + "]: '" +
				              std::string(curInsId) + "' obid=" + std::to_string(curObid) +
				              " commodity=" + std::to_string(curSeries.commodity_n) +
				              " expiry=" + std::to_string(curSeries.expiration_date_n));
			}

			if (curHasSeries) totalSeries++;
		}

		if (_sett.DebugAppMsgs || segment == 1)
		KT01_LOG_INFO(_logTag, "DA124 seg " + std::to_string(segment) +
		              " done: " + std::to_string(totalSeries) + " series so far, " +
		              std::to_string(mappedCount) + " mapped");

		if (ansSegment == 0)
			break;

		++segment;
	}

	KT01_LOG_INFO(_logTag, "DQ124 complete: " + std::to_string(totalSeries) +
	              " series, " + std::to_string(mappedCount) + " mapped (with orderbook_id)");
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

void OmnetWorker::CopySeriesCache(const OmnetWorker& src)
{
	_seriesCache = src._seriesCache;
	_secidToSeriesIdx = src._secidToSeriesIdx;
	KT01_LOG_INFO(_logTag, "Copied series cache from Worker[0]: " +
	              std::to_string(_seriesCache.size()) + " entries");
}

void OmnetWorker::PopulateSeries(KTN::OrderPod& ord, const series_t* series)
{
	if (!series)
	{
		KT01_LOG_ERROR(_logTag, "Series not found for secid=" + std::to_string(ord.secid));
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
		KT01_LOG_INFO(_logTag, "UI1 Ready-to-Trade sent successfully");
		return true;
	}

	KT01_LOG_ERROR(_logTag, "UI1 Ready-to-Trade failed: " + std::to_string(status));
	return false;
}

bool OmnetWorker::QuerySettlement()
{
	KT01_LOG_INFO(_logTag, "Querying settlement prices (RQ62)...");

	// Collect unique market codes from series cache
	std::set<uint8_t> markets;
	for (const auto& si : _seriesCache)
		markets.insert(si.series.market_c);

	if (markets.empty())
	{
		KT01_LOG_ERROR(_logTag, "RQ62: no markets in series cache — cannot query");
		return false;
	}

	// Business date = current date in JST (UTC+9)
	char bizDate[8] = {};
	{
		auto now = std::chrono::system_clock::now();
		auto utc_time = std::chrono::system_clock::to_time_t(now);
		struct tm jst = {};
		utc_time += 9 * 3600; // UTC → JST
		gmtime_r(&utc_time, &jst);
		snprintf(bizDate, sizeof(bizDate) + 1, "%04d%02d%02d",
		         jst.tm_year + 1900, jst.tm_mon + 1, jst.tm_mday);
		KT01_LOG_INFO(_logTag, "RQ62 using JST business_date=" + std::string(bizDate, 8));
	}

	KT01_LOG_INFO(_logTag, "RQ62: querying " + std::to_string(markets.size()) +
	              " market(s) for date=" + std::string(bizDate, 8));

	// EP4 = EP0 base + 4
	uint32_t ep4 = _session.GetFacilityEP0() + 4;

	static constexpr size_t ANS_BUF_SIZE = MAX_RESPONSE_SIZE;
	std::vector<char> ansBuf(ANS_BUF_SIZE);
	int totalItems = 0;

	// Collect DailyStats for DB writing
	uint32_t tradeDate = SettlementDbWriter::BizDateToDaysSinceEpoch(bizDate);
	std::vector<DailyStat> dbStats;

	for (uint8_t mkt : markets)
	{
	query_price_vola_settl_t qry;
	memset(&qry, 0, sizeof(qry));
	qry.transaction_type.central_module_c = 'R';
	qry.transaction_type.server_type_c = 'Q';
	PUTSHORT(qry.transaction_type.transaction_number_n, 62);
	qry.series.country_c = 96;
	qry.series.market_c = mkt;
	memcpy(qry.date_s, bizDate, 8);

	uint16_t segment = 1;

	while (true)
	{
		PUTSHORT(qry.segment_number_n, segment);
		size_t ansLen = ANS_BUF_SIZE;

		int status = _session.SendQuery(&qry, sizeof(qry), ansBuf.data(), ansLen, ep4);

		if (status != OMNIAPI_SUCCESS)
		{
			KT01_LOG_ERROR(_logTag, "RQ62 failed for market=" + std::to_string(mkt) +
			               " status=" + std::to_string(status) +
			               " (settlement may not be ready yet — requires BI7 type 100)");
			break; // next segment won't help, try next market
		}

		// Parse answer: answer_price_vola_settl_t header + items
		const char* base = ansBuf.data();
		const answer_price_vola_settl_t* ans = (const answer_price_vola_settl_t*)base;

		uint16_t itemCount;
		PUTSHORT(itemCount, ans->items_n);
		uint16_t segNum;
		PUTSHORT(segNum, ans->segment_number_n);

		if (segment == 1 && _sett.DebugAppMsgs)
		{
			KT01_LOG_INFO(_logTag, "RQ62 market=" + std::to_string(mkt) +
			              " type=" + std::string(SettlementPriceTypeName(ans->settlement_price_type_c)));
		}

		for (uint16_t i = 0; i < itemCount; ++i)
		{
			const answer_price_vola_settl_item_t& item = ans->item[i];

			// Byte-swap series fields
			uint16_t commodity, expiry;
			PUTSHORT(commodity, item.series.commodity_n);
			PUTSHORT(expiry, item.series.expiration_date_n);

			int32_t settlePrice, lastPrice, theoPrice, actualVol;
			PUTLONG(settlePrice, item.settle_price_i);
			PUTLONG(lastPrice, item.last_price_i);
			PUTLONG(theoPrice, item.theo_price_i);
			PUTLONG(actualVol, item.actual_vol_i);

			// Lookup symbol name from SecMaster
			std::string sym = SettlementCache::ResolveSymbol(
				item.series.country_c, item.series.market_c,
				item.series.instrument_group_c, item.series.modifier_c,
				commodity, expiry);

			// Store in cache
			SettlementData sd;
			memset(&sd, 0, sizeof(sd));
			sd.series = item.series;
			sd.series.commodity_n = commodity;
			sd.series.expiration_date_n = expiry;
			if (!sym.empty())
				strncpy(sd.name, sym.c_str(), sizeof(sd.name) - 1);
			sd.settle_price = settlePrice;
			sd.last_price = lastPrice;
			sd.theo_price = theoPrice;
			sd.implied_vol = 0;  // not in RQ62 answer struct
			sd.actual_vol = actualVol;
			sd.settlement_price_type = ans->settlement_price_type_c;
			sd.received_at = std::chrono::steady_clock::now();

			uint32_t key = SettlementCache::MakeKey(commodity, expiry);
			_settlCache.Update(key, sd);

			if (!sym.empty())
			{
				// Skip options — we only trade futures
				auto& sm = OseSecMaster::instance();
				uint32_t obid = sm.GetOrderbookIdBySeriesKey(
					((uint64_t)item.series.country_c << 56) |
					((uint64_t)item.series.market_c << 48) |
					((uint64_t)item.series.instrument_group_c << 40) |
					((uint64_t)item.series.modifier_c << 32) |
					((uint64_t)commodity << 16) | (uint64_t)expiry);
				if (obid != 0 && sm.Contains(obid) &&
				    sm.getSecDef(obid).prodtype != KOrderProdType::OPTION)
				{
					KT01_LOG_INFO(_logTag, "RQ62 settle: " + sym +
					              " settle=" + SettlementCache::PriceStr(settlePrice) +
					              " last=" + SettlementCache::PriceStr(lastPrice) +
					              " type=" + std::string(SettlementPriceTypeName(ans->settlement_price_type_c)));

					// Collect for DB write
					if (settlePrice != SettlementCache::NO_VALUE)
					{
						DailyStat stat = {};
						stat.Symbol = sym;
						stat.SecurityID = static_cast<int32_t>(obid);
						stat.Price = static_cast<double>(settlePrice);
						stat.Size = 0;
						stat.UpdateType = '6';
						stat.EntryType = "SettlementPrice";
						stat.TradeDate = tradeDate;
						stat.RptSeq = 0;
						dbStats.push_back(stat);
					}
				}
			}

			totalItems++;
		}

		// Check if more segments
		if (itemCount == 0)
			break;
		segment++;
	} // while segments

	} // for each market

	KT01_LOG_INFO(_logTag, "RQ62 complete: " + std::to_string(totalItems) +
	              " settlement entries loaded into cache");

	if (totalItems > 0)
	{
		_settlCache.Print();
		KTN::notify::NotifierRest::NotifyInfo(_sett.ExchName, _sett.Source, _sett.Org,
			"Settlement query: " + std::to_string(totalItems) + " prices loaded");
	}

	// Write to DB
	if (!dbStats.empty())
	{
		std::string table = _sett.SettlementTable;
		if (table.empty())
			table = "stats_daily";

		KT01_LOG_INFO(_logTag, "Writing " + std::to_string(dbStats.size()) +
		              " settlement prices to DB table=" + table);
		_settlDbWriter.WriteDailyStats(dbStats, table);
	}

	return totalItems > 0;
}

bool OmnetWorker::Reconnect()
{
	_readyToTrade.store(false, std::memory_order_release);
	_session.Logout();

	// Exponential backoff (CFE/EQT style: 15s, 30s, 60s, 120s, 240s, 300s cap)
	int backoffIdx = std::min(_reconnectAttempt, RECONNECT_BACKOFF_COUNT - 1);
	int waitMs = RECONNECT_BACKOFF_MS[backoffIdx];
	KT01_LOG_INFO(_logTag, "Reconnect attempt " + std::to_string(_reconnectAttempt + 1) +
	              ", waiting " + std::to_string(waitMs / 1000) + "s...");

	// Sleep in 1s increments so we can bail if _active goes false
	auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(waitMs);
	while (std::chrono::steady_clock::now() < deadline && _active.load(std::memory_order_acquire))
		std::this_thread::sleep_for(std::chrono::seconds(1));

	if (!_active.load(std::memory_order_acquire))
		return false;

	// Re-login (force login to break any stale session lock)
	if (!_session.Login(_creds, true))
	{
		KT01_LOG_ERROR(_logTag, "Reconnect login failed (attempt " +
		               std::to_string(_reconnectAttempt + 1) + ")");
		_reconnectAttempt++;
		return false;
	}

	// Re-query instruments and series (only Worker[0] — others already have cache)
	if (_workerId == 0)
	{
		QueryInstruments();
		QueryInstrumentSeries();
	}

	// Re-subscribe
	_session.SubscribeAll();

	// Re-send ready-to-trade
	if (!SendReadyToTrade())
	{
		KT01_LOG_ERROR(_logTag, "Reconnect UI1 failed (attempt " +
		               std::to_string(_reconnectAttempt + 1) + ")");
		_reconnectAttempt++;
		return false;
	}

	// Success — reset counters
	_reconnectAttempt = 0;
	_notified_session_lost = false;
	KT01_LOG_INFO(_logTag, "Reconnect successful — session restored");
	KTN::notify::NotifierRest::NotifyInfo(_sett.ExchName, _sett.Source, _sett.Org,
		"Worker #" + std::to_string(_workerId + 1) + " RECONNECTED");
	return true;
}

// TODO: If production has BD6 broadcast (event types beyond 1,1001),
// handle BD6 in BDX thread to update fills with real match_id + deal_price.

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

	// Price: Internal Price stores ITCH value (×10^4) as unshifted.
	// AsShifted() = ITCH_price × 10^9. Divide by Multiplier(10^9) to get premium_i.
	int32_t price = static_cast<int32_t>(OSEConverter::ToWire(ord.price));
	PUTLONG(trans->order_var.premium_i, price);

	// Side: OMnet uses 1=Buy, 2=Sell
	trans->order_var.bid_or_ask_c = (ord.OrdSide == KOrderSide::BUY) ? 1 : 2;

	// Block number (always 1 for single orders)
	uint32_t block = 1;
	PUTLONG(trans->order_var.block_n, block);

	// Open/close: 0 = not specified
	trans->order_var.open_close_req_c = 0;

	// Time validity (OMnet format: 0x0100=Day, 0x0200=GTC, 0x0300=IOC, 0x0400=FOK)
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

	// Total volume: 0 = normal order (no hidden volume). Per MessRef p99.
	// Do NOT set to qty — that triggers "hidden volume not allowed" on non-iceberg instruments.
	trans->total_volume_i = 0; // already 0 from memset, explicit for clarity

	// Customer info (ex_client) — store our order request ID for correlation
	snprintf(trans->order_var.ex_client_s, sizeof(trans->order_var.ex_client_s),
	         "%lu", (unsigned long)ord.orderReqId);

	// Exchange info (ose_exchange_info_t overlay)
	ose_exchange_info_t* exInfo = (ose_exchange_info_t*)trans->exchange_info_s;
	exInfo->acc_type_c = '0';          // '0'=Client, '9'=House (per MessRef p284)
	exInfo->tr_purpose_flag_c = ' ';   // must be space for orders

	if (_sett.DebugAppMsgs)
	{
		KT01_LOG_INFO(_logTag, "MO31: side=" + std::to_string(trans->order_var.bid_or_ask_c) +
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

	// Order number — OMnet keeps same order_number after alter
	memcpy(&trans->order_number_u, &ord.exchordid, sizeof(quad_word));

	// bid_or_ask_c — REQUIRED for order lookup (order_number + series + side)
	trans->order_var.bid_or_ask_c = (ord.OrdSide == KOrderSide::BUY) ? 1 : 2;

	// Fields to change (0 = don't change, except premium: INT_MIN = don't change)
	int64_t qty = ord.quantity;
	PutInt64(trans->order_var.mp_quantity_i, qty);

	int32_t price = static_cast<int32_t>(OSEConverter::ToWire(ord.price));
	PUTLONG(trans->order_var.premium_i, price);

	// delta_quantity_c — REQUIRED: 1 = absolute qty, 2 = delta qty
	trans->delta_quantity_c = 1;

	// exchange_info: first byte 0 = preserve original (don't change)
	// All other order_var fields left as 0 = "don't change"

	if (_sett.DebugAppMsgs)
	{
		KT01_LOG_INFO(_logTag, "MO33: price=" + std::to_string(price) + " qty=" + std::to_string(qty) +
		              " side=" + std::to_string(trans->order_var.bid_or_ask_c) +
		              " delta_qty_c='" + std::string(1, trans->delta_quantity_c) + "'" +
		              " ordid=" + HexDump((const char*)&trans->order_number_u, 8, 8));
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

	// Order number — OMnet keeps same order_number after alter
	memcpy(&trans->order_number_u, &ord.exchordid, sizeof(quad_word));

	// Side: 0=both (mass cancel), 1=buy, 2=sell
	trans->bid_or_ask_c = (ord.OrdSide == KOrderSide::BUY) ? 1 : 2;

	// Customer info
	snprintf(trans->customer_info_s, sizeof(trans->customer_info_s),
	         "%lu", (unsigned long)ord.orderReqId);

	// Exchange info (ose_exchange_info_t overlay)
	ose_exchange_info_t* exInfo = (ose_exchange_info_t*)trans->exchange_info_s;
	exInfo->acc_type_c = '0';          // '0'=Client, '9'=House
	exInfo->tr_purpose_flag_c = ' ';   // must be space

	if (_sett.DebugAppMsgs)
	{
		KT01_LOG_INFO(_logTag, "MO4: side=" + std::to_string(trans->bid_or_ask_c) +
		              " exchordid=" + std::to_string(ord.exchordid));
	}

	return sizeof(delete_trans_t);
}

} // namespace KTN::OSE
