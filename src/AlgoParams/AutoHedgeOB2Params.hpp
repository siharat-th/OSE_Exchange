//============================================================================
// Name        	: AutoHedgeOB2Params.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 5:44:11 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_AUTOHEDGEOB2PARAMS_HPP_
#define SRC_ALGOPARAMS_AUTOHEDGEOB2PARAMS_HPP_


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
 * @brief Struct representing the parameters for AutoHedgeOB2 algorithm.
 */
struct AutoHedgeOB2Params
{
	/**
  * @brief Parses the JSON string and returns an instance of AutoHedgeOB2Params.
  * @param json_str The JSON string to parse.
  * @return An instance of AutoHedgeOB2Params.
  */
	static AutoHedgeOB2Params parse(const std::string& json_str);

	bool ValidJson;

	algometa meta;
	string subid;

	std::unordered_map<int, instrument> LegSecIdMap;

	instrument Legs[4];
	int LegCount;

	std::unordered_map<string, KTN::Order> Ords;

	instrument HedgeLeg;

	string HdgSymbol;
	double HdgRatio;
	int HdgSecid;
	int HdgIndex;
	int TakeTicks;
	KOrderSide::Enum HdgSide;

	int HdgSent;
	double HdgRemainder;

	//placeholder
	int ProfitTaking;
	int StopLoss;
	int MinutesToLive;
};

#endif /* SRC_ALGOPARAMS_AUTOHEDGEOB2PARAMS_HPP_ */
