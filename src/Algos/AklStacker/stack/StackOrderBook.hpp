#pragma once

#include <cstdlib>
#include <list>

#include <akl/Quantity.hpp>
#include <akl/Quantity.hpp>
#include <akl/SideTraits.hpp>
#include <AlgoParams/AklStackerParams.hpp>

namespace akl::stacker
{

struct QuoteInstrumentSettings;
struct StackSideSettings;
class IExecutionInterface;
struct Order;

class StackOrderBook
{
public:
	static constexpr int SIZE = 1024;

	StackOrderBook(
		const QuoteInstrumentSettings &settings,
		const StackSideSettings &sideSettings,
		IExecutionInterface &execution,
		const Price tickIncrement,
		const KTN::ORD::KOrderSide::Enum side,
		const int8_t stackIndex);

	void SetDefaultStackQuantity(const Quantity qty);
	inline Quantity GetDefaultStackQuantity() const { return stackQty; }

	bool HasOrders(const Price px) const;
	bool HasOrder(const Order *o) const;

	void Submit(const Price px);
	void Submit(const Price px, const Quantity orderQty);
	void Move(const Price newPx, const Price oldPx);
	void Cancel(const Price px, const bool scratchImmediately = false, const bool canceledDueToPriceBands = false);
	void SendQueuedSubmits();

	void Requote(const Price px);

	bool AddOrder(Order *o);
	bool RemoveOrder(Order *o);
	void ReleaseAllOrders();
	int ReleaseOrders(const Price px);

	bool RefreshLevel(const Price px);
	void ClearForceCancel();

	Quantity GetStackQuantity(const Price px) const;
	bool CanRequote(const Price px) const;

	void ClearPriceBandCancelFlag(const Price px);

private:
	StackOrderBook(const StackOrderBook &) = delete;
	StackOrderBook &operator=(const StackOrderBook &) = delete;
	StackOrderBook(const StackOrderBook &&) = delete;
	StackOrderBook &operator=(const StackOrderBook &&) = delete;

	static constexpr int MASK = SIZE - 1;
	using OrderListType = std::list<Order *>;
	struct StackLevel
	{
		OrderListType orders;
		int8_t numRequotes { 0 };
		bool manualCancel { false };
		bool canceledDueToPriceBands { false };
	};
	StackLevel levels[SIZE];

	const QuoteInstrumentSettings &stackSettings;
	const StackSideSettings &sideSettings;
	IExecutionInterface &execution;
	const Price tickIncrement;
	const KTN::ORD::KOrderSide::Enum side;
	const int8_t stackIndex;
	Quantity stackQty { 0 };
	std::vector<Price> pricesToQuote;

	//Randomizer randomizer;

	int indexOf(Price px) const;
	void submit(const Price px, StackLevel &level);
	void submit(const Price px, StackLevel &level, const Quantity qty);
	void queueSubmit(const Price px);

	void move(const Price newPx, const Price oldPx);

	bool isInPriceBands(const Price px) const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool isInPriceBands(const Price px) const
	{
		return (!sideSettings.insidePrice || !SideTraits<SIDE>::IsBetter(px, *sideSettings.insidePrice))
			&& (!sideSettings.outsidePrice || !SideTraits<SIDE>::IsWorse(px, *sideSettings.outsidePrice));
	}

	Quantity takeOrders(const Price px, StackLevel &level);
};

}
