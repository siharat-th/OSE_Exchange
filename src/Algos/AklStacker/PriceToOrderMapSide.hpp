#pragma once

#include <Orders/OrderEnumsV2.hpp>
#include <akl/Price.hpp>

#include <map>
#include <vector>

namespace akl::stacker
{

struct Order;

template <typename COMPARER>
class PriceToOrderMapSide
{
public:
	using LevelType = std::vector<Order *>;
	using MapType = std::map<Price, LevelType, COMPARER>;
	using IteratorType = typename MapType::iterator;
	using ConstIteratorType = typename MapType::const_iterator;

	bool HasOrders() const;
	bool HasOrder(const Order *o) const;
	bool AddOrder(Order *o);
	bool RemoveOrder(Order *o);
	void ReleaseOrders();
	void Clear();
	IteratorType GetLevel(const Price px);
	size_t GetLevelSize(const Price px) const;

	IteratorType Begin();
	IteratorType End();

	ConstIteratorType CBegin() const;
	ConstIteratorType CEnd() const;

private:
	MapType map;
};

}

#include "PriceToOrderMapSide.ipp"
