#pragma once

#include <Orders/OrderPod.hpp>
#include <akl/Quantity.hpp>
#include "Order.hpp"

namespace akl::stacker
{

inline Quantity WorkingQuantity(const KTN::OrderPod *o)
{
	return Quantity(static_cast<int>(o->leavesqty));
}

inline Quantity FilledQuantity(const KTN::OrderPod *o)
{
	return Quantity(static_cast<int>(o->fillqty));
}

inline bool IsWorking(const KTN::OrderPod *o)
{
	return o->leavesqty > 0;
}

inline bool IsWorking(const Order *o)
{
	return o->filledQuantity < o->targetQuantity;
}

}