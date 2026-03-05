//
// "socket.cc" - TCP/IP sockets
//
// Copyright 2006 Lime Brokerage LLC.  All rights reserved.
//
// Change history:
//   12/12/06  rmf  Initial coding.
//

// Standard C++ headers.
#include <cstring>
#include <string>
#include <iostream>

#ifdef __linux__
// Standard POSIX headers.
#include <cerrno>
extern "C" {
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
}
#endif

#ifdef WIN32
#define NOMINMAX
#include <Ws2tcpip.h>
#endif

#include <algorithm>

// Local headers.
#include "socket.hh"
#include "util.hh"

const size_t LimeBrokerage::Socket::readBufferSize = 4096;



// Socket default constructor

LimeBrokerage::Socket::Socket() :
    fd(0), readBuffer(0) 
{
#ifdef WIN32
	  WSADATA wsaData;
	  int iResult;

	  // Initialize Winsock
	  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	  if (iResult != 0) {
	      printf("WSAStartup failed: %d\n", iResult);
	      return;
	  }
#endif
}

// Socket destructor

LimeBrokerage::Socket::~Socket()
{
#ifdef __linux__
    ::close(fd);
#endif

#ifdef WIN32
  ::closesocket(fd);
  WSACleanup();
#endif
    delete [] readBuffer;
}

//Socket::open
//

int LimeBrokerage::Socket::setNonBlocking() {
#if defined(__linux__)
    int val = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, val | O_NONBLOCK) < 0) {
            return errno;
    }
#elif defined(_WIN32)
	u_long O_NONBLOCK = 1;
	if(ioctlsocket(fd, FIONBIO, &O_NONBLOCK) == SOCKET_ERROR)
		return WSAGetLastError();
#endif
    return 0;
}

bool LimeBrokerage::Socket::isBlocking() {
#if defined(__linux__)
    int val = fcntl(fd, F_GETFL, 0);
    return !(val & O_NONBLOCK);
#elif defined(_WIN32)
    return true;
#endif
}

bool LimeBrokerage::Socket::open(bool blocking) {
    fd = ::socket(PF_INET, SOCK_STREAM, 0);

#if defined(__linux__)
    if (fd < 0) return false;

    // Enable TCP KEEPALIVE.
    int optval;
    optval = 1;
    int optlen = sizeof(optval);
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
    // Number of probes before declaring peer dead.
    optval = 3;
    setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &optval, optlen);
    // Interval for 1st probe.
    optval = 15;
    setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &optval, optlen);
    // Interval for subsequent probes.
    optval = 1;
    setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &optval, optlen);

#elif defined(WIN32)

    if(fd == INVALID_SOCKET)
    {
      int error = WSAGetLastError();
      std::cout<<"Error number while creating socket is "<<error<<std::endl;
      return false;
    }

    struct tcp_keepalive sio_keepalive_vals;

    //enable TCP keep-alive
    sio_keepalive_vals.onoff = 1;
    // Interval for 1st probe in msecs
    sio_keepalive_vals.keepalivetime = 15000;
    // Interval for subsequent probes in msecs.
    sio_keepalive_vals.keepaliveinterval = 1000;

    //Note: On windows, the number of keep-alive probes cannot be set. It is fixed as 10.

    DWORD nBytes=0;

    //set the keep-alive values
    int res = ::WSAIoctl( fd, SIO_KEEPALIVE_VALS, &sio_keepalive_vals, sizeof(sio_keepalive_vals), NULL, 0, &nBytes, NULL, NULL );
#endif

    if(!blocking)
        if(setNonBlocking() != 0)
            return false;

    return true;
}

// Socket::connect
/*
 * In case of non-blocking connect returns true if connection is in progress or already connected
 */
LimeBrokerage::Socket::ConnectionState LimeBrokerage::Socket::connect(std::string hostName, int port)
{
    /* Get the (first) IP address corresponding to the given host name or
     * dotted-decimal string. */

    /* Obtain address(es) matching host/port */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /* IPv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0; /* Any protocol */

    struct addrinfo *result = 0;
    int s = ::getaddrinfo(hostName.c_str(), NULL, &hints, &result);
    if (s != 0) {
		std::cout << "Unable to resolve host name " << hostName << ". Reason: '" << gai_strerror(s) << "'" << std::endl;
        return connectionStateFailure; /* can't resolve host name */
    }

    struct sockaddr_in *p = reinterpret_cast<struct sockaddr_in *>(result->ai_addr);
    p->sin_port = htons(port); /* replace uninitialized service (port) */

    int res = ::connect(fd, result->ai_addr, result->ai_addrlen);
    if (LimeBrokerage::Utility::isSocketError(res)) {
        int errorCode = LimeBrokerage::Utility::getLastError();
        if (Utility::isErrorInProgress(errorCode) || Utility::isErrorWouldBlock(errorCode) || Utility::isErrorAlready(errorCode)) {
            /* Assumes non-blocking socket: return connection pending */
            ::freeaddrinfo(result);
            return connectionStatePending;
        } else if (Utility::isErrorConnected(errorCode)) {
            ::freeaddrinfo(result);
            return connectionStateConnected;
        } else {
            close();
            ::freeaddrinfo(result);
            return connectionStateFailure;
        }
    }

    ::freeaddrinfo(result);
    return connectionStateConnected;
}

//LimeBrokerage::Socket::ConnectionState LimeBrokerage::Socket::isConnected() const
//{
//    if (fd < 0) return connectionStateFailure;
//    return connect();
//}

// Socket::read


bool LimeBrokerage::Socket::read(std::string& s)
{
    if (!readBuffer)
        readBuffer = new char[readBufferSize];
    int n = ::recv(fd, readBuffer, readBufferSize, 0);
    if (n <= 0) { /* read error */
        return false;
    }
    s += std::string(readBuffer, n);
    return true;
}

// Socket::read

bool LimeBrokerage::Socket::read(std::string& s, size_t n)
{
    if (!readBuffer)
        readBuffer = new char[readBufferSize];
    for (size_t bytesRead = 0; bytesRead < n; ) {
        int count = recv(fd, readBuffer, std::min((n - bytesRead), readBufferSize), 0);
        if (count <= 0) { /* read error */
          return false;
        }
        s += std::string(readBuffer, count);
        bytesRead += count;
    }
    return true;
}



// Socket::read

int LimeBrokerage::Socket::read(void* buffer, size_t bufferSize)
{
  return ::recv(fd, (char *)buffer, bufferSize, 0);

}



// Socket::write

int LimeBrokerage::Socket::write(void* buffer, size_t bufferSize)
{
    return ::send(fd, (char *)buffer, bufferSize, 0);
}



// Socket::close

void LimeBrokerage::Socket::close()
{
	if (fd < 0) return;
	::shutdown(fd, SHUT_RDWR);
#ifdef __linux__
  ::close(fd);
#endif

#ifdef WIN32
  ::closesocket(fd);
#endif
    fd = -1;
}


// Socket::shutdownRead

void LimeBrokerage::Socket::shutdownRead()
{
    ::shutdown(fd, SHUT_RD);
}



// Socket::shutdownWrite

void LimeBrokerage::Socket::shutdownWrite()
{
    ::shutdown(fd, SHUT_WR);
}



// Socket::isValid

bool LimeBrokerage::Socket::isValid() const
{
    return fd >= 0;
}



// Socket constructor

// Private constructor used to create a socket object using a given socket file
// descriptor.

LimeBrokerage::Socket::Socket(int fd) :
    fd(fd),
    readBuffer(0)
{
}
