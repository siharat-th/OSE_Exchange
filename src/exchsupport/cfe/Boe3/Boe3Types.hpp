/*
 * Integral.hpp
 *
 *  Created on: May 24, 2021
 *      Author: centos
 */

#ifndef SRC_BOE3_INTEGRAL_HPP_
#define SRC_BOE3_INTEGRAL_HPP_

#define KATANA_BOE3_API __attribute__((visibility("default")))

#pragma once

#include <string>
#include <iostream> // std::cout
#include <sstream>	// std::stringstream, std::stringbuf

using namespace std;

namespace KTN
{
	namespace CFE
	{
		namespace Boe3
		{

			typedef signed char Int8;
			typedef unsigned char UInt8;

			typedef signed short Int16;
			typedef unsigned short UInt16;

			typedef signed int Int32;
			typedef unsigned int UInt32;

			typedef signed long long Int64;
			typedef unsigned long long UInt64;

			typedef int64_t PRICE;
			typedef uint64_t DATETIME;
			typedef uint32_t DATE;

			//typedef uint64_t SECID8;
			typedef char SECID8[8];

			/// Serializes given integer into a string.
			KATANA_BOE3_API void toStr(std::string &, Int8);

			/// Serializes given integer into a string.
			inline std::string toStr(Int8 number)
			{
				ostringstream oss;
				oss << number;
				return oss.str();
			}

			/// Serializes given integer into a string.
			KATANA_BOE3_API void toStr(std::string &, UInt8);

			/// Serializes given integer into a string.
			inline std::string toStr(UInt8 number)
			{
				ostringstream oss;
				oss << number;
				return oss.str();
			}

			/// Serializes given integer into a string.
			KATANA_BOE3_API void toStr(std::string &, Int16);

			/// Serializes given integer into a string.
			inline std::string toStr(Int16 number)
			{
				ostringstream oss;
				oss << number;
				return oss.str();
			}

			/// Serializes given integer into a string.
			KATANA_BOE3_API void toStr(std::string &, UInt16);

			/// Serializes given integer into a string.
			inline std::string toStr(UInt16 number)
			{
				ostringstream oss;
				oss << number;
				return oss.str();
			}

			/// Serializes given integer into a string.
			KATANA_BOE3_API void toStr(std::string &, Int32);

			/// Serializes given integer into a string.
			inline std::string toStr(Int32 number)
			{
				ostringstream oss;
				oss << number;
				return oss.str();
			}

			/// Serializes given integer into a string.
			KATANA_BOE3_API void toStr(std::string &, UInt32);

			/// Serializes given integer into a string.
			inline std::string toStr(UInt32 number)
			{
				ostringstream oss;
				oss << number;
				return oss.str();
			}

			/// Serializes given integer into a string.
			KATANA_BOE3_API void toStr(std::string &, Int64);

			/// Serializes given integer into a string.
			inline std::string toStr(Int64 number)
			{
				ostringstream oss;
				oss << number;
				return oss.str();
			}

			/// Serializes given integer into a string.
			KATANA_BOE3_API void toStr(std::string &, UInt64);

			/// Serializes given integer into a string.
			inline std::string toStr(UInt64 number)
			{
				ostringstream oss;
				oss << number;
				return oss.str();
			}

		} /* namespace Boe3 */
	} /* namespace CFE */
} /* namespace KTN */

#endif /* SRC_BOE3_INTEGRAL_HPP_ */
