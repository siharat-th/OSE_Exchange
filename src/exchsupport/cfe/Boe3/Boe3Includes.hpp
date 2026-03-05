/*
 * Boe3Includes.hpp
 *
 *  Created on: May 24, 2021
 *      Author: centos
 */

#ifndef SRC_BOE3_BOE3INCLUDES_HPP_
#define SRC_BOE3_BOE3INCLUDES_HPP_

#pragma once

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <stdint.h>
using namespace std;

#include <KT01/Core/Settings.hpp>

#include <KT01/Core/Log.hpp>

#include <exchsupport/cfe/factory/AppMessageGen.hpp>
#include <exchsupport/cfe/factory/SessionMessageFactory.hpp>
#include <exchsupport/cfe/factory/AppMessageFactoryFastV2.hpp>
#include <exchsupport/cfe/settings/CfeBoe3Settings.hpp>
#include <exchsupport/cfe/settings/CfeSessionSettings.hpp>

#include <KT01/SecDefs/CfeSecMaster.hpp>
#include <akl/PriceConverters.hpp>

#include <KT01/Helpers/Base63.hpp>
//using namespace KT01::Base63;

#include <akl/Price.hpp>
// using namespace akl;

// using namespace KT01::SECDEF::CFE;
// using namespace KTN;
// using namespace KTN::NET;
// using namespace KT01::Core;

// using namespace KTN::CFE;
// using namespace KTN::CFE::Boe3;
// using namespace KTN::CFE::Boe3::Meta;




#endif /* SRC_BOE3_BOE3INCLUDES_HPP_ */
