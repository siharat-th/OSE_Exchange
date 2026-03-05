//
// "socket.hh" - TCP/IP sockets
//
// Copyright 2006 Lime Brokerage LLC.  All rights reserved.
//
// Change history:
//   12/12/06  rmf  Initial coding.
//

#ifndef LIME_BROKERAGE_SOCKET_HH
#define LIME_BROKERAGE_SOCKET_HH

// Standard C++ headers.
#include <string>

#ifdef WIN32
#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2
extern "C" {
#include <winsock2.h>
#include <mstcpip.h>    // for tcp_keepalive
}
#endif

namespace LimeBrokerage {

//  Socket - a minimal multiplatform POSIX-style socket class.
#ifdef __linux__
    typedef int SOCKET;
#endif

    class Socket {
    public:
        enum ConnectionState {
            connectionStateFailure = -1 ,
            connectionStateConnected = 0,
            connectionStatePending = 1
        };
        // 'Tors.
        Socket();
        ~Socket();

        // Standard Unix-style socket operations.
        int setNonBlocking();
        bool isBlocking();
        bool open() { return open(true); }
        bool open(bool blocking);
        bool isValid() const;
        ConnectionState connect(std::string hostName, int port);
        //ConnectionState isConnected() const;
        bool read(std::string& s);
        bool read(std::string& s, size_t n);
        int read(void* buffer, size_t bufferSize);
        int write(void* buffer, size_t bufferSize);
        void close();
        void shutdownRead();
        void shutdownWrite();

        // Accessors.
        SOCKET getFileDesc() const { return fd; }

    private:
        // Constants.
        static const size_t readBufferSize;

        // Member data.
        SOCKET fd;

        char* readBuffer;                       // read buffer (used when reading into a string)

        // Internal constructor used to create a socket object with a known file
        // descriptor (e.g., on accept operations).
        Socket(int fd);

        Socket(const Socket&);                  // undefined - prevents unintended copy
        Socket& operator=(const Socket&);       // undefined - prevents unintended assignment
    };
}  // namespace LimeBrokerage

#endif  // LIME_BROKERAGE_SOCKET_HH
