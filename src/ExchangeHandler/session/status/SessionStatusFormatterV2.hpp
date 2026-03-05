/*
 * SessionStatusFormatter.hpp
 *
 *  Created on: Nov 26, 2019
 *      Author: sgaer
 */

#ifndef SESSIONSTATUSFORMATTERV2_HPP_
#define SESSIONSTATUSFORMATTERV2_HPP_
#pragma once

#include <Json/FormatterBase.hpp>
#include <ExchangeHandler/session/status/SessionStatus.hpp>

using namespace rapidjson;

using namespace std;


/**
 * @brief Formatter class for converting SessionStatus objects to JSON format.
 */
class SessionStatusJsonFormatter : public FormatterBase
{

public:

	/**
  * @brief Converts a SessionStatus object to JSON format.
  * @param data A pointer to the SessionStatus object.
  * @return The JSON representation of the SessionStatus object.
  */
	std::string GetJson(const void* data) const override {
		const SessionStatus* sess = static_cast<const SessionStatus*>(data);

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
			writer.String(sess->source.c_str());

			writer.Key("subid");
			writer.String(sess->subid.c_str());

			writer.Key("ok");
			writer.String(sess->ok.c_str());

			writer.Key("sessionstatus");
			writer.String(sess->sessionstatus.c_str());

			writer.Key("msgseqnum");
			writer.Int(sess->msgseqnum);

			writer.Key("ipaddress");
			writer.String(sess->ipaddress.c_str());

			writer.Key("port");
			writer.Int(sess->port);

			writer.Key("exch");
			writer.String(sess->exch.c_str());

			writer.Key("mktsegid");
			writer.Int(sess->mktsegid);

			writer.Key("protocol");
			writer.Int(sess->protocol);

			writer.Key("state");
			writer.String(sess->state.c_str());

			writer.Key("desc");
			writer.String(sess->desc.c_str());
		writer.EndObject();

		writer.EndArray();
		writer.EndObject();

		std::string res = s.GetString();
		return res;
	}
};


#endif /* SESSIONSTATUSFORMATTER_HPP_ */
