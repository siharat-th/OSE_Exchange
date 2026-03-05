/*
 * Twap.hpp
 *
 *  Created on: Dec 27, 2023
 *      Author: sgaer
 */

#ifndef TWAPV3_HPP_
#define TWAPV3_HPP_

#pragma once
#include <Algos/AlgoBaseV3.hpp>
#include <AlgoCalculators/IntervalTimer.hpp>
#include <AlgoCalculators/TimerCallback.hpp>
#include <AlgoParams/TimedAvgParams.hpp>

    #include <KT01/Core/ObjectPool.hpp>

using namespace KTN::Core;
using namespace KTN::ALGO;
using namespace KTN::notify;
using namespace KTN;

namespace KTN
{

    /**
     * @brief The Twap class represents a TWAP (Time-Weighted Average Price) algorithm.
     */
    class Twap : public AlgoBaseV3, public TimerCallback
    {
    public:
        /**
         * @brief Constructs a new Twap object.
         * @param p The AlgoInitializer object.
         */
        Twap(AlgoInitializer p);

        /**
         * @brief Destroys the Twap object.
         */
        virtual ~Twap();

        /**
         * @brief Handles the JSON input.
         * @param json The JSON string.
         */
        void onJson(std::string json) override;

        /**
         * @brief Handles the launch event.
         * @param json The JSON string.
         */
        void onLaunch(std::string json) override;

        /**
         * @brief Performs the calculation.
         */
        void onCalculate() override;

        /**
         * @brief Executes the minimum order execution.
         * @param ord The OrderPod object.
         * @param hedgeSent A boolean indicating if the hedge is sent.
         */
        void minOrderExecution(KTN::OrderPod &ord, bool hedgeSent) override;

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
         * @param state The TimerState.
         */
        void onTimerStateChange(TimerState::Enum state);

        /**
         * @brief Handles the timer interval event.
         * @param interval The TimerInterval.
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
         * @brief Handles the received order.
         * @param incoming The Order object.
         * @param index The index of the order.
         */
        void OrderReceived(KTN::Order &incoming, int index) override;

        /**
         * @brief Updates the minimum order.
         * @param ord The OrderPod object.
         */
        void OrderMinUpdate(KTN::OrderPod &ord) override;

        /**
         * @brief Loads the spread leg definitions.
         * @param json The JSON string.
         */
        void LoadSpreadLegDefs(string json);

        /**
         * @brief Prepares the quoting order.
         * @param ord The OrderPod object.
         * @param inst The instrument object.
         * @param side The order side.
         * @param price The order price.
         * @param qty The order quantity.
         * @return A boolean indicating if the quoting order is prepared successfully.
         */
        bool PrepareQuotingOrder(KTN::OrderPod &ord, const instrument &inst,
                                 KOrderSide::Enum side, int32_t price, int qty);

        /**
         * @brief Modifies the order.
         * @param ord The OrderPod object.
         * @param leg The instrument object.
         * @param price The new order price.
         */
        void ModifyOrder(KTN::OrderPod &ord, instrument &leg, int price);

        /**
         * @brief Moves all orders to a specific price.
         * @param leg The instrument object.
         * @param side The order side.
         * @param price The new order price.
         */
        void MoveAllToPrice(instrument &leg, KOrderSide::Enum &side, int price);

        /**
         * @brief Cancels all orders.
         */
        void CancelAllOrders();

        /**
         * @brief Updates the position on fill.
         * @param ord The Order object.
         */
        void UpdatePositionOnFill(const KTN::Order &ord);

        /**
         * @brief Reports the status.
         */
        void ReportStatus();

        /**
         * @brief Checks for alerts.
         */
        void AlertCheck();

        /**
         * @brief Handles the working state.
         * @param ord The vector of OrderPod objects.
         * @param side The order side.
         * @param leg The instrument object.
         */
        void Working(vector<KTN::OrderPod> &ord, KOrderSide::Enum &side, const instrument &leg);

        /**
         * @brief Sends orders on timer.
         */
        void SendOnTimer();

        /**
         * @brief Sends orders on expiration.
         */
        void SendOnExpired();

        /**
         * @brief Checks for squeeze.
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
        void PrepareNewOrder(KTN::OrderPod &ord, const instrument &inst,
                             KOrderSide::Enum side, int price, int qty);

        /**
         * @brief Prepares a modified order.
         * @param ord The OrderPod object.
         * @param inst The instrument object.
         * @param side The order side.
         * @param price The order price.
         * @param qty The order quantity.
         */
        void PrepareModOrder(KTN::OrderPod &ord, const instrument &inst,
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
        void QuickOrdPrint(const KTN::Order &ord, const instrument &inst);

        /**
         * @brief Updates the state of the leg.
         * @param reqid The request ID of the order.
         * @param state The new state of the leg.
         */
        inline void UpdateLegState(uint64_t reqid, KOrderState::Enum state)
        {
            for (int i = 0; i < _cp.LegCount; i++)
            {
                instrument &leg = _cp.Legs[i];
                if (leg.lastBuyId == reqid)
                {
                    leg.buyState = state;
                    if (state == KOrderState::COMPLETE)
                        leg.lastBuyId = 0;
                    return;
                }
                if (leg.lastSellId == reqid)
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
                instrument &leg = _cp.Legs[i];
                leg.lastBuyId = 0;
                leg.lastSellId = 0;
                leg.buyState = KOrderState::UNKNOWN;
                leg.sellState = KOrderState::UNKNOWN;
            }
        }

    private:
        TimedAvgParams _cp;
        IntervalTimer _timer;
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

#endif /* TWAPV3_HPP_ */
