
#ifndef _LIME_BROKERAGE_QUOTE_SYSTEM_API_HH_
#define _LIME_BROKERAGE_QUOTE_SYSTEM_API_HH_

/**
 * @mainpage
 *
 *  This API enables access to Lime Brokerage's Citrius market data.
 *
 */

/*
 *  For more information about how to use this API, please visit our %Quote
 *  System API Overview page.
 *
 */

// Standard C++ headers.
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#ifdef __linux__
#include <pthread.h>
#endif

// Lime headers.
#include "dynamicArray.hh"
#include "limeq_def.h"
#include "limetypes.hh"
#include "session.hh"
#include "socketProcessor.hh"
#include "thread.hh"
#include "event.hh"
#include "Mutex.hh"
#include "Price.hh"
#include "timestamp.hh"
#include "SpreadDefinition.hh"

/**
 * Base namespace for API code.
 */
namespace LimeBrokerage {

    class SessionManager;

    typedef struct security_definition_msg SecurityDefinition;

    /**
     *  Base class that provides access to Lime Brokerage's Citrius quote
     *  server market data.
     */
    class QuoteSystemApi : Session {

    public:
        enum LoginResult {
            LOGIN_FAILURE = -1,
            LOGIN_SUCCESS =  0,
            LOGIN_PENDING =  1
        };

        /**
         *  Describes an error that has occurred in the API.
         *
         * @see
         *  QuoteSystemApi::onQuoteSystemApiError()
         */
        enum QuoteSystemApiError {

            /**
             *  An undefined error has occurred.
             */
            unknownError = 0,

            /**
             *  An invalid username or password was used during a login
             *  attempt.
             */
            invalidUserNamePassword = 1,

            /**
             *  The version of the API is incompatible with the version of
             *  the quote server.
             */
            versionMismatch = 2,

            /**
             *  The server is currently recovering from a disconnection and
             *  is prohibiting new logins until recovery is complete.
             *
             *  When a quote server re-connects to a quote source mid-day
             *  some quote sources "rewind" and replay all of the day's prior
             *  market data at a high rate of speed.
             *
             *  While a quote server is recovering replayed data it will not
             *  allow new clients to log in, but it will continue to forward
             *  replayed messages to clients that are already logged in.
             */
            serverRewind = 3,

            /**
             *  The quote server has been disabled by an administrator and is
             *  prohibiting new login attempts until it is re-enabled.
             */
            serverDisable = 4,

            /**
             *  A login attempt has failed because the specified account has
             *  been disabled by an administrator.
             */
            accountDisable = 5,

            /**
             *  A login attempt has failed because the specified account has
             *  insufficient privileges to log in.
             */
            insufficientPrivileges = 6,

            /**
             *  A login attempt has failed because it originated from an IP
             *  address that is not recorded in the account configuration
             *  database.
             */
            ipAddressOutOfRange = 7,

            /**
             *  The quote server is overloaded and is prohibiting new
             *  login attempts until its workload is reduced.
             */
            loadAllocationExceeded = 8,

            /**
             *  The maximum number of clients that the quote server can
             *  support has been met.  The quote server is prohibiting new
             *  login attempts until some clients log out.
             */
            maxNumberOfClients = 9,

            /**
             *  A subscription request has failed because the specified quote
             *  source did not match the quote source of the quote server.
             *  See the quoteSource parameter in QuoteSystemApi::subscribe()
             *  for more information.
             */
            quoteSourceNotFound = 10,

            /**
             *  A subscription request has failed because the specified
             *  account has exceeded its number of allocated licenses for the
             *  requested quote source.
             */
            licenseAllocationExceeded = 11,

            /**
             *  A subscription request has failed because the specified
             *  account does not have all of the required licenses for the
             *  requested quote source.
             */
            noLicense = 12,

            /**
             *  The API encountered an invalid message from the quote
             *  server.
             */
            invalidMessage = 13,

            /**
             *  The API received an unrecognized message type from the quote
             *  server.  This is not necessarily a fatal error as older
             *  versions of the API may safely ignore new message types that
             *  are only supported by newer versions of the API.
             */
            unknownMsgType = 14,

            /**
             *  The API instance has lost its TCP connection to the quote
             *  server.
             */
            ioError = 15,

            /**
             *  A prohibited API call was made prior to successfully logging
             *  in to the quote server with login().  Some API calls may
             *  only be used after a successful login attempt.
             */
            notLoggedIn = 16,

            /**
             *  The maximum number of sessions allowed per account has been
             *  exceeded for the specified account.  New login attempts for
             *  the specified account are prohibited until some previously
             *  established sessions are logged out.
             */
            exceededMaxSessions = 17,

            /**
             *  The server has disabled subscribe all.
             */
            subscribeAllDisabled = 18,

            /**
             * @brief
             *  This code indicates that IP validation for license failed.
             *  Some quote sources (NYSE, NYOU, NYST, ARCA, AMOU) require IP address validation
             */
            licenseIpValidationFailed = 19,

            /**
             *  The client has attempted an operation that can not be performed once
             *  a subscription request has been sent to the server as part of this session.
             */
            alreadySubscribed = 20,
            /**
             *  The client has attempted to login to a service that does not match their
             *  assigned server group.  This is only permitted during emergency fail-overs.
             */
            invalidServerGroup = 21,

            /**
             * Production server doesn't support traditional authentication method (username, password)
             * New authentication method is used. It require clients to login to ACS server and
             * request authentication token
             */
            invalidAuthMethod = 22,

            /**
             * Username provided in the authentication token doesn't agree with user name provided in the login call
             */
            usernameMismatch = 23,

            /**
             * Authentication token signature is invalid
             */
            invalidTokenSignature = 24,

            /**
             * Login attempt to incorrect market data service
             */
            marketDataServiceMismatch = 25,

            /**
             * Client IP address mismatch
             */
            ipAddressMismatch = 26,

            /**
             * Authentication token expired
             */
            authTokenExpired = 27,

            /**
             * Authentication token can only be used once
             */
            duplicateToken = 28,

            /**
             * Exceeds maximum number of symbols to subscribe
             */
            maxNumberOfSymbols = 29,

            /*
             * This feature is no longer supported
             */
            featureDisabled = 30
        };

        /**
         *  Describes a change in connection status between the API and the
         *  quote server or between the quote server and the quote source.
         *
         * @see
         *  QuoteSystemApi::onQuoteStreamConnectionStatus()
         */
        enum QuoteStreamStatus {
             /**
             *  The quote server has connected to the quote source.
             */
            quoteStreamConnected = 200,

            /**
             *  The quote server has disconnected from the quote source.
             */
            quoteStreamDisconnected = 201,

            /**
             *  The quote server detected a possible slowdown of market data
             *  from the quote source.
             *
             * @deprecated
             *  As of API Release 1.6.
             */
            quoteStreamPossibleSlowdown = 202,

            /**
             *  The API instance has connected to the quote server.
             */
            quoteServerConnected = 203,

            /**
             *  The API instance has disconnected from the quote server.
             */
            quoteServerDisconnected = 204
        };

        /**
         *  Describes an event that has occurred at the quote source.
         *
         *  Availability of Alerts varies by quote source. Some quote sources
         *  send venue-specific alert codes.
         *
         *  See <a href="http://resources.limebrokerage.com/client/quoteAlertOverview.php">
         *  Quote Alert Overview</a> for a detailed explanation.
         *
         * @see {@link onQuoteAlert(const QuoteAlert&)}
         */
        enum Alert {
            /**
             * Set when short sale restriction REG SHO 201 is in effect
             */
            alertRegShoUpdate = 300,
            /**
             *  Trading is halted for the associated symbol.  If symbol is
             *  empty (""), all trading is halted.
             */
            alertTradingHalt = 302,

            /**
             *  Trading has resumed for the associated symbol.  If symbol is
             *  empty (""), all trading is resumed.
             */
            alertResume = 303,

            /**
             *  Quotes have resumed for the associated symbol.  If symbol is
             *  empty (""), all quoting is resumed.
             */
            alertQuoteResume = 313,

            /**
             *  Opening delay at the quote source. (SIAC specific)
             */
            alertOpeningDelay = 301,

            /**
             *  No open and no resume at the quote source. (SIAC specific)
             */
            alertNoOpenNoResume = 304,

            /**
             *  Price indication is available for the given symbol.
             *  (SIAC specific)
             */
            alertPriceIndication = 305,

            /**
             *  Trading range indication is available for the given symbol.
             *  (SIAC specific)
             */
            alertTradingRangeIndication = 306,

            /**
             *  Buy side imbalance for the given symbol at the open.
             *
             * @deprecated
             *  Replaced by MarketImbalance object.
             */
            alertMarketImbalanceBuy = 307,

            /**
             *  Sell side imbalance for the given symbol at the open.
             *
             * @deprecated
             *  Replaced by MarketImbalance object.
             */
            alertMarketImbalanceSell = 308,

            /**
             *  Buy side imbalance for the given symbol at the close.
             *
             * @deprecated
             *  Replaced by MarketImbalance object.
             */
            alertMarketOnCloseImbalanceBuy = 309,

            /**
             *  Sell side imbalance for the given symbol at the close.
             *
             * @deprecated
             *  Replaced by MarketImbalance object.
             */
            alertMarketOnCloseImbalanceSell = 310,

            /**
             *  No imbalance for the given symbol at the open.
             *
             * @deprecated
             *  Replaced by MarketImbalance object.
             */
            alertNoMarketImbalance = 311,

            /**
             *  No imbalance for the given symbol at the close.
             *
             * @deprecated
             *  Replaced by MarketImbalance object.
             */
            alertNoMarketOnCloseImbalance = 312,

            /**
             *  Trading is halted for the assocaited symbol at the
             *  associated exchange.
             *  Currently used for volatility-triggered trading
             *  halts at an exchange.
             */
            alertExchangeTradingHalt= 314,

            /**
             *  Quoting resumed for the associated symbol at the
             *  associated exchange.
             *  Currently used for volatility-triggered quote
             *  resumes at an exchange where it signals a quotation-
             *  nly period.
             */
            alertExchangeQuoteResume = 315,

            /**
             *  Trading is paused for the associated symbol.
             */
            alertPause = 316
        };

        /**
         * Short Sale Restriction Indicator (Rule 201)
         */
        enum ShortSaleRestrictionIndicator {
            /**
             * Short Sale Restriction Activated <br>
             * This code is set when Rule 201 of Reg SHO is triggered <br>
             * Supported by: SIAC
             */
            shortSaleRestrictionActivated = SHORT_SALE_RESTRICTION_ACTIVATED,
            /**
             * Short Sale Restriction In Effect <br>
             * This code is set when Rule 201 is in effect <br>
             * Supported by: BATS, INET, BSX, SIAC, UTDF, UQDF
             */
            shortSaleRestrictionInEffect = SHORT_SALE_RESTRICTION_IN_EFFECT,
            /**
             * Short Sale Restriction Continued <br>
             * This code is set when Rule 201 is in effect on the next day after it was triggered <br>
             * Supported by: INET, BSX, SIAC, UTDF, UQDF
             */
            shortSaleRestrictionContinued = SHORT_SALE_RESTRICTION_CONTINUED,
            /**
             * Short Sale Restriction Deactivated <br>
             * This code is set when Rule 201 is deactivated <br>
             * Supported by: SIAC
             */
            shortSaleRestrictionDeactivated = SHORT_SALE_RESTRICTION_DEACTIVATED,
            /**
             * Short Sale Restriction is not in effect <br>
             * This code is set when Rule 201 is not in effect <br>
             * Supported by: BATS, INET, BSX, SIAC, UTDF, UQDF
             */
            shortSaleRestrictionNotInEffect = SHORT_SALE_RESTRICTION_NOT_IN_EFFECT,
            /**
             * Trading status update. <br>
             * Some market update short sale restriction indicator and trading status separate <br>
             * This value indicates that alert was generated by trading status update and not by short sale restriction <br>
             */
            tradingStatusUpdate = TRADING_STATUS_UPDATE
        };

        /**
         *  Option exchanges values in 'quotingExchanges' field of
         *  OptionAttributes object.
         *
         * @see
         *  QuoteSystemApi::OptionAttributes
         */
        enum OptionExchange {
            /**
             *  AMEX Option Exchange.
             */
            optionExchangeAmex = 1<<1,

            /**
             *  BOX Option Exchange.
             */
            optionExchangeBox = 1<<2,

            /**
             *  CBOE Option Exchange.
             */
            optionExchangeCboe = 1<<3,

            /**
             *  ISE Option Exchange.
             */
            optionExchangeIse = 1<<4,

            /**
             *  ARCA Option Exchange.
             */
            optionExchangeArca = 1<<5,

            /**
             *  OPRA Option Exchange.
             */
            optionExchangeOpra = 1<<6,

            /**
             *  PACIFIC Option Exchange.
             */
            optionExchangePacific = 1<<7,

            /**
             *  NASDAQ Option Exchange.
             */
            optionExchangeNasdaq = 1<<8,

            /**
             *  PHLX Option Exchange.
             */
            optionExchangePhlx = 1<<9,

            /**
             *  BATS Option Exchange.
             */
            optionExchangeBats = 1<<10,

            /**
             *  C2 Option Exchange.
             */
            optionExchangeC2 = 1<<11
        };

        /**
         *  Security Type values used in onAddSymbol to categorize symbols.
         */
        enum SecurityType {
            securityTypeMin = 0,

            /**
             * Equity Symbols
             */
            securityTypeEquity = 0,

            /**
             * Option Symbols
             */
            securityTypeOption,

            /**
             * Future Symbols
             */
            securityTypeFutures,

            /**
             * Index Symbols
             */
            securityTypeIndex,

            securityTypeMax,
        };

        /**
         *  Establishes a session with the quote server.
         *
         *  You MUST successfully call QuoteSystemApi::login() before calling other methods.
         *
         *  The <a href="http://resources.limebrokerage.com/client/citriusConfiguration.php">
         *  Citrius Configuration Page</a> contains host/port configurations for each quote source.
         *
         *  The account username and password used for quotes may be different
         *  from your TS3 account username/password.  Please check with the
         *  trading desk (td@limebrokerage.com) if there is any confusion.
         *
         * @param host
         *  Location of the quote server of the format: <tt>&lt;hostname or IP address&gt;[:port]</tt>
         *
         * @param username
         *  Account username.
         *
         * @param password
         *  Account password.
         *
         * @return
         *  0 on success, -1 otherwise
         *
         * @see
         *  onQuoteSystemApiError(),
         *  invalidUserNamePassword,
         *  versionMismatch,
         *  serverRewind,
         *  serverDisable,
         *  accountDisable,
         *  insufficientPrivileges,
         *  ipAddressOutOfRange,
         *  loadAllocationExceeded,
         *  maxNumberOfClients
         */
        int login(std::string host, std::string username, std::string password);
        //int auth(std::string host, std::string& token);

        /**
         *  Identical to default login method, but includes a flag to indicate whether the login
         *  call should be blocking or not.  The default login method is blocking.
         *  @return
         *  0  on success,
         *  1  if login pending,
         *  -1 on failure
         */
        int login(std::string host, std::string username, std::string password, bool isBlockingLogin);
        //int auth(std::string host, std::string& token, bool isBlockingLogin);

        /**
         *  Identical to default login method, but includes flags to indicate whether login is for an express
         *  session and whether the login call should be blocking or not.  The default login method is
         *  not express and is blocking.
         *  @return
         *  0  on success,
         *  1  if login pending,
         *  -1 on failure
         */
        int login(std::string host, std::string username, std::string password, bool isBlockingLogin, bool isExpress);
        //int auth(std::string host, std::string& token, bool isExpress, bool isBlockingLogin);

        /**
         *  Disconnects the current session from the quote server.
         *
         * @param immediateLogout
         *  If true, the client won't wait for confirmation from the quote
         *  server and will disconnect immediately.
         *
         * @return
         *  0 on success, -1 otherwise
         */
        int logout(bool immediateLogout);

        /**
         *  Disconnects the current session from the quote server
         *  immediately.  This is equivalent to calling logout(true).
         *
         * @return
         *  0 on success, -1 otherwise
         */
        int logout();

        /**
         *  Enables the "Trade Replay" feature for the duration of the
         *  session.  On symbol subscription, all trade data received for
         *  that symbol since open will be replayed after the book is sent
         *  but prior to the book's onQuoteBookRebuildComplete() callback.
         *
         * @return
         *  0 on success, -1 otherwise
         */
        int enableTradeReplay();

        /**
         *  Enables the "Trades Only" feature for the duration of the
         *  session.  No quote data will be sent, only trade and
         *  administrative data.
         *
         * @return
         *  0 on success, -1 otherwise
         */
        int enableTradesOnly();

        /**
         *  Enables the "Top of Book" mode for the duration of the session.
         *  Note that in "Top of Book" mode, no book is maintained.  To
         *  process quote data in this mode, the most recent bid or ask for
         *  any symbol replaces all previous bids or asks.  %Trade and
         *  administrative data will also be sent.
         *
         * @return
         *  0 on success, -1 otherwise
         */
        int enableTopOfBook();

        /**
         *  Enables the IPO and uplisted subscriptions feature allowing subscriptions
         *  to symbols which might currently be unknown to the database including
         * @return
         *  0 on success, -1 otherwise
         */
        int enableIpoAndUplisted();

        /**
         *  Enables the "Aggregate by Price" feature for the duration of the
         *  session.  In this mode the quote server will send quote data that
         *  corresponds to a price-aggregated view of a book.  All quotes at
         *  a given price will be combined into a single quote with an
         *  aggregated quantity.  %Trade and administrative data will also be
         *  sent.
         *
         * @return
         *  0 on success, -1 otherwise
         */
        int enableAggregateByPrice();

        /**
         * @return
         *  0 on success, -1 otherwise
         */
        int enableExtendedQuote();

        /**
         *  Sets an MMID filter for the duration of the session.  This filter
         *  is applied to the "MMID" field of Quote and Trade objects.
         *
         * @param exclude
         *  If true, then excludes quotes and trades with MMIDs that do not
         *  match mmidList.  Only quotes and trades with the MMIDs listed
         *  will be included.  If false, includes all quotes and trades
         *  except for the those in mmidList.
         *
         * @param mmidList
         *  List of MMIDs.
         *
         * @return
         *  0 on success, -1 otherwise
         */
        int filterMMID(bool exclude, const std::vector<std::string>& mmidList);

        /**
         *  Enables the "Snapshot Book" feature for the duration of the
         *  session.  In this mode the quote server will send books but no
         *  streaming quotes or trades.
         *
         * @return
         *  0 on success, -1 otherwise
         */
        int enableSnapshotBook();

        int enableNagle();

        int enableRoundLotTob();

        int enableSimpleFeatures(int featureCodeMask);

        /**
         *  Subscribes the current session to live market data at the
         *  specified quote source for the given security symbols.
         *
         *  For each requested security, the quote server will send the
         *  full book to the client.
         *
         *  The book consists of:
         *   1) A onQuoteBookRebuildStarted() callback to to clear existing
         *      data.
         *   2) Zero or more onQuote() callbacks that populate the quotes
         *      contained in the book.
         *   3) A onQuoteBookRebuildComplete() callback to indicate that the
         *      book for the given security symbol is complete.
         *
         *  Subscription can be done any time after login. Subscription to the
         *  same symbol twice is legal, but the quote server will send only
         *  one book. Subscription to an unknown symbol is legal and the quote
         *  server will send an empty book for the unknown symbol.
         *
         *  A list of available quote sources is available on the
         *  <a href="http://resources.limebrokerage.com/client/citriusConfiguration.php">
         *  Citrius Configuration Page</a>.
         *
         * @param quoteSource
         *  The quote source to subscribe to.
         *
         * @param symbols
         *  The set of symbols to subscribe to.
         *
         * @return
         *  0 on success, -1 otherwise
         *
         * @see
         *  onQuoteSystemApiError(),
         *  quoteSourceNotFound,
         *  licenseAllocationExceeded,
         *  noLicense
         */
        int subscribe(std::string quoteSource, const std::vector<std::string>& symbols);

        /**
         *  Subscribes the current session to live market data at the
         *  specified quote source for all security symbols.
         *
         *  When subscribing to all symbols on a quote source, you may
         *  receive empty books for symbols that do not exist on the given
         *  quote source but were requested by other clients on the quote
         *  server.
         *
         *  Subscribing to all symbols can result in receiving a significant
         *  amount of data.  If your client software does not keep up with
         *  the volume of data, it will fall behind and may be disconnected.
         *  Subscribing to a relevant subset of symbols is highly
         *  recommended.
         *
         * @param quoteSource
         *  The quote source to subscribe to all securities at.
         *
         * @return
         *  0 on success, -1 otherwise
         *
         * @see
         *  subscribe(std::string, const std::vector<std::string>&) for more information.
         */
        int subscribeAll(std::string quoteSource);

        /**
         *  Unsubscribes the current session from live market data at the
         *  specified quote source for the given security symbols.
         *
         * @param quoteSource
         *  %Quote source to unsubscribe from symbols on.
         *
         * @param symbols
         *  Set of symbols to unsubscribe from.
         *
         * @return
         *  0 on success, -1 otherwise
         */
        int unsubscribe(std::string quoteSource, const std::vector<std::string>& symbols);

        /**
         *  Unsubscribes the current session from all security symbols at the
         *  given quote source.
         *
         * @param quoteSource
         *  %Quote source to unsubscribe from.
         *
         * @return
         *  0 on success, -1 otherwise
         */
        int unsubscribeAll(std::string quoteSource);

        /**
         *  Characterizes the type of the symbols specified in the detailed
         *  subscription methods listed below.
         *
         * @see
         *  subscribe(std::string quoteSource, const std::vector<std::string>& symbols, SymbolType symbolType, int subscriptionTypeMask)<br />
         *  unsubscribe(std::string quoteSource, const std::vector<std::string>& symbols, SymbolType symbolType, int subscriptionTypeMask)
         */
        enum SymbolType {
            /**
             *  Indicates subscription to the specified symbol.  On options
             *  feeds, this could be the 21 char option name composed as:
             *  <tt>OCC Symbol(6) + Expiration YYMMDD(6) + [P|C](1) + Strike(8)</tt>.
             *
             *  Used in the detailed subscribe methods.
             */
            symbolTypeNormal = 0,

            /**
             *  Indicates subscription to all options with the specified
             *  underlying, expiration, and strike.  For most underlyings
             *  there is just one option series with these attributes,
             *  but with corporate actions and some index options, there may
             *  sometimes be two or even up to 5 series with different OCC
             *  symbols, but the same expiration and strike.
             *
             *  Symbol is 21 characters in the following format:
             *  <tt>OCC Symbol(6) + Expiration YYMMDD(6) + [P|C](1) + Strike(8)</tt>.
             *
             *  Used in the detailed subscribe methods.
             */
            symbolTypeUnderlyingAttributes = 1,

            /**
             *  Indicates subscription to all options with the specified
             *  option root.  Used in the detailed subscribe methods.
             */
            symbolTypeClass = 2,

            /**
             *  Indicates subscription to all options with the specified
             *  underlying securities.  Used in the detailed subscribe methods.
             */
            symbolTypeUnderlying = 3,

            /**
             *  Indicates subscription to an index. Only needed for
             *  subscribeAll() call.
             */
            symbolTypeIndex = 4

        };

        /**
         *  Characterizes the type of market data to subscribe to in the
         *  detailed subscription methods listed below.
         *
         * @see
         *  subscribe(std::string quoteSource, const std::vector<std::string>& symbols, SymbolType symbolType, int subscriptionTypeMask)<br />
         *  subscribeAll(std::string quoteSource, int subscriptionTypeMask)<br />
         *  unsubscribe(std::string quoteSource, const std::vector<std::string>& symbols, SymbolType symbolType, int subscriptionTypeMask)<br />
         *  unsubscribeAll(std::string quoteSource, int subscriptionTypeMask)
         */
        enum SubscriptionType {
            /**
             *  Indicates subscription to quotes and trades.
             *  Indicates subscription to option analytics, i.e. greeks (Option Analytics feed only)
             */
            subscriptionTypeMarketData = 1<<0,

            /*
             *  Indicates subscription to shocked values (Option Analytics feed only)
             */
            subscriptionTypeShockedPnl = 1<<1,

            /**
             *  Indicates subscription to options attributes.
             */
            subscriptionTypeAttributes = 1<<2
        };

        /**
         *  Subscribes the current session to live market data and/or
         *  options attributes, based on subscriptionTypeMask.  The
         *  symbol names can be a symbol in the feed or the name of an
         *  underlying symbol for an options feed, depending on the
         *  symbolType parameter.
         *
         * @param quoteSource
         *  %Quote source to subscribe to.
         *
         * @param symbols
         *  Set of symbols to subscribe to.
         *
         * @param symbolType
         *  The type of symbol names in use.
         *
         * @param subscriptionTypeMask
         *  The type of subscription.
         *
         * @return
         *  0 on success, -1 otherwise
         *
         * @see
         *  SubscriptionType,
         *  SymbolType,
         *  subscribe(std::string, const std::vector<std::string>&)
         */
        int subscribe(std::string quoteSource, const std::vector<std::string>& symbols, SymbolType symbolType, int subscriptionTypeMask);

        /**
         *  Subscribes the current session to all live market data and/or
         *  all options attributes, based on subscriptionTypeMask.
         *
         * @param quoteSource
         *  %Quote source to subscribe to.
         *
         * @param subscriptionTypeMask
         *  The type of subscription.
         *
         * @return
         *  0 on success, -1 otherwise
         *
         * @see
         *  SubscriptionType,
         *  subscribeAll(std::string)
         */
        int subscribeAll(std::string quoteSource, int subscriptionTypeMask);

        /**
         *  Subscribes the current session to all live market data and/or
         *  all options attributes, based on the symbolType and subscriptionTypeMask.
         *
         * @param quoteSource
         *  %Quote source to subscribe to.
         *
         * @param symbolType
         *  The type of symbol.
         *
         * @param subscriptionTypeMask
         *  The type of subscription.
         *
         * @return
         *  0 on success, -1 otherwise
         *
         * @see
         *  SubscriptionType,
         *  subscribeAll(std::string)
         */
        int subscribeAll(std::string quoteSource, SymbolType symbolType, int subscriptionTypeMask);

        /**
         *  Unsubscribes the current session from live market data and/or
         *  options attributes, based on subscriptionTypeMask.  The
         *  symbol names can be a symbol in the feed or the name of an
         *  underlying symbol for an options feed, depending on the
         *  symbolType parameter.
         *
         * @param quoteSource
         *  %Quote source to unsubscribe from.
         *
         * @param symbols
         *  Set of symbols to unsubscribe from.
         *
         * @param symbolType
         *  The type of symbol names in use.
         *
         * @param subscriptionTypeMask
         *  The type of subscription.
         *
         * @return
         *  0 on success, -1 otherwise
         *
         * @see
         *  SubscriptionType,
         *  SymbolType,
         *  unsubscribe(std::string, const std::vector<std::string>&)
         */
        int unsubscribe(std::string quoteSource, const std::vector<std::string>& symbols, SymbolType symbolType, int subscriptionTypeMask);

        /**
         *  Unsubscribes the current session from all live market data and/or
         *  all options attributes, based on subscriptionTypeMask.
         *
         * @param quoteSource
         *  %Quote source to unsubscribe from.
         *
         * @param subscriptionTypeMask
         *  The type of subscription.
         *
         * @return
         *  0 on success, -1 otherwise
         *
         * @see
         *  SubscriptionType,
         *  unsubscribeAll(std::string)
         */
        int unsubscribeAll(std::string quoteSource, int subscriptionTypeMask);

        //
        // Virtual callback functions to be implemented by the client
        // application.
        //

        /**
         *  Called when an ask quote changes, if onQuote() is not overridden.
         *
         * @param symbol
         *  The symbol of the security associated with the quote.
         *
         * @param id
         *  A unique identifier for the quote. (quote server specific)
         *
         * @param size
         *  The quantity of the quote.  Zero means the quote was removed.
         *
         * @param price
         *  The price of the quote.
         *
         * @param mmid
         *  Depending on the quote source, this field may contain either the
         *  MMID of the firm placing the order or the exchange ID where the
         *  quote originated.  This value is up to five characters long. If
         *  no MMID or exchange ID is available, value may be an empty
         *  string.
         *
         * @param quoteSource
         *  The quote source of the quote.
         *
         * @param timestamp
         *  The timestamp of the quote, represented in the number of
         *  milliseconds since midnight in the exchange timezone.
         *
         * @see
         *  onQuote()
         *
         * @deprecated
         *  Please use onQuote()
         */
        virtual void onAskQuote(std::string symbol, long id, int size, double price, std::string mmid, std::string quoteSource, uint32_t timestamp);

        /**
         *  Called when a bid quote changes, if onQuote() is not overridden.
         *
         * @param symbol
         *  The symbol of the security associated with the quote.
         *
         * @param id
         *  A unique identifier for the quote. (quote server specific)
         *
         * @param size
         *  The quantity of the quote.  Zero means the quote was removed.
         *
         * @param price
         *  The price of the quote.
         *
         * @param mmid
         *  Depending on the quote source, this field may contain either the
         *  MMID of the firm placing the order or the exchange ID where the
         *  quote originated.  This value is up to five characters long. If
         *  no MMID or exchange ID is available, value may be an empty
         *  string.
         *
         * @param quoteSource
         *  The quote source of the quote.
         *
         * @param timestamp
         *  The timestamp of the quote, represented in the number of
         *  milliseconds since midnight in the exchange timezone.
         *
         * @see
         *  onQuote()
         *
         * @deprecated
         *  Please use onQuote()
         */
        virtual void onBidQuote(std::string symbol, long id, int size, double price, std::string mmid, std::string quoteSource, uint32_t timestamp);

        /**
         *  Called when a trade tick occurrs, if onTrade() is not overridden.
         *  If an onTradeTick() callback is received after an
         *  onQuoteBookRebuildStarted() callback, but prior to an
         *  onQuoteBookRebuildComplete() callback, then that trade represents
         *  the last recorded trade that was encountered by the quote server
         *  prior to transmitting the book.  The last recorded trade may be
         *  from the previous day if no recent trade data has been received.
         *  If the quote server has no last trade data, it will send a trade
         *  with price and size equal to zero.
         *
         * @param symbol
         *  The symbol of the security associated with the trade.
         *
         * @param size
         *  The quantity of the trade.
         *
         * @param price
         *  The price of the trade.
         *
         * @param totalVolume
         *  The total volume traded for the security.  The accuracy of this
         *  value is feed-specific.  Some feeds provide an authoritative
         *  total volume.  Other feeds require our quote servers to track
         *  the volume of all trades we encounter -- in which case the value
         *  is estimated based on the data that each quote server receives.
         *
         * @param exchangeId
         *  Depending on the quote source, this field may contain the exchange
         *  ID where the trade originated.  This value is up to five characters
         *  long. If exchange ID is not available, the value will be an empty
         *  string.
         *
         * @param quoteSource
         *  The quote source of the trade.
         *
         * @param timestamp
         *  The timestamp of the trade, represented in the number of
         *  milliseconds since midnight in the exchange timezone.
         *
         * @param tradeType
         *  The order type of the aggressive party (liquidity remover). Only
         *  set for some feeds.  Values are "BUY", "SELL", or an empty string
         *  when not available.
         *
         * @see
         *  onTrade()
         *
         * @deprecated
         *  Please use onTrade()
         */
        virtual void onTradeTick(std::string symbol, int size, double price, int totalVolume, std::string exchangeId, std::string quoteSource, uint32_t timestamp, std::string tradeType);

        /**
         *  Please use {@link onQuoteAlert(const QuoteAlert&)} to
         *  process QuoteAlert data.
         *
         *  Called to report various alerts from the quote source.
         *
         * @param quoteSource
         *  The quote source of the alert.
         *
         * @param symbol
         *  The symbol of the security associated with this alert. Can be
         *  an empty string, if the alert does not apply to a specific
         *  security.
         *
         * @param code
         *  Alert code of the quote alert.
         *
         * @param message
         *  String describing the alert. May be an empty string.
         *
         * @see onQuoteAlert(const QuoteAlert&)
         */
        virtual void onQuoteAlert(std::string quoteSource, std::string symbol, Alert code, std::string message);

        /**
         *  Please use {@link onQuoteAlert(const QuoteAlert&)} to
         *  process QuoteAlert data.
         *
         *  Called to report various alerts from the quote source.
         *
         * @param quoteSource
         *  The source of the alert.
         *
         * @param symbol
         *  The security symbol associated with this alert. Can be null, if
         *  the alert is not security specific.
         *
         * @param code
         *  Type of the quote alert.
         *
         * @param message
         *  String describing the alert. May be null.
         *
         * @see onQuoteAlert(const QuoteAlert&)
         */
        virtual void onQuoteAlert(std::string quoteSource, std::string symbol, int code, std::string message);

        /**
         *  Called to notify client that the book for <tt>symbol</tt> must be
         *  cleared and rebuilt.
         *
         *  This is followed by zero or more onQuote() callbacks that
         *  populate the quotes contained in the book.
         *
         *  Finally the onQuoteBookRebuildComplete() callback is called to
         *  notify the client that the book rebuild for <tt>symbol</tt> is
         *  complete.
         *
         * @param quoteSource
         *  The quote source that the rebuild is occuring at.
         *
         * @param symbol
         *  The symbol of the book which is being rebuilt.
         *
         * @see
         *  onQuote(),
         *  onQuoteBookRebuildComplete()
         */
        virtual void onQuoteBookRebuildStarted(std::string quoteSource, std::string symbol);

        /**
         *  Called to notify client that all quotes required to build the
         *  current book for <tt>symbol</tt> have been delivered via
         *  onQuote() callbacks.
         *
         *  All subsequent onQuote() callbacks for <tt>symbol</tt> are
         *  realtime updates to <tt>symbol</tt>'s current book.
         *
         * @param quoteSource
         *  The quote source that the rebuild is occuring at.
         *
         * @param symbol
         *  The symbol of the book which is being rebuilt.
         *
         * @see
         *  onQuoteBookRebuildStarted(),
         *  onQuote()
         */
        virtual void onQuoteBookRebuildComplete(std::string quoteSource, std::string symbol);

        /**
         *  Called to notify client of changes in the connection status
         *  between the API and quote server as well as the quote server and
         *  quote source.
         *
         * @param quoteSource
         *  The quote source that the connection status change is occuring
         *  at.
         *
         * @param status
         *  Status code to indicate the current state of the connection.
         *
         * @param reason
         *  Plain text string identifying the reason for the quote stream
         *  connection status update.
         *
         * @see
         *  QuoteStreamStatus,
         *  quoteStreamConnected,
         *  quoteStreamDisconnected,
         *  quoteServerConnected,
         *  quoteServerDisconnected
         */
        virtual void onQuoteStreamConnectionStatus(std::string quoteSource, QuoteStreamStatus status, std::string reason);

        /**
         *  Called to notify client of errors.
         *
         * @param quoteSource
         *  The quote source that the error is occuring at.
         *
         * @param errorCode
         *  Code to indicate the error.
         *
         * @param errorMsg
         *  Plain text string containing the error message associated with
         *  the error.
         *
         * @see
         *  QuoteSystemApiError,
         *  unknownError,
         *  invalidUserNamePassword,
         *  versionMismatch,
         *  serverRewind,
         *  serverDisable,
         *  accountDisable,
         *  insufficientPrivileges,
         *  ipAddressOutOfRange,
         *  loadAllocationExceeded,
         *  maxNumberOfClients,
         *  quoteSourceNotFound,
         *  licenseAllocationExceeded,
         *  noLicense,
         *  invalidMessage,
         *  unknownMsgType,
         *  ioError,
         *  notLoggedIn,
         *  exceededMaxSessions
         *  usernameMismatch
         *  invalidTokenSignature
         *  quoteSourceMismatch
         *  authTokenExpired
         *  duplicateToken
         */
        virtual void onQuoteSystemApiError(std::string quoteSource, QuoteSystemApiError errorCode, std::string errorMsg);
        //Forward declaration
        struct Quote;
        struct Trade;
        struct MarketImbalance;
        struct OptionAttributes;
        struct AuctionUpdate;
        struct AuctionSummary;
        struct SecurityDefinitionHeader;
        struct SecurityDefinitionCommonData;
        struct SymbolStatus;
        struct Index;
        struct Analytics;
        struct ShockedPnl;
        struct QuoteAlert;
        class PriceBand;
        class RoundLotTob;
    protected:

        /**
         *  Called when a bid or ask quote changes.  If not overridden,
         *  calls either onBidQuote() or onAskQuote(), depending on the type
         *  of Quote.  Call super.onQuote() to continue to recieve
         *  onBidQuote() and onAskQuote() callbacks.
         *
         *  This method can be used to get additionalProperties for INET and
         *  quote conditions for SIAC and NYSE.  Please refer to the INET,
         *  SIAC and NYSE specifications for values and descriptions.
         *
         *  This method allows you to use a symbol index instead of string,
         *  as well as price mantissa and exponent to calculate the price,
         *  instead of double to avoid floating point calculation.
         *
         *  Quote instance MUST NOT be reused. The API reuses this object for
         *  the next API call.
         *
         * @param quote
         *  The quote data holder.
         *
         * @see
         *  Quote
         */
        virtual void onQuote(const Quote& quote);

        /**
         *  Called when a trade occurrs.  If not overridden, calls
         *  onTradeTick().  Call super.onTrade() to continue to recieve
         *  onTradeTick() callbacks.
         *
         *  This method can be used to get SIAC "Sale Conditions."
         *  Please refer to the SIAC specification for values and a description.
         *
         *  Trade instance MUST NOT be reused. The API resuses this object for the next API call.
         *
         * @param trade
         *  The trade data holder.
         *
         * @see
         *  Trade
         */
        virtual void onTrade(const Trade& trade);

        /**
         *  Called when a modify-execution occurs at the quote source.  If
         *  not overridden, calls onQuote() followed by onTrade() to
         *  represent the quote modify and the subsequent trade execution.
         *
         *  In feeds such as INET, EDGA, EDGX, TRAC, and BATS, we receive a
         *  modify execution message when an order has been partially or
         *  fully executed.  This results in an updated quote with a reduced
         *  quantity, followed by a trade for the number of shares that were
         *  executed.
         *
         *  This callback allows clients to see that a quote and trade are
         *  related to a modify execution action at the quote source.
         *
         *  This callback will never be called when used with the
         *  Aggregate-By-Price, Top-Of-Book, or Trades-Only features.
         *
         *  Quote and Trade instances MUST NOT be reused. The API resuses
         *  these objects for the next API call.
         *
         * @param quote
         *  The quote data holder for the "modify".
         *
         * @param trade
         *  The trade data holder for the "exeuction".
         */
        virtual void onModifyExecution(const Quote& quote, const Trade& trade);

        /**
         *  Called when an order-replace occurs at the quote source.  If not
         *  overridden, this method calls onQuote() followed by onQuote() to
         *  represent the quote deletion followed by the subsequent quote
         *  addition.
         *
         *  On INET and similar feeds we may receive an order replace
         *  message when an existing order has been replaced by a similar new
         *  order (with the same symbol, MMID, and side, but a different price
         *  and/or quanity).
         *
         *  This callback allows clients to see that a delete quote and an add
         *  quote are related to an order replace action at the quote source.
         *
         *  Quote instances MUST NOT be reused. The API resuses these objects
         *  for the next API call.
         *
         * @param oldOrder
         *  The delete quote for old order.
         *
         * @param newOrder
         *  The add quote for new older.
         */
        virtual void onOrderReplace(const Quote& oldOrder, const Quote& newOrder);

        /**
         *  Called when the Limit Up/Limit Down (LULD) price band for a symbol
         *  changes.
         */
        virtual void onPriceBand(const PriceBand& priceBand);

        /*
         *  Called when an extended limeq message is sent by the quote
         *  server.  For future use with the API.
         *
         * @param quoteSource
         *  The quote source that the limeq control message is originating
         *  from.
         *
         * @param type
         *  The type of the limeq control message.
         *
         * @param byteArray
         *  Type-specific byte array.
         *
         * @param len
         *  Length of the byte array.
         */
        virtual void onLimeqControlMessage(std::string quoteSource, int type, char *byteArray, int len);

        /*
         *  Called when channel status changes.
         *
         * @param quoteSource
         *  The quote source that the limeq control message is originating
         *  from.
         *
         * @param channelId
         *  The channel id for which the message is intended.
         *
         * @param statusType
         *  The only statusType supported so far is CHANNALE_STATUS_TYPE_LOSS, indicating multicast loss on the channel.
         *
         * @param value
         *  dependent on the statusType, this parameter indicates the value for the given status.
         *  For multicast loss this value indicates the amount of loss.
         *
         * @param timestamp
         *  Time of last channel event.
         */
        virtual void onChannelStatus(std::string quoteSource, uint16_t channelId, channel_status_type_t statusType, uint32_t value, uint32_t timestamp);

        /**
         *  Called when there is loss on a multicast channel.  For quote services that receive multicast
         *  market data, this function will be called to inform clients of a loss event on a multicast
         *  channel. It reports the total number of messages lost since the service started. For services
         *  that recover data, recovery is not currently reported.
         *
         * Venues are increasingly delivering real-time market data to Lime via multicast because
         * multicast delivery has considerably better latency and bandwidth characteristics than
         * traditional TCP delivery.
         *
         * Multicast is inherently an unreliable delivery mechanism and it lacks
         * flow control.  Thus packets containing market data can be dropped at the
         * venue, on the network, on a network interface, or in software.  To
         * minimize the chance of drops, Lime maintains a robust high-performance
         * network.  Nonetheless, data loss is a rare but inevitable consequence of
         * multicast market data delivery.
         *
         * Where practical, Lime has implemented venue-specific recovery mechanisms
         * to recover lost data.  But because recovery is inherently unreliable, it
         * is important that clients design their applications to handle rare
         * instances of multicast loss.
         *
         * Each Citrius market data services maintains an order book per security
         * and distributes order updates, trades, security status, and imbalance
         * information in real time.  When messages from the venue are lost,
         * the data associated with a given security can become inconsistent.
         *
         * Inconsistent data for a security may include:
         * <ul>
         *  <li>Orders that should have been removed are stuck in the book</li>
         *  <li>Orders may have an incorrect quantity, price, and/or other properties</li>
         *  <li>Orders that should have been added are not in the book</li>
         *  <li>Some trade, alert, and/or imbalance data may not be reported</li>
         *  <li>Total volume, last trade, and symbol status can become incorrect</li>
         * </ul>
         *
         * Many venues partition market data over several multicast distribution
         * channels, with multiple securities being assigned to each channel.  When
         * loss occurs on a channel, only securities associated with that channel
         * can have inconsistent data.  Every venue has a unique partitioning
         * scheme.  In the future, Lime may provide the security to channel mapping.
         *
         * @param quoteSource
         *  The quote source that has experienced the loss event.
         *
         * @param channelId
         *  The channel id for the channel on which the loss occurred.   In the future, we may provide a
         *  mapping of symbols associated with each channel id if such information is available. This
         *  would help narrow the subset of books potentially affected by loss.
         *
         * @param skippedMessages
         *  Indicates the total number of messages lost on the channel since the service started. If
         *  there are multiple loss events this number will increase cumulatively.
         *
         * @param timestamp
         *  Time of last loss event in milliseconds since midnight.
         */
        virtual void onMulticastLoss(std::string quoteSource, uint16_t channelId, uint32_t skippedMessages, uint32_t timestamp);

        /**
         *  Called to inform client of the mapping of a symbol name to a
         *  symbol index for a given quote source.  Symbol indicies are used
         *  in Quote and Trade objects to represent the symbol name in
         *  numeric form. This version of the callback also includes a symbol type
         *  which can be one of valid Security Types as defined in SecurityType
         *
         *  This call is guaranteed to occur before the first call to
         *  onQuote() or onTrade(), so the client application will have the
         *  necessary symbol_index:symbol mapping to interpret the Quote or
         *  Trade data.
         *
         *  Note that the symbol_index is not guaranteed to be the same
         *  between different instances of the quote server or over different
         *  days.
         *
         * @param quoteSource
         *  The quote source for which the mapping applies
         *
         * @param symbol
         *  The symbol name to be mapped
         *
         * @param symbolIndex
         *  The numeric index that the symbol name is mapped to for the
         *  specified quote source
         *
         * @param symbolType
         *  The type of symbol (Equity, Future, Option or Index). @see SecurityType for more info
         */
        virtual void onAddSymbol(std::string quoteSource, std::string& symbol, uint32_t symbolIndex, SecurityType symbolType);

        /**
         *  Called to inform client of the mapping of a symbol name to a
         *  symbol index for a given quote source.  Symbol indicies are used
         *  in Quote and Trade objects to represent the symbol name in
         *  numeric form.
         *
         *  This call is garaunteed to occur before the first call to
         *  onQuote() or onTrade(), so the client application will have the
         *  necessary symbol_index:symbol mapping to interpret the Quote or
         *  Trade data.
         *
         *  Note that the symbol_index is not guaranteed to be the same
         *  between different instances of the quote server or over different
         *  days.
         *
         * @param quoteSource
         *  The quote source for which the mapping applies
         *
         * @param symbol
         *  The symbol name to be mapped
         *
         * @param symbolIndex
         *  The numeric index that the symbol name is mapped to for the
         *  specified quote source
         */
        virtual void onAddSymbol(std::string quoteSource, std::string& symbol, uint32_t symbolIndex);

        /**
         *  Called to disseminate market imbalance data from the quote source.
         *
         * @param marketImbalance
         *  Market Imbalance object containing imbalance information.
         *
         * @see
         *  <a href="http://resources.limebrokerage.com/client/marketOrderImbalanceOverview.php">Market Order Imbalance Overview</a>
         */
        virtual void onMarketImbalance(const MarketImbalance &marketImbalance);

        /**
         *  Called when Citrius obtains attributes for an option.  The
         *  system guarantees this call occurs before the first call to
         *  onQuote() or onTrade(), so the client can use the symbolIndex in
         *  the OptionAttributes to interpret Quote or Trade data.
         *
         * @param optionAttributes
         *  Option Attributes object containing attribute information.
         *
         * @see
         *  QuoteSystemApi::OptionAttributes
         */
        virtual void onOptionAttributes(const OptionAttributes &optionAttributes);

        /**
         *  Called when Citrius receives an update to an existing auction.
         *
         * @param auctionUpdate
         *  Auction Update object containing information about the update.
         *
         * @see
         *  QuoteSystemApi::AuctionUpdate
         */
        virtual void onAuctionUpdate(const AuctionUpdate &auctionUpdate);

        /**
         *  Called when Citrius receives a summary of a completed auction.
         *
         * @param auctionSummary
         *  Auction Summary object containing information about the auction.
         *
         * @see
         *  QuoteSystemApi::AuctionSummary
         */
        virtual void onAuctionSummary(const AuctionSummary &auctionSummary);

        /**
         *  Called when a trade cancellation/correction is received.
         *
         * @param originalTrade
         *  The trade being cancelled/corrected.
         *
         * @param modifiedTrade
         *  The modified trade.
         */
        virtual void onTradeMod(const Trade& originalTrade, const Trade& modifiedTrade);

        /**
         *  Called to notify client of an update to the symbol status.
         *  Currently contains high/low/open/close/last trade information.
         *
         * @param symbolStatus
         *  Object containing symbol status information.
         */
        virtual void onSymbolStatus(const SymbolStatus& symbolStatus);

        /**
         * Called when the response to a feature control message is received.
         *
         * Quote servers might be configured to support a subset of features.
         * Clients should implement this callback to ensure their enable<feature>() calls have succeeded.
         *
         * @param featureCode
         *  Code for requested feature.
         *
         * @param response
         *  Response.
         */
        virtual void onFeatureControlResponse(int featureCode, int response);

        /**
         * Called when an index value is received.
         *
         * @param index
         *  Object containing index information.
         */
        virtual void onIndex(const Index& index);

        /**
         * Called to disseminate the definition of a spread, on Complex
         * Order feeds.
         *
         * @param spread
         *  Object containing spread definition information.
         */
        virtual void onSpreadDefinition(const SpreadDefinition& spread);

        /*
         * Called to provide analytics.
         *
         * Analytics instances MUST NOT be reused.  The API reuses this object
         * for the next API call.
         *
         * @param analytics
         *  The analytics (greeks) holder.
         *
         * @see
         *  Analytics
         */
        virtual void onAnalytics(const Analytics& analytics) { }

        /*
         * Called to provide shocked P&L values.
         *
         * ShockedPnl instances MUST NOT be reused.  The API reuses this object
         * for the next API call.
         *
         * @param shockedPnl
         *  The shocked P&L values holder.
         *
         * @see
         *  ShockedPnl
         */
        virtual void onShockedPnl(const ShockedPnl& shockedPnl) { }

        /**
         * Called when a security's trading state or Reg SHO short-sale
         * restriction state is disseminated by the quote source.
         *
         * See <a href="http://resources.limebrokerage.com/client/quoteAlertOverview.php">
         * Quote Alert Overview</a> for a detailed explanation.
         *
         * @see
         *  QuoteSystemApi::QuoteAlert
         */
        virtual void onQuoteAlert(const QuoteAlert& alert);

        virtual void onRoundLotTob(const RoundLotTob& quote);

    public:
        // Destructor.
        virtual ~QuoteSystemApi();

        /**
         * True if the API is logged in.  Please note that,
         * !loggedIn() != loggedOut().  Thus, if the API is not logged in,
         * it doesn't mean that it is logged out.  It may be in an
         * intermediate state.
         *
         * @return
         *  true if the API is logged in, false otherwise.
         */
        bool loggedIn() { return (loginStatus == LS_LOGGED_IN);}

        /**
         * Check if the API is logged out.  Please note that,
         * !loggedOut() != loggedIn().  Thus, if the API is not logged out,
         * it doesn't mean that it is logged in.  It may be in an
         * intermediate state.
         *
         * @return
         *  true if the API is logged out, false otherwise.
         */
        bool loggedOut() { return (loginStatus == LS_LOGGED_OUT);}

        /**
         * Check if a subscription request has been sent to the server as part of this
         * session.
         **/
        bool subscribed() { return (subscriptionStatus_ == SUBSCRIPTION_STATUS_SUBSCRIBED); }

        /**
         * Wait to be logged out and released from the Session Manager.
         * This function should be called after logout() has been called to
         * ensure that a previously established session is cleaned up
         * appropriately prior to calling login() again.
         */
        void joinReceiver();

        /**
         * Return the API's build identifier. (Note that this value may differ
         * from the version number in the API zip file.)
         */
        std::string getVersion() const;
    protected:

        // Default constructor (protected, since this is an abstract class.)
        QuoteSystemApi(SessionManager *pMgr = NULL);

        QuoteSystemApi(int cpu);

    private:
        // Quote Server and LimeQ constants.
        //List of old quote source names

        enum LoginStatus {
            LS_LOGGED_OUT,
            LS_CONNECTING,
            LS_SEND_LOGIN_REQUEST,
            LS_RECV_LOGIN_RESPONSE,
            LS_LOGGED_IN,
            LS_LOGOUT_PENDING
        };

        enum SubscriptionStatus { SUBSCRIPTION_STATUS_SUBSCRIBED, SUBSCRIPTION_STATUS_UNSUBSCRIBED };
        static const std::string NYSE_OB;
        static const std::string NYOB;
        static const std::string Level1;
        static const std::string LONE;
        static const std::string Level2;
        static const std::string LTWO;
        static const std::string CFUT;
        static const std::string CSTK_FUT;

        static const std::vector<std::string> emptyVector;
        static const int majorVersion;
        static const int minorVersion;
        static const int heartbeat; // in seconds
        static const int heartbeatTimeout; // in seconds

        static const uint32_t maxSymbols; // initial max number of symbols
        static const uint32_t maxEquitySymbols; // initial max number of symbols
        static const int minMessageLength; // minimum message length

        static const std::vector<std::string> errorCodes;
        static const uint32_t powers_of_ten[];
        static bool isValidSymbol(std::string symbol);
        inline const std::string& translateQuoteSource(std::string& quoteSource);
        inline void handleOrderReplace(const Quote& oldOrder, const Quote& newOrder);
        inline void handleModifyExecution(const Quote& quote, const Trade& trade);

        inline int writeSubRequest(uint16_t symbolLen, int subscriptionFlags, std::string& qsid, uint16_t numSymbols, char *symbols);
        inline int sendSubscribeUnsubscribe(std::string quoteSource, const std::vector<std::string>& symbols,
                                            bool subscribe, SymbolType symbolType, int subscriptionTypeMask);

        inline void addSymbol(std::string qsid, uint32_t symbol_indx, const char *symbol, int symbol_len, SecurityType symbol_type);

        inline void *getNextMessage(int *pLen);

        /**
         * Call this function to receive messages from the quote server and
         * process them.  This function should only be explicitly called when
         * the instance of Session Manager associated with this instance of
         * QuoteSystemApi is not configured to run in a separate thread. In
         * cases where the Session Manager associated with this instance of
         * QuoteSystemApi is configured to run in a separate thread there is no
         * need to explicitly call this function.
         *
         * @return the number of bytes left to be processed.
         */
        int processNextMsg();

        int genericFeatureEnable(int code);

        int cleanup();

        /*
         * Login helper methods:
         * open - opens socket
         * connect - tries to establish connection to the server, might be non blocking
         * sendLoginRequest - check connection status, if connected send login request
         */

        int open(bool blocking);

        int connect(std::string host);

        int processSessionState(std::string host, bool isBlockingLogin, bool isLoginRequest);

        int sendLoginRequest();

        int sendAuthRequest();

        int recvLoginResponse(std::string username);

        LoginStatus setLoginStatus(LoginStatus ls);

        void onCancelRegistration();

        void setTradeFlags(Trade& trade, uint8_t flags);

        void issueDelayedCallback();

        bool shouldIssueCallback(uint8_t msgType);

        void resetObject(Quote& quote);

        void resetObject(Trade& trade);

        // This method parses revision string to populate versions
        void populateVersion();

    protected:
        SocketProcessor* socketProcessor;
        inline std::string& getSymbol(uint32_t symbol_indx) {
            return getSymbol(symbol_indx, SecurityType(defaultSecType));
        }

        inline std::string& getSymbol(uint32_t symbol_indx, SecurityType secType);

    public:
        /**
         *  A container for raw quote (order) data.
         */
        struct Quote
        {
            /**
             *  True if this quote represents a bid.
             */
            bool isBid;

            /**
             *  Unique numeric identifier for the traded symbol.
             *
             *  Established by call from onAddSymbol().  Please note that
             *  symbol indicies are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t symbolIndex;

            /**
             *  Lime specific quote id.
             */
            uint32_t id;

            /**
             *  Number of shares offered.
             */
            uint32_t size;

            /**
             *  Price mantissa.
             */
            int32_t priceMantissa;

            /**
             *  Price exponent.
             *
             *  Use following formula to calculate price:
             *  <tt>price = priceMantissa * (10 ^ priceExponent)</tt>
             *
             *  Please note that both priceExponent and priceMantissa are
             *  signed.
             */
            int8_t priceExponent;

            /**
             *  Feed specific marke maker id.
             */
            std::string mmid;

            /**
             *  Lime specific quote source id.
             */
            std::string quoteSource;

            /**
             *  Quote source timestamp expressed in milliseconds since
             *  midnight.
             */
            uint32_t timestamp;

            /**
             *  Nanoseconds portion of the time stamp.
             */
            uint32_t nsecTimestamp;

            /**
             *  Conveys feed-specific quote conditions for UQDF, SIAC, NYSE, NYOU, and OPRA.
             *  For all other services this field contains legacy values that are deprecated,
             *  and it should not be processed.
             */
            uint8_t quoteProperties;

            /**
             *  Feed specific field, see details.
             *
             *  <ul>
             *      <li>
             *          For NYOU this field contains the tradingStatus. As per the NYOU specification - <br/>
             *          The current trading status of the equity. <br/>
             *          Valid Values: <br/>
             *          <ol>
             *              <li>'P' - Pre-Opening for messages sent before the stock is opened on a trade or quote</li>
             *              <li>'O' - The stock has opened or re-opened</li>
             *              <li>'C' - The stock was closed from the Closing template</li>
             *              <li>'H' - The stock is halted during a trading halt and has not resumed</li>
             *          </ol>
             *      </li>
             *      <br/>
             *      <li>
             *          For CBOE this field contains the type of book, Limit Order or Contingency (All or None) Order. <br/>
             *          As per the CBOE Market Data 1.3 specification - <br/>
             *          Volume Type (book type). <br/>
             *          Valid Values: <br/>
             *          <ol>
             *              <li>0 - Limit Order </li>
             *              <li>2 - Contingency (All or None) Order </li>
             *          </ol>
             *      </li>
             *  </ul><br/><br/>
             */
            uint8_t feedSpecificData;

            /**
             *  Depricated
             *  True if quote is the top of the quote server's quote book.
             */
            bool isTopOfBook;

            /**
             *  True if quote is designated as the NBBO by the venue
             */
            bool isNBBO;

            /**
             *  True if quote is part of a price-aggregated view of the
             *  quote server's quote book.
             */
            bool isAggregatedByPrice;

            /**
             *  Exchange order id.  Populated only on certain feeds that provide the data.
             *  Not populated when price aggregation is enabled.
             */
            uint64_t exchangeOrderId;

            /**
             *  Citrius server timestamp.  Populated only when the extended quotes feature
             *  is enabled.
             */
            uint64_t citriusTimestamp;

            /**
             *  Exchange sequence number.  Populated only when the extended quotes feature
             *  is enabled.
             */
            uint64_t exchangeSeqNum;
        };

        /**
         *  A container for raw trade (order execution) data.
         */
        struct Trade
        {
            /**
             *  Unique numeric identifier for the traded symbol.
             *
             *  Established by call from onAddSymbol().  Please note that
             *  symbol indicies are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t symbolIndex;

            /**
             *  Number of shares executed.
             */
            uint32_t size;

            /**
             *  Price mantissa.
             */
            int32_t priceMantissa;

            /**
             *  Price exponent.
             *
             *  Use following formula to calculate price:
             *  <tt>price = priceMantissa * (10 ^ priceExponent)</tt>
             *
             *  Please note that both priceExponent and priceMantissa are
             *  signed.
             */
            int8_t  priceExponent;

            /**
             *  Total trading volume for this security symbol.
             *
             *  This number does not currently reflect trade cancelations
             *  for most feeds.
             */
            uint32_t totalVolume;

            /**
             *  Feed specific exchange id.
             */
            std::string exchangeId;

            /**
             *  Lime specific quote source id.
             */
            std::string quoteSource;

            /**
             *  Quote source timestamp expressed in milliseconds since
             *  midnight.
             */
            uint32_t timestamp;

            /**
             *  Nanoseconds portion of the time stamp.
             */
            uint32_t nsecTimestamp;

            /**
             *  This field indicates which side was liquidity remover in the
             *  trade.
             */
            std::string tradeType;

            /**
             *  Up to 4 bytes SIAC specific sale conditions.  See SIAC
             *  specification for more information.
             */
            char tradeProperties[4];

            /**
             *  Filled with the TradeId provided by the feed,
             *  If the feed does not provide a TradeId, this field is 0.
             */
            uint64_t tradeId;

            /**
             *  True when this trade represents the new high tick.
             */
            bool isHigh;

            /**
             *  True when this trade represents the new low tick.
             */
            bool isLow;

            /**
             *  True when this trade has been cancelled.
             */
            bool isCancelled;

            /**
             *  True when this trade has been corrected.
             */
            bool isCorrected;

            /**
             *  True when this trade is the last trade of the previous trading session.
             */
            bool isPreviousSessionLastTrade;

            /**
             *  True when this trade is the last trade.
             */
            bool isLast;

            /**
             *  True when this trade is the opening trade on the listing exchange.
             */
            bool isOpen;

            /**
             *  True when this trade is the closing trade on the listing exchange.
             */
            bool isClose;

            /**
             * Exchange id of order which was executed.
             * Provided when the "Order Visibility" feature is enabled.
             */
            char exchangeOrderId[EXCHANGE_ORDER_ID_LEN];

            /**
             *  Citrius server timestamp.  Populated only when the extended quotes feature
             *  is enabled.
             */
            uint64_t citriusTimestamp;

            /**
             *  Exchange sequence number.  Populated only when the extended quotes feature
             *  is enabled.
             */
            uint64_t exchangeSeqNum;
        };

        /**
         *  A container for market imbalance data.
         */
        struct MarketImbalance
        {
            /**
             *  Unique numeric identifier for the traded symbol.
             *
             *  Established by call from onAddSymbol().  Please note that
             *  symbol indicies are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t symbolIndex;

            /**
             *  Side of imbalance.
             *  (<tt>unknown = 0; buy = 1; sell = 2;</tt>)
             */
            uint8_t side;

            /**
             *  Price mantissa.
             */
            int32_t priceMantissa;

            /**
             *  Price exponent.
             *
             *  Use following formula to calculate price:
             *  <tt>price = priceMantissa * (10 ^ priceExponent)</tt>
             *
             *  Please note that both priceExponent and priceMantissa are
             *  signed.
             */
            int8_t priceExponent;

            /**
             *  Total number of shares paired at the reference price.
             */
            uint32_t pairedShares;

            /**
             *  Depending on the quote source, this field may contain the exchange
             *  ID where the imbalance data originated.  This value is up to five
             *  characters long. If exchange ID is not available, the value will be
             *  an empty string.
             */
            std::string exchangeId;

            /**
             *  Lime specific quote source.
             */
            std::string quoteSource;

            /**
             *  Quote source timestamp expressed in milliseconds since
             *  midnight.
             */
            uint32_t timestamp;

            /**
             *  Quote source timestamp nanoseconds expressed in nanoseconds
             *  since millisecond timestamp.
             */
            uint32_t nsecTimestamp;

            /**
             *  Total imbalance between buy and sell orders.  This value can
             *  be negative.
             */
            int32_t totalImbalance;

            /**
             *  Feed specific field, see details.
             *
             *  For ARCA this field contains the total imbalance between
             *  market orders.  It can be negative.
             *
             *  For INET marketImbalance field equals the near indicative
             *  clearing price. The far indicative clearing price can be
             *  obtained from the feedSpecificData field.
             *
             *  For NYOU and NYSI, this field contains the
             *  clearingPriceNumerator for the opening imbalance messages.
             *  For the closing imbalance message, it contains the
             *  ContinuousBookClearingPriceNumerator.  As per the NYOU
             *  specification, this is the price closest to Reference
             *  Price where imbalance is zero.
             *
             *  For IEX, this field contains the indicative clearing
             *  price. As per the IEX specification, this is the clearing
             *  price using eligible auction orders.
             */
            int32_t marketImbalance;

            /**
             *  Type of imbalance: Open, Close, Halt, or No Imbalance.
             *
             *  <pre>
             *   unknown = 0;      // ARCA, NYOU, IEX
             *   open = 1;         // ARCA, NYOU, IEX
             *   market = 2;       // ARCA
             *   halt = 3;         // ARCA, IEX
             *   close = 4;        // ARCA, NYOU, IEX
             *   no imbalance = 5; // NYOU
             *  </pre>
             */
            char imbalanceType;

            /**
             *  Feed specific field, see details.
             *
             *  For ARCA: The least significant two bytes contain the
             *  projected auction time as a binary integer in hhmm format.
             *  e.g if the opening auction time is 9:30, a binary value of
             *  930 would be stored in the lower 16-bits of feedSpecificData.
             *  <br><br>
             *  For NYOU: The least significant byte contains the Imbalance
             *  Indicator.<br>
             *  <ul>
             *  <li>For opening imbalance, if this field is 1, it indicates
             *      that the stock is open and no more imbalance messages
             *      will be sent after this.</li><br>
             *  <li>For closing imbalance if this field is 1, it indicates
             *      that the imbalance is regulatory 0 means it is
             *      informational. A space indicates that it is not
             *      applicable.</li>
             *  </ul>
             *  <br><br>
             *  For INET: feedSpecificData equals the far Indicative
             *  clearing price for INET Imbalance Data. The near Indicative
             *  cleaing price is stored in the marketImbalance field.
             *  marketImbalance is not applicable for INET.
             *  For IEX: feedSpecificData contains the scheduled auction
             *  time as seconds since UNIX epoch.
             */
            uint32_t feedSpecificData;

            /**
             *  For NYOU closing imbalance and IEX.
             *
             * For NYOU, the Closing Only Clearing Price is defined in the <a
             * href="www.nyxdata.com/doc/5977">NYSE Order Imbalance
             * specification V1.12</a> as "the closing only interest where
             * price closest to last sale where imbalance is zero."  This field
             * is only available from Turbo servers and will be 0 when not
             * populated.  The priceExponent field provides the appropriate
             * exponent to apply to this field.
             *
             * For IEX, this field contains the auction book clearing price.
             * As per the IEX spec, this is the clearing price using orders on
             * the auction book.
             *
             *  Field is 0 for all other feeds.
             */
            uint32_t closingClearingPriceNumerator;

            MarketImbalance& operator=(const imbalance_msg *p_im);
        };

        /**
         * A container for Limit Up/Limit Down (LULD) Price Band data.
         *
         * <a href="http://resources.limebrokerage.com/client/includes/release-notes/2_1_8.php">See 2.1.8 release notes for more information.</a>
         */
        class PriceBand {
            friend class QuoteSystemApi;
        public:
            /**
             *  Gets unique numeric identifier for the symbol.
             *
             *  Established by call from onAddSymbol().  Please note that
             *  symbol indicies are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t getSymbolIndex() const { return symbolIndex_; }

            /**
             *  Get market timestamp for this LULD price band data.
             */
            const Timestamp& getTimestamp() const { return timestamp_; }

            /**
             *  Get quote source id.
             */
            const std::string& getQuoteSourceId() const { return quoteSourceId_; };

            /**
             *  Get Limit Down Price.
             */
            const Price& getLimitDownPrice() const { return limitDownPrice_; }

            /**
             *  Get Limit Up Price.
             */
            const Price& getLimitUpPrice() const { return limitUpPrice_; }

            /**
             *  Get timestamp that this LULD price band became effective.
             *  Timestamp value is 0 if effective timestamp is unknown.
             */
            const Timestamp& getEffectiveTimestamp() const { return effectiveTimestamp_; }
        private:
            PriceBand();
            PriceBand(const price_band_msg *msg);
            uint32_t symbolIndex_;
            Timestamp timestamp_;
            std::string quoteSourceId_;
            Price limitDownPrice_;
            Price limitUpPrice_;
            Timestamp effectiveTimestamp_;
        };

        /**
         *  A header for security definition data messages -- for future use
         *  with the API.
         */
        struct SecurityDefinitionHeader
        {
            /**
             *  Quote source timestamp expressed in milliseconds since
             *  midnight.
             */
            uint32_t timestamp;

            /**
             *  Quote source timestamp nanoseconds expressed in nanoseconds
             *  since millisecond timestamp.
             */
            uint32_t nsecTimestamp;

            /**
             *  Unique numeric identifier for the traded symbol.
             *
             *  Established by call from onAddSymbol().  Please note that
             *  symbol indicies are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t symbolIndex;

            /**
             *  Lime specific quote source.
             */
            std::string quoteSourceId;

            /**
             * The feed-specific type of the security definition message.
             */
            uint8_t type;

            SecurityDefinitionHeader(const SecurityDefinition& securityDefinition);
        };

        /**
         *  A container for security definition data -- for future use with
         *  the API.
         */
        struct SecurityDefinitionCommonData
        {
            /**
             *  Previous day's closing price.
             */
            double previousDayClosingPrice;

            SecurityDefinitionCommonData(const SecurityDefinition& securityDefinition);
        };

        /**
         * Conveys the last recorded high and low trade prices for this quote
         * server.  Subsequent high/low trade prices can be obtained by
         * monitoring the onTrade() callback for new highs and lows.
         */
        struct SymbolStatus
        {
            /**
             *  Unique numeric identifier for the traded symbol.
             *
             *  Established by call from onAddSymbol().  Please note that
             *  symbol indicies are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t symbolIndex;

            /**
             *  Symbol Type.
             */
            SecurityType symbolType;

            /**
             *  Lime specific quote source.
             */
            std::string quoteSourceId;

            /**
             *  The last recorded high trade price mantissa.
             */
            int32_t highTradePriceMantissa;

            /**
             *  The last recorded high trade price exponent.
             */
            int8_t highTradePriceExponent;

            /**
             *  The last recorded low trade price mantissa.
             */
            int32_t lowTradePriceMantissa;

            /**
             *  The last recorded low trade price exponent.
             */
            int8_t lowTradePriceExponent;

            /**
             * True when the high trade is no longer valid. This can occur
             * when a trade cancel/correction which affects the high trade
             * is received.
             *
             * @deprecated
             * No longer populated with valid values. Do not reference.
             */
            bool isHighTradeInvalid;

            /**
             * True when the low trade is no longer valid. This can occur
             * when a trade cancel/correction which affects the low trade
             * is received.
             *
             * @deprecated
             * No longer populated with valid values. Do not reference.
             */
            bool isLowTradeInvalid;

            /**
             *  The Opening  price mantissa.
             */
            int32_t openPriceMantissa;

            /**
             *  The Opening price exponent.
             */
            int8_t openPriceExponent;

            /**
             *  The closing  price mantissa.
             */
            int32_t closePriceMantissa;

            /**
             *  The closing price exponent.
             */
            int8_t closePriceExponent;

            /**
             * Year of closing price as YYYY (good till 2127)
             */
            uint32_t closePriceYr;

            /**
             * Month of closing price
             */
            uint8_t closePriceMon;

            /**
             * Day of month of closing price
             */
            uint8_t closePriceDay;

            /**
             *  The last  price mantissa.
             */
            int32_t lastPriceMantissa;

            /**
             *  The last price exponent.
             */
            int8_t lastPriceExponent;

            /*
             *  The exchange id that published the price update.
             *  Valid only for UTDF and SIAC for open and close updates from the feed.
             *  This is ' ' (space) when unknown or not populated.
             */
            char exchangeId;

            SymbolStatus(const symbol_status_msg *p_msg);
        };

        /**
         *  Identifier for an option series.  Typically the symbol field is
         *  an Option Class(OPRA root), and the Option Name uniquely
         *  specifies a single option series.
         *
         *  When dealing with new options symbology (OSI), the symbol
         *  field will be the underlying, except in cases involving
         *  corporate actions, when the symbol will probably be underlying
         *  concatenated with a numeric character to distinguish it.
         *
         *  The client application can specify option series to subscribe()
         *  using either OptionName or OpraName.
         *
         * @see
         *  OptionAttributes,
         *  subscribe(std::string, const std::vector<std::string>, SymbolType, int)
         */
        struct OptionName
        {
            /**
             *  OCC symbol, blank padded.
             */
            char symbol[6];

            /**
             *  Numeric expiration year in the format of 'YY'.
             */
            char expiration_year[2];

            /**
             *  Numeric expiration month in the format of 'MM'.
             */
            char expiration_month[2];

            /**
             *  Numeric expiration day in the format of 'DD'.
             */
            char expiration_day[2];

            /**
             *  'P' for put, or 'C' for call.
             */
            char put_call;

            /**
             *  Whole number of strike price.
             */
            char strike_price[5];

            /**
             *  Fraction of strike price.
             */
            char strike_decimal[3];
        };

        /**
         *  Old-style OPRA identifier for an option series.
         *
         *  The OPRA root is 1 to 3 alphabetic characters, followed by an
         *  alphabetic character expressing expiration month and Put/Call,
         *  and another alphabetic character to represent the strike price
         *  code.  This last character will be obsolete in Feb 2010 with the
         *  switchover to OSI symbology.
         *
         *  The client application can specify option series to subscribe()
         *  using either OptionName or OpraName.
         *
         * @see
         *  OptionAttributes,
         *  subscribe(std::string, const std::vector<std::string>, SymbolType, int)
         */
        struct OpraName
        {
            /**
             * Old-style OPRA identifier as described above.
             */
            char name[5];
        };

        /**
         *  Name of underlying security.
         *
         * @see
         *  OptionAttributes
         */
        struct UnderlyingName
        {
            /**
             * Name of underlying security.
             */
            char name[6];
        };

        const std::string exchangeCodes; // "ABCINOPQX"

        std::string exchangeMaskToString(uint16_t quotingExchanges);

        /**
         *  OptionAttributes consist of OptionName, OpraName, openInterest,
         *  and other attributes for the option series.  This object is
         *  passed to client application before any Quote or Trade objects.
         *
         * @see
         *  onOptionAttributes()
         */
        struct OptionAttributes
        {
            OptionAttributes(const option_attributes_message_t *option_attributes_message);

            /**
             *  Lime specific quote source id.
             */
            std::string quoteSource;

            /**
             *  Underlying name, passed to client application via
             *  onOptionAttributes().
             */
            UnderlyingName underlyingName;

            /**
             *  [OptionClass, expiration, P/C, and strike price], passed to
             *  client application via onOptionAttributes().
             */
            OptionName optionName;

            /**
             *  Old-style OPRA symbol, passed to client application via
             *  onOptionAttributes().
             */
            OpraName opraName;

            /**
             *  As reported by OPRA, passed to client application via
             *  onOptionAttributes().
             */
            uint32_t openInterest;

            /**
             *  Unique number to represent the option series (unique within
             *  feed), passed to client application via onOptionAttributes().
             */
            int32_t symbolIndex;

            /**
             *  Bitmask for exchanges quoting this series, passed to client
             *  application via onOptionAttributes().
             */
            uint16_t quotingExchanges;

            /**
             *  Unique number for underlying (unique across feeds), passed
             *  to client application via onOptionAttributes().
             */
            uint32_t underlyingIndex;

            /**
             *  Unique number for option series (unique across feeds), passed
             *  to client application via onOptionAttributes().
             */
            uint32_t seriesIndex;
        };

        /*
         * A container for option analytics (greeks) data
         */
        struct Analytics {
            /**
             * Unique numeric identifier for the traded symbol.
             *
             * Established by call from onAddSymbol().  Please note that
             * symbol indices are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t symbolIndex;

            /**
             * Quote source timestamp expressed in milliseconds since
             * midnight.
             */
            uint32_t timestamp;

            /**
             * Delta, scaled by 10^4
             */
            int32_t delta;

            /**
             * Gamma, scaled by 10^4
             */
            int32_t gamma;

            /**
             * Theta, scaled by 10^4
             */
            int32_t theta;

            /**
             * Delta, scaled by 10^4
             */
            int32_t vega;

            /**
             * Implied volatility %, scaled by 10^4
             */
            uint32_t impliedVolatility;

            /**
             * Option price scaled by 10^4
             */
            uint32_t optionPrice;

            /**
             * Underlying price scaled by 10^4
             */
            uint32_t underlyingPrice;
        };

        /*
         * A container for option shocked values data
         */
        struct ShockedPnl {
            /**
             * Unique numeric identifier for the traded symbol.
             *
             * Established by call from onAddSymbol().  Please note that
             * symbol indices are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t symbolIndex;

            /**
             * Quote source timestamp expressed in milliseconds since
             * midnight.
             */
            uint32_t timestamp;

            /**
             * Implied volatility %, scaled by 10^4
             */
            uint32_t impliedVolatility;

            /**
             * Option price, scaled by 10^4
             */
            uint32_t optionPrice;

            /**
             * Underlying price, scaled by 10^4
             */
            uint32_t underlyingPrice;

            /**
             * 5 negatively-shocked profit & loss values, least to
             * most shocked in intervals of -maxNegativeShockPercent / 5
             *
             * Scaled by 10^4
             */
            int32_t negativelyShockedPnlValues[5];

            /**
             * 5 positively-shocked profit & loss values, least to
             * most shocked, in intervals of maxPositiveShockPercent / 5
             *
             * Scaled by 10^4
             */
            int32_t positivelyShockedPnlValues[5];

            /**
             * Negative shock percent of greatest magnitude, e.g. -15
             */
            int8_t maxNegativeShockPercent;

            /**
             * Positive shock percent of greatest magnitude, e.g. +15
             */
            int8_t maxPositiveShockPercent;
        };

        /**
         * A container for index value data.
         */
        struct Index
        {
            /**
             * Unique numeric identifier for the index symbol.
             *
             * Established by call from onAddSymbol().  Please note that
             * symbol indices are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t symbolIndex;

            /**
             * Index value mantissa.
             */
            int32_t valueMantissa;

            /**
             * Index value exponent.
             */
            int8_t valueExponent;

            /**
             * Lime specific quote source.
             */
            std::string quoteSourceId;

            /**
             * Quote source timestamp expressed in milliseconds since
             * midnight.
             */
            uint32_t timestamp;

            /**
             *  Quote source timestamp nanoseconds expressed in nanoseconds
             *  since millisecond timestamp.
             */
            uint32_t nsecTimestamp;
        };

        /**
         * A container for updates to an auction's information during the auction.
         */
        struct AuctionUpdate
        {
            AuctionUpdate(const auction_update_message_t *auction_update_message);

            /**
             * Unique numeric identifier for the index symbol.
             *
             * Established by call from onAddSymbol().  Please note that
             * symbol indices are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t symbolIndex;

            /**
             * Quote source timestamp expressed in milliseconds since
             * midnight.
             */
            uint32_t timestamp;

            /**
             * Lime specific quote source.
             */
            std::string quoteSourceId;

            /**
             * Provides the type of auction:
             * 'O' = Opening Auction
             * 'C' = Closing Auction
             * 'H' = Halt Auction
             * 'I' = IPO Auction
             */
            uint8_t auctionType;

            /**
             * The BBO Collared auction price mantissa.
             */
            int32_t referencePriceMantissa;

            /**
             * The BBO Collared auction price exponent.
             */
            int8_t referencePriceExponent;

            /**
             * The number of shares on the buy side at the reference price.
             */
            uint32_t buyShares;

            /**
             * The number of shares on the sell side at the reference price.
             */
            uint32_t sellShares;

            /**
             * The mantissa for the price at which the auction book and the 
             * continuous book would match.
             */
            int32_t indicativePriceMantissa;

            /**
             * The exponent for the price at which the auction book and the 
             * continuous book would match.
             */
            int8_t indicativePriceExponent;

            /**
             * The mantissa for the price at which the auction book would
             * match using only Eligible Auction Orders.
             */
            int32_t auctionOnlyPriceMantissa;

            /**
             * The exponent for the price at which the auction book would
             * match using only Eligible Auction Orders.
             */
            int8_t auctionOnlyPriceExponent;
        };

        /**
         * A container for a summary of the results of an auction.
         */
        struct AuctionSummary
        {
            AuctionSummary(const auction_summary_message_t *auction_summary_message);

            /**
             * Unique numeric identifier for the index symbol.
             *
             * Established by call from onAddSymbol().  Please note that
             * symbol indices are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t symbolIndex;

            /**
             * Quote source timestamp expressed in milliseconds since
             * midnight.
             */
            uint32_t timestamp;

            /**
             * Lime specific quote source.
             */
            std::string quoteSourceId;

            /**
             * Provides the type of auction:
             * 'O' = Opening Auction
             * 'C' = Closing Auction
             * 'H' = Halt Auction
             * 'I' = IPO Auction
             */
            uint8_t auctionType;

            /**
             * The mantissa of the resulting price of the auction.
             */
            int32_t priceMantissa;

            /**
             * The exponent of the resulting price of the auction.
             */
            int8_t priceExponent;

            /**
             * The cumulative number of shares executed during the auction.
             */
            uint32_t shares;
        };

        /**
         * Represents a security's trading state and/or Reg SHO short-sale
         * restriction state as disseminated by the quote source.
         *
         * See <a href="http://resources.limebrokerage.com/client/quoteAlertOverview.php">
         * Quote Alert Overview</a> for a detailed explanation.
         *
         * @see onQuoteAlert(const QuoteAlert&)
         *
         * @see Alert
         *
         */
        struct QuoteAlert {
            /**
             *  Quote source timestamp expressed in milliseconds since
             *  midnight.
             */
            uint32_t timestamp;

            /**
             *  Quote source timestamp nanoseconds expressed in nanoseconds
             *  since millisecond timestamp.
             */
            uint32_t nsecTimestamp;

            /**
             *  Unique numeric identifier for the traded symbol.
             *
             *  Established by call from onAddSymbol().  Please note that
             *  symbol indicies are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol(std::string , std::string , int )
             */
            uint32_t symbolIndex;
            /**
             *  Lime specific quote source id.
             */
            std::string quoteSource;
            /**
             * Integer code representing the trading state of the security.
             *
             * See <a href="http://resources.limebrokerage.com/client/quoteAlertOverview.php">
             * Quote Alert Overview</a> for a more detailed description.
             *
             * @see Alert
             */
            int code;
            /**
             * Short Sale Restriction Indicator Reg SHO Rule 201
             *
             * See <a href="http://resources.limebrokerage.com/client/quoteAlertOverview.php">
             * Quote Alert Overview</a> for a more detailed description.
             *
             * @see
             * shortSaleRestrictionActivated
             * shortSaleRestrictionInEffect
             * shortSaleRestrictionContinued
             * shortSaleRestrictionDeactivated
             * shortSaleRestrictionNotInEffect
             * tradingStatusUpdate
             */
            uint8_t shortSaleRestrictionIndicator;
        };

        /**
         * A container for data regarding the three Round Lot Top of Book views.
         */
        class RoundLotTob
        {
        public:
            RoundLotTob& operator=(const RoundLotTobMsg* msg);
            /**
             *  Gets unique numeric identifier for the symbol.
             *
             *  Established by call from onAddSymbol().  Please note that
             *  symbol indicies are quote source specific and change daily.
             *
             * @see
             *  onAddSymbol()
             */
            uint32_t getSymbolIndex() const { return symbolIndex_; }
            /**
             *  Get quote source id.
             */
            const std::string& getQuoteSourceId() const { return quoteSourceId_; }
            /**
             * Aggregated size of the top price level on the book.
             */
            uint32_t getPriceAggregatedTobSize() const { return orders_[PriceAggregatedTob].getSize(); }
            /**
             * Price of the top price level on the book. 
             */
            const Price& getPriceAggregatedTobPrice() const { return orders_[PriceAggregatedTob].getPrice(); }
            /**
             * Total size of all quotes at or above the Odd Lot Aggregated price 
             * level.
             *
             * The Odd Lot Aggregated price level is calculated by aggregating
             * all price levels until the total size is at least the size of a 
             * Round Lot.
             */
            uint32_t getOddLotAggregatedTobSize() const { return orders_[OddLotAggregatedTob].getSize(); }
             /**
             * Price of the furthest quote from the top of book included in the
             * Odd Lot Aggregated Top of Book calculation.
             */
           const Price& getOddLotAggregatedTobPrice() const { return orders_[OddLotAggregatedTob].getPrice(); }
             /**
             * Total aggregated size of all quotes at the Odd Lot Filtered price 
             * level.
             *
             * The Odd Lot Filtered price level is calculated by finding the first
             * aggregated price level which contains at least the size of a Round
             * Lot.
             */
            uint32_t getOddLotFilteredTobSize() const { return orders_[OddLotFilteredTob].getSize(); }
             /**
             * Price of the first aggregated price level which contains at least
             * the size of a Round Lot.
             */
            const Price& getOddLotFilteredTobPrice() const { return orders_[OddLotFilteredTob].getPrice(); }
             /**
              * Indicates the side of the quote.
              */
            uint8_t getSide() const { return side_; }
            /**
             *  Get market timestamp.
             */
            const Timestamp& getTimestamp() const { return timestamp_; }
        private:
            enum OrderType {
                PriceAggregatedTob,
                OddLotAggregatedTob,
                OddLotFilteredTob,
                NumOrderType
            };

            class Order {
            public:
                Order& assign(const Price& price , uint32_t size);

                const Price& getPrice() const {
                    return price_;
                }

                uint32_t getSize() const {
                    return size_;
                }

            private:
                Price    price_;
                uint32_t size_;
            };

            uint32_t    symbolIndex_;
            std::string quoteSourceId_;
            Order       orders_[NumOrderType];
            uint8_t     side_;
            Timestamp timestamp_;
        }; //RoundLotTob

    private:
        std::string quoteSourceId;
        LoginStatus loginStatus;
        std::string username;
        std::string password;
        bool isExpress;
        SubscriptionStatus subscriptionStatus_;
        bool tradesOnlySession;
        bool ipoAndUplisted;
        bool aggregatedBookSession;
        bool extendedQuoteSession;
        bool topOfBookSession;
        bool roundLotTobSession;
        std::vector< DynamicArray<std::string> *> symbolVector;
        bool defaultSessionManager;
        SessionManager *pSessionManager;
        friend class SessionManager;

        int sockDesc;
        bool setSockReadable(bool sockReadable)
        {
            if (sockReadable == this->sockReadable) {
                return sockReadable;
            } else {
                this->sockReadable = sockReadable;
                return !sockReadable;
            }
        }

        bool isSockReadable() const { return sockReadable; }
        bool sockReadable;

        int bytesAvail;
        int totalMessages;

        Mutex  logoutLock;
        Event  isLoggedOut;

        //
        // The following variables are used to support optional field message processing.
        //

        // Stores the msg type of the previous message.
        uint8_t lastMsgType_;

        // True if we are waiting for a boundary message before issuing the next callback.
        bool waitingForBoundary_;

        Analytics analytics_;
        ShockedPnl shockedPnl_;
        Trade trade_;
        Quote quote_;
        Quote quoteToDelete_;
        Trade tc_original_trade_;
        Trade tc_modified_trade_;
        QuoteAlert quoteAlert_;
        MarketImbalance mi_;
        RoundLotTob roundLotTob_;
        int enabledFeatures_;

        int ver1;
        int ver2;
        int ver3;
        int revis;

        struct ServerVersion {
        public:
            ServerVersion(int major_, int minor_): majorVer(major_), minorVer(minor_) {}
            int getMajor() { return majorVer; }
            int getMinor() { return minorVer; }
        private:
            int majorVer;
            int minorVer;
        }serverVersion;

        SecurityType defaultSecType;

    public:

        int getSockDesc() const {
            return sockDesc;
        }

        /**
         *  Get the SessionManager associated with this instance of the API.
         *
         * @return
         *  SessionManager associated with this instance of the API.
         */
        SessionManager* getSessionManager();

        /**
         *  Set the SessionManager associated with this instance of the API.
         *  You can only set the SessionManager while not logged in.
         *
         * @param sm
         *  Instance of SessionManager to associate with this instance of the
         *  API.
         *
         * @return
         *  0 on success, -1 on error
         */
        int setSessionManager(SessionManager *sm);

        /**
         *  Get the total number of messages that have been received from the
         *  server.  This function is particularly useful after logout() to
         *  verify that we have received all the messages that the quote
         *  server sent us.  The callback routine onLimeqControlMessage() can
         *  be overriden to get the total number of messages sent by the
         *  server after logout.
         *
         * @return
         *  The number of messages received from the server.
         */
        int getTotalMessagesReceived() const
        {
            return totalMessages;
        }

        StringDynamicArray &getSymbolsArray(size_t index) {
            return *(symbolVector.at(index));
        }
    };

    inline std::string& LimeBrokerage::QuoteSystemApi::getSymbol(uint32_t symbol_index, SecurityType symbol_type) {
        static std::string invalid_sym("?Sym#%d?");

        if(symbol_type < securityTypeMin || symbol_type >= securityTypeMax) {
            return invalid_sym;
        }

        DynamicArray<std::string> *pSymArr;
        if(!(pSymArr = symbolVector.at(symbol_type))) {
            return invalid_sym;
        }

        std::string &sym = (*pSymArr)[symbol_index];
        if (!sym.length()) {
            sym = invalid_sym;
        }
        return sym;
    }

}; // namespace LimeBrokerage

namespace {
    static const double powers_of_ten[] = {
        1.0,
        10.0,
        100.0,
        1000.0,
        10000.0,
        100000.0,
        1000000.0,
        10000000.0,
        100000000.0,
        1000000000.0
    };

    /* Not Part of API
     *  Converts a price and exponent into a double using the powers_of_ten[]
     *  table.
     *
     * @param price
     *  The mantissa part of a price, where <tt>price = mantissa x
     *  10^exponent</tt>.
     *
     * @param exponent
     *  The exponents part of a price, where <tt>price = mantissa x
     *  10^exponent</tt>.
     *
     * @return
     *  The double representing the price where <tt>price = mantissa x
     *  10^exponent</tt>.
     */
    inline double priceToDouble(int32_t price, int8_t exponent) {
        int32_t max_exponent = sizeof(powers_of_ten)/sizeof(double);
        if (exponent < -1 * max_exponent || exponent > max_exponent) {
            //Invalid price conversion
            return -1;
        }
        if (exponent < 0) {
            return (double)price / powers_of_ten[-1 * exponent];
        }
        return (double)price * powers_of_ten[exponent];
    }

    inline void populateStringIgnoreSpaces(std::string& outstr, const char *str, int max) {
        int i = 0;
        for (i = 0; i < max && str[i] != ' '; i++);
        outstr.assign(str, 0, i);
    }
}

#endif  // LIME_BROKERAGE_QUOTE_SYSTEM_API_HH
