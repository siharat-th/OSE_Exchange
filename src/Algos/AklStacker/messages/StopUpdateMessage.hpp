#pragma once

#include <inttypes.h>
#include <limits>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include "MessageType.hpp"
#include "../Helpers.hpp"
#include <akl/Price.hpp>
#include <akl/Quantity.hpp>
#include <Orders/OrderEnumsV2.hpp>

namespace akl::stacker
{

struct StopUpdateMessage
{
	enum Action : uint8_t
	{
		DELETE = 0,
		NEW = 1,
		UPDATE = 2
	};

	//MessageHeader header { MessageType::STOP_UPDATE, sizeof(decltype(*this)) };
	static constexpr MessageType type = MessageType::STOP_UPDATE;
	std::string id;
	std::string clientId { "" };
	Price price { Price::Min() };
	Quantity qty { 0 };
	Quantity triggerQty { 0 };
	Quantity leanQty { 0 };
	uint32_t stopId { 0 };
	uint32_t serverStopId { 0 };
	KTN::ORD::KOrderSide::Enum side { KTN::ORD::KOrderSide::Enum::UNKNOWN };
	Action action { Action::DELETE };
	uint8_t deleteOnFlip { 0 };
	uint8_t hasLean { 0 };
	uint8_t sendOnTrade { 0 };

	// Serialize to JSON
	void Serialize(rapidjson::Document& document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
		
		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", static_cast<int>(type), allocator);
		header.AddMember("length", 89, allocator);
		header.AddMember("version", 25, allocator);
		
		// Create string value for id
		rapidjson::Value idValue;
		idValue.SetString(id.c_str(), static_cast<rapidjson::SizeType>(id.length()), allocator);

		rapidjson::Value clientIdValue;
		clientIdValue.SetString(clientId.c_str(), static_cast<rapidjson::SizeType>(clientId.length()), allocator);
		
		// Add members to document
		document.AddMember("header", header, allocator);
		document.AddMember("id", idValue, allocator);
		document.AddMember("clientId", clientIdValue, allocator);
		document.AddMember("price", price.AsShifted(), allocator);
		document.AddMember("quantity", qty.value(), allocator);  // Note field name change from qty to quantity
		document.AddMember("triggerQuantity", triggerQty.value(), allocator);  // Note field name change
		document.AddMember("lean", leanQty.value(), allocator);  // Note field name change from leanQty to lean
		document.AddMember("stopId", stopId, allocator);
		document.AddMember("serverStopId", serverStopId, allocator);

		const std::optional<int8_t> aklSide = KtnSideToAklSide(static_cast<KTN::ORD::KOrderSide::Enum>(side));

		if (aklSide)
		{
			document.AddMember("side", static_cast<int>(*aklSide), allocator);
		}
		else
		{
			document.AddMember("side", static_cast<int>(std::numeric_limits<int8_t>::min()), allocator);
		}
		document.AddMember("action", static_cast<int>(action), allocator);
		document.AddMember("deleteOnFlip", static_cast<int>(deleteOnFlip), allocator);
		document.AddMember("hasLean", static_cast<int>(hasLean), allocator);
		document.AddMember("sendOnTrade", static_cast<int>(sendOnTrade), allocator);
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
	static StopUpdateMessage Deserialize(const rapidjson::Document& document)
	{
		StopUpdateMessage message;
		
		// Parse id
		if (document.HasMember("id") && document["id"].IsString())
		{
			message.id = document["id"].GetString();
		}

		if (document.HasMember("clientId") && document["clientId"].IsString())
		{
			message.clientId = document["clientId"].GetString();
		}
		
		// Parse price
		if (document.HasMember("price") && document["price"].IsInt64())
		{
			message.price = Price::FromShifted(document["price"].GetInt64());
		}
		
		// Parse quantity (maps to qty)
		if (document.HasMember("quantity") && document["quantity"].IsInt())
		{
			message.qty = Quantity(document["quantity"].GetInt());
		}
		
		// Parse triggerQuantity (maps to triggerQty)
		if (document.HasMember("triggerQuantity") && document["triggerQuantity"].IsInt())
		{
			message.triggerQty = Quantity(document["triggerQuantity"].GetInt());
		}
		
		// Parse lean (maps to leanQty)
		if (document.HasMember("lean") && document["lean"].IsInt())
		{
			message.leanQty = Quantity(document["lean"].GetInt());
		}
		
		// Parse stopId
		if (document.HasMember("stopId") && document["stopId"].IsUint())
		{
			message.stopId = document["stopId"].GetUint();
		}
		
		// Parse serverStopId
		if (document.HasMember("serverStopId") && document["serverStopId"].IsUint())
		{
			message.serverStopId = document["serverStopId"].GetUint();
		}
		
		// Parse side
		if (document.HasMember("side") && document["side"].IsInt())
		{
			message.side = AklSideToKtnSide(document["side"].GetInt());
		}
		
		// Parse action
		if (document.HasMember("action") && document["action"].IsUint())
		{
			message.action = static_cast<Action>(document["action"].GetUint());
		}
		
		// Parse deleteOnFlip
		if (document.HasMember("deleteOnFlip") && document["deleteOnFlip"].IsUint())
		{
			message.deleteOnFlip = static_cast<uint8_t>(document["deleteOnFlip"].GetUint());
		}
		
		// Parse hasLean
		if (document.HasMember("hasLean") && document["hasLean"].IsUint())
		{
			message.hasLean = static_cast<uint8_t>(document["hasLean"].GetUint());
		}
		
		// Parse sendOnTrade
		if (document.HasMember("sendOnTrade") && document["sendOnTrade"].IsUint())
		{
			message.sendOnTrade = static_cast<uint8_t>(document["sendOnTrade"].GetUint());
		}
		
		return message;
	}
	
	// Deserialize from string
	static StopUpdateMessage DeserializeFromString(const std::string& json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		
		if (document.HasParseError())
		{
			// Handle parsing error
			// You might want to throw an exception or return a default message
			return StopUpdateMessage();
		}
		
		return Deserialize(document);
	}
};

}