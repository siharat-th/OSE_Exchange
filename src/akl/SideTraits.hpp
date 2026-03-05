#pragma once

#include "Quantity.hpp"
#include "Price.hpp"
#include <Orders/OrderEnumsV2.hpp>
#include <algorithm>

namespace akl
{
	
template<KTN::ORD::KOrderSide::Enum SIDE>
struct SideTraits
{
private:
	SideTraits() = delete;
	SideTraits(const SideTraits<SIDE> &) = delete;
	SideTraits<SIDE> &operator=(const SideTraits<SIDE> &) = delete;
	SideTraits(const SideTraits<SIDE> &&) = delete;
	SideTraits<SIDE> &operator=(const SideTraits<SIDE> &&) = delete;
};

template<>
struct SideTraits<KTN::ORD::KOrderSide::Enum::BUY>
{
	static constexpr int INDEX = 0;
	static constexpr KTN::ORD::KOrderSide::Enum OPPOSITE = KTN::ORD::KOrderSide::Enum::SELL;
	static constexpr Price INVALID_PRICE = Price::Min();

	using BestPriceComparerType = std::greater<Price>;
	using WorstPriceComparerType = std::less<Price>;

	static inline Price PriceIn(const Price px, const Price increment) noexcept
	{
		return px + increment;
	}

	static inline Price PriceOut(const Price px, const Price increment) noexcept
	{
		return px - increment;
	}

	static inline bool IsBetter(const Price px1, const Price px2) noexcept
	{
		return px1 > px2;
	}

	static inline bool IsWorse(const Price px1, const Price px2) noexcept
	{
		return px1 < px2;
	}

	static inline Price PickBest(const Price px1, const Price px2) noexcept
	{
		return std::max(px1, px2);
	}

	static inline Price PickWorst(const Price px1, const Price px2) noexcept
	{
		return std::min(px1, px2);
	}

	// New, Old
	static Price DeltaIn(const Price px1, const Price px2) noexcept
	{
		return px1 - px2;
	}

	static inline Quantity SidePosition(const Quantity qty) noexcept
	{
		return qty > Quantity(0) ? qty : Quantity(0);
	}

private:
	SideTraits() = delete;
	SideTraits(const SideTraits<KTN::ORD::KOrderSide::Enum::BUY> &) = delete;
	SideTraits<KTN::ORD::KOrderSide::Enum::BUY> &operator=(const SideTraits<KTN::ORD::KOrderSide::Enum::BUY> &) = delete;
	SideTraits(const SideTraits<KTN::ORD::KOrderSide::Enum::BUY> &&) = delete;
	SideTraits<KTN::ORD::KOrderSide::Enum::BUY> &operator=(const SideTraits<KTN::ORD::KOrderSide::Enum::BUY> &&) = delete;
};

template<>
struct SideTraits<KTN::ORD::KOrderSide::Enum::SELL>
{
	static constexpr int INDEX = 1;
	static constexpr KTN::ORD::KOrderSide::Enum OPPOSITE = KTN::ORD::KOrderSide::Enum::BUY;
	static constexpr Price INVALID_PRICE = Price::Max();

	using BestPriceComparerType = std::less<Price>;
	using WorstPriceComparerType = std::greater<Price>;

	static inline Price PriceIn(const Price px, const Price increment) noexcept
	{
		return px - increment;
	}

	static inline Price PriceOut(const Price px, const Price increment) noexcept
	{
		return px + increment;
	}

	static inline bool IsBetter(const Price px1, const Price px2) noexcept
	{
		return px1 < px2;
	}

	static inline bool IsWorse(const Price px1, const Price px2) noexcept
	{
		return px1 > px2;
	}

	static inline Price PickBest(const Price px1, const Price px2) noexcept
	{
		return std::min(px1, px2);
	}

	static inline Price PickWorst(const Price px1, const Price px2) noexcept
	{
		return std::max(px1, px2);
	}

	static inline Price DeltaIn(const Price px1, const Price px2) noexcept
	{
		return px2 - px1;
	}

	static inline Quantity SidePosition(const Quantity qty) noexcept
	{
		return qty < Quantity(0) ? -qty : Quantity(0);
	}

private:
	SideTraits() = delete;
	SideTraits(const SideTraits<KTN::ORD::KOrderSide::Enum::SELL> &) = delete;
	SideTraits<KTN::ORD::KOrderSide::Enum::SELL> &operator=(const SideTraits<KTN::ORD::KOrderSide::Enum::SELL> &) = delete;
	SideTraits(const SideTraits<KTN::ORD::KOrderSide::Enum::SELL> &&) = delete;
	SideTraits<KTN::ORD::KOrderSide::Enum::SELL> &operator=(const SideTraits<KTN::ORD::KOrderSide::Enum::SELL> &&) = delete;
};

}