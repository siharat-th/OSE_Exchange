//============================================================================
// Name        	: PLWatcherParams.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: May 24, 2024 1:07:59 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_PLWATCHERPARAMS_HPP_
#define SRC_ALGOPARAMS_PLWATCHERPARAMS_HPP_


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

/**
 * @brief The PLWatcherParams struct represents the parameters for the PLWatcher algorithm.
 */
struct PLWatcherParams
{
	static PLWatcherParams parse(const std::string& json_str);

	bool ValidJson;

	algometa meta;
	string subid;
	std::unordered_map<int, instrument> LegSecIdMap;
	instrument Legs[24];
	int LegCount;

};

/**
 * @brief The PLPosition struct represents the position parameters for the PLWatcher algorithm.
 */
struct PLPosition : IOBParams
{
	int index;
	KOrderSide::Enum side;
	KOrderAlgoTrigger::Enum trig;
	string symbol;
	int secid;
	int qty;
	double price;
	double mintick;

	int bid;
	int ask;

	double pl;

	/**
  * @brief Calculate the PL (Profit/Loss) for the position based on the new bid and ask prices.
  * @param newbid The new bid price.
  * @param newask The new ask price.
  */
	void Calculate(int newbid, int newask)
	{
		bid = newbid;
		ask = newask;

		int plprice = (side == KOrderSide::BUY) ? ask : bid;
		double templ = side * (plprice - price) * mintick * qty;

		pl = templ;
	}

};

/**
 * @brief The PLParams struct represents the parameters for the PL algorithm.
 */
struct PLParams
{
	vector<PLPosition> Positions;
	string SpreadId;

	bool UseBarriers;
	bool UseSqueeze ;
	int SqueezeQty ;
	int PrimaryLeg ;

	double MaxPl ;
	double MinPl ;
	int TimeToLiveSecs ;

	double LastPL;
	bool Elected;
	KOrderAlgoTrigger Trig;
};

#endif /* SRC_ALGOPARAMS_PLWATCHERPARAMS_HPP_ */
