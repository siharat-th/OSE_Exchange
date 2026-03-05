//============================================================================
// Name        	: TcpSocket.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 13, 2023 8:05:00 PM
//============================================================================

#ifndef SRC_KTNTCP_UDPSOCKET_HPP_
#define SRC_KTNTCP_UDPSOCKET_HPP_

#pragma once

#include <string>

#include <algorithm>
#include <cstdarg>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <math.h>
#include <memory.h>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>
#include <assert.h>
//#include <unordered_map>


#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#include <time.h>
#include <stdbool.h>
#include <netinet/tcp.h>

//onload:
//#include <onload/extensions.h>
//#include <onload/extensions_zc.h>

//#include <zf/zf_alts.h>
//#include "efvi/zf_utils.h"
#include <stdbool.h>
#include <netinet/tcp.h>

#include <KT01/Core/Log.hpp>

#include <KT01/Net/ServiceDescriptor.hpp>



using namespace std;

namespace KT01::NET::UDP
{

/**
 * @brief The UdpSocket class represents a UDP socket.
 */
class UdpSocket {
public:
	/**
  * @brief Constructs a UdpSocket object.
  */
	UdpSocket();
	
	/**
  * @brief Destroys the UdpSocket object.
  */
	virtual ~UdpSocket();

	/**
  * @brief Creates a UDP socket.
  * @param feed The service descriptor for the socket.
  * @return The socket file descriptor.
  */
	int CreateSocket(ServiceDescriptor feed);

private:
	/**
  * @brief Logs a message.
  * @param szMsg The message to log.
  * @param iColor The color of the log message.
  */
	void LogMe(std::string szMsg);
	
	/**
  * @brief Makes a socket non-blocking.
  * @param sfd The socket file descriptor.
  * @return 0 if successful, -1 otherwise.
  */
	inline int make_socket_non_blocking (int sfd)
	{
   int flags, s;

   flags = fcntl (sfd, F_GETFL, 0);
   if (flags == -1)
     {
       perror ("fcntl");
       return -1;
     }

   flags |= O_NONBLOCK;
   s = fcntl (sfd, F_SETFL, flags);
   if (s == -1)
     {
       perror ("fcntl");
       return -1;
     }

   return 0;
	}

	/**
  * @brief Handles the EINPROGRESS error.
  * @param sfd The socket file descriptor.
  * @return 0 if successful, -1 otherwise.
  */
	inline int handle_einprogress(int sfd)
	{

		fd_set scanSet;
		FD_ZERO(&scanSet);
		FD_SET(sfd,&scanSet);

		struct timeval waitTime;
		waitTime.tv_sec = 4;
		waitTime.tv_usec = 0;

		int tmp;
		tmp = select(sfd +1, (fd_set*)0, &scanSet, (fd_set*)0,&waitTime);
		if(tmp == -1 || !FD_ISSET(sfd,&scanSet))
		{
			int savedErrorNo = errno;
			char buff[222];
			sprintf(buff,"Connect to CFE Host failed after select, cause %d, error %s",savedErrorNo,strerror(savedErrorNo));
			LogMe(buff);
			close(sfd);
			return -1;
		}


		return 0;

	}


};

} // namespace KT01::NET::UDP

#endif /* SRC_KTNTCP_TCPSOCKET_HPP_ */
