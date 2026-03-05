#pragma once

#include "PriceToOrderMapSide.hpp"
#include <akl/SideTraits.hpp>

namespace akl::stacker
{

class PriceToOrderMap
{
public:
	PriceToOrderMap();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	using SideMapType = PriceToOrderMapSide<typename SideTraits<SIDE>::BestPriceComparerType>;

	using BidMapType = SideMapType<KTN::ORD::KOrderSide::Enum::BUY>;
	using AskMapType = SideMapType<KTN::ORD::KOrderSide::Enum::SELL>;

	bool HasOrders() const;
	bool HasOrder(const Order *o) const;
	bool AddOrder(Order *o);
	bool RemoveOrder(Order *o);
	void ReleaseOrders();
	void Clear();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	SideMapType<SIDE> &GetSideMap();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const SideMapType<SIDE> &GetSideMap() const;

private:
	PriceToOrderMap(const PriceToOrderMap &) = delete;
	PriceToOrderMap &operator=(const PriceToOrderMap &) = delete;
	PriceToOrderMap(const PriceToOrderMap &&) = delete;
	PriceToOrderMap &operator=(const PriceToOrderMap &&) = delete;

	BidMapType bidMap;
	AskMapType askMap;
};

}
