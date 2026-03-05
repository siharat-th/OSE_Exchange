//============================================================================
// Name        	: OrderReporterV6.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 21, 2023 1:52:41 PM
//============================================================================

#ifndef ORDERREPORTERV6_HPP
#define ORDERREPORTERV6_HPP
#pragma once

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
using namespace rapidjson;

#include <Orders/OrderPod.hpp>

#include <Maps/SPSCRingBuffer.hpp>
using namespace KTN::Core;


#include <KT01/SecDefs/CmeSecMasterV2.hpp>
#include <KT01/SecDefs/CfeSecMaster.hpp>
#include <KT01/SecDefs/EquitySecMaster.hpp>
#include <KT01/SecDefs/OseSecMaster.hpp>
using namespace KT01::SECDEF::CME;
using namespace KT01::SECDEF::CFE;
using namespace KT01::SECDEF::EQT;

#include <AlgoSupport/algo_structs.hpp>

#include <akl/PriceConverters.hpp>
#include <exchsupport/cme/il3/IL3ErrorCodes.hpp>
using namespace KTN::CME::IL3;

using namespace KTN;
using namespace KTN::Core;

#include <tbb/concurrent_queue.h>
#include <KT01/Core/NamedThread.hpp>

namespace KTN {
namespace REPORTER{

/**
 * @brief The OrderReporterV6 class is responsible for reporting orders.
 */
class OrderReporterV6 : public NamedThread
{
public:
	/**
  * @brief Constructs an OrderReporterV6 object with the specified owner.
  * @param owner The owner of the OrderReporterV6 object.
  */
	OrderReporterV6(string owner);

	/**
  * @brief Destroys the OrderReporterV6 object.
  */
    ~OrderReporterV6();

    /**
     * @brief Reports the specified order.
     * @param ord The order to be reported.
     */
    void Report(const KTN::OrderPod& ord);

    /**
     * @brief Reports the specified order with metadata.
     * @param ord The order to be reported.
     * @param meta The metadata to be included with the order.
     */
    void ReportWithMeta(const KTN::OrderPod& ord, const algometa& meta);
    

    /**
     * @brief Stops the work of the OrderReporterV6 object.
     */
    void Stop();

    /**
     * @brief Updates the meta parameters with the specified values.
     * @param meta The meta parameters to be updated.
     * @param certnr A flag indicating whether to update the certificate number.
     */
    void UpdateMetaParams(const algometa& meta);

    /**
     * @brief Updates the UGO map with the specified request ID and user info.
     * @param reqid The request ID.
     * @param ugo The user info.
     */
    void UpdateUGOMap(uint64_t reqid, UserInfo ugo);

    /**
     * @brief Updates the tag map with the specified request ID and tag.
     * @param reqid The request ID.
     * @param tag The tag.
     */
    void UpdateTagMap(uint64_t reqid, string tag);

private:
    /**
     * @brief The Run method for the thread.
     */
    void Run() override;

    /**
     * @brief Formats the orders as JSON.
     * @param ords The orders to be formatted.
     * @return The formatted orders as JSON.
     */
    std::string FormatOrdersJson(std::vector<KTN::OrderPod>& ords);

    /**
     * @brief Gets the time formatted as a string.
     * @param ssboe The time in seconds since the beginning of epoch.
     * @return The formatted time string.
     */
    string GetTimeFormatted(uint64_t ssboe);

    /**
     * @brief Gets the legacy report type for the specified order.
     * @param ord The order.
     * @return The legacy report type.
     */
    string LegacyRptType(const KTN::OrderPod& ord);

    /**
     * @brief Gets the legacy action for the specified order.
     * @param ord The order.
     * @return The legacy action.
     */
    string LegacyAction(const KTN::OrderPod& ord);

    /**
     * @brief Gets the legacy state for the specified order.
     * @param ord The order.
     * @return The legacy state.
     */
    string LegacyState(const KTN::OrderPod& ord);

    /**
     * @brief Gets the order tag for the specified order.
     * @param ord The order.
     * @return The order tag.
     */
    string OrderTag(const KTN::OrderPod& ord);

    /**
     * @brief Gets the reject text for the specified order.
     * @param ord The order.
     * @return The reject text.
     */
    string RejectText(const KTN::OrderPod& ord);

private:
    std::atomic<bool> _ACTIVE;

    algometa _PARAMS;

    std::string _ORG;
    std::string _SRC;
    string _KEY;
    string _ACCT;

    bool _SHOW = false;

    tbb::concurrent_queue<KTN::OrderPod> _qOrds;

    std::unordered_map<uint64_t, algometa> _algometaMap;

    std::unordered_map<uint64_t, UserInfo> _ugoMap;
    std::unordered_map<uint64_t, string> _tagmap;
    std::unordered_map<int32_t, string> _certIdMap;
};

} }
#endif
