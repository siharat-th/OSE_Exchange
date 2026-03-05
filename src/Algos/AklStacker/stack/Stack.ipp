#pragma once

#include "Stack.hpp"

namespace akl::stacker
{
	
template<KTN::ORD::KOrderSide::Enum SIDE>
std::optional<Price> Stack::StackPrice() const
{
	return GetSide<SIDE>().StackPrice();
}


template<KTN::ORD::KOrderSide::Enum SIDE>
std::optional<Price> Stack::BestPriceWithOrders() const
{
	return GetSide<SIDE>().BestPriceWithOrders();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void Stack::Cancel(const Price px, const bool scratchImmediately)
{
	GetSide<SIDE>().Cancel(px, scratchImmediately);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
int Stack::AbandonPrice(const Price px)
{
	return GetSide<SIDE>().AbandonPrice(px);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool Stack::HasOrders(const Price px) const
{
	return GetSide<SIDE>().HasOrders(px);
}

}