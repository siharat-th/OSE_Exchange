#include "ProrataProfileManager.hpp"
#include <Algos/AklStacker/messages/MessageType.hpp>
#include <Algos/AklStacker/messages/ProrataProfileDeleteMessage.hpp>
#include <KT01/Core/Log.hpp>

#include <Rest/JsonUrlReader.hpp>
#include <Rest/RestApi.hpp>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

namespace akl
{

ProrataProfileManager::ProrataProfileManager()
: publisher("ProrataProfileManager", 250)
{
}

void ProrataProfileManager::Init(const std::string &source)
{
#if !AKL_TEST_MODE
	const std::string url = RestApi::API_SERVER + "algos/getProrataProfiles?org=AKL"; // TODO make org configurable?
	KT01_LOG_INFO(__CLASS__, "Init() url: {}", url);

	const std::string respose = JsonUrlReader::Get(url);
	KT01_LOG_INFO(__CLASS__,"Init() response: {}", respose);

	std::vector<stacker::ProrataProfileUpdateMessage> profiles = stacker::ProrataProfileUpdateMessage::DeserializeMultipleProfilesFromRestApi(respose);
	KT01_LOG_INFO(__CLASS__,"Init() parsed {} profiles from response", profiles.size());

	{
		std::unique_lock<std::mutex> lock(mutex);
		for (const stacker::ProrataProfileUpdateMessage &profile : profiles)
		{
			if (!profile.name.empty())
			{
				profileMap[profile.name] = profile;
				KT01_LOG_INFO(__CLASS__,"Init() added profile: {}", profile.name);
			}
			else
			{
				KT01_LOG_INFO(__CLASS__,"Init() empty profile name, skipping");
			}
		}
	}

	publisher.SetSource(source);
	publisher.Start();
#endif
}

void ProrataProfileManager::UpdateProfile(const stacker::ProrataProfileUpdateMessage &profile)
{
	{
		std::unique_lock<std::mutex> lock(mutex);
		profileMap[profile.name] = profile;
	}
	KT01_LOG_INFO(__CLASS__,"UpdateProfile() updated profile: {}", profile.name);
}

void ProrataProfileManager::DeleteProfile(const char *name)
{
	bool result = false;
	{
		std::unique_lock<std::mutex> lock(mutex);
		ProrataProfileMapType::iterator it = profileMap.find(name);
		if (it != profileMap.end())
		{
			profileMap.erase(it);
			result = true;
		}
		else
		{
			result = false;
		}
	}
	if (result)
	{
		KT01_LOG_INFO(__CLASS__,"DeleteProfile() deleted profile: {}", name);
	}
	else
	{
		KT01_LOG_INFO(__CLASS__,"DeleteProfile() Failed to find profile: {}", name);
	}
}

void ProrataProfileManager::GetProfiles(std::vector<stacker::ProrataProfileUpdateMessage> &profiles) const
{
	std::unique_lock<std::mutex> lock(mutex);
	for (ProrataProfileMapType::const_iterator it = profileMap.cbegin(); it != profileMap.cend(); ++it)
	{
		profiles.push_back(it->second);
	}
}

std::optional<stacker::ProrataProfileUpdateMessage> ProrataProfileManager::GetProfile(const char *name) const
{
	std::unique_lock<std::mutex> lock(mutex);
	auto it = profileMap.find(name);
	if (it != profileMap.end())
	{
		return it->second;
	}
	else
	{
		return std::nullopt;
	}
}

void ProrataProfileManager::OnAlgoMessage(const std::string &json)
{
	rapidjson::Document document;
	rapidjson::ParseResult result = document.Parse(json.c_str());

	if (!result)
	{
		std::string error = std::string("JSON parse error: ") + rapidjson::GetParseError_En(result.Code()) +
							" at offset " + std::to_string(result.Offset());
		throw std::runtime_error(error);
	}

	int messageTypeInt = 0;
	std::string payload = "";

	for (rapidjson::GenericValue<rapidjson::UTF8<>>::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
	{
		if (itr->HasMember("Payload"))
		{
			payload = (*itr)["Payload"].GetString();
		}
		if (itr->HasMember("MessageType") && itr->operator[]("MessageType").IsInt())
		{
			messageTypeInt = (*itr)["MessageType"].GetInt();
		}
	}

	const stacker::MessageType messageType = static_cast<stacker::MessageType>(messageTypeInt);
	switch (messageType)
	{
		case stacker::MessageType::PRORATA_PROFILE_UPDATE:
		{
			stacker::ProrataProfileUpdateMessage msg = stacker::ProrataProfileUpdateMessage::DeserializeFromString(payload);
			UpdateProfile(msg);
			publisher.Push(msg);
			break;
		}
		case stacker::MessageType::PRORATA_PROFILE_DELETE:
		{
			stacker::ProrataProfileDeleteMessage msg = stacker::ProrataProfileDeleteMessage::DeserializeFromString(payload);
			DeleteProfile(msg.name.c_str());
			publisher.Push(msg);
			break;
		}
		default:
			break;
	}
}

void ProrataProfileManager::OnInternalProfileUpdate(const stacker::ProrataProfileUpdateMessage &profile)
{
	//internalProfileUpdates.push(profile);
	UpdateProfile(profile);
	publisher.Push(profile);

	const std::string profileJson = profile.SerializeToRestApi();

	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	rapidjson::Value jsonParams(profileJson.c_str(), allocator);
	doc.AddMember("json_params", jsonParams, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);

	const std::string request = buffer.GetString();

	const std::string url = RestApi::API_SERVER + "algos/saveProrataProfile";

	KT01_LOG_INFO(__CLASS__, "OnInternalProfileUpdate() url: {} | request: {}", url, request);
	const std::string response = JsonUrlReader::PostJson(url, request);
	KT01_LOG_INFO(__CLASS__, "OnInternalProfileUpdate() response: {}", response);
}

void ProrataProfileManager::Clear()
{
	std::unique_lock<std::mutex> lock(mutex);
	profileMap.clear();
	KT01_LOG_INFO(__CLASS__,"Clear(): cleared all profiles");
}

}