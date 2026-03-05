/**
 * @file AlgoBaseV3.hpp
 * @brief Header file for the AlgoBaseV3 class. This class handles algorithmic trading functionalities.
 *
 * @details This file defines the AlgoBaseV3 class, which is responsible for handling market data,
 * trade execution, session management, and other algorithmic functionalities.
 *
 * @author sgaer
 * @date Created: Apr 24, 2021
 * @date Updated: Oct 21, 2023 (Version 3)
 */

#pragma once


#ifndef ALGOBASEV3_HPP_
#define ALGOBASEV3_HPP_

#include <Algos/AlgoPch.hpp>
#include <AlgoParams/IParams.hpp>
#include <KT01/Core/Macro.hpp>
#include <KT01/Core/NamedThread.hpp>
#include <ExchangeHandler/IExchangeSender.hpp>
#include <ExchangeHandler/session/SessionCallback.hpp>
#include <exchsupport/cme/mktdata/KtnCmeMdpV5.hpp>
#include <exchsupport/cme/settings/CmeSettingsHelper.hpp>
#include <Orders/OrderManagerV5.hpp>
#include <Orders/OrderApiFunctions.hpp>
#include <Reporters/AlgoV3Reporter.hpp>

#include <KT01/SecDefs/CmeSecMasterV2.hpp>
using namespace KT01::SECDEF::CME;

#include <KT01/DataStructures/DepthBook.hpp>
using namespace KT01::DataStructures::MarketDepth;

#include <AlgoEngines/QuoterV3.hpp>

namespace KTN {

    /**
     * @class AlgoBaseV3
     * @brief The base class for algos, implementing market data callbacks and session management.
     *
     * @details This class handles common aspects of algos such as market data callbacks,
     * order management, and session status updates. It is designed to be extended with specific implementations
     * of algorithmic logic.
     */
    class AlgoBaseV3 : public AlgoMktDataCallback, public SessionCallback
    {
    public:
        /**
         * @brief Constructs a new AlgoBaseV3 object.
         * @param params Initialization parameters for the algorithm.
         */
        explicit AlgoBaseV3(AlgoInitializer params);

        /**
         * @brief Destructor for AlgoBaseV3.
         */
        virtual ~AlgoBaseV3();

        /**
         * @brief Prints information about the algorithm instance.
         */
        virtual void Print();

        /**
         * @brief Returns the globally unique identifier (GUID) of the algorithm.
         * @return The unique identifier as a string.
         */
        inline std::string GUID();

        void StartMarketDataHandler();

        /**
         * @brief Polls the system for events.
         */
        void Poll();

        /**
         * @brief Processes a JSON-formatted string.
         * @param json A JSON-formatted string representing algo parameters.
         * @note This is a pure virtual function that must be implemented by derived classes.
         */
        virtual void onJson(std::string json) = 0;

        /**
         * @brief Processes algo engine-specific commands.
         * @param cmd The engine command to process.
         */
        virtual void onEngineJson(EngineCommand& cmd);

        /**
         * @brief Passes a custom strategy message from the client.
         * @param json A JSON-formatted string containing the messaged.
         */
        virtual void onAlgoMessage(const std::string &json);

		virtual void onClientConnected(const std::string &clientId) { }
		virtual void onClientDisconnected(const std::string &clientId) { }

        /**
         * @brief Triggered when the algorithm is launched.
         * @param json A JSON-formatted string representing launch data.
         */
        virtual void onLaunch(std::string json) = 0;

        /**
         * @brief Performs calculation logic for the algorithm.
         */
        virtual void onCalculate() = 0;

        /**
         * @brief Handles a trade event.
         * @param index Trade index.
         * @param price Trade price.
         * @param size Trade size.
         * @param aggressor Trade aggressor indicator.
         */
        virtual void onTrade(int index, double price, double size, int aggressor);

        /**
         * @brief Handles market data updates.
         * @param index Market data index.
         */
        virtual void onMktData(int index);


        /**
         * @brief Handles state change events.
         * @param state New state of the algorithm.
         */
        virtual void onStateChange(int state);

        /**
         * @brief Handles session heartbeat events.
         * @param msgseqnum Message sequence number.
         */
        virtual void onHeartbeat(int msgseqnum);

        /**
         * @brief Returns the GUID of the callback.
         * @return The GUID as a string.
         */
        virtual const std::string& getGuid() const;
        

        /**
         * @brief Handles session status updates.
         * @param exch Exchange name.
         * @param mktsegid Market segment ID.
         * @param protocol Protocol number.
         * @param desc Description of the session status.
         * @param state State of the session.
         */
        virtual void onSessionStatus(std::string exch, int mktsegid, int protocol, std::string desc, std::string state);

        /**
         * @brief Executes spread leg orders.
         * @param ord Order object containing details of the spread leg.
         */
        void onSpreadLegOrderExecution(KTN::Order& ord);

        // Order management callbacks
        virtual void minOrderAck(KTN::OrderPod& ord) override;
        virtual void minOrderCancel(KTN::OrderPod& ord) override;
        virtual void minOrderModify(KTN::OrderPod& ord) override;
        virtual void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) override;

        /**
         * @brief Handles order rejection events.
         * @param ord The order object that was rejected.
         * @param text Reason for rejection.
         */
        virtual void minOrderReject(KTN::OrderPod& ord, const std::string& text);
        virtual void minOrderCancelReject(KTN::OrderPod& ord, const std::string& text);
        virtual void minOrderCancelReplaceReject(KTN::OrderPod& ord, const std::string& text);

        /**
         * @brief Handles hedge order sent events.
         * @param clordid Client order ID.
         * @param reqid Request ID.
         * @param secid Security ID.
         * @param side Order side.
         * @param qty Quantity.
         * @param price9 Price.
         * @param instindex Instrument index.
         */
        virtual void onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
            uint32_t qty, int32_t price9, uint16_t instindex);

        /**
         * @brief Handles order sent events.
         * @param clordid Client order ID.
         * @param reqid Request ID.
         * @param secid Security ID.
         * @param side Order side.
         * @param qty Quantity.
         * @param price Price.
         * @param trig Order trigger.
         */
        virtual void onOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
            uint32_t qty, int32_t price, KOrderAlgoTrigger::Enum trig);

        /**
         * @brief Enables or disables the algorithm.
         * @param bEnable A boolean indicating whether to enable or disable.
         */
        virtual void Enable(bool bEnable) = 0;

        /**
         * @brief Resets the algorithm's position.
         */
        virtual void ResetPosition();

        /**
         * @brief Toggles a specific command by ID.
         * @param id Command ID to toggle.
         */
        virtual void ToggleCommand(int id);

        /**
         * @brief Shuts down the algorithm.
         */
        virtual void Shutdown();

        /**
         * @brief Restarts the algorithm.
         */
        virtual void Restart();

        /**
         * @brief Cancels all orders.
         */
        virtual void CancelAllOrders();

        /**
         * @brief Processes received order events.
         * @param ord The received order.
         * @param index Order index.
         */
        virtual void OrderReceived(KTN::Order& ord, int index);

        /**
         * @brief Processes updates for an order. OrderMin is a minimal order object.
         * @param ord The order pod containing order details.
         */
        virtual void OrderMinUpdate(KTN::OrderPod& ord);

        /**
         * @brief Shuts down the algorithm handler.
         */
        void shutdownHandler();

        /**
         * @brief Handles bid-ask market data updates.
         * @param md The market data structure.
         */
        virtual void onMktDataBidAsk(const DepthBook& md);

        /**
         * @brief Handles trade market data updates.
         * @param md The market data structure.
         */
        virtual void onMktDataTrade(const DepthBook& md);

        /**
         * @brief Handles security status updates.
         * @param md The market data structure.
         */
        virtual void onMktDataSecurityStatus(int secid, BookSecurityStatus secstatus, BookSecurityTradingEvent event, const uint64_t recvTime);

        virtual void onKillswitchUpdate(const bool state);
        virtual void DisableRisk();
        virtual void TryStop();

    public:
        std::vector<instrument> _LEGS;

    protected:
        // Logging and Notification Functions
        void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);
        void Notify(std::string msg, MsgLevel::Enum level, MsgType::Enum msgtype);

        // Printing Functions
        void PrintMdMap();
        void PrintBookMap();
        void PrintBook(instrument inst);

        // State Management Functions
        void ChangeQuoteState(const QuoteState::Enum& newstate);
        void ChangeHedgeState(const HedgeState::Enum& newstate);

        // Registration Functions
        void RegisterCallbackId();
        void RegisterParams(std::string json);
        void RegisterCmeSecId(instrument& inst);
        void RegisterCfeSecId(instrument& inst);

        // Quote Retrieval Functions
        DepthBook& GetQuote(const instrument& inst);
        bool GetQuote(const instrument& inst, DepthBook& md);
        bool GetQuoteByIndex(int index, DepthBook& md);

        // Book Retrieval Functions
        DepthBook& GetBook(const instrument& inst);
        bool GetBook(const instrument& inst, DepthBook& book);

        // Risk Management Functions
        bool RiskCheckQty(int qty, int totalqty, int execqty, KOrderSide::Enum side);

        // Order Processing Functions
        void SendOrder(KTN::OrderPod& ord, instrument& inst);
        void SendOrderWithHedge(OrderWithHedge& oh, instrument& inst);

        // Validation and Lookup Functions
        DepthBook& findBySecId(int id);
        int indexBySecId(int id);

        // Utility Functions
        void GenIds(std::string prefix);
        void AddReportingMsg(std::string msg, std::string msgtype);
        void AddCmeSymbol(std::string symbol);

        // Parameter Update Functions
        void UpdateMetaParams(algometa meta);

        // Order Api Lookup Functions
        //void LoadOrdersFromApi();


        inline int GetBid(int index)
		{
			return _mdh->Obm.books[index].Bid;
		}

		inline int GetAsk(int index)
		{
			return _mdh->Obm.books[index].Ask;
		}

		inline void GetBid(int index, atomicquote& qt)
		{
			qt.price = _mdh->Obm.books[index].Bid;
			qt.qty = _mdh->Obm.books[index].BidSize;
		}

		inline void GetAsk(int index, atomicquote& qt)
		{
			qt.price = _mdh->Obm.books[index].Ask;
			qt.qty = _mdh->Obm.books[index].AskSize;
		}



    private:
        LogSvc* _logsvc;

    protected:
        // Atomic and Volatile Variables
        std::atomic<bool> _ACTIVE;
        volatile bool stopped_;
        volatile bool _RPT_ACTIVE;

        // Pointer Members
        IExchangeSender* _ordersender;
        //KtnCmeMdpV5* _mdh;
		MktDataHandlerBase *_mdh { nullptr };
        AlgoV3Reporter* _algorpt;
        QuoterV3 _quoter[24];

        // Boolean Flags
        bool _CME_BY_SECID;
        bool _bQuoteInFlight;
        bool _MEASURE;
        bool _MKT_DATA_OK;
        bool _GOT_DATA;
        bool _WATCH_TRADES;
        bool _WATCH_VOLUME;
        bool _USEAUDIT;
        bool _ENABLED;
        bool _CAN_TRADE;
        bool _HANDLER_STARTED;
        bool _USE_REPORTING_MSGS;
        bool _USE_STOPS;
        bool _CME_MKT_DATA_STARTED;
        bool _ALLSYMBOLS;
        bool _VERBOSE;

        // Integer Variables
        int _NUMLEGS;
        int _EXCH_CALLBACK_ID;
        int _MKT_DATA_CPU;
        int _TESTID;
        int _CNT;
        int _GROUPCNT;
        int _CORE;

        // Double Variables
        double _NETPOS;
        double _TOTALEXEC;

        // Long Variables
        long _DATA_TRIES;

        // String Variables
        std::string _GUID;
        std::string _ORG;
        std::string _BASE;
        std::string _SPRDID;
        std::string m_Name;

        // Unordered Maps
        std::unordered_map<std::string, instrument> _LEGMAP;
        std::unordered_map<int, instrument> _SECIDMAP;
        std::unordered_map<long, std::string> _certIdMap;
        std::unordered_map<std::string, long> _certIdSymbolMap;
        std::unordered_map<uint64_t, Order> _apiorders;

        // Enum Types
        QuoteState::Enum _QuotingState;
        HedgeState::Enum _HedgingState;

        // Other Member Objects
        CmeMsgw _msgw;
        OrderManagerV5 _Ords;
        PerformanceTracker _trk;
        AlgoHelpers helpers;
        Slacker _SLACKER;
        genericstats _mystats;
        LogMsg _LOG;
        algometa _PARAMS;
        Settings _settings;


    };

} /* namespace KTN */

#endif /* ALGOBASEV3_HPP_ */
