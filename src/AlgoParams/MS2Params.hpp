//============================================================================
// Name        	: MultiSpreader2Params.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 5:44:11 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_MULTISPREADER2PARAMS_HPP_
#define SRC_ALGOPARAMS_MULTISPREADER2PARAMS_HPP_


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
 * @brief The struct representing the parameters for the MultiSpreader2 algorithm.
 */
struct MS2Params
{
	/**
  * @brief Parses the JSON string and returns an instance of MS2Params.
  * @param json_str The JSON string to parse.
  * @return An instance of MS2Params.
  */
	static MS2Params parse(const std::string& json_str);

	bool ValidJson;

	algometa meta;
	string subid;
	std::unordered_map<int, instrument> LegSecIdMap;
	std::unordered_map<string, KTN::Order> Ords;
	instrument Legs[24];
	int LegCount;

	bool TestMode;
	bool Reset;

	double OrderQty;
	double TotalQty;
	double MaxPosition;
	bool ReloadOnFill;

	int MinLeanQty;

	//int QuoteRefreshMS;

	double NetPosition;
	double TotalExec;
	double TotalWorking;

	int MsgsSent;
	int MsgsExec;

	double Slop;

	bool SendBids;
	bool SendAsks;

	double SpreadBid;
	double SpreadAsk;
	double BuyDisc;
	double SellDisc;

	double HedgeStop;

	double SqueezeRatio;
	int SqueezeQty;

	bool WavgPlus;
	double WavgThreshold;
	double ExchFee;

	bool Generative;
	bool Doubles;
	bool Triples;

};



#endif /* SRC_ALGOPARAMS_MULTISPREADER2PARAMS_HPP_ */
