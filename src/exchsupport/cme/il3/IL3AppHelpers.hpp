//============================================================================
// Name        	: IL3AppHelpers.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Aug 2, 2023 3:10:00 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3APPHELPERS_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3APPHELPERS_HPP_

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>
#include <algorithm>

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <stdint.h>
#include <unistd.h>

#include <map>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <string.h>
#include <cstring>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>


#include <exchsupport/cme/il3/IL3Types.hpp>
#include <exchsupport/cme/il3/IL3Composites.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>

#include <Orders/Order.hpp>
#include <Orders/OrderUtils.hpp>

#include <onload/extensions.h>

using namespace std;

namespace KTN{
namespace CME{
namespace IL3{


/**
 * @brief The IL3AppHelpers class provides helper functions for the IL3 application.
 */
class IL3AppHelpers {
public:
	/**
  * @brief Generates an order timestamp in nanoseconds since the epoch.
  * @return The generated order timestamp.
  */
	static uint64_t GenOrdTimestamp() {
		auto currentTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
		return static_cast<uint64_t>(currentTimestamp);
	}

	/**
  * @brief Gets the current time in nanoseconds since the epoch.
  * @return The current time in nanoseconds since the epoch.
  */
	uint64_t GetNanosecondsSinceEpochOptimized() {
     static auto now = std::chrono::high_resolution_clock::now();  // Cache now()
     return now.time_since_epoch().count();
	}

	/**
  * @brief Generates a fast timestamp using the timestamp counter on x86_64 architecture.
  * @return The generated fast timestamp.
  */
	static uint64_t GenFastTimestamp() {
     uint64_t timestamp;
     // For GCC or Clang on x86_64 (Intel/AMD) architecture
     // Use inline assembly to access the timestamp counter
     uint32_t low, high;
     asm volatile("rdtsc" : "=a"(low), "=d"(high));
     timestamp = static_cast<uint64_t>(low) | (static_cast<uint64_t>(high) << 32);

     return timestamp;
	}

};


}}}

#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3APPHELPERS_HPP_ */
