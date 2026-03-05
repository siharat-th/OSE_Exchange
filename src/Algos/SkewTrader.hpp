    /*
    * SkewTrader.hpp
    *
    *  Created on: Mar 20, 2024
    *      Author: sgaer
    */

    #ifndef SKEWTRADER_HPP_
    #define SKEWTRADER_HPP_

    #pragma once
    #include <Algos/AlgoBaseV3.hpp>
    #include <AlgoCalculators/IntervalTimer.hpp>
    #include <AlgoCalculators/TimerCallback.hpp>

    #include <AlgoParams/Position.hpp>
    #include <AlgoParams/SkewTraderParams.hpp>

    #include <KT01/Core/ObjectPool.hpp>

    using namespace KTN::Core;
    using namespace KTN::ALGO;
    using namespace KTN::notify;
    using namespace KTN;

    namespace KTN {

    /**
    * SkewTrader class is responsible for implementing the order book SkewTrader algorithm.
    * It inherits from AlgoBaseV3 and TimerCallback classes.
    */
    class SkewTrader: public AlgoBaseV3, public TimerCallback
    {
    public:
    /**
    * Constructor for the SkewTrader class.
    * @param p AlgoInitializer object for initializing the algorithm.
    */
    SkewTrader(AlgoInitializer p);

    /**
    * Destructor for the SkewTrader class.
    */
    virtual ~SkewTrader();

    /**
    * Callback function for handling JSON data.
    * @param json The JSON data received.
    */
    void onJson(std::string json) override;

    /**
    * Callback function for handling launch event.
    * @param json The launch event data.
    */
    void onLaunch(std::string json) override;

    /**
    * Callback function for calculating the algorithm.
    */
    void onCalculate() override;

    /**
    * Callback function for handling market data.
    * @param index The index of the market data.
    */
    void onMktData(int index) override;

    /**
    * Callback function for an order execution
    * @param ord The order to be executed.
    * @param hedgeSent Flag indicating if hedge order is sent.
    */
    void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) override;

    /**
    * Callback function for handling hedge order sent event.
    * @param clordid The client order ID.
    * @param reqid The request ID.
    * @param secid The security ID.
    * @param side The order side.
    * @param qty The order quantity.
    * @param price The order price.
    * @param stratid The strategy ID.
    */
    void onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
						uint32_t qty, int32_t price, uint16_t stratid) override;

    /**
    * Callback function for handling order sent event.
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
    * Function to enable or disable the algorithm.
    * @param bEnable Flag indicating if the algorithm should be enabled or disabled.
    */
    void Enable(bool bEnable) override;

    /**
    * Function to reset the position of the algorithm.
    */
    void ResetPosition() override;

    /**
    * Function to print the algorithm details.
    */
    void Print() override;

    /**
    * Function to toggle a command.
    * @param id The ID of the command to toggle.
    */
    void ToggleCommand(int id) override;

    /**
    * Function to restart the algorithm.
    */
    void Restart() override;

    /**
    * Callback function for handling timer state change event.
    * @param state The new state of the timer.
    */
    void onTimerStateChange(TimerState::Enum state);

    /**
    * Callback function for handling timer interval event.
    * @param interval The new interval of the timer.
    */
    void onTimerInterval(TimerInterval::Enum interval);

    /**
    * Function to get the symbols used by the algorithm.
    * @return A vector of strings representing the symbols.
    */
    vector<string> Symbols();

    private:
    /**
    * Function to check if the algorithm is ready to execute.
    * @return True if the algorithm is ready, false otherwise.
    */
    bool GoodToGo();

    /**
    * Callback function for handling received order.
    * @param incoming The incoming order.
    * @param index The index of the order.
    */
    void OrderReceived(KTN::Order& incoming, int index) override;

    /**
    * Callback function for updating minimum order.
    * @param ord The order to be updated.
    */
    void OrderMinUpdate(KTN::OrderPod& ord) override;

    /**
    * Function to load spread leg definitions.
    * @param json The JSON data containing the leg definitions.
    */
    void LoadSpreadLegDefs(string json);

    /**
    * Function to prepare a quoting order.
    * @param ord The quoting order to be prepared.
    * @param inst The instrument for the quoting order.
    * @param side The side of the quoting order.
    * @param price The price of the quoting order.
    * @param qty The quantity of the quoting order.
    * @return True if the quoting order is prepared successfully, false otherwise.
    */
    bool PrepareQuotingOrder(KTN::OrderPod& ord, const instrument& inst,
    KOrderSide::Enum side, int32_t price, int qty);

    /**
    * Function to modify an order.
    * @param ord The order to be modified.
    * @param leg The instrument for the order.
    * @param price The new price of the order.
    */
    void ModifyOrder(KTN::OrderPod& ord, instrument& leg, int price);

    /**
    * Function to move all orders to a specific price.
    * @param leg The instrument for the orders.
    * @param side The side of the orders.
    * @param price The new price for the orders.
    */
    void MoveAllToPrice(instrument& leg, KOrderSide::Enum& side, int price);

    /**
    * Function to cancel all orders.
    */
    void CancelAllOrders();

    /**
    * Function to update position on order fill.
    * @param ord The order that was filled.
    */
    void UpdatePositionOnFill(const KTN::Order& ord);

    /**
    * Function to report the status of the algorithm.
    */
    void ReportStatus();

    /**
    * Function to run the status thread.
    */
    void StatusThread();

    /**
    * Function to check for alerts.
    */
    void AlertCheck();

    /**
    * Function to handle working orders.
    * @param ord The working orders.
    * @param side The side of the orders.
    * @param leg The instrument for the orders.
    */
    void Working(vector<KTN::OrderPod>& ord, KOrderSide::Enum& side, const instrument& leg);

    /**
    * Function to send orders on timer event.
    */
    void SendOnTimer();

    /**
    * Function to send orders on expired event.
    */
    void SendOnExpired();

    /**
    * Function to send orders on skew event.
    * @param side The side of the orders.
    * @param qty The quantity of the orders.
    * @param price The price of the orders.
    */
    void SendOnSkew(KOrderSide::Enum side, int qty, int price);

    /**
    * Function to check the skew.
    */
    void CheckSkew();

    /**
    * Function to calculate the skew.
    * @param leg The instrument for calculating the skew.
    * @return The calculated skew value.
    */
    double CalcSkew(const instrument& leg);

    /**
    * Function to check hedge stops.
    */
    void CheckHedgeStops();

    /**
    * Function to calculate the profit and loss.
    */
    void CalculatePL();

    /**
    * Function to prepare a new order.
    * @param ord The new order to be prepared.
    * @param inst The instrument for the new order.
    * @param side The side of the new order.
    * @param price The price of the new order.
    * @param qty The quantity of the new order.
    */
    void PrepareNewOrder(KTN::OrderPod& ord, const instrument& inst,
    KOrderSide::Enum side, int price, int qty);

    /**
    * Function to prepare a modified order.
    * @param ord The modified order to be prepared.
    * @param inst The instrument for the modified order.
    * @param side The side of the modified order.
    * @param price The price of the modified order.
    * @param qty The quantity of the modified order.
    */
    void PrepareModOrder(KTN::OrderPod& ord, const instrument& inst,
    KOrderSide::Enum side, int price, int qty);

    /**
    * Function to check if an order is squeezed.
    * @param reqid The request ID of the order.
    * @return True if the order is squeezed, false otherwise.
    */
    bool Squeezed(uint64_t reqid);

    /**
    * Function to print a quick order.
    * @param ord The order to be printed.
    * @param inst The instrument for the order.
    */
    void QuickOrdPrint(const KTN::Order& ord, const instrument& inst);

    /**
    * Function to update the state of a leg.
    * @param reqid The request ID of the order.
    * @param state The new state of the leg.
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
    * Function to clear the sent IDs of all legs.
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
    SkewTraderParams _cp;
    double _SKEW;
    bool _TIMER_ELAPSED;
    std::thread _statusthread;
    std::atomic<bool> _STATUS_ACTIVE;

    IntervalTimer _timer;
    PosNet _pos[8];

    msgstats _msgcnt[100];
    string newid;

    ObjectPool<OrderPod> _QuoteOrdPool;
    ObjectPool<SpreadHedgeV3> _HdgPool;

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

    #endif /* SKEWTRADER_HPP_ */
