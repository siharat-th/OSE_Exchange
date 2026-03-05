//============================================================================
// Name        : EquityMktData.hpp
// Author      : Siharat Thammaya
// Version     : 1.0
// Description : Main equity market data handler class
//============================================================================

#ifndef SRC_EQUITYMKTDATA_HPP_
#define SRC_EQUITYMKTDATA_HPP_

#include <string>
#include <vector>
#include <memory>

#include <Settings/DataProcSettings.hpp>
#include <exchsupport/eqt/mktdata/QuoteApiClient.hpp>

/**
 * @brief Main equity market data handler class
 *
 * Encapsulates all market data connection and subscription logic.
 * Usage:
 *   EquityMktData mktData;
 *   mktData.Start();
 */
class EquityMktData {
public:
    EquityMktData();
    ~EquityMktData();

    /**
     * @brief Start the market data handler
     *
     * Loads settings from config file, connects to quote server,
     * subscribes to symbols, and runs until interval expires.
     *
     * @return 0 on success, non-zero on error
     */
    int Start();

    /**
     * @brief Start asynchronously (non-blocking, spawns internal thread)
     */
    void StartAsync();

    /**
     * @brief Start with command line argument overrides
     *
     * @param argc Argument count from main()
     * @param argv Argument values from main()
     * @return 0 on success, non-zero on error
     */
    int Start(int argc, char* argv[]);

private:
    void LogMe(const std::string& msg, int color = LOG_WHITE);

    /**
     * @brief Apply feature flags to quote system
     */
    void ApplyFeatureFlags();

    /**
     * @brief Perform login with retry logic
     * @return true on success, false on failure
     */
    bool Login();

    /**
     * @brief Subscribe to symbols
     * @return 0 on success, negative on error
     */
    int Subscribe();

private:
    DataProcSettings _settings;
    std::unique_ptr<QuoteApiClient> _quoteSystem;

    // Connection settings (can be overridden by command line)
    std::string _hostname;
    std::string _username;
    std::string _password;
    std::string _quoteSource;

    // Feature flags (can be overridden by command line)
    bool _isExpress;
    bool _blockingLogin;
    bool _enableTopOfBook;
    bool _enableAggrByPrice;
    bool _enableExtQuote;
    bool _enableTradesOnly;
    bool _enableSnapshot;
    bool _subscribeAll;
    bool _printData;
    int _runInterval;

    // Subscription settings
    std::vector<std::string> _symbols;
    LimeBrokerage::QuoteSystemApi::SymbolType _symbolType;
    LimeBrokerage::QuoteSystemApi::SubscriptionType _subscriptionType;
};

#endif /* SRC_EQUITYMKTDATA_HPP_ */
