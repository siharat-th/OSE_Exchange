//============================================================================
// Name        	: HashMap.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 20, 2023 9:46:31 AM
//============================================================================

#ifndef SRC_MAPS_HASHMAP_HPP_
#define SRC_MAPS_HASHMAP_HPP_

#include <array>
#include <iostream>
using namespace std;


/**
 * @brief A hash map implementation that maps keys to values.
 *
 * @tparam Key The type of the keys in the hash map.
 * @tparam Value The type of the values in the hash map.
 * @tparam TableSize The size of the hash map table.
 */
template <typename Key, typename Value, size_t TableSize>
class HashMap {
public:
    /**
     * @brief Constructs an empty hash map.
     */
    HashMap() :  keys{}, values{}
    {

    }

    /**
     * @brief Returns the number of key-value pairs in the hash map.
     *
     * @return The number of key-value pairs in the hash map.
     */
    size_t size() const {
        return keys.size();
    }

    /**
     * @brief Removes all key-value pairs from the hash map.
     */
    void clear() {
        std::fill(keys.begin(), keys.end(), Key{});      // Default-construct keys
        std::fill(values.begin(), values.end(), Value{}); // Default-construct values
    }

    /**
     * @brief Inserts a key-value pair into the hash map.
     *
     * @param key The key to insert.
     * @param value The value to insert.
     */
    void insert(const Key& key, const Value& value) {
        size_t index =  hashFunction(key);
        // Insert/overwrite the new key-value pair
        keys[index] = key;
        values[index] = value;
    }

    /**
     * @brief Looks up the value associated with the given key in the hash map.
     *
     * @param key The key to look up.
     * @return A reference to the value associated with the key.
     * @note If the key is not found, a default-constructed value is returned.
     */
    Value& lookup(const Key& key) {
        for (size_t i = 0; i < TableSize; ++i) {
            if (keys[i] == key){
                return values[i];
            }
        }

        static Value default_value; // Assumes Value has a default constructor
        return default_value;
    }

    /**
     * @brief Checks if the given key exists in the hash map.
     *
     * @param key The key to check.
     * @return True if the key exists, false otherwise.
     */
    bool find(const Key& key) {
        for (size_t i = 0; i < TableSize; ++i) {
            if (keys[i] == key) return true;
        }
        // Key not found
        return false;
    }

private:
    std::array<Key, TableSize> keys;   ///< The array of keys in the hash map.
    std::array<Value, TableSize> values;   ///< The array of values in the hash map.

    /**
     * @brief Calculates the hash value for the given key.
     *
     * @param key The key to calculate the hash value for.
     * @return The hash value.
     */
    size_t hashFunction(const Key& key) const {
        // You need to implement an efficient hash function here
        // Example: simple modulo-based hash
        return std::hash<Key>{}(key) % TableSize;
    }

    /**
     * @brief Calculates the fast modulo-based hash for the given hash and bucket count.
     *
     * @param hash The hash value.
     * @param bucket_count The number of buckets in the hash map.
     * @return The fast modulo-based hash.
     */
    size_t fast_module(int hash, int bucket_count) {
        return (hash &  (bucket_count - 1));
    }
};

#endif /* SRC_MAPS_HASHMAP_HPP_ */
