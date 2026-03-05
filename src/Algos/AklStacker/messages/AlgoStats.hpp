#pragma once

#include <inttypes.h>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

namespace akl::stacker
{

struct AlgoStats
{
	const char *topic = "ALGOMESSAGE";
	const char *header = "stats";
	std::string source { "" };
	std::string instance {"" };
	std::string text { "" };
	std::string messageType { "" };

	void Serialize(rapidjson::Document &document) const
	{
		document.SetObject();
		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

		// Create string value for topic
		rapidjson::Value headerValue;
		headerValue.SetString(header, allocator);

		rapidjson::Value sourceValue;
		sourceValue.SetString(source.c_str(), static_cast<rapidjson::SizeType>(source.length()), allocator);

		rapidjson::Value instanceValue;
		instanceValue.SetString(instance.c_str(), static_cast<rapidjson::SizeType>(instance.length()), allocator);

		rapidjson::Value uniqueIdValue;
		uniqueIdValue.SetString(instance.c_str(), static_cast<rapidjson::SizeType>(instance.length()), allocator);

		// Create string value for payload
		rapidjson::Value textValue;
		textValue.SetString(text.c_str(), static_cast<rapidjson::SizeType>(text.length()), allocator);

		rapidjson::Value messageTypeValue;
		messageTypeValue.SetString(messageType.c_str(), static_cast<rapidjson::SizeType>(messageType.length()), allocator);

		document.AddMember("header", headerValue, allocator);
		document.AddMember("source", sourceValue, allocator);
		document.AddMember("instance", instanceValue, allocator);
		document.AddMember("uniqueid", uniqueIdValue, allocator);
		document.AddMember("msgtype", messageTypeValue, allocator);
		document.AddMember("text", textValue, allocator);
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

	// static AlgoMessage Deserialize(const rapidjson::Document &document)
	// {
	// 	AlgoMessage message;

	// 	if (document.HasMember("topic") && document["topic"].IsString())
	// 	{
	// 		message.topic = document["topic"].GetString();
	// 	}

	// 	if (document.HasMember("source") && document["source"].IsString())
	// 	{
	// 		message.source = document["source"].GetString();
	// 	}

	// 	if (document.HasMember("Payload") && document["Payload"].IsString())
	// 	{
	// 		message.payload = document["Payload"].GetString();
	// 	}

	// 	return message;
	// }

	// static AlgoMessage DeserializeFromString(const std::string &json)
	// {
	// 	rapidjson::Document document;
	// 	document.Parse(json.c_str());

	// 	if (document.HasParseError())
	// 	{
	// 		throw std::runtime_error("JSON parse error");
	// 	}

	// 	return Deserialize(document);
	// }
};

}