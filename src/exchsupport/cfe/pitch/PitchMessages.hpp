/*
 * PitchMessages.hpp
 *
 *  Created on: Dec 8, 2017
 *      Author: sgaer
 */

#ifndef BATSMKTDATA_HPP_
#define BATSMKTDATA_HPP_

#include <stdio.h>
# include <sys/types.h>
# include <iostream>
# include <vector>
# include <map>


namespace KTN{
namespace CFE{
namespace Pitch {

struct SequencedUnitHeader{
	uint16_t 	HdrLength;
	uint8_t 	HdrCount;
	uint8_t 	HdrUnit;
	uint32_t	HdrSequence;
}__attribute__((packed));

struct MsgHeader{
	uint8_t		Length;
	uint8_t		MsgType;
}__attribute__((packed));

struct TimeMessage{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	Time;
	uint32_t	EpochTime;
}__attribute__((packed));

struct UnitClearMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
}__attribute__((packed));

struct TimeReferenceMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	MidnightReference;
	uint32_t	Time;
	uint32_t	TimeOffset;
	uint32_t	TradeDate;
}__attribute__((packed));

//blocklen = 45 + sizeof(FuturesLeg) * LegCount (0 for outrights)
struct FuturesInstrumentDefinitionMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	uint32_t	UnitTimestamp;
	char		ReportSymbol[6];
	uint8_t		FuturesFlags; //1 = Variance ; 0 = Std uint8_t
	uint32_t	ExpirationDate;
	uint16_t	ContractSize;
	char		ListingState;
	int64_t	PriceIncrement;
	uint8_t		LegCount;
	uint8_t		LegOffset;
	uint8_t		Reserved;
	uint32_t	ContractDate;
}__attribute__((packed));

struct FuturesLeg
{
	int32_t LegRatio;
	char LegSymbol[6];
}__attribute__((packed));

struct FuturesVarianceSymbolMapping
{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	uint32_t	UnitTimestamp;
	char		FeedSymbol[6];
	char 		FuturesSymbol[12];
	int64_t     AccruedDayVariance;
	uint16_t    NumFinalReturns;
	uint16_t    NumElapsedReturns;
}__attribute__((packed));


struct PriceLimitsMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	uint64_t	UpperPriceLimit;
	uint64_t	LowerPriceLimit;
}__attribute__((packed));



//Len = 20 bytes
struct TwoSideUpdateMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	int16_t		BidPrice;
	uint16_t	BidQuantity;
	int16_t		AskPrice;
	uint16_t	AskQuantity;
}__attribute__((packed));

//Len = 37 bytes
struct TOPTradeMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	uint32_t	Quantity;
	int64_t		Price;
	uint64_t	ExecutionId;
	uint32_t	TotalVolume;
	char		TradeCondition;
}__attribute__((packed));

//Len = 25 Bytes
struct SettlementMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	uint32_t	TradeDate;
	int64_t		SettlementPrice;
	char		Issue;
}__attribute__((packed));

//Len = 56 bytes
struct EndOfDaySummaryMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	uint32_t	TradeDate;
	uint32_t	OpenInterest;
	int64_t		HighPrice;
	int64_t		LowPrice;
	int64_t		OpenPrice;
	int64_t		ClosePrice;
	uint32_t	TotalVolume;
}__attribute__((packed));

struct OpenInterestMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	uint32_t	TradeDate;
	uint32_t	OpenInterest;
};


//Len = 6 bytes
struct EndSessionMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	Timestamp;
}__attribute__((packed));

//Start Level 2/PITCH -- change namespace def?

//Len = 18 bytes
struct TradingStatusMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	char		Reserved1Stat[2];
	char		StatusInd;
	char		Reserved2Stat[3];
}__attribute__((packed));



//Len = 25 bytes
struct AddOrderShortMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	uint64_t	OrderId;
	char		Side;
	uint16_t	Quantity;
	char		Symbol[6];
//	char		Symbol[6];
	int16_t		Price;
}__attribute__((packed));


//Len = 27 bytes
struct OrderExecutedMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	uint64_t	OrderId;
	uint32_t	ExecQuantity;
	uint64_t	ExecId;
	char		TradeCondition;
}__attribute__((packed));


//Len = 39 bytes
struct OrderExecutedAtPriceSizeMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	uint64_t	OrderId;
	uint32_t	ExecQuantity;
	uint32_t	LeavesQuantity;
	uint64_t	ExecId;
	int64_t		Price;
	char			TradeCondition;
}__attribute__((packed));

//Len = 16 bytes (SHORT version)
struct ReduceSizeShortMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	uint64_t	OrderId;
	uint16_t	CxlQuantity;
}__attribute__((packed));

//Len = 18 bytes (SHORT version)
struct ModifyOrderShortMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	uint64_t	OrderId;
	uint16_t	Quantity;
	int16_t		Price;
}__attribute__((packed));

//Len = 14 bytes
struct DeleteOrderMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	uint64_t	OrderId;
}__attribute__((packed));

//Len = 34 bytes
struct TradeShortMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	uint64_t	OrderId;
	char		Side;
	uint16_t	Quantity;
	char		Symbol[6];
	int16_t		Price;
	uint64_t	ExecutionId;
	char		TradeCondition;
}__attribute__((packed));

//Len = 42 bytes (LONG version)
struct TradeLongMsg {
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	uint64_t	OrderId;
	char		Side;
	uint32_t	Quantity;      // Changed from uint16_t to uint32_t
	char		Symbol[6];
	int64_t		Price;         // Changed from int16_t to int64_t
	uint64_t	ExecutionId;
	char		TradeCondition;
}__attribute__((packed));

//Len = 6 bytes
struct TransactionBeginMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
}__attribute__((packed));

//Len = 6 bytes
struct TransactionEndMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
}__attribute__((packed));

//Len = 14 bytes
struct TradeBreakMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	uint64_t	ExecutionId;
}__attribute__((packed));


//top
//Len = 17 bytes
struct SingleSideUpdateShortMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	char		Side;
	int16_t		Price;
	uint16_t	Quantity;
}__attribute__((packed));

//Len = 20 bytes
struct TwoSideUpdateShortMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	int16_t		BidPrice;
	uint16_t	BidQuantity;
	int16_t		AskPrice;
	uint16_t	AskQuantity;
}__attribute__((packed));

//len = 37 bytes?
struct MarketSnapshotShortMsg{
	uint8_t		Length;
	uint8_t		MsgType;
	uint32_t	TimeOffset;
	char		Symbol[6];
	uint32_t 	UnitTimestamp;
	int16_t		BidPrice;
	uint16_t	BidQuantity;
	int16_t		AskPrice;
	uint16_t	AskQuantity;
	int16_t		LastPrice;
	uint16_t	LastQuantity;
	char		LastTradeCondition;
	uint32_t	TotalVolume;
	char		TradingStatus;
	char		Reserved[3];
}__attribute__((packed));


//end of namespaces
} // namespace Pitch
} // namespace CFE
} // namespace KTN

//---------------------------------

#endif /* BATSMKTDATA_HPP_ */
