#pragma once

#include "MessageType.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include <AlgoParams/AklStackerParams.hpp>

namespace akl::stacker
{

// StackerConfigMessage - First, define the SideConfig struct
struct StackerSideConfig
{
    int64_t altPrice { std::numeric_limits<int64_t>::max() };
    int64_t altPrice2 { std::numeric_limits<int64_t>::max() };
    int32_t lean { 0 };
    int32_t lean2 { 0 };
    int32_t payup { 0 };
    int32_t winnerPayup { 0 };
    int32_t stackOrderQty { 0 };
    int32_t stackOrderQty2 { 0 };
    int32_t timeoutCover { 0 };
    int32_t quickLean { 0 };
    int32_t quickLean2 { 0 };
    int32_t quickLeanWindow { 0 };
    int32_t quickLeanWindow2 { 0 };
    int32_t stackLength { 0 };
    int32_t stackLength2 { 0 };
    int32_t maxPayupTicks { 0 };
    int32_t manualAdjust { 0 };
    int32_t manualAdjust2 { 0 };
    uint8_t quickAddEnabled { 0 };
    uint8_t quickAddEnabled2 { 0 };
    uint8_t noAggressing { 0 };
    uint8_t noAggressing2 { 0 };
    int32_t altManualAdj { 0 };
    int32_t altManualAdj2 { 0 };
    
    // Serialize to a Value
    rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) const
    {
        rapidjson::Value configObj(rapidjson::kObjectType);
        
        configObj.AddMember("altPrice", altPrice, allocator);
        configObj.AddMember("altPrice2", altPrice2, allocator);
        configObj.AddMember("lean", lean, allocator);
        configObj.AddMember("lean2", lean2, allocator);
        configObj.AddMember("payup", payup, allocator);
        configObj.AddMember("winnerPayup", winnerPayup, allocator);
        configObj.AddMember("stackOrderQty", stackOrderQty, allocator);
        configObj.AddMember("stackOrderQty2", stackOrderQty2, allocator);
        configObj.AddMember("timeoutCover", timeoutCover, allocator);
        configObj.AddMember("quickLean", quickLean, allocator);
        configObj.AddMember("quickLean2", quickLean2, allocator);
        configObj.AddMember("quickLeanWindow", quickLeanWindow, allocator);
        configObj.AddMember("quickLeanWindow2", quickLeanWindow2, allocator);
        configObj.AddMember("stackLength", stackLength, allocator);
        configObj.AddMember("stackLength2", stackLength2, allocator);
        configObj.AddMember("maxPayupTicks", maxPayupTicks, allocator);
        configObj.AddMember("manualAdjust", manualAdjust, allocator);
        configObj.AddMember("manualAdjust2", manualAdjust2, allocator);
        configObj.AddMember("quickAddEnabled", static_cast<int>(quickAddEnabled), allocator);
        configObj.AddMember("quickAddEnabled2", static_cast<int>(quickAddEnabled2), allocator);
        configObj.AddMember("noAggressing", static_cast<int>(noAggressing), allocator);
        configObj.AddMember("noAggressing2", static_cast<int>(noAggressing2), allocator);
        configObj.AddMember("altManualAdj", altManualAdj, allocator);
        configObj.AddMember("altManualAdj2", altManualAdj2, allocator);
        
        return configObj;
    }
    
    // Deserialize from a Value
    static StackerSideConfig Deserialize(const rapidjson::Value& value)
    {
        StackerSideConfig config;
        
        if (value.IsObject())
        {
            if (value.HasMember("altPrice") && value["altPrice"].IsInt64())
                config.altPrice = value["altPrice"].GetInt64();
                
            if (value.HasMember("altPrice2") && value["altPrice2"].IsInt64())
                config.altPrice2 = value["altPrice2"].GetInt64();
                
            if (value.HasMember("lean") && value["lean"].IsInt())
                config.lean = value["lean"].GetInt();
                
            if (value.HasMember("lean2") && value["lean2"].IsInt())
                config.lean2 = value["lean2"].GetInt();
                
            if (value.HasMember("payup") && value["payup"].IsInt())
                config.payup = value["payup"].GetInt();
                
            if (value.HasMember("winnerPayup") && value["winnerPayup"].IsInt())
                config.winnerPayup = value["winnerPayup"].GetInt();
                
            if (value.HasMember("stackOrderQty") && value["stackOrderQty"].IsInt())
                config.stackOrderQty = value["stackOrderQty"].GetInt();
                
            if (value.HasMember("stackOrderQty2") && value["stackOrderQty2"].IsInt())
                config.stackOrderQty2 = value["stackOrderQty2"].GetInt();
                
            if (value.HasMember("timeoutCover") && value["timeoutCover"].IsInt())
                config.timeoutCover = value["timeoutCover"].GetInt();
                
            if (value.HasMember("quickLean") && value["quickLean"].IsInt())
                config.quickLean = value["quickLean"].GetInt();
                
            if (value.HasMember("quickLean2") && value["quickLean2"].IsInt())
                config.quickLean2 = value["quickLean2"].GetInt();
                
            if (value.HasMember("quickLeanWindow") && value["quickLeanWindow"].IsInt())
                config.quickLeanWindow = value["quickLeanWindow"].GetInt();
                
            if (value.HasMember("quickLeanWindow2") && value["quickLeanWindow2"].IsInt())
                config.quickLeanWindow2 = value["quickLeanWindow2"].GetInt();
                
            if (value.HasMember("stackLength") && value["stackLength"].IsInt())
                config.stackLength = value["stackLength"].GetInt();
                
            if (value.HasMember("stackLength2") && value["stackLength2"].IsInt())
                config.stackLength2 = value["stackLength2"].GetInt();
                
            if (value.HasMember("maxPayupTicks") && value["maxPayupTicks"].IsInt())
                config.maxPayupTicks = value["maxPayupTicks"].GetInt();
                
            if (value.HasMember("manualAdjust") && value["manualAdjust"].IsInt())
                config.manualAdjust = value["manualAdjust"].GetInt();
                
            if (value.HasMember("manualAdjust2") && value["manualAdjust2"].IsInt())
                config.manualAdjust2 = value["manualAdjust2"].GetInt();
                
            if (value.HasMember("quickAddEnabled") && value["quickAddEnabled"].IsInt())
                config.quickAddEnabled = static_cast<uint8_t>(value["quickAddEnabled"].GetInt());
                
            if (value.HasMember("quickAddEnabled2") && value["quickAddEnabled2"].IsInt())
                config.quickAddEnabled2 = static_cast<uint8_t>(value["quickAddEnabled2"].GetInt());
                
            if (value.HasMember("noAggressing") && value["noAggressing"].IsInt())
                config.noAggressing = static_cast<uint8_t>(value["noAggressing"].GetInt());
                
            if (value.HasMember("noAggressing2") && value["noAggressing2"].IsInt())
                config.noAggressing2 = static_cast<uint8_t>(value["noAggressing2"].GetInt());
                
            if (value.HasMember("altManualAdj") && value["altManualAdj"].IsInt())
                config.altManualAdj = value["altManualAdj"].GetInt();
                
            if (value.HasMember("altManualAdj2") && value["altManualAdj2"].IsInt())
                config.altManualAdj2 = value["altManualAdj2"].GetInt();
        }
        
        return config;
    }

	static StackerSideConfig FromParams(const QuoteSideSettings &settings)
	{
		StackerSideConfig config;
		config.altPrice = settings.stackSideSettings[0].altManualAdjPrice ?
			settings.stackSideSettings[0].altManualAdjPrice->AsShifted() : std::numeric_limits<int64_t>::max();
		config.altPrice2 = settings.stackSideSettings[1].altManualAdjPrice ?
			settings.stackSideSettings[1].altManualAdjPrice->AsShifted() : std::numeric_limits<int64_t>::max();
		config.lean = settings.stackSideSettings[0].lean.value();
		config.lean2 = settings.stackSideSettings[1].lean.value();
		config.payup = settings.payup.value();
		config.winnerPayup = settings.winnerPayup.value();
		config.stackOrderQty = settings.stackSideSettings[0].qty.value();
		config.stackOrderQty2 = settings.stackSideSettings[1].qty.value();
		config.timeoutCover = settings.timeoutCover;
		config.quickLean = settings.stackSideSettings[0].quickAddLean.value();
		config.quickLean2 = settings.stackSideSettings[1].quickAddLean.value();
		config.quickLeanWindow = settings.stackSideSettings[0].quickAddTime;
		config.quickLeanWindow2 = settings.stackSideSettings[1].quickAddTime;
		config.stackLength = settings.stackSideSettings[0].length;
		config.stackLength2 = settings.stackSideSettings[1].length;
		config.maxPayupTicks = settings.maxPayupTicks;
		config.manualAdjust = settings.stackSideSettings[0].manualAdj;
		config.manualAdjust2 = settings.stackSideSettings[1].manualAdj;
		config.quickAddEnabled = static_cast<uint8_t>(settings.stackSideSettings[0].quickAddEnabled);
		config.quickAddEnabled2 = static_cast<uint8_t>(settings.stackSideSettings[1].quickAddEnabled);
		config.noAggressing = static_cast<uint8_t>(settings.stackSideSettings[0].noAggressing);
		config.noAggressing2 = static_cast<uint8_t>(settings.stackSideSettings[1].noAggressing);
		config.altManualAdj = settings.stackSideSettings[0].altManualAdj;
		config.altManualAdj2 = settings.stackSideSettings[1].altManualAdj;
		return config;
	}
};

// Now the main StackerConfigMessage
struct StackerConfigMessage
{
    static constexpr MessageType type = MessageType::STACKER_CONFIG;
    std::string id;
    StackerSideConfig bidConfig;
    StackerSideConfig askConfig;
    double randomLotSizeMult { 0 };
    double prorataAllocation { 0 };
    int32_t leanPayupIncrement { 1 };
    int32_t stackRequoteDelay { 1000 };
    int32_t maxOrderQtyInternal { 1000 };
    int32_t maxPositionInternal { 10000 };
    int32_t rateLimit { 50 };
    int32_t rateLimitWindow { 1000 };
    int32_t stackRequoteCount { 0 };
    int32_t prorataRequoteCount { 0 };
    uint8_t newOrders { 0 };
    uint8_t noPayup { 0 };
    uint8_t moveWithCancelNew { 0 };
    uint8_t requoteOnMove { 0 };
    uint8_t quoteInPreopen { 0 };
    uint8_t preventAlgoCancelWhenWorkingOrders { 0 };
    uint8_t adoptFromUnknownStrategies { 0 };
    uint8_t hedgeOnLargeTrade { 0 };
    uint8_t hedgeOnLargeFill { 0 };
    uint8_t linkLeanPayup { 0 };
    uint8_t cancelUnadoptedOrders { 0 };
    uint8_t dynamicStack { 0 };
    uint8_t secondStackEnabled { 0 };
    uint8_t showProrata { 1 };
    uint8_t showAltManualAdj { 1 };
    
    // Serialize to JSON
    void Serialize(rapidjson::Document& document) const
    {
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        
        // Create header object
        rapidjson::Value header(rapidjson::kObjectType);
        header.AddMember("type", 80, allocator);
        header.AddMember("length", 257, allocator);
        header.AddMember("version", 25, allocator);
        
        // Create string value for id
        rapidjson::Value idValue;
        idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);
        
        // Add members to document
        document.AddMember("header", header, allocator);
        document.AddMember("id", idValue, allocator);
        document.AddMember("bidConfig", bidConfig.Serialize(allocator), allocator);
        document.AddMember("askConfig", askConfig.Serialize(allocator), allocator);
        document.AddMember("randomLotSizeMult", randomLotSizeMult, allocator);
        document.AddMember("prorataAllocation", prorataAllocation, allocator);
        document.AddMember("leanPayupIncrement", leanPayupIncrement, allocator);
        document.AddMember("stackRequoteDelay", stackRequoteDelay, allocator);
        document.AddMember("maxOrderQtyInternal", maxOrderQtyInternal, allocator);
        document.AddMember("maxPositionInternal", maxPositionInternal, allocator);
        document.AddMember("rateLimit", rateLimit, allocator);
        document.AddMember("rateLimitWindow", rateLimitWindow, allocator);
        document.AddMember("newOrders", static_cast<int>(newOrders), allocator);
        document.AddMember("noPayup", static_cast<int>(noPayup), allocator);
        document.AddMember("moveWithCancelNew", static_cast<int>(moveWithCancelNew), allocator);
        document.AddMember("requoteOnMove", static_cast<int>(requoteOnMove), allocator);
        document.AddMember("quoteInPreopen", static_cast<int>(quoteInPreopen), allocator);
        document.AddMember("preventAlgoCancelWhenWorkingOrders", static_cast<int>(preventAlgoCancelWhenWorkingOrders), allocator);
        document.AddMember("adoptFromUnknownStrategies", static_cast<int>(adoptFromUnknownStrategies), allocator);
        document.AddMember("hedgeOnLargeTrade", static_cast<int>(hedgeOnLargeTrade), allocator);
        document.AddMember("hedgeOnLargeFill", static_cast<int>(hedgeOnLargeFill), allocator);
        document.AddMember("linkLeanPayup", static_cast<int>(linkLeanPayup), allocator);
        document.AddMember("cancelUnadoptedOrders", static_cast<int>(cancelUnadoptedOrders), allocator);
        document.AddMember("stackRequoteCount", stackRequoteCount, allocator);
        document.AddMember("prorataRequoteCount", prorataRequoteCount, allocator);
        document.AddMember("dynamicStack", static_cast<int>(dynamicStack), allocator);
        document.AddMember("secondStackEnabled", static_cast<int>(secondStackEnabled), allocator);
        document.AddMember("showProrata", static_cast<int>(showProrata), allocator);
        document.AddMember("showAltManualAdj", static_cast<int>(showAltManualAdj), allocator);
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
    static StackerConfigMessage Deserialize(const rapidjson::Document& document)
    {
        StackerConfigMessage message;
        
        // Parse id
        if (document.HasMember("id") && document["id"].IsString())
        {
            message.id = document["id"].GetString();
        }
        
        // Parse configurations
        if (document.HasMember("bidConfig") && document["bidConfig"].IsObject())
        {
            message.bidConfig = StackerSideConfig::Deserialize(document["bidConfig"]);
        }
        
        if (document.HasMember("askConfig") && document["askConfig"].IsObject())
        {
            message.askConfig = StackerSideConfig::Deserialize(document["askConfig"]);
        }
        
        // Parse simple fields
        if (document.HasMember("randomLotSizeMult") && document["randomLotSizeMult"].IsInt())
            message.randomLotSizeMult = document["randomLotSizeMult"].GetDouble();
            
        if (document.HasMember("prorataAllocation") && document["prorataAllocation"].IsInt())
            message.prorataAllocation = document["prorataAllocation"].GetDouble();
            
        if (document.HasMember("leanPayupIncrement") && document["leanPayupIncrement"].IsInt())
            message.leanPayupIncrement = document["leanPayupIncrement"].GetInt();
            
        if (document.HasMember("stackRequoteDelay") && document["stackRequoteDelay"].IsInt())
            message.stackRequoteDelay = document["stackRequoteDelay"].GetInt();
            
        if (document.HasMember("maxOrderQtyInternal") && document["maxOrderQtyInternal"].IsInt())
            message.maxOrderQtyInternal = document["maxOrderQtyInternal"].GetInt();
            
        if (document.HasMember("maxPositionInternal") && document["maxPositionInternal"].IsInt())
            message.maxPositionInternal = document["maxPositionInternal"].GetInt();
            
        if (document.HasMember("rateLimit") && document["rateLimit"].IsInt())
            message.rateLimit = document["rateLimit"].GetInt();
            
        if (document.HasMember("rateLimitWindow") && document["rateLimitWindow"].IsInt())
            message.rateLimitWindow = document["rateLimitWindow"].GetInt();
            
        if (document.HasMember("newOrders") && document["newOrders"].IsInt())
            message.newOrders = static_cast<uint8_t>(document["newOrders"].GetInt());
            
        if (document.HasMember("noPayup") && document["noPayup"].IsInt())
            message.noPayup = static_cast<uint8_t>(document["noPayup"].GetInt());
            
        if (document.HasMember("moveWithCancelNew") && document["moveWithCancelNew"].IsInt())
            message.moveWithCancelNew = static_cast<uint8_t>(document["moveWithCancelNew"].GetInt());
            
        if (document.HasMember("requoteOnMove") && document["requoteOnMove"].IsInt())
            message.requoteOnMove = static_cast<uint8_t>(document["requoteOnMove"].GetInt());
            
        if (document.HasMember("quoteInPreopen") && document["quoteInPreopen"].IsInt())
            message.quoteInPreopen = static_cast<uint8_t>(document["quoteInPreopen"].GetInt());
            
        if (document.HasMember("preventAlgoCancelWhenWorkingOrders") && document["preventAlgoCancelWhenWorkingOrders"].IsInt())
            message.preventAlgoCancelWhenWorkingOrders = static_cast<uint8_t>(document["preventAlgoCancelWhenWorkingOrders"].GetInt());
            
        if (document.HasMember("adoptFromUnknownStrategies") && document["adoptFromUnknownStrategies"].IsInt())
            message.adoptFromUnknownStrategies = static_cast<uint8_t>(document["adoptFromUnknownStrategies"].GetInt());
            
        if (document.HasMember("hedgeOnLargeTrade") && document["hedgeOnLargeTrade"].IsInt())
            message.hedgeOnLargeTrade = static_cast<uint8_t>(document["hedgeOnLargeTrade"].GetInt());
            
        if (document.HasMember("hedgeOnLargeFill") && document["hedgeOnLargeFill"].IsInt())
            message.hedgeOnLargeFill = static_cast<uint8_t>(document["hedgeOnLargeFill"].GetInt());
            
        if (document.HasMember("linkLeanPayup") && document["linkLeanPayup"].IsInt())
            message.linkLeanPayup = static_cast<uint8_t>(document["linkLeanPayup"].GetInt());
            
        if (document.HasMember("cancelUnadoptedOrders") && document["cancelUnadoptedOrders"].IsInt())
            message.cancelUnadoptedOrders = static_cast<uint8_t>(document["cancelUnadoptedOrders"].GetInt());
            
        if (document.HasMember("stackRequoteCount") && document["stackRequoteCount"].IsInt())
            message.stackRequoteCount = document["stackRequoteCount"].GetInt();
            
        if (document.HasMember("prorataRequoteCount") && document["prorataRequoteCount"].IsInt())
            message.prorataRequoteCount = document["prorataRequoteCount"].GetInt();
            
        if (document.HasMember("dynamicStack") && document["dynamicStack"].IsInt())
            message.dynamicStack = static_cast<uint8_t>(document["dynamicStack"].GetInt());
            
        if (document.HasMember("secondStackEnabled") && document["secondStackEnabled"].IsInt())
            message.secondStackEnabled = static_cast<uint8_t>(document["secondStackEnabled"].GetInt());
            
        if (document.HasMember("showProrata") && document["showProrata"].IsInt())
            message.showProrata = static_cast<uint8_t>(document["showProrata"].GetInt());
            
        if (document.HasMember("showAltManualAdj") && document["showAltManualAdj"].IsInt())
            message.showAltManualAdj = static_cast<uint8_t>(document["showAltManualAdj"].GetInt());
        
        return message;
    }
    
    // Deserialize from string
    static StackerConfigMessage DeserializeFromString(const std::string& json)
    {
        rapidjson::Document document;
        document.Parse(json.c_str());
        
        if (document.HasParseError())
        {
            return StackerConfigMessage();
        }
        
        return Deserialize(document);
    }

	static StackerConfigMessage FromParams(const AklStackerParams &params)
	{
		StackerConfigMessage message;
		message.id = params.meta.UniqueId;
		message.bidConfig = StackerSideConfig::FromParams(params.quoteInstrumentSettings.bidSettings);
		message.askConfig = StackerSideConfig::FromParams(params.quoteInstrumentSettings.askSettings);
		message.randomLotSizeMult = params.quoteInstrumentSettings.randomLotSizeMult;
		message.prorataAllocation = params.quoteInstrumentSettings.prorataAllocation;
		message.leanPayupIncrement = params.quoteInstrumentSettings.leanPayupIncrement;
		message.stackRequoteDelay = params.quoteInstrumentSettings.requoteDelay;
		message.maxOrderQtyInternal = params.quoteInstrumentSettings.riskSettings.maxOrderQty.value();
		message.maxPositionInternal = params.quoteInstrumentSettings.riskSettings.maxPosition.value();
		message.rateLimit = params.quoteInstrumentSettings.rateLimitSettings.limit;
		message.rateLimitWindow = params.quoteInstrumentSettings.rateLimitSettings.window;
		message.stackRequoteCount = params.quoteInstrumentSettings.requoteCount;
		message.prorataRequoteCount = params.quoteInstrumentSettings.prorataRequoteCount;
		message.newOrders = params.quoteInstrumentSettings.newOrders ? 1 : 0;
		message.noPayup = 0;
		message.moveWithCancelNew = params.quoteInstrumentSettings.moveWithCancelNew ? 1 : 0;
		message.requoteOnMove = 0;
		message.quoteInPreopen = params.quoteInstrumentSettings.quoteInPreopen ? 1 : 0;
		message.preventAlgoCancelWhenWorkingOrders = params.quoteInstrumentSettings.preventAlgoCancelWhenWorkingOrders ? 1 : 0;
		message.adoptFromUnknownStrategies = params.quoteInstrumentSettings.adoptFromUnknownStrategies ? 1 : 0;
		message.hedgeOnLargeTrade = params.quoteInstrumentSettings.hedgeOnLargeTrade ? 1 : 0;
		message.hedgeOnLargeFill = params.quoteInstrumentSettings.hedgeOnLargeFill ? 1 : 0;
		message.linkLeanPayup = params.quoteInstrumentSettings.linkLeanPayup ? 1 : 0;
		message.cancelUnadoptedOrders = params.quoteInstrumentSettings.cancelUnadoptedOrders ? 1 : 0;
		message.dynamicStack = params.quoteInstrumentSettings.dynamicStack ? 1 : 0;
		message.secondStackEnabled = params.quoteInstrumentSettings.secondStackEnabled ? 1 : 0;
		message.showProrata = params.quoteInstrumentSettings.showProrata ? 1 : 0;
		message.showAltManualAdj = params.quoteInstrumentSettings.showAltManualAdj ? 1 : 0;
		return message;
	}
};

}