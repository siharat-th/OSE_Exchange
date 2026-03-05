//============================================================================
// Name        	: JsonUrlReader.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Oct 9, 2021 1:04:15 PM
//============================================================================

#ifndef SRC_REST_JSONURLREADER_HPP_
#define SRC_REST_JSONURLREADER_HPP_

#include <iostream>
#include <string>
#include <curl/curl.h>

using namespace std;

/**
 * @brief The JsonUrlReader class is responsible for reading JSON data from a URL.
 */
class JsonUrlReader {
public:
	/**
  * @brief Default constructor for JsonUrlReader.
  */
	JsonUrlReader();

	/**
  * @brief Destructor for JsonUrlReader.
  */
	virtual ~JsonUrlReader();

	/**
  * @brief Callback function used by libcurl to write received data to a string buffer.
  * @param contents Pointer to the received data.
  * @param size Size of each element in the received data.
  * @param nmemb Number of elements in the received data.
  * @param userp Pointer to the string buffer where the received data is stored.
  * @return The total number of bytes written to the string buffer.
  */
	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
	{
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	/**
  * @brief Sends an HTTP GET request to the specified URL and returns the response as a string.
  * @param url The URL to send the GET request to.
  * @return The response received from the URL as a string.
  */
	inline static std::string Get(std::string url)
	{
		std::cout << "url: " << url << std::endl;
		CURL *curl;
		std::string readBuffer;
		curl = curl_easy_init();
		if(curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			CURLcode res = curl_easy_perform(curl);
			if (res != CURLE_OK)
			{
				std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
			}
			// std::cout << "readBuffer: " << readBuffer << std::endl;
			curl_easy_cleanup(curl);
		}
		else
		{
			std::cout << "curl is null" << std::endl;
		}

		return readBuffer;
	}

	inline static std::string PostJson(const std::string& url, const std::string& jsonData)
	{
		CURL *curl;
		std::string readBuffer;
		curl = curl_easy_init();

		if(curl)
		{
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_POST, 1L);

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.length());

			struct curl_slist *headers = nullptr;
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

			CURLcode res = curl_easy_perform(curl);

			if (res != CURLE_OK)
			{
				std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
			}

			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
		}

		return readBuffer;
	}
};

#endif /* SRC_REST_JSONURLREADER_HPP_ */
