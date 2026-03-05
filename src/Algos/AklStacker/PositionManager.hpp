#pragma once

#include <map>
#include <optional>

#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include <akl/SideTraits.hpp>

namespace akl::stacker
{

class PositionManager
{
public:
	PositionManager(const Price tickIncrement);

	Quantity Position() const;
	void Add(const Quantity qty, const Price px);
	void ClearPosition();

	// Side is in terms of hedge
	template<KTN::ORD::KOrderSide::Enum SIDE>
	Quantity GetHedgeQuantity(const Price px, const int maxPayupTicks) const;

	// Side is in terms of position
	template<KTN::ORD::KOrderSide::Enum SIDE>
	Quantity ClearMaxPayupPosition(const Price px, const int maxPayupTicks);

	// Side is in terms of position
	template<KTN::ORD::KOrderSide::Enum SIDE>
	Quantity PositionAt(const Price px) const;

	// Side is in terms of position
	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool RepricePositions(const Price px);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	std::optional<Price> GetInnermostPrice() const;

private:
	PositionManager(const PositionManager &) = delete;
	PositionManager &operator=(const PositionManager &) = delete;
	PositionManager(const PositionManager &&) = delete;
	PositionManager &operator=(const PositionManager &&) = delete;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	using ComparerType = typename SideTraits<SIDE>::BestPriceComparerType;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	using MapType = std::map<Price, Quantity, ComparerType<SIDE>>;

	// Asks sorted from lowest price to highest price
	using AskMapType = MapType<KTN::ORD::KOrderSide::Enum::SELL>;
	// Bids sorted from highest price to lowest price
	using BidMapType = MapType<KTN::ORD::KOrderSide::Enum::BUY>;
	AskMapType askMap;
	BidMapType bidMap;

	Quantity pos { 0 };
	const Price tickIncrement;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	MapType<SIDE> &getMap();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const MapType<SIDE> &getMap() const;

	template<typename PM_MAP_TYPE>
	Quantity netAgainstExistingPositions(const Quantity qty, PM_MAP_TYPE &map) const;

	// Side is in terms of position
	template<KTN::ORD::KOrderSide::Enum SIDE, typename PM_MAP_TYPE>
	Quantity clearMaxPayupPosition(const Price px, const int maxPayupTicks, PM_MAP_TYPE &map);
};

}

#include "PositionManager.ipp"