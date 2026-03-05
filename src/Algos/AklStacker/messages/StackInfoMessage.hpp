#pragma once

#include <inttypes.h>
#include <limits>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include "MessageType.hpp"

namespace akl::stacker
{

struct StackInfoMessage
{
	static constexpr int SIZE = 10;
	static constexpr MessageType type = MessageType::STACK_INFO;
	//MessageHeader header { MessageType::STACK_INFO, sizeof(decltype(*this)) };
	std::string id { "" };
	int32_t askStackQty[SIZE] { 0 };
	int32_t bidStackQty[SIZE] { 0 };
	int64_t stackAskPrice { std::numeric_limits<int64_t>::max() };
	int64_t stackBidPrice { std::numeric_limits<int64_t>::min() };
	int8_t index { 0 };

    // Serialize to JSON
    void Serialize(rapidjson::Document& document) const
    {
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        
        // Create header object
        rapidjson::Value header(rapidjson::kObjectType);
        header.AddMember("type", static_cast<int>(type), allocator);
        header.AddMember("length", 153, allocator);
        header.AddMember("version", 25, allocator);
        
        // Create string value for id (needs to handle fixed char array)
        rapidjson::Value idValue;
        //idValue.SetString(id, strnlen(id, sizeof(id)), allocator);
        idValue.SetString(id.c_str(), id.length(), allocator);

        // Create arrays for askQtys and bidQtys
        rapidjson::Value askQtysArray(rapidjson::kArrayType);
        rapidjson::Value bidQtysArray(rapidjson::kArrayType);
        
        // Add elements to arrays
        for (int i = 0; i < SIZE; ++i)
        {
            askQtysArray.PushBack(askStackQty[i], allocator);
            bidQtysArray.PushBack(bidStackQty[i], allocator);
        }
        
        // Add members to document
        document.AddMember("header", header, allocator);
        document.AddMember("id", idValue, allocator);
        document.AddMember("askQtys", askQtysArray, allocator);
        document.AddMember("bidQtys", bidQtysArray, allocator);
        document.AddMember("stackAskPrice", stackAskPrice, allocator);
        document.AddMember("stackBidPrice", stackBidPrice, allocator);
        document.AddMember("index", static_cast<int>(index), allocator);
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
    static StackInfoMessage Deserialize(const rapidjson::Document& document)
    {
        StackInfoMessage message;
        
        // Parse id
        if (document.HasMember("id") && document["id"].IsString())
        {
			message.id = document["id"].GetString();
            //strncpy(message.id, document["id"].GetString(), sizeof(message.id) - 1);
            //message.id[sizeof(message.id) - 1] = '\0'; // Ensure null termination
        }
        
        // Parse askQtys array
        if (document.HasMember("askQtys") && document["askQtys"].IsArray())
        {
            const rapidjson::Value& askQtysArray = document["askQtys"];
            for (rapidjson::SizeType i = 0; i < askQtysArray.Size() && i < SIZE; ++i)
            {
                if (askQtysArray[i].IsInt())
                {
                    message.askStackQty[i] = askQtysArray[i].GetInt();
                }
            }
        }
        
        // Parse bidQtys array
        if (document.HasMember("bidQtys") && document["bidQtys"].IsArray())
        {
            const rapidjson::Value& bidQtysArray = document["bidQtys"];
            for (rapidjson::SizeType i = 0; i < bidQtysArray.Size() && i < SIZE; ++i)
            {
                if (bidQtysArray[i].IsInt())
                {
                    message.bidStackQty[i] = bidQtysArray[i].GetInt();
                }
            }
        }
        
        // Parse stackAskPrice
        if (document.HasMember("stackAskPrice") && document["stackAskPrice"].IsInt64())
        {
            message.stackAskPrice = document["stackAskPrice"].GetInt64();
        }
        
        // Parse stackBidPrice
        if (document.HasMember("stackBidPrice") && document["stackBidPrice"].IsInt64())
        {
            message.stackBidPrice = document["stackBidPrice"].GetInt64();
        }
        
        // Parse index
        if (document.HasMember("index") && document["index"].IsInt())
        {
            message.index = static_cast<int8_t>(document["index"].GetInt());
        }
        
        return message;
    }
    
    // Deserialize from string
    static StackInfoMessage DeserializeFromString(const std::string& json)
    {
        rapidjson::Document document;
        document.Parse(json.c_str());
        
        if (document.HasParseError())
        {
            // Handle parsing error
            // You might want to throw an exception or return a default message
            return StackInfoMessage();
        }
        
        return Deserialize(document);
    }
};
	

}