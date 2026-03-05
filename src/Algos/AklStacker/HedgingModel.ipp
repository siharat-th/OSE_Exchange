#pragma once

#include "HedgingModel.hpp"
#include "PriceToOrderMapSide.hpp"
#include "FillInfo.hpp"
#include "OrderHelpers.hpp"
#include "Order.hpp"
#include "AdoptLevelInfo.hpp"
#include "StrategyNotifier.hpp"
#include "StopHandler.hpp"
#include <akl/Quantity.hpp>
#include <AlgoParams/AklStackerParams.hpp>

#include <chrono>
#include <list>
#include <akl/SideTraits.hpp>
#include "PositionManager.hpp"
#include <akl/Book.hpp>
#include <akl/BranchPrediction.hpp>
#include <sstream>

namespace akl::stacker
{

// Side is in terms of stop aggessing side
template<KTN::ORD::KOrderSide::Enum SIDE>
bool HedgingModel::SendStop(const bool strategyRunning)
{
	constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
	if (book.HasSide<OPP>())
	{
		const Price bestPx = book.BestPrice<OPP>();
		StopInfo *stopInfo = stopHandler.GetStop<SIDE>(bestPx);
		if (stopInfo)
		{
			if (strategyRunning)
			{
				cancelQuoteOrders<OPP>(bestPx, false);
				return doOffset<OPP>(bestPx, bestPx, true, Quantity(0), true);
			}
			else
			{
				bool result = sendStop(stopInfo->orders);
				stopHandler.RemoveStop<SIDE>(bestPx, true, true);
				return result;
			}
		}
	}

	return false;
}

// Side is in terms of position
template<KTN::ORD::KOrderSide::Enum SIDE>
void HedgingModel::recalculate()
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
	constexpr int OPP_INDEX = SideTraits<OPP>::INDEX;

	if (book.HasSide<SIDE>())
	{
		const Price bestPx = book.BestPrice<SIDE>();
		if (book.HasPreviousSide<SIDE>())
		{
			const Price prevPx = book.PreviousBestPrice<SIDE>();
			if (SideTraits<SIDE>::IsBetter(bestPx, prevPx))
			{
				const Quantity sidePosition = SideTraits<SIDE>::SidePosition(pm.Position());
				if (sidePosition > Quantity(0))
				{
					lastTickUp[INDEX] = time.Now();
					payupAllowedPx[OPP_INDEX] = SideTraits<SIDE>::PriceOut(bestPx, tickIncrement);
				}
				canHedge[OPP_INDEX] = true;
			}
			else
			{
				canHedge[OPP_INDEX] |= (book.BestQuantity<SIDE>() != book.PreviousBestQuantity<SIDE>()) || (bestPx != prevPx);
			}
		}
		else
		{
			const Quantity sidePosition = SideTraits<SIDE>::SidePosition(pm.Position());
			if (sidePosition > Quantity(0))
			{
				lastTickUp[INDEX] = time.Now();
				payupAllowedPx[OPP_INDEX] = SideTraits<SIDE>::PriceOut(bestPx, tickIncrement);
			}
			canHedge[OPP_INDEX] = true;
		}
	}
	else
	{
		lastTickUp[INDEX] = Time::Min();
	}
}

// Side is in terms of position
template<KTN::ORD::KOrderSide::Enum SIDE>
bool HedgingModel::handleOffset(const std::optional<FillInfo> &fillInfo, const std::optional<CanceledHedgeInfo> &canceledHedgeInfo)
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
	constexpr int OPP_INDEX = SideTraits<OPP>::INDEX;

	if (book.HasSide<SIDE>())
	{
		const Price bestPx = book.BestPrice<SIDE>();
		const Quantity bestQty = book.BestQuantity<SIDE>();
		const QuoteSideSettings &sideSettings = settings.GetSideSettings<SIDE>();
		const std::optional<ProrataTarget> &target = getTarget<SIDE>();
		const StopInfo *stopInfo = stopHandler.GetStop<OPP>(bestPx);
		if (fillInfo && target && target->ltpQuantity > Quantity(0) && fillInfo->estimatedTrade >= target->ltpQuantity)
		{
			cancelQuoteOrders<SIDE>(fillInfo->px, true); // TODO will price always be valid??
			const Quantity position = SideTraits<SIDE>::SidePosition(pm.Position());
			const Quantity additionalHedge = target->addHedgeQuantity;
			if (position > Quantity(0) || additionalHedge > Quantity(0))
			{
				canHedge[OPP_INDEX] = true;
				doOffset<SIDE>(fillInfo->px, fillInfo->px, stopInfo != nullptr && stopInfo->sendOnTrade, additionalHedge);
				if (additionalHedge > Quantity(0))
				{
					disableAdditionalHedge<SIDE>();
				}
			}

			std::stringstream ss;
			ss << "Large estimated trade on " << KTN::ORD::KOrderSide::toString(SIDE) << " in " << symbol << ". Total size: " << fillInfo->estimatedTrade;
			notifier.Notify(ss.str(), KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
			LOGWARN("Canceling {} side due to large estimated trade: {}@{}", KTN::ORD::KOrderSide::toString(SIDE), fillInfo->estimatedTrade.value(), fillInfo->px.AsShifted());
		}
		else if (bestQty < getPayup<SIDE>())
		{
			if (time.Now() >= lastTickUp[INDEX] + Millis(sideSettings.timeoutCover)
				|| !SideTraits<SIDE>::IsBetter(bestPx, payupAllowedPx[OPP_INDEX])
				|| (fillInfo && fillInfo->side == SIDE && !SideTraits<SIDE>::IsBetter(bestPx, fillInfo->px)))
			{
				return doOffset<SIDE>(bestPx, bestPx, stopInfo != nullptr && bestQty < stopInfo->triggerQuantity);
			}
			else
			{
				return checkStop<SIDE>();
			}
		}
		else if (stopInfo != nullptr && bestQty < stopInfo->triggerQuantity)
		{
			if (time.Now() >= lastTickUp[INDEX] + Millis(sideSettings.timeoutCover)
				|| !SideTraits<SIDE>::IsBetter(bestPx, payupAllowedPx[OPP_INDEX])
				|| (fillInfo && fillInfo->side == SIDE && !SideTraits<SIDE>::IsBetter(bestPx, fillInfo->px)))
			{
				return doOffset<SIDE>(bestPx, bestPx, true);
			}
			else
			{
				return checkStop<SIDE>();
			}
		}
		else if (fillInfo)
		{
			if (fillInfo->scratchImmediately)
			{
				return doOffset<SIDE>(fillInfo->px, fillInfo->px, stopInfo != nullptr && stopInfo->sendOnTrade);
			}
			else if (fillInfo->side == SIDE && SideTraits<SIDE>::IsWorse(bestPx, fillInfo->px))
			{
				const Price targetPx = SideTraits<SIDE>::PriceIn(bestPx, tickIncrement);
				return doOffset<SIDE>(targetPx, fillInfo->px, stopInfo != nullptr && stopInfo->sendOnTrade); // TODO what to do with stop send here?
			}
		}
		else if (canceledHedgeInfo)
		{
			const Price targetPx = SideTraits<SIDE>::PriceIn(bestPx, tickIncrement);
			if (time.Now() >= lastTickUp[INDEX] + Millis(sideSettings.timeoutCover) || !SideTraits<SIDE>::IsBetter(targetPx, payupAllowedPx[OPP_INDEX]))
			{
				if (canceledHedgeInfo->side == OPP && SideTraits<SIDE>::IsWorse(targetPx, canceledHedgeInfo->px))
				{
					return doOffset<SIDE>(targetPx, canceledHedgeInfo->px, false);
				}
			}
		}
		else if (book.HasPreviousSide<SIDE>())
		{
			const Price prevPx = book.PreviousBestPrice<SIDE>();
			if (SideTraits<SIDE>::IsWorse(bestPx, prevPx))
			{
				const Price targetPx = SideTraits<SIDE>::PriceIn(bestPx, tickIncrement);
				if (time.Now() >= lastTickUp[INDEX] + Millis(sideSettings.timeoutCover) || !SideTraits<SIDE>::IsBetter(targetPx, payupAllowedPx[OPP_INDEX]))
				{
					return doOffset<SIDE>(SideTraits<SIDE>::PriceIn(bestPx, tickIncrement), prevPx, true);
				}
			}
			else
			{
				return MaintainExistingOrders(SIDE);
			}
		}
		else
		{
			return MaintainExistingOrders(SIDE);
		}
	}
	return false;
}

// Side is in terms of position
template<KTN::ORD::KOrderSide::Enum SIDE>
void HedgingModel::maintainOrders(
	const Price targetPx,
	OrderListType &ordersToCancel,
	OrderListType &ordersGivenToStack,
	OrderReduceListType &ordersToReduce,
	Quantity &pendingHedge)
{
	constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;

	using SideMapType = PriceToOrderMapSide<typename SideTraits<OPP>::BestPriceComparerType>;
	using LevelType = typename SideMapType::LevelType;

	const QuoteSideSettings &sideSettings = settings.GetSideSettings<SIDE>();

	SideMapType &sideMap = map.GetSideMap<OPP>();
	for (typename SideMapType::IteratorType mapIt = sideMap.Begin(); mapIt != sideMap.End(); ++mapIt)
	{
		const Price px = mapIt->first;
		const Quantity hedgeAllowedAtPx = pm.GetHedgeQuantity<OPP>(px, sideSettings.maxPayupTicks);
		const Quantity hedgeAllowedAtBetterPx = pm.GetHedgeQuantity<OPP>(SideTraits<OPP>::PriceIn(px, tickIncrement), sideSettings.maxPayupTicks);
		const Quantity maxPayupAtPx = hedgeAllowedAtPx > hedgeAllowedAtBetterPx ? hedgeAllowedAtPx - hedgeAllowedAtBetterPx : Quantity(0);
		LevelType &level = mapIt->second;

		Quantity hedgeQtyAtPx { 0 };

		for (typename LevelType::iterator levelIt = level.begin(); levelIt != level.end(); ++levelIt)
		{
			Order *o = *levelIt;
			if (SideTraits<OPP>::IsBetter(targetPx, px) && hedgeQtyAtPx >= maxPayupAtPx)
			{
				if (giveOrderToStack(o))
				{
					ordersGivenToStack.push_back(o);
					continue;
				}
				else
				{
					ordersToCancel.push_back(o);
				}
			}
			else if (SideTraits<OPP>::IsBetter(targetPx, px) && hedgeQtyAtPx + o->WorkingQuantity() > maxPayupAtPx)
			{
				const Quantity newQty = maxPayupAtPx - hedgeQtyAtPx;
				if (likely(newQty > Quantity(0)))
				{
					if (newQty < o->WorkingQuantity() && newQty + o->filledQuantity < o->targetQuantity)
					{
						const std::optional<Price> stackTarget = getStackTarget<OPP>();
						if (settings.GetSideSettings<SIDE>().stackSideSettings[0].length < 1
							|| !stackTarget
							|| SideTraits<OPP>::IsBetter(px, *stackTarget))
						{
							ordersToReduce.push_back({ o, newQty });
						}
					}
				}
				else
				{
					if (giveOrderToStack(o))
					{
						ordersGivenToStack.push_back(o);
						continue;
					}
					else
					{
						ordersToCancel.push_back(o);
					}
				}
			}
			else if (pendingHedge >= hedgeAllowedAtPx)
			{
				if (giveOrderToStack(o))
				{
					ordersGivenToStack.push_back(o);
					continue;
				}
				else
				{
					ordersToCancel.push_back(o);
				}
			}
			else if (pendingHedge + o->WorkingQuantity() > hedgeAllowedAtPx)
			{
				const Quantity newQty = hedgeAllowedAtPx - pendingHedge;
				if (likely(newQty > Quantity(0)))
				{
					if (newQty < o->WorkingQuantity() && newQty + o->filledQuantity < o->targetQuantity)
					{
						const std::optional<Price> stackTarget = getStackTarget<OPP>();
						if (settings.GetSideSettings<SIDE>().stackSideSettings[0].length < 1
							|| !stackTarget
							|| SideTraits<OPP>::IsBetter(px, *stackTarget))
						{
							ordersToReduce.push_back({ o, newQty });
						}
					}
				}
				else
				{
					if (giveOrderToStack(o))
					{
						ordersGivenToStack.push_back(o);
						continue;
					}
					else
					{
						ordersToCancel.push_back(o);
					}
				}
			}
			hedgeQtyAtPx += o->WorkingQuantity();
			pendingHedge += o->WorkingQuantity();
		}
	}
}

// Side is in terms of position
template<KTN::ORD::KOrderSide::Enum SIDE>
bool HedgingModel::doOffset(const Price targetPx, const Price prevPx, const bool sendStop, const Quantity additionalHedge, const bool stopTriggeredFromLink)
{
	bool result = false;

	constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
	constexpr int OPP_INDEX = SideTraits<OPP>::INDEX;

	using SideMapType = PriceToOrderMapSide<typename SideTraits<OPP>::BestPriceComparerType>;
	using LevelType = typename SideMapType::LevelType;

	const QuoteSideSettings &sideSettings = settings.GetSideSettings<SIDE>();
	const Quantity hedgeQty = pm.GetHedgeQuantity<OPP>(targetPx, sideSettings.maxPayupTicks);

	Quantity clearedQty { 0 };

	if (hedgeQty < SideTraits<SIDE>::SidePosition(pm.Position()))
	{
		clearedQty = pm.ClearMaxPayupPosition<SIDE>(targetPx, sideSettings.maxPayupTicks);
		if constexpr (SIDE == KTN::ORD::KOrderSide::Enum::SELL)
		{
			clearedQty = -clearedQty;
		}
		LOGWARN("Cleared {} qty from position due to max payup ticks | newPos={}", clearedQty.value(), pm.Position().value());
	}

	const Quantity sidePosition = SideTraits<SIDE>::SidePosition(pm.Position());
	SideMapType &sideMap = map.GetSideMap<OPP>();

	OrderListType ordersToCancel;
	OrderListType ordersGivenToStack;
	OrderReduceListType ordersToReduce;
	Quantity pendingHedge { 0 };

	maintainOrders<SIDE>(
		targetPx,
		ordersToCancel,
		ordersGivenToStack,
		ordersToReduce,
		pendingHedge);

	const StopInfo *stopInfo = stopHandler.GetStop<OPP>(targetPx);
	const bool hasStop = stopInfo != nullptr && sendStop;
	bool stopSent = false;

	if (sidePosition > pendingHedge || additionalHedge > Quantity(0) || hasStop)
	{
		const Quantity hedgeNeeded = std::min(sidePosition - pendingHedge, hedgeQty);
		if (((hedgeNeeded > Quantity(0) || additionalHedge > Quantity(0)) && canHedge[OPP_INDEX]) || hasStop)
		{
			result |= sendOffset(OPP, hedgeNeeded, targetPx, OPP_INDEX, additionalHedge, sendStop ? stopInfo : nullptr);
			LOGINFO("Submitted hedge | position={} | hedgeNeeded={} | pendingHedge={} | additionalHedge={}",
				pm.Position().value(), hedgeNeeded.value(), pendingHedge.value(), additionalHedge.value());
			stopSent = hasStop;
		}
		if (SideTraits<SIDE>::IsWorse(targetPx, prevPx))
		{
			for (Price px = SideTraits<SIDE>::PriceIn(targetPx, tickIncrement); !SideTraits<SIDE>::IsBetter(px, prevPx); px = SideTraits<SIDE>::PriceIn(px, tickIncrement))
			{
				const Quantity hedgeAllowedAtPx = pm.GetHedgeQuantity<OPP>(px, sideSettings.maxPayupTicks);
				const Quantity hedgeAllowedAtBetterPx = pm.GetHedgeQuantity<OPP>(SideTraits<OPP>::PriceIn(px, tickIncrement), sideSettings.maxPayupTicks);
				const Quantity maxPayupAtPx = hedgeAllowedAtPx > hedgeAllowedAtBetterPx ? hedgeAllowedAtPx - hedgeAllowedAtBetterPx : Quantity(0);
				if (maxPayupAtPx > Quantity(0))
				{
					typename SideMapType::IteratorType mapIt = sideMap.GetLevel(px);
					Quantity sumAtLevel { 0 };
					if (mapIt != sideMap.End())
					{
						LevelType &level = mapIt->second;
						for (typename LevelType::const_iterator levelIt = level.cbegin(); levelIt != level.cend(); ++levelIt)
						{
							const Order *o = *levelIt;
							sumAtLevel += o->WorkingQuantity();
						}
					}
					if (sumAtLevel < maxPayupAtPx)
					{
						const Quantity hedgeNeeded = maxPayupAtPx - sumAtLevel;
						result |= sendOffset(OPP, hedgeNeeded, px, OPP_INDEX);
						LOGINFO("Submitted hedge | position={} | hedgeNeeded={} | pendingHedge={}",
							pm.Position().value(), hedgeNeeded.value(), pendingHedge.value());
					}
				}
			}
		}
	}

	for (OrderListType::iterator it = ordersToCancel.begin(); it != ordersToCancel.end(); ++it)
	{
		Order *o = *it;
		cancel(o);
	}
	for (OrderReduceListType::iterator it = ordersToReduce.begin(); it != ordersToReduce.end(); ++it)
	{
		modify(it->o, it->newQty + it->o->filledQuantity);
	}
	for (OrderListType::iterator it = ordersGivenToStack.begin(); it != ordersGivenToStack.end(); ++it)
	{
		Order *o = *it;
		RemoveOrder(o);
	}
	if (!ordersGivenToStack.empty())
	{
		queueStackInfoUpdate();
	}

	if (clearedQty != Quantity(0))
	{
		std::ostringstream ss;
		ss << symbol << " stacker cleared position of " << clearedQty << " due to max payup ticks";
		notifier.Notify(ss.str(), KTN::notify::MsgLevel::Enum::INFO, KTN::notify::MsgType::Enum::NOTIFY);
		queuePositionUpdate();
	}

	if (stopSent)
	{
		stopHandler.RemoveStop<OPP>(targetPx, true, stopTriggeredFromLink);
	}

	return result;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void HedgingModel::cancelSideOrders()
{
	using SideMapType = PriceToOrderMapSide<typename SideTraits<SIDE>::BestPriceComparerType>;
	using LevelType = typename SideMapType::LevelType;

	SideMapType &sideMap = map.GetSideMap<SIDE>();
	for (typename SideMapType::IteratorType mapIt = sideMap.Begin(); mapIt != sideMap.End(); ++mapIt)
	{
		LevelType &level = mapIt->second;
		for (typename LevelType::iterator levelIt = level.begin(); levelIt != level.end(); ++levelIt)
		{
			Order *o = *levelIt;
			cancel(o);
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
size_t HedgingModel::adopt(std::list<Order *> &adoptedOrders, const AdoptLevelInfo *levels, const int count, const int stackIndex)
{
	using SideMapType = PriceToOrderMapSide<typename SideTraits<SIDE>::BestPriceComparerType>;
	using LevelType = typename SideMapType::LevelType;

	SideMapType &sideMap = map.GetSideMap<SIDE>();

	OrderListType ordersToCancel;
	OrderListType ordersGivenToStack;

	for (typename SideMapType::IteratorType mapIt = sideMap.Begin(); mapIt != sideMap.End(); ++mapIt)
	{
		const Price levelPx = mapIt->first;

		bool matchesPx = true;
		bool stack2Adopt = stackIndex == 1;
		bool stack1Adopt = !stack2Adopt;
		if (levels != nullptr && count > 0)
		{
			matchesPx = false;
			for (int i = 0; i < count; ++i)
			{
				const AdoptLevelInfo &ali = levels[i];
				if (SIDE == ali.side && levelPx == ali.px)
				{
					matchesPx = true;
					stack1Adopt = ali.stackAdoptAllowed[0];
					stack2Adopt = ali.stackAdoptAllowed[1];
				}
			}
		}

		if (matchesPx)
		{
			LevelType &level = mapIt->second;
			for (typename LevelType::iterator levelIt = level.begin(); levelIt != level.end(); ++levelIt)
			{
				Order *o = *levelIt;
				if (giveOrderToStack(o, stack1Adopt, stack2Adopt))
				{
					ordersGivenToStack.push_back(o);
				}
				else
				{
					ordersToCancel.push_back(o);
				}
			}
		}
		else
		{
			LOGINFO("Not adopting order {} because price {} is not in list", (*mapIt->second.begin())->orderReqId, levelPx.AsShifted());
		}
	}

	if (settings.cancelUnadoptedOrders)
	{
		for (OrderListType::iterator it = ordersToCancel.begin(); it != ordersToCancel.end(); ++it)
		{
			Order *o = *it;
			cancel(o);
		}
	}

	for (OrderListType::iterator it = ordersGivenToStack.begin(); it != ordersGivenToStack.end(); ++it)
	{
		Order *o = *it;
		RemoveOrder(o);
	}

	return ordersGivenToStack.size();
}

// Side is in terms of book side
template<KTN::ORD::KOrderSide::Enum SIDE>
bool HedgingModel::checkStop()
{
	if (likely(book.HasSide<SIDE>()))
	{
		const Price bestPx = book.BestPrice<SIDE>();
		constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
		StopInfo *stopInfo = stopHandler.GetStop<OPP>(bestPx);
		if (stopInfo != nullptr)
		{
			const Quantity bestQty = book.BestQuantity<SIDE>();
			if (bestQty < stopInfo->triggerQuantity)
			{
				cancelQuoteOrders<SIDE>(bestPx, false);
				sendStop(stopInfo->orders);
				stopHandler.RemoveStop<OPP>(bestPx, true);
				return true;
			}
		}
	}
	return false;
}

// Side is in terms of book side
template<KTN::ORD::KOrderSide::Enum SIDE>
bool HedgingModel::checkMissedStops()
{
	bool result = false;
	if (likely(book.HasSide<SIDE>()))
	{
		const Price bestPx = book.BestPrice<SIDE>();
		if (!book.HasPreviousSide<SIDE>() || SideTraits<SIDE>::IsWorse(bestPx, book.PreviousBestPrice<SIDE>()))
		{
			std::vector<Price> stopPrices;
			constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
			using IteratorType = StopHandler::IteratorType<OPP>;
			for (IteratorType it = stopHandler.Begin<OPP>(); it != stopHandler.End<OPP>(); ++it)
			{
				StopInfo &stopInfo = it->second;
				if (SideTraits<SIDE>::IsWorse(bestPx, stopInfo.price))
				{
					cancelQuoteOrders<SIDE>(stopInfo.price, false);
					sendStop(stopInfo.orders);
					result = true;
					stopPrices.push_back(stopInfo.price);
				}
			}
			for (const Price &px : stopPrices)
			{
				stopHandler.RemoveStop<OPP>(px, true);
			}
		}
	}
	return result;
}

}
