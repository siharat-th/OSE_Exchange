#pragma once

#include <Orders/OrderEnumsV2.hpp>
#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include <akl/containers/CircularArray.hpp>
#include "Time.hpp"
#include "PriceToOrderMap.hpp"
#include <unordered_map>
#include <list>
#include "FillInfo.hpp"
#include "TradeTracker.hpp"
//#include "ProrataPricingModel.hpp"
#include "pricing/ProrataTarget.hpp"
#include "StopInfo.hpp"



namespace akl
{

class Book;

}

namespace akl::stacker
{

class PositionManager;
struct QuoteInstrumentSettings;
class Instrument;
class StrategyNotifier;
struct Order;
struct AdoptLevelInfo;
class StopHandler;

class HedgingModel
{
public:
	HedgingModel(
		const QuoteInstrumentSettings &settings,
		const Book &book,
		Instrument &instr,
		StrategyNotifier &notifier,
		PositionManager &pm,
		const Time &time,
		StopHandler &stopHandler);

	bool HasOrders() const;
	bool HasOrder(const Order *o) const;

	bool MaintainExistingOrders(const KTN::ORD::KOrderSide::Enum side);

	bool HandleTrade(const Timestamp &now, const KTN::ORD::KOrderSide::Enum aggressingSide, const Quantity tradeQty, const Price tradePx);
	bool HandleOffset(const std::optional<FillInfo> &fillInfo, const std::optional<CanceledHedgeInfo> &canceledHedgeInfo);
	bool RemoveOrder(Order *o);
	void ReleaseAllOrders();
	void CancelAllOrders();
	size_t Adopt(std::list<Order *> &adoptedOrders, const AdoptLevelInfo *levels = nullptr, const int count = 0, const int index = 0);

	bool CheckStops();
	bool CheckTimers(const Timestamp now);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool SendStop(const bool strategyRunning);

	inline const PriceToOrderMap &GetOrderMap() const { return map; }

private:
	HedgingModel(const HedgingModel &) = delete;
	HedgingModel &operator=(const HedgingModel &) = delete;
	HedgingModel(const HedgingModel &&) = delete;
	HedgingModel &operator=(const HedgingModel &&) = delete;

	using OrderListType = std::list<Order *>;
	struct OrderToReduce
	{
		Order *o { nullptr };
		Quantity newQty { 0 };
	};
	using OrderReduceListType = std::list<OrderToReduce>;

	const QuoteInstrumentSettings &settings;
	const Book &book;
	Instrument &instr;
	StrategyNotifier &notifier;
	const std::string symbol;
	const Price tickIncrement;
	PositionManager &pm;
	const Time &time;
	StopHandler &stopHandler;

	// Side is in terms of position
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void recalculate();
	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool handleOffset(const std::optional<FillInfo> &fillInfo, const std::optional<CanceledHedgeInfo> &canceledHedgeInfo);
	template<KTN::ORD::KOrderSide::Enum SIDE>
	void maintainOrders(
		const Price targetPx,
		OrderListType &ordersToCancel,
		OrderListType &ordersGivenToStack,
		OrderReduceListType &ordersToReduce,
		Quantity &pendingHedge);
	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool doOffset(const Price targetPx, const Price prevPx, const bool sendStop, const Quantity additionalHedge = Quantity(0), const bool stopTriggeredFromLink = false);

	bool sendOffset(
		const KTN::ORD::KOrderSide::Enum side,
		const Quantity qty,
		const Price px,
		const int sideIndex,
		const Quantity additionalHedge = Quantity(0),
		const StopInfo *stopInfo = nullptr);
	bool sendStop(StopInfo::OrderListType &orders);
	void modify(Order *o, const Quantity qty);
	void cancel(Order *o);
	bool giveOrderToStack(Order *o, const bool stack1Adopt = true, const bool stack2Adopt = true);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void cancelSideOrders();

	void queuePositionUpdate();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const std::optional<ProrataTarget> &getTarget() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	std::optional<Price> getStackTarget() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void cancelQuoteOrders(const Price px, bool scratchImmediately);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void disableAdditionalHedge();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	size_t adopt(std::list<Order *> &adoptedOrders, const AdoptLevelInfo *levels, const int count, const int stackIndex);

	void queueStackInfoUpdate();

	// Side is in terms of book side
	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool checkStop();
	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool checkMissedStops();

	// Side is in terms of position
	template<KTN::ORD::KOrderSide::Enum SIDE>
	Quantity getPayup() const;

	PriceToOrderMap map;

	Quantity oldPosition { 0 };
	Timestamp lastTickUp[2] { Time::Min(), Time::Min() }; // In terms of book
	Price payupAllowedPx[2] { Price::Min(), Price::Max() }; // In terms of hedge order
	bool canHedge[2] { true, true }; // In terms of hedge order

	TradeTracker tradeTracker;
	akl::CircularArray<Order *, 768> ordersToSend;

	Order *createOrder(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px, const Order::Type type);
	void insertByQuantity(Order *o);
};

}

#include "HedgingModel.ipp"
