//============================================================================
// Name        	: StringUtils.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Jun 7, 2024 10:36:59 AM
//============================================================================

#ifndef SRC_CORE_STRINGUTILS_HPP_
#define SRC_CORE_STRINGUTILS_HPP_

#include <string>
#include <algorithm>

/**
 * @brief A utility class for string manipulation.
 */
class StringUtils {
public:
    /**
     * @brief Removes all occurrences of double quotes from the input string.
     * @param input The input string.
     * @return The input string with all double quotes removed.
     */
    static std::string RemoveDoubleQuotes(const std::string& input) {
        std::string result = input;
        result.erase(std::remove(result.begin(), result.end(), '\"'), result.end());
        return result;
    }

    /**
     * @brief Removes all occurrences of single quotes from the input string.
     * @param input The input string.
     * @return The input string with all single quotes removed.
     */
    static std::string RemoveSingleQuotes(const std::string& input) {
		std::string result = input;
		result.erase(std::remove(result.begin(), result.end(), '\''), result.end());
		return result;
	}
};

#endif /* SRC_CORE_STRINGUTILS_HPP_ */
