/*
 * util.cc
 *
 *  Created on: Jun 7, 2011
 *      Author: val
 */
#include "util.hh"
#include "time.h"

#ifdef __linux__
extern "C" {
#include <sys/time.h>
}
#endif
namespace LimeBrokerage {
void Utility::sleep(int sleep_time)
{
#if defined (__linux__)
    int time_remaining = sleep_time;
    while (time_remaining) {
        time_remaining = ::sleep(sleep_time);
    }
#elif defined (_WIN32)
    Sleep(sleep_time * 1000);
#endif
}

#if defined(_WIN32)
     static char errorMsg[32*1024];
#endif

char* Utility::getErrorMessage()
{
    return getErrorMessage(getLastError());
}

char* Utility::getErrorMessage(int error) {
#if defined(__linux__)
    return strerror(error);
#elif defined(_WIN32)
    DWORD res = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,NULL,(DWORD)error,LANG_SYSTEM_DEFAULT,(LPSTR)errorMsg,sizeof(errorMsg),NULL);
    return (res > 0) ? errorMsg : "";
#endif
}

int Utility::getLastError()
{
#if defined (__linux__)
    return errno;
#elif defined (_WIN32)
    return GetLastError();
#endif
}

bool Utility::isError(int retVal)
{
#if defined (__linux__)
    return (retVal < 0);
#elif defined (_WIN32)
    return (retVal == SOCKET_ERROR);
#endif
}

bool Utility::isErrorBadFileDesc()
{
#if defined (__linux__)
    return (errno == EBADF);
#elif defined (_WIN32)
    return (WSAGetLastError() == WSAENOTSOCK);
#endif
}

bool Utility::isErrorInterrupted()
{
#if defined (__linux__)
    return (errno == EINTR);
#elif defined (_WIN32)
    return (WSAGetLastError() == WSAEINTR);
#endif
}

bool Utility::isErrorWouldBlock()
{
    return isErrorWouldBlock(getLastError());
}

bool Utility::isErrorWouldBlock(int error)
{
#if defined (__linux__)
    return (error == EAGAIN);
#elif defined (_WIN32)
    return (error == WSAEWOULDBLOCK);
#endif
}

bool Utility::isErrorInProgress()
{
    return isErrorInProgress(getLastError());
}

bool Utility::isErrorInProgress(int error)
{
#if defined (__linux__)
    return (error == EINPROGRESS);
#elif defined (_WIN32)
    return (error == WSAEINPROGRESS);
#endif
}

bool Utility::isErrorAlready()
{
    return isErrorAlready(getLastError());
}

bool Utility::isErrorAlready(int error)
{
#if defined (__linux__)
    return (error == EALREADY);
#elif defined (_WIN32)
    return (error == WSAEALREADY);
#endif
}

bool Utility::isErrorConnected()
{
    return isErrorConnected(getLastError());
}

bool Utility::isErrorConnected(int error)
{
#if defined (__linux__)
    return (error == EISCONN);
#elif defined (_WIN32)
    return (error == WSAEISCONN);
#endif
}
int Utility::getSoError(int fd)
{
    int error = -1;
#if defined(__linux__)
    socklen_t size = sizeof(error);
    int res = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &size);
#elif defined (_WIN32)
    int size = sizeof(error);
    int res = getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&error, &size);
#endif
    return res == 0 ? error : getLastError();
}

bool Utility::isSocketError(int returnCode)
{
#if defined(__linux__)
    return returnCode < 0;
#elif defined(_WIN32)
    return returnCode == SOCKET_ERROR;
#endif

}

#if defined(_WIN32)
static time_t secsToMidnight = -1;
#endif
int Utility::gettimeofday(timeval *tv, struct timezone *x)
{
#if defined(__linux__)
    return ::gettimeofday(tv, x);
#elif defined(_WIN32)
    if (secsToMidnight == -1) {
        time_t now = time(NULL);
        struct tm *today = localtime(&now);
        today->tm_sec = 0;
        today->tm_min = 0;
        today->tm_hour = 0;
        secsToMidnight = mktime(today);
    }
    SYSTEMTIME st;
    GetLocalTime(&st);
    tv->tv_sec = ((st.wHour) * 60 + st.wMinute) * 60 + st.wSecond + secsToMidnight;
    tv->tv_usec = st.wMilliseconds * 1000;
    return 0;
#endif
}
}
