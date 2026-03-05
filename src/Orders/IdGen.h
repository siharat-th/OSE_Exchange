#pragma once

#include <ctime>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <cstdint>
#include <stdio.h>
#include <iostream>
#include <chrono>

#include <random>
#include <sstream>
#include <iomanip>
#include <string>


#include <boost/algorithm/string.hpp>
#include <KT01/DataStructures/UserStructs.hpp>
#include <KT01/Core/Log.hpp>
//#include "sole.hpp"

/*
 * Sole UUID gen:
 * https://github.com/r-lyeh-archived/sole
 */

using namespace std;


namespace KTN{
class IdGen
{
public:
	IdGen();
	~IdGen();
	void Seed();
	int Current();
	int Next();

	struct DecodedData {
	    uint8_t callbackid;
	    int32_t secid;
	    int16_t differential;
	    int8_t multiplier;
	    uint32_t sequencing;
	};


	// static std::string UUID(int len = 6)
	// {
	// 	sole::uuid u4 = sole::uuid4();
	// 	std::string uuid =  u4.str();

	// 	std::string res = (len < 10) ? uuid.substr(0,len) : uuid;

	// 	std::string res2 = (len < 10) ? boost::to_upper_copy<std::string>(res) : res;

	// 	return res2;
	// }

	static std::string UUID(int len = 6)
    {
        static std::random_device rd;
        static std::mt19937 rng(rd());
        static std::uniform_int_distribution<> dist(0, 15); // 0–F

        std::ostringstream oss;
        for (int i = 0; i < len; ++i)
            oss << std::hex << std::uppercase << dist(rng);

        return oss.str();
    }

	static void GenClOrdID10(char* buffer, const std::string& prefix, unsigned int int_value)
	{
		char buf[20];
		sprintf(buf,  "%s%06d", prefix.c_str(), int_value);

		buffer = buf;
	}

	static void Append(char* dest, char* src, size_t numChars)
	{
		// Find the length of dest
		size_t destLength = 0;
		while (dest[destLength] != '\0' && destLength < 20) {
			++destLength;
		}
		// Check if there is enough space in the destination array
		if (destLength + numChars < 20) {
			// Append characters directly
			for (size_t i = 0; i < numChars; ++i) {
				dest[destLength + i] = src[i];
			}
			//dest[destLength + numChars] = '\0'; // Null-terminate the string
		} else {
			// Handle insufficient space (if needed)
		}
	}

	static void AppendQuickChar(char* dest, const char* src, uint8_t fillnbr)
	{
		// Find the length of dest
		size_t orig_len = 0;
		size_t i = 0;
		int char0 = 49;

		fillnbr += char0;

		while (src[i] != '\0' && orig_len < 20) {
			dest[i] = src[i];
			i++;
		}

		dest[i] = static_cast<char>(fillnbr);
	}

	static void Append(const char* orig, char* dest, char* src, size_t numChars)
	{
		// Find the length of dest
		size_t orig_len = 0;
		size_t i = 0;
		while (orig[i] != '\0' && orig_len < 20) {
			dest[i] = orig[i];
			i++;
		}
		orig_len = i;

		int k = 0;
		for(size_t j = i; j < i + numChars; j++)
		{
			if (j >= 20)
				return;

			dest[j] = src[k];
			k++;
		}
	}

	static string NumericID()
	{
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(now);
		auto epoch = now_ms.time_since_epoch();
		auto value = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
		long duration = value.count();

		ostringstream idss;
		idss << duration;
		return idss.str();
	}

	static uint64_t ReqId() {
		std::time_t now = std::time(nullptr);
		std::tm* localTime = std::localtime(&now);

			int millisecondsSinceMidnight = localTime->tm_hour * 3600000 + localTime->tm_min * 60000 + localTime->tm_sec * 1000;
			int uniqueNumber = millisecondsSinceMidnight % 10000;

			return uniqueNumber;
		}


// 	static uint64_t encodeRequestID(int algoBaseID, uint64_t uniqueMessageID) {
// 	    // Combine the Algo's Base ID and the unique Message ID into a RequestID
// //	    return (static_cast<uint64_t>(algoBaseID) << 32) | (uniqueMessageID & 0xFFFFFFFF);
// 	    return (static_cast<uint64_t>(algoBaseID & 0xFF) << 56) | (uniqueMessageID & 0xFFFFFFFF);
// 	}

// 	static int extractAlgoBaseID(uint64_t requestID) {
// 	    // Extract the Algo's Base ID from the most significant bits
// 	    //return static_cast<int>(requestID >> 32);
// 	    return static_cast<int>((requestID >> 56) & 0xFF);
// 	}



	//nanos since midnight for encoding
	static uint32_t ReqId24() {
	    auto now = std::chrono::high_resolution_clock::now();
	    auto ns_since_midnight = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()) % std::chrono::hours(24);

			return ns_since_midnight.count();
		}

		static uint16_t ReqId16()
		{
			auto now = std::chrono::high_resolution_clock::now();
			auto ns_since_midnight = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()) % std::chrono::hours(24);

			// Convert nanoseconds to a uint16_t value
			uint64_t ns_count = ns_since_midnight.count();
			uint16_t unique_sequence = static_cast<uint16_t>(ns_count % std::numeric_limits<uint16_t>::max());

			return unique_sequence;
		}

	private:
		int m_iSeq=0;
		void inc();
		void load();
	};
}
