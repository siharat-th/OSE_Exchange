//
//	limeTypes.hh
//
//  Copyright 2010 Lime Brokerage LLC.  All rights reserved.
//
//

#ifndef LIME_TYPES_HH_
#define LIME_TYPES_HH_

#include <string>

extern "C" {
#include <stdint.h>
}

namespace LimeBrokerage {

    // All orders are identified by an ID of type OrderId.
    typedef uint64_t OrderId;
    // All prices are submitted and returned in ScaledPrice format. This means
    // that the prices are multiplied by a fixed factor to accommodate precision
    // of up to N digits after the decimal point. The default precision is N=4
    // meaning that the dollar values is obtained by dividing the API values by
    // priceScalingFactor = 10,000 (see below).
    typedef int64_t ScaledPrice;
    // The precision of strike price specified for US Options is N=3 so
    // a separate type is used for defining strike prices.
    // usOptionStrikePriceScalingFactor = 1,000 (see below).
    typedef uint32_t USOptionScaledStrikePrice;
    // The precision of the the minimumTriggerPercent is N=2, so in order to
    // obtain the percentage the API value needs to be divided by
    // percentScalingFactor = 100 (see below).
    typedef uint32_t ScaledPercent;
    // Submit AlgoParticipation as a fixed-point number, with an implicit
    // decimal. i.e., multiply your desired value by
    // algoParticipationScalingFactor (see below).
    typedef uint32_t AlgoParticipation;

    typedef uint32_t PurgeId;

    enum Side {
        sideBuy,
        sideSell,
        sideSellShort,
        sideSellShortExempt,
        sideBuyToCover               // identical to Buy, the only difference being
    };                               // how the trade will be reported to clearing

    enum PositionEffect {
        positionEffectNone = 0,
        positionEffectOpen,
        positionEffectClose
    };

    enum AlgoTracking {
        algoTrackingNone,

        algoTrackingApRevertLow,
        algoTrackingApRevertMed,
        algoTrackingApRevertHigh,
        algoTrackingApTrendLow,
        algoTrackingApTrendMed,
        algoTrackingApTrendHigh,

        algoTrackingSpRevertLow,
        algoTrackingSpRevertMed,
        algoTrackingSpRevertHigh,
        algoTrackingSpTrendLow,
        algoTrackingSpTrendMed,
        algoTrackingSpTrendHigh,

        algoTrackingSectorRevertLow,
        algoTrackingSectorRevertMed,
        algoTrackingSectorRevertHigh,
        algoTrackingSectorTrendLow,
        algoTrackingSectorTrendMed,
        algoTrackingSectorTrendHigh,

        // NOTE: The RefPx options require RefPx to be included in the
        // AlgoOrderProperties.
        algoTrackingRefPxRevertLow,
        algoTrackingRefPxRevertMed,
        algoTrackingRefPxRevertHigh,
        algoTrackingRefPxTrendLow,
        algoTrackingRefPxTrendMed,
        algoTrackingRefPxTrendHigh,

        algoTrackingCount
    };

    typedef uint32_t AlgoBlockLimit;

    // A trading call is accepted by the API if and only if it can be
    // transmitted to Lime trading platform. Thus, if the connection with
    // trading platform is congested or down, calls are synchronously rejected
    enum CallStatus {
        statusSuccess,
        statusConnectionBusy,
        statusConnectionError,
        statusMax = statusConnectionError
    };

    static const int priceScalingFactor = 10000;
    static const int usOptionStrikePriceScalingFactor = 1000;
    static const int percentScalingFactor = 100;
    static const int algoParticipationScalingFactor = 100;

    // This structure specifies mandatory fields specific
    // to a US options order. It is also returned back in
    // fill or manual order echo callabcks.
    struct USOptionSymbol {
        enum PutOrCall {
            put = 0,
            call
        };
        USOptionSymbol() :
            baseSymbol(), putOrCall(put), expirationYear(0),
            expirationMonth(0), expirationDay(0), strikePrice(0) {}

        std::string baseSymbol;
        PutOrCall putOrCall;
        uint8_t expirationYear;         // last 2 digits (YY)
        uint8_t expirationMonth;        // MM
        uint8_t expirationDay;          // DD
        USOptionScaledStrikePrice strikePrice;
    };

    //Lime Exceptions
    class Exception {
    public:
        Exception(const std::string& message) : message(message) {}
        ~Exception() {}
        const char* what() const { return message.c_str(); }
    private:
        const std::string message;
    };
}

#endif
