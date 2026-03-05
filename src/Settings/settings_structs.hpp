//============================================================================
// Name        	: settings_structs.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Aug 10, 2023 10:57:13 AM
//============================================================================

#ifndef SRC_SETTINGS_SETTINGS_STRUCTS_HPP_
#define SRC_SETTINGS_SETTINGS_STRUCTS_HPP_

#pragma once

#include <string>
using namespace std;


struct ServerInfo
{
	string ip;
	int port;
	int sleepms;
	int core;
};



#endif /* SRC_SETTINGS_SETTINGS_STRUCTS_HPP_ */
