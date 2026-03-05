//============================================================================
// Name        	: PricingStrategyBase.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 19, 2023 10:13:45 AM
//============================================================================

#ifndef SRC_ALGOCORE_PRICINGSTRATEGYBASE_HPP_
#define SRC_ALGOCORE_PRICINGSTRATEGYBASE_HPP_
#pragma once

#include <tbb/concurrent_vector.h>

#include <AlgoSupport/algo_structs.hpp>
#include <AlgoEngines/pricing_structs.hpp>

#include <KT01/DataStructures/DepthBook.hpp>
#include <KT01/DataStructures/DepthBookTypes.hpp>
using namespace KT01::DataStructures::MarketDepth;

#include <KT01/Core/Log.hpp>

namespace KTN {
namespace ALGO {
namespace PRICING {

/**
 * @brief The base class for pricing strategies.
 */
class PricingStrategyBase {
public:
	/**
  * @brief Default constructor.
  */
	PricingStrategyBase();

	/**
  * @brief Destructor.
  */
	virtual ~PricingStrategyBase();

	/**
  * @brief Load the pricing strategy with the given legs.
  * @param legs An array of instrument objects representing the legs.
  * @param len The length of the legs array.

  */
	void Load(instrument* legs, int len);

protected:
	/**
  * @brief Log a message.
  * @param szMsg The message to log.
  * @param iColor The color of the log message (optional).
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

	/**
  * @brief Round a value to the nearest tick.
  * @param value The value to round.
  * @param MINTICK The minimum tick size.
  * @param side The rounding side (-1 for rounding up, 1 for rounding down).
  * @return The rounded value.
  */
	inline int roundToTick(int value, int MINTICK, int side) {
     int remainder = value % MINTICK;
     int result = value;

     if (side == -1) {
         if (remainder != 0)
             result = value + (MINTICK - remainder);
         else
             result = value;
     } else if (side == 1) {
         result = value - remainder;
     }

     return result;
	}

	/**
  * @brief Round and convert a price value.
  * @param px The price value to round and convert.
  * @param tickprecision The precision of the tick size.
  * @param mintick The minimum tick size.
  * @param roundUp Flag indicating whether to round up or down.
  * @return The rounded and converted price value.
  */
	inline float priceRoundConvert(float px, int tickprecision, float mintick, bool roundUp=false)
	{
		if (tickprecision == 0)
			return px;

		float origpx = px;

		float pow_10 = pow(10.0f, (float)tickprecision);
		float pow_minus10 = pow(10.0f, (float)(-1 * tickprecision));

		int pxinflate = origpx * pow_10;
		int tickinflate = mintick * pow_10;

		if (roundUp)
		{
			int temppx = pxinflate;


			int remainder = abs(pxinflate) % tickinflate;
			if (remainder == 0)
				px = origpx ;

			if (pxinflate < 0)
				temppx = -1 *(abs(pxinflate) - remainder);
			else
				temppx = pxinflate + tickinflate - remainder;

			float newpx = temppx * pow_minus10;
			px = newpx;
		}
		else
		{
			//new rounding:
			//https://stackoverflow.com/questions/27115595/standard-way-to-round-integer-down-to-nearest-multiple-of-4
			int temppx = (pxinflate / tickinflate) * tickinflate;

			float newpx = temppx * pow_minus10;

			px = newpx;
		}


		return px;
	}

protected:
	instrument _legs[24];

	int _numLegs;
	bool _priceround;
};

//end namespace:
}}}

#endif /* SRC_ALGOCORE_PRICINGSTRATEGYBASE_HPP_ */
