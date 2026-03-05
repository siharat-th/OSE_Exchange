#pragma once

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <AlgoParams/AklStackerParams.hpp>

namespace akl::stacker
{

// StackerPriceBandsMessage
struct StackerPriceBandsMessage
{
    static constexpr MessageType type = MessageType::STACKER_PRICE_BANDS;
    std::string id;
    int64_t askOutsidePrice { std::numeric_limits<int64_t>::max() };
    int64_t askInsidePrice { std::numeric_limits<int64_t>::min() };
    int64_t bidOutsidePrice { std::numeric_limits<int64_t>::max() };
    int64_t bidInsidePrice { std::numeric_limits<int64_t>::min() };
    int64_t askOutsidePrice2 { std::numeric_limits<int64_t>::max() };
    int64_t askInsidePrice2 { std::numeric_limits<int64_t>::min() };
    int64_t bidOutsidePrice2 { std::numeric_limits<int64_t>::max() };
    int64_t bidInsidePrice2 { std::numeric_limits<int64_t>::min() };
    
    // Serialize to JSON
    void Serialize(rapidjson::Document& document) const
    {
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        
        // Create header object
        rapidjson::Value header(rapidjson::kObjectType);
        header.AddMember("type", 103, allocator);
        header.AddMember("length", 120, allocator);
        header.AddMember("version", 25, allocator);
        
        // Create string value for id
        rapidjson::Value idValue;
        idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);
        
        // Add members to document
        document.AddMember("header", header, allocator);
        document.AddMember("id", idValue, allocator);
        document.AddMember("askOutsidePrice", askOutsidePrice, allocator);
        document.AddMember("askInsidePrice", askInsidePrice, allocator);
        document.AddMember("bidOutsidePrice", bidOutsidePrice, allocator);
        document.AddMember("bidInsidePrice", bidInsidePrice, allocator);
        document.AddMember("askOutsidePrice2", askOutsidePrice2, allocator);
        document.AddMember("askInsidePrice2", askInsidePrice2, allocator);
        document.AddMember("bidOutsidePrice2", bidOutsidePrice2, allocator);
        document.AddMember("bidInsidePrice2", bidInsidePrice2, allocator);
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
    static StackerPriceBandsMessage Deserialize(const rapidjson::Document& document)
    {
        StackerPriceBandsMessage message;
        
        // Parse id
        if (document.HasMember("id") && document["id"].IsString())
        {
            message.id = document["id"].GetString();
        }
        
        // Parse price fields
        if (document.HasMember("askOutsidePrice") && document["askOutsidePrice"].IsInt64())
            message.askOutsidePrice = document["askOutsidePrice"].GetInt64();
            
        if (document.HasMember("askInsidePrice") && document["askInsidePrice"].IsInt64())
            message.askInsidePrice = document["askInsidePrice"].GetInt64();
            
        if (document.HasMember("bidOutsidePrice") && document["bidOutsidePrice"].IsInt64())
            message.bidOutsidePrice = document["bidOutsidePrice"].GetInt64();
            
        if (document.HasMember("bidInsidePrice") && document["bidInsidePrice"].IsInt64())
            message.bidInsidePrice = document["bidInsidePrice"].GetInt64();
            
        if (document.HasMember("askOutsidePrice2") && document["askOutsidePrice2"].IsInt64())
            message.askOutsidePrice2 = document["askOutsidePrice2"].GetInt64();
            
        if (document.HasMember("askInsidePrice2") && document["askInsidePrice2"].IsInt64())
            message.askInsidePrice2 = document["askInsidePrice2"].GetInt64();
            
        if (document.HasMember("bidOutsidePrice2") && document["bidOutsidePrice2"].IsInt64())
            message.bidOutsidePrice2 = document["bidOutsidePrice2"].GetInt64();
            
        if (document.HasMember("bidInsidePrice2") && document["bidInsidePrice2"].IsInt64())
            message.bidInsidePrice2 = document["bidInsidePrice2"].GetInt64();
        
        return message;
    }
    
    // Deserialize from string
    static StackerPriceBandsMessage DeserializeFromString(const std::string& json)
    {
        rapidjson::Document document;
        document.Parse(json.c_str());
        
        if (document.HasParseError())
        {
            return StackerPriceBandsMessage();
        }
        
        return Deserialize(document);
    }

	static StackerPriceBandsMessage FromParams(const AklStackerParams &params)
	{
		StackerPriceBandsMessage message;
		message.id = params.meta.UniqueId;

		const QuoteInstrumentSettings &quoteSettings = params.quoteInstrumentSettings;
		const QuoteSideSettings &bidSettings = quoteSettings.bidSettings;
		const QuoteSideSettings &askSettings = quoteSettings.askSettings;

		message.askOutsidePrice = askSettings.stackSideSettings[0].outsidePrice ? askSettings.stackSideSettings[0].outsidePrice->AsShifted() : std::numeric_limits<int64_t>::max();
		message.askInsidePrice = askSettings.stackSideSettings[0].insidePrice ? askSettings.stackSideSettings[0].insidePrice->AsShifted() : std::numeric_limits<int64_t>::min();
		message.bidOutsidePrice = bidSettings.stackSideSettings[0].outsidePrice ? bidSettings.stackSideSettings[0].outsidePrice->AsShifted() : std::numeric_limits<int64_t>::max();
		message.bidInsidePrice = bidSettings.stackSideSettings[0].insidePrice ? bidSettings.stackSideSettings[0].insidePrice->AsShifted() : std::numeric_limits<int64_t>::min();
		message.askOutsidePrice2 = askSettings.stackSideSettings[1].outsidePrice ? askSettings.stackSideSettings[1].outsidePrice->AsShifted() : std::numeric_limits<int64_t>::max();
		message.askInsidePrice2 = askSettings.stackSideSettings[1].insidePrice ? askSettings.stackSideSettings[1].insidePrice->AsShifted() : std::numeric_limits<int64_t>::min();
		message.bidOutsidePrice2 = bidSettings.stackSideSettings[1].outsidePrice ? bidSettings.stackSideSettings[1].outsidePrice->AsShifted() : std::numeric_limits<int64_t>::max();
		message.bidInsidePrice2 = bidSettings.stackSideSettings[1].insidePrice ? bidSettings.stackSideSettings[1].insidePrice->AsShifted() : std::numeric_limits<int64_t>::min();

		return message;
	}
};

}