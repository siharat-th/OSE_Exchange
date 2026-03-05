/*
 * KtnCmeMdpV4.hpp
 *
 *  Created on: March 5, 2025
 *      Author: sgaer
 */

#ifndef KTNCMEMDPV5_HPP_
#define KTNCMEMDPV5_HPP_

#pragma once
#pragma GCC diagnostic ignored "-Wreorder"

#include <KT01/Core/Log.hpp>
#include <KT01/Core/PerformanceCounter.h>
#include <KT01/Core/PerformanceTracker.hpp>
#include <KT01/Core/NamedThread.hpp>
#include <KT01/Core/Macro.hpp>

#include <exchsupport/cme/mdp3/Mdp3Messages.hpp>
#include <exchsupport/cme/mdp3/Mdp3MessagePrinter.hpp>
#include <exchsupport/cme/mdp3/Mdp3Templates.hpp>
#include <KT01/SecDefs/CmeSecMasterV2.hpp>
#include <akl/PriceConverters.hpp>
using namespace KTN::Mdp3;

#include <exchsupport/cme/mktdata/ChannelStructsV2.hpp>
using namespace KTN;
using namespace KTN::Feeds::CME;
using namespace KT01::SECDEF::CME;

#include <AlgoSupport/algo_structs.hpp>
#include <AlgoSupport/AlgoMktDataCallback.hpp>

#include <KT01/Net/Udp/EFVIUdpV4.hpp>
#include <KT01/Net/Udp/UdpCallback.hpp>
using namespace KT01::NET::UDP;

//book building
#include <KT01/DataStructures/IdxOrderBookManager.hpp>
#include <KT01/DataStructures/DepthEnums.hpp>
#include <KT01/DataStructures/DepthBook.hpp>
using namespace KT01::DataStructures::MarketDepth;

using namespace std;
using namespace KTN::EFVI;
using namespace KT01::Core;
using namespace KTN;

#include <exchsupport/MktDataHandlerBase.hpp>

/**
 * @class KtnCmeMdpV4
 * @brief Represents a class that handles CME MDP3 market data. 
 * @details Version 5 uses incremental book building to give a 10-deep book as well as top of book.  
 * This class should be backwards-compatabile with the previous version 4.
 */
class KtnCmeMdpV5 : public NamedThread, public EFVIUdpV4, public MktDataHandlerBase
{
public:
    using UpdateAlgoCallback = void (*)(const DepthBook&);

    /**
     * @brief Constructs a KtnCmeMdpV4 object.
     * @param name The name of the object.
     * @param core The core number.
     * @param handletrades Flag indicating whether to handle trades.
     */
    explicit KtnCmeMdpV5(string name, int core, bool handletrades);

    /**
     * @brief Destroys the KtnCmeMdpV4 object.
     */
    virtual ~KtnCmeMdpV5();

    /**
     * @brief Starts the KtnCmeMdpV4 object.
     */
    virtual void Start() override final;

    /**
     * @brief Stops the KtnCmeMdpV4 object.
     */
    void Stop();

    /**
     * @brief Resets the KtnCmeMdpV4 object.
     */
    void Reset();

    /**
     * @brief Prints the statistics of the KtnCmeMdpV4 object.
     */
    virtual void PrintStats() override final;

    /**
     * @brief Adds a list of symbols to the KtnCmeMdpV4 object.
     * @param symbols The list of symbols to add.
     */
    void AddSymbols(vector<string> symbols);

    /**
     * @brief Adds a list of instruments to the KtnCmeMdpV4 object.
     * @param legs The list of instruments to add.
     */
    void AddSymbols(vector<instrument> legs);

    /**
     * @brief Adds a symbol to the KtnCmeMdpV4 object.
     * @param symb The symbol to add.
     * @param index The index of the symbol for the OrderBook Manager vector
     */
    virtual void AddSymbol(const std::string &symb, int index, uint32_t secId) override final;

   
    /**
     * @brief Adds MBO (Market By Order) for a symbol to the KtnCmeMdpV4 object.
     * @param symb The symbol to add MBO for.
     */
    void AddMbo(string symb);

    /**
     * @brief Adds a channel filter to the KtnCmeMdpV4 object.
     * @param secdef The CME security definition.
     * @param feedtype The feed type.
     */
    void AddChannelFilter(const CmeSecDef& secdef, const string& feedtype);

    /**
     * @brief Subscribes to the market data callback.
     * @param subscriber The market data callback subscriber.
     */
    virtual void Subscribe(KTN::AlgoMktDataCallback* subscriber) override final;

    /**
     * @brief Prints the KtnCmeMdpV4 object.
     * @param verbose Flag indicating whether to print verbose information.
     */
    void Print(bool verbose);

    /**
     * @brief Gets the flag indicating whether to handle MBO (Market By Order).
     * @return True if MBO should be handled, false otherwise.
     */
    inline bool HandleMbo() const {
        return _handleMbo;
    }

    /**
     * @brief Sets the flag indicating whether to handle MBO (Market By Order).
     * @param handleMbo The flag value to set.
     */
    inline void SetHandleMbo(bool handleMbo) {
        _handleMbo = handleMbo;
    }

    /**
     * @brief Gets the flag indicating whether to handle trades.
     * @return True if trades should be handled, false otherwise.
     */
    inline bool HandleTrades() const {
        return _handleTrades;
    }

    /**
     * @brief Sets the flag indicating whether to handle trades.
     * @param handleTrades The flag value to set.
     */
    // inline void SetHandleTrades(bool handleTrades) {
    //     _handleTrades = handleTrades;
    // }

    /**
     * @brief Gets the flag indicating whether to handle volume.
     * @return True if volume should be handled, false otherwise.
     */
    inline bool HandleVolume() const {
        return _handleVolume;
    }

    /**
     * @brief Sets the flag indicating whether to handle volume.
     * @param handleVolume The flag value to set.
     */
    inline void SetHandleVolume(bool handleVolume) {
        _handleVolume = handleVolume;
    }

public:
	//IndexedOrderBookManager Obm;
	std::vector<string> _FILTERS;
  

private:
	UpdateAlgoCallback updateCallback;
	AlgoMktDataCallback* _callback;

	const int _MAXSECS = 16;
	std::vector<int> updatedIndices;

    /**
     * @brief Runs the KtnCmeMdpV4 object.
     */
    void Run();


    /**
     * @brief Prints the sequences.
     * @param feedtype The feed type.
     * @param servicename_ The service name.
     */
    void printSeqs(int feedtype, std::string servicename_);

    /**
     * @brief Callback function for receiving data.
     * @param buf The data buffer.
     * @param len The length of the data buffer.
     */
    void on_rx_cb(const char* buf, const int len, const uint32_t address, const uint64_t recvTime) override;

    /**
     * @brief Logs a message.
     * @param szMsg The message to log.
     * @param iColor The color of the log message.
     */
    void LogMe(const std::string& szMsg, LogLevel::Enum level =LogLevel::INFO);

    /**
     * @brief Processes the MD Incremental Refresh Price 46 message.
     * @param buf The message buffer.
     * @param iMsgLen The length of the message buffer.
     * @return The result of the processing.
     */
    int ProcessMDIncrementalRefreshPrice46(const char* buf, const int iMsgLen, const uint64_t recvTime);

    
    /**
     * @brief Processes the MD Incremental Refresh Trade Summary 48 message.
     * @param buf The message buffer.
     * @param iMsgLen The length of the message buffer.
     * @return The result of the processing.
     */
    int ProcessMDIncrementalRefreshTradeSummary48(const char* buf, const int iMsgLen, const uint64_t recvTime);

    /**
     * @brief Processes the Security Status 30 message.
     * @param buf The message buffer.
     * @param iMsgLen The length of the message buffer.
     */
    void ProcessSecurityStatus30(const char* buf, const int iMsgLen, const uint64_t recvTime);

    /**
     * @brief Processes the Snapshot Full Refresh 52 message.
     * @param buf The message buffer.
     * @param iMsgLen The length of the message buffer.
     * @return The result of the processing.
     */
    int ProcessSnapshotFullRefresh52(const char* buf, const int iMsgLen);

    /**
     * @brief Processes the Snapshot Full Refresh Entry.
     * @param entry The Snapshot Full Refresh entry.
     * @param secid The security ID.
     * @param rptseq The report sequence.
     * @param book The book market data, Version 2.
     */
    void ProcessSnapshotEntry(const SnapshotFullRefreshEntry* entry, int32_t secid, uint32_t rptseq, DepthBook& book);


private:
    bool _DEBUGMODE;
    PerformanceTracker _trk;

    int _numCallbacks = 0;

    bool _handleMbo;

    bool _handleVolume;
    bool _handleTrades;
   

    Configuration _Channels;

    vector<std::pair<int, string>> _chids;
    vector<uint32_t> _secids;
    vector<string> _symbs;

    volatile int _CHANNEL;
    volatile long _COUNT;
    std::string _NAME;
    std::string _PREFIX_NAME;
    volatile int _CPUINC;
    volatile int _CORE;
    std::atomic<bool> _ACTIVE;

    vector<KTN::Mdp3::Template> _vtemplates;

    uint32_t _LAST_SEQa;
    uint32_t _LAST_SEQb;

    using SeqTracker = std::pair<uint32_t, uint32_t>;
    SeqTracker seqTracker[2];
    int index { 0 };

    std::vector<int> MsgFilter;
};

#endif /* KTNCMEMDPV4_H_ */
