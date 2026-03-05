#pragma once

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

// StrategyStatusMessage
struct StrategyStatusMessage
{
    static constexpr MessageType type = MessageType::STRATEGY_STATUS;
    std::string id;
    int32_t status { 0 };
    
    // Serialize to JSON
    void Serialize(rapidjson::Document& document) const
    {
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        
        // Create header object
        rapidjson::Value header(rapidjson::kObjectType);
        header.AddMember("type", 19, allocator);
        header.AddMember("length", 57, allocator);
        header.AddMember("version", 25, allocator);
        
        // Create string value for id
        rapidjson::Value idValue;
        idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);
        
        // Add members to document
        document.AddMember("header", header, allocator);
        document.AddMember("id", idValue, allocator);
        document.AddMember("status", status, allocator);
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
    static StrategyStatusMessage Deserialize(const rapidjson::Document& document)
    {
        StrategyStatusMessage message;
        
        // Parse id
        if (document.HasMember("id") && document["id"].IsString())
        {
            message.id = document["id"].GetString();
        }
        
        // Parse status
        if (document.HasMember("status") && document["status"].IsInt())
        {
            message.status = document["status"].GetInt();
        }
        
        return message;
    }
    
    // Deserialize from string
    static StrategyStatusMessage DeserializeFromString(const std::string& json)
    {
        rapidjson::Document document;
        document.Parse(json.c_str());
        
        if (document.HasParseError())
        {
            return StrategyStatusMessage();
        }
        
        return Deserialize(document);
    }
};

}