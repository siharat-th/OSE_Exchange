    /*
    * Skewer.hpp
    *
    *  Created on: Mar 15, 2024
    *      Author: sgaer
    */

    #ifndef SKEWER_HPP_
    #define SKEWER_HPP_

    #pragma once
    #include <Algos/AlgoBaseV3.hpp>
    #include <AlgoCalculators/IntervalTimer.hpp>
    #include <AlgoCalculators/TimerCallback.hpp>
    #include <AlgoParams/SkewerParams.hpp>

    #include <KT01/Core/ObjectPool.hpp>

    using namespace KTN::Core;
    using namespace KTN::ALGO;
    using namespace KTN::notify;
    using namespace KTN;

    namespace KTN {

    /**
    * @brief The Skewer class represents an implementation of an order book skew trading algorithm.
    */
    class Skewer: public AlgoBaseV3, public TimerCallback
    {
    public:
    /**
    * @brief Constructs a Skewer object.
    * @param p The AlgoInitializer object.
    */
    Skewer(AlgoInitializer p);

    /**
    * @brief Destroys the Skewer object.
    */
    virtual ~Skewer();

    /**
    * @brief Handles the JSON input for the Skewer algorithm.
    * @param json The JSON input string.
    */
    void onJson(std::string json)override;

    /**
    * @brief Handles the launch of the Skewer algorithm.
    * @param json The JSON input string.
    */
    void onLaunch(std::string json)override;

    /**
    * @brief Performs the calculation for the Skewer algorithm.
    */
    void onCalculate()override;

    /**
    * @brief Handles the market data for the Skewer algorithm.
    * @param index The index of the market data.
    */
    void onMktData(int index) override;

    /**
    * @brief Executes the minimum order for the Skewer algorithm.
    * @param ord The OrderPod object.
    * @param hedgeSent A boolean indicating if the hedge order is sent.
    */
    void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) override;

    /**
    * @brief Enables or disables the Skewer algorithm.
    * @param bEnable A boolean indicating if the algorithm should be enabled.
    */
    void Enable(bool bEnable) override;

    /**
    * @brief Resets the position for the Skewer algorithm.
    */
    void ResetPosition() override;

    /**
    * @brief Prints the status of the Skewer algorithm.
    */
    void Print() override;

    /**
    * @brief Toggles a command for the Skewer algorithm.
    * @param id The ID of the command to toggle.
    */
    void ToggleCommand(int id) override;

    /**
    * @brief Restarts the Skewer algorithm.
    */
    void Restart() override;

    /**
    * @brief Handles the state change of the timer for the Skewer algorithm.
    * @param state The TimerState enumeration value.
    */
    void onTimerStateChange(TimerState::Enum state);

    /**
    * @brief Handles the interval change of the timer for the Skewer algorithm.
    * @param interval The TimerInterval enumeration value.
    */
    void onTimerInterval(TimerInterval::Enum interval);

    /**
    * @brief Retrieves the symbols used in the Skewer algorithm.
    * @return A vector of strings representing the symbols.
    */
    vector<string> Symbols();

    private:
    /**
    * @brief Checks if the Skewer algorithm is ready to execute.
    * @return A boolean indicating if the algorithm is ready.
    */
    bool GoodToGo();

    /**
    * @brief Checks if the Skewer algorithm has received market data.
    * @return A boolean indicating if market data has been received.
    */
    bool GotMktData();

    /**
    * @brief Handles the received order for the Skewer algorithm.
    * @param incoming The Order object.
    * @param index The index of the order.
    */
    void OrderReceived(KTN::Order& incoming, int index) override;

    /**
    * @brief Updates the minimum order for the Skewer algorithm.
    * @param ord The OrderPod object.
    */
    void OrderMinUpdate(KTN::OrderPod& ord) override;

    /**
    * @brief Loads the spread leg definitions for the Skewer algorithm.
    * @param json The JSON input string.
    */
    void LoadSpreadLegDefs(string json);

    /**
    * @brief Prepares the quoting order for the Skewer algorithm.
    * @param ord The OrderPod object.
    * @param inst The instrument object.
    * @param side The order side.
    * @param price The order price.
    * @param qty The order quantity.
    * @return A boolean indicating if the quoting order is prepared successfully.
    */
    bool PrepareQuotingOrder(KTN::OrderPod &ord, const instrument& inst,
    KOrderSide::Enum side, int32_t price, int qty);

    /**
    * @brief Modifies the order for the Skewer algorithm.
    * @param ord The OrderPod object.
    * @param leg The instrument object.
    * @param price The new order price.
    */
    void ModifyOrder(KTN::OrderPod& ord, instrument& leg, int price);

    /**
    * @brief Moves all orders to a specific price for the Skewer algorithm.
    * @param leg The instrument object.
    * @param side The order side.
    * @param price The new order price.
    */
    void MoveAllToPrice(instrument& leg, KOrderSide::Enum& side, int price);

    /**
    * @brief Cancels all orders for the Skewer algorithm.
    */
    void CancelAllOrders() override;

    /**
    * @brief Updates the position on fill for the Skewer algorithm.
    * @param ord The Order object.
    */
    void UpdatePositionOnFill(const KTN::Order& ord);

    /**
    * @brief Reports the status of the Skewer algorithm.
    */
    void ReportStatus();

    /**
    * @brief Starts the status thread for the Skewer algorithm.
    */
    void StatusThread();

    /**
    * @brief Checks for alerts in the Skewer algorithm.
    */
    void AlertCheck();

    /**
    * @brief Checks if the orders are working for the Skewer algorithm.
    * @param ord The vector of OrderPod objects.
    * @param side The order side.
    * @param leg The instrument object.
    * @return A boolean indicating if the orders are working.
    */
    bool Working(vector<KTN::OrderPod>& ord, KOrderSide::Enum side, instrument& leg);

    /**
    * @brief Sends orders on timer for the Skewer algorithm.
    */
    void SendOnTimer();

    /**
    * @brief Sends orders on expiration for the Skewer algorithm.
    */
    void SendOnExpired();

    /**
    * @brief Sends orders on skew for the Skewer algorithm.
    * @param side The order side.
    * @param qty The order quantity.
    * @param price The order price.
    * @param leg The instrument object.
    */
    void SendOnSkew(KOrderSide::Enum side, int qty, int price, instrument& leg);

    /**
    * @brief Checks for squeeze orders and status in the Skewer algorithm.
    * @param index The index of the market data.
    */
    void CheckSqueeze(int index);

    /**
    * @brief Checks order book skew in the Skewer algorithm.
    */
    void CheckSkew();

    /**
    * @brief Calculates the skew for the Skewer algorithm.
    * @param leg The instrument object.
    * @param side The order side.
    * @return The calculated skew value.
    */
    double CalcSkew(const instrument& leg, KOrderSide::Enum side);

    /**
    * @brief Prepares a new order for the Skewer algorithm.
    * @param ord The OrderPod object.
    * @param inst The instrument object.
    * @param side The order side.
    * @param price The order price.
    * @param qty The order quantity.
    */
    void PrepareNewOrder(KTN::OrderPod &ord, const instrument& inst,
    KOrderSide::Enum side, int price, int qty);

    /**
    * @brief Prepares a modified order for the Skewer algorithm.
    * @param ord The OrderPod object.
    * @param inst The instrument object.
    * @param side The order side.
    * @param price The order price.
    * @param qty The order quantity.
    */
    void PrepareModOrder(KTN::OrderPod &ord, const instrument& inst,
    KOrderSide::Enum side, int price, int qty);

    /**
    * @brief Checks if a aqueeze order has been triggered or sent in the Skewer algorithm.
    * @param reqid The request ID of the order.
    * @return A boolean indicating if the order has been squeezed.
    */
    bool Squeezed(uint64_t reqid);

    /**
    * @brief Prints the details of an order for the Skewer algorithm.
    * @param ord The Order object.
    * @param inst The instrument object.
    */
    void QuickOrdPrint(const KTN::Order& ord, const instrument& inst);

    /**
    * @brief Updates the state of a leg in the Skewer algorithm.
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
    * @brief Clears the sent order IDs for all legs in the Skewer algorithm.
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
    SkewerParams _cp;
    double _SKEW;
    bool _TIMER_ELAPSED;
    std::thread _statusthread;
    std::atomic<bool> _STATUS_ACTIVE;

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

    #endif /* SKEWER_HPP_ */
