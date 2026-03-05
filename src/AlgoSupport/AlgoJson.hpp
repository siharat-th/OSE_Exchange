/*
 * AlgoJson.hpp
 *
 *  Created on: Oct 14, 2019
 *      Author: sgaer
 */

#ifndef ALGOJSON_HPP_
#define ALGOJSON_HPP_

#include <cstring>
#include <vector>
#include "algo_structs.hpp"


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

using namespace std;

/**
 * @brief The AlgoJson class provides a static method to format algorithm statistics into JSON.
 */
class AlgoJson {
public:
	/**
  * @brief Formats algorithm statistics into JSON.
  * @param msgtype The message type.
  * @param source The source.
  * @param enabled The enabled flag.
  * @param templatename The template name.
  * @param uniqueid The unique ID.
  * @param text The text.
  * @param name The instance name.
  * @param symb The symbol.
  * @param exch The exchange.
  * @param pos The position.
  * @param totalexec The total execution.
  * @return The formatted JSON string.
  */
	static std::string FormatAlgoStats(string msgtype, std::string source, int enabled, std::string templatename,
					std::string uniqueid, std::string text, std::string name, std::string symb, std::string exch,
					double pos, double totalexec)
	{
		StringBuffer s = StringBuffer();
		s.Clear();
		s.Flush();

		Writer<StringBuffer> writer(s);

		writer.StartObject();

			writer.Key("header");
			writer.String("stats");

			writer.Key("source");
			writer.String(source.c_str());

			writer.Key("instance");
			writer.String(name.c_str());

			writer.Key("enabled");
			//int enabled = (isEnabled) ? 1 : 0;
			writer.Int(enabled);

			writer.Key("state");
			std::string state = (enabled > 0) ? "ENABLED" : "DISABLED";
			if (enabled < 0) state = "KILLED";
			writer.String(state.c_str());

			writer.Key("templateid");
			writer.String(templatename.c_str());

			writer.Key("uniqueid");
			writer.String(uniqueid.c_str());

			writer.Key("text");
			writer.String(text.c_str());

			writer.Key("symbol");
			writer.String(symb.c_str());

			writer.Key("exchange");
			writer.String(exch.c_str());

			writer.Key("position");
			writer.Double(pos);

			writer.Key("totalexec");
			writer.Double(totalexec);

			writer.Key("msgtype");
			writer.String(msgtype.c_str());

		writer.EndObject();

		//std::string szPrefix = "JSAR";
		std::string szBody = s.GetString();
		std::string res = szBody;

		//cout << "JSON FORMAT: " << res << endl;

		return res;

	}

};

#endif /* ALGOJSON_HPP_ */
