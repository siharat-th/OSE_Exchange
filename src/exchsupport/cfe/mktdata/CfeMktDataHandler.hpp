/**
 * Copyright (c) $2017-2024, Katana Financial LLC. All rights reserved.
 *
 * @file CfePitchHandler.h
 * @date Saturday, September 28, 2024
 */

#ifndef CFE_MKTDATA_HANDLER_H_
#define CFE_MKTDATA_HANDLER_H_

#pragma once
#pragma GCC diagnostic ignored "-Wreorder"

#include <cassert>
#include <string>

// should we move this to the processor?
#include <KT01/Net/feed_defines.hpp>
#include <KT01/Net/FeedHandler.hpp>
#include <KT01/Net/Udp/UdpCallback.hpp>

//#include <KtnEfvi/SocketFeed.h>
//using namespace i01::net;

#include <KT01/Core/StringSplitter.hpp>
#include <KT01/Core/Macro.hpp>
#include <KT01/Core/Log.hpp>

#include <exchsupport/cfe/config/CfeChannels.hpp>
#include <exchsupport/cfe/config/CfeSpinConfig.hpp>
#include <exchsupport/cfe/msgproc/CfeMsgProc.hpp>

#include <KT01/Net/Udp/EFVIUdpV4.hpp>

#include <AlgoSupport/AlgoMktDataCallback.hpp>
#include <exchsupport/MktDataHandlerBase.hpp>


/**
 * @class CfePitchHandler
 * @brief This class represents a the main CME MDP3 Market Data Server object that inherits from UdpCallback.
 *        It provides functionality for starting, stopping, and resetting the object,
 *        as well as printing statistics and handling menu choices.
 */
class CfeMktDataHandler : public UdpCallback, public MktDataHandlerBase
{
public:
    /**
     * @brief Default constructor for CfePitchHandler class.
     */
    CfeMktDataHandler(const std::string &name, int core);

    /**
     * @brief Destructor for CfePitchHandler class.
     */
    virtual ~CfeMktDataHandler();

    /**
     * @brief Starts the CfePitchHandler object.
     */
    virtual void Start() override final;

    /**
     * @brief Stops the CfePitchHandler object.
     */
    void Stop();

    /**
     * @brief Resets the CfePitchHandler object.
     */
    void Reset();

    /**
     * @brief Prints the statistics of the CfePitchHandler object.
     */
    void PrintStats();

    /**
     * @brief Handles the menu choice for the CfePitchHandler object.
     * @param choice The menu choice to handle.
     */
    void MenuChoice(int choice);

	virtual void Subscribe(KTN::AlgoMktDataCallback *subscriber) override final;
	virtual void AddSymbol(const std::string &symbol, int index, uint32_t secId) override final;

private:
    /**
     * @brief Logs a message with an optional color.
     * @param szMsg The message to log.
     * @param iColor The color of the message (optional, default is LOG_WHITE).
     */
    void LogMe(std::string szMsg);


    /**
     * @brief Static method for the consumer thread.
     * @param pValue The value passed to the thread.
     * @return void* The thread result.
     */
    static void *ConsumerThread(void *pValue);

    /**
     * @brief Callback function for receiving data.
     * @param buf The buffer containing the received data.
     * @param len The length of the received data.
     * @param segid The segment ID.
     */
    void onRecv(char *buf, int len, int segid) override;

    /**
     * @brief Prints the sequences for a given feed type and service name.
     * @param feedtype The feed type.
     * @param servicename_ The service name.
     */
    void printSeqs(int feedtype, std::string servicename_);

    /**
     * @brief Prints a hex dump of the given data.
     * @param pv The data to dump.
     * @param len The length of the data.
     */
    inline void hexdump(const void *pv, int len);

    /**
     * @brief Moves the given buffer pointer by the specified size.
     * @param buf The buffer pointer to move.
     * @param size The size to move the buffer pointer by.
     */
    inline void bytemover(char *&buf, size_t size);

    /**
     * @brief Tests a bit in a register.
     * @param reg The register to test.
     * @param i The index of the bit to test.
     * @return bool True if the bit is set, false otherwise.
     */
    inline bool testBit(uint8_t reg, int i);

private:
    KTN::CFE::Pitch::CfeMsgProc _mp;

    std::string m_Me;
    
    bool _USE_PITCH;
    bool _USE_TOP;

    vector<string> _chids;
    vector<string> _roots;

    DataProcSettings _settings;

    bool m_bVERBOSE;
    bool m_bRunning;

    KTN::EFVI::FeedHandler *_FEED;

	const int core;
	const std::string name;
};

#endif /* CFE_MKTDATA_HANDLER_H_ */
