#pragma once

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <AlgoParams/AklStackerParams.hpp>

namespace akl::stacker
{

// StackerProrataConfigMessage
struct StackerProrataConfigMessage
{
    static constexpr MessageType type = MessageType::STACKER_PRORATA_CONFIG;
    std::string id;
    std::string askProrataProfile;
    std::string bidProrataProfile;
    std::string alternateAskProrataProfile;
    std::string alternateBidProrataProfile;
    std::string prorataGroup;
    int32_t askAddHedgeQty { 0 };
    int32_t bidAddHedgeQty { 0 };
    int32_t alternateAskProfileDelay { 5000 };
    int32_t alternateBidProfileDelay { 5000 };
    uint8_t askProrataEnabled { 0 };
    uint8_t bidProrataEnabled { 0 };
    
    // Serialize to JSON
    void Serialize(rapidjson::Document& document) const
    {
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        
        // Create header object
        rapidjson::Value header(rapidjson::kObjectType);
        header.AddMember("type", 88, allocator);
        header.AddMember("length", 154, allocator);
        header.AddMember("version", 25, allocator);
        
        // Create string values
        rapidjson::Value idValue;
        idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);
        
        rapidjson::Value askProrataProfileValue;
        askProrataProfileValue.SetString(askProrataProfile.c_str(), static_cast<rapidjson::SizeType>(askProrataProfile.length()), allocator);
        
        rapidjson::Value bidProrataProfileValue;
        bidProrataProfileValue.SetString(bidProrataProfile.c_str(), static_cast<rapidjson::SizeType>(bidProrataProfile.length()), allocator);
        
        rapidjson::Value alternateAskProrataProfileValue;
        alternateAskProrataProfileValue.SetString(alternateAskProrataProfile.c_str(), static_cast<rapidjson::SizeType>(alternateAskProrataProfile.length()), allocator);
        
        rapidjson::Value alternateBidProrataProfileValue;
        alternateBidProrataProfileValue.SetString(alternateBidProrataProfile.c_str(), static_cast<rapidjson::SizeType>(alternateBidProrataProfile.length()), allocator);
        
        rapidjson::Value prorataGroupValue;
        prorataGroupValue.SetString(prorataGroup.c_str(), static_cast<rapidjson::SizeType>(prorataGroup.length()), allocator);
        
        // Add members to document
        document.AddMember("header", header, allocator);
        document.AddMember("id", idValue, allocator);
        document.AddMember("askProrataProfile", askProrataProfileValue, allocator);
        document.AddMember("bidProrataProfile", bidProrataProfileValue, allocator);
        document.AddMember("alternateAskProrataProfile", alternateAskProrataProfileValue, allocator);
        document.AddMember("alternateBidProrataProfile", alternateBidProrataProfileValue, allocator);
        document.AddMember("prorataGroup", prorataGroupValue, allocator);
        document.AddMember("askAddHedgeQty", askAddHedgeQty, allocator);
        document.AddMember("bidAddHedgeQty", bidAddHedgeQty, allocator);
        document.AddMember("alternateAskProfileDelay", alternateAskProfileDelay, allocator);
        document.AddMember("alternateBidProfileDelay", alternateBidProfileDelay, allocator);
        document.AddMember("askProrataEnabled", static_cast<int>(askProrataEnabled), allocator);
        document.AddMember("bidProrataEnabled", static_cast<int>(bidProrataEnabled), allocator);
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
    static StackerProrataConfigMessage Deserialize(const rapidjson::Document& document)
    {
        StackerProrataConfigMessage message;
        
        // Parse id and string fields
        if (document.HasMember("id") && document["id"].IsString())
            message.id = document["id"].GetString();
            
        if (document.HasMember("askProrataProfile") && document["askProrataProfile"].IsString())
            message.askProrataProfile = document["askProrataProfile"].GetString();
            
        if (document.HasMember("bidProrataProfile") && document["bidProrataProfile"].IsString())
            message.bidProrataProfile = document["bidProrataProfile"].GetString();
            
        if (document.HasMember("alternateAskProrataProfile") && document["alternateAskProrataProfile"].IsString())
            message.alternateAskProrataProfile = document["alternateAskProrataProfile"].GetString();
            
        if (document.HasMember("alternateBidProrataProfile") && document["alternateBidProrataProfile"].IsString())
            message.alternateBidProrataProfile = document["alternateBidProrataProfile"].GetString();
            
        if (document.HasMember("prorataGroup") && document["prorataGroup"].IsString())
            message.prorataGroup = document["prorataGroup"].GetString();
        
        // Parse numeric fields
        if (document.HasMember("askAddHedgeQty") && document["askAddHedgeQty"].IsInt())
            message.askAddHedgeQty = document["askAddHedgeQty"].GetInt();
            
        if (document.HasMember("bidAddHedgeQty") && document["bidAddHedgeQty"].IsInt())
            message.bidAddHedgeQty = document["bidAddHedgeQty"].GetInt();
            
        if (document.HasMember("alternateAskProfileDelay") && document["alternateAskProfileDelay"].IsInt())
            message.alternateAskProfileDelay = document["alternateAskProfileDelay"].GetInt();
            
        if (document.HasMember("alternateBidProfileDelay") && document["alternateBidProfileDelay"].IsInt())
            message.alternateBidProfileDelay = document["alternateBidProfileDelay"].GetInt();
            
        if (document.HasMember("askProrataEnabled") && document["askProrataEnabled"].IsInt())
            message.askProrataEnabled = static_cast<uint8_t>(document["askProrataEnabled"].GetInt());
            
        if (document.HasMember("bidProrataEnabled") && document["bidProrataEnabled"].IsInt())
            message.bidProrataEnabled = static_cast<uint8_t>(document["bidProrataEnabled"].GetInt());
        
        return message;
    }
    
    // Deserialize from string
    static StackerProrataConfigMessage DeserializeFromString(const std::string& json)
    {
        rapidjson::Document document;
        document.Parse(json.c_str());
        
        if (document.HasParseError())
        {
            return StackerProrataConfigMessage();
		}
		
		return Deserialize(document);
	}

	static StackerProrataConfigMessage FromParams(const AklStackerParams &params)
	{
		StackerProrataConfigMessage message;
		message.id = params.meta.UniqueId;
		message.askProrataProfile = params.quoteInstrumentSettings.askSettings.prorataProfile;
		message.bidProrataProfile = params.quoteInstrumentSettings.bidSettings.prorataProfile;
		message.alternateAskProrataProfile = params.quoteInstrumentSettings.askSettings.alternateProrataProfile;
		message.alternateBidProrataProfile = params.quoteInstrumentSettings.bidSettings.alternateProrataProfile;
		message.prorataGroup = params.quoteInstrumentSettings.prorataGroup;
		message.askAddHedgeQty = 0;
		message.bidAddHedgeQty = 0;
		message.alternateAskProfileDelay = params.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay;
		message.alternateBidProfileDelay = params.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay;
		message.askProrataEnabled = static_cast<uint8_t>(params.quoteInstrumentSettings.askSettings.prorataEnabled);
		message.bidProrataEnabled = static_cast<uint8_t>(params.quoteInstrumentSettings.bidSettings.prorataEnabled);
		
		return message;
	}
};

}