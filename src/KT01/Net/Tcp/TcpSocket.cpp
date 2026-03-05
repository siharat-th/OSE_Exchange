//============================================================================
// Name        	: TcpSocket.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 13, 2023 8:05:00 PM
//============================================================================

#include <KT01/Net/Tcp/TcpSocket.hpp>

using namespace KT01::NET::TCP;

TcpSocket::TcpSocket() {
	// TODO Auto-generated constructor stub

}

TcpSocket::~TcpSocket() {
	// TODO Auto-generated destructor stub
}

void TcpSocket::LogMe(std::string szMsg)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

int TcpSocket::CreateSocket(ServiceDescriptor & feed)
{


	int fd = 0;
//	ostringstream os;
//	os << "SETTING UP ORDER SOCKET[" << feed.localinterface << "] " << szDesc << " on " << feed.address << ":" << feed.port ;
//	LogMe(os.str());

//| SOCK_NONBLOCK
	u_int yes=1;
	if ((fd =socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,0)) < 0) {
		  perror("socket OEa");
		  KT01_LOG_ERROR(__CLASS__, "Failed to create socket");
		  return -1;
		 }


	 /* allow multiple sockets to use the same PORT number */
//	if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
//	   perror("Reusing ADDR failed");
//	   exit(1);
//	   }


	int n = 16 * 1024 * 1024;

	n = 128 * 1024 * 1024;

	if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) == -1) {
		perror("Set RCV BUF FAILED");
		KT01_LOG_WARN(__CLASS__, "Failed to set SO_RCVBUF, continuing anyway");
		// Don't return as this is not critical
	}


	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes,sizeof(yes)) < 0)
	{
		perror("Set KEEPALIVE FAILED");
		KT01_LOG_WARN(__CLASS__, "Failed to set SO_KEEPALIVE, continuing anyway");
		// Don't return as this is not critical
	}

	if (setsockopt(fd, SOL_SOCKET, TCP_NODELAY, &yes,sizeof(yes)) < 0)
	{
		perror("Set NODELAY FAILED");
		KT01_LOG_WARN(__CLASS__, "Failed to set TCP_NODELAY, continuing anyway");
		// Don't return as this is not critical
	}

	if (make_socket_non_blocking(fd) < 0)
	{
		LogMe("CANNOT MAKE SOCKET NONBLOCK!");
	}


	//struct sockaddr_in sa_local, sa_tcpremote;

	 memset(&feed.sa_tcpremote,0,sizeof(feed.sa_tcpremote));
	 feed.sa_tcpremote.sin_family=AF_INET;
	 inet_aton(feed.address.c_str(), &(feed.sa_tcpremote.sin_addr));
	 feed.sa_tcpremote.sin_port=htons(feed.port);

//AHHHHH... THIS IS FOR THE SENDER!!!!!!!

	 //For later..
	 memset(&feed.sa_tcplocal,0,sizeof(feed.sa_tcplocal));
	 feed.sa_tcplocal.sin_family=AF_INET;
	 inet_aton(feed.localinterface.c_str(), &(feed.sa_tcplocal.sin_addr));
	 feed.sa_tcplocal.sin_port=htons(feed.port);


	 ostringstream oss;
	 oss << "CONNECTING SESSION TCP SOCKET TO " << feed.address << ":" << (int)feed.port;
	 LogMe(oss.str());


	 if (connect(fd,(struct sockaddr *)&feed.sa_tcpremote,sizeof(feed.sa_tcpremote)) < 0)
	 {
		 if (errno == EINPROGRESS)
		 {
			 if (handle_einprogress(fd) < 0)
			 {
				 perror("EINPROGRESS TIMED OUT");
				 KT01_LOG_ERROR(__CLASS__, "Connection timed out");
				 close(fd);
				 return -1;
			 }
		 }
		 else
		 {
			 perror("Connect TCP");
			 ostringstream oss;
			 oss << "Connection failed. errno=" << errno << " (" << strerror(errno) << ")";
			 KT01_LOG_ERROR(__CLASS__, oss.str());
			 close(fd);
			 return -1;
		 }
	 }


	ostringstream os;
	os.clear();
	os << "SESSION TCP SOCKET CONNECTED OK. fd=" <<  fd ;
	LogMe(os.str());

	feed.fd = fd;

	return fd;

}

