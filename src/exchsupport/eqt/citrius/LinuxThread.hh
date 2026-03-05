/*
 * LinuxThread.hh
 *
 *  Created on: Jul 29, 2011
 *      Author: val
 */

#ifndef LINUXTHREAD_HH_
#define LINUXTHREAD_HH_

#include <iostream>
#include <string>
using namespace std;

#ifdef __linux__

extern "C" {
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/syscall.h>
#include <sys/types.h>
}

namespace LimeBrokerage {

// Thread - a minimal multiplatform thread class.
class LinuxThread {
    typedef void *(*StartRoutine)(void*);

public:
    // Basic operations available to other threads.
    // start execution of a fully-constructed thread
    void start();

    virtual void shutdown() {}

    // wait for thread termination
    void join();

    bool isMyself() const;

    const string& getName() const { return threadName_; }
    pthread_t getThreadId() const { return pthread_; }

    int getCpuAffinity() const { return cpuAffinity_; }

    pid_t getPid() const { return pid_; }

    void setupPid();

    void interrupt() const;
    void setCpuAffinity(int cpuAffinity);

    static void setCpuAffinity(int cpuAffinity, pthread_t pthread);

    static int maxAvailableCores();

    //Thread Wrapper
    LinuxThread(const std::string& name, int cpuAffinity)
    : started_(true), joined_(false), threadName_(name),
      cpuAffinity_(cpuAffinity) , pthread_(::pthread_self()), entry_(0), pid_(::syscall(SYS_gettid))
    {
        setCpuAffinity(cpuAffinity_, pthread_);
    }

    virtual ~LinuxThread()
    {

    }

protected:
    // 'Tors.
    LinuxThread(StartRoutine entry, string& threadName, int cpuAffinity = -1)
    : started_(false), joined_(false), threadName_(threadName),
      cpuAffinity_(cpuAffinity) , pthread_(0), entry_(entry), pid_(-1)
    {

    }

    volatile bool started_;
    bool joined_;
    string threadName_;

private:
    LinuxThread(const LinuxThread&);                  // undefined - prevents unintended copy
    LinuxThread& operator=(const LinuxThread&);       // undefined - prevents unintended assignment

    // Member data.
    int cpuAffinity_;
    pthread_t pthread_;
    StartRoutine entry_;
    pid_t pid_; //process id visible by top

};

ostream& operator<<(ostream& stream, const LinuxThread& thread);

typedef LinuxThread BaseThread;

}
#endif
#endif /* LINUXTHREAD_HH_ */
