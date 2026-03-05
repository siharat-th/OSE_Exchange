#pragma once

#include <KT01/Core/Singleton.hpp>

#include <Algos/AklStacker/Order.hpp>

#include <tbb/concurrent_vector.h>
#include <unordered_map>
#include <thread>
#include <atomic>

namespace akl
{

namespace stacker
{

class AklStacker;	
}

struct StrategyOrderInfo
{
	uint64_t cmeId;
	int16_t stackIndex;
};

struct OrderUpdate
{
	uint64_t cmeId { 0 };
	int8_t stackIndex { -1 };
	bool deleted { false };
};

class StrategyManager : public Singleton<StrategyManager>
{
public:
	~StrategyManager() = default;

	void AddStacker(stacker::AklStacker *stacker);
	void Init();
	void Stop();

	std::optional<StrategyOrderInfo> GetOrderInfo(const uint64_t cmeId) const;

private:
	friend Singleton<StrategyManager>;

	StrategyManager();

	StrategyManager(const StrategyManager &) = delete;
	StrategyManager& operator=(const StrategyManager &) = delete;
	StrategyManager(StrategyManager &&) = delete;
	StrategyManager& operator=(StrategyManager &&) = delete;

	const char *filename = "orders.csv";

	using StackerListType = tbb::concurrent_vector<stacker::AklStacker *>;
	StackerListType stackers;

	using OrderMapType = std::unordered_map<uint64_t, StrategyOrderInfo>;
	OrderMapType orderMap;

	std::thread strategyManagerThread;
	std::atomic_bool running { false };

	static constexpr std::chrono::duration ORDER_CHECK_DURATION = std::chrono::seconds(5);
	std::chrono::time_point<std::chrono::steady_clock> lastOrderCheckTime { std::chrono::time_point<std::chrono::steady_clock>::min() };

	static constexpr std::chrono::duration FILE_WRITE_DURATION = std::chrono::seconds(60);
	std::chrono::time_point<std::chrono::steady_clock> lastFileWriteTime { std::chrono::time_point<std::chrono::steady_clock>::min() };

	static constexpr std::chrono::duration PRORATA_PROFILE_CHECK_DURATION = std::chrono::milliseconds(500);
	std::chrono::time_point<std::chrono::steady_clock> lastProrataProfileCheckTime { std::chrono::time_point<std::chrono::steady_clock>::min() };

	void run();

	bool processOrderUpdates();
	void processInternalProrataUpdates();

	void readOrdersFromFile();
	void saveOrdersToFile();
};

}