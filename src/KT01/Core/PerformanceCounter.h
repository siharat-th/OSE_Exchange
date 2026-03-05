#ifndef PEFORMANCECOUNTER_H
#define PEFORMANCECOUNTER_H

#include <iosfwd>
#include <time.h>
#include <cstddef>
#include <sstream>

// High-resolution performance counter.
class PerformanceCounter
{
public:

	 /**
	 * @brief Represents a high-resolution performance counter value.
	 */
	typedef timespec Count;

	/**
	 * @brief Represents a high resolution time span value
	 */
	typedef long long Span;

    /**
     * @brief Sets the given performance counter value to an undefined state.
     * 
     * @param value The performance counter value to set.
     */
    static void setToUndefinedValue(Count* value);

    /**
     * @brief Checks if the given performance counter value is in an undefined state.
     * 
     * @param value The performance counter value to check.
     * @return true if the performance counter value is undefined, false otherwise.
     */
    static bool isUndefinedValue(Count& value);

    /**
     * @brief Retrieves the current value of the high-resolution performance counter.
     * 
     * @return The current performance counter value.
     */
    static Count current();

    /**
     * @brief Retrieves the current value of the high-resolution performance counter.
     * 
     * @param result The variable to store the current performance counter value.
     */
    static void current(Count* result);


    /**
     * @brief Calculates the time span in nanoseconds between two performance counter values.
     * 
     * @param stop The stop performance counter value.
     * @param start The start performance counter value.
     * @return The time span in nanoseconds.
     */
    static Span nsSpan(const PerformanceCounter::Count& stop, const PerformanceCounter::Count& start);

    /**
     * @brief Calculates the time span in microseconds between two performance counter values.
     * 
     * @param stop The stop performance counter value.
     * @param start The start performance counter value.
     * @return The time span in microseconds.
     */
    static Span usSpan(const PerformanceCounter::Count& stop, const PerformanceCounter::Count& start);

    /**
     * @brief Calculates the elapsed time in nanoseconds since the given performance counter value.
     * 
     * @param start The start performance counter value.
     * @return The elapsed time in nanoseconds.
     */
    static Span elapsedNanoseconds(const PerformanceCounter::Count& start);

    /**
     * @brief Calculates the elapsed time in microseconds since the given performance counter value.
     * 
     * @param start The start performance counter value.
     * @return The elapsed time in microseconds.
     */
    static Span elapsedMicroseconds(const PerformanceCounter::Count& start);


};

/**
 * @brief Overloads the output stream operator for printing a performance counter value.
 * 
 * @param os The output stream.
 * @param value The performance counter value to print.
 * @return The output stream.
 */
std::ostream& operator<< (std::ostream& os, const PerformanceCounter::Count& value);

/**
 * @brief Overloads the equality operator for comparing two performance counter values.
 * 
 * @param left The left-hand side performance counter value.
 * @param right The right-hand side performance counter value.
 * @return true if the performance counter values are equal, false otherwise.
 */
inline bool operator == (const PerformanceCounter::Count& left, const PerformanceCounter::Count& right)
{
    return (left.tv_nsec == right.tv_nsec && left.tv_sec == right.tv_sec);
}

/**
 * @brief Overloads the inequality operator for comparing two performance counter values.
 * 
 * @param left The left-hand side performance counter value.
 * @param right The right-hand side performance counter value.
 * @return true if the performance counter values are not equal, false otherwise.
 */
inline bool operator != (const PerformanceCounter::Count& left, const PerformanceCounter::Count& right)
{
    return !(left == right);
}

inline PerformanceCounter::Span PerformanceCounter::nsSpan(const PerformanceCounter::Count& stop, const PerformanceCounter::Count& start)
{
    return (Span)((stop.tv_sec - start.tv_sec) * 1000000000 + (stop.tv_nsec - start.tv_nsec));
}

inline PerformanceCounter::Span PerformanceCounter::usSpan(const PerformanceCounter::Count& stop, const PerformanceCounter::Count& start)
{
    return (Span)((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000);
}

inline PerformanceCounter::Span PerformanceCounter::elapsedNanoseconds(const PerformanceCounter::Count& start)
{
    return nsSpan(PerformanceCounter::current(), start);
}

inline PerformanceCounter::Span PerformanceCounter::elapsedMicroseconds(const PerformanceCounter::Count& start)
{
    return usSpan(PerformanceCounter::current(), start);
}

inline void PerformanceCounter::setToUndefinedValue(Count* value)
{
    value->tv_sec = 0;
    value->tv_nsec = 0;
}

inline bool PerformanceCounter::isUndefinedValue(Count& value)
{
    return (0 == value.tv_sec && 0 == value.tv_nsec);
}

inline void PerformanceCounter::current(PerformanceCounter::Count* result)
{
    clock_gettime(CLOCK_REALTIME, result);
}

inline PerformanceCounter::Count PerformanceCounter::current()
{
    Count result;
    current(&result);
    return result;
}


#endif /*PEFORMANCECOUNTER_H*/
