//============================================================================
// Name        	: notifiier_structs.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Jun 6, 2024 2:52:28 PM
//============================================================================

#ifndef SRC_NOTIFICATIONS_NOTIFIER_STRUCTS_HPP_
#define SRC_NOTIFICATIONS_NOTIFIER_STRUCTS_HPP_

#include <iostream>
#include <sstream>
#include <memory>
#include <pthread.h>
#include <sys/time.h>

namespace KTN {
namespace notify{

using namespace std;

struct MsgLevel
{
    enum Enum : int8_t
    {
    	INFO = 1,
		DEBUG,
		WARN,
		ERROR,
		CRTICIAL
    };

    static std::string toString(Enum level)
    {
        switch(level) {
        	case MsgLevel::INFO:
        		return "INFO";
        	case MsgLevel::DEBUG:
        		return "DEBUG";
        	case MsgLevel::WARN:
        		return "WARN";
        	case MsgLevel::ERROR:
        		return "ERROR";
        	case MsgLevel::CRTICIAL:
        		return "CRITICAL";
        	default:
        		return "UNKNOWN";
        }
    }
};

struct MsgType
{
    enum Enum : int8_t
    {
    	HB = 1,
		NOTIFY,
		ALERT,
		NOTICE,
		LOG
    };

    static std::string toString(Enum msgtype)
    {
        switch(msgtype) {
        	case MsgType::HB:
        		return "HB";
        	case MsgType::NOTIFY:
        		return "NOTIFY";
        	case MsgType::ALERT:
        		return "ALERT";
        	case MsgType::NOTICE:
        		return "NOTICE";
        	case MsgType::LOG:
        		return "LOG";
        	default:
        		return "UNKNOWN";
        }
    }
};


struct Notification
{
	string source;
	string exch;
	string org;
	string user;
	string msgtype;
	string text;
	uint64_t ssboe;
	string updatetime;
	MsgLevel::Enum level;
	MsgType::Enum imsg;
};


}
}



#endif /* SRC_NOTIFICATIONS_NOTIFIER_STRUCTS_HPP_ */
