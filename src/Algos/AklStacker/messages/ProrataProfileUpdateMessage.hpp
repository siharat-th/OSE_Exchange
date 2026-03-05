#pragma once

#include "MessageType.hpp"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <vector>

namespace akl::stacker
{

struct ProrataProfileUpdateMessage
{
	static constexpr int SIZE = 10;
	static constexpr MessageType type = MessageType::PRORATA_PROFILE_UPDATE;
	std::string name;
	std::string group;
	std::string org { "AKL"};
	std::string createdBy { "" };
	std::string updatedBy { "" };
	int32_t bookQuantity[SIZE] { 0 };
	int32_t quoteQuantity[SIZE] { 0 };
	int32_t ltpQuantity[SIZE] { 0 };
	int32_t window[SIZE] { 0 };
	int32_t addHedgeQuantity[SIZE] { 0 };
	uint8_t addHedgeEnabled[SIZE] { 0 };
	int32_t msgIndex { 0 };
	int32_t msgCount { 0 };
	int32_t levelCount { 0 };
	
	// Serialize to JSON
	void Serialize(rapidjson::Document& document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
		
		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", 49, allocator);
		header.AddMember("length", 255, allocator);
		header.AddMember("version", 25, allocator);
		
		// Create string values
		rapidjson::Value nameValue;
		nameValue.SetString(name.c_str(), static_cast<rapidjson::SizeType>(name.length()), allocator);
		
		rapidjson::Value groupValue;
		groupValue.SetString(group.c_str(), static_cast<rapidjson::SizeType>(group.length()), allocator);
		
		// Create arrays
		rapidjson::Value bookQuantityArray(rapidjson::kArrayType);
		rapidjson::Value quoteQuantityArray(rapidjson::kArrayType);
		rapidjson::Value ltpQuantityArray(rapidjson::kArrayType);
		rapidjson::Value windowArray(rapidjson::kArrayType);
		rapidjson::Value addHedgeQuantityArray(rapidjson::kArrayType);
		rapidjson::Value addHedgeEnabledArray(rapidjson::kArrayType);
		
		for (int i = 0; i < SIZE; ++i)
		{
			bookQuantityArray.PushBack(bookQuantity[i], allocator);
			quoteQuantityArray.PushBack(quoteQuantity[i], allocator);
			ltpQuantityArray.PushBack(ltpQuantity[i], allocator);
			windowArray.PushBack(window[i], allocator);
			addHedgeQuantityArray.PushBack(addHedgeQuantity[i], allocator);
			addHedgeEnabledArray.PushBack(static_cast<int>(addHedgeEnabled[i]), allocator);
		}
		
		// Add members to document
		document.AddMember("header", header, allocator);
		document.AddMember("name", nameValue, allocator);
		document.AddMember("group", groupValue, allocator);
		document.AddMember("bookQuantity", bookQuantityArray, allocator);
		document.AddMember("quoteQuantity", quoteQuantityArray, allocator);
		document.AddMember("ltpQuantity", ltpQuantityArray, allocator);
		document.AddMember("window", windowArray, allocator);
		document.AddMember("addHedgeQuantity", addHedgeQuantityArray, allocator);
		document.AddMember("addHedgeEnabled", addHedgeEnabledArray, allocator);
		document.AddMember("msgIndex", msgIndex, allocator);
		document.AddMember("msgCount", msgCount, allocator);
		document.AddMember("levelCount", levelCount, allocator);
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
	static ProrataProfileUpdateMessage Deserialize(const rapidjson::Document& document)
	{
		ProrataProfileUpdateMessage message;
		
		// Parse string fields
		if (document.HasMember("name") && document["name"].IsString())
			message.name = document["name"].GetString();
			
		if (document.HasMember("group") && document["group"].IsString())
			message.group = document["group"].GetString();
		
		// Parse arrays
		if (document.HasMember("bookQuantity") && document["bookQuantity"].IsArray())
		{
			const rapidjson::Value& array = document["bookQuantity"];
			for (rapidjson::SizeType i = 0; i < array.Size() && i < SIZE; ++i)
			{
				if (array[i].IsInt())
				{
					message.bookQuantity[i] = array[i].GetInt();
				}
			}
		}
		
		if (document.HasMember("quoteQuantity") && document["quoteQuantity"].IsArray())
		{
			const rapidjson::Value& array = document["quoteQuantity"];
			for (rapidjson::SizeType i = 0; i < array.Size() && i < SIZE; ++i)
			{
				if (array[i].IsInt())
				{
					message.quoteQuantity[i] = array[i].GetInt();
				}
			}
		}
		
		if (document.HasMember("ltpQuantity") && document["ltpQuantity"].IsArray())
		{
			const rapidjson::Value& array = document["ltpQuantity"];
			for (rapidjson::SizeType i = 0; i < array.Size() && i < SIZE; ++i)
			{
				if (array[i].IsInt())
				{
					message.ltpQuantity[i] = array[i].GetInt();
				}
			}
		}
		
		if (document.HasMember("window") && document["window"].IsArray())
		{
			const rapidjson::Value& array = document["window"];
			for (rapidjson::SizeType i = 0; i < array.Size() && i < SIZE; ++i)
			{
				if (array[i].IsInt())
				{
					message.window[i] = array[i].GetInt();
				}
			}
		}
		
		if (document.HasMember("addHedgeQuantity") && document["addHedgeQuantity"].IsArray())
		{
			const rapidjson::Value& array = document["addHedgeQuantity"];
			for (rapidjson::SizeType i = 0; i < array.Size() && i < SIZE; ++i)
			{
				if (array[i].IsInt())
				{
					message.addHedgeQuantity[i] = array[i].GetInt();
				}
			}
		}
		
		if (document.HasMember("addHedgeEnabled") && document["addHedgeEnabled"].IsArray())
		{
			const rapidjson::Value& array = document["addHedgeEnabled"];
			for (rapidjson::SizeType i = 0; i < array.Size() && i < SIZE; ++i)
			{
				if (array[i].IsInt())
				{
					message.addHedgeEnabled[i] = static_cast<uint8_t>(array[i].GetInt());
				}
			}
		}
		
		// Parse other fields
		if (document.HasMember("msgIndex") && document["msgIndex"].IsInt())
			message.msgIndex = document["msgIndex"].GetInt();
			
		if (document.HasMember("msgCount") && document["msgCount"].IsInt())
			message.msgCount = document["msgCount"].GetInt();
			
		if (document.HasMember("levelCount") && document["levelCount"].IsInt())
			message.levelCount = document["levelCount"].GetInt();
		
		return message;
	}
	
	// Deserialize from string
	static ProrataProfileUpdateMessage DeserializeFromString(const std::string& json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		
		if (document.HasParseError())
		{
			return ProrataProfileUpdateMessage();
		}
		
		return Deserialize(document);
	}

	static ProrataProfileUpdateMessage DeserializeFromRestApi(const rapidjson::Value& obj)
	{
		ProrataProfileUpdateMessage message;

		if (obj.HasMember("prorata_profile_name") && obj["prorata_profile_name"].IsString())
			message.name = obj["prorata_profile_name"].GetString();
		if (obj.HasMember("prorata_group_name") && obj["prorata_group_name"].IsString())
			message.group = obj["prorata_group_name"].GetString();
		if (obj.HasMember("org") && obj["org"].IsString())
			message.org = obj["org"].GetString();
		if (obj.HasMember("created_by") && obj["created_by"].IsString())
			message.createdBy = obj["created_by"].GetString();
		if (obj.HasMember("updated_by") && obj["updated_by"].IsString())
			message.updatedBy = obj["updated_by"].GetString();

		if (obj.HasMember("json") && obj["json"].IsString())
		{
			std::string jsonArrayStr = obj["json"].GetString();

			rapidjson::Document jsonArrayDoc;
			jsonArrayDoc.Parse(jsonArrayStr.c_str());

			if (!jsonArrayDoc.HasParseError() && jsonArrayDoc.IsArray())
			{
				message.levelCount = std::min(static_cast<int>(jsonArrayDoc.Size()), SIZE);

				for (rapidjson::SizeType i = 0; i < jsonArrayDoc.Size() && i < SIZE; ++i)
				{
					const rapidjson::Value& level = jsonArrayDoc[i];
					if (level.HasMember("BookQty") && level["BookQty"].IsInt())
						message.bookQuantity[i] = level["BookQty"].GetInt();
					if (level.HasMember("QuoteQty") && level["QuoteQty"].IsInt())
						message.quoteQuantity[i] = level["QuoteQty"].GetInt();
					if (level.HasMember("TradeQty") && level["TradeQty"].IsInt())
						message.ltpQuantity[i] = level["TradeQty"].GetInt();
					if (level.HasMember("Window") && level["Window"].IsInt())
						message.window[i] = level["Window"].GetInt();
					if (level.HasMember("AddHedgeQty") && level["AddHedgeQty"].IsInt())
						message.addHedgeQuantity[i] = level["AddHedgeQty"].GetInt();
					if (level.HasMember("AddHedgeOn") && level["AddHedgeOn"].IsBool())
						message.addHedgeEnabled[i] = level["AddHedgeOn"].GetBool() ? 1 : 0;
				}
				message.msgIndex = 0;
				message.msgCount = 1;
			}
		}

		return message;
	}

	static ProrataProfileUpdateMessage DeserializeFromRestApi(const std::string& json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());

		if (document.HasParseError())
		{
			return ProrataProfileUpdateMessage();
		}

		if (document.IsObject())
		{
			return DeserializeFromRestApi(document);
		}
		else if (document.IsArray() && document.Size() > 0)
		{
			return DeserializeFromRestApi(document[0]);
		}

		return ProrataProfileUpdateMessage();
	}

	static std::vector<ProrataProfileUpdateMessage> DeserializeMultipleProfilesFromRestApi(const std::string& json)
	{
		std::vector<ProrataProfileUpdateMessage> profiles;
		rapidjson::Document document;
		document.Parse(json.c_str());

		if (document.HasParseError())
		{
			return profiles;
		}

		if (document.IsObject())
		{
			profiles.push_back(DeserializeFromRestApi(document));
			return profiles;
		}

		if (!document.IsArray())
		{
			return profiles;
		}

		for (rapidjson::SizeType i = 0; i < document.Size(); ++i)
		{
			const rapidjson::Value& obj = document[i];
			ProrataProfileUpdateMessage message = DeserializeFromRestApi(obj);
			profiles.push_back(message);
		}

		return profiles;
	}

	std::string SerializeToRestApi() const
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

		document.AddMember(
			"prorata_profile_name",
			rapidjson::Value(name.c_str(), allocator),
			allocator);
		document.AddMember(
			"prorata_group_name",
			rapidjson::Value(group.c_str(), allocator),
			allocator);
		document.AddMember(
			"org",
			rapidjson::Value(org.c_str(), allocator),
			allocator);

		rapidjson::Document levelsArray;
		levelsArray.SetArray();

		for (int i = 0; i < levelCount && i < SIZE; ++i)
		{
			rapidjson::Value level(rapidjson::kObjectType);

			level.AddMember("BookQty", bookQuantity[i], allocator);
			level.AddMember("QuoteQty", quoteQuantity[i], allocator);
			level.AddMember("TradeQty", ltpQuantity[i], allocator);
			level.AddMember("Window", window[i], allocator);
			level.AddMember("AddHedgeQty", addHedgeQuantity[i], allocator);
			level.AddMember("AddHedgeOn", addHedgeEnabled[i] != 0, allocator);

			levelsArray.PushBack(level, allocator);
		}

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		levelsArray.Accept(writer);

		document.AddMember(
			"json",
			rapidjson::Value(buffer.GetString(), allocator),
			allocator);

		document.AddMember("created_by", rapidjson::Value(createdBy.c_str(), allocator), allocator);
		document.AddMember("created_time", rapidjson::Value(rapidjson::kNullType), allocator);
		document.AddMember("updated_by", rapidjson::Value(updatedBy.c_str(), allocator), allocator);
		document.AddMember("updated_time", rapidjson::Value(rapidjson::kNullType), allocator);

		rapidjson::StringBuffer finalBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> finalWriter(finalBuffer);
		document.Accept(finalWriter);

		return std::string(finalBuffer.GetString());
	}

};

}