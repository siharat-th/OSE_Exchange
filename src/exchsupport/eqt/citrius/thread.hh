#ifndef THREAD_HH_
#define THREAD_HH_
#include <iostream>
#include "WinThread.hh"
#include "LinuxThread.hh"
#include "Mutex.hh"

namespace LimeBrokerage {
class NullSemaphore
{
public:
    NullSemaphore() {}
    ~NullSemaphore() {}
    void acquire() const {}
    void release() const {}
};


#ifdef   __linux__
template <class Runnable, typename PlatformThread=LinuxThread>
#endif
#ifdef WIN32
template <class Runnable, typename PlatformThread=WinThread>
#endif
class Thread : public PlatformThread
{
public:
	Thread(string threadName = "Unknown", int cpuAffinity = -1)
	: PlatformThread(entry, threadName, cpuAffinity), runner_(new Runnable()), done_(false), deallocateRunner_(true)
	{
	}
    Thread(Runnable* runner, string threadName, int cpuAffinity = -1)
    : PlatformThread(entry, threadName, cpuAffinity), runner_(runner), done_(false), deallocateRunner_(false)
    {
    }
    ~Thread()
    {
    	if (deallocateRunner_) delete runner_;
    }
    void shutdown()
    {
        runner_->shutdown();
//        done_ = true;
        this->interrupt();

    }

    Runnable& getRunnable() { return *runner_; }
private:
    void drive()
    {
        this->started_ = true;
//        while (!done_) {
            runner_->run();
//        }
    }
    // common entry point for all threads
    static void* entry(void* p)
    {
        Thread* t = static_cast<Thread*>(p);
        t->setupPid();
        t->drive();
        return p;
    }

    Thread(const Thread&);                  // undefined - prevents unintended copy
    Thread& operator=(const Thread&);       // undefined - prevents unintended assignment

    // Member data.
    Runnable* runner_;
    bool done_;
    bool deallocateRunner_;
    SpinLock spinLock_;
};

}

#endif /*THREAD_HH_*/
