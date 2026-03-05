#pragma once

#include <boost/lockfree/queue.hpp>

namespace akl::stacker
{

template<typename T>
class MemoryPool
{
public:
	MemoryPool(const int count, const int blockSize)
	: pool(count)
	, count(count)
	, blockSize(blockSize)
	{
		Refresh();
	}

	T *Get()
	{
		T *obj = nullptr;
		if (likely(pool.pop(obj)))
		{
			--size;
			return obj;
		}
		Refresh();
		if (likely(pool.pop(obj)))
		{
			--size;
			return obj;
		}
		return nullptr;
	}

	void Refresh()
	{
		while (size < count)
		{
			T *block = new T[blockSize];
			size += blockSize;
			for (int i = 0; i < blockSize; ++i)
			{
				pool.push(&block[i]);
			}
		}
	}

	int Size() const
	{
		return size.load(std::memory_order_relaxed);
	}

private:
	MemoryPool(const MemoryPool &) = delete;
	MemoryPool &operator=(const MemoryPool &) = delete;
	MemoryPool(MemoryPool &&) = delete;
	MemoryPool &operator=(MemoryPool &&) = delete;
	
	boost::lockfree::queue<T *> pool;
	std::atomic_int size { 0 };
	const int count;
	const int blockSize;
};

}