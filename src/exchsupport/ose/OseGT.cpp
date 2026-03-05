//============================================================================
// Name        	: OseGT.cpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
//============================================================================

#include "OseGT.hpp"
#include <KT01/Core/Log.hpp>
#include <KT01/SecDefs/OseSecMaster.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <chrono>

using namespace KT01::SECDEF::OSE;

#undef __CLASS__
#define __CLASS__ "OseGT"

namespace KTN::OSE {

OseGT::OseGT(tbb::concurrent_queue<KTN::OrderPod>& qords,
               const std::string& settingsfile, const std::string& source)
	: ExchangeBase2(settingsfile, source, qords, "OseGT")
	, _orderQueue(4096)
	, _responseQueue(4096)
	, _started(false)
{
	// Load settings (two-level: exchange config + session credentials)
	_sett.Load(settingsfile);

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

	KT01_LOG_INFO(__CLASS__, "OSE Exchange Handler initialized");
	KT01_LOG_INFO(__CLASS__, "Gateway: " + _sett.GatewayHost + ":" + std::to_string(_sett.GatewayPort));
	KT01_LOG_INFO(__CLASS__, "User: " + _sett.LoginId);
	KT01_LOG_INFO(__CLASS__, "Workers: " + std::to_string(_sett.WorkerCount));

	// Create worker and BDX threads
	_worker = std::make_unique<OmnetWorker>(_orderQueue, _responseQueue, _sett);
	_bdx = std::make_unique<OmnetBdxThread>(_responseQueue, _sett);

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

	// Start worker (login + order submission thread)
	if (!_worker->Start())
	{
		KT01_LOG_ERROR(__CLASS__, "Failed to start worker");
		return;
	}

	// Pre-trade activities
	_worker->QuerySeries();
	_worker->QueryInstruments();
	_worker->SendReadyToTrade();

	// Start BDX thread (login + broadcast polling)
	if (!_bdx->Start())
	{
		KT01_LOG_ERROR(__CLASS__, "Failed to start BDX thread");
		return;
	}

	_started.store(true);
	KT01_LOG_INFO(__CLASS__, "OSE exchange handler started successfully");
}

void OseGT::Stop()
{
	if (!_started.load())
		return;

	KT01_LOG_INFO(__CLASS__, "Stopping OSE exchange handler...");

	// Stop worker first (finish pending orders)
	if (_worker)
		_worker->Stop();

	// Then stop BDX thread
	if (_bdx)
		_bdx->Stop();

	_started.store(false);
	KT01_LOG_INFO(__CLASS__, "OSE exchange handler stopped");
}

void OseGT::Send(KTN::OrderPod& order, int action)
{
	if (!_started.load() || !_worker->IsReady())
	{
		KT01_LOG_ERROR(__CLASS__, "Cannot send order - not ready");
		order.OrdStatus = KOrderStatus::REJECTED;
		return;
	}

	// Set action on order
	order.OrdAction = static_cast<KOrderAction::Enum>(action);

	// Enqueue to SPSC queue for worker thread
	if (!_orderQueue.enqueue(order))
	{
		KT01_LOG_ERROR(__CLASS__, "Order queue full!");
		order.OrdStatus = KOrderStatus::REJECTED;
		return;
	}

	if (_sett.DebugAppMsgs)
	{
		KT01_LOG_INFO(__CLASS__, "Order enqueued: action=" + std::to_string(action) +
		              " secid=" + std::to_string(order.secid));
	}
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

	while (_responseQueue.dequeue(ord))
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
			break;

		default:
			if (_State.DebugAppMsgs)
				KT01_LOG_INFO(__CLASS__, "Response status=" + std::to_string((int)ord.OrdStatus));
			break;
		}

		// Push to upper layer via tbb queue (same pattern as CME/CFE)
		_qOrdsProc.push(ord);
	}
}

} // namespace KTN::OSE
