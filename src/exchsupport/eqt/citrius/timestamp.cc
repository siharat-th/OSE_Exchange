
#include "timestamp.hh"

namespace LimeBrokerage
{

    Timestamp::Timestamp()
    {
#if defined(WIN32)
        setTimestamp((time(0) % 86400) * 1000, 0);
#elif defined (__linux__)
        struct timeval t;
        gettimeofday(&t, NULL);
        uint32_t milliseconds = (t.tv_sec % 86400) * 1000;
        milliseconds += (t.tv_usec / 1000);
        uint32_t nanoseconds = (t.tv_usec - ((t.tv_usec / 1000) * 1000)) * 1000;
        setTimestamp(milliseconds, nanoseconds);
#endif
    }

    // Will be used for implicit conversion from int.
    Timestamp::Timestamp(uint64_t nanosecondsSinceMidnight)
    {
        setTimestamp(nanosecondsSinceMidnight);
    }

    Timestamp::Timestamp(int millisecondsSinceMidnight, int nanoseconds)
    {
        setTimestamp(millisecondsSinceMidnight, nanoseconds);
    }

    void Timestamp::setTimestamp(int millisecondsSinceMidnight, int nanoseconds)
    {
        nanosecondsSinceMidnight_ = ((uint64_t)millisecondsSinceMidnight * 1000UL * 1000UL) + (uint64_t)nanoseconds;
    }

    void Timestamp::setTimestamp(uint64_t nanosecondsSinceMidnight)
    {
        nanosecondsSinceMidnight_ = nanosecondsSinceMidnight;
    }

}

