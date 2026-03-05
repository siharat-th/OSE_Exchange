//============================================================================
// Name        	: Reporter.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 21, 2023 1:52:41 PM
//============================================================================

#ifndef ALGOPLREPORTER_HPP
#define ALGOPLREPORTER_HPP

#include <Reporters/ReporterBase.hpp>
#include <AlgoSupport/algo_structs.hpp>
#include <AlgoSupport/AlgoJson.hpp>
#include <Maps/SPSCRingBuffer.hpp>
using namespace KTN::Core;

#include <chrono>
using namespace std::chrono;

#include <AlgoParams/Position.hpp>

using namespace std;

namespace KTN {
namespace REPORTER{

/**
 * @brief The AlgoPLReporter class is responsible for reporting the position data.
 */
class AlgoPLReporter : ReporterBase
{
public:
	/**
  * @brief Constructs an AlgoPLReporter object with the specified GUID.
  * @param guid The GUID of the reporter.
  */
	AlgoPLReporter(string guid);

	/**
  * @brief Destroys the AlgoPLReporter object.
  */
    ~AlgoPLReporter();

    /**
     * @brief Reports the position data.
     * @param data The position data to be reported.
     */
    void Report(Position& data);

    /**
     * @brief Performs the work of the reporter.
     */
    virtual void DoWork() override;

    /**
     * @brief Stops the work of the reporter.
     */
    void StopWork();


private:

    SPSCRingBuffer<Position> _qPos; /**< The position queue. */
    const int _hb_interval = 2; /**< The heartbeat interval. */
    uint64_t _ssboe_us; /**< The start time in microseconds. */
    string _GUID; /**< The GUID of the reporter. */

    std::unordered_map<string, Position> _positions; /**< The map of positions. */

};

} }
#endif
