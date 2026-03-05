//============================================================================
// Name        	: Mdp3MessagePrinter.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Aug 24, 2023 12:54:09 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_MDP3MESSAGEPRINTER_HPP_
#define SRC_EXCHSUPPORT_CME_MDP3MESSAGEPRINTER_HPP_

#include <string>
#include <stdint.h>
#include <stdio.h>
# include <sys/types.h>
# include <iostream>
# include <vector>
# include <map>
# include <inttypes.h>

#include <exchsupport/cme/mdp3/Mdp3Messages.hpp>

using namespace std;

namespace KTN{
namespace Mdp3{

class Mdp3MessagePrinter
{
public:
	static string PrintStatus(EnumSecurityTradingStatus value)
	{

		switch (value) {
			case TradingHalt:
				return "TradingHalt";
			case Close:
				return "Close";
			case NewPriceIndication:
				return "NewPriceIndication";
			case ReadyToTrade:
				return "ReadyToTrade";
			case NotAvailableForTrading:
				return "NotAvailableForTrading";
			case UnknownOrInvalid:
				return "UnknownOrInvalid";
			case PreOpen:
				return "PreOpen";
			case PreCross:
				return "PreCross";
			case Cross:
				return "Cross";
			case PostClose:
				return "PostClose";
			case NoChange:
				return "NoChange";
			case PrivateWorkup:
				return "PrivateWorkup";
			case PublicWorkup:
				return "PublicWorkup";
			default:
				return "Unknown";
		}
	}

	static string PrintUpdateAction(EnumMDUpdateAction value){
		switch(value)
		{
		case MdNew:
			return "NEW";
		case MdChange:
			return "CHANGE";
		case MdDelete:
			return "DELETE";
		case MdDeleteThru:
			return "DEL_THRU";
		case MdDeleteFrom:
			return "DEL_FROM";
		case MdOverlay:
			return "OVERLAY";
		default:
			return "UNKNOWN";
		};
	}
};


//end ns:
} }

#endif /* SRC_EXCHSUPPORT_CME_MDP3MESSAGEPRINTER_HPP_ */
