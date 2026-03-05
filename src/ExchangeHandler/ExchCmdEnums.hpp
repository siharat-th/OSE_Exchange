//============================================================================
// Name        	: ExchCommands.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Jan 14, 2024 8:52:29 AM
//============================================================================

#ifndef SRC_EXCHANGEHANDLERS2_EXCHCMDENUMS_HPP_
#define SRC_EXCHANGEHANDLERS2_EXCHCMDENUMS_HPP_

#pragma once

#include <iostream>
#include <string>
#include <sstream>

#include <KT01/Core/Macro.hpp>

using namespace std;

struct ExchCmd
{
	enum Enum : int
	{
		MENU_CHOICE = 1,
		SESS_STATUS = 92,
		PARTY_DET_REQ = 93,
		MASS_ORDER_STATUS_REQ = 94,
		MEASURE = 95,
		PARTY_LIST_REQ = 96,
		RETRANS_REQ = 97,

		PRINT_LATENCIES = 102,
		PRINT_MSG_RATIOS = 103,
		CLEAR = 104,
		STOP = 105,

		SEQ_NUM_CHANGE = 199,
		TCP_RECONNECT = 210,
		RESET_RECONNECT_TRACKING = 211,

		CXL_ALL_SEGS = 201,
		CXL_ALL_SYMB = 202,
		CXL_ALL_INSTRUCTIONS = 203,

		ORD_SEND = 300,
		ORD_SEND_WITH_HEDGE = 301,
		ORD_SEND_HDG_INSTRUCT = 302,

		RISK_RESET = 400,

		ALGO_LAUNCH = 500,
		ALGO_PARAMS,
		ALGO_ORDER,
		ALGO_ENGINE,
		ALGO_RESET_POS,
		ALGO_CXL_ALL,
		ALGO_PRINT,
		ALGO_STOP,
		ALGO_MESSAGE,
		ALGO_KILLSWITCH,

		TERMINATE = 999,
	};

	static std::string toString(ExchCmd::Enum cmd)
	{
	    switch (cmd) {
	    case Enum::SESS_STATUS:
	        return "SESS_STATUS";
	    case Enum::PARTY_DET_REQ:
	        return "PARTY_DET_REQ";
	    case Enum::MASS_ORDER_STATUS_REQ:
	    	return "MASS_ORDER_STATUS_REQ";
	    case Enum::PARTY_LIST_REQ:
	    	 return "PARTY_LIST_REQ";
	    case Enum::RETRANS_REQ:
	    	 return "RETRANS_REQ";
	    case Enum::CXL_ALL_SEGS:
	        return "CXL_ALL_SEGS";
	    case Enum::CXL_ALL_SYMB:
	    	return "CXL_ALL_SYMB";
		case Enum::CXL_ALL_INSTRUCTIONS:
	    	return "CXL_ALL_INSTRUCTIONS";
	    case Enum::SEQ_NUM_CHANGE:
	        return "SEQ_NUM_CHANGE";
	    case Enum::TCP_RECONNECT:
	        return "TCP_RECONNECT";
	    case Enum::RESET_RECONNECT_TRACKING:
	        return "RESET_RECONNECT_TRACKING";
	    case Enum::ALGO_LAUNCH:
	    	return "ALGO_LAUNCH";
	    case Enum::ALGO_PARAMS:
	        return "ALGO_PARAMS";
	    case Enum::ALGO_ORDER:
	        return "ALGO_ORDER";
	    case Enum::ALGO_ENGINE:
	        return "ALGO_ENGINE";
	    case Enum::ALGO_RESET_POS:
	        return "ALGO_RESET_POS";
	    case Enum::ALGO_STOP:
	        return "ALGO_STOP";
	    case Enum::ALGO_MESSAGE:
	        return "ALGO_MESSAGE";
	    case Enum::ALGO_KILLSWITCH:
	        return "ALGO_KILLSWITCH";
	    case Enum::TERMINATE:
	        return "TERMINATE";

	    case Enum::ORD_SEND:
	        return "ORD_SEND";
	    case Enum::ORD_SEND_WITH_HEDGE:
	        return "ORD_SEND_WITH_HEDGE";
	    case Enum::ORD_SEND_HDG_INSTRUCT:
	        return "ORD_SEND_HDG_INSTRUCT";

	    case Enum::PRINT_LATENCIES:
	        return "PRINT_LATENCIES";
	    case Enum::PRINT_MSG_RATIOS:
	        return "PRINT_MSG_RATIOS";
	    case Enum::CLEAR:
	        return "CLEAR";
	    case Enum::STOP:
	        return "STOP";

	    default:
	        return "UNKNOWN";
	    }
	}

};

//all-purpose instruction so we have a number of different
//possible params and types... just needs to be quick and dirty
struct Instruction
{
	ExchCmd::Enum command;
	string cmd_json;
	int cmd_value;
	string cmd_symbol;
	string cmd_text;

	//carriers for now....
	bool isorder;
	KTN::OrderPod ord;
	vector<SpreadHedgeV3> hdgs;
	uint64_t reqid;

	Instruction(){
		cmd_json= "";
		cmd_value = 0;
		ord = {};
		hdgs.clear();
	}
};

#endif /* SRC_EXCHANGEHANDLERS2_EXCHCMDENUMS_HPP_ */
