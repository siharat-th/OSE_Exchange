#pragma once

#include "PositionManager.hpp"
#include <akl/SideTraits.hpp>
#include <KT01/Core/Log.hpp>

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
Quantity PositionManager::GetHedgeQuantity(const Price px, const int maxPayupTicks) const
{
	constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;

	const MapType<OPP> &map = getMap<OPP>();
	const Price hedgeAllowedPx = SideTraits<SIDE>::PriceOut(px, tickIncrement * maxPayupTicks);
	Quantity hedgeQty = Quantity(0);
	for (typename MapType<OPP>::const_reverse_iterator it = map.rbegin(); it != map.rend(); ++it)
	{
		if (!SideTraits<SIDE>::IsWorse(it->first, hedgeAllowedPx))
		{
			hedgeQty += it->second;
		}
		else
		{
			break;
		}
	}
	return hedgeQty;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
Quantity PositionManager::PositionAt(const Price px) const
{
	const MapType<SIDE> &map = getMap<SIDE>();
	typename MapType<SIDE>::const_iterator it = map.find(px);
	if (it != map.end())
	{
		return it->second;
	}
	else
	{
		return Quantity(0);
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool PositionManager::RepricePositions(const Price px)
{
	MapType<SIDE> &map = getMap<SIDE>();
	Quantity repricedPosition { 0 };
	while (!map.empty() && SideTraits<SIDE>::IsBetter(map.begin()->first, px))
	{
		const Quantity positionAtPx = map.begin()->second;
		LOGINFO("Repricing {} position {}@{}", KTN::ORD::KOrderSide::toString(SIDE), positionAtPx.value(), map.begin()->first.AsShifted());
		repricedPosition += positionAtPx;
		map.erase(map.begin());
	}
	if (repricedPosition > Quantity(0))
	{
		map[px] += repricedPosition;
		LOGINFO("Repriced total {} position of {} to {}", KTN::ORD::KOrderSide::toString(SIDE), repricedPosition.value(), px.AsShifted());
		return true;
	}
	else
	{
		return false;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
std::optional<Price> PositionManager::GetInnermostPrice() const
{
	const MapType<SIDE> &map = getMap<SIDE>();
	if (!map.empty())
	{
		return map.begin()->first;
	}
	else
	{
		return std::nullopt;
	}
}

template<typename PM_MAP_TYPE>
Quantity PositionManager::netAgainstExistingPositions(const Quantity qty, PM_MAP_TYPE &map) const
{
	Quantity remaining = qty;
	while (!map.empty() && remaining > Quantity(0))
	{
		typename PM_MAP_TYPE::reverse_iterator it = map.rbegin();
		if (remaining >= it->second)
		{
			remaining -= it->second;
			map.erase(std::next(it).base());
		}
		else
		{
			it->second -= remaining;
			remaining = Quantity(0);
		}
	}
	return remaining;
}

template<KTN::ORD::KOrderSide::Enum SIDE, typename PM_MAP_TYPE>
Quantity PositionManager::clearMaxPayupPosition(const Price px, const int maxPayupTicks, PM_MAP_TYPE &map)
{
	const Price hedgeAllowedPx = SideTraits<SIDE>::PriceIn(px, tickIncrement * maxPayupTicks);
	Quantity clearedQty { 0 };
	typename PM_MAP_TYPE::const_iterator it = map.begin();
	while (it != map.end())
	{
		if (SideTraits<SIDE>::IsBetter(it->first, hedgeAllowedPx))
		{
			clearedQty += it->second;
			it = map.erase(it);
		}
		else
		{
			break;
		}
	}
	return clearedQty;
}

}