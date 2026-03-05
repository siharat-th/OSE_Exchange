/*
 * StackerTimer.hpp
 *
 *  Created on: Jan 22, 2024
 *      Author: sgaer
 */

#ifndef STACKERTIMER_HPP_
#define STACKERTIMER_HPP_

#pragma once
#include <Algos/AlgoBaseV3.hpp>
#include <AlgoCalculators/SimpleTimer.hpp>
#include <AlgoCalculators/TimerCallback.hpp>
#include <AlgoParams/StackerTimerParams.hpp>

#include <KT01/Core/ObjectPool.hpp>

using namespace KTN::Core;
using namespace KTN::ALGO;
using namespace KTN::notify;
using namespace KTN;


namespace KTN {

/**
 * @brief The StackerTimer class is a derived class of AlgoBaseV3 and TimerCallback.
 * It implements various methods for managing a stacker timer algorithm.
 */
class StackerTimer: public AlgoBaseV3, public TimerCallback
{
public:
	/**
  * @brief Constructs a StackerTimer object.
  * @param p The AlgoInitializer object.
  */
	StackerTimer(AlgoInitializer p);

	/**
  * @brief Destroys the StackerTimer object.
  */
	virtual ~StackerTimer();

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
  * @brief Performs the calculation.
  */
	void onCalculate()override;

	/**
  * @brief Handles the trade event.
  * @param index The index of the trade.
  * @param price The price of the trade.
  * @param size The size of the trade.
  * @param aggressor The aggressor of the trade.
  */
	void onTrade(int index, double price, double size, int aggressor) override;

	/**
  * @brief Executes the minimum order execution.
  * @param ord The OrderPod object.
  * @param hedgeSent A boolean indicating if the hedge is sent.
  */
	void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) override;

	/**
  * @brief Enables or disables the algorithm.
  * @param bEnable A boolean indicating if the algorithm should be enabled.
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
  * @param id The ID of the command to toggle.
  */
	void ToggleCommand(int id) override;

	/**
  * @brief Restarts the algorithm.
  */
	void Restart() override;

	/**
  * @brief Handles the timer state change event.
  * @param state The TimerState enumeration value.
  */
	void onTimerStateChange(TimerState::Enum state);

	/**
  * @brief Handles the timer interval event.
  * @param interval The TimerInterval enumeration value.
  */
	void onTimerInterval(TimerInterval::Enum interval);

	/**
  * @brief Gets the list of symbols.
  * @return A vector of strings representing the symbols.
  */
	vector<string> Symbols();

private:
	/**
  * @brief Checks if it is good to go.
  * @return A boolean indicating if it is good to go.
  */
	bool GoodToGo();

	/**
  * @brief Checks if it is time to puke.
  */
	void CheckPuke();

	/**
  * @brief Pukes at a specific price.
  * @param secid The security ID.
  * @param price The price to puke at.
  * @param trig The trigger type.
  */
	void PukeAtPrice(int32_t secid, int price, KOrderAlgoTrigger::Enum trig);

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
  * @brief Prepares a quoting order.
  * @param ord The OrderPod object.
  * @param inst The instrument object.
  * @param side The order side.
  * @param price The order price.
  * @param qty The order quantity.
  * @return A boolean indicating if the order preparation was successful.
  */
	bool PrepareQuotingOrder(KTN::OrderPod &ord, const instrument& inst,
			KOrderSide::Enum side, int32_t price, int qty);

	/**
  * @brief Cancels all orders.
  */
	void CancelAllOrders();

	/**
  * @brief Cancels a specific order.
  * @param ord The OrderPod object.
  */
	void CancelOrder(const KTN::OrderPod& ord);

	/**
  * @brief Updates the position on fill.
  * @param ord The Order object.
  */
	void UpdatePositionOnFill(const KTN::Order& ord);

	/**
  * @brief Reports the status.
  */
	void ReportStatus();

	/**
  * @brief Checks for alerts.
  */
	void AlertCheck();

	/**
  * @brief Performs the working process.
  * @param ord The vector of OrderPod objects.
  * @param side The order side.
  * @param leg The instrument object.
  */
	void Working(vector<KTN::OrderPod>& ord, KOrderSide::Enum& side, const instrument& leg);

	/**
  * @brief Sends a stack order.
  * @param legindex The index of the leg.
  * @param price The order price.
  */
	void SendStackOrder(int legindex, int price);

	/**
  * @brief Sends an order on expiration.
  */
	void SendOnExpired();

	/**
  * @brief Checks for squeeze conditions.
  */
	void CheckSqueeze();

	/**
  * @brief Prepares a new order.
  * @param ord The OrderPod object.
  * @param inst The instrument object.
  * @param side The order side.
  * @param price The order price.
  * @param qty The order quantity.
  */
	void PrepareNewOrder(KTN::OrderPod &ord, const instrument& inst,
			KOrderSide::Enum side, int price, int qty);

	/**
  * @brief Prepares a modified order.
  * @param ord The OrderPod object.
  * @param inst The instrument object.
  * @param side The order side.
  * @param price The order price.
  * @param qty The order quantity.
  */
	void PrepareModOrder(KTN::OrderPod &ord, const instrument& inst,
			KOrderSide::Enum side, int price, int qty);

	/**
  * @brief Checks if an order is squeezed.
  * @param reqid The request ID of the order.
  * @return A boolean indicating if the order is squeezed.
  */
	bool Squeezed(uint64_t reqid);

	/**
  * @brief Prints the order details.
  * @param ord The Order object.
  * @param inst The instrument object.
  */
	void QuickOrdPrint(const KTN::Order& ord, const instrument& inst);

	/**
  * @brief Updates the state of a leg.
  * @param reqid The request ID of the order.
  * @param state The state of the order.
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
  * @brief Clears the sent IDs of the legs.
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

private:
	StackerTimerParams _cp;
	SimpleTimer _timer;

	msgstats _msgcnt[100];
	string newid;

	ObjectPool<OrderPod> _QuoteOrdPool;

	vector<uint64_t> _SqueezedOrds;

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

#endif /* STACKERTIMER_HPP_ */
