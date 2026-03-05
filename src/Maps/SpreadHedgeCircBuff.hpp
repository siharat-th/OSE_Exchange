//============================================================================
// Name        	: SpreadHedgeCircBuff.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Jun 24, 2024 6:03:31 AM
//============================================================================

#ifndef SRC_MAPS_SPREADHEDGECIRCBUFF_HPP_
#define SRC_MAPS_SPREADHEDGECIRCBUFF_HPP_

#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <cstring>

#include <AlgoEngines/pricing_structs.hpp>
using namespace KTN::ALGO::PRICING;

/**
 * @class SpreadHedgeCircBuff
 * @brief Circular buffer implementation for storing spread hedge data
 */
class SpreadHedgeCircBuff {
private:
	std::vector<std::vector<SpreadHedgeV3>> buffer; /**< The buffer to store spread hedge data */
	std::vector<uint64_t> indices; /**< The indices of the spread hedge data in the buffer */
	const int size = 16; /**< The size of the circular buffer */
	int head; /**< The head index of the circular buffer */
	int tail; /**< The tail index of the circular buffer */

	int last; /**< The index of the last element added to the buffer */
	int count; /**< The number of elements currently in the buffer */

	bool full; /**< Flag indicating if the buffer is full */
	SpreadHedgeV3* ptr_ini; /**< Pointer to the initial element of the buffer */
	SpreadHedgeV3* ptr_end; /**< Pointer to the end element of the buffer */

	/**
  * @brief Get the next index in the circular buffer
  * @return The next index
  */
	int next()
	{
		int n = last;
		last++;
		if (last > size - 1)
			last = 0;
		return n;
	}

	/**
  * @brief Get the index of the given request ID in the buffer
  * @param reqid The request ID to search for
  * @return The index of the request ID in the buffer, or -1 if not found
  */
	int reqid_to_index(uint64_t reqid) {
		for (int i = 0; i < count; i++)
		{
			if (indices[i] == reqid) {
				return i;
			}
		}
		return -1;
	}

public:
	/**
  * @brief Default constructor for SpreadHedgeCircBuff
  */
	SpreadHedgeCircBuff() : head(0), tail(0), full(false) {
		buffer.resize(size);
		indices.resize(size);
		ptr_ini = nullptr;
		ptr_end = nullptr;
		last = 0;
		count = 0;
	}

	/**
  * @brief Overloaded operator to access the spread hedge data by request ID
  * @param reqid The request ID
  * @return A reference to the spread hedge data vector
  */
	std::vector<SpreadHedgeV3>& operator[](uint64_t reqid) {
		int index = reqid_to_index(reqid);

		if (index < 0) {
			index = next();

			if (count < size)
				count++;
			indices[index] = reqid;
			buffer[index].clear();
		}

		return buffer[index];
	}

	/**
  * @brief Clear the circular buffer
  */
	void clear()
	{
		buffer.clear();
		indices.clear();
		buffer.resize(size);
		indices.resize(size);
		ptr_ini = nullptr;
		ptr_end = nullptr;
		last = 0;
		count = 0;
	}

	/**
  * @brief Remove the spread hedge data with the given request ID from the buffer
  * @param reqid The request ID
  * @return True if the spread hedge data was successfully removed, false otherwise
  */
	bool remove(uint64_t reqid) {

		int index = reqid_to_index(reqid);
		if (index < 0) return false;

		if (indices[index] == reqid) {
			buffer[index] = std::vector<SpreadHedgeV3>();
			indices[index] = -1;

			return true;
		}
		return false;
	}

	/**
  * @brief Find the spread hedge data with the given request ID in the buffer
  * @param reqid The request ID
  * @return The spread hedge data vector if found, an empty vector otherwise
  */
	std::vector<SpreadHedgeV3> find(uint64_t reqid) {
		int index = reqid_to_index(reqid);
		if (index < 0) return std::vector<SpreadHedgeV3>();
		if (indices[index] == reqid) {
			return buffer[index];
		}
		return std::vector<SpreadHedgeV3>();
	}

};

#endif /* SRC_MAPS_SPREADHEDGECIRCBUFF_HPP_ */
