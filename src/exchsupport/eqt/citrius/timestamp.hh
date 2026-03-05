
#ifndef _LIME_TIMESTAMP_H_
#define _LIME_TIMESTAMP_H_

#include <ostream>
#include <iomanip>
#include "limetypes.hh"

#if defined(WIN32)
#include <time.h>
#elif defined (__linux__)
#include <sys/time.h>
#endif

namespace LimeBrokerage
{


/**
 * A container for timestamp data.
 */
class Timestamp {
public:

    /**
     * The default constructor initializes a Timestamp to the current UTC time
     * elapsed since midnight.  The expected granularity is usecs in Linux or
     * secs in Windows.  However most Timestamp objects will be initialzied
     * to contain a nanosecondSinceMidngiht value in the most accurate
     * granularity available.
     */
    Timestamp();

    /**
     * Initialize timestamp using value of nanoseconds elsapsed since midnight.
     */
    Timestamp(uint64_t nanosecondsSinceMidnight);

    /**
     * Initialize timestamp using value of milliseconds elsapsed since
     * midnight and nanoseconds elapsed since last millisecond.
     */
    Timestamp(int millisecondsSinceMidnight, int nanoseconds);

    ~Timestamp() { }

    /**
     * Get the value of this timestamp expressed in the number of nanoseconds
     * elapsed since midnight.
     *
     */
    uint64_t getNsecsSinceMidnight() const { return nanosecondsSinceMidnight_; }

private:
    void setTimestamp(int millisecondsSinceMidnight, int nanoseconds);
    void setTimestamp(uint64_t nanosecondsSinceMidnight);
    uint64_t nanosecondsSinceMidnight_;
}; /* class Timestamp */

} /* namespace LimeBrokerage */

#endif
