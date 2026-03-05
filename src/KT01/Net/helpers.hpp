//============================================================================
// Name        	: helpers.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 19, 2023 10:53:32 AM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_NET_HELPERS_HPP_
#define SRC_EXCHSUPPORT_CME_NET_HELPERS_HPP_

#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

class NetHelpers
{
public:
	inline static void hexdump(void *ptr, int buflen) {
	  unsigned char *buf = (unsigned char*)ptr;
	  int i, j;
	  for (i=0; i<buflen; i+=16) {
		  //line label:
	   // printf("%06x: ", i);
	    for (j=0; j<16; j++)
	      if (i+j < buflen)
	        printf("%02x ", buf[i+j]);
	      else
	        printf("   ");
//	    printf(" ");
//	    for (j=0; j<16; j++)
//	      if (i+j < buflen)
//	        printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
	    printf("\n");
	  }
	}

	inline static std::string getIPAddress(const std::string& interfaceName) {
		struct ifaddrs* ifaddr;
		if (getifaddrs(&ifaddr) == -1) {
			perror("getifaddrs");
			return "";
		}
	
		std::string ipAddress;
	
		for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr == nullptr) continue;
	
			// Check for correct interface and address family (IPv4)
			if (interfaceName == ifa->ifa_name && ifa->ifa_addr->sa_family == AF_INET) {
				char addrBuf[INET_ADDRSTRLEN];
				void* addrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
	
				if (inet_ntop(AF_INET, addrPtr, addrBuf, sizeof(addrBuf)) != nullptr) {
					ipAddress = addrBuf;
					break;  // Found the IP, stop searching
				}
			}
		}
	
		freeifaddrs(ifaddr);
		return ipAddress;
	}

};


#endif /* SRC_EXCHSUPPORT_CME_NET_HELPERS_HPP_ */
