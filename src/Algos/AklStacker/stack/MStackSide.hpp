#pragma once

#include <Orders/OrderEnumsV2.hpp>
#include <akl/Price.hpp>
#include <akl/SideTraits.hpp>
#include <akl/containers/FixedSizeVector.hpp>

#include <map>
#include <list>
#include <optional>

namespace akl::stacker
{

struct QuoteInstrumentSettings;
struct StackSideSettings;
class IExecutionInterface;
struct Order;

template<KTN::ORD::KOrderSide::Enum SIDE>
class MStackSide
{
public:
	MStackSide(
		const QuoteInstrumentSettings &quoteSettings,
		const StackSideSettings &sideSettings,
		IExecutionInterface &execution,
		const Price tickIncrement,
		const int stackIndex);

	bool Update(const std::optional<Price> &mPx, const std::optional<Price> &targetPx);
	bool Remove(Order *o);

	void Cancel(const Price px, const bool scratchImmediately);

	bool HasOrder(const Order *o) const;

	bool CanAddOrder(const uint64_t reqId, const Price px) const;
	bool AddOrder(Order *o);
	Quantity GiveUpOrders(const Price px, FixedSizeVector<Order *, 10> &orders, bool &ordersRemaining);
	int AbandonPrice(const Price px);
	int ReleaseAllOrders();

	void GetStackInfo(int64_t &stackPrice, int32_t *qtys, int &index, const int size) const;
	bool HasOrders(const Price px) const;

	inline std::optional<Price> BestStackPrice() const { return !orderBook.empty() ? std::optional<Price>(orderBook.begin()->first) : std::nullopt; }

private:
	MStackSide(const MStackSide &) = delete;
	MStackSide &operator=(const MStackSide &) = delete;
	MStackSide(const MStackSide &&) = delete;
	MStackSide &operator=(const MStackSide &&) = delete;

	const QuoteInstrumentSettings &quoteSettings;
	const StackSideSettings &sideSettings;
	IExecutionInterface &execution;
	const Price tickIncrement;
	const int stackIndex;

	// Map will be sorted from best price to worst price
	using ComparerType = typename SideTraits<SIDE>::BestPriceComparerType;
	using LevelType = std::list<Order *>;
	using BookType = std::map<Price, LevelType, ComparerType>;
	BookType orderBook;

	std::optional<Price> mPx { std::nullopt };
	std::optional<Price> stackPx { std::nullopt };
};

}

#include "MStackSide.ipp"
