#ifndef KT01_DATASTRUCTURES_MARKETDEPTH_DEPTHENUMS_HPP
#define KT01_DATASTRUCTURES_MARKETDEPTH_DEPTHENUMS_HPP

#pragma once


#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdint>
#include <unordered_map>

#include <tbb/concurrent_queue.h>

#include <akl/PriceConverters.hpp>

#include <akl/Price.hpp>
using namespace akl;


using namespace std;

namespace KT01 {
namespace DataStructures {
namespace MarketDepth {

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

	enum BookSecurityStatus : uint8_t {
		TradingHalt = 2,
		Close = 4,
		NewPriceIndication = 15,
		ReadyToTrade = 17,
		NotAvailableForTrading = 18,
		UnknownOrInvalid = 20,
		PreOpen = 21,
		PreCross = 24,
		Cross = 25,
		PostClose = 26,
		NoChange = 103,
		PrivateWorkup = 201,
		PublicWorkup = 202
	};

	enum BookSecurityTradingEvent : uint8_t {
		NoEvent = 0,
		NoCancel = 1,
		ResetStatistics = 4,
		ImpliedMatchingON = 5,
		ImpliedMatchingOFF = 6,
		EndOfWorkup = 7
	};


} // namespace MktData
} // namespace DataStructures
} // namespace KTN


#endif
