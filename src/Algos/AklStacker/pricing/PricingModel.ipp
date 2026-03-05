#pragma once

#include "PricingModel.hpp"
#include "SpreaderPricingModel.hpp"
#include "../StopHandler.hpp"
#include <akl/BranchPrediction.hpp>
#include <akl/SideTraits.hpp>
#include <akl/Book.hpp>

#include <AlgoParams/AklStackerParams.hpp>

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
void PricingModel::TickBack(const Price px)
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	if (stackTargetPx[INDEX])
	{
		if (SideTraits<SIDE>::IsBetter(*stackTargetPx[INDEX], px))
		{
			stackTargetPx[INDEX] = px;
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
PricingModel::Targets PricingModel::recalculate(const bool isRunning, const std::optional<Price> &stackPx, const bool canMove)
{
	if (likely(book.HasSide<SIDE>()))
	{
		constexpr int INDEX = SideTraits<SIDE>::INDEX;
		const QuoteSideSettings &sideSettings = quoteSettings.GetSideSettings<SIDE>();
		const Price bestPx = book.BestPrice<SIDE>();
		const Quantity bestQty = book.BestQuantity<SIDE>();
		const Price oneTickBackPx = SideTraits<SIDE>::PriceOut(bestPx, tickIncrement);

		if (likely(book.HasPreviousSide<SIDE>()))
		{
			const Price prevPx = book.PreviousBestPrice<SIDE>();
			if (SideTraits<SIDE>::IsBetter(bestPx, prevPx))
			{
				lastMoveIn[INDEX] = time.Now();
				quickAddStatus[INDEX] = QuickAddStatus::WAITING;
			}
			else if (SideTraits<SIDE>::IsWorse(bestPx, prevPx))
			{
				lastMoveIn[INDEX] = Time::Min();
				quickAddStatus[INDEX] = QuickAddStatus::EXPIRED;
			}
			else if (time.Now() >= lastMoveIn[INDEX] + Millis(sideSettings.stackSideSettings[stackIndex].quickAddTime))
			{
				quickAddStatus[INDEX] = QuickAddStatus::EXPIRED;
			}
		}
		else
		{
			quickAddStatus[INDEX] = QuickAddStatus::EXPIRED;
		}

		// If spreader is enabled and lockQty is lower than lean, use lockQty instead of lean
		const Quantity lean = !hasSpreader || !spreaderPricingModel.GetSettings().SideEnabled<SIDE>(stackIndex)
			? sideSettings.stackSideSettings[stackIndex].lean
			: std::min(sideSettings.stackSideSettings[stackIndex].lean, spreaderPricingModel.GetSettings().lockQty[stackIndex]);

		std::optional<Price> px = book.CalculateLean<SIDE>(lean);

		if (unlikely(!px))
		{
			if (book.IsLtpAdjusted() && stackTargetPx[INDEX])
			{
				px = *stackTargetPx[INDEX];
			}
			else
			{
				px = SideTraits<SIDE>::PriceOut(bestPx, noLeanAdj);
			}
		}

		bool noAggressing;
		int manualAdj;

		if (sideSettings.stackSideSettings[stackIndex].altManualAdjPrice && !SideTraits<SIDE>::IsWorse(bestPx, *sideSettings.stackSideSettings[stackIndex].altManualAdjPrice))
		{
			noAggressing = false;
			manualAdj = sideSettings.stackSideSettings[stackIndex].altManualAdj;
		}
		else
		{
			noAggressing = sideSettings.stackSideSettings[stackIndex].noAggressing;
			manualAdj = sideSettings.stackSideSettings[stackIndex].manualAdj;
		}

		const SpreaderPricingModel::QuoteStatus spreaderQuoteStatus = spreaderPricingModel.CanQuote<SIDE>(stackIndex);
		if (spreaderQuoteStatus == SpreaderPricingModel::QuoteStatus::YES)
		{
			if (manualAdj == 0 && px == oneTickBackPx)
			{
				px = bestPx;
			}
		}

		if (manualAdj > 0)
		{
			const Price manualAdjPx = SideTraits<SIDE>::PriceOut(bestPx, manualAdj * tickIncrement);
			px = SideTraits<SIDE>::PickWorst(*px, manualAdjPx);
			quickAddStatus[INDEX] = QuickAddStatus::EXPIRED;
		}
		else if (*px == SideTraits<SIDE>::PriceOut(bestPx, tickIncrement))
		{
			if (quickAddStatus[INDEX] == QuickAddStatus::WAITING)
			{
				if (sideSettings.stackSideSettings[stackIndex].quickAddEnabled
					&& sideSettings.stackSideSettings[stackIndex].quickAddLean > Quantity(0)
					&& sideSettings.stackSideSettings[stackIndex].quickAddTime > 0)
				{
					if (bestQty >= sideSettings.stackSideSettings[stackIndex].quickAddLean)
					{
						quickAddStatus[INDEX] = QuickAddStatus::MET;
						px = bestPx;
					}
				}
				else
				{
					quickAddStatus[INDEX] = QuickAddStatus::EXPIRED;
				}
			}
			else if (quickAddStatus[INDEX] == QuickAddStatus::MET)
			{
				if (sideSettings.stackSideSettings[stackIndex].quickAddEnabled
					&& sideSettings.stackSideSettings[stackIndex].quickAddLean > Quantity(0)
					&& sideSettings.stackSideSettings[stackIndex].quickAddTime > 0
					&& bestQty >= sideSettings.stackSideSettings[stackIndex].quickAddLean)
				{
					quickAddStatus[INDEX] = QuickAddStatus::MET;
					px = bestPx;
				}
				else
				{
					quickAddStatus[INDEX] = QuickAddStatus::EXPIRED;
				}
			}
		}

		if (spreaderQuoteStatus == SpreaderPricingModel::QuoteStatus::NO)
		{
			if (px == bestPx)
			{
				px = oneTickBackPx;
			}
		}

		constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;

		if (!SideTraits<SIDE>::IsWorse(*px, bestPx))
		{
			const Quantity sidePosition = SideTraits<SIDE>::SidePosition(pm.Position());
			if (sidePosition > Quantity(0))
			{
				const std::optional<Price> innerPx = pm.GetInnermostPrice<SIDE>();
				if (innerPx && SideTraits<SIDE>::IsWorse(*innerPx, bestPx))
				{
					if (bestQty < sideSettings.winnerPayup)
					{
						px = SideTraits<SIDE>::PriceOut(bestPx, tickIncrement);
						lastMoveIn[INDEX] = Time::Min();
					}
				}
				else if (bestQty < sideSettings.payup)
				{
					px = SideTraits<SIDE>::PriceOut(bestPx, tickIncrement);
					lastMoveIn[INDEX] = Time::Min();
				}
			}

			if (!SideTraits<SIDE>::IsWorse(*px, bestPx))
			{
				const StopInfo *stopInfo = stopHandler.GetStop<OPP>(bestPx);
				if (stopInfo != nullptr)
				{
					if (bestQty < stopInfo->triggerQuantity || bestQty < stopInfo->lean)
					{
						px = SideTraits<SIDE>::PriceOut(bestPx, tickIncrement);
						lastMoveIn[INDEX] = Time::Min();
					}
				}
			}
		}

		const std::optional<Price> lastTriggeredStopPx = stopHandler.LastTriggeredStop<OPP>();
		if (lastTriggeredStopPx)
		{
			if (!SideTraits<SIDE>::IsWorse(*px, *lastTriggeredStopPx))
			{
				px = SideTraits<SIDE>::PriceOut(*lastTriggeredStopPx, tickIncrement);
			}
		}

		if (book.HasSide<OPP>())
		{
			const Price oppPx = book.BestPrice<OPP>();
			if (unlikely(!SideTraits<SIDE>::IsWorse(*px, oppPx)))
			{
				px = SideTraits<SIDE>::PriceOut(oppPx, tickIncrement);
			}
		}

		if (sideSettings.stackSideSettings[stackIndex].insidePrice && SideTraits<SIDE>::IsBetter(*px, *sideSettings.stackSideSettings[stackIndex].insidePrice))
		{
			px = *sideSettings.stackSideSettings[stackIndex].insidePrice;
		}

		Price mpx = *px;
		if (likely(stackTargetPx[INDEX]))
		{
			if (SideTraits<SIDE>::IsBetter(*px, *stackTargetPx[INDEX]))
			{
				if (isRunning && noAggressing && likely(canMove) && likely(couldMove))
				{
					px = *stackTargetPx[INDEX];
				}
				else if (time.Now() < lastMoveOut[INDEX] + Millis(quoteSettings.quoteTimeout) && likely(couldMove))
				{
					px = *stackTargetPx[INDEX];
					if (maintainPx[INDEX])
					{
						mpx = *maintainPx[INDEX];
					}
				}
			}
			else if (SideTraits<SIDE>::IsWorse(*px, *stackTargetPx[INDEX]))
			{
				lastMoveOut[INDEX] = time.Now();
			}
		}
		if (likely(stackPx && canMove) && isRunning && noAggressing)
		{
			if (SideTraits<SIDE>::IsBetter(*px, *stackPx))
			{
				px = *stackPx;
			}
		}

		return { px, mpx };
	}
	else
	{
		return { std::nullopt, std::nullopt };
	}
}

}
