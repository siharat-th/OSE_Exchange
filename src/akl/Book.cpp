#include "Book.hpp"
#include <KT01/Core/Macro.hpp>
#include <KT01/Core/Log.hpp>
#include <akl/BranchPrediction.hpp>

namespace akl
{

Book::Book()
{

}

void Book::SetSymbol(const std::string &symbol)
{
	this->symbol = symbol;
}

void Book::Process(const KT01::DataStructures::MarketDepth::BookSecurityStatus secStatus, const KT01::DataStructures::MarketDepth::BookSecurityTradingEvent event, const uint64_t recvTime)
{
	if (secStatus != KT01::DataStructures::MarketDepth::BookSecurityStatus::NoChange) // TODO double check this?
	{
		this->secStatus = secStatus;
	}
	tradingEvent = event;
	this->recvTime = recvTime;
}

void Book::Process(const KT01::DataStructures::MarketDepth::DepthBook &snapshot)
{
	processSide(snapshot.Bids, bids, bidCount, 0);
	processSide(snapshot.Asks, asks, askCount, 1);
	isLtpAdjusted = false;
	depthSeqNum = snapshot.RptSeq;
	recvTime = snapshot.recvTime;

	if (unlikely(!snapshotStatusProcessed && secStatus == KT01::DataStructures::MarketDepth::BookSecurityStatus::UnknownOrInvalid))
	{
		if (unlikely(snapshot.SecStatus == KT01::DataStructures::MarketDepth::BookSecurityStatus::UnknownOrInvalid))
		{
			if (logMarketStateWarning)
			{
				KT01_LOG_WARN(__CLASS__, "Book::Process: received market data update with market state unknown for {}", symbol);
				logMarketStateWarning = false;
			}
		}
		else
		{
			snapshotStatusProcessed = true;
			secStatus = static_cast<KT01::DataStructures::MarketDepth::BookSecurityStatus>(snapshot.SecStatus);
			KT01_LOG_INFO(__CLASS__, "Book::Process: market state changed to {} for {}", static_cast<uint16_t>(secStatus), symbol);
		}
	}
}

void Book::Process(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px, const uint32_t rptseq, const uint64_t recvTime)
{
	switch (side)
	{
	case KTN::ORD::KOrderSide::Enum::BUY:
		if (askCount > 0 && px >= asks[0].price)
		{
			processLtp<KTN::ORD::KOrderSide::Enum::SELL>(qty, px, asks, askCount);
		}
		break;
	case KTN::ORD::KOrderSide::Enum::SELL:
		if (bidCount > 0 && px <= bids[0].price)
		{
			processLtp<KTN::ORD::KOrderSide::Enum::BUY>(qty, px, bids, bidCount);
		}	
		break;
	default:
		break;
	}
	depthSeqNum = rptseq;
	this->recvTime = recvTime;
}

void Book::CleanCross()
{
	size_t bidIndex = 0;
	size_t askIndex = 0;

	while (bidIndex < bidCount && askIndex < askCount && bids[bidIndex].price >= asks[askIndex].price)
	{
		const Quantity q = std::min(bids[bidIndex].quantity, asks[askIndex].quantity);
		bids[bidIndex].quantity -= q;
		asks[askIndex].quantity -= q;
		if (bids[bidIndex].quantity <= Quantity(0))
		{
			++bidIndex;
		}
		if (asks[askIndex].quantity <= Quantity(0))
		{
			++askIndex;
		}
	}

	if (bidIndex >= 0)
	{
		cleanUpEmptyLevels(bids, bidCount, bidIndex, 0);
	}
	if (askIndex >= 0)
	{
		cleanUpEmptyLevels(asks, askCount, askIndex, 1);
	}
}

bool Book::ValidateBook() const
{
	if (likely(TickIncrement > 0))
	{
		if (likely(bidCount > 0))
		{
			for (size_t i = 1; i < bidCount; ++i)
			{
				if (unlikely(bids[i - 1].price - TickIncrement != bids[i].price))
				{
					return false;
				}
			}
		}
		if (likely(askCount > 0))
		{
			for (size_t i = 1; i < askCount; ++i)
			{
				if (unlikely(asks[i - 1].price + TickIncrement != asks[i].price))
				{
					return false;
				}
			}
		}
		if (likely(bidCount > 0 && askCount > 0))
		{
			if (unlikely(bids[0].price >= asks[0].price))
			{
				return false;
			}
		}
	}
	return true;
}

std::optional<Price> Book::CalculateBidLean(const Quantity qty) const
{
	return calculateLean(qty, bids, bidCount, 0);
}

std::optional<Price> Book::CalculateAskLean(const Quantity qty) const
{
	return calculateLean(qty, asks, askCount, 1);
}

template<>
size_t Book::LevelCount<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return BidCount();
}

template<>
size_t Book::LevelCount<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return AskCount();
}

template<>
const Level &Book::GetLevel<KTN::ORD::KOrderSide::Enum::BUY>(int index) const
{
	return GetBidLevel(index);
}

template<>
const Level &Book::GetLevel<KTN::ORD::KOrderSide::Enum::SELL>(int index) const
{
	return GetAskLevel(index);
}

std::optional<Level> Book::TotalBidLean() const
{
	if (bidCount > 0)
	{
		Level sum = { Quantity(0), bids[bidCount - 1].price };
		for (size_t i = 0; i < bidCount; ++i)
		{
			sum.quantity += bids[i].quantity;
		}
		return sum;
	}
	else
	{
		return std::nullopt;
	}
}

std::optional<Level> Book::TotalAskLean() const
{
	if (askCount > 0)
	{
		Level sum = { Quantity(0), asks[askCount - 1].price };
		for (size_t i = 0; i < askCount; ++i)
		{
			sum.quantity += asks[i].quantity;
		}
		return sum;
	}
	else
	{
		return std::nullopt;
	}
}

void Book::LogDepth() const
{
	LOGINFO("{} Ask | {}@{} | {}@{} | {}@{} | {}@{} | {}@{} | {}@{} | {}@{} | {}@{} | {}@{} | {}@{}",
		symbol.c_str(),
		asks[0].quantity.value(), asks[0].price.AsShifted(),
		asks[1].quantity.value(), asks[1].price.AsShifted(),
		asks[2].quantity.value(), asks[2].price.AsShifted(),
		asks[3].quantity.value(), asks[3].price.AsShifted(),
		asks[4].quantity.value(), asks[4].price.AsShifted(),
		asks[5].quantity.value(), asks[5].price.AsShifted(),
		asks[6].quantity.value(), asks[6].price.AsShifted(),
		asks[7].quantity.value(), asks[7].price.AsShifted(),
		asks[8].quantity.value(), asks[8].price.AsShifted(),
		asks[9].quantity.value(), asks[9].price.AsShifted());
	LOGINFO("{} Bid | {}@{} | {}@{} | {}@{} | {}@{} | {}@{} | {}@{} | {}@{} | {}@{} | {}@{} | {}@{}",
		symbol.c_str(),
		bids[0].quantity.value(), bids[0].price.AsShifted(),
		bids[1].quantity.value(), bids[1].price.AsShifted(),
		bids[2].quantity.value(), bids[2].price.AsShifted(),
		bids[3].quantity.value(), bids[3].price.AsShifted(),
		bids[4].quantity.value(), bids[4].price.AsShifted(),
		bids[5].quantity.value(), bids[5].price.AsShifted(),
		bids[6].quantity.value(), bids[6].price.AsShifted(),
		bids[7].quantity.value(), bids[7].price.AsShifted(),
		bids[8].quantity.value(), bids[8].price.AsShifted(),
		bids[9].quantity.value(), bids[9].price.AsShifted());
	LOGINFO("{} book | seqNum={} | recvTime={} | ltpAdjust={}", symbol.c_str(), depthSeqNum, recvTime, static_cast<int>(isLtpAdjusted));
}

void Book::LogTob() const
{
	LOGINFO("{} BBO: [ {}@%{} | {}@{} ] Unadjusted: [ {}@%{} | {}@%{} ] | seq={}",
		symbol,
		BestBidQuantity().value(),
		HasBid() ? BestBid().AsShifted() : Price::Min().AsShifted(),
		BestAskQuantity().value(),
		HasAsk() ? BestAsk().AsShifted() : Price::Max().AsShifted(),
		UnadjustedBestBidQuantity().value(),
		HasUnadjustedBid() ? UnadjustedBestBid().AsShifted() : Price::Min().AsShifted(),
		UnadjustedBestAskQuantity().value(),
		HasUnadjustedAsk() ? UnadjustedBestAsk().AsShifted() : Price::Max().AsShifted(),
		depthSeqNum);
}

template<>
bool Book::HasSide<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return HasBid();
}

template<>
bool Book::HasSide<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return HasAsk();
}

template<>
Price Book::BestPrice<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return BestBid();
}

template<>
Price Book::BestPrice<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return BestAsk();
}

template<>
Quantity Book::BestQuantity<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return BestBidQuantity();
}

template<>
Quantity Book::BestQuantity<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return BestAskQuantity();
}

template<>
bool Book::HasPreviousSide<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return HasPreviousBid();
}

template<>
bool Book::HasPreviousSide<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return HasPreviousAsk();
}

template<>
Price Book::PreviousBestPrice<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return PreviousBestBid();
}

template<>
Price Book::PreviousBestPrice<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return PreviousBestAsk();
}

template<>
Quantity Book::PreviousBestQuantity<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return PreviousBestBidQuantity();
}

template<>
Quantity Book::PreviousBestQuantity<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return PreviousBestAskQuantity();
}

void Book::UpdatePrevious()
{
	prevBestPx[0] = bestPx[0];
	prevBestPx[1] = bestPx[1];
	prevBestQty[0] = bestQty[0];
	prevBestQty[1] = bestQty[1];
}

template<>
std::optional<Price> Book::CalculateLean<KTN::ORD::KOrderSide::Enum::BUY>(const Quantity qty) const
{
	return CalculateBidLean(qty);
}

template<>
std::optional<Price> Book::CalculateLean<KTN::ORD::KOrderSide::Enum::SELL>(const Quantity qty) const
{
	return CalculateAskLean(qty);
}

bool Book::TobPriceChanged() const
{
	return (HasBid() && (!HasPreviousBid() || BestBid() != PreviousBestBid()))
		|| (HasAsk() && (!HasPreviousAsk() || BestAsk() != PreviousBestAsk()));
}

bool Book::IsBookGapped() const
{
	return TickIncrement <= Price::FromShifted(0)
		|| !HasBid()
		|| !HasAsk()
		|| BestAsk() - BestBid() != TickIncrement;
}

void Book::processSide(const std::vector<KT01::DataStructures::MarketDepth::pxl> &sideDepth, Level *levels, size_t &count, const int index)
{
	size_t i = 0;
	count = 0;
	for (; i < sideDepth.size() && i < MAX_LEVELS; ++i)
	{
		const KT01::DataStructures::MarketDepth::pxl &level = sideDepth[i];
		if (level.size > 0)
		{
			levels[i] = { Quantity(level.size), level.px };
			++count;
		}
		else
		{
			levels[i] = { Quantity(0), Price::FromShifted(0) };
			break;
		}
	}
	for (; i < MAX_LEVELS; ++i)
	{
		levels[i] = { Quantity(0), Price::FromShifted(0) };
	}

	if (likely(count > 0))
	{
		bestPx[index] = levels[0].price;
		unadjustedBestPx[index] = levels[0].price;
		bestQty[index] = levels[0].quantity;
		unadjustedBestQty[index] = levels[0].quantity;
	}
	else
	{
		bestPx[index].reset();
		unadjustedBestPx[index].reset();
		bestQty[index] = Quantity(0);
		unadjustedBestQty[index] = Quantity(0);
	}
}

std::optional<Price> Book::calculateLean(const Quantity qty, const Level *levels, const size_t count, const int index) const
{
	Quantity sum = Quantity(0);
	for (size_t i = 0; i < count; ++i)
	{
		sum += levels[i].quantity;
		if (sum >= qty)
		{
			return levels[i].price;
		}
	}
	return std::nullopt; // TODO what to do if lean not met?
}

void Book::cleanUpEmptyLevels(Level *levels, size_t &count, const size_t firstIndex, const int sideIndex)
{
	if (firstIndex == 0)
	{
		bestPx[sideIndex] = levels[0].price;
		bestQty[sideIndex] = levels[0].quantity;
		unadjustedBestPx[sideIndex] = bestPx[sideIndex];
		unadjustedBestQty[sideIndex] = bestQty[sideIndex];
	}
	else if (firstIndex < count)
	{
		size_t moveFromIndex = firstIndex;
		size_t moveToIndex = 0;
		while (moveFromIndex < count)
		{
			levels[moveToIndex] = levels[moveFromIndex];
			++moveToIndex;
			++moveFromIndex;
		}
		const size_t previousCount = count;
		while (moveToIndex < previousCount)
		{
			levels[moveToIndex].quantity = Quantity(0);
			++moveToIndex;
			--count;
		}

		bestPx[sideIndex] = levels[0].price;
		bestQty[sideIndex] = levels[0].quantity;
		unadjustedBestPx[sideIndex] = bestPx[sideIndex];
		unadjustedBestQty[sideIndex] = bestQty[sideIndex];
	}
	else
	{
		count = 0;
		bestPx[sideIndex].reset();
		bestQty[sideIndex] = Quantity(0);
		unadjustedBestPx[sideIndex] = bestPx[sideIndex];
		unadjustedBestQty[sideIndex] = bestQty[sideIndex];
	}
}


}
