#include <exchsupport/eqt/mktdata/QuoteApiClient.hpp>
#include <cstdarg>

//const float PRICE_FACTOR = 1.0f;// 10.0f;

QuoteApiClient::QuoteApiClient(bool print_quotes, string id) : printData(print_quotes), id_(id), numSymbols(0)
{
    _debugSettings = Settings("DebugSettings.txt");
    _DebugOnQuote = _debugSettings.getBoolean("Debug.onQuote");
    _DebugOnTrade = _debugSettings.getBoolean("Debug.onTrade");
    _DebugListSymbol = _debugSettings.getBoolean("Debug.symbolList");
	_DebugSymbol = _debugSettings.getString("Debug.Symbol");
    
    _settings.Init();
    _secmaster = SECMASTER_EQT;

    // Init Processor
    int cpu_core = sched_getcpu();
    if (cpu_core == -1) {
        LogMe("Error in getting CPU core.", LOG_RED);
    }
    else {
        LogMe("Running on CPU core: " + std::to_string(cpu_core), LOG_CYAN);
    }

    // Initialize error throttling
    _suppressedErrorCount = 0;
    _lastErrorCode = static_cast<QuoteSystemApiError>(-1);
    _lastErrorLogTime = std::chrono::steady_clock::now();

    // Initialize reconnection support
    _isReconnecting = false;
    _retryCount = 0;
    _reconnectThread = nullptr;
    _extendedQuoteEnabled = false;
    _snapshotBookEnabled = false;
    _isExpress = false;

    //enableSnapshotBook();

    //// Open the log file
    //logFile.open("quote_trade_log.txt", std::ios::out | std::ios::app);
    //if (!logFile.is_open()) {
    //    std::cerr << "Failed to open log file." << std::endl;
    //}
}

QuoteApiClient::~QuoteApiClient()
{
}

bool QuoteApiClient::ValidSecId(int32_t secid)
{
    return true;
}

bool QuoteApiClient::ValidMboSecId(int32_t secid)
{
    return true;
}

void QuoteApiClient::onNewSubscription(int32_t secid)
{
    LogMe("Eqt Processor: onNewSubscription: secid=" + std::to_string(secid));

    if (obm.get(secid).SnapReceived)
    {
        LogMe("PUBLISHING onNewSubscription: secid=" + std::to_string(secid), LOG_GREEN);
    }
}

/**
 * Use this method to associate symbol with symbol_index.
 */
void QuoteApiClient::onAddSymbol(std::string quoteSource, std::string& symbol, uint32_t symbolIndex, SecurityType securityType)
{
    if (!printData) { return; }

    // Construct fullSymbol from base symbol + venue (e.g., AAPL@INET)
    string venue = SECMASTER_EQT->getVenueFromCQS(quoteSource);
    string fullSymbol = symbol + "@" + venue;
    symbolIndexToName[symbolIndex] = fullSymbol;
    //std::cout << "onAddSymbol: quoteSource = " << quoteSource << ", symbol = " << symbol << ", symbolIndex = " << symbolIndex << ", securityType = " << securityType << std::endl;
    numSymbols++;

}

void QuoteApiClient::onBidQuote(std::string symbol, long id, int size, double price, std::string mmid, std::string quoteSource, uint32_t timestamp)
{
    // Construct fullSymbol from base symbol + venue (e.g., AAPL@INET)
    string venue = SECMASTER_EQT->getVenueFromCQS(quoteSource);
    string fullSymbol = symbol + "@" + venue;
    EquitySecDef def = _secmaster->getSecDef(fullSymbol);

    if (def.secid != 0) {
        // TODO: Integrate equity OrderBookManager with processBid/processAsk
        //obm.processBid(def.secid, (uint32_t)id, price * def.priceFactor, size, lastQuoteSeqNum);
    }
}

void QuoteApiClient::onAskQuote(std::string symbol, long id, int size, double price, std::string mmid, std::string quoteSource, uint32_t timestamp)
{
    // Construct fullSymbol from base symbol + venue (e.g., AAPL@INET)
    string venue = SECMASTER_EQT->getVenueFromCQS(quoteSource);
    string fullSymbol = symbol + "@" + venue;
    EquitySecDef def = _secmaster->getSecDef(fullSymbol);

    if (def.secid != 0) {
        // TODO: Integrate equity OrderBookManager with processBid/processAsk
        //obm.processAsk(def.secid, (uint32_t)id, price * def.priceFactor, size, lastQuoteSeqNum);
    }
}

void QuoteApiClient::onTradeTick(std::string symbol, int size, double price, int totalVolume, std::string exchangeId, std::string quoteSource, uint32_t timestamp, std::string tradeType)
{
    //LogMe("onTradeTick", LOG_YELLOW);
    // Construct fullSymbol from base symbol + venue (e.g., AAPL@INET)
    string venue = SECMASTER_EQT->getVenueFromCQS(quoteSource);
    string fullSymbol = symbol + "@" + venue;
    EquitySecDef def = _secmaster->getSecDef(fullSymbol);
    if (def.secid != 0 && obm.get(def.secid).SnapReceived) {
        // TODO: Integrate equity OrderBookManager with processTrade/processLastTime
        //obm.processTrade(def.secid, price * def.priceFactor, size, tradeType, lastTradeSeqNum);
        //obm.processLastTime(def.secid, timestamp);
    }
}

void QuoteApiClient::onModifyExecution(const Quote& quote, const Trade& trade)
{
    onQuote(quote);
    onTrade(trade);
}

void QuoteApiClient::onOrderReplace(const Quote& oldQuote, const Quote& newQuote)
{
    onQuote(oldQuote);
    onQuote(newQuote);
}

/**
 * Override this method to get additional information that is not passed in
 * onBidQuote and onAskQuote methods.
 *
 * Make sure you copy data from the quote object before returning from this
 * method, because it will be reused in subsequent calls.
 *
 * If you want to make sure that onBidQuote and onAskQuote methods are
 * called call QuoteSystemAPI::onQuote(quote).
 */
void QuoteApiClient::onQuote(const Quote& quote)
{
    lastQuoteSeqNum = quote.exchangeSeqNum;
    // Enable onBidQuote/onAskQuote callbacks
    QuoteSystemApi::onQuote(quote);

  //  if (inBook) {
		//quotes.push_back(quote);
  //      //return;
  //  }
  //  else {
  //      QuoteSystemApi::onQuote(quote);
  //  }

    //if (!printData) { return; }

    std::string symbol;
    map<int, std::string>::const_iterator searchResult = symbolIndexToName.find(quote.symbolIndex);
    if (searchResult != symbolIndexToName.end()) {
        symbol = symbolIndexToName[quote.symbolIndex];
    }
    double price = quote.priceMantissa * (double)pow(10.0, quote.priceExponent);
    uint32_t timestamp = quote.timestamp;

    string side = "Ask";
    if (quote.isBid) {
        side = "Bid";
    }

    int ms = timestamp % 1000;
    timestamp /= 1000;
    int sec = timestamp % 60;
    timestamp /= 60;
    int min = timestamp % 60;
    timestamp /= 60;

    if (_DebugOnQuote && _DebugSymbol==symbol) {
        ostringstream oss;
        oss << "onQuote: symbol = " << symbol
            << ", symbolIndex = " << quote.symbolIndex
            << ", side = " << side
            << ", id = " << quote.id
            << ", exchangeOrderId = " << quote.exchangeOrderId
            << ", size = " << quote.size
            << ", price = " << std::showpoint << std::fixed << std::setprecision(4) << price
            << ", mmid = " << quote.mmid
            << ", quoteSource = " << quote.quoteSource
            << ", timestamp = " << quote.timestamp
            << ", nsecTimestamp = " << quote.nsecTimestamp
            << setfill('0') << ", time = " << setw(2) << timestamp << ":" << setw(2) << min << ":" << setw(2) << sec << "." << setw(3) << ms
            << ", quoteProperties = " << (char)quote.quoteProperties
            << ", feedSpecificData = " << (char)(quote.feedSpecificData)
            << (quote.isNBBO ? ", NBBO" : "")
            << ", citriusTimestamp = " << quote.citriusTimestamp
            << ", exchangeSeqNum = " << quote.exchangeSeqNum;
        LogMe(oss.str(), LOG_CYAN);
    }

    // Construct fullSymbol from base symbol + venue (e.g., AAPL@INET)
    string venue = SECMASTER_EQT->getVenueFromCQS(quote.quoteSource);
    string fullSymbol = symbol + "@" + venue;
    EquitySecDef def = SECMASTER_EQT->getSecDef(fullSymbol);
    if (def.secid != 0 && obm.get(def.secid).SnapReceived) {
        // TODO: Integrate equity OrderBookManager with processBid/processAsk
        //uint32_t orderId = quote.id;
        //if (quote.isBid) {
        //    obm.processBid(def.secid, orderId, price * def.priceFactor, quote.size, quote.exchangeSeqNum);
        //} else {
        //    obm.processAsk(def.secid, orderId, price * def.priceFactor, quote.size, quote.exchangeSeqNum);
        //}
    }
}

enum EnumAggressorSide : uint8_t
{
    None = 0,
    Buy = 1,
    Sell = 2
};

/**
 * Override this method to get additional information that is not passed in
 * onTradeTick method.
 *
 * Make sure you copy data from the trade object before returning from this
 * method, because it will be reused in subsequent calls.
 */
void QuoteApiClient::onTrade(const Trade& trade)
{
    lastTradeSeqNum = trade.exchangeSeqNum;
    // Uncomment next line to receive onTradeTick callbacks.
    QuoteSystemApi::onTrade(trade);

    if (!printData) { return; }

    std::string symbol;
    map<int, std::string>::const_iterator searchResult = symbolIndexToName.find(trade.symbolIndex);
    if (searchResult != symbolIndexToName.end()) {
        symbol = symbolIndexToName[trade.symbolIndex];
    }
    double price = trade.priceMantissa * (double)pow(10.0, trade.priceExponent);

    if (_DebugOnTrade && _DebugSymbol == symbol) {
        ostringstream oss;
        oss << "onTrade: symbol = " << symbol
            << ", symbolIndex = " << trade.symbolIndex
            << ", size = " << trade.size
            << ", price = " << std::showpoint << std::fixed << std::setprecision(4) << price
            << ", totalVolume = " << trade.totalVolume
            << ", tradeId: " << trade.tradeId
            << ", exchangeId = " << trade.exchangeId
            << ", quoteSource = " << trade.quoteSource
            << ", timestamp = " << trade.timestamp
            << ", nsecTimestamp = " << trade.nsecTimestamp
            << ", tradeType = " << trade.tradeType
            << ", tradeProperties = " << (int)trade.tradeProperties[0]
            << ":" << (int)trade.tradeProperties[1]
            << ":" << (int)trade.tradeProperties[2]
            << ":" << (int)trade.tradeProperties[3]
            << ", citriusTimestamp = " << trade.citriusTimestamp
            << ", exchangeSeqNum = " << trade.exchangeSeqNum
            << ((trade.isHigh) ? " HIGH" : "")
            << ((trade.isLow) ? " LOW" : "")
            << ((trade.isCancelled) ? " CANCELLED" : "")
            << ((trade.isCorrected) ? " CORRECTED" : "")
            << ((trade.isPreviousSessionLastTrade) ? " PREVIOUS SESSION LAST" : "")
            << ((trade.isLast) ? " LAST " : "")
            << ((trade.isOpen) ? " OPEN " : "")
            << ((trade.isClose) ? " CLOSE " : "");
        LogMe(oss.str(), LOG_CYAN);
    }

}

void QuoteApiClient::onRoundLotTob(const RoundLotTob& quote)
{
    static uint32_t askSize = 0;
    static LimeBrokerage::Price askPrice = LimeBrokerage::Price(-1, 4);
    static uint32_t bidSize = 0;
    static LimeBrokerage::Price bidPrice = LimeBrokerage::Price(-1, 4);
    string side = quote.getSide() == LimeBrokerage::BUY ? "Bid" : "Ask";
    ostringstream oss;
    oss << "onRoundLotTob: side = " << side
        << ", timestamp = " << quote.getTimestamp().getNsecsSinceMidnight()
        << ", PriceAggregatedTob = " << quote.getPriceAggregatedTobSize() << " @ " << std::showpoint << std::fixed << std::setprecision(4) << quote.getPriceAggregatedTobPrice()
        << ", OddLotAggregatedTob = " << quote.getOddLotAggregatedTobSize() << " @ " << std::showpoint << std::fixed << std::setprecision(4) << quote.getOddLotAggregatedTobPrice()
        << ", OddLotFilteredTob = " << quote.getOddLotFilteredTobSize() << " @ " << std::showpoint << std::fixed << std::setprecision(4) << quote.getOddLotFilteredTobPrice();
    LogMe(oss.str());

}

void QuoteApiClient::onAuctionUpdate(const AuctionUpdate& update)
{
    ostringstream oss;
    oss << "onAuctionUpdate: symbolIndex = " << update.symbolIndex
        << ", timestamp = " << update.timestamp
        << ", quoteSource = " << update.quoteSourceId
        << ", auctionType = " << update.auctionType
        << ", referencePrice = " << priceToDouble(update.referencePriceMantissa, update.referencePriceExponent)
        << ", buyShares = " << update.buyShares
        << ", sellShares = " << update.sellShares
        << ", indicativePrice = " << priceToDouble(update.indicativePriceMantissa, update.indicativePriceExponent)
        << ", auctionOnlyPrice= " << priceToDouble(update.auctionOnlyPriceMantissa, update.auctionOnlyPriceExponent);
    LogMe(oss.str());
}

void QuoteApiClient::onAuctionSummary(const AuctionSummary& summary)
{
    ostringstream oss;
    oss << "onAuctionSummary: symbolIndex = " << summary.symbolIndex
        << ", timestamp = " << summary.timestamp
        << ", quoteSource = " << summary.quoteSourceId
        << ", auctionType = " << summary.auctionType
        << ", shares = " << summary.shares
        << ", price = " << priceToDouble(summary.priceMantissa, summary.priceExponent);
    LogMe(oss.str());
}

/**
 * Override this method to get Market Imbalance information.
 */
void QuoteApiClient::onMarketImbalance(const MarketImbalance& mi)
{
    if (!printData) { return; }
    ostringstream oss;
    oss << "onMarketImbalance: symbol: " << getSymbol(mi.symbolIndex)
        << ", side: " << ((mi.side == LimeBrokerage::BUY) ? "BUY" : "SELL")
        << ", paired shares: " << mi.pairedShares
        << ", referece price: " << priceToDouble(mi.priceMantissa, mi.priceExponent)
        << ", quoteSource: " << mi.quoteSource
        << ", symbol: " << getSymbol(mi.symbolIndex)
        << ", MarketImbalance: " << int(mi.marketImbalance)
        << ", TotalImbalance: " << int(mi.totalImbalance)
        << ", ImbalanceType: " << int(mi.imbalanceType)
        << ", ClosingClearingPrice: " << priceToDouble(mi.closingClearingPriceNumerator, mi.priceExponent)
        << ", feedSpecificData *hex*: " << hex << mi.feedSpecificData << dec
        << ", exchangeId: " << mi.exchangeId
        << ", timestamp: " << mi.timestamp
        << ", nsecTimestamp: " << mi.nsecTimestamp;
    LogMe(oss.str());
}

/**
 * Override this methdod to get LULD Price Band information.
 */
void QuoteApiClient::onPriceBand(const PriceBand& priceBand)
{
    if (!printData) { return; }
    ostringstream oss;
    oss << "onPriceBand: symbol = " << getSymbol(priceBand.getSymbolIndex())
        << ", symbolIndex = " << priceBand.getSymbolIndex()
        << ", quoteSourceId = " << priceBand.getQuoteSourceId()
        << ", limitDownPrice = " << priceBand.getLimitDownPrice().toDouble()
        << ", limitUpPrice = " << priceBand.getLimitUpPrice().toDouble()
        << ", effectivetimestamp = " << priceBand.getEffectiveTimestamp().getNsecsSinceMidnight() / 1000000
        << ", timestamp =  " << priceBand.getTimestamp().getNsecsSinceMidnight() / 1000000;
    LogMe(oss.str());
    return;
}

void QuoteApiClient::onFeatureControlResponse(int featureCode, int response)
{
    std::string featureStr("unknown");
    switch (featureCode)
    {
    case LimeBrokerage::FEATURE_TRADE_REPLAY:
        featureStr.assign("trade replay");
        break;
    case LimeBrokerage::FEATURE_TRADES_ONLY:
        featureStr.assign("trades only");
        break;
    case LimeBrokerage::FEATURE_TOP_OF_BOOK:
        featureStr.assign("top of book");
        break;
    case LimeBrokerage::FEATURE_PRICE_AGGREGATED:
        featureStr.assign("price aggregated");
        break;
    case LimeBrokerage::FEATURE_SNAPSHOT_BOOK:
        featureStr.assign("snapshot book");
        break;
    case LimeBrokerage::FEATURE_ROUND_LOT_TOB:
        featureStr.assign("round lot tob");
        break;
    case LimeBrokerage::FEATURE_FILTER_MMID:
        featureStr.assign("mmid filter");
        break;
    }

    std::string responseStr;
    switch (response)
    {
    case LimeBrokerage::FEATURE_CONTROL_RESPONSE_ENABLED:
        responseStr.assign("enabled");
        break;
    case LimeBrokerage::FEATURE_CONTROL_RESPONSE_DISABLED:
        responseStr.assign("disabled");
        break;
    }

    ostringstream oss;
    oss << "<" << featureStr << "> " << responseStr << ".";
    LogMe(oss.str());
}

static const string getTradingStatusString(const int code)
{
    switch (code)
    {
    case LimeBrokerage::QuoteSystemApi::alertRegShoUpdate:
        return "alertRegShoUpdate";
    case LimeBrokerage::QuoteSystemApi::alertTradingHalt:
        return "alertTradingHalt";
    case LimeBrokerage::QuoteSystemApi::alertResume:
        return "alertResume";
    case LimeBrokerage::QuoteSystemApi::alertQuoteResume:
        return "alertQuoteResume";
    case LimeBrokerage::QuoteSystemApi::alertOpeningDelay:
        return "alertOpeningDelay";
    case LimeBrokerage::QuoteSystemApi::alertNoOpenNoResume:
        return "alertNoOpenNoResume";
    case LimeBrokerage::QuoteSystemApi::alertPriceIndication:
        return "alertPriceIndication";
    case LimeBrokerage::QuoteSystemApi::alertTradingRangeIndication:
        return "alertTradingRangeIndication";
    case LimeBrokerage::QuoteSystemApi::alertMarketImbalanceBuy:
        return "alertMarketImbalanceBuy";
    case LimeBrokerage::QuoteSystemApi::alertMarketImbalanceSell:
        return "alertMarketImbalanceSell";
    case LimeBrokerage::QuoteSystemApi::alertMarketOnCloseImbalanceBuy:
        return "alertMarketOnCloseImbalanceBuy";
    case LimeBrokerage::QuoteSystemApi::alertMarketOnCloseImbalanceSell:
        return "alertMarketOnCloseImbalanceSell";
    case LimeBrokerage::QuoteSystemApi::alertNoMarketImbalance:
        return "alertNoMarketImbalance";
    case LimeBrokerage::QuoteSystemApi::alertNoMarketOnCloseImbalance:
        return "alertNoMarketOnCloseImbalance";
    case LimeBrokerage::QuoteSystemApi::alertPause:
        return "alertPause";
    }
    return "alertUnknown";
}

static const string getShortSaleIndicatorString(const int code)
{
    switch (code)
    {
    case LimeBrokerage::QuoteSystemApi::shortSaleRestrictionActivated:
        return "SHORT_SALE_RESTRICTION_ACTIVATED";
    case LimeBrokerage::QuoteSystemApi::shortSaleRestrictionInEffect:
        return "SHORT_SALE_RESTRICTION_IN_EFFECT";
    case LimeBrokerage::QuoteSystemApi::shortSaleRestrictionContinued:
        return "SHORT_SALE_RESTRICTION_CONTINUED";
    case LimeBrokerage::QuoteSystemApi::shortSaleRestrictionDeactivated:
        return "SHORT_SALE_RESTRICTION_DEACTIVATED";
    case LimeBrokerage::QuoteSystemApi::shortSaleRestrictionNotInEffect:
        return "SHORT_SALE_RESTRICTION_NOT_IN_EFFECT";
    case LimeBrokerage::QuoteSystemApi::tradingStatusUpdate:
        return "TRADING_STATUS_UPDATE";
    }
    return "UNKNOWN";
}

void QuoteApiClient::onQuoteAlert(const QuoteAlert& alert)
{
    // Map alert codes to market states
    char newState = 0;
    std::string reason;

    switch (alert.code) {
        // ──── HALTED (H) ────
        case alertTradingHalt:
            newState = 'H';
            reason = "TradingHalt";
            break;
        case alertExchangeTradingHalt:
            newState = 'H';
            reason = "ExchangeTradingHalt";
            break;
        case alertPause:
            newState = 'H';
            reason = "Pause";
            break;

        // ──── TRADING (T) ────
        case alertResume:
            newState = 'T';
            reason = "Resume";
            break;
        case alertQuoteResume:
            newState = 'T';
            reason = "QuoteResume";
            break;
        case alertExchangeQuoteResume:
            newState = 'T';
            reason = "ExchangeQuoteResume";
            break;

        // ──── SUSPENDED (S) ────
        case alertOpeningDelay:
            newState = 'S';
            reason = "OpeningDelay";
            break;
        case alertNoOpenNoResume:
            newState = 'S';
            reason = "NoOpenNoResume";
            break;

        default:
            // Ignore other alerts
            break;
    }

    if (newState != 0) {
        updateMarketState(alert.symbolIndex, newState, reason, alert.quoteSource);
    }

    // Existing log code
    if (!printData) { return; }
    ostringstream oss;
    oss << "onQuoteAlert: "
        << "symbol = " << getSymbol(alert.symbolIndex)
        << ", symbolIndex = " << alert.symbolIndex
        << ", timestamp = " << alert.timestamp
        << ", nsecTimestamp = " << alert.nsecTimestamp
        << ", quoteSource = " << alert.quoteSource
        << ", code = " << alert.code << " (" << getTradingStatusString(alert.code) << ")"
        << ", shortSaleRestrictionIndicator = " << (int)alert.shortSaleRestrictionIndicator
        << " (" << getShortSaleIndicatorString(alert.shortSaleRestrictionIndicator) << ")";
    LogMe(oss.str(), LOG_YELLOW);
}

void QuoteApiClient::onQuoteBookRebuildStarted(std::string quoteSource, std::string symbol)
{
    // Construct fullSymbol from base symbol + venue (e.g., AAPL@INET)
    string venue = SECMASTER_EQT->getVenueFromCQS(quoteSource);
    string fullSymbol = symbol + "@" + venue;
    EquitySecDef def = SECMASTER_EQT->getSecDef(fullSymbol);
    if (def.secid != 0) {
        ostringstream os;
        os << id_ << ": Rebuild started: " << fullSymbol;
        LogMe(os.str(), LOG_GREEN);

        // TODO: Integrate equity OrderBookManager
        //obm.clearOrders(def.secid);
        //obm.setSnapReceived(def.secid, false);
    }
}

void QuoteApiClient::onQuoteBookRebuildComplete(std::string quoteSource, std::string symbol)
{
    // Construct fullSymbol from base symbol + venue (e.g., AAPL@INET)
    string venue = SECMASTER_EQT->getVenueFromCQS(quoteSource);
    string fullSymbol = symbol + "@" + venue;
    EquitySecDef def = SECMASTER_EQT->getSecDef(fullSymbol);
    if (def.secid != 0) {
        // TODO: Integrate equity OrderBookManager
        //obm.rebuildPriceLevels(def.secid, BookSide::Bid, 0);
        //obm.rebuildPriceLevels(def.secid, BookSide::Ask, 0);
        //obm.setSnapReceived(def.secid, true);
        //obm.setSecId(def.secid);

        ostringstream os;
        os << id_ << ": Rebuild complete: " << fullSymbol;
        LogMe(os.str(), LOG_GREEN);
    }
}

void QuoteApiClient::onQuoteStreamConnectionStatus(std::string quoteSource, QuoteStreamStatus status, std::string reason)
{
    string statusStr = "unknown";
    int logColor = LOG_GREEN;

    if (status == quoteServerConnected) {
        statusStr = "quoteServerConnected";
    }
    else if (status == quoteServerDisconnected) {
        statusStr = "quoteServerDisconnected";
        logColor = LOG_RED;
        // Trigger reconnection
        scheduleReconnect(quoteSource, reason);
    }
    if (status == quoteStreamConnected) {
        statusStr = "quoteStreamConnected";
    }
    else if (status == quoteStreamDisconnected) {
        statusStr = "quoteStreamDisconnected";
        logColor = LOG_RED;
    }
    ostringstream oss;
    oss << id_ << ": onQuoteStreamConnectionStatus: quoteSource = " << quoteSource << ", status = " << statusStr << "(" << status << "), reason = \"" << reason << "\"";
    LogMe(oss.str(), logColor);
}

void QuoteApiClient::onQuoteSystemApiError(std::string quoteSource, QuoteSystemApiError errorCode, std::string errorMsg)
{
    const int THROTTLE_SECONDS = 60; // Log same error every 60 seconds

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - _lastErrorLogTime).count();

    bool isDifferentError = (errorCode != _lastErrorCode || errorMsg != _lastErrorMsg);

    if (isDifferentError || elapsed >= THROTTLE_SECONDS) {
        // Log suppressed count if any
        if (_suppressedErrorCount > 0) {
            ostringstream suppMsg;
            suppMsg << id_ << ": Previous error repeated " << _suppressedErrorCount << " times";
            LogMe(suppMsg.str(), LOG_YELLOW);
        }

        // Log the current error
        ostringstream oss;
        oss << id_ << ": onQuoteSystemApiError: quoteSource = " << quoteSource
            << ", errorCode = " << errorCode << ", errorMsg = \"" << errorMsg << "\"";
        LogMe(oss.str(), LOG_RED);

        // Reset tracking
        _lastErrorLogTime = now;
        _lastErrorCode = errorCode;
        _lastErrorMsg = errorMsg;
        _suppressedErrorCount = 0;
    } else {
        // Suppress this error, just count it
        _suppressedErrorCount++;
    }
}

void QuoteApiClient::onSpreadDefinition(const LimeBrokerage::SpreadDefinition& spread)
{
    if (!printData) { return; }
    ostringstream oss;
    oss << const_cast<LimeBrokerage::SpreadDefinition&>(spread);
    LogMe(oss.str());
}

void QuoteApiClient::onOptionAttributes(const OptionAttributes& optionAttributes)
{
    if (!printData) { return; }
    ostringstream oss;
    oss << "onOptionAttributes: underlyingName = " << std::string(optionAttributes.underlyingName.name, OPTION_SYMBOL_LEN)
        << ", optionName = " << std::string(optionAttributes.optionName.symbol, OPTION_NAME_LEN)
        << ", opraName = " << std::string(optionAttributes.opraName.name, OPRA_NAME_LEN)
        << ", openInterest = " << optionAttributes.openInterest
        << ", quotingExchanges = " << exchangeMaskToString(optionAttributes.quotingExchanges)
        << ", symbolIndex = " << optionAttributes.symbolIndex
        << ", underlyingIndex = " << optionAttributes.underlyingIndex
        << ", seriesIndex = " << optionAttributes.seriesIndex;
    LogMe(oss.str());
}

void QuoteApiClient::onMulticastLoss(std::string quoteSource, uint16_t channelId, uint32_t totalSkippedMessages, uint32_t timestamp)
{
    ostringstream oss;
    oss << id_ << ": onMulticastLoss: quoteSource = " << quoteSource
        << ", channel = " << channelId
        << ", totalSkippedMessages = " << totalSkippedMessages
        << ", timestamp(of most recent loss event) = " << timestamp;
    LogMe(oss.str(), LOG_RED);

    // Rebuild books by unsubscribe + resubscribe (faster than full reconnection)
    // According to Lime API docs: multicast loss can cause inconsistent books
    // (stale orders, incorrect quantities, missing updates). Book rebuild required.
    LogMe("Rebuilding books for " + quoteSource + " due to multicast loss...", LOG_YELLOW);

    // Only rebuild the affected quote source (not all sources)
    auto it = _subscriptionParamsMap.find(quoteSource);
    if (it != _subscriptionParamsMap.end() && !it->second.empty()) {
        const std::vector<std::string>& symbols = it->second;

        // Unsubscribe only this quote source
        LogMe("Unsubscribing " + quoteSource + "...", LOG_CYAN);
        unsubscribeAll(quoteSource);

        // Brief pause before resubscribing
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Resubscribe to trigger book rebuild for this source only
        ostringstream os;
        os << "Re-subscribing to " << symbols.size() << " symbols on " << quoteSource;
        LogMe(os.str(), LOG_CYAN);

        QuoteSystemApi::subscribe(quoteSource, symbols, _symbolType, _subscriptionTypeMask);
        LogMe("Book rebuild initiated for " + quoteSource, LOG_GREEN);
    }
}

void QuoteApiClient::sendDailyStat(const EquitySecDef& def, char updateType, double price)
{
    // TODO: Implement when DataHandlers are integrated
}

// ============================================================================
// Market State Tracking Methods
// ============================================================================

void QuoteApiClient::updateMarketState(uint32_t symbolIndex, char newState, const std::string& reason, const std::string& quoteSource)
{
    std::string symbol = getSymbol(symbolIndex, securityTypeEquity);
    logMarketStateChange(symbol, 'S', newState, reason);
}

void QuoteApiClient::logMarketStateChange(const std::string& symbol,
                                           char oldState, char newState,
                                           const std::string& reason)
{
    ostringstream oss;
    oss << "MarketState: " << symbol
        << " [" << oldState << " -> " << newState << "] "
        << "(" << reason << ")";

    // Color: T=Green, H=Red, S=Yellow
    int color = (newState == 'T') ? LOG_GREEN :
                (newState == 'H') ? LOG_RED : LOG_YELLOW;

    LogMe(oss.str(), color);
}

char QuoteApiClient::getMarketState(uint32_t symbolIndex) const
{
    return 'S';  // Default = Suspended
}

// Helper: Get current date as YYYY-MM-DD string
static std::string getCurrentDate()
{
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
    return std::string(buffer);
}

void QuoteApiClient::checkAndResetMarketStates()
{
}

void QuoteApiClient::onSymbolStatus(const SymbolStatus& symbolStatus)
{
    std::string symbol = getSymbol(symbolStatus.symbolIndex, symbolStatus.symbolType);
    std::string venue = SECMASTER_EQT->getVenueFromCQS(symbolStatus.quoteSourceId);
    std::string fullSymbol = symbol + "@" + venue;
    EquitySecDef def = _secmaster->getSecDef(fullSymbol);

    // Existing logging - only for symbols in secdef
    if (!printData) { return; }
    if (def.secid == 0) { return; }  // Skip logging for symbols not in secdef
    ostringstream oss;
    oss << "onSymbolStatus: symbolIndex = " << symbol
        << ", exchangeId = '" << symbolStatus.exchangeId << "'"
        << ", highTradePrice = " << priceToDouble(symbolStatus.highTradePriceMantissa, symbolStatus.highTradePriceExponent)
        << ", lowTradePrice = " << priceToDouble(symbolStatus.lowTradePriceMantissa, symbolStatus.lowTradePriceExponent)
        << ", lastPrice = " << priceToDouble(symbolStatus.lastPriceMantissa, symbolStatus.lastPriceExponent)
        << ", openPrice = " << priceToDouble(symbolStatus.openPriceMantissa, symbolStatus.openPriceExponent)
        << ", closePrice = " << priceToDouble(symbolStatus.closePriceMantissa, symbolStatus.closePriceExponent)
        << ", closePriceDate = " << int(symbolStatus.closePriceMon) << "/" << int(symbolStatus.closePriceDay) << "/" << int(symbolStatus.closePriceYr);
    LogMe(oss.str());
}

void QuoteApiClient::onIndex(const Index& index)
{
    if (!printData) return;

    ostringstream oss;
    oss << "onIndex: symbol = " << getSymbol(index.symbolIndex, securityTypeIndex)
        << ", symbolIndex = " << index.symbolIndex
        << ", price = " << priceToDouble(index.valueMantissa, index.valueExponent)
        << ", timestamp = " << index.timestamp
        << ", nsecTimestamp = " << index.nsecTimestamp;
    LogMe(oss.str());
}

string QuoteApiClient::varArgsToString(const char* format, ...) {
    char aux_buffer[10000];
    string retStr("");

    if (nullptr != format) {
        va_list marker;

        // initialize variable arguments
        va_start(marker, format);

        // Get formatted string length adding one for nullptr
        size_t len = vsprintf(aux_buffer, format, marker) + 1;

        // Reset variable arguments
        va_end(marker);

        if (len > 0) {
            va_list args;

            // initialize variable arguments
            va_start(args, format);

            // Create a char vector to hold the formatted string.
            vector<char> buffer(len, '\0');
            vsprintf(&buffer[0], format, args);
            retStr = &buffer[0];
            va_end(args);
        }

    }

    return retStr;
}

// Reconnection methods implementation

void QuoteApiClient::scheduleReconnect(std::string quoteSource, std::string reason)
{
    std::lock_guard<std::mutex> lock(_reconnectMutex);

    // Prevent multiple reconnection threads
    if (_isReconnecting) {
        LogMe("Reconnection already in progress, skipping duplicate reconnect request", LOG_YELLOW);
        return;
    }

    _isReconnecting = true;
    ostringstream os;
    os << id_ << ": Scheduling reconnection. Source=" << quoteSource << ", Reason=" << reason;
    LogMe(os.str(), LOG_YELLOW);

    // Create detached thread for reconnection
    if (_reconnectThread != nullptr) {
        delete _reconnectThread;
    }

    _reconnectThread = new std::thread(&QuoteApiClient::attemptReconnect, this);
    _reconnectThread->detach();
}

void QuoteApiClient::attemptReconnect()
{
    const int backoffDelays[] = {5, 10, 20, 40, 80, 160, 320, 600}; // seconds
    const int maxBackoffIndex = sizeof(backoffDelays) / sizeof(backoffDelays[0]) - 1;

    _retryCount = 0;

    while (true) {
        // Calculate backoff delay
        int backoffIndex = (_retryCount < maxBackoffIndex) ? _retryCount : maxBackoffIndex;
        int delaySeconds = backoffDelays[backoffIndex];

        ostringstream os;
        os << id_ << ": Reconnection attempt #" << (_retryCount + 1)
           << " in " << delaySeconds << " seconds...";
        LogMe(os.str(), LOG_YELLOW);

        // Wait before attempting reconnection
        std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));

        // Check if already successfully reconnected
        if (loggedIn()) {
            LogMe("Already reconnected (possibly by another thread)", LOG_GREEN);
            std::lock_guard<std::mutex> lock(_reconnectMutex);
            _isReconnecting = false;
            _retryCount = 0;
            return;
        }

        // Attempt login (non-blocking)
        LogMe("Attempting to login...", LOG_CYAN);
        int loginResult = login(_hostname, _username, _password, false, _isExpress);

        if (loginResult == LOGIN_SUCCESS || loginResult == 0) {
            // Login successful - restore connection
            LogMe("Login successful! Restoring connection...", LOG_GREEN);
            restoreConnection();
            return;
        }
        else if (loginResult == LOGIN_PENDING || loginResult == 1) {
            // Poll until login completes
            LogMe("Login pending, polling...", LOG_CYAN);
            int pollCount = 0;
            const int maxPolls = 100; // 10 seconds max (100ms * 100)

            while (pollCount < maxPolls) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                if (loggedIn()) {
                    LogMe("Login completed! Restoring connection...", LOG_GREEN);
                    restoreConnection();
                    return;
                }

                pollCount++;
            }

            LogMe("Login polling timeout", LOG_RED);
        }
        else {
            // Login failed
            ostringstream os;
            os << "Login failed with result: " << loginResult;
            LogMe(os.str(), LOG_RED);
        }

        // Increment retry count and continue loop
        _retryCount++;
    }
}

void QuoteApiClient::restoreConnection()
{
    LogMe("Restoring connection features and subscriptions...", LOG_CYAN);

    // TODO: Integrate equity OrderBookManager
    //obm.checkAndClearVAPsIfNewDay();

    // Check and reset market states if new trading day
    checkAndResetMarketStates();

    // Re-enable extended quote if it was enabled
    if (_extendedQuoteEnabled) {
        LogMe("Re-enabling extended quote", LOG_CYAN);
        enableExtendedQuote();
    }

    // Re-enable snapshot book if it was enabled
    if (_snapshotBookEnabled) {
        LogMe("Re-enabling snapshot book", LOG_CYAN);
        enableSnapshotBook();
    }

    // Re-subscribe to all quote sources (multi-source support)
    {
        std::lock_guard<std::mutex> lock(_reconnectMutex);
        for (const auto& pair : _subscriptionParamsMap) {
            const std::string& qs = pair.first;
            const std::vector<std::string>& symbols = pair.second;
            if (!symbols.empty()) {
                ostringstream os;
                os << "Re-subscribing to " << symbols.size() << " symbols on source: " << qs;
                LogMe(os.str(), LOG_CYAN);
                QuoteSystemApi::subscribe(qs, symbols, _symbolType, _subscriptionTypeMask);
            }
        }
    }

    // Reset reconnection state
    {
        std::lock_guard<std::mutex> lock(_reconnectMutex);
        _isReconnecting = false;
        _retryCount = 0;
    }

    LogMe("Connection fully restored!", LOG_GREEN);
}

// Set connection credentials for reconnection
void QuoteApiClient::setConnectionCredentials(const std::string& hostname, const std::string& username,
                                              const std::string& password, bool isExpress)
{
    _hostname = hostname;
    _username = username;
    _password = password;
    _isExpress = isExpress;

    ostringstream os;
    os << "Connection credentials stored for reconnection: " << hostname << " / " << username;
    LogMe(os.str(), LOG_CYAN);
}

// Set subscription parameters for reconnection
void QuoteApiClient::setSubscriptionParams(const std::string& quoteSource, const std::vector<std::string>& symbols,
                                           QuoteSystemApi::SymbolType symbolType, int subscriptionTypeMask)
{
    std::lock_guard<std::mutex> lock(_reconnectMutex);
    // Store in map for multi-source reconnection support
    _subscriptionParamsMap[quoteSource] = symbols;
    _symbolType = symbolType;
    _subscriptionTypeMask = subscriptionTypeMask;

    ostringstream os;
    os << "Subscription parameters stored: " << symbols.size() << " symbols on source: " << quoteSource;
    LogMe(os.str(), LOG_CYAN);
}