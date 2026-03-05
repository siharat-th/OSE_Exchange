//============================================================================
// Name        	: algo_enums.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 30, 2023 9:03:53 AM
//============================================================================

#ifndef SRC_ALGOS_ALGO_ENUMS_HPP_
#define SRC_ALGOS_ALGO_ENUMS_HPP_

#pragma once

namespace KTN {
namespace ALGO {

struct GoodToGoResult
{
	enum Enum
	{
		OK = 1,
		NotEnabled = -1,
		CannotTrade = -2,
		NeedAllLegData = -3,
		InvalidQuoteState = -4
	};

	static std::string toString(Enum state)
	{
		switch(state) {
			case GoodToGoResult::OK:
				return "OK(1)";

			case GoodToGoResult::CannotTrade:
				return "CanTrade=False(-2)";

			case GoodToGoResult::NotEnabled:
				return "Not Enabled(-1)";

			case GoodToGoResult::NeedAllLegData:
				return "Not All Legs Have Data(-3)";

			case GoodToGoResult::InvalidQuoteState:
				return "InvalidQuoteState(-4)";

			default:
				return "INVALID";
		}
	}
};

struct QuoteState
{
    enum Enum : uint8_t
    {
    	Disabled,
    	Calculating,
    	ReadyToRefresh,
    	TimerTick,
    	ModifyInFlight,
    	SentOrders,
    	GotOrderAcks,
    	SentCancels,
    	GotCancelAcks,
    	ReadyToSendOrders,
    	GotPrimaryLegFill,
    	ReadyToHedge,
    	MaxExposure,
    	MonitorPL,
    	MonitorPrimaryPL,
    	CoveredPosition
    };

    static std::string toString(Enum state)
    {
        switch(state) {

        case QuoteState::Disabled:
                return "Disabled";
        	case QuoteState::Calculating:
                return "Calculating";

            case QuoteState::ReadyToRefresh:
                return "ReadyToRefresh";

            case QuoteState::TimerTick:
                return "TimerTick";

            case QuoteState::ModifyInFlight:
                return "ModifyInFlight";

            case QuoteState::SentOrders:
                return "SentOrders";

            case QuoteState::GotOrderAcks:
                return "GotOrderAcks";

            case QuoteState::SentCancels:
                return "SentCancels";

            case QuoteState::GotCancelAcks:
                return "GotCancelAcks";

            case QuoteState::ReadyToSendOrders:
                return "ReadyToSendOrders";

            case QuoteState::GotPrimaryLegFill:
                return "GotPrimaryLegFill";

            case QuoteState::ReadyToHedge:
                return "ReadyToHedge";

            case QuoteState::MaxExposure:
                return "MaxExposure";

            case QuoteState::MonitorPL:
                return "MonitorPL";

            case QuoteState::MonitorPrimaryPL:
                return "MonitorPrimaryPL";

            case QuoteState::CoveredPosition:
                return "CoveredPosition";

            default:
                return "INVALID";
        }
    }
};

struct HedgeState
{
    enum Enum : uint8_t
    {
    	NoHedges,
    	ActiveHedges
    };

    static std::string toString(Enum state)
    {
        switch(state) {
        case HedgeState::NoHedges:
            return "Disabled";

		case HedgeState::ActiveHedges:
			return "ActiveHedges";

		default:
			return "INVALID";
        }
    }
};

enum QuoteSide
{
	BUY = 1,
	SELL = -1
};



} }

#endif /* SRC_ALGOS_ALGO_ENUMS_HPP_ */
