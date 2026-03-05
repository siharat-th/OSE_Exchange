#pragma once

#include "Book.hpp"
#include "SideTraits.hpp"

namespace akl
{

template<KTN::ORD::KOrderSide::Enum SIDE>
void Book::processLtp(const Quantity tradeQty, const Price tradePx, Level *levels, size_t &count)
{
	size_t i = 0;
	while (i < count && !SideTraits<SIDE>::IsBetter(tradePx, levels[i].price))
	{
		if (tradePx == levels[i].price)
		{
			if (tradeQty < levels[i].quantity)
			{
				levels[i].quantity -= tradeQty;
			}
			else
			{
				levels[i].quantity = Quantity(0);
				++i;
			}
			isLtpAdjusted = true;
			break;
		}
		else if (SideTraits<SIDE>::IsWorse(tradePx, levels[i].price))
		{
			levels[i].quantity = Quantity(0);
			++i;
			isLtpAdjusted = true;
		}
	}

	constexpr int INDEX = SideTraits<SIDE>::INDEX;
	cleanUpEmptyLevels(levels, count, i, INDEX);
}

}