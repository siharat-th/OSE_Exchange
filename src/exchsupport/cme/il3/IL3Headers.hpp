//============================================================================
// Name        	: IL3Headers.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 11, 2023 5:20:08 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3HEADERS_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3HEADERS_HPP_

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <ostream>


namespace KTN{
namespace CME{
namespace IL3{

struct SOFH { //Simple Open Framing Header
    std::uint16_t msgSize_;
    std::uint16_t enodingType_;
};

struct SBEHeader {//Simple Binary Encoding Header
    std::uint16_t blockLength_;
    std::uint16_t templateId_;
    std::uint16_t schemaId_;
    std::uint16_t version_;
};



} } }



#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3HEADERS_HPP_ */
