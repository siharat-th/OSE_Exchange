//============================================================================
// Name        	: BufferPool.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 27, 2023 1:06:23 PM
//============================================================================

#ifndef SRC_CORE_ORDERPODPOOL_HPP_
#define SRC_CORE_ORDERPODPOOL_HPP_

#pragma once

#include <unistd.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <cstdint>
#include <functional>
#include <sstream>

#include <boost/lockfree/stack.hpp>

#include <KT01/Core/Macro.hpp>
#include <KT01/Core/PerformanceTracker.hpp>

#include <Orders/OrderPod.hpp>

namespace KTN { namespace Core {

/**
 * @brief The OrderPodPool class represents a pool of OrderPod objects.
 */
class OrderPodPool {
public:
    /**
     * @brief Constructs an OrderPodPool object with the specified pool size.
     * @param pool_size The size of the pool.
     */
	OrderPodPool(size_t pool_size);

    /**
     * @brief Destroys the OrderPodPool object.
     */
    ~OrderPodPool();

    /**
     * @brief Retrieves an OrderPod object from the pool.
     * @return A pointer to the retrieved OrderPod object, or nullptr if the pool is empty.
     */
    OrderPod* get();

    /**
     * @brief Puts an OrderPod object back into the pool.
     * @param buffer A pointer to the OrderPod object to be put back into the pool.
     * @return True if the OrderPod object was successfully put back into the pool, false otherwise.
     */
    bool put(OrderPod* buffer);

private:
    boost::lockfree::stack<OrderPod*> _pool_lifo; /**< The LIFO stack used to store the OrderPod objects. */
    std::atomic<int> _pool_size; /**< The current size of the pool. */
    std::atomic<int> _max_size; /**< The maximum size of the pool. */
};

}}
#endif /* SRC_CORE_BUFFERPOOL_HPP_ */
