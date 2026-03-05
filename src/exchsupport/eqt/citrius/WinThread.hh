/*
 * WinThread.hh
 *
 *  Created on: Jul 29, 2011
 *      Author: val
 */

#ifndef WINTHREAD_HH_
#define WINTHREAD_HH_

#ifdef WIN32

extern "C" {
#include <winsock2.h>
}
#include <string>

namespace LimeBrokerage {

class WinThread {

    typedef void *(*StartRoutine)(void*);
public:
    // Basic operations available to other threads.
    // start execution of a fully-constructed thread
    void start()
    {
        threadHandle_ = CreateThread(
                NULL, // default security attributes
                0, // use default stack size
                (LPTHREAD_START_ROUTINE)entry_, // thread function
                this, // argument to thread function
                0, // use default creation flags
                &threadId_); // returns the thread identifier
        if (threadHandle_ == NULL) {
            //FIXME: error handling
        }
    }

    // wait for thread termination
    void join()
    {
        if (WaitForSingleObject(threadHandle_, INFINITE) != WAIT_OBJECT_0) {
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
        CloseHandle(threadHandle_);
    }

    bool isMyself() const
    {
        return threadId_ == GetCurrentThreadId();
    }
protected:
    WinThread(StartRoutine entry, std::string threadName, int cpuAffinity = -1)
        : started_(false), joined_(false), threadName_(threadName),
          cpuAffinity_(cpuAffinity), threadHandle_(0), threadId_(0), entry_(entry)
    {
    }

    ~WinThread()
    {

    }
    void setupPid() {}
    volatile bool started_;
    bool joined_;
    std::string threadName_;

private:
    WinThread(const WinThread&); // undefined - prevents unintended copy
    WinThread& operator=(const WinThread&); // undefined - prevents unintended assignment

    // Member data.
    int cpuAffinity_;
    HANDLE threadHandle_;
    DWORD threadId_;
    StartRoutine entry_;
};

typedef WinThread BaseThread;
}//namespace LimeBrokerage
#endif /* WIN32 */

#endif /* WINTHREAD_HH_ */
