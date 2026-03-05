#pragma once

#include <Orders/OrderEnumsV2.hpp>
#include <akl/Quantity.hpp>
#include <akl/Price.hpp>

namespace akl::stacker
{

struct FillInfo
{
	Price px;
	Quantity qty { 0 };
	KTN::ORD::KOrderSide::Enum side;
	bool scratchImmediately { false };
	Quantity estimatedTrade { 0 };
};

struct CanceledHedgeInfo
{
	Price px;
	KTN::ORD::KOrderSide::Enum side;
};

}