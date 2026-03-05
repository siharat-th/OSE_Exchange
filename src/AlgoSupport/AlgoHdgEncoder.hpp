//============================================================================
// Name        	: AlgoEncoders.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Feb 26, 2024 11:14:52 AM
//============================================================================

#ifndef SRC_ALGOSUPPORT_ALGOHDGENCODER_HPP_
#define SRC_ALGOSUPPORT_ALGOHDGENCODER_HPP_


#include <cstdint>
#include <iostream>
#include <KT01/Core/PerformanceTracker.hpp>

class AlgoHdgEncoder
{
public:
    //PerformanceTracker _trk;
    struct DecodedData {
        uint8_t callbackid;
        int32_t secid;
        int16_t differential;
        int8_t multiplier;
        uint32_t sequencing;
    };

    /**
     * Encodes the given data into a 64-bit value.
     * This is a super important concept- we encode some of our callback and hedge data into a 64-bit value
     * and pass it as part of the order to the exchange.  This is a way to pass data to the exchange, have the exchange
     * persist it, and then have it sent back to us in the execution report.  We can encode various data into this 64-bit
     *  value and then decode it when we get it back in the execution report. We like this because we use a 64-bit integer
     *  rather than a string, which is orders of magnitude slower to decode/decipher and make quick decisions on.
     *  So, in CME, we use the OrderRequestId field to pass this data back and forth, which is a 64-bit integer that we assign
     *  and it "lives" with the order throughout its order cycle, enabling us to efficiently pass data back and forth.
     *
     * @param callbackid The callback ID (8 bits).
     * @param secid The security ID (12 bits).
     * @param differential The differential value (16 bits).
     * @param multiplier The multiplier value (8 bits).
     * @param sequencing The sequencing value (20 bits).
     * @return The encoded 64-bit value.
     */
    static uint64_t encodeData(uint8_t callbackid, int32_t secid, int16_t differential, uint8_t multiplier, uint32_t sequencing) {
        uint64_t encodedValue = 0;

        // Assuming 8 bits for callbackid, 12 bits for secid, 16 bits for differential, 8 bits for multiplier, and 20 bits for sequencing

        encodedValue |= static_cast<uint64_t>(callbackid) << 56;            // 8 bits for callbackid, shifted to the left by 56 bits
        encodedValue |= static_cast<uint64_t>(secid & 0xFFF) << 44;        // 12 bits for secid, shifted to the left by 44 bits
        encodedValue |= static_cast<uint64_t>(differential & 0xFFFF) << 28; // 16 bits for differential, shifted to the left by 28 bits
        encodedValue |= static_cast<uint64_t>(multiplier) << 20;            // 8 bits for multiplier, shifted to the left by 20 bits
        encodedValue |= sequencing & 0xFFFFF;                               // 20 bits for sequencing

        return encodedValue;
    }

    /**
     * Decodes the given 64-bit value into its individual data components.
     *
     * @param encodedValue The encoded 64-bit value.
     * @return The decoded data structure.
     */
    static DecodedData decodeData(uint64_t encodedValue) {
        DecodedData result;

        result.callbackid = static_cast<uint8_t>(encodedValue >> 56);
        result.secid = static_cast<int32_t>((encodedValue >> 44) & 0xFFF);
        result.differential = static_cast<int16_t>((encodedValue >> 28) & 0xFFFF);
        result.multiplier = static_cast<int8_t>((encodedValue >> 20) & 0xFF);
        result.sequencing = static_cast<uint32_t>(encodedValue & 0xFFFFF);

        return result;
    }

};


#endif /* SRC_ALGOSUPPORT_ALGOHDGENCODER_HPP_ */
