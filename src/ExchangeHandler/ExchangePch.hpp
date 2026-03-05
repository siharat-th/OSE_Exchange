//============================================================================
// Name        	: ExchangePch.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 23, 2023 5:45:31 PM
//============================================================================

#ifndef SRC_ExchangeHandler_EXCHANGEPCH_HPP_
#define SRC_ExchangeHandler_EXCHANGEPCH_HPP_

#pragma once

#include <stdio.h>
#include <unordered_map>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <memory>
#include <stdio.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
using namespace rapidjson;


#include <Rest/JsonUrlReader.hpp>
#include <Rest/RestApi.hpp>

#include <Notifications/NotifierRest.hpp>
using namespace KTN::notify;

#include <KT01/Core/Log.hpp>
#include <KT01/Core/Macro.hpp>
#include <KT01/Core/PerformanceCounter.h>
#include <KT01/Core/PerformanceTracker.hpp>

#include <Orders/IdGen.h>
#include <Orders/OrderUtils.hpp>
#include <Orders/IOrderHandler.hpp>
#include <Orders/OrderPod.hpp>
#include <Orders/OrderPodPool.hpp>
#include <Orders/OrderNums.hpp>

#include <KT01/Core/Settings.hpp>

#include <KT01/Core/Log.hpp>
#include <Loggers/LogSvc.hpp>

#include <Reporters/StatusReporter.hpp>
using namespace KTN::REPORTER;

#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
using namespace KTN::NET;

#include <ExchangeHandler/session/SessionCallback.hpp>
#include <ExchangeHandler/IExchangeSender.hpp>
#include <ExchangeHandler/ExchCmdEnums.hpp>
#include <ExchangeHandler/ExchStructs.hpp>

#include <KT01/Core/BufferPool.hpp>
using namespace KTN::Core;

//...
#include <KT01/Net/Tcp/TcpCallback.hpp>
#include <KT01/Net/Tcp/TcpHandlerInterface.hpp>
#include <KT01/Net/Tcp/EFVITcpV5.hpp>
#include <KT01/Net/Tcp/StandardTcpV1.hpp> 
#include <KT01/Net/Tcp/TcpHandlerFactory.hpp>
using namespace KT01::NET::TCP;
using namespace KT01::NET;

#include <KT01/Core/ObjectPool.hpp>
#include <Maps/SPSCRingBuffer.hpp>

#include <KT01/Core/NamedThread.hpp>

#include <Notifications/NotifierRest.hpp>
using namespace KTN::notify;

#include <tbb/concurrent_queue.h>
using namespace tbb;


using namespace KTN;



#endif /* SRC_ExchangeHandler_EXCHANGEPCH_HPP_ */
