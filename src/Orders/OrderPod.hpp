#ifndef ORDERPOD_HPP_
#define ORDERPOD_HPP_

#pragma once
#include <string>
#include <vector>
#include <string>       // std::string
#include <iostream>
#include <sstream>

#include <unordered_map>

#include <KT01/DataStructures/UserStructs.hpp>
#include <KT01/Core/PerformanceCounter.h>
#include <KT01/Core/Macro.hpp>

#include <Orders/OrderEnumsV2.hpp>

#include <akl/Price.hpp>

namespace KTN {
#pragma pack(push, 1)  // Disable padding for packing
/**
 * @brief Represents an order in the system.
 */
struct OrderPod
{
    char execid[40]; /**< The execution ID of the order. */
    char ordernum[20]; /**< The order number. */
    char origordernum[20]; /**< The original order number. */

    akl::Price price; /**< The price of the order. */
    akl::Price stopprice; /**< The stop limit price of the order. */
    akl::Price lastpx; /**< The last price of the order. */

    uint64_t timestamp; /**< The timestamp of the order. */
    uint64_t orderReqId; /**< The order request ID. */
    uint64_t exchordid { 0 }; /**< The CME ID of the order. */
    //uint64_t execid; /**< The execution ID of the order. */

    uint64_t recvTime;
    uint64_t queueWriteTime;

    //NOTE: this is changed for CFE handling of their weird secids which are base63 strings...
    uint64_t secid; /**< The security ID of the order. */
    
    uint32_t quantity; /**< The quantity of the order. */
    uint32_t leavesqty; /**< The leaves quantity of the order. */
    uint32_t fillqty; /**< The fill quantity of the order. */
    uint32_t lastqty; /**< The last quantity of the order. */
    uint32_t dispqty; /**< The display quantity of the order. Used in Icebergs */
    uint32_t minqty; /**< The minimum fill quantity of the order. Used in FAK and FOK */

    //uint32_t origqty; /**< The original quantity of the order. This is needed for CFE specifically in the case of a modify to a partially filled order. */
    //uint32_t origlvs;
    uint32_t tradeDate; /**< Trade date in YYYYMMDD format (from fill msg). */

    int16_t stratid; /**< The strategy ID of the order. */
    uint16_t rejreason; /**< The rejection reason of the order. */
    char rejcode;
    // Since this was only populated on messages coming from the exchange,
    // I'll reuse this for manual Tag50s, so that I don't have to add another field and increase the size of the class.
    char text[100]; /**< The text description of the order. */

    int8_t callbackid; /**< The callback ID of the order. */
    uint8_t mktsegid; /**< The market segment ID of the order. */
    uint8_t instindex; /**< The instrument index of the order. */

    bool isManual; /**< Indicates if the order is manual. */
    bool possRetransmit; /**< Indicates if the order is a possible retransmit. */
    bool hasMeta; /**< Indicates if the order has metadata that's been mapped */


    KTN::ORD::KOrderExchange::Enum OrdExch; /**< The exchange of the order. */
    KTN::ORD::KOrderType::Enum OrdType; /**< The type of the order. */
    KTN::ORD::KOrderTif::Enum OrdTif; /**< The time-in-force of the order. */
    KTN::ORD::KOrderAction::Enum OrdAction; /**< The action of the order. */
    KTN::ORD::KOrderSide::Enum OrdSide; /**< The side of the order. */
    KTN::ORD::KOrderStatus::Enum OrdStatus; /**< The status of the order. */
    KTN::ORD::KOrderState::Enum OrdState; /**< The state of the order. */
    KTN::ORD::KOrderFillType::Enum OrdFillType; /**< The fill type of the order. */
    KTN::ORD::KOrderAlgoTrigger::Enum OrdAlgoTrig; /**< The algorithm trigger of the order. */
    KTN::ORD::KOrderOpenCloseInd::Enum OrdOpenClose; /**< The open/close of the order. */
};//__attribute__((aligned(64)));
#pragma pack(pop)  // Restore default packing


struct OrderException
{
    int8_t callbackid; /**< The callback ID of the order exception. */
    uint8_t mktsegid; /**< The market segment ID of the order exception. */

    uint64_t exchordid; /**< The CME ID of the order exception. */
    uint16_t reason; /**< The reason code of the order exception. */
    std::string text; /**< The text description of the order exception. */
};


}

#endif
