#pragma once

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

struct StackerProrataOrderRequestMessage
{
	static constexpr MessageType type = MessageType::STACKER_PRORATA_ORDER_REQ;
	std::string id;
	
	// Serialize to JSON
	void Serialize(rapidjson::Document& document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
		
		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", 96, allocator);
		header.AddMember("length", 56, allocator);
		header.AddMember("version", 25, allocator);
		
		// Create string value for id
		rapidjson::Value idValue;
		idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);
		
		// Add members to document
		document.AddMember("header", header, allocator);
		document.AddMember("id", idValue, allocator);
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
	static StackerProrataOrderRequestMessage Deserialize(const rapidjson::Document& document)
	{
		StackerProrataOrderRequestMessage message;
		
		// Parse id
		if (document.HasMember("id") && document["id"].IsString())
		{
			message.id = document["id"].GetString();
		}
		
		return message;
	}
	
	// Deserialize from string
	static StackerProrataOrderRequestMessage DeserializeFromString(const std::string& json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		
		if (document.HasParseError())
		{
			return StackerProrataOrderRequestMessage();
		}
		
		return Deserialize(document);
	}
};

}