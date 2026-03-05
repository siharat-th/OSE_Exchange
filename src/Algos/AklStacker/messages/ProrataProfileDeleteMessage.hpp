#pragma once

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

struct ProrataProfileDeleteMessage
{
	static constexpr MessageType type = MessageType::PRORATA_PROFILE_DELETE;
	std::string name;
	
	// Serialize to JSON
	void Serialize(rapidjson::Document& document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
		
		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", 50, allocator);
		header.AddMember("length", 24, allocator);
		header.AddMember("version", 25, allocator);
		
		// Create string value for name
		rapidjson::Value nameValue;
		nameValue.SetString(name.c_str(), static_cast<rapidjson::SizeType>(name.length()), allocator);
		
		// Add members to document
		document.AddMember("header", header, allocator);
		document.AddMember("name", nameValue, allocator);
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
	static ProrataProfileDeleteMessage Deserialize(const rapidjson::Document& document)
	{
		ProrataProfileDeleteMessage message;
		
		// Parse name
		if (document.HasMember("name") && document["name"].IsString())
		{
			message.name = document["name"].GetString();
		}
		
		return message;
	}
	
	// Deserialize from string
	static ProrataProfileDeleteMessage DeserializeFromString(const std::string& json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		
		if (document.HasParseError())
		{
			return ProrataProfileDeleteMessage();
		}
		
		return Deserialize(document);
	}
};

}