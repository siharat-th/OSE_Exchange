//============================================================================
// Name        	: AlgoInitializer.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 28, 2023 4:22:37 PM
//============================================================================

#ifndef SRC_ALGOPARAMS_ALGOINITIALIZER_HPP_
#define SRC_ALGOPARAMS_ALGOINITIALIZER_HPP_


#include <string>
#include <ExchangeHandler/IExchangeSender.hpp>
#include <ExchangeHandler/session/SessionSettings.hpp>

#include <exchsupport/cme/settings/CmeSessionSettings.hpp>

using namespace std;

/**
 * @brief The AlgoInitializer struct represents the parameters required to initialize the algorithm.
 *@details the idea is to create an easy to use all-in-one struct to init the algo 
 * with std::string name, std::string guid, int algoid, ExchangeSender& ordsender, vector<int> channels, int cpu, CmeSecMaster & sec
 */
struct AlgoInitializer
{
	string name; /**< The name of the algorithm. */
	string guid; /**< The unique identifier of the algorithm. */
	int cpu; /**< The CPU number to run the algorithm on. */
	IExchangeSender* ordersender; /**< The exchange sender for sending orders. */
	vector <instrument> legs; /**< The list of instruments for the algorithm. */
	vector<KTN::OrderPod> orders; /**< The list of LIVE recovered orders associated with the algorithm. */
};

#endif /* SRC_ALGOPARAMS_ALGOINITIALIZER_HPP_ */
