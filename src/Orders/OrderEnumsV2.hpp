//============================================================================
// Name        	: OrderEnumsV2.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 17, 2023 2:11:11 PM
//============================================================================

#ifndef SRC_ORDERS_ORDERENUMSV2_HPP_
#define SRC_ORDERS_ORDERENUMSV2_HPP_
#pragma once

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

namespace KTN {
namespace ORD {

/*********** ORDER STATUS ******************/
struct KOrderStatus
{
    enum Enum : int8_t
    {
    	BANKED = 1,
    	sent_new,
		sent_mod,
		sent_cxl,
		EXPIRED,
		NEW,
		CANCELED,
		MODIFIED,
		PARTIALLY_FILLED,
		FILLED,
		REJECTED,
		CXL_REPL_REJECT,
		CXL_REJECT,
		FAILED,
		HEDGE_SENT,
		PASSOUT_SENT,
		UNKNOWN = -1
    };

    static KOrderStatus::Enum Value(string value)
	{
		if (value == "NEW")
            return Enum::NEW;
        if (value == "CANCELED")
            return Enum::CANCELED;
        if (value == "MODIFIED")
            return Enum::MODIFIED;
        if (value == "PARTIAL")
            return Enum::PARTIALLY_FILLED;
        if (value == "FILLED")
            return Enum::FILLED;
        if (value == "REJECTED")
            return Enum::REJECTED;
        if (value == "CXL_REJECT")
            return Enum::CXL_REJECT;
        if (value == "CXL_REPL_REJECT")
            return Enum::CXL_REPL_REJECT;
        if (value == "EXPIRED")
            return Enum::EXPIRED;
        if (value == "FAILED")
            return Enum::FAILED;
        if (value == "HEDGE_SENT")
            return Enum::HEDGE_SENT;
        if (value == "PASSOUT_SENT")
            return Enum::PASSOUT_SENT;
        if (value == "BANKED")
            return Enum::BANKED;
        if (value == "sent_new")
            return Enum::sent_new;
        if (value == "sent_mod")
            return Enum::sent_mod;
        if (value == "sent_cxl")
            return Enum::sent_cxl;
       
		return Enum::UNKNOWN;
	}

    static std::string toString(Enum status)
    {
        switch(status) {
        	case KOrderStatus::BANKED:
        		return "BANKED";
            case KOrderStatus::sent_new:
                return "sent_new";
            case KOrderStatus::sent_mod:
                return "sent_mod";
            case KOrderStatus::sent_cxl:
                return "sent_cxl";
            case KOrderStatus::EXPIRED:
                return "EXPIRED";
            case KOrderStatus::NEW:
                return "NEW";
            case KOrderStatus::CANCELED:
                return "CANCELED";
            case KOrderStatus::MODIFIED:
                return "MODIFIED";
            case KOrderStatus::PARTIALLY_FILLED:
                return "PARTIAL";
            case KOrderStatus::FILLED:
                return "FILLED";
            case KOrderStatus::REJECTED:
                return "REJECTED";
            case KOrderStatus::CXL_REPL_REJECT:
                return "CXL_REPL_REJECT";
            case KOrderStatus::CXL_REJECT:
                 return "CXL_REJECT";
            case KOrderStatus::FAILED:
                return "FAILED";
            case KOrderStatus::HEDGE_SENT:
			   return "HEDGE_SENT";
		   case KOrderStatus::PASSOUT_SENT:
			   return "PASSOUT_SENT";
            case KOrderStatus::UNKNOWN:
                return "UNKNOWN";
            default:
                return "INVALID";
        }
    }
};

/*********** ORDER SIDE ******************/
struct KOrderSide
{
    enum Enum : int8_t
    {
    	BUY=1,
		SELL=-1,
		UNKNOWN = 0
    };

    // Add the assignment operator
	Enum& operator=(const int8_t& value)
	{
		switch (value)
		{
			case -1:
				return *this = Enum::SELL;
			case 1:
				return *this = Enum::BUY;
			default:
				return *this = Enum::UNKNOWN;
		}
	}

    static KOrderSide::Enum Value(string side)
    {
    	if (side[0] == 'S')
    		return Enum::SELL;
    	if (side[0] == 'B')
    		return Enum::BUY;

    	return Enum::UNKNOWN;
    }

    static KOrderSide::Enum Value(int side)
	{
		if (side == -1)
			return Enum::SELL;
		if (side ==  1)
			return Enum::BUY;

		return Enum::UNKNOWN;
	}

    static KOrderSide::Enum Not(Enum side)
	{
    	switch(side)
    	{
			case KOrderSide::BUY:
				return Enum::SELL;
			case KOrderSide::SELL:
				return Enum::BUY;
			default:
				return KOrderSide::UNKNOWN;
    	}
	}

    static KOrderSide::Enum Not(string side)
	{
		if (side[0] == 'S')
			return Enum::BUY;
		if (side[0] == 'B')
			return Enum::SELL;

		return Enum::UNKNOWN;
	}


    static std::string toString(Enum side)
	{
	   switch(side) {
		   case KOrderSide::BUY:
			   return "BUY";
		   case KOrderSide::SELL:
			   return "SELL";
		   case KOrderSide::UNKNOWN:
			   return "UNKNOWN";
		   default:
			   return "INVALID";
	   }
	}

    static std::string toOrdStructString(Enum side)
	{
	   switch(side) {
		   case KOrderSide::BUY:
			   return "B";
		   case KOrderSide::SELL:
			   return "S";
		   case KOrderSide::UNKNOWN:
			   return "UNKNOWN";
		   default:
			   return "INVALID";
	   }
	}
};


/*********** ORDER TYPE ******************/
struct KOrderType
{
    enum Enum : uint8_t
    {
    	MARKET = 1,
    	LIMIT,
		MARKET_LIMIT,
		STOPLIMIT,
		STOP,
		UNKNOWN = 0
    };

    static KOrderType::Enum Value(string ordtype)
	{
		if (ordtype[0] == 'M')
			return Enum::MARKET;
		if (ordtype[0] == 'L')
			return Enum::LIMIT;
        if (ordtype[0] == 'S')
            return Enum::STOP;
        if (ordtype[0] == 'T')
            return Enum::STOPLIMIT;

		return Enum::UNKNOWN;
	}


    static std::string toString(Enum type)
	{
	  switch(type) {
		  case KOrderType::MARKET:
			  return "MARKET";
		  case KOrderType::LIMIT:
			  return "LIMIT";
		  case KOrderType::MARKET_LIMIT:
			  return "MARKET_LIMIT";
		  case KOrderType::STOPLIMIT:
			  return "STOPLIMIT";
		  case KOrderType::STOP:
			  return "STOP";
		  case KOrderType::UNKNOWN:
			  return "UNKNOWN";
		  default:
			  return "INVALID";
	  }
	}
};


/*********** TIME IN FORCE ******************/
struct KOrderTif
{
    enum Enum : int8_t
    {
    	DAY,
    	GTC,
		FAK,
        IOC,
        FOK,
		UNKNOWN = -1
    };

    static KOrderTif::Enum Value(string value)
	{
		if (value == "DAY")
            return Enum::DAY;
        if (value == "GTC")
            return Enum::GTC;
        if (value == "FAK")
            return Enum::FAK;
        if (value == "IOC")
            return Enum::IOC;
        if (value == "FOK")
            return Enum::FOK;
		return Enum::UNKNOWN;
	}

    static std::string toString(Enum tif)
	{
		switch(tif) {
			case KOrderTif::DAY:
				return "DAY";
			case KOrderTif::GTC:
				return "GTC";
			case KOrderTif::FAK:
				return "FAK";
            case KOrderTif::IOC:
                return "IOC";
            case KOrderTif::FOK:
                return "FOK";
			case KOrderTif::UNKNOWN:
				return "UNKNOWN";
			default:
				return "INVALID";
		}
	}
};


/*********** FILLTYPE ******************/
struct KOrderFillType
{
    enum Enum : uint8_t
    {
    	OUTRIGHT_FILL =1,
    	SPREAD_LEG_FILL,
		SPREAD_FILL,
		UNKNOWN = 0
    };

    static KOrderFillType::Enum Value(string value)
	{
		if (value == "OUTRIGHT_FILL")
            return Enum::OUTRIGHT_FILL;
        if (value == "SPREAD_LEG_FILL")
            return Enum::SPREAD_LEG_FILL;
        if (value == "SPREAD_FILL")
            return Enum::SPREAD_FILL;
		return Enum::UNKNOWN;
	}

    static int toInt(Enum fillType)
    {
        switch(fillType) {
            case KOrderFillType::OUTRIGHT_FILL:
                return 1;
            case KOrderFillType::SPREAD_LEG_FILL:
                return 2;
            case KOrderFillType::SPREAD_FILL:
                return 3;
            case KOrderFillType::UNKNOWN:
                return 0;
            default:
                return -1;
        }
    }

    static std::string toString(Enum fillType)
    {
        switch(fillType) {
            case KOrderFillType::OUTRIGHT_FILL:
                return "OUTRIGHT_FILL";
            case KOrderFillType::SPREAD_LEG_FILL:
                return "SPREAD_LEG_FILL";
            case KOrderFillType::SPREAD_FILL:
                return "SPREAD_FILL";
            case KOrderFillType::UNKNOWN:
                return "UNKNOWN";
            default:
                return "INVALID";
        }
    }
};

/*********** PRODUCT TYPE ******************/
struct KOrderProdType
{
    enum Enum : uint8_t
    {
    	FUTURE = 1,
		SPREAD,
		OPTION,
		EQT,
		UNKNOWN = 0
    };

    static KOrderProdType::Enum Value(string prodtype)
	{
    	if (prodtype[0] == 'O')
    		return Enum::OPTION;
    	if (prodtype[0] == 'F' || prodtype[0] == 'f')
			return Enum::FUTURE;
		if (prodtype[0] == 'M' || prodtype[0] == 's')
			return Enum::SPREAD;
		if (prodtype[0] == 'E' || prodtype[0] == 'e')
			return Enum::EQT;

		return Enum::UNKNOWN;
	}


    static std::string toString(Enum productType)
    {
        switch(productType) {
            case KOrderProdType::FUTURE:
                return "FUTURE";
            case KOrderProdType::SPREAD:
                return "SPREAD";
            case KOrderProdType::OPTION:
                return "OPTION";
            case KOrderProdType::EQT:
                return "EQT";
            case KOrderProdType::UNKNOWN:
                return "UNKNOWN";
            default:
                return "INVALID";
        }
    }
};


/*********** PUTCALL TYPE ******************/
struct KOrderPutCall
{
    enum Enum : int8_t
    {
    	NA = 0,
    	CALL = 1,
		PUT = 2,
		UNKNOWN = -1
    };

    static KOrderPutCall::Enum Value(string value)
    {
        if (value == "CALL")
            return Enum::CALL;
        if (value == "PUT")
            return Enum::PUT;
        return Enum::UNKNOWN;
    }


    static std::string toString(Enum putCall)
    {
        switch(putCall) {
            case KOrderPutCall::CALL:
                return "CALL";
            case KOrderPutCall::PUT:
                return "PUT";
            case KOrderPutCall::NA:
                 return "NA";
            case KOrderPutCall::UNKNOWN:
                return "UNKNOWN";
            default:
                return "INVALID";
        }
    }
};

/*********** EXCHANGE TYPE ******************/
struct KOrderExchange
{
    enum Enum : int8_t
    {
        OSE = 12,    // Osaka Exchange (J-GATE)
        EDGAX = 11,  // CBOE EDGA Top
        EDGAB = 10,  // CBOE EDGA Book
        BZXB = 9,    // BATS BZX
        BYXB = 8,    // BATS BYX
        ARCP = 7,    // NYSE Arca
        INET = 6,    // NASDAQ TotalView
        CFE = 2,
    	CME = 1,
		OTHER = 0,
		UNKNOWN = -1
    };

    static KOrderExchange::Enum Value(string value)
	{
		if (value == "CME")
			return Enum::CME;
		if (value == "CFE")
            return Enum::CFE;
		if (value == "OSE")
            return Enum::OSE;
		if (value == "INET")
            return Enum::INET;
        if (value == "ARCP")
            return Enum::ARCP;
        if (value == "BYXB")
            return Enum::BYXB;
        if (value == "BZXB")
            return Enum::BZXB;
        if (value == "EDGAB")
            return Enum::EDGAB;
        if (value == "EDGAX")
            return Enum::EDGAX;
		if (value == "OTHER")
            return Enum::OTHER;
        if (value == "UNKNOWN")
            return Enum::UNKNOWN;

		return Enum::UNKNOWN;
	}

    static std::string toString(Enum exchange)
    {
        switch(exchange) {
            case KOrderExchange::CME:
                return "CME";
            case KOrderExchange::CFE:
                return "CFE";
            case KOrderExchange::INET:
                return "INET";
            case KOrderExchange::ARCP:
                return "ARCP";
            case KOrderExchange::BYXB:
                return "BYXB";
            case KOrderExchange::BZXB:
                return "BZXB";
            case KOrderExchange::EDGAB:
                return "EDGAB";
            case KOrderExchange::EDGAX:
                return "EDGAX";
            case KOrderExchange::OSE:
                return "OSE";
            case KOrderExchange::OTHER:
                return "OTHER";
            case KOrderExchange::UNKNOWN:
                return "UNKNOWN";
            default:
                return "INVALID";
        }
    }

	static bool isEquityVenue(Enum exchange)
	{
		switch(exchange) {
			case KOrderExchange::INET:
			case KOrderExchange::ARCP:
			case KOrderExchange::BYXB:
			case KOrderExchange::BZXB:
			case KOrderExchange::EDGAB:
			case KOrderExchange::EDGAX:
				return true;
			default:
				return false;
		}
	}
};

/*********** ORDER ACTION ******************/
struct KOrderAction
{

    enum Enum : uint8_t
    {
    	ACTION_NEW = 1,
		ACTION_MOD = 2,
		ACTION_CXL = 3,
		ACTION_CXL_ALL = 55,
		ACTION_GETORDERS = 56,
		ACTION_CXL_ALL_SEG =57,
		ACTION_REJ = 73,
		ACTION_CXL_REJ = 74,
		ACTION_CXL_REPL_REJ = 75,
//		ACK_NEW = 101,
//		ACK_MOD = 102,
//		ACK_CXL = 103,
//		ACK_REJ = 104,
//		ACK_CXLALL = 105,
//		ACK_BANK = 106,
		UNKNOWN = 0
    };

    static KOrderAction::Enum Value(string action)
	{
		if (action[0] == 'N' || action[0] == 'n')
			return Enum::ACTION_NEW;
		if (action[0] == 'M' || action[0] == 'm')
			return Enum::ACTION_MOD;
		if (action[0] == 'C' || action[0] == 'c')
			return Enum::ACTION_CXL;

		return Enum::UNKNOWN;
	}

    static std::string toString(Enum action)
    {
        switch(action) {
            case KOrderAction::ACTION_NEW:
                return "ACTION_NEW";
            case KOrderAction::ACTION_MOD:
                return "ACTION_MOD";
            case KOrderAction::ACTION_CXL:
                return "ACTION_CXL";
            case KOrderAction::ACTION_CXL_ALL:
                return "ACTION_CXL_ALL";
            case KOrderAction::ACTION_GETORDERS:
                return "ACTION_GETORDERS";
            case KOrderAction::ACTION_CXL_ALL_SEG:
                return "ACTION_CXL_ALL_SEG";
            case KOrderAction::UNKNOWN:
                return "UNKNOWN";
            default:
                return "INVALID";
        }
    }
};

/*********** IN FLIGHT STATUS  ******************/
struct KOrderState
{

    enum Enum : uint8_t
    {
    	INFLIGHT = 1,
		WORKING = 2,
		COMPLETE = 3,
        STATUS_UPDATE = 4,
		UNKNOWN = 0
    };

    static KOrderState::Enum Value(string state)
	{
		if (state == "INFLIGHT")
			return Enum::INFLIGHT;
		if (state == "WORKING")
            return Enum::WORKING;
        if (state == "STATUS_UPDATE")
            return Enum::STATUS_UPDATE;
		if (state == "COMPLETE")
            return Enum::COMPLETE;
			
		return Enum::UNKNOWN;
	}

    static std::string toString(uint8_t state)
    {
        switch(state) {
            case KOrderState::INFLIGHT:
                return "INFLIGHT";
            case KOrderState::WORKING:
                return "WORKING";
            case KOrderState::COMPLETE:
                return "COMPLETE";
            case KOrderState::STATUS_UPDATE:
                return "STATUS_UPDATE";
            case KOrderState::UNKNOWN:
                return "UNKNOWN";
            default:
                return "INVALID";
        }
    }
};


/*********** ORDER HEDGE OR LEG  ******************/
struct KOrderAlgoTrigger
{

    enum Enum : uint8_t
    {
    	UNKNOWN = 0,
    	ALGO_LEG ,
		ALGO_HEDGE,
		ALGO_CXL,
		ALGO_PROFIT,
		ALGO_STOPLOSS,
		ALGO_TIMEREXP,
		ALGO_GOTO_MKT,
		ALGO_SQZ,
		ALGO_PASSOUT,
		ALGO_PUKE,
		ALGO_LARGE_TRADE_KILL,
		ALGO_SQZ_RATIO,
		ALGO_SQZ_QTY,
		ALGO_SQZ_LARGETRD,
		ALGO_SQZ_PRICEJUMP,
		ORDERBOOK,
		STOP_ELECTED,
		TASBOOK,
		ORPHAN
    };

    static KOrderAlgoTrigger::Enum Value(string state)
	{
		if (state == "ALGO_LEG")
            return Enum::ALGO_LEG;
        if (state == "ALGO_HEDGE")
            return Enum::ALGO_HEDGE;
        if (state == "ALGO_CXL")
            return Enum::ALGO_CXL;
        if (state == "ALGO_PROFIT")
            return Enum::ALGO_PROFIT;
        if (state == "ALGO_STOPLOSS")
            return Enum::ALGO_STOPLOSS;
        if (state == "ALGO_TIMEREXP")
            return Enum::ALGO_TIMEREXP;
        if (state == "ALGO_GOTO_MKT")
            return Enum::ALGO_GOTO_MKT;
        if (state == "ALGO_SQZ")
            return Enum::ALGO_SQZ;
        if (state == "ALGO_PASSOUT")
            return Enum::ALGO_PASSOUT;
        if (state == "ALGO_PUKE")
            return Enum::ALGO_PUKE;
        if (state == "ALGO_LARGE_TRADE_KILL")
            return Enum::ALGO_LARGE_TRADE_KILL;
        if (state == "ALGO_SQZ_RATIO")
            return Enum::ALGO_SQZ_RATIO;
        if (state == "ALGO_SQZ_QTY")
            return Enum::ALGO_SQZ_QTY;
        if (state == "ALGO_SQZ_LARGETRD")
            return Enum::ALGO_SQZ_LARGETRD;
        if (state == "ALGO_SQZ_PRICEJUMP")
            return Enum::ALGO_SQZ_PRICEJUMP;
        if (state == "ORDERBOOK")
            return Enum::ORDERBOOK;
        if (state == "TASBOOK")
            return Enum::TASBOOK;
        if (state == "ORPHAN")
            return Enum::ORPHAN;
        if (state == "STOP_ELECTED")
            return Enum::STOP_ELECTED;
			
		return Enum::UNKNOWN;
	}

    static std::string toString(Enum trigger)
    {
        switch(trigger) {
            case KOrderAlgoTrigger::ALGO_LEG:
                return "ALGO_LEG";

            case KOrderAlgoTrigger::ALGO_HEDGE:
                return "ALGO_HEDGE";

            case KOrderAlgoTrigger::ALGO_CXL:
                return "ALGO_CXL";

            case KOrderAlgoTrigger::ALGO_PROFIT:
                return "ALGO_PROFIT";

            case KOrderAlgoTrigger::ALGO_STOPLOSS:
                return "ALGO_STOPLOSS";

            case KOrderAlgoTrigger::ALGO_TIMEREXP:
                return "ALGO_TIMEREXP";

            case KOrderAlgoTrigger::ALGO_GOTO_MKT:
                 return "ALGO_GOTO_MKT";

            case KOrderAlgoTrigger::ALGO_SQZ:
                 return "ALGO_SZQ";

            case KOrderAlgoTrigger::ALGO_PASSOUT:
                 return "ALGO_PASSOUT";

            case KOrderAlgoTrigger::ALGO_PUKE:
                 return "ALGO_PUKE";

            case KOrderAlgoTrigger::ALGO_LARGE_TRADE_KILL:
                  return "ALGO_LARGE_TRADE_KILL";

            case KOrderAlgoTrigger::ALGO_SQZ_RATIO:
                  return "ALGO_SQZ_RATIO";

            case KOrderAlgoTrigger::ALGO_SQZ_QTY:
                  return "ALGO_SQZ_QTY";

            case KOrderAlgoTrigger::ALGO_SQZ_LARGETRD:
                  return "ALGO_SQZ_LARGETRD";

            case KOrderAlgoTrigger::ALGO_SQZ_PRICEJUMP:
                  return "ALGO_SQZ_PRICEJUMP";

            case KOrderAlgoTrigger::ORDERBOOK:
                return "ORDERBOOK";

            case KOrderAlgoTrigger::TASBOOK:
                return "TASBOOK";

            case KOrderAlgoTrigger::UNKNOWN:
                return "UNKNOWN";

            case KOrderAlgoTrigger::ORPHAN:
                 return "ORPHAN";

            default:
                return "INVALID";
        }
    }
};

/*********** OPEN CLOSE ENUM  ******************/
struct KOrderOpenCloseInd
{

    enum Enum : uint8_t
    {
    	NONE = 0,
        OPEN = 1,
        CLOSE = 2,
    };

    static KOrderOpenCloseInd::Enum Value(string ind)
	{
		if (ind == "OPEN")
			return Enum::OPEN;
		if (ind == "CLOSE")
            return Enum::CLOSE;

		return Enum::NONE;
	}

    static std::string toString(uint8_t ind)
    {
        switch(ind) {
            case KOrderOpenCloseInd::OPEN:
                return "OPEN";
            case KOrderOpenCloseInd::CLOSE:
                return "CLOSE";
            case KOrderOpenCloseInd::NONE:
                return "NONE";
            default:
                return "NONE";
        }
    }
};

} }

#endif /* SRC_ORDERS_ORDERENUMSV2_HPP_ */
