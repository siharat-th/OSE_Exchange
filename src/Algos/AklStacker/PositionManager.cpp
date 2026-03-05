#include "PositionManager.hpp"

namespace akl::stacker
{

PositionManager::PositionManager(const Price tickIncrement)
: tickIncrement(tickIncrement)
{

}

Quantity PositionManager::Position() const
{
	return pos;
}

void PositionManager::Add(const Quantity qty, const Price px)
{
	if (qty > Quantity(0))
	{
		const Quantity remaining = netAgainstExistingPositions(qty, askMap);
		if (remaining > Quantity(0))
		{
			bidMap[px] += remaining;
		}
		pos += qty;
	}
	else if (qty < Quantity(0))
	{
		const Quantity remaining = netAgainstExistingPositions(-qty, bidMap);
		if (remaining > Quantity(0))
		{
			askMap[px] += remaining;
		}
		pos += qty;
	}
}

void PositionManager::ClearPosition()
{
	pos = Quantity(0);
	bidMap.clear();
	askMap.clear();
}

template<>
Quantity PositionManager::ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(const Price px, const int maxPayupTicks)
{
	const Quantity clearedQty = clearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(px, maxPayupTicks, bidMap);
	pos -= clearedQty;
	return clearedQty;
}

template<>
Quantity PositionManager::ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(const Price px, const int maxPayupTicks)
{
	const Quantity clearedQty = clearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(px, maxPayupTicks, askMap);
	pos += clearedQty;
	return clearedQty;
}

template<>
PositionManager::MapType<KTN::ORD::KOrderSide::Enum::BUY> &PositionManager::getMap<KTN::ORD::KOrderSide::Enum::BUY>()
{
	return bidMap;
}

template<>
PositionManager::MapType<KTN::ORD::KOrderSide::Enum::SELL> &PositionManager::getMap<KTN::ORD::KOrderSide::Enum::SELL>()
{
	return askMap;
}

template<>
const PositionManager::MapType<KTN::ORD::KOrderSide::Enum::BUY> &PositionManager::getMap<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return bidMap;
}

template<>
const PositionManager::MapType<KTN::ORD::KOrderSide::Enum::SELL> &PositionManager::getMap<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return askMap;
}

}