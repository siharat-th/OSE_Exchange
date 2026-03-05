#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iterator>

using namespace std;

class StringSplitter
{
public:
    /**
     * Splits a string into multiple tokens based on a delimiter.
     * 
     * @param src The source string to split.
     * @param delim The delimiter to split the string by.
     * @return A vector of strings containing the tokens.
     */
    inline static std::vector<std::string> Split(const std::string& src, const std::string& delim)
    {
        std::vector<std::string> tokens;
        std::string::size_type start = 0;
        std::string::size_type end;
        for (;;) {
            end = src.find(delim, start);
            tokens.push_back(src.substr(start, end - start));
            // We just copied the last token
            if (end == std::string::npos)
                break;
            // Exclude the delimiter in the next search
            start = end + delim.size();
        }
        return tokens;
    }

    /**
     * Checks if a string contains a specific delimiter.
     * 
     * @param szLine The string to check.
     * @param szDelim The delimiter to search for.
     * @return True if the delimiter is found, false otherwise.
     */
    inline static bool CheckDelim(std::string szLine, std::string szDelim)
    {
        if (szLine.find(szDelim) == std::string::npos)
            return false;
        else
            return true;
    }

    /**
     * Splits a string into multiple tokens based on a single character delimiter.
     * 
     * @param input The input string to split.
     * @param delim The delimiter character to split the string by.
     * @return A vector of strings containing the tokens.
     */
    inline static std::vector<std::string> SplitStream(std::string input, char delim)
    {
        std::vector<std::string> tokens;
        std::stringstream check1(input);
        std::string intermediate;
        while (getline(check1, intermediate, delim))
        {
            tokens.push_back(intermediate);
        }
        return tokens;
    }

    /**
     * Converts a string to uppercase.
     * 
     * @param s The string to convert.
     */
    inline static void toUpperCase(std::string& s)
    {
        for (unsigned int l = 0; l < s.length(); l++)
        {
            s[l] = toupper(s[l]);
        }
    }

    /**
     * Joins a vector of strings into a single string using a delimiter.
     * 
     * @param strings The vector of strings to join.
     * @param delimiter The delimiter to insert between the strings.
     * @return The joined string.
     */
    inline static std::string join(const std::vector<std::string>& strings, const std::string& delimiter)
    {
        switch (strings.size()) {
        case 0:
            return "";
        case 1:
            return strings[0];
        default:
            std::ostringstream joined;
            std::copy(strings.begin(), strings.end() - 1, std::ostream_iterator<std::string>(joined, delimiter.c_str()));
            joined << *strings.rbegin();
            return joined.str();
        }
    }
};
