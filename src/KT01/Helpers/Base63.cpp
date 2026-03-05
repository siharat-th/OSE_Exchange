#include "Base63.hpp"

namespace KT01 {
namespace Base63 {

// Helper function to build the lookup table.
static std::array<uint8_t, 128> make_lookup_table() {
    std::array<uint8_t, 128> table = {};
    // Initialize all entries to 255 (invalid)
    for (size_t i = 0; i < table.size(); ++i)
        table[i] = 255;
    // Fill in valid entries based on the alphabet.
    for (int i = 0; i < Base63Codec::BASE; ++i)
        table[static_cast<size_t>(Base63Codec::alphabet[i])] = static_cast<uint8_t>(i);
    return table;
}

// Define the static lookup_table member.
const std::array<uint8_t, 128> Base63Codec::lookup_table = make_lookup_table();

uint64_t Base63Codec::decode(const char* str) {
    uint64_t result = 0;
    for (int i = 0; i < 6; ++i) {
        char c = str[i];
        if (static_cast<unsigned char>(c) >= 128)
            throw std::runtime_error("Invalid character in input");
        uint8_t val = lookup_table[static_cast<size_t>(c)];
        if (val == 255)
            throw std::runtime_error("Invalid base63 character");
        result = result * BASE + val;
    }
    return result;
}

std::array<char, 6> Base63Codec::encode(uint64_t value) {
    if (value > Base63Codec::MAX_VALUE)
        throw std::runtime_error("Value too large for 6-char base63 encoding");
    std::array<char, 6> result = {};
    for (int i = 5; i >= 0; --i) {
        result[i] = alphabet[value % BASE];
        value /= BASE;
    }
    return result;
}

std::string Base63Codec::encode_to_string(uint64_t value) {
    if (value > Base63Codec::MAX_VALUE)
        throw std::runtime_error("Value too large for 6-char base63 encoding");

    std::string result(6, '\0');
    for (int i = 5; i >= 0; --i) {
        result[i] = alphabet[value % BASE];
        value /= BASE;
    }
    return result;
}


} // namespace Base63
} // namespace KT01
