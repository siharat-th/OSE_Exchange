//============================================================================
// Name        	: UdpSocket.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 13, 2023 8:05:00 PM
//============================================================================

#include <KT01/Net/Udp/UdpSocket.hpp>
using namespace KT01::NET::UDP;

UdpSocket::UdpSocket() {
	// TODO Auto-generated constructor stub

}

UdpSocket::~UdpSocket() {
	// TODO Auto-generated destructor stub
}

void UdpSocket::LogMe(std::string szMsg)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}



//create a basic udp socket that joins a multicast group
int UdpSocket::CreateSocket(ServiceDescriptor feed)
{
	int fd = 0;
	u_int yes=1;            /*** MODIFICATION TO ORIGINAL */
	if ((fd =socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		  perror("udp socket create");
		  exit(1);
	}

	 /* allow multiple sockets to use the same PORT number */
	if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
	   perror("udp: Reusing ADDR failed");
	   exit(1);
	}

	int n = 16 * 1024 * 1024;
	if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) == -1) {
		perror("udp: Set RCV BUF FAILED");
		exit(1);
	}

	/* use setsockopt() to request that the kernel join a multicast group */
	struct ip_mreq ip;
	ip.imr_multiaddr.s_addr=inet_addr(feed.address.c_str());
	ip.imr_interface.s_addr=inet_addr(feed.localinterface.c_str());
	if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&ip,sizeof(ip)) < 0) {
		perror("udp: setsockopt");
		exit(1);
	}

	return fd;

}

