#include <exchsupport/eqt/LimeClient.hh>

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>

extern "C" {
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sched.h>
}

using namespace LimeBrokerage;

ApiContext::ApiContext(LimeClient &owner,
                       char* account,
                       char* user,
                       char* password,
                       char* hostname,
                       uint64_t eventId,
                       TransportType transport,
                       bool cancelAllOnDisconnect,
                       const cpu_set_t& cpus) :
    stats(), eventId(eventId),
    listener(*this), owner(owner), markForDeletion(false)
{
    currOrderId = (eventId == 0) ? 1 : eventId;
    id = owner.generateApiId();
    api = new TradingApi(listener, account, user, password, eventId, cancelAllOnDisconnect, hostname, transport, cpus);
}

void LimeClient::removeApiCtx(ApiContext* ctx)
{
    ApiContextTable::iterator it;
    if ((it = apiCtxTable.find(ctx->getId())) != apiCtxTable.end()) {
        apiCtxTable.erase(it);
        delete (*it).second;
    }
}

void LimeClient::algo() {
    char* input;
    ApiContext *ctx = NULL;
    while (1) {

        if (ctx) {
            if (ctx->isMarkedForDeletion()) {
                removeApiCtx(ctx);
                ctx = NULL;
            }
        }
        else if (apiCtxTable.size())
            ctx = apiCtxTable.begin()->second;

        char rdline[20];
        if (ctx)
            sprintf(rdline,"%s%d%s", "LimeClient(", ctx->getId(), "): ");
        else
            sprintf(rdline, "%s", "LimeClient(): ");

        input = readline(rdline);
        if (input == NULL)
        	continue;

        if (strlen(input)) {
            add_history(input);
            char* cmd = strtok(input, " ");
            if(cmd == 0){
            	continue;
            }
            if (strcmp(cmd,"quit") == 0) {
                exit(0);
                return;
            }

            if (strcmp(cmd,"login") == 0) {
                login();
                continue;
            }

            if (strcmp(cmd,"dump") == 0) {
                dump();
                continue;
            }

            if (strcmp(cmd,"help") == 0) {
                std::cout << "\"login <account> <user> <password> <hostname> <eventId> [<transport type> [<cpu>]]\" -> create a new API instance  " << std::endl
                          << "\"logout\" -> logout from the current API instance." << std::endl
                          << "\"switchApi <API id>\" -> switch to another API instance." << std::endl
                          << "\"pinThread <cpu>\" -> pin the callback thread to a CPU core." << std::endl
                          << "\"resetThreadCpuAffinity <numCores>\" -> reset callback thread CPU affinity to default." << std::endl
                          << "\"order <symbol> <quantity> <price> <side>\" -> place an order with the specified information." << std::endl
                          << "\"opt-order <symbol> <quantity> <price> <side> <put/call> <expDate(YYMMDD)> <strike> [<posEffect>]\" -> place an options order with the specified info." << std::endl
                          << "\"cancel [<orderId>]\" -> cancel a previous order or the order identified by the orderId." << std::endl
                          << "\"cancelAll\" -> cancel all open orders." << std::endl
                          << "\"replace <quantity> <price> [<orderId>]\" -> replace a previous order or the order identified by the orderId with the specified information." << std::endl
                          << "                                              If price is unchanged, specify '0' or 'same'"  << std::endl
                          << "\"replaceOptOrder <quantity> <price> [<orderId>]\" -> replace a previous US Options order or the order identified by the orderId with the specified information." << std::endl
                          << "                                              If price is unchanged, specify 'same'. Zero is a valid price for options!"  << std::endl
                          << "\"dumpStats\"-> display statistics." << std::endl
                          << "\"clearStats\"-> clear statistics." << std::endl
                          << "\"dump\"-> dump API context and eventId tables." << std::endl
                          << "\"quit\"-> quit example client." << std::endl;
                continue;
            }

            if (!ctx) {
                while (apiCtxTable.size()) {
                    ctx = apiCtxTable.begin()->second;
                    if (!ctx->isMarkedForDeletion())
                        break;

                    removeApiCtx(ctx);
                    ctx = NULL;
                }

                if (!ctx) {
                    std::cout << "No APIs configured!" << std::endl;
                    continue;
                }
            }

            if (strcmp(cmd,"logout") == 0)
                ctx->quit();
            else if (strcmp(cmd,"switchApi") == 0) {
                char* idStr;
                int ctxId;
                if ((idStr = strtok(NULL, " ")) == NULL)
                    std::cout << "Must specify API id." << std::endl;
                else {
                    ApiContextTable::iterator it;
                    if ((it = apiCtxTable.find(ctxId = parseStr(idStr))) == apiCtxTable.end())
                        std::cout << "Id " << ctxId << " invalid." << std::endl;
                    else
                        ctx = (*it).second;
                }
            }
            else if (strcmp(cmd,"pinThread") == 0)
                setCpuAffinity(*ctx);
            else if (strcmp(cmd,"resetThreadCpuAffinity") == 0)
                resetCpuAffinity(*ctx);
            else if (strcmp(cmd,"order") == 0)
                placeOrder(*ctx);
            else if (strcmp(cmd,"opt-order") == 0)
                placeUSOptionsOrder(*ctx);
            else if (strcmp(cmd,"cancel") == 0)
                placeCancel(*ctx);
            else if (strcmp(cmd,"cancelAll") == 0)
                placeCancelAll(*ctx);
            else if (strcmp(cmd,"replace") == 0)
                placeReplace(*ctx);
            else if (strcmp(cmd,"replaceOptOrder") == 0)
                placeReplace(*ctx, true);
            else if (strcmp(cmd,"dumpStats") == 0) {
                ctx->dumpStats();
                dumpApiStats(*ctx);
            } else if (strcmp(cmd,"clearStats") == 0)
                ctx->clearStats();
            else {
                std::cout << "Invalid command!" << std::endl;
            }
        }
        free(input);
    }
}

void LimeClient::login() {
    char* account;
    char* user;
    char* password;
    char* hostname;
    char* eventIdStr;
    char* transportStr;
    int transportType;
    int eventId;
    cpu_set_t cpus;

    CPU_ZERO(&cpus);

    if ((account = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify account." << std::endl;
        return;
    }

    if ((user = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify user." << std::endl;
        return;
    }

    if ((password = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify password." << std::endl;
        return;
    }

    if ((hostname = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify a host." << std::endl;
        return;
    }

    if ((eventIdStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify eventId." << std::endl;
        return;
    }
    eventId = parseStr(eventIdStr);

    if ((transportStr = strtok(NULL, " ")) == NULL) 
        transportType = transportTypeTcp;
    else {
        if (strcmp(transportStr, "tcp") == 0)
            transportType = transportTypeTcp;
        else if (strcmp(transportStr, "ib") == 0)
            transportType = transportTypeInfiniband;
        else {
            std::cout << "Invalid transport type " << transportType << std::endl;
            return;
        }

        char* cpuStr;
        if ((cpuStr = strtok(NULL, " ")) != NULL) {
            int cpu = parseStr(cpuStr);
            if (cpu < 0 || cpu >= CPU_SETSIZE) {
                std::cout << "Invalid CPU identifier" << std::endl;
                return;
            }

            CPU_SET(cpu, &cpus);
        }
    }

    try {
        ApiContext *apiContext = new ApiContext(*this, account, user, password, hostname, eventId, static_cast<TransportType>(transportType), true, cpus);
        apiCtxTable[apiContext->getId()] = apiContext;
    } catch (const TradingApiException& exn) {
        std::cout << "[LimeClient] ***EXCEPTION***: " << exn.what() << std::endl;
    }
}

void LimeClient::setCpuAffinity(ApiContext &ctx)
{
    char* cpuStr;
    int cpu;

    if ((cpuStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Missing CPU identifier" << std::endl;
        return;
    }

    cpu = parseStr(cpuStr);
    if (cpu < 0 || cpu >= CPU_SETSIZE) {
        std::cout << "Invalid CPU identifier" << std::endl;
        return;
    }

    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET(cpu, &cpus);

    if (ctx.getApi().setCallbackCpuAffinity(cpus))
        std::cout << "Setting callback thread CPU affinity successful" << std::endl;
    else
        std::cout << "Setting callback thread CPU affinity failed" << std::endl;
}

void LimeClient::resetCpuAffinity(ApiContext &ctx)
{
    char* cpuNumStr;
    int cpuNum;

    if ((cpuNumStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must supply number of CPU cores" << std::endl;
        return;
    }

    cpuNum = parseStr(cpuNumStr);
    if (cpuNum <= 0 || cpuNum > CPU_SETSIZE) {
        std::cout << "Invalid number of CPU cores" << std::endl;
        return;
    }

    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    for (int i = 0; i < cpuNum; ++i)
        CPU_SET(i, &cpus);

    if (ctx.getApi().setCallbackCpuAffinity(cpus))
        std::cout << "Resetting callback thread CPU affinity successful" << std::endl;
    else
        std::cout << "Resetting callback thread CPU affinity failed" << std::endl;
}

void LimeClient::dump() {
    std::cout << "Dumping API context table (API ids): " << std::endl;
    for (ApiContextTable::iterator it = apiCtxTable.begin(); it != apiCtxTable.end(); ++it)
        std::cout << (*it).first << ", "<< std::endl;
    std::cout << "Dumping API eventId table (<API id, current eventId> pairs): " << std::endl;
    for (ApiIdTable::iterator it = apiIdTable.begin(); it != apiIdTable.end(); ++it)
        std::cout << "(" << (*it).first << "," << (*it).second << ")" << ", "<< std::endl;
}

void LimeClient::handleApiException(ApiContext &ctx, const TradingApiException &exn)
{
    std::cout << "[LimeClient] ***EXCEPTION***: " << exn.what() << std::endl;
    ctx.quit();
}

void LimeClient::placeOrder(ApiContext &ctx) {
    char* symbol;
    char* quantityStr;
    int quantity;
    char* priceStr;
    double price;
    bool marketPrice = false;
    char* sideStr;

    if ((symbol = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify a symbol." << std::endl;
        return;
    }
    if ((quantityStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify a quantity." << std::endl;
        return;
    }
    if ((quantity = parseStr(quantityStr)) == 0) {
        std::cout << "Quantity invalid." << std::endl;
        return;
    }
    if ((priceStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify a price." << std::endl;
        return;
    }
    if (strcmp(priceStr, "market") == 0)
        marketPrice = true;
    else if ((price = atof(priceStr)) < 0.0) {
        std::cout << "Price invalid." << std::endl;
        return;
    }
    if ((sideStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify side." << std::endl;
        return;
    }
    if (strcmp(sideStr, "buy") && strcmp(sideStr, "sell")) {
        std::cout << "Invalid side." << std::endl;
        return;
    }

    TradingApi &api = ctx.getApi();
    OrderId orderId = ctx.generateOrderId();
    LimeBrokerage::ScaledPrice scaledPrice = (marketPrice) ? TradingApi::marketPrice : (LimeBrokerage::ScaledPrice)(price * priceScalingFactor);
    Side side = (strcmp(sideStr, "buy") == 0) ? sideBuy : sideSell;
    std::string route = "LIME-ECN";
    std::string strSymbol = symbol;
    CallStatus status;

    try {
        status = api.placeOrder(orderId,
                                strSymbol,
                                quantity,
                                scaledPrice,
                                side,
                                route);
    } catch (const TradingApiException& exn) {
        handleApiException(ctx, exn);
    }

    if (status != statusSuccess)
        std::cout << "[LimeClient] order rejected due to " << status << std::endl;
}


void LimeClient::placeUSOptionsOrder(ApiContext &ctx) {
    char* symbol;
    char* quantityStr;
    int quantity;
    char* priceStr;
    double price;
    bool marketPrice = false;
    char* sideStr;
    
    char* expDateStr;
    char* putOrCallStr;
    char* posEffectStr;
    char* strikePriceStr;
    double strikePrice;
    int expirationDay, expirationMonth, expirationYear;
    PositionEffect positionEffect = positionEffectOpen;

    if ((symbol = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify a base symbol." << std::endl;
        return;
    } 
    if ((quantityStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify a quantity." << std::endl;
        return;
    }
    if ((quantity = parseStr(quantityStr)) == 0) {
        std::cout << "Quantity invalid." << std::endl;
        return;
    }
    if ((priceStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify a price." << std::endl;
        return;
    }
    if (strcmp(priceStr, "market") == 0)
        marketPrice = true;
    else if ((price = atof(priceStr)) < 0.0) {
        std::cout << "Price invalid." << std::endl;
        return;
    }
    if ((sideStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify side." << std::endl;
        return;
    }
    if (strcmp(sideStr, "buy") && strcmp(sideStr, "sell")) {
        std::cout << "Invalid side." << std::endl;
        return;
    }
    if ((putOrCallStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify whether the option put or call." << std::endl;
        return;
    }
    if (strcmp(putOrCallStr, "put") && strcmp(putOrCallStr, "call")) {
        std::cout << "Invalid put/call indication." << std::endl;
        return;
    }
    if ((expDateStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify maturity date (YYMMDD)." << std::endl;
        return;
    }
    if ((expirationDay = parseStr(expDateStr + 4)) == 0) {
        std::cout << "Option maturity date invalid." << std::endl;
        return;
    }
    expDateStr[4] = '\0';
    if ((expirationMonth = parseStr(expDateStr + 2)) == 0) {
        std::cout << "Option maturity date invalid." << std::endl;
        return;
    }
    expDateStr[2] = '\0';
    if ((expirationYear = parseStr(expDateStr)) == 0) {
        std::cout << "Option maturity date invalid." << std::endl;
        return;
    }

    if ((strikePriceStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify the option strike price." << std::endl;
        return;
    }
    if ((strikePrice = atof(strikePriceStr)) == 0.0) {
        std::cout << "Option strike price invalid." << std::endl;
        return;
    }
    if ((posEffectStr = strtok(NULL, " "))) {
        if (strcmp(posEffectStr, "open") && strcmp(posEffectStr, "close")) {
            std::cout << "Invalid positionEffect." << std::endl;
            return;
        }
        positionEffect = (strcmp(putOrCallStr, "open") == 0) ? positionEffectOpen : positionEffectClose;
    }

    TradingApi &api = ctx.getApi();
    OrderId orderId = ctx.generateOrderId();
    LimeBrokerage::ScaledPrice scaledPrice = (marketPrice) ? TradingApi::marketPrice : (LimeBrokerage::ScaledPrice)(price * priceScalingFactor);
    std::string route = "LIME-OPT-ECN";
    Side side = (strcmp(sideStr, "buy") == 0) ? sideBuy : sideSell;
    USOptionSymbol usOptSymbol;
    usOptSymbol.baseSymbol = symbol;
    usOptSymbol.expirationDay = expirationDay;
    usOptSymbol.expirationMonth = expirationMonth;
    usOptSymbol.expirationYear = expirationYear;
    usOptSymbol.putOrCall = (strcmp(putOrCallStr, "put") == 0) ? USOptionSymbol::put : USOptionSymbol::call;
    usOptSymbol.strikePrice = (int)(strikePrice * usOptionStrikePriceScalingFactor);
    CallStatus status;

    try {
        status = api.placeUSOptionsOrder(orderId,
                                         route,
                                         usOptSymbol,
                                         side,
                                         positionEffect,
                                         quantity,
                                         scaledPrice);
    } catch (const TradingApiException& exn) {
        handleApiException(ctx, exn);
    }

    if (status != statusSuccess)
        std::cout << "[LimeClient] order rejected due to " << status << std::endl;
}


void LimeClient::placeCancel(ApiContext &ctx) {
    char* orderIdStr;
    uint64_t orderId;
    TradingApi &api = ctx.getApi();
    CallStatus status;

    if ((orderIdStr = strtok(NULL, " ")) == NULL) {
        orderId = ctx.getPrevOrderId();
    }
    else if ((orderId = parseStr(orderIdStr)) == 0) {
        std::cout << "OrderId invalid." << std::endl;
        return;
    }

    try {
        status = api.cancelOrder(orderId);
    } catch (const TradingApiException& exn) {
        handleApiException(ctx, exn);
    }

    if (status != statusSuccess)
        std::cout << "[LimeClient] cancel rejected due to " << status << std::endl;
}

void LimeClient::placeCancelAll(ApiContext &ctx) {

    TradingApi &api = ctx.getApi();
    CallStatus status;

    try {
        status = api.cancelAllOpenOrders();
    } catch (const TradingApiException& exn) {
        handleApiException(ctx, exn);
    }

    if (status != statusSuccess)
        std::cout << "[LimeClient] cancel all rejected due to " << status << std::endl;
}

void LimeClient::placeReplace(ApiContext &ctx, bool usOptions) {
    char* orderIdStr;
    uint64_t orderId;
    char* quantityStr;
    int quantity;
    char* priceStr;
    double price;
    bool samePrice = false;
    TradingApi &api = ctx.getApi();

    if ((quantityStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify a quantity." << std::endl;
        return;
    }
    if ((quantity = parseStr(quantityStr)) == 0) {
        std::cout << "Quantity invalid." << std::endl;
        return;
    }
    if ((priceStr = strtok(NULL, " ")) == NULL) {
        std::cout << "Must specify a price." << std::endl;
        return;
    }
    if (strcmp(priceStr, "same") == 0)
        samePrice = true;
    else if ((price = atof(priceStr)) < 0.0) {
        std::cout << "Price invalid." << std::endl;
        return;
    }

    if ((orderIdStr = strtok(NULL, " ")) == NULL) {
        orderId = ctx.getPrevOrderId();
    }
    else if ((orderId = parseStr(orderIdStr)) == 0) {
        std::cout << "OrderId invalid." << std::endl;
        return;
    }

    OrderId replaceOrderId = ctx.generateOrderId();
    CallStatus status;
    try {
        if (samePrice)
            if (usOptions)
                status = api.cancelReplaceUSOptionsOrder(orderId,
                                                         replaceOrderId,
                                                         quantity);
            else 
                status = api.cancelReplaceOrder(orderId,
                                                replaceOrderId,
                                                quantity);
        else
            if (usOptions)
                status = api.cancelReplaceUSOptionsOrder(orderId,
                                                         replaceOrderId,
                                                         quantity,
                                                         (LimeBrokerage::ScaledPrice)(price * priceScalingFactor));
            else
                status = api.cancelReplaceOrder(orderId,
                                                replaceOrderId,
                                                quantity,
                                                (LimeBrokerage::ScaledPrice)(price * priceScalingFactor));
    } catch (const TradingApiException& exn) {
        handleApiException(ctx, exn);
    }

    if (status != statusSuccess)
        std::cout << "[LimeClient] cancel replace rejected due to " << status << std::endl;
}

void LimeClient::dumpApiStats(ApiContext &ctx) {
    TradingApi &api = ctx.getApi();
    api.debugDumpStatistics(std::cout);
}

void ApiContext::clearStats()
{
    getApi().debugResetStatistics();

    for (int i=0; i <= statusMax; ++i) {
        stats.orders[i] = 0;
        stats.cancels[i] = 0;
        stats.replaces[i] = 0;
    }
    stats.accepts = 0;
    stats.fills = 0;
    stats.usOptFills = 0;
    stats.cancelAcks = 0;
    stats.partialCancelAcks = 0;
    stats.replaceAcks = 0;
    stats.rejects = 0;
    stats.cancelRejects = 0;
    stats.crRejects = 0;
    stats.manualOrderEchoes = 0;
    stats.manualUsOptOrderEchoes = 0;
    stats.manualReplaceEchoes = 0;
}

void ApiContext::dumpStats()
{
    std::cout << "Statistics: " << std::endl
              << "      Orders       : " << stats.orders[0] << " " << stats.orders[1] << " " << stats.orders[2] << std::endl
              << "      Cancels      : " << stats.cancels[0] << " " << stats.cancels[1] << " " << stats.cancels[2] << std::endl
              << "      Replaces     : " << stats.replaces[0] << " " << stats.replaces[1] << " " << stats.replaces[2] << std::endl
              << "      Acks         : " << stats.accepts << std::endl
              << "      Fills        : " << stats.fills << std::endl
              << "      UsOptFills   : " << stats.usOptFills << std::endl
              << "      CancelAcks   : " << stats.cancelAcks << std::endl
              << "      ParCancelAcks: " << stats.partialCancelAcks << std::endl
              << "      ReplaceAcks  : " << stats.replaceAcks << std::endl
              << "      Rejects      : " << stats.rejects << std::endl
              << "      CancelRejects: " << stats.cancelRejects << std::endl
              << "      CrRejects    : " << stats.crRejects << std::endl
              << "      ManualOrders : " << stats.manualOrderEchoes << std::endl
              << "      ManualOptOrds: " << stats.manualUsOptOrderEchoes << std::endl
              << "      ManualCrs    : " << stats.manualReplaceEchoes << std::endl;
}

void LimeListener::onOrderReject(
    OrderId orderId,
    const std::string& reason,
    uint64_t eventId)
{
    ++apiCtx.stats.rejects;
    std::cout << "[LimeClient::onOrderReject]" << std::endl
              << "  orderId           : " << orderId << std::endl
              << "  eventId           : " << eventId << std::endl
              << "  reason            : " << reason.c_str() << std::endl;

    apiCtx.updateEventId(eventId);
}

void LimeListener::onOrderCancelReject(
    OrderId orderId,
    const std::string& reason,
    uint64_t eventId)
{
    ++apiCtx.stats.cancelRejects;
    std::cout << "[LimeClient::onOrderCancelReject]" << std::endl
              << "  orderId           : " << orderId << std::endl
              << "  eventId           : " << eventId << std::endl
              << "  reason            : " << reason.c_str() << std::endl;

    apiCtx.updateEventId(eventId);
}

void LimeListener::onOrderCancelReplaceReject(
    OrderId orderId,
    OrderId replaceOrderId,
    const std::string& reason,
    uint64_t eventId)
{
    ++apiCtx.stats.crRejects;
    std::cout << "[LimeClient::onOrderCancelReplaceReject]"
              << "  orderId           : " << orderId << std::endl
              << "  replaceOrderId    : " << replaceOrderId << std::endl
              << "  eventId           : " << eventId << std::endl
              << "  reason            : " << reason.c_str() << std::endl;

    apiCtx.updateEventId(eventId);
}

void LimeListener::onOrderAccept(
    OrderId orderId,
    OrderId limeOrderId,
    const OrderAckAttributes &attributes,
    uint64_t eventId)
{
    ++apiCtx.stats.accepts;
    std::cout << "[LimeClient::onOrderAccept]" << std::endl
              << "  orderId           : " << orderId << std::endl
              << "  limeOrderId       : " << limeOrderId << std::endl
              << "  eventId           : " << eventId << std::endl;

    if (attributes.adjustedPrice) {
        std::cout << "  adjusted price    : " << attributes.adjustedPrice / priceScalingFactor << std::endl;
    }
    if (attributes.displayPriceAdjusted) {
        std::cout << "  display price adjusted set" << std::endl;
    }
    if (attributes.displayPrice) {
        std::cout << "  display price     : " << attributes.displayPrice / priceScalingFactor << std::endl;
    }

    if (!attributes.marketClOrdId.empty()) {
        std::cout << "   market ClOrdID   : " << attributes.marketClOrdId << std::endl;
    }

    if (!attributes.marketOrderId.empty()) {
        std::cout << "   market order ID  : " << attributes.marketOrderId << std::endl;
    }
    apiCtx.updateEventId(eventId);
}

void LimeListener::onOrderFill(
    OrderId orderId,
    const FillInfo& fillInfo,
    uint64_t eventId)
{
    ++apiCtx.stats.fills;
    std::cout << "[LimeClient::onOrderFill]" << std::endl
              << "  orderId               : " << orderId  << std::endl
              << "  eventId               : " << eventId << std::endl
              << "  fillInfo              : " << std::endl
              << "     symbol             : " << fillInfo.symbol << std::endl
              << "     side               : " << fillInfo.side << std::endl
              << "     lastShares         : " << fillInfo.lastShares << std::endl
              << "     lastPrice          : " << fillInfo.lastPrice / priceScalingFactor << std::endl
              << "     leftQty            : " << fillInfo.leftQty << std::endl
              << "     liquidity          : " << fillInfo.liquidity << std::endl
              << "     transactTime       : " << fillInfo.transactTime << std::endl
              << "     execType           : " << fillInfo.execType << std::endl
              << "     execId             : " << fillInfo.execId << std::endl
              << "     contra             : " << fillInfo.contraBroker << std::endl;

    apiCtx.updateEventId(eventId);
}

void LimeListener::onUSOptionsOrderFill( 
    OrderId orderId, 
    const USOptionsFillInfo& fillInfo, 
    uint64_t eventId) 
{
    ++apiCtx.stats.usOptFills;
    char putOrCall[5] = "";
    if (fillInfo.symbol.putOrCall == USOptionSymbol::put) 
        memcpy(putOrCall, "put", 5);
    else
        memcpy(putOrCall, "call", 5);
    
    std::cout << "[TestListener::onUSOptionsOrderFill] " << std::endl 
              << " orderId                : " << orderId  << std::endl
              << " eventId                : " << eventId << std::endl
              << " fillInfo               : " << std::endl
              << "     baseSymbol         : " << fillInfo.symbol.baseSymbol << std::endl
              << "     putOrCall          : " << putOrCall << std::endl
              << "     expirationDay      : " << (int)fillInfo.symbol.expirationDay << std::endl
              << "     expirationMonth    : " << (int)fillInfo.symbol.expirationMonth << std::endl
              << "     expirationYear     : " << (int)fillInfo.symbol.expirationYear << std::endl
              << "     strikePrice        : " << fillInfo.symbol.strikePrice / usOptionStrikePriceScalingFactor << "."
              <<                                 fillInfo.symbol.strikePrice % usOptionStrikePriceScalingFactor << std::endl
              << "     side               : " << fillInfo.side << std::endl
              << "     lastShares         : " << fillInfo.lastShares << std::endl
              << "     lastPrice          : " << fillInfo.lastPrice / priceScalingFactor << "." << fillInfo.lastPrice % priceScalingFactor << std::endl
              << "     leftQty            : " << fillInfo.leftQty << std::endl
              << "     liquidity          : " << fillInfo.liquidity << std::endl
              << "     transactTime       : " << fillInfo.transactTime << std::endl
              << "     execType           : " << fillInfo.execType << std::endl
              << "     execId             : " << fillInfo.execId << std::endl
              << "     contra             : " << fillInfo.contraBroker << std::endl;
    
    apiCtx.updateEventId(eventId);
}

void LimeListener::onOrderCancel(
    OrderId orderId,
    uint64_t eventId)
{
    ++apiCtx.stats.cancelAcks;
    std::cout << "[LimeClient::onOrderCancel]" << std::endl
              << "  orderId           : " << orderId << std::endl
              << "  eventId           : " << eventId << std::endl;

    apiCtx.updateEventId(eventId);
}

void LimeListener::onOrderPartialCancel(
    OrderId orderId,
    int leftQty,
    uint64_t eventId)
{
    ++apiCtx.stats.partialCancelAcks;
    std::cout << "[LimeClient::onOrderPartialCancel]" << std::endl
              << "  orderId           : " << orderId << std::endl
              << "  leftQty           : " << leftQty << std::endl
              << "  eventId           : " << eventId << std::endl;

    apiCtx.updateEventId(eventId);
}

void LimeListener::onOrderReplace(
    OrderId orderId,
    OrderId replaceOrderId,
    OrderId limeReplaceOrderId,
    const OrderAckAttributes &attributes,
    uint64_t eventId)
{
    ++apiCtx.stats.replaceAcks;
    std::cout << "[LimeClient::onOrderReplace]" << std::endl
              << "  orderId           : " << orderId << std::endl
              << "  replaceId         : " << replaceOrderId << std::endl
              << "  limeReplaceId     : " << limeReplaceOrderId << std::endl
              << "  eventId           : " << eventId << std::endl;

    if (attributes.adjustedPrice) {
        std::cout << "  adjusted price    : " << attributes.adjustedPrice / priceScalingFactor << std::endl;
    }
    if (attributes.displayPriceAdjusted) {
        std::cout << "  display price adjusted set" << std::endl;
    }
    if (attributes.displayPrice) {
        std::cout << "  display price     : " << attributes.displayPrice / priceScalingFactor << std::endl;
    }

    if (!attributes.marketClOrdId.empty()) {
        std::cout << "   market ClOrdID   : " << attributes.marketClOrdId << std::endl;
    }

    if (!attributes.marketOrderId.empty()) {
        std::cout << "   market order ID  : " << attributes.marketOrderId << std::endl;
    }
    apiCtx.updateEventId(eventId);
}


void LimeListener::onManualOrder( 
    OrderId orderId, 
    const ManualOrderInfo& info,
    uint64_t eventId) 
{
    ++apiCtx.stats.manualOrderEchoes;
    
    std::cout << "[LimeListener::onManualOrder]" << std::endl
              << "  orderId            : " << orderId << std::endl
              << "  symbol             : " << info.symbol << std::endl
              << "  route              : " << info.route << std::endl
              << "  quantity           : " << info.quantity << std::endl
              << "  price              : " << info.price / priceScalingFactor << std::endl      
              << "  side               : " << info.side << std::endl
              << "  eventId            : " << eventId << std::endl;

    apiCtx.updateEventId(eventId);
}

void LimeListener::onManualUSOptionsOrder( 
    OrderId orderId, 
    const ManualUSOptionsOrderInfo& info,
    uint64_t eventId) 
{
    ++apiCtx.stats.manualUsOptOrderEchoes;
    char posEffect[10] = "open";
    if (info.positionEffect == positionEffectClose) 
        memcpy(posEffect, "close", 10);
    char putOrCall[5] = "";
    if (info.symbol.putOrCall == USOptionSymbol::put) 
        memcpy(putOrCall, "put", 5);
    else
        memcpy(putOrCall, "call", 5);
    
    std::cout << "[TestListener::onManualUSOptionsOrder]" << std::endl 
              << "  orderId            : " << orderId << std::endl
              << "     baseSymbol      : " << info.symbol.baseSymbol << std::endl
              << "     putOrCall       : " << putOrCall << std::endl
              << "     expirationDay   : " << (int)info.symbol.expirationDay << std::endl
              << "     expirationMonth : " << (int)info.symbol.expirationMonth << std::endl
              << "     expirationYear  : " << (int)info.symbol.expirationYear << std::endl
              << "     strikePrice     : " << info.symbol.strikePrice  / usOptionStrikePriceScalingFactor << "."
              <<                              info.symbol.strikePrice % usOptionStrikePriceScalingFactor << std::endl
              << "  route              : " << info.route << std::endl
              << "  quantity           : " << info.quantity << std::endl
              << "  price              : " << info.price / priceScalingFactor << std::endl      
              << "  side               : " << info.side << std::endl
              << "  positionEffect     : " << posEffect << std::endl
              << "  eventId            : " << eventId << std::endl;

    apiCtx.updateEventId(eventId);
}

void LimeListener::onManualOrderReplace( 
    OrderId orderId, 
    OrderId replaceOrderId, 
    int quantity,
    ScaledPrice price, 
    uint64_t eventId) 
{
    ++apiCtx.stats.manualReplaceEchoes;
    std::cout << "[LimeListener::onManualOrderReplace] " << std::endl
              << "  orderId           : " << orderId << std::endl
              << "  replaceId         : " << replaceOrderId << std::endl
              << "  quantity          : " << quantity << std::endl
              << "  price             : " << price /  priceScalingFactor << std::endl
              << "  eventId           : " << eventId << std::endl;

    apiCtx.updateEventId(eventId);
}


void LimeListener::onLoginAccepted()
{
    std::cout << "[LimeClient::onLoginAccepted] " << std::endl;
}

void LimeListener::onConnectionFailed(const std::string& reason)
{
    std::cout << "[LimeClient::onConnectionFailed] "
              << ": " << reason.c_str() << std::endl;
    apiCtx.quit();
}

void LimeListener::onConnectionBusy()
{
    std::cout << "[LimeClient::onConnectionBusy] " << std::endl;
}

void LimeListener::onConnectionAvailable()
{
    std::cout << "[LimeClient::onConnectionAvailable] @ " << std::endl;
}

void LimeListener::onLoginFailed(const std::string& reason)
{
    std::cout << "[LimeClient::onLoginFailed] "
              << ": " << reason.c_str() << std::endl;
    apiCtx.quit();
}

//int main(int argc, char* argv[]) {
//    std::cout << "[LimeClient] Starting example client..." << std::endl;
//
//    LimeClient* client = new LimeClient();
//
//    client->algo();
//}
