#pragma once

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

struct StackerSpreaderUpdateMessage
{
	static constexpr MessageType type = MessageType::STACKER_SPREADER_UPDATE;
	std::string id;
	int64_t askStrike{std::numeric_limits<int64_t>::max()};
	int64_t bidStrike{std::numeric_limits<int64_t>::max()};

	// Serialize to JSON
	void Serialize(rapidjson::Document &document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", 81, allocator);
		header.AddMember("length", 129, allocator);
		header.AddMember("version", 1, allocator);

		// Create string value for id
		rapidjson::Value idValue;
		idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);

		// Add members to document
		document.AddMember("header", header, allocator);
		document.AddMember("id", idValue, allocator);
		document.AddMember("askStrike", askStrike, allocator);
		document.AddMember("bidStrike", bidStrike, allocator);
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
	static StackerSpreaderUpdateMessage Deserialize(const rapidjson::Document &document)
	{
		StackerSpreaderUpdateMessage message;

		if (document.HasMember("id") && document["id"].IsString())
			message.id = document["id"].GetString();

		if (document.HasMember("askStrike") && document["askStrike"].IsInt64())
			message.askStrike = document["askStrike"].GetInt64();

		if (document.HasMember("bidStrike") && document["bidStrike"].IsInt64())
			message.bidStrike = document["bidStrike"].GetInt64();

		return message;
	}

	// Deserialize from string
	static StackerSpreaderUpdateMessage DeserializeFromString(const std::string &json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());

		if (document.HasParseError())
		{
			return StackerSpreaderUpdateMessage();
		}

		return Deserialize(document);
	}
};

} // namespace akl::stacker