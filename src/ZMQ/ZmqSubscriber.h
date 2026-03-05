#pragma once

#ifndef ZMQSUBSCRIBER_H_
#define ZMQSUBSCRIBER_H_

#define THREAD_AUDIT_ID 0
#define THREAD_RMS_ID 1

#include <algorithm>
#include <memory.h>

#include "stdio.h"
#include <pthread.h>
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>

#include "zmq.h"

#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>
#include <KT01/Core/Settings.hpp>

using namespace std;

/**
 * @brief The ZmqSubListener class is an abstract base class for ZMQ subscriber listeners.
 */
class ZmqSubListener
{
public:
    /**
     * @brief onPubMsg is a pure virtual function that is called when a new message is received.
     * @param szTopic The topic of the received message.
     * @param szMsg The content of the received message.
     */
    virtual void onPubMsg(std::string szTopic, std::string szMsg) = 0;

    virtual ~ZmqSubListener(){}
};

/**
 * @brief The ZmqMessage class represents a ZMQ message.
 */
class ZmqMessage
{
    std::string szTopic; /**< The topic of the message. */
    std::string szMsg; /**< The content of the message. */
};


/**
 * @class ZmqSubscriber
 * @brief The ZmqSubscriber class represents a ZMQ subscriber.
 */
class ZmqSubscriber
{
public:
    /**
     * @brief ZmqSubscriber constructor.
     */
    ZmqSubscriber();

    /**
     * @brief ZmqSubscriber destructor.
     */
    virtual ~ZmqSubscriber();

    /**
     * @brief Initializes the ZmqSubscriber with the specified server port, topics, name, and callback function.
     * @param szServerPort The server port to connect to.
     * @param vTopics The vector of topics to subscribe to.
     * @param name The name of the subscriber.
     * @param fpSubCallback The callback function to be called when a new message is received.
     * @return True if initialization is successful, false otherwise.
     */
    bool Init(std::string szServerPort, std::vector<string> vTopics, std::string name, void*(*fpSubCallback)(string, string));

    /**
     * @brief Initializes the ZmqSubscriber with the specified server port, filename, key, name, and callback function.
     * @param szServerPort The server port to connect to.
     * @param filename The name of the file containing the topics to subscribe to.
     * @param key The key to access the topics in the file.
     * @param name The name of the subscriber.
     * @param fpSubCallback The callback function to be called when a new message is received.
     * @return A vector of topics that were successfully added.
     */
    vector<string> Init(std::string szServerPort, std::string filename, std::string key, std::string name, void*(*fpSubCallback)(string, string));

    /**
     * @brief Adds a topic to the list of subscribed topics.
     * @param szTopic The topic to add.
     * @return True if the topic is successfully added, false otherwise.
     */
    bool AddTopic(std::string szTopic);

    /**
     * @brief Loads topics from a file using the specified filename and key.
     * @param filename The name of the file containing the topics.
     * @param key The key to access the topics in the file.
     */
    void LoadTopics(const std::string& filename, const std::string& key);

private:
    /**
     * @brief The entry point for the subscriber thread.
     * @param arg The argument passed to the thread.
     * @return The result of the thread execution.
     */
    static void *EntryPoint(void* arg);

    /**
     * @brief Logs a message with an optional color.
     * @param szMsg The message to log.
     * @param iColor The color of the log message.
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

private:
    std::string _NAME; /**< The name of the subscriber. */
    ZmqSubListener* listener_; /**< The listener for the subscriber. */
    std::vector<string> topics_; /**< The list of subscribed topics. */
    std::string m_serverport; /**< The server port to connect to. */
    pthread_t m_tidSub; /**< The subscriber thread ID. */
    void *(*m_fpCallback)(std::string, std::string); /**< The callback function for message reception. */
};

#endif /*ZMQSUBSCRIBER_H_*/
