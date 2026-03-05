//============================================================================
// Name        	: AutoHedgeOB2Params.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 5:44:11 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_TIMEDAVGPARAMS_HPP_
#define SRC_ALGOPARAMS_TIMEDAVGPARAMS_HPP_


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
using namespace rapidjson;

#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>

#include <AlgoSupport/algo_structs.hpp>



/**
 * @brief Struct representing the parameters for the Timed Average algorithm.
 */
struct TimedAvgParams
{
	/**
  * @brief Parses the JSON string and returns a TimedAvgParams object.
  * @param json_str The JSON string to parse.
  * @return A TimedAvgParams object.
  */
	static TimedAvgParams parse(const std::string& json_str);

	bool ValidJson;
	algometa meta;
	string subid;
	std::unordered_map<int, instrument> LegSecIdMap;
	instrument Legs[4];
	int LegCount;

	string StartTime;
	string EndTime;

	double OrderQty;
	double TotalQty;

	int IntervalSecs;
	int MinWorkSecs;
	int MaxWorkSecs;

	double SqueezeRatio;
	bool Randomize;

	int NetPosition;
	int TotalExec;

	bool Reset;

};



#endif /* SRC_ALGOPARAMS_TIMEDAVGPARAMS_HPP_ */
