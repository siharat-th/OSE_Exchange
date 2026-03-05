#include <iostream>
#include <cstring>


#ifdef __linux__
#include <cerrno>
extern "C" {
#include <unistd.h>
}
#endif

#include "socketProcessor.hh"
#include "util.hh"

//SocketProcessor Implementation
namespace LimeBrokerage {
const size_t SocketProcessor::maxBufferSize = 32 * 1024;
const std::string nullString;
//Socket Processor Implementation
SocketProcessor::~SocketProcessor()
{
    delete [] inBuffer;
    delete [] spareBuffer;
    delete [] outBuffer;
}

SocketProcessor::SocketProcessor(size_t insize, size_t outsize) :
    sock(),
    inRead(0), inConsumed(0), inBufferSize(insize),
    outConsumed(0), outWritten(0), outBufferSize(outsize)
{
    spareBuffer = new char[inBufferSize];
    inBuffer = new char[inBufferSize];
    outBuffer = new char[outBufferSize];
}

Socket::ConnectionState SocketProcessor::connect(std::string hostName, int port)
{
	Socket::ConnectionState state =  sock.connect(hostName, port);
    return state;
}

bool SocketProcessor::open(bool blocking)
{
	bool res= sock.open(blocking);
    return res;
}


bool SocketProcessor::isValid() const
{
	return sock.isValid();
}

void SocketProcessor::close()
{ 
	sock.shutdownRead();
	sock.shutdownWrite();
    sock.close();
    inRead = inConsumed = 0;
    outConsumed = outWritten = 0;
}

int SocketProcessor::read() {
    int bytesRead = sock.read(&inBuffer[inRead], inBufferSize - inRead);
    if (bytesRead <= 0) return bytesRead;
    else inRead += bytesRead;
    return bytesRead;
}

int  SocketProcessor::setNonBlocking()
{
	return sock.setNonBlocking();
}

char *SocketProcessor::getBytesAvailable(int *pLen)
{
    *pLen = (inRead - inConsumed);
    return (inBuffer + inConsumed);
}
char *SocketProcessor::getInputData(int *pLen, bool readFromSock, bool block)
{
    unsigned int len = *pLen;
    unsigned int bytesAvailable = *pLen = inRead - inConsumed;

    if ((int)bytesAvailable < 0) {
        // there is no reason for this to happen, except for a programming bug
        return NULL;
    } else if (bytesAvailable >= len) {
        return &inBuffer[inConsumed];
    }
     
    if (len > inBufferSize) {
        // Reallocate buffer.
//        std::cout << "Reallocating inBuffer to " << inBufferSize << " plus " << len << std::endl;
        delete [] spareBuffer;
        inBufferSize += len;
        spareBuffer = new char[inBufferSize];
        std::memcpy(spareBuffer, &inBuffer[inConsumed], bytesAvailable);
        delete [] inBuffer;
        inBuffer = spareBuffer;
        spareBuffer = new char[inBufferSize];
        inConsumed = 0;
        inRead = inConsumed + bytesAvailable;
    }

    // Copy residual bytes to front of buffer.
    if (inBufferSize - inConsumed < len) {
        memmove(inBuffer, inBuffer+inConsumed, bytesAvailable);
        inConsumed = 0;
        inRead = inConsumed + bytesAvailable;
    }

    // If caller specified blocking, loop iterates forever.
    // If non-blocking, the loop runs just once.
    for (bool loop = true; readFromSock && loop && bytesAvailable < len; bytesAvailable = inRead -inConsumed) {
        int retVal = read();
        if ((retVal == 0) || 
			(Utility::isError(retVal) && !Utility::isErrorWouldBlock())) {
            *pLen = -1;
            return NULL;
        }
        loop = block;
    }
    *pLen = inRead - inConsumed;
    return &inBuffer[inConsumed];
}


void SocketProcessor::consume(size_t len)
{
	inConsumed += len;
}
int SocketProcessor::write(char *buffer, size_t len)
{

	unsigned int bytesWritten = 0;
//    	int res = sock.write(buffer, len);
//    	while (bytesWritten != len) {
//    		if (res < 0 && (errno != EAGAIN)) {
//    				return res;
//    		}
//    		res = sock.write(buffer, len);
//
//    	}
	do {
		int res = sock.write(&buffer[bytesWritten], len - bytesWritten);
		if (res < 0) {
			if (!Utility::isErrorWouldBlock()) return res;
		}
		else {
			bytesWritten+=res;
		}
	} while (bytesWritten != len);
	return bytesWritten;
}

SOCKET SocketProcessor::getFileDesc()
{
	return sock.getFileDesc();
}


int SocketProcessor::isBlocking()
{
    return sock.isBlocking();
}

//Socket::ConnectionState SocketProcessor::isConnected()
//{
//    Socket::ConnectionState state = sock.isConnected();
//    return state;
//}
}
