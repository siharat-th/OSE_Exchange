#include "StopHandler.hpp"
#include "AklStacker.hpp"
#include "Instrument.hpp"
#include "StrategyNotifier.hpp"
#include <sstream>

namespace akl::stacker
{

StopHandler::StopHandler(
	AklStacker &strategy,
	Instrument &instr,
	const AklStackerParams &settings,
	const Book &book,
	const Time &time,
	StrategyNotifier &notifier)
: strategy(strategy)
, instr(instr)
, settings(settings)
, stopSettings(settings.stopSettings)
, quoteSettings(settings.quoteInstrumentSettings)
, book(book)
, time(time)
, notifier(notifier)
{
	cachedOrders.reserve(128);
}

void StopHandler::Handle(int sock, const StopUpdateMessage &msg)
{
	const KTN::ORD::KOrderSide::Enum side = static_cast<KTN::ORD::KOrderSide::Enum>(msg.side);
	switch(side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			handle<KTN::ORD::KOrderSide::Enum::BUY>(sock, msg);
			break;
		case KTN::ORD::KOrderSide::Enum::SELL:
			handle<KTN::ORD::KOrderSide::Enum::SELL>(sock, msg);
			break;
		default:
			LOGWARN("Strategy {} received StopUpdateMessage with unknown side {}", strategy.StrategyId(), static_cast<uint16_t>(msg.side));
			break;
	}
}

void StopHandler::HandleStopSubscribe()
{
	for (BidMapType::const_iterator it = bidMap.cbegin(); it != bidMap.cend(); ++it)
	{
		sendUpdate(it->second, StopUpdateMessage::Action::UPDATE);
	}
	for (AskMapType::const_iterator it = askMap.cbegin(); it != askMap.cend(); ++it)
	{
		sendUpdate(it->second, StopUpdateMessage::Action::UPDATE);
	}
}

void StopHandler::CancelAll()
{
	while (!bidMap.empty())
	{
		BidMapType::iterator it = bidMap.begin();
		sendUpdate(it->second, StopUpdateMessage::Action::DELETE);
		removeRisk(it->second.orders);
		returnPreallocatedOrders(it->second.orders);
		bidMap.erase(it);
	}
	while (!askMap.empty())
	{
		AskMapType::iterator it = askMap.begin();
		sendUpdate(it->second, StopUpdateMessage::Action::DELETE);
		removeRisk(it->second.orders);
		returnPreallocatedOrders(it->second.orders);
		askMap.erase(it);
	}
}

void StopHandler::CancelMissedStops()
{
	const Price bb = book.HasBid() ? book.BestBid() : Price::Min();
	const Price ba = book.HasAsk() ? book.BestAsk() : Price::Max();

	BidMapType::iterator bit = bidMap.begin();
	while (bit != bidMap.end())
	{
		const StopInfo &stopInfo = bit->second;
		if (stopInfo.price < ba)
		{
			sendUpdate(bit->second, StopUpdateMessage::Action::DELETE);
			removeRisk(bit->second.orders);
			returnPreallocatedOrders(bit->second.orders);
			bit = bidMap.erase(bit);
		}
		else
		{
			++bit;
		}
	}

	AskMapType::iterator ait = askMap.begin();
	while (ait != askMap.end())
	{
		const StopInfo &stopInfo = ait->second;
		if (stopInfo.price > bb)
		{
			sendUpdate(ait->second, StopUpdateMessage::Action::DELETE);
			removeRisk(ait->second.orders);
			returnPreallocatedOrders(ait->second.orders);
			ait = askMap.erase(ait);
		}
		else
		{
			++ait;
		}
	}
}

bool StopHandler::CheckTimers(const Timestamp now)
{
	bool result = false;
	const Duration delayTrigger = Millis(strategy.GetSettings().stopSettings.triggerQuoteDelay);
	if (lastTriggeredTime[0] != Time::Min() && now >= lastTriggeredTime[0] + delayTrigger)
	{
		lastTriggeredTime[0] = Time::Min();
		result = true;
	}
	if (lastTriggeredTime[1] != Time::Min() && now >= lastTriggeredTime[1] + delayTrigger)
	{
		lastTriggeredTime[1] = Time::Min();
		result = true;
	}
	return result;
}

void StopHandler::CheckAllStopsForNumberOfOrders()
{
	int numStopsRemoved = 0;

	BidMapType::iterator bit = bidMap.begin();
	while (bit != bidMap.end())
	{
		if (likely(isNumOrdersAllowed(bit->second.quantity)))
		{
			++bit;
		}
		else
		{
			sendUpdate(bit->second, StopUpdateMessage::Action::DELETE);
			removeRisk(bit->second.orders);
			returnPreallocatedOrders(bit->second.orders);
			bit = bidMap.erase(bit);
			++numStopsRemoved;
		}
	}
	AskMapType::iterator ait = askMap.begin();
	while (ait != askMap.end())
	{
		if (likely(isNumOrdersAllowed(ait->second.quantity)))
		{
			++ait;
		}
		else
		{
			sendUpdate(ait->second, StopUpdateMessage::Action::DELETE);
			removeRisk(ait->second.orders);
			returnPreallocatedOrders(ait->second.orders);
			ait = askMap.erase(ait);
			++numStopsRemoved;
		}
	}

	if (unlikely(numStopsRemoved > 0))
	{
		std::stringstream ss;
		ss << "Canceled " << numStopsRemoved << " stops in " << instr.Symbol << " due to exceededing max number of orders of " << MAX_NUM_ORDERS;
		notifier.Notify(ss.str(), KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
	}
}

void StopHandler::SendStop(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px)
{
	Quantity remaining = qty;
	const Quantity maxQty = quoteSettings.riskSettings.maxOrderQty;
	while (remaining > Quantity(0))
	{
		const Quantity orderQty = maxQty > Quantity(0) && remaining > maxQty ? maxQty : remaining;
		Order *o = instr.CreateOrder(side, orderQty, px);
		if (likely(o != nullptr))
		{
			o->type = Order::Type::STOP;
			//o->Set();
			if (instr.CanSubmit(o, true, true))
			{
				instr.Submit(o);
			}
		}
		else
		{
			break;
		}
		remaining -= orderQty;
	}
}

void StopHandler::HandleClientDisconnect(const std::string &clientId)
{
	cancelStopsFromClient<KTN::ORD::KOrderSide::Enum::BUY>(clientId);
	cancelStopsFromClient<KTN::ORD::KOrderSide::Enum::SELL>(clientId);
}

template<>
StopHandler::BidMapType &StopHandler::getMap<KTN::ORD::KOrderSide::Enum::BUY>()
{
	return bidMap;
}

template<>
StopHandler::AskMapType &StopHandler::getMap<KTN::ORD::KOrderSide::Enum::SELL>()
{
	return askMap;
}

template<>
const StopHandler::BidMapType &StopHandler::getMap<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return bidMap;
}

template<>
const StopHandler::AskMapType &StopHandler::getMap<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return askMap;
}

void StopHandler::sendUpdate(const StopInfo &stopInfo, const StopUpdateMessage::Action action)
{
	StopUpdateMessage msg;
	msg.id = strategy.StrategyId();
	msg.price = stopInfo.price;
	msg.qty = stopInfo.quantity;
	msg.triggerQty = stopInfo.triggerQuantity;
	msg.stopId = stopInfo.id;
	msg.serverStopId = stopInfo.serverId;
	msg.action = action;
	msg.side = stopInfo.side;
	msg.deleteOnFlip = stopInfo.deleteOnFlip ? 1 : 0;
	msg.sendOnTrade = stopInfo.sendOnTrade ? 1 : 0;
	if (stopInfo.lean > Quantity(0))
	{
		msg.leanQty = stopInfo.lean;
		msg.hasLean = 1;
	}
	else
	{
		msg.leanQty = Quantity(0);
		msg.hasLean = 0;
	}
	strategy.Publish(msg);
}

void StopHandler::sendUpdate(const StopUpdateMessage &msg, const StopUpdateMessage::Action action)
{
	StopUpdateMessage copy = msg;
	copy.action = action;
	strategy.Publish(copy);
}

bool StopHandler::isNumOrdersAllowed(const Quantity qty) const
{
	if (likely(qty > Quantity(0)))
	{
		const Quantity maxOrderQty = quoteSettings.riskSettings.maxOrderQty;
		if (likely(maxOrderQty > Quantity(0)))
		{
			int numOrders = qty.value() / maxOrderQty.value();
			if (qty.value() % maxOrderQty.value() > 0)
			{
				++numOrders;
			}
			return numOrders <= MAX_NUM_ORDERS;
		}
	}
	return false;
}

void StopHandler::sendStopReject(const StopUpdateMessage &msg)
{
	std::stringstream ss;
	ss << "Stop in " << instr.Symbol << " exceeded max number of orders of " << MAX_NUM_ORDERS;
	notifier.Notify(ss.str(), KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
}

void StopHandler::sendRiskStopReject(const StopUpdateMessage &msg)
{
	std::stringstream ss;
	ss << "Stop in " << instr.Symbol
		<< " exceeded available risk. Qty=" << msg.qty
		<< " Working=" << instr.GetRisk().GetWorkingQty(msg.side)
		<< " Pos=" << instr.GetRisk().GetPosition()
		<< " Limit=" << instr.GetRisk().GetPositionLimit();
	const std::string str = ss.str();
	LOGWARN(str.c_str());
	notifier.Notify(str, KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
}

void StopHandler::sendPreallocationReject(const StopUpdateMessage &msg)
{
	std::stringstream ss;
	ss << "Failed to pre-allocate stop in " << instr.Symbol << " for quantity " << msg.qty;
	notifier.Notify(ss.str(), KTN::notify::MsgLevel::Enum::ERROR, KTN::notify::MsgType::Enum::ALERT);
}

template<>
bool StopHandler::hasRiskRoom<KTN::ORD::KOrderSide::Enum::BUY>(const Quantity qty) const
{
	return instr.GetRisk().GetAvailableQuantity<KTN::ORD::KOrderSide::Enum::BUY>() >= qty;
}

template<>
bool StopHandler::hasRiskRoom<KTN::ORD::KOrderSide::Enum::SELL>(const Quantity qty) const
{
	return instr.GetRisk().GetAvailableQuantity<KTN::ORD::KOrderSide::Enum::SELL>() >= qty;
}

bool StopHandler::updateOrders(const KTN::ORD::KOrderSide::Enum side, const Quantity targetQty, const Price px, StopInfo::OrderListType &orders)
{
	Quantity sum { 0 };
	for (int i = 0; i < orders.Size(); ++i)
	{
		const Order *o = orders[i];
		sum += o->targetQuantity;
	}

	if (targetQty > sum)
	{
		const Quantity maxQty = quoteSettings.riskSettings.maxOrderQty;
		if (likely(maxQty > Quantity(0)))
		{
			Quantity remaining = targetQty - sum;
			for (int i = 0; i < orders.Size() && remaining > Quantity(0); ++i)
			{
				Order *o = orders[i];
				if (o->targetQuantity < maxQty)
				{
					const Quantity qtyToAdd = o->targetQuantity + remaining <= maxQty ? remaining : maxQty - o->targetQuantity;
					const RiskStatus riskResult = instr.GetRisk().TryModify(o, o->targetQuantity + qtyToAdd);
					if (riskResult == RiskStatus::RISK_OK)
					{
						o->targetQuantity += qtyToAdd;
						o->orderPod.quantity += qtyToAdd.value();
						remaining -= qtyToAdd;
					}
					else
					{
						LOGWARN("Strategy {} failed to increase qty {} on pre-allocated stop order {} by {} due to risk error: {},{}",
							strategy.StrategyId(),
							static_cast<int>(o->targetQuantity),
							o->orderReqId,
							static_cast<int>(qtyToAdd),
							static_cast<int>(riskResult),
							toString(riskResult));
						return false;
					}
				}
			}

			while (remaining > Quantity(0) && !orders.Full())
			{
				const Quantity orderQty = remaining > maxQty ? maxQty : remaining;
				Order *o = getPreallocatedOrder(side, orderQty, px);
				if (likely(o != nullptr))
				{
					// o->side = side;
					// o->targetQuantity = orderQty;
					// o->price = px;
					o->type = Order::Type::STOP;
					//o->Set();
					const RiskStatus riskResult = instr.GetRisk().TrySubmit(o);
					if (riskResult == RiskStatus::RISK_OK)
					{
						orders.PushBack(o);
						o->riskPreallocated = true;
						remaining -= orderQty;
					}
					else
					{
						LOGWARN("Strategy {} failed to pre-allocate stop order {} with qty {} due to risk error: {},{}",
							strategy.StrategyId(),
							o->orderReqId,
							static_cast<int>(o->targetQuantity),
							static_cast<int>(riskResult),
							toString(riskResult));
						return false;
					}
				}
				else
				{
					LOGWARN("Strategy {} failed to create pre-allocated order for stop", strategy.StrategyId());
					return false;
				}
			}
		}
	}

	return true;
}

void StopHandler::removeRisk(StopInfo::OrderListType &orders)
{
	for (int i = 0; i < orders.Size(); ++i)
	{
		Order *o = orders[i];
		instr.GetRisk().Remove(o);
	}
}

Order *StopHandler::getPreallocatedOrder(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px)
{
	while (!cachedOrders.empty())
	{
		Order *o = cachedOrders.back();
		cachedOrders.pop_back();
		if (likely(!o->submitted))
		{
			o->side = side;
			o->targetQuantity = qty;
			o->price = px;
			o->type = Order::Type::STOP;

			// TODO do i need to set any other fields here?
			o->orderPod.OrdSide = side;
			o->orderPod.quantity = qty.value();
			o->orderPod.price = px;

			return o;
		}
		else
		{
			LOGWARN("Strategy {} StopHandler found submitted order {} in cachedOrders", strategy.StrategyId(), o->orderReqId);
		}
	}
	return instr.CreateOrder(side, qty, px);
}

void StopHandler::returnPreallocatedOrders(StopInfo::OrderListType &orders)
{
	for (int i = 0; i < orders.Size(); ++i)
	{
		Order *o = orders[i];
		returnPreallocatedOrder(o);
	}
	orders.Clear();
}

void StopHandler::returnPreallocatedOrder(Order *o)
{
	if (likely(!o->submitted))
	{
		//o->Reset();
		o->targetQuantity = Quantity(0);
		o->orderPod.quantity = 0;
		cachedOrders.push_back(o);
	}
	else
	{
		LOGWARN("Strategy {} trying to release submitted stop order {}", strategy.StrategyId(), o->orderReqId);
	}
}

}