//============================================================================
// Name        	: SimpleMap.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 10, 2023 12:42:04 PM
//============================================================================

#ifndef SRC_MAPS_SIMPLEMAP_HPP_
#define SRC_MAPS_SIMPLEMAP_HPP_
#pragma once

#include <vector>

#include <Maps/AllocatorBase.hpp>

namespace KTN { namespace Core {

/**
 * @brief A simple map implementation that associates keys with values.
 *
 * @tparam K The type of the keys.
 * @tparam T The type of the values.
 */
template <typename K, typename T>
class SimpleMap {
public:
	/**
  * @brief Constructs a SimpleMap object with the specified capacity.
  *
  * @param capacity The maximum number of elements the map can hold.
  */
	SimpleMap(size_t capacity) : capacity_(capacity), values_(capacity), index_(0) {}

    /**
     * @brief Inserts a key-value pair into the map.
     *
     * @param key The key to insert.
     * @param value A pointer to the value to insert.
     */
    void insert(K key, T* value) {
        if (index_ < capacity_) {
            keys_[index_] = key;
            values_.get()[index_] = std::make_pair(key, value);
            index_++;
        }
        // Handle the case where the map is full (you may want to add an error condition or resize the map).
    }

    /**
     * @brief Retrieves the value associated with the specified key.
     *
     * @param key The key to search for.
     * @return A pointer to the value associated with the key, or nullptr if the key is not found.
     */
    T* get(K key) {
        for (size_t i = 0; i < index_; ++i) {
            if (keys_[i] == key) {
                return values_.get()[i].second;
            }
        }
        return nullptr; // Key not found
    }

    /**
     * @brief Checks if the map contains the specified key.
     *
     * @param key The key to search for.
     * @return true if the key is found, false otherwise.
     */
    bool find(K key) {
        for (size_t i = 0; i < index_; ++i) {
            if (keys_[i] == key) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Gets the number of elements in the map.
     *
     * @return The number of elements in the map.
     */
    inline int size() const {
        return static_cast<int>(index_);
    }

    /**
     * @brief Retrieves the value at the specified index.
     *
     * @param index The index of the value to retrieve.
     * @return A pointer to the value at the specified index, or nullptr if the index is out of bounds.
     */
    T* at(int index) const {
        if (index >= 0 && static_cast<size_t>(index) < index_) {
            return values_.get()[index].second;
        } else {
            return nullptr; // Index out of bounds
        }
    }

    /**
     * @brief Clears the map, removing all elements.
     */
    void clear() {
        index_ = 0;
    }

    /**
     * @brief Clears any dead elements from the map.
     * 
     * Implement logic to clear dead elements if needed.
     */
    void cleardead() {
        // Implement logic to clear dead elements if needed
    }

    /**
     * @brief Removes the element with the specified key from the map.
     *
     * @param key The key of the element to remove.
     */
    void erase(K key) {
        for (size_t i = 0; i < index_; ++i) {
            if (keys_[i] == key) {
                // Shift elements to fill the gap
                for (size_t j = i; j < index_ - 1; ++j) {
                    keys_[j] = keys_[j + 1];
                    values_.get()[j] = values_.get()[j + 1];
                }
                index_--;
                break;
            }
        }
    }

    /**
     * @brief Retrieves the value associated with the specified key.
     *
     * If the key is not found, a new value is created and associated with the key.
     *
     * @param key The key to search for.
     * @return A reference to the value associated with the key.
     */
    T& operator[](K key) {
        T* result = get(key);
        if (!result) {
            // Key not found, create a new value
            result = new T();
            insert(key, result);
        }
        return *result;
    }

private:
    size_t capacity_; ///< The maximum number of elements the map can hold.
    AllocatorBase<std::pair<K, T*>> values_; ///< The underlying storage for the key-value pairs.
    K keys_[100]; ///< The array of keys.
    size_t index_; ///< The current number of elements in the map.
};


}}

#endif /* SRC_MAPS_SIMPLEMAP_HPP_ */
