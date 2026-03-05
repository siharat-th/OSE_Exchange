//============================================================================
// Name        	: AppMsgProc.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Jan 12, 2024 1:09:07 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_MSGPROC_APPMSGPROC_HPP_
#define SRC_EXCHSUPPORT_CME_MSGPROC_APPMSGPROC_HPP_

#pragma once

#include <KT01/Core/Log.hpp>
#include <Loggers/LogSvc.hpp>
#include <KT01/Core/Macro.hpp>
#include <KT01/Core/PerformanceCounter.h>
#include <KT01/Core/PerformanceTracker.hpp>

#include <exchsupport/cme/il3/IL3Includes.hpp>
#include <ExchangeHandler/session/SessionCallback.hpp>

#include <Orders/OrderPod.hpp>
#include <Orders/IdGen.h>
#include <Orders/OrderUtils.hpp>
#include <Orders/OrderPod.hpp>
#include <Orders/OrderPodPool.hpp>

using namespace KTN::Core;
using namespace KTN;


/**
 * @class AppMsgProc
 * @brief This class handles the processing of CME iLink3 application messages.
 */
class AppMsgProc {
public:
	/**
  * @brief Default constructor.
  */
	AppMsgProc();

	/**
  * @brief Destructor.
  */
	virtual ~AppMsgProc();

private:
	LogSvc * _logsvc; /**< Pointer to the LogSvc object. */

	int _MAXCBS = 20; /**< Maximum number of SessionCallback objects. */
	int _CBCNT; /**< Current count of SessionCallback objects. */
	SessionCallback* _cbs[20]; /**< Array of SessionCallback objects. */
};

#endif /* SRC_EXCHSUPPORT_CME_MSGPROC_APPMSGPROC_HPP_ */
