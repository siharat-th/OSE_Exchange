/*
 * OrderFormatter.h
 *
 *  Created on: Nov 29, 2016
 *      Author: sgaer
 */

#ifndef ORDERFORMATTER_H_
#define ORDERFORMATTER_H_

#pragma once
#include <cstring>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
using namespace rapidjson;

#include <Orders/Order.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <Orders/OrderUtils.hpp>

using namespace std;

namespace KTN{

class OrderFormatter {
public:


inline static std::string FormatOrdersJson(std::vector<KTN::Order> & ords)
	{
		StringBuffer s;
		s.Clear();

		Writer<StringBuffer> writer(s);

		writer.StartObject();

		writer.Key("header");
		writer.String("orders");

		writer.Key("version");
		writer.String("V3");

		writer.Key("source");
		writer.String(ords[0].source.c_str());


		std::string hdr("orders");
		writer.String(hdr.c_str());

		writer.StartArray();

		for (auto ord : ords)
		{
			string transtime = "";
			//first get the time:
			if (ord.timestamp > 0)
			{
				 // Convert nanoseconds to a duration
				std::chrono::nanoseconds ns(ord.timestamp);

				// Convert nanoseconds to time_point
				std::chrono::system_clock::time_point timePoint(std::chrono::duration_cast<std::chrono::system_clock::duration>(ns));

				// Convert time_point to time in local time zone
				std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
				std::tm tm;

				localtime_r(&time, &tm);

				// Format the time in EST (Eastern Standard Time)
				ostringstream otime;
				otime	  << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
					  << "." << std::setw(9) << std::setfill('0') << ord.timestamp % 1000000000;

				transtime = otime.str();
			}


			writer.StartObject();

				writer.Key("user");
				writer.String(ord.user.c_str());

				writer.Key("org");
				writer.String(ord.org.c_str());

				writer.Key("groupname");
				writer.String(ord.groupname.c_str());

				writer.Key("algoid");
				writer.String(ord.algoid.c_str());

				writer.Key("action");
				writer.String(ord.action.c_str());

			    string exchid = OrderUtils::ConvertCmeId(ord.exchordid);
				writer.Key("exchordid");
				writer.String(exchid.c_str());

				writer.Key("strategy");
				writer.String(ord.strategy.c_str());

				writer.Key("stratid");
				writer.Int(ord.stratid);

				string ordernum = OrderUtils::toString(ord.ordernum);
				writer.Key("ordernum");
				writer.String(ordernum.c_str());

				string origordernum = OrderUtils::toString(ord.origclordid);
				writer.Key("origclordid");
				writer.String(ord.origclordid);

				writer.Key("exchange");
				writer.String(ord.exchange.c_str());

				writer.Key("product");
				writer.String(ord.product.c_str());

				writer.Key("symbol");
				string symbol = OrderUtils::toString(ord.symbol);
				writer.String(symbol.c_str());

				writer.Key("secid");
				writer.Int(ord.secid);

				writer.Key("prodtype");
				writer.String(KOrderProdType::toString(ord.OrdProdType).c_str());

				writer.Key("side");
				writer.String(KOrderSide::toString(ord.OrdSide).c_str());

				writer.Key("price");
				writer.Int(ord.price);

				writer.Key("quantity");
				writer.Int(ord.quantity);

				writer.Key("leavesqty");
				writer.Int(ord.leavesqty);

				writer.Key("fillqty");
				writer.Int(ord.fillqty);

				writer.Key("lastqty");
				writer.Int(ord.lastqty);

				writer.Key("lastpx");
				writer.Int(ord.lastpx);

				writer.Key("disqty");
				writer.Int(ord.dispqty);

				writer.Key("status");
				writer.String(KOrderStatus::toString(ord.OrdStatus).c_str());

				writer.Key("tag");
				writer.String(ord.tag.c_str());

				writer.Key("tag2");
				writer.String(ord.tag2.c_str());

				writer.Key("source");
				writer.String(ord.source.c_str());


				writer.Key("execid");
				writer.String(ord.execid.c_str());

				writer.Key("text");
				writer.String(ord.text.c_str());

				writer.Key("rpt");
				writer.Int(ord.rpt);

				writer.Key("state");
				writer.String(ord.state.c_str());

				writer.Key("ssboe");
				writer.Int64(ord.timestamp);

				writer.Key("iRptType");
				writer.Int((int)ord.OrdAction);

				writer.Key("filltype");
				writer.Int(ord.filltype);

				writer.Key("reporttype");
				writer.String(ord.reporttype.c_str());

				writer.Key("reference");
				writer.String(ord.reference.c_str());

				writer.Key("stoppx");
				writer.Int(ord.stoppx);

				writer.Key("ordtype");
				string ordtype = KOrderType::toString(ord.OrdType);
				writer.String(ordtype.c_str());

				writer.Key("tif");
				string ordtif = KOrderTif::toString(ord.OrdTif);
				writer.String(ordtif.c_str());

				//new KOrder stuff
				writer.Key("KOrdSide");
				writer.String(KOrderSide::toString(ord.OrdSide).c_str());

				writer.Key("KOrdType");
				writer.String(KOrderType::toString(ord.OrdType).c_str());

				writer.Key("KOrdState");
				writer.String(KOrderState::toString(ord.OrdState).c_str());

				writer.Key("KOrdExch");
				writer.String(KOrderExchange::toString(ord.OrdExch).c_str());

				writer.Key("KOrdProdType");
				writer.String(KOrderProdType::toString(ord.OrdProdType).c_str());

				writer.Key("KOrdFillType");
				writer.String(KOrderFillType::toString(ord.OrdFillType).c_str());

				writer.Key("KOrdStatus");
				writer.String(KOrderStatus::toString(ord.OrdStatus).c_str());

				writer.Key("KOrdTif");
				writer.String(KOrderTif::toString(ord.OrdTif).c_str());

				writer.Key("KOrdAction");
				writer.String(KOrderAction::toString(ord.OrdAction).c_str());

				writer.Key("KOrdPutCall");
				writer.String(KOrderPutCall::toString(ord.OrdPutCall).c_str());

				writer.Key("KOrdAlgoTrigger");
				writer.String(KOrderAlgoTrigger::toString(ord.OrdAlgoTrig).c_str());

				writer.Key("timestamp");
				writer.Int64(ord.timestamp);

				writer.Key("transacttime");
				writer.String(transtime.c_str());


			writer.EndObject();

		}

		writer.EndArray();
		writer.EndObject();

		std::string res = s.GetString() ;

		return res;

}



};

};//end namespace

#endif /* ORDERFORMATTER_H_ */
