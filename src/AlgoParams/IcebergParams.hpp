//============================================================================
// Name        	: SRC_ALGOPARAMS_ICEBERGPARAMS_HPP_.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 5:44:11 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_ICEBERGPARAMS_HPP_
#define SRC_ALGOPARAMS_ICEBERGPARAMS_HPP_


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
using namespace rapidjson;

#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>

#include <AlgoSupport/algo_structs.hpp>

struct IcebergParams
{
	static IcebergParams parse(const std::string& json_str);

	bool ValidJson;

	algometa meta;
	string subid;
	std::unordered_map<int, instrument> LegSecIdMap;
	instrument Legs[24];
	int LegCount;

	std::unordered_map<string, KTN::Order> Ords;

//	int OrderQty;
//	int TotalQty;
//
	int MinShowQty;
	int MaxShowQty;

};

struct IceOrdParam
{
	int index;
	KOrderSide::Enum side;
	string symbol;

	string parentid;

	string ordnum;
	string sprdid;

	int totalfillqty;
	int ordqty;

	int minqty;
	int maxqty;
};



#endif /* SRC_ALGOPARAMS_AUTOHEDGEOB2PARAMS_HPP_ */
