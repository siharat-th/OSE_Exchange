//============================================================================
// Name        	: AutoHedgeOB2Params.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 5:44:11 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_STACKERPARAMS_HPP_
#define SRC_ALGOPARAMS_STACKERPARAMS_HPP_


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
 * @brief The StackerParams struct represents the parameters for the Stacker algorithm.
 */
struct StackerParams
{
	/**
  * @brief parse the StackerParams from a JSON string.
  * @param json_str The JSON string to parse.
  * @return The parsed StackerParams object.
  */
	static StackerParams parse(const std::string& json_str);

	bool ValidJson;

	algometa meta;
	string subid;

	std::unordered_map<int, instrument> LegSecIdMap;

	instrument Legs[4];
	int LegCount;

	double OrderQty;
	double TotalQty;
	double TotalExec;
	double TotalSent;
	double NetPosition;

	int MaxOpenBuys;
	int MaxOpenSells;
	int CurrOpenBuys;
	int CurrOpenSells;

	int BidLevels;
	int AskLevels;
	int OffsetLevels;

	bool SendBids;
	bool SendAsks;

	bool AutoHedge;
	int Edge;

	double SqueezeRatio;
	int SqueezeQty;


	double SkewThreshold;
	int SkewLevels;

	bool EnablePuke;
	double PukeRatio;
	int PukeQty;
	double PukePctPriority;

	bool EnableRestack;
	int RestackQty;
	bool RestackTopLevel;

	bool PullOnLargeTrade;
	int LargeTradeQty;
	int LargeOrdQty;
	bool TradeOnLargeQty;

	bool Reset;

};



#endif /* SRC_ALGOPARAMS_STACKERPARAMS_HPP_ */
