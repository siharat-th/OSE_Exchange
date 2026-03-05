//============================================================================
// Name        	: MemPool.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 8, 2023 10:27:48 AM
//============================================================================

#ifndef SRC_CORE_MEMPOOL_HPP_
#define SRC_CORE_MEMPOOL_HPP_

#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include <KT01/Core/Macro.hpp>

namespace KTN {
namespace Core {
  /**
   * @brief A memory pool implementation for efficient object allocation and deallocation.
   * 
   * @tparam T The type of objects to be stored in the memory pool.
   */
  template<typename T>
  class MemPool final {
  public:
    /**
     * @brief Constructs a memory pool with a specified number of elements.
     * 
     * @param num_elems The number of elements to pre-allocate in the memory pool.
     */
    explicit MemPool(std::size_t num_elems) :
        store_(num_elems, {T(), true}) /* pre-allocation of vector storage. */ {
      ASSERT(reinterpret_cast<const ObjectBlock *>(&(store_[0].object_)) == &(store_[0]), "T object should be first member of ObjectBlock.");
    }

    /**
     * @brief Allocates an object from the memory pool.
     * 
     * @tparam Args The types of arguments to be passed to the object's constructor.
     * @param args The arguments to be passed to the object's constructor.
     * @return T* A pointer to the allocated object.
     */
    template<typename... Args>
    T *allocate(Args... args) noexcept {
      auto obj_block = &(store_[next_free_index_]);
      ASSERT(obj_block->is_free_, "Expected free ObjectBlock at index:" + std::to_string(next_free_index_));
      T *ret = &(obj_block->object_);
      ret = new(ret) T(args...); // placement new.
      obj_block->is_free_ = false;

      updateNextFreeIndex();

      return ret;
    }

    /**
     * @brief Deallocates an object from the memory pool.
     * 
     * @param elem A pointer to the object to be deallocated.
     */
    auto deallocate(const T *elem) noexcept {
      const auto elem_index = (reinterpret_cast<const ObjectBlock *>(elem) - &store_[0]);
      ASSERT(elem_index >= 0 && static_cast<size_t>(elem_index) < store_.size(), "Element being deallocated does not belong to this Memory pool.");
      ASSERT(!store_[elem_index].is_free_, "Expected in-use ObjectBlock at index:" + std::to_string(elem_index));
      store_[elem_index].is_free_ = true;
    }

    // Deleted default, copy & move constructors and assignment-operators.
    MemPool() = delete;

    MemPool(const MemPool &) = delete;

    MemPool(const MemPool &&) = delete;

    MemPool &operator=(const MemPool &) = delete;

    MemPool &operator=(const MemPool &&) = delete;

  private:
    /**
     * @brief Updates the index of the next free object in the memory pool.
     */
    auto updateNextFreeIndex() noexcept {
      const auto initial_free_index = next_free_index_;
      while (!store_[next_free_index_].is_free_) {
        ++next_free_index_;
        if (UNLIKELY(next_free_index_ == store_.size())) { // hardware branch predictor should almost always predict this to be false any ways.
          next_free_index_ = 0;
        }
        if (UNLIKELY(initial_free_index == next_free_index_)) {
          ASSERT(initial_free_index != next_free_index_, "Memory Pool out of space.");
        }
      }
    }

    // It is better to have one vector of structs with two objects than two vectors of one object.
    // Consider how these are accessed and cache performance.
    struct ObjectBlock {
      T object_;
      bool is_free_ = true;
    };

    // We could've chosen to use a std::array that would allocate the memory on the stack instead of the heap.
    // We would have to measure to see which one yields better performance.
    // It is good to have objects on the stack but performance starts getting worse as the size of the pool increases.
    std::vector<ObjectBlock> store_;

    size_t next_free_index_ = 0;
  };

}} //namespaces

#endif /* SRC_CORE_MEMPOOL_HPP_ */
