#pragma once

#include <stdio.h>
#include <unordered_map>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <algorithm>

#include <chrono>
using namespace std::chrono;

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

#include <tbb/concurrent_queue.h>
using namespace tbb;

#include <KT01/Core/Log.hpp>
#include <Orders/Order.hpp>
#include <Orders/OrderFormatter.h>

#include <ZMQ/ZmqSubscriber.h>


#include "RiskLimits.h"

using namespace std;


namespace KTN{

/**
 * @brief The RiskManager class handles risk management operations.
 */
class RiskManager
{
public:

    /**
     * @brief RiskManager constructor.
     */
    RiskManager();

    /**
     * @brief RiskManager destructor.
     */
    ~RiskManager();

    /**
     * @brief Initializes the RiskManager with the provided parameters.
     * @param key The key parameter.
     * @param account The account parameter.
     * @param source The source parameter.
     * @param argoserver The argoserver parameter.
     * @param argosubport The argosubport parameter.
     * @param argopushport The argopushport parameter.
     * @param affinity The affinity parameter.
     */
    void Init(std::string key, std::string account, std::string source,
              std::string argoserver, int argosubport, int argopushport, int affinity);

    /**
     * @brief Processes the limits using the provided XML string.
     * @param szXml The XML string.
     */
    void ProcessLimits(std::string szXml);

    /**
     * @brief Handles the risk limits using the provided JSON string.
     * @param szJson The JSON string.
     */
    void HandleRiskLimitsJSON(std::string szJson);

    /**
     * @brief Retrieves the limits.
     */
    void GetLimits();

    /**
     * @brief Sends the order to the Risk Management System (RMS).
     * @param ord The order to send.
     * @param iStatus The status parameter.
     * @param iReportType The report type parameter.
     * @param bResend The resend flag.
     * @param bUseJsonArray The flag indicating whether to use JSON array.
     */
    void SendToRMS(Order ord, int iStatus, int iReportType, bool bResend, bool bUseJsonArray);

    /**
     * @brief Sends the orders to the Risk Management System (RMS).
     * @param ords The vector of orders to send.
     */
    void SendToRMS(std::vector<Order> ords);

    /**
     * @brief Updates the limits on the given order.
     * @param ord The order to update the limits on.
     */
    void UpdateLimitsOnOrder(Order ord);

    /**
     * @brief Sends the raw JSON string to the Risk Management System (RMS).
     * @param szJson The raw JSON string to send.
     */
    void SendRaw(std::string szJson);

    /**
     * @brief Checks if the provided key is valid.
     * @param key The key to check.
     * @return True if the key is valid, false otherwise.
     */
    bool OK(std::string key);

    /**
     * @brief Retrieves the maximum order quantity for the provided key.
     * @param key The key to retrieve the maximum order quantity for.
     * @return The maximum order quantity.
     */
    int MaxOrdQty(std::string key);

    /**
     * @brief Checks if the order size is valid for the provided key.
     * @param key The key to check the order size for.
     * @param ordqty The order quantity.
     * @param bIsBuy The flag indicating whether the order is a buy order.
     * @param reason The reason for the order size validation result.
     * @param oldqty The old order quantity.
     * @return True if the order size is valid, false otherwise.
     */
    bool OrderSizeOK(std::string key, int ordqty, bool bIsBuy, std::string &reason, int oldqty = 0);

    /**
     * @brief Handles the exception with the provided message.
     * @param e The exception object.
     * @param sz The exception message.
     */
    void HandleException(const std::exception &e, std::string sz);

    /**
     * @brief Handles the update of the risk limits.
     */
    void onLimitsUpdate();

    /**
     * @brief Checks if the risk limits are available.
     * @return True if the risk limits are available, false otherwise.
     */
    bool GotLimits();

    /**
     * @brief Prints the RiskManager information.
     */
    void Print();

    /**
     * @brief Handles the update of the profit and loss (PnL).
     * @param sl The stop loss value.
     * @param tp The take profit value.
     */
    void onPnlUpdate(double sl, double tp);

    /**
     * @brief Sets the enable flag for the provided risk limits.
     * @param rl The risk limits to set the enable flag for.
     */
    void setEnable(RiskLimits &rl);

    /**
     * @brief Activates the panic mode for the specified number of seconds.
     * @param secsToClose The number of seconds to activate the panic mode for.
     */
    void panicMode(int secsToClose);

    /**
     * @brief Closes all positions.
     */
    void closePositions();

    /**
     * @brief Closes positions by the provided group IDs.
     * @param groupIds The array of group IDs to close positions for.
     * @param groupCount The number of group IDs in the array.
     */
    void closePositionsByGroup(const char* const* groupIds, int groupCount);

    /**
     * @brief Cancels all orders.
     */
    void cancelOrders();

    /**
     * @brief Cancels orders by the provided order IDs.
     * @param orderIds The array of order IDs to cancel.
     * @param orderCount The number of order IDs in the array.
     */
    void cancelOrders(const char* const* orderIds, int orderCount);

    /**
     * @brief Cancels orders by the provided group IDs.
     * @param groupIds The array of group IDs to cancel orders for.
     * @param groupCount The number of group IDs in the array.
     */
    void cancelOrdersByGroup(const char* const* groupIds, int groupCount);

    /**
     * @brief Shuts down the RiskManager.
     */
    void shutdown();

    /**
     * @brief Handles the error with the provided text.
     * @param text The error text.
     */
    void onError(const char *text);

    /**
     * @brief Handles the disconnection event.
     */
    void onDisconnected();

    /**
     * @brief Resets the position for the specified session, symbol, and Bats ID.
     * @param sessionId The session ID.
     * @param symbol The symbol.
     * @param szBatsId The Bats ID.
     */
    void ResetPosition(std::string sessionId, std::string symbol, std::string szBatsId);

    /**
     * @brief Changes the position for the specified Bats ID by the provided net change.
     * @param szBatsId The Bats ID.
     * @param netchange The net change.
     */
    void PositionChange(std::string szBatsId, int netchange);

    /**
     * @brief Retrieves the positions.
     */
    void Positions();

    /**
     * @brief Retrieves the net position for the specified Bats ID.
     * @param szBatsId The Bats ID.
     * @return The net position.
     */
    int NetPosition(std::string szBatsId);

private:
    /**
     * @brief The consumer thread function.
     * @param pValue The thread value.
     * @return The thread result.
     */
    static void *ConsumerThread(void* pValue);

private:
    bool m_bIsInited;

    std::string ARGO_ZMQ_SERVER_;

    std::string SOURCE_;
    std::string ACCOUNT_;
    std::string KEY_;

    std::string ZMQ_RISK_PUSH_SERVER_;

    pthread_t tid_;
    pthread_t tid2_;
    int m_affinity;

    tbb::concurrent_queue<Order> _qOrds;
    tbb::concurrent_queue<std::string> _qJSON;
    tbb::concurrent_queue<std::string> _qRMS;

    /**
     * @brief Logs the provided message with the specified color.
     * @param szMsg The message to log.
     * @param iColor The color of the log message.
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

    bool m_bGotLimits;

    /**
     * @brief Retrieves the current time in the format used by the Risk Management System (RMS).
     * @return The current time as a string.
     */
    std::string getRMSTime();

    std::map<std::string, RiskLimits> _Limits;
    std::map<std::string, RiskLimits> _ProductLimits;

    using time_stamp = std::chrono::time_point<std::chrono::system_clock,
                                               std::chrono::microseconds>;
};
};
