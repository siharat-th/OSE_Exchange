#ifndef KT01_ORDERBOOKENUMS_HPP
#define KT01_ORDERBOOKENUMS_HPP

#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdint>


using namespace std;

namespace KT01 {
namespace OrderBook {
namespace MarketDepth {

    enum OrderBookState : uint8_t {
        NoState = 0,
        SpinRequested = 1,
        Recovering = 2,
        BuildingBooks = 3,
        Dequeuing = 4,
        Current = 5,
        GapDetected = 6
    };

    static string toString(OrderBookState value) {
        switch (value) {
            case NoState:
                return "NoState";
            case SpinRequested:
                return "SpinRequested";
            case Recovering:
                return "Recovering";
            case BuildingBooks:
                return "BuildingBooks";
            case Dequeuing:
                return "Dequeuing";
            case Current:
                return "Current";
            case GapDetected:
                return "GapDetected";
            default:
                return "UNKNOWN BOOK STATE";
        }
    };
    
    enum BookSide : uint8_t {
        Bid    = 1,
        Ask    = 2,
        ImpBid = 3,
        ImpAsk = 4
    };

    enum BookAction : uint8_t {
        New    = 1,
        Change = 2,
        Del    = 3
    };


} // namespace OrderBook
} // namespace KT01
} // namespace MarketDepth
//============================================================================
 #endif /* KT01_ORDERBOOKENUMS_HPP */