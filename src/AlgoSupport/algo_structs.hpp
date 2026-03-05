/*
 * algostructs.hpp
 *
 *  Created on: Apr 24, 2021
 *      Author: sgaer
 */

#ifndef ALGOSTRUCTS_HPP_
#define ALGOSTRUCTS_HPP_

#pragma once
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

#include <tbb/concurrent_unordered_map.h>


#include <KT01/DataStructures/UserStructs.hpp>
#include <Orders/Order.hpp>
#include <Orders/OrderUtils.hpp>
#include <Orders/OrderEnumsV2.hpp>
using namespace KTN;
using namespace KTN::ORD;


//akl types
#include <akl/Price.hpp>


using namespace std;

/**
 * @brief Enumeration for the fill type.
 */
enum FillType
{
	NONE,
	KEY,
	HDG,
	CVR,
	PRIM_HDG,
	STOP
};

/**
 * @brief Structure representing the header of an algorithm.
 */
struct AlgoHeader
{
	string archetype; /**< The archetype of the algorithm. */
	string guid; /**< The GUID of the algorithm. */
	int enabled; /**< Flag indicating if the algorithm is enabled. */
	bool reset_pos; /**< Flag indicating if the position should be reset. */
};


/**
 * @brief Structure representing an instrument.
 */
struct instrument
{
	// Larger data types first
	double totalexec; /**< The total executed quantity. */
	double position; /**< The current position. */
	double effective; /**< The effective price. */
	double pricemult; /**< The price multiplier. */
	double spreadmult; /**< The spread multiplier. */
	double minsize; /**< The minimum size. */
	double mintick; /**< The minimum tick. */
	double tickvalue; /**< The tick value. */

	uint64_t lastBuyId; /**< The ID of the last buy order. */
	uint64_t lastSellId; /**< The ID of the last sell order. */

	// Then integer types
	int callbackid; /**< The callback ID. */
	int index; /**< The index. */
	int mdindex; /**< The market data index. */
	int secid; /**< The security ID. */
	int mktsegid; /**< The market segment ID. */
	int iside; /**< The side. */
	int id; /**< The ID. */
	int tickprecision; /**< The tick precision. */
	int lastBid; /**< The last bid price. */
	int lastAsk; /**< The last ask price. */
	int lastSentBuyPrice; /**< The last sent buy price. */
	int lastSentSellPrice; /**< The last sent sell price. */
	int row; /**< The row. */
	int col; /**< The column. */


	// Then smaller types
	uint8_t connindex; /**< The connection index. */

	// Then boolean flags
	bool gotdata; /**< Flag indicating if data is available. */
	bool ishedge; /**< Flag indicating if it is a hedge. */
	bool isquote; /**< Flag indicating if it is a quote. */
	bool squeeze; /**< Flag indicating if it is a squeeze. */
	bool activeQuoting; /**< Flag indicating if quoting is active. */
	bool bidChanged; /**< Flag indicating if the bid has changed. */
	bool askChanged; /**< Flag indicating if the ask has changed. */
	bool iscme; /**< Flag indicating if it is a CME instrument. */
	bool istas; /**< Flag indicating if it is a TAS instrument. */

	// Enums
	KOrderTif::Enum tif; /**< The time in force. */
	KOrderProdType::Enum prodtype; /**< The product type. */
	KOrderState::Enum buyState; /**< The buy order state. */
	KOrderState::Enum sellState; /**< The sell order state. */
	KOrderExchange::Enum exchEnum; /**< The exchange. */

	// Then strings
	std::string label; /**< The label. */
	std::string exch; /**< The exchange. */
	std::string product; /**< The product. */
	std::string symbol; /**< The symbol. */


	/**
  * @brief Prints the instrument information.
  * @return The string representation of the instrument.
  */
	string Print()
	{
		string active = (activeQuoting) ? "ACTIVE" : "NOT_ACTIVE";

		ostringstream oss;
		oss << exch << "/" << symbol << "/"	<< prodtype << " PM=" << pricemult
				<< " SM=" << spreadmult;
		return oss.str();
	}
};

/**
 * @brief Structure representing a combination path.
 */
struct combopath
{
	instrument combo; /**< The combination instrument. */
	vector<instrument> legs; /**< The vector of leg instruments. */
};

/**
 * @brief Structure representing an exchange instrument.
 */
struct exchinst
{
	string exch; /**< The exchange. */
	string prodtype; /**< The product type. */
	instrument inst; /**< The instrument. */
};

// /**
//  * @brief Structure representing algorithm market data.
//  */
// struct alignas(64)algomd
// {
// 	uint8_t lastagressor; /**< The last aggressor. */
// 	uint16_t index; /**< The index. */

//     akl::Price bid; /**< The bid price. */
//     akl::Price ask; /**< The ask price. */
//     int32_t bidsize; /**< The bid size. */
//     int32_t asksize; /**< The ask size. */

//     akl::Price ibid; /**< The implied bid price. */
//     akl::Price iask; /**< The implied ask price. */
//     int32_t ibidsize; /**< The implied bid size. */
//     int32_t iasksize; /**< The implied ask size. */

//     akl::Price high; /**< The high price. */
//     akl::Price low; /**< The low price. */

//     int32_t volume; /**< The volume. */
//     akl::Price last; /**< The last price. */
//     int32_t lastsize; /**< The last size. */


//     int32_t secid; /**< The security ID. */

//     bool hasImpliedEnabled; /**< Flag indicating if implied is enabled. */
//     bool isBidImplied; /**< Flag indicating if the bid is implied. */
//     bool isAskImplied; /**< Flag indicating if the ask is implied. */
//     bool isVolume; /**< Flag indicating if it is volume. */
//    // bool isTrade;
// };

/**
 * @brief Structure representing an atomic quote.
 */
struct atomicquote
{
	akl::Price price; /**< The price. */
	int qty; /**< The quantity. */
};

/**
 * @brief Structure representing a price level.
 */
struct alignas(64) pxlvl
{
	uint8_t level; /**< The level. */
	akl::Price price; /**< The price. */
	int32_t size; /**< The size. */
	int32_t count; /**< The count. */
};

// /**
//  * @brief Structure representing book market data.
//  */
// struct alignas(64) bookmd
// {
// 	uint16_t index; /**< The index. */
// 	int32_t secid; /**< The security ID. */

// 	pxlvl bids[10]; /**< The array of bid price levels. */
// 	pxlvl asks[10]; /**< The array of ask price levels. */

// 	pxlvl ibids[10]; /**< The array of implied bid price levels. */
// 	pxlvl iasks[10]; /**< The array of implied ask price levels. */

// //	int32_t bid[10];
// //	int32_t bidsize[10];
// //	int32_t bidcnt[10];
// //
// //	int32_t ask[10];
// //	int32_t asksize[10];
// //	int32_t askcnt[10];
// };




/**
 * @brief Structure representing an order level.
 */
struct ordlvl {
	uint8_t refID; /**< The reference ID. */
    uint64_t orderID; /**< The order ID. */
    uint64_t orderPriority; /**< The order priority. */
    int64_t price; /**< The price. */
    int32_t displayQty; /**< The display quantity. */
    int32_t securityID; /**< The security ID. */
    int updateAction; /**< The update action. */
    char entryType; /**< The entry type. */
    string side; /**< The side. */
};

/**
 * @brief Structure representing market-by-order market data.
 */
struct alignas(64) mbomd
{
	uint16_t index; /**< The index. */
	int32_t secid; /**< The security ID. */

	pxlvl bids[10]; /**< The array of bid price levels. */
	pxlvl asks[10]; /**< The array of ask price levels. */
};

/**
 * @brief Structure representing algorithm metadata.
 */
struct algometa
{
	std::string Strategy; /**< The strategy. */
	int StratId; /**< The strategy ID. */
	std::string TemplateName; /**< The template name. */

	int Enabled; /**< Flag indicating if the algorithm is enabled. */
	std::string UniqueId; /**< The unique ID. */
	std::string Source; /**< The source. */

	std::string Tag; /**< The tag. */
	std::string HedgeTag; /**< The hedge tag. */
	std::string CoverTag; /**< The cover tag. */
	std::string PrimHdgTag; /**< The primary hedge tag. */
	bool CanHedge; /**< Flag indicating if hedging is allowed. */

	//std::string Channel;
	//std::string LeanChannel;

	//optional/convenience
	std::string Symbol; /**< The symbol. */
	std::string Exch; /**< The exchange. */

	double Position; /**< The position. */

	UserInfo userinfo; /**< The user information. */

//	std::vector<std::string> Channels;
};

/**
 * @brief Structure representing an algorithm leg.
 */
struct algoleg
{
	int LegId; /**< The leg ID. */
	std::string Symbol; /**< The symbol. */
	std::string Side; /**< The side. */
	int iSide; /**< The integer side. */

	double PriceMult; /**< The price multiplier. */
	double QtyMult; /**< The quantity multiplier. */

	//int Channel;


	/**
  * @brief Converts the side string to the integer side.
  */
	void sideint()
	{
		if (Side.compare("B") == 0 || Side.compare("BUY") == 0)
			iSide = 1;
		else
			iSide = -1;
	}
};

/**
 * @brief Structure representing a plain instrument.
 */
struct algoinstr
{
	std::string Symbol; /**< The symbol. */
	std::string Exch; /**< The exchange. */
	int Channel; /**< The channel. */
};

/**
 * @brief Structure representing the state of a side.
 */
struct sidestate
{
	akl::Price lastPrice; /**< The last price. */
	int lastSize; /**< The last size. */
	bool inFlight; /**< Flag indicating if it is in flight. */
	std::string lastOrdernumSent; /**< The last order number sent. */
	std::string lastOrdernumRecd; /**< The last order number received. */
};

/**
 * @brief Structure representing the types of algorithm statistics.
 */
struct AlgoStats
{
	enum Enum
	{
		GENERIC = 0, /**< Generic statistics. */
		MESSAGING = 1, /**< Messaging statistics. */
		ALERT = 2 /**< Alert statistics. */
	};

	/**
  * @brief Converts the statistics type to string.
  * @param statstype The statistics type.
  * @return The string representation of the statistics type.
  */
	static std::string toString(Enum statstype)
	{
		switch(statstype) {
			case AlgoStats::GENERIC:
				return "GENERIC";
			case AlgoStats::MESSAGING:
				return "MESSAGING";
			case AlgoStats::ALERT:
				return "ALERT";
			default:
				return "INVALID";
		}
	}
};

/**
 * @brief Class representing algorithm statistics.
 */
class stats
{
public:
protected:
	AlgoStats::Enum StatType; /**< The statistics type. */

	/**
  * @brief Gets the type of the statistics.
  * @return The type of the statistics.
  */
	inline string GetType()
	{
		return AlgoStats::toString(StatType);
	}
};

/**
 * @brief Structure representing messaging statistics.
 */
struct msgstats
{
public:
	msgstats():sent(0),news(0),mods(0),volume(0)
	{

	}

	string product; /**< The product. */
	uint64_t sent; /**< The number of messages sent. */
	uint64_t news; /**< The number of news messages. */
	uint64_t mods; /**< The number of modified messages. */
	int volume; /**< The volume. */

	/**
  * @brief Calculates the volume ratio.
  * @return The volume ratio.
  */
	double ratio()
	{
		if (sent == 0)
			return 0;

		return (double)volume/sent;
	}

	/**
  * @brief Converts the messaging statistics to string.
  * @return The string representation of the messaging statistics.
  */
	string toString()
	{
		ostringstream oss;
		oss << product << ": SENT=" << sent << " VOL=" << volume
				<< " RATIO=" << std::setprecision(2) << ratio() << ":1";
		return oss.str();
	}
};

/**
 * @brief Class representing generic statistics.
 */
class genericstats : public stats
{
public:
	genericstats()// : StatType(AlgoStats::Enum::GENERIC)
	{}

	bool enabled; /**< Flag indicating if the statistics is enabled. */
	std::string name; /**< The name. */
	std::string templatename; /**< The template name. */
	std::string uniqueid; /**< The unique ID. */

	std::string source; /**< The source. */
	std::string exch; /**< The exchange. */
	std::string symbol; /**< The symbol. */

	std::string msgtype; /**< The message type. */
	std::string text; /**< The text. */

	double position; /**< The position. */
	double totalexec; /**< The total executed quantity. */

	uint64_t ssboe; /**< The SSBOE. */
	string updatetime; /**< The update time. */

};


/**
 * @brief Structure representing the prices of a leg.
 */
struct LegPrices
{
	akl::Price bid; /**< The bid price. */
	akl::Price ask; /**< The ask price. */
};

/**
 * @brief Structure representing the weighted average calculation.
 */
struct WavgCalc
{
	double wavg; /**< The weighted average. */
	double wsum; /**< The weighted sum. */
	int volume; /**< The volume. */
	int count; /**< The count. */
	int row; /**< The row. */
	int col; /**< The column. */
	string symb; /**< The symbol. */
	double stpr; /**< The starting price. */

	/**
  * @brief Calculates the weighted average based on the price and size.
  * @param price The price.
  * @param size The size.
  */
	void calculate(int32_t price, int32_t size)
	{
		wsum += (double)price * (double)size;
		volume += (double)size;
		wavg = wsum / volume;
		count ++;
	}
};

/**
 * @brief Structure representing the edge information.
 */
struct EdgeStruct
{
	bool istas; /**< Flag indicating if it is a TAS instrument. */
	bool exists; /**< Flag indicating if it exists. */
	uint8_t index; /**< The index. */
	uint8_t hdgindex; /**< The hedge index. */

	LegPrices prices; /**< The leg prices. */
};

/**
 * @brief Structure representing the weighted average edge information.
 */
struct WavgEdgeStruct
{
	bool istas; /**< Flag indicating if it is a TAS instrument. */
	bool exists; /**< Flag indicating if it exists. */
	uint8_t index; /**< The index. */
	uint8_t hdgindex; /**< The hedge index. */

	WavgCalc calc; /**< The weighted average calculation. */
	LegPrices prices; /**< The leg prices. */
};

#endif /* ALGOSTRUCTS_HPP_ */
