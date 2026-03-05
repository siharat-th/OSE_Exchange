//============================================================================
// Name        	: IOrderHandler.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 1, 2023 4:06:56 PM
//============================================================================

#ifndef SRC_ORDERS_IORDERHANDLER_HPP_
#define SRC_ORDERS_IORDERHANDLER_HPP_

#include <Orders/Order.hpp>
#include <Orders/OrderPod.hpp>

namespace KTN { namespace ORD {
/**
 * @brief Interface for handling order-related operations.
 */
class IOrderHandler {
public:
	/**
  * @brief Process the order ID.
  * @param ordid The order ID to process.
  */
	virtual void ProcessOrderID(uint64_t ordid) = 0;

	/**
  * @brief Process the order.
  * @param ord The order to process.
  */
	virtual void ProcessOrder(OrderPod &ord) = 0;

	/**
  * @brief Process the sent order.
  * @param ord The sent order to process.
  */
	virtual void ProcessSentOrder(OrderPod &ord) = 0;

	/**
  * @brief Process the rejected order.
  * @param ord The rejected order to process.
  * @param text The rejection text.
  */
	virtual void ProcessRejectedOrder(OrderPod &ord, const string& text) = 0;

	/**
  * @brief Register an order template.
  * @param ord The order template to register.
  */
	virtual void RegisterOrderTemplate(Order& ord) = 0;
};

}}

#endif /* SRC_ORDERS_IORDERHANDLER_HPP_ */
