//============================================================================
// Name        	: BufferPool.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 27, 2023 1:06:23 PM
//============================================================================

#ifndef SRC_CORE_BUFFERPOOL_HPP_
#define SRC_CORE_BUFFERPOOL_HPP_

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
#include <boost/lockfree/queue.hpp>

#include <KT01/Core/Macro.hpp>
#include <KT01/Core/PerformanceTracker.hpp>

//move this later
#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
using namespace KTN::NET;

namespace KTN { namespace Core {

/**
 * @brief The BufferPool class represents a pool of buffers.
 */
class BufferPool {
public:
    /**
     * @brief Constructs a BufferPool object with the specified buffer size and pool size.
     * @param buffer_size The size of each buffer in bytes.
     * @param pool_size The maximum number of buffers in the pool.
     */
    BufferPool(size_t buffer_size, size_t pool_size);

    /**
     * @brief Destructor for the BufferPool object.
     */
    ~BufferPool();

    /**
     * @brief Retrieves a buffer from the pool.
     * @return A pointer to the retrieved buffer.
     */
    KTNBuf* get();

    /**
     * @brief Puts a buffer back into the pool.
     * @param buffer A pointer to the buffer to be put back.
     * @return True if the buffer was successfully put back, false otherwise.
     */
    bool put(KTNBuf* buffer);

    /**
     * @brief Gets the current size of the buffer pool.
     * @return The current size of the buffer pool.
     */
    size_t size()
    {
					return _pool_size;
    }

    /**
     * @brief Loads the buffer pool.
     */
    void Load();


private:
    boost::lockfree::queue<KTNBuf*> _pool_lifo; /**< The queue representing the buffer pool. */
    int _pool_size; /**< The current size of the buffer pool. */
    int _max_size; /**< The maximum size of the buffer pool. */
    int _buf_size; /**< The size of each buffer in bytes. */
    int _loads; /**< The number of times the buffer pool has been loaded. */
};

}}
#endif /* SRC_CORE_BUFFERPOOL_HPP_ */
