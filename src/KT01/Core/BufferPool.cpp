//============================================================================
// Name        	: BufferPool.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 27, 2023 1:06:23 PM
//============================================================================

#include <KT01/Core/BufferPool.hpp>

using namespace KTN::Core;

BufferPool::BufferPool(size_t buffer_size, size_t pool_size):
		_pool_lifo(pool_size), _pool_size(0), _max_size(pool_size),
		_buf_size(buffer_size),_loads(0)
{
    // Preallocate the buffers and add them to the pool
	Load();
//    for (size_t i = 0; i < pool_size; ++i) {
//    	KTNBuf* msg = new KTNBuf();
//        msg->buffer = new char[buffer_size];
//        msg->buffer_length = 0;
//        memset(msg->buf,0,buffer_size);
//        ++_pool_size;
//        _pool_lifo.push(msg);
//    }
}

BufferPool::~BufferPool() {
    // Deallocate any remaining buffers when the pool is destroyed
	KTNBuf* msg;
    while (_pool_lifo.pop(msg)) {
       // delete[] msg->buffer;
        delete msg;
    }
}

void BufferPool::Load()
{
	_pool_size = 0;
	for (int i = 0; i < _max_size; ++i) {
		KTNBuf* msg = new KTNBuf();
		msg->buffer = new char[_buf_size];
		msg->buffer_length = 0;
		memset(msg->buf,0,_buf_size);
		++_pool_size;
		_pool_lifo.push(msg);
	}
	_loads++;
}

KTNBuf* BufferPool::get()
{
	if (_pool_size < 10)
		Load();

	KTNBuf* ret = nullptr;
    if (_pool_lifo.pop(ret) && ret) {
    	--_pool_size;

		ret->buffer_length = 0;
		

        return ret;
    }

    Load();

    return new KTNBuf(); // Return nullptr if pool is empty
}

bool BufferPool::put(KTNBuf* msg)
{
	//memset(msg->buf,0,msg->buffer_length);
	//msg->buffer_length = 0;
	if(_pool_lifo.push(msg))
	{
		++_pool_size;
		return true;
	}
    return false;
}
