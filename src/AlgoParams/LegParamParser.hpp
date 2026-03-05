//============================================================================
// Name        	: LegParamParser.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 8:11:32 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_LEGPARAMPARSER_HPP_
#define SRC_ALGOPARAMS_LEGPARAMPARSER_HPP_
#pragma once

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
using namespace rapidjson;

#include <KT01/Core/Macro.hpp>
#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>

#include <AlgoSupport/algo_structs.hpp>

/**
 * @brief The LegParamParser class provides methods to parse and retrieve leg parameters.
 */
class LegParamParser {
public:
	/**
  * @brief Parses the spread legs from the given JSON string.
  * @param json The JSON string containing the spread legs.
  * @return A vector of instrument objects representing the spread legs.
  */
	static vector<instrument> ParseSpreadLegs(string json);

	/**
  * @brief Retrieves the active quote settings from the given JSON document.
  * @param d The JSON document.
  * @return An unordered map with the active quote settings.
  */
	static std::unordered_map<string, bool> GetActiveQuoteSetting(Document& d);

	/**
  * @brief Retrieves the CME leg for the given symbol, product type, and index.
  * @param symbol The symbol of the leg.
  * @param prodtype The product type of the leg.
  * @param index The index of the leg.
  * @return The CME leg instrument.
  */
	static instrument GetCmeLeg(string symbol, KOrderProdType::Enum prodtype, int index);
};

#endif /* SRC_ALGOPARAMS_LEGPARAMPARSER_HPP_ */
