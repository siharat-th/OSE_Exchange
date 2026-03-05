//============================================================================
// Name        	: OrderPodPool2.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 27, 2023 1:06:23 PM
//============================================================================

#include <Orders/OrderPodPool2.hpp>

using namespace KTN::Core;

OrderPodPool2::OrderPodPool2(size_t pool_size):
		_pool_lifo(pool_size),_pool_size(0),_max_size(pool_size)
{
    // Preallocate the buffers and add them to the pool
    for (size_t i = 0; i < pool_size; ++i) {
    	void* ptr = boost::alignment::aligned_alloc(64, sizeof(KTN::OrderPod));
    	KTN::OrderPod* msg = new(ptr)KTN::OrderPod;
        ++_pool_size;
        _pool_lifo.push(msg);
    }
}

OrderPodPool2::~OrderPodPool2() {
    // Deallocate any remaining buffers when the pool is destroyed
	KTN::OrderPod* msg;
    while (_pool_lifo.pop(msg)) {
    	  msg->~OrderPod();  // Call destructor
    	  boost::alignment::aligned_free(msg);
    }
}

KTN::OrderPod* OrderPodPool2::get()
{
	KTN::OrderPod* ret = nullptr;
    if (_pool_lifo.pop(ret) && ret) {
    	--_pool_size;
        return ret;
    }
    _pool_size = 0;
	for (int i = 0; i < _max_size; ++i) {
	   void* ptr = boost::alignment::aligned_alloc(64, sizeof(KTN::OrderPod));
	   KTN::OrderPod* msg = new(ptr) KTN::OrderPod;  // Placement new
	   ++_pool_size;
	   _pool_lifo.push(msg);
	}

	void* ptr = boost::alignment::aligned_alloc(64, sizeof(KTN::OrderPod));
	return new(ptr) KTN::OrderPod;  // Placement new
}

bool OrderPodPool2::put(KTN::OrderPod* msg)
{
	if(_pool_lifo.push(msg))
	{
		++_pool_size;
		return true;
	}
    return false;
}
