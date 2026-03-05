#pragma once

#include "ProrataPricingModel.hpp"
#include <akl/Quantity.hpp>
#include <akl/Book.hpp>
#include <akl/SideTraits.hpp>
#include <akl/BranchPrediction.hpp>
#include <optional>

#include "SpreaderPricingModel.hpp"

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataPricingModel::SetSideProfile(const ProrataProfileUpdateMessage &profile)
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	profiles[INDEX] = profile;
}


template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataPricingModel::ClearSideProfile()
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	profiles[INDEX].reset();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataPricingModel::SetAlternateSideProfile(const ProrataProfileUpdateMessage &profile)
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	altProfiles[INDEX] = profile;
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void ProrataPricingModel::ClearAlternateSideProfile()
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	altProfiles[INDEX].reset();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool ProrataPricingModel::DisableAdditionalHedge()
{
	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	if (likely(profiles[INDEX]))
	{
		bool result = false;
		for (int i = 0; i < profiles[INDEX]->levelCount; ++i)
		{
			if (profiles[INDEX]->addHedgeEnabled[i])
			{
				profiles[INDEX]->addHedgeEnabled[i] = false;
				result = true;
			}
		}
		return result;
	}
	else
	{
		return false;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
std::optional<ProrataTarget> ProrataPricingModel::recalculate(const std::optional<Price> &bestQuotePx)
{
	const Timestamp now = time.Now();
	constexpr int INDEX = SideTraits<SIDE>::INDEX;

	if (unlikely(!book.HasSide<SIDE>()))
	{
		if (targets[INDEX])
		{
			lastMoveBack[INDEX] = now;
		}
		lastMoveIn[INDEX] = Time::Min();
		return std::nullopt;
	}

	const Price bestPx = book.BestPrice<SIDE>();
	const QuoteSideSettings &sideSettings = quoteSettings.GetSideSettings<SIDE>();

	if (altProfiles[INDEX].has_value())
	{
		if (book.HasPreviousSide<SIDE>())
		{
			const Price prevPx = book.PreviousBestPrice<SIDE>();
			if (SideTraits<SIDE>::IsBetter(bestPx, prevPx))
			{
				lastMoveIn[INDEX] = now;
			}
			else if (SideTraits<SIDE>::IsWorse(bestPx, prevPx))
			{
				if (lastMoveIn[INDEX] != Time::Min())
				{
					if (now < lastMoveIn[INDEX] + Millis(sideSettings.alternateProrataProfileDelay + 5000))
					{
						// TODO wtf am I doing here? would this code ever be reached?
						profiles[INDEX] = altProfiles[INDEX];
						altProfiles[INDEX].reset();
						SwitchedToAlternateProfile[INDEX] = true;
					}
					lastMoveIn[INDEX] = Time::Min();
				}
			}
		}
		else
		{
			lastMoveIn[INDEX] = Time::Min();
		}

		if (lastMoveIn[INDEX] != Time::Min() && now >= lastMoveIn[INDEX] + Millis(sideSettings.alternateProrataProfileDelay))
		{
			profiles[INDEX] = altProfiles[INDEX];
			altProfiles[INDEX].reset();
			lastMoveIn[INDEX] = Time::Min();
			SwitchedToAlternateProfile[INDEX] = true;
		}
	}
	else
	{
		lastMoveIn[INDEX] = Time::Min();
	}

	if (!profiles[INDEX]
		|| !sideSettings.prorataEnabled
		|| unlikely(!isPriceInBands<SIDE>(book.BestPrice<SIDE>()))
		|| unlikely(!bestQuotePx)
		|| SideTraits<SIDE>::IsBetter(book.BestPrice<SIDE>(), *bestQuotePx))
	{
		if (targets[INDEX])
		{
			lastMoveBack[INDEX] = now;
		}
		return std::nullopt;
	}
	else
	{
		const Quantity bestQty = book.BestQuantity<SIDE>();
		const ProrataProfileUpdateMessage &profile = *profiles[INDEX];

		const SpreaderPricingModel::QuoteStatus spreaderQuoteStatus = spreaderPricingModel.CanQuote<SIDE>(2);

		int newIndex = -1;
		ProrataTarget::ListType quoteQuantities;

		if (spreaderQuoteStatus != SpreaderPricingModel::QuoteStatus::NO)
		{
			if (profile.levelCount > 0)
			{
				// If spreader is enabled and lockQty is lower than lean, use lockQty instead of lean
				const Quantity threshold = hasSpreader && spreaderPricingModel.GetSettings().SideEnabled<SIDE>(2)
					? std::min(Quantity(profile.bookQuantity[0]), spreaderPricingModel.GetSettings().lockQty[2])
					: Quantity(profile.bookQuantity[0]);
				if (bestQty >= threshold)
				{
					newIndex = 0;
					quoteQuantities.PushBack(Quantity(profile.quoteQuantity[0]));
				}
			}
			for (int i = 1; i < profile.levelCount; ++i)
			{
				const Quantity threshold { profile.bookQuantity[i] };
				if (bestQty >= threshold)
				{
					newIndex = i;
					quoteQuantities.PushBack(Quantity(profile.quoteQuantity[i]));
				}
			}
		}

		if (newIndex == -1 && spreaderQuoteStatus == SpreaderPricingModel::QuoteStatus::YES)
		{
			if (profile.levelCount > 0)
			{
				newIndex = 0;
				quoteQuantities.PushBack(Quantity(profile.quoteQuantity[0]));
			}
		}

		if (newIndex >= 0)
		{

			std::optional<ProrataTarget> newTarget =
				ProrataTarget
				{
					quoteQuantities,
					bestPx,
					Quantity(profile.ltpQuantity[newIndex]),
					Millis(profile.window[newIndex]),
					profile.addHedgeEnabled[newIndex] ? Quantity(profile.addHedgeQuantity[newIndex]) : Quantity(0)
				};

			if (targets[INDEX])
			{
				if (SideTraits<SIDE>::IsBetter(newTarget->price, targets[INDEX]->price))
				{
					if (now < lastMoveBack[INDEX] + Millis(quoteSettings.quoteTimeout))
					{
						newTarget = targets[INDEX];
					}
				}
				else if (quoteQuantities.Size() > targets[INDEX]->quoteQuantities.Size())
				{
					if (now < lastMoveBack[INDEX] + Millis(quoteSettings.quoteTimeout)
						|| (now < lastQtyReduceTime[INDEX] + Millis(quoteSettings.prorataQuoteTimeout) && bestPx == lastQtyReducePx[INDEX]))
					{
						newTarget = targets[INDEX];
					}
				}
				else if (SideTraits<SIDE>::IsWorse(newTarget->price, targets[INDEX]->price))
				{
					lastMoveBack[INDEX] = now;
				}
				else if (quoteQuantities.Size() < targets[INDEX]->quoteQuantities.Size())
				{
					lastQtyReducePx[INDEX] = bestPx;
					lastQtyReduceTime[INDEX] = now;
				}
			}
			else
			{
				if (now < lastMoveBack[INDEX] + Millis(quoteSettings.quoteTimeout)
					|| (now < lastQtyReduceTime[INDEX] + Millis(quoteSettings.prorataQuoteTimeout) && bestPx == lastQtyReducePx[INDEX]))
				{
					newTarget = targets[INDEX];
				}
			}

			return newTarget;
		}
		else
		{
			if (targets[INDEX])
			{
				lastMoveBack[INDEX] = now;
				lastQtyReduceTime[INDEX] = now;
				lastQtyReducePx[INDEX] = bestPx;
			}
			return std::nullopt;
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool ProrataPricingModel::isPriceInBands(const Price px) const
{
	const StackSideSettings &stackSideSettings = quoteSettings.GetSideSettings<SIDE>().stackSideSettings[0];
	return (!stackSideSettings.insidePrice || !SideTraits<SIDE>::IsBetter(px, *stackSideSettings.insidePrice))
		&& (!stackSideSettings.outsidePrice || !SideTraits<SIDE>::IsWorse(px, *stackSideSettings.outsidePrice));
}

}
