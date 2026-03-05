#pragma once

#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include "Time.hpp"

#include <deque>
#include <optional>

namespace akl::stacker
{

struct TradeInfo
{
	TradeInfo(const Quantity qty, const Timestamp timestamp)
	: quantity(qty)
	, timestamp(timestamp)
	{

	}

	const Quantity quantity;
	const Timestamp timestamp;
};

class TradeSideTracker
{
public:
	using TradeInfoListType = std::deque<TradeInfo>;

	TradeSideTracker();

	void ClearOldTrades(const Timestamp &now, const Duration &window);
	void AddTrade(const Timestamp &now, const Quantity qty, const Price px);

	inline std::optional<Price> GetTradePrice() const { return lastPx; }
	Quantity GetTotalTradeQuantity() const;

private:
	TradeSideTracker(const TradeSideTracker &) = delete;
	const TradeSideTracker &operator=(const TradeSideTracker &) = delete;
	TradeSideTracker(const TradeSideTracker &&) = delete;
	const TradeSideTracker &operator=(const TradeSideTracker &&) = delete;

	TradeInfoListType trades;
	std::optional<Price> lastPx { std::nullopt };
};



}