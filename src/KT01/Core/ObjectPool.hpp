//============================================================================
// Name        	: ObjectPool.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 11, 2023 11:45:39 AM
//============================================================================

#ifndef SRC_CORE_OBJECTPOOL_HPP_
#define SRC_CORE_OBJECTPOOL_HPP_

#include <boost/lockfree/stack.hpp>

namespace KTN { namespace Core {

/**
 * @brief A generic object pool implementation.
 * 
 * This class provides a thread-safe object pool that can be used to efficiently manage and reuse objects of a specific type.
 * Objects are preallocated and added to the pool during construction, and can be obtained and returned to the pool as needed.
 * 
 * @tparam T The type of objects to be managed by the pool.
 */
template <typename T>
class ObjectPool {
public:
    /**
     * @brief Constructs an ObjectPool with the specified pool size.
     * 
     * @param pool_size The number of objects to preallocate and add to the pool.
     */
    ObjectPool(size_t pool_size) : _pool_lifo(pool_size), _pool_size(0) {
        // Preallocate the objects and add them to the pool
        for (size_t i = 0; i < pool_size; ++i) {
            T* obj = new T();
            _pool_lifo.push(obj);
            ++_pool_size;
        }
    }

    /**
     * @brief Destroys the ObjectPool and releases all allocated objects.
     */
    ~ObjectPool() {
        T* obj;
        while (_pool_lifo.pop(obj)) {
            delete obj;
        }
    }

    /**
     * @brief Retrieves an object from the pool.
     * 
     * If there are available objects in the pool, an object is obtained from the pool and returned.
     * If the pool is empty, a new object is created and returned.
     * 
     * @return A pointer to the retrieved object.
     */
    T* get() {
        T* obj;
        if (_pool_lifo.pop(obj)) {
            --_pool_size;
            return obj;
        }
        return new T();
    }

    /**
     * @brief Returns an object to the pool.
     * 
     * The specified object is returned to the pool for reuse.
     * 
     * @param obj A pointer to the object to be returned to the pool.
     * @return True if the object was successfully returned to the pool, false otherwise.
     */
    bool put(T* obj) {
        _pool_lifo.push(obj);
        ++_pool_size;
        return true;
    }

private:
    boost::lockfree::stack<T*> _pool_lifo; /**< The LIFO stack used to store the objects in the pool. */
    size_t _pool_size; /**< The current size of the object pool. */
};


}}
#endif /* SRC_CORE_OBJECTPOOL_HPP_ */
