//============================================================================
// Name        	: algo_state.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Mar 11, 2024 11:38:49 AM
//============================================================================

#ifndef SRC_ALGOS_ALGO_STATE_HPP_
#define SRC_ALGOS_ALGO_STATE_HPP_

#pragma once
#include <string>
#include <AlgoSupport/algo_enums.hpp>

using namespace std;

/**
 * @brief Represents the state of an algorithm.
 */
struct AlgoState
{
	QuoteState::Enum QuotingState; /**< The quoting state of the algorithm. */
	HedgeState::Enum HedgingState; /**< The hedging state of the algorithm. */

	std::string Name; /**< The name of the algorithm. */
	volatile bool stopped; /**< Flag indicating if the algorithm is stopped. */

	int EXCH_CALLBACK_ID; /**< The exchange callback ID. */
	std::string GUID; /**< The GUID of the algorithm. */


	bool GOT_DATA; /**< Flag indicating if the algorithm has received data. */
	long DATA_TRIES; /**< The number of tries to receive data. */
	bool WATCH_TRADES; /**< Flag indicating if the algorithm is watching trades. */
	bool WATCH_VOLUME; /**< Flag indicating if the algorithm is watching volume. */

	bool ENABLED; /**< Flag indicating if the algorithm is enabled. */
//	bool CAN_TRADE;
//	bool CIRCUITBREAKER;

	double NETPOS; /**< The net position of the algorithm. */
	double TOTALEXEC; /**< The total execution of the algorithm. */

	bool USE_REPORTING_MSGS; /**< Flag indicating if the algorithm uses reporting messages. */
	volatile bool RPT_ACTIVE; /**< Flag indicating if the reporting is active. */

	bool USE_STOPS; /**< Flag indicating if the algorithm uses stops. */

	bool VERBOSE; /**< Flag indicating if the algorithm is in verbose mode. */

	HedgeType::Enum HDGTYPE; /**< The hedge type of the algorithm. */

	NewOrderSingleMsg hdg_ords_BUY[8]; /**< The buy hedge orders of the algorithm. */
	NewOrderSingleMsg hdg_ords_SELL[8]; /**< The sell hedge orders of the algorithm. */

	OrderCancelReplaceRequestMsg mod_ords_BUY[8]; /**< The buy modified orders of the algorithm. */
	OrderCancelReplaceRequestMsg mod_ords_SELL[8]; /**< The sell modified orders of the algorithm. */

	int inflight; /**< The number of inflight orders. */
};

#endif /* SRC_ALGOS_ALGO_STATE_HPP_ */
