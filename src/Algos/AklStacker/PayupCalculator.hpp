#pragma once

#include <map>

#include <Orders/OrderEnumsV2.hpp>
#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include <akl/SideTraits.hpp>

#include <AlgoParams/AklStackerParams.hpp>

#include "PositionManager.hpp"

namespace akl
{
	class Book;
}

namespace akl::stacker
{

struct QuoteInstrumentSettings;
class PricingModel;
class DynamicPricingModel;
struct Order;

class PayupCalculator
{
public:
	PayupCalculator(const Book &book, const QuoteInstrumentSettings &settings, const PricingModel &spm, /*const DynamicPricingModel &dpm,*/ const PositionManager &pm);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline Quantity GetPayup() const
	{
		return payupQty[SideTraits<SIDE>::INDEX];
	}

	void Recalculate();
	void OnFill(const Order *o, const Price fillPx);

	void Clear();

private:
	PayupCalculator(const PayupCalculator &) = delete;
	const PayupCalculator &operator=(const PayupCalculator &) = delete;
	PayupCalculator(const PayupCalculator &&) = delete;
	const PayupCalculator &operator=(const PayupCalculator &&) = delete;

	const Book &book;
	const QuoteInstrumentSettings &settings;
	const PricingModel &spm;
	//const DynamicPricingModel &dpm;
	const PositionManager &pm;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	using ComparerType = typename SideTraits<SIDE>::BestPriceComparerType;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	using MapType = std::map<Price, int8_t, ComparerType<SIDE>>;

	// Asks sorted from lowest price to highest price
	using AskMapType = MapType<KTN::ORD::KOrderSide::Enum::SELL>;
	// Bids sorted from highest price to lowest price
	using BidMapType = MapType<KTN::ORD::KOrderSide::Enum::BUY>;
	AskMapType askMap;
	BidMapType bidMap;

	Quantity payupQty[2] { Quantity(0), Quantity(0) };

	template<KTN::ORD::KOrderSide::Enum SIDE>
	MapType<SIDE> &getMap();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const MapType<SIDE> &getMap() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void removeLockedLevels();
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void removeCrossingLevels();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	Quantity calculatePayup();
};

}

#include "PayupCalculator.ipp"