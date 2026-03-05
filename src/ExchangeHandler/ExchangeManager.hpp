//============================================================================
// Name        	: ExchangeManager.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 24, 2023 9:17:45 AM
//============================================================================

#ifndef SRC_EXCHANGEHANDLERS2_EXCHANGEMANAGER_HPP_
#define SRC_EXCHANGEHANDLERS2_EXCHANGEMANAGER_HPP_

#include <ExchangeHandler/ExchIncludes.hpp>
#include <KT01/Core/NamedThread.hpp>

#include <exchsupport/cme/CmeGT5.hpp>
#include <exchsupport/cfe/CfeGT4.hpp>
#include <exchsupport/eqt/EqtGT4.hpp>
#include <exchsupport/ose/OseGT.hpp>

using namespace KTN::Core;
using namespace KTN::Session;


namespace KTN {

/**
 * Class Name: ExchangeManager
 *  Purpose: Container class for exchange handlers.
 *  This class will spawn a thread for polling of exchange handler
 *  You can also pass messages/commands to the exchange handler via queues
 */
class ExchangeManager : public NamedThread
{
public:
    /**
     * Constructor for ExchangeManager class.
     * @param qords The concurrent queue for storing order pods.
     * @param settingsfile The settings file for the exchange manager.
     */
    ExchangeManager(tbb::concurrent_queue<KTN::OrderPod>& qords, const string& settingsfile);

    /**
     * Destructor for ExchangeManager class.
     */
    virtual ~ExchangeManager();

    /**
     * Handles the command received by the exchange manager.
     * @param cmd The instruction/command to be handled.
     */
    void onCommand(const Instruction& cmd);

    /**
     * Sends an order pod to the exchange.
     * @param ord The order pod to be sent.
     */
    void Send(KTN::OrderPod& ord);

    /**
     * Sends an order with hedge to the exchange.
     * @param oh The order with hedge to be sent.
     */
    void SendOrderWithHedge(OrderWithHedge& oh);

    /**
     * Sends a hedge instruction to the exchange.
     * @param ordereqid The order request ID.
     * @param hdg The vector of spread hedge instructions.
     */
    void SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3>& hdg);

private:
    /**
     * Logs a message.
     * @param szMsg The message to be logged.
     * @param iColor The color of the log message.
     * @param Persist Indicates whether the log message should be persisted.
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO, bool Persist = false);

    /**
     * Runs the exchange manager thread.
     */
    void Run() override;

    /**
     * Handles the command received by the exchange manager.
     * @param cmd The instruction/command to be handled.
     * @param sess The exchange base for handling the command.
     */
    void HandleCommand(Instruction& cmd, ExchangeBase2* sess);

    /**
     * Checks if a file exists.
     * @param fileName The name of the file to check.
     * @return True if the file exists, false otherwise.
     */
    inline bool file_exists(const char* fileName)
    {
        std::ifstream infile(fileName);
        return infile.good();
    }

private:
    tbb::concurrent_queue<KTN::OrderPod>& _qords;
    tbb::concurrent_queue<Instruction> _qcmds;
    std::string _filename;
    std::atomic<bool> _ACTIVE;
    std::string _SOURCE;
};

} // namespace KTN

#endif /* SRC_EXCHANGEHANDLERS2_EXCHANGEMANAGER_HPP_ */
