#pragma once

#include <cmath>
#include <cstring>
#include <akl/BranchPrediction.hpp>

namespace akl
{

template<typename T, int CAPACITY>
class FixedSizeVector
{
public:
	FixedSizeVector() { }
	FixedSizeVector(const T &initialValue)
	{
		for (int i = 0; i < CAPACITY; ++i)
		{
			data[i] = initialValue;
		}
	}
	FixedSizeVector(const T &initialValue, int count)
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
	inline T &operator[](const int index) { return data[index]; }
	inline const T &operator[](const int index) const { return data[index]; }

	// It is up to the user to make sure the container is not empty.
	// Calling these on an empty container is undefined behavior.
	inline T &Front() { return data[0]; }
	inline const T &Front() const { return data[0]; }
	inline T &Back() { return data[size - 1]; }
	inline const T &Back() const { return data[size - 1]; }

	inline void Clear() noexcept { size = 0; }
	inline void PushBack(const T &value) { data[size++] = value; }

	inline void PopBack() noexcept
	{
		if (likely(size > 0))
		{
			--size;
		}
	}

	inline bool operator==(const FixedSizeVector &rhs) const
	{
		if (size != rhs.size)
		{
			return false;
		}
		return Empty() || memcmp(data, rhs.data, size * sizeof(T)) == 0;
	}

	inline bool operator!=(const FixedSizeVector &rhs) const
	{
		return !(*this == rhs);
	}

private:
	T data[CAPACITY];
	int size { 0 };
};

}