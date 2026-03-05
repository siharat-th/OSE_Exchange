#pragma once

#include "StackOrderBook.hpp"
#include "StackSide.hpp"
#include "../Time.hpp"
#include "../Order.hpp"

#include <AlgoParams/AklStackerParams.hpp>
#include <akl/SideTraits.hpp>
#include <akl/BranchPrediction.hpp>

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
StackSide<SIDE>::StackSide(
	const QuoteInstrumentSettings &settings,
	const StackSideSettings &sideSettings,
	IExecutionInterface &execution,
	const Time &time,
	const Price tickIncrement,
	const int8_t stackIndex)
: settings(settings)
, sideSettings(sideSettings)
, time(time)
, orderBook(settings, sideSettings, execution, tickIncrement, SIDE, stackIndex)
, tickIncrement(tickIncrement)
{

}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::SetDefaultStackQuantity(const Quantity qty)
{
	orderBook.SetDefaultStackQuantity(qty);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StackSide<SIDE>::HasOrders() const
{
	if (stackPx)
	{
		Price px = *stackPx;
		for (int i = 0; i < sideSettings.length; ++i)
		{
			if (orderBook.HasOrders(px))
			{
				return true;
			}
			else
			{
				px = SideTraits<SIDE>::PriceOut(px, tickIncrement);
			}
		}
	}

	return false;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StackSide<SIDE>::HasOrders(const Price px) const
{
	return orderBook.HasOrders(px);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StackSide<SIDE>::HasOrder(const Order *o) const
{
	return orderBook.HasOrder(o);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
std::optional<Price> StackSide<SIDE>::BestPriceWithOrders() const
{
	if (stackPx)
	{
		Price px = *stackPx;
		for (int i = 0; i < sideSettings.length; ++i)
		{
			if (orderBook.HasOrders(px))
			{
				return px;
			}
			else
			{
				px = SideTraits<SIDE>::PriceOut(px, tickIncrement);
			}
		}
	}

	return std::nullopt;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StackSide<SIDE>::Update(const std::optional<Price> px, const bool forced)
{
	bool result = false;
	if (px)
	{
		if (stackPx)
		{
			if (sideSettings.length > 0)
			{
				if (SideTraits<SIDE>::IsBetter(*px, *stackPx))
				{
					moveIn(*px, *stackPx);
					result = true;
				}
				else if (SideTraits<SIDE>::IsWorse(*px, *stackPx))
				{
					moveOut(*px, *stackPx);
					result = true;
				}
			}
			else
			{
				stackPx = px;
			}
		}
		else
		{
			stackPx = px;
			result = true;
		}

		if (forced)
		{
			result |= refreshStack();
			pendingRequotes.clear();
		}
	}
	return result;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StackSide<SIDE>::CanAddOrder(const uint64_t reqId, const Price px) const
{
	if (sideSettings.length > 0)
	{
		if (stackPx)
		{
			const Price lastPx = SideTraits<SIDE>::PriceOut(*stackPx, Price::FromShifted(tickIncrement.AsShifted() * (sideSettings.length - 1)));
			if (SideTraits<SIDE>::IsBetter(px, *stackPx))
			{
				LOGINFO("Can't add order {} to stack because order price {} is better than stack price {}", reqId, px.AsShifted(), stackPx->AsShifted());
				return false;
			}
			else if (SideTraits<SIDE>::IsWorse(px, lastPx))
			{
				LOGINFO("Can't add order {} to stack because order price {} is worse than last stack price {}", reqId, px.AsShifted(), lastPx.AsShifted());
				return false;
			}
			else if (sideSettings.insidePrice && SideTraits<SIDE>::IsBetter(px, *sideSettings.insidePrice))
			{
				LOGINFO("Can't add order {} to stack because order price {} is better than inside price {}", reqId, px.AsShifted(), sideSettings.insidePrice->AsShifted());
				return false;
			}
			else if (sideSettings.outsidePrice && SideTraits<SIDE>::IsWorse(px, *sideSettings.outsidePrice))
			{
				LOGINFO("Can't add order {} to stack because order price {} is worse than outside price {}", reqId, px.AsShifted(), sideSettings.outsidePrice->AsShifted());
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			LOGINFO("Can't add order {} to stack because stack has no price", reqId);
			return false;
		}
	}
	else
	{
		LOGINFO("Can't add order {} to stack because stack length is 0", reqId);
		return false;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StackSide<SIDE>::AddOrder(Order *o)
{
	if (sideSettings.length > 0)
	{
		if (stackPx)
		{
			const Price lastPx = SideTraits<SIDE>::PriceOut(*stackPx, Price::FromShifted(tickIncrement.AsShifted() * (sideSettings.length - 1)));
			if (SideTraits<SIDE>::IsBetter(o->price, *stackPx))
			{
				LOGINFO("Can't add order {} to stack because order price {} is better than stack price {}", o->orderReqId, o->price.AsShifted(), stackPx->AsShifted());
				return false;
			}
			else if (SideTraits<SIDE>::IsWorse(o->price, lastPx))
			{
				LOGINFO("Can't add order {} to stack because order price {} is worse than last stack price {}", o->orderReqId, o->price.AsShifted(), lastPx.AsShifted());
				return false;
			}
			else if (sideSettings.insidePrice && SideTraits<SIDE>::IsBetter(o->price, *sideSettings.insidePrice))
			{
				LOGINFO("Can't add order {} to stack because order price {} is better than inside price {}", o->orderReqId, o->price.AsShifted(), sideSettings.insidePrice->AsShifted());
				return false;
			}
			else if (sideSettings.outsidePrice && SideTraits<SIDE>::IsWorse(o->price, *sideSettings.outsidePrice))
			{
				LOGINFO("Can't add order {} to stack because order price {} is worse than outside price {}", o->orderReqId, o->price.AsShifted(), sideSettings.outsidePrice->AsShifted());
				return false;
			}
			else if (unlikely(o->submitQueued))
			{
				LOGINFO("Can't add order {} to stack because it is queued for submission", o->orderReqId);
				return false;
			}
			else
			{
				return orderBook.AddOrder(o);
			}
		}
		else
		{
			LOGINFO("Can't add order {} to stack because stack has no price", o->orderReqId);
			return false;
		}
	}
	else
	{
		LOGINFO("Can't add order {} to stack because stack length is 0", o->orderReqId);
		return false;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StackSide<SIDE>::RemoveOrder(Order *o, const bool wasFill)
{
	const bool result = orderBook.RemoveOrder(o);
	if (result)
	{
		if (wasFill && orderBook.CanRequote(o->price))
		{
			pendingRequotes.push_back({o->price, time.Now()});
		}
	}
	return result;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::ReleaseAllOrders()
{
	orderBook.ReleaseAllOrders();
	stackPx.reset();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::CancelAllOrders()
{
	if (stackPx && length > 0)
	{
		Price px = *stackPx;
		for (int i = 0; i < length; ++i)
		{
			orderBook.Cancel(px);
			px = SideTraits<SIDE>::PriceOut(px, tickIncrement);
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::Cancel(const Price cancelPx, const bool scratchImmediately)
{
	if (stackPx && length > 0)
	{
		Price px = *stackPx;
		while (!SideTraits<SIDE>::IsWorse(px, cancelPx))
		{
			orderBook.Cancel(px, scratchImmediately);
			px = SideTraits<SIDE>::PriceOut(px, tickIncrement);
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::SendQueuedSubmits()
{
	orderBook.SendQueuedSubmits();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::UpdateLength(const int newLength)
{
	if (stackPx && newLength < length)
	{
		for (int i = length - 1; i >= newLength; --i)
		{
			const Price px = SideTraits<SIDE>::PriceOut(*stackPx, tickIncrement * i);
			orderBook.Cancel(px);
		}
	}
	length = newLength;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::GetStackInfo(int64_t &stackPrice, int32_t *qtys, int &index, const int size) const
{
	if (stackPrice != SideTraits<SIDE>::INVALID_PRICE.AsShifted())
	{
		if (stackPx)
		{
			index = SideTraits<SIDE>::DeltaIn(Price::FromShifted(stackPrice), *stackPx) / tickIncrement;
			Price px = *stackPx;
			const int startingIndex = index;
			while ((index - startingIndex) < sideSettings.length && index >= 0 && index < size)
			{
				qtys[index] += orderBook.GetStackQuantity(px).value();
				++index;
				px = SideTraits<SIDE>::PriceOut(px, tickIncrement);
			}
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
int StackSide<SIDE>::AbandonPrice(const Price px)
{
	return orderBook.ReleaseOrders(px);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StackSide<SIDE>::IsPriceInStack(const Price px) const
{
	return stackPx
		&& !SideTraits<SIDE>::IsBetter(px, *stackPx)
		&& !SideTraits<SIDE>::IsWorse(px, SideTraits<SIDE>::PriceOut(*stackPx, std::max(sideSettings.length - 1, 0) * tickIncrement))
		&& (!sideSettings.insidePrice || !SideTraits<SIDE>::IsBetter(px, *sideSettings.insidePrice))
		&& (!sideSettings.outsidePrice || !SideTraits<SIDE>::IsWorse(px, *sideSettings.outsidePrice));
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StackSide<SIDE>::CheckRequote(const Timestamp now)
{
	const Duration delay = Millis(settings.requoteDelay);
	bool result = false;
	while (!pendingRequotes.empty() && now >= pendingRequotes.front().fillTime + delay)
	{
		const Price px = pendingRequotes.front().px;
		if (settings.newOrders && IsPriceInStack(px) && orderBook.CanRequote(px))
		{
			orderBook.Requote(px);
			result = true;
		}
		pendingRequotes.pop_front();
	}
	return result;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::HandlePriceBandsUpdate()
{
	if (stackPx)
	{
		Price px = *stackPx;
		for (int i = 0; i < sideSettings.length; ++i)
		{
			if ((sideSettings.insidePrice && SideTraits<SIDE>::IsBetter(px, *sideSettings.insidePrice))
				|| (sideSettings.outsidePrice && SideTraits<SIDE>::IsWorse(px, *sideSettings.outsidePrice)))
			{
				orderBook.Cancel(px, false, true);
			}
			else
			{
				orderBook.ClearPriceBandCancelFlag(px);
			}
			px = SideTraits<SIDE>::PriceOut(px, tickIncrement);
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::moveIn(const Price newPx, const Price oldPx)
{
	const int tickDelta = static_cast<int>(SideTraits<SIDE>::DeltaIn(newPx, oldPx).AsShifted() / tickIncrement.AsShifted());

	int numLevelsToMove;
	Price priceDelta;

	if (tickDelta < sideSettings.length)
	{
		numLevelsToMove = tickDelta;
		priceDelta = sideSettings.length * tickIncrement;
	}
	else
	{
		numLevelsToMove = sideSettings.length;
		priceDelta = std::min(tickDelta, StackOrderBook::SIZE / 2) * tickIncrement;
	}

	Price moveFrom = SideTraits<SIDE>::PriceOut(oldPx, Price::FromShifted(tickIncrement.AsShifted() * (sideSettings.length - 1)));
	for (int i = 0; i < numLevelsToMove; ++i)
	{
		const Price moveTo = SideTraits<SIDE>::PriceIn(moveFrom, priceDelta);
		if (likely(!SideTraits<SIDE>::IsBetter(moveTo, newPx)))
		{
			orderBook.Move(moveTo, moveFrom);
			updateRequotePrice(moveFrom, moveTo);
		}
		else
		{
			LOGWARN("{} stack not moving level {} to {} because new price is better than target stack price {}",
				KTN::ORD::KOrderSide::toString(SIDE), moveFrom.AsShifted(), moveTo.AsShifted(), newPx.AsShifted());
		}
		moveFrom = SideTraits<SIDE>::PriceIn(moveFrom, tickIncrement);
		stackPx = moveTo;
	}
	LOGINFO("{} stack moved in from {} to {}", KTN::ORD::KOrderSide::toString(SIDE), oldPx.AsShifted(), stackPx->AsShifted());
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::moveOut(const Price newPx, const Price oldPx)
{
	const int tickDelta = static_cast<int>(-SideTraits<SIDE>::DeltaIn(newPx, oldPx).AsShifted() / tickIncrement.AsShifted());

	int numLevelsToMove;
	Price priceDelta;

	if (tickDelta < sideSettings.length)
	{
		numLevelsToMove = tickDelta;
		priceDelta = sideSettings.length * tickIncrement;
	}
	else
	{
		numLevelsToMove = sideSettings.length;
		priceDelta = std::min(tickDelta, StackOrderBook::SIZE / 2) * tickIncrement;
	}

	Price moveFrom = oldPx;
	for (int i = 0; i < numLevelsToMove; ++i)
	{
		const Price moveTo = SideTraits<SIDE>::PriceOut(moveFrom, priceDelta);
		if (likely(!SideTraits<SIDE>::IsBetter(moveTo, oldPx)))
		{
			orderBook.Move(moveTo, moveFrom);
			updateRequotePrice(moveFrom, moveTo);
		}
		else
		{
			LOGWARN("{} stack not moving level {} to {}, because new price is better than previous target stack price {}",
				KTN::ORD::KOrderSide::toString(SIDE), moveFrom.AsShifted(), moveTo.AsShifted(), oldPx.AsShifted());
		}
		moveFrom = SideTraits<SIDE>::PriceOut(moveFrom, tickIncrement);
		stackPx = SideTraits<SIDE>::PriceIn(moveTo, (sideSettings.length - 1) * tickIncrement);
	}
	LOGINFO("{} stack moved out from {} to {}", KTN::ORD::KOrderSide::toString(SIDE), oldPx.AsShifted(), stackPx->AsShifted());
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StackSide<SIDE>::refreshStack()
{
	bool result = false;
	orderBook.ClearForceCancel();
	if (orderBook.GetDefaultStackQuantity() > Quantity(0) && sideSettings.length > 0)
	{
		Price px = *stackPx;
		for (int i = 0; i < sideSettings.length; ++i)
		{
			LOGINFO("{} StackSide Refresh Level {}", KTN::ORD::KOrderSide::toString(SIDE), px.AsShifted());
			result |= orderBook.RefreshLevel(px);
			px = SideTraits<SIDE>::PriceOut(px, tickIncrement);
		}
	}
	return result;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StackSide<SIDE>::updateRequotePrice(const Price oldPx, const Price newPx)
{
	for (typename RequoteListType::iterator it = pendingRequotes.begin(); it != pendingRequotes.end(); ++it)
	{
		if (it->px == oldPx)
		{
			it->px = newPx;
			return;
		}
	}
}

}
