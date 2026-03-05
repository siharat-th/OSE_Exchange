//============================================================================
// Name        	: AppMessageGen.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 20, 2023 4:11:32 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_APPMESSAGEGEN_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_APPMESSAGEGEN_HPP_
#pragma once

#include <cassert>
#include <memory>
#include <KT01/Core/Settings.hpp>

#include <akl/PriceConverters.hpp>
#include <exchsupport/cme/settings/CmeSessionSettings.hpp>

#include <exchsupport/cme/il3/IL3Types.hpp>
#include <exchsupport/cme/il3/IL3Composites.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>

#include <exchsupport/cme/il3/IL3Headers.hpp>
#include <exchsupport/cme/il3/IL3Messages2.hpp>
#include <exchsupport/cme/il3/IL3AppHelpers.hpp>


#include <KT01/Net/byte_writer.hpp>
#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
using namespace KTN::NET;

#include <KT01/Net/helpers.hpp>

#include <KT01/Core/PerformanceTracker.hpp>

#include <Orders/Order.hpp>
#include <Orders/IdGen.h>
using namespace KTN;



namespace KTN{
namespace CME{
namespace IL3{
	/**
	 * @brief This class is used to generate CME IL3 application messages and provide pre-filled templates for them.
	 *
	 */
	class AppMessageGen {
public:
	template <typename MessageType>
	static KTNBuf MsgTemplate(const string& settingsfile, bool isMkt, bool isManual)
	{
		CmeSessionSettings sess;
		sess.Load(settingsfile);

		char* buffer = new char[1600];
		uint32_t buffer_length = 1600;
		uint32_t bytes_written = 0;

		ByteWriter writer(buffer, buffer_length, &bytes_written);

		MessageType* msg = reinterpret_cast<MessageType*>(writer.cur());

		msg->Construct();
		msg->msgSize = msg->blockLength_ + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;
		msg->OrderQty = 0;
		msg->SecurityID = 0;
		msg->SeqNum = 0;
		msg->SendingTimeEpoch = 0;
		msg->ExecutionMode = EnumExecMode::EnumExecMode_Aggressive;
		msg->PartyDetailsListReqID = (uint64_t)sess.PartyListId;
		msg->OrderRequestID = 0;
		msg->ManualOrderIndicator = (isManual) ? EnumManualOrdIndReq::EnumManualOrdIndReq_Manual : EnumManualOrdIndReq::EnumManualOrdIndReq_Automated;

		strncpy(msg->SenderID,((isManual) ? sess.Tag50_Manual.c_str() : sess.Tag50_Auto.c_str()), sizeof(msg->SenderID));
		strncpy(msg->Location, sess.Location.c_str(), sizeof(msg->Location));

		if constexpr (!std::is_same_v<MessageType, OrderCancelRequestMsg>) {
			// Common initialization for all message types- says ShitGPT which is always wrong
			msg->OrdType = (isMkt) ? EnumOrderTypeReq::EnumOrderTypeReq_MarketWithProtection : EnumOrderTypeReq::EnumOrderTypeReq_Limit;
			msg->StopPx = IL3::PRICE_NULL;
			msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
			msg->DisplayQty = 0;
			msg->MinQty = 0;
		 }

		KTNBuf container = {};
		container.buffer = buffer;
		container.buffer_length = msg->msgSize;

		return container;
	}


	template <typename MessageType>
	static void UpdateMsgSeq(char*& buf, uint32_t newSeqNum) {
//		MessageType* msg = reinterpret_cast<MessageType*>(buf);
//		// Update SeqNum and SendingTimeEpoch directly
//		msg->SeqNum = newSeqNum;
//		msg->SendingTimeEpoch = IL3AppHelpers::GenOrdTimestamp();

		// Align buffer pointer to 64-byte boundary (if necessary)
		//buf = reinterpret_cast<char*>(__builtin_align_up(reinterpret_cast<uintptr_t>(buf), 64));

		// Use std::memcpy for potentially better optimization and safety
		__builtin_memcpy(buf + offsetof(MessageType, SeqNum), &newSeqNum, sizeof(uint32_t));

		// Consider inlining or caching timestamp generation
		uint64_t sendingTimeEpoch = IL3AppHelpers::GenOrdTimestamp();
		std::memcpy(buf + offsetof(MessageType, SendingTimeEpoch), &sendingTimeEpoch, sizeof(uint64_t));

	}

	template <typename MessageType>
	static void UpdateMsgPrice(char*& buf, uint32_t newSeqNum, uint64_t price) {
		MessageType* msg = reinterpret_cast<MessageType*>(buf);

		// Update SeqNum and SendingTimeEpoch directly
		msg->SeqNum = newSeqNum;
		msg->SendingTimeEpoch = IL3AppHelpers::GenOrdTimestamp();;
		//asssumes mantissa
		msg->Price = price;
	}

	static KTNBuf NewOrderTemplate(const string& settingsfile, KOrderType::Enum ordtype, EnumManualOrdIndReq manual_indicator)
	{
		CmeSessionSettings sess;
		sess.Load(settingsfile);

		char* buffer = new char[1600];
		uint32_t buffer_length=1600;
		uint32_t bytes_written = 0;

		ByteWriter writer(buffer, buffer_length, &bytes_written);

		NewOrderSingleMsg * msg = (NewOrderSingleMsg *)writer.cur();

		msg->Construct();
		msg->msgSize = msg->blockLength_ + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;
		msg->Price = IL3::PRICE_NULL;
		msg->OrderQty =0;
		msg->SecurityID = 0;
		//msg->Side = 0;
		msg->SeqNum = 0;
		msg->SendingTimeEpoch = 0;
		msg->ExecutionMode = EnumExecMode::EnumExecMode_Aggressive;
		msg->PartyDetailsListReqID = (uint64_t)sess.PartyListId;
		msg->OrderRequestID = 0;
		msg->ManualOrderIndicator = manual_indicator;
		msg->StopPx = IL3::PRICE_NULL;
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		msg->DisplayQty = 0;
		msg->MinQty = KTN::CME::IL3::UINT32_NULL;
		strncpy(msg->Location, sess.Location.c_str(), sizeof(msg->Location));

		strncpy(msg->SenderID,((manual_indicator == EnumManualOrdIndReq::EnumManualOrdIndReq_Manual)
				? sess.Tag50_Manual.c_str() : sess.Tag50_Auto.c_str()), sizeof(msg->SenderID));

		//order type
		switch (ordtype)
		{
			case KOrderType::LIMIT:
				msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_Limit;
				break;

			case KOrderType::MARKET:
				msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_MarketWithProtection;
				break;

			case KOrderType::MARKET_LIMIT:
				msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_MarketWithLeftoverAsLimit;
				break;

//			case KOrderType::STOP:
//				  msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_StopWithProtection;
//				  break;
//
//			case KOrderType::STOPLIMIT:
//			  msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_StopLimit;
//			  break;
		}


		int iMsgSize = msg->msgSize;

		KTNBuf container = {};
		container.buffer = buffer;
		container.buffer_length =  iMsgSize;

		return container;
	}

	//Note: Stop with Protection is a standard stop; StopLimit is NOT a stop with protection.
	static KTNBuf NewOrderStopOrderTemplate(const string& settingsfile, KOrderType::Enum ordtype, EnumManualOrdIndReq manual_indicator)
	{
		CmeSessionSettings sess;
		sess.Load(settingsfile);

		char* buffer = new char[1600];
		uint32_t buffer_length=1600;
		uint32_t bytes_written = 0;

		ByteWriter writer(buffer, buffer_length, &bytes_written);

		NewOrderSingleMsg * msg = (NewOrderSingleMsg *)writer.cur();

		msg->Construct();
		msg->msgSize = msg->blockLength_ + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;
		msg->Price = 0;
		msg->OrderQty =0;
		msg->SecurityID = 0;
		//msg->Side = 0;
		msg->SeqNum = 0;
		msg->SendingTimeEpoch = 0;
		msg->ExecutionMode = EnumExecMode::EnumExecMode_Aggressive;
		msg->PartyDetailsListReqID = (uint64_t)sess.PartyListId;
		msg->OrderRequestID = 0;
		msg->ManualOrderIndicator = manual_indicator;

		if (ordtype == KOrderType::STOP)
			msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_StopWithProtection;

		if (ordtype == KOrderType::STOPLIMIT)
			msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_StopLimit;


		msg->StopPx = IL3::PRICE_NULL;
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		msg->DisplayQty = 0;
		msg->MinQty = 0;
		strncpy(msg->Location, sess.Location.c_str(), sizeof(msg->Location));
		strncpy(msg->SenderID,((manual_indicator == EnumManualOrdIndReq::EnumManualOrdIndReq_Manual)
						? sess.Tag50_Manual.c_str() : sess.Tag50_Auto.c_str()), sizeof(msg->SenderID));

		int iMsgSize = msg->msgSize;

		KTNBuf container = {};
		container.buffer = buffer;
		container.buffer_length =  iMsgSize;

		return container;
	}

	static KTNBuf ModifyOrderTemplate(const string& settingsfile, KOrderType::Enum ordtype, EnumManualOrdIndReq manual_indicator)
	{
		CmeSessionSettings sess;
		sess.Load(settingsfile);

		char* buffer = new char[1600];
		uint32_t buffer_length=1600;
		uint32_t bytes_written = 0;

		ByteWriter writer(buffer, buffer_length, &bytes_written);

		OrderCancelReplaceRequestMsg * msg = (OrderCancelReplaceRequestMsg *)writer.cur();

		msg->Construct();
		msg->msgSize = msg->blockLength_ + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;

		msg->OrderQty =0;
		msg->SecurityID = 0;
		//msg->Side = 0;// (ord.OrdSide == KOrderSide::BUY) ? EnumSideReq::EnumSideReq_Buy : EnumSideReq_Sell;
		msg->SeqNum = 0;
		msg->SendingTimeEpoch = 0;
		msg->ExecutionMode = EnumExecMode::EnumExecMode_Aggressive;
		msg->PartyDetailsListReqID = (uint64_t)sess.PartyListId;
		msg->OrderRequestID = 0;
		msg->OrderID = 0;
		msg->ManualOrderIndicator = manual_indicator;
		msg->StopPx = IL3::PRICE_NULL;
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		msg->DisplayQty = 0;
		msg->MinQty = KTN::CME::IL3::UINT32_NULL;
		msg->OFMOverride = EnumOFMOverrideReq::EnumOFMOverrideReq_Enabled;
		strncpy(msg->Location, sess.Location.c_str(), sizeof(msg->Location));
		strncpy(msg->SenderID,((manual_indicator == EnumManualOrdIndReq::EnumManualOrdIndReq_Manual)
						? sess.Tag50_Manual.c_str() : sess.Tag50_Auto.c_str()), sizeof(msg->SenderID));


		//order type
		switch (ordtype)
		{
			case KOrderType::LIMIT:
				msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_Limit;
				break;

			case KOrderType::MARKET:
				msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_MarketWithProtection;
				break;

			case KOrderType::MARKET_LIMIT:
				msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_MarketWithLeftoverAsLimit;
				break;

//			case KOrderType::STOP:
//				  msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_StopWithProtection;
//				  break;
//
//			case KOrderType::STOPLIMIT:
//			  msg->OrdType = EnumOrderTypeReq::EnumOrderTypeReq_StopLimit;
//			  break;
		}


		int iMsgSize = msg->msgSize;

		KTNBuf container = {};
		container.buffer = buffer;
		container.buffer_length =  iMsgSize;

		return container;
	}


	static KTNBuf CancelOrderTemplate(const string& settingsfile, EnumManualOrdIndReq manual_indicator)
	{
		CmeSessionSettings sess;
		sess.Load(settingsfile);

		char* buffer = new char[1600];
		uint32_t buffer_length=1600;
		uint32_t bytes_written = 0;

		ByteWriter writer(buffer, buffer_length, &bytes_written);

		OrderCancelRequestMsg * msg = (OrderCancelRequestMsg *)writer.cur();

		msg->Construct();
		msg->msgSize = msg->blockLength_ + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;

		msg->SecurityID = 0;
	//	msg->Side = 0;
		msg->SeqNum = 0;
		msg->SendingTimeEpoch = 0;
		msg->PartyDetailsListReqID = (uint64_t)sess.PartyListId;
		msg->OrderRequestID = 0;
		msg->OrderID =0;
		msg->ManualOrderIndicator = manual_indicator;
		strncpy(msg->Location, sess.Location.c_str(), sizeof(msg->Location));
		strncpy(msg->SenderID,((manual_indicator == EnumManualOrdIndReq::EnumManualOrdIndReq_Manual)
						? sess.Tag50_Manual.c_str() : sess.Tag50_Auto.c_str()), sizeof(msg->SenderID));

		int iMsgSize = msg->msgSize;

		KTNBuf container = {};
		container.buffer = buffer;
		container.buffer_length =  iMsgSize;

		return container;
	}

	//*********** NON-LATENCY SENSITIVE MESSAGES:
	/*
	 * Here, we generate the whole message, inclusive of seq num and details
	 * This is not latency sensistive, so we are just generating bytes and don't really
	 * care if it take 1ns or 100ms
	 */
	static KTNBuf PartyDetailsDefEncode(const CmeSessionSettings& sess, uint32_t seqnum)
	{
		char* buffer = new char[1600];
		uint32_t buffer_length=1600;
		uint32_t bytes_written = 0;

		ByteWriter writer(buffer, buffer_length, &bytes_written);

		PartyDetailsDefinitionRequestMsg * msg = (PartyDetailsDefinitionRequestMsg *)writer.cur();

		msg->Construct();
		msg->msgSize = msg->blockLength_  + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;
		msg->SeqNum = seqnum;

		msg->PartyDetailsListReqID = (uint64_t)sess.PartyListId;

		msg->SendingTimeEpoch = IL3AppHelpers::GenOrdTimestamp();

		msg->ListUpdateAction = EnumListUpdAct::EnumListUpdAct_Add;//::'A';

		//placeholder for SenderID for audit trail files:
		strncpy(msg->Memo, sess.Tag50_Manual.c_str(), sizeof(msg->Memo));



		EnumCustOrderCapacity cti;
		switch(sess.CustOrdCapacity)
		{
		case 1:
				cti = EnumCustOrderCapacity::EnumCustOrderCapacity_MemberTradingForOwnAccount;
				break;
		case 2:
				cti = EnumCustOrderCapacity::EnumCustOrderCapacity_MemberFirmTradingForProprietaryAccount;
				break;
		case 3:
				cti = EnumCustOrderCapacity::EnumCustOrderCapacity_MemberTradingForAnotherMember;
				break;
		case 4:
				cti = EnumCustOrderCapacity::EnumCustOrderCapacity_AllOther;
				break;
		default:{
				cout << "BAD CTI/CUST ORD CAPACITY! WILL NOT SEND!!!" << endl;
				KTNBuf container = {};
				container.buffer = buffer;
				container.buffer_length =  0;
				return container;
			}
		}



		msg->CustOrderCapacity = cti;
		msg->CustOrderHandlingInst = EnumCustOrdHandlInst::EnumCustOrdHandlInst_ClientElectronic;


		msg->ClearingAccountType = EnumClearingAcctType::EnumClearingAcctType_Customer;
		msg->ClearingTradePriceType = EnumSLEDS::EnumSLEDS_TradeClearingAtExecutionPrice;

		msg->AvgPxIndicator = EnumAvgPxInd::EnumAvgPxInd_NoAveragePricing;

		msg->SelfMatchPreventionID = sess.SelfMatchId;
		msg->SelfMatchPreventionInstruction = EnumSMPI::EnumSMPI_CancelOldest;

		cout << "[AppMsgGen] PartyDetails: SenderID->Memo=" << msg->Memo
				<< " CTI=" << (int)msg->CustOrderCapacity << " CustOrdHandlInst=" << std::string(1, msg->CustOrderHandlingInst)
				<< " ClearAcctType=" << (int) msg->ClearingAccountType << endl;

		writer.Seek(msg->msgSize);

		int NUM_IN_GRP = 3;

		groupSize * grp = new groupSize();
		grp->blockLength = 22;//sizeof(GrpPartyDetail);
		grp->numInGroup = NUM_IN_GRP;
		writer.Write(grp, sizeof(groupSize));

		GrpPartyDetail* det0 = new GrpPartyDetail();
		strncpy(det0->PartyDetailID, sess.TraderId.c_str(), sizeof(det0->PartyDetailID));
		det0->PartyDetailRole = EnumPartyDetailRole::EnumPartyDetailRole_Operator;
		writer.Write(det0, sizeof(GrpPartyDetail));

		GrpPartyDetail* det1 = new GrpPartyDetail();
		strncpy(det1->PartyDetailID, sess.Account.c_str(), sizeof(det1->PartyDetailID));
		det1->PartyDetailRole = EnumPartyDetailRole::EnumPartyDetailRole_CustomerAccount;
		writer.Write(det1, sizeof(GrpPartyDetail));

		GrpPartyDetail* det2 = new GrpPartyDetail();
		strncpy(det2->PartyDetailID, sess.FirmId.c_str(), sizeof(det2->PartyDetailID));
		det2->PartyDetailRole = EnumPartyDetailRole::EnumPartyDetailRole_ExecutingFirm;
		writer.Write(det2, sizeof(GrpPartyDetail));


		groupSize * grp2 = new groupSize();
		grp2->blockLength = sizeof(GrpTrdRegPublication);
		grp2->numInGroup = 0;
		writer.Write(grp2, sizeof(groupSize));

		msg->msgSize += (2 * sizeof(groupSize)) + (NUM_IN_GRP * grp->blockLength);

//		cout << "PTYID " << msg->PartyDetailsListReqID <<" REQ BYTES WRITTEN=" << bytes_written << " MSGSIZE="
//				<< msg->msgSize << " NUM=" << NUM_IN_GRP << " BLOCK=" << grp->blockLength << endl;

		KTNBuf container = {};
		container.buffer = buffer;
		container.buffer_length =  msg->msgSize;

		//NetHelpers::hexdump(container.buffer, msg->msgSize);
		return container;
	}

	static KTNBuf PartyDetailsListEncode(const CmeSessionSettings& sess, uint32_t seqnum)
	{
		char* buffer = new char[1600];
		uint32_t buffer_length=1600;
		uint32_t bytes_written = 0;

		ByteWriter writer(buffer, buffer_length, &bytes_written);

		PartyDetailsListRequestMsg * msg = (PartyDetailsListRequestMsg *)writer.cur();

		msg->Construct();
		msg->msgSize = msg->blockLength_  + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;
		msg->SeqNum = seqnum;
		msg->PartyDetailsListReqID = (uint64_t)sess.PartyListId;
		msg->SendingTimeEpoch = IL3AppHelpers::GenOrdTimestamp();

		writer.Seek(msg->msgSize);

		int NUM_IN_GRP = 1;
		groupSize * grp = new groupSize();
		grp->blockLength = 22;//sizeof(GrpPartyDetail);
		grp->numInGroup = NUM_IN_GRP;
		writer.Write(grp, sizeof(groupSize));

		GrpRequestingPartyIDs* det0 = new GrpRequestingPartyIDs();
		strncpy(det0->RequestingPartyID, sess.FirmId.c_str(), sizeof(det0->RequestingPartyID));
		det0->RequestingPartyIDSource = 'C';
		det0->RequestingPartyRole = '1';
		
		writer.Write(det0, sizeof(GrpRequestingPartyIDs));

		groupSize * grp2 = new groupSize();
		grp2->blockLength = sizeof(GrpPartyIDs);
		grp2->numInGroup = 0;
		writer.Write(grp2, sizeof(groupSize));

		msg->msgSize += (2 * sizeof(groupSize)) + (NUM_IN_GRP * grp->blockLength);

		KTNBuf container = {};
		container.buffer = buffer;
		container.buffer_length =  msg->msgSize;

		//NetHelpers::hexdump(container.buffer, msg->msgSize);
		return container;
	}

	static KTNBuf MassActionEncode(const CmeSessionSettings& sess, const EnumMassActionScope scope,
			int id, uint64_t ordreqid, uint32_t seqnum)
	{
		char* buffer = new char[1600];
		uint32_t buffer_length=1600;
		uint32_t bytes_written = 0;

		ByteWriter writer(buffer, buffer_length, &bytes_written);

		OrderMassActionRequestMsg * msg = (OrderMassActionRequestMsg *)writer.cur();

		msg->Construct();
		msg->msgSize = msg->blockLength_ + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;

		msg->SeqNum = seqnum;
		msg->SendingTimeEpoch = IL3AppHelpers::GenOrdTimestamp();
		msg->PartyDetailsListReqID = sess.PartyListId;


		assert(ordreqid != 0 && "massact.reqid should not be equal to 0.");
		msg->OrderRequestID = ordreqid;


		msg->ManualOrderIndicator = EnumManualOrdIndReq::EnumManualOrdIndReq_Manual;
		strncpy(msg->SenderID, sess.Tag50_Manual.c_str(), sizeof(msg->SenderID));
		strncpy(msg->Location, sess.Location.c_str(), sizeof(msg->Location));

		msg->MassActionScope = scope;

		switch(scope)
		{
			case EnumMassActionScope::EnumMassActionScope_MarketSegmentID:
			{
				//cout << "PREPARING MASS CXL FOR SEG=" << id << endl;
				msg->MarketSegmentID = (uint8_t)id;
				break;
			}
			case EnumMassActionScope::EnumMassActionScope_Instrument:
			{
				//cout << "PREPARING MASS CXL FOR INSTRUMENT=" << id << endl;
				msg->SecurityID = id;
				break;
			}
			default:
				//cout << "MASS ACTION SCOPE " << (int) scope << " NOT HANDLED" << endl;
				break;
		}

		KTNBuf container = {};
		container.buffer = buffer;
		container.buffer_length =  msg->msgSize;

		//NetHelpers::hexdump(container.buffer, msg->msgSize);
		return container;
	}

	//returns a status exec report.
	static KTNBuf MassOrdStatusEncode(const CmeSessionSettings& sess, const EnumMassStatusReqTyp reqtype,
				int id, uint64_t orderreqid, uint32_t seqnum)
	{
		char* buffer = new char[1600];
		uint32_t buffer_length=1600;
		uint32_t bytes_written = 0;

		ByteWriter writer(buffer, buffer_length, &bytes_written);

		OrderMassStatusRequestMsg * msg = (OrderMassStatusRequestMsg *)writer.cur();

		msg->Construct();
		msg->msgSize = msg->blockLength_ + IL3::SOH_HDR_SIZE + IL3::SBE_HDR_SIZE;

		msg->SeqNum = seqnum;
		msg->SendingTimeEpoch = IL3AppHelpers::GenOrdTimestamp();

		strncpy(msg->SenderID, sess.Tag50_Manual.c_str(), sizeof(msg->SenderID));
		strncpy(msg->Location, sess.Location.c_str(), sizeof(msg->Location));

		//Message Fields
		msg->PartyDetailsListReqID = sess.PartyListId;
		msg->MassStatusReqID = orderreqid;
		msg->ManualOrderIndicator = EnumManualOrdIndReq::EnumManualOrdIndReq_Manual;
		msg->MassStatusReqType = reqtype;

		switch(reqtype)
		{
			case EnumMassStatusReqTyp::EnumMassStatusReqTyp_MarketSegment:
			{
				cout << "PREPARING MASS STATUS FOR SEG=" << id << endl;
				msg->MarketSegmentID = (uint8_t)id;
				//msg->OrdStatusReqType = EnumMassStatusOrdTyp::EnumMassStatusOrdTyp_SenderSubID;
				break;
			}
			case EnumMassStatusReqTyp::EnumMassStatusReqTyp_Instrument:
			{
				cout << "PREPARING MASS STATUS FOR INSTRUMENT=" << id << endl;
				msg->SecurityID = id;
				break;
			}
			default:
				//cout << "MASS ACTION SCOPE " << (int) scope << " NOT HANDLED" << endl;
				break;
		}

		KTNBuf container = {};
		container.buffer = buffer;
		container.buffer_length =  msg->msgSize;

//		cout << "ORD MASS REQ SIZE=" << msg->msgSize << endl;
//		NetHelpers::hexdump(container.buffer, msg->msgSize);
		return container;
	}
	

};


} } }
#endif /* SRC_EXCHSUPPORT_CME_IL3_APPMESSAGEFACTORY_HPP_ */
