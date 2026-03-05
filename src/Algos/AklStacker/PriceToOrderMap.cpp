#include "PriceToOrderMap.hpp"
#include <KT01/Core/Log.hpp>

namespace akl::stacker
{

PriceToOrderMap::PriceToOrderMap()
{
	
}

bool PriceToOrderMap::HasOrders() const
{
	return bidMap.HasOrders() || askMap.HasOrders();
}

bool PriceToOrderMap::HasOrder(const Order *o) const
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bidMap.HasOrder(o);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return askMap.HasOrder(o);
		default:
			LOGWARN("Unabled to check if order {} with unknown side {} is in map", o->orderReqId, static_cast<int>(o->side));
			return false;
	}
}

bool PriceToOrderMap::AddOrder(Order *o)
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bidMap.AddOrder(o);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return askMap.AddOrder(o);
		default:
			LOGWARN("Unabled to add order {} with unknown side {} to map", o->orderReqId, static_cast<int>(o->side));
			return false;
	}
}

bool PriceToOrderMap::RemoveOrder(Order *o)
{
	switch (o->side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return bidMap.RemoveOrder(o);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return askMap.RemoveOrder(o);
		default:
			LOGWARN("Unabled to remove order {} with unknown side {} from map", o->orderReqId, static_cast<int>(o->side));
			return false;
	}
}

void PriceToOrderMap::ReleaseOrders()
{
	bidMap.ReleaseOrders();
	askMap.ReleaseOrders();
}

void PriceToOrderMap::Clear()
{
	bidMap.Clear();
	askMap.Clear();
}

template<>
PriceToOrderMap::SideMapType<KTN::ORD::KOrderSide::Enum::BUY> &PriceToOrderMap::GetSideMap<KTN::ORD::KOrderSide::Enum::BUY>()
{
	return bidMap;
}

template<>
PriceToOrderMap::SideMapType<KTN::ORD::KOrderSide::Enum::SELL> &PriceToOrderMap::GetSideMap<KTN::ORD::KOrderSide::Enum::SELL>()
{
	return askMap;
}

template<>
const PriceToOrderMap::SideMapType<KTN::ORD::KOrderSide::Enum::BUY> &PriceToOrderMap::GetSideMap<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return bidMap;
}

template<>
const PriceToOrderMap::SideMapType<KTN::ORD::KOrderSide::Enum::SELL> &PriceToOrderMap::GetSideMap<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return askMap;
}

}
