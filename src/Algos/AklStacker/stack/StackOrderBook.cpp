#include "StackOrderBook.hpp"
#include "../IExecutionInterface.hpp"

#include <algorithm>
#include "../Order.hpp"
#include <akl/containers/FixedSizeVector.hpp>

namespace akl::stacker
{

StackOrderBook::StackOrderBook(
	const QuoteInstrumentSettings &stackSettings,
	const StackSideSettings &sideSettings,
	IExecutionInterface &execution,
	const Price tickIncrement,
	const KTN::ORD::KOrderSide::Enum side,
	const int8_t stackIndex)
: stackSettings(stackSettings)
, sideSettings(sideSettings)
, execution(execution)
, tickIncrement(tickIncrement)
, side(side)
, stackIndex(stackIndex)
{
	pricesToQuote.reserve(32);
}

void StackOrderBook::SetDefaultStackQuantity(const Quantity qty)
{
	stackQty = qty;
}

bool StackOrderBook::HasOrders(const Price px) const
{
	const int index = indexOf(px);
	for (Order *o : levels[index].orders)
	{
		if (o && o->IsWorking())// && o->order && (!o->order->GetCurrentState() || o->order->GetCurrentState()->IsWorking()))
		{
			return true;
		}
	}
	return false;
}

bool StackOrderBook::HasOrder(const Order *o) const
{
	const int index = indexOf(o->price);
	return std::find(levels[index].orders.begin(), levels[index].orders.end(), o) != levels[index].orders.end();
}

void StackOrderBook::Submit(const Price px)
{
	const int index = indexOf(px);
	submit(px, levels[index]);
}

void StackOrderBook::Submit(const Price px, const Quantity orderQty)
{
	const int index = indexOf(px);
	submit(px, levels[index], orderQty);
}

void StackOrderBook::Move(const Price newPx, const Price oldPx)
{
	move(newPx, oldPx);
}

void StackOrderBook::Cancel(const Price px, const bool scratchImmediately, const bool canceledDueToPriceBands)
{
	const int index = indexOf(px);
	for (Order *o : levels[index].orders)
	{
		execution.Cancel(o);
		o->scratchFillsImmediately = scratchImmediately;
	}
	levels[index].orders.clear();
	levels[index].manualCancel |= scratchImmediately;
	levels[index].canceledDueToPriceBands |= canceledDueToPriceBands;
}

void StackOrderBook::SendQueuedSubmits()
{
	for (const Price &px : pricesToQuote)
	{
		Submit(px);
	}
	pricesToQuote.clear();
}

void StackOrderBook::Requote(const Price px)
{
	const int index = indexOf(px);
	submit(px, levels[index]);
	++levels[index].numRequotes;
}

bool StackOrderBook::AddOrder(Order *o)
{
	const int index = indexOf(o->price);
	if (std::find(levels[index].orders.begin(), levels[index].orders.end(), o) == levels[index].orders.end())
	{
		levels[index].orders.push_back(o);
		o->type = Order::Type::STACK;
		o->stackIndex = stackIndex;
		o->wasAbandoned = false;
		LOGINFO("Added order {} to stack | px={} | index={}", o->orderReqId, o->price.AsShifted(), index);
		return true;
	}
	else
	{
		LOGINFO("Stack can't add order {} because it is already on level | px={} | index={}", o->orderReqId, o->price.AsShifted(), index);
		return false;
	}
}

bool StackOrderBook::RemoveOrder(Order *o)
{
	const int index = indexOf(o->price);
	OrderListType::iterator it = std::find(levels[index].orders.begin(), levels[index].orders.end(), o);
	if (it != levels[index].orders.end())
	{
		levels[index].orders.erase(it);
		return true;
	}
	else
	{
		return false;
	}
}

void StackOrderBook::ReleaseAllOrders()
{
	for (int i = 0; i < SIZE; ++i)
	{
		StackLevel &level = levels[i];
		if (!level.orders.empty())
		{
			for (OrderListType::iterator it = level.orders.begin(); it != level.orders.end(); ++it)
			{
				Order *o = *it;
				o->type = Order::Type::NONE;
			}
			level.canceledDueToPriceBands = false;
			level.orders.clear();
		}
	}
}

int StackOrderBook::ReleaseOrders(const Price px)
{
	const int index = indexOf(px);
	StackLevel &level = levels[index];
	if (!level.orders.empty())
	{
		int count = 0;
		for (OrderListType::iterator it = level.orders.begin(); it != level.orders.end(); ++it)
		{
			Order *o = *it;
			o->type = Order::Type::NONE;
			o->wasAbandoned = true;
			++count;
			LOGINFO("Abandoned order {}", o->orderReqId);
		}
		level.orders.clear();
		return count;
	}
	else
	{
		return 0;
	}
}

bool StackOrderBook::RefreshLevel(const Price px)
{
	const int index = indexOf(px);
	if (levels[index].orders.empty())
	{
		if (isInPriceBands(px))
		{
			submit(px, levels[index]);
			return true;
		}
		else
		{
			levels[index].canceledDueToPriceBands = true;
		}
	}
	return false;
}

void StackOrderBook::ClearForceCancel()
{
	for (int i = 0; i < SIZE; ++i)
	{
		levels[i].manualCancel = false;
		levels[i].numRequotes = 0;
	}
}

Quantity StackOrderBook::GetStackQuantity(const Price px) const
{
	const int index = indexOf(px);
	Quantity sum { 0 };
	for (OrderListType::const_iterator it = levels[index].orders.begin(); it != levels[index].orders.end(); ++it)
	{
		const Order *o = *it;
		sum += o->WorkingQuantity();
	}
	return sum;
}

bool StackOrderBook::CanRequote(const Price px) const
{
	const int index = indexOf(px);
	const StackLevel &level = levels[index];
	return level.orders.empty() && !level.manualCancel && level.numRequotes < stackSettings.requoteCount;
}

void StackOrderBook::ClearPriceBandCancelFlag(const Price px)
{
	const int index = indexOf(px);
	StackLevel &level = levels[index];
	level.canceledDueToPriceBands = false;
}

int StackOrderBook::indexOf(Price px) const
{
	return (px.AsShifted() / tickIncrement.AsShifted()) & MASK;
}

void StackOrderBook::submit(const Price px, StackLevel &level)
{
	submit(px, level, stackQty);
}

void StackOrderBook::submit(const Price px, StackLevel &level, const Quantity orderQty)
{
	if (stackSettings.newOrders && orderQty > Quantity(0) && !level.manualCancel)
	{
		Quantity qty = orderQty;
		// if (stackSettings.randomLotSizeMult > 0.0)
		// {
		// 	qty = std::max(randomizer.Randomize(orderQty, stackSettings.randomLotSizeMult), Quantity(1));
		// }
		Order *o = execution.CreateOrder(side, qty, px);
		if (o != nullptr)
		{
			o->price = px;
			o->targetQuantity = qty > Quantity(0) ? qty : Quantity(0);
			//o->OrdSide = side;
			o->type = Order::Type::STACK;
			o->stackIndex = stackIndex;
			if (execution.CanSubmit(o))
			{
				level.orders.push_back(o);
				execution.Submit(o);
			}
		}
		else
		{
			LOGINFO("Failed to create order {} order at price {}", KTN::ORD::KOrderSide::toString(side), px.AsShifted());
		}
	}
}

void StackOrderBook::queueSubmit(const Price px)
{
	pricesToQuote.push_back(px);
}

void StackOrderBook::move(const Price newPx, const Price oldPx)
{
	LOGINFO("Modify | newPx: {}, oldPx: {}", newPx.AsShifted(), oldPx.AsShifted());

	const bool oldPxInPriceBands = isInPriceBands(oldPx);
	const bool newPxInPriceBands = isInPriceBands(newPx);

	const int newIndex = indexOf(newPx);
	const int oldIndex = indexOf(oldPx);

	if ((oldPxInPriceBands || !levels[oldIndex].orders.empty()) && newPxInPriceBands)
	{
		const Quantity takenQty = takeOrders(newPx, levels[newIndex]);
		if (takenQty > Quantity(0))
		{
			if (HasOrders(oldPx))
			{
				Cancel(oldPx);
				if (stackQty > takenQty)
				{
					Submit(newPx, stackQty - takenQty);
				}
			}
			levels[newIndex].numRequotes = levels[indexOf(oldPx)].numRequotes;
		}
		else if (HasOrders(oldPx))
		{
			if (stackSettings.moveWithCancelNew)
			{
				Cancel(oldPx);
				queueSubmit(newPx);
				levels[newIndex].numRequotes = levels[indexOf(oldPx)].numRequotes;
			}
			else
			{
				const int oldIndex = indexOf(oldPx);
				for (Order *o : levels[oldIndex].orders)
				{
					o->price = newPx;
					execution.Modify(o, newPx);
				}

				for (Order *o : levels[oldIndex].orders)
				{
					levels[newIndex].orders.push_back(o);
				}
				levels[oldIndex].orders.clear();
				levels[newIndex].numRequotes = levels[oldIndex].numRequotes;
			}
		}
		else if (stackSettings.requoteOnMove)
		{
			queueSubmit(newPx);
		}
		levels[newIndex].canceledDueToPriceBands = false;
		levels[oldIndex].canceledDueToPriceBands = false;
	}
	else if (oldPxInPriceBands || !levels[oldIndex].orders.empty())
	{
		if (HasOrders(oldPx))
		{
			Cancel(oldPx);
			levels[newIndex].canceledDueToPriceBands = true;
		}
		else
		{
			levels[newIndex].canceledDueToPriceBands = false;
		}
		levels[newIndex].numRequotes = levels[indexOf(oldPx)].numRequotes;
	}
	else if (newPxInPriceBands)
	{
		const Quantity takenQty = takeOrders(newPx, levels[newIndex]);
		if (takenQty > Quantity(0))
		{
			if (HasOrders(oldPx))
			{
				Cancel(oldPx);
				if (stackQty > takenQty)
				{
					Submit(newPx, stackQty - takenQty);
				}
			}
			levels[newIndex].numRequotes = levels[indexOf(oldPx)].numRequotes;
		}
		else if (HasOrders(oldPx) || levels[indexOf(oldPx)].canceledDueToPriceBands)
		{
			Cancel(oldPx);
			queueSubmit(newPx);
			levels[newIndex].numRequotes = levels[indexOf(oldPx)].numRequotes;
		}

		levels[oldIndex].canceledDueToPriceBands = false;
		levels[newIndex].canceledDueToPriceBands = false;
	}
}

bool StackOrderBook::isInPriceBands(const Price px) const
{
	switch (side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return isInPriceBands<KTN::ORD::KOrderSide::Enum::BUY>(px);
		case KTN::ORD::KOrderSide::Enum::SELL:
			return isInPriceBands<KTN::ORD::KOrderSide::Enum::SELL>(px);
		default:
			return false;
	}
}

Quantity StackOrderBook::takeOrders(const Price px, StackLevel &level)
{
	// TODO
	FixedSizeVector<Order *, 10> orders;
	const Quantity dynamicQtyTaken = Quantity(0);//stackSettings.dynamicStack ? execution.TakeDynamicOrders(side, px, orders) : Quantity(0);
	// if (dynamicQtyTaken > Quantity(0))
	// {
	// 	for (int i = 0; i < orders.Size(); ++i)
	// 	{
	// 		level.orders.push_back(orders[i]);
	// 	}
	// 	orders.Clear();
	// }

	Quantity totalAdoptedQty { 0 };
	bool ordersRemaining = false;
	do
	{
		const Quantity adoptedQty = execution.TakeMOrders(side, px, stackIndex, orders, ordersRemaining);
		if (adoptedQty > Quantity(0))
		{
			totalAdoptedQty += adoptedQty;
			for (int i = 0; i < orders.Size(); ++i)
			{
				level.orders.push_back(orders[i]);
			}
			orders.Clear();
		}
		else
		{
			break;
		}
	} while (ordersRemaining);

	return dynamicQtyTaken + totalAdoptedQty;
}

}
