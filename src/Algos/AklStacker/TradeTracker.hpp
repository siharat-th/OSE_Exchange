#pragma once

#include "TradeSideTracker.hpp"
#include "pricing/ProrataTarget.hpp"
#include "Time.hpp"

#include <Orders/OrderEnumsV2.hpp>

namespace akl
{

class Book;

}

namespace akl::stacker
{

class TradeTracker
{
public:
	TradeTracker();

	void SetTargets(const std::optional<ProrataTarget> &bidTarget, const std::optional<ProrataTarget> &askTarget);
	void HandleTrade(const Book &book, const Timestamp &now, const KTN::ORD::KOrderSide::Enum aggressingSide, const Quantity tradeQty, const Price tradePx);

	inline Quantity GetBidTrades() const { return tradesOnBid.GetTotalTradeQuantity(); }
	std::optional<Price> GetBidTradePrice() const { return tradesOnBid.GetTradePrice(); }
	inline Quantity GetAskTrades() const { return tradesOnAsk.GetTotalTradeQuantity(); }
	std::optional<Price> GetAskTradePrice() const { return tradesOnAsk.GetTradePrice(); }

private:
	TradeTracker(const TradeTracker &) = delete;
	const TradeTracker &operator=(const TradeTracker &) = delete;
	TradeTracker(const TradeTracker &&) = delete;
	const TradeTracker &operator=(const TradeTracker &&) = delete;

	TradeSideTracker tradesOnBid;
	TradeSideTracker tradesOnAsk;

	std::optional<ProrataTarget> targets[2] { std::nullopt, std::nullopt };
};

}