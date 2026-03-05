#ifndef __EVENT__
#define __EVENT__

#include <exception>
#include <stdexcept>

#if defined(__linux__)
#include <cerrno>
extern "C" {
#include <pthread.h>
}
#elif defined(WIN32)
extern "C" {
#include <Windows.h>
}
#endif

#include "Mutex.hh"

namespace LimeBrokerage {

    class Event {
    private:
        Mutex *pMutex;

#if defined (__linux__)
        pthread_cond_t event;

    public:
        Event(Mutex *pMutex_) :
            pMutex(pMutex_)
        {
            if (pMutex == NULL) throw std::range_error("NULL Mutex Argument");
            if (pthread_cond_init(&event, NULL) != 0) throw std::runtime_error(strerror(errno));
        }

        ~Event()
        {
            pthread_cond_destroy(&event);
        }

        int wait()
        {
            return pthread_cond_wait(&event, pMutex->getMutexHandle());
        }

        int notify()
        {
            return pthread_cond_signal(&event);
        }

        int notifyAll()
        {
            return pthread_cond_broadcast(&event);
        }
#elif defined(WIN32)
        HANDLE event;

    public:
        Event(Mutex *pMutex_): pMutex(pMutex_)
        {
            if (pMutex == NULL) throw std::range_error("NULL Mutex Argument");
            event = CreateEvent(NULL, FALSE, FALSE, NULL);
            if (event == NULL) throw std::runtime_error(LastError());
        }

        ~Event ()
        {
            CloseHandle(event);
        }

        int wait()
        {
            ResetEvent(event);
            SignalObjectAndWait(pMutex->getMutexHandle(), event, INFINITE, false);
            return pMutex->lock();
        }

        int notify()
        {
            return !SetEvent(event);
        }

        int notifyAll()
        {
            return notify();
        }
#endif
    }; // class Event
} // namespace LimeBrokerage
#endif /* __EVENT__ */
