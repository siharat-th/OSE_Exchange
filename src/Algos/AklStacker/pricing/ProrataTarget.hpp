#pragma once

#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include <akl/containers/FixedSizeVector.hpp>
#include "../Time.hpp"

namespace akl::stacker
{

struct ProrataTarget
{
	using ListType = akl::FixedSizeVector<Quantity, 10>;

	ProrataTarget(const ListType &quoteQuantities, const Price price, const Quantity ltpQuantity, const Duration window, const Quantity addHedgeQuantity)
	: quoteQuantities(quoteQuantities)
	, price(price)
	, ltpQuantity(ltpQuantity)
	, window(window)
	, addHedgeQuantity(addHedgeQuantity)
	{

	}

	ListType quoteQuantities;
	Price price;
	Quantity ltpQuantity { 0 };
	Duration window { 0 };
	Quantity addHedgeQuantity { 0 };

	inline bool operator==(const ProrataTarget &rhs) const
	{
		return quoteQuantities == rhs.quoteQuantities
			&& price == rhs.price
			&& ltpQuantity == rhs.ltpQuantity
			&& window == rhs.window
			&& addHedgeQuantity == rhs.addHedgeQuantity;
	}

	inline bool operator!=(const ProrataTarget &rhs) const
	{
		return !(*this == rhs);
	}
};

}