#ifndef KT01_BASE63_HPP
#define KT01_BASE63_HPP


#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>

namespace KT01 {
namespace Base63 {

class Base63Codec {
public:
    // Base-63 alphabet: exactly 63 characters.
    static constexpr char alphabet[64] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
    static constexpr int BASE = 63;
    static constexpr uint64_t MAX_VALUE = 569468382239ULL; // 63^6 - 1

    // Declaration of the lookup table for decoding.
    // Its definition is provided in Base63.cpp.
    static const std::array<uint8_t, 128> lookup_table;

    // Decode a 6-character base-63 encoded string to a uint64_t.
    // The input string must be at least 6 characters.
    static uint64_t decode(const char* str);

    // Encode a uint64_t into a 6-character base-63 representation.
    static std::array<char, 6> encode(uint64_t value);
    static std::string encode_to_string(uint64_t value);
};

} // namespace Base63
} // namespace KT01

#endif // KT01_BASE63_HPP