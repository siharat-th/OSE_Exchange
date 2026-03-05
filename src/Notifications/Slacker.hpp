//============================================================================
// Name        	: Slacker.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 13, 2021 10:02:00 AM
//============================================================================

#ifndef SRC_SLACKER_HPP_
#define SRC_SLACKER_HPP_

#pragma once

#include <stdio.h>
#include <unordered_map>

#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <chrono>
using namespace std::chrono;

#include <tbb/concurrent_queue.h>
using namespace tbb;

#include <curl/curl.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;


#include <KT01/Core/Log.hpp>
#include <KT01/Core/NamedThread.hpp>
#include <KT01/Core/Settings.hpp>


using namespace std;


namespace KTN{
namespace notify{

/**
 * @brief Structure representing a Slack message.
 */
struct slackmsg
{
	string preview; /**< The preview of the message. */
	string topic; /**< The topic of the message. */
	string text; /**< The text of the message. */
	int color; /**< The color of the message. */
};

/**
 * @brief Structure representing a Slack bot.
 */
struct slackbot
{
	string token; /**< The token of the Slack bot. */
	string baseurl; /**< The base URL of the Slack bot. */
	string url; /**< The URL of the Slack bot. */
	string channel; /**< The channel of the Slack bot. */
	string color; /**< The color of the Slack bot. */
	string username; /**< The username of the Slack bot. */

	/**
  * @brief Prints the Slack bot information.
  * @return The string representation of the Slack bot.
  */
	string Print()
	{
		ostringstream oss;
		oss << channel << ":" << username << " color=" << color << " url=" << baseurl << " token=" << token;
		return oss.str();
	}
};

/**
 * @brief Class representing a Slack-enabled messaging class for alerts and notifications.
 */
class Slacker : public NamedThread
{
public:

	/**
  * @brief Default constructor for Slacker.
  */
	Slacker();

	/**
  * @brief Destructor for Slacker.
  */
	~Slacker();

	/**
  * @brief Initializes and runs the Slacker.
  */
	void InitAndRun();

	/**
  * @brief Sends a Slack message.
  * @param preview The preview of the message.
  * @param topic The topic of the message.
  * @param msg The text of the message.
  * @param color The color of the message.
  * @param force A flag indicating whether to force sending the message.
  */
	void Send(string preview, string topic, string msg,  LogLevel::Enum = LogLevel::INFO, bool force=false);

	/**
  * @brief Sends a Slack message.
  * @param msg The Slack message to send.
  */
	void Send(slackmsg& msg);

	/**
  * @brief Enables or disables sending messages.
  * @param sendmessages A flag indicating whether to enable sending messages.
  */
	void Enable(bool sendmessages);

private:
	/**
  * @brief Runs the Slacker.
  */
	void Run();

	/**
  * @brief Logs a message.
  * @param szMsg The message to log.
  * @param iColor The color of the log message.
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

	/**
  * @brief Performs a CURL API call with headers.
  * @param url The URL of the API.
  * @param str_result The result of the API call.
  * @param extra_http_header The extra HTTP headers to include.
  * @param post_data The data to post.
  * @param action The action to perform.
  */
	static void curl_api_with_header(string& url, string& str_result, vector<string>& extra_http_header, string& post_data, string& action);

	/**
  * @brief Callback function for CURL.
  * @param content The content received from CURL.
  * @param size The size of the content.
  * @param nmemb The number of members.
  * @param buffer The buffer to store the content.
  * @return The size of the content.
  */
	static size_t curl_cb(void* content, size_t size, size_t nmemb, string* buffer);

	/**
  * @brief Builds and sends a Slack message.
  * @param msg The Slack message to send.
  */
	void BuildAndSend(slackmsg msg);

	/**
  * @brief Formats a Slack message as JSON.
  * @param msg The Slack message to format.
  * @return The formatted Slack message as JSON.
  */
	string FormatJson(slackmsg msg);

	/**
  * @brief Formats a color value.
  * @param color The color value to format.
  * @return The formatted color value.
  */
	string FormatColor(int color);

private:
	slackbot _BOT; /**< The Slack bot. */
	std::atomic<bool> _ACTIVE; /**< A flag indicating whether the Slacker is active. */
	std::atomic<bool> _ENABLED; /**< A flag indicating whether sending messages is enabled. */
	tbb::concurrent_queue<slackmsg> _qSlack; /**< The concurrent queue of Slack messages. */

};

//end namespaces
}
}

#endif /*SLACKER_HPP */
