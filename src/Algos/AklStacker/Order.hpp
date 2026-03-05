#pragma once

#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <Orders/OrderPod.hpp>
#include "TradeSideTracker.hpp"

#include "Time.hpp"

namespace akl::stacker
{

struct Order
{
	enum Type : uint8_t
	{
		NONE = 0,
		STACK = 1,
		HEDGE = 2,
		PRORATA = 3,
		STOP = 4
	};

	Order() { }

	KTN::OrderPod orderPod;
	Quantity targetQuantity;
	Quantity ackedQuantity;
	Quantity filledQuantity { 0 };
	Quantity lastRiskQuantity;
	Price price;
	Price ackedPrice;
	uint64_t orderReqId { 0 };
	uint64_t cmeId { 0 };
	uint64_t strategyOrderId { 0 };
	KTN::ORD::KOrderSide::Enum side;
	Type type { Type::NONE };
	std::atomic_bool pendingCancel { false };
	std::atomic_bool pendingModify { false };
	std::atomic_bool cancelQueued { false };
	bool scratchFillsImmediately { false };
	bool wasAbandoned { false };
	bool submitQueued { false };
	bool riskPreallocated { false };
	bool submitted { false };
	int numCancelRejects { 0 };
	int8_t dynamicIndex { -1 };
	int8_t stackIndex { 0 };
	int8_t numPendingMods { 0 };
	KTN::ORD::KOrderTif::Enum tif { KTN::ORD::KOrderTif::Enum::DAY };
	Timestamp lastCancelRejectTime { Time::Min() };
	Timestamp cancelTime { Time::Min() };
	uint64_t priorityTransactTime { 0 };
	stacker::TradeSideTracker tradeTracker;

	Quantity WorkingQuantity() const
	{
		return ackedQuantity > filledQuantity ? ackedQuantity - filledQuantity : Quantity(0);
	}

	bool IsWorking() const
	{
		return WorkingQuantity() > Quantity(0);
	}

private:
	Order(const Order &other) = delete;
	Order &operator=(const Order &other) = delete;
	Order(Order &&other) = delete;
	Order &operator=(Order &&other) = delete;
};

}