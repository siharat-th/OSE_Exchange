#pragma once

#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include <akl/containers/FixedSizeVector.hpp>
#include <Orders/OrderEnumsV2.hpp>

namespace akl::stacker
{

struct Order;

class IExecutionInterface
{
public:
	virtual Order *CreateOrder(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px) = 0;

	virtual bool CanSubmit(const Order *o, bool canCross = false, bool canBypassWait = false, int numOrders = 1) const = 0;
	virtual bool CanModify(const Order *o, const Price px, bool canCross = false) const = 0;

	virtual void Submit(Order *o) = 0;
	virtual void Modify(Order *o, const Price px) = 0;
	virtual void Modify(Order *o, const Quantity qty) = 0;
	virtual void Cancel(Order *o) = 0;

	virtual Quantity TakeDynamicOrders(const KTN::ORD::KOrderSide::Enum side, const Price px, FixedSizeVector<Order *, 10> &orders) = 0;
	virtual Quantity TakeMOrders(const KTN::ORD::KOrderSide::Enum side, const Price px, const int stackIndex, FixedSizeVector<Order *, 10> &orders, bool &ordersRemaining) = 0;
};

}
