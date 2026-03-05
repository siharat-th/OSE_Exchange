//============================================================================
// Name        	: NotifierRest.cpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Sept 7, 2022 2:23:35 PM
//
// Generic Rest Notifier; simple, reliable, and clean to hit a REST api and then do what you will
//
//============================================================================

#include <Notifications/NotifierRest.hpp>

using namespace KTN::notify;

//this is a singleton, so anyone anywhere can use it without declaring
//cme example: CmeSecDef def = CmeSecMaster::instance().getSecDef(symb);


NotifierRest::NotifierRest()
{
	_verbose = false;
	_NAME = "NotifierRest";
	CreateAndRun(_NAME, 1);
}

NotifierRest::~NotifierRest() {
	// TODO Auto-generated destructor stub
}

void NotifierRest::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void NotifierRest::Notify(std::string source, string exch, string org, string user,
		std::string text, MsgLevel::Enum lvl, MsgType::Enum msgtype )
{
	Notification msg = {};
	msg.source = source;
	msg.exch = exch;
	msg.org = org;
	msg.user = user;
	msg.msgtype = msgtype;
	msg.text = text;
	msg.level = lvl;

	msg.ssboe =  get_current_ms_epoch();
	msg.updatetime = getUTCTime();
	msg.imsg = MsgType::NOTIFY;

	Notify(msg);
}



void NotifierRest::Heartbeat(std::string source, std::string exch, std::string org)
{
	Notification msg = {};
	msg.source = source;
	msg.exch = exch;
	msg.org = org;
	msg.msgtype = "HB";

	msg.ssboe =  get_current_ms_epoch();
	msg.updatetime = getUTCTime();
	msg.imsg = MsgType::HB;

	Notify(msg);
}

void NotifierRest::Stop()
{
	_RPT_ACTIVE.store(false);
}


void NotifierRest::Notify(Notification& msg)
{
	//LogMe("GOT NOTIFICATION: " + msg.text);
	_queue.push(msg);
}

void NotifierRest::Run()
{
	_RPT_ACTIVE.store(true);
	//sleep for 500 millis
	int isleepus = 5000;//500 * 1000;

	while(_RPT_ACTIVE.load(std::memory_order_relaxed))
	{
		if (!_queue.empty()) {
			Notification item = {};
			while (_queue.try_pop(item)) {
				PersistMsg(item);
			}
		}

		usleep(isleepus);
	}

}


//void NotifierRest::PersistMsg(Notification& msg) {
//    std::unordered_map<string, string> params;
//
//    string msgtext = StringUtils::RemoveDoubleQuotes(msg.text);
//    msgtext = StringUtils::RemoveSingleQuotes(msg.text);
//
//    params["source"] = msg.source;
//    params["exch"] = msg.exch;
//    params["ssboe"] = to_string(get_current_ms_epoch());
//    params["updatetime"] = getLocalTime();
//    params["org"] = msg.org;
//    params["text"] = msgtext;
//    params["level"] = MsgLevel::toString(msg.level);
//    params["msgtype"] = MsgType::toString(msg.imsg);
//
//    // Extract ssboe and msgtype
//    string ssboe = params["ssboe"];
//    string msgtype = params["msgtype"];
//
//    // Create json_notify JSON string
//    Document json_notify_doc;
//    json_notify_doc.SetObject();
//    Document::AllocatorType& allocator = json_notify_doc.GetAllocator();
//
//    for (const auto& kv : params) {
//        if (kv.first != "ssboe" && kv.first != "msgtype") {
//            json_notify_doc.AddMember(StringRef(kv.first.c_str()), Value().SetString(kv.second.c_str(), allocator), allocator);
//        }
//    }
//
//    StringBuffer buffer;
//    Writer<StringBuffer> writer(buffer);
//    json_notify_doc.Accept(writer);
//    string json_notify = buffer.GetString();
//
//    // Prepare the final payload
//    std::unordered_map<string, string> final_params;
//    final_params["ssboe"] = ssboe;
//    final_params["msgtype"] = msgtype;
//    final_params["msglevel"] = params["level"];
//    final_params["org"] = msg.org;
//    final_params["json_params"] = json_notify;
//
//    string res = JsonUrlPoster::Post(API_NOTIFY_PERSIST, final_params);
//
//    //LogMe("NOTIFY RES=" + res);
//}

void NotifierRest::PersistMsg(Notification& msg) {
    // Prepare the params map with message details
    std::unordered_map<string, string> params;

    string msgtext = StringUtils::RemoveDoubleQuotes(msg.text);
    msgtext = StringUtils::RemoveSingleQuotes(msg.text);

    params["source"] = msg.source;
    params["exch"] = msg.exch;
    params["ssboe"] = to_string(get_current_ms_epoch());
    params["updatetime"] = getUTCTime();
    params["org"] = msg.org;
    params["text"] = msgtext;
    params["level"] = MsgLevel::toString(msg.level);
    params["msgtype"] = MsgType::toString(msg.imsg);

    // Extract ssboe and msgtype
    string ssboe = params["ssboe"];
    string msgtype = params["msgtype"];

    // Create the root JSON object
    rapidjson::Document root_doc;
    root_doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = root_doc.GetAllocator();

    // Add the required top-level fields
    root_doc.AddMember("ssboe", Value().SetString(ssboe.c_str(), allocator), allocator);
    root_doc.AddMember("msgtype", Value().SetString(msgtype.c_str(), allocator), allocator);
    root_doc.AddMember("msglevel", Value().SetString(params["level"].c_str(), allocator), allocator);
    root_doc.AddMember("org", Value().SetString(msg.org.c_str(), allocator), allocator);

    // Create the json_params object
    rapidjson::Document json_notify_doc;
    json_notify_doc.SetObject();

    for (const auto& kv : params) {
        if (kv.first != "ssboe" && kv.first != "msgtype") {
            json_notify_doc.AddMember(StringRef(kv.first.c_str()), Value().SetString(kv.second.c_str(), allocator), allocator);
        }
    }

    // Add the json_params object to the root JSON
    root_doc.AddMember("json_params", json_notify_doc, allocator);

    // Serialize the root JSON object
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    root_doc.Accept(writer);
    string serialized_payload = buffer.GetString();

    // Create the final map with just json_params
    std::unordered_map<string, string> final_params;
    final_params["json_params"] = serialized_payload;

    // POST the payload
    string res = JsonUrlPoster::Post(RestApi::NOTIFY_PERSIST, final_params);
}




