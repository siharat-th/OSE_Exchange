//============================================================================
// Name        	: OrderHashTable.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 2, 2023 7:18:03 PM
//============================================================================

#ifndef SRC_ORDERS_ORDERHASHTABLE_HPP_
#define SRC_ORDERS_ORDERHASHTABLE_HPP_


#include <vector>
#include <functional>
#include <string>

#include <Orders/Order.hpp>
#include <Orders/OrderUtils.hpp>
using namespace std;
using namespace KTN;


//#include <city.h>

/**
 * @class OrderHashTable
 * @brief A hash table implementation for storing orders.
 */
class OrderHashTable {
public:
	/**
  * @brief Constructs an OrderHashTable object with the specified initial size.
  * @param initial_size The initial size of the hash table.
  */
	OrderHashTable(size_t initial_size);

	/**
  * @brief Destructor for the OrderHashTable object.
  */
	virtual ~OrderHashTable();

	/**
  * @brief Inserts an order into the hash table.
  * @param key The key associated with the order.
  * @param value The order to be inserted.
  */
    void insert(const string& key, const KTN::Order& value);

    /**
     * @brief Looks up an order in the hash table.
     * @param key The key associated with the order.
     * @param value The found order will be stored in this parameter.
     * @return True if the order was found, false otherwise.
     */
    bool lookup(const string& key, KTN::Order& value) const;

    /**
     * @brief Retrieves all orders stored in the hash table.
     * @param result The vector to store the retrieved orders.
     * @return True if there are orders in the hash table, false otherwise.
     */
    bool all(vector<KTN::Order>& result);

    /**
     * @brief Removes an order from the hash table.
     * @param key The key associated with the order to be removed.
     */
    void erase(const string& key);

    /**
     * @brief Clears all orders from the hash table.
     */
    void clear();

    /**
     * @brief Returns the number of orders in the hash table.
     * @return The number of orders in the hash table.
     */
    inline int count()
    {
					return count_;
    }

public:
    static constexpr float LOAD_FACTOR = 0.75f;

private:
    /**
     * @brief Resizes the hash table when the load factor is exceeded.
     */
    void resize_table();

private:
    size_t size_;
    size_t count_;
    std::vector<std::vector<std::pair<string, Order>>> table_;
    std::hash<string> hash_function_;
};

#endif /* SRC_ORDERS_ORDERHASHTABLE_HPP_ */
