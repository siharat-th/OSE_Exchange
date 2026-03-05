/*
 * Misc.hh
 *
 *  Created on: Oct 8, 2012
 *      Author: val
 */

#ifndef MISC_HH_
#define MISC_HH_

#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <inttypes.h>
extern "C" {
#include <sys/time.h>
}

namespace LimeBrokerage {
namespace Helpers {
static const int8_t base62ParseTable[] =
        {
            -1, -1, -1, -1, -1, -1, -1, -1, /* 0 -> 7 */
            -1, -1, -1, -1, -1, -1, -1, -1, /* 8 -> 15 */
            -1, -1, -1, -1, -1, -1, -1, -1, /* 16 -> 23 */
            -1, -1, -1, -1, -1, -1, -1, -1, /* 24 -> 31 */
            -1, -1, -1, -1, -1, -1, -1, -1, /* 32 -> 39 */
            -1, -1, -1, -1, -1, -1, -1, -1, /* 40 -> 47 */
            0, 1, 2, 3, 4, 5, 6, 7,  /* 48 -> 55 */
            8, 9, -1, -1, -1, -1, -1, -1, /* 56 -> 63 */
            -1, 10, 11, 12, 13, 14, 15, 16, /* 64 -> 71 */
            17, 18, 19, 20, 21, 22, 23, 24, /* 72 -> 79 */
            25, 26, 27, 28, 29, 30, 31, 32, /* 80 -> 87 */
            33, 34, 35, -1, -1, -1, -1, -1, /* 88 -> 95*/
            -1, 36, 37, 38, 39, 40, 41, 42, /* 96 -> 103 */
            43, 44, 45, 46, 47, 48, 49, 50, /* 104 -> 111 */
            51, 52, 53, 54, 55, 56, 57, 58, /* 112 -> 119 */
            59, 60, 61, -1, -1, -1, -1, -1, /* 120 -> 127*/
        };
}


class Misc
{
public:
    static std::string& getMachineName();

    static int mkdir(const std::string& dirname);

    static std::string concatenate(const std::string&, const std::string&, const std::string&, int id);
    static std::string concatenate(const std::string&, const std::string&, const std::string&);
    static std::string concatenate(const std::string&, const std::string& s2);
    static std::string concatenate(const std::string&, int id, const std::string&);

    template <typename Type>
    static void clearVector(std::vector<Type*>& collection)
    {
        while(!collection.empty()) {
            delete collection.back();
            collection.pop_back();
        }
    }

    template<typename T, size_t N>
    static size_t arraylen( T(&)[N] )
    { return N; }

    //Accessing data
    static inline uint16_t bigendian16toh(const char* data) {return be16toh(*(reinterpret_cast<const uint16_t*>(data)));}
    static inline uint32_t bigendian32toh(const char* data) {return be32toh(*(reinterpret_cast<const uint32_t*>(data)));}
    static inline uint64_t bigendian64toh(const char* data) {return be64toh(*(reinterpret_cast<const uint64_t*>(data)));}

    static inline uint16_t toNetworkOrder(uint16_t val) { return htobe16(val); }
    static inline uint32_t toNetworkOrder(uint32_t val) { return htobe32(val); }
    static inline uint64_t toNetworkOrder(uint64_t val) { return htobe64(val); }

    static inline uint16_t toNetworkOrder(int16_t val) { return htobe16(val); }
    static inline uint32_t toNetworkOrder(int32_t val) { return htobe32(val); }
    static inline uint64_t toNetworkOrder(int64_t val) { return htobe64(val); }

    static inline uint32_t timespecToMillis(const timespec& ts) { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds{ts.tv_sec} + std::chrono::nanoseconds{ts.tv_nsec}).count(); }

    static inline uint32_t toMillis(const struct timespec& timestamp)
    { return ((timestamp.tv_sec*1000)+(timestamp.tv_nsec/1000000)); }
    static inline uint32_t toNanos(const struct timespec& timestamp)
    { return (timestamp.tv_nsec % 1000000); }
    static inline uint32_t toNetworkOrderMills(const timespec& timestamp)
    { return Misc::toNetworkOrder( (uint32_t)((timestamp.tv_sec*1000)+(timestamp.tv_nsec/1000000)) ); }
    static inline uint32_t toNetworkOrderNanos(const struct timespec& timestamp)
    { return Misc::toNetworkOrder((uint32_t)(timestamp.tv_nsec % 1000000)); }

    static inline uint16_t toHostOrder(uint16_t val) { return be16toh(val); }
    static inline uint32_t toHostOrder(uint32_t val) { return be32toh(val); }
    static inline uint64_t toHostOrder(uint64_t val) { return be64toh(val); }

    static inline uint16_t toHostOrder(int16_t val) { return be16toh(val); }
    static inline uint32_t toHostOrder(int32_t val) { return be32toh(val); }
    static inline uint64_t toHostOrder(int64_t val) { return be64toh(val); }

    static struct timespec milliSecondsSinceMidnight(const char*);
    static const uint32_t DAY_SECS = 60*60*24;

    static const std::string to_string(int i) {
        std::ostringstream ostr;
        ostr << i;
        return ostr.str();
    }

    static void stringTrimRight(std::string& s, const char* delimiters = " \f\n\r\t\v" )
    {
        s = s.substr( 0, s.find_last_not_of( delimiters ) + 1 );
    }

    static void stringTrimLeft(std::string& s, const char* delimiters = " \f\n\r\t\v" )
    {
        s = s.substr( s.find_first_not_of( delimiters ) );
    }

    static void stringTrim(std::string& s, const char* delimiters = " \f\n\r\t\v" )
    {
        stringTrimLeft(s, delimiters);
        stringTrimRight(s, delimiters);
    }

    //base conversions
    template <typename T>
    static void asciiBase62toBase10(const char* data, const size_t length, T* retval)
    {
        static const uint16_t tableSize = sizeof(Helpers::base62ParseTable)/sizeof(int8_t);
        *retval = 0;
        for (size_t i = 0; i < length; ++i) {
            unsigned char c = static_cast<unsigned char>(*data++);
            if (c == ' ') continue;
            int8_t val = (c < tableSize) ? Helpers::base62ParseTable[c] : -1;
            if (val < 0) {
                *retval = 0;
                return;
            }
            *retval = val + (*retval)*62;
        }
        return;
    }

    //Get current time in usecs since midnight
    static struct timespec currentTimeInUsecsSinceMidnight();
    //Get current time in msecs since midnight
    static struct timespec currentTimeInMsecsSinceMidnight();
    //Get time from EPOCH to today midnight
    static const uint32_t getTodayMidnight();

    //convert an int to a c-string
    static char* itoa(int value, char* result, int base = 10);

    static int asciiToUint8(const char *data, const int num_chars, uint8_t* retval) {
        return asciiToUint<uint8_t>(data, num_chars, retval);
    }
    static int asciiToUint16(const char *data, const int num_chars, uint16_t* retval) {
        return asciiToUint<uint16_t>(data, num_chars, retval);
    }
    static int asciiToUint32(const char *data, const int num_chars, uint32_t* retval) {
        return asciiToUint<uint32_t>(data, num_chars, retval);
    }
    static int asciiToUint64(const char *data, const int num_chars, uint64_t* retval) {
        return asciiToUint<uint64_t>(data, num_chars, retval);
    }

private:
    // Time in msecs since midnight today.
    // Gets initialized the first time currentTimeInMsecsSinceMidnight()
    static void initTodayMidnight();

    /**
     * Converts the string argument into its 64 bit unsigned integer representation.
     *
     * @param data
     *  value (in ascii) to convert.
     * @param num_chars
     *  number of characters.
     * @param retval
     *  return value. 64 bit binary representation of <data>.
     *
     * @return
     *  0 if value was successfully converted, -1 otherwise.
     */
    template <typename T>
    static int asciiToUint(const char *data, const int num_chars, T* retval) {
        static char space = ' ';
        T value = 0;

        int i;
        for (i = 0; i < num_chars; i++, data++) {   // loop until you find a digit
            if (*data == space) {   // space
                continue;   // loop
            } else if (isdigit(*data)) {  // found a digit!
                break;  // break from loop
            } else {
                return -1;  // not a space or digit, error!
            }
        }

        for ( ; i < num_chars; i++, data++) {   // loop on digits
            value *= 10;
            int ret = *data - '0';
            if (ret >= 0 && ret <= 9) {
                value += ret;
            } else {
                return -1;
            }
        }

        *retval = value;

        return 0;
    }
};

}  // namespace LimeBrokerage

#endif /* MISC_HH_ */
