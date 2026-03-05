//============================================================================
// Name        	: ServiceDescriptor.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 13, 2023 8:02:05 PM
//============================================================================

#include "ServiceDescriptor.hpp"

using namespace KT01::NET;

ServiceDescriptor::ServiceDescriptor()
{
}

ServiceDescriptor::~ServiceDescriptor()
{
}
ServiceDescriptor::ServiceDescriptor(std::string _address, unsigned _port,  std::string _intf, std::string _intfname, int _feed, int _isrc)
{
	address = _address;
	port = _port;
	localinterfacename = _intfname;
	localinterface = _intf;
	feedtype = _feed;
	iSource = _isrc;
}

