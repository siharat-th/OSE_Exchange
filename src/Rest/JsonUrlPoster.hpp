//============================================================================
// Name        	: JsonUrlReader.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Oct 9, 2021 1:04:15 PM
//============================================================================

#ifndef SRC_REST_JSONURLPOSTER_HPP_
#define SRC_REST_JSONURLPOSTER_HPP_

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <unordered_map>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

using namespace std;

class JsonUrlPoster {
public:
	/**
	* @brief Default constructor for JsonUrlPoster class.
	*/
	JsonUrlPoster();
	/**
	* @brief Destructor for JsonUrlPoster class.
	*/
	virtual ~JsonUrlPoster();

	/**
	* @brief Callback function for writing response data.
	* @param contents Pointer to the response data.
	* @param size Size of each element in the response data.
	* @param nmemb Number of elements in the response data.
	* @param userp Pointer to the user data.
	* @return The total size of the response data.
	*/
	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}


	/**
	* @brief Sends a POST request to the specified URL with the given postfields.
	*
	* @param url The URL to send the POST request to.
	* @param postfields The key-value pairs of the postfields to include in the request.
	* @return The response received from the server.
	*/
	 static std::string Post(const std::string& url, const std::unordered_map<std::string, std::string>& postfields) {
		// Create a JSON object using RapidJSON
		Document document;
		document.SetObject();
		Document::AllocatorType& allocator = document.GetAllocator();

		for (const auto& kv : postfields) {
			document.AddMember(StringRef(kv.first.c_str()), Value().SetString(kv.second.c_str(), allocator), allocator);
		}

		// Convert JSON object to string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		document.Accept(writer);
		string post_data = buffer.GetString();


		CURL *curl;
		std::string readBuffer;

		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

			struct curl_slist *headers = NULL;
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}

		return readBuffer;
	}





};

#endif /* SRC_REST_JSONURLREADER_HPP_ */
