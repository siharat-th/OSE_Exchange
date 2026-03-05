#pragma once

#include <Orders/OrderEnumsV2.hpp>
#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include "../Time.hpp"
#include <akl/SideTraits.hpp>

#include <AlgoParams/AklStackerParams.hpp>
#include "ProrataTarget.hpp"
#include "../messages/ProrataProfileUpdateMessage.hpp"

#include <optional>

namespace akl
{

class Book;

}

namespace akl::stacker
{

class Time;
class SpreaderPricingModel;

class ProrataPricingModel
{
public:
	ProrataPricingModel(
		const Book &book,
		const QuoteInstrumentSettings &quoteSettings,
		const SpreaderPricingModel &spreaderPricingModel,
		const Time &time,
		const std::string &strategyId);

	bool Recalculate(const std::optional<Price> &bestBidQuotePx, const std::optional<Price> &bestAskQuotePx);

	void SetBidProfile(const ProrataProfileUpdateMessage &profile);
	void SetAskProfile(const ProrataProfileUpdateMessage &profile);
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void SetSideProfile(const ProrataProfileUpdateMessage &profile);

	void ClearBidProfile();
	void ClearAskProfile();
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void ClearSideProfile();

	void SetAlternateBidProfile(const ProrataProfileUpdateMessage &profile);
	void SetAlternateAskProfile(const ProrataProfileUpdateMessage &profile);
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void SetAlternateSideProfile(const ProrataProfileUpdateMessage &profile);

	void ClearAlternateBidProfile();
	void ClearAlternateAskProfile();
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void ClearAlternateSideProfile();

	inline bool HasBidTarget() const { return targets[0].has_value(); }
	inline bool HasAskTarget() const { return targets[1].has_value(); }
	inline const std::optional<ProrataTarget> &BidTarget() const { return targets[0]; }
	inline const std::optional<ProrataTarget> &AskTarget() const { return targets[1]; }

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline const std::optional<ProrataProfileUpdateMessage> &GetProfile() const { return profiles[SideTraits<SIDE>::INDEX]; }

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline const std::optional<ProrataProfileUpdateMessage> &GetAlternateProfile() const { return altProfiles[SideTraits<SIDE>::INDEX]; }

	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline bool HasTarget() const;
	template<KTN::ORD::KOrderSide::Enum SIDE>
	inline const std::optional<ProrataTarget> &Target() const;

	bool ProrataProfileUpdated(const ProrataProfileUpdateMessage &profile);
	bool ProrataProfileDeleted(const char *name);

	bool CheckTimers(const Timestamp now);

	bool SwitchedToAlternateProfile[2] { false, false };

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool DisableAdditionalHedge();

	void SetHasSpreader(const bool val) { hasSpreader = val; }

private:
	ProrataPricingModel(const ProrataPricingModel &) = delete;
	const ProrataPricingModel &operator=(const ProrataPricingModel &) = delete;
	ProrataPricingModel(const ProrataPricingModel &&) = delete;
	const ProrataPricingModel &operator=(const ProrataPricingModel &&) = delete;

	const Book &book;
	const QuoteInstrumentSettings &quoteSettings;
	const SpreaderPricingModel &spreaderPricingModel;
	const Time &time;
	const std::string strategyId;

	std::optional<ProrataProfileUpdateMessage> profiles[2] { std::nullopt, std::nullopt };
	std::optional<ProrataProfileUpdateMessage> altProfiles[2] { std::nullopt, std::nullopt };
	std::optional<ProrataTarget> targets[2] { std::nullopt, std::nullopt };
	Timestamp lastMoveBack[2] { Time::Min(), Time::Min() };
	Timestamp lastMoveIn[2] { Time::Min(), Time::Min() };
	Timestamp lastQtyReduceTime[2] { Time::Min(), Time::Min() };
	Price lastQtyReducePx[2] { Price::Min(), Price::Max() };

	bool hasSpreader { false };

	template<KTN::ORD::KOrderSide::Enum SIDE>
	std::optional<ProrataTarget> recalculate(const std::optional<Price> &bestQuotePx);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool isPriceInBands(const Price px) const;
};

}

#include "ProrataPricingModel.ipp"
