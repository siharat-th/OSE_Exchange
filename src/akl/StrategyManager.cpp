#include "StrategyManager.hpp"
#include "ProrataProfileManager.hpp"

#include <KT01/Core/Log.hpp>
#include <Algos/AklStacker/AklStacker.hpp>

#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace akl
{

StrategyManager::StrategyManager()
{

}

void StrategyManager::AddStacker(stacker::AklStacker *stacker)
{
	if (stacker != nullptr)
	{
		for (stacker::AklStacker *s : stackers)
		{
			if (s == stacker)
			{
				return;
			}
		}

		stackers.push_back(stacker);
	}
}

void StrategyManager::Init()
{
	readOrdersFromFile();

	running.store(true, std::memory_order_release);
	strategyManagerThread = std::thread(&StrategyManager::run, this);
}

void StrategyManager::Stop()
{
	running.store(false, std::memory_order_release);
	if (strategyManagerThread.joinable())
	{
		strategyManagerThread.join();
	}
	
	processOrderUpdates();
	saveOrdersToFile();
}

std::optional<StrategyOrderInfo> StrategyManager::GetOrderInfo(const uint64_t cmeId) const
{
	OrderMapType::const_iterator it = orderMap.find(cmeId);
	if (it != orderMap.end())
	{
		return it->second;
	}
	else
	{
		return std::nullopt;
	}
}

void StrategyManager::run()
{
	while (running.load(std::memory_order_acquire))
	{
		bool ordersChanged = false;

		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		if (now >= lastProrataProfileCheckTime + PRORATA_PROFILE_CHECK_DURATION)
		{
			processInternalProrataUpdates();
			lastProrataProfileCheckTime = now;
		}
		if (now >= lastOrderCheckTime + ORDER_CHECK_DURATION)
		{
			ordersChanged |= processOrderUpdates();

			if (now >= lastFileWriteTime + FILE_WRITE_DURATION)
			{
				if (ordersChanged)
				{
					saveOrdersToFile();
					ordersChanged = false;
				}
				lastFileWriteTime = now;
			}
			lastOrderCheckTime = now;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

bool StrategyManager::processOrderUpdates()
{
	bool ordersChanged = false;
	for (stacker::AklStacker *stacker : stackers)
	{
		OrderUpdate up;
		while (stacker->pendingOrderUpdates.try_pop(up))
		{
			if (up.cmeId > 0)
			{
				if (up.deleted)
				{
					OrderMapType::iterator it = orderMap.find(up.cmeId);
					if (it != orderMap.end())
					{
						orderMap.erase(it);
						ordersChanged = true;
					}
				}
				else
				{
					if (up.stackIndex == 0 || up.stackIndex == 1)
					{
						orderMap[up.cmeId] = { up.cmeId, up.stackIndex };
						ordersChanged = true;
					}
				}
			}
		}
	}
	return ordersChanged;
}

void StrategyManager::processInternalProrataUpdates()
{
	using UpdateType = std::pair<stacker::ProrataProfileUpdateMessage, const stacker::AklStacker *>;
	std::vector<UpdateType> updates;
	for (stacker::AklStacker *stacker : stackers)
	{
		stacker::ProrataProfileUpdateMessage msg;
		while (stacker->pendingInternalProrataProfileUpdates.try_pop(msg))
		{
			updates.emplace_back(msg, stacker);
		}
	}

	for (const UpdateType &update : updates)
	{
		const stacker::ProrataProfileUpdateMessage &msg = update.first;
		const stacker::AklStacker *stacker = update.second;

		for (stacker::AklStacker *s : stackers)
		{
			if (s != stacker)
			{
				s->ProcessProrataProfileUpdateFromOtherStrategy(msg);
			}
		}
	}

	for (const UpdateType &update : updates)
	{
		ProrataProfileManager::instance().OnInternalProfileUpdate(update.first);
	}
}

void StrategyManager::readOrdersFromFile()
{
	std::fstream file;
	file.open(filename, std::ios::in);

	if (!file)
	{
		KT01_LOG_WARN(__CLASS__, "Orders file \"{}\" not found. Creating a new file.", filename);
		return;
	}

	KT01_LOG_INFO(__CLASS__, "Reading orders file \"{}\"", filename);

	
	string line;
	int lineNum = 0;
	while (getline(file, line))
	{
		++lineNum;

		std::vector<std::string> fields;
		boost::split(fields, line, boost::is_any_of(","));

		if (fields.size() >= 2)
		{
			StrategyOrderInfo info;
			try
			{
				info.cmeId = std::stoull(fields[0]);
			}
			catch (const std::invalid_argument &ex)
			{
				KT01_LOG_WARN(__CLASS__, "Caught invalid_argument exception when trying to parse orderId on line {}: {}",
					lineNum, ex.what());
				continue;
			}
			catch (const std::out_of_range &ex)
			{
				KT01_LOG_WARN(__CLASS__, "Caught out_of_range exception when trying to parse orderId on line {}: {}",
					lineNum, ex.what());
				continue;
			}
			try
			{
				info.stackIndex = std::stoi(fields[1]);
			}
			catch (const std::invalid_argument &ex)
			{
				info.stackIndex = 0;
				KT01_LOG_WARN(__CLASS__, "Caught invalid_argument exception when trying to parse stackIndex for order {}: {}",
					info.cmeId, ex.what());
			}
			catch (const std::out_of_range &ex)
			{
				info.stackIndex = 0;
				KT01_LOG_WARN(__CLASS__, "Caught out_of_range exception when trying to parse stackIndex for order {}: {}",
					info.cmeId, ex.what());
			}

			if (info.stackIndex < 0 || info.stackIndex > 1)
			{
				info.stackIndex = 0;
				KT01_LOG_WARN(__CLASS__, "Invalid stack index {} for order {} in file \"{}\". Defaulting to 0.",
					info.stackIndex, info.cmeId, filename);
			}

			if (orderMap.find(info.cmeId) == orderMap.end())
			{
				orderMap[info.cmeId] = info;
				KT01_LOG_INFO(__CLASS__, "Loaded orderId {} with stackIndex {} from file \"{}\" at line {}",
					info.cmeId, static_cast<int16_t>(info.stackIndex), filename, lineNum);
			}
			else
			{
				KT01_LOG_WARN(__CLASS__, "Duplicate orderId {} found in file \"{}\" at line {}. Skipping.",
					info.cmeId, filename, lineNum);
			}
		}
		else
		{
			KT01_LOG_WARN(__CLASS__, "Invalid line {} in orders file \"{}\": {}", lineNum, filename, line);
			continue;
		}
	}
}

void StrategyManager::saveOrdersToFile()
{
	std::fstream file;
	file.open(filename, std::ios::out | std::ios::in);

	if (!file)
	{
		KT01_LOG_WARN(__CLASS__, "Orders file \"{}\" not found. Creating a new file.", filename);

	    file.open(filename, std::ios::out);
	    if (!file)
		{
			KT01_LOG_WARN(__CLASS__, "Failed to create orders file: \"{}\"", filename);
	        return;
	    }
	}

	KT01_LOG_INFO(__CLASS__, "Opening orders file \"{}\"", filename);

	for (OrderMapType::const_iterator it = orderMap.begin(); it != orderMap.end(); ++it)
	{
		const StrategyOrderInfo &soi = it->second;
		if (soi.cmeId > 0)
		{
			file << soi.cmeId << "," << static_cast<int16_t>(soi.stackIndex) << std::endl;
		}
	}

	if (file.is_open())
	{
		file.close();
		KT01_LOG_INFO(__CLASS__, "Closing orders file \"{}\"", filename);
	}
}

}