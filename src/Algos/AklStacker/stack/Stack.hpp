#pragma once

#include "StackSide.hpp"

namespace akl::stacker
{

struct QuoteInstrumentSettings;
struct StackSideSettings;
class IExecutionInterface;
class Time;
struct Order;
struct StackInfoMessage;

class Stack
{
public:
	Stack(
		const QuoteInstrumentSettings &settings,
		const StackSideSettings &bidSettings,
		const StackSideSettings &askSettings,
		IExecutionInterface &execution,
		const Time &time,
		const Price tickIncrement,
		const int8_t stackIndex);

	void SetDefaultStackQuantity(const Quantity bidStackQty, const Quantity askStackQty);

	bool Update(const std::optional<Price> bidPx, const std::optional<Price> askPx, const bool doRequote);
	void SendQueuedSubmits();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	StackSide<SIDE> &GetSide();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const StackSide<SIDE> &GetSide() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	std::optional<Price> StackPrice() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	std::optional<Price> BestPriceWithOrders() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void Cancel(const Price px, const bool scratchImmediately);

	bool HasOrders() const;
	bool HasOrder(const Order *o) const;

	bool CanAddOrder(const KTN::ORD::KOrderSide::Enum side, const uint64_t reqId, const Price px) const;
	bool AddOrder(Order *o);
	bool RemoveOrder(Order *o, const bool wasFill);
	void ReleaseAllOrders();
	void CancelAllOrders();

	void GetStackInfo(StackInfoMessage &msg, int &bidIndex, int &askIndex) const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	int AbandonPrice(const Price px);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool HasOrders(const Price px) const;

	bool CheckRequote(const Timestamp now);

	void HandlePriceBandUpdate();

private:
	Stack(const Stack &) = delete;
	Stack &operator=(const Stack &) = delete;
	Stack(const Stack &&) = delete;
	Stack &operator=(const Stack &&) = delete;

	StackSide<KTN::ORD::KOrderSide::Enum::BUY> bids;
	StackSide<KTN::ORD::KOrderSide::Enum::SELL> asks;
};

}

#include "Stack.ipp"
