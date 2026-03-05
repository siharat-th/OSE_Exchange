//============================================================================
// Name        	: CustomHFTMap.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 27, 2023 7:34:07 PM
//============================================================================

#ifndef CUSTOMHFTMAP_H
#define CUSTOMHFTMAP_H

#include <cstdint>
#include <tbb/concurrent_unordered_map.h>

#include <Orders/OrderPod.hpp>

/**
 * @brief CustomHFTMap class represents a custom ultra low latency hash map implementation.
 */
class CustomHFTMap
{
  public:
	/**
	 * @brief Default constructor for CustomHFTMap.
	 */
	CustomHFTMap();

	/**
	 * @brief Inserts a key-value pair into the map.
	 * @param key The key to insert.
	 * @param value The value to insert.
	 */
	void insert(uint64_t key, KTN::OrderPod *value);

	/**
	 * @brief Retrieves the value associated with the given key.
	 * @param key The key to search for.
	 * @return A pointer to the value associated with the key, or nullptr if the key is not found.
	 */
	KTN::OrderPod *get(uint64_t key);

	/**
	 * @brief Checks if the given key exists in the map.
	 * @param key The key to search for.
	 * @return True if the key exists, false otherwise.
	 */
	bool find(uint64_t key) const;

	/**
	 * @brief Returns the number of key-value pairs in the map.
	 * @return The size of the map.
	 */
	inline int size() const
	{
		return map.size();
		//return index_;
	}

	/**
	 * @brief Returns the value at the specified index.
	 * @param index The index of the value to retrieve.
	 * @return A pointer to the value at the specified index, or a new OrderPod object if the index is out of range.
	 */
	//inline OrderPod *at(size_t index)
	//{
		// if (index <= index_)
		//     return values_[index];
		// else
		//     return new OrderPod();
	//}

	/**
	 * @brief Clears the map, removing all key-value pairs.
	 */
	void clear();

	/**
	 * @brief Clears the map, removing all key-value pairs with dead keys.
	 */
	void cleardead();

	/**
	 * @brief Removes the key-value pair with the given key from the map.
	 * @param key The key to remove.
	 */
	void erase(uint64_t key);

	/**
	 * @brief Overloaded [] operator for assignment and creation.
	 * @param key The key to access.
	 * @return A reference to the value associated with the key.
	 */
	KTN::OrderPod &operator[](uint64_t key);

	bool getByOrderNum(KTN::OrderPod &ord, const std::string &ordernum);
	bool getByExchOrdId(KTN::OrderPod &ord, uint64_t exchordid);
	void getBySecId(std::vector<KTN::OrderPod> &ords, int32_t secid);

	void updateOrderNum(KTN::OrderPod &ord);
	void updateExchOrdId(KTN::OrderPod &ord, uint64_t exchordid);

  private:
	using OrderMapType = tbb::concurrent_unordered_map<uint64_t, KTN::OrderPod>;
	OrderMapType map;

	using OrderNumMapType = tbb::concurrent_unordered_map<std::string, uint64_t>;
	OrderNumMapType orderNumMap;
	using ExchOrdIdMapType = tbb::concurrent_unordered_map<uint64_t, uint64_t>;
	ExchOrdIdMapType exchOrdIdMap;
	// size_t index_; // Current number of key-value pairs in the map
	// const size_t capacity_ = 10000; // Maximum capacity of the map
	// uint64_t keys_[10000]; // Array to store the keys
	// OrderPod* values_[10000]; // Array to store the values
};

#endif
