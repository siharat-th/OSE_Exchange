#pragma once

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

struct StackerAdoptMessage
{
	static constexpr MessageType type = MessageType::STACKER_ADOPT;
	std::string id;
	int stackIndex{-1};

	// Serialize to JSON
	void Serialize(rapidjson::Document &document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", static_cast<int>(type), allocator);
		header.AddMember("length", 57, allocator);
		header.AddMember("version", 25, allocator);

		// Add members to document
		document.AddMember("header", header, allocator);

		rapidjson::Value idValue;
		idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);
		document.AddMember("id", idValue, allocator);

		// Cast stackIndex to int to ensure it's treated as a number
		document.AddMember("stackIndex", static_cast<int>(stackIndex), allocator);
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
	static StackerAdoptMessage Deserialize(const rapidjson::Document &document)
	{
		StackerAdoptMessage message;

		if (document.HasMember("stackIndex") && document["stackIndex"].IsInt())
		{
			message.stackIndex = static_cast<int8_t>(document["stackIndex"].GetInt());
		}

		return message;
	}

	// Deserialize from string
	static StackerAdoptMessage DeserializeFromString(const std::string &json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());

		if (document.HasParseError())
		{
			// Handle parsing error
			// You might want to throw an exception or return a default message
			return StackerAdoptMessage();
		}

		return Deserialize(document);
	}
};

} // namespace akl::stacker