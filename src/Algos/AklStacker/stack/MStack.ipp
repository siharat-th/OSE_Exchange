#pragma once

#include "MStack.hpp"
#include <Orders/OrderPod.hpp>

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
void MStack::Cancel(const Price px, const bool scratchImmediately)
{
	getSide<SIDE>().Cancel(px, scratchImmediately);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
Quantity MStack::GiveUpOrders(const Price px, FixedSizeVector<Order *, 10> &orders, bool &ordersRemaining)
{
	return getSide<SIDE>().GiveUpOrders(px, orders, ordersRemaining);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
int MStack::AbandonPrice(const Price px)
{
	return getSide<SIDE>().AbandonPrice(px);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool MStack::HasOrders(const Price px) const
{
	return getSide<SIDE>().HasOrders(px);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
std::optional<Price> MStack::BestStackPrice() const
{
	return getSide<SIDE>().BestStackPrice();
}

}
