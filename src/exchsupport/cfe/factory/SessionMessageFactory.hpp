//============================================================================
// Name        	: MessageFactory.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 11, 2023 1:20:36 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CFE_BOE3_SESSIONMESSAGEFACTORY_HPP_
#define SRC_EXCHSUPPORT_CFE_BOE3_SESSIONMESSAGEFACTORY_HPP_

#pragma once

#include <exchsupport/cfe/Boe3/Boe3Includes.hpp>

#include <KT01/Net/byte_writer.hpp>
#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
#include <KT01/Net/helpers.hpp>
using namespace KTN::NET;

namespace KTN{
namespace CFE{
namespace Boe3{

	/**
 * @brief The SessionMessageFactory class is responsible for creating various session messages.
 */
class SessionMessageFactory {
public:
	SessionMessageFactory();
	~SessionMessageFactory();

	/**
* @brief Initializes the SessionMessageFactory with the specified settings file.
* @param settingsfile The path to the settings file.
*/
	void Init(string settingsfile);

	KTNBuf LoginMsgEncode(uint32_t unitseqnum, uint8_t nbrunits, bool debug);

	KTNBuf LogoutMsgEncode();

	KTNBuf HeartbeatEncode();

  /** 
  * @brief Gets the current sending time epoch.
  * @return The current sending time epoch.
  */
	static uint64_t getSendingTimeEpoch()
	{
		auto now = std::chrono::system_clock::now();
		return now.time_since_epoch().count();
	}


private:
	CfeBoe3Settings _sess;
	const int BUFFER_SIZE = 1600;


};


} } }
#endif /* SRC_EXCHSUPPORT_CME_IL3_SESSIONMESSAGEFACTORY_HPP_ */

