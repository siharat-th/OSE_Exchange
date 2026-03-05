//============================================================================
// Name        	: Reporter.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 21, 2023 1:52:41 PM
//============================================================================

#ifndef STATUSREPORTER_HPP
#define STATUSREPORTER_HPP

#include <string>
#include <utility>
#include <thread>
#include <functional>
#include <unistd.h>
#include <memory>
#include <unordered_map>
#include <typeindex>

#include <ZMQ/ZmqSubscriber.h>
#include <KT01/Core/Settings.hpp>
#include <KT01/Core/Log.hpp>

#include <tbb/concurrent_queue.h>

#include <chrono>
using namespace std::chrono;

#include <Json/FormatterBase.hpp>
#include <ExchangeHandler/session/status/SessionStatusFormatter.hpp>

#include <tbb/concurrent_queue.h>
using namespace tbb;

using namespace std;

namespace KTN {
namespace REPORTER{

/**
 * @class StatusReporter
 * @brief The StatusReporter class is responsible for reporting session status.
 */
class StatusReporter {
public:
    /**
     * @brief Default constructor for StatusReporter.
     */
    StatusReporter();

    /**
     * @brief Destructor for StatusReporter.
     */
    ~StatusReporter();

    /**
     * @brief Stops the StatusReporter.
     */
    void Stop();

    /**
     * @brief Reports the session status.
     * @param data The session status to be reported.
     */
    void Report(SessionStatus& data);

    /**
     * @brief Checks if the StatusReporter is active.
     * @return True if the StatusReporter is active, false otherwise.
     */
    inline bool is_active()
    {
        return _RPT_ACTIVE;
    }

    /**
     * @brief Gets the size of the status queue.
     * @return The size of the status queue.
     */
    inline int status_queue_size()
    {
        return _qStatus.unsafe_size();
    }

    tbb::concurrent_queue<SessionStatus> _qStatus;

private:
    /**
     * @brief Logs a message.
     * @param szMsg The message to be logged.
     * @param iColor The color of the log message.
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

    /**
     * @brief Starts the StatusReporter.
     * @param ip The IP address to start the StatusReporter.
     * @param port The port number to start the StatusReporter.
     */
    void start(std::string ip, int port);

    /**
     * @brief The reporting thread function.
     */
    void reporting_thread();

    bool _RPT_ACTIVE; /**< Flag indicating if the StatusReporter is active. */
    std::string _endpoint; /**< The endpoint of the StatusReporter. */
    std::thread _thread; /**< The thread for the StatusReporter. */

    //session status keeper
    struct hash_pair {
        /**
         * @brief Hashes a pair.
         * @param p The pair to be hashed.
         * @return The hash value of the pair.
         */
        template <class T1, class T2>
        size_t operator()(const pair<T1, T2>& p) const
        {
            auto hash1 = std::hash<T1>{}(p.first);
            auto hash2 = std::hash<T2>{}(p.second);
            return hash1 ^ hash2;
        }
    };
};

} }
#endif
