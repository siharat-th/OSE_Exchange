#ifndef QUOTE_API_CLIENT_HPP_
#define QUOTE_API_CLIENT_HPP_

// C++ headers
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iterator>
#include <iomanip>
#include <map>
#include <set>
#include <mutex>
#include <thread>
#include <math.h>
#include <chrono>

// POSIX headers
#ifdef __linux__
#include <unistd.h>
#endif

// Lime headers
#include <exchsupport/eqt/citrius/quoteSystemApi.hh>
#include <exchsupport/eqt/citrius/util.hh>
#include <exchsupport/eqt/citrius/CmdLineHandler.hh>
#include <exchsupport/eqt/citrius/Misc.hh>

#include <KT01/Core/Log.hpp>

//#include <DataHandlers/MbpProcessorV2.hpp>
//#include <DataHandlers/MboProcessor.hpp>
//#include <DataHandlers/TradeProcessor.hpp>
//#include <DataHandlers/DbProcessor.hpp>
//#include <DataHandlers/BarProcessor.hpp>
//#include <DataHandlers/DataSimulator.hpp>

//#include <DataHandlers/SecIdSubscriber.hpp>

#include <KT01/DataStructures/OrderBookManager.hpp>
#include <KT01/SecDefs/EquitySecMaster.hpp>
#include <Settings/DataProcSettings.hpp>

using LimeBrokerage::QuoteSystemApi;
using namespace std;
using namespace KT01::DataStructures::MarketDepth;
using namespace KT01::SECDEF::EQT;

static  LimeBrokerage::CmdArgument SERVER_NAME_ARG('s', "server", true, true,
    "Server Name or IP address including port number (i.e. 10.22.11.5:7047)");

static  LimeBrokerage::CmdArgument QUOTE_SOURCE_ARG('q', "quotesource", true, true,
    "Quote Source Id defined by citrius configuration page");

static  LimeBrokerage::CmdArgument USERNAME_ARG('u', "username", true, true,
    "Quote Server Username");

static  LimeBrokerage::CmdArgument PASSWORD_ARG('p', "password", true, true,
    "Quote Server Password");

static  LimeBrokerage::CmdArgument INTERVAL_ARG('i', "interval", true, false,
    "Number of seconds to run this application. Default 60 seconds", "60");

static  LimeBrokerage::CmdArgument DONT_PRINT_ARG('r', "dont-print", false, false,
    "Don't print market data. Default is to print", "false");

static  LimeBrokerage::CmdArgument SUBSCRIBE_ATTR_ARG('o', "sub-attributes", false, false,
    "Subscribe to options/spread attributes. Default false", "false");

static  LimeBrokerage::CmdArgument SUBSCRIBE_BY_UNDERLYING_ARG('n', "sub-underlying", false, false,
    "Subscribe to options by underlying. Default false", "false");

static  LimeBrokerage::CmdArgument SUBSCRIBE_BY_OSI_ARG('c', "sub-osi", false, false,
    "Subscribe to specific options by specifying their OSI symbol. Default false", "false");

static  LimeBrokerage::CmdArgument SUBSCRIBE_INDEX_ARG('I', "sub-index", false, false,
    "Subscribe to index only. Default false", "false");

static  LimeBrokerage::CmdArgument SUBSCRIBE_ALL_ARG('a', "sub-all", false, false,
    "Subscribe to all symbols. Default false", "false");

static  LimeBrokerage::CmdArgument TOP_BOOK_ARG('t', "top", false, false,
    "Enable top of book feature. Default false", "false");

static  LimeBrokerage::CmdArgument AGGR_BY_PRICE_ARG('g', "price-aggr", false, false,
    "Enable aggregation by price feature. Default false", "false");

static  LimeBrokerage::CmdArgument EXT_QUOTE_ARG('x', "ext-quote", false, false,
    "Enable extended quote feature. Default false", "false");

static  LimeBrokerage::CmdArgument TRADES_ONLY_ARG('d', "trades-only", false, false,
    "Enable trades only feature. Default false", "false");

static  LimeBrokerage::CmdArgument SNAPSHOT_ARG('S', "snaposhot", false, false,
    "Enable quote book snapshot feature. Default false", "false");

static  LimeBrokerage::CmdArgument NON_BLOCKING_LOGIN_ARG('b', "non-blocking-login", false, false,
    "Make login non-blocking. Default false", "false");

static  LimeBrokerage::CmdArgument EXPRESS_ARG('e', "express", false, false,
    "Login with an Express session. Default non-Express", "false");

static  LimeBrokerage::CmdArgument VERSION_ARG('v', "version", false, false,
    "Print build version and exit", "false");

static  LimeBrokerage::CmdArgument MMID_FILTER_ARG('m', "mmid-filter", true, false,
    "Enable MMID filters. Default is no filter.", "");

static  LimeBrokerage::CmdArgument ROUND_LOT_TOB_ARG('l', "round-lot-tob", false, false,
    "Enable round lot tob feature", "false");

static  LimeBrokerage::CmdArgument IPO_AND_UPLISTED_ARG('U', "ipo and uplisted subscriptions", false, false,
    "Enable ipo and uplisted feature", "false");


static LimeBrokerage::CmdArgument parameterDefinitionList[] = {
    SERVER_NAME_ARG, QUOTE_SOURCE_ARG, USERNAME_ARG, PASSWORD_ARG,
    INTERVAL_ARG, DONT_PRINT_ARG, SUBSCRIBE_ATTR_ARG, SUBSCRIBE_BY_UNDERLYING_ARG, SUBSCRIBE_BY_OSI_ARG, SUBSCRIBE_INDEX_ARG,
    SUBSCRIBE_ALL_ARG, TOP_BOOK_ARG, AGGR_BY_PRICE_ARG, EXT_QUOTE_ARG, TRADES_ONLY_ARG, SNAPSHOT_ARG, NON_BLOCKING_LOGIN_ARG,
    EXPRESS_ARG, VERSION_ARG, MMID_FILTER_ARG, ROUND_LOT_TOB_ARG, IPO_AND_UPLISTED_ARG
};

static const int parDefSize = sizeof(parameterDefinitionList) / sizeof(parameterDefinitionList[0]);
static const char* DESCRIPTION = "Reference implementation for QuoteSystmeApi.\nEach symbol in the subscription list enclosed in double qutoes";
static const char* USAGE = " <Subscription List>";

static  LimeBrokerage::CmdLineHandler cmdLineDefinition(parameterDefinitionList, parDefSize, DESCRIPTION, USAGE);

class QuoteApiClient : public QuoteSystemApi
{
public:
    QuoteApiClient(bool printData = false, string = "");
    ~QuoteApiClient();

    //
    // Callbacks - Override in order to be called for market data events.
    //
    void onAddSymbol(std::string quoteSource, std::string& symbol, uint32_t symbolIndex, SecurityType securityType);
    void onQuote(const Quote& quote);
    void onTrade(const Trade& trade);
    void onPriceBand(const PriceBand& priceBand);
    void onMarketImbalance(const MarketImbalance& mi);
    void onQuoteAlert(const QuoteAlert& alert);
    void onModifyExecution(const Quote& quote, const Trade& trade);
    void onOrderReplace(const Quote& oldQuote, const Quote& newQuote);
    void onBidQuote(std::string symbol, long id, int size, double price, std::string mmid, std::string quoteSource, uint32_t timestamp);
    void onAskQuote(std::string symbol, long id, int size, double price, std::string mmid, std::string quoteSource, uint32_t timestamp);
    void onTradeTick(std::string symbol, int size, double price, int totalVolume, std::string exchangeId, std::string quoteSource, uint32_t timestamp, std::string tradeType);
    void onQuoteBookRebuildStarted(std::string quoteSource, std::string symbol);
    void onQuoteBookRebuildComplete(std::string quoteSource, std::string symbol);
    void onRoundLotTob(const RoundLotTob& quote);
    void onAuctionUpdate(const AuctionUpdate& auctionUpdate);
    void onAuctionSummary(const AuctionSummary& auctionSummary);
    void onQuoteStreamConnectionStatus(std::string quoteSource, QuoteStreamStatus status, std::string reason);
    void onQuoteSystemApiError(std::string quoteSource, QuoteSystemApiError errorCode, std::string errorMsg);
    void onSpreadDefinition(const LimeBrokerage::SpreadDefinition& spread);
    void onOptionAttributes(const OptionAttributes& optionAttributes);
    void onFeatureControlResponse(int featureCode, int response);
    void onSymbolStatus(const SymbolStatus& symbolStatus);
    void onMulticastLoss(std::string quoteSource, uint16_t channelId, uint32_t totalSkippedMessages, uint32_t timestamp);
    void onIndex(const Index& index);

    void sendDailyStat(const EquitySecDef& def, char updateType, double price);

    // Market State Tracking
    char getMarketState(uint32_t symbolIndex) const;

    string varArgsToString(const char* format, ...);

protected:
    bool printData;
    string id_;
    map<int, std::string> symbolIndexToName;
    size_t numSymbols;
    //bool inBook;

private:
    void LogMe(std::string msg, int color=LOG_WHITE) {
		AKL_LOG(msg, "QuoteApi", color);
    }
    bool ValidSecId(int32_t secid);
    bool ValidMboSecId(int32_t secid);
    void onNewSubscription(int32_t secid);

    // Market State Helper Methods
    void updateMarketState(uint32_t symbolIndex, char newState, const std::string& reason, const std::string& quoteSource);
    void logMarketStateChange(const std::string& symbol, char oldState, char newState, const std::string& reason);
    void checkAndResetMarketStates();

private:
    std::shared_ptr<EquitySecMaster> _secmaster;
    Settings _debugSettings;
    DataProcSettings _settings;

    //std::unique_ptr<MbpProcessorV2> _mbp;
    //std::unique_ptr<MboProcessor> _mbo;
    //std::unique_ptr<TradeProcessor> _trades;
    //std::unique_ptr<DbProcessor> _db;
    //std::unique_ptr<BarProcessor> _bars;

    // UAT Data Simulator
    //DataSimulator _simulator;

    //SecIdSubscriber _SecSub;
    bool _DebugOnTrade;
    bool _DebugOnQuote;
    bool _DebugListSymbol;
	string _DebugSymbol;

    //std::vector<Quote> quotes;
    //std::ofstream logFile;
    OrderBookManager obm;
    uint64_t lastQuoteSeqNum, lastTradeSeqNum;

    // Market State Tracking
    //std::map<uint32_t, MarketState> _marketStates;  // symbolIndex -> MarketState
    mutable tbb::spin_mutex _marketStateMutex;
    std::string _lastMarketStateResetDate;

    // Error throttling
    std::chrono::steady_clock::time_point _lastErrorLogTime;
    QuoteSystemApiError _lastErrorCode;
    std::string _lastErrorMsg;
    int _suppressedErrorCount;

    // Reconnection support
    std::string _hostname;
    std::string _username;
    std::string _password;
    bool _isExpress;
    bool _extendedQuoteEnabled;
    bool _snapshotBookEnabled;
    std::string _quoteSource;
    std::vector<std::string> _subscribedSymbols;
    std::map<std::string, std::vector<std::string>> _subscriptionParamsMap;  // quoteSource -> symbols (for multi-source reconnection)
    QuoteSystemApi::SymbolType _symbolType;
    int _subscriptionTypeMask;
    bool _isReconnecting;
    int _retryCount;
    std::mutex _reconnectMutex;
    std::thread* _reconnectThread;

    // Reconnection methods
    void scheduleReconnect(std::string quoteSource, std::string reason);
    void attemptReconnect();
    void restoreConnection();

public:
    // Set connection credentials for reconnection
    void setConnectionCredentials(const std::string& hostname, const std::string& username,
                                  const std::string& password, bool isExpress);

    // Set subscription parameters for reconnection
    void setSubscriptionParams(const std::string& quoteSource, const std::vector<std::string>& symbols,
                              QuoteSystemApi::SymbolType symbolType, int subscriptionTypeMask);

    // Set feature flags
    void setExtendedQuoteEnabled(bool enabled) { _extendedQuoteEnabled = enabled; }
    void setSnapshotBookEnabled(bool enabled) { _snapshotBookEnabled = enabled; }
};

#endif // QUOTE_API_CLIENT_HPP_
