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

	// OMnet worker (order submission) + BDX thread (broadcast polling)
	std::unique_ptr<OmnetWorker> _worker;
	std::unique_ptr<OmnetBdxThread> _bdx;

	// SPSC queues (lock-free, cache-aligned)
	SPSCRingBuffer<KTN::OrderPod> _orderQueue;     // Strategy → Worker
	SPSCRingBuffer<KTN::OrderPod> _responseQueue;   // BDX/Worker → OseGT

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
