#pragma once

#include "StackOrderBook.hpp"

#include "../Time.hpp"

#include <optional>

namespace akl::stacker
{

struct QuoteInstrumentSettings;
struct StackSideSettings;
class IExecutionInterface;

template<KTN::ORD::KOrderSide::Enum SIDE>
class StackSide
{
public:
	StackSide(
		const QuoteInstrumentSettings &stackSettings,
		const StackSideSettings &sideSettings,
		IExecutionInterface &execution,
		const Time &time,
		const Price tickIncrement,
		const int8_t stackIndex);

	void SetDefaultStackQuantity(const Quantity qty);

	bool HasOrders() const;
	bool HasOrders(const Price px) const;
	bool HasOrder(const Order *o) const;

	inline std::optional<Price> StackPrice() const { return stackPx; }
	std::optional<Price> BestPriceWithOrders() const;

	bool Update(const std::optional<Price> px, const bool doRequote);
	bool CanAddOrder(const uint64_t reqId, const Price px) const;
	bool AddOrder(Order *o);
	bool RemoveOrder(Order *o, const bool wasFill);
	void ReleaseAllOrders();
	void CancelAllOrders();
	void Cancel(const Price cancelPx, const bool scratchImmediately);
	void SendQueuedSubmits();

	void UpdateLength(const int newLength);

	void GetStackInfo(int64_t &stackPrice, int32_t *qtys, int& index, const int size) const;
	int AbandonPrice(const Price px);

	bool IsPriceInStack(const Price px) const;
	bool CheckRequote(const Timestamp now);

	void HandlePriceBandsUpdate();

private:
	StackSide(const StackSide<SIDE> &) = delete;
	StackSide<SIDE> &operator=(const StackSide &) = delete;
	StackSide(const StackSide<SIDE> &&) = delete;
	StackSide<SIDE> &operator=(const StackSide<SIDE> &&) = delete;

	const QuoteInstrumentSettings &settings;
	const StackSideSettings &sideSettings;
	const Time &time;
	StackOrderBook orderBook;
	const Price tickIncrement;
	int length { 0 };

	std::optional<Price> stackPx { std::nullopt };

	void moveIn(const Price newPx, const Price oldPx);
	void moveOut(const Price newPx, const Price oldPx);

	bool refreshStack();

	struct RequoteInfo
	{
		Price px;
		Timestamp fillTime;
	};
	using RequoteListType = std::list<RequoteInfo>;
	RequoteListType pendingRequotes;

	void updateRequotePrice(const Price oldPx, const Price newPx);
};

}

#include "StackSide.ipp"
