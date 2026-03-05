#include "SpreaderPricingModel.hpp"

#include <akl/Book.hpp>
#include <akl/BranchPrediction.hpp>
#include <akl/SideTraits.hpp>
#include <AlgoParams/AklStackerParams.hpp>

#include <KT01/Core/Log.hpp>
#include <Algos/AklStacker/Formatters.hpp>
#include <Algos/AklStacker/StrategyNotifier.hpp>

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
bool SpreaderPricingModel::HasStrikePrice() const
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	return strikePx[INDEX].has_value();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
int SpreaderPricingModel::TicksFromStrikePrice() const
{
	if (likely(spreadBook.TickIncrement > 0))
	{
		constexpr int INDEX = SideTraits<SIDE>::INDEX;
		constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
		if (strikePx[INDEX].has_value() && spreadBook.HasSide<OPP>())
		{
			const Price px = SideTraits<OPP>::PriceIn(spreadBook.BestPrice<OPP>(), spreadBook.TickIncrement);
			return SideTraits<SIDE>::DeltaIn(*strikePx[INDEX], px) / spreadBook.TickIncrement;
			//return SideTraits<SIDE>::DeltaIn(*strikePx[INDEX], spreadBook.BestPrice<SIDE>()) / spreadBook.TickIncrement;
		}
		else
		{
			return std::numeric_limits<int>::min();
		}
	}
	else
	{
		return std::numeric_limits<int>::min();
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
SpreaderPricingModel::QuoteStatus SpreaderPricingModel::CanQuote(const int index) const
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;

	if (spreaderPricingSettings.SideEnabled<SIDE>(index))
	{
		switch (pricingStatus[INDEX][index])
		{
			case WAITING:
				return YES_IF_LEAN_MET;
			case REMOVE_TICKS_MET:
				return NO;
			case ADD_TICKS_MET:
				return YES;
			default:
				return YES_IF_LEAN_MET;
		}
	}
	else
	{
		return YES_IF_LEAN_MET;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
const std::optional<Price> &SpreaderPricingModel::StrikePrice() const
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	return strikePx[INDEX];
}

template <KTN::ORD::KOrderSide::Enum SIDE>
bool SpreaderPricingModel::recalculate()
{
	bool result = false;

	if (likely(quoteBook.IsOpen() /*&& spreadBook.IsOpen()*/))
	{
		constexpr int INDEX = SideTraits<SIDE>::INDEX;

		bool flipIn = false;
		bool flipOut = false;
		bool resetStrike = false;

		Quantity quoteQty;

		if (likely(quoteBook.HasSide<SIDE>()))
		{
			const Price quotePx = quoteBook.BestPrice<SIDE>();
			const Price prevQuotePx = quoteBook.HasPreviousSide<SIDE>() ? quoteBook.PreviousBestPrice<SIDE>() : quotePx;
			quoteQty = quoteBook.BestQuantity<SIDE>();
			flipIn = SideTraits<SIDE>::IsBetter(quotePx, prevQuotePx);
			flipOut = SideTraits<SIDE>::IsWorse(quotePx, prevQuotePx);
			resetStrike = strikePx[INDEX].has_value() && quoteQty < spreaderPricingSettings.resetQty;
		}
		else
		{
			flipOut = true;
			quoteQty = Quantity(0);
		}

		if (spreadBook.HasSide<SIDE>())
		{
			if (flipIn)
			{
				strikePx[INDEX] = spreadBook.BestPrice<SIDE>();
				pricingStatus[INDEX][0] = WAITING;
				pricingStatus[INDEX][1] = WAITING;
				pricingStatus[INDEX][2] = WAITING;
				result = true;
				LOGINFO("Setting {} strike to {} on flip in", KTN::ORD::KOrderSide::toString(SIDE), *strikePx[INDEX]);
			}
			else if (flipOut)
			{
				strikePx[INDEX] = std::nullopt;
				pricingStatus[INDEX][0] = WAITING;
				pricingStatus[INDEX][1] = WAITING;
				pricingStatus[INDEX][2] = WAITING;
				result = true;
				LOGINFO("Clearing {} strike on flip out", KTN::ORD::KOrderSide::toString(SIDE));
			}
			else if (resetStrike)
			{
				if (strikePx[INDEX] && strikePx[INDEX] != spreadBook.BestPrice<SIDE>())
				{
					strikePx[INDEX] = spreadBook.BestPrice<SIDE>();
					LOGINFO("Resetting {} strike to {} due to resetQty", KTN::ORD::KOrderSide::toString(SIDE), *strikePx[INDEX]);
				}
				pricingStatus[INDEX][0] = WAITING;
				pricingStatus[INDEX][1] = WAITING;
				pricingStatus[INDEX][2] = WAITING;
				result = true;
			}

			constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
			if (spreadBook.HasSide<OPP>() && likely(spreadBook.TickIncrement > Price::FromShifted(0)))
			{
				if (strikePx[INDEX].has_value())
				{
					const Price currentPx = SideTraits<OPP>::PriceIn(spreadBook.BestPrice<OPP>(), spreadBook.TickIncrement);
					const int ticksFromLock = SideTraits<SIDE>::DeltaIn(*strikePx[INDEX], currentPx) / spreadBook.TickIncrement;
					updatePricingStatus<SIDE>(0, ticksFromLock, quoteQty);
					updatePricingStatus<SIDE>(1, ticksFromLock, quoteQty);
					updatePricingStatus<SIDE>(2, ticksFromLock, quoteQty);

					if (SideTraits<SIDE>::IsBetter(spreadBook.BestPrice<SIDE>(), *strikePx[INDEX]))
					{
						if (pricingStatus[INDEX][0] != PricingStatus::ADD_TICKS_MET
							&& pricingStatus[INDEX][1] != PricingStatus::ADD_TICKS_MET
							&& pricingStatus[INDEX][2] != PricingStatus::ADD_TICKS_MET)
						{
							strikePx[INDEX] = spreadBook.BestPrice<SIDE>();
							// pricingStatus[INDEX][0] = WAITING;
							// pricingStatus[INDEX][1] = WAITING;
							// pricingStatus[INDEX][2] = WAITING;
							result = true;
							LOGINFO("Resetting {} strike to {} due to better price", KTN::ORD::KOrderSide::toString(SIDE), *strikePx[INDEX]);
						}
					}
				}
				else
				{
					pricingStatus[INDEX][0] = WAITING;
					pricingStatus[INDEX][1] = WAITING;
					pricingStatus[INDEX][2] = WAITING;
				}
			}
			else
			{
				pricingStatus[INDEX][0] = WAITING;
				pricingStatus[INDEX][1] = WAITING;
				pricingStatus[INDEX][2] = WAITING;
			}
		}
		else
		{
			if (strikePx[INDEX].has_value())
			{
				strikePx[INDEX] = std::nullopt;
				pricingStatus[INDEX][0] = WAITING;
				pricingStatus[INDEX][1] = WAITING;
				pricingStatus[INDEX][2] = WAITING;
				result = true;
				LOGINFO("Clearing {} strike due to no market data", KTN::ORD::KOrderSide::toString(SIDE));
			}
		}
	}

	return result;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool SpreaderPricingModel::overrideStrikePrice(const std::optional<Price> &price, const int maxTicks)
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	if (price != strikePx[INDEX])
	{
		if (price)
		{
			if (spreadBook.HasSide<SIDE>())
			{
				const Price bestPx = spreadBook.BestPrice<SIDE>();
				const int tickDiff = (bestPx - *price) / spreadBook.TickIncrement;
				if (std::abs(tickDiff) <= maxTicks * 2) // Per Amol check if the new price is within 2x the add ticks from the best price, to prevent fat fingering a completely invalid price
				{
					LOGINFO("Spreader {} strike override from {} to {}",
						KTN::ORD::KOrderSide::toString(SIDE),
						strikePx[INDEX] ? *strikePx[INDEX] : Price::Min(),
						*price);
					strikePx[INDEX] = price;
					return true;
				}
				else
				{
					LOGWARN("Spreader {} received invalid override {} | best={} | maxTicks={} | tickDiff={}", KTN::ORD::KOrderSide::toString(SIDE), *price, bestPx, maxTicks, tickDiff);
					const std::string msg = fmt::format("Spreader rejected {} strike override of {} for being too far from market",
						KTN::ORD::KOrderSide::toString(SIDE),
						*price);
					notifier.Notify(std::move(msg), KTN::notify::MsgLevel::Enum::WARN, KTN::notify::MsgType::Enum::ALERT);
					return true;
				}
			}
			else
			{
				LOGWARN("Spreader {} rejected override {} due to no market data", KTN::ORD::KOrderSide::toString(SIDE), *price);
				const std::string msg = fmt::format("Spreader rejected {} strike override of {} due to no market data",
					KTN::ORD::KOrderSide::toString(SIDE),
					*price);
				notifier.Notify(std::move(msg), KTN::notify::MsgLevel::Enum::WARN, KTN::notify::MsgType::Enum::ALERT);
				return true;
			}
		}
		else
		{
			LOGINFO("Spreader {} strike of {} cleared",
				KTN::ORD::KOrderSide::toString(SIDE),
				strikePx[INDEX] ? *strikePx[INDEX] : Price::Min());
			strikePx[INDEX].reset();
			return true;
		}
	}
	else
	{
		return false;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool SpreaderPricingModel::updatePricingStatus(const int index, const int ticksFromStrike, const Quantity quoteQty)
{
	constexpr int SIDE_INDEX = SideTraits<SIDE>::INDEX;

	if (quoteQty >= spreaderPricingSettings.lockQty[index])
	{
		const PricingStatus prevStatus = pricingStatus[SIDE_INDEX][index];
		pricingStatus[SIDE_INDEX][index] = LOCK_QTY_MET;
		return pricingStatus[SIDE_INDEX][index] != prevStatus;
	}

	switch (pricingStatus[SIDE_INDEX][index])
	{
		case WAITING:
		case REMOVE_TICKS_MET:
			if (ticksFromStrike >= addTicks[index] && !spreadBook.IsBookGapped())
			{
				pricingStatus[SIDE_INDEX][index] = ADD_TICKS_MET;
				TicksChanged |= randomizeAddTicks(index);
				return true;
			}
			break;
		case ADD_TICKS_MET:
			if (ticksFromStrike < -removeTicks[index])
			{
				pricingStatus[SIDE_INDEX][index] = REMOVE_TICKS_MET;
				TicksChanged |= randomizeRemoveTicks(index);
				return true;
			}
			break;
		case LOCK_QTY_MET:
			break;
	}
	return false;
}

}