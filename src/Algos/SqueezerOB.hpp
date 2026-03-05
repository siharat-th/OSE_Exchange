/*
 * SqueezerOB.hpp
 *
 *  Created on: Apr 24, 2021
 *      Author: sgaer
 */

#ifndef SQZOB2_HPP_
#define SQZOB2_HPP_

#pragma once
#include <Algos/AlgoBaseV3.hpp>

#include <AlgoParams/SqueezerParams.hpp>
#include <AlgoParams/RiskBase.hpp>

#include <AlgoEngines/engine_structs.hpp>

using namespace KTN::ALGO;
using namespace KTN;

namespace chrono = std::chrono;

namespace KTN {

/**
 * @brief The SqueezerOB class represents an implementation of the SqueezerOB algorithm.
 * THe SqueezerOB algorithm is akin to an OCO order that rests on the order book and watches a "Squeez" price.
 * Once the quantity at the price is lower than the threshold, the order is then modified and moves to the target price.
 * This is the same functionality as "leaning" on an limit order at a price, and acting when the price is "squeezed" by decreases in quantity or the ratio of bid/ask quantity.
 */
class SqueezerOB: public AlgoBaseV3 {
public:
    /**
     * @brief Constructs a new instance of the SqueezerOB algorithm.
     * @param p The AlgoInitializer object used to initialize the algorithm.
     */
    SqueezerOB(AlgoInitializer p);

    /**
     * @brief Destroys the SqueezerOB algorithm instance.
     */
    virtual ~SqueezerOB();

    //algo base impl
    virtual void onJson(std::string json);
    virtual void onLaunch(std::string json);
    virtual void onCalculate();

    /**
     * @brief Handles the market data event for the specified index.
     * @param index The index of the market data event.
     */
    void onMktData(int index) override;

    /**
     * @brief Handles the trade event for the specified index.
     * @param index The index of the trade event.
     * @param price The price of the trade.
     * @param size The size of the trade.
     * @param aggressor The aggressor flag indicating the aggressor side of the trade.
     */
    void onTrade(int index, double price, double size, int aggressor) override;

    /**
     * @brief Executes the minimum order execution logic for the specified order and hedge flag.
     * @param ord The OrderPod object representing the order.
     * @param hedgeSent The flag indicating whether the order is a hedge order.
     */
    void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) override;

    /**
     * @brief Enables or disables the algorithm.
     * @param bEnable The flag indicating whether to enable or disable the algorithm.
     */
    virtual void Enable(bool bEnable);

    /**
     * @brief Resets the position of the algorithm.
     */
    void ResetPosition() override;

    /**
     * @brief Prints the algorithm's information.
     */
    void Print() override;

    /**
     * @brief Toggles the specified command.
     * @param id The ID of the command to toggle.
     */
    void ToggleCommand(int id) override;

    /**
     * @brief Restarts the algorithm.
     */
    void Restart() override;

private:
    /**
     * @brief Checks if the algorithm is good to go for the specified index.
     * @param index The index to check.
     * @return True if the algorithm is good to go, false otherwise.
     */
    bool GoodToGo(int index);

    /**
     * @brief Checks if the algorithm has received market data for the specified index.
     * @param index The index to check.
     * @return True if the algorithm has received market data, false otherwise.
     */
    bool GotMktData(int index);

    /**
     * @brief Reports the status of the algorithm.
     */
    void ReportStatus();

    /**
     * @brief Adds a leg to the algorithm.
     * @param ord The Order object representing the leg.
     * @param ln The instrument object representing the leg.
     */
    void AddLeg(const KTN::Order& ord, instrument& ln);

    /**
     * @brief Handles the received order event for the specified order and index.
     * @param incoming The Order object representing the received order.
     * @param index The index of the received order.
     */
    void OrderReceived(KTN::Order& incoming, int index) override;

    /**
     * @brief Handles the minimum order update event for the specified order.
     * @param ord The OrderPod object representing the order.
     */
    void OrderMinUpdate(KTN::OrderPod& ord) override;

    /**
     * @brief Maps the squeeze order parameters for the specified order, action, and squeeze parameters.
     * @param ord The OrderPod object representing the order.
     * @param action The action to perform.
     * @param sq The SqzOrdParam object representing the squeeze parameters.
     */
    void MapSqz(const KTN::OrderPod& ord, int action, const SqzOrdParam& sq);

    /**
     * @brief Checks if the squeeze condition is met for the specified index and squeeze parameters.
     * @param index The index to check.
     * @param sq The SqzOrdParam object representing the squeeze parameters.
     * @return True if the squeeze condition is met, false otherwise.
     */
    bool CheckSqz(int index, SqzOrdParam& sq);

    /**
     * @brief Sends the squeeze order for the specified index, request ID, squeeze price, and trigger type.
     * @param index The index to send the squeeze order.
     * @param reqid The request ID of the squeeze order.
     * @param sqz_price The squeeze price.
     * @param trig The trigger type of the squeeze order.
     */
    void SqueezeSender(int index, uint64_t reqid, int sqz_price, KOrderAlgoTrigger::Enum trig);

private:
    SqueezerParams _cp;
   
    bool _CIRCUITBREAKER;

    std::unordered_map<string, int> _SymbolIndexMap;
    std::unordered_map<string, string> _hdgOrdMap;
    std::unordered_map<uint64_t, SqzOrdParam> _sqzMap;
    std::unordered_map<string, uint64_t> _reqMap;

};

} /* namespace KTN */

#endif /* SQZOB2_HPP_ */
