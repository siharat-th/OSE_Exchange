#include "PricingModel.hpp"
#include "SpreaderPricingModel.hpp"

#include <akl/BranchPrediction.hpp>
#include <akl/Book.hpp>
#include <akl/SideTraits.hpp>
#include <AlgoParams/AklStackerParams.hpp>

namespace akl::stacker
{

PricingModel::PricingModel(
	const Book &book,
	const PositionManager &pm,
	const Time &time,
	const StopHandler &stopHandler,
	const SpreaderPricingModel &spreaderPricingModel,
	const QuoteInstrumentSettings &quoteSettings,
	const int stackIndex,
	const Price tickIncrement)
: book(book)
, pm(pm)
, time(time)
, stopHandler(stopHandler)
, spreaderPricingModel(spreaderPricingModel)
, quoteSettings(quoteSettings)
, stackIndex(stackIndex)
, tickIncrement(tickIncrement)
, noLeanAdj(NO_LEAN_TICKS_ADJ * tickIncrement)
{

}

bool PricingModel::Recalculate(const bool isRunning, const std::optional<Price> &stackBidPx, const std::optional<Price> &stackAskPx)
{
	const std::optional<Price> prevTargetPx[2] = { stackTargetPx[0], stackTargetPx[1] };
	const std::optional<Price> prevMaintainPx[2] = { maintainPx[0], maintainPx[1] };

	const bool canMove = book.IsOpen() || (book.IsPreopen() && quoteSettings.quoteInPreopen);

	const Targets bidTargets = recalculate<KTN::ORD::KOrderSide::Enum::BUY>(isRunning, stackBidPx, canMove);
	const Targets askTargets = recalculate<KTN::ORD::KOrderSide::Enum::SELL>(isRunning, stackAskPx, canMove);

	stackTargetPx[0] = bidTargets.stackPx;
	stackTargetPx[1] = askTargets.stackPx;
	maintainPx[0] = bidTargets.maintainPx;
	maintainPx[1] = askTargets.maintainPx;

	couldMove = canMove;

	const bool targetsChanged =
		prevTargetPx[0] != stackTargetPx[0]
		|| prevTargetPx[1] != stackTargetPx[1]
		|| prevMaintainPx[0] != maintainPx[0]
		|| prevMaintainPx[1] != maintainPx[1];
	if (targetsChanged)
	{
		LogTargets();
	}

	return targetsChanged;
}

bool PricingModel::CheckTimers(const Timestamp now)
{
	bool result = false;
	if (lastMoveOut[0] != Time::Min() && now >= lastMoveOut[0] + Millis(quoteSettings.quoteTimeout))
	{
		lastMoveOut[0] = Time::Min();
		result = true;
	}
	if (lastMoveOut[1] != Time::Min() && now >= lastMoveOut[1] + Millis(quoteSettings.quoteTimeout))
	{
		lastMoveOut[1] = Time::Min();
		result = true;
	}
	if (lastMoveIn[0] != Time::Min() && now >= lastMoveIn[0] + Millis(quoteSettings.bidSettings.stackSideSettings[stackIndex].quickAddTime))
	{
		lastMoveIn[0] = Time::Min();
		result = true;
	}
	if (lastMoveIn[1] != Time::Min() && now >= lastMoveIn[1] + Millis(quoteSettings.askSettings.stackSideSettings[stackIndex].quickAddTime))
	{
		lastMoveIn[1] = Time::Min();
		result = true;
	}
	return result;
}

std::optional<Price> PricingModel::BidLean() const
{
	return stackTargetPx[0];
}

std::optional<Price> PricingModel::AskLean() const
{
	return stackTargetPx[1];
}

void PricingModel::LogTargets() const
{
	LOGINFO("Stack targets: [ {} / {}  ] | Maintain: [ {} / {} ] index={}",
		BidLean() ? BidLean()->AsShifted() : Price::Min().AsShifted(),
		AskLean() ? AskLean()->AsShifted() : Price::Max().AsShifted(),
		MaintainBid() ? MaintainBid()->AsShifted() : Price::Min().AsShifted(),
		MaintainAsk() ? MaintainAsk()->AsShifted() : Price::Max().AsShifted(),
		stackIndex);
}

}
