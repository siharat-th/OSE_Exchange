#pragma once

#include "MessageType.hpp"
#include "../Helpers.hpp"
#include <string>
#include <akl/Price.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

struct StackerPriceAdoptMessage
{
	struct level
	{
		Price price;
		KTN::ORD::KOrderSide::Enum side;
		int8_t index;
	};

	static constexpr MessageType type = MessageType::STACKER_PRICE_ADOPT;
	std::string id;
	std::vector<level> levels;

    void Serialize(rapidjson::Document &document) const
    {
        document.SetObject();
        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        
        // Create header object
        rapidjson::Value header(rapidjson::kObjectType);
        header.AddMember("type", static_cast<int>(type), allocator);
        header.AddMember("length", 160, allocator);
        header.AddMember("version", 25, allocator);
        
        // Create string value for id
        rapidjson::Value idValue;
        idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);
        
        // Add members to document
        document.AddMember("header", header, allocator);
        document.AddMember("id", idValue, allocator);
        
        // Create arrays for prices, sides, and indices
        rapidjson::Value pricesArray(rapidjson::kArrayType);
        rapidjson::Value sidesArray(rapidjson::kArrayType);
        rapidjson::Value indicesArray(rapidjson::kArrayType);
        
        // Populate arrays from levels vector
        for (const auto& level : levels)
        {
            
			const std::optional<int8_t> aklSide = KtnSideToAklSide(level.side);
			if (aklSide)
			{
				pricesArray.PushBack(level.price.AsShifted(), allocator);
				sidesArray.PushBack(static_cast<int>(*aklSide), allocator);
				indicesArray.PushBack(static_cast<int>(level.index), allocator);
			}
        }
        
        // // Ensure arrays have 10 elements (based on your example)
        // const size_t requiredSize = 10;
        // while (pricesArray.Size() < requiredSize)
        // {
        //     pricesArray.PushBack(0, allocator);
        // }
        // while (sidesArray.Size() < requiredSize)
        // {
        //     sidesArray.PushBack(0, allocator);
        // }
        // while (indicesArray.Size() < requiredSize)
        // {
        //     indicesArray.PushBack(0, allocator);
        // }
        
        // Add arrays to document
        document.AddMember("prices", pricesArray, allocator);
        document.AddMember("sides", sidesArray, allocator);
        document.AddMember("indices", indicesArray, allocator);
        
        // Add count of valid levels
        document.AddMember("count", static_cast<int>(levels.size()), allocator);
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
		static StackerPriceAdoptMessage Deserialize(const rapidjson::Document& document)
		{
			StackerPriceAdoptMessage message;
			
			// Parse id
			if (document.HasMember("id") && document["id"].IsString())
			{
				message.id = document["id"].GetString();
			}
			
			// Check for required arrays and count
			if (document.HasMember("prices") && document["prices"].IsArray() &&
				document.HasMember("sides") && document["sides"].IsArray() &&
				document.HasMember("indices") && document["indices"].IsArray() &&
				document.HasMember("count") && document["count"].IsInt())
			{
				const rapidjson::Value& pricesArray = document["prices"];
				const rapidjson::Value& sidesArray = document["sides"];
				const rapidjson::Value& indicesArray = document["indices"];
				int count = document["count"].GetInt();
				
				// Resize the levels vector to match count
				message.levels.clear();
				
				// Only process up to 'count' entries or the smallest array size
				int maxIndex = std::min(static_cast<uint32_t>(count), 
								std::min(pricesArray.Size(), 
									std::min(sidesArray.Size(), indicesArray.Size())));
				
				for (int i = 0; i < maxIndex; ++i)
				{
					level newLevel;
					
					// Extract values from arrays
					if (pricesArray[i].IsInt64())
					{
						newLevel.price = Price::FromShifted(pricesArray[i].GetInt64());
					}
					
					if (sidesArray[i].IsInt())
					{
						newLevel.side = AklSideToKtnSide(sidesArray[i].GetInt());
					}
					
					if (indicesArray[i].IsInt())
					{
						newLevel.index = static_cast<int8_t>(indicesArray[i].GetInt());
					}
					
					message.levels.push_back(newLevel);
				}
			}
			
			return message;
		}
		
		// Deserialize from string
		static StackerPriceAdoptMessage DeserializeFromString(const std::string& json)
		{
			rapidjson::Document document;
			document.Parse(json.c_str());
			
			if (document.HasParseError())
			{
				// Handle parsing error
				// You might want to throw an exception or return a default message
				return StackerPriceAdoptMessage();
			}
			
			return Deserialize(document);
		}

};

}