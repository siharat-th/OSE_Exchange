#pragma once

#include <KT01/Core/Singleton.hpp>
#include "SpinClient.hpp"
#include "SpinCallback.hpp"

#include <mutex>

namespace KTN::CFE::Spin
{

class SharedSpinClient : public Singleton<SharedSpinClient>, public SpinCallback
{
public:
	void Register(SpinCallback *cb);
	void SendSpinRequest(uint32_t seq);
	void Start();
	void Stop();
	uint32_t CurrentSpinImageSeq() const;

	void CheckIdleTimeout();

private:
	friend class Singleton<SharedSpinClient>;
	SharedSpinClient();
	~SharedSpinClient();

	SharedSpinClient(const SharedSpinClient &) = delete;
	SharedSpinClient& operator=(const SharedSpinClient &) = delete;
	SharedSpinClient(SharedSpinClient &&) = delete;
	SharedSpinClient& operator=(SharedSpinClient &&) = delete;

	bool ensureConnected();

	void onSpinImageAvailable(uint32_t sequence) override;
	void onSpinStart(uint32_t startseq, uint32_t endseq, uint32_t count) override;
	void onSpinEnd(uint32_t seqnum, uint32_t total_processed) override;
	void onBooksAvailable(std::vector<LimitOrderBook> &books) override;

	void trySendPendingSpinRequest();
	void processNextRequest();

	SpinClient *spinClient { nullptr };

	std::mutex mutex;
	std::vector<SpinCallback *> callbacks;
	std::atomic_bool spinning { false };
	std::atomic_bool started { false };

	uint32_t pendingSeq { 0 };

	std::chrono::seconds idleTimeout { 15 };
	std::chrono::steady_clock::time_point lastSpinCompleteTime;
	std::atomic_bool idleTimerActive { false };
	std::thread idleThread;
	std::atomic_bool idleThreadRunning { false };
};

}