#pragma once

#include <optional>
#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <KT01/DataStructures/DepthBook.hpp>

namespace akl
{

struct Level
{
	Quantity quantity;
	Price price;

	bool operator==(const Level &rhs) const
	{
		return quantity == rhs.quantity && price == rhs.price;
	}
};

class Book
{
public:
	Book();

	void SetSymbol(const std::string &symbol);

	void Process(const KT01::DataStructures::MarketDepth::BookSecurityStatus secStatus, const KT01::DataStructures::MarketDepth::BookSecurityTradingEvent event, const uint64_t recvTime);
	void Process(const KT01::DataStructures::MarketDepth::DepthBook &snapshot);
	void Process(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px, const uint32_t rptseq, const uint64_t recvTime);
	void CleanCross();

	bool ValidateBook() const;

	std::optional<Price> CalculateBidLean(const Quantity qty) const;
	std::optional<Price> CalculateAskLean(const Quantity qty) const;

	inline size_t BidCount() const { return bidCount; }
	inline size_t AskCount() const { return askCount; }

	template<KTN::ORD::KOrderSide::Enum SIDE>
	size_t LevelCount() const;

	inline const Level &GetBidLevel(int index) const { return bids[index]; }
	inline const Level &GetAskLevel(int index) const { return asks[index]; }

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const Level &GetLevel(int index) const;

	std::optional<Level> TotalBidLean() const;
	std::optional<Level> TotalAskLean() const;

	void LogDepth() const;
	void LogTob() const;

	uint64_t ServerTime() const;
	uint64_t SdkTime() const;
	void ClearTimes();

	inline bool IsLtpAdjusted() const { return isLtpAdjusted; }

	inline bool IsOpen() const { return secStatus == KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade; }
	inline bool IsPreopen() const { return secStatus == KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen; }
	//inline bool IsPretrading() const { return marketState == ttsdk::TradingStatus::Pretrading; }
	inline bool IsClosed() const { return secStatus == KT01::DataStructures::MarketDepth::BookSecurityStatus::Close; }
	//inline bool IsHalted() const { return marketState == ttsdk::TradingStatus::Mwcbhalt1 || marketState == ttsdk::TradingStatus::Mwcbhalt2 || marketState == ttsdk::TradingStatus::Mwcbhalt3; }
	inline KT01::DataStructures::MarketDepth::BookSecurityStatus MarketState() const { return secStatus; }

	inline bool NoCancel() const { return tradingEvent == KT01::DataStructures::MarketDepth::BookSecurityTradingEvent::NoCancel; }

	inline bool HasBid() const { return bestPx[0].has_value(); }
	inline bool HasAsk() const { return bestPx[1].has_value(); }
	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool HasSide() const;

	inline Price BestBid() const { return *(bestPx[0]); }
	inline Price BestAsk() const { return *(bestPx[1]); }
	template<KTN::ORD::KOrderSide::Enum SIDE>
	Price BestPrice() const;
	
	inline Quantity BestBidQuantity() const { return bestQty[0]; }
	inline Quantity BestAskQuantity() const { return bestQty[1]; }
	template<KTN::ORD::KOrderSide::Enum SIDE>
	Quantity BestQuantity() const;

	inline bool HasPreviousBid() const { return prevBestPx[0].has_value(); }
	inline bool HasPreviousAsk() const { return prevBestPx[1].has_value(); }
	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool HasPreviousSide() const;

	inline Price PreviousBestBid() const { return *(prevBestPx[0]); }
	inline Price PreviousBestAsk() const { return *(prevBestPx[1]); }
	template<KTN::ORD::KOrderSide::Enum SIDE>
	Price PreviousBestPrice() const;

	inline Quantity PreviousBestBidQuantity() const { return prevBestQty[0]; }
	inline Quantity PreviousBestAskQuantity() const { return prevBestQty[1]; }
	template<KTN::ORD::KOrderSide::Enum SIDE>
	Quantity PreviousBestQuantity() const;

	inline bool HasUnadjustedBid() const { return unadjustedBestPx[0].has_value(); }
	inline bool HasUnadjustedAsk() const { return unadjustedBestPx[1].has_value(); }
	inline Price UnadjustedBestBid() const { return *unadjustedBestPx[0]; }
	inline Price UnadjustedBestAsk() const { return *unadjustedBestPx[1]; }
	inline Quantity UnadjustedBestBidQuantity() const { return unadjustedBestQty[0]; }
	inline Quantity UnadjustedBestAskQuantity() const { return unadjustedBestQty[1]; }

	template<KTN::ORD::KOrderSide::Enum SIDE>
	std::optional<Price> CalculateLean(const Quantity qty) const;

	void UpdatePrevious();


	inline const std::string &Symbol() const { return symbol; }

	bool TobPriceChanged() const;
	bool IsBookGapped() const;

	Price TickIncrement { Price::FromShifted(0) };

private:
	static constexpr size_t MAX_LEVELS = 10;

	Level bids[MAX_LEVELS];
	Level asks[MAX_LEVELS];
	size_t bidCount { 0 };
	size_t askCount { 0 };
	uint64_t depthSeqNum { 0 };
	uint64_t recvTime { 0 };
	uint64_t sdkTime { 0 };
	uint64_t lastDepthExchangeTime { 0 };
	bool isLtpAdjusted { false };

	std::optional<Price> bestPx[2] { std::nullopt, std::nullopt };
	std::optional<Price> prevBestPx[2] { std::nullopt, std::nullopt };
	std::optional<Price> unadjustedBestPx[2] { std::nullopt, std::nullopt };
	Quantity bestQty[2] { Quantity(0), Quantity(0) };
	Quantity prevBestQty[2] { Quantity(0), Quantity(0) };
	Quantity unadjustedBestQty[2] { Quantity(0), Quantity(0) };
	KT01::DataStructures::MarketDepth::BookSecurityStatus secStatus { KT01::DataStructures::MarketDepth::BookSecurityStatus::UnknownOrInvalid };
	KT01::DataStructures::MarketDepth::BookSecurityTradingEvent tradingEvent { KT01::DataStructures::MarketDepth::BookSecurityTradingEvent::NoEvent };
	bool snapshotStatusProcessed { false };
	bool logMarketStateWarning { true };

	void processSide(const std::vector<KT01::DataStructures::MarketDepth::pxl> &sideDepth, Level *levels, size_t &count, const int index);
	std::optional<Price> calculateLean(const Quantity qty, const Level *levels, const size_t count, const int index) const;

	// bool shouldProcessLtp(const ttsdk::TimeSales &ts, const int index) const;
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void processLtp(const Quantity tradeQty, const Price tradePx, Level *levels, size_t &count);

	void cleanUpEmptyLevels(Level *levels, size_t &count, const size_t firstIndex, const int sideIndex);



	std::string symbol;
};

}

#include "Book.ipp"