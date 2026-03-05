//============================================================================
// Name        : EquityMktData.cpp
// Author      : Siharat Thammaya
// Version     : 1.0
// Description : Main equity market data handler implementation
//============================================================================

#include "EquityMktData.hpp"
#include <KT01/Core/Log.hpp>
#include <KT01/SecDefs/EquitySecMaster.hpp>
#include <thread>
#include <chrono>

using namespace LimeBrokerage;

EquityMktData::EquityMktData()
    : _quoteSystem(nullptr)
    , _isExpress(false)
    , _blockingLogin(true)
    , _enableTopOfBook(false)
    , _enableAggrByPrice(false)
    , _enableExtQuote(false)
    , _enableTradesOnly(false)
    , _enableSnapshot(false)
    , _subscribeAll(false)
    , _printData(true)
    , _runInterval(-1)
    , _symbolType(QuoteSystemApi::symbolTypeNormal)
    , _subscriptionType(QuoteSystemApi::subscriptionTypeMarketData)
{
}

EquityMktData::~EquityMktData()
{
    if (_quoteSystem) {
        _quoteSystem->unsubscribeAll(_quoteSource);
        _quoteSystem->logout();
    }
}

void EquityMktData::LogMe(const std::string& msg, int color)
{
    AKL_LOG(msg, "EQMD", color);
}

int EquityMktData::Start()
{
    // Load settings from config file
    _settings.Init();

    // Use config file values as defaults
    _printData = _settings.RunPrintData;
    _hostname = _settings.ConnectionServer;
    _username = _settings.ConnectionUsername;
    _password = _settings.ConnectionPassword;
    _quoteSource = _settings.ConnectionQuoteSource;

    _isExpress = _settings.FeatureExpress;
    _blockingLogin = _settings.FeatureBlockingLogin;
    _enableTopOfBook = _settings.FeatureTopOfBook;
    _enableAggrByPrice = _settings.FeatureAggregateByPrice;
    _enableExtQuote = _settings.FeatureExtendedQuote;
    _enableTradesOnly = _settings.FeatureTradesOnly;
    _enableSnapshot = _settings.FeatureSnapshotBook;
    _subscribeAll = _settings.FeatureSubscribeAll;
    _runInterval = _settings.RunInterval;

    // Create quote system
    _quoteSystem = std::make_unique<QuoteApiClient>(_printData);

    if (_isExpress) {
        std::cout << "Login with Express session." << std::endl;
    }

    // Login
    if (!Login()) {
        return 1;
    }

    // Store connection credentials for auto-reconnection
    _quoteSystem->setConnectionCredentials(_hostname, _username, _password, _isExpress);

    // Apply feature flags
    ApplyFeatureFlags();

    // Subscribe
    int sub_result = Subscribe();
    if (sub_result < 0) {
        std::cout << "Error subscribing." << std::endl;
        return 1;
    }

    // Run until interval expires
    LogMe("Running... (interval=" + std::to_string(_runInterval) + "s)", LOG_GREEN);
    Utility::sleep(_runInterval);

    // Cleanup
    _quoteSystem->unsubscribeAll(_quoteSource);
    _quoteSystem->logout();

    return 0;
}

void EquityMktData::StartAsync()
{
    std::thread([this]() {
        this->Start();
    }).detach();
}

int EquityMktData::Start(int argc, char* argv[])
{
    // Load settings from config file first
    _settings.Init();

    // Process command line arguments
    cmdLineDefinition.processCmd(argc, argv);

    if (cmdLineDefinition.getBoolValue(VERSION_ARG())) {
        QuoteApiClient quoteSystem(false);
        std::cout << "API build version: " << quoteSystem.getVersion() << std::endl;
        return 0;
    }

    // Use config file values as defaults, command line overrides if provided
    _printData = _settings.RunPrintData;
    if (cmdLineDefinition.isOptionSet(DONT_PRINT_ARG())) {
        _printData = false;
    }

    // Connection settings: command line overrides config file
    _hostname = cmdLineDefinition.isOptionSet(SERVER_NAME_ARG())
        ? cmdLineDefinition.getStringValue(SERVER_NAME_ARG())
        : _settings.ConnectionServer;

    _username = cmdLineDefinition.isOptionSet(USERNAME_ARG())
        ? cmdLineDefinition.getStringValue(USERNAME_ARG())
        : _settings.ConnectionUsername;

    _password = cmdLineDefinition.isOptionSet(PASSWORD_ARG())
        ? cmdLineDefinition.getStringValue(PASSWORD_ARG())
        : _settings.ConnectionPassword;

    _quoteSource = cmdLineDefinition.isOptionSet(QUOTE_SOURCE_ARG())
        ? cmdLineDefinition.getStringValue(QUOTE_SOURCE_ARG())
        : _settings.ConnectionQuoteSource;

    // Feature flags: command line overrides config file
    _isExpress = cmdLineDefinition.isOptionSet(EXPRESS_ARG())
        ? cmdLineDefinition.getBoolValue(EXPRESS_ARG())
        : _settings.FeatureExpress;

    _blockingLogin = cmdLineDefinition.isOptionSet(NON_BLOCKING_LOGIN_ARG())
        ? !cmdLineDefinition.getBoolValue(NON_BLOCKING_LOGIN_ARG())
        : _settings.FeatureBlockingLogin;

    _enableTopOfBook = cmdLineDefinition.isOptionSet(TOP_BOOK_ARG())
        ? cmdLineDefinition.getBoolValue(TOP_BOOK_ARG())
        : _settings.FeatureTopOfBook;

    _enableAggrByPrice = cmdLineDefinition.isOptionSet(AGGR_BY_PRICE_ARG())
        ? cmdLineDefinition.getBoolValue(AGGR_BY_PRICE_ARG())
        : _settings.FeatureAggregateByPrice;

    _enableExtQuote = cmdLineDefinition.isOptionSet(EXT_QUOTE_ARG())
        ? cmdLineDefinition.getBoolValue(EXT_QUOTE_ARG())
        : _settings.FeatureExtendedQuote;

    _enableTradesOnly = cmdLineDefinition.isOptionSet(TRADES_ONLY_ARG())
        ? cmdLineDefinition.getBoolValue(TRADES_ONLY_ARG())
        : _settings.FeatureTradesOnly;

    _enableSnapshot = cmdLineDefinition.isOptionSet(SNAPSHOT_ARG())
        ? cmdLineDefinition.getBoolValue(SNAPSHOT_ARG())
        : _settings.FeatureSnapshotBook;

    _subscribeAll = cmdLineDefinition.isOptionSet(SUBSCRIBE_ALL_ARG())
        ? cmdLineDefinition.getBoolValue(SUBSCRIBE_ALL_ARG())
        : _settings.FeatureSubscribeAll;

    _runInterval = cmdLineDefinition.isOptionSet(INTERVAL_ARG())
        ? cmdLineDefinition.getIntValue(INTERVAL_ARG())
        : _settings.RunInterval;

    // Symbol type from command line
    _symbolType = QuoteSystemApi::symbolTypeNormal;
    if (cmdLineDefinition.getBoolValue(SUBSCRIBE_BY_UNDERLYING_ARG())) {
        _symbolType = QuoteSystemApi::symbolTypeUnderlying;
    }
    else if (cmdLineDefinition.getBoolValue(SUBSCRIBE_INDEX_ARG())) {
        _symbolType = QuoteSystemApi::symbolTypeIndex;
    }
    else if (cmdLineDefinition.getBoolValue(SUBSCRIBE_BY_OSI_ARG())) {
        _symbolType = QuoteSystemApi::symbolTypeUnderlyingAttributes;
    }

    // Subscription type from command line
    _subscriptionType = QuoteSystemApi::subscriptionTypeMarketData;
    if (cmdLineDefinition.getBoolValue(SUBSCRIBE_ATTR_ARG())) {
        _subscriptionType = QuoteSystemApi::subscriptionTypeAttributes;
    }

    // Get symbols from command line
    _symbols = cmdLineDefinition.getMandatoryAttributes();

    // Create quote system
    _quoteSystem = std::make_unique<QuoteApiClient>(_printData);

    if (_isExpress) {
        std::cout << "Login with Express session." << std::endl;
    }

    // Login
    if (!Login()) {
        return 1;
    }

    // Store connection credentials for auto-reconnection
    _quoteSystem->setConnectionCredentials(_hostname, _username, _password, _isExpress);

    // MMID filter from command line
    if (cmdLineDefinition.getStringValue(MMID_FILTER_ARG()).size() > 0) {
        std::istringstream is(cmdLineDefinition.getStringValue(MMID_FILTER_ARG()));
        std::vector<std::string> filters((std::istream_iterator<std::string>(is)), (std::istream_iterator<std::string>()));
        std::cout << filters.size() << " MMID Filters: " << cmdLineDefinition.getStringValue(MMID_FILTER_ARG()) << std::endl;
        _quoteSystem->filterMMID(true, filters);
    }

    if (cmdLineDefinition.getBoolValue(IPO_AND_UPLISTED_ARG())) {
        _quoteSystem->enableIpoAndUplisted();
    }

    if (cmdLineDefinition.getBoolValue(ROUND_LOT_TOB_ARG())) {
        _quoteSystem->enableRoundLotTob();
    }

    // Apply feature flags
    ApplyFeatureFlags();

    // Subscribe
    int sub_result = Subscribe();
    if (sub_result < 0) {
        std::cout << "Error subscribing." << std::endl;
        return 1;
    }

    // Run until interval expires
    LogMe("Running... (interval=" + std::to_string(_runInterval) + "s)", LOG_GREEN);
    Utility::sleep(_runInterval);

    // Cleanup
    _quoteSystem->unsubscribeAll(_quoteSource);
    _quoteSystem->logout();

    return 0;
}

bool EquityMktData::Login()
{
    int res = 0;

    if (_blockingLogin == false) {
        int i = 0;
        do {
            res = _quoteSystem->login(_hostname, _username, _password, _blockingLogin, _isExpress);
            if (res == QuoteSystemApi::LOGIN_FAILURE) {
                std::cout << "Error logging in." << std::endl;
                return false;
            }
            i++;
        } while (res == QuoteSystemApi::LOGIN_PENDING);
        std::cout << "Login took " << i << " iterations." << std::endl;
    }
    else {
        // Retry login with exponential backoff
        const int backoffDelays[] = {5, 10, 20, 40, 80, 160, 320, 600}; // seconds
        const int maxBackoffIndex = sizeof(backoffDelays) / sizeof(backoffDelays[0]) - 1;
        int loginAttempt = 0;
        bool loginSuccess = false;

        while (!loginSuccess) {
            if (loginAttempt > 0) {
                int backoffIndex = (loginAttempt - 1 < maxBackoffIndex) ? (loginAttempt - 1) : maxBackoffIndex;
                int delaySeconds = backoffDelays[backoffIndex];

                std::cout << "Login attempt #" << loginAttempt << " failed. Retrying in "
                          << delaySeconds << " seconds..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
            }

            LogMe("Logging in...", LOG_CYAN);
            res = _quoteSystem->login(_hostname, _username, _password, _blockingLogin, _isExpress);

            if (res == QuoteSystemApi::LOGIN_FAILURE) {
                loginAttempt++;
            }
            else {
                loginSuccess = true;
                std::cout << "Login successful!" << std::endl;
            }
        }
    }

    return true;
}

void EquityMktData::ApplyFeatureFlags()
{
    if (_enableTopOfBook) {
        _quoteSystem->enableTopOfBook();
    }

    if (_enableAggrByPrice) {
        _quoteSystem->enableAggregateByPrice();
    }

    if (_enableExtQuote) {
        LogMe("Enabling extended quote", LOG_CYAN);
        _quoteSystem->enableExtendedQuote();
        _quoteSystem->setExtendedQuoteEnabled(true);
    }

    if (_enableTradesOnly) {
        _quoteSystem->enableTradesOnly();
    }

    if (_enableSnapshot) {
        LogMe("Enabling snapshot book", LOG_CYAN);
        _quoteSystem->enableSnapshotBook();
        _quoteSystem->setSnapshotBookEnabled(true);
    }
    else {
        LogMe("Disabling snapshot book", LOG_YELLOW);
    }
}

int EquityMktData::Subscribe()
{
    int sub_result = 0;

    if (_settings.FeatureSubscribeFromSecdef) {
        // Subscribe from secdef - multiple quote sources with specific symbols
        auto venues = SECMASTER_EQT->getUniqueVenues();

        for (const auto& venue : venues) {
            auto symbols = SECMASTER_EQT->getBaseSymbolsForVenue(venue);
            string cqs = SECMASTER_EQT->getCQSFromVenue(venue);
            if (!symbols.empty()) {
                LogMe("Subscribing to " + cqs + " (venue=" + venue + ") with " + std::to_string(symbols.size()) + " symbols");
                sub_result = _quoteSystem->subscribe(cqs, symbols, _symbolType, _subscriptionType);
                _quoteSystem->setSubscriptionParams(cqs, symbols, _symbolType, _subscriptionType);
            }
        }
    }
    else if (_subscribeAll) {
        sub_result = _quoteSystem->subscribeAll(_quoteSource, _symbolType, _subscriptionType);
        // For subscribeAll, store empty symbols list (will subscribe to all on reconnect)
        _quoteSystem->setSubscriptionParams(_quoteSource, std::vector<std::string>(), _symbolType, _subscriptionType);
    }
    else {
        sub_result = _quoteSystem->subscribe(_quoteSource, _symbols, _symbolType, _subscriptionType);
        // Store subscription parameters for reconnection
        _quoteSystem->setSubscriptionParams(_quoteSource, _symbols, _symbolType, _subscriptionType);
    }

    return sub_result;
}
