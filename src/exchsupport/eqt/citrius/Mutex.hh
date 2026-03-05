#ifndef __MUTEX__
#define __MUTEX__

#include <exception>
#include <stdexcept>
#include <string.h>
#include <iostream>
#include <atomic>

#include "NoLock.hh"

#if defined(__linux__)
#include <cerrno>
#include <pthread.h>
#elif defined(WIN32)
#include <Windows.h>
#endif

namespace LimeBrokerage {

#if defined(WIN32)
    static inline const char *LastError() {
        static char errorMsg[64*1024];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, (LPSTR)&errorMsg, sizeof(errorMsg), NULL);
        return errorMsg;
    }
#endif

    class Mutex {
#if defined(__linux__)
    private:
        pthread_mutex_t mutex;

    public:
        typedef pthread_mutex_t * MutexHandle;

        MutexHandle getMutexHandle() { return &mutex; }
        Mutex(bool initialOwner = false) {
            if(pthread_mutex_init(&mutex, NULL) != 0)
                throw std::runtime_error(strerror(errno));

            if(initialOwner)
                lock();
        }

        ~Mutex() {
            pthread_mutex_destroy(&mutex);
        }

        int tryLock() {
            int res =  pthread_mutex_trylock(&mutex);
            if (res != 0) {
                std::cout << "pthread_mutex_lock: errno = " << errno << ", " << strerror(errno) << std::endl;
            }
            return res;
        }

        int lock() {
            int res = pthread_mutex_lock(&mutex);
            if (res != 0) {
                std::cout << "pthread_mutex_lock: errno = " << errno << ", " << strerror(errno) << std::endl;
            }
            return res;
        }

        int unlock() {
            return pthread_mutex_unlock(&mutex); 
        } 
        
#elif defined(WIN32)
    private:
        HANDLE mutex;

    public:
        typedef HANDLE MutexHandle;
        
        MutexHandle getMutexHandle() { return mutex; }

        Mutex(bool initialOwner = false) {
            mutex = CreateMutex(NULL, initialOwner, NULL);
            if(mutex == NULL) throw std::runtime_error(LastError());
        }

        ~Mutex() {
            CloseHandle(mutex);
        }

        int lock() {
            int retVal = -1;
            switch(WaitForSingleObject(mutex, INFINITE)) {
                case WAIT_OBJECT_0:
                    retVal = 0;
                    break;
                case WAIT_ABANDONED:
                    ReleaseMutex(mutex);
                default:
                    retVal=-1;
            }
            return retVal;
        }

        int tryLock() {
            int retVal = -1;
            switch(WaitForSingleObject(mutex, 0)) {
                case WAIT_OBJECT_0:
                    retVal = 0;
                    break;
                case WAIT_ABANDONED:
                    ReleaseMutex(mutex);
                default:
                    retVal=-1;
            }
            return retVal;
        }

        int unlock() {
            return !ReleaseMutex(mutex);
        }
#endif
    }; // Class Mutex


    class SpinLock {
#ifdef __linux__
    public:
        SpinLock() : lock_(false)
        {
        }
        ~SpinLock()
        {
            lock_ = false;
        }

        int tryLock()
        {
            return lock_.exchange(true) == false;
        }

        void lock()
        {
            while (lock_.exchange(true));
        }

        void unlock()
        {
            lock_.exchange(false);
        }
    private:
        std::atomic<bool> lock_;
#endif
    };


    // Read/write (n readers XOR one writer) semaphore.

    class ReadWriteLock {
#ifdef __linux__
    public:
        ReadWriteLock()
        {
            if (pthread_rwlock_init(&rwlock, 0) != 0)
                throw CreateError();
        }
        ~ReadWriteLock()
        {
            pthread_rwlock_destroy(&rwlock);
        }

        // Operations.

        // spins till it acquires a read lock
        // thread can aquire lock n times but must release n times
        void acquireRead() { pthread_rwlock_rdlock(&rwlock); }
        // spins till it acquires a write lock
        // Results are undefined if the calling thread holds the read-write lock
        // (whether a read or write lock) at the time the call is made.
        void acquireWrite() { pthread_rwlock_wrlock(&rwlock); }
        bool tryAcquireRead() { return pthread_rwlock_tryrdlock(&rwlock) == 0; }
        bool tryAcquireWrite() { return pthread_rwlock_trywrlock(&rwlock) == 0; }
        void release() { pthread_rwlock_unlock(&rwlock); }

        // Exception classes.
        class CreateError : public std::exception {
        public:
            // TODO: uncomment below when all server's g++ gets upgraded.
//            const char* what() const noexcept { return "unable to create read/write semaphore"; }
        };

    private:
        // Member data.
        pthread_rwlock_t rwlock;                // POSIX read/write lock

        // Prevent unintended copy/assignment.
        ReadWriteLock(const ReadWriteLock&);
        ReadWriteLock& operator=(const ReadWriteLock&);
#endif
    };


    // Guarded Lock template.
    template<typename Lock, bool doLock = true>
    class Guard
    {
    public:
        __attribute__((always_inline)) inline explicit Guard(Lock& _lock) : lock(_lock) { lock.lock(); }
        ~Guard() { lock.unlock(); }

    private:
        // Member data.
        Lock& lock;                   // reference to the controlled Lock

        // Private, undefined copy constructor and assignment operator used to
        // prevent unintended copy/assignment.
        Guard(const Guard&);
        Guard& operator=(const Guard&);
    };

    //Partial template instantiation to be able to create guard without lock
    template<typename Lock>
    class Guard<Lock, false>
    {
    public:
        explicit Guard(Lock& lock) {}
        ~Guard() {}
    };

    typedef Guard<SpinLock> SpinlockGuard;
    typedef Guard<Mutex> MutexGuard;
    typedef Guard<NoLock> NoLockGuard;
} //namespace LimeBrokerage
#endif //__MUTEX__
