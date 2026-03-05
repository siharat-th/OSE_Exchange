#pragma once

#include "PayupCalculator.hpp"
#include "pricing/PricingModel.hpp"
//#include "DynamicPricingModel.hpp"

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
void PayupCalculator::removeLockedLevels()
{
	const std::optional<Price> stackPx = spm.Lean<SIDE>();
	if (stackPx)
	{
		MapType<SIDE> &map = getMap<SIDE>();
		while (!map.empty())
		{
			typename MapType<SIDE>::reverse_iterator it = map.rbegin();
			if (!SideTraits<SIDE>::IsBetter(it->first, *stackPx))
			{
				map.erase(std::next(it).base());
			}
			else
			{
				return;
			}
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void PayupCalculator::removeCrossingLevels()
{
	constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
	if (book.HasSide<OPP>())
	{
		const Price px = book.BestPrice<OPP>();
		MapType<SIDE> &map = getMap<SIDE>();
		while (!map.empty())
		{
			typename MapType<SIDE>::iterator it = map.begin();
			if (!SideTraits<SIDE>::IsWorse(it->first, px))
			{
				map.erase(it);
			}
			else
			{
				return;
			}
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
Quantity PayupCalculator::calculatePayup()
{
	const QuoteSideSettings &sideSettings = settings.GetSideSettings<SIDE>();

	if (likely(book.HasSide<SIDE>()))
	{
		if (settings.dynamicStack && sideSettings.dynamicProfileEnabled)
		{
			const Price bestPx = book.BestPrice<SIDE>();
			MapType<SIDE> &map = getMap<SIDE>();
			typename MapType<SIDE>::iterator it = map.find(bestPx);
			if (it != map.end())
			{
				if (pm.PositionAt<SIDE>(bestPx) == Quantity(0))
				{
					map.erase(it);
				}
				else
				{
					// TODO
					// const std::optional<messaging::DynamicProfileMessage> &profile = dpm.GetProfile<SIDE>();
					// if (profile && profile->levelCount > 0)
					// {
					// 	const int count = std::min(profile->levelCount, static_cast<int8_t>(messaging::DynamicProfileMessage::SIZE));
					// 	const int index = it->second < count ? it->second : count;
					// 	if (profile->payup[index] > 0)
					// 	{
					// 		return Quantity(profile->payup[index]);
					// 	}
					// }
				}
			}
		}
		else
		{
			MapType<SIDE> &map = getMap<SIDE>();
			if (unlikely(!map.empty()))
			{
				map.clear();
			}
		}

		const std::optional<Price> innerPx = pm.GetInnermostPrice<SIDE>();
		if (innerPx && SideTraits<SIDE>::IsBetter(book.BestPrice<SIDE>(), *innerPx))
		{
			return sideSettings.winnerPayup;
		}
	}

	return sideSettings.payup;
}

}