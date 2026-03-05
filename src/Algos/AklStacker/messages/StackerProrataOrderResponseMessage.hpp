#pragma once

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include "../Helpers.hpp"

namespace akl::stacker
{

struct StackerProrataOrderResponseMessage
{
	static constexpr MessageType type = MessageType::STACKER_PRORATA_ORDER;
	std::string id;
	uint64_t oid;
	//std::string oid;
	int32_t workingQuantity { 0 };
	int32_t piq { 0 };
	KTN::ORD::KOrderSide::Enum side { KTN::ORD::KOrderSide::Enum::UNKNOWN };
	int8_t index { 0 };
	int32_t count { 0 };
	
	// Serialize to JSON
	void Serialize(rapidjson::Document& document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
		
		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", 97, allocator);
		header.AddMember("length", 113, allocator);
		header.AddMember("version", 25, allocator);
		
		// Create string values
		rapidjson::Value idValue;
		idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);
		
		rapidjson::Value oidValue;
		//oidValue.SetString(oid.c_str(), static_cast<rapidjson::SizeType>(oid.length()), allocator);
		const std::string oidStr = uint64ToString(oid);
		oidValue.SetString(oidStr.c_str(), static_cast<rapidjson::SizeType>(oidStr.length()), allocator);
		
		// Add members to document
		document.AddMember("header", header, allocator);
		document.AddMember("id", idValue, allocator);
		document.AddMember("oid", oidValue, allocator);
		document.AddMember("workingQuantity", workingQuantity, allocator);
		document.AddMember("piq", piq, allocator);

		const std::optional<int8_t> aklSide = KtnSideToAklSide(side);
		if (aklSide)
		{
			document.AddMember("side", static_cast<int>(*aklSide), allocator);
		}

		document.AddMember("index", static_cast<int>(index), allocator);
		document.AddMember("count", count, allocator);
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
	static StackerProrataOrderResponseMessage Deserialize(const rapidjson::Document& document)
	{
		StackerProrataOrderResponseMessage message;
		
		// Parse string fields
		if (document.HasMember("id") && document["id"].IsString())
			message.id = document["id"].GetString();
			
		if (document.HasMember("oid") && document["oid"].IsString())
			message.oid = stringToUint64(document["oid"].GetString());
			//message.oid = document["oid"].GetString();
		
		// Parse numeric fields
		if (document.HasMember("workingQuantity") && document["workingQuantity"].IsInt())
			message.workingQuantity = document["workingQuantity"].GetInt();
			
		if (document.HasMember("piq") && document["piq"].IsInt())
			message.piq = document["piq"].GetInt();
			
		if (document.HasMember("side") && document["side"].IsInt())
			message.side = AklSideToKtnSide(static_cast<int8_t>(document["side"].GetInt()));
			
		if (document.HasMember("index") && document["index"].IsInt())
			message.index = static_cast<int8_t>(document["index"].GetInt());
			
		if (document.HasMember("count") && document["count"].IsInt())
			message.count = document["count"].GetInt();
		
		return message;
	}
	
	// Deserialize from string
	static StackerProrataOrderResponseMessage DeserializeFromString(const std::string& json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		
		if (document.HasParseError())
		{
			return StackerProrataOrderResponseMessage();
		}
		
		return Deserialize(document);
	}
};

}