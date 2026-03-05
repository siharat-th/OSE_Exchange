#pragma once

#include <akl/Price.hpp>
#include <Orders/OrderEnumsV2.hpp>

namespace akl::stacker
{

struct AdoptLevelInfo
{
	Price px;
	KTN::ORD::KOrderSide::Enum side;
	bool stackAdoptAllowed[2] { false, false };
};

}