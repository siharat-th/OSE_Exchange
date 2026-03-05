//============================================================================
// Name        	: LegParamParser.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 8:11:32 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_ORDERPARSER_HPP_
#define SRC_ALGOPARAMS_ORDERPARSER_HPP_

#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
using namespace rapidjson;



#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>

#include <AlgoSupport/algo_structs.hpp>
#include <Orders/Order.hpp>
#include <Orders/OrderUtils.hpp>

/**
 * @class OrderParser
 * @brief Parses JSON order data and converts it into Order objects.
 */
class OrderParser {
public:
	/**
  * @brief Parses a JSON order and returns an Order object.
  * @param json_ord The JSON order data.
  * @return The parsed Order object.
  */
	static KTN::Order parseorder(const rapidjson::Value& json_ord);

	/**
  * @brief Parses incoming JSON order data and returns an Order object.
  * @param json_ord The incoming JSON order data.
  * @return The parsed Order object.
  */
	static KTN::Order parseincoming(const rapidjson::Value& json_ord);

	/**
  * @brief Parses a JSON order and returns an OrderPod object.
  * @param json_ord The JSON order data.
  * @return The parsed OrderPod object.
  */
	static KTN::OrderPod parsepod(const rapidjson::Value& json_ord);
};

#endif /* SRC_ALGOPARAMS_LEGPARAMPARSER_HPP_ */
