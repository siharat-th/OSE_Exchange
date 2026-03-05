#ifdef __linux__
extern "C" {
#include <sys/select.h>
#include <sys/time.h>
}
#include <cerrno>
#endif
extern "C" {
#include <stdio.h>
#include <string.h>
}
#include <cstring>
#include <iostream>
#include "sessionManager.hh"
#include "util.hh"
#include "socket.hh"

namespace LimeBrokerage
{
    SessionManager::SessionManager(int cpuAffinity) :
        thread(this, "QuoteApiSessionManager", cpuAffinity), pMsgQ(new MsgQ), stopped(false),joined(true), maxFd(0), numReadableApis(0)
    {
        FD_ZERO(&readFdSet);
    }

    SessionManager::SessionManager() :
        thread(this, "QuoteApiSessionManager"), pMsgQ(new MsgQ), stopped(false),joined(true), maxFd(0), numReadableApis(0)
    {
        FD_ZERO(&readFdSet);
    }

    SessionManager::~SessionManager()
    {
        delete pMsgQ;
    }

    void SessionManager::start()
    {
        stopped = false;
        joined = false;
        thread.start();
    }

    void SessionManager::shutdown()
    {
        stopped = true;
        pMsgQ->sendMsg(new Msg(this));
    }

    void SessionManager::join()
    {
        if (!joined) thread.join();
        joined = true;
    }

    void SessionManager::run()
    {
        //wait for a max of 1ms before checking for other messages
        while (!stopped) {
            timeval waitTime = {0, 1000};
            processMessages(&waitTime);
        }
    }

    int SessionManager::processServerMessages(const timeval *pWaitTime, int maxMessages)
    {
        if (quoteApiSet.empty())
            return 0;

        // Find out if any more sockets are now readable.
        fd_set readableFdSet;
        memcpy(&readableFdSet, &readFdSet, sizeof(fd_set));

        int numFds = 0;

        // If either of numReadableApis or pWaitTime is non zero, then use waitTime
        if(numReadableApis || pWaitTime){

            // If we have data pending from earlier calls, do not allow select() to block.
            const static timeval ZeroWaitTime = {0,0};
            timeval  waitTime(numReadableApis?ZeroWaitTime:(*pWaitTime));

            numFds = select(maxFd, &readableFdSet, NULL, NULL, &waitTime);
        } else {
            numFds = select(maxFd, &readableFdSet, NULL, NULL, NULL);
        }

        if (numFds < 0) {
            // this might happen when selecting on a socket closed by the other thread
            if (Utility::isErrorBadFileDesc()) return -2;
            if (!Utility::isErrorInterrupted()) perror("select failed");
            return -1;
        }

        // For each API instance, if socket is now readable, setSockReadable(true).
        for (QuoteApiSet::iterator iter = quoteApiSet.begin(); numFds && iter != quoteApiSet.end(); ++iter) {
            Session *session = *iter;
            if (FD_ISSET(session->getSockDesc(), &readableFdSet)) {
                // Set API instance to readable.  If it wasn't already readable, bump numReadableApis.
                if (!session->setSockReadable(true))
                    ++numReadableApis;
                --numFds;
            }
        }

        // Process API instances with readable sockets, until all msgs processed, or msgsProcessed exceeds maxMessages.
        int msgsProcessed = 0;
        while (numReadableApis && (maxMessages==0 || msgsProcessed<maxMessages)) {
            // Sequence through every API instance, processing just 1 message (fairness).
            int readableApis = 0;
            for (QuoteApiSet::iterator iter = quoteApiSet.begin(); numReadableApis && iter != quoteApiSet.end(); ++iter) {
                Session *session = *iter;
                if (session->isSockReadable()) {
                    if (session->processNextMsg()>0) {
                        ++readableApis;
                    } else {
                        session->setSockReadable(false);
                        --numReadableApis;
                    }
                    ++msgsProcessed;
                }
            }
            // We just recounted the number of readable APIs, so use this estimate next time.
            numReadableApis = readableApis;
        }
        return msgsProcessed;
    }

    int SessionManager::processApiRegistration(Session *pQ)
    {
        if (!FD_ISSET(pQ->getSockDesc(), &readFdSet)) {
            FD_SET(pQ->getSockDesc(), &readFdSet);
            if (pQ->getSockDesc() >= maxFd)
                maxFd = pQ->getSockDesc() + 1;
            quoteApiSet.insert(quoteApiSet.end(), pQ);
            // Make sure the new API enters the quoteApiSet as not readable, making it consistent with numReadableApis.
            pQ->setSockReadable(false);
            return 1;
        }
        return 0;
    }

    int SessionManager::processApiDeRegistration(Session *pQ)
    {
        FD_CLR(pQ->getSockDesc(), &readFdSet);

        quoteApiSet.erase(pQ);
        // numReadableApis needs to track the number of readable APIs in quoteApiSet.
        if (pQ->isSockReadable())
            --numReadableApis;

        pQ->onCancelRegistration();

        if (quoteApiSet.empty()) {
            maxFd = 0;
        } else {
            maxFd = (*(quoteApiSet.rbegin()))->getSockDesc() + 1;
        }

        return 1;
    }

    void SessionManager::registerApiInstance(Session *pQ)
    {
        pMsgQ->sendMsg(new RegisterMsg(this, SmMsgTypeRegister, pQ));
    }

    void SessionManager::deRegisterApiInstance(Session *pQ)
    {
        pMsgQ->sendMsg(new RegisterMsg(this, SmMsgTypeDeregister, pQ));
    }

    size_t SessionManager::quoteApiSetSize()
    {
        return quoteApiSet.size();
    }

    int SessionManager::RegisterMsg::process()
    {
        switch(type) {
            case SmMsgTypeRegister:
                return pMgr->processApiRegistration(pQ);
            case SmMsgTypeDeregister:
                return pMgr->processApiDeRegistration(pQ);
            default:
                printf("Invalid error message: %d", type);
                return 0;
        }
    }

    int SessionManager::processNextLocalMsg(bool wait)
    {
        Msg *pMsg;

        /* I can only block if running in my own thread */
        bool block = (wait && isSessionManagerThread());
        if (!(pMsg = pMsgQ->getNextMsg(block, NULL)))
            return MSGQ_ERR_NO_MSG;

        if (!isMsgValid(pMsg))
            return MSGQ_ERR_INVALID_TYPE;

        int retVal = pMsg->process();
        delete pMsg;

        return retVal;
    }

    int SessionManager::processMessages(timeval *pWaitTime, int maxMessages)
    {
        /* process any requests from the client, block if no clients registered */
        processNextLocalMsg(quoteApiSet.empty());

        /* a request to stop could have been processed so check for it */
        if (!(stopped && isSessionManagerThread())) {
            /* process messages from the quote server */
            return processServerMessages(pWaitTime, maxMessages);
        }
        return 0;
    }

    int SessionManager::processMessages(timeval *waitTime)
    {
        return processMessages(waitTime, 0);
    }

    SessionManager::Msg *SessionManager::MsgQ::getNextMsg(bool wait, int *error)
    {
        int retVal;

        /* try to get a lock. If busy and if asked to wait, block for the lock otherwise just return*/
        retVal = (wait) ? queueLock.lock() : queueLock.tryLock();

        if (retVal != 0) {
            if (error)
                *error = Utility::getLastError();
            return NULL;
        }

        // only wait if asked to, otherwise just return.
        while (msgs.empty()) {
            if (wait) {
                wait = false;
                qHasMsg.wait();
            } else {
                queueLock.unlock();
                return NULL;
            }
        }

        Msg *pMsg = msgs.front();
        msgs.pop();
        queueLock.unlock();
        return pMsg;
    }
}
