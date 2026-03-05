//============================================================================
// Name        	: MDOrder.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Apr 8, 2024 4:21:02 PM
//============================================================================

#ifndef SRC_ORDERBOOK_MDORDER_HPP_
#define SRC_ORDERBOOK_MDORDER_HPP_

#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>

namespace KT01{
namespace OrderBook{

class MdOrder {
public:
    /**
     * @brief Represents a market data order for MBO purposes.
     */
    uint64_t id; /**< The ID of the order. */
    int16_t price; /**< The price of the order. */
    int16_t quantity; /**< The quantity of the order. */
    uint32_t ns; /**< The time offset of the order, in nanoseconds from last unit timestamp. */
    uint32_t seqnum;
    std::string base63_secid; /**< The Base63 encoded security ID. */
    std::string symbol; /**< The symbol of the order. */
    uint32_t secid; /**< The security ID. */

    /**
     * @brief Default constructor for MdOrder.
     */
    MdOrder() : id(0), price(0), quantity(0), ns(0), seqnum(0) {}

    /**
     * @brief Constructor for MdOrder.
     * @param id The ID of the order.
     * @param price The price of the order.
     * @param quantity The quantity of the order.
     * @param priority The priority of the order.
     */
    MdOrder(int id, double price, int quantity, int priority, std::string secid) : id(id), price(price),
            quantity(quantity), ns(ns), seqnum(seqnum), base63_secid(secid) {}

    /**
     * @brief Resets the values of the MdOrder to their default values.
     */
    void reset()
    {
        id = 0;
        price = 0;
        quantity = 0;
        seqnum = 0;
        ns = 0;
    }
};

} // namespace OrderBook
} // namespace KTN
//============================================================================

#endif /* SRC_ORDERBOOK_MDORDER_HPP_ */