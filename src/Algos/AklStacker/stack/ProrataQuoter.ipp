#pragma once

#include "ProrataQuoter.hpp"

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataQuoter::Cancel(const Price px)
{
	getSideQuoter<SIDE>().Cancel(px);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
const std::optional<ProrataTarget> &ProrataQuoter::Target() const
{
	return getSideQuoter<SIDE>().Target();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataQuoter::GetOrders(std::vector<Order *> &orders) const
{
	getSideQuoter<SIDE>().GetOrders(orders);
}

}