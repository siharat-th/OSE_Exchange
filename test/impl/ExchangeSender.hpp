#pragma once

#include <ExchangeHandler/IExchangeSender.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <iostream>
#include <vector>

#include <KT01/Core/Log.hpp>

namespace akl::test::impl
{

class ExchangeSender : public IExchangeSender
{
public:
	struct transaction
	{
		KTN::OrderPod order;
		KTN::ORD::KOrderAction::Enum action;
	};

	ExchangeSender() : IExchangeSender()
	{

	}

	virtual int RegisterCallback(SessionCallback* cb) override final
	{
		return 0;
	}

	/**
  * @brief Send an order to the exchange.
  * @param ord The order to send.
  * @param action The action to perform on the order.
  */
	virtual void Send(KTN::OrderPod& ord, int action) override final
	{
		const KTN::ORD::KOrderAction::Enum kAction = static_cast<KTN::ORD::KOrderAction::Enum>(action);
		std::cout
			<< KTN::ORD::KOrderAction::toString(kAction) << " "
			<< ord.ordernum << " "
			<< KTN::ORD::KOrderSide::toString(ord.OrdSide) << " "
			<< ord.quantity << "@" << ord.price.AsShifted()
			<< std::endl;

		if (action == KTN::ORD::KOrderAction::ACTION_NEW)
		{
			ord.leavesqty = ord.quantity;
		}

		transactions.push_back({ ord, kAction });
	}

  /**
   * @brief Change the callback ID of an order and notify the exchange sender interface.
   * @param ord The order to change.
   * @param newcallbackid The new callback ID to set.
   */
	virtual void RedirectOrderCallback(KTN::Order& ord, int newcallbackid, uint64_t ordreqid) override final
	{

	}

	/**
  * @brief Send a hedge instruction to the exchange.
  * @param ordereqid The order request ID.
  * @param hdg The spread hedge vector.
  */
	virtual void SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3>& hdg) override final
	{

	}

	/**
  * @brief Send an order with hedge to the exchange.
  * @param oh The order with hedge.
  */
	virtual void SendOrderWithHedge(OrderWithHedge& oh) override final
	{

	}

	/**
  * @brief Send an algorithm JSON command to the exchange.
  * @param json The JSON string.
  * @param cmd The exchange command.
  */
	virtual void AlgoJson(std::string json, ExchCmd::Enum cmd) override final
	{

	}

	/**
  * @brief Sends a generic exchange Command Instruction; usually mass order or action related
  * @param inst Instruction object
	 */
	virtual void ExchCommand(Instruction inst) override final
	{

	}

	 /**
  * @brief Handler for orders recovered on startup, either from the exchange or from the database. 
  * @details This is primarily for orders working in a long-running algo such as GTCs etc or 
  * for orders in an algo recovered after a disconnectino. 
  * @note Orders can be from an order status request or from retransmission of orders from the exchange.
  * @param ord The FULL order that's been recovered, with all details.
  */
	virtual void OrderRecovered(KTN::OrderPod& ord, std::string guid, std::string tag) override final
	{

	}

	virtual void GetManualOrdersBySecId(std::vector<KTN::OrderPod>& ords, int32_t secId) override final
	{

	}

	virtual void MoveOrderToStrategy(const KTN::OrderPod& ord) override final
	{

	}

	bool ReadNothing() const
	{
		return readIndex == transactions.size();
	}

	bool ReadSubmit(const KTN::ORD::KOrderSide::Enum side, int qty, double px)
	{
		if (readIndex == transactions.size())
		{
			KT01_LOG_INFO(__CLASS__, "No transactions found");
			return false;
		}
		if (transactions[readIndex].action != KTN::ORD::KOrderAction::ACTION_NEW)
		{
			KT01_LOG_INFO(__CLASS__, "Actions is not ACTION_NEW, but {}", KTN::ORD::KOrderAction::toString(transactions[readIndex].action));
			return false;
		}
		if (transactions[readIndex].order.OrdSide != side)
		{
			KT01_LOG_INFO(__CLASS__, "Expected side {} does not match actual side {}", KTN::ORD::KOrderSide::toString(side), KTN::ORD::KOrderSide::toString(transactions[readIndex].order.OrdSide));
			return false;
		}
		if (transactions[readIndex].order.quantity != qty)
		{
			KT01_LOG_INFO(__CLASS__, "Expected quantity {} does not match actual quantity {}", qty, transactions[readIndex].order.quantity);
			return false;
		}
		if (transactions[readIndex].order.price.AsUnshifted() != px)
		{
			KT01_LOG_INFO(__CLASS__, "Expected price {} does not match actual price {}", px, transactions[readIndex].order.price.AsUnshifted());
			return false;
		}
		++readIndex;
		return true;
	}

	bool ReadSubmit(const KTN::ORD::KOrderSide::Enum side, int qty, double px, KTN::OrderPod &o)
	{
		if (readIndex == transactions.size())
		{
			KT01_LOG_INFO(__CLASS__, "No transactions found");
			return false;
		}
		if (transactions[readIndex].action != KTN::ORD::KOrderAction::ACTION_NEW)
		{
			KT01_LOG_INFO(__CLASS__, "Actions is not ACTION_NEW, but {}", KTN::ORD::KOrderAction::toString(transactions[readIndex].action));
			return false;
		}
		if (transactions[readIndex].order.OrdSide != side)
		{
			KT01_LOG_INFO(__CLASS__, "Expected side {} does not match actual side {}", KTN::ORD::KOrderSide::toString(side), KTN::ORD::KOrderSide::toString(transactions[readIndex].order.OrdSide));
			return false;
		}
		if (transactions[readIndex].order.quantity != qty)
		{
			KT01_LOG_INFO(__CLASS__, "Expected quantity {} does not match actual quantity {}", qty, transactions[readIndex].order.quantity);
			return false;
		}
		if (transactions[readIndex].order.price.AsUnshifted() != px)
		{
			KT01_LOG_INFO(__CLASS__, "Expected price {} does not match actual price {}", px, transactions[readIndex].order.price.AsUnshifted());
			return false;
		}
		o = transactions[readIndex].order;
		++readIndex;
		return true;
	}

	bool ReadModify(KTN::OrderPod &o, double px)
	{
		if (readIndex == transactions.size())
		{
			KT01_LOG_INFO(__CLASS__, "No transactions found");
			return false;
		}
		if (transactions[readIndex].action != KTN::ORD::KOrderAction::ACTION_MOD)
		{
			KT01_LOG_INFO(__CLASS__, "Actions is not ACTION_MOD, but {}", KTN::ORD::KOrderAction::toString(transactions[readIndex].action));
			return false;
		}
		if (o.orderReqId != transactions[readIndex].order.orderReqId)
		{
			KT01_LOG_INFO(__CLASS__, "Order does not match, expected {} but got {}", o.orderReqId, transactions[readIndex].order.orderReqId);
			return false;
		}
		if (transactions[readIndex].order.price.AsUnshifted() != px)
		{
			KT01_LOG_INFO(__CLASS__, "Expected price {} does not match actual price {}", px, transactions[readIndex].order.price.AsUnshifted());
			return false;
		}
		o = transactions[readIndex].order;
		++readIndex;
		return true;
	}

	bool ReadQtyModify(KTN::OrderPod &o, int qty)
	{
		if (readIndex == transactions.size())
		{
			KT01_LOG_INFO(__CLASS__, "No transactions found");
			return false;
		}
		if (transactions[readIndex].action != KTN::ORD::KOrderAction::ACTION_MOD)
		{
			KT01_LOG_INFO(__CLASS__, "Actions is not ACTION_MOD, but {}", KTN::ORD::KOrderAction::toString(transactions[readIndex].action));
			return false;
		}
		if (o.orderReqId != transactions[readIndex].order.orderReqId)
		{
			KT01_LOG_INFO(__CLASS__, "Order does not match, expected {} but got {}", o.orderReqId, transactions[readIndex].order.orderReqId);
			return false;
		}
		if (transactions[readIndex].order.quantity != qty)
		{
			KT01_LOG_INFO(__CLASS__, "Expected quantity {} does not match actual quantity {}", qty, transactions[readIndex].order.quantity);
			return false;
		}
		o = transactions[readIndex].order;
		++readIndex;
		return true;
	}

	bool ReadCancel(const KTN::OrderPod &o)
	{
		if (readIndex == transactions.size())
		{
			KT01_LOG_INFO(__CLASS__, "No transactions found");
			return false;
		}
		if (transactions[readIndex].action != KTN::ORD::KOrderAction::ACTION_CXL)
		{
			KT01_LOG_INFO(__CLASS__, "Actions is not ACTION_CXL, but {}", KTN::ORD::KOrderAction::toString(transactions[readIndex].action));
			return false;
		}
		if (o.orderReqId != transactions[readIndex].order.orderReqId)
		{
			KT01_LOG_INFO(__CLASS__, "Order does not match, expected {} but got {}", o.orderReqId, transactions[readIndex].order.orderReqId);
			return false;
		}
		++readIndex;
		return true;
	}

	bool GetTransaction(transaction &t)
	{
		if (readIndex == transactions.size())
		{
			return false;
		}
		t = transactions[readIndex];
		++readIndex;
		return true;
	}

	size_t GetTransactionCount() const
	{
		return transactions.size();
	}

private:
	std::vector<transaction> transactions;
	size_t readIndex { 0 };
};

}
