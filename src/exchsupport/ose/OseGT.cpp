//============================================================================
// Name        	: OseGT.cpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
//============================================================================

#include "OseGT.hpp"
#include <KT01/Core/Log.hpp>
#include <KT01/SecDefs/OseSecMaster.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <Notifications/NotifierRest.hpp>
#include <chrono>

using namespace KT01::SECDEF::OSE;

#undef __CLASS__
#define __CLASS__ "OseGT"

namespace KTN::OSE {

OseGT::OseGT(tbb::concurrent_queue<KTN::OrderPod>& qords,
               const std::string& settingsfile, const std::string& source)
	: ExchangeBase2(settingsfile, source, qords, "OseGT")
	, _bdxResponseQueue(4096)
	, _started(false)
{
	// Load settings (two-level: exchange config + session credentials)
	_sett.Load(settingsfile);
	_sett.Source = source; // Pass source (e.g. "ODIN") for NotifierRest calls

	// Configure base state
	_State.EXCHNAME = _sett.ExchName;
	_State.ORG = _sett.Org;
	_State.SOURCE = source;
	_State.MEASURE = _sett.MeasureLatency;
	_State.USE_AUDIT = _sett.UseAuditTrail;
	_State.STATEFILE_DIR = _sett.StateFileDirectory;
	_State.DebugSession = _sett.DebugSession;
	_State.DebugAppMsgs = _sett.DebugAppMsgs;
	_State.DebugRecvBytes = _sett.DebugRecvBytes;
	_State.KEEPALIVE = _sett.KeepAliveInterval;

	// Load SecMaster (singleton — loads on first access)
	if (!OseSecMaster::instance().IsLoaded())
	{
		KT01_LOG_CRITICAL(__CLASS__, "FAILED TO LOAD OSE SECMASTER! FATAL!");
		std::exit(1);
	}
	KT01_LOG_INFO(__CLASS__, "OSE SecMaster loaded");

	int N = _sett.WorkerCount;
	KT01_LOG_INFO(__CLASS__, "OSE Exchange Handler initialized — " + std::to_string(N) + " worker(s)");
	for (int i = 0; i < N; ++i)
	{
		KT01_LOG_INFO(__CLASS__, "Worker[" + std::to_string(i) + "]: " +
		              _sett.WorkerSessions[i].LoginId + " @ " +
		              _sett.WorkerSessions[i].GatewayHost + ":" +
		              std::to_string(_sett.WorkerSessions[i].GatewayPort));
	}
	KT01_LOG_INFO(__CLASS__, "BDX: " + _sett.BdxSession.LoginId);

	// Create N workers + queues
	for (int i = 0; i < N; ++i)
	{
		_orderQueues.push_back(std::make_unique<SPSCRingBuffer<KTN::OrderPod>>(4096));
		_responseQueues.push_back(std::make_unique<SPSCRingBuffer<KTN::OrderPod>>(4096));
		_workers.push_back(std::make_unique<OmnetWorker>(
			i, *_orderQueues[i], *_responseQueues[i], _sett,
			_sett.WorkerSessions[i], _settlCache, _settlementReady, _settlementQueryReq));
	}

	// Create BDX thread (uses bdxResponseQueue, not worker response queues)
	_bdx = std::make_unique<OmnetBdxThread>(_bdxResponseQueue, _sett,
	                                         _settlCache, _settlementReady);

	// Start connections
	Start();
}

OseGT::~OseGT()
{
	Stop();
}

void OseGT::Start()
{
	if (_started.load())
		return;

	KT01_LOG_INFO(__CLASS__, "Starting OSE exchange handler...");

	// Start Worker[0] first — it does DQ3/DQ124 to build series cache
	if (!_workers[0]->Start())
	{
		KT01_LOG_ERROR(__CLASS__, "Failed to start worker[0]");
		KTN::notify::NotifierRest::NotifyError(_sett.ExchName, _sett.Source, _sett.Org, "OSE Worker[0] start failed");
		return;
	}

	// Start remaining workers — they copy series cache from Worker[0], skip DQ3/DQ124
	for (size_t i = 1; i < _workers.size(); ++i)
	{
		_workers[i]->CopySeriesCache(*_workers[0]);
		if (!_workers[i]->Start())
		{
			KT01_LOG_ERROR(__CLASS__, "Failed to start worker[" + std::to_string(i) + "]");
			KTN::notify::NotifierRest::NotifyError(_sett.ExchName, _sett.Source, _sett.Org,
				"OSE Worker[" + std::to_string(i) + "] start failed");
			return;
		}
	}

	// Start BDX thread (login + broadcast polling)
	if (!_bdx->Start())
	{
		KT01_LOG_ERROR(__CLASS__, "Failed to start BDX thread");
		KTN::notify::NotifierRest::NotifyError(_sett.ExchName, _sett.Source, _sett.Org, "OSE BDX start failed");
		return;
	}

	_started.store(true);
	KT01_LOG_INFO(__CLASS__, "OSE exchange handler started — " +
	              std::to_string(_workers.size()) + " worker(s) ready");
	KTN::notify::NotifierRest::NotifyInfo(_sett.ExchName, _sett.Source, _sett.Org,
		"OSE Exchange Handler STARTED (" + std::to_string(_workers.size()) + " workers)");
}

void OseGT::Stop()
{
	if (!_started.load())
		return;

	KT01_LOG_INFO(__CLASS__, "Stopping OSE exchange handler...");

	// Stop all workers first (finish pending orders)
	for (auto& w : _workers)
		if (w) w->Stop();

	// Then stop BDX thread
	if (_bdx)
		_bdx->Stop();

	_started.store(false);
	KT01_LOG_INFO(__CLASS__, "OSE exchange handler stopped");
}

void OseGT::Send(KTN::OrderPod& order, int action)
{
	// Check any worker is ready
	bool anyReady = false;
	for (auto& w : _workers)
		if (w->IsReady()) { anyReady = true; break; }

	if (!_started.load() || !anyReady)
	{
		KT01_LOG_ERROR(__CLASS__, "Cannot send order - not ready");
		KTN::notify::NotifierRest::NotifyError(_sett.ExchName, _sett.Source, _sett.Org, "Order rejected: exchange not ready");
		order.OrdStatus = KOrderStatus::REJECTED;
		return;
	}

	// Set action on order
	order.OrdAction = static_cast<KOrderAction::Enum>(action);

	// Round-robin to a ready worker
	uint32_t N = (uint32_t)_workers.size();
	uint32_t start = _nextWorker.fetch_add(1, std::memory_order_relaxed) % N;
	for (uint32_t attempt = 0; attempt < N; ++attempt)
	{
		uint32_t idx = (start + attempt) % N;
		if (_workers[idx]->IsReady() && _orderQueues[idx]->enqueue(order))
		{
			if (_sett.DebugAppMsgs)
			{
				KT01_LOG_INFO(__CLASS__, "Order enqueued: worker=" + std::to_string(idx) +
				              " action=" + std::to_string(action) +
				              " secid=" + std::to_string(order.secid));
			}
			return;
		}
	}

	// All queues full or no worker ready
	KT01_LOG_ERROR(__CLASS__, "All order queues full!");
	KTN::notify::NotifierRest::NotifyError(_sett.ExchName, _sett.Source, _sett.Org, "Order rejected: all queues full");
	order.OrdStatus = KOrderStatus::REJECTED;
}

void OseGT::SendMassAction(KTN::Order& order)
{
	// TODO: Implement mass cancel via MO4 with bid_or_ask=0 and order_number=0
	KT01_LOG_INFO(__CLASS__, "SendMassAction: TODO - implement mass cancel");
}

void OseGT::Poll()
{
	// Called by ExchangeManager main loop
	// Process any responses from BDX thread or worker thread
	ProcessResponses();
}

void OseGT::ProcessResponses()
{
	KTN::OrderPod ord;

	// Drain all worker response queues + BDX queue
	auto processOne = [&](KTN::OrderPod& ord)
	{
		// Set exchange identifier
		ord.OrdExch = KOrderExchange::OSE;

		// Set queue write time for latency tracking
		ord.queueWriteTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
		    std::chrono::high_resolution_clock::now().time_since_epoch()).count();

		// Log based on status
		switch (ord.OrdStatus)
		{
		case KOrderStatus::NEW:
			if (_State.DebugAppMsgs)
				KT01_LOG_INFO(__CLASS__, "Order ACK: reqid=" + std::to_string(ord.orderReqId) +
				              " secid=" + std::to_string(ord.secid));
			break;

		case KOrderStatus::MODIFIED:
			if (_State.DebugAppMsgs)
				KT01_LOG_INFO(__CLASS__, "Order MOD: reqid=" + std::to_string(ord.orderReqId) +
				              " price=" + std::to_string(ord.price.AsShifted()) +
				              " qty=" + std::to_string(ord.quantity));
			break;

		case KOrderStatus::FILLED:
		case KOrderStatus::PARTIALLY_FILLED:
			if (_State.DebugAppMsgs)
				KT01_LOG_INFO(__CLASS__, "Execution: reqid=" + std::to_string(ord.orderReqId) +
				              " lastqty=" + std::to_string(ord.lastqty) +
				              " leaves=" + std::to_string(ord.leavesqty));
			break;

		case KOrderStatus::CANCELED:
			if (_State.DebugAppMsgs)
				KT01_LOG_INFO(__CLASS__, "Cancel: reqid=" + std::to_string(ord.orderReqId));
			break;

		case KOrderStatus::REJECTED:
			KT01_LOG_ERROR(__CLASS__, "Reject: reqid=" + std::to_string(ord.orderReqId) +
			               " reason=" + std::string(ord.text));
			KTN::notify::NotifierRest::NotifyError(_sett.ExchName, _sett.Source, _sett.Org,
				"Order rejected: reqid=" + std::to_string(ord.orderReqId) + " " + std::string(ord.text));
			break;

		default:
			if (_State.DebugAppMsgs)
				KT01_LOG_INFO(__CLASS__, "Response status=" + std::to_string((int)ord.OrdStatus));
			break;
		}

		// Push to upper layer via tbb queue (same pattern as CME/CFE)
		_qOrdsProc.push(ord);
	};

	// Worker response queues
	for (auto& q : _responseQueues)
		while (q->dequeue(ord))
			processOne(ord);

	// BDX response queue
	while (_bdxResponseQueue.dequeue(ord))
		processOne(ord);
}

void OseGT::Command(Instruction cmd)
{
	switch (cmd.command)
	{
	case ExchCmd::QUERY_SETTLEMENT:
	{
		KT01_LOG_INFO(__CLASS__, "Manual settlement query requested (RQ62)");
		_settlementQueryReq.store(true, std::memory_order_release);
	}
	break;

	default:
		KT01_LOG_INFO(__CLASS__, "Unhandled command: " + ExchCmd::toString(cmd.command));
		break;
	}
}

} // namespace KTN::OSE
