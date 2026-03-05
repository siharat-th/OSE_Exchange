#include "SpreaderPricingModel.hpp"
#include <akl/Book.hpp>

namespace akl::stacker
{

SpreaderPricingModel::SpreaderPricingModel(const SpreaderPricingSettings &spreaderSettings, const Book &quoteBook, const Book &spreadBook, const Price tickIncrement, StrategyNotifier &notifier)
: spreaderPricingSettings(spreaderSettings)
, quoteBook(quoteBook)
, spreadBook(spreadBook)
, tickIncrement(tickIncrement)
, notifier(notifier)
{

}

bool SpreaderPricingModel::Recalculate()
{
	bool changed = recalculate<KTN::ORD::KOrderSide::Enum::BUY>();
	changed |= recalculate<KTN::ORD::KOrderSide::Enum::SELL>();
	return changed;
}

bool SpreaderPricingModel::OverrideStrikePrices(const std::optional<Price> &bidPrice, const std::optional<Price> &askPrice)
{
	const int maxAddTicks = std::max({spreaderPricingSettings.addTicks[0], spreaderPricingSettings.addTicks[1], spreaderPricingSettings.addTicks[2]});

	bool result = overrideStrikePrice<KTN::ORD::KOrderSide::Enum::BUY>(bidPrice, maxAddTicks);
	result |= overrideStrikePrice<KTN::ORD::KOrderSide::Enum::SELL>(askPrice, maxAddTicks);
	return result;
}

void SpreaderPricingModel::RandomizeTicks()
{
	TicksChanged = randomizeAddTicks(0);
	TicksChanged |= randomizeAddTicks(1);
	TicksChanged |= randomizeAddTicks(2);
	TicksChanged |= randomizeRemoveTicks(0);
	TicksChanged |= randomizeRemoveTicks(1);
	TicksChanged |= randomizeRemoveTicks(2);
}

void SpreaderPricingModel::LogStrikes() const
{
	LOGINFO("Spreader strikes: [ {} / {} ]",
		strikePx[0] ? *strikePx[0] : Price::Min(),
		strikePx[1] ? *strikePx[1] : Price::Min());
}

void SpreaderPricingModel::LogTicks() const
{
	LOGINFO("Spreader ticks: Add [ {} , {} , {} ] Remove [ {} , {} , {} ]",
		addTicks[0],
		addTicks[1],
		addTicks[2],
		removeTicks[0],
		removeTicks[1],
		removeTicks[2]);
}

bool SpreaderPricingModel::randomizeAddTicks(const int index)
{
	const int prev = addTicks[index];
	if (spreaderPricingSettings.tickVariance > 0)
	{
		addTicks[index] = randomizer.GetRandomInt(spreaderPricingSettings.addTicks[index], spreaderPricingSettings.tickVariance);
	}
	else
	{
		addTicks[index] = spreaderPricingSettings.addTicks[index];
	}
	addTicks[index] = std::max(addTicks[index], 0);
	addTicks[index] = std::min(addTicks[index], 100);
	return addTicks[index] != prev;
}

bool SpreaderPricingModel::randomizeRemoveTicks(const int index)
{
	const int prev = removeTicks[index];
	if (spreaderPricingSettings.tickVariance > 0)
	{
		removeTicks[index] = randomizer.GetRandomInt(spreaderPricingSettings.removeTicks[index], spreaderPricingSettings.tickVariance);
	}
	else
	{
		removeTicks[index] = spreaderPricingSettings.removeTicks[index];
	}
	removeTicks[index] = std::max(removeTicks[index], 0);
	removeTicks[index] = std::min(removeTicks[index], 100);
	return removeTicks[index] != prev;
}

}