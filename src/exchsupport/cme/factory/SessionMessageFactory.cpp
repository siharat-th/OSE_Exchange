//============================================================================
// Name        	: SessionMessageFactory.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 11, 2023 1:20:36 PM
//============================================================================

#include <exchsupport/cme/factory/SessionMessageFactory.hpp>
using namespace KTN::CME::IL3;

//SessionMessageFactory::SessionMessageFactory() {
//	//LoadSettings...
//
//}
//
//SessionMessageFactory::~SessionMessageFactory() {
//	// TODO Auto-generated destructor stub
//}

void SessionMessageFactory::Init(string settingsfile)
{
	_sess.Load(settingsfile);
}

KTN_MSG SessionMessageFactory::SofhEncode(uint16_t msglen)
{
	char* buffer = new char[1600];
	uint32_t buffer_length=1600;
	uint32_t bytes_written = 0;

	ByteWriter writer(buffer, buffer_length, &bytes_written);

	SOFH* msg = (SOFH *)writer.cur();
	msg->msgSize_ = msglen + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE ;//+ 2 ;// 2 for credentials? I dunno
	msg->enodingType_ = IL3::ENCODING_TYPE;

	int iMsgSize = sizeof(SOFH);

	//cout << "SOFH MSG_SIZE=" << msg->msgSize_ << endl;

	KTN_MSG container = {};
	//memcpy(container.buffer,buffer, iMsgSize);
	container.buffer = buffer;
	container.buffer_length =  iMsgSize;

	return container;
}


KTN_MSG SessionMessageFactory::NegotiateEncode(bool debug)
{
	//char buffer[1600];
	char* buffer = new char[1600];
	uint32_t buffer_length=1600;
	uint32_t bytes_written = 0;

	ByteWriter writer(buffer, buffer_length, &bytes_written);

	NegotiateMsg * msg = (NegotiateMsg *)writer.cur();

	int BLOCK_LEN = 76;
	int RPT_LEN = 2;

	uint64_t uuid = IL3LoginHelpers::GenerateUUID();
	uint64_t tstamp = IL3LoginHelpers::GenerateRequestTimestamp();

	msg->Construct();

	msg->msgSize = BLOCK_LEN + RPT_LEN + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;
	msg->UUID = uuid;
	msg->RequestTimestamp = tstamp;
	msg->Credentials = 0;

	strncpy(msg->Session, _sess.SessionId.c_str(), sizeof(msg->Session));
	strncpy(msg->Firm, _sess.FirmId.c_str(), sizeof(msg->Firm));
	strncpy(msg->AccessKeyID, _sess.AccessKey.c_str(), sizeof(msg->AccessKeyID));

	string canonicalRequest = IL3LoginHelpers::GenerateCanonicalMessageForNegotiate(_sess.SessionId,
			_sess.FirmId, tstamp, uuid);

	//old way that gives us problems.
//	std::string hm =  IL3LoginHelpers::calculateHMAC3Mtx(canonicalRequest.c_str(),
//				_sess.SecretKey.c_str());
//	char hmc[32];
//	memset(hmc,0,sizeof(hmc));
//	memcpy(hmc, hm.c_str(),sizeof(hmc));
//	memcpy(msg->HMACSignature, hmc, 32);

	//new, more compact way using cryptopp
	std::string hm =  IL3LoginHelpers::calculateHMAC(canonicalRequest.c_str(), _sess.SecretKey.c_str(), debug);

	//strncpy(msg->HMACSignature, hm.c_str(), sizeof(msg->HMACSignature));
	char hmc[32];
	memset(hmc,0,sizeof(hmc));
	memcpy(hmc, hm.c_str(),sizeof(hmc));
	memcpy(msg->HMACSignature, hmc, 32);

	int iMsgSize = sizeof(NegotiateMsg);

	KTN_MSG container = {};
	container.buffer = buffer;
	//memcpy(container.buffer, buffer, iMsgSize);
	container.buffer_length =  iMsgSize;
	container.uuid = uuid;

	if (debug)
	{
		cout << "HEXDUMP HMAC:" << endl;
		NetHelpers::hexdump(msg->HMACSignature, sizeof(msg->HMACSignature));
	}


//	NetHelpers::hexdump(container.buffer, iMsgSize);


	return container;
}

KTNBuf SessionMessageFactory::EstablishEncode(uint64_t uuid, uint64_t nextseq=1, bool debug=false)
{
	if (uuid == 0)
	{
		cout << "UUID = 0!!!! ERROR ON ESTABLISH-- YOU NEED TO NEGOTIATE FIRST!!!" << endl;
		std::exit(1);
//		uuid = IL3LoginHelpers::GenerateUUID();
	}

	//char buffer[1600];
	char* buffer = new char[1600];
	uint32_t buffer_length=1600;
	uint32_t bytes_written = 0;

	ByteWriter writer(buffer, buffer_length, &bytes_written);

	EstablishMsg * msg = (EstablishMsg *)writer.cur();

	int RPT_LEN = 2;

	msg->Construct();
	msg->msgSize = msg->blockLength_ + RPT_LEN + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;

	msg->KeepAliveInterval = _sess.KeepAliveSecs * 1000; //milliseconds.
	msg->NextSeqNo = nextseq;
	msg->UUID = uuid;
	msg->Credentials = 0;

	uint64_t tstamp = IL3LoginHelpers::GenerateRequestTimestamp();
	msg->RequestTimestamp = tstamp;

	strncpy(msg->AccessKeyID, _sess.AccessKey.c_str(), sizeof(msg->AccessKeyID));
	strncpy(msg->Firm, _sess.FirmId.c_str(), sizeof(msg->Firm));
	strncpy(msg->Session, _sess.SessionId.c_str(), sizeof(msg->Session));

	strncpy(msg->TradingSystemName, _sess.SystemName.c_str(), sizeof(msg->TradingSystemName));
	strncpy(msg->TradingSystemVendor, _sess.SystemVendor.c_str(), sizeof(msg->TradingSystemVendor));
	strncpy(msg->TradingSystemVersion, _sess.SystemVersionId.c_str(), sizeof(msg->TradingSystemVersion));


	string canonicalRequest = IL3LoginHelpers::GenerateCanonicalMessageForEstablish(_sess.SessionId, _sess.FirmId, tstamp, uuid,
			_sess.SystemName, _sess.SystemVersionId, _sess.SystemVendor, nextseq, _sess.KeepAliveSecs);

// Old way
//	std::string hm =  IL3LoginHelpers::calculateHMAC3Mtx(canonicalRequest.c_str(),
//			_sess.SecretKey.c_str());
//	char hmc[32];
//	memset(hmc,0,sizeof(hmc));
//	memcpy(hmc, hm.c_str(),sizeof(hmc));
//	memcpy(msg->HMACSignature, hmc, 32);


	//new way with cryptopp...
	std::string hm =  IL3LoginHelpers::calculateHMAC(canonicalRequest.c_str(), _sess.SecretKey.c_str(), debug);
	char hmc[32];
	memset(hmc,0,sizeof(hmc));
	memcpy(hmc, hm.c_str(),sizeof(hmc));
	memcpy(msg->HMACSignature, hmc, 32);

	//strncpy(msg->HMACSignature, hm.c_str(), sizeof(msg->HMACSignature));

	int iMsgSize = sizeof(EstablishMsg);
	KTNBuf container = {};
	container.buffer = buffer;
	container.buffer_length =  iMsgSize;

	if (debug)
	{
		cout << "HEXDUMP HMAC:" << endl;
		NetHelpers::hexdump(msg->HMACSignature, sizeof(msg->HMACSignature));
	}

	//NetHelpers::hexdump(container.buffer, iMsgSize);

	return container;
}

KTN_MSG SessionMessageFactory::SequenceEncode(uint64_t uuid, uint64_t nextseq, EnumFTI fti, EnumKeepAliveLapsed lapsed)
{

	char* buffer = new char[1600];
	uint32_t buffer_length=1600;
	uint32_t bytes_written = 0;

	ByteWriter writer(buffer, buffer_length, &bytes_written);


	SequenceMsg * msg = (SequenceMsg *)writer.cur();

	//int BLOCK_LEN = 14;

	//SOFH header is overall msg len + sofh size.  Now if we have repeating fields or
	//multiple messages in the frame, we need to adjust overall length to take both into account
	msg->Construct();
	msg->msgSize = msg->blockLength_  + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;

	msg->FaultToleranceIndicator = fti;
	msg->NextSeqNo = nextseq;
	msg->UUID = uuid;
	msg->KeepAliveIntervalLapsed =  lapsed;

	if (msg->KeepAliveIntervalLapsed == EnumKeepAliveLapsed::EnumKeepAliveLapsed_Lapsed)
		cout << "***FACTORY: SEND SEQ ENCODE: LAPSED=" << (int)msg->KeepAliveIntervalLapsed << endl;

	int iMsgSize = sizeof(SequenceMsg);
	KTN_MSG container = {};
	//memcpy(container.buffer,buffer, iMsgSize);
	container.buffer = buffer;
	container.buffer_length =  iMsgSize;
	container.uuid = uuid;

	//cout << "SEND SEQ=" << msg->NextSeqNo << endl;
	//NetHelpers::hexdump(container.buffer, iMsgSize);

	return container;
}

KTNBuf SessionMessageFactory::TerminateEncode(uint64_t uuid, string reason, int errorcodes)//, EnumSplitMsg split=NULL)
{

	char* buffer = new char[1600];
	uint32_t buffer_length=1600;
	uint32_t bytes_written = 0;

	ByteWriter writer(buffer, buffer_length, &bytes_written);


	TerminateMsg * msg = (TerminateMsg *)writer.cur();


	//SOFH header is overall msg len + sofh size.  Now if we have repeating fields or
	//multiple messages in the frame, we need to adjust overall length to take both into account
	msg->Construct();
	msg->msgSize = msg->blockLength_  + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;


	//msg->SplitMsg = split;
	msg->ErrorCodes = errorcodes;
	msg->UUID = uuid;
	strncpy(msg->Reason, reason.c_str(), sizeof(msg->Reason));

	msg->RequestTimestamp = IL3LoginHelpers::GenerateRequestTimestamp();

	int iMsgSize = sizeof(TerminateMsg);
	KTNBuf container = {};
	//memcpy(container.buffer,buffer, iMsgSize);
	container.buffer = buffer;
	container.buffer_length =  iMsgSize;
	//container.uuid = uuid;

	//NetHelpers::hexdump(container.buffer, iMsgSize);

	return container;
}

KTNBuf SessionMessageFactory::RetransRequestEncode(uint64_t uuid, uint32_t FromSeqNo, uint16_t MsgCount, uInt64NULL lastuuid=IL3::UINT64_NULL)
{
	char* buffer = new char[1600];
	uint32_t buffer_length=1600;
	uint32_t bytes_written = 0;

	ByteWriter writer(buffer, buffer_length, &bytes_written);

	RetransmitRequestMsg * msg = (RetransmitRequestMsg *)writer.cur();

	//SOFH header is overall msg len + sofh size.  Now if we have repeating fields or
	//multiple messages in the frame, we need to adjust overall length to take both into account
	msg->Construct();
	msg->msgSize = msg->blockLength_  + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;

	msg->FromSeqNo = FromSeqNo;
	msg->MsgCount = MsgCount;
	msg->UUID = uuid;
	msg->LastUUID = lastuuid;

	uint64_t tstamp = IL3LoginHelpers::GenerateRequestTimestamp();
	msg->RequestTimestamp = tstamp;



	int iMsgSize = sizeof(RetransmitRequestMsg);
	KTNBuf container = {};

	container.buffer = buffer;
	container.buffer_length =  iMsgSize;

	//NetHelpers::hexdump(container.buffer, iMsgSize);

	return container;
}



