#pragma once

#include "MStackSide.hpp"

#include <Orders/OrderEnumsV2.hpp>

namespace akl
{

template <typename T, int count>
class FixedSizeVector;

}

namespace akl::stacker
{

struct QuoteInstrumentSettings;
struct StackSideSettings;
class IExecutionInterface;
struct Order;
struct StackInfoMessage;

class MStack
{
  public:
	MStack(
		const QuoteInstrumentSettings &quoteSettings,
		const StackSideSettings &bidSettings,
		const StackSideSettings &askSettings,
		IExecutionInterface &instr,
		const Price tickIncrement,
		const int stackIndex);

	bool Update(
		const std::optional<Price> &mBidPx,
		const std::optional<Price> &mAskPx,
		const std::optional<Price> &bidPx,
		const std::optional<Price> &askPx);

	bool Remove(Order *o);

	bool HasOrder(const Order *o) const;

	bool CanAddOrder(const KTN::ORD::KOrderSide::Enum side, const uint64_t reqId, const Price px) const;
	bool AddOrder(Order *o);

	template <KTN::ORD::KOrderSide::Enum SIDE>
	void Cancel(const Price px, const bool scratchImmediately);

	template <KTN::ORD::KOrderSide::Enum SIDE>
	Quantity GiveUpOrders(const Price px, FixedSizeVector<Order *, 10> &orders, bool &ordersRemaining);
	template <KTN::ORD::KOrderSide::Enum SIDE>
	int AbandonPrice(const Price px);
	int ReleaseAllOrders();

	void GetStackInfo(StackInfoMessage &msg, int &bidIndex, int &askIndex) const;

	template <KTN::ORD::KOrderSide::Enum SIDE>
	bool HasOrders(const Price px) const;

	template <KTN::ORD::KOrderSide::Enum SIDE>
	std::optional<Price> BestStackPrice() const;

  private:
	MStack(const MStack &) = delete;
	MStack &operator=(const MStack &) = delete;
	MStack(const MStack &&) = delete;
	MStack &operator=(const MStack &&) = delete;

	template <KTN::ORD::KOrderSide::Enum SIDE>
	MStackSide<SIDE> &getSide();

	template <KTN::ORD::KOrderSide::Enum SIDE>
	const MStackSide<SIDE> &getSide() const;

	MStackSide<KTN::ORD::KOrderSide::Enum::BUY> bids;
	MStackSide<KTN::ORD::KOrderSide::Enum::SELL> asks;
};

}

#include "MStack.ipp"
