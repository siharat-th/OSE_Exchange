/*
 * Boe3Msgs.hpp
 *
 *  Created on: May 22, 2021
 *      Author: samg
 */

#ifndef SRC_BOE3_V3SESSIONMESSAGES_HPP_
#define SRC_BOE3_V3SESSIONMESSAGES_HPP_

#include <stdio.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <map>
#include <inttypes.h>

namespace KTN
{
	namespace CFE
	{
		namespace Boe3
		{

			struct MessageHeaderMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
			} __attribute__((packed));

			struct LoginRequestMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				char SessionId[4];
				char SessionSubId[4];
				char Password[10];
				EnumReplayUnspecifiedUnit ReplayUnspecifiedUnit;
				uint8_t NumberOfUnits;
				uint8_t UnitNumber;
				uint32_t UnitSequence;
			} __attribute__((packed));

			struct LoginRequestShortMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				char SessionId[4];
				char SessionSubId[4];
				char Password[10];
				char ReplayUnspecifiedUnit;
				uint8_t NumberOfUnits;
			} __attribute__((packed));

			struct LogoutRequestMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
			} __attribute__((packed));

			struct ClientHeartbeatMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
			} __attribute__((packed));

			struct LoginResponseMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				EnumLoginResponseStatus LoginResponseStatus;
				char LoginResponseText[60];
				uint32_t ClientSeqNum;
				uint8_t NbrUnits;
				uint8_t UnitNumber;
				uint32_t UnitSequence;
				// we can calculate where to move the cursor:
			} __attribute__((packed));

			struct ReplayCompleteMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
			} __attribute__((packed));

			struct LogoutResponseMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				EnumLogoutReason LogoutReason;
				char LogoutReasonText[60];
			} __attribute__((packed));

			struct ServerHeartbeatMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
			} __attribute__((packed));

		} /* namespace Boe3 */
	} /* namespace CFE */
} /* namespace KTN */

#endif /* SRC_BOE3_V3SESSIONMESSAGES_HPP_ */
