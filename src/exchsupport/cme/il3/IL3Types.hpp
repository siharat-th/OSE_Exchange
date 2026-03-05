//============================================================================
// Name        	: IL3Types.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 31, 2023 7:38:13 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3TYPES_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3TYPES_HPP_

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <stdint.h>

namespace KTN{
namespace CME{
namespace IL3{


	inline static const std::uint16_t SCHEMA_ID=8;
	inline static const std::uint16_t VERSION=8;
	inline static const std::uint16_t ENCODING_TYPE=0xcafe;

	inline static const std::uint16_t SOH_HDR_SIZE = 4;
	inline static const std::uint16_t SBE_HDR_SIZE = 8;

	// CHAR
	typedef char CHAR;

	// ClientFlowType
	// char
	inline static const char ClientFlowType[] = "IDEMPOTENT";

	/**** NULL DEFINITTIONS FROM CME ILINK XML FILE 
	<type name="uInt16" description="uInt16" primitiveType="uint16" semanticType="int"/>
	<type name="uInt16NULL" description="Unsigned Int16 with NULL" presence="optional" nullValue="65535" primitiveType="uint16" semanticType="int"/>
	<type name="uInt32" description="uInt32" primitiveType="uint32" semanticType="int"/>
	<type name="uInt32NULL" description="uInt32NULL" presence="optional" nullValue="4294967295" primitiveType="uint32" semanticType="int"/>
	<type name="uInt64" description="uInt64" primitiveType="uint64" semanticType="int"/>
	<type name="uInt64NULL" description="uInt64NULL" presence="optional" nullValue="18446744073709551615" primitiveType="uint64" semanticType="int"/>
	<type name="uInt8" description="uInt8" primitiveType="uint8"/>
	<type name="uInt8NULL" description="uInt8NULL" presence="optional" nullValue="255" primitiveType="uint8"/>
	*/

	inline static const int64_t PRICE_NULL = 9223372036854775807LL;
	inline static const uint8_t UINT8_NULL = 255;
	inline static const uint8_t BOOLEAN_NULL = 255;
	inline static const uint8_t ENUM_NULL = 255;
	inline static const uint8_t CHAR_NULL = 0;
	inline static const uint16_t UINT16_NULL = 65535;
	inline static const uint32_t UINT32_NULL = 4294967295;
	inline static const int32_t INT32_NULL = 2147483647;
	inline static const uint64_t UINT64_NULL = 18446744073709551615ULL;


	inline static const char HMACVersion[] = "CME-1-SHA-256";

	// Int32
	typedef int32_t Int32;

	// Int32NULL
	typedef int32_t Int32NULL; // Optional, NULL value: 2147483647

	// String10Req
	typedef char String10Req[10];

	// String17
	typedef char String17[17]; // Optional

	// String2
	typedef char String2[2];

	// String20
	typedef char String20[20]; // Optional

	// String20Req
	typedef char String20Req[20];

	// String256
	typedef char String256[256]; // Optional

	// String3
	typedef char String3[3]; // Optional

	// String30
	typedef char String30[30]; // Optional

	// String30Req
	typedef char String30Req[30];

	// String32Req
	typedef char String32Req[32];

	// String3Req
	typedef char String3Req[3];

	// String40
	typedef char String40[40];

	// String48
	typedef char String48[48]; // Optional

	// String5
	typedef char String5[5]; // Optional

	// String5Req
	typedef char String5Req[5];

	// String75
	typedef char String75[75]; // Optional

	// String8
	typedef char String8[8]; // Optional

	// String8Req
	typedef char String8Req[8];

	// StringLength35
	typedef char StringLength35[35]; // Optional

	// StringLength6
	typedef char StringLength6[6]; // Optional

	// UDI
	// UDI
	inline static const char UDI = 'Y';

	// charNULL
	typedef char charNULL; // Optional, NULL value: 0

	// enumNULL
	typedef uint8_t enumNULL; // Optional, NULL value: 255

	// uInt16
	typedef uint16_t uInt16;

	// uInt16NULL
	typedef uint16_t uInt16NULL; // Optional, NULL value: 65535

	// uInt32
	typedef uint32_t uInt32;

	// uInt32NULL
	typedef uint32_t uInt32NULL; // Optional, NULL value: 4294967295

	// uInt64
	typedef uint64_t uInt64;

	// uInt64NULL
	typedef uint64_t uInt64NULL; // Optional, NULL value: 18446744073709551615

	// uInt8
	typedef uint8_t uInt8;

	// uInt8NULL
	typedef uint8_t uInt8NULL; // Optional, NULL value: 255

}
}
}

#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3TYPES_HPP_ */
