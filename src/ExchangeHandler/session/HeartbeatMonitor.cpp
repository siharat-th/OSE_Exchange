//============================================================================
// Name        	: HeartbeatMonitor.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 21, 2023 1:15:52 PM
//============================================================================

#include <ExchangeHandler/session/HeartbeatMonitor.hpp>

using namespace KTN::Session;

HeartbeatMonitor::HeartbeatMonitor() {
	atomic_flag_.store(false);
	reset_flag_.store(false);

}

HeartbeatMonitor::~HeartbeatMonitor() {

}

void HeartbeatMonitor::Start(int seconds, int segid, int bufferms)
{
	heartbeat_seconds_ = seconds;
	segid_ = segid;
	buffer_ms_ = bufferms;

	ostringstream oss;
	oss << "Seg" << segid_ << "Hb";

	CreateAndRun(oss.str(),2);

	//pthread_create(&monitor_thread_, nullptr, &MonitorThread, this);
}

void HeartbeatMonitor::Stop()
{
	 if (_ACTIVE.load(std::memory_order_relaxed) == true) {
		_ACTIVE.store(false, std::memory_order_relaxed);
	//	 pthread_join(monitor_thread_, nullptr);
	 }
}

void HeartbeatMonitor::Reset()
{
	reset_flag_.store(true);
//	atomic_flag_.store(false);
}

void HeartbeatMonitor::Run()
{
	uint64_t now, elapsed;
	int realhb = heartbeat_seconds_ ;
	if (heartbeat_seconds_ > 1)
		realhb = heartbeat_seconds_ -1;
	
	uint64_t realhb_us = static_cast<uint64_t>(realhb) * 1000ULL * 1000ULL;

	uint64_t last_sent_time_ = 0;

	//************ MICROSECONDS NOT NANOS HERE **************************
	_ACTIVE.store(true, std::memory_order_relaxed);
	while(_ACTIVE.load(std::memory_order_relaxed) == true){
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		now =  ((ts.tv_sec) * 1000000ULL) + ((ts.tv_nsec) / 1000ULL);

		elapsed = now - last_sent_time_;

		if (!IsFlagSet() &&
				elapsed >= realhb_us) {
			atomic_flag_.store(true);
		}

		if (ResetFlag())
		{
			clock_gettime(CLOCK_REALTIME, &ts);
			uint64_t now_ns =  ((ts.tv_sec) * 1000000ULL) + ((ts.tv_nsec) / 1000ULL);
			last_sent_time_ = now_ns;

			const uint64_t us_buffer = buffer_ms_ * 1000;
			last_sent_time_ = now_ns - us_buffer;

			reset_flag_.store(false);
			atomic_flag_.store(false);
			//usleep(ns_buffer);
		}
		usleep(100); // Sleep for 1 microsecond to avoid busy-waiting
	}
}
