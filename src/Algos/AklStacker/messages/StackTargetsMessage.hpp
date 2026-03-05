#pragma once

#include <inttypes.h>
#include <limits>
#include <string>

#include "MessageType.hpp"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

struct StackTargetsMessage
{
	static constexpr MessageType type = MessageType::STACK_TARGETS;
	std::string id;
	int64_t askTargetPrice{std::numeric_limits<int64_t>::max()};
	int64_t bidTargetPrice{std::numeric_limits<int64_t>::min()};
	int64_t askTargetPrice2{std::numeric_limits<int64_t>::max()};
	int64_t bidTargetPrice2{std::numeric_limits<int64_t>::min()};

	void Serialize(rapidjson::Document &document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", static_cast<int>(type), allocator);
		header.AddMember("length", 88, allocator);
		header.AddMember("version", 25, allocator);

		// Create string value for id
		rapidjson::Value idValue;
		idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);

		// Add members to document
		document.AddMember("header", header, allocator);
		document.AddMember("id", idValue, allocator);

		// Handle the int64_t values carefully
		// RapidJSON has special methods for int64_t types
		document.AddMember("askTargetPrice", askTargetPrice, allocator);
		document.AddMember("bidTargetPrice", bidTargetPrice, allocator);
		document.AddMember("askTargetPrice2", askTargetPrice2, allocator);
		document.AddMember("bidTargetPrice2", bidTargetPrice2, allocator);
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
	static StackTargetsMessage Deserialize(const rapidjson::Document &document)
	{
		StackTargetsMessage message;

		// Parse id
		if (document.HasMember("id") && document["id"].IsString())
		{
			message.id = document["id"].GetString();
		}

		// Parse askTargetPrice
		if (document.HasMember("askTargetPrice") && document["askTargetPrice"].IsInt64())
		{
			message.askTargetPrice = document["askTargetPrice"].GetInt64();
		}

		// Parse bidTargetPrice
		if (document.HasMember("bidTargetPrice") && document["bidTargetPrice"].IsInt64())
		{
			message.bidTargetPrice = document["bidTargetPrice"].GetInt64();
		}

		// Parse askTargetPrice2
		if (document.HasMember("askTargetPrice2") && document["askTargetPrice2"].IsInt64())
		{
			message.askTargetPrice2 = document["askTargetPrice2"].GetInt64();
		}

		// Parse bidTargetPrice2
		if (document.HasMember("bidTargetPrice2") && document["bidTargetPrice2"].IsInt64())
		{
			message.bidTargetPrice2 = document["bidTargetPrice2"].GetInt64();
		}

		return message;
	}

	// Deserialize from string
	static StackTargetsMessage DeserializeFromString(const std::string &json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());

		if (document.HasParseError())
		{
			// Handle parsing error
			// You might want to throw an exception or return a default message
			return StackTargetsMessage();
		}

		return Deserialize(document);
	}
};

} // namespace akl::stacker