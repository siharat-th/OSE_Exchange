//============================================================================
// Name        	: AlgoV3Reporter.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 31, 2023 1:52:41 PM
//============================================================================

#ifndef ALGOV3REPORTER_HPP
#define ALGOV3REPORTER_HPP
#pragma once

#include <Reporters/ReporterBase.hpp>
#include <AlgoSupport/algo_structs.hpp>
#include <AlgoSupport/AlgoJson.hpp>
#include <Maps/SPSCRingBuffer.hpp>
//#include <tbb/concurrent_queue.h>
//using namespace tbb;

using namespace KTN;
using namespace KTN::Core;

namespace KTN {
namespace REPORTER{

/**
 * @brief The AlgoV3Reporter class is responsible for reporting generic statistics.
 */
class AlgoV3Reporter : ReporterBase
{
public:
	/**
  * @brief Constructs an instance of AlgoV3Reporter.
  */
	AlgoV3Reporter();

	/**
  * @brief Destroys the AlgoV3Reporter instance.
  */
    ~AlgoV3Reporter();

    /**
     * @brief Reports the generic statistics.
     * @param msg The generic statistics to report.
     */
    void Report(const genericstats& msg);

    /**
     * @brief Performs the work of the reporter.
     */
    virtual void DoWork() override;

    /**
     * @brief Stops the work of the reporter.
     */
    void StopWork();

    /**
     * @brief Sends a message with generic statistics, ssboe, and updtime.
     * @param gmsg The generic statistics to send.
     * @param ssboe The ssboe value.
     * @param updtime The updtime value.
     */
    void SendMessage(const genericstats& gmsg, uint64_t ssboe, const string& updtime);


private:
    string _KEY; /**< The key. */
    string _ACCT; /**< The account. */

    SPSCRingBuffer<genericstats> _qMsgs; /**< The message queue. */
    //tbb::concurrent_queue<genericstats> _qMsgs;

    const int _hb_interval = 2; /**< The heartbeat interval. */
    uint64_t _ssboe_us; /**< The ssboe value. */

    std::unordered_map<string, genericstats> _stats; /**< The statistics map. */

};

} }
#endif
