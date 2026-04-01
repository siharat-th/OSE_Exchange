//============================================================================
// Name        	: SettlementCache.hpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
// Description 	: Thread-safe cache for settlement prices (EB10/RQ62)
//============================================================================

#ifndef SRC_EXCHSUPPORT_OSE_SETTLEMENTCACHE_HPP_
#define SRC_EXCHSUPPORT_OSE_SETTLEMENTCACHE_HPP_

#pragma once

#include <mutex>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <string>
#include <omex_ose.h>
#include <KT01/Core/Log.hpp>
#include <KT01/SecDefs/OseSecMaster.hpp>
#include <Orders/OrderEnumsV2.hpp>

namespace KTN::OSE {

struct SettlementData
{
	series_t series;
	char name[32];               // symbol name (from DQ3 cache, if available)
	int32_t settle_price;        // raw (÷10000 for display)
	int32_t last_price;
	int32_t theo_price;
	int32_t implied_vol;
	int32_t actual_vol;
	uint8_t settlement_price_type;
	std::chrono::steady_clock::time_point received_at;
};

// settlement_price_type_c → readable string
inline const char* SettlementPriceTypeName(uint8_t t)
{
	switch (t)
	{
	case 1:  return "All";
	case 2:  return "Normal";
	case 7:  return "Emergency";
	default: return "Unknown";
	}
}

class SettlementCache
{
public:
	// OMnet "no value" sentinel for int32_t fields
	static constexpr int32_t NO_VALUE = (int32_t)0x80000000;  // INT32_MIN

	static std::string PriceStr(int32_t raw)
	{
		if (raw == NO_VALUE) return "N/A";
		return std::to_string(raw);
	}

	// Resolve symbol name from series_t (host byte order fields)
	static std::string ResolveSymbol(uint8_t country, uint8_t market, uint8_t ig,
	                                  uint8_t modifier, uint16_t commodity, uint16_t expiry)
	{
		uint64_t seriesKey = ((uint64_t)country << 56) |
		                     ((uint64_t)market << 48) |
		                     ((uint64_t)ig << 40) |
		                     ((uint64_t)modifier << 32) |
		                     ((uint64_t)commodity << 16) |
		                     (uint64_t)expiry;
		auto& sm = KT01::SECDEF::OSE::OseSecMaster::instance();
		uint32_t obid = sm.GetOrderbookIdBySeriesKey(seriesKey);
		if (obid != 0 && sm.Contains(obid))
			return sm.getSymbol(obid);
		return "";
	}

	// Key: commodity_n << 16 | expiration_date_n
	static uint32_t MakeKey(uint16_t commodity, uint16_t expiration)
	{
		return (static_cast<uint32_t>(commodity) << 16) | expiration;
	}

	static uint32_t MakeKey(const series_t& s)
	{
		return MakeKey(s.commodity_n, s.expiration_date_n);
	}

	void Update(uint32_t key, const SettlementData& d)
	{
		std::lock_guard<std::mutex> lk(_mtx);
		_data[key] = d;
	}

	bool Get(uint32_t key, SettlementData& out) const
	{
		std::lock_guard<std::mutex> lk(_mtx);
		auto it = _data.find(key);
		if (it == _data.end()) return false;
		out = it->second;
		return true;
	}

	std::vector<SettlementData> GetAll() const
	{
		std::lock_guard<std::mutex> lk(_mtx);
		std::vector<SettlementData> result;
		result.reserve(_data.size());
		for (auto& [k, v] : _data)
			result.push_back(v);
		return result;
	}

	size_t Size() const
	{
		std::lock_guard<std::mutex> lk(_mtx);
		return _data.size();
	}

	void Clear()
	{
		std::lock_guard<std::mutex> lk(_mtx);
		_data.clear();
	}

	void Print() const
	{
		std::lock_guard<std::mutex> lk(_mtx);
		int named = 0;
		for (auto& [key, d] : _data)
		{
			if (d.name[0] == 0) continue;

			// Skip options
			uint16_t commodity = key >> 16;
			uint16_t expiry = key & 0xFFFF;
			std::string resolvedSym = ResolveSymbol(
				d.series.country_c, d.series.market_c,
				d.series.instrument_group_c, d.series.modifier_c,
				commodity, expiry);
			if (resolvedSym.empty()) continue;
			auto& sm = KT01::SECDEF::OSE::OseSecMaster::instance();
			uint32_t obid = sm.GetOrderbookIdBySeriesKey(
				((uint64_t)d.series.country_c << 56) |
				((uint64_t)d.series.market_c << 48) |
				((uint64_t)d.series.instrument_group_c << 40) |
				((uint64_t)d.series.modifier_c << 32) |
				((uint64_t)commodity << 16) | (uint64_t)expiry);
			if (obid != 0 && sm.Contains(obid) &&
			    sm.getSecDef(obid).prodtype == KTN::ORD::KOrderProdType::OPTION)
				continue;

			char symName[33] = {};
			memcpy(symName, d.name, 32);
			for (int i = 31; i >= 0 && (symName[i] == ' ' || symName[i] == 0); --i)
				symName[i] = 0;

			std::string settleStr = (d.settle_price == NO_VALUE) ? "N/A" :
				(std::to_string(d.settle_price / 10000) + "." + std::to_string(d.settle_price % 10000));
			std::string lastStr = (d.last_price == NO_VALUE) ? "N/A" :
				(std::to_string(d.last_price / 10000) + "." + std::to_string(d.last_price % 10000));

			KT01_LOG_INFO("SettlementCache",
				"  " + std::string(symName) +
				" settle=" + settleStr +
				" last=" + lastStr +
				" type=" + std::string(SettlementPriceTypeName(d.settlement_price_type)));
			named++;
		}
		KT01_LOG_INFO("SettlementCache", "Settlement cache: " + std::to_string(named) +
		              " known / " + std::to_string(_data.size()) + " total entries");
	}

private:
	mutable std::mutex _mtx;
	std::unordered_map<uint32_t, SettlementData> _data;
};

} // namespace KTN::OSE

#endif /* SRC_EXCHSUPPORT_OSE_SETTLEMENTCACHE_HPP_ */
