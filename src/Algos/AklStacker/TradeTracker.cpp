#include "TradeTracker.hpp"
#include <akl/Book.hpp>

namespace akl::stacker
{

TradeTracker::TradeTracker()
{

}

void TradeTracker::SetTargets(const std::optional<ProrataTarget> &bidTarget, const std::optional<ProrataTarget> &askTarget)
{
	targets[0] = bidTarget;
	targets[1] = askTarget;
}

void TradeTracker::HandleTrade(const Book &book, const Timestamp &now, const KTN::ORD::KOrderSide::Enum aggressingSide, const Quantity tradeQty, const Price tradePx)
{
	const Duration bidWindow = targets[0] ? targets[0]->window : Duration(0);
	tradesOnBid.ClearOldTrades(now, bidWindow);

	const Duration askWindow = targets[1] ? targets[1]->window : Duration(0);
	tradesOnAsk.ClearOldTrades(now, askWindow);

	Quantity totalTradeQty[2] { Quantity(0), Quantity(0) };


	if (book.HasUnadjustedBid() && tradePx <= book.UnadjustedBestBid())
	{
		if (targets[0] && tradePx == targets[0]->price)
		{
			totalTradeQty[0] += tradeQty;
		}
	}
	else if (book.HasUnadjustedAsk() && tradePx >= book.UnadjustedBestAsk())
	{
		if (targets[1] && tradePx == targets[1]->price)
		{
			totalTradeQty[1] += tradeQty;
		}
	}


	if (targets[0] && totalTradeQty[0] > Quantity(0))
	{
		tradesOnBid.AddTrade(now, totalTradeQty[0], targets[0]->price);
	}
	if (targets[1] && totalTradeQty[1] > Quantity(0))
	{
		tradesOnAsk.AddTrade(now, totalTradeQty[1], targets[1]->price);
	}
}

}