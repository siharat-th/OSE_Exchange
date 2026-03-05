#ifndef ORDER_HPP_
#define ORDER_HPP_

#pragma once
#include <string>
#include <vector>
#include <string>       // std::string
#include <iostream>
#include <sstream>

#include <unordered_map>

#include <KT01/DataStructures/UserStructs.hpp>
#include <KT01/Core/PerformanceCounter.h>
#include <KT01/Core/Macro.hpp>

#include <Orders/OrderEnumsV2.hpp>

using namespace KTN;

using namespace std;
using namespace KTN::ORD;

namespace KTN {

/**
 * Enumeration for Put and Call options.
 */
enum PutCall
{
	PUT, CALL
};

/**
 * Struct representing JSON header information.
 */
struct JsonHeader
{
	string topic;
	string source;
	string destination;
	bool isEmpty;
	bool hasUGO;
};

/**
 * Struct representing an order.
 * This is a legacy struct that is still used in some parts of the code.
 * Primarily for database and reporting ops
 */
struct Order
{
	uint64_t orderReqId;
	uint64_t exchordid;

	KOrderExchange::Enum 	OrdExch;
	KOrderSide::Enum 		OrdSide;
	KOrderProdType::Enum 	OrdProdType;
	KOrderType::Enum 		OrdType;
	KOrderFillType::Enum 	OrdFillType;
	KOrderStatus::Enum 		OrdStatus;
	KOrderTif::Enum 		OrdTif;
	KOrderPutCall::Enum		OrdPutCall;
	KOrderAction::Enum		OrdAction;
	KOrderState::Enum		OrdState;
	KOrderAlgoTrigger::Enum	OrdAlgoTrig;

	int callbackid;
	int stratid;

	//auto hedging
	double hedgeratio;
	int taketicks;
	double mintick;
	int hedgeparent;

	uint16_t revision;

	int partydetails;
	int mktsegid;
	int massscope;
	bool isManual;

	string openclose;


	char symbol[20];
	//char secgrp[20];

	int32_t secid;
	bool cert;

	char ordernum[20];
	char origclordid[20];

	uint32_t quantity;
	uint32_t lastqty;
	uint32_t leavesqty;
	uint32_t fillqty;
	uint32_t dispqty;
	uint32_t minqty;

	float price;
	float lastpx;
	float stoppx;

	int filltype;
	std::string execid;

	int iStatusReason;
	string reason;

	//non-essential fields:
	std::string user;
	std::string org;
	std::string groupname;
	std::string algoid;

	std::string guid;
	std::string strategy;
	std::string tag;
	std::string tag2;
	std::string source;
	std::string text;
	std::string state;
	std::string action;

	//for ob algos:
	int instindex;
	bool isSpreadLeg;

	std::string product;
	std::string exchange;

	std::string ind;
	std::string reporttype;
	int rpt;



	//options fields
	int strike;
	string exp;
	int mdy;

	long timestamp;

	std::string transacttime;

	//let's use this to track hedge to originating fills as a unit
	string reference;

	//ordermgr internal
	bool isFound;

	//order synchronization
	bool isSync;

};//__attribute__((packed));


/**
 * Struct representing options fields.
 */
struct OptionsFields
{
	KOrderPutCall::Enum	OrdPutCall;
	int strike;
	string exp;
	int mdy;

};

/**
 * Struct representing an order pool.
 */
struct OrderPool
{
	// We could've chosen to use a std::array that would allocate the memory on the stack instead of the heap.
	// We would have to measure to see which one yields better performance.
	// It is good to have objects on the stack but performance starts getting worse as the size of the pool increases.
	std::vector<KTN::Order*> pool;
	size_t count = 0;
	size_t max = 0;

	/**
  * Add an order to the pool.
  * @param ord The order to add.
  */
	void add(KTN::Order * ord)
	{
		pool.push_back(ord);
		max = pool.size()-1;
	}

	/**
  * Get an order from the pool.
  * @return The order.
  */
	KTN::Order* get()
	{
		count ++;
		if (UNLIKELY(count >= max))
		{
    count = 0;
		}
		//cout << " ORD POOL SERVING " << count << ": " << pool[count]->ordernum << " SYMB=" << pool[count]->symbol << endl;
		return pool[count];
	}
};

};

#endif
