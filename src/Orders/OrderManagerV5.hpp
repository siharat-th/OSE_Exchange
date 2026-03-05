//============================================================================
// Name        	: OrderMgrV4.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 17, 2023 11:16:58 AM
//============================================================================

#ifndef SRC_ORDERS_ORDERMGRV5_HPP_
#define SRC_ORDERS_ORDERMGRV5_HPP_

//#define NOREPORT_FILLS 1

#pragma once



#include <Orders/Order.hpp>
#include <Orders/OrderPod.hpp>
#include <Orders/IdGen.h>
#include <Orders/OrderUtils.hpp>
#include <Orders/IOrderHandler.hpp>
#include <AlgoSupport/algo_structs.hpp>

#include <Reporters/OrderReporterV6.hpp>
using namespace KTN::REPORTER;

#include <KT01/SecDefs/CmeSecMasterV2.hpp>
using namespace KT01::SECDEF::CME;

#include <Maps/CustomHFTMap.hpp>
#include <Maps/SPSCRingBuffer.hpp>
using namespace KTN::Core;

#include <KT01/Core/Log.hpp>
using namespace std;

#include <KT01/Core/NamedThread.hpp>

#include <tbb/concurrent_unordered_map.h>

using namespace KTN;
using namespace KTN::ORD;

/**
 * @brief The OrderManagerV5 class is responsible for managing orders and their execution.
 * It implements the IOrderHandler interface and inherits from the NamedThread class.
 */
class OrderManagerV5 : public IOrderHandler, public NamedThread
{
public:
	/**
  * @brief Constructs an OrderManagerV5 object.
  * @param core The core number.
  * @param owner The owner of the order manager.
  */
	OrderManagerV5(int core, string owner);

	/**
  * @brief Destroys the OrderManagerV5 object.
  */
	virtual ~OrderManagerV5();

	/**
  * @brief Stops the order manager.
  */
	void Stop();

	/**
  * @brief Loads a template order.
  * @param ord The template order to load.
  */
	void LoadTemplate(const KTN::Order& ord);

	/**
  * @brief Gets the order with the specified request ID.
  * @param reqid The request ID of the order.
  * @return The order with the specified request ID.
  */
	KTN::OrderPod GetOrder(uint64_t reqid);

	/**
  * @brief Checks if an order with the specified request ID exists.
  * @param reqid The request ID of the order.
  * @return True if an order with the specified request ID exists, false otherwise.
  */
	bool Exists(uint64_t reqid);

	/**
  * @brief Clears all orders.
  */
	void Clear();

	/**
  * @brief Clears all dead orders.
  */
	void ClearDeadOrders();

	/**
  * @brief Gets the count of orders.
  * @return The count of orders.
  */
	int Count();

	/**
  * @brief Prints the orders.
  */
	void Print();

	/**
  * @brief Updates the meta parameters with the given algorithm metadata.
  * @param meta The algorithm metadata.
  * @param certnr The flag indicating whether the algorithm is certified.
  */
	void UpdateMetaParams(const algometa& meta);

	/**
  * @brief Enables or disables the order reporter.
  * @param value True to enable the order reporter, false to disable it.
  */
	inline void EnableOrderReporter(bool value)
	{
		_rpt = value;
	}

	/**
  * @brief Sets whether to process spread legs.
  * @param value True to process spread legs, false otherwise.
  */
	inline void ProcessSpreadLegs(bool value)
	{
		_spreadlegs = value;
	}

	/**
  * @brief Handles a banked order.
  * @param ord The banked order to handle.
  */
	void onBankedOrder(KTN::Order& ord);

	/**
  * @brief Handles a new sent order.
  * @param ord The new sent order to handle.
  */
	void onSentOrderNew(KTN::OrderPod& ord);

	/**
  * @brief Handles a modified sent order.
  * @param ord The modified sent order to handle.
  */
	void onSentOrderMod(KTN::OrderPod& ord);

	/**
  * @brief Handles a canceled sent order.
  * @param ord The canceled sent order to handle.
  */
	void onSentOrderCxl(KTN::OrderPod& ord);

	/**
  * @brief Handles a fast sent order.
  * @param ord The fast sent order to handle.
  */
	void onSentOrderFast(KTN::OrderPod& ord);

	/**
  * @brief Handles an order book sent order.
  * @param ord The order book sent order to handle.
  * @param info The user info associated with the order.
  */
	void onSentOrderOrdbook(KTN::OrderPod& ord, UserInfo& info);

// 	/**
//   * @brief Updates an order with the specified request ID.
//   * @param reqid The request ID of the order.
//   * @param cmeid The CME ID of the order.
//   * @param status The status of the order.
//   * @param state The state of the order.
//   * @param qty The quantity of the order.
//   * @param timestamp The timestamp of the order update.
//   * @return The number of orders updated.
//   */
// 	int UpdateOrder(uint64_t reqid, uint64_t cmeid, KOrderStatus::Enum status, KOrderState::Enum state,
// 			uint32_t qty, uint64_t timestamp);


	/**
  * @brief Generic order update handler
  * @param ord The  order to update.
  * @param status The new status of the order.
  * @param state The new state of the order.
  */
	void minOrderUpdate(KTN::OrderPod &ord, KOrderStatus::Enum status, KOrderState::Enum state);

	/**
  * @brief Updates an order with the specified request ID.
  * @param reqid The request ID of the order.
  * @param exchordid The CME ID of the order.
  * @param status The status of the order.
  * @param state The state of the order.
  * @param qty The quantity of the order.
  * @param timestamp The timestamp of the order update.
  * @return The number of orders updated.
  */
	int UpdateOrder(uint64_t reqid, uint64_t exchordid, KOrderStatus::Enum status, KOrderState::Enum state,
			uint32_t qty, uint64_t timestamp);



	/**
  * @brief Handles the case where we need to add an order as-is
  * NOTE: We add the order "in state" to the order manager, meaning it's already
  * processed/handled somewhere else
  * @param ord The new order to handle.
  */
	void minOrderAddInState(KTN::OrderPod& ord);

   

    /**
  * @brief Handles a minimum order acknowledgment.
  * @param ord The minimum order to acknowledge.
  */
	void minOrderAck(KTN::OrderPod& ord);

	/**
  * @brief Handles a minimum order cancellation.
  * @param ord The minimum order to cancel.
  */
	void minOrderCancel(KTN::OrderPod& ord);

	/**
  * @brief Handles a minimum order modification.
  * @param ord The minimum order to modify.
  */
	void minOrderModify(KTN::OrderPod& ord);

	/**
  * @brief Handles the execution of a minimum order.
  * @param ord The minimum order to execute.
  * @param hedgeSent The flag indicating whether the hedge order was sent.
  */
	void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent);

	/**
  * @brief Handles the execution of a minimum order spread leg.
  * @param ord The minimum order spread leg to execute.
  */
	void minOrderExecSpreadLeg(KTN::OrderPod& ord);

	/**
  * @brief Handles the rejection of a minimum order.
  * @param ord The minimum order to reject.
  * @param text The rejection text.
  */
	void minOrderReject(KTN::OrderPod& ord, const string& text);

	/**
  * @brief Handles the rejection of a minimum order cancellation.
  * @param ord The minimum order cancellation to reject.
  * @param text The rejection text.
  */
	void minOrderCancelReject(KTN::OrderPod& ord, const string& text);

	/**
  * @brief Handles the rejection of a minimum order cancel/replace.
  * @param ord The minimum order cancel/replace to reject.
  * @param text The rejection text.
  */
	void minOrderCancelReplaceReject(KTN::OrderPod& ord, const string& text);

	//IOrderHandler
	virtual void ProcessOrderID(uint64_t ordid);
	virtual void ProcessOrder(OrderPod &ord);
	virtual void ProcessSentOrder(OrderPod &ord);
	virtual void ProcessRejectedOrder(OrderPod &ord, const string& text);
	virtual void RegisterOrderTemplate(Order& ord);

	//order queries
	bool GetByOrdernum(KTN::OrderPod& ord, const string& ordernum);
	bool GetByExchId(KTN::OrderPod& ord, uint64_t exchordid);
	void Get(KTN::OrderPod& ord, uint64_t reqid);

	vector<KTN::OrderPod> GetAll();
	vector<KTN::OrderPod> GetWorking();
	void GetBySecIdSideStrat(vector<KTN::OrderPod>& ords, int32_t secid, const KOrderSide::Enum side, int stratid);
	bool GetBySecIdSideStrat(KTN::OrderPod& ord, int32_t secid, const KOrderSide::Enum side, int stratid);
	void GetByStratId(vector<KTN::OrderPod>& ords, int stratid);
	void GetByAlgoTrigger(vector<KTN::OrderPod>& ords, const KOrderAlgoTrigger::Enum trig);
	void GetByAlgoTriggerSecId(vector<KTN::OrderPod>& ords, const KOrderAlgoTrigger::Enum trig, int32_t secid);
	void GetByAlgoTriggerPrice(vector<KTN::OrderPod>& ords, const KOrderAlgoTrigger::Enum trig, int32_t secid, int32_t price);
	bool GetAtPrice(vector<KTN::OrderPod>& orders, int32_t secid, KOrderSide::Enum side, int32_t price,
			KOrderAlgoTrigger::Enum trig);
	void GetBySecId(vector<KTN::OrderPod>& ords, int32_t secid);

	int GetSideExposure(int32_t secid, KOrderSide::Enum side, KOrderAlgoTrigger::Enum trig);

	bool HasInflight(const string& tag);
	bool HasInflight();
	
	void ClearInflight();

private:
	/**
  * @brief Logs a message with the specified color.
  * @param szMsg The message to log.
  * @param iColor The color of the log message.
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

	/**
  * @brief Runs the order manager.
  */
	void Run();

	/**
  * @brief Gets the template order from the given order pod.
  * @param ord The order pod.
  * @return The template order.
  */
	KTN::Order GetFromTemp(const KTN::OrderPod& ord)
	{
		Order temp = _templates[make_pair(ord.callbackid, ord.secid)];
		return temp;
	}

private:
	CustomHFTMap _ords;
	
	algometa _PARAMS;

	int _CORE;

	int _inflight_count;

	SPSCRingBuffer<OrderPod> _qOrdProcessed;
	
	std::thread _ordthread;
	std::atomic<bool> _CLEARFLAG;
	std::atomic<bool> _PARAMSFLAG;

	std::unordered_map<uint64_t, UserInfo> _ugomap;

	std::atomic<bool> _ORDS_ACTIVE;
	bool _rpt;
	bool _spreadlegs;
	long _cnt = 0;
	int _version;
	string _NAME;
	string _owner;

	//session status keeper
	struct hash_pair {
		template <class T1, class T2>
		size_t operator()(const pair<T1, T2>& p) const
		{
			auto hash1 = std::hash<T1>{}(p.first);
			auto hash2 = std::hash<T2>{}(p.second);
			return hash1 ^ hash2;
		}
	};

	std::unordered_map< std::pair<int,int>, KTN::Order, hash_pair> _templates;
};

#endif /* SRC_ORDERS_ALGOORDERMGR_HPP_ */
