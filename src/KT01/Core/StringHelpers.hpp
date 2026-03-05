#ifndef STRINGHELPERS_HPP
#define STRINGHELPERS_HPP


#include <string>
#include <sstream>
#include <cstring>

class StringHelpers
{
public:
static char* ToAlphanumeric(const std::string& input, size_t length) {
    char* result = new char[length];
    size_t j = 0;

    // Fill with spaces
    std::memset(result, ' ', length);

    // Copy non-null characters, left-justified
    for (char c : input) {
        if (c == '\0') continue;
        if (j >= length) break;
        result[j++] = c;
    }

    return result;
};

};

#endif /* STRINGHELPERS_HPP */