//============================================================================
// Name        	: LegParamParser.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 8:11:32 PM
//============================================================================

#include <AlgoParams/OrderParser.hpp>


KTN::Order OrderParser::parseorder(const rapidjson::Value& json_ord)
{
//#ifdef DEBUG_TESTING
//std::cout << "ORDER: " << json_ord << endl;
//#endif

	KTN::Order ord = {};
	OrderUtils::Copy(ord.symbol, json_ord["symbol"].GetString());
	string side = json_ord["side"].GetString();

	if (side == "B")
		ord.OrdSide = KOrderSide::BUY;

	if (side == "S")
		ord.OrdSide = KOrderSide::SELL;


	if (json_ord.HasMember("algoid"))
		ord.algoid = json_ord["algoid"].GetString();

	ord.quantity = json_ord["quantity"].GetInt();


	if (json_ord.HasMember("ordtype"))
	{
		string ordtype =json_ord["ordtype"].GetString();
		if (ordtype == "LIMIT")
			ord.OrdType = KOrderType::LIMIT;
		if (ordtype == "MARKET")
			ord.OrdType = KOrderType::MARKET;
		if (ordtype == "STOP")
			ord.OrdType = KOrderType::STOPLIMIT;
	}
	else
		ord.OrdType = KOrderType::LIMIT;

	ord.price = json_ord["price"].GetDouble();
	ord.action = json_ord["action"].GetString();
	ord.OrdAction = KOrderAction::Value(ord.action);

	if (ord.OrdAction == KOrderAction::ACTION_MOD || ord.OrdAction == KOrderAction::ACTION_CXL)
		OrderUtils::Copy(ord.origclordid, json_ord["ordernum"].GetString());
#ifdef DEBUG_TESTING
	cout << "ORD PARSE AFTER: " + OrderUtils::Print(ord) << endl;
#endif

	return ord;

}

KTN::Order OrderParser::parseincoming(const rapidjson::Value& json_ord)
{
	KTN::Order ord = {};

	ord.action = json_ord["action"].GetString();
	OrderUtils::Copy(ord.symbol, json_ord["symbol"].GetString());

	string side = json_ord["side"].GetString();
	if (side == "B")
		ord.OrdSide = KOrderSide::BUY;
	if (side == "S")
		ord.OrdSide = KOrderSide::SELL;

	ord.exchange = json_ord["exchange"].GetString();
	ord.strategy = json_ord["strategy"].GetString();
	//ord.prodtype = json_ord["prodtype"].GetString();
	ord.tag = json_ord["tag"].GetString();
	ord.tag2 = json_ord["tag2"].GetString();

	ord.source = json_ord["source"].GetString();

	ord.quantity = json_ord["quantity"].GetInt();
	ord.price = json_ord["price"].GetDouble();

	ord.leavesqty = json_ord["leavesqty"].GetInt();

	if (json_ord.HasMember("fillqty") && json_ord["fillqty"].IsInt())
	{
		ord.fillqty = json_ord["fillqty"].GetInt();
	}
	else
	{
		ord.fillqty = 0;
	}

	OrderUtils::Copy(ord.ordernum, json_ord["ordernum"].GetString());

	if (json_ord.HasMember("exchordid"))
	{
		string id = json_ord["exchordid"].GetString();
		if (id.length() > 0)
				ord.exchordid = stoull(id.c_str());
	}

	ord.timestamp = 0;
	ord.stoppx = 0;
	if (json_ord.HasMember("stoppx"))
		ord.stoppx = json_ord["stoppx"].GetDouble();


	ord.user = json_ord["user"].GetString();
	ord.org = json_ord["org"].GetString();
	ord.groupname = json_ord["groupname"].GetString();



	if (ord.action.compare("NEW") == 0)
	{
		string tif = json_ord["tif"].GetString();
		if (tif.compare("GTC") ==0)
		{
			ord.OrdTif = KOrderTif::GTC;
		}

		if (tif.compare("FOK")== 0 || tif.compare("IOC") == 0)
		{
			ord.OrdTif = KOrderTif::FAK;
		}

		if (tif.compare("FAK")== 0)
		{
			ord.OrdTif = KOrderTif::FAK;
		}

	}


	if (json_ord.HasMember("algoid"))
		ord.algoid = json_ord["algoid"].GetString();
	else
		ord.algoid = "ORDBOOK";

	ord.guid = ord.algoid;

	if (json_ord.HasMember("ordtype"))
	{
		string ordtype =json_ord["ordtype"].GetString();
		if (ordtype == "LIMIT")
			ord.OrdType = KOrderType::LIMIT;
		if (ordtype == "MARKET")
			ord.OrdType = KOrderType::MARKET;
		if (ordtype == "STOP")
			ord.OrdType = KOrderType::STOP;
		if (ordtype == "STOPLIMIT")
			ord.OrdType = KOrderType::STOPLIMIT;
	}
	else
		ord.OrdType = KOrderType::LIMIT;


	ord.OrdAction = KOrderAction::Value(ord.action);
	if (ord.OrdAction == KOrderAction::ACTION_MOD || ord.OrdAction == KOrderAction::ACTION_CXL)
		OrderUtils::Copy(ord.origclordid, json_ord["ordernum"].GetString());

	return ord;

}

