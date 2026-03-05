//============================================================================
// Name        	: OrderRingBuffer.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Sep 21, 2023 12:53:30 PM
//============================================================================

#ifndef SRC_ORDERS_ORDERRINGBUFFER_HPP_
#define SRC_ORDERS_ORDERRINGBUFFER_HPP_

#include <string>
#include <vector>
#include <cstring>
using namespace std;

#include <Orders/Order.hpp>
using namespace KTN;

/**
 * @class OrderRingBuffer
 * @brief A ring buffer implementation for storing orders.
 */
class OrderRingBuffer {
public:
    /**
     * @brief Constructs an OrderRingBuffer with the specified capacity.
     * @param capacity The maximum number of orders that can be stored in the buffer.
     */
    OrderRingBuffer(size_t capacity);

    /**
     * @brief Destroys the OrderRingBuffer object.
     */
    ~OrderRingBuffer();

    /**
     * @brief Inserts an order into the buffer with the specified key and value.
     * @param key The key associated with the order.
     * @param value The order to be inserted.
     */
    void insert(const uint64_t& key, const KTN::Order& value);

    /**
     * @brief Looks up orders in the buffer with the specified key.
     * @param key The key to search for.
     * @param values The vector to store the matching orders.
     * @return True if at least one order is found, false otherwise.
     */
    bool lookup(const uint64_t& key, std::vector<KTN::Order>& values) const;

private:
    size_t capacity_; ///< The capacity of the buffer.
    size_t write_index_ = 0; ///< The index to write the next order.
    uint64_t* keys_; ///< The array to store the keys.
    KTN::Order* values_; ///< The array to store the orders.
    size_t* valueCounts_; ///< The array to store the number of orders for each key.
};
#endif /* SRC_ORDERS_ORDERRINGBUFFER_HPP_ */
