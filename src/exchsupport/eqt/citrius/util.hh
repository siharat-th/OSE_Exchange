#ifndef _UTIL_HH_
#define _UTIL_HH_

#if defined (__linux__)
#include <cerrno>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#elif defined (_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#endif

namespace LimeBrokerage {
class Utility {
public:
    static void sleep(int sleep_time);

    static char *getErrorMessage() ;
    static char *getErrorMessage(int error) ;

    static int getLastError();

    static bool isError(int retVal);
    static bool isErrorBadFileDesc();

    static bool isErrorInterrupted();

    static bool isErrorWouldBlock();
    static bool isErrorWouldBlock(int error);

    static bool isErrorInProgress();
    static bool isErrorInProgress(int error);

    static bool isErrorAlready();
    static bool isErrorAlready(int error);

    static bool isErrorConnected();
    static bool isErrorConnected(int error);

    static int getSoError(int fd);

    static bool isSocketError(int returnCode);

    static int gettimeofday(struct timeval* tv, struct timezone* x);
};//class Utility
}//namespace LimeBrokerage
#endif // _UTIL_HH_
