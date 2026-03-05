#pragma once

#include <optional>

#include "../Time.hpp"
#include <akl/Price.hpp>
#include <akl/SideTraits.hpp>
#include <Algos/AklStacker/PositionManager.hpp>
#include <Orders/OrderEnumsV2.hpp>

namespace akl
{

class Book;

}

namespace akl::stacker
{

struct QuoteInstrumentSettings;
class StopHandler;
class SpreaderPricingModel;

class PricingModel
{
private:
	struct Targets
	{
		Targets(const std::optional<Price> &stackPx, const std::optional<Price> &maintainPx) : stackPx(stackPx), maintainPx(maintainPx) { };
		const std::optional<Price> stackPx;
		const std::optional<Price> maintainPx;
	};

public:
	PricingModel(
		const Book &book,
		const PositionManager &pm,
		const Time &time,
		const StopHandler &stopHandler,
		const SpreaderPricingModel &spreaderPricingModel,
		const QuoteInstrumentSettings &quoteSettings,
		const int stackIndex,
		const Price tickIncrement);

	bool Recalculate(const bool isRunning, const std::optional<Price> &stackBidPx, const std::optional<Price> &stackAskPx);
	bool CheckTimers(const Timestamp now);

	std::optional<Price> BidLean() const;
	std::optional<Price> AskLean() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	std::optional<Price> Lean() const
	{
		return stackTargetPx[SideTraits<SIDE>::INDEX];
	}

	inline std::optional<Price> MaintainBid() const { return maintainPx[0]; }
	inline std::optional<Price> MaintainAsk() const { return maintainPx[1]; }

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void TickBack(const Price px);

	void LogTargets() const;

	void SetHasSpreader(const bool val) { hasSpreader = val; }

private:
	PricingModel(const PricingModel &) = delete;
	PricingModel &operator=(const PricingModel &) = delete;
	PricingModel(const PricingModel &&) = delete;
	PricingModel &operator=(const PricingModel &&) = delete;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	Targets recalculate(const bool isRunning, const std::optional<Price> &stackPx, const bool canMove);

	const Book &book;
	const PositionManager &pm;
	const Time &time;
	const StopHandler &stopHandler;
	const SpreaderPricingModel &spreaderPricingModel;
	const QuoteInstrumentSettings &quoteSettings;
	const int stackIndex;
	const Price tickIncrement;

	std::optional<Price> stackTargetPx[2] { std::nullopt, std::nullopt };
	std::optional<Price> maintainPx[2] { std::nullopt, std::nullopt };

	Timestamp lastMoveIn[2] { Time::Min(), Time::Min() };
	Timestamp lastMoveOut[2] { Time::Min(), Time::Min() };

	enum QuickAddStatus
	{
		WAITING,
		MET,
		EXPIRED
	};
	QuickAddStatus quickAddStatus[2] { QuickAddStatus::EXPIRED, QuickAddStatus::EXPIRED };

	bool couldMove { true };
	bool hasSpreader { false };

	static constexpr int NO_LEAN_TICKS_ADJ = 15;
	const Price noLeanAdj;
};

}

#include "PricingModel.ipp"