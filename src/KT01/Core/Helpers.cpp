
#include "Helpers.h"

#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <signal.h>
#include <pthread.h>

// Please note that an SSL-enabled engine could receive SIGPIPE when there are
// network issues. Unless handled this signal just breaks the process, so we
// need to suppress it (or handle it by some other way).

// The code below illustrates how to manage some chosen signals:
// - Prevent all threads against signal receisigemptyset(&signalSet);ving;
// - Establish separate thread to catch signals;
// - Track & suppress chosen signal(s) in this separate thread.

// Vector to keep suppressed signal numbers,
// it is used just for initialization of entire
// signal handling scheme.
typedef std::vector<int> SuppressedSignals;
SuppressedSignals suppressedSignals;

// Set of suppressed signals,
// used to block signals at inherited threads
// and to wait signals at signalWaiter() thread function.
sigset_t signalSet;

void errorAbort (int status, const char* reason)
{
    printf ("Aborted due to status %d: %s\n", status, reason ? reason : "no error message");
    exit (1);
}

void setThreadName(const char* name)
{
#if __GNUC__ > 4 || \
    (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || \
                      (__GNUC_MINOR__ == 4 && \
                      __GNUC_PATCHLEVEL__ > 0)))


    pthread_setname_np(pthread_self(), name);

#endif
}

// Suppress signals enumerated at the SuppressedSignals vector.
void* signalWaiter (void* arg)
{
    setThreadName("sigwait");

    int signalNumber = 0;

    while (1)
    {
        sigwait (&signalSet, &signalNumber);
        if (SIGINT == signalNumber || SIGTERM == signalNumber)
        {
            printf ("Signal %d received, exit from the application.\n", signalNumber);
            exit (EXIT_SUCCESS);
        }
        else
            printf ("Signal %d received and suppressed.\n", signalNumber);
    }
    return NULL;
}

void waitUntilSignal()
{
    int signalNumber = 0;

    while (1)
    {
        sigwait (&signalSet, &signalNumber);
        if (SIGINT == signalNumber || SIGTERM == signalNumber)
        {
            printf ("Signal %d received, exit from waiting.\n", signalNumber);
            return;
        }
        else
            printf ("Signal %d received and suppressed.\n", signalNumber);
    }
}

void manageLinuxSignals()
{
    setThreadName("main");

    // Initialize a set of suppressed signals, e.g. - signals which we are waiting for.
    suppressedSignals.push_back(SIGINT);
    //

    pthread_t signalThreadId;
    int status;

    // Start by masking suppressed signals at the primary thread. All other
    // threads inherit this signal mask and therefore will have the same signals
    // suppressed.
    sigemptyset(&signalSet);

    for(SuppressedSignals::const_iterator i = suppressedSignals.begin(), e = suppressedSignals.end();
            i != e; ++i)
    {
        sigaddset(&signalSet, *i);
    }

    sigfillset (&signalSet);

    sigdelset(&signalSet, SIGBUS);
    sigdelset(&signalSet, SIGSEGV);
    sigdelset(&signalSet, SIGABRT);
    sigdelset(&signalSet, SIGFPE);
    sigdelset(&signalSet, SIGILL);
    sigdelset(&signalSet, SIGQUIT);
    sigdelset(&signalSet, SIGTRAP);

    // Install the signal mask against primary thread.
    status = pthread_sigmask (SIG_BLOCK, &signalSet, NULL);
    if (status != 0)
        errorAbort (status, "Set signal mask");

    // Create the sigwait thread.
    // status = pthread_create (&signalThreadId, NULL, signalWaiter, NULL);
    // if (status != 0)
    //     errorAbort (status, "Create signalWaiter");

}



void waitUntilEnterKey()
{
   // std::cout << "\nSend SIGINT to stop, analyze statistics and exit Sample. " << std::endl;
    waitUntilSignal();

}



