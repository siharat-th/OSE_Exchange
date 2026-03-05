#pragma once

#include <akl/SideTraits.hpp>
#include <akl/Price.hpp>
#include "Time.hpp"
#include "StopInfo.hpp"
#include "messages/StopUpdateMessage.hpp"
#include "messages/HeartbeatMessage.hpp"

#include <map>
#include <optional>

namespace akl
{

class Book;

}

namespace akl::stacker
{

class AklStacker;
struct AklStackerParams;
class Instrument;
class StopSettings;
struct QuoteInstrumentSettings;
class StrategyNotifier;

class StopHandler
{
public:
	template<KTN::ORD::KOrderSide::Enum SIDE>
	using MapType = std::map<Price, StopInfo, typename SideTraits<SIDE>::BestPriceComparerType>;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	using IteratorType = typename MapType<SIDE>::iterator;

	StopHandler(
		AklStacker &strategy,
		Instrument &instr,
		const AklStackerParams &settings,
		const Book &book,
		const Time &time,
		StrategyNotifier &notifier);

	void Handle(int sock, const StopUpdateMessage &msg);
	void HandleStopSubscribe();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	size_t GetStopCount() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	StopInfo *GetStop(const Price px);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const StopInfo *GetStop(const Price px) const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool RemoveStop(const Price px, bool triggered, bool triggeredFromLink = false);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	std::optional<Price> LastTriggeredStop() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	std::pair<Price, Timestamp> LastTriggeredStopAndTime() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	IteratorType<SIDE> Begin();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	IteratorType<SIDE> End();

	void CancelAll();
	void CancelMissedStops();
	bool CheckTimers(const Timestamp now);
	void CheckAllStopsForNumberOfOrders();

	void SendStop(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void HandleTickIn();

	void HandleClientDisconnect(const std::string &clientId);

private:
	StopHandler(const StopHandler &) = delete;
	StopHandler &operator=(const StopHandler &) = delete;
	StopHandler(const StopHandler &&) = delete;
	StopHandler &operator=(const StopHandler &&) = delete;

	AklStacker &strategy;
	Instrument &instr;
	const AklStackerParams &settings;
	const StopSettings &stopSettings;
	const QuoteInstrumentSettings &quoteSettings;
	const Book &book;
	const Time &time;
	StrategyNotifier &notifier;

	using BidMapType = MapType<KTN::ORD::KOrderSide::Enum::BUY>;
	using AskMapType = MapType<KTN::ORD::KOrderSide::Enum::SELL>;
	BidMapType bidMap;
	AskMapType askMap;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	MapType<SIDE> &getMap();

	template<KTN::ORD::KOrderSide::Enum SIDE>
	const MapType<SIDE> &getMap() const;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void handle(int sock, const StopUpdateMessage &msg);

	void sendUpdate(const StopInfo &stopInfo, const StopUpdateMessage::Action action);
	void sendUpdate(const StopUpdateMessage &msg, const StopUpdateMessage::Action action);

	static constexpr int MAX_NUM_ORDERS = StopInfo::MAX_NUM_ORDERS;
	bool isNumOrdersAllowed(const Quantity qty) const;
	void sendStopReject(const StopUpdateMessage &msg);
	void sendRiskStopReject(const StopUpdateMessage &msg);
	void sendPreallocationReject(const StopUpdateMessage &msg);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool hasRiskRoom(const Quantity qty) const;

	bool updateOrders(const KTN::ORD::KOrderSide::Enum side, const Quantity targetQty, const Price px, StopInfo::OrderListType &orders);
	void removeRisk(StopInfo::OrderListType &orders);
	Order *getPreallocatedOrder(const KTN::ORD::KOrderSide::Enum side, const Quantity qty, const Price px);
	void returnPreallocatedOrders(StopInfo::OrderListType &orders);
	void returnPreallocatedOrder(Order *o);

	Price lastTriggeredStop[2] { Price::Min(), Price::Max() };
	Timestamp lastTriggeredTime[2] { Time::Min(), Time::Min() };
	uint32_t nextServerStopId = 1;
	std::vector<Order *> cachedOrders;

	using HeartbeatTimeMapType = std::unordered_map<std::string, Timestamp>;
	HeartbeatTimeMapType heartbeatTimeMap;
	static constexpr Duration HEARTBEAT_TIMEOUT = Seconds(10);

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void cancelStopsFromClient(const std::string &cliendId);
};

}

#include "StopHandler.ipp"
