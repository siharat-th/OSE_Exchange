//============================================================================
// Name        	: LegParamParser.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 8:11:32 PM
//============================================================================

#include <AlgoParams/LegParamParser.hpp>


vector <instrument> LegParamParser::ParseSpreadLegs(string json)
{
	vector <instrument> legs;
	Document d;

	if (d.Parse(json.c_str()).HasParseError()) {
		int e = (unsigned)d.GetErrorOffset();
		ostringstream oss;
		oss << "JSON OFFSET ERROR AT " << e ;
		KT01_LOG_INFO("LegParamParser", oss.str())
;
	}

	//cout << json << endl;

	for (Value::ConstValueIterator itrmain = d.Begin(); itrmain != d.End(); ++itrmain)
	{
		string leglabel = "Legs";
		if ((*itrmain).HasMember("SpreadLegs"))
			leglabel = "SpreadLegs";

		const rapidjson::Value& lns = Value((*itrmain)[leglabel.c_str()], d.GetAllocator());
		assert(lns.IsArray());

		int index = 0;

		for (Value::ConstValueIterator itr = lns.Begin(); itr != lns.End(); ++itr)
		{
			instrument leg = {};
			leg.index = index;
			index++;

			if ((*itr).HasMember("Index"))
				leg.index = (*itr)["Index"].GetInt();

			leg.exch = (*itr)["Exch"].GetString();
			leg.symbol = (*itr)["Symbol"].GetString();
			string prodtype = (*itr)["ProdType"].GetString();

			if (prodtype == "future" || prodtype == "FUTURE" || prodtype == "FUT")
				leg.prodtype = KOrderProdType::FUTURE;
			else
				leg.prodtype = KOrderProdType::SPREAD;
			//TODO:options?


			//************ HACK FOR NOW>>>>>>>>
			if (leg.symbol.substr(0, 2).compare("ES") == 0)
				leg.tickvalue = 0.50;

			if (leg.symbol.substr(0, 2).compare("NQ") == 0)
				leg.tickvalue = 0.20;

			if (leg.symbol.substr(0, 3).compare("MES") == 0)
				leg.tickvalue = 0.050;

			if (leg.symbol.substr(0, 3).compare("MNQ") == 0)
				leg.tickvalue = 0.020;

			if (leg.symbol.substr(0, 2).compare("CL") == 0)
				leg.tickvalue = 10;

			if (leg.symbol.substr(0, 2).compare("NG") == 0)
				leg.tickvalue = 10;

			if (leg.symbol.substr(0, 2).compare("BZ") == 0)
				leg.tickvalue = 10;

			if (leg.symbol.substr(0, 2).compare("RB") == 0 || leg.symbol.substr(0, 2).compare("HO") == 0)
				leg.tickvalue = 4.2;


			if (leg.exch == "cme" || leg.exch == "CME")
			{
				leg.iscme = true;
				leg.exchEnum = KOrderExchange::CME;
			}
			else if (leg.exch == "cfe" || leg.exch == "CFE")
			{
				leg.exchEnum = KOrderExchange::CFE;
			}

			if (leg.symbol.length() < 3)
				continue;

			leg.pricemult = (*itr)["PriceMult"].GetDouble();
			leg.spreadmult = (*itr)["SpreadMult"].GetDouble();
			leg.iside = (leg.spreadmult > 0) ? 1 : -1;
			leg.activeQuoting = (*itr)["ActiveQuoting"].GetBool();
			//using the old inverse tag for now
			leg.squeeze = (*itr)["Inverse"].GetBool();
			leg.lastAsk = 0.0;
			leg.lastBid = 0.0;

			legs.push_back(leg);

			KT01_LOG_INFO("LegParamParser", "LEG: " + leg.Print());
		}
	}//end top level list (convenience, remember?)
	return legs;
}

std::unordered_map<string, bool> LegParamParser::GetActiveQuoteSetting(Document& d)
{
	std::unordered_map<string, bool> res;
	for (Value::ConstValueIterator itrmain = d.Begin(); itrmain != d.End(); ++itrmain)
	{
		string leglabel = "Legs";
		if ((*itrmain).HasMember("SpreadLegs"))
			leglabel = "SpreadLegs";

		const rapidjson::Value& lns = Value((*itrmain)[leglabel.c_str()], d.GetAllocator());

		for (Value::ConstValueIterator itr = lns.Begin(); itr != lns.End(); ++itr)
		{
			string symbol = (*itr)["Symbol"].GetString();
			res[symbol] = (*itr)["ActiveQuoting"].GetBool();
		}
	}
	return res;
}


instrument LegParamParser::GetCmeLeg(string symbol, KOrderProdType::Enum prodtype, int index)
{
	instrument leg = {};
	leg.index = index;
	index++;

	leg.exch = "cme";
	leg.symbol = symbol;
	leg.prodtype = prodtype;

	//************ HACK FOR NOW>>>>>>>>
	if (leg.symbol.substr(0, 2).compare("ES") == 0)
		leg.tickvalue = 0.50;

	if (leg.symbol.substr(0, 2).compare("NQ") == 0)
		leg.tickvalue = 0.20;

	if (leg.symbol.substr(0, 3).compare("MES") == 0)
		leg.tickvalue = 0.050;

	if (leg.symbol.substr(0, 3).compare("MNQ") == 0)
		leg.tickvalue = 0.020;

	if (leg.symbol.substr(0, 2).compare("CL") == 0)
		leg.tickvalue = 10;

	if (leg.symbol.substr(0, 3).compare("MCL") == 0)
		leg.tickvalue = 1;

	if (leg.exch == "cme" || leg.exch == "CME")
	{
		KT01_LOG_INFO(__CLASS_STATIC__(LegParamParser), leg.symbol + " GOT CME LEG!!!");
		leg.iscme = true;
	}

	leg.pricemult = 1;
	leg.spreadmult = 1;

	leg.iside = (leg.spreadmult > 0) ? 1 : -1;

	leg.activeQuoting = false;
	leg.lastAsk = 0.0;
	leg.lastBid = 0.0;

	return leg;
}
