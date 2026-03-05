//============================================================================
// Name        	: SRC_ALGOPARAMS_SQZOB2PARAMS_HPP_.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 5:44:11 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_SQZOB2PARAMS_HPP_
#define SRC_ALGOPARAMS_SQZOB2PARAMS_HPP_


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
using namespace rapidjson;

#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>

#include <AlgoSupport/algo_structs.hpp>
#include <AlgoParams/IParams.hpp>

#include <akl/Price.hpp>

/**
 * @brief The SqueezerParams struct represents the parameters for the Squeezer algorithm.
 */
struct SqueezerParams
{
	static SqueezerParams parse(const std::string& json_str);

	bool ValidJson;

	algometa meta;
	string subid;
	std::unordered_map<int, instrument> LegSecIdMap;
	instrument Legs[24];
	int LegCount;

	std::unordered_map<string, KTN::Order> Ords;

	double OrderQty;
	double TotalQty;

};

/**
 * @brief The SqzOrdParam struct represents the parameters for a Squeezer order.
 */
struct SqzOrdParam : IOBParams
{
	int index;
	KOrderSide::Enum side;
	KOrderAlgoTrigger::Enum trig;
	string symbol;
	int secid;

	string ordnum;
	string sprdid;

	akl::Price sqzprice;
	int sqzqty;
	float sqzratio;
	int largetrade;

};

#endif /* SRC_ALGOPARAMS_SQZOB2PARAMS_HPP_ */
