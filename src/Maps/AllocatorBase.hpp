//============================================================================
// Name        	: Allocator.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 10, 2023 12:35:18 PM
//============================================================================

#ifndef SRC_MAPS_ALLOCATORBASE_HPP_
#define SRC_MAPS_ALLOCATORBASE_HPP_

#pragma once
#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <unistd.h> // For mmap
#include <sys/mman.h> // Include the mmap-related header
using namespace std;
#include <KT01/Core/Macro.hpp>

#include <memory>                 // For std::allocator
#include <boost/pool/pool_alloc.hpp> // For boost::fast_pool_allocator
#include <tbb/scalable_allocator.h>  // For tbb::scalable_allocator

//#define USE_BOOST_POOL_ALLOCATOR 1
#define USE_STD_ALLOCATOR 1
//#define USE_TBB_ALLOCATOR 1

namespace KTN{ namespace Core{

#define huge_page_size    (2ll * 1024 * 1024)
#define USE_HUGE_PAGE 1

#define ROUND_UP(p, align)   (((p)+(align)-1u) & ~((align)-1u))
#define IS_POW2(n)           (((n) & (n - 1)) == 0)


/**
 * @brief The AllocatorBase class is a generic allocator class that provides different allocator types based on the selected options.
 * 
 * @tparam T The type of the elements to be allocated.
 */
template <typename T>
class AllocatorBase {
public:
#if USE_STD_ALLOCATOR
    using AllocatorType = std::allocator<T>;
#elif USE_BOOST_POOL_ALLOCATOR
    using AllocatorType = boost::fast_pool_allocator<T>;
#elif USE_GNU_POOL_ALLOCATOR
    using AllocatorType = __gnu_cxx::__pool_alloc<T>;
#elif USE_BITMAP_ALLOCATOR
    using AllocatorType = __gnu_cxx::bitmap_allocator<T>;
#elif USE_TBB_ALLOCATOR
    using AllocatorType = tbb::scalable_allocator<T>;
#else
#error "Invalid allocator selected. Please choose one of the available options."
#endif

    /**
     * @brief Constructs an AllocatorBase object with the specified capacity.
     * 
     * @param capacity The capacity of the allocator.
     */
	AllocatorBase(size_t capacity) : capacity_(capacity) {
        data_ = new T[capacity];
    }

    /**
     * @brief Returns a pointer to the allocated memory.
     * 
     * @return T* A pointer to the allocated memory.
     */
    T* get() {
        return data_;
    }

    /**
     * @brief Destroys the AllocatorBase object.
     */
    ~AllocatorBase() = default;

private:
    size_t capacity_;
    T* data_;
    AllocatorType allocator_;
};

}}
#endif /* SRC_MAPS_ALLOCATORBASE_HPP_ */
