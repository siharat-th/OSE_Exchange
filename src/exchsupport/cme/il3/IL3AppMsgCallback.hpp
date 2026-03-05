//============================================================================
// Name        	: IL3AppMsgCallback.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Feb 11, 2024 11:26:12 AM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3APPMSGCALLBACK_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3APPMSGCALLBACK_HPP_

#include <exchsupport/cme/il3/IL3Includes.hpp>
using namespace KTN;

/**
 * @brief The IL3AppMsgCallback class is an abstract base class that defines the interface for handling various CME iLink3 execution report messages and other related application level messages.
 */
class IL3AppMsgCallback {
public:
	/**
  * @brief Default constructor for IL3AppMsgCallback.
  */
	IL3AppMsgCallback();
	
	/**
  * @brief Virtual destructor for IL3AppMsgCallback.
  */
	virtual ~IL3AppMsgCallback();

	/**
  * @brief This method is called when a new execution report message is received.
  * @param data The block data of the execution report new message.
  * @param segid The segment ID.
  */
	virtual void onExecRptNew(ExecutionReportNewMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when an execution report modify message is received.
  * @param data The block data of the execution report modify message.
  * @param segid The segment ID.
  */
	virtual void onExecRptModify(ExecutionReportModifyMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when an execution report cancel message is received.
  * @param data The block data of the execution report cancel message.
  * @param segid The segment ID.
  */
	virtual void onExecRptCancel(ExecutionReportCancelMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when an execution report trade outright message is received.
  * @param data The block data of the execution report trade outright message.
  * @param segid The segment ID.
  */
	virtual void onExecRptTradeOutright(ExecutionReportTradeOutrightMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when an execution report trade spread message is received.
  * @param data The block data of the execution report trade spread message.
  * @param segid The segment ID.
  */
	virtual void onExecRptTradeSpread(ExecutionReportTradeSpreadMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when an execution report trade spread leg message is received.
  * @param data The block data of the execution report trade spread leg message.
  * @param segid The segment ID.
  */
	virtual void onExecRptTradeSpreadLeg(ExecutionReportTradeSpreadLegMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when an execution report reject message is received.
  * @param data The block data of the execution report reject message.
  * @param segid The segment ID.
  */
	virtual void onExecRptReject(ExecutionReportRejectMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when an execution report elimination message is received.
  * @param data The block data of the execution report elimination message.
  * @param segid The segment ID.
  */
	virtual void onExecRptElimination(ExecutionReportEliminationMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when an order mass action report message is received.
  * @param data The block data of the order mass action report message.
  * @param segid The segment ID.
  */
	virtual void onOrderMassActionReport(OrderMassActionReportMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when an order cancel reject message is received.
  * @param data The block data of the order cancel reject message.
  * @param segid The segment ID.
  */
	virtual void onOrderCancelReject(OrderCancelRejectMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when an order cancel replace reject message is received.
  * @param data The block data of the order cancel replace reject message.
  * @param segid The segment ID.
  */
	virtual void onOrderCancelReplaceReject(OrderCancelReplaceRejectMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when a party details definition request acknowledgement message is received.
  * @param data The block data of the party details definition request acknowledgement message.
  * @param segid The segment ID.
  */
	virtual void onPartyDetailsDefinitionRequestAck(PartyDetailsDefinitionRequestAckMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when a party details list report message is received.
  * @param data The block data of the party details list report message.
  * @param segid The segment ID.
  */
	virtual void onPartyDetailsListReport(PartyDetailsListReportMsg::BlockData* data, int segid) = 0;

	/**
  * @brief This method is called when a business reject message is received.
  * @param data The block data of the business reject message.
  * @param segid The segment ID.
  */
	virtual void onBusinessReject(BusinessRejectMsg::BlockData* data, int segid) = 0;

	// Uncomment the following methods if needed

//	/**
//	 * @brief This method is called when an execution report status message is received.
//	 * @param buf The byte pointer to the message buffer.
//	 * @param iMsgLen The length of the message.
//	 * @param segid The segment ID.
//	 */
//	virtual void onExecRptStatus(byte_ptr buf, int iMsgLen, int segid) = 0;
//
//	/**
//	 * @brief This method is called when an execution acknowledgement message is received.
//	 * @param buf The byte pointer to the message buffer.
//	 * @param iMsgLen The length of the message.
//	 * @param segid The segment ID.
//	 */
//	virtual void onExecutionAck(byte_ptr buf, int iMsgLen, int segid) = 0;
//
//	/**
//	 * @brief This method is called when a trade addendum outright message is received.
//	 * @param buf The byte pointer to the message buffer.
//	 * @param iMsgLen The length of the message.
//	 * @param segid The segment ID.
//	 */
//	virtual void onTradeAddendumOutright(byte_ptr buf, int iMsgLen, int segid) {}
//
//	/**
//	 * @brief This method is called when a trade addendum spread message is received.
//	 * @param buf The byte pointer to the message buffer.
//	 * @param iMsgLen The length of the message.
//	 * @param segid The segment ID.
//	 */
//	virtual void onTradeAddendumSpread(byte_ptr buf, int iMsgLen, int segid) {}
//
//	/**
//	 * @brief This method is called when a trade addendum spread leg message is received.
//	 * @param buf The byte pointer to the message buffer.
//	 * @param iMsgLen The length of the message.
//	 * @param segid The segment ID.
//	 */
//	virtual void onTradeAddendumSpreadLeg(byte_ptr buf, int iMsgLen, int segid) {}
};

#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3APPMSGCALLBACK_HPP_ */
