//============================================================================
// Name        	: HugePageAllocator.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 29, 2023 5:10:10 PM
//============================================================================

#ifndef HUGE_PAGE_ALLOCATOR_HPP
#define HUGE_PAGE_ALLOCATOR_HPP
#pragma once
#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <unistd.h> // For mmap
#include <sys/mman.h> // Include the mmap-related header
using namespace std;
#include <KT01/Core/Macro.hpp>


namespace KTN{ namespace Core{

#define huge_page_size    (2ll * 1024 * 1024)
#define USE_HUGE_PAGE 1

#define ROUND_UP(p, align)   (((p)+(align)-1u) & ~((align)-1u))
#define IS_POW2(n)           (((n) & (n - 1)) == 0)


template <typename T>
class HugePageAllocator {
public:
    HugePageAllocator(size_t capacity) : capacity_(capacity) {
        data_ = new T[capacity];
		// Allocate huge pages
//    	size_t alloc_size = capacity * sizeof(T);
//    	alloc_size = ROUND_UP(alloc_size, huge_page_size);
//
//		void* ptr = mmap(nullptr, sizeof(T) * capacity, PROT_READ | PROT_WRITE, MAP_ANONYMOUS
//				| MAP_HUGETLB | MAP_PRIVATE, -1, 0);
//    	if (ptr == MAP_FAILED) {
//			cout << "mmap() failed. Are huge pages configured?" << endl;
//			std::exit(EXIT_FAILURE);
//		}
//
//		/* Allocate huge-page-aligned memory to give best chance of allocating
//		 * transparent huge-pages.
//		 */
//		//posix_memalign((void**)ptr, huge_page_size, capacity * sizeof(T));
//
//
//		data_ = static_cast<T*>(ptr);
    }

    T* get() {
        return data_;
    }

    ~HugePageAllocator() {
        delete[] data_;
    }

private:
    size_t capacity_;
    T* data_;
};

}}
#endif

