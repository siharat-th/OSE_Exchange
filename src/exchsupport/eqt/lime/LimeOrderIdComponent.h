#ifndef SRC_LIMEORDERIDCOMPONENT_HPP_
#define SRC_LIMEORDERIDCOMPONENT_HPP_
#pragma once

#include <string>

// OF-LIME-ECN5-E-20250603-000003TG1A112GE2JISHG53
struct LimeOrderIdComponents {
    std::string venue;         // "OF-LIME-ECN5"
    std::string side;          // "E" (Equity)
    std::string date;          // "20250603"
    std::string sequenceNum;   // "000003"
    std::string identifier;    // "TG1A112GE2JISHG53"
    uint64_t base63Value;      // Decoded from "TG1A112GE2JISHG53"
};

LimeOrderIdComponents parseLimeOrderId(const std::string& orderIdStr) {
    LimeOrderIdComponents components;

    // Parse the string components (adjust positions as needed for your format)
    size_t pos1 = orderIdStr.find('-', 0);
    size_t pos2 = orderIdStr.find('-', pos1 + 1);
    size_t pos3 = orderIdStr.find('-', pos2 + 1);
    size_t pos4 = orderIdStr.find('-', pos3 + 1);

    components.venue = orderIdStr.substr(0, pos4);
    components.side = orderIdStr.substr(pos4 + 1, 1);
    components.date = orderIdStr.substr(pos4 + 3, 8);
    components.sequenceNum = orderIdStr.substr(pos4 + 12, 6);

    // Extract the base63 part (last 17 chars)
    components.identifier = orderIdStr.substr(orderIdStr.length() - 17);
    components.base63Value = KT01::Base63::Base63Codec::decode(components.identifier.c_str());

    return components;
}

#endif // !SRC_LIMEORDERIDCOMPONENT_HPP_



