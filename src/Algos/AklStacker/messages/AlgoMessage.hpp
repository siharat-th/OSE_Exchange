#pragma once

#include <inttypes.h>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

struct AlgoMessage
{
	const char * topic = "ALGOMESSAGE";
	std::string source { "" };
	int MessageType { 0 };
	std::string payload { "" };

	void Serialize(rapidjson::Document &document) const
	{
		document.SetArray();
		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

		// Create header object
		rapidjson::Value header(rapidjson::kObjectType);
		header.AddMember("type", MessageType, allocator);
		header.AddMember("length", 0, allocator);
		header.AddMember("version", 0, allocator);

		// Create string value for topic
		rapidjson::Value topicValue;
		topicValue.SetString(topic, allocator);

		rapidjson::Value sourceValue;
		sourceValue.SetString(source.c_str(), static_cast<rapidjson::SizeType>(source.length()), allocator);

		// Create string value for payload
		rapidjson::Value payloadValue;
		payloadValue.SetString(payload.c_str(), static_cast<rapidjson::SizeType>(payload.length()), allocator);

		rapidjson::Value messageObject(rapidjson::kObjectType);
		messageObject.AddMember("header", header, allocator);
		messageObject.AddMember("topic", topicValue, allocator);
		messageObject.AddMember("source", sourceValue, allocator);
		messageObject.AddMember("Payload", payloadValue, allocator);
		messageObject.AddMember("MessageType", MessageType, allocator);

		// Add members to document
		document.PushBack(messageObject, allocator);
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

	static AlgoMessage Deserialize(const rapidjson::Document &document)
	{
		AlgoMessage message;

		if (document.HasMember("topic") && document["topic"].IsString())
		{
			message.topic = document["topic"].GetString();
		}

		if (document.HasMember("source") && document["source"].IsString())
		{
			message.source = document["source"].GetString();
		}

		if (document.HasMember("Payload") && document["Payload"].IsString())
		{
			message.payload = document["Payload"].GetString();
		}

		return message;
	}

	static AlgoMessage DeserializeFromString(const std::string &json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());

		if (document.HasParseError())
		{
			throw std::runtime_error("JSON parse error");
		}

		return Deserialize(document);
	}
};

}