//============================================================================
// Name        	: lockfree_limitorderbook.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Apr 8, 2024 4:19:34 PM
//============================================================================

#ifndef SRC_ORDERBOOK_LOCKFREE_LIMITORDERBOOK_HPP_
#define SRC_ORDERBOOK_LOCKFREE_LIMITORDERBOOK_HPP_
#pragma once
#include <Maps/circular_array.hpp>
#include <tbb/concurrent_vector.h>

namespace KTN {
namespace MD {

/**
 * @brief Represents a lock-free implementation of a limit order book.
 */
class LockFreeLimitOrderBook : public LimitOrderBook {
private:
	tbb::concurrent_vector<MdOrder> bids;   /**< The concurrent vector to store the bid orders. */
	tbb::concurrent_vector<MdOrder> offers; /**< The concurrent vector to store the offer orders. */

public:
	/**
  * @brief Constructs a new LockFreeLimitOrderBook object.
  * 
  * @param precision The precision of the limit order book.
  * @param depth The depth of the limit order book.
  */
	LockFreeLimitOrderBook(int precision, int depth) : LimitOrderBook(precision, depth) {
		bids.resize(depth);
		offers.resize(depth);
	}

	/**
  * @brief Adds an order to the limit order book.
  * 
  * @param order The order to be added.
  * @param is_bid A flag indicating whether the order is a bid or an offer.
  */
	void add_order(const MdOrder& order, bool is_bid) override {
		if (is_bid) {
			int index = price_to_index(order.price, true);
			if (index > -1)
				bids[index] = order;
		} else {
			int index = price_to_index(order.price, false);
			if (index > -1)
				offers[index] = order;
		}
	}

	/**
  * @brief Gets the best bid order from the limit order book.
  * 
  * @return The best bid order.
  */
	MdOrder get_best_bid() override {
		return *ptr_bid_end;
	}

	/**
  * @brief Gets the best offer order from the limit order book.
  * 
  * @return The best offer order.
  */
	MdOrder get_best_offer() {
		return *ptr_offer_ini;
	}
};

} // namespace MD
} // namespace KTN

#endif /* SRC_ORDERBOOK_LOCKFREE_LIMITORDERBOOK_HPP_ */
