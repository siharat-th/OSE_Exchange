//============================================================================
// Name        	: circular_book_array.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Apr 15, 2024 5:44:05 PM
//============================================================================

#ifndef SRC_CORE_CIRCULAR_BOOK_ARRAY_HPP_
#define SRC_CORE_CIRCULAR_BOOK_ARRAY_HPP_

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm> // for std::find_if

/**
 * @brief Represents a market data order.
 */
class MdOrder {
public:
    int id; /**< The order ID. */
    int32_t price; /**< The order price. */
    int quantity; /**< The order quantity. */
    uint64_t priority; /**< The order priority. */

    /**
     * @brief Default constructor for MdOrder.
     */
    MdOrder() : id(0), price(0), quantity(0), priority(0) {}

    /**
     * @brief Constructor for MdOrder.
     * @param id The order ID.
     * @param price The order price.
     * @param quantity The order quantity.
     * @param priority The order priority.
     */
    MdOrder(int id, double price, int quantity, int priority) : id(id), price(price),
            quantity(quantity), priority(priority) {}

    /**
     * @brief Resets the order values to their default.
     */
    void reset()
    {
        id = 0;
        price = 0.0;
        quantity = 0.0;
        priority = 0.0;
    }
};

/**
 * @brief Represents a limit order book.
 */
class LimitOrderBook {
private:
    std::vector<MdOrder> bids; /**< The bid orders. */
    std::vector<MdOrder> offers; /**< The offer orders. */
    int precision; /**< The precision of the order prices. */
    int depth; /**< The depth of the order book. */
    double step_value; /**< The step value for price calculations. */

protected:
    MdOrder* ptr_bid_ini; /**< Pointer to the initial bid order. */
    MdOrder* ptr_bid_end; /**< Pointer to the end bid order. */
    MdOrder* ptr_offer_ini; /**< Pointer to the initial offer order. */
    MdOrder* ptr_offer_end; /**< Pointer to the end offer order. */

    /**
     * @brief Converts a price to its corresponding index in the order book.
     * @param price The price to convert.
     * @param is_bid Flag indicating if the price is for a bid order.
     * @param update_pointers Flag indicating if the order book pointers should be updated.
     * @return The index of the price in the order book.
     */
    int price_to_index(int32_t price, bool is_bid, bool update_pointers = true)
    {
        // Implementation of price_to_index method...
    }

    /**
     * @brief Converts an order ID to its corresponding index in the order book.
     * @param order_id The order ID to convert.
     * @param is_bid Flag indicating if the order is a bid order.
     * @return The index of the order ID in the order book.
     */
    int order_id_to_index(int order_id, bool is_bid) {
        auto& orders = is_bid ? bids : offers;
        auto it = std::find_if(orders.begin(), orders.end(), [order_id](const MdOrder& order) {
            return order.id == order_id;
        });
        if (it != orders.end()) {
            return std::distance(orders.begin(), it);
        }
        return -1; // Return -1 if order with given ID is not found
    }

public:
    /**
     * @brief Constructor for LimitOrderBook.
     * @param precision The precision of the order prices.
     * @param depth The depth of the order book.
     */
    LimitOrderBook(int precision, int depth) : precision(precision), depth(depth) {
        bids.resize(depth);
        offers.resize(depth);
        ptr_bid_ini = ptr_offer_ini = nullptr;
        ptr_bid_end = ptr_offer_end = nullptr;
        step_value = std::pow(10, precision);
    }

    /**
     * @brief Adds an order to the order book.
     * @param order The order to add.
     * @param is_bid Flag indicating if the order is a bid order.
     */
    virtual void add_order(const MdOrder& order, bool is_bid) {
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
     * @brief Updates an order in the order book.
     * @param order The order to update.
     * @param is_bid Flag indicating if the order is a bid order.
     */
    void update_order(const MdOrder& order, bool is_bid) {
        int index = price_to_index(order.price, is_bid);
        if (index == -1)
            return;
        if (is_bid) {
            bids[index] = order;
        } else {
            offers[index] = order;
        }
    }

    /**
     * @brief Deletes an order from the order book.
     * @param order The order to delete.
     * @param is_bid Flag indicating if the order is a bid order.
     */
    void delete_order(const MdOrder& order, bool is_bid) {
        int index = price_to_index(order.price, is_bid);
        if (index == -1)
            return;
        if (is_bid) {
            bids[index] = MdOrder();
        } else {
            offers[index] = MdOrder();
        }
    }

    /**
     * @brief Finds an order in the order book by its ID.
     * @param order_id The ID of the order to find.
     * @return The order with the given ID, or a default order if not found.
     */
    MdOrder find_order_by_id(int order_id) {
        int index = order_id_to_index(order_id, true);
        if (index != -1) {
            return bids[index];
        }
        index = order_id_to_index(order_id, false);
        if (index != -1) {
            return offers[index];
        }
        // Return a default order if order with given ID is not found
        return MdOrder();
    }

    /**
     * @brief Gets the best bid order in the order book.
     * @return The best bid order.
     */
    virtual MdOrder get_best_bid() {
        return *ptr_bid_end;
    }

    /**
     * @brief Gets the lowest bid order in the order book.
     * @return The lowest bid order.
     */
    MdOrder get_lowest_bid() {
        return *ptr_bid_ini;
    }

    /**
     * @brief Gets the best offer order in the order book.
     * @return The best offer order.
     */
    MdOrder get_best_offer() {
        return *ptr_offer_ini;
    }

    /**
     * @brief Gets the highest offer order in the order book.
     * @return The highest offer order.
     */
    MdOrder get_highest_offer() {
        return *ptr_offer_end;
    }

    /**
     * @brief Prints the bid orders in the order book.
     */
    void print_bids()
    {
        for (int i = 0; i < bids.size(); i++)
        {
            std::cout << i << "_" << bids[i].price << " * ";
        }
        std::cout << std::endl;
        std::cout << "Bid ini/end=" << ptr_bid_ini->price << "/" << ptr_bid_end->price << std::endl;
    }

    /**
     * @brief Prints the offer orders in the order book.
     */
    void print_offers()
    {
        for (int i = 0; i < offers.size(); i++)
        {
            std::cout << i << "_" << offers[i].price << " * ";
        }
        std::cout << std::endl;
        std::cout << "Offer ini/end=" << ptr_offer_ini->price << "/" << ptr_offer_end->price << std::endl;
    }
};

#endif /* SRC_CORE_CIRCULAR_BOOK_ARRAY_HPP_ */
