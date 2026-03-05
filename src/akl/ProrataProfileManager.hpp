#pragma once

#include <unordered_map>
#include <string>
#include <mutex>
#include <optional>

#include <Algos/AklStacker/messages/ProrataProfileUpdateMessage.hpp>
#include <KT01/Core/Singleton.hpp>

#include "ProrataMessagePublisher.hpp"

namespace akl
{

class ProrataProfileManager : public Singleton<ProrataProfileManager>
{
public:
	~ProrataProfileManager() = default;	
	void Init(const std::string &source);

	void UpdateProfile(const stacker::ProrataProfileUpdateMessage &profile);
	void DeleteProfile(const char *name);
	void GetProfiles(std::vector<stacker::ProrataProfileUpdateMessage> &profiles) const;
	std::optional<stacker::ProrataProfileUpdateMessage> GetProfile(const char *name) const;

	void OnAlgoMessage(const std::string &json);

	void OnInternalProfileUpdate(const stacker::ProrataProfileUpdateMessage &profile);

	void Clear();
	
private:
	friend Singleton<ProrataProfileManager>;
	ProrataProfileManager();

	ProrataProfileManager(const ProrataProfileManager &) = delete;
	ProrataProfileManager& operator=(const ProrataProfileManager &) = delete;
	ProrataProfileManager(ProrataProfileManager &&) = delete;
	ProrataProfileManager& operator=(ProrataProfileManager &&) = delete;

	using ProrataProfileMapType = std::unordered_map<std::string, stacker::ProrataProfileUpdateMessage>;
	ProrataProfileMapType profileMap;

	ProrataMessagePublisher publisher;

	mutable std::mutex mutex;

	//tbb::concurrent_queue<stacker::ProrataProfileUpdateMessage> internalProfileUpdates;
};

}