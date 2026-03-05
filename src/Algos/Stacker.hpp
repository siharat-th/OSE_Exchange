/*
 * StackerTimer.hpp
 *
 *  Created on: Jan 22, 2024
 *      Author: sgaer
 */

#ifndef STACKER_HPP_
#define STACKER_HPP_

#pragma once
#include <Algos/AlgoBaseV3.hpp>
#include <AlgoParams/StackerParams.hpp>

#include <KT01/Core/ObjectPool.hpp>

using namespace KTN::Core;
using namespace KTN::ALGO;
using namespace KTN::notify;
using namespace KTN;


namespace KTN {

/**
 * @brief The Stacker class is a derived class of AlgoBaseV3 and implements the stacking algorithm.
 * This algorithm "seeds" price levels in an automated fashion based on input parameters and current market conditions.
 */
class Stacker: public AlgoBaseV3
{
public:
	/**
  * @brief Constructs a Stacker object.
  * @param p The AlgoInitializer object.
  */
	Stacker(AlgoInitializer p);
	
	/**
  * @brief Destroys the Stacker object.
  */
	virtual ~Stacker();

	/**
  * @brief Handles the JSON input.
  * @param json The JSON string.
  */
	void onJson(std::string json)override;
	
	/**
  * @brief Handles the launch event.
  * @param json The JSON string.
  */
	void onLaunch(std::string json)override;
	
	/**
  * @brief Calculates the algorithm.
  */
	void onCalculate()override;
	
	/**
  * @brief Handles the trade event.
  * @param index The index of the trade.
  * @param price The trade price.
  * @param size The trade size.
  * @param aggressor The aggressor flag.
  */
	void onTrade(int index, double price, double size, int aggressor) override;
	
	/**
  * @brief Handles the market data event.
  * @param index The index of the market data.
  */
	void onMktData(int index) override;

	/**
  * @brief Handles the minimum order execution event.
  * @param ord The OrderPod object.
  * @param hedgeSent The hedgeSent flag.
  */
	void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) override;
	
	/**
  * @brief Handles the minimum order cancel event.
  * @param ord The OrderPod object.
  */
	void minOrderCancel(KTN::OrderPod& ord) override;
	
	/**
  * @brief Handles the minimum order cancel replace reject event.
  * @param ord The OrderPod object.
  * @param text The reject text.
  */
	void minOrderCancelReplaceReject(KTN::OrderPod& ord, const string& text) override;
	
	/**
  * @brief Handles the minimum order cancel reject event.
  * @param ord The OrderPod object.
  * @param text The reject text.
  */
	void minOrderCancelReject(KTN::OrderPod& ord, const string& text) override;

	/**
  * @brief Handles the hedge order sent event.
  * @param clordid The client order ID.
  * @param reqid The request ID.
  * @param secid The security ID.
  * @param side The order side.
  * @param qty The order quantity.
  * @param price The order price.
  * @param stratid The strategy ID.
  */
	void onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid,KOrderSide::Enum side,
						uint32_t qty, int32_t price, uint16_t stratid) override;

	/**
  * @brief Handles the order sent event.
  * @param clordid The client order ID.
  * @param reqid The request ID.
  * @param secid The security ID.
  * @param side The order side.
  * @param qty The order quantity.
  * @param price The order price.
  * @param trig The order trigger.
  */
	void onOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
						uint32_t qty, int32_t price, KOrderAlgoTrigger::Enum trig) override;

	/**
  * @brief Enables or disables the algorithm.
  * @param bEnable The enable flag.
  */
	void Enable(bool bEnable) override;
	
	/**
  * @brief Resets the position.
  */
	void ResetPosition() override;
	
	/**
  * @brief Prints the algorithm details.
  */
	void Print() override;
	
	/**
  * @brief Toggles a command.
  * @param id The command ID.
  */
	void ToggleCommand(int id) override;
	
	/**
  * @brief Restarts the algorithm.
  */
	void Restart() override;

	/**
  * @brief Gets the list of symbols.
  * @return The list of symbols.
  */
	vector<string> Symbols();

private:
	/**
  * @brief Checks if it is good to proceed with the algorithm.
  * @return True if it is good to proceed, false otherwise.
  */
	bool GoodToGo();
	
	/**
  * @brief Checks if market data is available.
  * @return True if market data is available, false otherwise.
  */
	bool GotMktData();

	/**
  * @brief Checks the stack.
  * @param index The index of the stack.
  */
	void CheckStack(int index);
	
	/**
  * @brief Runs the stack thread.
  */
	void StackThread();

	/**
  * @brief Checks the squeeze.
  */
	void CheckSqueeze();
	
	/**
  * @brief Checks the squeeze for a specific index.
  * @param index The index to check.
  */
	void CheckSqueeze(int index);

	/**
  * @brief Checks the puke condition.
  */
	void CheckPuke();
	
	/**
  * @brief Checks the puke condition for a specific index.
  * @param index The index to check.
  * @param isBid The flag indicating if it is a bid.
  * @return True if the puke condition is met, false otherwise.
  */
	bool CheckPuke(int index, bool isBid);
	
	/**
  * @brief Pukes at a specific price.
  * @param secid The security ID.
  * @param price The puke price.
  * @param side The order side.
  * @param trig The order trigger.
  * @param ratio The puke ratio.
  * @param size The puke size.
  * @return True if the puke order is sent successfully, false otherwise.
  */
	bool PukeAtPrice(int32_t secid, int price, KOrderSide::Enum side,
			KOrderAlgoTrigger::Enum trig, double ratio, int size);

	/**
  * @brief Calculates the skew for a specific leg.
  * @param leg The leg to calculate the skew for.
  * @return The calculated skew.
  */
	double CalcSkew(const instrument& leg);
	
	/**
  * @brief Checks if an order is squeezed.
  * @param reqid The request ID of the order.
  * @return True if the order is squeezed, false otherwise.
  */
	bool Squeezed(uint64_t reqid);

	/**
  * @brief Handles the received order.
  * @param incoming The incoming order.
  * @param index The index of the order.
  */
	void OrderReceived(KTN::Order& incoming, int index) override;
	
	/**
  * @brief Updates the minimum order.
  * @param ord The OrderPod object.
  */
	void OrderMinUpdate(KTN::OrderPod& ord) override;

	/**
  * @brief Loads the spread leg definitions.
  * @param json The JSON string.
  */
	void LoadSpreadLegDefs(string json);
	
	/**
  * @brief Constructs the hedge.
  * @param inst The instrument to construct the hedge for.
  */
	void ConstructHedge(instrument& inst);

	/**
  * @brief Prepares the quoting order.
  * @param ord The OrderPod object.
  * @param inst The instrument to prepare the quoting order for.
  * @param side The order side.
  * @param price The order price.
  * @param qty The order quantity.
  * @return True if the quoting order is prepared successfully, false otherwise.
  */
	bool PrepareQuotingOrder(KTN::OrderPod &ord, const instrument& inst,
			KOrderSide::Enum side, int32_t price, int qty);

	/**
  * @brief Cancels all orders.
  */
	void CancelAllOrders() override;
	
	/**
  * @brief Cancels a specific order.
  * @param ord The OrderPod object to cancel.
  */
	void CancelOrder(const KTN::OrderPod& ord);

	/**
  * @brief Updates the position on fill.
  * @param ord The filled order.
  */
	void UpdatePositionOnFill(const KTN::Order& ord);

	/**
  * @brief Reports the status.
  */
	void ReportStatus();
	
	/**
  * @brief Runs the status thread.
  */
	void StatusThread();
	
	/**
  * @brief Checks for alerts.
  */
	void AlertCheck();

	/**
  * @brief Checks if the orders are working.
  * @param ord The list of orders.
  * @param leg The leg to check.
  * @param trig The order trigger.
  * @return True if the orders are working, false otherwise.
  */
	bool Working(vector<KTN::OrderPod>& ord, instrument& leg, KOrderAlgoTrigger::Enum trig);

	/**
  * @brief Sends stack orders.
  * @param legindex The index of the leg.
  * @param price The order price.
  * @param side The order side.
  * @param largetrade The flag indicating if it is a large trade.
  * @return True if the stack orders are sent successfully, false otherwise.
  */
	bool SendStackOrders(int legindex, int price, KOrderSide::Enum side, bool largetrade);

	/**
  * @brief Prepares a new order.
  * @param ord The OrderPod object.
  * @param inst The instrument to prepare the new order for.
  * @param side The order side.
  * @param price The order price.
  * @param qty The order quantity.
  */
	void PrepareNewOrder(KTN::OrderPod &ord, const instrument& inst,
			KOrderSide::Enum side, int price, int qty);
	
	/**
  * @brief Prepares a modified order.
  * @param ord The OrderPod object.
  * @param inst The instrument to prepare the modified order for.
  * @param side The order side.
  * @param price The order price.
  * @param qty The order quantity.
  */
	void PrepareModOrder(KTN::OrderPod &ord, const instrument& inst,
			KOrderSide::Enum side, int price, int qty);

	/**
  * @brief Prints a quick order.
  * @param ord The order to print.
  * @param inst The instrument of the order.
  */
	void QuickOrdPrint(const KTN::Order& ord, const instrument& inst);

	/**
  * @brief Updates the leg state.
  * @param reqid The request ID of the order.
  * @param state The state to update.
  */
	inline void UpdateLegState(uint64_t reqid, KOrderState::Enum state)
	{
		for (int i = 0; i < _cp.LegCount; i++)
		{
			instrument& leg = _cp.Legs[i];
			if (leg.lastBuyId == reqid )
			{
				leg.buyState = state;
				if (state == KOrderState::COMPLETE)
					leg.lastBuyId = 0;
				return;
			}
			if (leg.lastSellId == reqid )
			{
				leg.sellState = state;
				if (state == KOrderState::COMPLETE)
					leg.lastSellId = 0;
				return;
			}
		}
	}

	/**
  * @brief Clears the last sent IDs of the legs.
  */
	inline void ClearLegSentIds()
	{
		for (int i = 0; i < _cp.LegCount; i++)
		{
			instrument& leg = _cp.Legs[i];
			leg.lastBuyId = 0;
			leg.lastSellId = 0;
			leg.buyState = KOrderState::UNKNOWN;
			leg.sellState = KOrderState::UNKNOWN;
		}
	}

	/**
  * @brief Checks if an order is canceled.
  * @param exchordid The CME ID of the order.
  * @return True if the order is canceled, false otherwise.
  */
	inline bool Canceled(uint64_t exchordid)
	{
		for(uint64_t id : _CxlOrds)
		{
			if (id == exchordid)
				return true;
		}

		return false;
	}



private:

	StackerParams _cp;

	std::thread _stackthread;
	std::atomic<bool> _STACK_ACTIVE;

	std::thread _statusthread;
  std::atomic<bool> _STATUS_ACTIVE;

	SpreadHedgeV3 _hdg[8];
	vector<SpreadHedgeV3> _hdgPath;

	msgstats _msgcnt[100];
	string newid;

	ObjectPool<OrderPod> _QuoteOrdPool;
	ObjectPool<SpreadHedgeV3> _HdgPool;

	vector<uint64_t> _SqueezedOrds;
	vector<uint64_t> _CxlOrds;

	int _alloCount;
	const int _alloMAX = 10000;

	bool bCancelAllSent;

	string _UUID;
	string _PREFIX;


	long _QUOTEID;
	long _LASTQUOTEID;
	int _MAXWORK;

	bool _CIRCUITBREAKER;
	bool _QUOTE_TIMER_OK;

	bool _SHOW_QUOTES;

};

} /* namespace KTN */

#endif /* Multi_HPP_ */
