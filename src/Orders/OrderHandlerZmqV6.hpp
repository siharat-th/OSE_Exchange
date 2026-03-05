/*
 * OrderHandlerZmqBase.hpp
 *
 *  Created on: Nov 1 2023
 *      Author: sgaer
 */

#ifndef ORDERHANDLERZMQV6_BASE_HPP_
#define ORDERHANDLERZMQV6_BASE_HPP_

#pragma once


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;


#include <KT01/Core/Settings.hpp>
#include <KT01/Core/NamedThread.hpp>

#include <Orders/OrderNums.hpp>
#include <Orders/Order.hpp>
#include <Orders/OrderPod.hpp>
#include <Orders/OrderApiFunctions.hpp>
#include <Orders/OrderUtils.hpp>
#include <Orders/OrderManagerV5.hpp>
#include <Orders/OrderRecoveryService.hpp>
#include <Orders/OrderNums.hpp>

#include <Json/JsonOrderDecoders.hpp>

#include <ExchangeHandler/IExchangeSender.hpp>
#include <ExchangeHandler/session/SessionCallback.hpp>

#include <ZMQ/ZmqSubscriberV5.hpp>


#include <exchsupport/cme/msgw/CmeMsgw.hpp>
#include <exchsupport/cme/il3/IL3AppHelpers.hpp>
#include <exchsupport/cme/settings/CmeSettingsHelper.hpp>
using namespace KT01::MSGW;
using namespace KTN::CME::IL3;

#include <KT01/SecDefs/CfeSecMaster.hpp>
#include <KT01/SecDefs/CmeSecMasterV2.hpp>
#include <KT01/SecDefs/EquitySecMaster.hpp>
#include <KT01/SecDefs/OseSecMaster.hpp>
using namespace KT01::SECDEF::CME;
using namespace KT01::SECDEF::OSE;
using namespace KT01::SECDEF::CFE;
using namespace KT01::SECDEF::EQT;


#include <Maps/CustomHFTMap.hpp>
using namespace KTN::Core;

#include <Reporters/OrderReporterV6.hpp>
using namespace KTN::REPORTER;

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

#include <Rest/RestApi.hpp>
#include <Rest/JsonUrlReader.hpp>
#include <Rest/JsonUrlPoster.hpp>

using namespace std;


namespace KTN{ namespace ORD {

/**
 * @brief The OrderHandlerZmqV6 class handles the ZMQ communication for order handling.
 * It implements the SessionCallback and IZmqSubscriber interfaces.
 */
class OrderHandlerZmqV6 : public SessionCallback, public IZmqSubscriber//, public NamedThread
{
public:
	/**
  * @brief Constructs an OrderHandlerZmqV6 object. 
  * @param exch The exchange sender object.
  * @param cbid The callback ID.
  */
	OrderHandlerZmqV6(IExchangeSender& exch);

	/**
  * @brief Destroys the OrderHandlerZmqV6 object.
  */
	virtual ~OrderHandlerZmqV6();

	
	/**
  * @brief Registers an order to be recognized by the order book. Does not send.
  * @param ord The order template to register.
  */
	void RegisterOrderWithOrderbook(Order& ord);

	/**
  * @brief Stops the order handler.
  */
	void Stop();

	/**
  * @brief Prints the order handler.
  */
	void Print();


	/**
  * @brief Clears the order handler.
  */
	inline void Clear()
	{
		_CLEARFLAG.store(true, std::memory_order_relaxed);
	}

	void GetOrdersBySecId(std::vector<KTN::OrderPod> &ords, int32_t secId);

private:
	/**
  * @brief Logs a message.
  * @param szMsg The message to log.
  * @param iColor The color of the log message.
  * @param persist Flag indicating whether to persist the log message.
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO, bool persist = false);

	/**
  * @brief Handles a published message.
  * @param szTopic The topic of the message.
  * @param szMsg The message.
  */
	void onPubMsg(std::string szTopic, std::string szMsg) override;

	/**
  * @brief Initializes the ZMQ communication.
  * @param ArgoServer The Argo server address.
  * @param ArgoPort The Argo server port.
  */
	void initZmq(std::string ArgoServer, int ArgoPort);

	/**
  * @brief Handles a JSON message.
  * @param json The JSON message.
  */
	void onJson(const std::string& json);

	//session callback

	/**
         * @brief Returns the GUID of the callback.
         * @return The GUID as a string.
         */
    virtual const std::string& getGuid() const {
		cout << "[ZmqOrderHandlerV6 WARNING] getGuid" << endl;
		return _GUID;
	}

	virtual void onHeartbeat(int msgseqnum);
	virtual void onSessionStatus(string exch, int mktsegid, int protocol, string desc, string state);
	void onSpreadLegOrderExecution(KTN::Order& ord);

	virtual void minOrderAck(KTN::OrderPod& ord) override;
	virtual void minOrderCancel(KTN::OrderPod& ord) override;
	virtual void minOrderModify(KTN::OrderPod& ord) override;
	virtual void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) override;

	virtual void minOrderReject(KTN::OrderPod& ord, const string& text);
	virtual void minOrderCancelReject(KTN::OrderPod& ord, const string& text);
	virtual void minOrderCancelReplaceReject(KTN::OrderPod& ord, const string& text);

	virtual void onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid,KOrderSide::Enum side,
				uint32_t qty, int32_t price9, uint16_t instindex);
	virtual void onOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
					uint32_t qty, int32_t price, KOrderAlgoTrigger::Enum trig);

	/**
  * @brief RETRIEVES the next order ID from the OrderNums Singleton
  * @return The next order ID.
  */
	inline std::string NextOrdId()
	{
		return OrderNums::instance().NextClOrdId();
	}

	/**
  * @brief Checks if a JSON message has a valid header.
  * @param json The JSON message.
  * @return True if the header is valid, false otherwise.
  */
	bool ValidHeader(const std::string& json);

	/**
  * @brief Handles incoming orders from a JSON message.
  * @param json The JSON message.
  */
	void HandleOrders(const string& json);

	
	/**
  * @brief Handles a new order.
  * @param ord The new order.
  */
	void HandleNewOrder(const KTN::Order& ord);

	/**
  * @brief Handles a modified order.
  * @param ord The modified order.
  */
	void HandleModifyOrder(const KTN::Order& ord);

	/**
  * @brief Handles a canceled order.
  * @param ordernum The order number to cancel.
  */
	void HandleCancelOrder(const std::string &ordernum, const std::string &user);

	/**
  * @brief Handles mass requests for canceling all orders.
  * @param ord The order to validate.
  */
	void HandleCancelAll(KTN::Order& ord);

	/**
  * @brief Sends an order.
  * @param ord The order to send.
  */
	void SendOrder(KTN::OrderPod& ord);

	/**
	  * @brief Sends a command instruction
	  * @param cmd The instruction to send
	  */
	void SendCommand(Instruction& cmd);

	/**
  * @brief Finds the request ID for a given order number.
  * @param ordernum The order number.
  * @return The request ID.
  */
	inline uint64_t FindReqId(string ordernum)
	{
		for(auto kv : _reqIdMap)
		{
			if (kv.second == ordernum)
				return kv.first;
		}
		return 0;
	}

	/**
  * @brief Finds the request ID for a given order number.
  * @param ordernum The order number.
  * @return The request ID.
  */
	inline uint64_t FindReqId(uint64_t exchid)
	{
		if (_exchIdMap.find(exchid) == _exchIdMap.end())
			return 0;
		return _exchIdMap[exchid];
	}

	inline uint64_t FindReqId(char* clordid)
	{
		if (_clOrdIdMap.find(clordid) == _clOrdIdMap.end())
			return 0;
		return _clOrdIdMap[clordid];
	}

	/**
  * @brief Checks the status of an order from a state perspective. Mostly used to see if we have 
  * recovered an order from a status request or a retransmission.
  * @param pod The order to process.
  * @return True if the order is OK to process and update database, false otherwise.
  */
	inline bool ProcessOrderStatus(KTN::OrderPod& pod)
	{
		if (_SHOW_MSGS)
		{
			string statustype = (pod.possRetransmit == true) ? "ProcessRetransmit" : "ProcessOrderStatus" ;
			string action = KOrderAction::toString(pod.OrdAction);
			LogMe("[Info] "+ statustype + " Order " + std::to_string(pod.exchordid)
			+ ": CBID=" + std::to_string(pod.callbackid) 
			+ " CLORDID=" + std::string(pod.ordernum) + " STATUS=" + KOrderStatus::toString(pod.OrdStatus)
			+ " STATE=" + KOrderState::toString(pod.OrdState) + " SECID=" + std::to_string(pod.secid)
			+ " ACTION=" + action, LogLevel::WARN);
		}

		bool res = true;

		if (pod.OrdState == KOrderState::STATUS_UPDATE || pod.possRetransmit == true)
		{
			//if it's been passed to us here, it's not an active Algo order or it's an 
			//active order book order. 
			//check and see if we have this order's request id and meta info, otherwise, fill it in:
			bool found = AddOrderFromStatusRequest(pod);
			bool statusupdate = (pod.OrdState == KOrderState::STATUS_UPDATE) ? true : false;
			
			if (!found)
			{
				string action = KOrderAction::toString(pod.OrdAction);
				string status = KOrderStatus::toString(pod.OrdStatus);
				LogMe("ProcessStatus: Order " + std::string(pod.ordernum) 
				+ " NOT FOUND IN ORDERS. ACTION=" + action + " STATUS=" + status, LogLevel::ERROR);
				
				res = false;
			}

			cout << "***** OK STATUS UPDATE: " << pod.OrdStatus << " " << pod.OrdState << " statusupdate=" << statusupdate << " found=" << found << endl;

			//we return false because we don't want to send another fill to the database which has presumably already been sent.
			//this would be reporting a duplicate fill to the database and our position would be incorrect. 
			if (statusupdate && (pod.OrdStatus == KOrderStatus::PARTIALLY_FILLED || pod.OrdStatus == KOrderStatus::FILLED))
				return false;
		}
		
		return res;
	}

	/**
  * @brief Adds an order from a status request.
  * @param pod The order to add.
  * @return True if the order was added, false otherwise.
  */
	
	inline bool AddOrderFromStatusRequest(KTN::OrderPod& pod)
	{
		Order ord;
		if (OrderRecoveryService::instance().GetOrder(pod.exchordid, ord))
		{
			//one-liner is now portable to other modules such as algos
			OrderUtils::UpdateFromApi(pod, ord);

			if (pod.possRetransmit)
			{
				pod.lastpx = 0;
				pod.lastqty = 0;
			}
		
			_exchIdMap[pod.exchordid] = pod.orderReqId;
			_reqIdMap[pod.orderReqId] = pod.ordernum;
			_clOrdIdMap[pod.ordernum] = pod.orderReqId;
			_tagmap[pod.orderReqId] = ord.tag;
			_algoidmap[pod.orderReqId] = ord.algoid;
			_algoidmap[pod.orderReqId] = ord.algoid;

			UserInfo u = UserInfo::Construct(ord.user, ord.org, ord.groupname);
			_ugoMap[pod.orderReqId] = u;

			if (_SHOW_MSGS)
				cout <<"[AddOrderFromStatusRequest DEBUG] AddFromStatus: POD_FROM_STATUS reqid=" << pod.orderReqId << " ordernum=" << pod.ordernum 
					<< " status="  << KOrderStatus::toString(pod.OrdStatus) << " fillqty=" << pod.fillqty << " lvsqty=" << pod.leavesqty
					<< "  ORD INPUTS: reqid=" << ord.orderReqId << " ordernum=" << ord.ordernum << " exchid=" << ord.exchordid
					<< " ACTION=" << KOrderAction::toString(ord.OrdAction) << " STATUS=" << KOrderStatus::toString(ord.OrdStatus) 
					<< " TRIGGER=" << KOrderAlgoTrigger::toString(ord.OrdAlgoTrig) 
					<<  " minqty=" << ord.minqty  << " dispqty=" << ord.dispqty 
					<< " user=" << u.user << " org=" << u.org << " groupname=" << u.groupname
					<< " algoid=" << ord.algoid << " tag=" << ord.tag
					<< endl;
		

			_Ords.onSentOrderOrdbook(pod, _ugoMap[pod.orderReqId]);
		
			return true;
		}

		return false;
	}
	

private:
	const std::string _GUID = "ORDERBOOK";
	std::unordered_map<string, long> _certIdMap;
	int8_t _EXCH_CALLBACK_ID ;

	std::atomic<bool> _ACTIVE;
	std::atomic<bool> _CLEARFLAG;

	CmeMsgw _msgw;

	ZmqSubscriberV5*  _zmq;
	OrderManagerV5  _Ords;

	IExchangeSender& _ordsender;

	std::unordered_map<uint64_t, uint64_t > _exchIdMap;
	std::unordered_map<uint64_t, string > _reqIdMap;
	std::unordered_map<string, uint64_t > _clOrdIdMap;
	std::unordered_map<uint64_t, uint64_t > _cme_to_req_Map;
	std::unordered_map<int32_t, string > _certids;
	std::unordered_map<uint64_t, string> _tagmap;
	std::unordered_map<uint64_t, string> _algoidmap;

	//std::unordered_map<uint64_t, Order> _apiorders;

	std::unordered_map<uint64_t, UserInfo> _ugoMap;

	std::mutex reqToPrevOrderMutex;
	std::unordered_map<uint64_t, KTN::OrderPod> reqIdToPrevOrderMap;

	std::string _SOURCE;
	std::string _ACCOUNT;
	std::string _KEY;
	bool _SESSION_ENABLED;
	bool _SHOW_MSGS;

	struct hash_pair {
		template <class T1, class T2>
		size_t operator()(const pair<T1, T2>& p) const
		{
			auto hash1 = std::hash<T1>{}(p.first);
			auto hash2 = std::hash<T2>{}(p.second);
			return hash1 ^ hash2;
		}
	};

	std::unordered_map<int, Order> _templates;

};

} }

#endif /* ORDERHANDLERZMQ_HPP_ */
