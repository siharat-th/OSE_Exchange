//============================================================================
// Name        	: OmnetBdxThread.hpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
// Description 	: Broadcast (BDX) polling thread for BO5 order events
//============================================================================

#ifndef SRC_EXCHSUPPORT_OSE_OMNETBDXTHREAD_HPP_
#define SRC_EXCHSUPPORT_OSE_OMNETBDXTHREAD_HPP_

#pragma once

#include <thread>
#include <atomic>
#include <Maps/SPSCRingBuffer.hpp>
#include <Orders/OrderPod.hpp>
#include "OmnetSession.hpp"
#include "SettlementCache.hpp"
#include "settings/OseSessionSettings.hpp"

namespace KTN::OSE {

using KTN::Core::SPSCRingBuffer;
using namespace KTN::ORD;

class OmnetBdxThread
{
public:
	OmnetBdxThread(SPSCRingBuffer<KTN::OrderPod>& responseQueue,
	               const OseSessionSettings& sett,
	               SettlementCache& settlCache,
	               std::atomic<bool>& settlementReady);
	~OmnetBdxThread();

	bool Start();
	void Stop();

private:
	OmnetSession _session;
	SPSCRingBuffer<KTN::OrderPod>& _responseQueue;
	const OseSessionSettings& _sett;
	std::thread _thread;
	std::atomic<bool> _active;
	bool _notified_overflow = false; // Prevent flood — notify once
	std::atomic<uint64_t> _fillSeq{0}; // Fallback execid counter when 34920 not present

	// Settlement
	SettlementCache& _settlCache;
	std::atomic<bool>& _settlementReady;  // Signal Worker to auto-query RQ62

	// Reconnect backoff (CFE/EQT style: 15s → 300s cap)
	static constexpr int RECONNECT_BACKOFF_MS[] = {15000, 30000, 60000, 120000, 240000, 300000};
	static constexpr int RECONNECT_BACKOFF_COUNT = 6;
	int _reconnectAttempt = 0;

	void Run();
	bool Reconnect();
	void ParseBO5(const char* buf, size_t len);
	void ParseBD6(const char* buf, size_t len);
	void ParseEB10(const char* buf, size_t len);
	void ParseNetworkEvent(const char* buf, size_t len);
};

} // namespace KTN::OSE

#endif /* SRC_EXCHSUPPORT_OSE_OMNETBDXTHREAD_HPP_ */
