#pragma once

#include "MessageType.hpp"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

struct HeartbeatMessage
{
    static constexpr MessageType type = MessageType::HEARTBEAT;
    std::string clientId;
    
    void Serialize(rapidjson::Document& document) const
    {
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        
        rapidjson::Value header(rapidjson::kObjectType);
        header.AddMember("type", 1, allocator);
        header.AddMember("length", 8, allocator);
        header.AddMember("version", 25, allocator);
        
        rapidjson::Value clientIdValue;
        clientIdValue.SetString(clientId.c_str(), static_cast<rapidjson::SizeType>(clientId.length()), allocator);
        
        document.AddMember("header", header, allocator);
        document.AddMember("clientId", clientIdValue, allocator);
    }
    
    std::string SerializeToString() const
    {
        rapidjson::Document document;
        Serialize(document);
        
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        
        return buffer.GetString();
    }
    
    static HeartbeatMessage Deserialize(const rapidjson::Document& document)
    {
        HeartbeatMessage message;
        
        // Parse clientId
        if (document.HasMember("clientId") && document["clientId"].IsString())
        {
            message.clientId = document["clientId"].GetString();
        }
        
        return message;
    }
    
    static HeartbeatMessage DeserializeFromString(const std::string& json)
    {
        rapidjson::Document document;
        document.Parse(json.c_str());
        
        if (document.HasParseError())
        {
            return HeartbeatMessage();
        }
        
        return Deserialize(document);
    }
};

}