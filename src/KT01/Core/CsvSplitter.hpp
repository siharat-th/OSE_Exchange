#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iterator>

using namespace std;

class CsvSplitter {
public:
    static std::vector<std::string> Split(const std::string& line) {
        std::vector<std::string> result;
        //std::string token;
        bool inQuotes = false;
        std::stringstream ss;

        for (char ch : line) {
            if (ch == '\"') {
                inQuotes = !inQuotes;
            }
            else if (ch == ',' && !inQuotes) {
                result.push_back(ss.str());
                ss.str("");
                ss.clear();
            }
            else {
                ss << ch;
            }
        }
        result.push_back(ss.str());
        return result;
    }
};
