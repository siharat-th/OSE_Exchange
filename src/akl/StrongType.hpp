#pragma once

#include <ostream>
#include <limits>

#define AKL_STRONG_TYPE(TYPE, NAME)																				\
																												\
class NAME;																										\
																												\
inline std::ostream &operator<<(std::ostream &os, const NAME &n);												\
																												\
class NAME																										\
{																												\
public:																											\
	static_assert(std::is_arithmetic<TYPE>::value,																\
		"AKL_STRONG_TYPE requires an arithmetic type");															\
																												\
	inline explicit constexpr NAME(const TYPE val) noexcept : val(val) { }										\
	inline constexpr NAME() noexcept : val(0) { }																\
	inline constexpr NAME(const NAME &rhs) noexcept : val(rhs.val) { }											\
	inline NAME &operator=(const NAME &rhs) noexcept { val = rhs.val; return *this; }							\
	inline explicit constexpr operator TYPE() const noexcept { return val; }									\
	template<typename T = TYPE>																					\
	inline explicit constexpr operator double() const noexcept													\
	{																											\
		return typename std::enable_if<!std::is_same<T, double>::value, double>::type(val);						\
	}																											\
	inline constexpr bool operator==(const NAME &rhs) const noexcept { return val == rhs.val; }					\
	inline constexpr bool operator!=(const NAME &rhs) const noexcept { return val != rhs.val; }					\
	inline constexpr bool operator<(const NAME &rhs) const noexcept { return val < rhs.val; }					\
	inline constexpr bool operator<=(const NAME &rhs) const noexcept { return val <= rhs.val; }					\
	inline constexpr bool operator>(const NAME &rhs) const noexcept { return val > rhs.val; }					\
	inline constexpr bool operator>=(const NAME &rhs) const noexcept { return val >= rhs.val; }					\
	inline NAME &operator+=(const NAME &rhs) noexcept { val += rhs.val; return *this; }							\
	inline NAME &operator-=(const NAME &rhs) noexcept { val -= rhs.val; return *this; }							\
	inline NAME &operator++() noexcept { ++val; return *this; };												\
	inline NAME operator++(int) noexcept { NAME copy = *this; ++val; return copy; }								\
	inline NAME &operator--() noexcept { --val; return *this; }													\
	inline NAME operator--(int) noexcept { NAME copy = *this; --val; return copy; }								\
	inline constexpr NAME operator-() const noexcept { return NAME(-val); }										\
	inline constexpr NAME operator+(const NAME &rhs) const noexcept { return NAME(val + rhs.val); }				\
	inline constexpr NAME operator-(const NAME &rhs) const noexcept { return NAME(val - rhs.val); }				\
	inline constexpr TYPE value() const noexcept { return val; };												\
	inline static constexpr NAME Min() { return NAME(std::numeric_limits<TYPE>::min()); }						\
	inline static constexpr NAME Max() { return NAME(std::numeric_limits<TYPE>::max()); }						\
private:																										\
	TYPE val;																									\
	friend std::ostream &operator<<(std::ostream &os, const NAME &n);											\
};																												\
																												\
inline std::ostream &operator<<(std::ostream &os, const NAME &n)												\
{																												\
	os << n.val;																								\
	return os;																									\
}
