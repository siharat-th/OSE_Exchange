//============================================================================
// Name        	: BufferPool.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 27, 2023 1:06:23 PM
//============================================================================

#ifndef SRC_CORE_ORDERPODPOOL2_HPP_
#define SRC_CORE_ORDERPODPOOL2_HPP_

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
#include <boost/align/aligned_alloc.hpp>

#include <KT01/Core/Macro.hpp>
#include <KT01/Core/PerformanceTracker.hpp>

#include <Orders/OrderPod.hpp>

namespace KTN { namespace Core {

/**
 * @class OrderPodPool2
 * @brief A pool of OrderPod objects for efficient memory management.
 */
class OrderPodPool2 {
public:
    /**
     * @brief Constructs an OrderPodPool2 object with the specified pool size.
     * @param pool_size The size of the pool.
     */
    OrderPodPool2(size_t pool_size);

    /**
     * @brief Destroys the OrderPodPool2 object.
     */
    ~OrderPodPool2();

    /**
     * @brief Retrieves an OrderPod object from the pool.
     * @return A pointer to the retrieved OrderPod object, or nullptr if the pool is empty.
     */
    OrderPod* get();

    /**
     * @brief Returns an OrderPod object to the pool.
     * @param buffer A pointer to the OrderPod object to be returned.
     * @return True if the OrderPod object was successfully returned to the pool, false otherwise.
     */
    bool put(OrderPod* buffer);

private:
    boost::lockfree::stack<OrderPod*> _pool_lifo; ///< The LIFO stack to store the OrderPod objects.
    std::atomic<int> _pool_size; ///< The current size of the pool.
    std::atomic<int> _max_size; ///< The maximum size of the pool.
};

}}
#endif /* SRC_CORE_BUFFERPOOL_HPP_ */
