/*
 * SessionCallback.hpp
 *
 *  Created on: Apr 19, 2021
 *      Author: sgaer
 */

#ifndef SESSIONCALLBACK_HPP_
#define SESSIONCALLBACK_HPP_

#include <Orders/Order.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <Orders/OrderPod.hpp>

namespace KTN {

/**
 * @brief The SessionCallback class is an abstract base class that defines the interface for session callbacks.
 *        Session callbacks are used to handle various events and actions related to a trading session.
 */
class SessionCallback
{
public:

  virtual const std::string& getGuid() const = 0;

	/**
  * @brief This method is called when a heartbeat message is received.
  * @param msgseqnum The sequence number of the heartbeat message.
  */
	virtual void onHeartbeat(int msgseqnum) = 0;

	/**
  * @brief This method is called when the session status changes.
  * @param exch The exchange name.
  * @param mktsegid The market segment ID.
  * @param protocol The protocol version.
  * @param desc The description of the session status.
  * @param state The state of the session.
  */
	virtual void onSessionStatus(string exch, int mktsegid, int protocol, string desc, string state) = 0;

	/**
  * @brief This method is called when a minimum order acknowledgement is received.
  * @param ord The order details.
  */
	virtual void minOrderAck(KTN::OrderPod& ord) = 0;

	/**
  * @brief This method is called when a minimum order cancellation is received.
  * @param ord The order details.
  */
	virtual void minOrderCancel(KTN::OrderPod& ord) = 0;

	/**
  * @brief This method is called when a minimum order modification is received.
  * @param ord The order details.
  */
	virtual void minOrderModify(KTN::OrderPod& ord) = 0;

	/**
  * @brief This method is called when a minimum order execution is received.
  * @param ord The order details.
  * @param hedgeSent Indicates whether a hedge order was sent.
  */
	virtual void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) = 0;

	//virtual void minOrderExecSpreadLeg(KTN::OrderPod& ord) = 0;

	/**
  * @brief This method is called when a minimum order rejection is received.
  * @param ord The order details.
  * @param text The rejection reason.
  */
	virtual void minOrderReject(KTN::OrderPod& ord, const string& text) = 0;

	/**
  * @brief This method is called when a minimum order cancellation rejection is received.
  * @param ord The order details.
  * @param text The rejection reason.
  */
	virtual void minOrderCancelReject(KTN::OrderPod& ord, const string& text) = 0;

	/**
  * @brief This method is called when a minimum order cancel/replace rejection is received.
  * @param ord The order details.
  * @param text The rejection reason.
  */
	virtual void minOrderCancelReplaceReject(KTN::OrderPod& ord, const string& text) = 0;

	/**
  * @brief This method is called when a hedge order is sent.
  * @param clordid The client order ID.
  * @param reqid The request ID.
  * @param secid The security ID.
  * @param side The order side.
  * @param qty The order quantity.
  * @param price The order price.
  * @param instindex The instrument index.
  */
	virtual void onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
			uint32_t qty, int32_t price, uint16_t instindex) = 0;

	/**
  * @brief This method is called when an order is sent.
  * @param clordid The client order ID.
  * @param reqid The request ID.
  * @param secid The security ID.
  * @param side The order side.
  * @param qty The order quantity.
  * @param price The order price.
  * @param trig The order algorithm trigger.
  */
	virtual void onOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
				uint32_t qty, int32_t price, KOrderAlgoTrigger::Enum trig) = 0;
};

} /* namespace KTN */

#endif /* SESSIONCALLBACK_HPP_ */
