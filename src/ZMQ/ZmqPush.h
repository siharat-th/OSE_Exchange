/*
 * ZmqPush.h
 *
 *  Created on: Jan 3, 2017
 *      Author: sgaer
 */

#ifndef ZMQPUSH_H_
#define ZMQPUSH_H_

#pragma once

#include <algorithm>
#include <memory.h>
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

using namespace std;

#define REQUEST_TIMEOUT     2500    //  msecs, (> 1000!)
#define REQUEST_RETRIES     3       //  Before we abandon
#define PUBLISH_SLEEP_US    1

/**
 * @class ZmqPush
 * @brief Represents a ZMQ push socket for sending messages to a ZMQ pull socket.
 */
class ZmqPush {
public:
    /**
     * @brief Default constructor for ZmqPush.
     */
    ZmqPush();

    /**
     * @brief Destructor for ZmqPush.
     */
    virtual ~ZmqPush();

    /**
     * @brief Initializes the ZmqPush object with the specified server address and port.
     * @param szServerAddressPort The server address and port in the format "address:port".
     * @return True if initialization is successful, false otherwise.
     */
    bool Init(char *szServerAddressPort);

    /**
     * @brief Pushes a message to the ZMQ pull socket.
     * @param msg The message to push.
     * @param len The length of the message.
     */
    void Push(char * msg, int len);

private:
    /**
     * @brief Logs a message with an optional color.
     * @param szMsg The message to log.
     * @param iColor The color of the log message (default is LOG_WHITE).
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

private:
    void *ctx;
    void *s;
    char * srv_;
};

#endif /* ZMQPUSH_H_ */
