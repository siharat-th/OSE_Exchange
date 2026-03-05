#include "PositionStore.hpp"
#include <KT01/Core/Log.hpp>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <Algos/AklStacker/Helpers.hpp>

namespace akl
{

PositionStore::PositionStore()
: positionInfoPool(64)
{
	for (int i = 0; i < 64; ++i)
	{
		PositionInfo *pi = new PositionInfo();
		positionInfoPool.push(pi);
	}
}

void PositionStore::Init()
{
	std::ifstream positionFile;
	positionFile.open(positionFileName);

	if (positionFile.is_open())
	{
		std::string line;
		int lineNum = 0;
		while (getline(positionFile, line))
		{
			std::vector<std::string> fields;
			boost::split(fields, line, boost::is_any_of(","));

			if (fields.size() >= 3)
			{
				const std::string &id = fields[0];
				
				std::optional<int32_t> bidFills = stacker::stringToInt32(fields[1]);
				std::optional<int32_t> askFills = stacker::stringToInt32(fields[2]);
				if (!bidFills)
				{
					KT01_LOG_INFO(__CLASS__, "Failed to parse bid fills for strategy {} in line {}", id, lineNum);
					bidFills = 0;
				}
				if (!askFills)
				{
					KT01_LOG_INFO(__CLASS__, "Failed to parse ask fills for strategy {} in line {}", id, lineNum);
					askFills = 0;
				}
				Push(id, *bidFills, *askFills);

				KT01_LOG_INFO(__CLASS__, "Read position for strategy {}: BidFills: {}, AskFills: {}", id, *bidFills, *askFills);
			}
			else
			{
				KT01_LOG_INFO(__CLASS__, "Found incorret number of fields in line {} of position file: {}", lineNum, line);
			}
			++lineNum;
		}
		positionFile.close();
	}
	else
	{
		KT01_LOG_INFO(__CLASS__, "Failed to open position file: {}", positionFileName);
	}

	running.store(true, std::memory_order_release);
	positionSaverThread = std::thread(&PositionStore::run, this);
}

void PositionStore::Save()
{
	if (positionsChanged)
	{
        std::ofstream positionFile;
        positionFile.open(positionFileName);
        if (positionFile.is_open())
        {
            for (const auto& entry : positionMap)
            {
                PositionInfo *pi = entry.second.load();
                
                if (pi != nullptr)
                {
					PositionInfo copy = *pi;
                    positionFile << copy.strategyId
                        << "," << copy.bidFills
                        << "," << copy.askFills
                        << std::endl;
                }
            }
            positionFile.close();
            positionsChanged = false;
        }
        else
        {
            KT01_LOG_INFO(__CLASS__, "Failed to open position file for writing: {}", positionFileName);
        }
	}
}

void PositionStore::Push(const std::string &strategyId, int bidFills, int askFills)
{
	PositionInfo *pi = get();
	strncpy(pi->strategyId, strategyId.c_str(), sizeof(pi->strategyId) - 1);
	pi->strategyId[sizeof(pi->strategyId) - 1] = '\0';
	pi->bidFills = bidFills;
	pi->askFills = askFills;
	//pi->inUse = true;

	PositionMapType::iterator it = positionMap.find(strategyId);
	if (it != positionMap.end())
	{
		PositionInfo *oldPi = it->second.exchange(pi, std::memory_order_release);
		//oldPi->inUse = false;
		put(oldPi);
	}
	else
	{
		positionMap.emplace(std::piecewise_construct,
			std::forward_as_tuple(strategyId),
			std::forward_as_tuple(pi));
	}
	positionsChanged = true;
}

std::optional<PositionStore::PositionInfo> PositionStore::GetPositionInfo(const std::string &strategyId) const
{
	PositionMapType::const_iterator it = positionMap.find(strategyId);
	if (it != positionMap.end())
	{
		PositionInfo *pi = it->second.load(std::memory_order_acquire);
		if (pi != nullptr)
		{
			return *pi;
		}
		
	}
	return std::nullopt;
}

void PositionStore::Stop()
{
	running.store(false, std::memory_order_release);
	if (positionSaverThread.joinable())
	{
		positionSaverThread.join();
	}
	positionsChanged = true;
	Save();
}

void PositionStore::run()
{
	while (running.load(std::memory_order_acquire))
	{
		time.SetTime();
		if (time.Now() >= lastSaveTime + saveInterval)
		{
			Save();
			lastSaveTime = time.Now();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

PositionStore::PositionInfo *PositionStore::get()
{
	PositionInfo *pi;
	if (positionInfoPool.pop(pi))
	{
		return pi;
	}
	else
	{
		return new PositionInfo();
	}
}

void PositionStore::put(PositionStore::PositionInfo *pi)
{
	positionInfoPool.push(pi);
}
	
}