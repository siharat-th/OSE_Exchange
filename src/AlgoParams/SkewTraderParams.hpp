//============================================================================
// Name        	: SkewTraderParams.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Mar 20, 2024 5:44:11 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_SKEWTRADERARAMS_HPP_
#define SRC_ALGOPARAMS_SKEWTRADERARAMS_HPP_

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
 * @brief The SkewTraderParams struct represents the parameters for the SkewTrader algorithm.
 */
struct SkewTraderParams
{
    /**
     * @brief parse the JSON string and create a SkewTraderParams object.
     * @param json_str the JSON string to parse.
     * @return the parsed SkewTraderParams object.
     */
    static SkewTraderParams parse(const std::string& json_str);

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

    // double SqueezeRatio;
    // bool Randomize;

    bool SendBids;
    bool SendAsks;

    double SkewThreshold;
    double SkewMax;
    int SkewMaxQty;
    int SkewLevels;
    bool FlipSkew;

    // bool Aggressive;

    bool UseTotalPL;

    int ProfitTicks;
    int StopTicks;
    int TimeoutMS;

    int NetPosition;
    int TotalExec;

    int TotalBuys;
    int TotalSells;
    double TotalBuyPrice;
    double TotalSellPrice;
    double AvgBuyPrice;
    double AvgSellPrice;

    double SqueezeRatio;

    bool Reset;
};

#endif /* SRC_ALGOPARAMS_SKEWTRADERARAMS_HPP_ */
