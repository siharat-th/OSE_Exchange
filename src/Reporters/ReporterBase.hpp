//============================================================================
// Name        	: ReporterBase.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 21, 2023 1:52:41 PM
//============================================================================

#ifndef REPORTER_BASE_HPP
#define REPORTER_BASE_HPP

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
using namespace tbb;

#include <chrono>
using namespace std::chrono;

#include <Json/FormatterBase.hpp>
#include <ExchangeHandler/session/status/SessionStatusFormatter.hpp>

using namespace std;

namespace KTN {
namespace REPORTER{

/**
 * @brief The base class for all reporters.
 */
class ReporterBase {
public:
	/**
  * @brief Constructs a ReporterBase object with the given name and sleep time.
  * @param name The name of the reporter.
  * @param sleepms The sleep time in milliseconds.
  */
	ReporterBase(string name, int sleepms);

	/**
  * @brief Destructor for the ReporterBase object.
  */
    virtual ~ReporterBase();

    /**
     * @brief Stops the reporter.
     */
    void Stop();

    /**
     * @brief Starts the reporter.
     */
    void Start();

protected:
    /**
     * @brief Performs the work of the reporter.
     */
    virtual void DoWork()=0;

    /**
     * @brief Sends the given JSON string.
     * @param json The JSON string to send.
     */
    void SendJson(string json);

    /**
     * @brief Logs the given message.
     * @param szMsg The message to log.
     * @param iColor The color of the log message.
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

    /**
     * @brief Gets the current time as a string.
     * @return The current time as a string.
     */
    inline string getTimeString()
    {
					time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time (&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer,sizeof(buffer),"%H:%M:%S",timeinfo);
		std::string strTime(buffer);

		return strTime;
    }

    /**
     * @brief Gets the current system time in microseconds since the epoch.
     * @return The current system time in microseconds since the epoch.
     */
    inline uint64_t getSsboeUs()
    {
					auto now = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
		auto epoch = now.time_since_epoch();
		auto ssboe = std::chrono::duration_cast<std::chrono::microseconds>(epoch).count();

		return ssboe;
    }

private:
    void start(std::string ip, int port);
    void messaging_thread();
    void reporting_thread();

protected:
    std::atomic<bool> _ACTIVE;

private:
    std::string _endpoint;
    std::thread _rptthread;
    std::thread _msgthread;
    string _name;
    int _sleepus;

    tbb::concurrent_queue<std::string> _qJson;
};

} }
#endif