#pragma once

#include "PriceToOrderMapSide.hpp"
#include "OrderHelpers.hpp"
#include "Order.hpp"
#include <algorithm>

namespace akl::stacker
{

template<typename COMPARER>
bool PriceToOrderMapSide<COMPARER>::HasOrders() const
{
	for (typename MapType::const_iterator mapIt = map.begin(); mapIt != map.end(); ++mapIt)
	{
		const LevelType &level = mapIt->second;
		for (typename LevelType::const_iterator levelIt = level.begin(); levelIt != level.end(); ++levelIt)
		{
			const Order *o = *levelIt;
			if (o  && IsWorking(o))
			{
				return true;
			}
		}
	}
	return false;
}

template<typename COMPARER>
bool PriceToOrderMapSide<COMPARER>::HasOrder(const Order *o) const
{
	typename MapType::const_iterator it = map.find(o->price);
	if (it != map.cend())
	{
		const LevelType &level = it->second;
		return std::find(level.cbegin(), level.cend(), o) != level.cend();
	}

	return false;
}

template<typename COMPARER>
bool PriceToOrderMapSide<COMPARER>::AddOrder(Order *o)
{
	LevelType &level = map[o->price];
	if (std::find(level.begin(), level.end(), o) == level.end())
	{
		level.push_back(o);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename COMPARER>
bool PriceToOrderMapSide<COMPARER>::RemoveOrder(Order *o)
{
	typename MapType::iterator it = map.find(o->price);
	if (it != map.end())
	{
		LevelType &level = it->second;
		LevelType::iterator it2 = std::find(level.begin(), level.end(), o);
		if (it2 != level.end())
		{
			level.erase(it2);
			if (level.empty())
			{
				map.erase(it);
			}
			return true;
		}
	}

	return false;
}

template<typename COMPARER>
void PriceToOrderMapSide<COMPARER>::ReleaseOrders()
{
	for (typename MapType::iterator mapIt = map.begin(); mapIt != map.end(); ++mapIt)
	{
		LevelType &level = mapIt->second;
		for (typename LevelType::iterator levelIt = level.begin(); levelIt != level.end(); ++levelIt)
		{
			(*levelIt)->type = Order::Type::NONE;
		}
	}
	map.clear();
}

template<typename COMPARER>
void PriceToOrderMapSide<COMPARER>::Clear()
{
	map.clear();
}

template<typename COMPARER>
typename PriceToOrderMapSide<COMPARER>::IteratorType PriceToOrderMapSide<COMPARER>::GetLevel(const Price px)
{
	return map.find(px);
}

template<typename COMPARER>
size_t PriceToOrderMapSide<COMPARER>::GetLevelSize(const Price px) const
{
	ConstIteratorType it = map.find(px);
	if (it != CEnd())
	{
		return it->second.size();
	}
	else
	{
		return 0;
	}
}

template<typename COMPARER>
typename PriceToOrderMapSide<COMPARER>::IteratorType PriceToOrderMapSide<COMPARER>::Begin()
{
	return map.begin();
}

template<typename COMPARER>
typename PriceToOrderMapSide<COMPARER>::IteratorType PriceToOrderMapSide<COMPARER>::End()
{
	return map.end();
}

template<typename COMPARER>
typename PriceToOrderMapSide<COMPARER>::ConstIteratorType PriceToOrderMapSide<COMPARER>::CBegin() const
{
	return map.cbegin();
}

template<typename COMPARER>
typename PriceToOrderMapSide<COMPARER>::ConstIteratorType PriceToOrderMapSide<COMPARER>::CEnd() const
{
	return map.cend();
}

}
