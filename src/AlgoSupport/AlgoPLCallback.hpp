//============================================================================
// Name        	: AlgoPLCallback.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 5, 2023 12:15:51 PM
//============================================================================

#ifndef SRC_ALGOS_ALGOPLCALLBACK_HPP_
#define SRC_ALGOS_ALGOPLCALLBACK_HPP_

#include <string>
#include <AlgoParams/Position.hpp>
using namespace std;

/**
 * @brief The AlgoPLCallback class is an abstract class that defines the interface for handling profit and loss events.
 */
class AlgoPLCallback {
public:
	enum ProfitEvent
	{
		NONE,       /**< No profit event */
		PROFITTAKE, /**< Profit take event */
		STOPLOSS,   /**< Stop loss event */
		TIMERHIT,   /**< Timer hit event */
		MKT         /**< Market event */
	};

	/**
  * @brief This method is called when a profit or loss event occurs.
  * @param pos The position associated with the event.
  * @param event The type of profit event.
  */
	virtual void onPL(Position& pos, ProfitEvent event) = 0;

};

#endif /* SRC_ALGOS_ALGOPLCALLBACK_HPP_ */
