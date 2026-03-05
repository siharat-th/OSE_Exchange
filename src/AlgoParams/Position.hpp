//============================================================================
// Name        	: Position.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 5, 2023 10:11:57 AM
//============================================================================

#ifndef SRC_ALGOPARAMS_POSITION_HPP_
#define SRC_ALGOPARAMS_POSITION_HPP_
#pragma once
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <AlgoSupport/algo_structs.hpp>
#include <AlgoParams/RiskBase.hpp>

struct KPosStatus
{
	enum Enum
	{
		NONE,
		MONITOR_PL,
		PROFIT_HIT,
		STOP_HIT,
		TIMED_OUT,
		CANCELED,
		COVERED,
		WAITING,
		OTHER
	};

	static std::string toString(Enum status)
	{
		switch(status) {
			case KPosStatus::NONE:
				return "NONE";
			case KPosStatus::MONITOR_PL:
				return "MONITOR_PL";
			case KPosStatus::PROFIT_HIT:
				return "PROFIT_HIT";
			case KPosStatus::STOP_HIT:
				return "STOP_HIT";
			case KPosStatus::TIMED_OUT:
				return "TIMED_OUT";
			case KPosStatus::CANCELED:
				return "CANCELED";
			case KPosStatus::COVERED:
				return "COVERED";
			case KPosStatus::WAITING:
				return "WAITING";
			case KPosStatus::OTHER:
				return "OTHER";
			default:
				return "INVALID";
		}
	}
};

struct PositionSimple
{
	int total;
	int sumpx;
	double avgpx;

	void add(int qty, int px)
	{
		total += qty;
		sumpx += px;

		avgpx = sumpx / (double)total;
	}
};

struct PosNet
{
	PositionSimple b;
	PositionSimple s;
};

struct PositionLeg
{
	instrument leg;
	int index;

	KOrderSide::Enum side;
	int netpos;
	double price;

	double bid;
	double ask;

	double pl;

	void calc()
	{
		double px = (netpos > 0) ? bid : ask;
		pl = netpos * (px - price ) * leg.tickvalue;
	}

};

struct Position
{
	//for concurrent maps etc...
	bool erased;

	string algoid;
	string source;

	string posid;
	RiskBase risk;

	int side;

	int spreadpos;

	double totpl;
	double plper;
	double price;
	double mktprice;

	string entrytime;
	double targetprice;
	double stoprice;

	double pt_pl;
	double sl_pl;
	int ttl_secs;

	//for reporting
	uint64_t ssboe;
	string updatetime;

	KPosStatus::Enum status;

	std::map<int, PositionLeg> legs;

//	PositionLeg legs[24];
//	vector<PositionLeg> legs;
//	int numlegs;

	void addleg(PositionLeg leg)
	{
		if (legs.find(leg.index) == legs.end())
			legs[leg.index] = leg;
		else
			legs[leg.index].netpos += leg.netpos;

		//later, we can do wavg price of legs....
		//hello shit gpt
	}

//	int findlegid(string symbol)
//	{
//		for (int i = 0; i < numlegs; i++)
//			if (legs[i].leg.symbol == symbol)
//				return i;
//
//		return -1;
//	}

};


class PositionJsonConverter
{
public:
    static std::string toJsonString(const Position& pos)
    {
        rapidjson::Document document;
        document.SetObject();

        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

        rapidjson::Value posJson(rapidjson::kObjectType);
        posJson.AddMember("header", rapidjson::Value("algopl", allocator).Move(), allocator);
        posJson.AddMember("source", rapidjson::Value(pos.source.c_str(), allocator).Move(), allocator);
        posJson.AddMember("algoid", rapidjson::Value(pos.algoid.c_str(), allocator).Move(), allocator);
        posJson.AddMember("posid", rapidjson::Value(pos.posid.c_str(), allocator).Move(), allocator);
        posJson.AddMember("status", rapidjson::Value(KPosStatus::toString(pos.status).c_str(), allocator).Move(), allocator);
        posJson.AddMember("risk", toJson(pos.risk, allocator), allocator);
        posJson.AddMember("totpl", pos.totpl, allocator);
        posJson.AddMember("plper", pos.plper, allocator);
        posJson.AddMember("sprdpos", pos.spreadpos, allocator);
        posJson.AddMember("price", pos.price, allocator);
        posJson.AddMember("last", pos.mktprice, allocator);

        posJson.AddMember("targetprice", pos.targetprice, allocator);
        posJson.AddMember("stopprice", pos.stoprice, allocator);
        posJson.AddMember("entrytime", rapidjson::Value(pos.entrytime.c_str(), allocator).Move(), allocator);
        posJson.AddMember("secondsleft", pos.risk.seconds_left, allocator);
        posJson.AddMember("ssboe", pos.ssboe, allocator);
        posJson.AddMember("updatetime", rapidjson::Value(pos.updatetime.c_str(), allocator).Move(), allocator);

        rapidjson::Value legsJson(rapidjson::kArrayType);
        for (auto kv : pos.legs)
        {
            legsJson.PushBack(toJson(kv.second, allocator), allocator);
        }
        posJson.AddMember("legs", legsJson, allocator);

        document.Swap(posJson);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);

        return buffer.GetString();
    }

private:
    static rapidjson::Value toJson(const RiskBase& risk, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value riskJson(rapidjson::kObjectType);
        riskJson.AddMember("ProfitTaking", risk.ProfitTaking, allocator);
        riskJson.AddMember("StopLoss", risk.StopLoss, allocator);
        riskJson.AddMember("MinutesToLive", risk.MinutesToLive, allocator);

        return riskJson;
    }

    static rapidjson::Value toJson(const PositionLeg& leg, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value legJson(rapidjson::kObjectType);
        legJson.AddMember("exch", rapidjson::Value(leg.leg.exch.c_str(), allocator).Move(), allocator);

        legJson.AddMember("prodtype", rapidjson::Value(KOrderProdType::toString(leg.leg.prodtype).c_str(), allocator).Move(), allocator);
        legJson.AddMember("product", rapidjson::Value(leg.leg.product.c_str(), allocator).Move(), allocator);
        legJson.AddMember("symbol", rapidjson::Value(leg.leg.symbol.c_str(), allocator).Move(), allocator);
        legJson.AddMember("secid", leg.leg.secid, allocator);
        legJson.AddMember("mktsegid", leg.leg.mktsegid, allocator);
        legJson.AddMember("netpos", leg.netpos, allocator);
        legJson.AddMember("price", leg.price, allocator);
        legJson.AddMember("bid", leg.bid, allocator);
        legJson.AddMember("ask", leg.ask, allocator);
        legJson.AddMember("pl", leg.pl, allocator);
        return legJson;
    }
};


#endif /* SRC_ALGOPARAMS_POSITION_HPP_ */
