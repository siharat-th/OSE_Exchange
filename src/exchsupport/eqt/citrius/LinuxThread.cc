/*
 * LinuxThread.cc
 *
 *  Created on: May 31, 2012
 *      Author: val
 */
#include "LinuxThread.hh"
#include "SystemCallException.hh"

extern "C"
{
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <errno.h>
}

#include <iomanip>

namespace LimeBrokerage
{

// Basic operations available to other threads.
// start execution of a fully-constructed thread
void LinuxThread::start()
{
    if (started_) return;
    int res = pthread_create(&pthread_, 0, entry_, this);
    if (res != 0) {
        throw SystemCallException(errno, "pthread_create failed");
        return;
    }
    setCpuAffinity(cpuAffinity_);
    //Wait until thread is started Thread::drive is called to make sure pid is set
    while (!started_) ;

}

// wait for thread termination
void LinuxThread::join()
{
    if (joined_) return;
    int retVal = pthread_join(pthread_, 0);
    if (retVal == 0) joined_ = true;
}

bool LinuxThread::isMyself() const
{
    return pthread_ == pthread_self();
}

void LinuxThread::setupPid()
{
    pid_ = ::syscall(SYS_gettid);
}

void LinuxThread::setCpuAffinity(int cpuAffinity)
{
    cpuAffinity_ = cpuAffinity;
    LinuxThread::setCpuAffinity(cpuAffinity_, pthread_);
}

void LinuxThread::setCpuAffinity(int cpuAffinity, pthread_t pthread)
{
    if (cpuAffinity < 0 || cpuAffinity > maxAvailableCores()) return;

    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpuAffinity, &cpu_set);
    pthread_setaffinity_np(pthread, sizeof(cpu_set), &cpu_set);
}

int LinuxThread::maxAvailableCores()
{
    static int maxAvailableCores_ = 0;
    if (maxAvailableCores_ != 0) return maxAvailableCores_;
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);

    /*
     * sched_getaffinity returns the set of cores that the current
     * process id is allowed to run on. If the current process is not
     * pinned to any particular set of cores, this function should return
     * the set of total available cores in the system.
     *
     * get the number of available cores in the system
     */
    sched_getaffinity(0, sizeof(cpu_set), &cpu_set);

    for (int i=0;i < CPU_SETSIZE;i++)
        if (CPU_ISSET(i, &cpu_set))
            maxAvailableCores_++;

    return maxAvailableCores_;
}

void LinuxThread::interrupt() const
{
    pthread_kill(pthread_, SIGUSR1);
}

ostream& operator<<(ostream& stream, const LinuxThread& thread)
{
    stream << "ThreadInfo, " << thread.getName() << "," << std::setw(10 - thread.getName().size()) << " ";
    int len = 1;
    int af = thread.getCpuAffinity();
    if (af >= 0 && af < 10) {
        stream << af;
    }
    else if ( af >= 10) {
        len = 2;
        stream << af;
    }
    else if (af < 0) {
        len = 3;
        stream << "ANY";
    }
    stream << "," << std::setw(4-len) << " " << thread.getPid() << endl;
    return stream;
}

} //namespace LimeBrokerage
