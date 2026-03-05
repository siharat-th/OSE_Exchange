#pragma once

#include <cmath>

namespace akl
{

template<typename T, int CAPACITY>
class CircularArray
{
public:
	CircularArray() { }
	CircularArray(const T &initialValue)
	{
		for (int i = 0; i < CAPACITY; ++i)
		{
			data[i] = initialValue;
		}
	}
	CircularArray(const T &initialValue, int count)
	{
		size = std::min(count, CAPACITY);
		for (int i = 0; i < size; ++i)
		{
			data[i] = initialValue;
		}
	}

	inline int Size() const noexcept { return size; }
	inline constexpr int Capacity() const noexcept { return CAPACITY; }
	inline bool Empty() const noexcept { return size == 0; }
	inline bool Full() const noexcept { return size == CAPACITY; }

	// It is up to the user to make sure the index is within range.
	// Calling these with an out-of-bounds index is undefined behavior.
	inline T &operator[](int index)
	{
		index += start;
		return index >= CAPACITY ? data[index - CAPACITY] : data[index];
	}
	inline const T &operator[](int index) const
	{
		index += start;
		return index >= CAPACITY ? data[index - CAPACITY] : data[index];
	}

	// It is up to the user to make sure the container is not empty.
	// Calling these on an empty container is undefined behavior.
	inline T &Front() { return (*this)[0]; }
	inline const T &Front() const { return (*this)[0]; }
	inline T &Back() { return (*this)[size - 1]; }
	inline const T &Back() const { return (*this)[size - 1]; }

	inline void Clear() noexcept
	{
		start = 0;
		size = 0;
	}

	inline bool PushFront(const T &value) noexcept
	{
		if (!Full())
		{
			--start;
			if (start < 0)
			{
				start += CAPACITY;
			}
			data[start] = value;
			++size;
			return true;
		}
		else
		{
			return false;
		}
	}

	inline bool PopFront() noexcept
	{
		if (!Empty())
		{
			++start;
			if (start >= CAPACITY)
			{
				start -= CAPACITY;
			}
			--size;
			return true;
		}
		else
		{
			return false;
		}
	}

	inline bool PushBack(const T &value) noexcept
	{
		if (!Full())
		{
			int index = start + size;
			if (index >= CAPACITY)
			{
				index -= CAPACITY;
			}
			data[index] = value;
			++size;
			return true;
		}
		else
		{
			return false;
		}
	}

	inline bool PopBack() noexcept
	{
		if (!Empty())
		{
			--size;
			return true;
		}
		else
		{
			return false;
		}
	}

	inline bool InsertAt(const int index, const T &value)
	{
		if (!Full() && index <= size)
		{
			for (int i = size - 1; i >= index; --i)
			{
				(*this)[i + 1] = (*this)[i];
			}
			(*this)[index] = value;
			++size;
			return true;
		}
		else
		{
			return false;
		}
	}

	inline bool RemoveAt(const int index)
	{
		if (index >= 0 && index < size)
		{
			for (int i = index; i < size - 1; ++i)
			{
				(*this)[i] = (*this)[i + 1];
			}
			--size;
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	T data[CAPACITY];
	int start { 0 };
	int size { 0 };
};

}