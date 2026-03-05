#pragma once

#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include <akl/containers/FixedSizeVector.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include "Order.hpp"

namespace akl::stacker
{

struct StopInfo
{
	static constexpr int MAX_NUM_ORDERS = 20;

	StopInfo()
	: price(Price::Min())
	, quantity(0)
	, triggerQuantity(0)
	, lean(0)
	, id(0)
	, serverId(0)
	, client(-1)
	, side(KTN::ORD::KOrderSide::Enum::BUY)
	, deleteOnFlip(false)
	{

	}

	StopInfo(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Quantity triggerQty, const Price px, const uint32_t id, uint32_t serverId, const bool deleteOnFlip, const int client)
	: price(px)
	, quantity(qty)
	, triggerQuantity(triggerQty)
	, lean(0)
	, id(id)
	, serverId(serverId)
	, client(client)
	, side(side)
	, deleteOnFlip(deleteOnFlip)
	{

	}

	using OrderListType = FixedSizeVector<Order *, MAX_NUM_ORDERS>;
	OrderListType orders;
	std::string clientId { "" };
	Price price;
	Quantity quantity;
	Quantity triggerQuantity;
	Quantity lean;
	uint32_t id;
	uint32_t serverId;
	int client;
	KTN::ORD::KOrderSide::Enum side;
	bool deleteOnFlip;
	bool sendOnTrade { false };

private:
	StopInfo(const StopInfo &) = delete;
	const StopInfo &operator=(const StopInfo &) = delete;
	StopInfo(const StopInfo &&) = delete;
	const StopInfo &operator=(const StopInfo &&) = delete;
};

}