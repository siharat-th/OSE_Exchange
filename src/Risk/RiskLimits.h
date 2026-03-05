#pragma once

//#include "CLogMe.h"
#include <KT01/Core/Log.hpp>
#include <sstream>

using namespace std;

/**
 * @brief The RiskLimits class represents the risk limits for a particular trading account.
 * 
 * The RiskLimits class provides methods to update the risk limits, set the log level, enable/disable the risk limits,
 * check various risk conditions, and perform other operations related to risk management.
 */
class RiskLimits
{
public:
    /**
     * @brief Constructs a new RiskLimits object.
     */
    RiskLimits();

    /**
     * @brief Destroys the RiskLimits object.
     */
    ~RiskLimits();

    /**
     * @brief Updates the risk limits with the specified parameters.
     * 
     * @param enable The enable flag indicating whether the risk limits are enabled or disabled.
     * @param acro The acronym of the trading account.
     * @param prod The product name.
     * @param prodtype The product type.
     * @param exch The exchange name.
     * @param iMaxOrderSize The maximum order size.
     * @param iMaxLong The maximum long position.
     * @param iMaxShort The maximum short position.
     * @param iMaxBuyOpen The maximum buy open quantity.
     * @param iMaxSellOpen The maximum sell open quantity.
     * @param iLogLevel The log level.
     */
    void Update(int enable, std::string acro, std::string prod, std::string prodtype, std::string exch, int iMaxOrderSize, int iMaxLong, int iMaxShort, int iMaxBuyOpen, int iMaxSellOpen, int iLogLevel);

    /**
     * @brief Sets the log level.
     * 
     * @param iLogLevel The log level to set.
     */
    void LogLevel(int iLogLevel);

    /**
     * @brief Sets the enable flag to enable or disable the risk limits.
     * 
     * @param enable The enable flag.
     */
    void SetEnable(bool enable);

    //Top level risk values; take this from XML
    string acronym;
    string account;
    string product;
    string producttype;
    string exchange;
    int max_order_qty;
    int max_open_qty_buy;
    int max_open_qty_sell;
    int max_pos_long;
    int max_pos_short;

    int curr_open_qty_buy;
    int curr_open_qty_sell;
    int curr_pos_long;
    int curr_pos_short;

    int enabled;

    string keyvalue;

    bool hasData;

    /**
     * @brief Creates a key for the risk limits.
     */
    void CreateKey();

    /**
     * @brief Checks if the risk limits are within acceptable limits.
     * 
     * @return True if the risk limits are within acceptable limits, false otherwise.
     */
    bool OK();

    /**
     * @brief Checks if the current position is within acceptable limits.
     * 
     * @return True if the current position is within acceptable limits, false otherwise.
     */
    bool CheckPos();

    /**
     * @brief Checks if the order quantity is within acceptable limits.
     * 
     * @param bIsBid A flag indicating whether the order is a bid or ask.
     * @return True if the order quantity is within acceptable limits, false otherwise.
     */
    bool CheckOrds(bool bIsBid);

    /**
     * @brief Prints the risk limits.
     * 
     * @param current_risk_only A flag indicating whether to print only the current risk limits.
     */
    void Print(bool current_risk_only = false);

    /**
     * @brief Checks if the order size is within legal limits.
     * 
     * @param iOrderQty The order quantity.
     * @return True if the order size is within legal limits, false otherwise.
     */
    bool LegalSize(int iOrderQty);

    /**
     * @brief Updates the risk limits based on a fill.
     * 
     * @param iFillSize The fill size.
     * @param bIsBuy A flag indicating whether the fill is a buy or sell.
     */
    void GotFill(int iFillSize, bool bIsBuy);

    /**
     * @brief Updates the risk limits based on an order.
     * 
     * @param iSize The order size.
     * @param bIsBid A flag indicating whether the order is a bid or ask.
     * @param bIsCxl A flag indicating whether the order is a cancellation.
     */
    void GotOrder(int iSize, bool bIsBid, bool bIsCxl);

    /**
     * @brief Calculates the net position.
     * 
     * @return The net position.
     */
    int NetPos();

    /**
     * @brief Gets the maximum number of contracts for the specified bid/ask.
     * 
     * @param bIsBid A flag indicating whether the bid/ask is a bid or ask.
     * @return The maximum number of contracts.
     */
    int GetMaxContracts(bool bIsBid);

    /**
     * @brief Gets the number of orders for the specified bid/ask.
     * 
     * @param bIsBid A flag indicating whether the bid/ask is a bid or ask.
     * @return The number of orders.
     */
    int GetOrderCount(bool bIsBid);

    /**
     * @brief Gets the position for the specified bid/ask.
     * 
     * @param bIsBid A flag indicating whether the bid/ask is a bid or ask.
     * @return The position.
     */
    int GetPosition(bool bIsBid);

    /**
     * @brief Gets the maximum order size.
     * 
     * @return The maximum order size.
     */
    int GetMaxOrderSize();

    /**
     * @brief Checks if adding the specified quantity to the bid/ask will exceed the maximum limits.
     * 
     * @param bIsBid A flag indicating whether the bid/ask is a bid or ask.
     * @param iAdd The quantity to add.
     * @return True if adding the quantity will exceed the maximum limits, false otherwise.
     */
    bool WillMaxOut(bool bIsBid, int iAdd);

private:
    bool m_bIsEnabled;

    //CLogMe m_log;
    //void LogMe(std::string szMsg);
    /**
     * @brief Logs a message with an optional value.
     * 
     * @param szMsg The message to log.
     * @param iValue The optional value to log.
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

    int m_iLogLevel;
};
