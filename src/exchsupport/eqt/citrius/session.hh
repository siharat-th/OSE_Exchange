
#ifndef LIME_BROKERAGE_SESSION_HH
#define LIME_BROKERAGE_SESSION_HH

namespace LimeBrokerage {

    /**
     * Classes that derive from Session can be managed by an instance of a
     * SessionManager.  The Sesssoin is reponsible for socket reading/writing
     * while the SessionManager is reponsible for notifying th Session when
     * data is available.
     */
    class Session {
    public:
        /**
         * Returns the socket file descriptor for this Session.
         */
        virtual int getSockDesc() const = 0;

        /**
         * This function is called when the socket associated with
         * the Session has data to read.
         * @return a number greater than zero if a message was
         *         processed.
         */
        virtual int processNextMsg() = 0;

        /**
         * Called when this Session is deregistered from the
         * SessionManager.
         */
        virtual void onCancelRegistration() = 0;

        /**
         * Session destructor.
         */
        virtual ~Session() { }

    protected:
        Session() : sockReadable(false) { }

    private:
        bool sockReadable;
        bool setSockReadable(bool sockReadable)
        {
            if (sockReadable == this->sockReadable) {
                return sockReadable;
            } else {
                this->sockReadable = sockReadable;
                return !sockReadable;
            }
        }
        bool isSockReadable() const { return sockReadable; }
        friend class SessionManager;
    };
}

#endif  /* LIME_BROKERAGE_SESSION_HH */
