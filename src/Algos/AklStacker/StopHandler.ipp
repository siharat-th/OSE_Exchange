#pragma once

#include <akl/SideTraits.hpp>
#include <akl/BranchPrediction.hpp>
#include <akl/Book.hpp>
#include <AlgoParams/AklStackerParams.hpp>
#include "StopHandler.hpp"

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
size_t StopHandler::GetStopCount() const
{
	return getMap<SIDE>().size();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
StopInfo *StopHandler::GetStop(const Price px)
{
	MapType<SIDE> &map = getMap<SIDE>();
	typename MapType<SIDE>::iterator it = map.find(px);
	if (it != map.cend())
	{
		return &(it->second);
	}
	else
	{
		return nullptr;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
const StopInfo *StopHandler::GetStop(const Price px) const
{
	const MapType<SIDE> &map = getMap<SIDE>();
	typename MapType<SIDE>::const_iterator it = map.find(px);
	if (it != map.cend())
	{
		return &(it->second);
	}
	else
	{
		return nullptr;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
bool StopHandler::RemoveStop(const Price px, bool triggered, bool triggeredFromLink) // TODO it seems like this is only called when stop is triggered?
{
	MapType<SIDE> &map = getMap<SIDE>();
	typename MapType<SIDE>::iterator it = map.find(px);
	if (likely(it != map.end()))
	{
		if (triggered)
		{
			// constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
			// if (strategy.GetSettings().stopSettings.LinkEnabled<OPP>())
			// {
			// 	if (!triggeredFromLink)
			// 	{
			// 		strategy.StopTriggerNotify(SIDE);
			// 	}
			// 	strategy.GetSettings().stopSettings.DisableLink<OPP>();
			// 	strategy.QueueStopLinkUpdate();
			// }
		}
		sendUpdate(it->second, StopUpdateMessage::Action::DELETE);
		if (!triggered)
		{
			removeRisk(it->second.orders);
			returnPreallocatedOrders(it->second.orders);
		}
		map.erase(it);
		if (triggered)
		{
			constexpr int SIDE_INDEX = SideTraits<SIDE>::INDEX;
			const Timestamp now = time.Now();
			const Duration triggerDelay = Millis(stopSettings.triggerQuoteDelay);
			if (now < lastTriggeredTime[SIDE_INDEX] + triggerDelay)
			{
				lastTriggeredStop[SIDE_INDEX] = SideTraits<SIDE>::PickBest(lastTriggeredStop[SIDE_INDEX], px);
				lastTriggeredTime[SIDE_INDEX] = now;
			}
			else
			{
				lastTriggeredStop[SIDE_INDEX] = px;
				lastTriggeredTime[SIDE_INDEX] = now;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
std::optional<Price> StopHandler::LastTriggeredStop() const
{
	constexpr int SIDE_INDEX = SideTraits<SIDE>::INDEX;
	const Timestamp now = time.Now();
	const Duration triggerDelay = Millis(stopSettings.triggerQuoteDelay);
	if (now < lastTriggeredTime[SIDE_INDEX] + triggerDelay)
	{
		return lastTriggeredStop[SIDE_INDEX];
	}
	else
	{
		return std::nullopt;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
std::pair<Price, Timestamp> StopHandler::LastTriggeredStopAndTime() const
{
	constexpr int SIDE_INDEX = SideTraits<SIDE>::INDEX;
	return { lastTriggeredStop[SIDE_INDEX], lastTriggeredTime[SIDE_INDEX] };
}

template<KTN::ORD::KOrderSide::Enum SIDE>
StopHandler::IteratorType<SIDE> StopHandler::Begin()
{
	return getMap<SIDE>().begin();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
StopHandler::IteratorType<SIDE> StopHandler::End()
{
	return getMap<SIDE>().end();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StopHandler::HandleTickIn()
{
	MapType<SIDE> &map = getMap<SIDE>();
	typename MapType<SIDE>::iterator it = map.begin();
	while (it != map.end())
	{
		StopInfo &stopInfo = it->second;
		if (stopInfo.deleteOnFlip)
		{
			sendUpdate(stopInfo, StopUpdateMessage::Action::DELETE);
			removeRisk(it->second.orders);
			returnPreallocatedOrders(it->second.orders);
			it = map.erase(it);
		}
		else
		{
			++it;
		}
	}

	// constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
	// if (strategy.GetSettings().stopSettings.LinkEnabled<OPP>())
	// {
	// 	strategy.GetSettings().stopSettings.DisableLink<OPP>();
	// 	strategy.QueueStopLinkUpdate();
	// }
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StopHandler::handle(int sock, const StopUpdateMessage &msg)
{
	MapType<SIDE> &map = getMap<SIDE>();
	const Price px = msg.price;
	typename MapType<SIDE>::iterator it = map.find(px);
	if (it != map.end())
	{
		switch (msg.action)
		{
			case StopUpdateMessage::Action::DELETE:
			{
				sendUpdate(it->second, StopUpdateMessage::Action::DELETE);
				StopInfo &stopInfo = it->second;
				removeRisk(stopInfo.orders);
				returnPreallocatedOrders(stopInfo.orders);
				map.erase(it);
				break;
			}
			case StopUpdateMessage::Action::NEW:
			{
				sendUpdate(msg, StopUpdateMessage::Action::DELETE);
				break;
			}
			case StopUpdateMessage::Action::UPDATE:
			{
				StopInfo &stopInfo = it->second;
				const Quantity prevQty = stopInfo.quantity;
				stopInfo.quantity = std::max(Quantity(msg.qty), stopInfo.quantity);
				stopInfo.triggerQuantity = std::max(Quantity(msg.triggerQty), stopInfo.triggerQuantity);
				stopInfo.lean = msg.hasLean != 0 ? std::max(Quantity(msg.leanQty), stopInfo.lean) : Quantity(0);
				stopInfo.id = msg.stopId;
				stopInfo.deleteOnFlip = msg.deleteOnFlip != 0;
				stopInfo.sendOnTrade = msg.sendOnTrade != 0;
				if (likely(isNumOrdersAllowed(stopInfo.quantity)))
				{
					if (likely(hasRiskRoom<SIDE>(stopInfo.quantity - prevQty)))
					{
						const bool result = updateOrders(stopInfo.side, stopInfo.quantity, stopInfo.price, stopInfo.orders);
						if (likely(result))
						{
							sendUpdate(stopInfo, StopUpdateMessage::Action::UPDATE);
						}
						else
						{
							sendUpdate(stopInfo, StopUpdateMessage::Action::DELETE);
							sendPreallocationReject(msg);
							removeRisk(stopInfo.orders);
							returnPreallocatedOrders(stopInfo.orders);
							map.erase(it);
						}
					}
					else
					{
						sendUpdate(stopInfo, StopUpdateMessage::Action::DELETE);
						sendRiskStopReject(msg);
						removeRisk(stopInfo.orders);
						returnPreallocatedOrders(stopInfo.orders);
						map.erase(it);
					}
				}
				else
				{
					sendUpdate(stopInfo, StopUpdateMessage::Action::DELETE);
					sendStopReject(msg);
					removeRisk(stopInfo.orders);
					returnPreallocatedOrders(stopInfo.orders);
					map.erase(it);
				}
				break;
			}
			default:
			{
				sendUpdate(msg, StopUpdateMessage::Action::DELETE);
				break;
			}
		}
	}
	else
	{
		switch (msg.action)
		{
			case StopUpdateMessage::Action::DELETE:
			{
				sendUpdate(msg, StopUpdateMessage::Action::DELETE);
				LOGWARN("Received stop delete for unknown stop");
				break;
			}
			case StopUpdateMessage::Action::NEW:
			{
				constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
				if (book.HasSide<OPP>() && !SideTraits<SIDE>::IsBetter(book.BestPrice<OPP>(), px))
				{
					const Quantity stopQty = Quantity(msg.qty);
					if (likely(isNumOrdersAllowed(stopQty)))
					{
						if (likely(hasRiskRoom<SIDE>(stopQty)))
						{
							StopInfo &stopInfo = map[px];
							stopInfo.quantity = Quantity(msg.qty);
							stopInfo.triggerQuantity = Quantity(msg.triggerQty);
							stopInfo.lean = msg.hasLean != 0 ? Quantity(msg.leanQty) : Quantity(0);
							stopInfo.id = msg.stopId;
							stopInfo.serverId = nextServerStopId++;
							stopInfo.price = msg.price;
							stopInfo.side = SIDE;
							stopInfo.client = sock;
							stopInfo.deleteOnFlip = msg.deleteOnFlip != 0;
							stopInfo.sendOnTrade = msg.sendOnTrade != 0;
							stopInfo.clientId = msg.clientId;

							const bool result = updateOrders(stopInfo.side, stopInfo.quantity, stopInfo.price, stopInfo.orders);
							if (likely(result))
							{
								sendUpdate(stopInfo, StopUpdateMessage::Action::NEW);
							}
							else
							{
								sendUpdate(msg, StopUpdateMessage::Action::DELETE);
								sendPreallocationReject(msg);
								removeRisk(stopInfo.orders);
								returnPreallocatedOrders(stopInfo.orders);
								map.erase(px);
							}
						}
						else
						{
							sendUpdate(msg, StopUpdateMessage::Action::DELETE);
							sendRiskStopReject(msg);
						}
					}
					else
					{
						sendUpdate(msg, StopUpdateMessage::Action::DELETE);
						sendStopReject(msg);
					}
				}
				else
				{
					SendStop(SIDE, Quantity(msg.qty), msg.price);
					sendUpdate(msg, StopUpdateMessage::Action::DELETE);
				}
				break;
			}
			case StopUpdateMessage::Action::UPDATE:
			{
				sendUpdate(msg, StopUpdateMessage::Action::DELETE);
				LOGWARN("Received stop update for unknown stop");
				break;
			}
			default:
			{
				sendUpdate(msg, StopUpdateMessage::Action::DELETE);
				LOGWARN("Received stop message with unknown action {}", static_cast<uint16_t>(msg.action));
				break;
			}
		}
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void StopHandler::cancelStopsFromClient(const std::string &cliendId)
{
	MapType<SIDE> &map = getMap<SIDE>();
	typename MapType<SIDE>::iterator it = map.begin();
	while (it != map.end())
	{
		if (it->second.clientId == cliendId)
		{
			sendUpdate(it->second, StopUpdateMessage::Action::DELETE);
			removeRisk(it->second.orders);
			returnPreallocatedOrders(it->second.orders);
			LOGWARN("Removed stop id={} {} {}@{} for client {} due to missed heartbeats",
				it->second.id,
				KTN::ORD::KOrderSide::toString(it->second.side),
				it->second.quantity.value(),
				it->second.price.AsShifted(),
				it->second.clientId);
			it = map.erase(it);
		}
		else
		{
			++it;
		}
	}
}

}
