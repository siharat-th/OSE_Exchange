#pragma once

#ifndef ZMQSUBSCRIBERV5_HPP_
#define ZMQSUBSCRIBERV5_HPP_

#include <algorithm>
#include <memory.h>

#include "stdio.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>

#include "zmq.h"

#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>
#include <KT01/Core/Settings.hpp>

#include <KT01/Core/NamedThread.hpp>

using namespace std;

/**
 * @brief The interface for a ZMQ subscriber.
 */
class IZmqSubscriber
{
public:
	/**
  * @brief Called when a message is received.
  * @param topic The topic of the message.
  * @param payload The payload of the message.
  */
	virtual void onPubMsg(string topic, string payload) = 0;
};

/**
 * @brief A ZMQ subscriber class.
 */
class ZmqSubscriberV5 : NamedThread
{
public:
	/**
  * @brief Constructs a ZmqSubscriberV5 object.
  * @param sub The IZmqSubscriber object to handle received messages.
  * @param server The ZMQ server address.
  * @param port The ZMQ server port.
  * @param name The name of the subscriber.
  * @param topics The initial topics to subscribe to.
  */
	explicit ZmqSubscriberV5(IZmqSubscriber* sub, const string& server, int port, const string& name, vector<string> topics);

	/**
  * @brief Constructs a ZmqSubscriberV5 object.
  * @param sub The IZmqSubscriber object to handle received messages.
  * @param server The ZMQ server address.
  * @param port The ZMQ server port.
  * @param name The name of the subscriber.
  * @param filename The name of the file containing the topics to subscribe to.
  * @param key The key to access the topics in the file.
  */
	explicit ZmqSubscriberV5(IZmqSubscriber* sub, const string& server, int port, const string& name, const string& filename, const string& key);

	/**
  * @brief Destroys the ZmqSubscriberV5 object.
  */
	virtual ~ZmqSubscriberV5();

	/**
  * @brief Adds a topic to subscribe to.
  * @param szTopic The topic to add.
  * @return True if the topic was added successfully, false otherwise.
  */
	bool AddTopic(string szTopic);

	/**
  * @brief Loads topics from a file.
  * @param filename The name of the file containing the topics.
  * @param key The key to access the topics in the file.
  */
	void LoadTopics(const string& filename, const string& key);

	/**
  * @brief Gets the list of subscribed topics.
  * @return The list of subscribed topics.
  */
	inline const vector<string> Topics() const
	{
		return topics_;
	}

private:
	/**
  * @brief Logs a message.
  * @param szMsg The message to log.
  * @param iColor The color of the log message.
  */
	void LogMe(string szMsg, LogLevel::Enum level = LogLevel::INFO);

	/**
  * @brief Runs the subscriber thread.
  */
	void Run() override;

	/**
  * @brief Checks the length of a name and truncates it if necessary.
  * @param str The name to check.
  * @return The truncated name if its length is greater than 15 characters, otherwise the original name.
  */
	inline std::string CheckName(const std::string& str) {
     return str.length() > 15 ? str.substr(0, 15) : str;
	}

private:
	std::atomic<bool> _ACTIVE;

	IZmqSubscriber* _callback;

	std::string _NAME;
	std::vector<string> topics_;

	std::string _SERVER;
	int _PORT;

};

#endif /*ZMQSUBSCRIBERV5_HPP_*/
