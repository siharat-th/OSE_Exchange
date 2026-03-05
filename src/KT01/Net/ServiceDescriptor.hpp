//============================================================================
// Name        	: ServiceDescriptor.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 13, 2023 8:02:05 PM
//============================================================================

#ifndef SRC_KTNTCP_SERVICEDESCRIPTOR_HPP_
#define SRC_KTNTCP_SERVICEDESCRIPTOR_HPP_

#pragma once


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

#include <netinet/in.h>

#include <assert.h>

#include <unordered_map>


using namespace std;
namespace KT01 {
namespace NET {

/**
 * @brief The ServiceDescriptor class represents a network service parameter set.
 */
class ServiceDescriptor
{
public:
	/**
  * @brief Default constructor for ServiceDescriptor.
  */
	ServiceDescriptor();

	/**
  * @brief Constructor for ServiceDescriptor.
  * @param _address The address of the service.
  * @param _port The port of the service.
  * @param _intf The interface of the service.
  * @param _intfname The name of the interface of the service.
  * @param _feed The feed type of the service.
  * @param _isrc The source of the service.
  */
	ServiceDescriptor(std::string _address, unsigned _port,  std::string _intf, std::string _intfname, int _feed, int _isrc);

	/**
  * @brief Destructor for ServiceDescriptor.
  */
	virtual ~ServiceDescriptor();

	std::string address; /**< The address of the service. */
	unsigned port; /**< The port of the service. */

	std::string localinterfacename; /**< The name of the local interface. */
	std::string localinterface; /**< The local interface. */
	int feedtype; /**< The feed type. */
	int iSource; /**< The source. */

	struct sockaddr_in sa_tcplocal; /**< The TCP local socket address. */
	struct sockaddr_in sa_tcpremote; /**< The TCP remote socket address. */
	int fd; /**< The file descriptor. */
	int index; /**< The index. */
	int segid; /**< The segment ID. */

	/**
  * @brief Print the service descriptor.
  */
	void Print();
};

}}
#endif /* SRC_KTNTCP_SERVICEDESCRIPTOR_HPP_ */
