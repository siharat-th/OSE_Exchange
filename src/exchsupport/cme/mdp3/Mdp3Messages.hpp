//============================================================================
// Name        	: Mdp3Messages.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Oct 25, 2022 8:18:17 PM
//============================================================================


#ifndef SRC_CME_MDP3MESSAGES_HPP_
#define SRC_CME_MDP3MESSAGES_HPP_

#include <stdio.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <map>
#include <inttypes.h>
#include <cstdint>
#include <bitset>

namespace KTN{
namespace Mdp3{

#define NULL64 9223372036854775807

typedef char Text[180];

inline static const int64_t PRICE_NULL = 9223372036854775807LL;
inline static const uint8_t UINT8_NULL = 255;
inline static const uint8_t BOOLEAN_NULL = 255;
inline static const uint8_t ENUM_NULL = 255;
inline static const uint8_t CHAR_NULL = 0;
inline static const uint16_t UINT16_NULL = 65535;
inline static const uint32_t UINT32_NULL = 4294967295;
inline static const int32_t INT32_NULL = 2147483647;
inline static const uint64_t UINT64_NULL = 18446744073709551615ULL;

enum EnumMDUpdateAction : uint8_t
{
	MdNew=0,
	MdChange,
	MdDelete,
	MdDeleteThru,
	MdDeleteFrom,
	MdOverlay
};
typedef uint8_t TypeMDUpdateAction;


enum EnumMDEntryType : char
{
    Bid = '0',
    Offer = '1',
    ImpliedBid = 'E',
    ImpliedOffer = 'F',
    BookReset = 'J',
    MarketBestOffer = 'w',
    MarketBestBid = 'x'
};
typedef char TypeMDEntryTypeBook;


enum EnumOrderUpdateAction
{
	OrdNew=0,
	OrdUpdate,
	OrdDelete
};
typedef uint8_t TypeOrderUpdateAction;

enum EnumMatchEventIndicator
{
	LastTradeMsg=0,
	LastVolumeMsg,
	LastQuoteMsg,
	LastStasMsg,
	LastImpliedMsg,
	RecoveryMsg,
	Reserved,
	EndOfEvent
};
typedef uint8_t TypeMatchEventIndicator;


enum EnumAggressorSide
{
	None = 0,
	Buy = 1,
	Sell = 2
};
typedef uint8_t TypeAggressorSide;


enum EnumSecurityTradingStatus : uint8_t {
    TradingHalt = 2,
    Close = 4,
    NewPriceIndication = 15,
    ReadyToTrade = 17,
    NotAvailableForTrading = 18,
    UnknownOrInvalid = 20,
    PreOpen = 21,
    PreCross = 24,
    Cross = 25,
    PostClose = 26,
    NoChange = 103,
    PrivateWorkup = 201,
    PublicWorkup = 202
};

enum EnumHaltReason : uint8_t {
    GroupSchedule = 0,
    SurveillanceIntervention = 1,
    MarketEvent = 2,
    InstrumentActivation = 3,
    InstrumentExpiration = 4,
    Unknown = 5,
    RecoveryInProcess = 6,
    TradeDateRoll = 7
};

enum EnumSecurityTradingEvent : uint8_t {
    NoEvent = 0,
    NoCancel = 1,
    ResetStatistics = 4,
    ImpliedMatchingON = 5,
    ImpliedMatchingOFF = 6,
    EndOfWorkup = 7
};

enum EnumOpenCloseSettleFlag : uint8_t{
	DailyOpenPrice = 0,
	IndicativeOpenPrice = 5,
	IntradayVWAP = 100,
	RepoAvg8_30AM = 101,
	RepoAvg10AM = 102,
	PrevSessionRepoAvg10AM=103
};

class MatchEventIndicatorBitsEnum {
public:
    enum Bits : uint8_t {
        LastTradeSummary = 0,    // Bit 0
        LastElectronicVolume,    // Bit 1
        LastCustomerOrderQuote,  // Bit 2
        LastStatistic,           // Bit 3
        LastImpliedQuote,        // Bit 4
        MessageResent,           // Bit 5
        Reserved,                // Bit 6
        LastMessage              // Bit 7
    };

    static bool isBitSet(uint8_t field, Bits bit) {
        return (field & (1 << static_cast<uint8_t>(bit))) != 0;
    }

    static std::bitset<8> getSetFields(uint8_t field) {
        std::bitset<8> setFields;
        setFields[static_cast<uint8_t>(LastTradeSummary)] = isBitSet(field, LastTradeSummary);
        setFields[static_cast<uint8_t>(LastElectronicVolume)] = isBitSet(field, LastElectronicVolume);
        setFields[static_cast<uint8_t>(LastCustomerOrderQuote)] = isBitSet(field, LastCustomerOrderQuote);
        setFields[static_cast<uint8_t>(LastStatistic)] = isBitSet(field, LastStatistic);
        setFields[static_cast<uint8_t>(LastImpliedQuote)] = isBitSet(field, LastImpliedQuote);
        setFields[static_cast<uint8_t>(MessageResent)] = isBitSet(field, MessageResent);
        setFields[static_cast<uint8_t>(Reserved)] = isBitSet(field, Reserved);
        setFields[static_cast<uint8_t>(LastMessage)] = isBitSet(field, LastMessage);
        return setFields;
    }

    static uint8_t getValue(Bits bit) {
        return (1 << static_cast<uint8_t>(bit));
    }
};

struct Decimal32{
	int32_t		mantissa;
	int8_t		exponent;
}__attribute__((packed));

struct Decimal64{
	int64_t		mantissa;
	int8_t		exponent;
}__attribute__((packed));


struct PRICE9{
	int64_t		mantissa;
	int8_t		exponent;
}__attribute__((packed));

//The exponent is defined as a constant of -9 for the type PRICENULL9 in the SBE schema
//and is not sent in the SBE message on the wire and is not part of the block length calculation.
struct PRICENULL9{
	int64_t		mantissa;
	//int8_t		exponent;
}__attribute__((packed));

//**********************************************************

struct MaturityMonthYear{
	uint16_t		year; //YYYY
	uint8_t		month; //MM
	uint8_t		day;//DD
	uint8_t		week; //WW
}__attribute__((packed));


//Repeating Group Dimensions
struct groupSize{
	uint16_t	blockLength;
	uint8_t		numInGroup;
}__attribute__((packed));

struct groupSize8Byte{
	uint16_t	blockLength;
	uint32_t	padding1;
	uint8_t		padding2;
	uint8_t		numInGroup;
}__attribute__((packed));

//Repeating Group Dimensions
struct groupSizeEncoding{
	uint16_t	blockLength;
	uint16_t	numInGroup;
}__attribute__((packed));


struct PacketHeader{
	uint32_t	MsgSeqNum;
	uint64_t	SendingTime;
}__attribute__((packed));

struct MsgHeader{
	uint16_t	MsgSize; // Length of entire message, including binary header in nbr of bytes
	//sbe header....
	uint16_t	blockLength;
	uint16_t	templateid;
	uint16_t	schemaId;
	uint16_t	version;
}__attribute__((packed));




struct MdOrderBookEntry46
{
	uint64_t OrderID;
	uint64_t MDOrderPriority;
	uint32_t MDDisplayQty;
	uint8_t ReferenceID;
	uint8_t OrderUpdateAction;
}__attribute__((packed));

struct AdminHeartbeat312
{
	MsgHeader hdr;
}__attribute__((packed));

struct AdminLogin315
{
	MsgHeader hdr;
	int8_t	HeartBtInt;
}__attribute__((packed));

struct AdminLogout316
{
	MsgHeader hdr;
	Text text;
}__attribute__((packed));


//Security Status 30 BlockLen = 30
struct SecurityStatus30
{
	uint64_t TransactTime;
	char SecurityGroup[6];
	char Asset[6];
	int32_t SecurityID;
	uint16_t LocalMktDate;
	uint8_t MatchEventIndicator;
	EnumSecurityTradingStatus SecurityTradingStatus;
	EnumHaltReason HaltReason;
	EnumSecurityTradingEvent SecurityTradingEvent;
}__attribute__((packed));

//length= 30
struct SecurityStatus30Fast
{
	uint64_t TransactTime;
	char SecurityGroup[6];
	char Asset[6];
	int32_t SecurityID;
	uint16_t LocalMktDate;
	uint8_t MatchEventIndicator;
	uint8_t SecurityTradingStatus;
	uint8_t HaltReason;
	uint8_t SecurityTradingEvent;
}__attribute__((packed));

//Includes Orders for MBO...
//Padding is determined by block size - sum(offsets):
//https://www.cmegroup.com/confluence/display/EPICSANDBOX/MDP+3.0+-+SBE+Decoding+Example
struct MDIncrementalRefreshBook46
{
	uint64_t TransactTime;
	uint8_t MatchEventIndicator;
	uint16_t padding;
}__attribute__((packed));

struct MDIncrementalRefreshBook46Entry
{
	PRICENULL9 MDEntryPx;
	int32_t MDEntrySize;
	int32_t SecurityID;
	uint32_t RptSeq;
	int32_t NumberOfOrders;
	uint8_t MDPriceLevel;
	EnumMDUpdateAction MDUpdateAction;
	EnumMDEntryType MDEntryType;
	uint32_t TradeableSize;
	uint8_t padding;
}__attribute__((packed));

struct MDIncrementalRefreshBook46OrderDetails
{
	uint64_t OrderID ;
	uint64_t MDOrderPriority;
	uint32_t MDDisplayQty;
	uint8_t ReferenceID;
	uint8_t MDUpdateAction;
	uint16_t Padding;
}__attribute__((packed));


//MBO..............
struct MDIncrementalRefreshBook47
{
	//MsgHeader hdr;
	uint64_t TransactTime;
	uint8_t MatchEventIndicator;
	uint16_t Padding;
	//MdEntries follw
	//groupSize NoMdEntries;
}__attribute__((packed));

struct MDIncrementalRefreshBook47Entry
{
	uint64_t OrderID ;
	uint64_t MDOrderPriority;
	PRICENULL9 MDEntryPx;
	uint32_t MDDisplayQty;
	int32_t SecurityID;
	uint8_t MDUpdateAction;
	char MDEntryType;
}__attribute__((packed));

//Trade48


struct MDTradeEntry
{
	PRICENULL9 MDEntryPx;
	//uint8_t Padding;
	int32_t MDEntrySize;
	int32_t SecurityID;
	uint32_t RptSeq;
	int32_t NumberOfOrders;
	uint8_t AggressorSide;
	uint8_t MDUpdateAction;
	char MDEntryType;
	uint32_t MDTradeEntryID;
	uint8_t Padding;
}__attribute__((packed));

struct MDTradeEntryOrderDetails
{
	uint64_t OrderID;
	int32_t LastQty;
}__attribute__((packed));

struct MDIncrementalRefreshTradeSummary48
{
	uint64_t TransactTime;
	uint8_t MatchEventIndicator;
	uint16_t Padding;
}__attribute__((packed));


//Volume 37
struct MDIncrementalRefreshVolume37
{
	uint64_t TransactTime;
	uint8_t MatchEventIndicator;
	uint16_t Padding;
}__attribute__((packed));

//block len = 16
struct MDVolumeEntry
{
	int32_t MDEntrySize;
	int32_t SecurityID;
	uint32_t RptSeq;
	uint8_t MDUpdateAction;
	//char MDEntryType;
	uint8_t constantEntryType;
	uint16_t Padding2;
}__attribute__((packed));




//blocklen = 59
struct SnapshotFullRefresh52
{
	uint32_t LastMsgSeqNumProcessed;
	uint32_t TotNumReports;
	int32_t SecurityID;
	uint32_t RptSeq;
	uint64_t TransactTime;
	uint64_t LastUpdateTime;
	uint16_t TradeDate;
	EnumSecurityTradingStatus SecurityTradingStatus;
	PRICENULL9 HighLimitPrice;
	PRICENULL9 LowLimitPrice;
	PRICENULL9 MaxPriceVariation; //banding
}__attribute__((packed));

//blocklen = 22
struct SnapshotFullRefreshEntry
{
	PRICENULL9 MDEntryPx;
	int32_t MDEntrySize;
	int32_t NumberOfOrders;
	int8_t MDPriceLevel;
	uint16_t TradeReferenceDate;
	EnumOpenCloseSettleFlag OpenCloseSettleFlag;
	uint8_t SettlePriceType;
	EnumMDEntryType MDEntryType;
}__attribute__((packed));


//blocklen = 28
struct SnapshotFullRefreshOrderBook53
{
	uint32_t LastMsgSeqNumProcessed;
	uint32_t TotNumReports;
	int32_t SecurityID;
	uint32_t NoChunks;
	uint32_t CurrentChunk;
	uint64_t TransactTime;
}__attribute__((packed));

//blocklen = 29
struct SnapshotFullRefreshOrderBookEntry
{
	uint64_t OrderID ;
	uint64_t MDOrderPriority;
	PRICENULL9 MDEntryPx;
	uint32_t MDDisplayQty;
	char MDEntryType;
}__attribute__((packed));


///SnapshotRefreshTopOrders59
//blocklen = 13
struct SnapshotRefreshTopOrders59
{
	uint64_t TransactTime;
	uint8_t MatchEventIndicator;
	int32_t SecurityID;
}__attribute__((packed));

//blocklen = 29
struct SnapshotRefreshTopOrdersEntry
{
	uint64_t OrderID ;
	uint64_t MDOrderPriority;
	PRICENULL9 MDEntryPx;
	uint32_t MDDisplayQty;
	char MDEntryType;
}__attribute__((packed));

/**************  END **************/
}
}


#endif /* SRC_CME_MDP3MESSAGES_HPP_ */
