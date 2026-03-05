#pragma once

#include <AlgoParams/AklStackerParams.hpp>
#include <Algos/AlgoBaseV3.hpp>
#include <akl/Book.hpp>
#include "Time.hpp"
#include "StrategyNotifier.hpp"
#include "AdoptLevelInfo.hpp"
#include "StrategyMessagePublisher.hpp"
#include "messages/ProrataProfileUpdateMessage.hpp"
#include "messages/StackerProrataOrderAdoptMessage.hpp"
#include "messages/StackerProrataOrderAbandonMessage.hpp"
#include "messages/HeartbeatMessage.hpp"

#include <AlgoEngines/QuoterV3.hpp>
#include <KT01/Core/ObjectPool.hpp>

#include <akl/StrategyManager.hpp>

#include <mutex>

namespace akl::stacker
{

class Instrument;
class PriceConverter;
struct Order;

class AklStacker : public KTN::AlgoBaseV3
{
public:
	enum Status
	{
		PAUSED = 0,
		RUNNING = 1
	};

	using OrderPoolType = KTN::Core::ObjectPool<OrderPod>;

	AklStacker(AlgoInitializer &params);
	virtual ~AklStacker();

	virtual void onLaunch(std::string json) override final;
	virtual void onJson(std::string json) override final;

	virtual void onCalculate() override final;
	virtual void Enable(bool enable) override final;

	virtual void onTrade(int index, double price, double size, int aggressor) override final;
	virtual void onMktData(int index) override final;
	virtual void onHeartbeat(int msgseqnum) override final;
	virtual void onSessionStatus(std::string exch, int mktsegid, int protocol, std::string desc, std::string state) override final;
	
	virtual void minOrderAck(KTN::OrderPod &ord) override final;
	virtual void minOrderCancel(KTN::OrderPod &ord) override final;
	virtual void minOrderModify(KTN::OrderPod &ord) override final;
	virtual void minOrderExecution(KTN::OrderPod &ord, bool hedgeSent) override final;
	virtual void minOrderReject(KTN::OrderPod &ord, const std::string &text) override final;
	virtual void minOrderCancelReject(KTN::OrderPod &ord, const std::string &text) override final;
	virtual void minOrderCancelReplaceReject(KTN::OrderPod &ord, const std::string &text) override final;

	virtual void onHedgeOrderSent(
		char *clordid,
		uint64_t reqid,
		int32_t secid,
		KOrderSide::Enum side,
		uint32_t qty,
		int32_t price9,
		uint16_t instindex) override final;

	virtual void onOrderSent(
		char *clordid,
		uint64_t reqid,
		int32_t secid,
		KOrderSide::Enum side,
		uint32_t qty,
		int32_t price,
		KOrderAlgoTrigger::Enum trig) override final;

	virtual void ResetPosition() override final;
	virtual void ToggleCommand(int id) override final;
	virtual void Shutdown() override final;
	virtual void Restart() override final;
	virtual void CancelAllOrders() override final;

	virtual void OrderReceived(KTN::Order &incoming, int index) override final;
	virtual void OrderMinUpdate(KTN::OrderPod &ord) override final;

	virtual void onMktDataBidAsk(const KT01::DataStructures::MarketDepth::DepthBook &md) override final;
	virtual void onMktDataTrade(const KT01::DataStructures::MarketDepth::DepthBook &md) override final;
	virtual void onMktDataSecurityStatus(
		int secid,
		KT01::DataStructures::MarketDepth::BookSecurityStatus secstatus,
		KT01::DataStructures::MarketDepth::BookSecurityTradingEvent event,
		const uint64_t recvTime) override final;

	virtual void onAlgoMessage(const std::string &json) override final;
	virtual void onClientConnected(const std::string &clientId) override final;
	virtual void onClientDisconnected(const std::string &clientId) override final;

	virtual void onKillswitchUpdate(const bool state) override final;
	virtual void DisableRisk() override final;
	virtual void TryStop() override final;

	// void start() override final;
	// void stop() override final;

	// inline void Start() { isRunning = true; }
	// inline void Stop() { isRunning = false; }
	inline bool IsRunning() const { return status == Status::RUNNING; } // TODO
	inline bool IsReady() const { return instr != nullptr; }
	bool Recalculate(const bool requote = false);

	Time &GetTime() { return time; }
	const Time &GetTime() const { return time; }
	Time::TimestampType Now() const { return time.Now(); }
	const Book &GetBook(int index) const { return book[index]; }

	inline const Instrument *GetInstrument() const { return instr; }
	inline int ExchangeCallbackId() const { return _EXCH_CALLBACK_ID; }

	inline OrderManagerV5 &GetOrderManager() { return _Ords; }

	template<typename ... Args>
	void LogFmt(const char *msg, const Args ... args)
	{
		KT01_LOG_INFO(__CLASS__, msg, args...);
		//LogMe(fmt::format(msg, args...), color);
	}

	template<typename ... Args>
	void LogVerbose(const char *msg, const Args ... args)
	{
		if (_verboseLog) {
			KT01_LOG_INFO(__CLASS__, msg, args...);
		}
	}

	inline void Log(const std::string &msg, LogLevel::Enum level = LogLevel::Enum::INFO)
	{
		LogMe(msg, level);
	}

	inline AklStackerParams &GetSettings() { return settings; }
	inline const AklStackerParams &GetSettings() const { return settings; }
	inline const std::string &StrategyId() const { return _GUID; }
	inline const PriceConverter *GetPriceConverter() const { return priceConverter; }

	void OnInternalCancel(Order *o);
	void OnInternalReject(Order *o);

	StrategyMessagePublisher &GetPublisher() { return publisher; }

	template<typename MESSAGE_TYPE>
	void Publish(const MESSAGE_TYPE &msg)
	{
		publisher.Push(msg);
	}

	void PublishStackInfo();
	void PublishStackTargets();
	void PublishRiskInfo();
	void PublishPosition();
	void PublishSettings();
	void PublishProrataSettings();
	void PublishStatus();
	void PublishProrataOrder(const uint64_t reqId, const Quantity workingQty, const int piq, const KTN::ORD::KOrderSide::Enum side, const int8_t index, const int count);
	void PublishSpreaderUpdate();

	inline void QueueStackInfoUpdate() { stackInfoUpdateNeeded = true; }
	inline void QueueStackTargetsUpdate() { stackTargetUpdateNeeded = true; }
	inline void QueuePositionUpdate() { positionUpdateNeeded = true; }
	inline void QueueProrataOrderUpdate() { prorataOrderUpdateNeeded = true; }
	inline void QueueStrategyStatusUpdate() { strategyStatusUpdateNeeded = true; }
	inline void QueueSettingsUpdate() { settingsUpdateNeeded.store(true, std::memory_order_release); }
	inline void QueueProrataSettingsUpdate() { prorataSettingsUpdateNeeded.store(true, std::memory_order_release); }
	inline void QueueSpreaderStrikeUpdate() { spreaderUpdateNeeded.store(true, std::memory_order_release); }

	inline const std::string &GetSymbol() const { return symbol; }
	inline uint32_t GetSecId() const { return secId; }
	inline uint32_t GetSpreaderSecId() const { return spreaderSecId; }
	inline KTN::ORD::KOrderExchange::Enum GetQuoteExchange() const { return quoteExchange; }

	void OnInternalProrataProfileUpdate(const ProrataProfileUpdateMessage &msg);
	std::optional<ProrataProfileUpdateMessage> GetProrataProfile(const std::string &name) const;

	tbb::concurrent_queue<akl::OrderUpdate> pendingOrderUpdates;
	tbb::concurrent_queue<ProrataProfileUpdateMessage> pendingInternalProrataProfileUpdates;

	void ProcessProrataProfileUpdateFromOtherStrategy(const ProrataProfileUpdateMessage &msg);

protected:
	AklStackerParams settings;

	void runStatusThread();
	void checkTimers(const Timestamp now);

	std::string symbol {""};
	std::string spreaderSymbol {""};
	int secId { 0 };
	int spreaderSecId { 0 };
	KTN::ORD::KOrderExchange::Enum quoteExchange { KTN::ORD::KOrderExchange::Enum::CME };
	std::thread statusThread;
	std::atomic<bool> statusThreadRunning { false };
	bool _verboseLog { false };

private:
	AklStacker(const AklStacker &other) = delete;
	AklStacker &operator=(const AklStacker &other) = delete;
	AklStacker(AklStacker &&other) = delete;
	AklStacker &operator=(AklStacker &&other) = delete;

	Book book[2];
	Instrument *instr { nullptr };
	PriceConverter *priceConverter { nullptr };
	Time time;
	StrategyNotifier *notifier;

	QuoterV3 quoter;
	OrderPoolType orderPool;

	StrategyMessagePublisher publisher;

	std::mutex mutex;
	Status status { Status::PAUSED };

	std::vector<KTN::OrderPod> recoveredOrders;

	void doStart();
	void doStop();
	void processStatusUpdate(const int newStatus);

	void processBookUpdate(const KT01::DataStructures::MarketDepth::DepthBook &md, Book &b) const;

	Timestamp lastTimerCheckTime { Time::Min() };
	static constexpr Duration timerCheckInterval { Millis(10) };

	Timestamp lastReportingTime { Time::Min() };
	static constexpr Duration reportingInterval { Seconds(2) };

	Timestamp lastStackInfoSendTime { Time::Min() };
	static constexpr Duration stackInfoInterval { Seconds(1) };
	bool stackInfoUpdateNeeded { false };

	Timestamp lastStackTargetSendTime { Time::Min() };
	static constexpr Duration stackTargetInterval { Millis(500) };
	bool stackTargetUpdateNeeded { false };

	Timestamp lastRiskInfoSendTime { Time::Min() };
	static constexpr Duration riskInfoInterval { Seconds(5) };

	Timestamp lastSettingsUpdateTime { Time::Min() };
	static constexpr Duration settingsUpdateInterval { Millis(250) };
	std::atomic_bool settingsUpdateNeeded { false };

	Timestamp lastProrataSettingsUpdateTime { Time::Min() };
	static constexpr Duration prorataSettingsUpdateInterval { Millis(250) };
	std::atomic_bool prorataSettingsUpdateNeeded { false };

	Timestamp lastPositionSendTime { Time::Min() };
	static constexpr Duration positionUpdateInterval { Millis(250) };
	std::atomic_bool positionUpdateNeeded { false };

	Timestamp lastProrataOrderUpdate { Time::Min() };
	static constexpr Duration prorataOrderUpdateInterval { Millis(500) };
	std::atomic_bool prorataOrderUpdateNeeded { false };

	Timestamp lastStrategyStatusUpdate { Time::Min() };
	static constexpr Duration strategyStatusUpdateInterval { Millis(100) };
	std::atomic_bool strategyStatusUpdateNeeded { false };

	Timestamp lastSpreaderUpdateTime { Time::Min() };
	static constexpr Duration spreaderUpdateInterval { Millis(500) };
	std::atomic_bool spreaderUpdateNeeded { false };

	Timestamp lastStopHeartbeatCheckTime { Time::Min() };
	static constexpr Duration stopHeartbeatCheckInterval { Millis(500) };

	Timestamp lastInvalidBookLog { Time::Min() };
	static constexpr Duration invalidBookLogInterval { Seconds(1) };

	Timestamp lastQuoterCheckTime { Time::Min() };
	static constexpr Duration quoterCheckInterval { Seconds(5) };

	using ProrataProfileMapType = std::unordered_map<std::string, ProrataProfileUpdateMessage>;
	ProrataProfileMapType prorataMap;

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void initProrataProfile()
	{
		const std::string &profileName = settings.quoteInstrumentSettings.GetSideSettings<SIDE>().prorataProfile;
		if (!profileName.empty())
		{
			ProrataProfileMapType::const_iterator it = prorataMap.find(profileName);
			if (it != prorataMap.end())
			{
				setProrataProfile<SIDE>(it->second);
				LOGINFO("AklStacker {} | Initializing {} prorata profile {}.",
					StrategyId(), KTN::ORD::KOrderSide::toString(SIDE), profileName);
			}
			else
			{
				LOGWARN("AklStacker {} | Not setting {} prorata profile because it's not found",
					StrategyId(), KTN::ORD::KOrderSide::toString(SIDE));
			}
		}
		else
		{
			LOGINFO("AklStacker {} | Not setting {} prorata profile because name is empty",
				StrategyId(), KTN::ORD::KOrderSide::toString(SIDE));
		}

	}

	template<KTN::ORD::KOrderSide::Enum SIDE>
	void setProrataProfile(const ProrataProfileUpdateMessage &msg);

	void initProrataProfiles();

	void initStacks();

	using PendingProrataAdoptListType = std::vector<StackerProrataOrderAdoptMessage>;
	PendingProrataAdoptListType pendingProrataAdoptList;
	using PendingProrataAbandonListType = std::vector<StackerProrataOrderAbandonMessage>;
	PendingProrataAbandonListType pendingProrataAbandonList;
};

}
