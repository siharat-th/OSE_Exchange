//============================================================================
// Name        	: ExchEnums.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 24, 2023 8:51:59 AM
//============================================================================

#ifndef SRC_EXCHANGEHANDLERS2_EXCHENUMS_HPP_
#define SRC_EXCHANGEHANDLERS2_EXCHENUMS_HPP_
#pragma once

#include <iostream>
#include <string>
#include <sstream>



using namespace std;

namespace KTN {

struct Exch
{
    enum Enum : uint8_t
    {
    	NONE=0,
    	FIX42,
		FIX44,
		CME_iL3,
		CME_GT4,
		CME_GT5,
		ICE,
		OSE_OMNET
    };

    static std::string toString(Enum exch)
    {
        switch(exch) {
        	case Exch::NONE:
        		return "NONE";
        	case Exch::FIX42:
        		return "FIX42";
        	case Exch::FIX44:
        	     return "FIX44";
        	case Exch::CME_iL3:
        	     return "CME_iL3";
        	case Exch::CME_GT5:
        	     return "CME_GT5";
        	case Exch::ICE:
        	     return "ICE";
        	case Exch::OSE_OMNET:
        	     return "OSE_OMNET";
            default:
                return "INVALID";
        }
    }

    static Enum toEnum(int value)
    {
        switch (value)
        {
        case 0:
            return Exch::NONE;
        case 1:
            return Exch::FIX42;
        case 2:
            return Exch::FIX44;
        case 3:
            return Exch::CME_iL3;
        case 4:
            return Exch::CME_GT5;
        case 5:
            return Exch::ICE;
        case 6:
            return Exch::OSE_OMNET;
        default:
            throw std::invalid_argument("Invalid integer value");
        }
    }

};



}

#endif /* SRC_EXCHANGEHANDLERS2_EXCHENUMS_HPP_ */
