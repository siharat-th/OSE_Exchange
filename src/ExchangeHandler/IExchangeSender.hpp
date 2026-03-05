//============================================================================
// Name        	: IExchangeSener.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 28, 2023 1:26:34 PM
//============================================================================

#ifndef SRC_EXCHANGEHANDLERS2_IEXCHANGESENDER_HPP_
#define SRC_EXCHANGEHANDLERS2_IEXCHANGESENDER_HPP_
#pragma once
#include <Orders/Order.hpp>
#include <Orders/OrderPod.hpp>
using namespace KTN;

#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
using namespace KTN::NET;

#include <AlgoEngines/pricing_structs.hpp>
using namespace KTN::ALGO::PRICING;

#include <ExchangeHandler/session/SessionCallback.hpp>
#include <ExchangeHandler/ExchCmdEnums.hpp>

/**
 * @brief The interface for sending messages to the exchange.
 */
class IExchangeSender {
public:
	/**
  * @brief Default constructor.
  */
	IExchangeSender();

	/**
  * @brief Destructor.
  */
	virtual ~IExchangeSender();

	/**
  * @brief Subscribe to the exchange session.
  * @param cb The session callback object.
  * @return The result of the subscription.
  */
	virtual int RegisterCallback(SessionCallback* cb) = 0;

	/**
  * @brief Send an order to the exchange.
  * @param ord The order to send.
  * @param action The action to perform on the order.
  */
	virtual void Send(KTN::OrderPod& ord, int action) = 0;


  /**
   * @brief Change the callback ID of an order and notify the exchange sender interface.
   * @param ord The order to change.
   * @param newcallbackid The new callback ID to set.
   */
  virtual void RedirectOrderCallback(KTN::Order& ord, int newcallbackid, uint64_t ordreqid) = 0;

	/**
  * @brief Send a hedge instruction to the exchange.
  * @param ordereqid The order request ID.
  * @param hdg The spread hedge vector.
  */
	virtual void SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3>& hdg) = 0;

	/**
  * @brief Send an order with hedge to the exchange.
  * @param oh The order with hedge.
  */
	virtual void SendOrderWithHedge(OrderWithHedge& oh) = 0;

	/**
  * @brief Send an algorithm JSON command to the exchange.
  * @param json The JSON string.
  * @param cmd The exchange command.
  */
	virtual void AlgoJson(std::string json, ExchCmd::Enum cmd) = 0;

	/**
  * @brief Sends a generic exchange Command Instruction; usually mass order or action related
  * @param inst Instruction object
	 */
	virtual void ExchCommand(Instruction inst) = 0;

 /**
  * @brief Handler for orders recovered on startup, either from the exchange or from the database. 
  * @details This is primarily for orders working in a long-running algo such as GTCs etc or 
  * for orders in an algo recovered after a disconnectino. 
  * @note Orders can be from an order status request or from retransmission of orders from the exchange.
  * @param ord The FULL order that's been recovered, with all details.
  */
  virtual void OrderRecovered(KTN::OrderPod& ord, std::string guid, std::string tag) = 0;

  virtual void GetManualOrdersBySecId(std::vector<KTN::OrderPod> &ords, int32_t secId) = 0;
  virtual void MoveOrderToStrategy(const KTN::OrderPod &ord) = 0;

};

/**
 * @brief The interface for sending buffer to the exchange.
 */
class IBufferSender {
	/**
  * @brief Send a buffer to the exchange.
  * @param buf The buffer to send.
  */
	virtual void SendBuffer(KTNBuf& buf) = 0;
};

#endif /* SRC_EXCHANGEHANDLERS2_IEXCHANGESENDER_HPP_ */
