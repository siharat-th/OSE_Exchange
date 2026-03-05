#pragma once

#include <inttypes.h>
#include <string>

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

struct StackerRiskInfoMessage
{
	static constexpr MessageType type = MessageType::STACKER_RISK_INFO;
	struct RiskInfo
	{
		int32_t bidWorking{0};
		int32_t askWorking{0};
		int32_t bidFilled{0};
		int32_t askFilled{0};
		int32_t bidPositionLimit{0};
		int32_t askPositionLimit{0};

		// Serialize RiskInfo to a JSON value
		rapidjson::Value Serialize(rapidjson::Document::AllocatorType &allocator) const
		{
			rapidjson::Value riskInfoObj(rapidjson::kObjectType);
			riskInfoObj.AddMember("bidWorking", bidWorking, allocator);
			riskInfoObj.AddMember("askWorking", askWorking, allocator);
			riskInfoObj.AddMember("bidFilled", bidFilled, allocator);
			riskInfoObj.AddMember("askFilled", askFilled, allocator);
			riskInfoObj.AddMember("bidPositionLimit", bidPositionLimit, allocator);
			riskInfoObj.AddMember("askPositionLimit", askPositionLimit, allocator);
			return riskInfoObj;
		}

		// Deserialize from JSON value
		static RiskInfo Deserialize(const rapidjson::Value &value)
		{
			RiskInfo riskInfo;

			if (value.HasMember("bidWorking") && value["bidWorking"].IsInt())
			{
				riskInfo.bidWorking = value["bidWorking"].GetInt();
			}

			if (value.HasMember("askWorking") && value["askWorking"].IsInt())
			{
				riskInfo.askWorking = value["askWorking"].GetInt();
			}

			if (value.HasMember("bidFilled") && value["bidFilled"].IsInt())
			{
				riskInfo.bidFilled = value["bidFilled"].GetInt();
			}

			if (value.HasMember("askFilled") && value["askFilled"].IsInt())
			{
				riskInfo.askFilled = value["askFilled"].GetInt();
			}

			if (value.HasMember("bidPositionLimit") && value["bidPositionLimit"].IsInt())
			{
				riskInfo.bidPositionLimit = value["bidPositionLimit"].GetInt();
			}

			if (value.HasMember("askPositionLimit") && value["askPositionLimit"].IsInt())
			{
				riskInfo.askPositionLimit = value["askPositionLimit"].GetInt();
			}

			return riskInfo;
		}
	};

	std::string id;
	RiskInfo internalRisk;

	// Serialize to JSON
	void Serialize(rapidjson::Document &document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", static_cast<int>(type), allocator);
		header.AddMember("length", 104, allocator);
		header.AddMember("version", 25, allocator);

		// Create string value for id
		rapidjson::Value idValue;
		idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);

		// Add members to document
		document.AddMember("header", header, allocator);
		document.AddMember("id", idValue, allocator);
		document.AddMember("internalRisk", internalRisk.Serialize(allocator), allocator);
	}

	// Serialize to string
	std::string SerializeToString() const
	{
		rapidjson::Document document;
		Serialize(document);

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);

		return buffer.GetString();
	}

	// Deserialize from JSON
	static StackerRiskInfoMessage Deserialize(const rapidjson::Document &document)
	{
		StackerRiskInfoMessage message;

		// Parse id
		if (document.HasMember("id") && document["id"].IsString())
		{
			message.id = document["id"].GetString();
		}

		// Parse internalRisk
		if (document.HasMember("internalRisk") && document["internalRisk"].IsObject())
		{
			message.internalRisk = RiskInfo::Deserialize(document["internalRisk"]);
		}

		return message;
	}

	// Deserialize from string
	static StackerRiskInfoMessage DeserializeFromString(const std::string &json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());

		if (document.HasParseError())
		{
			// Handle parsing error
			// You might want to throw an exception or return a default message
			return StackerRiskInfoMessage();
		}

		return Deserialize(document);
	}
};

} // namespace akl::stacker