
#ifndef __SESSION_MANAGER__
#define __SESSION_MANAGER__

#include <set>
#include <queue>
#include <list>

#if defined(__linux__)
extern "C" {
#include <sys/types.h>
#include <pthread.h>
}
#elif defined(WIN32)
extern "C" {
#include <winsock2.h>
}
#endif

#include "event.hh"
#include "thread.hh"
#include "session.hh"

namespace LimeBrokerage {
    
    /**
     *  A SessionManager handles the job of waiting for the arrival of data 
     *  from one or more Citrius Quote System API instances.
     *
     *  It selects on a socket waiting for data to arrive.  As data arrives, 
     *  it calls the callback routines in each associated instance of the 
     *  API.
     *
     *  Every API instance is associated with an instance of 
     *  SessionManager. Users can explicitly specify an instance of 
     *  SessionManager in the API constructor. If an instance of 
     *  SessionManager is not explicitly specified while instantiating an 
     *  API instance, a new instance of SessionManager will be 
     *  created automatically.  This maintains the behavior of all legacy 
     *  code that was written before SessionManager existed.
     *
     * @see
     *  The <a
     *  href="http://www.limebrokerage.com/production/client/citrius/SessionManager_Overview.html">Session 
     *  Manager Overview</a> contains a more detailed explanation.
     */
    class SessionManager {
    private:
        struct QuoteApiCompare {
            bool operator () (const Session* pQ1, const Session* pQ2) const
            {
                return (pQ1->getSockDesc() < pQ2->getSockDesc()); 
            }
        };

        typedef std::set<Session*, QuoteApiCompare> QuoteApiSet;

        QuoteApiSet quoteApiSet;
        enum SmMsgType {
            SmMsgTypeShutdown,
            SmMsgTypeRegister,
            SmMsgTypeDeregister,
            SmMsgTypeMax
        };

        class Msg {
            protected:
            SessionManager* pMgr;
            SmMsgType type;

            public:
            Msg(SessionManager* pM, SmMsgType msgType): pMgr(pM), type(msgType) { }
            Msg(SessionManager* pM): pMgr(pM), type(SmMsgTypeShutdown) { }
            virtual ~Msg() { };

            SmMsgType getType() { return type; }

            virtual int process()
            {
                pMgr->processShutdown();
                return 1;
            }
        };

        struct RegisterMsg : public Msg {
            Session* pQ;

            RegisterMsg(SessionManager *pM, SmMsgType msgType, Session *_pQ) :
                Msg(pM, msgType), pQ(_pQ) { }

            int process();
        };

        bool isMsgValid(Msg *pM) { return pM->getType() < SmMsgTypeMax; }

        enum MsqQProcessErrors {
            MSGQ_ERR_INVALID_TYPE = -5,
            MSGQ_ERR_NO_MSG = -6
        };

        class MsgQ {

            typedef std::queue<Msg*, std::list<Msg*> > Msgs;

            private:
            Msgs            msgs;
            Mutex            queueLock;
            Event            qHasMsg;

            public:
            MsgQ():queueLock(false), qHasMsg(&queueLock) {}

            virtual ~MsgQ() {}

            void sendMsg(Msg* pMsg) {
                // TODO: Check return value?
                queueLock.lock();
                msgs.push(pMsg);
                qHasMsg.notify();
                queueLock.unlock();
            }

            Msg *getNextMsg(bool wait, int *error = NULL);

            size_t size() { return msgs.size(); }

            bool empty() { return msgs.empty(); }
        }; //class MsgQ

        Thread<SessionManager> thread;
        MsgQ* pMsgQ;

        bool stopped;
        bool joined;
        fd_set readFdSet;
        int maxFd;
        int numReadableApis;

        int processNextLocalMsg(bool wait);
        int processServerMessages(const timeval* pWaitTime, int maxMessages);

        void processShutdown()
        {
            stopped = true;
        }

        bool isSessionManagerThread() {
            return thread.isMyself();
        }

        int processApiRegistration(Session* pQ);
        int processApiDeRegistration(Session* pQ);
        size_t quoteApiSetSize();

    public:

        /**
         *  Create a new instance of SessionManager.
         */
        SessionManager();

        /* param cpuAffinity
         *  An optional parameter used to bind the instance of SessionManager 
         *  to a particular core on a multi-core machine.  Passing a value of 
         *  -1 yields the default behavior in which the thread is not bound 
         *  to any core. Not supported!
         */
        SessionManager(int cpuAffinity);

        /**
         *  Session Manager destructor.
         */
        ~SessionManager();

        /**
         *  Executed when SessionManager is run in a separate thread.
         */
        void run();

        /**
         *  Starts a separate thread for this instance of SessionManager.  
         *  Each instance of SessionManager represents a single thread.  
         *  Therefore this function should be called only once for each 
         *  instance of the SessionManager.
         */
        void start();

        /**
         *  Tell the SessionManager thread to terminate.
         */
        void shutdown();

        /**
         *  Wait for the SessionManager thread to terminate.
         */
        void join();

        void registerApiInstance(Session* pQ);
        void deRegisterApiInstance(Session* pQ);

        /*
         *  Check if a given API instance is registered with this 
         *  instance of SessionManager.
         *
         * @param pQ
         *  The instance of an API.
         *
         * @return
         *  True if pQ is registered.
         */
        bool isRegistered(Session* pQ) {
            return (FD_ISSET(pQ->getSockDesc(), &readFdSet) != 0);
        }

        /**
         *  When not running in separate thread, call this function to 
         *  receive data and process the related callbacks.  This function 
         *  should only be explicitly called when the instance of 
         *  SessionManager is not being run in a separate thread.  In case 
         *  where the SessionManager instance is running in a separate 
         *  thread, this function is used by the separate thread.
         *
         * @param waitTime
         *  A timeval struct, specifying the maximum amount of time to wait 
         *  in seconds and micro-seconds if no data is available.  Pass a 
         *  NULL value to wait indefinitely for the data to arrive.
         *
         * @param maxMessages
         *  Maximum numbers of messages to process.
         *  But processMessages() will give each API instance the same
         *  number of chances to process a message, so it may overshoot
         *  this value by the number of API instances.
         *
         * @return
         *  The number of messages processed.
         */
        int processMessages(timeval* waitTime, int maxMessages);

        /**
         *  When not running in separate thread, call this function to 
         *  receive data and process the related callbacks.  This function 
         *  should only be explicitly called when the instance of 
         *  SessionManager is not being run in a separate thread.  In case 
         *  where the SessionManager instance is running in a separate 
         *  thread, this function is used by the separate thread.
         *
         * @param waitTime
         *  A timeval struct, specifying the maximum amount of time to wait 
         *  in seconds and micro-seconds if no data is available.  Pass a 
         *  NULL value to wait indefinitely for the data to arrive.
         *
         * @return
         *  The number of messages processed.
         */
        int processMessages(timeval* waitTime);
    }; // class SessionManager
};  // namespace LimeBrokerage
#endif
