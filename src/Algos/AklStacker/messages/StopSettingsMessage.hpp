#pragma once

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <AlgoParams/AklStackerParams.hpp>

namespace akl::stacker
{

struct StopSettingsMessage
{
	static constexpr MessageType type = MessageType::STOP_SETTINGS;
	static constexpr int SIZE = 5;
	std::string id;
	int32_t stopQuantity[SIZE] { 0 };
	int32_t stopTrigger[SIZE] { 0 };
	int32_t stopLean[SIZE] { 0 };
	int32_t middleClickQuantity { 0 };
	int32_t middleClickTrigger { 0 };
	int32_t stopQuantityMax { 100 };
	int32_t flipDelay { 1000 };
	int32_t triggerQuoteDelay { 1000 };
	uint8_t deleteOnFlipDefault { 0 };
	uint8_t showLeanColumn { 0 };
	uint8_t showTradeColumn { 0 };
	
	// Serialize to JSON
	void Serialize(rapidjson::Document& document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
		
		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", 259, allocator);
		header.AddMember("length", 139, allocator);
		header.AddMember("version", 25, allocator);
		
		// Create string value for id
		rapidjson::Value idValue;
		idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);
		
		// Create arrays for stop quantities, triggers, and leans
		rapidjson::Value stopQuantityArray(rapidjson::kArrayType);
		rapidjson::Value stopTriggerArray(rapidjson::kArrayType);
		rapidjson::Value stopLeanArray(rapidjson::kArrayType);
		
		for (int i = 0; i < SIZE; ++i)
		{
			stopQuantityArray.PushBack(stopQuantity[i], allocator);
			stopTriggerArray.PushBack(stopTrigger[i], allocator);
			stopLeanArray.PushBack(stopLean[i], allocator);
		}
		
		// Add members to document
		document.AddMember("header", header, allocator);
		document.AddMember("id", idValue, allocator);
		document.AddMember("stopQuantity", stopQuantityArray, allocator);
		document.AddMember("stopTrigger", stopTriggerArray, allocator);
		document.AddMember("stopLean", stopLeanArray, allocator);
		document.AddMember("middleClickQuantity", middleClickQuantity, allocator);
		document.AddMember("middleClickTrigger", middleClickTrigger, allocator);
		document.AddMember("stopQuantityMax", stopQuantityMax, allocator);
		document.AddMember("flipDelay", flipDelay, allocator);
		document.AddMember("triggerQuoteDelay", triggerQuoteDelay, allocator);
		document.AddMember("deleteOnFlipDefault", static_cast<int>(deleteOnFlipDefault), allocator);
		document.AddMember("showLeanColumn", static_cast<int>(showLeanColumn), allocator);
		document.AddMember("showTradeColumn", static_cast<int>(showTradeColumn), allocator);
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
	static StopSettingsMessage Deserialize(const rapidjson::Document& document)
	{
		StopSettingsMessage message;
		
		// Parse id
		if (document.HasMember("id") && document["id"].IsString())
		{
			message.id = document["id"].GetString();
		}
		
		// Parse arrays
		if (document.HasMember("stopQuantity") && document["stopQuantity"].IsArray())
		{
			const rapidjson::Value& array = document["stopQuantity"];
			for (rapidjson::SizeType i = 0; i < array.Size() && i < SIZE; ++i)
			{
				if (array[i].IsInt())
				{
					message.stopQuantity[i] = array[i].GetInt();
				}
			}
		}
		
		if (document.HasMember("stopTrigger") && document["stopTrigger"].IsArray())
		{
			const rapidjson::Value& array = document["stopTrigger"];
			for (rapidjson::SizeType i = 0; i < array.Size() && i < SIZE; ++i)
			{
				if (array[i].IsInt())
				{
					message.stopTrigger[i] = array[i].GetInt();
				}
			}
		}
		
		if (document.HasMember("stopLean") && document["stopLean"].IsArray())
		{
			const rapidjson::Value& array = document["stopLean"];
			for (rapidjson::SizeType i = 0; i < array.Size() && i < SIZE; ++i)
			{
				if (array[i].IsInt())
				{
					message.stopLean[i] = array[i].GetInt();
				}
			}
		}
		
		// Parse other fields
		if (document.HasMember("middleClickQuantity") && document["middleClickQuantity"].IsInt())
			message.middleClickQuantity = document["middleClickQuantity"].GetInt();
			
		if (document.HasMember("middleClickTrigger") && document["middleClickTrigger"].IsInt())
			message.middleClickTrigger = document["middleClickTrigger"].GetInt();
			
		if (document.HasMember("stopQuantityMax") && document["stopQuantityMax"].IsInt())
			message.stopQuantityMax = document["stopQuantityMax"].GetInt();
			
		if (document.HasMember("flipDelay") && document["flipDelay"].IsInt())
			message.flipDelay = document["flipDelay"].GetInt();
			
		if (document.HasMember("triggerQuoteDelay") && document["triggerQuoteDelay"].IsInt())
			message.triggerQuoteDelay = document["triggerQuoteDelay"].GetInt();
			
		if (document.HasMember("deleteOnFlipDefault") && document["deleteOnFlipDefault"].IsInt())
			message.deleteOnFlipDefault = static_cast<uint8_t>(document["deleteOnFlipDefault"].GetInt());
			
		if (document.HasMember("showLeanColumn") && document["showLeanColumn"].IsInt())
			message.showLeanColumn = static_cast<uint8_t>(document["showLeanColumn"].GetInt());
			
		if (document.HasMember("showTradeColumn") && document["showTradeColumn"].IsInt())
			message.showTradeColumn = static_cast<uint8_t>(document["showTradeColumn"].GetInt());
		
		return message;
	}
	
	// Deserialize from string
	static StopSettingsMessage DeserializeFromString(const std::string& json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		
		if (document.HasParseError())
		{
			return StopSettingsMessage();
		}
		
		return Deserialize(document);
	}

	static StopSettingsMessage FromParams(const AklStackerParams &params)
	{
		StopSettingsMessage message;
		message.id = params.meta.UniqueId;
		
		for (int i = 0; i < SIZE; ++i)
		{
			message.stopQuantity[i] = params.stopSettings.stopQuantities[i];
			message.stopTrigger[i] = params.stopSettings.stopTriggers[i];
			message.stopLean[i] = params.stopSettings.stopLean[i];
		}
		
		message.middleClickQuantity = params.stopSettings.middleClickQuantity;
		message.middleClickTrigger = params.stopSettings.middleClickTrigger;
		message.stopQuantityMax = params.stopSettings.stopQuantityMax;
		message.flipDelay = params.stopSettings.flipDelay;
		message.triggerQuoteDelay = params.stopSettings.triggerQuoteDelay;
		message.deleteOnFlipDefault = static_cast<uint8_t>(params.stopSettings.deleteOnFlipDefault);
		message.showLeanColumn = static_cast<uint8_t>(params.stopSettings.showLeanColumn);
		message.showTradeColumn = static_cast<uint8_t>(params.stopSettings.showTradeColumn);
		
		return message;
	}
};

}