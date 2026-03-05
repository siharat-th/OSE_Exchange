//============================================================================
// Name        	: ImpliedPricer.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 19, 2023 2:22:23 PM
//============================================================================

#ifndef SRC_ALGOENGINES_IMPPRICER_HPP_
#define SRC_ALGOENGINES_IMPPRICER_HPP_

#include <AlgoEngines/PricingStrategyBase.hpp>
#include <AlgoEngines/QuoterV3.hpp>

namespace KTN {
namespace ALGO {
	namespace PRICING {

/**
 * @brief The ImpliedPricer class is a pricing strategy that calculates implied prices based on given orders.
 */
class ImpliedPricer : public PricingStrategyBase
{
public:
	static const int MAX_SIZE = 24;
	const float INF = numeric_limits<float>::infinity();

public:
	/**
  * @brief Default constructor for the ImpliedPricer class.
  */
	ImpliedPricer();

	/**
  * @brief Destructor for the ImpliedPricer class.
  */
	virtual ~ImpliedPricer();

	/**
  * @brief Load orders for the ImpliedPricer.
  * @param cbid The cbid value.
  * @param meta The algometa value.
  */
	void LoadOrders(int cbid, algometa meta);

	/**
  * @brief Calculate the shortest paths using Floyd's algorithm.
  * @param adjMatrix The adjacency matrix.
  * @param isBid A flag indicating if it is a bid.
  */
	void floydShortestPaths(EdgeStruct (&adjMatrix)[MAX_SIZE][MAX_SIZE], bool isBid);

	/**
  * @brief Calculate the shortest paths using Floyd's algorithm.
  * @param adjMatrix The adjacency matrix.
  * @param isBid A flag indicating if it is a bid.
  */
	void floydShortestPaths2(EdgeStruct (&adjMatrix)[MAX_SIZE][MAX_SIZE], bool isBid);

	/**
  * @brief Calculate the side based on the given parameters.
  * @param index The index value.
  * @param spmult The spmult value.
  * @param isBid A flag indicating if it is a bid.
  * @param edge The edge value.
  * @param prices The prices array.
  * @param testmode A flag indicating if it is in test mode.
  * @return The calculated LittleImpSide.
  */
	LittleImpSide CalculateSide(int index, int spmult, bool isBid, double edge, DepthBook* prices, bool testmode);
};


}}}

#endif /* SRC_ALGOENGINES_IMPPRICER_HPP_ */
