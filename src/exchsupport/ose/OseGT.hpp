//============================================================================
// Name        	: OseGT.hpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
// Description 	: OSE Exchange Handler using OMnet API
//============================================================================

#ifndef SRC_EXCHSUPPORT_OSE_OSEGT_HPP_
#define SRC_EXCHSUPPORT_OSE_OSEGT_HPP_

#pragma once

#include <ExchangeHandler/ExchangeBase2.hpp>
#include <Maps/SPSCRingBuffer.hpp>
#include <Orders/OrderPod.hpp>
#include "OmnetWorker.hpp"
#include "OmnetBdxThread.hpp"
#include "SettlementCache.hpp"
#include "settings/OseSessionSettings.hpp"

namespace KTN::OSE {

using KTN::Core::SPSCRingBuffer;

class OseGT : public ExchangeBase2
{
public:
	explicit OseGT(tbb::concurrent_queue<KTN::OrderPod>& qords,
	               const std::string& settingsfile, const std::string& source);
	~OseGT();

	// ExchangeBase2 pure virtuals
	void Stop() override;
	void Send(KTN::OrderPod& order, int action) override;
	void SendMassAction(KTN::Order& order) override;
	void Poll() override;
	void Command(Instruction cmd) override;

private:
	OseSessionSettings _sett;

	// OMnet workers (order submission) + BDX thread (broadcast polling)
	std::vector<std::unique_ptr<OmnetWorker>> _workers;
	std::unique_ptr<OmnetBdxThread> _bdx;

	// Per-worker SPSC queues (lock-free, no contention)
	std::vector<std::unique_ptr<SPSCRingBuffer<KTN::OrderPod>>> _orderQueues;
	std::vector<std::unique_ptr<SPSCRingBuffer<KTN::OrderPod>>> _responseQueues;
	SPSCRingBuffer<KTN::OrderPod> _bdxResponseQueue; // BDX → OseGT

	// Round-robin worker selection
	std::atomic<uint32_t> _nextWorker{0};

	// Settlement cache (shared between Worker + BDX)
	SettlementCache _settlCache;
	std::atomic<bool> _settlementReady{false};   // Set by BDX on BI7 type 100 → auto RQ62
	std::atomic<bool> _settlementQueryReq{false}; // Set by menu Command → manual RQ62

	// State
	std::atomic<bool> _started;

	void Start();
	void ProcessResponses();
};

} // namespace KTN::OSE

#endif /* SRC_EXCHSUPPORT_OSE_OSEGT_HPP_ */
