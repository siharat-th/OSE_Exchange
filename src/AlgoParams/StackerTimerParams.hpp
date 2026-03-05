//============================================================================
// Name        	: SRC_ALGOPARAMS_STACKERTIMERPARAMS_HPP_.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jan 20, 2024 5:44:11 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_STACKERTIMERPARAMS_HPP_
#define SRC_ALGOPARAMS_STACKERTIMERPARAMS_HPP_


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
 * @brief The StackerTimerParams struct represents the parameters for the StackerTimer algorithm.
 */
struct StackerTimerParams
{
	/**
  * @brief parse the JSON string and create a StackerTimerParams object.
  * @param json_str The JSON string to parse.
  * @return The parsed StackerTimerParams object.
  */
	static StackerTimerParams parse(const std::string& json_str);
	
	bool ValidJson;

	algometa meta;
	string subid;

	std::unordered_map<int, instrument> LegSecIdMap;
	instrument Legs[4];
	int LegCount;

	int OrderQty;
	int TotalQty;
	int TotalExec;

	int NetPosition;
	bool Reset;

	int StackLevels;
	int StackOffsetTicks;
	string StackSide;
	KOrderSide::Enum OrdSide;


	bool EnablePuke;
	double PukeRatio;
	double SqueezeRatio;
	int PukeQty;

	bool PullOnLargeTrade;
	int LargeTradeQty;

	bool CoverAtClose;
	string CloseStartTime;
	string CloseEndTime;

};



#endif /* SRC_ALGOPARAMS_STACKERTIMERPARAMS_HPP_ */
