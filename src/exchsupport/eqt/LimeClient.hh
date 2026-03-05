#include <exchsupport/eqt/lime/limeTradingApi.hh>

#include <string>
#include <map>

using namespace LimeBrokerage;

class ApiContext;

class LimeListener : public Listener {
public:

    LimeListener(ApiContext &ctx) : apiCtx(ctx) {}

    void onOrderAccept( 
        OrderId orderId, 
        OrderId limeOrderId, 
        const OrderAckAttributes &attributes,
        uint64_t eventId);

    void onOrderFill( 
        OrderId orderId, 
        const FillInfo& fillInfo, 
        uint64_t eventId);
    
    void onUSOptionsOrderFill( 
        OrderId orderId, 
        const USOptionsFillInfo& fillInfo, 
        uint64_t eventId);
    
    void onOrderCancel( 
        OrderId orderId, 
        uint64_t eventId);
    
    void onOrderPartialCancel(
        OrderId orderId,
        int leftQty,
        uint64_t eventId);

    void onOrderReplace( 
        OrderId orderId, 
        OrderId replaceOrderId, 
        OrderId limeReplaceOrderId, 
        const OrderAckAttributes &attributes,
        uint64_t eventId);
    
    void onOrderReject( 
        OrderId orderId, 
        const std::string& reason, 
        uint64_t eventId);

    void onLoginFailed(
        const std::string& reason);

    void onLoginAccepted();

    void onManualOrder(
        OrderId orderId,
        const ManualOrderInfo& info,
        uint64_t eventId);

    void onManualUSOptionsOrder(
        OrderId orderId,
        const ManualUSOptionsOrderInfo& info,
        uint64_t eventId);

    void onManualOrderReplace(
        OrderId orderId,
        OrderId replaceOrderId,
        int quantity,
        ScaledPrice price, 
        uint64_t eventId);

    void onConnectionFailed(
        const std::string& reason);

    void onConnectionBusy();
    void onConnectionAvailable();

    void onOrderCancelReject( 
        OrderId orderId, 
        const std::string& reason,
        uint64_t eventId);

    void onOrderCancelReplaceReject( 
        OrderId orderId, 
        OrderId replaceOrderId, 
        const std::string& reason,
        uint64_t eventId);

private:
    ApiContext &apiCtx;
};

class LimeClient {
public:
    LimeClient() : currApiId(1) {}
    void algo();
    void dump();

    int generateApiId() { return currApiId++; }
    void removeApiCtx(ApiContext* ctx);
    void updateEventId(int apiId, uint64_t eventId) { apiIdTable[apiId] = eventId; }

    uint64_t parseStr(char* s)
    {
        uint64_t value = *s - '0';
        while (*++s >= '0' && *s <= '9')
            value = value * 10 + (*s - '0');
        return value;
    }

private:

    void login();
    void setCpuAffinity(ApiContext &ctx);
    void resetCpuAffinity(ApiContext &ctx);
    void placeOrder(ApiContext &ctx);
    void placeUSOptionsOrder(ApiContext &ctx);
    void placeCancel(ApiContext &ctx);
    void placeCancelAll(ApiContext &ctx);
    void placeReplace(ApiContext &ctx, bool usOptions = false);
    void handleApiException(ApiContext &ctx, const TradingApiException &exn);
    void dumpApiStats(ApiContext &ctx);

    typedef std::map<int, ApiContext*> ApiContextTable;
    ApiContextTable apiCtxTable;

    typedef std::map<int, uint64_t> ApiIdTable;
    ApiIdTable apiIdTable;
    int currApiId;

    friend struct ApiContext;
};

class ApiContext {
public:
    ApiContext(LimeClient &owner, 
               char* account, 
               char* user, 
               char* password, 
               char* hostname, 
               uint64_t eventId,
               TransportType transport,
               bool cancelAllOnDisconnect,
               const cpu_set_t& cpus); 
    ~ApiContext() { delete api; }
    
    struct Statistics {

        Statistics() :
            rejects(0), cancelRejects(0), crRejects(0), accepts(0),
            fills(0), usOptFills(0), cancelAcks(0), partialCancelAcks(0),replaceAcks(0),
            manualOrderEchoes(0), manualUsOptOrderEchoes(0), manualReplaceEchoes(0)
        {
            for (int i=0; i <= statusMax; ++i) {
                orders[i] = 0;
                cancels[i] = 0;
                replaces[i] = 0;
            }
        }

        uint32_t orders[statusMax + 1];
        uint32_t cancels[statusMax + 1];
        uint32_t replaces[statusMax + 1];
        uint32_t rejects;
        uint32_t cancelRejects;
        uint32_t crRejects;
        uint32_t accepts;
        uint32_t fills;
        uint32_t usOptFills;
        uint32_t cancelAcks;
        uint32_t partialCancelAcks;
        uint32_t replaceAcks;
        uint32_t manualOrderEchoes;
        uint32_t manualUsOptOrderEchoes;
        uint32_t manualReplaceEchoes;
    } stats;

    TradingApi& getApi() const { return *api; }
    int getId() const { return id; }
    LimeBrokerage::OrderId generateOrderId() { return currOrderId++; }
    LimeBrokerage::OrderId getPrevOrderId() { return currOrderId - 1; }
    void updateEventId(uint64_t eventId) { owner.updateEventId(id, eventId); }
    void dumpStats();
    void clearStats();
    bool isMarkedForDeletion() { return markForDeletion; }
    void quit() { markForDeletion = true; }

private:
    
    int id;
    int eventId;
    TradingApi* api;    
    LimeBrokerage::OrderId currOrderId;
    LimeListener listener;
    LimeClient &owner;
    bool markForDeletion;
};

