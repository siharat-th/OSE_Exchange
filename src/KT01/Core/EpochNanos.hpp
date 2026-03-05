//============================================================================
// Name        	: EpochNanos.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 6, 2023 1:01:21 PM
//============================================================================

#ifndef SRC_CORE_EPOCHNANOS_HPP_
#define SRC_CORE_EPOCHNANOS_HPP_
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <stdint.h>
#include <unistd.h>

#include <KT01/Core/Time.hpp>
using namespace KT01::Core;


using namespace std;

//Note: this is not intended to be timing mechanism, but rather
//is a class the quickly computes the nanos since epoch timestamp.
//It's not intended to super accurate, just accurate enought to satisfy
//echange clock resolution tolerances.

namespace KTN { namespace Core {
/**
 * @brief Class that quickly computes the nanos since epoch timestamp.
 * 
 * This class is not intended to be a precise timing mechanism, but rather a class that quickly computes the nanoseconds since the epoch timestamp.
 * It is not intended to be super accurate, but accurate enough to satisfy exchange clock resolution tolerances.
 */
class EpochNanos{
public:
	/**
  * @brief Default constructor.
  * 
  * Initializes the EpochNanos object by capturing the current timestamp and initializing the necessary variables.
  */
	EpochNanos()
	{
		auto currentTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();

		_initial = static_cast<uint64_t>(currentTimestamp);
		_rdtsc_init = rdtsc();

		_cpu = GetCPUClockSpeedInHz();
		_multcpu = 1/_cpu;
	}

	/**
  * @brief Computes the current nanoseconds since the epoch timestamp.
  * 
  * @param en The EpochNanos object.
  * @return The current nanoseconds since the epoch timestamp.
  */
	static uint64_t Current(const EpochNanos& en)
	{
		return static_cast<uint64_t>(en._initial + ((rdtscp() - en._rdtsc_init) * en._multcpu));
	}

	/**
  * @brief Computes the current nanoseconds since the epoch timestamp.
  * 
  * @return The current nanoseconds since the epoch timestamp.
  */
	uint64_t Current() {
		uint32_t low, high;
		asm volatile("rdtsc" : "=a"(low), "=d"(high));
		uint64_t rdts = static_cast<uint64_t>(low) | (static_cast<uint64_t>(high) << 32);
		return static_cast<uint64_t>(_initial + ((rdts - _rdtsc_init) * _multcpu));
	}

	/**
  * @brief Retrieves the CPU clock speed in Hz.
  * 
  * @return The CPU clock speed in Hz.
  */
	double GetCPUClockSpeedInHz()
	{
		double cpuClockSpeedInHz = 0.0;
		std::ifstream cpuinfo("/proc/cpuinfo");
		std::string line;

		while (std::getline(cpuinfo, line)) {
			std::istringstream iss(line);
			std::string key, value;
			if (std::getline(iss, key, ':') && std::getline(iss, value)) {
				if (key.find("MHz") != std::string::npos) {
					// Assuming "CPU MHz" is the line that contains CPU clock speed
					cpuClockSpeedInHz = std::stod(value) /1000.0;
					break;
				}
			}
		}

		return cpuClockSpeedInHz;
	}


private:
	Timestamp _ts;
	MonotonicTimer _mt;

	uint64_t _initial;
	uint64_t _rdtsc_init;
	double _cpu;
	double _multcpu;


};

}}

#endif /* SRC_CORE_EPOCHNANOS_HPP_ */
