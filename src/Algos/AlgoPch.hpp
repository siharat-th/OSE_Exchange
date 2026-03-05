//============================================================================
// Name        	: AlgoPch.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 23, 2023 5:35:48 PM
//============================================================================

#ifndef SRC_ALGOS_ALGOPCH_HPP_
#define SRC_ALGOS_ALGOPCH_HPP_
#pragma once


//#include <random>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <memory>
#include <math.h>
#include <functional>
#include <string>
#include <utility>
#include <cmath>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
using namespace rapidjson;

#include <Orders/Order.hpp>
#include <Orders/OrderPod.hpp>
#include <Orders/OrderUtils.hpp>

#include <KT01/Core/Settings.hpp>

#include <Loggers/LogEnums.hpp>
#include <Loggers/LogSvc.hpp>

#include <Notifications/Slacker.hpp>
using namespace KTN::notify;

#include <Rest/JsonUrlReader.hpp>
#include <Rest/RestApi.hpp>

#include <KT01/DataStructures/UserStructs.hpp>


#include <KT01/SecDefs/CmeSecMasterV2.hpp>
#include <exchsupport/cme/msgw/CmeMsgw.hpp>
using namespace KT01::SECDEF::CME;
using namespace KT01::MSGW;

//v2:
#include <Orders/OrderEnumsV2.hpp>

#include <AlgoParams/LegParamParser.hpp>
#include <AlgoParams/OrderParser.hpp>
#include <AlgoParams/AlgoInitializer.hpp>

#include <AlgoSupport/algo_helpers.hpp>
#include <AlgoEngines/engine_structs.hpp>

#include <KT01/Core/PerformanceTracker.hpp>

#include <AlgoSupport/algo_structs.hpp>
#include <AlgoSupport/algo_pl.hpp>
#include <AlgoSupport/AlgoJson.hpp>
#include <AlgoSupport/AlgoMktDataCallback.hpp>

#include <exchsupport/cme/il3/IL3Messages2.hpp>
using namespace KTN::CME::IL3;

#include <Notifications/NotifierRest.hpp>
using namespace KTN::notify;


using namespace std;
using namespace KTN;
using namespace KTN::ORD;
using namespace KTN::ALGO;

#endif /* SRC_ALGOS_ALGOPCH_HPP_ */
