#include <exchsupport/eqt/lime/LimeApiContext.hpp>
#include <exchsupport/eqt/EqtGT4.hpp>
#include <string>
#include <map>
#include <sstream>

using namespace KTN::EQT;

LimeApiContext::LimeApiContext(
    KTN::EQT::EqtGT4& owner,
    char* account,
    char* user,
    char* password,
    char* hostname,
    uint64_t eventId,
    TransportType transport,
    bool cancelAllOnDisconnect,
    const cpu_set_t& cpus) : 
    stats(), 
    eventId(eventId), 
    listener(*this, owner), 
    owner(owner), 
    markForDeletion(false)
{
    currOrderId = (eventId == 0) ? 1 : eventId;
    id = owner.generateApiId();
    /*api = new TradingApi(listener, account, user, password, eventId, cancelAllOnDisconnect, hostname, transport, cpus);*/
    
    api = new TradingApi(listener, account, user, password, eventId, cancelAllOnDisconnect, hostname);
}

void LimeApiContext::clearStats()
{
    getApi().debugResetStatistics();

    for (int i = 0; i <= statusMax; ++i) {
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

void LimeApiContext::dumpStats()
{
    ostringstream oss;
    oss << "Statistics: " << std::endl
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
	KT01_LOG_INFO("LIME", oss.str());
}

void LimeApiContext::updateEventId(uint64_t eventId)
{ 
    owner.updateEventId(id, eventId);
}


 
// LimeApiListener
void LimeApiListener::onOrderReject(
    OrderId orderId,
    const std::string& reason,
    uint64_t eventId)
{
    ++apiCtx.stats.rejects;
    std::cout << "[LimeApiListener::onOrderReject]" << std::endl
        << "  orderId           : " << orderId << std::endl
        << "  eventId           : " << eventId << std::endl
        << "  reason            : " << reason.c_str() << std::endl;

    apiOwner.onOrderReject(orderId, reason, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onOrderCancelReject(
    OrderId orderId,
    const std::string& reason,
    uint64_t eventId)
{
    ++apiCtx.stats.cancelRejects;
    std::cout << "[LimeApiListener::onOrderCancelReject]" << std::endl
        << "  orderId           : " << orderId << std::endl
        << "  eventId           : " << eventId << std::endl
        << "  reason            : " << reason.c_str() << std::endl;

	apiOwner.onOrderCancelReject(orderId, reason, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onOrderCancelReplaceReject(
    OrderId orderId,
    OrderId replaceOrderId,
    const std::string& reason,
    uint64_t eventId)
{
    ++apiCtx.stats.crRejects;
    std::cout << "[LimeApiListener::onOrderCancelReplaceReject]"
        << "  orderId           : " << orderId << std::endl
        << "  replaceOrderId    : " << replaceOrderId << std::endl
        << "  eventId           : " << eventId << std::endl
        << "  reason            : " << reason.c_str() << std::endl;

	apiOwner.onOrderCancelReplaceReject(orderId, replaceOrderId, reason, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onOrderAccept(
    OrderId orderId,
    OrderId limeOrderId,
    const OrderAckAttributes& attributes,
    uint64_t eventId)
{
    ++apiCtx.stats.accepts;
    std::cout << "[LimeApiListener::onOrderAccept]" << std::endl
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

	apiOwner.onOrderAccept(orderId, limeOrderId, attributes, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onOrderFill(
    OrderId orderId,
    const FillInfo& fillInfo,
    uint64_t eventId)
{
    ++apiCtx.stats.fills;
    std::cout << "[LimeApiListener::onOrderFill]" << std::endl
        << "  orderId               : " << orderId << std::endl
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

	apiOwner.onOrderFill(orderId, fillInfo, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onUSOptionsOrderFill(
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

    std::cout << "[LimeApiListener::onUSOptionsOrderFill] " << std::endl
        << " orderId                : " << orderId << std::endl
        << " eventId                : " << eventId << std::endl
        << " fillInfo               : " << std::endl
        << "     baseSymbol         : " << fillInfo.symbol.baseSymbol << std::endl
        << "     putOrCall          : " << putOrCall << std::endl
        << "     expirationDay      : " << (int)fillInfo.symbol.expirationDay << std::endl
        << "     expirationMonth    : " << (int)fillInfo.symbol.expirationMonth << std::endl
        << "     expirationYear     : " << (int)fillInfo.symbol.expirationYear << std::endl
        << "     strikePrice        : " << fillInfo.symbol.strikePrice / usOptionStrikePriceScalingFactor << "."
        << fillInfo.symbol.strikePrice % usOptionStrikePriceScalingFactor << std::endl
        << "     side               : " << fillInfo.side << std::endl
        << "     lastShares         : " << fillInfo.lastShares << std::endl
        << "     lastPrice          : " << fillInfo.lastPrice / priceScalingFactor << "." << fillInfo.lastPrice % priceScalingFactor << std::endl
        << "     leftQty            : " << fillInfo.leftQty << std::endl
        << "     liquidity          : " << fillInfo.liquidity << std::endl
        << "     transactTime       : " << fillInfo.transactTime << std::endl
        << "     execType           : " << fillInfo.execType << std::endl
        << "     execId             : " << fillInfo.execId << std::endl
        << "     contra             : " << fillInfo.contraBroker << std::endl;

	apiOwner.onUSOptionsOrderFill(orderId, fillInfo, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onOrderCancel(
    OrderId orderId,
    uint64_t eventId)
{
    ++apiCtx.stats.cancelAcks;
    std::cout << "[LimeApiListener::onOrderCancel]" << std::endl
        << "  orderId           : " << orderId << std::endl
        << "  eventId           : " << eventId << std::endl;

	apiOwner.onOrderCancel(orderId, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onOrderPartialCancel(
    OrderId orderId,
    int leftQty,
    uint64_t eventId)
{
    ++apiCtx.stats.partialCancelAcks;
    std::cout << "[LimeApiListener::onOrderPartialCancel]" << std::endl
        << "  orderId           : " << orderId << std::endl
        << "  leftQty           : " << leftQty << std::endl
        << "  eventId           : " << eventId << std::endl;

	apiOwner.onOrderPartialCancel(orderId, leftQty, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onOrderReplace(
    OrderId orderId,
    OrderId replaceOrderId,
    OrderId limeReplaceOrderId,
    const OrderAckAttributes& attributes,
    uint64_t eventId)
{
    ++apiCtx.stats.replaceAcks;
    std::cout << "[LimeApiListener::onOrderReplace]" << std::endl
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

	apiOwner.onOrderReplace(orderId, replaceOrderId, limeReplaceOrderId, attributes, eventId);
    apiCtx.updateEventId(eventId);
}


void LimeApiListener::onManualOrder(
    OrderId orderId,
    const ManualOrderInfo& info,
    uint64_t eventId)
{
    ++apiCtx.stats.manualOrderEchoes;

    std::cout << "[LimeApiListener::onManualOrder]" << std::endl
        << "  orderId            : " << orderId << std::endl
        << "  symbol             : " << info.symbol << std::endl
        << "  route              : " << info.route << std::endl
        << "  quantity           : " << info.quantity << std::endl
        << "  price              : " << info.price / priceScalingFactor << std::endl
        << "  side               : " << info.side << std::endl
        << "  eventId            : " << eventId << std::endl;

    apiOwner.onManualOrder(orderId, info, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onManualUSOptionsOrder(
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

    std::cout << "[LimeApiListener::onManualUSOptionsOrder]" << std::endl
        << "  orderId            : " << orderId << std::endl
        << "     baseSymbol      : " << info.symbol.baseSymbol << std::endl
        << "     putOrCall       : " << putOrCall << std::endl
        << "     expirationDay   : " << (int)info.symbol.expirationDay << std::endl
        << "     expirationMonth : " << (int)info.symbol.expirationMonth << std::endl
        << "     expirationYear  : " << (int)info.symbol.expirationYear << std::endl
        << "     strikePrice     : " << info.symbol.strikePrice / usOptionStrikePriceScalingFactor << "."
        << info.symbol.strikePrice % usOptionStrikePriceScalingFactor << std::endl
        << "  route              : " << info.route << std::endl
        << "  quantity           : " << info.quantity << std::endl
        << "  price              : " << info.price / priceScalingFactor << std::endl
        << "  side               : " << info.side << std::endl
        << "  positionEffect     : " << posEffect << std::endl
        << "  eventId            : " << eventId << std::endl;

	apiOwner.onManualUSOptionsOrder(orderId, info, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onManualOrderReplace(
    OrderId orderId,
    OrderId replaceOrderId,
    int quantity,
    ScaledPrice price,
    uint64_t eventId)
{
    ++apiCtx.stats.manualReplaceEchoes;
    std::cout << "[LimeApiListener::onManualOrderReplace] " << std::endl
        << "  orderId           : " << orderId << std::endl
        << "  replaceId         : " << replaceOrderId << std::endl
        << "  quantity          : " << quantity << std::endl
        << "  price             : " << price / priceScalingFactor << std::endl
        << "  eventId           : " << eventId << std::endl;

	apiOwner.onManualOrderReplace(orderId, replaceOrderId, quantity, price, eventId);
    apiCtx.updateEventId(eventId);
}

void LimeApiListener::onLoginAccepted(uint64_t eventId)
{
    std::cout << "[LimeApiListener::onLoginAccepted] " << std::endl;
    apiCtx.updateEventId(eventId);  // Persist server's eventId for reconnect
    apiOwner.onLoginAccepted(eventId);
}

void LimeApiListener::onLoginAccepted()
{
    std::cout << "[LimeApiListener::onLoginAccepted] " << std::endl;
	apiOwner.onLoginAccepted();
}

void LimeApiListener::onConnectionFailed(const std::string& reason)
{
    std::cout << "[LimeApiListener::onConnectionFailed] "
        << ": " << reason.c_str() << std::endl;
	apiOwner.onConnectionFailed(reason);
    apiCtx.quit();
}

void LimeApiListener::onConnectionBusy()
{
    std::cout << "[LimeApiListener::onConnectionBusy] " << std::endl;
	apiOwner.onConnectionBusy();
}

void LimeApiListener::onConnectionAvailable()
{
    std::cout << "[LimeApiListener::onConnectionAvailable] @ " << std::endl;
	apiOwner.onConnectionAvailable();
}

void LimeApiListener::onLoginFailed(const std::string& reason)
{
    std::cout << "[LimeApiListener::onLoginFailed] "
        << ": " << reason.c_str() << std::endl;
	apiOwner.onLoginFailed(reason);
    apiCtx.quit();
}

