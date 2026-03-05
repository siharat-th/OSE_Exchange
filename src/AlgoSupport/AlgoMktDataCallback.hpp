/*
 * AlgoMktDataCallback.hpp
 *
 *  Created on: May 5, 2021
 *      Author: sgaer
 */

#ifndef ALGOMKTDATACALLBACK_HPP_
#define ALGOMKTDATACALLBACK_HPP_

//#include "algo_structs.hpp"
#include <KT01/DataStructures/DepthBook.hpp>
using namespace KT01::DataStructures::MarketDepth;

namespace KTN {

/**
 * @brief The AlgoMktDataCallback class is an abstract base class that defines the interface for receiving market data updates.
 */
class AlgoMktDataCallback {
public:
	/**
  * @brief Default constructor for AlgoMktDataCallback.
  */
	AlgoMktDataCallback();

	/**
  * @brief Virtual destructor for AlgoMktDataCallback.
  */
	virtual ~AlgoMktDataCallback();

	/**
  * @brief This method is called when the state of the algorithm changes.
  * @param state The new state of the algorithm.
  */
	virtual void onStateChange(int state){};

	/**
  * @brief This method is called when a bid/ask market data update is received.
  * @param md The market data update.
  */
	virtual void onMktDataBidAsk(const DepthBook & md) = 0;

	/**
  * @brief This method is called when a trade market data update is received.
  * @param md The market data update.
  */
	virtual void onMktDataTrade(const DepthBook & md) = 0;


  /**
   * @brief This method is called when a security status update is received.
   * @param secid The security ID.
   * @param secstatus The security status.
   * @param event The security trading event.
   */
  virtual void onMktDataSecurityStatus(int secid, BookSecurityStatus secstatus, BookSecurityTradingEvent event, const uint64_t recvTime) = 0;

};

} /* namespace KTN */

#endif /* ALGOMKTDATACALLBACK_HPP_ */
