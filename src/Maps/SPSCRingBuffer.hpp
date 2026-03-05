//============================================================================
// Name        	: SPSCRingBuffer.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 29, 2023 5:10:40 PM
//============================================================================

#ifndef SRC_CORE_SPSCRINGBUFFER_HPP_
#define SRC_CORE_SPSCRINGBUFFER_HPP_
#pragma once

#include <vector>
#include <atomic>
#include <iostream>
using namespace std;

#include <KT01/Core/Macro.hpp>

#include <tbb/cache_aligned_allocator.h>


namespace KTN{ namespace Core {

/**
 * @brief A single-producer, single-consumer ring buffer implementation.
 *
 * This class provides a thread-safe ring buffer that can be used in a single-producer, single-consumer scenario.
 * It supports enqueueing and dequeueing elements, as well as checking the size and emptiness of the buffer.
 *
 * @tparam T The type of elements stored in the ring buffer.
 */
template <typename T>
class SPSCRingBuffer {
public:
    /**
     * @brief Constructs a new SPSCRingBuffer object with the specified capacity.
     *
     * @param capacity The capacity of the ring buffer.
     */
	SPSCRingBuffer(size_t capacity)
	: capacity_(nextPowerOfTwo(capacity))
	, _data(capacity_, tbb::cache_aligned_allocator<T>())
	{
					producerIndex_.store(0, std::memory_order_relaxed);
        consumerIndex_.store(0, std::memory_order_relaxed);
    }

    /**
     * @brief Destroys the SPSCRingBuffer object.
     */
    ~SPSCRingBuffer() {
    }

    /**
     * @brief Enqueues an item into the ring buffer.
     *
     * @param item The item to enqueue.
     * @param warmup Flag indicating whether this is a warmup enqueue.
     * @return true if the item was successfully enqueued, false otherwise.
     */
    inline bool enqueue(const T& item, bool warmup = false) {
        size_t currentProducer = producerIndex_.load(std::memory_order_relaxed);
        size_t next = (currentProducer + 1) & (capacity_ - 1);
        PREFETCH(&_data[next]);  // Prefetch the next write location

        if (warmup) return false;

        if (LIKELY(next != consumerIndex_.load(std::memory_order_relaxed))) {
            _data[currentProducer] = item;
            producerIndex_.store(next, std::memory_order_release);
            return true;
        }
        return false;
    }

    /**
     * @brief Pushes an item into the ring buffer.
     *
     * @param item The item to push.
     * @param warmup Flag indicating whether this is a warmup push.
     * @return true if the item was successfully pushed, false otherwise.
     */
    inline bool push(const T& item, bool warmup = false) {
					size_t currentProducer = producerIndex_.load(std::memory_order_relaxed);
        size_t next = (currentProducer + 1) & (capacity_ - 1);
        PREFETCH(&_data[next]);  // Prefetch the next write location

        if (warmup) return false;

		if (LIKELY(next != consumerIndex_.load(std::memory_order_relaxed))) {
			_data[currentProducer] = item;
			producerIndex_.store(next, std::memory_order_release);
			return true;
		}
		return false;
    }

    /**
     * @brief Dequeues an item from the ring buffer.
     *
     * @param item The dequeued item.
     * @return true if an item was successfully dequeued, false otherwise.
     */
    inline bool dequeue(T& item) {
        size_t currentConsumer = consumerIndex_.load(std::memory_order_relaxed);
        size_t next = (currentConsumer + 1) & (capacity_ - 1);

        if (LIKELY(currentConsumer != producerIndex_.load(std::memory_order_acquire))) {
            item = _data[currentConsumer];
            consumerIndex_.store(next, std::memory_order_release);
            return true;
        }
        return false;
    }

    /**
     * @brief Tries to dequeue an item from the ring buffer.
     *
     * @param item The dequeued item.
     * @return true if an item was successfully dequeued, false otherwise.
     */
    inline bool try_dequeue(T& item) {
        size_t currentConsumer = consumerIndex_.load(std::memory_order_relaxed);
        size_t next = (currentConsumer + 1) & (capacity_ - 1);

        if (LIKELY(currentConsumer != producerIndex_.load(std::memory_order_acquire))) {
            item = _data[currentConsumer];
            consumerIndex_.store(next, std::memory_order_release);
            return true;
        }
        return false;
    }

    /**
     * @brief Tries to pop an item from the ring buffer.
     *
     * @param item The popped item.
     * @return true if an item was successfully popped, false otherwise.
     */
    inline bool try_pop(T& item) {
        size_t currentConsumer = consumerIndex_.load(std::memory_order_relaxed);
        size_t next = (currentConsumer + 1) & (capacity_ - 1);

        if (LIKELY(currentConsumer != producerIndex_.load(std::memory_order_acquire))) {
            item = _data[currentConsumer];
            consumerIndex_.store(next, std::memory_order_release);
            return true;
        }
        return false;
    }

    /**
     * @brief Gets the current size of the ring buffer.
     *
     * @return The current size of the ring buffer.
     */
    inline size_t size() const {
    size_t currentProducer = producerIndex_.load(std::memory_order_relaxed);
    size_t currentConsumer = consumerIndex_.load(std::memory_order_relaxed);

    if (currentProducer >= currentConsumer) {
     return currentProducer - currentConsumer;
    } else {
     return currentProducer + capacity_ - currentConsumer;
    }
   }

   /**
    * @brief Gets an approximate size of the ring buffer.
    *
    * @return An approximate size of the ring buffer.
    */
  inline size_t size_approx() const {

   size_t currentProducer = producerIndex_.load(std::memory_order_relaxed);
   size_t currentConsumer = consumerIndex_.load(std::memory_order_relaxed);

   if (currentProducer >= currentConsumer) {
    return currentProducer - currentConsumer;
   } else {
    return currentProducer + capacity_ - currentConsumer;
   }
 }

 /**
  * @brief Checks if the ring buffer is empty.
  *
  * @return true if the ring buffer is empty, false otherwise.
  */
  inline bool empty() const {
      return (producerIndex_.load(std::memory_order_relaxed) == consumerIndex_.load(std::memory_order_acquire));
  }

  inline size_t capacity() const {
	  return capacity_;
  }

private:
    alignas(64) std::atomic<size_t> producerIndex_; // Align to cache line
    alignas(64) std::atomic<size_t> consumerIndex_; // Align to cache line

    size_t capacity_;
    std::vector<T, tbb::cache_aligned_allocator<T>> _data;

    /**
     * @brief Calculates the next power of two for a given number.
     *
     * @param n The number to calculate the next power of two for.
     * @return The next power of two for the given number.
     */
    static size_t nextPowerOfTwo(size_t n) {
		if (n == 0) {
			return 1;
		}
		--n;
		n |= n >> 1;
		n |= n >> 2;
		n |= n >> 4;
		n |= n >> 8;
		n |= n >> 16;
		n |= n >> 32; // This line is for 64-bit numbers. Omit if using 32-bit numbers.
		return n + 1;
	}

};

}}

#endif /* SRC_CORE_SPSCRINGBUFFER_HPP_ */
