//============================================================================
// Name        	: IL3Composites.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 31, 2023 7:41:45 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3COMPOSITES_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3COMPOSITES_HPP_

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <stdint.h>

namespace KTN{
namespace CME{
namespace IL3{

	struct DATA {
		uint16_t length;
		char varData[1];  // Assuming variable length data, adjust size as needed
	}__attribute__((packed));

	struct Decimal32NULL {
		int32_t mantissa;
		int8_t exponent;
	}__attribute__((packed));

	struct Decimal64NULL {
		int64_t mantissa;
		int8_t exponent;
	}__attribute__((packed));

	struct MaturityMonthYear {
		uint16_t year;
		uint8_t month;
		uint8_t day;
		uint8_t week;
	}__attribute__((packed));

	struct PRICE9 {
		int64_t mantissa;
		//int8_t exponent;
	}__attribute__((packed));

	struct PRICENULL9 {
		int64_t mantissa;
		//int8_t exponent;
	}__attribute__((packed));

	struct groupSize {
		uint16_t blockLength;
		uint8_t numInGroup;
	}__attribute__((packed));

	struct groupSizeEncoding {
		uint16_t blockLength;
		uint16_t numInGroup;
	}__attribute__((packed));

	struct messageHeader {
		uint16_t blockLength;
		uint16_t templateId;
		uint16_t schemaId;
		uint16_t version;
	}__attribute__((packed));

	struct sofhHeader{
		uint16_t messageLength;
		uint16_t encodingType = 0XCAFE;
	}__attribute__((packed));

}
}
}



#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3COMPOSITES_HPP_ */
