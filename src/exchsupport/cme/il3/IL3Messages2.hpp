//============================================================================
// Name        	: IL3Messages2.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 11, 2023 7:01:39 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3MESSAGES2_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3MESSAGES2_HPP_
#pragma once

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <bitset>
#include <stdint.h>

#include <exchsupport/cme/il3/IL3Types.hpp>
#include <exchsupport/cme/il3/IL3Composites.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>

namespace KTN{
namespace CME{
namespace IL3{

// Define a common base class
struct MessageBase {
    virtual ~MessageBase() = default;
    // A virtual function to return the message id.
    virtual std::size_t getId() const = 0;
    // (Optionally, add other common interfaces)
};

// Negotiate500   BlockLength=76 id=500
struct NegotiateMsg
{
//	inline static const std::size_t id{500};
//	inline static const std::size_t blockLength{76};
		std::uint16_t msgSize;
		std::uint16_t enodingType_ = IL3::ENCODING_TYPE;
		std::uint16_t blockLength_ = 76;
		std::uint16_t templateId_ = 500;
		std::uint16_t schemaId_ = IL3::SCHEMA_ID;
		std::uint16_t version_ =IL3::VERSION;

		String32Req HMACSignature;
		String20Req AccessKeyID;
		uInt64 UUID;
		uInt64 RequestTimestamp;
		String3Req Session;
		String5Req Firm;
		uint16_t Credentials = 0;

		void Construct()
		{
			enodingType_ = IL3::ENCODING_TYPE;
			blockLength_ = 76;
			templateId_ = 500;
			schemaId_ = IL3::SCHEMA_ID;
			version_ =IL3::VERSION;
			Credentials = 0;
		}

}__attribute__((packed));

// NegotiationResponse501   BlockLength=32 id=501
struct NegotiationResponseMsg
{
	inline static const std::size_t id{501};
	inline static const std::size_t blockLength{32};

	struct BlockData
	{
		//ExchFlowTyp ServerFlow;
		uInt64 UUID;
		uInt64 RequestTimestamp;
		uInt16NULL SecretKeySecureIDExpiration;
		EnumFTI FaultToleranceIndicator;
		uint8_t SplitMsg;
		uInt32 PreviousSeqNo;
		uInt64 PreviousUUID;
		//uint16_t Credentials;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NegotiationReject502   BlockLength=68 id=502
struct NegotiationRejectMsg
{
	inline static const std::size_t id{502};
	inline static const std::size_t blockLength{68};

	struct BlockData
	{
		String48 Reason;
		uInt64 UUID;
		uInt64 RequestTimestamp;
		uInt16 ErrorCodes;
		EnumFTI FaultToleranceIndicator;
		uint8_t SplitMsg;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// Establish503   BlockLength=132 id=503
struct EstablishMsg
{

		std::uint16_t msgSize;
		std::uint16_t enodingType_;
		std::uint16_t blockLength_;
		std::uint16_t templateId_ ;
		std::uint16_t schemaId_;
		std::uint16_t version_;

		String32Req HMACSignature;
		String20Req AccessKeyID;
		String30Req TradingSystemName;
		String10Req TradingSystemVersion;
		String10Req TradingSystemVendor;
		uInt64 UUID;
		uInt64 RequestTimestamp;
		uInt32 NextSeqNo;
		String3Req Session;
		String5Req Firm;
		uInt16 KeepAliveInterval;
		uint16_t Credentials = 0;

		void Construct()
		{
			enodingType_ = IL3::ENCODING_TYPE;
			blockLength_ = 132;
			templateId_ = 503;
			schemaId_ = IL3::SCHEMA_ID;
			version_ =IL3::VERSION;
			Credentials = 0;
		}
}__attribute__((packed));

// EstablishmentAck504   BlockLength=38 id=504
struct EstablishmentAckMsg //: public MessageBase
{
	inline static const std::size_t id{504};
	inline static const std::size_t blockLength{38};

	struct BlockData
	{
		uInt64 UUID;
		uInt64 RequestTimestamp;
		uInt32 NextSeqNo;
		uInt32 PreviousSeqNo;
		uInt64 PreviousUUID;
		uInt16 KeepAliveInterval;
		uInt16NULL SecretKeySecureIDExpiration;
		EnumFTI FaultToleranceIndicator;
		uint8_t SplitMsg;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};

	// Override getId() to return our static id.
	//std::size_t getId() const override { return id; }
};

// EstablishmentReject505   BlockLength=72 id=505
struct EstablishmentRejectMsg
{
	inline static const std::size_t id{505};
	inline static const std::size_t blockLength{72};

	struct BlockData
	{
		String48 Reason;
		uInt64 UUID;
		uInt64 RequestTimestamp;
		uInt32 NextSeqNo;
		uInt16 ErrorCodes;
		EnumFTI FaultToleranceIndicator;
		uint8_t SplitMsg;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// Sequence506   BlockLength=14 id=506
struct SequenceMsg
{

	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;

	uInt64 UUID;
	uInt32 NextSeqNo;
	EnumFTI FaultToleranceIndicator;
	EnumKeepAliveLapsed KeepAliveIntervalLapsed;

	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 14;
		templateId_ = 506;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;
	}

} __attribute__((packed));

struct SequenceRecvMsg
{
	inline static const std::size_t id{506};
	inline static const std::size_t blockLength{14};

	struct BlockData
	{
		uInt64 UUID;
		uInt32 NextSeqNo;
		EnumFTI FaultToleranceIndicator;
		EnumKeepAliveLapsed KeepAliveIntervalLapsed;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};

} __attribute__((packed));

// Terminate507   BlockLength=67 id=507
struct TerminateMsg
{
	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;

	String48 Reason;
	uInt64 UUID;
	uInt64 RequestTimestamp;
	uInt16 ErrorCodes;
	uint8_t SplitMsg;


	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 67;
		templateId_ = 507;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;
	}

}__attribute__((packed));

struct TerminateRecvMsg
{
	inline static const std::size_t id{507};
	inline static const std::size_t blockLength{67};

	struct BlockData
	{
		String48 Reason;
		uInt64 UUID;
		uInt64 RequestTimestamp;
		uInt16 ErrorCodes;
		uint8_t SplitMsg;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};

}__attribute__((packed));

// RetransmitRequest508   BlockLength=30 id=508
struct RetransmitRequestMsg
{
	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;


	uInt64 UUID;
	uInt64 LastUUID;
	uInt64 RequestTimestamp;
	uInt32 FromSeqNo;
	uint16_t MsgCount;

	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 30;
		templateId_ = 508;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;

		UUID = 0;
		LastUUID = UINT64_NULL;
		FromSeqNo = 0;
		MsgCount = 0;

	}

}__attribute__((packed));

// Retransmission509   BlockLength=31 id=509
struct RetransmissionMsg
{
	inline static const std::size_t id{509};
	inline static const std::size_t blockLength{31};

	struct BlockData
	{
		uInt64 UUID;
		uInt64NULL LastUUID;
		uInt64 RequestTimestamp;
		uInt32 FromSeqNo;
		uInt16 MsgCount;
		uint8_t SplitMsg;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// RetransmitReject510   BlockLength=75 id=510
struct RetransmitRejectMsg
{
	inline static const std::size_t id{510};
	inline static const std::size_t blockLength{75};

	struct BlockData
	{
		String48 Reason;
		uInt64 UUID;
		uInt64NULL LastUUID;
		uInt64 RequestTimestamp;
		uInt16 ErrorCodes;
		uint8_t SplitMsg;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NotApplied513   BlockLength=17 id=513
struct NotAppliedMsg
{
	inline static const std::size_t id{513};
	inline static const std::size_t blockLength{17};

	struct BlockData
	{
		uInt64 UUID;
		uInt32 FromSeqNo;
		uInt32 MsgCount;
		uint8_t SplitMsg;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NewOrderSingle514   BlockLength=132 id=514
struct NewOrderSingleMsg
{
	//0
	std::uint16_t msgSize; //0-
	std::uint16_t enodingType_;//2
	std::uint16_t blockLength_;//4
	std::uint16_t templateId_;//6
	std::uint16_t schemaId_;//8
	std::uint16_t version_; //10

	int64_t Price; //offset = 12
	uInt32 OrderQty; //20
	Int32 SecurityID; // 24
	EnumSideReq Side; //28
	uInt32 SeqNum; //29
	String20Req SenderID; //33
	String20Req ClOrdID; //53
	uInt64 PartyDetailsListReqID; //73
	uInt64 OrderRequestID; //81
	uInt64 SendingTimeEpoch; //89
	int64_t StopPx; //97
	String5Req Location; //105
	uInt32NULL MinQty; //110
	uInt32NULL DisplayQty; //114
	uint16_t  ExpireDate; //118
	EnumOrderTypeReq OrdType; //126
	EnumTimeInForce TimeInForce; //127
	EnumManualOrdIndReq ManualOrderIndicator; //128
	uint8_t ExecInst;

	EnumExecMode ExecutionMode;
	uint8_t LiquidityFlag;
	uint8_t ManagedOrder;
	uint8_t ShortSaleType;

	int64_t DiscretionPrice;
	int64_t ReservationPrice;

	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 132;
		templateId_ = 514;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;

		ExpireDate = 65535;

		ExecInst = 00000000;// 0xFF;
		ExecutionMode = EnumExecMode::EnumExecMode_Aggressive;
		LiquidityFlag = 0xFF;
		ManagedOrder = 0xFF;
		ShortSaleType = 0xFF;

		DiscretionPrice = 0xFFFFFFFFFFFF7F;
		ReservationPrice = 0xFFFFFFFFFFFF7F;

	}

}__attribute__((packed));

// OrderCancelReplaceRequest515   BlockLength=133 id=515
struct OrderCancelReplaceRequestMsg
{
	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;

	int64_t Price; //12
	uInt32 OrderQty; //20
	Int32 SecurityID; //24
	EnumSideReq Side; //28
	uInt32 SeqNum; //29 --> SAME AS NEW
	String20Req SenderID;
	String20Req ClOrdID;
	uInt64 PartyDetailsListReqID;
	uInt64 OrderID;
	int64_t StopPx;
	uInt64 OrderRequestID;
	uInt64 SendingTimeEpoch;
	String5Req Location;
	uInt32NULL MinQty;
	uInt32NULL DisplayQty;
	uint16_t  ExpireDate;
	EnumOrderTypeReq OrdType;
	EnumTimeInForce TimeInForce;
	EnumManualOrdIndReq ManualOrderIndicator;
	EnumOFMOverrideReq OFMOverride;
	uint8_t ExecInst;
	EnumExecMode ExecutionMode;
	uint8_t LiquidityFlag;
	uint8_t ManagedOrder;
	uint8_t ShortSaleType;
	int64_t DiscretionPrice;

	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 133;
		templateId_ = 515;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;

		ExpireDate = 65535;

		MinQty = IL3::UINT32_NULL;
		DisplayQty = IL3::UINT32_NULL;

		OFMOverride = EnumOFMOverrideReq::EnumOFMOverrideReq_Enabled;


		ExecInst = 00000000;// 0xFF;
		ExecutionMode = EnumExecMode::EnumExecMode_Aggressive;
		LiquidityFlag = 0xFF;
		ManagedOrder = 0xFF;
		ShortSaleType = 0xFF;
		DiscretionPrice = 0xFFFFFFFFFFFF7F;
		//StopPx= 0xFFFFFFFFFFFF7F;
		StopPx = IL3::PRICE_NULL;

		ManualOrderIndicator = EnumManualOrdIndReq::EnumManualOrdIndReq_Manual;
	}


}__attribute__((packed));

// OrderCancelRequest516   BlockLength=88 id=516
struct OrderCancelRequestMsg
{
	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;

	uInt64 OrderID;
	uInt64 PartyDetailsListReqID;
	EnumManualOrdIndReq ManualOrderIndicator;
	uInt32 SeqNum;
	String20Req SenderID;
	String20Req ClOrdID;
	uInt64 OrderRequestID;
	uInt64 SendingTimeEpoch;
	String5Req Location;
	Int32 SecurityID;
	EnumSideReq Side;
	uint8_t LiquidityFlag;
	String8 OrigOrderUser;


	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 96;
		templateId_ = 516;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;

		for (int i = 0; i < 8; i++)
			OrigOrderUser[i] = 0;


		LiquidityFlag = 0xFF;
	}

}__attribute__((packed));

// MassQuote517   BlockLength=92 id=517
struct MassQuoteMsg
{
	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;

	uInt64 PartyDetailsListReqID;
	uInt64 SendingTimeEpoch;
	EnumManualOrdIndReq ManualOrderIndicator;
	uInt32 SeqNum;
	String20Req SenderID;
	uInt64NULL QuoteReqID;
	String5Req Location;
	uInt32 QuoteID;
	uInt8 TotNoQuoteEntries;
	EnumBooleanFlag MMProtectionReset;
	EnumBooleanNULL LiquidityFlag;
	EnumShortSaleType ShortSaleType;
	String30 Reserved;

	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 92;
		templateId_ = 517;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;

		ManualOrderIndicator = EnumManualOrdIndReq::EnumManualOrdIndReq_Manual;
	}

}__attribute__((packed));

// NoQuoteEntries   BlockLength=38 id=295
struct GrpQuoteEntries
{
	inline static const std::size_t id{295};
	inline static const std::size_t blockLength{38};

	struct BlockData
	{
		PRICENULL9 BidPx;
		PRICENULL9 OfferPx;
		uInt32 QuoteEntryID;
		Int32 SecurityID;
		uInt32NULL BidSize;
		uInt32NULL OfferSize;
		Int32NULL UnderlyingSecurityID;
		uInt16 QuoteSetID;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// PartyDetailsDefinitionRequest518   BlockLength=147 id=518
struct PartyDetailsDefinitionRequestMsg
{
	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;

	uInt64 PartyDetailsListReqID;
	uInt64 SendingTimeEpoch;
	EnumListUpdAct ListUpdateAction;
	uInt32 SeqNum;  //33  //29
	char Memo[75]; //108 //104
	char AvgPxGroupID[20];//128 //124
	uInt64NULL SelfMatchPreventionID; //136
	EnumCmtaGiveUpCD CmtaGiveupCD;
	EnumCustOrderCapacity CustOrderCapacity;
	EnumClearingAcctType ClearingAccountType;
	EnumSMPI SelfMatchPreventionInstruction; //140
	EnumAvgPxInd AvgPxIndicator;
	EnumSLEDS ClearingTradePriceType;
	EnumCustOrdHandlInst CustOrderHandlingInst; //143
	uInt64NULL Executor; //151
	uInt64NULL IDMShortCode; //159   - 12 = 147
	//NoPtyUpd NoPartyUpdates;

	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 147;
		templateId_ = 518;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;

		Executor = 0xFFFFFFFFFFFFFFFF;//IL3::UINT64_NULL;// 0xFFFFFFFFFFFF7F;
		IDMShortCode = 0xFFFFFFFFFFFFFFFF;


		for(int i = 0; i < 20; i++)
			AvgPxGroupID[i] =  '\0';
		for(int i = 0; i < 75; i++)
			Memo[i] = '\0';

//		SelfMatchPreventionInstruction = IL3::CHAR_NULL;
//		CmtaGiveupCD = IL3::CHAR_NULL;
//		CustOrderHandlingInst = IL3::CHAR_NULL;
	}
}__attribute__((packed));

// NoPartyDetails   BlockLength=22 id=1671
struct GrpPartyDetail
{
	String20Req PartyDetailID;
	//PartyIDSource PartyDetailIDSource;
	EnumPartyDetailRole PartyDetailRole;
}__attribute__((packed));

// NoTrdRegPublications   BlockLength=2 id=2668
struct GrpTrdRegPublication
{
	uInt8 TrdRegPublicationType;
	uInt8 TrdRegPublicationReason;
}__attribute__((packed));

// PartyDetailsDefinitionRequestAck519   BlockLength=159 id=519
struct PartyDetailsDefinitionRequestAckMsg
{
	inline static const std::size_t id{519};
	inline static const std::size_t blockLength{159};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String75 Memo;
		String20 AvgPxGroupID;
		uInt64 PartyDetailsListReqID;
		uInt64 SendingTimeEpoch;
		uInt64NULL SelfMatchPreventionID;
		uInt8 PartyDetailRequestStatus;
		EnumCustOrderCapacity CustOrderCapacity;
		EnumClearingAcctType ClearingAccountType;
		EnumSMPI SelfMatchPreventionInstruction;
		EnumAvgPxInd AvgPxIndicator;
		EnumSLEDS ClearingTradePriceType;
		EnumCmtaGiveUpCD CmtaGiveupCD;
		EnumCustOrdHandlInst CustOrderHandlingInst;
		//NoPtyUpd NoPartyUpdates;
		EnumListUpdAct ListUpdateAction;
		uInt8 PartyDetailDefinitionStatus;
		uInt64NULL Executor;
		uInt64NULL IDMShortCode;
		EnumBooleanFlag PossRetransFlag;
		uint8_t SplitMsg;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoPartyDetails   BlockLength=22 id=1671
struct GrpPartyDetails519
{
	inline static const std::size_t id{1671};
	inline static const std::size_t blockLength{22};

	struct BlockData
	{
		String20Req PartyDetailID;
		//PartyIDSource PartyDetailIDSource;
		EnumPartyDetailRole PartyDetailRole;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoTrdRegPublications   BlockLength=2 id=2668
struct GrpTrdRegPublications519
{
	inline static const std::size_t id{2668};
	inline static const std::size_t blockLength{2};

	struct BlockData
	{
		uInt8 TrdRegPublicationType;
		uInt8 TrdRegPublicationReason;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// BusinessReject521   BlockLength=330 id=521
struct BusinessRejectMsg
{
	inline static const std::size_t id{521};
	inline static const std::size_t blockLength{330};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String256 Text;
		String20 SenderID;
		uInt64NULL PartyDetailsListReqID;
		uInt64 SendingTimeEpoch;
		uInt64NULL BusinessRejectRefID;
		String5 Location;
		uInt32NULL RefSeqNum;
		uInt16NULL RefTagID;
		uInt16 BusinessRejectReason;
		String2 RefMsgType;
		EnumBooleanFlag PossRetransFlag;
		EnumManualOrdInd ManualOrderIndicator;
		uint8_t SplitMsg;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// ExecutionReportNew522   BlockLength=209 id=522
struct ExecutionReportNewMsg
{
	inline static const std::size_t id{522};
	inline static const std::size_t blockLength{226};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		uInt64 OrderID;
		PRICE9 Price;
		PRICENULL9 StopPx;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		uInt64NULL CrossID;
		uInt64NULL HostCrossID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 OrderQty;
		uInt32NULL MinQty;
		uInt32NULL DisplayQty;
		uint16_t  ExpireDate;
		uInt16NULL DelayDuration;
//		OrdStatusNew OrdStatus;
//		ExecTypNew ExecType;
		//EnumOrderType OrdType;
		//EnumSideReq Side;
		EnumOrderType OrdType;
		EnumSideReq Side;
		EnumTimeInForce TimeInForce;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		EnumSplitMsg SplitMsg;
		uInt8NULL CrossType;
		EnumExecInst ExecInst;
		EnumExecMode ExecutionMode;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanNULL ManagedOrder;
		EnumShortSaleType ShortSaleType;
		uInt64NULL DelayToTime;
		PRICENULL9 DiscretionPrice;
		PRICENULL9 ReservationPrice;
		uInt8NULL PriorityIndicator;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// ExecutionReportReject523   BlockLength=467 id=523
struct ExecutionReportRejectMsg
{
	inline static const std::size_t id{523};
	inline static const std::size_t blockLength{483};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String256 Text;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		uInt64 OrderID;
		PRICENULL9 Price;
		PRICENULL9 StopPx;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		uInt64NULL CrossID;
		uInt64NULL HostCrossID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 OrderQty;
		uInt32NULL MinQty;
		uInt32NULL DisplayQty;
		uInt16 OrdRejReason;
		uint16_t  ExpireDate;
		uInt16NULL DelayDuration;
//		OrdStatusRej OrdStatus;
//		ExecTypRej ExecType;
		EnumOrderType OrdType;
		EnumSideReq Side;
		EnumTimeInForce TimeInForce;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		EnumSplitMsg SplitMsg;
		uInt8NULL CrossType;
		EnumExecInst ExecInst;
		EnumExecMode ExecutionMode;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanNULL ManagedOrder;
		EnumShortSaleType ShortSaleType;
		uInt64NULL DelayToTime;
		PRICENULL9 DiscretionPrice;
		PRICENULL9 ReservationPrice;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// ExecutionReportElimination524   BlockLength=202 id=524
struct ExecutionReportEliminationMsg
{
	inline static const std::size_t id{524};
	inline static const std::size_t blockLength{219};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		uInt64 OrderID;
		PRICE9 Price;
		PRICENULL9 StopPx;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		uInt64NULL CrossID;
		uInt64NULL HostCrossID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 CumQty;
		uInt32 OrderQty;
		uInt32NULL MinQty;
		uInt32NULL DisplayQty;
//		OrdStatusExp OrdStatus;
//		ExecTypeExp ExecType;
		uint16_t  ExpireDate;
		EnumOrderType OrdType;
		EnumSideReq Side;
		EnumTimeInForce TimeInForce;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		uInt8NULL CrossType;
		uint8_t SplitMsg;
		EnumExecMode ExecutionMode;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanNULL ManagedOrder;
		EnumShortSaleType ShortSaleType;
		PRICENULL9 DiscretionPrice;
		PRICENULL9 ReservationPrice;
		uInt8NULL PriorityIndicator;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// ExecutionReportTradeOutright525   BlockLength=235 id=525
struct ExecutionReportTradeOutrightMsg
{
	inline static const std::size_t id{525};
	inline static const std::size_t blockLength{293};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		PRICE9 LastPx;
		uInt64 OrderID;
		PRICE9 Price;
		PRICENULL9 StopPx;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		uInt64 SecExecID;
		uInt64NULL CrossID;
		uInt64NULL HostCrossID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 OrderQty;
		uInt32 LastQty;
		uInt32 CumQty;
		uInt32 MDTradeEntryID;
		uInt32 SideTradeID;
		uInt32NULL TradeLinkID;
		uInt32 LeavesQty;
		uint16_t  TradeDate;
		uint16_t  ExpireDate;
		EnumOrdStatusTrd OrdStatus;
		//EnumExecTypeTrade ExecType;
		EnumOrderType OrdType;
		EnumSideReq Side;
		EnumTimeInForce TimeInForce;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		EnumBooleanFlag AggressorIndicator;
		uInt8NULL CrossType;
		uint8_t SplitMsg;
		EnumExecMode ExecutionMode;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanNULL ManagedOrder;
		EnumShortSaleType ShortSaleType;
		uint8_t Ownership;
		PRICENULL9 DiscretionPrice;
		uInt16NULL TrdType;
		EnumExecRestateReason ExecRestatementReason;
		uint16_t SettlDate;
		uint16_t MaturityDate;
		Decimal64NULL CalculatedCcyLastQty;
		Decimal64NULL GrossTradeAmt;
		PRICENULL9 BenchmarkPrice;
		PRICENULL9 ReservationPrice;
		uInt8NULL PriorityIndicator;
		PRICENULL9 DisplayLimitPrice;
//		uint8_t NoFills;
//		uint8_t NoOrders;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoFills   BlockLength=15 id=1362
struct GrpOutrightFillEvent
{
	inline static const std::size_t id{1362};
	inline static const std::size_t blockLength{15};

	struct BlockData
	{
		PRICE9 FillPx;
		uInt32 FillQty;
		String2 FillExecID;
		uInt8 FillYieldType;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoOrderEvents   BlockLength=23 id=1795
struct GrpOutrightOrderEvent
{
	inline static const std::size_t id{1795};
	inline static const std::size_t blockLength{41};

	struct BlockData
	{
		PRICE9 OrderEventPx;
		String5 OrderEventText;
		uInt32 OrderEventExecID;
		uInt32 OrderEventQty;
		EnumOrderEventType OrderEventType;
		uInt8 OrderEventReason;
		Decimal64NULL ContraGrossTradeAmt;
		Decimal64NULL ContraCalculatedCcyLastQty;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// ExecutionReportTradeSpread526   BlockLength=230 id=526
struct ExecutionReportTradeSpreadMsg
{
	inline static const std::size_t id{526};
	inline static const std::size_t blockLength{230};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		PRICE9 LastPx;
		uInt64 OrderID;
		PRICE9 Price;
		PRICENULL9 StopPx;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		uInt64 SecExecID;
		uInt64NULL CrossID;
		uInt64NULL HostCrossID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 OrderQty;
		uInt32 LastQty;
		uInt32 CumQty;
		uInt32 MDTradeEntryID;
		uInt32 SideTradeID;
		uInt32 LeavesQty;
		uint16_t  TradeDate;
		uint16_t  ExpireDate;
		EnumOrdStatusTrd OrdStatus;
		//ExecTypeTrade ExecType;
		EnumOrderType OrdType;
		EnumSideReq Side;
		EnumTimeInForce TimeInForce;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		EnumBooleanFlag AggressorIndicator;
		uInt8NULL CrossType;
		uInt8 TotalNumSecurities;
		uint8_t SplitMsg;
		EnumExecMode ExecutionMode;
		EnumBooleanNULL LiquidityFlag;
		EnumShortSaleType ShortSaleType;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoFills   BlockLength=15 id=1362
struct GrpSpreadFillEvent
{
	inline static const std::size_t id{1362};
	inline static const std::size_t blockLength{15};

	struct BlockData
	{
		PRICE9 FillPx;
		uInt32 FillQty;
		String2 FillExecID;
		uInt8 FillYieldType;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoLegs   BlockLength=29 id=555
struct GrpSpreadFillLegEvent
{
	inline static const std::size_t id{555};
	inline static const std::size_t blockLength{29};

	struct BlockData
	{
		uInt64 LegExecID;
		PRICE9 LegLastPx;
		Int32 LegSecurityID;
		uInt32 LegTradeID;
		uInt32 LegLastQty;
		EnumSideReq LegSide;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoOrderEvents   BlockLength=23 id=1795
struct GrpSpreadFillOrderEvent
{
	inline static const std::size_t id{1795};
	inline static const std::size_t blockLength{23};

	struct BlockData
	{
		PRICE9 OrderEventPx;
		String5 OrderEventText;
		uInt32 OrderEventExecID;
		uInt32 OrderEventQty;
		EnumOrderEventType OrderEventType;
		uInt8 OrderEventReason;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// ExecutionReportTradeSpreadLeg527   BlockLength=199 id=527
struct ExecutionReportTradeSpreadLegMsg
{
	inline static const std::size_t id{527};
	inline static const std::size_t blockLength{219};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		Decimal64NULL Volatility;
		uInt64 PartyDetailsListReqID;
		PRICE9 LastPx;
		uInt64 OrderID;
		PRICENULL9 UnderlyingPx;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 SecExecID;
		String5Req Location;
		Decimal32NULL OptionDelta;
		Decimal32NULL TimeToExpiration;
		Decimal32NULL RiskFreeRate;
		Int32 SecurityID;
		uInt32 LastQty;
		uInt32 CumQty;
		uInt32 SideTradeID;
		uint16_t  TradeDate;
		EnumOrdStatusTrd OrdStatus;
		//ExecTypeTrade ExecType;
		EnumOrderType OrdType;
		EnumSideReq Side;
		EnumBooleanFlag PossRetransFlag;
		uint16_t SettlDate;
		Decimal64NULL CalculatedCcyLastQty;
		Decimal64NULL GrossTradeAmt;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoFills   BlockLength=15 id=1362
struct GrpSpreadLegFillEvent
{
	inline static const std::size_t id{1362};
	inline static const std::size_t blockLength{15};

	struct BlockData
	{
		PRICE9 FillPx;
		uInt32 FillQty;
		String2 FillExecID;
		uInt8 FillYieldType;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoOrderEvents   BlockLength=23 id=1795
struct GrpSpreadLegFillOrderEvent
{
	inline static const std::size_t id{1795};
	inline static const std::size_t blockLength{23};

	struct BlockData
	{
		PRICE9 OrderEventPx;
		String5 OrderEventText;
		uInt32 OrderEventExecID;
		uInt32 OrderEventQty;
		EnumOrderEventType OrderEventType;
		uInt8 OrderEventReason;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// QuoteCancel528   BlockLength=52 id=528
struct QuoteCancelMsg
{
	inline static const std::size_t id{528};
	inline static const std::size_t blockLength{52};

	struct BlockData
	{
		uInt64 PartyDetailsListReqID;
		uInt64 SendingTimeEpoch;
		EnumManualOrdIndReq ManualOrderIndicator;
		uInt32 SeqNum;
		String20Req SenderID;
		String5Req Location;
		uInt32 QuoteID;
		EnumQuoteCxlTyp QuoteCancelType;
		EnumBooleanNULL LiquidityFlag;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoQuoteEntries   BlockLength=10 id=295
//struct GrpQuoteEntries
//{
//	inline static const std::size_t id{295};
//	inline static const std::size_t blockLength{10};
//
//	struct BlockData
//	{
//		StringLength6 SecurityGroup;
//		Int32NULL SecurityID;
//	} __attribute__((packed));
//
//	BlockData *blockData_{nullptr};
//};

// NoQuoteSets   BlockLength=10 id=296
struct GrpQuoteSets
{
	inline static const std::size_t id{296};
	inline static const std::size_t blockLength{10};

	struct BlockData
	{
		uInt32NULL BidSize;
		uInt32NULL OfferSize;
		uInt16 QuoteSetID;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// OrderMassActionRequest529   BlockLength=71 id=529
struct OrderMassActionRequestMsg
{
	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;

	uInt64 PartyDetailsListReqID;
	uInt64 OrderRequestID;
	EnumManualOrdIndReq ManualOrderIndicator;
	uInt32 SeqNum;
	String20Req SenderID;
	//MassAction MassActionType;n="Specifies the type of action requested; Constant value" semanticType="char"/>
	uInt64 SendingTimeEpoch;
	StringLength6 SecurityGroup;
	String5Req Location;
	Int32NULL SecurityID;
	EnumMassActionScope MassActionScope;
	uInt8NULL MarketSegmentID;
	uint8_t MassCancelRequestType;
	uint8_t Side;
	char OrdType;
	uint8_t TimeInForce;
	uint8_t LiquidityFlag;
	String8 OrigOrderUser;

	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 79;
		templateId_ = 529;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;

		ManualOrderIndicator = EnumManualOrdIndReq::EnumManualOrdIndReq_Manual;

		MassCancelRequestType = IL3::UINT8_NULL;
		SecurityID = IL3::INT32_NULL;
		MarketSegmentID = IL3::UINT8_NULL;

		MassCancelRequestType = 0xFF;
		Side = 0xFF;


		MarketSegmentID = IL3::UINT8_NULL;
		LiquidityFlag = 0xFF;

		OrdType = CHAR_NULL;
		TimeInForce = 0xFF;
		for (int i = 0; i < 8; i++)
			OrigOrderUser[i] = 0;

		for (int i = 0; i < 6; i++)
			SecurityGroup[i] = 0;
	}

}__attribute__((packed));

// OrderMassStatusRequest530   BlockLength=68 id=530
struct OrderMassStatusRequestMsg
{
	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;

	uInt64 PartyDetailsListReqID;
	uInt64 MassStatusReqID;
	EnumManualOrdIndReq ManualOrderIndicator;
	uInt32 SeqNum;
	String20Req SenderID;
	uInt64 SendingTimeEpoch;
	StringLength6 SecurityGroup;
	String5Req Location;
	Int32NULL SecurityID;
	EnumMassStatusReqTyp MassStatusReqType;
	uint8_t OrdStatusReqType;
	uint8_t TimeInForce;
	uInt8NULL MarketSegmentID;

	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 68;
		templateId_ = 530;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;

		ManualOrderIndicator = EnumManualOrdIndReq::EnumManualOrdIndReq_Manual;
		TimeInForce = 0xFF;
		OrdStatusReqType = 0XFF;

		SecurityID = IL3::INT32_NULL;
		MarketSegmentID = IL3::UINT8_NULL;
		MassStatusReqID = 0;


		for (int i = 0; i < 6; i++)
			SecurityGroup[i] = 0;
	}

}__attribute__((packed));

// ExecutionReportModify531   BlockLength=217 id=531
struct ExecutionReportModifyMsg
{
	inline static const std::size_t id{531};
	inline static const std::size_t blockLength{226};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		uInt64 OrderID;
		PRICE9 Price;
		PRICENULL9 StopPx;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		uInt64NULL CrossID;
		uInt64NULL HostCrossID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 OrderQty;
		uInt32 CumQty;
		uInt32 LeavesQty;
		uInt32NULL MinQty;
		uInt32NULL DisplayQty;
		uint16_t  ExpireDate;
		uInt16NULL DelayDuration;
		//ModifyStatus OrdStatus;
		//ExecTypeModify ExecType;
		EnumOrderType OrdType;
		EnumSideReq Side;
		EnumTimeInForce TimeInForce;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		EnumSplitMsg SplitMsg;
		uInt8NULL CrossType;
		EnumExecInst ExecInst;
		EnumExecMode ExecutionMode;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanNULL ManagedOrder;
		EnumShortSaleType ShortSaleType;
		uInt64NULL DelayToTime;
		PRICENULL9 DiscretionPrice;
		uInt8NULL PriorityIndicator;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// ExecutionReportStatus532   BlockLength=480 id=532
struct ExecutionReportStatusMsg
{
	inline static const std::size_t id{532};
	inline static const std::size_t blockLength{480};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String256 Text;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		uInt64 OrderID;
		PRICENULL9 Price;
		PRICENULL9 StopPx;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		uInt64NULL OrdStatusReqID;
		uInt64NULL MassStatusReqID;
		uInt64NULL CrossID;
		uInt64NULL HostCrossID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 OrderQty;
		uInt32 CumQty;
		uInt32 LeavesQty;
		uInt32NULL MinQty;
		uInt32NULL DisplayQty;
		uint16_t  ExpireDate;
		EnumOrderStatus OrdStatus;
		//ExecTypStatus ExecType;
		EnumOrderType OrdType;
		EnumSideReq Side;
		EnumTimeInForce TimeInForce;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		EnumBooleanNULL LastRptRequested;
		uInt8NULL CrossType;
		uint8_t SplitMsg;
		EnumExecMode ExecutionMode;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanNULL ManagedOrder;
		EnumShortSaleType ShortSaleType;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// OrderStatusRequest533   BlockLength=62 id=533
struct OrderStatusRequestMsg
{
	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;

	uInt64 PartyDetailsListReqID;
	uInt64 OrdStatusReqID;
	EnumManualOrdIndReq ManualOrderIndicator;
	uInt32 SeqNum;
	String20Req SenderID;
	uInt64 OrderID;
	uInt64 SendingTimeEpoch;
	String5Req Location;

	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 62;
		templateId_ = 533;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;
	}

}__attribute__((packed));

// ExecutionReportCancel534   BlockLength=214 id=534
struct ExecutionReportCancelMsg
{
	inline static const std::size_t id{534};
	inline static const std::size_t blockLength{247};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		uInt64 OrderID;
		PRICE9 Price;
		PRICENULL9 StopPx;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		uInt64NULL CrossID;
		uInt64NULL HostCrossID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 OrderQty;
		uInt32 CumQty;
		uInt32NULL MinQty;
		uInt32NULL DisplayQty;
		uint16_t  ExpireDate;
		uInt16NULL DelayDuration;
		//OrdStatusCxl OrdStatus;
		//ExecTypeCxl ExecType;
		EnumOrderType OrdType;
		EnumSideReq Side;
		EnumTimeInForce TimeInForce;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		EnumSplitMsg SplitMsg;
		EnumExecRestateReason ExecRestatementReason;
		uInt8NULL CrossType;
		EnumExecInst ExecInst;
		EnumExecMode ExecutionMode;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanNULL ManagedOrder;
		EnumShortSaleType ShortSaleType;
		uInt64NULL DelayToTime;
		PRICENULL9 DiscretionPrice;
		PRICENULL9 ReservationPrice;
		uInt8NULL PriorityIndicator;
		String8 OrigOrderUser;
		String8 CancelText;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// OrderCancelReject535   BlockLength=409 id=535
struct OrderCancelRejectMsg
{
	inline static const std::size_t id{535};
	inline static const std::size_t blockLength{409};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String256 Text;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		//OrdStatusCxlRej OrdStatus;
		//CxlRejRsp CxlRejResponseTo;
		uInt64 OrderID;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		String5Req Location;
		uInt16 CxlRejReason;
		uInt16NULL DelayDuration;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		uint8_t SplitMsg;
		EnumBooleanNULL LiquidityFlag;
		uInt64NULL DelayToTime;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// OrderCancelReplaceReject536   BlockLength=409 id=536
struct OrderCancelReplaceRejectMsg
{
	inline static const std::size_t id{536};
	inline static const std::size_t blockLength{409};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String256 Text;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		uInt64 OrderID;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		String5Req Location;
		uInt16 CxlRejReason;
		uInt16NULL DelayDuration;
		//OrdStatusCxrRej OrdStatus;
		//CxrRejRsp CxlRejResponseTo;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		uint8_t SplitMsg;
		EnumBooleanNULL LiquidityFlag;
		uInt64NULL DelayToTime;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// PartyDetailsListRequest537   BlockLength=20 id=537
struct PartyDetailsListRequestMsg
{
	std::uint16_t msgSize;
	std::uint16_t enodingType_;
	std::uint16_t blockLength_;
	std::uint16_t templateId_;
	std::uint16_t schemaId_;
	std::uint16_t version_;

	uInt64 PartyDetailsListReqID;
	uInt64 SendingTimeEpoch;
	uInt32 SeqNum;

	void Construct()
	{
		enodingType_ = IL3::ENCODING_TYPE;
		blockLength_ = 20;
		templateId_ = 537;
		schemaId_ = IL3::SCHEMA_ID;
		version_ =IL3::VERSION;
	}

} __attribute__((packed));

// NoRequestingPartyIDs   BlockLength=7 id=1657
struct GrpRequestingPartyIDs
{
//	inline static const std::size_t id{1657};
//	inline static const std::size_t blockLength{7};
//
//	struct BlockData
//	{
		String5 RequestingPartyID;
		CHAR RequestingPartyIDSource;
		CHAR RequestingPartyRole;
//	} __attribute__((packed));
//
//	BlockData *blockData_{nullptr};
}__attribute__((packed));

// NoPartyIDs   BlockLength=11 id=453
struct GrpPartyIDs
{
//	inline static const std::size_t id{453};
//	inline static const std::size_t blockLength{11};
//
//	struct BlockData
//	{
		uInt64 PartyID;
		CHAR PartyIDSource;
		uInt16 PartyRole;
//	} __attribute__((packed));
//
//	BlockData *blockData_{nullptr};
}__attribute__((packed));

// PartyDetailsListReport538   BlockLength=93 id=538
struct PartyDetailsListReportMsg
{
	inline static const std::size_t id{538};
	inline static const std::size_t blockLength{93};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String20 AvgPxGroupID;
		uInt64 PartyDetailsListReqID;
		uInt64 PartyDetailsListReportID;
		uInt64 SendingTimeEpoch;
		uInt64NULL SelfMatchPreventionID;
		uInt16 TotNumParties;
		EnumReqResult RequestResult;
		EnumBooleanFlag LastFragment;
		EnumCustOrderCapacity CustOrderCapacity;
		EnumClearingAcctType ClearingAccountType;
		EnumSMPI SelfMatchPreventionInstruction;
		EnumAvgPxInd AvgPxIndicator;
		EnumSLEDS ClearingTradePriceType;
		EnumCmtaGiveUpCD CmtaGiveupCD;
		EnumCustOrdHandlInst CustOrderHandlingInst;
		uInt64NULL Executor;
		uInt64NULL IDMShortCode;
		EnumBooleanFlag PossRetransFlag;
		uint8_t SplitMsg;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoPartyDetails   BlockLength=22 id=1671
struct GrpPartyDetailsListReport
{
	inline static const std::size_t id{1671};
	inline static const std::size_t blockLength{22};

	struct BlockData
	{
		String20Req PartyDetailID;
		//char PartyDetailIDSource;
		EnumPartyDetailRole PartyDetailRole;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoTrdRegPublications   BlockLength=2 id=2668
struct GrpTrdRegPublicationsListReport
{
	inline static const std::size_t id{2668};
	inline static const std::size_t blockLength{2};

	struct BlockData
	{
		uInt8 TrdRegPublicationType;
		uInt8 TrdRegPublicationReason;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// ExecutionAck539   BlockLength=101 id=539
struct ExecutionAckMsg
{
	inline static const std::size_t id{539};
	inline static const std::size_t blockLength{101};

	struct BlockData
	{
		uInt64 PartyDetailsListReqID;
		uInt64 OrderID;
		EnumExecAckStatus ExecAckStatus;
		uInt32 SeqNum;
		String20Req ClOrdID;
		uInt64 SecExecID;
		PRICE9 LastPx;
		Int32 SecurityID;
		uInt32 LastQty;
		EnumDKReason DKReason;
		EnumSideReq Side;
		String20Req SenderID;
		uInt64 SendingTimeEpoch;
		String5Req Location;
		EnumManualOrdIndReq ManualOrderIndicator;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// RequestForQuote543   BlockLength=55 id=543
struct RequestForQuoteMsg
{
	inline static const std::size_t id{543};
	inline static const std::size_t blockLength{55};

	struct BlockData
	{
		uInt64 PartyDetailsListReqID;
		uInt64 QuoteReqID;
		EnumManualOrdIndReq ManualOrderIndicator;
		uInt32 SeqNum;
		String20Req SenderID;
		uInt64 SendingTimeEpoch;
		String5Req Location;
		EnumQuoteTyp QuoteType;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoRelatedSym   BlockLength=9 id=146
struct GrpRelatedSym
{
	inline static const std::size_t id{146};
	inline static const std::size_t blockLength{9};

	struct BlockData
	{
		Int32 SecurityID;
		uInt32NULL OrderQty;
		EnumRFQSide Side;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NewOrderCross544   BlockLength=74 id=544
struct NewOrderCrossMsg
{
	inline static const std::size_t id{544};
	inline static const std::size_t blockLength{74};

	struct BlockData
	{
		uInt64 CrossID;
		uInt64 OrderRequestID;
		EnumManualOrdIndReq ManualOrderIndicator;
		uInt32 SeqNum;
		String20Req SenderID;
		//CrossOrderType OrdType;
		////CrossType CrossType;
		//CrossPrioritization CrossPrioritization;
		PRICE9 Price;
		uInt64 TransBkdTime;
		uInt64 SendingTimeEpoch;
		String5Req Location;
		Int32 SecurityID;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoSides   BlockLength=34 id=552
struct GrpSides
{
	inline static const std::size_t id{552};
	inline static const std::size_t blockLength{34};

	struct BlockData
	{
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		uInt32 OrderQty;
		EnumSideReq Side;
		EnumSideTimeInForce SideTimeInForce;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// MassQuoteAck545   BlockLength=350 id=545
struct MassQuoteAckMsg
{
	inline static const std::size_t id{545};
	inline static const std::size_t blockLength{350};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String256 Text;
		String20Req SenderID;
		uInt64 PartyDetailsListReqID;
		uInt64 RequestTime;
		uInt64 SendingTimeEpoch;
		uInt64NULL QuoteReqID;
		String5Req Location;
		uInt32 QuoteID;
		uInt16NULL QuoteRejectReason;
		uInt16NULL DelayDuration;
		EnumQuoteAckStatus QuoteStatus;
		EnumManualOrdIndReq ManualOrderIndicator;
		uInt8 NoProcessedEntries;
		EnumBooleanFlag MMProtectionReset;
		uint8_t SplitMsg;
		EnumBooleanNULL LiquidityFlag;
		EnumShortSaleType ShortSaleType;
		uInt8NULL TotNoQuoteEntries;
		EnumBooleanFlag PossRetransFlag;
		uInt64NULL DelayToTime;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoQuoteEntries   BlockLength=11 id=295
struct GrpQuoteEntriesMassQuoteAck
{
	inline static const std::size_t id{295};
	inline static const std::size_t blockLength{11};

	struct BlockData
	{
		uInt32 QuoteEntryID;
		Int32 SecurityID;
		uInt16 QuoteSetID;
		uInt8 QuoteEntryRejectReason;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// RequestForQuoteAck546   BlockLength=358 id=546
struct RequestForQuoteAckMsg
{
	inline static const std::size_t id{546};
	inline static const std::size_t blockLength{358};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String256 Text;
		String20Req SenderID;
		String17 ExchangeQuoteReqID;
		uInt64 PartyDetailsListReqID;
		uInt64 RequestTime;
		uInt64 SendingTimeEpoch;
		uInt64 QuoteReqID;
		String5Req Location;
		uInt16NULL QuoteRejectReason;
		uInt16NULL DelayDuration;
		EnumQuoteAckStatus QuoteStatus;
		EnumManualOrdIndReq ManualOrderIndicator;
		uint8_t SplitMsg;
		EnumBooleanFlag PossRetransFlag;
		uInt64NULL DelayToTime;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// ExecutionReportTradeAddendumOutright548   BlockLength=181 id=548
struct ExecutionReportTradeAddendumOutrightMsg
{
	inline static const std::size_t id{548};
	inline static const std::size_t blockLength{222};

	//NOTE: Here, we reuse GrpOutrightFillEvent and GrpOutrightOrderEvent as repeating groups following BlockData
	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		PRICE9 LastPx;
		uInt64 OrderID;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 SecExecID;
		uInt64NULL OrigSecondaryExecutionID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 LastQty;
		uInt32 SideTradeID;
		uInt32NULL OrigSideTradeID;
		uint16_t  TradeDate;
		EnumOrdStatusTrdCxl OrdStatus;
		EnumExecTypTrdCxl ExecType;
		EnumSideReq Side;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;//////
		EnumExecInst ExecInst;
		EnumExecMode ExecutionMode;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanNULL ManagedOrder;
		EnumShortSaleType ShortSaleType;
		//added version7
		PRICE9 DiscretionPrice;
		uint16_t TrdType;
		EnumExecRestateReason ExecRestatement;
		uint16_t SettlDate;
		uint16_t MaturityDate;
		Decimal64NULL CalculatedCcyLastQty;
		Decimal64NULL GrossTradeAmt;
		Decimal64NULL BenchmarkPrice;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// ExecutionReportTradeAddendumSpread549   BlockLength=187 id=549
struct ExecutionReportTradeAddendumSpreadMsg
{
	inline static const std::size_t id{549};
	inline static const std::size_t blockLength{187};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		PRICE9 LastPx;
		uInt64 OrderID;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 SecExecID;
		uInt64NULL OrigSecondaryExecutionID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 MDTradeEntryID;
		uInt32 LastQty;
		uInt32 SideTradeID;
		uInt32NULL OrigSideTradeID;
		uint16_t  TradeDate;
		EnumOrdStatusTrdCxl OrdStatus;
		EnumExecTypTrdCxl ExecType;
		EnumOrderType OrdType;
		EnumSideReq Side;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		uInt8 TotalNumSecurities;
		uint8_t SplitMsg;
		EnumExecMode ExecutionMode;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanNULL ManagedOrder;
		EnumShortSaleType ShortSaleType;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};
//
//// NoFills   BlockLength=15 id=1362
//struct GrpFills
//{
//	inline static const std::size_t id{1362};
//	inline static const std::size_t blockLength{15};
//
//	struct BlockData
//	{
//		PRICE9 FillPx;
//		uInt32 FillQty;
//		String2 FillExecID;
//		uInt8 FillYieldType;
//	} __attribute__((packed));
//
//	BlockData *blockData_{nullptr};
//};
//
//// NoLegs   BlockLength=41 id=555
//struct GrpLegs
//{
//	inline static const std::size_t id{555};
//	inline static const std::size_t blockLength{41};
//
//	struct BlockData
//	{
//		uInt64 LegExecID;
//		PRICE9 LegLastPx;
//		uInt64NULL LegExecRefID;
//		uInt32 LegTradeID;
//		uInt32NULL LegTradeRefID;
//		Int32 LegSecurityID;
//		uInt32 LegLastQty;
//		SideReq LegSide;
//	} __attribute__((packed));
//
//	BlockData *blockData_{nullptr};
//};
//
//// NoOrderEvents   BlockLength=27 id=1795
//struct GrpOrderEvents
//{
//	inline static const std::size_t id{1795};
//	inline static const std::size_t blockLength{27};
//
//	struct BlockData
//	{
//		PRICE9 OrderEventPx;
//		String5 OrderEventText;
//		uInt32 OrderEventExecID;
//		uInt32 OrderEventQty;
//		uint8_t  OrderEventType;
//		uInt8 OrderEventReason;
//		uInt32NULL OriginalOrderEventExecID;
//	} __attribute__((packed));
//
//	BlockData *blockData_{nullptr};
//};

// ExecutionReportTradeAddendumSpreadLeg550   BlockLength=176 id=550
struct ExecutionReportTradeAddendumSpreadLegMsg
{
	inline static const std::size_t id{550};
	inline static const std::size_t blockLength{176};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String40 ExecID;
		String20Req SenderID;
		String20Req ClOrdID;
		uInt64 PartyDetailsListReqID;
		PRICE9 LastPx;
		uInt64 OrderID;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 SecExecID;
		uInt64NULL OrigSecondaryExecutionID;
		String5Req Location;
		Int32 SecurityID;
		uInt32 LastQty;
		uInt32 SideTradeID;
		uInt32NULL OrigSideTradeID;
		uint16_t  TradeDate;
		EnumOrdStatusTrdCxl OrdStatus;
		EnumExecTypTrdCxl ExecType;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumBooleanFlag PossRetransFlag;
		EnumSideReq Side;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};
//
//// NoFills   BlockLength=15 id=1362
//struct GrpFills
//{
//	inline static const std::size_t id{1362};
//	inline static const std::size_t blockLength{15};
//
//	struct BlockData
//	{
//		PRICE9 FillPx;
//		uInt32 FillQty;
//		String2 FillExecID;
//		uInt8 FillYieldType;
//	} __attribute__((packed));
//
//	BlockData *blockData_{nullptr};
//};
//
//// NoOrderEvents   BlockLength=27 id=1795
//struct GrpOrderEvents
//{
//	inline static const std::size_t id{1795};
//	inline static const std::size_t blockLength{27};
//
//	struct BlockData
//	{
//		PRICE9 OrderEventPx;
//		String5 OrderEventText;
//		uInt32 OrderEventExecID;
//		uInt32 OrderEventQty;
//		uint8_t  OrderEventType;
//		uInt8 OrderEventReason;
//		uInt32NULL OriginalOrderEventExecID;
//	} __attribute__((packed));
//
//	BlockData *blockData_{nullptr};
//};

// SecurityDefinitionRequest560   BlockLength=71 id=560
struct SecurityDefinitionRequestMsg
{
	inline static const std::size_t id{560};
	inline static const std::size_t blockLength{71};

	struct BlockData
	{
		uInt64 PartyDetailsListReqID;
		uInt64 SecurityReqID;
		EnumManualOrdIndReq ManualOrderIndicator;
		uInt32 SeqNum;
		String20 SenderID;
		//char SecurityReqType;description="Type of Security Definition Request. Constant value" semanticType="char"/>
		uInt64 SendingTimeEpoch;
		String8Req SecuritySubType;
		String5Req Location;
		uint16_t  StartDate;
		uint16_t  EndDate;
		uInt8NULL MaxNoOfSubstitutions;
		Int32NULL SourceRepoID;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoLegs   BlockLength=19 id=555
struct GrpLegs560
{
	inline static const std::size_t id{555};
	inline static const std::size_t blockLength{19};

	struct BlockData
	{
		//LegSecIDSource LegSecurityIDSource;="Leg security ID source in UDS creation" presence="constant" length="1" primitiveType="char" semanticType="char">8</type>
		PRICENULL9 LegPrice;
		Int32 LegSecurityID;
		Decimal32NULL LegOptionDelta;
		EnumSideReq LegSide;
		uInt8NULL LegRatioQty;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// SecurityDefinitionResponse561   BlockLength=429 id=561
struct SecurityDefinitionResponseMsg
{
	inline static const std::size_t id{561};
	inline static const std::size_t blockLength{429};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String256 Text;
		StringLength35 FinancialInstrumentFullName;
		String20Req SenderID;
		String20 Symbol;
		uInt64 PartyDetailsListReqID;
		uInt64 SecurityReqID;
		uInt64 SecurityResponseID;
		uInt64 SendingTimeEpoch;
		StringLength6 SecurityGroup;
		StringLength6 SecurityType;
		String5Req Location;
		Int32NULL SecurityID;
		String3 Currency;
		//SecurityIDSource SecurityIDSource;
		MaturityMonthYear MMY;
		uInt16NULL DelayDuration;
		uint16_t  StartDate;
		uint16_t  EndDate;
		uInt8NULL MaxNoOfSubstitutions;
		Int32NULL SourceRepoID;
		String8 TerminationType;
		EnumSecRspTyp SecurityResponseType;
		char UserDefinedInstrument;
		EnumExpCycle ExpirationCycle;
		EnumManualOrdIndReq ManualOrderIndicator;
		uint8_t SplitMsg;
		EnumBooleanFlag AutoQuoteRequest;
		EnumBooleanFlag PossRetransFlag;


	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoLegs   BlockLength=19 id=555
struct GrpLegs
{
	inline static const std::size_t id{555};
	inline static const std::size_t blockLength{19};

	struct BlockData
	{
		PRICENULL9 LegPrice;
		Decimal32NULL LegOptionDelta;
		//LegSecIDSource LegSecurityIDSource;
		Int32 LegSecurityID;
		EnumSideReq LegSide;
		uInt8NULL LegRatioQty;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// OrderMassActionReport562   BlockLength=114 id=562
struct OrderMassActionReportMsg
{
	inline static const std::size_t id{562};
	inline static const std::size_t blockLength{130};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String20Req SenderID;
		uInt64 PartyDetailsListReqID;
		uInt64 TransactTime;
		uInt64 SendingTimeEpoch;
		uInt64 OrderRequestID;
		uInt64 MassActionReportID; //offset = 64; 72 after
		//MassAction MassActionType; ="MassAction" description="Mass action type to represent mass cancel" presence="constant" length="1" primitiveType="char" semanticType="char">3</type>
		StringLength6 SecurityGroup;
		String5Req Location;
		Int32NULL SecurityID;
		uInt16NULL DelayDuration; //89
		EnumMassActionResponse MassActionResponse;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumMassActionScope MassActionScope;
		uInt32 TotalAffectedOrders; //96
		EnumBooleanFlag LastFragment;
		uInt8NULL MassActionRejectReason;
		uInt8NULL MarketSegmentID;
		EnumMassCxlReqTyp MassCancelRequestType;
		EnumSideNULL Side;
		EnumMassActionOrdTyp OrdType;
		EnumMassCancelTIF TimeInForce;
		uint8_t SplitMsg;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanFlag PossRetransFlag;
		uInt64NULL DelayToTime;
		String8 OrigOrderUser;
		String8 CancelText;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// NoAffectedOrders   BlockLength=32 id=534
struct GrpAffectedOrdersMassActionReport
{
	inline static const std::size_t id{534};
	inline static const std::size_t blockLength{32};

	struct BlockData
	{
		String20Req OrigCIOrdID;
		uInt64 AffectedOrderID;
		uInt32 CxlQuantity;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};

// QuoteCancelAck563   BlockLength=351 id=563
struct QuoteCancelAckMsg
{
	inline static const std::size_t id{563};
	inline static const std::size_t blockLength{351};

	struct BlockData
	{
		uInt32 SeqNum;
		uInt64 UUID;
		String256 Text;
		String20Req SenderID;
		uInt64 PartyDetailsListReqID;
		uInt64 RequestTime;
		uInt64 SendingTimeEpoch;
		StringLength6 CancelledSymbol;
		String5Req Location;
		uInt32 QuoteID;
		uInt16NULL QuoteRejectReason;
		uInt16NULL DelayDuration;
		EnumManualOrdIndReq ManualOrderIndicator;
		EnumQuoteCxlStatus QuoteStatus;
		uInt32 NoProcessedEntries;
		EnumBooleanFlag MMProtectionReset;
		charNULL UnsolicitedCancelType;
		uint8_t SplitMsg;
		uInt8NULL TotNoQuoteEntries;
		EnumBooleanNULL LiquidityFlag;
		EnumBooleanFlag PossRetransFlag;
		uInt64NULL DelayToTime;
	} __attribute__((packed));

	BlockData *blockData_{nullptr};
};
//
//// NoQuoteEntries   BlockLength=9 id=295
//struct GrpQuoteEntries
//{
//	inline static const std::size_t id{295};
//	inline static const std::size_t blockLength{9};
//
//	struct BlockData
//	{
//		uInt32 QuoteEntryID;
//		Int32 SecurityID;
//		uInt8 QuoteEntryRejectReason;
//	} __attribute__((packed));
//
//	BlockData *blockData_{nullptr};
//};

// NoQuoteSets   BlockLength=4 id=296
//struct GrpQuoteSets
//{
//	inline static const std::size_t id{296};
//	inline static const std::size_t blockLength{4};
//
//	struct BlockData
//	{
//		uInt16 QuoteSetID;
//		uInt16 QuoteErrorCode;
//	} __attribute__((packed));
//
//	BlockData *blockData_{nullptr};
//};



}}}
#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3MESSAGES2_HPP_ */
