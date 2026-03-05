#pragma once

#include <optional>
#include <akl/Price.hpp>
#include <akl/Quantity.hpp>
#include <Orders/OrderEnumsV2.hpp>

#include <Algos/AklStacker/Randomizer.hpp>

namespace akl
{
	class Book;
}

namespace akl::stacker
{

struct SpreaderPricingSettings;
class StrategyNotifier;

class SpreaderPricingModel
{
public:
	enum QuoteStatus
	{
		NO,
		YES,
		YES_IF_LEAN_MET
	};

	SpreaderPricingModel(const SpreaderPricingSettings &spreaderSettings, const Book &quoteBook, const Book &spreadBook, const Price tickIncrement, StrategyNotifier &notifier);

	bool Recalculate();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool HasStrikePrice() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	int TicksFromStrikePrice() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool SpreadQuotingMet(int index) const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	QuoteStatus CanQuote(const int index) const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const std::optional<Price> &StrikePrice() const;

	bool OverrideStrikePrices(const std::optional<Price> &bidPrice, const std::optional<Price> &askPrice);

	void LogStrikes() const;
	void LogTicks() const;
	void RandomizeTicks();

	bool TicksChanged { false };

	const SpreaderPricingSettings &GetSettings() const { return spreaderPricingSettings; }

private:
	SpreaderPricingModel(const SpreaderPricingModel &) = delete;
	SpreaderPricingModel &operator=(const SpreaderPricingModel &) = delete;
	SpreaderPricingModel(SpreaderPricingModel &&) = delete;
	SpreaderPricingModel &operator=(SpreaderPricingModel &&) = delete;

	enum PricingStatus
	{
		WAITING,
		ADD_TICKS_MET,
		REMOVE_TICKS_MET,
		LOCK_QTY_MET
	};

	const SpreaderPricingSettings &spreaderPricingSettings;
	const Book &quoteBook;
	const Book &spreadBook;
	const Price tickIncrement;

	std::optional<Price> strikePx[2] { std::nullopt, std::nullopt };

	template <KTN::ORD::KOrderSide::Enum SIDE>
	bool recalculate();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool overrideStrikePrice(const std::optional<Price> &price, const int maxTicks);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool updatePricingStatus(const int index, const int ticksFromStrike, const Quantity quoteQty);

	bool randomizeAddTicks(const int index);
	bool randomizeRemoveTicks(const int index);

	PricingStatus pricingStatus[2][3] { { WAITING, WAITING, WAITING }, { WAITING, WAITING, WAITING } };
	int addTicks[3] { -1, -1, -1 };
	int removeTicks[3] { -1, -1, -1 };
	Randomizer randomizer;

	StrategyNotifier &notifier;
};

}

#include "SpreaderPricingModel.ipp"