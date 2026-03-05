/*
 * Misc.cc
 *
 *  Created on: Oct 8, 2012
 *      Author: val
 */

#include "Misc.hh"

extern "C" {
#include "sys/stat.h"
#include "sys/types.h"
#include "unistd.h"
}

#include <sstream>

namespace LimeBrokerage {

std::string& Misc::getMachineName()
{

    static std::string machineName;
    if (machineName.empty()) {
        char mn[64];
        ::gethostname(mn, 64);
        machineName = mn;
    }
    return machineName;
}

int Misc::mkdir(const std::string& dirname)
{
    if ((::access(dirname.c_str(), R_OK|W_OK|X_OK) == 0)) {
        //Directory exist and it is accessible
        return 0;
    }
    int res = ::mkdir (dirname.c_str(), S_IRWXU|S_IRWXG);
    if (res < 0) return res;
    return ::access(dirname.c_str(), R_OK|W_OK|X_OK);
}

std::string Misc::concatenate(const std::string& s1, const std::string& s2, const std::string& s3, int id)
{
    std::ostringstream s;
    s << s1 << s2 << s3 << id;
    return s.str();
}
std::string Misc::concatenate(const std::string& s1, const std::string& s2, const std::string& s3)
{
    std::ostringstream s;
    s << s1 << s2 << s3;
    return s.str();
}
std::string Misc::concatenate(const std::string& s1, const std::string& s2)
{
    std::ostringstream s;
    s << s1 << s2;
    return s.str();
}

std::string Misc::concatenate(const std::string& s1, int i1, const std::string& s2)
{

    std::ostringstream s;
    s << s1 << i1 << s2;
    return s.str();
}

//This function convert uint32_t that represents milliseconds since midnight into timespec that used by QuoteHandler
struct timespec Misc::milliSecondsSinceMidnight(const char* data)
{
    struct timespec timestamp;
    uint64_t milliseconds = bigendian32toh(data);
    timestamp.tv_sec = static_cast<int>(milliseconds/1000);
    timestamp.tv_nsec = static_cast<int>(milliseconds%1000)*1000000;
    return timestamp;
}
static uint32_t todayMidnight = 0;
struct timespec Misc::currentTimeInUsecsSinceMidnight()
{
    //if not initialized, then initialize
    if(!todayMidnight)
    {
        initTodayMidnight();
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct timespec timestamp;
    timestamp.tv_sec = tv.tv_sec - todayMidnight;
    timestamp.tv_nsec = tv.tv_usec * 1000;
    return timestamp;
}

struct timespec Misc::currentTimeInMsecsSinceMidnight()
{
    struct timespec retval = currentTimeInUsecsSinceMidnight();
    retval.tv_nsec = (retval.tv_nsec/1000000)*1000000;
    return retval;
}
const uint32_t Misc::getTodayMidnight()
{
    //if not initialized, then initialize
    if(!todayMidnight)
    {
        initTodayMidnight();
    }
    return todayMidnight;
}
void Misc::initTodayMidnight()
{
    time_t now = time(0);
    struct tm* tm = localtime(&now);
    tm->tm_hour = tm->tm_min = tm->tm_sec = 0;
    todayMidnight = mktime(tm);
}

char* Misc::itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

}  // namespace LimeBrokerage
