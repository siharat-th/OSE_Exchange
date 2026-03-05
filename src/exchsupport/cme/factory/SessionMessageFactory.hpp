//============================================================================
// Name        	: MessageFactory.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 11, 2023 1:20:36 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_SESSIONMESSAGEFACTORY_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_SESSIONMESSAGEFACTORY_HPP_

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <stdint.h>
using namespace std;

#include <KT01/Core/Settings.hpp>

#include <exchsupport/cme/settings/CmeSessionSettings.hpp>

#include <exchsupport/cme/il3/IL3Types.hpp>
#include <exchsupport/cme/il3/IL3Composites.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>

#include <exchsupport/cme/il3/IL3Headers.hpp>
#include <exchsupport/cme/il3/IL3Messages2.hpp>
#include <exchsupport/cme/il3/IL3LoginHelpers.hpp>

#include <KT01/Net/byte_writer.hpp>
#include <KT01/Net/netstructs.hpp>
using namespace KTN::NET;

#include <KT01/Net/helpers.hpp>



namespace KTN{
namespace CME{
namespace IL3{

	/**
 * @brief The SessionMessageFactory class is responsible for creating various session messages.
 */
class SessionMessageFactory {
public:
	/**
* @brief Initializes the SessionMessageFactory with the specified settings file.
* @param settingsfile The path to the settings file.
*/
	void Init(string settingsfile);

	/**
  * @brief Encodes the SOFH (Start of Frame Header) for the given message length.
  * @param msglen The length of the message.
  * @return The encoded SOFH message.
  */
	KTN_MSG SofhEncode(uint16_t msglen);

	/**
  * @brief Encodes the Negotiate message with the specified UUID.
  * @return The encoded Negotiate message.
  */
	KTN_MSG NegotiateEncode(bool debug);

	/**
  * @brief Encodes the Establish message with the specified UUID and sequence ID.
  * @param uuid The UUID for the message.
  * @param seqid The sequence ID for the message.
  * @return The encoded Establish message.
  */
	KTNBuf EstablishEncode(uint64_t uuid, uint64_t seqid, bool debug);

	/**
  * @brief Encodes the Sequence message with the specified UUID, sequence ID, primary flag, and lapse flag.
  * @param uuid The UUID for the message.
  * @param seqid The sequence ID for the message.
  * @param primary The primary flag for the message.
  * @param lapse The lapse flag for the message.
  * @return The encoded Sequence message.
  */
	KTN_MSG SequenceEncode(uint64_t uuid, uint64_t seqid, EnumFTI fti, EnumKeepAliveLapsed lapsed);

	/**
  * @brief Encodes the Terminate message with the specified UUID, reason, and error codes.
  * @param uuid The UUID for the message.
  * @param reason The reason for termination.
  * @param errorcodes The error codes for the message.
  * @return The encoded Terminate message.
  */
	KTNBuf TerminateEncode(uint64_t uuid, string reason, int errorcodes);

	/**
  * @brief Encodes the RetransRequest message with the specified UUID, FromSeqNo, MsgCount, and last UUID.
  * @param uuid The UUID for the message.
  * @param FromSeqNo The starting sequence number for retransmission.
  * @param MsgCount The number of messages to retransmit.
  * @param lastuuid The last UUID for retransmission.
  * @return The encoded RetransRequest message.
  */
	KTNBuf RetransRequestEncode(uint64_t uuid, uint32_t FromSeqNo, uint16_t MsgCount, uInt64NULL lastuuid);

	/**
  * @brief Encodes the PartyDetailsDef message with the specified UUID, sequence ID, partyDetailsListReqID, selfmatchId, account, firmid, and traderid.
  * @param uuid The UUID for the message.
  * @param seqid The sequence ID for the message.
  * @param partyDetailsListReqID The partyDetailsListReqID for the message.
  * @param selfmatchId The selfmatchId for the message.
  * @param account The account for the message.
  * @param firmid The firmid for the message.
  * @param traderid The traderid for the message.
  * @return The encoded PartyDetailsDef message.
  */
	KTN_MSG PartyDetailsDefEncode(uint64_t uuid, uint64_t seqid, uint64_t partyDetailsListReqID, uint64_t selfmatchId, string account, string firmid, string traderid);

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
//	template<typename MsgType>
//	KTN_MSG Encode(const MsgType& message)
//	{
//		constexpr uint32_t BUFFER_SIZE = 1600;
//	    char buffer[BUFFER_SIZE];
//	    uint32_t bytes_written = 0;
//
//	    ByteWriter writer(buffer, BUFFER_SIZE, &bytes_written);
//
//	    // Write SOFH
//	    SOFH* sofh = (SOFH*)writer.cur();
//	    sofh->enodingType_ = IL3::ENCODING_TYPE;
//
//		SBEHeader* hdr = (SBEHeader*)writer.cur();
//		hdr->schemaId_ = IL3::SCHEMA_ID;
//		hdr->version_ = IL3::VERSION;
//		hdr->templateId_ = MsgType::id;
//		hdr->blockLength_ = MsgType::blockLength;
//
//
//		cout << "ENCODER TEMPLATE=" << hdr->templateId_ <<" LEN=" << hdr->blockLength_ << endl;
//
//		typename MsgType::BlockData* blockData = (typename MsgType::BlockData*)writer.cur();
//		memcpy(blockData, &(message.blockData_), MsgType::blockLength);
//
//
//		//SOFH header is overall msg len + sofh size.  Now if we have repeating fields or
//		//multiple messages in the frame, we need to adjust overall length to take both into account
//		sofh->msgSize_ = hdr->blockLength_ + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;
//
//		cout << "ENCODE MSG LEN=" << sofh->msgSize_ << endl;
//
//		KTN_MSG container;
//		memcpy(container.buffer, buffer, bytes_written);
//		container.buffer_length = bytes_written;
//
//		return container;
//	}


private:
	CmeSessionSettings _sess;
	const int BUFFER_SIZE = 1600;

	//uint64_t _UUID;
	//uint64_t _TSTAMP;
//	std::string _CANON_NEG;
//	std::string _CANON_EST;
//	std::string _HMAC_NEG;
//	std::string _HMAC_EST;



};


} } }
#endif /* SRC_EXCHSUPPORT_CME_IL3_SESSIONMESSAGEFACTORY_HPP_ */

