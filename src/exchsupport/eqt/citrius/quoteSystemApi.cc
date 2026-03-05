//
// "quoteSystemApi.cc" -- API for the LimeQ-based Citrius quote server.
//

// Standard C++ headers.
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

// Standard POSIX headers.
#ifdef __linux__
#include <cerrno>
// extern "C"
// {
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <pthread.h>
#include <endian.h>
// }

#ifndef be64toh
#define be64toh(x) __bswap_64 (x)
#endif /* be64toh */

#endif /* __linux__ */

#ifdef WIN32
#define NOMINMAX
extern "C"
{
#include <string.h>
}
#endif /* WIN32 */

#ifdef __GNUC__
extern "C"
{
#include <byteswap.h>
}
#endif /* __GNUC__ */

// Lime Brokerage headers.
#include "util.hh"
#include "quoteSystemApi.hh"
#include "dynamicArray.hh"
#include "sessionManager.hh"
#include "version.h"

// Host to network byte-order
inline uint16_t hostToNetwork16(uint16_t x) { return htons(x); }
inline uint32_t hostToNetwork32(uint32_t x) { return htonl(x); }
inline uint64_t hostToNetwork64(uint64_t x)
{
#ifdef __GNUC__
    return (__BYTE_ORDER == __BIG_ENDIAN) ? x : bswap_64(x);
#else
    return (htonl(1) == 1) ? x : ((uint64_t(ntohl(x)) << 32) | ntohl(x >> 32));
#endif
}

// Network to host byte-order
inline uint16_t networkToHost16(uint16_t x) { return ntohs(x); }
inline uint32_t networkToHost32(uint32_t x) { return ntohl(x); }
inline uint64_t networkToHost64(uint64_t x)
{
#ifdef __GNUC__
    return (__BYTE_ORDER == __BIG_ENDIAN) ? x : bswap_64(x);
#else
    return (htonl(1) == 1) ? x : ((uint64_t(ntohl(x)) << 32) | ntohl(x >> 32));
#endif
}

// Static data member initialization.
const int      LimeBrokerage::QuoteSystemApi::majorVersion     = 2;
const int      LimeBrokerage::QuoteSystemApi::minorVersion     = 0;
const int      LimeBrokerage::QuoteSystemApi::heartbeat        = 30;
const int      LimeBrokerage::QuoteSystemApi::heartbeatTimeout = 90;
const uint32_t LimeBrokerage::QuoteSystemApi::maxEquitySymbols = 16 * 1024;
const uint32_t LimeBrokerage::QuoteSystemApi::maxSymbols       = 128 * maxEquitySymbols;
const int      LimeBrokerage::QuoteSystemApi::minMessageLength = sizeof(uint16_t) + sizeof(uint8_t); // At least 1 byte + 2 byte header.

const std::string LimeBrokerage::QuoteSystemApi::NYSE_OB("NYSE_OB");
const std::string LimeBrokerage::QuoteSystemApi::NYOB("NYOB");
const std::string LimeBrokerage::QuoteSystemApi::Level1("Level1");
const std::string LimeBrokerage::QuoteSystemApi::LONE("LONE");
const std::string LimeBrokerage::QuoteSystemApi::Level2("Level2");
const std::string LimeBrokerage::QuoteSystemApi::LTWO("LTWO");
const std::string LimeBrokerage::QuoteSystemApi::CFUT("CFUT");
const std::string LimeBrokerage::QuoteSystemApi::CSTK_FUT("CSTK_FUT");

const std::vector<std::string> LimeBrokerage::QuoteSystemApi::emptyVector(0);

//
// Utility functions.
//

// nullString - a constant null (empty) string.

namespace
{
    const std::string nullString;
}

LimeBrokerage::QuoteSystemApi::RoundLotTob&
LimeBrokerage::QuoteSystemApi::RoundLotTob::operator=(const RoundLotTobMsg* msg)
{
    symbolIndex_ = ntohl(msg->symbolIndex);
    populateStringIgnoreSpaces(quoteSourceId_, msg->quoteSourceId, sizeof(msg->quoteSourceId));
    timestamp_ = networkToHost64(msg->timestamp);
    side_ = msg->side;
    orders_[PriceAggregatedTob].assign(Price(ntohl(msg->priceAggregatedTobMantissa), msg->priceAggregatedTobExponent), ntohl(msg->priceAggregatedTobSize));
    orders_[OddLotAggregatedTob].assign(Price(ntohl(msg->oddLotAggregatedTobMantissa), msg->oddLotAggregatedTobExponent), ntohl(msg->oddLotAggregatedTobSize));
    orders_[OddLotFilteredTob].assign(Price(ntohl(msg->oddLotFilteredTobMantissa), msg->oddLotFilteredTobExponent), ntohl(msg->oddLotFilteredTobSize));
    return *this;
}

LimeBrokerage::QuoteSystemApi::RoundLotTob::Order&
LimeBrokerage::QuoteSystemApi::RoundLotTob::Order::assign(const Price& price, uint32_t size)
{
    price_ = price;
    size_ = size;
    return *this;
}

// isValidSymbol - rudimentary stock symbol validation.

inline bool LimeBrokerage::QuoteSystemApi::isValidSymbol(std::string symbol)
{
    return symbol.length() > 0 && symbol.length() <= SYMBOL_LEN;
}

std::string LimeBrokerage::QuoteSystemApi::exchangeMaskToString(uint16_t quotingExchanges)
{
    std::string str = exchangeCodes;
    for (size_t j=1; j<=str.size(); j++)
    {
        if (!(quotingExchanges & (1<<j)))
        {
            str[j-1]=' ';
        }
    }
    return str;
}

LimeBrokerage::QuoteSystemApi::QuoteSystemApi(SessionManager *pMgr) :
    socketProcessor(new SocketProcessor()),
    exchangeCodes("ABCINOPQXZ"),
    quoteSourceId(nullString),
    loginStatus(LS_LOGGED_OUT),
    isExpress(false),
    subscriptionStatus_(SUBSCRIPTION_STATUS_UNSUBSCRIBED),
    tradesOnlySession(false),
    aggregatedBookSession(false),
    extendedQuoteSession(false),
    topOfBookSession(false),
    ipoAndUplisted(false),
    roundLotTobSession(false),
    symbolVector(securityTypeMax),
    defaultSessionManager(false),
    pSessionManager(pMgr),
    sockDesc(0),
    sockReadable(false),
    bytesAvail(0),
    totalMessages(0),
    logoutLock(false),
    isLoggedOut(&logoutLock),
    lastMsgType_(0),
    waitingForBoundary_(false),
    enabledFeatures_(0),
    ver1(0),
    ver2(0),
    ver3(0),
    revis(0),
    serverVersion(0,0),
    defaultSecType(securityTypeMax)
{
    populateVersion();
    // create my own session manager
    if(!pSessionManager) {
        // default session manager runs in its own thread
        defaultSessionManager = true;
        pSessionManager = new SessionManager();
        pSessionManager->start();
    }

    for(size_t securityType = securityTypeMin; securityType < securityTypeMax; securityType++)
        symbolVector.at(securityType) = NULL;

    resetObject(quote_);
    resetObject(trade_);
}

LimeBrokerage::QuoteSystemApi::QuoteSystemApi(int cpu):
    socketProcessor(new SocketProcessor()),
    exchangeCodes("ABCINOPQXZ"),
    quoteSourceId(nullString),
    loginStatus(LS_LOGGED_OUT),
    isExpress(false),
    subscriptionStatus_(SUBSCRIPTION_STATUS_UNSUBSCRIBED),
    tradesOnlySession(false),
    aggregatedBookSession(false),
    extendedQuoteSession(false),
    topOfBookSession(false),
    roundLotTobSession(false),
    symbolVector(securityTypeMax),
    defaultSessionManager(true),
    pSessionManager(new SessionManager(cpu)),
    sockDesc(0),
    sockReadable(false),
    bytesAvail(0),
    totalMessages(0),
    logoutLock(false),
    isLoggedOut(&logoutLock),
    lastMsgType_(0),
    waitingForBoundary_(false),
    enabledFeatures_(0),
    ver1(0),
    ver2(0),
    ver3(0),
    revis(0),
    serverVersion(0,0),
    defaultSecType(securityTypeMax)
{
    populateVersion();
    if(defaultSessionManager)
        pSessionManager->start();

    for(size_t securityType = securityTypeMin; securityType < securityTypeMax; securityType++)
        symbolVector.at(securityType) = NULL;

    resetObject(quote_);
    resetObject(trade_);
}

LimeBrokerage::QuoteSystemApi::~QuoteSystemApi()
{
    cleanup();
    // if I created the SessionManager destroy it
    if(defaultSessionManager) {
        pSessionManager->shutdown();
        pSessionManager->join();
        delete pSessionManager;
    }

    delete socketProcessor;
}

int LimeBrokerage::QuoteSystemApi::open(bool blocking)
{
    if (!socketProcessor->open(blocking)) {
        onQuoteSystemApiError(nullString, ioError, "Unable to open socket");
        socketProcessor->close();
        return LOGIN_FAILURE;
    }
    return LOGIN_PENDING;
}

int LimeBrokerage::QuoteSystemApi::connect(std::string host)
{
    //Assuming api already logged out reset totalMessages_
    totalMessages = 0;
    std::ostringstream  errorMsg;
    // Create the socket to be used to communicate with the quote server and
    // connect to the quote server port on the given host.

    int port = QUOTE_SERVER_PORT;
    std::string hostName = host;
    size_t portLoc = host.find(':');
    if (portLoc != std::string::npos) {
        hostName = host.substr(0, portLoc);
        port = std::atoi(host.substr(portLoc + 1, host.length()).data());
    }

    sockDesc = socketProcessor->getFileDesc();
    if (!socketProcessor->isValid()) {
        errorMsg << "Connection closed";
        onQuoteSystemApiError(nullString, ioError, errorMsg.str());
        socketProcessor->close();
        return LOGIN_FAILURE;
    }
    Socket::ConnectionState res = socketProcessor->connect(hostName, port);
    if (res == Socket::connectionStateFailure) {
        errorMsg << "Unable to connect to: " << hostName << ":" << port;
        onQuoteSystemApiError(nullString, ioError, errorMsg.str());
        socketProcessor->close();
        return LOGIN_FAILURE;
    }

    if (res == Socket::connectionStatePending) return LOGIN_PENDING;
    if (res == Socket::connectionStateConnected) return LOGIN_SUCCESS;
    return LOGIN_FAILURE;
}

int LimeBrokerage::QuoteSystemApi::sendLoginRequest()
{
    // Username/password sanity checks.
    if (username.length() > UNAME_LEN  || password.length() > PASSWD_LEN) {
        onQuoteSystemApiError("", invalidUserNamePassword, "Username and/or password exceeds max length");
        return LOGIN_FAILURE;
    }
    // Send a login request message.
    login_request_msg msg;
    memset (&msg, 0, sizeof(msg));
    msg.msg_len = htons(sizeof(login_request_msg));
    msg.msg_type = LOGIN_REQUEST;
    msg.auth_type = CLEAR_TEXT;

    if (isExpress) msg.expressSession = 1;
    else msg.expressSession = 0;

    strncpy(msg.uname, username.c_str(), UNAME_LEN);
    strncpy(msg.passwd, password.c_str(), PASSWD_LEN);
    memset(&msg.host_id, 0, HOST_ID_LEN);

    msg.session_type = CPP_API;
    msg.heartbeat_interval = heartbeat;
    msg.timeout_interval = heartbeatTimeout;
    msg.ver_major = majorVersion;
    msg.ver_minor = minorVersion;

    if (socketProcessor->write((char *)&msg, sizeof (login_request_msg)) != sizeof(login_request_msg)) {
        std::string errorMsg = "Unable to send login message. Reason: ";
        errorMsg += Utility::getErrorMessage();
        onQuoteSystemApiError(nullString, ioError, errorMsg);
        return LOGIN_FAILURE;
    }
    return LOGIN_SUCCESS;
}

int LimeBrokerage::QuoteSystemApi::sendAuthRequest()
{
    auth_request_msg msg;
    memset (&msg, 0, sizeof(msg));
    msg.msg_len = htons(sizeof(auth_request_msg));
    msg.msg_type = AUTH_REQUEST;
    msg.session_type = CPP_API;
    msg.heartbeat_interval = heartbeat;
    msg.timeout_interval = heartbeatTimeout;
    msg.ver_1= ver1;
    msg.ver_2= ver2;
    msg.ver_3= ver3;
    msg.revis= revis;
    ::memcpy(msg.token, password.data(), password.size());
    ::memcpy(msg.uname, username.data(), username.size());

    if (socketProcessor->write((char *)&msg, sizeof (auth_request_msg)) != sizeof(auth_request_msg)) {
        std::string errorMsg = "Unable to send login message. Reason: ";
        errorMsg += Utility::getErrorMessage();
        onQuoteSystemApiError(nullString, ioError, errorMsg);
        return LOGIN_FAILURE;
    }
    return LOGIN_SUCCESS;
}

int LimeBrokerage::QuoteSystemApi::recvLoginResponse(std::string username)
{
    // Read login accept message.
    // To make sure we don't read more messages it needs to be done in 2 steps:

    // First read the header to determine message type and length

    char login_resp_buffer[1024] = { 0 };
    int bytesRead = recv(sockDesc, login_resp_buffer, HEADER_LEN, 0);
    if (bytesRead < 0) {
        return Utility::isErrorWouldBlock() ? LOGIN_PENDING : LOGIN_FAILURE;
    }
    if (bytesRead < (int)HEADER_LEN) {
        //In unlikely case we read less then the header (3 bytes) return error don't bother
        std::ostringstream ostr;
        ostr << "Unable to read login response: " << errno << ": " << strerror(errno);
        onQuoteSystemApiError(nullString, unknownError, ostr.str());
        return LOGIN_FAILURE;
    }

    limeq_header* h = (limeq_header*)login_resp_buffer;

    std::ostringstream  errorMsg;
    if (h->msg_type != LOGIN_RESPONSE) {
        errorMsg << "Unexpected response from quote server, expected login response message < "
                << (char) LOGIN_RESPONSE
                << "> , got "
                << (char)h->msg_type;
        onQuoteSystemApiError(nullString, unknownError, errorMsg.str());
        return LOGIN_FAILURE;
    }

    //Now it is time to read the rest of the response message based on the message length in the header
    int response_len = ntohs(h->msg_len) - HEADER_LEN;
    bytesRead = recv(sockDesc, login_resp_buffer + HEADER_LEN, ntohs(h->msg_len) - HEADER_LEN, 0);
    if (bytesRead < response_len) {
        //In unlikely case we read less then full message return error
        std::ostringstream ostr;
        ostr << "Unable to read login response: " << errno << ": " << strerror(errno);
        onQuoteSystemApiError(nullString, unknownError, ostr.str());
        return LOGIN_FAILURE;
    }
    login_response_msg *resp_msg = (login_response_msg *)h;

    int res = 0;
    switch(resp_msg->response_code)
    {
    case INVALID_UNAME_PASSWORD:
        onQuoteSystemApiError(nullString, invalidUserNamePassword, "Invalid username or password");
        res = LOGIN_FAILURE;
        break;
    case VERSION_MISMATCH:
        errorMsg << "Version mismatch. Server Version = "
        << (int)resp_msg->ver_major << "." << (int)resp_msg->ver_minor << ", API version = "
        << (int)majorVersion << "." << (int)minorVersion;
        onQuoteSystemApiError(nullString, versionMismatch,  errorMsg.str());
        res = LOGIN_FAILURE;
        break;
    case REWIND:
        onQuoteSystemApiError(nullString, serverRewind, "Quote Server Rewind feed data");
        res = LOGIN_FAILURE;
        break;
    case ADMIN_DISABLED:
        onQuoteSystemApiError(nullString, serverDisable, "Quote Server disabled");
        res = LOGIN_FAILURE;
        break;
    case ACCOUNT_DISABLED:
        errorMsg << "Account <" << username <<  "> is disabled";
        onQuoteSystemApiError(nullString, accountDisable, errorMsg.str());
        res = LOGIN_FAILURE;
        break;
    case INSUFFICIENT_PRIVILEGE:
        errorMsg << "Account <"  << username << "> has insufficient privileges";
        onQuoteSystemApiError(nullString, insufficientPrivileges, errorMsg.str());
        res = LOGIN_FAILURE;
        break;
    case INVALID_IP:
        onQuoteSystemApiError(nullString, ipAddressOutOfRange, "IP address validation failed");
        res = LOGIN_FAILURE;
        break;
    case EXCEEDED_MAX_SESSIONS:
        onQuoteSystemApiError(nullString, exceededMaxSessions, "Exceeded maximum number of sessions for the account");
        res = LOGIN_FAILURE;
        break;
    case LOGIN_FAILED:
        onQuoteSystemApiError(nullString, unknownError, "Login failed");
        res = LOGIN_FAILURE;
        break;
    case INVALID_SERVER_GROUP:
        onQuoteSystemApiError(nullString, invalidServerGroup, "Invalid server group");
        res = LOGIN_FAILURE;
        break;
    case INVALID_AUTH_METHOD:
        onQuoteSystemApiError(nullString, invalidAuthMethod, "Traditional authentication method is not supported. ACS access is required");
        res = LOGIN_FAILURE;
        break;
    case USERNAME_MISMATCH:
        onQuoteSystemApiError(nullString, usernameMismatch, "Username provided in the authentication token doesn't agree with user name provided in the login call");
        res = LOGIN_FAILURE;
        break;
    case INVALID_TOKEN_SIGNATURE:
        onQuoteSystemApiError(nullString, invalidTokenSignature, "Authentication token signature is invalid");
        res = LOGIN_FAILURE;
        break;
    case MARKET_DATA_SERVICE_MISMATCH:
        onQuoteSystemApiError(nullString, marketDataServiceMismatch, "Login attempt to incorrect market data service");
        res = LOGIN_FAILURE;
        break;
    case IP_ADDRESS_MISMATCH:
        onQuoteSystemApiError(nullString, ipAddressMismatch, "Client IP address mismatch");
        res = LOGIN_FAILURE;
        break;
    case AUTH_TOKEN_EXPIRED:
        onQuoteSystemApiError(nullString, authTokenExpired, "Authentication token expired");
        res = LOGIN_FAILURE;
        break;
    case DUPLICATE_TOKEN:
        onQuoteSystemApiError(nullString, duplicateToken, "Duplicate authentication token");
        res = LOGIN_FAILURE;
        break;
    case LOGIN_SUCCEEDED:
        loginStatus = LS_LOGGED_IN;
        serverVersion = ServerVersion(resp_msg->ver_major, resp_msg->ver_minor);
        socketProcessor->setNonBlocking();
        bytesAvail = 0;
        pSessionManager->registerApiInstance(this);
        res = LOGIN_SUCCESS;
        break;
    default :
        errorMsg << "Unknown response code '" << (char) resp_msg->response_code << "'";
        onQuoteSystemApiError(nullString, unknownError, errorMsg.str());
        res = LOGIN_FAILURE;
    }
    if (res != LOGIN_SUCCESS) {
        socketProcessor->close();
    }
    return res;
}

int LimeBrokerage::QuoteSystemApi::processSessionState(std::string host, bool isBlockingLogin, bool isLoginRequest)
{
    int res = LOGIN_SUCCESS;
    switch (loginStatus) {
    case LS_LOGGED_OUT:
        res = open(isBlockingLogin);
        if (res != LOGIN_FAILURE) {
            setLoginStatus(LS_CONNECTING);
        }
        else break;
    case LS_CONNECTING:
        res = connect(host);
        if (res == LOGIN_SUCCESS) {
            setLoginStatus(LS_SEND_LOGIN_REQUEST);
        }
        else break;
    case LS_SEND_LOGIN_REQUEST:
        if (isLoginRequest)
            res = sendLoginRequest();
        else
            res = sendAuthRequest();
        if (res == LOGIN_SUCCESS) {
            setLoginStatus(LS_RECV_LOGIN_RESPONSE);
        }
        else break;
    case LS_RECV_LOGIN_RESPONSE:
        res = recvLoginResponse(username);
        if (res == LOGIN_SUCCESS) {
            setLoginStatus(LS_LOGGED_IN);
            return LOGIN_SUCCESS;
        }
        else break;
    case LS_LOGGED_IN:
        onQuoteSystemApiError(nullString, unknownError, "Already logged in");
        res = LOGIN_SUCCESS;
        break;
    case LS_LOGOUT_PENDING:
        onQuoteSystemApiError(nullString, unknownError, "Logout pending");
        res = LOGIN_FAILURE;
        break;
    }
    if (res == LOGIN_FAILURE) {
        socketProcessor->close();
        setLoginStatus(LS_LOGGED_OUT);
    }
    return res;
}

/*
 * This call go through login state machine to allow non blocking login
 * switch statement intentionally don't have breaks to allow going from one state to the next
 * Calling login after api already logged return success, but calls onQuoteSystemApiError callback
 */
int LimeBrokerage::QuoteSystemApi::login(std::string host, std::string username, std::string password, bool isBlockingLogin, bool isExpress) {
    this->username = username;
    this->password = password;
    this->isExpress = isExpress;
    //If password size >= PASSWD_LEN(32 bytes) it is used as authentication token, otherwise traditional login method to call password
    return processSessionState(host, isBlockingLogin, (this->password.size() > PASSWD_LEN ? false : true));
}

int LimeBrokerage::QuoteSystemApi::login(std::string host, std::string username, std::string password, bool isBlockingLogin)
{
    return login(host, username, password, isBlockingLogin, false);
}

int LimeBrokerage::QuoteSystemApi::login(std::string host, std::string username, std::string password)
{
    return login(host, username, password, true);
}

int LimeBrokerage::QuoteSystemApi::logout()
{
    return logout(true);
}

int LimeBrokerage::QuoteSystemApi::logout(bool immediateLogout)
{
    if (!loggedIn()) {
        //onQuoteSystemApiError("", unknownError, "Already logged out session");
        return -1;
    }

    // Send a logout message.

    loginStatus = LS_LOGOUT_PENDING;
    logout_request_msg logoutRequestMessage = { htons(sizeof(logout_request_msg)), LOGOUT_REQUEST, 0 };
    socketProcessor->write((char *)&logoutRequestMessage, sizeof(logout_request_msg));

    /* logout the client immediately instead of waiting for confirmation from the server */
    if(immediateLogout) {
        std::string qsId = quoteSourceId;
        cleanup();
        onQuoteStreamConnectionStatus(qsId, quoteServerDisconnected, nullString);
    }

    return 0;
}

int LimeBrokerage::QuoteSystemApi::genericFeatureEnable(int code)
{
    int len = sizeof(feature_control_message);

    if (!loggedIn()) {
        onQuoteSystemApiError(quoteSourceId, notLoggedIn, "API is not logged in yet");
        return -1;
    }

    if (subscribed()) {
        onQuoteSystemApiError(quoteSourceId, alreadySubscribed, "API is already subscribed");
        return -1;
    }

    feature_control_message message = { htons(len), FEATURE_CONTROL, 0, htons(code), ""};
    socketProcessor->write((char *)&message, len);

    return 0;
}

int LimeBrokerage::QuoteSystemApi::enableTradeReplay()
{
    return genericFeatureEnable(FEATURE_TRADE_REPLAY);
}

int LimeBrokerage::QuoteSystemApi::enableTradesOnly()
{
    int result = genericFeatureEnable(FEATURE_TRADES_ONLY);
    if (result == 0) {
        tradesOnlySession = true;
    }
    return result;
}

int LimeBrokerage::QuoteSystemApi::enableIpoAndUplisted()
{
    int result = genericFeatureEnable(FEATURE_ENABLE_ENABLE_IPO_AND_UPLISTED);
    if (result == 0) {
        ipoAndUplisted = true;
    }
    return result;
}

int LimeBrokerage::QuoteSystemApi::enableTopOfBook()
{
    int result = genericFeatureEnable(FEATURE_TOP_OF_BOOK);
    if (result == 0) {
        topOfBookSession = true;
    }
    return result;
}

int LimeBrokerage::QuoteSystemApi::enableAggregateByPrice()
{
    int result = genericFeatureEnable(FEATURE_PRICE_AGGREGATED);
    if (result == 0) {
        aggregatedBookSession = true;
    }
    return result;
}

int LimeBrokerage::QuoteSystemApi::enableExtendedQuote()
{
    int result = genericFeatureEnable(FEATURE_EXTENDED_QUOTE);
    if (result == 0) {
        extendedQuoteSession = true;
    }
    return result;
}

int LimeBrokerage::QuoteSystemApi::enableSnapshotBook()
{
    return genericFeatureEnable(FEATURE_SNAPSHOT_BOOK);
}

int LimeBrokerage::QuoteSystemApi::enableNagle()
{
    return genericFeatureEnable(FEATURE_ENABLE_NAGLE);
}

int LimeBrokerage::QuoteSystemApi::enableRoundLotTob()
{
    int result = genericFeatureEnable(FEATURE_ROUND_LOT_TOB);
    if (result == 0) {
        roundLotTobSession = true;
    }
    return result;
}

int LimeBrokerage::QuoteSystemApi::enableSimpleFeatures(int featureCodeMask)
{
    if (featureCodeMask & FEATURE_TRADE_REPLAY)
        enableTradeReplay();

    if (featureCodeMask & FEATURE_TRADES_ONLY)
        enableTradesOnly();

    if (featureCodeMask & FEATURE_TOP_OF_BOOK)
        enableTopOfBook();

    if (featureCodeMask & FEATURE_ENABLE_ENABLE_IPO_AND_UPLISTED)
        enableIpoAndUplisted();

    if (featureCodeMask & FEATURE_PRICE_AGGREGATED)
        enableAggregateByPrice();

    if (featureCodeMask & FEATURE_SNAPSHOT_BOOK)
        enableSnapshotBook();

    if (featureCodeMask & FEATURE_ORDER_VISIBILITY)
        onQuoteSystemApiError(quoteSourceId, featureDisabled, "The order visibility feature has been deprecated");

    if (featureCodeMask & FEATURE_BROKER_IDS)
        onQuoteSystemApiError(quoteSourceId, featureDisabled, "The Broker IDs feature has been deprecated");

    if (featureCodeMask & FEATURE_USEC_TIMESTAMP)
        onQuoteSystemApiError(quoteSourceId, featureDisabled, "The optional Usec Timestamp feature has been deprecated");

    return 0;
}

int LimeBrokerage::QuoteSystemApi::filterMMID(bool exclude, const std::vector<std::string>& mmidList)
{
    if (!loggedIn()) {
        onQuoteSystemApiError(quoteSourceId, notLoggedIn, "API is not logged in yet");
        return -1;
    }

    // We can only fit 12 MMIDs in one feature control message.
    if (mmidList.size() > 12) {
        onQuoteSystemApiError(quoteSourceId, unknownError, "Maximum number of MMIDs: 12");
        return -1;
    }

    int len = sizeof(feature_control_message);
    feature_control_message message = { htons(len), FEATURE_CONTROL, 0, htons(FEATURE_FILTER_MMID), "" };

    int offset = 0;
    message.feature_specific_data[offset++] = exclude ? 'E' : 'I';
    message.feature_specific_data[offset++] = (uint8_t)mmidList.size();

    for (unsigned int i = 0; i < mmidList.size(); i++) {
        int mmidLen = 0;
        for (int j = 0; j < MMID_LEN; j++) {
            // Copy MMID until the first space or null character.
            if (mmidList[i][j] == ' ' || mmidList[i][j] == '\0') {
                break;
            }
            message.feature_specific_data[offset++] = mmidList[i][j];
            mmidLen++;
        }
        while (mmidLen < MMID_LEN) {
            // Null-padding.
            message.feature_specific_data[offset++] = '\0';
            mmidLen++;
        }
    }

    socketProcessor->write((char *)&message, len);

    return 0;
}

void LimeBrokerage::QuoteSystemApi::joinReceiver() {
    if(logoutLock.lock() == 0) {
        while(!loggedOut() || pSessionManager->isRegistered(this)){
            isLoggedOut.wait() ;
        }
        logoutLock.unlock();
    }
}

// Private auxiliary methods.

inline void LimeBrokerage::QuoteSystemApi::handleOrderReplace(const Quote& oldOrder, const Quote& newOrder)
{
    //Server doesn't send orderReplace messages to trades only or aggregated book sessions.
    //it doesn't send order replace to top of book sessions if new order is not top of book
    //If old order was top of book, but new is not server won't send order replace message,
    //it will send another quote that became top of book quote
    //For topOfBookSession don't forward oldOrder (it is delete
    //and we don't care about them for top of book sessions
    if (topOfBookSession) {
        onQuote(newOrder);
    } else {
        onOrderReplace(oldOrder, newOrder);
    }
}

inline void LimeBrokerage::QuoteSystemApi::handleModifyExecution(const Quote& quote, const Trade& trade)
{
    if ((topOfBookSession && ! quote.isTopOfBook) || aggregatedBookSession || tradesOnlySession) {
        //Server always forward modify execution messages to deliver trades to every session
        //In case it is tradesOnlySession dont send quote
        //In case it is aggregatedBookSession quote will be sent separately so don't send the quote
        //In case of top of book session make sure quote is top of book quote, otherwise don't send it
        //Don't send the quote
        onTrade(trade);
    }
    else {
        onModifyExecution(quote, trade);
    }
}

inline const std::string& LimeBrokerage::QuoteSystemApi::translateQuoteSource(std::string& quoteSource)
{
    if (quoteSource.length() > 4) {
        if (quoteSource == NYSE_OB)    return NYOB;
        if (quoteSource == Level1)     return LONE;
        if (quoteSource == Level2)     return LTWO;
        if (quoteSource == CSTK_FUT)   return CFUT;
    }
    return quoteSource;
}

inline int LimeBrokerage::QuoteSystemApi::writeSubRequest(uint16_t symbolLen, int subscriptionFlags, std::string& qsid, uint16_t numSymbols, char *symbols)
{
    subscription_request_msg msg;
    memset(&msg, 0, sizeof(subscription_request_msg));
    msg.msg_len = htons(symbolLen + sizeof(subscription_request_msg));
    msg.msg_type = SUBSCRIPTION_REQUEST;
    msg.flags = subscriptionFlags;
    strncpy(msg.qsid, qsid.c_str(), QUOTE_SOURCE_NAME_LEN);
    msg.num_symbols = htons(numSymbols);
    if (socketProcessor->write((char *)&msg, sizeof(subscription_request_msg)) < 0) {
        onQuoteSystemApiError(qsid, ioError, Utility::getErrorMessage());
        return -1;
    }
    if (socketProcessor->write(symbols, symbolLen) < 0) {
        onQuoteSystemApiError(qsid, ioError, Utility::getErrorMessage());
        return -1;
    }
    return 0;
}

inline int LimeBrokerage::QuoteSystemApi::sendSubscribeUnsubscribe(std::string quoteSource, const std::vector<std::string>& symbols,
                                                                   bool subscribe, SymbolType symbolType, int subscriptionTypeMask)
{
    if (!loggedIn()) {
        onQuoteSystemApiError(quoteSource, notLoggedIn, "API is not logged in yet");
        return -1;
    }
    int subscriptionFlags = subscribe ? 0 : SUBSCRIPTION_FLAG_UNSUBSCRIBE;
    subscriptionFlags |= (symbolType<<SYMBOL_TYPE_MASK_BIT_OFFSET);
    subscriptionFlags |= (subscriptionTypeMask<<SUBSCRIBE_TYPE_MASK_BIT_OFFSET);

    char symbols_buf[MAX_SUBSCRIPTION_REQUEST];
    uint16_t offset = 0, symbolCount = 0;

    std::string qsid = translateQuoteSource(quoteSource);
    for (unsigned int i = 0; i < symbols.size(); ++i) {
        if (isValidSymbol(symbols[i])) {
            if (offset + symbols[i].length() + 1 > MAX_SUBSCRIPTION_REQUEST - sizeof(subscription_request_msg)) {
                if (writeSubRequest(offset,subscriptionFlags,qsid,symbolCount, symbols_buf) < 0) return -1;
                offset = 0;
                symbolCount = 0;
            }
            strcpy(&symbols_buf[offset], symbols[i].c_str());
            offset += symbols[i].length() + 1;
            symbolCount++;
        }
    }
    if (writeSubRequest(offset,subscriptionFlags,qsid,symbolCount, symbols_buf) < 0) return -1;

    if (subscriptionFlags == 0)
        subscriptionStatus_ = SUBSCRIPTION_STATUS_SUBSCRIBED;

    return 0;
}

int LimeBrokerage::QuoteSystemApi::subscribe(std::string quoteSource, const std::vector<std::string>& symbols)
{
    if (!loggedIn()) {
        onQuoteSystemApiError(quoteSource, notLoggedIn, "API is not logged in yet");
        return -1;
    }
    if (symbols.empty() == true) {
        onQuoteSystemApiError(quoteSource, unknownError, "Invalid subscription attempt, empty subscription list");
        return -1;
    }
    return sendSubscribeUnsubscribe(quoteSource, symbols, true, symbolTypeNormal, subscriptionTypeMarketData);
}

int LimeBrokerage::QuoteSystemApi::subscribeAll(std::string quoteSource)
{
    if (!loggedIn()) {
        onQuoteSystemApiError(quoteSource, notLoggedIn, "API is not logged in yet");
        return -1;
    }
    return sendSubscribeUnsubscribe(quoteSource, emptyVector, true, symbolTypeNormal, subscriptionTypeMarketData);
}

int LimeBrokerage::QuoteSystemApi::unsubscribe(std::string quoteSource, const std::vector<std::string>& symbols)
{
    if (!loggedIn()) {
        onQuoteSystemApiError(quoteSource, notLoggedIn, "API is not logged in yet");
        return -1;
    }
    if (symbols.empty() == true) {
        onQuoteSystemApiError(quoteSource, unknownError, "Invalid subscription attempt, empty subscription list");
        return -1;
    }
    return sendSubscribeUnsubscribe(quoteSource, symbols, false, symbolTypeNormal, subscriptionTypeMarketData);
}

int LimeBrokerage::QuoteSystemApi::unsubscribeAll(std::string quoteSource)
{
    if (!loggedIn()) {
        onQuoteSystemApiError(quoteSource, notLoggedIn, "API is not logged in yet");
        return -1;
    }
    return sendSubscribeUnsubscribe(quoteSource, emptyVector, false, symbolTypeNormal, subscriptionTypeMarketData);
}

int LimeBrokerage::QuoteSystemApi::subscribe(std::string quoteSource, const std::vector<std::string>& symbols,
                                             QuoteSystemApi::SymbolType symbolType, int subscriptionTypeMask)
{
    if (symbols.empty() == true) {
        onQuoteSystemApiError(quoteSource, unknownError, "Invalid subscription attempt, empty subscription list");
        return -1;
    }
    return sendSubscribeUnsubscribe(quoteSource, symbols, true, symbolType, subscriptionTypeMask);
}

int LimeBrokerage::QuoteSystemApi::subscribeAll(std::string quoteSource, int subscriptionTypeMask)
{
    return sendSubscribeUnsubscribe(quoteSource, emptyVector, true, symbolTypeNormal, subscriptionTypeMask);
}

int LimeBrokerage::QuoteSystemApi::subscribeAll(std::string quoteSource, SymbolType symbolType, int subscriptionTypeMask)
{
    return sendSubscribeUnsubscribe(quoteSource, emptyVector, true, symbolType, subscriptionTypeMask);
}

int LimeBrokerage::QuoteSystemApi::unsubscribe(std::string quoteSource, const std::vector<std::string>& symbols,
                                               QuoteSystemApi::SymbolType symbolType, int subscriptionTypeMask)
{
    if (symbols.empty() == true) {
        onQuoteSystemApiError(quoteSource, unknownError, "Invalid subscription attempt, empty subscription list");
        return -1;
    }
    return sendSubscribeUnsubscribe(quoteSource, symbols, false, symbolType, subscriptionTypeMask);
}

int LimeBrokerage::QuoteSystemApi::unsubscribeAll(std::string quoteSource, int subscriptionTypeMask)
{
    return sendSubscribeUnsubscribe(quoteSource, emptyVector, false, symbolTypeNormal, subscriptionTypeMask);
}

// Default implementations:

void LimeBrokerage::QuoteSystemApi::onBidQuote(std::string symbol, long id, int size, double price, std::string mmid, std::string quoteSource, uint32_t timestamp)
{
}

void LimeBrokerage::QuoteSystemApi::onAskQuote(std::string symbol, long id, int size, double price, std::string mmid, std::string quoteSource, uint32_t timestamp)
{
}

void LimeBrokerage::QuoteSystemApi::onTradeTick(std::string symbol, int size, double price, int totalVolume, std::string exchangeId, std::string quoteSource, uint32_t timestamp, std::string tradeType)
{
}

void LimeBrokerage::QuoteSystemApi::onQuoteAlert(std::string quoteSource, std::string symbol, Alert code, std::string message)
{
}

void LimeBrokerage::QuoteSystemApi::onQuoteAlert(std::string quoteSource, std::string symbol, int code, std::string message)
{
}

void LimeBrokerage::QuoteSystemApi::onRoundLotTob(const RoundLotTob& quote)
{
}

void LimeBrokerage::QuoteSystemApi::onQuoteBookRebuildStarted(std::string quoteSource, std::string symbol)
{
}

void LimeBrokerage::QuoteSystemApi::onQuoteBookRebuildComplete(std::string quoteSource, std::string symbol)
{
}

void LimeBrokerage::QuoteSystemApi::onQuoteStreamConnectionStatus(std::string quoteSource, QuoteStreamStatus status, std::string reason)
{
}

void LimeBrokerage::QuoteSystemApi::onQuoteSystemApiError(std::string quoteSource, QuoteSystemApiError errorCode, std::string errorMsg)
{
}

void LimeBrokerage::QuoteSystemApi::onOptionAttributes(const OptionAttributes &optionAttributes)
{
}

void LimeBrokerage::QuoteSystemApi::onAuctionUpdate(const AuctionUpdate &auctionUpdate)
{
}

void LimeBrokerage::QuoteSystemApi::onAuctionSummary(const AuctionSummary &auctionSummary)
{
}

void LimeBrokerage::QuoteSystemApi::onModifyExecution(const Quote& quote, const Trade& trade)
{
    onQuote(quote);
    onTrade(trade);
}

void LimeBrokerage::QuoteSystemApi::onOrderReplace(const Quote& oldOrder, const Quote& newOrder)
{
    onQuote(oldOrder);
    onQuote(newOrder);
}

void LimeBrokerage::QuoteSystemApi::onQuote(const Quote& quote)
{
    if (quote.isBid) {
        onBidQuote(getSymbol(quote.symbolIndex), quote.id, quote.size, priceToDouble(quote.priceMantissa, quote.priceExponent), quote.mmid, quote.quoteSource, quote.timestamp);
    } else {
        onAskQuote(getSymbol(quote.symbolIndex), quote.id, quote.size, priceToDouble(quote.priceMantissa, quote.priceExponent), quote.mmid, quote.quoteSource, quote.timestamp);
    }
}

void LimeBrokerage::QuoteSystemApi::onTrade(const Trade& trade)
{
    onTradeTick(getSymbol(trade.symbolIndex), trade.size, priceToDouble(trade.priceMantissa, trade.priceExponent), trade.totalVolume, trade.exchangeId, trade.quoteSource, trade.timestamp, trade.tradeType);
}

void LimeBrokerage::QuoteSystemApi::onPriceBand(const PriceBand& priceBand)
{
}

void LimeBrokerage::QuoteSystemApi::onLimeqControlMessage(std::string quoteSource, int type, char *byteArray, int len)
{
    switch(type) {
        case MSG_COUNT:
        break;

        case CHANNEL_STATUS:
        {
            LimeBrokerage::channel_status_type_t status_type = (LimeBrokerage::channel_status_type_t)byteArray[1];
            uint16_t channel_id = ntohs(*(uint16_t *)(byteArray + 2));
            uint32_t skipped_messages = ntohl(*(uint32_t *)(byteArray + 4));
            uint32_t timestamp = ntohl(*(uint32_t *)(byteArray + 8));

            onChannelStatus(quoteSourceId, channel_id, status_type, skipped_messages, timestamp);
            break;
        }

        default:
            break;
    }
}

void LimeBrokerage::QuoteSystemApi::onChannelStatus(std::string quoteSource, uint16_t channelId, LimeBrokerage::channel_status_type_t statusType, uint32_t value, uint32_t timestamp)
{
    if(statusType == CHANNEL_STATUS_TYPE_LOSS)
        onMulticastLoss(quoteSourceId, channelId, value, timestamp);
}

void LimeBrokerage::QuoteSystemApi::onMulticastLoss(std::string quoteSource, uint16_t channelId, uint32_t skippedMessages, uint32_t timestamp)
{
}

void LimeBrokerage::QuoteSystemApi::onAddSymbol(std::string quoteSource, std::string& symbol, uint32_t symbolIndex, SecurityType symbolType)
{
    onAddSymbol(quoteSource, symbol, symbolIndex);
}

void LimeBrokerage::QuoteSystemApi::onAddSymbol(std::string quoteSource, std::string& symbol, uint32_t symbolIndex)
{
}

void LimeBrokerage::QuoteSystemApi::onMarketImbalance(const MarketImbalance &marketImbalance)
{
}

void LimeBrokerage::QuoteSystemApi::onTradeMod(const Trade& originalTrade, const Trade& modifiedTrade)
{
}

void LimeBrokerage::QuoteSystemApi::onSymbolStatus(const SymbolStatus& symbolStatus)
{
}

void LimeBrokerage::QuoteSystemApi::onFeatureControlResponse(int featureCode, int response)
{
}

void LimeBrokerage::QuoteSystemApi::onIndex(const Index& index)
{
}

void LimeBrokerage::QuoteSystemApi::onSpreadDefinition(const SpreadDefinition& index)
{
}
void LimeBrokerage::QuoteSystemApi::onQuoteAlert(const QuoteAlert& alert)
{
    //There are 2 onQuoteAlert methods with int as alert code and using enum Alert s alert code, hence 2 calls
    onQuoteAlert(alert.quoteSource, getSymbol(alert.symbolIndex), alert.code, nullString);
    onQuoteAlert(alert.quoteSource, getSymbol(alert.symbolIndex), (Alert)alert.code, nullString);
}

// Auxiliary methods.

inline void LimeBrokerage::QuoteSystemApi::addSymbol(std::string qsid, uint32_t symbol_index, const char *symbol, int symbol_len, SecurityType symbol_type)
{
    // FIXME if symbol is already indexed, do not call onAddSymbol(). ??
    if(symbol_type < securityTypeMin || symbol_type >= securityTypeMax) {
        std::ostringstream  errorMsg;
        errorMsg << "onAddSymbol(): Invalid value for symbol_type: " << std::hex << std::showbase
                << int(symbol_type) << ", index: "<< std::dec << std::noshowbase << symbol_index
                << ", symbol: " << symbol;
        onQuoteSystemApiError(qsid, invalidMessage, errorMsg.str());
        return;
    }

    DynamicArray<std::string> *pSymArr = symbolVector.at(symbol_type);

    if(!pSymArr) {
        if(defaultSecType == securityTypeMax && symbol_type != securityTypeIndex)
            defaultSecType = symbol_type;

        pSymArr = symbolVector.at(symbol_type) = new DynamicArray<std::string>(maxEquitySymbols, maxSymbols);
    }

    if (symbol_index >= (*pSymArr).capacity()) {
        std::ostringstream errorMsg;
        errorMsg << "Symbol Add: Symbol index out of range, index: " << symbol_index << ", symbol: "<< symbol;
        onQuoteSystemApiError(quoteSourceId,invalidMessage,errorMsg.str());
        return;
    }

    (*pSymArr)[symbol_index].assign(symbol,symbol_len);
    onAddSymbol(qsid, (*pSymArr)[symbol_index], symbol_index, symbol_type);
}

LimeBrokerage::SessionManager* LimeBrokerage::QuoteSystemApi::getSessionManager()
{
    return pSessionManager;
}

int LimeBrokerage::QuoteSystemApi::setSessionManager(SessionManager *sm)
{
    if(!loggedOut()) {
        onQuoteSystemApiError(nullString, unknownError, "Cannot change Session Manager before logging Out completely. "
                "Use joinReceiver to wait for logout first");
        return -1;
    }

    if(sm) {
        if(defaultSessionManager) {
            if(!loggedOut()) pSessionManager->deRegisterApiInstance(this);
            pSessionManager->shutdown();
            pSessionManager->join();
            delete(pSessionManager);
            defaultSessionManager = false;
        }

        pSessionManager = sm;
    } else {
        if(!defaultSessionManager) {
            defaultSessionManager = true;
            pSessionManager = new SessionManager();
            pSessionManager->start();
            if(loggedIn()) pSessionManager->registerApiInstance(this);
        }
    }

    return 0;
}

inline void * LimeBrokerage::QuoteSystemApi::getNextMessage(int *pLen)
{
    char *msg = NULL;
    bool readFromSock = (*pLen == 0);
    int len = *pLen = HEADER_BYTES;

    // first time (when *pLen is 0), read from the socket
    msg = socketProcessor->getInputData(pLen, readFromSock, false);
    if (msg == NULL) {
        return NULL;
    }

    if(*pLen < len) {
        return NULL;
    }

    *pLen = len = ntohs( ((limeq_header *)msg)->msg_len );

    msg = socketProcessor->getInputData(pLen, readFromSock, false);

    if(*pLen < len) {
       return NULL;
    }

    socketProcessor->consume(len);
    ((limeq_header *)msg)->msg_len = len;

    return msg;
}

int LimeBrokerage::QuoteSystemApi::cleanup()
{
    if (loggedOut())
        return -1;

    setLoginStatus(LS_LOGGED_OUT);
    subscriptionStatus_ = SUBSCRIPTION_STATUS_UNSUBSCRIBED;
    tradesOnlySession = false;
    topOfBookSession = false;
    roundLotTobSession = false;
    aggregatedBookSession = false;
    extendedQuoteSession = false;
    ipoAndUplisted = false;
    pSessionManager->deRegisterApiInstance(this);
    socketProcessor->close();
    quoteSourceId = nullString; //Reset quoteSourceId to an empty string to avoid NullPointerException.

    return 0;
}

LimeBrokerage::QuoteSystemApi::LoginStatus LimeBrokerage::QuoteSystemApi::setLoginStatus(LoginStatus ls)
{
    logoutLock.lock();
    loginStatus = ls;
    isLoggedOut.notifyAll();
    logoutLock.unlock();

    return loginStatus;
}

void LimeBrokerage::QuoteSystemApi::onCancelRegistration()
{
    logoutLock.lock();
    isLoggedOut.notifyAll();
    logoutLock.unlock();
}

LimeBrokerage::QuoteSystemApi::PriceBand::PriceBand(const price_band_msg* band) :
    symbolIndex_(ntohl(band->symbol_index)),
    timestamp_(htonl(band->timestamp), 0),
    limitDownPrice_(ntohl(band->down_price_mantissa), band->down_price_exponent),
    limitUpPrice_(ntohl(band->up_price_mantissa), band->up_price_exponent),
    effectiveTimestamp_(htonl(band->effective_timestamp), 0)
{
    populateStringIgnoreSpaces(quoteSourceId_, band->quote_source_id, QUOTE_SOURCE_NAME_LEN);
}

LimeBrokerage::QuoteSystemApi::MarketImbalance&
LimeBrokerage::QuoteSystemApi::MarketImbalance::operator=(const imbalance_msg *p_im)
{
    symbolIndex = ntohl(p_im->symbol_index);
    side = p_im->side;
    priceMantissa = ntohl(p_im->price_mantissa);
    priceExponent = p_im->price_exponent;
    timestamp = ntohl(p_im->timestamp);
    pairedShares = ntohl(p_im->shares);

    populateStringIgnoreSpaces(quoteSource, p_im->quote_source_id, QUOTE_SOURCE_NAME_LEN);

    totalImbalance = ntohl(p_im->total_imbalance);
    marketImbalance = ntohl(p_im->market_imbalance);
    imbalanceType = p_im->imbalance_type;
    populateStringIgnoreSpaces(exchangeId, p_im->exchange_id, MMID_LEN);
    feedSpecificData = ntohl(p_im->feed_specific_data);

    nsecTimestamp = 0;
    if (p_im->msg_len < sizeof(imbalance_msg) - sizeof(p_im->closing_clearing_price_numerator))
        nsecTimestamp = 0;
    else
        nsecTimestamp = ntohl(p_im->nanoseconds);

    if (p_im->msg_len < sizeof(imbalance_msg))
        closingClearingPriceNumerator = 0;
    else
        closingClearingPriceNumerator = ntohl(p_im->closing_clearing_price_numerator);

    return *this;
}

LimeBrokerage::QuoteSystemApi::SecurityDefinitionHeader::SecurityDefinitionHeader(const SecurityDefinition& securityDefinition) {
    timestamp = ntohl(securityDefinition.timestamp);
    nsecTimestamp = 0;
    if (securityDefinition.msg_len < sizeof(security_definition_msg_t))
        nsecTimestamp = 0;
    else
        nsecTimestamp = ntohl(securityDefinition.nanoseconds);
    symbolIndex = ntohl(securityDefinition.symbol_index);
    populateStringIgnoreSpaces(quoteSourceId,
                               securityDefinition.quote_source_id, sizeof(securityDefinition.quote_source_id));
    type = securityDefinition.sec_def_type;
}

LimeBrokerage::QuoteSystemApi::SecurityDefinitionCommonData::SecurityDefinitionCommonData(const SecurityDefinition& securityDefinition) {
    previousDayClosingPrice = priceToDouble(ntohl(securityDefinition.previous_day_closing_price_mant),
                                            securityDefinition.previous_day_closing_price_exp);
}

//
// Constructor for OptionAttributes.
// Copy Limeq message fields into OptionAttributes.
//
LimeBrokerage::QuoteSystemApi::OptionAttributes::OptionAttributes(const option_attributes_message_t *option_attributes_message)
{
    populateStringIgnoreSpaces(quoteSource, option_attributes_message->quote_source_id, QUOTE_SOURCE_NAME_LEN);
    memcpy(&underlyingName, &option_attributes_message->underlying_name, OPTION_SYMBOL_LEN);
    memcpy(&optionName, &option_attributes_message->option_name, OPTION_NAME_LEN);
    memcpy(&opraName, &option_attributes_message->opra_name, OPRA_NAME_LEN);
    openInterest = ntohl(option_attributes_message->open_interest);
    symbolIndex  = ntohl(option_attributes_message->symbol_index);
    quotingExchanges = ntohs(option_attributes_message->quoting_exchanges);
    underlyingIndex  = ntohl(option_attributes_message->underlying_index);
    seriesIndex  = ntohl(option_attributes_message->series_index);
}

LimeBrokerage::QuoteSystemApi::AuctionUpdate::AuctionUpdate(const auction_update_message_t *auction_update_message)
{
    symbolIndex = ntohl(auction_update_message->symbol_index);
    timestamp = ntohl(auction_update_message->timestamp);
    populateStringIgnoreSpaces(quoteSourceId, auction_update_message->quote_source_id, QUOTE_SOURCE_NAME_LEN);
    auctionType = auction_update_message->auction_type;
    buyShares = ntohl(auction_update_message->buy_shares);
    sellShares = ntohl(auction_update_message->sell_shares);
    indicativePriceMantissa = ntohl(auction_update_message->indicative_price_mant);
    indicativePriceExponent = auction_update_message->indicative_price_exp;
    auctionOnlyPriceMantissa = ntohl(auction_update_message->auction_only_price_mant);
    auctionOnlyPriceExponent = auction_update_message->auction_only_price_exp;
}

LimeBrokerage::QuoteSystemApi::AuctionSummary::AuctionSummary(const auction_summary_message_t *auction_summary_message)
{
    symbolIndex = ntohl(auction_summary_message->symbol_index);
    timestamp = ntohl(auction_summary_message->timestamp);
    populateStringIgnoreSpaces(quoteSourceId, auction_summary_message->quote_source_id, QUOTE_SOURCE_NAME_LEN);
    auctionType = auction_summary_message->auction_type;
    shares = ntohl(auction_summary_message->shares);
    priceMantissa = ntohl(auction_summary_message->price_mant);
    priceExponent = auction_summary_message->price_exp;
}

LimeBrokerage::QuoteSystemApi::SymbolStatus::SymbolStatus(const symbol_status_msg *p_msg)
{
    symbolIndex = ntohl(p_msg->symbol_index);

    symbolType  = (p_msg->symbol_type > 0)?SecurityType(p_msg->symbol_type - 1):securityTypeMin;

    populateStringIgnoreSpaces(quoteSourceId, p_msg->quote_source_id, QUOTE_SOURCE_NAME_LEN);
    highTradePriceMantissa = ntohl(p_msg->high_trade_price_mantissa);
    highTradePriceExponent = p_msg->high_trade_price_exponent;
    lowTradePriceMantissa = ntohl(p_msg->low_trade_price_mantissa);
    lowTradePriceExponent = p_msg->low_trade_price_exponent;
    isHighTradeInvalid = false; //p_msg->flags & SYMBOL_STATUS_FLAGS_INVALID_HIGH;
    isLowTradeInvalid = false; //(p_msg->flags & SYMBOL_STATUS_FLAGS_INVALID_LOW)?true:false;
    openPriceExponent = p_msg->open_exponent;
    openPriceMantissa = ntohl(p_msg->open_mantissa);
    lastPriceExponent = p_msg->last_exponent;
    lastPriceMantissa = ntohl(p_msg->last_mantissa);
    closePriceExponent = p_msg->close_exponent;
    closePriceMantissa = ntohl(p_msg->close_mantissa);
    uint16_t closePriceDate = ntohs(p_msg->close_date);
    if(closePriceDate) {
        closePriceDay = (0x1f & (closePriceDate >> 11));
        closePriceMon = 0xf & (closePriceDate >> 7);
        closePriceYr = (0x7f & closePriceDate) + 2000;
    } else {
        closePriceDay = closePriceMon = closePriceYr = 0;
    }
    exchangeId = (p_msg->exchange_id != 0)? p_msg->exchange_id : ' ';
}

void LimeBrokerage::QuoteSystemApi::setTradeFlags(Trade& trade, uint8_t flags)
{
    trade.isHigh = trade.isLow = trade.isCancelled = trade.isCorrected = trade.isPreviousSessionLastTrade = false;
    trade.isClose = trade.isOpen = trade.isLast = false;
    if (flags & TRADE_MSG_FLAGS_HIGH_TICK)
        trade.isHigh = true;
    if (flags & TRADE_MSG_FLAGS_LOW_TICK)
        trade.isLow = true;
    if (flags & TRADE_MSG_FLAGS_CANCELLED)
        trade.isCancelled = true;
    if (flags & TRADE_MSG_FLAGS_CORRECTED)
        trade.isCorrected = true;
    if (flags & TRADE_MSG_FLAGS_PREVIOUS_SESSION_LAST_TRADE)
        trade.isPreviousSessionLastTrade = true;
    if (flags & TRADE_MSG_FLAGS_LAST_TICK)
        trade.isLast = true;
    if (flags & TRADE_MSG_FLAGS_OPEN_TICK)
        trade.isOpen = true;
    if (flags & TRADE_MSG_FLAGS_CLOSE_TICK)
        trade.isClose = true;
}

void LimeBrokerage::QuoteSystemApi::issueDelayedCallback()
{
    switch (lastMsgType_) {
        case ORDER:
            onQuote(quote_);
            break;
        case TRADE:
            onTrade(trade_);
            break;
        case MOD_EXECUTION:
            handleModifyExecution(quote_, trade_);
            break;
        case ORDER_REPLACE:
            handleOrderReplace(quoteToDelete_, quote_);
            break;
        case TRADE_CANCEL_CORRECT:
            onTradeMod(tc_original_trade_, tc_modified_trade_);
            break;
        default:
            // We only expect to delay callbacks for the msg types above.
            // If we find ourselves here, something's probably wrong.
            onQuoteSystemApiError(quoteSourceId, unknownError,
                                  "Unexpected msg type when trying to issue delayed callback");
    }

    waitingForBoundary_ = false;
}

bool LimeBrokerage::QuoteSystemApi::shouldIssueCallback(uint8_t msgType) {
    bool ret = true;

    // Check if we are waiting for exchange order id optional msg.
    if ((enabledFeatures_ & FEATURE_ORDER_VISIBILITY) &&
        (msgType == ORDER || msgType == TRADE || msgType == MOD_EXECUTION || msgType == ORDER_REPLACE ||
         msgType == TRADE_CANCEL_CORRECT))
        ret = false;

    // Check if we are waiting for broker ids optional msg.
    if ((enabledFeatures_ & FEATURE_BROKER_IDS) && msgType == TRADE)
        ret = false;

    if ((enabledFeatures_ & FEATURE_USEC_TIMESTAMP) &&
        (msgType == ORDER || msgType == MOD_EXECUTION || msgType == TRADE || msgType == ORDER_REPLACE))
        ret = false;

    return ret;
}

void LimeBrokerage::QuoteSystemApi::resetObject(Quote& quote) {
    quote.isBid = false;
    quote.symbolIndex = 0;
    quote.id = 0;
    quote.size = 0;
    quote.priceMantissa = 0;
    quote.priceExponent = 0;
    quote.mmid.clear();
    quote.quoteSource.clear();
    quote.timestamp = 0;
    quote.nsecTimestamp = 0;
    quote.quoteProperties = 0;
    quote.feedSpecificData = 0;
    quote.isNBBO = false;
    quote.isTopOfBook = quote.isAggregatedByPrice = false;
    quote.exchangeOrderId = 0;
    quote.citriusTimestamp = 0;
    quote.exchangeSeqNum = 0;
}

void LimeBrokerage::QuoteSystemApi::resetObject(Trade& trade) {
    trade.symbolIndex = 0;
    trade.size = 0;
    trade.priceMantissa = 0;
    trade.priceExponent = 0;
    trade.totalVolume = 0;
    trade.exchangeId.clear();
    trade.quoteSource.clear();
    trade.timestamp = 0;
    trade.nsecTimestamp = 0;
    trade.tradeType.clear();
    std::memset(trade.tradeProperties, 0, sizeof(trade.tradeProperties));
    trade.tradeId = 0;
    trade.isHigh = trade.isLow = trade.isCancelled = trade.isCorrected =
        trade.isPreviousSessionLastTrade = false;
    std::memset(trade.exchangeOrderId, 0, sizeof(trade.exchangeOrderId));
    trade.citriusTimestamp = 0;
    trade.exchangeSeqNum = 0;
}

int LimeBrokerage::QuoteSystemApi::processNextMsg()
{
    if (!waitingForBoundary_) {
        switch (lastMsgType_) {
            case ORDER:
                resetObject(quote_);
                break;
            case MOD_EXECUTION:
                resetObject(quote_);
                resetObject(trade_);
                break;
            case ORDER_REPLACE:
                resetObject(quote_);
                resetObject(quoteToDelete_);
                break;
            case TRADE:
                resetObject(trade_);
                break;
            case TRADE_CANCEL_CORRECT:
                resetObject(tc_original_trade_);
                resetObject(tc_modified_trade_);
                break;
        }
    }

    // Read any available data into the input buffer.
    limeq_header *header = (limeq_header *)getNextMessage(&bytesAvail);

    if (bytesAvail < 0) {
        // End-of-file or IO error detected.
        if (!loggedOut()) {
            onQuoteSystemApiError(quoteSourceId, ioError, "Quote Server disconnected");

            // Notify the parent that the quote stream has been disconnected.
            std::string qsId = quoteSourceId;
            cleanup();
            onQuoteStreamConnectionStatus(qsId, quoteServerDisconnected, nullString);
        }

        return -1;
    }

    if(!header)
        return (bytesAvail = 0);

    if (waitingForBoundary_ && header->msg_type < OPTIONAL_FIELDS_BOUNDARY) {
        // We expect an optional field message or the boundary message but received another
        // message instead.
        std::ostringstream errorStr;
        errorStr << "waiting for boundary but did not receive it. last msg type = " << int(lastMsgType_);
        onQuoteSystemApiError(quoteSourceId, invalidMessage, errorStr.str());
        issueDelayedCallback();
    }

    switch (header->msg_type)
    {
        case LOGIN_RESPONSE:
            // Login Accepted: We should have already processed this message.
            break;

        case FEATURE_CONTROL_RESPONSE:
        {
            feature_control_response_msg* msg = (feature_control_response_msg*)header;
            uint16_t feature_code = ntohs(msg->feature_code);

            if (msg->response == FEATURE_CONTROL_RESPONSE_ENABLED)
                enabledFeatures_ |= feature_code;

            onFeatureControlResponse(feature_code, msg->response);
            break;
        }

        case HEARTBEAT:
            break;

        case SUBSCRIPTION_REPLY:
        {
            subscription_reply_msg * msg = (subscription_reply_msg *)header;
            populateStringIgnoreSpaces(quoteSourceId, msg->qsid, QUOTE_SOURCE_NAME_LEN);
            switch (msg->outcome)
            {
                case LOAD_ALLOCATION_EXCEEDED :
                    onQuoteSystemApiError(quoteSourceId, loadAllocationExceeded, "Load allocation exceeded");
                    break;
                case MAX_NUMBER_OF_CLIENTS_EXCEEDED :
                    onQuoteSystemApiError(quoteSourceId, maxNumberOfClients, "Maximum number of clients exceeded" );
                    break;
                case MAX_NUMBER_OF_SYMBOLS_EXCEEDED :
                    onQuoteSystemApiError(quoteSourceId, maxNumberOfSymbols, "Maximum number of symbols exceeded");
                    break;
                case QUOTE_SOURCE_NOT_FOUND :
                    onQuoteSystemApiError(quoteSourceId, quoteSourceNotFound, "Quote source not found");
                    break;
                case SUBSCRIPTION_SUCCESSFUL :
                    break;
                case LICENSE_ALLOCATION_EXCEEDED :
                    onQuoteSystemApiError(quoteSourceId, licenseAllocationExceeded, "License allocation exceeded");
                    break;
                case NO_LICENSE_FOR_QUOTE_SOURCE :
                    onQuoteSystemApiError(quoteSourceId, noLicense, "No license");
                    break;
                case SUBSCRIBE_ALL_DISABLED:
                    onQuoteSystemApiError(quoteSourceId, subscribeAllDisabled, "Subscribe all disabled");
                    break;
                case LICENSE_IP_VALIDATION_FAILED :
                    onQuoteSystemApiError(quoteSourceId, licenseIpValidationFailed, "License IP validation failed");
                    break;
            }
            break;
        }

        case BOOK_REBUILD:
        {
            book_rebuild_msg *brm = (book_rebuild_msg *)header;

            populateStringIgnoreSpaces(quoteSourceId, brm->quote_source_id, QUOTE_SOURCE_NAME_LEN);
            std::string symbol_str = std::string(brm->symbol, brm->symbol_len);
            brm->symbol_index = ntohl(brm->symbol_index);

            switch (brm->action)
            {
                case BOOK_REBUILD_STARTED:
                {
                    if(brm->symbol_type == 0) {
                        // Old Quote Server, doesn't set this value
                        if(defaultSecType == securityTypeMax)
                            defaultSecType = securityTypeMin;
                    } else if (brm->symbol_type > 0) {
                        // New Quote Server increases by one before sending this value
                        // This is done to allow for zero to be sent to indicate no value
                        brm->symbol_type--;
                    }
                    addSymbol(quoteSourceId, brm->symbol_index, brm->symbol, brm->symbol_len, SecurityType(brm->symbol_type));
                    onQuoteBookRebuildStarted(quoteSourceId, symbol_str);
                    break;
                }
                case BOOK_REBUILD_ENDED:
                    onQuoteBookRebuildComplete(quoteSourceId, symbol_str);
                    break;
            }
            break;
        }

        case ORDER:
        case EXT_ORDER:
        {
            uint16_t msgLenRemaining = header->msg_len;
            msgLenRemaining -= sizeof(header->msg_len) + sizeof(header->msg_type); //header size

            order_msg *om = (order_msg *) header;

            quote_.isBid = om->side == BUY;
            msgLenRemaining -= sizeof(uint8_t); // includes om->feed_specific_properties

            quote_.timestamp = ntohl(om->timestamp);
            msgLenRemaining -= sizeof(om->timestamp);

            quote_.id = ntohl(om->order_id);

            msgLenRemaining -= sizeof(om->order_id);

            quote_.size = ntohl(om->shares);
            msgLenRemaining -= sizeof(om->shares);

            quote_.symbolIndex = ntohl(om->symbol_index);
            msgLenRemaining -= sizeof(om->symbol_index);

            quote_.priceMantissa = ntohl(om->price_mantissa);
            msgLenRemaining -= sizeof(om->price_mantissa);

            quote_.priceExponent = om->price_exponent;
            msgLenRemaining -= sizeof(om->price_exponent);

            quote_.isTopOfBook = om->u.quote_flags & QUOTE_FLAGS_TOP_OF_BOOK;
            quote_.isAggregatedByPrice = (om->u.quote_flags & QUOTE_FLAGS_PRICE_AGGR)?true:false;
            quote_.isNBBO = (om->u.quote_flags & QUOTE_FLAGS_NBBO)?true:false;
            msgLenRemaining -= sizeof(om->u);

            quote_.quoteProperties = om->additional_prop;
            msgLenRemaining -= sizeof(om->additional_prop);

            /* Decode any NYOU specific properties */
            if(om->feed_specific_properties == QUOTE_FEED_SPECIFIC_NYOU) {
                decode_nyou_quote_data(om->additional_prop,
                        &quote_.quoteProperties, &quote_.feedSpecificData);
            }

            populateStringIgnoreSpaces(quote_.quoteSource, om->quote_source_id, QUOTE_SOURCE_NAME_LEN);
            msgLenRemaining -= sizeof(om->quote_source_id);

            populateStringIgnoreSpaces(quote_.mmid, om->mmid, MMID_LEN);
            msgLenRemaining -= sizeof(om->mmid);

            if (quote_.symbolIndex >= getSymbolsArray(defaultSecType).capacity()) {
                std::ostringstream s;

                s << "QUOTE: Symbol index is out of range " << quote_.symbolIndex;
                onQuoteSystemApiError(quote_.quoteSource,invalidMessage,s.str());
                break;
            }

            quote_.nsecTimestamp = 0;
            if(msgLenRemaining > 0) {
                char *order_msg_beg = (char *)header;
                if (msgLenRemaining >= sizeof(om->nanoseconds)) {
                    if(serverVersion.getMajor() < 4) { // legacy servers
                        quote_.nsecTimestamp = ntohs(*((uint16_t *)(order_msg_beg + header->msg_len - msgLenRemaining))) * 1000;
                        msgLenRemaining -= sizeof(uint16_t);
                    } else { // turbo servers
                        quote_.nsecTimestamp = ntohl(om->nanoseconds);
                        msgLenRemaining -= sizeof(om->nanoseconds);
                    }
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "QUOTE: Invalid message length: nanoseconds");
                }
            }

            quote_.exchangeOrderId = 0;
            if(msgLenRemaining > 0) {
                if (msgLenRemaining >= sizeof(om->exchangeOrderId)) {
                    quote_.exchangeOrderId = networkToHost64(om->exchangeOrderId);
                    msgLenRemaining -= sizeof(om->exchangeOrderId);
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "QUOTE: Invalid message length: exchange order id");
                }
            }
            if (header->msg_type == EXT_ORDER)
            {
                ext_order_msg *eom = (ext_order_msg *) header;
                quote_.citriusTimestamp = networkToHost64(eom->citrius_timestamp);
                quote_.exchangeSeqNum = networkToHost64(eom->exchange_seq_num);
            }

            if (shouldIssueCallback(header->msg_type))
                onQuote(quote_);
            else
                waitingForBoundary_ = true;

            break;
        }

        case TRADE:
        case EXT_TRADE:
        {
            trade_msg *tm = (trade_msg *) header;
            uint16_t msgLenRemaining = header->msg_len;
            msgLenRemaining -= sizeof(header->msg_len) + sizeof(header->msg_type); //header size

            msgLenRemaining--; //side /feed_specific_properties

            trade_.timestamp = ntohl(tm->timestamp);
            msgLenRemaining -= sizeof(tm->timestamp);

            trade_.size = ntohl(tm->shares);
            msgLenRemaining -= sizeof(tm->shares);

            trade_.symbolIndex = ntohl(tm->symbol_index);
            msgLenRemaining -= sizeof(tm->symbol_index);

            trade_.priceMantissa = ntohl(tm->price_mantissa);
            msgLenRemaining -= sizeof(tm->price_mantissa);

            trade_.priceExponent = tm->price_exponent;
            msgLenRemaining -= sizeof(tm->price_exponent);

            uint8_t salesConditions = tm->u.sales_conditions;
            msgLenRemaining -= sizeof(tm->u);

            trade_.tradeId = ntohl(tm->order_id);

            msgLenRemaining -= sizeof(tm->order_id);

            populateStringIgnoreSpaces(trade_.quoteSource, tm->quote_source_id, QUOTE_SOURCE_NAME_LEN);
            msgLenRemaining -= sizeof(tm->quote_source_id);

            populateStringIgnoreSpaces(trade_.exchangeId, tm->mmid, MMID_LEN);
            msgLenRemaining -= sizeof(tm->mmid);

            trade_.totalVolume = ntohl(tm->total_volume);
            msgLenRemaining -= sizeof(tm->total_volume);

            if (header->msg_type == EXT_TRADE)
            {
                ext_trade_msg *etm = (ext_trade_msg *) header;
                trade_.citriusTimestamp = networkToHost64(etm->citrius_timestamp);
                trade_.exchangeSeqNum = networkToHost64(etm->exchange_seq_num);
            }

            if (trade_.symbolIndex >= getSymbolsArray(defaultSecType).capacity()) {
                onQuoteSystemApiError(trade_.quoteSource,invalidMessage,"Trade: Symbol index is out of range");
                break;
            }

            switch (tm->side)
            {
                case BUY:
                    trade_.tradeType = "BUY";
                    break;
                case SELL:
                    trade_.tradeType = "SELL";
                    break;
                default:
                    trade_.tradeType = nullString;
                    break;
            }

            if (tm->feed_specific_properties == SIAC_LONG_MSG_SALE_CONDITIONS)
            {
                decode_siac_sales_conditions(salesConditions,tm->additional_prop,trade_.tradeProperties);
            }
            else if (tm->feed_specific_properties == UTDF_LONG_MSG_SALE_CONDITIONS) {
                decode_utdf_sales_conditions(salesConditions, tm->additional_prop, trade_.tradeProperties);
            }
            else
            {
                trade_.tradeProperties[0] = tm->additional_prop;
                trade_.tradeProperties[1] = 0;
                trade_.tradeProperties[2] = 0;
                trade_.tradeProperties[3] = 0;
            }
            msgLenRemaining -= sizeof(tm->additional_prop);

            setTradeFlags(trade_, tm->flags);
            msgLenRemaining -= sizeof(tm->flags);

            msgLenRemaining--; //padding

            // this is for legacy servers.  Used what is now padding for microseconds field.
            char *trade_msg_beg = (char *)header;
            trade_.nsecTimestamp = ntohs(*((uint16_t *)(trade_msg_beg + header->msg_len - msgLenRemaining))) * 1000;
            msgLenRemaining -= sizeof(uint16_t);

            if(msgLenRemaining > 0) {
                if(msgLenRemaining >= sizeof(tm->nanoseconds)) {
                    trade_.nsecTimestamp = ntohl(tm->nanoseconds);
                    msgLenRemaining -= sizeof(tm->nanoseconds);
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "TRADE:Invalid message length: nanoseconds");
                }
            }

            if(msgLenRemaining > 0) {
                if (msgLenRemaining >= sizeof(tm->sale_condition)) { // new sale condition field.
                    memcpy(trade_.tradeProperties, tm->sale_condition, 4);
                    msgLenRemaining -= sizeof(tm->sale_condition);
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "TRADE:Invalid message length: sale condition");
                }
            }

            // check for upper trade id and combine to make new trade id
            if (msgLenRemaining > 0) {
                if (msgLenRemaining >= sizeof(tm->upper_trade_id)) {
                    trade_.tradeId = (static_cast<uint64_t>(tm->order_id)<<32);
                    trade_.tradeId |= static_cast<uint64_t>(tm->upper_trade_id);
                    trade_.tradeId = networkToHost64(trade_.tradeId);
                    msgLenRemaining -= sizeof(tm->upper_trade_id);
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "TRADE:Invalid message length: upper trade id");
                }
            }

            if (shouldIssueCallback(header->msg_type))
                onTrade(trade_);
            else
                waitingForBoundary_ = true;

            break;
        }

        case MOD_EXECUTION:
        case EXT_MOD_EXECUTION:
        {
            mod_execution_msg *me = (mod_execution_msg *) header;
            uint16_t msgLenRemaining = header->msg_len;
            msgLenRemaining -= sizeof(header->msg_len) + sizeof(header->msg_type); //header size

            trade_.timestamp = ntohl(me->timestamp);
            quote_.timestamp = trade_.timestamp;
            msgLenRemaining -= sizeof(me->timestamp);

            quote_.id = ntohl(me->order_id);

            msgLenRemaining -= sizeof(me->order_id);

            quote_.size = ntohl(me->shares);
            msgLenRemaining -= sizeof(me->shares);

            quote_.isTopOfBook = me->u.quote_flags & QUOTE_FLAGS_TOP_OF_BOOK;
            quote_.isNBBO = (me->u.quote_flags & QUOTE_FLAGS_NBBO)?true:false;
            quote_.isAggregatedByPrice = (me->u.quote_flags & QUOTE_FLAGS_PRICE_AGGR)?true:false;
            msgLenRemaining -= sizeof(me->u);

            quote_.quoteProperties = me->additional_prop;
            trade_.tradeProperties[0] = me->additional_prop;
            msgLenRemaining -= sizeof(me->additional_prop);

            quote_.symbolIndex = ntohl(me->symbol_index);
            trade_.symbolIndex = quote_.symbolIndex;
            msgLenRemaining -= sizeof(me->symbol_index);

            quote_.priceMantissa = ntohl(me->price_mantissa);
            msgLenRemaining -= sizeof(me->price_mantissa);

            quote_.priceExponent = me->price_exponent;
            msgLenRemaining -= sizeof(me->price_exponent);

            trade_.priceMantissa = ntohl(me->trade_price_mantissa);
            msgLenRemaining -= sizeof(me->trade_price_mantissa);

            trade_.priceExponent = me->trade_price_exponent;
            msgLenRemaining -= sizeof(me->trade_price_exponent);

            populateStringIgnoreSpaces(quote_.quoteSource, me->quote_source_id, QUOTE_SOURCE_NAME_LEN);
            trade_.quoteSource = quote_.quoteSource;
            msgLenRemaining -= sizeof(me->quote_source_id);

            populateStringIgnoreSpaces(quote_.mmid, me->mmid, MMID_LEN);
            trade_.exchangeId = quote_.mmid;
            msgLenRemaining -= sizeof(me->mmid);

            trade_.totalVolume = ntohl(me->total_volume);
            msgLenRemaining -= sizeof(me->total_volume);

            trade_.size = ntohl(me->shares_executed);
            msgLenRemaining -= sizeof(me->shares_executed );

            trade_.tradeId = ntohl(me->trade_id);
            msgLenRemaining -= sizeof(me->trade_id);

            setTradeFlags(trade_, me->trade_flags);
            msgLenRemaining -= sizeof(me->trade_flags);

            quote_.nsecTimestamp = trade_.nsecTimestamp = ntohs(me->pad) * 1000; // only true for express

            msgLenRemaining -= sizeof(me->pad);

            if (quote_.symbolIndex >= getSymbolsArray(defaultSecType).capacity()) {
                onQuoteSystemApiError(quote_.quoteSource,invalidMessage,"MOD_EXEC: Symbol index is out of range");
                break;
            }
            switch (me->side)
            {
                case BUY:
                    quote_.isBid = true;
                    trade_.tradeType = "SELL";
                    break;
                case SELL:
                    quote_.isBid = false;
                    trade_.tradeType = "BUY";
                    break;
                default:
                    // Shouldn't be here!
                    trade_.tradeType = nullString;
                    break;
            }
            msgLenRemaining--; //side

            // check for nanosecond timestamp
            if(msgLenRemaining > 0) {
                if(msgLenRemaining >= sizeof(me->nanoseconds)) {
                    quote_.nsecTimestamp = trade_.nsecTimestamp = ntohl(me->nanoseconds);
                    msgLenRemaining -= sizeof(me->nanoseconds);
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "MODIFY EXECUTION:Invalid message length");
                }
            }

            // check for upper trade id and combine to make new trade id
            if (msgLenRemaining > 0) {
                if (msgLenRemaining >= sizeof(me->upper_trade_id)) {
                    trade_.tradeId = (static_cast<uint64_t>(me->order_id)<<32);
                    trade_.tradeId |= static_cast<uint64_t>(me->upper_trade_id);
                    trade_.tradeId = networkToHost64(trade_.tradeId);
                    msgLenRemaining -= sizeof(me->upper_trade_id);
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "MODIFY EXECUTION:Invalid message length: upper trade id");
                }
            }

            // check for exchangeOrderId
            if (msgLenRemaining > 0) {
                if (msgLenRemaining >= sizeof(me->exchange_order_id)) {
                    quote_.exchangeOrderId = networkToHost64(me->exchange_order_id);
                    msgLenRemaining -= sizeof(me->exchange_order_id);
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "MODIFY EXECUTION:Invalid message length: oldExchangeOrderId");
                }
            }

            if (header->msg_type == EXT_MOD_EXECUTION)
            {
                ext_mod_execution_msg *emem = (ext_mod_execution_msg *) header;
                quote_.citriusTimestamp = networkToHost64(emem->citrius_timestamp);
                quote_.exchangeSeqNum = networkToHost64(emem->exchange_seq_num);
                trade_.citriusTimestamp = networkToHost64(emem->citrius_timestamp);
                trade_.exchangeSeqNum = networkToHost64(emem->exchange_seq_num);
            }

            if (shouldIssueCallback(header->msg_type)) {
                handleModifyExecution(quote_, trade_);
            }
            else {
                waitingForBoundary_ = true;
            }

            break;
        }

        case ORDER_REPLACE:
        case EXT_ORDER_REPLACE:
        {
            order_replace_msg *rep = (order_replace_msg *)header;
            uint16_t msgLenRemaining = header->msg_len;
            msgLenRemaining -= sizeof(header->msg_len) + sizeof(header->msg_type); //header size

            // Fill out the new order.
            quote_.timestamp            = ntohl(rep->timestamp);
            msgLenRemaining -= sizeof(rep->timestamp);

            quote_.id                   = ntohl(rep->order_id);

            msgLenRemaining -= sizeof(rep->order_id);

            quote_.size                 = ntohl(rep->shares);
            msgLenRemaining -= sizeof(rep->shares);

            quote_.isTopOfBook          = rep->u.quote_flags & QUOTE_FLAGS_TOP_OF_BOOK;
            quote_.isNBBO               = (rep->u.quote_flags & QUOTE_FLAGS_NBBO)?true:false;
            quote_.isAggregatedByPrice  = (rep->u.quote_flags & QUOTE_FLAGS_PRICE_AGGR)?true:false;
            msgLenRemaining -= sizeof(rep->u);

            quote_.quoteProperties      = rep->additional_prop;
            msgLenRemaining -= sizeof(rep->additional_prop);

            quoteToDelete_.feedSpecificData = quote_.feedSpecificData = 0;
            if(rep->feed_specific_properties == QUOTE_FEED_SPECIFIC_NYOU) {
                decode_nyou_quote_data(rep->additional_prop,
                        &quote_.quoteProperties, &quote_.feedSpecificData);
                decode_nyou_quote_data(rep->additional_prop,
                        &quoteToDelete_.quoteProperties, &quoteToDelete_.feedSpecificData);
            }

            quote_.symbolIndex          = ntohl(rep->symbol_index);
            msgLenRemaining -= sizeof(rep->symbol_index);

            quote_.priceMantissa        = ntohl(rep->price_mantissa);
            msgLenRemaining -= sizeof(rep->price_mantissa);

            quote_.priceExponent        = rep->price_exponent;
            msgLenRemaining -= sizeof(rep->price_exponent);

            populateStringIgnoreSpaces(quote_.quoteSource, rep->quote_source_id, QUOTE_SOURCE_NAME_LEN);
            msgLenRemaining -= sizeof(rep->quote_source_id);

            populateStringIgnoreSpaces(quote_.mmid, rep->mmid, MMID_LEN);
            msgLenRemaining -= sizeof(rep->mmid);

            if (rep->side == BUY)
                quote_.isBid = true;
            else
                quote_.isBid = false;
            msgLenRemaining --; // side

            // Fill out the delete with the fields it shares with the new order.
            quoteToDelete_.timestamp       = quote_.timestamp;
            quoteToDelete_.id              = ntohl(rep->old_order_id);
            msgLenRemaining -= sizeof(rep->old_order_id);

            quoteToDelete_.size            = 0;
            quoteToDelete_.isTopOfBook     = rep->u.quote_flags & QUOTE_FLAGS_TOP_OF_BOOK;
            quoteToDelete_.isNBBO          = false;  // quote getting deleted cant be the nbbo.
            quoteToDelete_.isAggregatedByPrice     = (rep->u.quote_flags & QUOTE_FLAGS_PRICE_AGGR)?true:false;
            quoteToDelete_.quoteProperties = rep->old_additional_prop;
            msgLenRemaining -= sizeof(rep->old_additional_prop);

            quoteToDelete_.symbolIndex     = quote_.symbolIndex;
            quoteToDelete_.priceMantissa   = ntohl(rep->old_mantissa);
            msgLenRemaining -= sizeof(rep->old_mantissa);

            quoteToDelete_.priceExponent   = rep->old_exponent;
            msgLenRemaining -= sizeof(rep->old_exponent);

            quoteToDelete_.quoteSource     = quote_.quoteSource;
            quoteToDelete_.mmid            = quote_.mmid;
            quoteToDelete_.isBid           = quote_.isBid;

            quoteToDelete_.nsecTimestamp = quote_.nsecTimestamp = ntohs(rep->pad) * 1000;
            msgLenRemaining -= sizeof(rep->pad);

            // check for nanosecond timestamp
            if(msgLenRemaining > 0) {
                if(msgLenRemaining >= sizeof(rep->nanoseconds)) {
                    quote_.nsecTimestamp = quoteToDelete_.nsecTimestamp = ntohl(rep->nanoseconds);
                    msgLenRemaining -= sizeof(rep->nanoseconds);
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "ORDER REPLACE: Invalid message length: nanoseconds");
                }
            }

            // check for pad2
            if (msgLenRemaining > 0) {
                if (msgLenRemaining >= sizeof(rep->pad2)) {
                    msgLenRemaining -= sizeof(rep->pad2);
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "ORDER REPLACE: Invalid message length: pad2");
                }
            }

            // check for both exchange order IDs
            if (msgLenRemaining > 0) {
                if (msgLenRemaining >= sizeof(rep->old_exchange_order_id) + sizeof(rep->new_exchange_order_id)) {
                    quoteToDelete_.exchangeOrderId = networkToHost64(rep->old_exchange_order_id);
                    quote_.exchangeOrderId = networkToHost64(rep->new_exchange_order_id);
                    msgLenRemaining -= (sizeof(rep->old_exchange_order_id) + sizeof(rep->new_exchange_order_id));
                } else {
                    onQuoteSystemApiError(quoteSourceId, invalidMessage, "ORDER REPLACE: Invalid message length: oldExchangeOrderId");
                }
            }

            if (header->msg_type == EXT_ORDER_REPLACE)
            {
                ext_order_replace_msg *eorm= (ext_order_replace_msg *) header;
                quote_.citriusTimestamp = networkToHost64(eorm->citrius_timestamp);
                quoteToDelete_.citriusTimestamp = networkToHost64(eorm->citrius_timestamp);
                quote_.exchangeSeqNum = networkToHost64(eorm->exchange_seq_num);
                quoteToDelete_.exchangeSeqNum = networkToHost64(eorm->exchange_seq_num);
            }

            if (shouldIssueCallback(header->msg_type)) {
                handleOrderReplace(quoteToDelete_, quote_);
            }
            else {
                waitingForBoundary_ = true;
            }

            break;
        }

        case QUOTE_SOURCE_CONTROL:
            break;

        case LIMEQ_CONTROL:
        {
            limeq_control_msg *lc = (limeq_control_msg *) header;
            char messageCode = lc->code;

            populateStringIgnoreSpaces(quoteSourceId, lc->quote_source_id, QUOTE_SOURCE_NAME_LEN);

            // don't count control messages, as the quote server doesn't count them
            switch (messageCode)
            {
                case QUOTE_SOURCE_CONNECTED:
                    onQuoteStreamConnectionStatus(quoteSourceId, quoteStreamConnected, nullString);
                    break;
                case QUOTE_SOURCE_DISCONNECTED:
                    onQuoteStreamConnectionStatus(quoteSourceId, quoteStreamDisconnected, nullString);
                    break;
                case SLOW_MESSAGE_PROCESSING:
                    onQuoteStreamConnectionStatus(quoteSourceId, quoteStreamPossibleSlowdown, nullString);
                    break;
                case START_OF_SESSION:
                    onQuoteStreamConnectionStatus(quoteSourceId, quoteServerConnected, nullString);
                    break;
                case END_OF_SESSION:
                {
                    std::string qsId = quoteSourceId;
                    cleanup();
                    onQuoteStreamConnectionStatus(qsId, quoteServerDisconnected, nullString);
                    break;
                }
                case EXTENDED_LIMEQ_CONTROL_MSG:
                    uint16_t sub_type = ntohs(lc->sub_type);
                    onLimeqControlMessage(quoteSourceId, sub_type, lc->byte_array, lc->msg_len - ntohs(lc->common_data_len));
                    break;
            }
            totalMessages--;
            break;
        }
        case PRICE_BAND:
        {
            price_band_msg *band = (price_band_msg *)header;
            PriceBand priceBand(band);
            onPriceBand(priceBand);
            break;
        }
        case TRADING_ACTION:
        {
            quote_alert_msg *qa = (quote_alert_msg *) header;
            quoteAlert_.symbolIndex = ntohl(qa->symbol_index);

            populateStringIgnoreSpaces(quoteAlert_.quoteSource, qa->quote_source_id, QUOTE_SOURCE_NAME_LEN);

            quoteAlert_.code = qa->halted_state;
            quoteAlert_.timestamp = htonl(qa->timestamp);
            if (quoteAlert_.symbolIndex >= getSymbolsArray(defaultSecType).capacity()) {
                onQuoteSystemApiError(quoteSourceId,invalidMessage,"TA: Symbol index is out of range");
                break;
            }
            quoteAlert_.shortSaleRestrictionIndicator = qa->short_sale_restriction_indicator;

            quoteAlert_.nsecTimestamp = 0;
            if (qa->msg_len < sizeof(quote_alert_msg))
                quoteAlert_.nsecTimestamp = 0;
            else
                quoteAlert_.nsecTimestamp = ntohl(qa->nanoseconds);

            if (qa->flags & LIMEQ_QUOTE_ALERT_FLAG_PASS_THROUGH_STATE)
            {
                onQuoteAlert(quoteAlert_);
            }
            else
            {
                switch (qa->halted_state) {
                case REG_SHO_UPDATE:
                    quoteAlert_.code = alertRegShoUpdate;
                    onQuoteAlert(quoteAlert_);
                    break;
                case OPENING_DELAY:
                    quoteAlert_.code = alertOpeningDelay;
                    onQuoteAlert(quoteAlert_);
                    break;
                case TRADING_HALT:
                    quoteAlert_.code = alertTradingHalt;
                    onQuoteAlert(quoteAlert_);
                    break;
                case TRADING_RESUME:
                    quoteAlert_.code = alertResume;
                    onQuoteAlert(quoteAlert_);
                    break;
                case TRADING_PAUSE:
                    quoteAlert_.code = alertPause;
                    onQuoteAlert(quoteAlert_);
                    break;
                case NO_OPEN_NO_RESUME:
                    quoteAlert_.code = alertNoOpenNoResume;
                    onQuoteAlert(quoteAlert_);
                    break;
                case PRICE_INDICATION:
                    quoteAlert_.code = alertPriceIndication;
                    onQuoteAlert(quoteAlert_);
                    break;
                case TRADING_RANGE_INDICATION:
                    quoteAlert_.code = alertTradingRangeIndication;
                    onQuoteAlert(quoteAlert_);
                    break;
                case MARKET_IMBALANCE_BUY:
                    quoteAlert_.code = alertMarketImbalanceBuy;
                    onQuoteAlert(quoteAlert_);
                    break;
                case MARKET_IMBALANCE_SELL:
                    quoteAlert_.code = alertMarketImbalanceSell;
                    onQuoteAlert(quoteAlert_);
                    break;
                case MARKET_ON_CLOSE_IMBALALNCE_BUY:
                    quoteAlert_.code = alertMarketOnCloseImbalanceBuy;
                    onQuoteAlert(quoteAlert_);
                    break;
                case MARKET_ON_CLOSE_IMBALALNCE_SELL:
                    quoteAlert_.code = alertMarketOnCloseImbalanceSell;
                    onQuoteAlert(quoteAlert_);
                    break;
                case NO_MARKET_IMBALANCE:
                    quoteAlert_.code = alertNoMarketImbalance;
                    onQuoteAlert(quoteAlert_);
                    break;
                case NO_MARKET_ON_CLOSE_IMBALANCE:
                    quoteAlert_.code = alertNoMarketOnCloseImbalance;
                    onQuoteAlert(quoteAlert_);
                    break;

                    //the following case handles "QuoteResume" alerts on UTDF and UQDF
                case 'Q':
                    quoteAlert_.code = alertQuoteResume;
                    onQuoteAlert(quoteAlert_);
                    break;
                case 'V':
                    quoteAlert_.code = alertExchangeTradingHalt;
                    onQuoteAlert(quoteAlert_);
                    break;
                case 'R':
                    quoteAlert_.code = alertExchangeQuoteResume;
                    onQuoteAlert(quoteAlert_);
                    break;
                }
            }
            break;
        }

        case IMBALANCE_MESSAGE:
        {
            imbalance_msg   *p_im = (imbalance_msg *)header;
            mi_ = p_im;
            onMarketImbalance(mi_);
            break;
        }

        case OPTION_ATTRIBUTES:
        {
            option_attributes_message_t *option_attributes_msg = (option_attributes_message_t*)header;
            populateStringIgnoreSpaces(quoteSourceId, option_attributes_msg->quote_source_id, QUOTE_SOURCE_NAME_LEN);
            uint32_t symbol_index = htonl(option_attributes_msg->symbol_index);
            if (symbol_index != 0 && (int32_t)symbol_index != -1) {
                if(defaultSecType == securityTypeMax) {
                    defaultSecType = securityTypeOption;
                }
                addSymbol(quoteSourceId, symbol_index, (char*)&option_attributes_msg->option_name, OPTION_NAME_LEN, defaultSecType);
            }
            OptionAttributes optionAttributes(option_attributes_msg);
            onOptionAttributes(optionAttributes);
            break;
        }

        case AUCTION_UPDATE:
        {
            auction_update_message *auction_update_msg = (auction_update_message_t*)header;
            AuctionUpdate auctionUpdate(auction_update_msg);
            onAuctionUpdate(auctionUpdate);
            break;
        }

        case AUCTION_SUMMARY:
        {
            auction_summary_message_t *auction_summary_msg = (auction_summary_message_t*)header;
            AuctionSummary auctionSummary(auction_summary_msg);
            onAuctionSummary(auctionSummary);
            break;
        }

        case SECURITY_DEFINITION:
        {
            // DEPRECATED: Only used for Brazillian market data in the past.
            break;
        }

        case TRADE_CANCEL_CORRECT:
        {
            trade_cancel_correct_msg* trade_cancel_correct = (trade_cancel_correct_msg*)header;
            tc_original_trade_.symbolIndex = ntohl(trade_cancel_correct->symbol_index);
            tc_original_trade_.size = ntohl(trade_cancel_correct->shares);
            tc_original_trade_.priceMantissa = ntohl(trade_cancel_correct->price_mantissa);
            tc_original_trade_.priceExponent = trade_cancel_correct->price_exponent;
            tc_original_trade_.totalVolume = 0;
            populateStringIgnoreSpaces(tc_original_trade_.quoteSource,
                                       trade_cancel_correct->quote_source_id,
                                       QUOTE_SOURCE_NAME_LEN);
            populateStringIgnoreSpaces(tc_original_trade_.exchangeId, trade_cancel_correct->mmid, MMID_LEN);
            tc_original_trade_.timestamp = ntohl(trade_cancel_correct->timestamp);
            tc_original_trade_.tradeId = ntohl(trade_cancel_correct->order_id);

            tc_modified_trade_.symbolIndex = ntohl(trade_cancel_correct->symbol_index);;
            tc_modified_trade_.size = ntohl(trade_cancel_correct->updated_trade.shares);
            tc_modified_trade_.priceMantissa = ntohl(trade_cancel_correct->updated_trade.price_mantissa);
            tc_modified_trade_.priceExponent = trade_cancel_correct->updated_trade.price_exp;
            tc_modified_trade_.totalVolume = ntohl(trade_cancel_correct->updated_total_volume);
            populateStringIgnoreSpaces(tc_modified_trade_.quoteSource,
                                       trade_cancel_correct->quote_source_id,
                                       QUOTE_SOURCE_NAME_LEN);
            populateStringIgnoreSpaces(tc_modified_trade_.exchangeId, trade_cancel_correct->mmid, MMID_LEN);
            tc_modified_trade_.timestamp = ntohl(trade_cancel_correct->updated_trade.timestamp);
            tc_modified_trade_.tradeId = tc_original_trade_.tradeId;

            // These were both added at the same time, so the message size is an indicator of their presence.
            tc_modified_trade_.nsecTimestamp = 0;
            tc_original_trade_.nsecTimestamp = 0;
            if (trade_cancel_correct->msg_len < sizeof(trade_cancel_correct_msg)) {
                tc_modified_trade_.nsecTimestamp = 0;
                tc_original_trade_.nsecTimestamp = 0;
            }
            else {
                tc_modified_trade_.nsecTimestamp = ntohl(trade_cancel_correct->updated_trade.nanoseconds);
                tc_original_trade_.nsecTimestamp = ntohl(trade_cancel_correct->nanoseconds);
            }

            if (shouldIssueCallback(header->msg_type))
                onTradeMod(tc_original_trade_, tc_modified_trade_);
            else
                waitingForBoundary_ = true;

            break;
        }

        case SYMBOL_STATUS:
        {
            symbol_status_msg* msg = (symbol_status_msg*)header;
            SymbolStatus symbol_status(msg);
            onSymbolStatus(symbol_status);
            break;
        }

        case INDEX:
        {
            index_msg* msg = (index_msg*)header;
            Index index;

            index.symbolIndex = ntohl(msg->symbol_index);
            populateStringIgnoreSpaces(index.quoteSourceId, msg->quote_source_id, QUOTE_SOURCE_NAME_LEN);
            index.valueMantissa = ntohl(msg->value_mantissa);
            index.valueExponent = msg->value_exponent;
            index.timestamp = ntohl(msg->timestamp);
            index.nsecTimestamp = 0;
            if (msg->msg_len < sizeof(index_msg))
                quoteAlert_.nsecTimestamp = 0;
            else
                quoteAlert_.nsecTimestamp = ntohl(msg->nanoseconds);

            onIndex(index);
            break;
        }
        case SPREAD_DEFINITION:
        {
            spread_def_msg_t* msg = (spread_def_msg_t*)header;
            SpreadDefinition spread(msg->spread_info, msg->quote_source_id, msg->spread_name, true);
            onSpreadDefinition(spread);
            break;
        }
        case OPTIONAL_FIELDS_BOUNDARY:
        {
            // Issue the callback we have been delaying while waiting for optional field messages
            // to be processed.
            issueDelayedCallback();
            break;
        }
        case OPTION_ANALYTICS:
        {
            option_analytics_msg* msg = (option_analytics_msg*)header;
            analytics_.symbolIndex = ntohl(msg->symbol_index);
            analytics_.timestamp = ntohl(msg->timestamp);
            analytics_.delta = ntohl(msg->delta_scaled);
            analytics_.gamma = ntohl(msg->gamma_scaled);
            analytics_.theta = ntohl(msg->theta_scaled);
            analytics_.vega = ntohl(msg->vega_scaled);
            analytics_.impliedVolatility = ntohl(msg->volatility);
            analytics_.optionPrice = ntohl(msg->option_price_scaled);
            analytics_.underlyingPrice = ntohl(msg->underlying_price_scaled);

            if (shouldIssueCallback(header->msg_type))
                onAnalytics(analytics_);
            else
                waitingForBoundary_ = true;

            break;
        }

        case OPTION_SHOCKED_PNL:
        {
            option_shocked_pnl_msg* msg = (option_shocked_pnl_msg*)header;
            shockedPnl_.symbolIndex = ntohl(msg->symbol_index);
            shockedPnl_.timestamp = ntohl(msg->timestamp);
            shockedPnl_.impliedVolatility = ntohl(msg->volatility);
            shockedPnl_.optionPrice = ntohl(msg->option_price_scaled);
            shockedPnl_.underlyingPrice = ntohl(msg->underlying_price_scaled);
            for(unsigned ii = 0; ii != 5; ++ii) {
                shockedPnl_.negativelyShockedPnlValues[ii] = ntohl(msg->neg_shocked_values_scaled[ii]);
                shockedPnl_.positivelyShockedPnlValues[ii] = ntohl(msg->pos_shocked_values_scaled[ii]);
            }
            shockedPnl_.maxNegativeShockPercent = msg->max_negative_shock;
            shockedPnl_.maxPositiveShockPercent = msg->max_positive_shock;

            if (shouldIssueCallback(header->msg_type))
                onShockedPnl(shockedPnl_);
            else
                waitingForBoundary_ = true;

            break;
        }


        case ROUND_LOT_TOB:
        {
            roundLotTob_ = (RoundLotTobMsg* )header;
            onRoundLotTob(roundLotTob_);
            break;
        }

        default:
        {
            // The message isn't one of the recognized types.
            if (header->msg_type >= 1 && header->msg_type <= 127)
                break; // gracefully ignore message types in this range.
            std::ostringstream  errorMsg;
            errorMsg << "Received unrecognized message type: " << (int) header->msg_type << ", len: " << header->msg_len;
            onQuoteSystemApiError(quoteSourceId, unknownMsgType, errorMsg.str());
            break;
        }
    }

    if (header->msg_type < OPTIONAL_FIELDS_BOUNDARY)
        lastMsgType_ = header->msg_type;

    totalMessages++;

    return (bytesAvail -= (header->msg_len));
}

void LimeBrokerage::QuoteSystemApi::populateVersion()
{
    ver1 = ver2 = ver3 = revis = 0;
    const char* ver = __QS_VERSION__;
    int res = sscanf(ver, __QS_VERSION_FORMAT_3__, &ver1, &ver2, &ver3, &revis);
    if (res < 4) {
        ver3 = 0;
        res = sscanf(ver, __QS_VERSION_FORMAT_2__, &ver1, &ver2, &revis);
        if (res < 3) {
            ver1 = ver2 = ver3 = 0;
            res = sscanf(ver, __QS_VERSION_FORMAT_R__, &ver1, &ver2);
            if (res < 1) revis = 0;
        }
    }
}

std::string LimeBrokerage::QuoteSystemApi::getVersion() const
{
    std::ostringstream s;
    s << ver1 << "." << ver2 << "." << ver3 << "." << revis;
    return s.str();
}
