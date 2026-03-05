#pragma once

#include <unordered_map>
#include <string>
#include <mutex>
#include <optional>

#include <thread>
#include <atomic>

#include <Algos/AklStacker/Time.hpp>
#include <KT01/Core/Singleton.hpp>
#include <tbb/concurrent_unordered_map.h>
#include <boost/lockfree/queue.hpp>

namespace akl
{

class PositionStore : public Singleton<PositionStore>
{
public:
	struct PositionInfo
	{
		char strategyId[16] { 0 };
		int bidFills { 0 };
		int askFills { 0 };
	};

	~PositionStore() = default;	
	void Init();
	void Save();

	void Push(const std::string &strategyId, int bidFills, int askFills);

	std::optional<PositionInfo> GetPositionInfo(const std::string &strategyId) const;

	void Stop();
	
private:
	friend Singleton<PositionStore>;
	PositionStore();

	PositionStore(const PositionStore &) = delete;
	PositionStore& operator=(const PositionStore &) = delete;
	PositionStore(PositionStore &&) = delete;
	PositionStore& operator=(PositionStore &&) = delete;

	using PositionMapType = tbb::concurrent_unordered_map<std::string, std::atomic<PositionInfo *>>;
	PositionMapType positionMap;

	static constexpr const char *positionFileName = "positions.csv";

	std::thread positionSaverThread;
	std::atomic_bool running { false };
	bool positionsChanged { false };

	void run();
	stacker::Timestamp lastSaveTime { stacker::Time::Min() };
	stacker::Duration saveInterval { stacker::Seconds(60 * 5) };

	stacker::Time time;

	boost::lockfree::queue<PositionInfo *> positionInfoPool;
	PositionInfo *get();
	void put(PositionInfo *pi);
};

}