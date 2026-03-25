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
#include "settings/OseSessionSettings.hpp"

namespace KTN::OSE {

using KTN::Core::SPSCRingBuffer;
using namespace KTN::ORD;

class OmnetBdxThread
{
public:
	OmnetBdxThread(SPSCRingBuffer<KTN::OrderPod>& responseQueue,
	               const OseSessionSettings& sett);
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

	void Run();
	void ParseBO5(const char* buf, size_t len);
	void ParseNetworkEvent(const char* buf, size_t len);
};

} // namespace KTN::OSE

#endif /* SRC_EXCHSUPPORT_OSE_OMNETBDXTHREAD_HPP_ */
