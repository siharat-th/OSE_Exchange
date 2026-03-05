#include "TradeSideTracker.hpp"

namespace akl::stacker
{

TradeSideTracker::TradeSideTracker()
{

}

void TradeSideTracker::ClearOldTrades(const Timestamp &now, const Duration &window)
{
	while (!trades.empty() && now > trades.front().timestamp && now - trades.front().timestamp >= window)
	{
		trades.pop_front();
	}
}

void TradeSideTracker::AddTrade(const Timestamp &now, const Quantity qty, const Price px)
{
	if (lastPx && px != lastPx)
	{
		trades.clear();
	}
	lastPx = px;
	trades.push_back(TradeInfo(qty, now));
}

Quantity TradeSideTracker::GetTotalTradeQuantity() const
{
	Quantity sum { 0 };
	for (TradeInfoListType::const_iterator it = trades.begin(); it != trades.end(); ++it)
	{
		sum += it->quantity;
	}
	return sum;
}

}