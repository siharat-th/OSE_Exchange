//============================================================================
// Name        	: SpreadPricer.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 19, 2023 2:22:23 PM
//============================================================================

#ifndef SRC_ALGOENGINES_SPREADPRICER_HPP_
#define SRC_ALGOENGINES_SPREADPRICER_HPP_


#include <AlgoEngines/PricingStrategyBase.hpp>
#include <AlgoEngines/QuoterV3.hpp>

#include <KT01/Core/ObjectPool.hpp>
using namespace KTN::Core;



namespace KTN {
	namespace ALGO {
		namespace PRICING {

/**
 * @brief The SpreadPricer class is a pricing strategy for calculating spread prices.
 */
class SpreadPricer : public PricingStrategyBase
{
public:
	/**
  * @brief Constructs a new SpreadPricer object.
  */
	SpreadPricer();

	/**
  * @brief Destroys the SpreadPricer object.
  */
	virtual ~SpreadPricer();

	/**
  * @brief Loads orders for the SpreadPricer.
  * @param cbid The cbid value.
  * @param meta The algometa object.
  */
	void LoadOrders(int cbid, algometa meta);

	/**
  * @brief Calculates the side 2 for the SpreadPricer.
  * @param imp The LittleImpSide object.
  * @param index The index value.
  * @param spmult The spmult value.
  * @param isBid The isBid flag.
  * @param edge The edge value.
  * @param ticks The ticks value.
  * @param prices The array of algomd objects.
  * @param testmode The testmode flag.
  */
	void CalculateSide2(LittleImpSide& imp, int index, int spmult, bool isBid, double edge,
			int ticks, const DepthBookContainer& prices, bool testmode);

	/**
  * @brief Calculates the side 3 for the SpreadPricer.
  * @param imp The LittleImpSide object.
  * @param index The index value.
  * @param spmult The spmult value.
  * @param isBid The isBid flag.
  * @param edge The edge value.
  * @param ticks The ticks value.
  * @param prices The array of algomd objects.
  * @param testmode The testmode flag.
  */
	void CalculateSide3(LittleImpSide& imp, int index, int spmult, bool isBid, double edge, int ticks,
    const DepthBookContainer& prices, bool testmode);

	/**
  * @brief Calculates the simple spread for the SpreadPricer.
  * @param imp The LittleImpSide object.
  * @param index The index value.
  * @param spmult The spmult value.
  * @param isBid The isBid flag.
  * @param edge The edge value.
  * @param ticks The ticks value.
  * @param prices The array of algomd objects.
  * @param testmode The testmode flag.
  */
	void CalculateSimple(LittleImpSide& imp, int index, int spmult, bool isBid, double edge, int ticks,
    const DepthBookContainer& prices, bool testmode);

private:
	//ObjectPool<LittleImpSide> _impool;
	ObjectPool<SpreadHedgeV3> _hdgpool;
	QuoterV3 _quoter[8];

	HedgeType::Enum _HDGTYPE;
};

		}
	}
}

#endif /* SRC_ALGOENGINES_SPREADPRICER_HPP_ */
