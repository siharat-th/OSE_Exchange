/*
 * MutliSpreader2.hpp
 *
 *  Created on: Apr 14, 2023
 *  Updated to V3 on: Oct 22, 2023
 *      Author: sgaer
 */

#ifndef MULTI_SPREADERV3_HPP_
#define MULTI_SPREADERV3_HPP_

#pragma once
#include <Algos/AlgoBaseV3.hpp>

#include <AlgoEngines/SpreadPricer.hpp>


#include <AlgoParams/MS2Params.hpp>
#include <AlgoSupport/AlgoHdgEncoder.hpp>

using namespace KTN::ALGO::PRICING;
using namespace KTN::ALGO;
using namespace KTN;

namespace KTN
{
    /**
     * @brief The MultiSpreaderV3 class is a derived class of AlgoBaseV3 and implements an automated multi-leg spread trading algorithm.
     *
     */
    class MultiSpreaderV3 : public AlgoBaseV3
    {
    public:
        /**
         * @brief Constructs a new instance of the MultiSpreaderV3 class.
         *
         * @param p The AlgoInitializer object used to initialize the algorithm.
         */
        MultiSpreaderV3(AlgoInitializer p);

        /**
         * @brief Destroys the instance of the MultiSpreaderV3 class.
         *
         */
        virtual ~MultiSpreaderV3();

        /**
         * @brief Handles the JSON input for the algorithm.
         *
         * @param json The JSON input string.
         */
        void onJson(std::string json) override;

        /**
         * @brief Handles the launch of the algorithm.
         *
         * @param json The JSON input string.
         */
        void onLaunch(std::string json) override;

        /**
         * @brief Performs the calculation logic of the algorithm.
         *
         */
        void onCalculate() override;

        /**
         * @brief Handles Order Execution events from the exchange handler
         *
         * @param ord The OrderPod object representing the order.
         * @param hedgeSent A boolean indicating whether a hedge order has been sent.
         */
        void minOrderExecution(KTN::OrderPod &ord, bool hedgeSent) override;

        /**
         * @brief Handles the event when a hedge order is sent.
         *
         * @param clordid The client order ID.
         * @param reqid The request ID.
         * @param secid The security ID.
         * @param side The order side.
         * @param qty The order quantity.
         * @param price The order price.
         * @param stratid The strategy ID.
         */
        void onHedgeOrderSent(char *clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
                              uint32_t qty, int32_t price, uint16_t stratid) override;

        /**
         * @brief Handles the event when an order is sent.
         *
         * @param clordid The client order ID.
         * @param reqid The request ID.
         * @param secid The security ID.
         * @param side The order side.
         * @param qty The order quantity.
         * @param price The order price.
         * @param trig The order trigger.
         */
        void onOrderSent(char *clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
                         uint32_t qty, int32_t price, KOrderAlgoTrigger::Enum trig) override;

        /**
         * @brief Enables or disables the algorithm.
         *
         * @param bEnable A boolean indicating whether to enable or disable the algorithm.
         */
        void Enable(bool bEnable) override;

        /**
         * @brief Resets the position of the algorithm.
         *
         */
        void ResetPosition() override;

        /**
         * @brief Prints the algorithm's information.
         *
         */
        void Print() override;

        /**
         * @brief Toggles a command in the algorithm.
         *
         * @param id The ID of the command to toggle.
         */
        void ToggleCommand(int id) override;

        /**
         * @brief Restarts the algorithm.
         *
         */
        void Restart() override;

        /**
         * @brief Gets the symbols used in the algorithm.
         *
         * @return A vector of strings representing the symbols.
         */
        vector<string> Symbols();

    private:
        /**
         * @brief Performs a set of sanity and data integrity checks. Call this prior to any automated trading/quoting to ensure the correct state of market data and orders in flight.
         *
         * @return A boolean indicating whether the algorithm is ready to start trading.
         */
        bool GoodToGo();

        /**
         * @brief Checks if the algorithm has received market data.
         *
         * @return A boolean indicating whether the algorithm has received market data.
         */
        bool GotMktData();

        /**
         * @brief Checks the position and quantity of the algorithm.
         *
         * @return A boolean indicating whether the position and quantity are valid.
         */
        bool CheckPositionAndQty();

        /**
         * @brief Handles the event when an order is received.
         *
         * @param incoming The Order object representing the incoming order.
         * @param index The index of the order.
         */
        void OrderReceived(KTN::Order &incoming, int index) override;

        /**
         * @brief Updates the minimum order information.
         *
         * @param ord The OrderPod object representing the order.
         */
        void OrderMinUpdate(KTN::OrderPod &ord) override;

        /**
         * @brief Loads the spread leg definitions from JSON.
         * @param json The JSON string representing the spread leg definitions.
         */
        void LoadSpreadLegDefs(string json);

        /**
         * @brief Prepares a quoting order.
         *
         * @param ord The OrderPod object representing the order.
         * @param inst The instrument object representing the instrument.
         * @param side The order side.
         * @param price The order price.
         * @param qty The order quantity.
         * @return A boolean indicating whether the quoting order was prepared successfully.
         */
        bool PrepareQuotingOrder(KTN::OrderPod &ord, const instrument &inst,
                                 KOrderSide::Enum side, int32_t price, int qty);

        /**
         * @brief Cancels all orders.
         *
         */
        void CancelAllOrders();

        /**
         * @brief Updates the position on order fill.
         *
         * @param ord The Order object representing the filled order.
         */
        void UpdatePositionOnFill(const KTN::Order &ord);

        /**
         * @brief Checks the hedge stops.
         *
         */
        void CheckHedgeStops();

        /**
         * @brief Checks the squeeze condition.
         *
         */
        void CheckSqueeze();

        /**
         * @brief Checks the squeeze condition for a specific index.
         *
         * @param index The index of the leg.
         */
        void CheckSqueeze(int index);

        /**
         * @brief Checks if an order has been squeezed.
         *
         * @param reqid The request ID of the order.
         * @return A boolean indicating whether the order has been squeezed.
         */
        bool Squeezed(uint64_t reqid);

        /**
         * @brief Reports the status of the algorithm.
         *
         */
        void ReportStatus();

        /**
         * @brief Checks for alerts.
         *
         */
        void AlertCheck();

        /**
         * @brief Generates quotes for a leg.
         *
         * @param leg The instrument object representing the leg.
         * @param bidChange A boolean indicating whether the bid has changed.
         * @param askChange A boolean indicating whether the ask has changed.
         */
        void QuoteGenerator(instrument &leg, bool bidChange, bool askChange);

        /**
         * @brief Sends a quote for a leg.
         *
         * @param leg The instrument object representing the leg.
         * @param isbid A boolean indicating whether the quote is for the bid.
         * @param edge The edge value.
         * @param ticks The number of ticks.
         * @return A boolean indicating whether the quote was sent successfully.
         */
        bool SendQuote(instrument &leg, bool isbid, int edge, int ticks);

        /**
         * @brief Prints a quick order.
         *
         * @param ord The Order object representing the order.
         * @param inst The instrument object representing the instrument.
         */
        void QuickOrdPrint(const KTN::Order &ord, const instrument &inst);

        /**
         * @brief Updates the state of a leg based on the request ID and order state.
         *
         * @param reqid The request ID of the order.
         * @param state The state of the order.
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
         * @brief Clears the sent order IDs and states of all legs.
         *
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
        MS2Params _cp;

        SpreadPricer _pricer;
        LittleImpSide _imp[8];
        //QuoterV3 _quoter[8];

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

        // bool _CIRCUITBREAKER;
        bool _QUOTE_TIMER_OK;

        bool _SHOW_QUOTES;

        inline int roundUp(int numToRound, int multiple)
        {
            if (multiple == 0)
                return numToRound;

            int remainder = abs(numToRound) % multiple;
            if (remainder == 0)
                return numToRound;

            if (numToRound < 0)
                return -(abs(numToRound) - remainder);
            else
                return numToRound + multiple - remainder;
        }

        inline float priceRoundConvert(float px, int tickprecision, float mintick, bool roundUp = false)
        {
            if (tickprecision == 0)
                return px;

            float origpx = px;

            float pow_10 = pow(10.0f, (float)tickprecision);
            float pow_minus10 = pow(10.0f, (float)(-1 * tickprecision));

            int pxinflate = origpx * pow_10;
            int tickinflate = mintick * pow_10;

            if (roundUp)
            {
                int temppx = pxinflate;

                int remainder = abs(pxinflate) % tickinflate;
                if (remainder == 0)
                    px = origpx;

                if (pxinflate < 0)
                    temppx = -1 * (abs(pxinflate) - remainder);
                else
                    temppx = pxinflate + tickinflate - remainder;

                float newpx = temppx * pow_minus10;
                px = newpx;
            }
            else
            {
                int temppx = (pxinflate / tickinflate) * tickinflate;

                float newpx = temppx * pow_minus10;

                px = newpx;
            }

            return px;
        }
    };

} /* namespace KTN */

#endif /* MULTI_SPREADERV3_HPP_ */
