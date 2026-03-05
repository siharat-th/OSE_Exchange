//============================================================================
// Name        	: RiskBase.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 5, 2023 9:06:34 AM
//============================================================================

#ifndef SRC_ALGOPARAMS_RISKBASE_HPP_
#define SRC_ALGOPARAMS_RISKBASE_HPP_

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
using namespace rapidjson;

/**
 * @brief Struct representing the risk parameters for an algorithm.
 * @details This struct is used to represent the risk parameters for an algorithm.  The model we use here is 
 * called a "Triple Barrier" risk model, where we have a profit taking value, a stop loss value, and a time to live value.
 */
struct RiskBase
{
	bool ValidJson; /**< Flag indicating if the JSON is valid. */
	bool UsePL; /**< Flag indicating if profit taking and stop loss are used. */

	int ProfitTaking; /**< The profit taking value. */
	int StopLoss; /**< The stop loss value. */
	int MinutesToLive; /**< The time to live in minutes. */

	double elapsedMinutes; /**< The elapsed minutes. */

	uint64_t entrytime_us; /**< The entry time in microseconds. */
	uint64_t ttl_us; /**< The time to live in microseconds. */
	uint64_t expiretime_us; /**< The expiration time in microseconds. */
	int64_t timeleft_us; /**< The time left in microseconds. */
	int64_t seconds_left; /**< The time left in seconds. */

	/**
  * @brief Parses the JSON string and returns a RiskBase object.
  * @param json_str The JSON string to parse.
  * @return The parsed RiskBase object.
  */
	static RiskBase parse(const std::string &json_str)
	{
		RiskBase cp = {};
		cp.ValidJson = false;

		Document d;

		if (d.Parse(json_str.c_str()).HasParseError()) {
			int e = (unsigned)d.GetErrorOffset();
			ostringstream oss;
			cout << "JSON OFFSET ERROR AT " << e << endl;
			return cp;
		}

		if (!d.IsArray())
		{
			cout << "ALGO JSON OBJECT MUST BE AN ARRAY!!!!" << endl;
			return cp;
		}

		cp.ValidJson = true;

		for (Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
		{
			if ((*itr).HasMember("ProfitTaking"))
				cp.ProfitTaking = (*itr)["ProfitTaking"].GetInt();

			if ((*itr).HasMember("StopLoss"))
				cp.StopLoss = (*itr)["StopLoss"].GetInt();

			if ((*itr).HasMember("MinutesToLive"))
				cp.MinutesToLive = (*itr)["MinutesToLive"].GetInt();

			break;
		}

		if (cp.ProfitTaking > 0 || cp.StopLoss > 0)
			cp.UsePL = true;

		return cp;

	};

};

#endif /* SRC_ALGOPARAMS_RISKBASE_HPP_ */
