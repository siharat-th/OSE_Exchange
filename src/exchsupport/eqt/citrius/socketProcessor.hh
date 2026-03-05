#ifndef SOCKETPROCCESSOR_HH_
#define SOCKETPROCCESSOR_HH_

#if defined (__linux__)
#include <cerrno>
#endif

#include "socket.hh"

namespace LimeBrokerage {
    
//Socket Processor implementation

class SocketProcessor {

private:
    static const size_t maxBufferSize;
    
    Socket sock;
    
    char *spareBuffer;
    char *inBuffer;
    size_t inRead;
    size_t inConsumed;
    size_t inBufferSize;

    char *outBuffer;
    size_t outConsumed;
    size_t outWritten;
    size_t outBufferSize;
    
public:
//    SocketProcessor();
    ~SocketProcessor();
    SocketProcessor(size_t inBufferSize = maxBufferSize, size_t outBufferSize = maxBufferSize);
    
    //Socket delegation
    Socket::ConnectionState connect(std::string hostName, int port);
    bool open(bool blocking = true);
    bool isValid() const;
    void close();
    int read();
    int setNonBlocking();
    char *getBytesAvailable(int *pLen);
    char *getInputData(int *pLen, bool readFromSock = true, bool block = true);
    void consume(size_t len);
    int write(char *buffer, size_t len);
    SOCKET getFileDesc();
    int isBlocking();
//    Socket::ConnectionState isConnected();
private:
        
    SocketProcessor(const SocketProcessor&);
    SocketProcessor& operator=(const SocketProcessor&);
        
};
    
}
#endif /*SOCKETPROCCESSOR_HH_*/
