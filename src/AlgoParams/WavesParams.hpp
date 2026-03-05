//============================================================================
// Name        	: WavesParams.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Mar 27, 2024 5:44:11 PM
//============================================================================

#ifndef SRC_WAVESPARAMS_LITTLEIMPPARAMS_HPP_
#define SRC_WAVESPARAMS_LITTLEIMPPARAMS_HPP_

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
 * @brief Struct representing the parameters for the Waves algorithm.
 */
struct WavesParams
{
	static const int MAX_SIZE = 24;
	//replacing the old n and j....
	EdgeStruct curveMatrix[MAX_SIZE][MAX_SIZE];

	/**
  * @brief Initializes the curveMatrix with default values.
  */
	void init()
	{
		for (int i = 0; i < MAX_SIZE; ++i) {
			for (int j = 0; j < MAX_SIZE; ++j) {
				curveMatrix[i][j].index = 0;
				curveMatrix[i][j].istas = false;
				curveMatrix[i][j].exists = false; // Initially, no edges exist
				curveMatrix[i][j].prices.bid = 0; // Initialize bid price to some default value
				curveMatrix[i][j].prices.ask = 0; // Initialize ask price to some default value
			}
		}
	}

	/**
  * @brief Parses the WavesParams from a JSON string.
  * @param json_str The JSON string to parse.
  * @return The parsed WavesParams object.
  */
	static WavesParams parse(const std::string& json_str);

	bool ValidJson;

	algometa meta;
	string subid;

	std::unordered_map<int, instrument> LegSecIdMap;

	instrument Legs[32];
	int LegCount;

	combopath paths[MAX_SIZE][MAX_SIZE];

	bool TestMode;
	bool Reset;

	string Product;
	string StartMonth;
	int StartYear;
	int NumMonths;

	int MinWidth;
	int MaxWidth;

	int OrderQty;
	int TotalQty;
	int TotalExec;

	int BidEdge;
	int AskEdge;

	bool SendBids;
	bool SendAsks;

	bool Doubles;
	bool Triples;
	bool Quads;
	double WavgThreshold;

	double SqueezeRatio;
	int SqueezeQty;
};

#endif /* SRC_WAVESPARAMS_LITTLEIMPPARAMS_HPP_ */
