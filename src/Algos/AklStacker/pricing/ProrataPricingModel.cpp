#include "ProrataPricingModel.hpp"
#include <akl/Book.hpp>

namespace akl::stacker
{

ProrataPricingModel::ProrataPricingModel(
	const Book &book,
	const QuoteInstrumentSettings &quoteSettings,
	const SpreaderPricingModel &spreaderPricingModel,
	const Time &time,
	const std::string &strategyId)
: book(book)
, quoteSettings(quoteSettings)
, spreaderPricingModel(spreaderPricingModel)
, time(time)
, strategyId(strategyId)
{

}

bool ProrataPricingModel::Recalculate(const std::optional<Price> &bestBidQuotePx, const std::optional<Price> &bestAskQuotePx)
{
	const std::optional<ProrataTarget> prevTargets[2] = { targets[0], targets[1] };
	targets[0] = recalculate<KTN::ORD::KOrderSide::Enum::BUY>(bestBidQuotePx);
	targets[1] = recalculate<KTN::ORD::KOrderSide::Enum::SELL>(bestAskQuotePx);
	return prevTargets[0] != targets[0] || prevTargets[1] != targets[1];
}

void ProrataPricingModel::SetBidProfile(const ProrataProfileUpdateMessage &profile)
{
	profiles[0] = profile;
}

void ProrataPricingModel::SetAskProfile(const ProrataProfileUpdateMessage &profile)
{
	profiles[1] = profile;
}

void ProrataPricingModel::ClearBidProfile()
{
	profiles[0].reset();
}

void ProrataPricingModel::ClearAskProfile()
{
	profiles[1].reset();
}

void ProrataPricingModel::SetAlternateBidProfile(const ProrataProfileUpdateMessage &profile)
{
	altProfiles[0] = profile;
}

void ProrataPricingModel::SetAlternateAskProfile(const ProrataProfileUpdateMessage &profile)
{
	altProfiles[1] = profile;
}

void ProrataPricingModel::ClearAlternateBidProfile()
{
	altProfiles[0].reset();
}

void ProrataPricingModel::ClearAlternateAskProfile()
{
	altProfiles[1].reset();
}

template<>
inline bool ProrataPricingModel::HasTarget<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return targets[0].has_value();
}

template<>
inline bool ProrataPricingModel::HasTarget<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return targets[1].has_value();
}

template<>
const std::optional<ProrataTarget> &ProrataPricingModel::Target<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return targets[0];
}

template<>
const std::optional<ProrataTarget> &ProrataPricingModel::Target<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return targets[1];
}

bool ProrataPricingModel::ProrataProfileUpdated(const ProrataProfileUpdateMessage &profile)
{
	bool result = false;
	if (strcmp(quoteSettings.bidSettings.prorataProfile.c_str(), profile.name.c_str()) == 0)
	{
		profiles[0] = profile;
		result = true;
		LOGINFO("Strategy {} updated bid pro-rata profile {}", strategyId.c_str(), profiles[0]->name);
	}
	if (strcmp(quoteSettings.askSettings.prorataProfile.c_str(), profile.name.c_str()) == 0)
	{
		profiles[1] = profile;
		result = true;
		LOGINFO("Strategy {} updated ask pro-rata profile {}", strategyId.c_str(), profiles[1]->name);
	}
	if (strcmp(quoteSettings.bidSettings.alternateProrataProfile.c_str(), profile.name.c_str()) == 0)
	{
		altProfiles[0] = profile;
		// Don't set result to true, as we don't need to recalculate when alternate profile is updated
		LOGINFO("Strategy {} updated bid alternate pro-rata profile {}", strategyId.c_str(), altProfiles[0]->name);
	}
	if (strcmp(quoteSettings.askSettings.alternateProrataProfile.c_str(), profile.name.c_str()) == 0)
	{
		altProfiles[1] = profile;
		// Don't set result to true, as we don't need to recalculate when alternate profile is updated
		LOGINFO("Strategy {} updated ask alternate pro-rata profile {}", strategyId.c_str(), altProfiles[1]->name);
	}
	return result;
}

bool ProrataPricingModel::ProrataProfileDeleted(const char *name)
{
	bool result = false;
	if (strcmp(quoteSettings.bidSettings.prorataProfile.c_str(), name) == 0)
	{
		profiles[0].reset();
		result = true;
		LOGINFO("Strategy {} cleared bid pro-rata profile {}", strategyId.c_str(), name);
	}
	if (strcmp(quoteSettings.askSettings.prorataProfile.c_str(), name) == 0)
	{
		profiles[1].reset();
		result = true;
		LOGINFO("Strategy {} cleared ask pro-rata profile {}", strategyId.c_str(), name);
	}
	if (strcmp(quoteSettings.bidSettings.alternateProrataProfile.c_str(), name) == 0)
	{
		altProfiles[0].reset();
		result = true;
		LOGINFO("Strategy {} cleared bid alternate pro-rata profile {}", strategyId.c_str(), name);
	}
	if (strcmp(quoteSettings.askSettings.alternateProrataProfile.c_str(), name) == 0)
	{
		altProfiles[1].reset();
		result = true;
		LOGINFO("Strategy {} cleared ask alternate pro-rata profile {}", strategyId.c_str(), name);
	}
	return result;
}

bool ProrataPricingModel::CheckTimers(const Timestamp now)
{
	bool result = false;
	if (lastMoveBack[0] != Time::Min() && now >= lastMoveBack[0] + Millis(quoteSettings.quoteTimeout))
	{
		lastMoveBack[0] = Time::Min();
		result = true;
	}
	if (lastMoveBack[1] != Time::Min() && now >= lastMoveBack[1] + Millis(quoteSettings.quoteTimeout))
	{
		lastMoveBack[1] = Time::Min();
		result = true;
	}
	if (lastQtyReduceTime[0] != Time::Min() && now >= lastQtyReduceTime[0] + Millis(quoteSettings.prorataQuoteTimeout))
	{
		lastQtyReduceTime[0] = Time::Min();
		result = true;
	}
	if (lastQtyReduceTime[1] != Time::Min() && now >= lastQtyReduceTime[1] + Millis(quoteSettings.prorataQuoteTimeout))
	{
		lastQtyReduceTime[1] = Time::Min();
		result = true;
	}
	if (lastMoveIn[0] != Time::Min() && now >= lastMoveIn[0] + Millis(quoteSettings.bidSettings.alternateProrataProfileDelay))
	{
		result = true;
		// Timestamp will be reset during price recalculation
	}
	if (lastMoveIn[1] != Time::Min() && now >= lastMoveIn[1] + Millis(quoteSettings.askSettings.alternateProrataProfileDelay))
	{
		result = true;
		// Timestamp will be reset during price recalculation
	}
	return result;
}

}
