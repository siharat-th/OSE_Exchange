#ifndef SRC_EXCHANGEHANDLERS_EQTGT4_HPP_  
#define SRC_EXCHANGEHANDLERS_EQTGT4_HPP_  

#pragma once  

#include <ExchangeHandler/ExchangeBase2.hpp>  
#include <akl/PriceConverters.hpp>  
#include <exchsupport/eqt/lime/limeTradingApi.hh>  
#include <exchsupport/eqt/lime/LimeApiContext.hpp>  
#include <exchsupport/eqt/settings/EqtLimeSettings.hpp>  
#include <exchsupport/eqt/storage/EventIdSvc.hpp>
#include <Orders/OrderRecoveryService.hpp>
#include <chrono>

using namespace KTN;  
using namespace KTN::Session;  
using namespace KTN::NET;  
using namespace KTN::Core;  
using namespace LimeBrokerage;  

namespace KTN { namespace EQT {  

class EqtGT4 : public ExchangeBase2  
{  
public:  
   explicit EqtGT4(tbb::concurrent_queue<KTN::OrderPod>& qords, const std::string& settingsfile, const std::string& source);  
   virtual ~EqtGT4();

   void Start();  
   void Stop();  
   void ClearMaps();  
   void Reset();  
   void Command(Instruction cmd);  
   void PrintLatencies(bool clear);  
   void Poll();  
   void ProcessingThread();  
   void Send(KTN::OrderPod& ord, int action);  
   void SendMassAction(KTN::Order& ord);  
   void SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3>& hdg);
   bool HedgeComplex(uint64_t ordreqid, int lastqty, akl::Price lastpx, uint8_t cbid);

   void dump();  
   int generateApiId() { return currApiId++; }  
   void removeApiCtx(LimeApiContext* ctx);  
   void updateEventId(int apiId, uint64_t eventId);  

private:  
   void ProcessHdgBuf(std::vector<std::unique_ptr<KTNBuf>>& vbufs);  
   // void onStateChange(EnumSessionState::Enum newState, ConnState& conn);
   void onStateChange(EnumSessionState::Enum newState);

   // Lime
   void login();
   void dumpApiStats(LimeApiContext& ctx);
   void attemptReconnect();

   typedef std::map<int, LimeApiContext*> ApiContextTable;  
   ApiContextTable apiCtxTable;  

   typedef std::map<int, uint64_t> ApiIdTable;  
   ApiIdTable apiIdTable;  
   int currApiId;  

   friend struct LimeApiContext;  

private:  
   void CheckEvents();  
   void persistEventId(int eventId);      // Async (via EventIdSvc)
   void persistEventIdSync(int eventId);  // Sync (direct file write)
   int loadPersistedEventId(const std::string& filepath);  
   uint64_t getSecidFromOrderReqId(uint64_t orderReqId);

   EqtLimeSettings settings;  
   EventIdSvc *eventIdSvc;

   ConnState connectionState;
   bool connectionAvailable;
   std::unordered_map<uint64_t, int> _orderReqIdToSecIdMap;

   // Net position per secid for short sell detection (SEC Reg SHO)
   std::unordered_map<uint64_t, int> _netPosition;

   // Local order cache - replaces GetOrderManager() dependency.
   // Keyed by orderReqId. Populated at Send() time and from OrderRecoveryService on startup.
   std::unordered_map<uint64_t, KTN::OrderPod> _sentOrders;
   // Reverse lookup: ordernum string → reqId (for cancel-replace ACTION_MOD)
   std::unordered_map<std::string, uint64_t> _ordernumToReqId;

   // Lime cancel-replace reqId mapping:
   // After a successful cancel-replace, Lime uses the replacement order's reqId for subsequent events.
   // Internally we use the original reqId to update the same order record in place (CFE pattern).
   std::unordered_map<OrderId, OrderId> _limeActiveReqId;   // origReqId → current Lime-facing reqId (for Send)
   std::unordered_map<OrderId, OrderId> _origReqIdLookup;   // Lime reqId → origReqId (for callbacks)

   // Reconnect tracking (adapted from CFE pattern)
   bool _shouldReconnect = false;
   bool _reconnectInProgress = false;
   int _reconnectAttempt = 0;
   int _reconnectBackoffMs = 15000;
   std::chrono::steady_clock::time_point _lastDisconnectTime;
   bool _hasNotifiedDisconnect = false;

   // Price boundary test mode with binary search
   struct TestOrderInfo {
       std::string symbol;
       int64_t price;
   };
   struct BinarySearchState {
       std::string symbol;
       int64_t low;      // lowest accepted price (scaled, 10000 = $1)
       int64_t high;     // lowest rejected price (scaled)
       int64_t current;  // current test price
       bool found;       // true when converged
   };
   bool _priceBoundaryTestMode = false;
   bool _testOrdersSendingComplete = false;
   int _binarySearchPhase = 0;  // 0=not started, 1=coarse scan, 2=binary search
   size_t _currentSymbolIndex = 0;
   std::vector<std::string> _testSymbols;
   std::map<std::string, BinarySearchState> _binarySearchStates;
   std::map<OrderId, TestOrderInfo> _pendingTestOrders;
   std::map<std::string, std::vector<std::pair<int64_t, bool>>> _testResults;
   void runPriceBoundaryTest();
   void checkTestComplete();
   void startBinarySearchPhase();
   void sendNextBinarySearchOrder();
   void processBinarySearchResponse(const std::string& symbol, int64_t price, bool accepted);

public:  
   void onOrderAccept(OrderId orderId, OrderId limeOrderId, const Listener::OrderAckAttributes& attributes, uint64_t eventId);  
   void onOrderFill(OrderId orderId, const Listener::FillInfo& fillInfo, uint64_t eventId);
   void onUSOptionsOrderFill(OrderId orderId, const Listener::USOptionsFillInfo& fillInfo, uint64_t eventId);
   void onOrderCancel(OrderId orderId, uint64_t eventId);  
   void onOrderPartialCancel(OrderId orderId, int leftQty, uint64_t eventId);  
   void onOrderReplace(OrderId orderId, OrderId replaceOrderId, OrderId limeReplaceOrderId, const Listener::OrderAckAttributes& attributes, uint64_t eventId);
   void onOrderReject(OrderId orderId, const std::string& reason, uint64_t eventId);  
   void onLoginFailed(const std::string& reason);  
   void onLoginAccepted(uint64_t eventId);  
   void onLoginAccepted();  
   void onManualOrder(OrderId orderId, const Listener::ManualOrderInfo& info, uint64_t eventId);
   void onManualUSOptionsOrder(OrderId orderId, const Listener::ManualUSOptionsOrderInfo& info, uint64_t eventId);
   void onManualOrderReplace(OrderId orderId, OrderId replaceOrderId, int quantity, ScaledPrice price, uint64_t eventId);  
   void onConnectionFailed(const std::string& reason);  
   void onConnectionBusy();  
   void onConnectionAvailable();  
   void onOrderCancelReject(OrderId orderId, const std::string& reason, uint64_t eventId);  
   void onOrderCancelReplaceReject(OrderId orderId, OrderId replaceOrderId, const std::string& reason, uint64_t eventId);  
};  

}}  

#endif /* SRC_EXCHANGEHANDLERS_EQTGT4_HPP_ */
