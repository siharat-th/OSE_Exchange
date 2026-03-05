//============================================================================
// Name        	: NotifierRest.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: May 25, 2021 2:23:35 PM
//============================================================================

#ifndef SRC_NOTIFY_REST_HPP_
#define SRC_NOTIFY_REST_HPP_

#pragma once

#include <iostream>
#include <sstream>
#include <memory>
#include <pthread.h>
#include <sys/time.h>

#include <tbb/concurrent_queue.h>
using namespace tbb;

#include <KT01/Core/Log.hpp>

#include <Rest/RestApi.hpp>
#include <Rest/JsonUrlPoster.hpp>

#include <Notifications/notifier_structs.hpp>

#include <KT01/Core/Singleton.hpp>
#include <KT01/Core/NamedThread.hpp>
#include <KT01/Core/StringUtils.hpp>

using namespace std;

namespace KTN {
namespace notify{

/**
 * @class NotifierRest
 * @brief The NotifierRest class is responsible for sending notifications via REST.
 */
class NotifierRest : public NamedThread, public Singleton<NotifierRest>
{
public:
	/**
  * @brief Constructs a NotifierRest object.
  */
	NotifierRest();

	/**
  * @brief Destroys the NotifierRest object.
  */
	virtual ~NotifierRest();

	/**
  * @brief Sends a notification with the specified parameters.
  * @param source The source of the notification.
  * @param exch The exchange of the notification.
  * @param org The organization of the notification.
  * @param user The user of the notification.
  * @param text The text of the notification.
  * @param lvl The level of the notification.
  * @param msgtype The type of the notification.
  */
	void Notify(std::string source, string exch, string org, string user, std::string text, MsgLevel::Enum lvl, MsgType::Enum msgtype );

	/**
  * @brief Sends a notification.
  * @param msg The notification to send.
  */
	void Notify(Notification& msg);

	/**
  * @brief Sends a heartbeat notification with the specified parameters.
  * @param source The source of the heartbeat.
  * @param exch The exchange of the heartbeat.
  * @param org The organization of the heartbeat.
  */
	void Heartbeat(std::string source, std::string exch, string org);

	/**
  * @brief Starts the NotifierRest thread.
  */
	void Start();

	void Stop();

	static void NotifyInfo(const std::string& exch, const std::string& source, const std::string& org, const std::string& text) {
		Notification n = {};
		n.exch = exch;
		n.source = source;
		n.org = org;
		n.level = MsgLevel::INFO;
		n.imsg = MsgType::NOTICE;
		n.text = text;
		instance().Notify(n);
	}

	static void NotifyError(const std::string& exch, const std::string& source, const std::string& org, const std::string& text) {
		Notification n = {};
		n.exch = exch;
		n.source = source;
		n.org = org;
		n.level = MsgLevel::ERROR;
		n.imsg = MsgType::ALERT;
		n.text = text;
		instance().Notify(n);
	}

private:
	/**
  * @brief The main execution loop of the NotifierRest thread.
  */
	void Run() override;

	/**
  * @brief Logs a message with the specified color.
  * @param szMsg The message to log.
  * @param iColor The color of the log message.
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

	/**
  * @brief Persists a notification message.
  * @param msg The notification message to persist.
  */
	void PersistMsg(Notification& msg);

	/**
  * @brief Gets the current epoch time.
  * @return The current epoch time.
  */
	inline time_t get_current_epoch( ) {

     struct timeval tv;
     gettimeofday(&tv, NULL);

     return tv.tv_sec ;
	}

	/**
  * @brief Gets the current epoch time in milliseconds.
  * @return The current epoch time in milliseconds.
  */
	inline unsigned long get_current_ms_epoch( ) {

     struct timeval tv;
     gettimeofday(&tv, NULL);

     return tv.tv_sec * 1000 + tv.tv_usec / 1000 ;

	}

	/**
  * @brief Gets the local time in the format "MM-DD-YYYY HH:MM:SS.000".
  * @return The local time.
  */
	inline std::string getLocalTime()
	{
		char buffer[50];
		time_t tt = time(0);
		tm *tm = localtime(&tt);
		strftime(buffer, sizeof(buffer), "%m-%d-%Y %H:%M:%S.000", tm);
		return buffer;
	}

	inline std::string getUTCTime()
	{
		char buffer[50];
		time_t tt = time(0);
		tm tm_buf;
		tm *tm = gmtime_r(&tt, &tm_buf);
		strftime(buffer, sizeof(buffer), "%m-%d-%Y %H:%M:%S.000", tm);
		return buffer;
	}

private:
	bool _verbose;
	std::atomic<bool> _RPT_ACTIVE;
	string _NAME;

	tbb::concurrent_queue<Notification> _queue;

};

}
}

#endif /* SRC_NOTIFY_REST_HPP_ */
