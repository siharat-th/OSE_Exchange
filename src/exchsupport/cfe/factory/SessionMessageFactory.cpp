//============================================================================
// Name        	: SessionMessageFactory.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 11, 2023 1:20:36 PM
//============================================================================

#include <exchsupport/cfe/factory/SessionMessageFactory.hpp>
#include "SessionMessageFactory.hpp"
using namespace KTN::CFE::Boe3;

SessionMessageFactory::SessionMessageFactory() {
}

SessionMessageFactory::~SessionMessageFactory() {
}

void SessionMessageFactory::Init(string settingsfile)
{
	_sess.Load(settingsfile);
}

KTNBuf SessionMessageFactory::LoginMsgEncode(uint32_t unitseqnum, uint8_t nbrunits, bool debug)
{
	char buffer[1600];
	uint32_t buffer_length=1600;
	uint32_t bytes_written = 0;

	ByteWriter writer(buffer, buffer_length, &bytes_written);

	LoginRequestMsg * msg = (LoginRequestMsg *)writer.cur();

	msg->StartOfMessage = STARTMSG::B0E3;
	msg->MessageType = SessMsgType::LoginRequest;
	msg->MatchingUnit = 0;
	msg->Reserved = 0;
	msg->SeqNum = 0;
	
	strncpy(msg->SessionId, _sess.SessionSettings.SessionId.c_str(), sizeof(msg->SessionId));
	strncpy(msg->SessionSubId, _sess.SessionSettings.SubId.c_str(), sizeof(msg->SessionSubId));

	strncpy(msg->Password, _sess.SessionSettings.Password.c_str(),sizeof(msg->Password));

	char replaytype = _sess.SessionSettings.ReplayType[0];
	switch (replaytype)
	{
		case 'F':
			msg->ReplayUnspecifiedUnit = EnumReplayUnspecifiedUnit::Fail;
			break;
		case 'S':
			msg->ReplayUnspecifiedUnit = EnumReplayUnspecifiedUnit::Skip;
			break;
		case 'R':
			msg->ReplayUnspecifiedUnit = EnumReplayUnspecifiedUnit::Replay_From_Zero;
			break;
		default:
			msg->ReplayUnspecifiedUnit = EnumReplayUnspecifiedUnit::Skip;
			break;
	}
	
	msg->NumberOfUnits = nbrunits;
	msg->UnitNumber = _sess.SessionSettings.UnitNbr;
	msg->UnitSequence = unitseqnum;

	uint32_t iMsgLen = sizeof(LoginRequestMsg);
	msg->MessageLength = iMsgLen-2;

	if (debug)
	{
		KT01_LOG_DEBUG("[SessionFact]","HEXDUMP LOGIN:");
		NetHelpers::hexdump(buffer, iMsgLen);
	}

	KTNBuf container;
	memcpy(container.buffer,buffer,iMsgLen);
	container.buffer_length = iMsgLen;

	return container;
}

KTNBuf SessionMessageFactory:: LogoutMsgEncode()
{
	char buffer[1600];
	uint32_t buffer_length=1600;
	uint32_t bytes_written = 0;

	ByteWriter writer(buffer, buffer_length, &bytes_written);

	LogoutRequestMsg * msg = (LogoutRequestMsg *)writer.cur();

	//fill in the fields of my main msg:
	msg->StartOfMessage = STARTMSG::B0E3;
	msg->MessageType = SessMsgType::LogoutRequest;
	msg->MatchingUnit = 0;
	msg->Reserved = 0;
	msg->SeqNum = 0;

	uint32_t iMsgLen = sizeof(LogoutRequestMsg);
	msg->MessageLength = iMsgLen-2;

	KTNBuf container;
	memcpy(container.buffer,buffer,iMsgLen);
	container.buffer_length = iMsgLen;

	return container;
};

KTNBuf SessionMessageFactory:: HeartbeatEncode()
{
	char buffer[1600];
	uint32_t buffer_length=1600;
	uint32_t bytes_written = 0;

	ByteWriter writer(buffer, buffer_length, &bytes_written);

	ClientHeartbeatMsg * msg = (ClientHeartbeatMsg *)writer.cur();

	//fill in the fields of my main msg:
	msg->StartOfMessage = STARTMSG::B0E3;
	msg->MessageType = SessMsgType::ClientHeartbeat;
	msg->MatchingUnit = 0;
	msg->Reserved = 0;
	msg->SeqNum = 0;

	uint32_t iMsgLen = sizeof(LogoutRequestMsg);
	msg->MessageLength = iMsgLen-2;

	KTNBuf container;
	memcpy(container.buffer,buffer,iMsgLen);
	container.buffer_length = iMsgLen;

	return container;
};


