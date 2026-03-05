#pragma once

#include <chrono>
#include <ctime>
#include <atomic>

namespace akl::stacker
{

class Time
{
public:
	using ClockType = std::chrono::steady_clock;
	using TimestampType = ClockType::time_point;
	using DurationType = ClockType::duration;

	Time() { }

	inline void SetTime()
	{
#if !AKL_TEST_MODE
		now.store(ClockType::now(), std::memory_order_release);
#endif
	}

#if AKL_TEST_MODE
	inline void SetTime(const TimestampType &t)
	{
		now.store(t, std::memory_order_release);
	}
#endif

	inline TimestampType Now() const
	{
		return now.load(std::memory_order_acquire);
	}

	static constexpr inline TimestampType Min()
	{
		return TimestampType { DurationType(0) };
	}

	static constexpr inline TimestampType Max()
	{
		return TimestampType::max();
	}

private:
	Time(const Time &) = delete;
	Time &operator=(const Time &) = delete;
	Time(const Time &&) = delete;
	Time &operator=(const Time &&) = delete;

	std::atomic<TimestampType> now;
};

using Timestamp = Time::TimestampType;
using Duration = Time::DurationType;

inline constexpr Duration Nanos(const int64_t n)
{
	return std::chrono::nanoseconds(n);
}

inline constexpr Duration Micros(const int64_t n)
{
	return std::chrono::duration_cast<Duration>(std::chrono::microseconds(n));
}

inline constexpr Duration Millis(const int64_t n)
{
	return std::chrono::duration_cast<Duration>(std::chrono::milliseconds(n));
}

inline constexpr Duration Seconds(const int64_t n)
{
	return std::chrono::duration_cast<Duration>(std::chrono::seconds(n));
}

}