
//============================================================================
// Name        	: algo_helpers.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 30, 2023 8:55:15 AM
//============================================================================

#ifndef SRC_ALGOS_ALGO_HELPERS_HPP_
#define SRC_ALGOS_ALGO_HELPERS_HPP_
#pragma once

#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <memory>

#include <functional>
#include <string>
#include <utility>

#include "algo_structs.hpp"
#include <Orders/IdGen.h>

#include "algo_enums.hpp"
using namespace KTN::ALGO;

namespace KTN{
namespace ALGO{

/**
 * @brief Helper class for various algorithmic functions and calculations.
 */
class AlgoHelpers
{
public:

	/**
  * @brief Rounds up a number to the nearest multiple.
  * @param numToRound The number to round up.
  * @param multiple The multiple to round up to.
  * @return The rounded up number.
  */
	inline int roundUp(int numToRound, int multiple)
	{
		if (multiple == 0)
			return numToRound;

		int remainder = abs(numToRound) % multiple;
		if (remainder == 0)
			return numToRound;

		if (numToRound < 0)
			return -(abs(numToRound) - remainder);
		else
			return numToRound + multiple - remainder;
	}

	/**
  * @brief Gets the side value based on the given string.
  * @param side The side string.
  * @return The side value (-1 for 'S', 1 for 'B', 0 otherwise).
  */
	inline int getSide(std::string side)
	{
		if (side.at(0) == 'B')
			return 1;
		if (side.at(0) == 'S')
			return -1;

		return 0;
	}

	/**
  * @brief Calculates the average of a vector of doubles.
  * @param px The vector of doubles.
  * @return The average value.
  */
	inline double avg(std::vector<double> px)
	{
		int len = px.size();

		double res = 0;
		for (auto d : px)
			res += d;

		double avg = res/len;
		return avg;
	};

	/**
  * @brief Computes the mean of a vector of doubles.
  * @param v The vector of doubles.
  * @return The mean value.
  */
	inline double computeMean( const vector<double> &v)
	{
		double sum = 0;
    for ( auto e : v) { sum += e; }
    return sum / v.size();
	};

	// Add documentation comments for the remaining functions...

};

}}

#endif /* SRC_ALGOS_ALGO_HELPERS_HPP_ */
