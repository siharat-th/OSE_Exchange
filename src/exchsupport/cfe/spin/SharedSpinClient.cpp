#include "SharedSpinClient.hpp"

namespace KTN::CFE::Spin
{

SharedSpinClient::SharedSpinClient()
{
	spinClient = new SpinClient(this);
}

SharedSpinClient::~SharedSpinClient()
{

}

void SharedSpinClient::Register(SpinCallback *cb)
{
	std::lock_guard<std::mutex> lock(mutex);
	for (SpinCallback *existing : callbacks)
	{
		if (existing == cb)
		{
			return;
		}
	}
	callbacks.push_back(cb);
}

void SharedSpinClient::SendSpinRequest(uint32_t seq)
{
	std::lock_guard<std::mutex> lock(mutex);

	if (!started.load())
	{
		return;
	}
	if (!ensureConnected())
	{
		return;
	}

	idleTimerActive = false;

	// Always keep the highest requested sequence
	if (seq > pendingSeq)
		pendingSeq = seq;

	trySendPendingSpinRequest();
}

void SharedSpinClient::Start()
{
	if (!started.exchange(true))
	{
		spinClient->Start();
		LOGINFO("[SharedSpinClient] Spin client started");

		idleThreadRunning = true;
		idleThread = std::thread([this]()
		{
			while (idleThreadRunning.load(std::memory_order_relaxed))
			{
				std::this_thread::sleep_for(std::chrono::seconds(10));
				CheckIdleTimeout();
			}
		});
		LOGINFO("[SharedSpinClient] Idle timer thread started with timeout {} seconds", idleTimeout.count());
	}
	else
	{
		std::lock_guard<std::mutex> lock(mutex);
		if (!spinClient->IsConnected())
		{
			idleTimerActive = false;
			spinClient->Connect();
			LOGINFO("[SharedSpinClient] Spin client was already started but not connected, reconnected to spin server");
		}
	}
}

void SharedSpinClient::Stop()
{
	if (started.exchange(false))
	{
		LOGINFO("[SharedSpinClient] Spin client stopped");

		idleThreadRunning = false;
		if (idleThread.joinable())
		{
			idleThread.join();
			LOGINFO("[SharedSpinClient] Idle timer thread stopped");
		}

		spinClient->Stop();
	}
}

uint32_t SharedSpinClient::CurrentSpinImageSeq() const
{
	return spinClient->CurrentSpinImageSeq();
}

void SharedSpinClient::CheckIdleTimeout()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (!idleTimerActive.load())
		return;

	if (spinning.load())
		return; // Don't disconnect while spinning

	// if (!spinClient->IsConnected())
	// 	return; // Already disconnected

	auto elapsed = std::chrono::steady_clock::now() - lastSpinCompleteTime;
	if (elapsed >= idleTimeout)
	{
		LOGINFO("[SharedSpinClient] Idle timeout reached ({}s) - disconnecting from spin server", idleTimeout.count());
		idleTimerActive = false;
		spinClient->Disconnect();
	}
}
	

bool SharedSpinClient::ensureConnected()
{
	if (spinClient->IsConnected())
		return true;

	LOGINFO("Not connected - reconnecting to spin server");
	bool ok = spinClient->Connect();
	if (ok)
	{
		LOGINFO("Reconnected to spin server successfully");
	}
	else
	{
		LOGERROR("[SharedSpinClient] Failed to reconnect to spin server");
	}
	return ok;
}

void SharedSpinClient::onSpinImageAvailable(uint32_t sequence)
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		for (SpinCallback *cb : callbacks)
		{
			cb->onSpinImageAvailable(sequence);
		}

		trySendPendingSpinRequest();
	}
	LOGINFO("[SharedSpinClient] onSpinImageAvailable for seq {}", sequence);
}

void SharedSpinClient::onSpinStart(uint32_t startseq, uint32_t endseq, uint32_t count)
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		for (SpinCallback *cb : callbacks)
		{
			cb->onSpinStart(startseq, endseq, count);
		}
	}
	LOGINFO("[SharedSpinClient] onSpinStart for seq range {}-{} with count {}", startseq, endseq, count);
}

void SharedSpinClient::onSpinEnd(uint32_t seqnum, uint32_t total_processed)
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		for (SpinCallback *cb : callbacks)
		{
			cb->onSpinEnd(seqnum, total_processed);
		}
	}
	LOGINFO("[SharedSpinClient] onSpinEnd for seq {} with total processed {}", seqnum, total_processed);
	processNextRequest();
}

void SharedSpinClient::onBooksAvailable(std::vector<LimitOrderBook> &books)
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		for (SpinCallback *cb : callbacks)
		{
			cb->onBooksAvailable(books);
		}
	}
	LOGINFO("[SharedSpinClient] onBooksAvailable for {} books", books.size());
}

void SharedSpinClient::trySendPendingSpinRequest()
{
	// Caller must hold mutex
	if (pendingSeq == 0)
		return;

	if (spinning.load())
		return;

	uint32_t currentImage = spinClient->CurrentSpinImageSeq();
	if (currentImage >= pendingSeq)
	{
		LOGINFO("[SharedSpinClient] Sending pending spin for seq={} (image={})", pendingSeq, currentImage);
		pendingSeq = 0;
		spinning.store(true);
		spinClient->SendSpinRequest(currentImage);
	}
}

void SharedSpinClient::processNextRequest()
{
	std::lock_guard<std::mutex> lock(mutex);
	spinning.store(false);

	trySendPendingSpinRequest();

	if (!spinning.load() && pendingSeq == 0)
	{
		lastSpinCompleteTime = std::chrono::steady_clock::now();
		idleTimerActive = true;
	}
}

}