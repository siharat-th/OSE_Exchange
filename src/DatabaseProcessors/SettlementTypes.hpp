//============================================================================
// Name        : SettlementTypes.hpp
// Description : DailyStat struct for settlement DB writing (matches MarketData schema)
// Copyright   : Copyright (C) 2026 Katana Financial
//============================================================================

#ifndef SRC_DATABASEPROCESSORS_SETTLEMENTTYPES_HPP_
#define SRC_DATABASEPROCESSORS_SETTLEMENTTYPES_HPP_

#include <string>
#include <cstdint>

struct DailyStat
{
	uint64_t TransactTime = 0;
	int32_t SecurityID = 0;
	std::string Symbol;
	double Price = 0.0;
	char UpdateType = '6';         // FIX MDEntryType '6' = SettlementPrice
	std::string EntryType;
	int32_t Size = 0;
	uint32_t RptSeq = 0;
	uint32_t TradeDate = 0;        // days since Unix epoch
};

#endif /* SRC_DATABASEPROCESSORS_SETTLEMENTTYPES_HPP_ */
