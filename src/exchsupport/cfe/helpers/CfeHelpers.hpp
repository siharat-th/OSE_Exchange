//============================================================================
// Name        	: CfeHelpers.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 1, 2024 12:40:03 PM
//============================================================================

#ifndef SRC_CFE_HELPERS_HPP_
#define SRC_CFE_HELPERS_HPP_

#include <string>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace KTN
{
    namespace CFE
    {
        namespace Helpers
        {

            class CfeHelpers
            {
            public:
                static std::string CfeSpreadSymbol(const std::string &input)
                {
                    // Find the delimiter " - " separating the two symbols.
                    size_t delimPos = input.find(" - ");
                    if (delimPos == std::string::npos)
                    {
                        return "";
                    }

                    // Extract the left and right parts.
                    std::string leftSymbol = input.substr(0, delimPos);
                    std::string rightSymbol = input.substr(delimPos + 3); // skip " - "

                    // Process left symbol: get the part before the first colon.
                    size_t colonPos = leftSymbol.find(':');
                    if (colonPos != std::string::npos)
                    {
                        leftSymbol = leftSymbol.substr(0, colonPos);
                    }
                    // Remove any slash from the left symbol.
                    leftSymbol.erase(std::remove(leftSymbol.begin(), leftSymbol.end(), '/'), leftSymbol.end());

                    // Process right symbol similarly.
                    colonPos = rightSymbol.find(':');
                    if (colonPos != std::string::npos)
                    {
                        rightSymbol = rightSymbol.substr(0, colonPos);
                    }
                    rightSymbol.erase(std::remove(rightSymbol.begin(), rightSymbol.end(), '/'), rightSymbol.end());

                    // Combine the processed symbols with a hyphen.
                    return leftSymbol + "-" + rightSymbol;
                };


                static std::string BinaryDateToFuturesCode(uint32_t yyyymmdd)
                {
                    static const char* MONTH_CODES = "FGHJKMNQUVXZ";
                
                    uint32_t year = yyyymmdd / 10000;
                    uint32_t month = (yyyymmdd / 100) % 100;
                
                    if (month < 1 || month > 12)
                        throw std::invalid_argument("Invalid month in binary date");
                
                    char monthCode = MONTH_CODES[month - 1];
                    char yearDigit = '0' + (year % 10);
                
                    return std::string{monthCode, yearDigit};
                };

                static std::string StripRightSpaces(const std::string& s)
                {
                    size_t end = s.find_last_not_of(' ');
                    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
                }

                static std::string CfeSymbolFromRaw(const char* buf, size_t len, uint32_t contract_date)
                {
                    if (len < 6)
                        throw std::runtime_error("CfeSymbol: Buffer length is less than 6 bytes");

                    // Create a string from the first 6 bytes and strip right spaces
                    std::string szbuf(buf, 6);
                    //we are purposefully not inlcluding the forward slash to align with our normalized symbol
                    std::string symb =  StripRightSpaces(szbuf) + 
                        CfeHelpers::BinaryDateToFuturesCode(contract_date);
                    
                    //cout << "[DEBUG] CfeSymbolFromRaw: " << szbuf << " -> " << symb << endl;
                    return symb;
                }

                static std::string CfeSpreadSymbolNorm(const std::string& input_str) {
                    size_t delim_pos = input_str.find(" - ");
                    if (delim_pos == std::string::npos) {
                        return "";
                    }
            
                    std::string left_symbol = input_str.substr(0, delim_pos);
                    std::string right_symbol = input_str.substr(delim_pos + 3);
            
                    size_t colon_pos = left_symbol.find(':');
                    if (colon_pos != std::string::npos) {
                        left_symbol = left_symbol.substr(0, colon_pos);
                    }
                    left_symbol.erase(std::remove(left_symbol.begin(), left_symbol.end(), '/'), left_symbol.end());
            
                    colon_pos = right_symbol.find(':');
                    if (colon_pos != std::string::npos) {
                        right_symbol = right_symbol.substr(0, colon_pos);
                    }
                    right_symbol.erase(std::remove(right_symbol.begin(), right_symbol.end(), '/'), right_symbol.end());
            
                    return left_symbol + "-" + right_symbol;
                };


            };

        } // namespace Helpers
    } // namespace CFE
} // namespace KTN
#endif /* SRC_CFE_HELPERS_HPP_ */
       //============================================================================