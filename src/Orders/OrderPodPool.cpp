//============================================================================
// Name        	: OrderPodPool.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 27, 2023 1:06:23 PM
//============================================================================

#include <Orders/OrderPodPool.hpp>

using namespace KTN::Core;

OrderPodPool::OrderPodPool(size_t pool_size):
		_pool_lifo(pool_size),_pool_size(0),_max_size(pool_size)
{
    // Preallocate the buffers and add them to the pool
    for (size_t i = 0; i < pool_size; ++i) {
    	KTN::OrderPod* msg = new KTN::OrderPod();
        ++_pool_size;
        _pool_lifo.push(msg);
    }
}

OrderPodPool::~OrderPodPool() {
    // Deallocate any remaining buffers when the pool is destroyed
	KTN::OrderPod* msg;
    while (_pool_lifo.pop(msg)) {
       // delete[] msg->buffer;
        delete msg;
    }
}

KTN::OrderPod* OrderPodPool::get()
{
	KTN::OrderPod* ret = nullptr;
    if (_pool_lifo.pop(ret) && ret) {
    	--_pool_size;
        return ret;
    }
    _pool_size = 0;
    for (int i = 0; i < _max_size; ++i) {
		KTN::OrderPod* msg = new KTN::OrderPod();
		++_pool_size;
		_pool_lifo.push(msg);
	}

    return new KTN::OrderPod();
}

bool OrderPodPool::put(KTN::OrderPod* msg)
{
	if(_pool_lifo.push(msg))
	{
		++_pool_size;
		return true;
	}
    return false;
}
