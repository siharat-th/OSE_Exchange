/*
 * SessionStatusFormatter.hpp
 *
 *  Created on: Nov 26, 2019
 *      Author: sgaer
 */

#ifndef SESSIONSTATUSFORMATTER_HPP_
#define SESSIONSTATUSFORMATTER_HPP_
#pragma once
#include <cstring>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <ExchangeHandler/session/status/SessionStatus.hpp>


using namespace rapidjson;

using namespace std;

/**
 * @brief The SessionStatusFormatter class provides a static method to format a SessionStatus object into a JSON string.
 */
class SessionStatusFormatter
{

public:

    /**
     * @brief Get the JSON representation of a SessionStatus object.
     * @param sess The SessionStatus object to be formatted.
     * @return The JSON string representation of the SessionStatus object.
     */
    inline static std::string Get(SessionStatus sess)
    {
        StringBuffer s;
        s.Clear();

        Writer<StringBuffer> writer(s);

        writer.StartObject();

        writer.Key("header");
        writer.String("sess");

        std::string hdr("sessionstatusrecord");
        writer.String(hdr.c_str());

        writer.StartArray();

        writer.StartObject();

        writer.Key("source");
        writer.String(sess.source.c_str());

        writer.Key("subid");
        writer.String(sess.subid.c_str());

        writer.Key("ok");
        writer.String(sess.ok.c_str());

        writer.Key("sessionstatus");
        writer.String(sess.sessionstatus.c_str());

        writer.Key("msgseqnum");
        writer.Int(sess.msgseqnum);

        writer.Key("ipaddress");
        writer.String(sess.ipaddress.c_str());

        writer.Key("port");
        writer.Int(sess.port);

        writer.Key("exch");
        writer.String(sess.exch.c_str());

        writer.Key("mktsegid");
        writer.Int(sess.mktsegid);

        writer.Key("protocol");
        writer.Int(sess.protocol);

        writer.Key("state");
        writer.String(sess.state.c_str());

        writer.Key("desc");
        writer.String(sess.desc.c_str());
        writer.EndObject();

        writer.EndArray();
        writer.EndObject();

        std::string res = s.GetString();
        return res;
    }
};

#endif /* SESSIONSTATUSFORMATTER_HPP_ */
