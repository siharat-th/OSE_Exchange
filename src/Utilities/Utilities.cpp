#include <Utilities/Utilities.hpp>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <cstdarg>

using namespace std;
using namespace AKL::Utilities;
using namespace AKL::MathUtilities;
using namespace AKL::StringUtilities;

vector<string> SplitString(const string& source, const string& delimiter, bool ignoreEmpty, bool trim) {
    vector<string> result;

    if (delimiter.empty()) {
        result.push_back(source);
        return result;
    }

    string::const_iterator substart = source.begin(), subend;

    while (true) {
        subend = search(substart, source.end(), delimiter.begin(), delimiter.end());
        string sub(substart, subend);

        if (trim) {
            sub = AKL::StringUtilities::Trim(sub);
        }

        if (!ignoreEmpty || !sub.empty()) {
            result.push_back(sub);
        }

        if (subend == source.end()) {
            break;
        }

        substart = subend + delimiter.size();
    }

    return result;
}

string JoinString(const vector<string>& stringElements, const string& delimiter) {
    string str;

    if (stringElements.empty()) {
        return str;
    }

    auto numStrings = stringElements.size();
    string::size_type strSize = delimiter.size() * (numStrings - 1);

    for (const string& s : stringElements) {
        strSize += s.size();
    }

    str.reserve(strSize);
    str += stringElements[0];

    for (decltype(numStrings) i = 1; i < numStrings; ++i) {
        str += delimiter;
        str += stringElements[i];
    }

    return str;
}

void StringReplace(string& input, const string& searchStr, const string& replaceStr) {
    auto pos = input.find(searchStr);

    while (pos != std::string::npos) {
        input.replace(pos, searchStr.size(), replaceStr);
        pos += replaceStr.size();
        std::string nextfind(input.begin() + pos, input.end());
        auto nextpos = nextfind.find(searchStr);

        if (nextpos == std::string::npos) {
            break;
        }

        pos += nextpos;
    }
}

bool IsStringInString(const string& haystack, const string& needle) {
    return haystack.find(needle) != std::string::npos;
}

bool IsStringEndsWith(std::string const& value, std::string const& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool IsStringStartsWith(std::string const& value, std::string const& starting) {
    if (starting.size() > value.size()) return false;
    return !value.compare(0, starting.length(), starting);
}

// Check how many times a string contains another string.
std::size_t StringTimesInString(const string& haystack, const string& needle) {
    const size_t step = needle.size();

    size_t count(0);
    size_t pos(0);

    while ((pos = haystack.find(needle, pos)) != std::string::npos) {
        pos += step;
        ++count;
    }

    return count;
}

bool StringIsDigits(const std::string& str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}

string AKL::StringUtilities::TrimFront(const string& src) {
    auto dst = src;
    dst.erase(dst.begin(), std::find_if_not(dst.begin(), dst.end(), [&](char& c) {return ::isspace(c); }));
    return dst;
}
string AKL::StringUtilities::TrimBack(const string& src) {
    auto dst = src;
    dst.erase(std::find_if_not(dst.rbegin(), dst.rend(), [&](char& c) {return ::isspace(c); }).base(), dst.end());
    return dst;
}
string AKL::StringUtilities::Trim(const string& src) {
    return AKL::StringUtilities::TrimFront(AKL::StringUtilities::TrimBack(src));
}

string VAArgsToString(const char* format, ...) {
    // variadic args to string:
    char aux_buffer[10000];
    string retStr("");

    if (nullptr != format) {
        va_list marker;

        // initialize variable arguments
        va_start(marker, format);

        // Get formatted string length adding one for nullptr
        size_t len = vsprintf(aux_buffer, format, marker) + 1;

        // Reset variable arguments
        va_end(marker);

        if (len > 0) {
            va_list args;

            // initialize variable arguments
            va_start(args, format);

            // Create a char vector to hold the formatted string.
            vector<char> buffer(len, '\0');
            vsprintf(&buffer[0], format, args);
            retStr = &buffer[0];
            va_end(args);
        }

    }

    return retStr;
}

string VAArgsToString(const char* format, va_list args) {
    // variadic args to string:
    char aux_buffer[10000];
    string retStr("");

    if (nullptr != format) {

        // Get formatted string length adding one for nullptr
        vsprintf(aux_buffer, format, args);
        retStr = aux_buffer;

    }

    return retStr;
}

int ToInt(const string& intString) {
    int x = 0;
    istringstream cur(intString);
    cur >> x;
    return x;
}

float ToFloat(const string& floatString) {
    float x = 0;
    istringstream cur(floatString);
    cur >> x;
    return x;
}