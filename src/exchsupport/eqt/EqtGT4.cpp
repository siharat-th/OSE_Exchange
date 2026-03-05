#include <exchsupport/eqt/EqtGT4.hpp>
#include <KT01/SecDefs/EquitySecMaster.hpp>
#include <Orders/OrderNums.hpp>
#include <filesystem>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <Orders/OrderManagerV5.hpp>
#include <akl/PriceConverters.hpp>
#include <KT01/Core/EpochNanos.hpp>
#include <exchsupport/eqt/lime/LimeOrderIdComponent.h>
#include <exchsupport/cme/il3/IL3Enums.hpp>

uint64_t getCurrentTimeNano() {
    KTN::Core::EpochNanos epochNanos;
    return epochNanos.Current();
}

using namespace KTN::EQT;
using namespace KT01::SECDEF::EQT;
using KTN::CME::IL3::EnumMassActionScope;

EqtGT4::EqtGT4(tbb::concurrent_queue<KTN::OrderPod>& qords, const std::string& settingsfile, const std::string& source) 
	:ExchangeBase2(settingsfile, source, qords, "EqtGT4")
	, currApiId(1)
{
    if (!EquitySecMaster::instance().IsLoaded())
        LogMe("LOADING EQT SECMASTER...");

    settings.Load(settingsfile);

    _State.ORG = settings.Org;
	_State.SOURCE = source;
    _State.EXCHNAME = settings.ExchName;
    _logsvc->ChangeName(_State.EXCHNAME, source);

    LogMe("LOG SVC CHANGED NAME TO " + _State.EXCHNAME + " SRC=" + source);

    Settings mainsett("Settings.txt");
    _State.ALGO_AFFINITY = mainsett.getInteger("AlgoAffinity");
    _State.logToScreen = settings.LogMessagesToScreen;

	eventIdSvc = new EventIdSvc(settings.EventIdDir, settings.EventIdFile);

    usleep(10000);

    _procthread = std::thread(&EqtGT4::ProcessingThread, this);

    Notification n = {};
    n.exch = _State.EXCHNAME;
    n.source = _State.SOURCE;
    n.org = _State.ORG;
    n.level = MsgLevel::INFO;
    n.imsg = MsgType::NOTICE;
    n.text = "STARTING EXCHANGE HANDLER: " + _State.EXCHNAME + " SOURCE=" + _State.SOURCE;

    NotifierRest::instance().Notify(n);

    Start();
}

EqtGT4::~EqtGT4() {
    _State.ACTIVE.store(false, std::memory_order_relaxed);
    _PROC_THREAD_ACTIVE.store(false, std::memory_order_relaxed);

    for (ApiContextTable::iterator it = apiCtxTable.begin(); it != apiCtxTable.end(); ++it)
		it->second->quit();
}

void EqtGT4::Stop() {
    LogMe("GOT STOP REQUEST");

    Instruction cmd = {};
    cmd.command = ExchCmd::TERMINATE;

    _qCmds.push(cmd);
    _State.EVENT_WAITING.store(true, std::memory_order_relaxed);
}

void EqtGT4::Start() {
    _State.CHECK_EVENTS_TRIG = _State.EVENTPOLLMAX * 2;
    _State.CHECK_EVENT_LOOP_COUNT = 0;
    _State.LOOPER = 0;
    _State.LOOP_TRIG = _State.EVENTPOLLMAX;
    _State.ACTIVE.store(true, std::memory_order_relaxed);

    cout << "[POLLERS DEBUG] EventPollerMax=" << _State.EVENTPOLLMAX
        << " CHECK_EVENTS_TRIG=" << _State.CHECK_EVENTS_TRIG
        << " LOOP_TRIG=" << _State.LOOP_TRIG
        << endl;

    // Load recovered orders into local cache (same pattern as CfeGT4/OrdZmq)
    auto& apiorders = OrderRecoveryService::instance().GetApiOrders();
    for (auto it = apiorders.begin(); it != apiorders.end(); ++it) {
        const KTN::Order& apiOrd = it->second;
        if (!KOrderExchange::isEquityVenue(apiOrd.OrdExch))
            continue;

        OrderPod pod = {};
        OrderUtils::Transpose(pod, apiOrd, apiOrd.ordernum);
        pod.orderReqId = OrderNums::instance().GenOrderReqId(apiOrd.ordernum);

        _sentOrders[pod.orderReqId] = pod;
        _ordernumToReqId[std::string(pod.ordernum)] = pod.orderReqId;

        LogMe("Recovery: loaded order " + std::string(pod.ordernum)
            + " REQID=" + std::to_string(pod.orderReqId)
            + " SECID=" + std::to_string(pod.secid)
            + " EXCHORDID=" + std::to_string(pod.exchordid));
    }

    login();
}

void EqtGT4::removeApiCtx(LimeApiContext* ctx) {
    ApiContextTable::iterator it;
    if ((it = apiCtxTable.find(ctx->getId())) != apiCtxTable.end()) {
        apiCtxTable.erase(it);
        delete (*it).second;
    }
}

void EqtGT4::updateEventId(int apiId, uint64_t eventId) {
    apiIdTable[apiId] = eventId; 
    persistEventId(eventId);
}

void EqtGT4::persistEventId(int eventId) {
	//std::string filepath = settings.EventIdDir + "/" + settings.EventIdFile;
	//// create directory if not exists
 //   std::filesystem::create_directories(std::filesystem::path(filepath).parent_path());
 //   int fd = open(filepath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
 //   if (fd == -1) {
 //       perror("open");
 //       return;
 //   }

 //   std::string data = std::to_string(eventId);
 //   write(fd, data.c_str(), data.size());
 //   fsync(fd);
 //   close(fd);
	eventIdSvc->Persist(eventId);
}

void EqtGT4::persistEventIdSync(int eventId) {
    // Synchronous write - use when immediate file update is required
    // (e.g., before login() which reads the file immediately)
    std::string filepath = settings.EventIdDir + "/" + settings.EventIdFile;
    std::ofstream file(filepath, std::ios::trunc);
    if (file.is_open()) {
        file << eventId;
        file.flush();
        file.close();
        LogMe("EventId sync write: " + std::to_string(eventId) + " -> " + filepath);
    } else {
        KT01_LOG_ERROR(__CLASS__, "Failed to open EventId file for sync write: " + filepath);
    }
}

int EqtGT4::loadPersistedEventId(const std::string& filepath) {
    //std::ifstream file(filepath);
    //if (!file.is_open()) {
    //    KT01_LOG_WARN(__CLASS__, "EventId file not found: " + filepath);
    //    return 0;
    //}
    //int value = 0;
    //file >> value;
    //if (file.fail() || file.bad()) {
    //    KT01_LOG_WARN(__CLASS__, "Failed to read eventId from file: " + filepath);
    //    return 0;
    //}
    //return value;

    // Make sure directory exists first
    std::filesystem::path eventIdPath(filepath);
    std::filesystem::path parentDir = eventIdPath.parent_path();

    try {
        // Create directory if it doesn't exist
        if (!std::filesystem::exists(parentDir)) {
            KT01_LOG_INFO(__CLASS__, "Creating directory for EventId: " + parentDir.string());
            std::filesystem::create_directories(parentDir);
        }

        // If file doesn't exist, create it with initial value
        if (!std::filesystem::exists(filepath)) {
            KT01_LOG_INFO(__CLASS__, "EventId file doesn't exist, creating new file: " + filepath);
            std::ofstream newFile(filepath);
            if (newFile.is_open()) {
                newFile << "0";
                newFile.close();
            }
            else {
                KT01_LOG_ERROR(__CLASS__, "Failed to create EventId file: " + filepath);
                return 0;
            }
        }

        // Check file size - if empty, write initial value
        if (std::filesystem::file_size(filepath) == 0) {
            KT01_LOG_INFO(__CLASS__, "EventId file is empty, initializing with 0: " + filepath);
            std::ofstream emptyFile(filepath);
            if (emptyFile.is_open()) {
                emptyFile << "0";
                emptyFile.close();
            }
            else {
                KT01_LOG_ERROR(__CLASS__, "Failed to write to empty EventId file: " + filepath);
                return 0;
            }
        }

        // Try to read the value
        std::ifstream file(filepath);
        if (!file.is_open()) {
            KT01_LOG_ERROR(__CLASS__, "Failed to open EventId file: " + filepath);
            return 0;
        }

        std::string content;
        std::getline(file, content);
        file.close();

        // Trim whitespace and check content
        content.erase(0, content.find_first_not_of(" \t\r\n"));
        content.erase(content.find_last_not_of(" \t\r\n") + 1);

        if (content.empty()) {
            KT01_LOG_WARN(__CLASS__, "EventId file contains only whitespace: " + filepath);
            return 0;
        }

        // Check if the content is a valid integer
        for (char c : content) {
            if (!std::isdigit(c)) {
                KT01_LOG_WARN(__CLASS__, "EventId file contains non-numeric data: '" + content + "'");
                return 0;
            }
        }

        int value = std::stoi(content);
        KT01_LOG_INFO(__CLASS__, "Successfully read EventId: " + std::to_string(value));
        return value;
    }
    catch (const std::invalid_argument& e) {
        KT01_LOG_ERROR(__CLASS__, "Invalid argument when parsing EventId: " + std::string(e.what()));
        return 0;
    }
    catch (const std::out_of_range& e) {
        KT01_LOG_ERROR(__CLASS__, "EventId value out of range: " + std::string(e.what()));
        return 0;
    }
    catch (const std::filesystem::filesystem_error& e) {
        KT01_LOG_ERROR(__CLASS__, "Filesystem error: " + std::string(e.what()));
        return 0;
    }
    catch (const std::exception& e) {
        KT01_LOG_ERROR(__CLASS__, "Error handling EventId file: " + std::string(e.what()));
        return 0;
    }
}


void EqtGT4::Reset() {

}

void EqtGT4::ProcessingThread() {
    LimeApiContext* ctx = NULL;
    _PROC_THREAD_ACTIVE.store(true, std::memory_order_relaxed);
    while (_PROC_THREAD_ACTIVE.load(std::memory_order_relaxed) == true)
    {
        if (ctx) {
            if (ctx->isMarkedForDeletion()) {
                removeApiCtx(ctx);
                ctx = NULL;
            }
        }
        else if (apiCtxTable.size()) {
            ctx = apiCtxTable.begin()->second;
        }

        // Reconnect timer check
        if (_shouldReconnect && !_reconnectInProgress) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - _lastDisconnectTime).count();
            if (elapsed >= _reconnectBackoffMs) {
                attemptReconnect();
            }
        }

        //cout << "[POLLERS DEBUG] ProcessingThread: " << _State.CHECK_EVENT_LOOP_COUNT << " vs " << _State.CHECK_EVENTS_TRIG << endl;
        if (_State.CHECK_EVENT_LOOP_COUNT >= _State.CHECK_EVENTS_TRIG)
            //if (_State.EVENT_WAITING.load(std::memory_order_relaxed) == true)
        {
            CheckEvents();
            _State.CHECK_EVENT_LOOP_COUNT = 0;
        }

        if (!_qHdgProc.empty())
        {
            std::vector<std::unique_ptr<KTNBuf>> vbufs;
            KTNBuf buf;
            while (_qHdgProc.try_pop(buf))
            {
                auto bufPtr = std::make_unique<KTNBuf>(buf);
                vbufs.push_back(std::move(bufPtr)); // Move unique_ptr into vector
            }

            ProcessHdgBuf(vbufs);
        }

        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

void EqtGT4::ClearMaps() {
    LogMe(_State.EXCHNAME + "CLEARING MAPS!");
    _msgratio.clear();
    _msgevents.clear();
    _hedgeOrds.clear();
    _hedgeMap.clear();
    _limeActiveReqId.clear();
    _origReqIdLookup.clear();
}

void EqtGT4::Command(Instruction cmd) {
    LogMe("*** [OK] " + _State.EXCHNAME + " SESS GOT GENERIC COMMAND REQUEST: " + ExchCmd::toString(cmd.command));

    switch (cmd.command)
    {
    case ExchCmd::SESS_STATUS:
        _qCmds.push(cmd);
        _State.EVENT_WAITING.store(true, std::memory_order_relaxed);
        break;
    case ExchCmd::MEASURE:
    {
        if (_State.MEASURE)
        {
            LogMe("DISABLING LATENCY MEASURING");
            _State.MEASURE = false;
        }
        else
        {
            LogMe("ENABLING LATENCY MEASURING");
            _State.MEASURE = true;
        }
    } break;

    default:
        cout << "[ExchCmd DEBUG] default switch: " << ExchCmd::toString(cmd.command) << ". EVENT_WAITING=True" << endl;
        _qCmds.push(cmd);
        _State.EVENT_WAITING.store(true, std::memory_order_relaxed);
    }
}

void EqtGT4::PrintLatencies(bool clear) {
    LogMe("--- LATENCY STATS ---");

    if (_State.MEASURE)
    {
        _State.MEASURE = false;
        _trk.printMeasurements();
        _State.MEASURE = true;
    }
    else
        LogMe("  ->MEASURE IS NOT ENABLED.");

    if (clear)
        _trk.clear();
}

void EqtGT4::Poll() {

}

void EqtGT4::CheckEvents()
{
    if (_State.EVENT_WAITING.load(std::memory_order_relaxed) == false)
        return;


    while (!_qCmds.empty())
    {
        Instruction cmd = {};
        _qCmds.try_pop(cmd);

        switch (cmd.command)
        {

        case ExchCmd::MENU_CHOICE:
        {
            if (cmd.cmd_value == 999)
            {
                runPriceBoundaryTest();
            }
            else if (cmd.cmd_value > 1000)
            {
                int newseq = cmd.cmd_value - 1000;
                LogMe("DANGER! GOT COMMAND > 1000 - SEQUENCE CHANGE FOR TESTING ONLY: SEQ=" + std::to_string(newseq));
                //conn.sessmgr.SetSeqIn(newseq);
            }
        }
        break;

        case ExchCmd::ALGO_PARAMS:
        case ExchCmd::ALGO_ORDER:
            AlgoJson(cmd.cmd_json, cmd.command);
            break;

        /*case ExchCmd::SESS_STATUS:
            PrintSessionMap(conn);
            break;*/

        case ExchCmd::CXL_ALL_SEGS:
        {
            KT01_LOG_WARN(__CLASS__, "GOT CXL ALL SEGS COMMAND!!!!");
            //SendMassCxl(conn, KOrderAction::ACTION_CXL_ALL, "VX", "FBN");
        }
        break;

        case ExchCmd::RISK_RESET:
        {
            KT01_LOG_WARN(__CLASS__, "GOT RISK RESET COMMAND!!!!");
            //SendRiskReset(conn);
        }
        break;

        case ExchCmd::CXL_ALL_INSTRUCTIONS:
        {
            KT01_LOG_WARN(__CLASS__, "GOT CXL ALL WITH INSTRUCTIONS COMMAND: " + cmd.cmd_text);
            //SendMassCxl(conn, KOrderAction::ACTION_CXL_ALL, "VX", cmd.cmd_text);
        }
        break;

        case ExchCmd::TERMINATE:
        {
            KT01_LOG_WARN(__CLASS__, "GOT LOGOUT/TERMINATE MESSAGE IN Q");
            for (ApiContextTable::iterator it = apiCtxTable.begin(); it != apiCtxTable.end(); ++it)
                it->second->quit();
        }
        break;
        case ExchCmd::PRINT_LATENCIES:
        {
            bool clear = (cmd.cmd_value == 12) ? true : false;
            PrintLatencies(clear);
        }
        break;
        }
    }

    _State.EVENT_WAITING.store(false, std::memory_order_relaxed);
}

void EqtGT4::ProcessHdgBuf(std::vector<std::unique_ptr<KTNBuf>>& vbufs)
{
    // Lime hedge orders go through HedgeComplex() → _qOrdsProc → Send() directly.
    // KTNBuf/ProcessHdgBuf pattern is only used by CME/CFE (raw TCP encoding).
    // This function is kept as a stub for ExchangeBase2 compatibility.
    if (!vbufs.empty()) {
        LogMe("WARNING: ProcessHdgBuf called with " + std::to_string(vbufs.size())
            + " buffers - unexpected for Lime exchange");
    }
}


void EqtGT4::Send(KTN::OrderPod& ord, int action) {
    // Check if we have an active API context
    if (apiCtxTable.empty()) {
        LogMe("Cannot send order - no active API context available");
        return;
    }

    // Store the mapping
    _orderReqIdToSecIdMap[ord.orderReqId] = ord.secid;

    //int msgtype = 0;

    // because we don't have the OrderRequestId like CME does, we need to store this info
    //_clordid_to_callback.insert(ord.ordernum, ord.callbackid);
    SetOrderCallback(ord.ordernum, ord.callbackid);

    // Get the first available API context
    LimeApiContext* ctx = apiCtxTable.begin()->second;
    TradingApi& api = ctx->getApi();
    
    OrderId orderId = ord.orderReqId;
    
    EquitySecDef secdef = SECMASTER_EQT->getSecDef(ord.secid);
    std::string symbol = secdef.baseSymbol;
	double displayFactor = secdef.displayFactor;
    
    // Convert price to Lime's format (scaled price)
    LimeBrokerage::ScaledPrice scaledPrice;

    // Check order type to determine price
    if (ord.OrdType == KOrderType::MARKET) {
        // Market order - use special constant
        scaledPrice = TradingApi::marketPrice;
    }
    else {
        // Limit order (or any other priced order)
        // Convert internal price (cents × 10^9) to Lime ScaledPrice (dollars × 10^4)
        scaledPrice = (ord.price > 0) ?
            static_cast<LimeBrokerage::ScaledPrice>(EQTConverter::ToWire(ord.price)) :
            TradingApi::marketPrice;
    }
    
    // Convert side (SEC Reg SHO: default sideSell, retry as short on reject)
    Side side;
    if (ord.OrdSide == KOrderSide::BUY) {
        side = sideBuy;
    } else {
        auto it = _netPosition.find(ord.secid);
        if (it != _netPosition.end()) {
            // Known position - use it to determine side
            side = (it->second >= static_cast<int>(ord.quantity)) ? sideSell : sideSellShort;
        } else {
            // Unknown position - default to sideSell, auto-retry as short if rejected
            side = sideSell;
        }
    }
    
    std::string route = secdef.fixRoute.empty() ? "LIME-ECN" : secdef.fixRoute;

    // Create OrderProperties and set TimeInForce based on ord.OrdTif
    TradingApi::OrderProperties properties;

    // Map KOrderTif to Lime's TimeInForce
    switch (ord.OrdTif) {
    case KOrderTif::DAY:
        properties.setTimeInForce(TradingApi::OrderProperties::timeInForceDay);
        break;
    case KOrderTif::FAK:  // IOC
        properties.setTimeInForce(TradingApi::OrderProperties::timeInForceIoc);
        break;
    case KOrderTif::FOK:
        // Lime doesn't have FOK for equities, use IOC + minQty workaround
        properties.setTimeInForce(TradingApi::OrderProperties::timeInForceIoc);
        properties.setMinQty(ord.quantity);  // All or nothing behavior
        break;
        // Add more mappings as needed
    default:
        properties.setTimeInForce(TradingApi::OrderProperties::timeInForceDay);
        break;
    }
    
    // Log order details if measuring is enabled
    if (_State.MEASURE) {
        ostringstream oss;
        oss << "Sending order via Lime API: Symbol=" << symbol
            << " Route=" << route
            << " Side=" << KOrderSide::toString(ord.OrdSide)
            << " LimeSide=" << (side == sideBuy ? "BUY" : (side == sideSellShort ? "SELL_SHORT" : "SELL"))
            << " Price=" << scaledPrice
            << " Qty=" << ord.quantity
            << " OrderId=" << orderId;
        LogMe(oss.str());
    }    // Call appropriate API based on action
    CallStatus status;

    // KTNBuf* msg = _KtnBufs.get();
    // Declare replaceOrderId outside of switch statement to avoid cross-initialization error
    OrderId replaceOrderId = 0;
    KTN::OrderPod originalOrder;
    try {
        switch (action) {
            case KOrderAction::ACTION_NEW:
                status = api.placeOrder(orderId, symbol, ord.quantity, scaledPrice, side, route, properties);
                _sentOrders[orderId] = ord;
                _ordernumToReqId[std::string(ord.ordernum)] = orderId;
                break;
            case KOrderAction::ACTION_MOD:
            {
                auto itNum = _ordernumToReqId.find(std::string(ord.origordernum));
                if (itNum != _ordernumToReqId.end()) {
                    auto itOrig = _sentOrders.find(itNum->second);
                    if (itOrig != _sentOrders.end()) {
                        originalOrder = itOrig->second;
                        OrderId originalOrderId = originalOrder.orderReqId;
                        // Translate to Lime-facing ID if order was previously modified
                        auto itMod = _limeActiveReqId.find(originalOrderId);
                        OrderId limeOrigId = (itMod != _limeActiveReqId.end()) ? itMod->second : originalOrderId;
                        replaceOrderId = OrderNums::instance().GenOrderReqId(std::string(ord.ordernum));
                        status = api.cancelReplaceOrder(limeOrigId, replaceOrderId, ord.quantity, scaledPrice);
                        _sentOrders[replaceOrderId] = ord;
                        _ordernumToReqId[std::string(ord.ordernum)] = replaceOrderId;
                    }
                }
                break;
            }
            case KOrderAction::ACTION_CXL:
                {
                    // Translate to Lime-facing ID if order was previously modified
                    OrderId cancelId = orderId;
                    auto itCxl = _limeActiveReqId.find(orderId);
                    if (itCxl != _limeActiveReqId.end())
                        cancelId = itCxl->second;
                    status = api.cancelOrder(cancelId);
                }
                break;
            default:
                LogMe("Unsupported order action: " + std::to_string(action));
                return;
        }
    } catch (const TradingApiException& exn) {
        LogMe("Exception while sending order: " + std::string(exn.what()));
        return;
    }
    
    if (status != statusSuccess) {
        LogMe("Order rejected with status: " + std::to_string(status));
    } else if (_State.MEASURE) {
        LogMe("Order successfully sent with ID: " + std::to_string(orderId));
    }
}

void EqtGT4::SendMassAction(KTN::Order& ord) {
    // NOTE: At this point, we just cancel all orders for all segements. This can change later  but for now...
    if (ord.massscope == EnumMassActionScope::EnumMassActionScope_MarketSegmentID)
    {
        Instruction cmd = {};
        cmd.command = ExchCmd::CXL_ALL_SEGS;
        cmd.cmd_value = EnumMassActionScope::EnumMassActionScope_MarketSegmentID;
        _qCmds.push(cmd);
        _State.EVENT_WAITING.store(true, std::memory_order_relaxed);
    }
}

void EqtGT4::SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3>& hdg) {
    if (_State.MEASURE)
        _trk.startMeasurement("HdgMsgMapIns");
    _hedgeMap[ordereqid] = hdg;
    //_hedgeMap.insert(ordereqid, hdg);
    if (_State.MEASURE)
        _trk.stopMeasurement("HdgMsgMapIns");
}

bool EqtGT4::HedgeComplex(uint64_t ordreqid, int lastqty, akl::Price lastpx, uint8_t cbid)
{
    auto hdgs = _hedgeMap.find(ordreqid);
    if (hdgs.size() == 0)
        return false;

    if (_State.MEASURE)
        LogMe("GOT HEDGE COMPLEX: CBID=" + std::to_string(cbid)
            + " REQID=" + std::to_string(ordreqid) + " QTY=" + std::to_string(lastqty));

    int i = 0;
    for (SpreadHedgeV3& hdg : hdgs)
    {
        // Adjust quantity by ratio
        if (LIKELY(hdg.ratio == 1))
            hdg.qty = lastqty;
        else
            hdg.qty = static_cast<int>(hdg.ratio * lastqty);

        if (hdg.cbid > 0)
            cbid = hdg.cbid;

        i++;
        uint64_t newreqid = ordreqid + i;

        // Create OrderPod and send through regular Send() path
        KTN::OrderPod ord = {};
        OrderUtils::fastCopy(ord.ordernum, hdg.ordernum, sizeof(ord.ordernum));
        ord.orderReqId = newreqid;
        ord.callbackid = cbid;
        ord.secid = hdg.secid;
        ord.OrdAction = KOrderAction::ACTION_NEW;
        ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_HEDGE;
        ord.OrdStatus = KOrderStatus::HEDGE_SENT;
        ord.quantity = hdg.qty;
        ord.price = hdg.price;
        ord.OrdSide = hdg.OrdSide;
        ord.leavesqty = hdg.qty;

        if (_State.MEASURE) {
            ostringstream oss;
            oss << "**HDG SENT " << ord.ordernum
                << " SIDE=" << KOrderSide::toString(ord.OrdSide)
                << " PX=" << ord.price
                << " QTY=" << ord.quantity
                << " SECID=" << ord.secid
                << " REQID=" << newreqid;
            LogMe(oss.str());
        }

        // Push to order processing queue (routes through Send() → Lime API)
        _qOrdsProc.push(ord);
    }

    hdgs.clear();
    return true;
}

// Lime
void EqtGT4::login() {
    char* account = const_cast<char*>(settings.Account.c_str());       // "AKLCAPITAL_TEST";
    char* user = const_cast<char*>(settings.Username.c_str());         // "AKLCAPITAL";
    char* password = const_cast<char*>(settings.Password.c_str());     // "limeAKLCAPITAL";
    char* hostname = const_cast<char*>(settings.Host.c_str());         // "74.120.50.45";
    char* eventIdStr;
    char* transportStr;
    int transportType = transportTypeTcp;

    // Load persisted eventId from file
    std::string eventIdFilePath = settings.EventIdDir + "/" + settings.EventIdFile;
    LogMe("Loading event ID from file: " + eventIdFilePath);
    int eventId = loadPersistedEventId(eventIdFilePath);
    LogMe("Current event ID: " + std::to_string(eventId));
    
    // Increment it by 1 for this session
    eventId++;
    LogMe("Using event ID for this session: " + std::to_string(eventId));

    // Only persist immediately for initial login, not reconnect attempts
    // For reconnects, EventId is persisted when login succeeds via onLoginAccepted()
    if (!_reconnectInProgress) {
        persistEventId(eventId);
    }

    int affinity = Settings("Settings.txt").getInteger("Affinity");
    
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
	int cpu = affinity;
    if (cpu < 0 || cpu >= CPU_SETSIZE) {
		KT01_LOG_WARN(__CLASS__, "Invalid CPU identifier");
        return;
    }
    CPU_SET(cpu, &cpus);

    try {
		onStateChange(EnumSessionState::StartingLoginProcess);
        LimeApiContext* apiContext = new LimeApiContext(*this, account, user, password, hostname, eventId, static_cast<TransportType>(transportType), true, cpus);
        apiCtxTable[apiContext->getId()] = apiContext;
    }
    catch (const TradingApiException& exn) {
        //std::cout << "[LimeClient] ***EXCEPTION***: " << exn.what() << std::endl;
		KT01_LOG_ERROR(__CLASS__, "Error in login: " + string(exn.what()));
		onStateChange(EnumSessionState::Disconnected);
    }
}

void EqtGT4::attemptReconnect() {
    _reconnectInProgress = true;
    _shouldReconnect = false;
    _reconnectAttempt++;

    // Notify on attempts: 1, 5, 10, 20, then every 40 (CFE pattern)
    bool shouldNotify = (_reconnectAttempt == 1) || (_reconnectAttempt == 5) ||
                        (_reconnectAttempt == 10) || (_reconnectAttempt == 20) ||
                        (_reconnectAttempt >= 40 && (_reconnectAttempt % 40 == 0));
    if (shouldNotify) {
        Notification n = {};
        n.exch = _State.EXCHNAME;
        n.source = _State.SOURCE;
        n.org = _State.ORG;
        n.level = MsgLevel::INFO;
        n.imsg = MsgType::NOTICE;
        n.text = "Reconnect attempt #" + std::to_string(_reconnectAttempt);
        NotifierRest::instance().Notify(n);
    }

    LogMe("Attempting reconnect #" + std::to_string(_reconnectAttempt));

    // Reset eventId to 0 after N failed attempts (threshold-based)
    // Use sync write because login() reads file immediately after
    const int EVENTID_RESET_THRESHOLD = 3;
    if (_reconnectAttempt >= EVENTID_RESET_THRESHOLD) {
        persistEventIdSync(0);
        if (_reconnectAttempt == EVENTID_RESET_THRESHOLD) {
            LogMe("Auto-resetting eventId to 0 after " + std::to_string(_reconnectAttempt) + " failed attempts");
        }
    }

    // Cleanup all existing API contexts
    for (auto& pair : apiCtxTable) {
        delete pair.second;
    }
    apiCtxTable.clear();
    apiIdTable.clear();

    // Re-login (creates new LimeApiContext)
    login();

    _reconnectInProgress = false;
}

void EqtGT4::runPriceBoundaryTest() {
    if (apiCtxTable.empty()) {
        LogMe("TEST: No active API context available");
        return;
    }

    _priceBoundaryTestMode = true;
    _testOrdersSendingComplete = false;
    _binarySearchPhase = 1;  // Start with coarse scan
    _testResults.clear();
    _pendingTestOrders.clear();
    _binarySearchStates.clear();

    // Get all @INET symbols from EquitySecMaster
    auto& secmaster = EquitySecMaster::instance();
    _testSymbols = secmaster.getBaseSymbolsForVenue("INET");

    LogMe("========== PRICE BOUNDARY TEST (Binary Search) ==========");
    LogMe("Phase 1: Coarse scan for " + std::to_string(_testSymbols.size()) + " @INET symbols");

    LimeApiContext* ctx = apiCtxTable.begin()->second;
    TradingApi& api = ctx->getApi();
    TradingApi::OrderProperties props;
    props.setTimeInForce(TradingApi::OrderProperties::timeInForceDay);

    // Phase 1: Coarse scan with few price points to find approximate range
    int64_t coarsePrices[] = {10000, 50000, 100000, 500000, 1000000, 5000000, 10000000, 50000000, 100000000};  // $1, $5, $10, $50, $100, $500, $1000, $5000, $10000
    int numCoarse = 9;

    int totalOrders = _testSymbols.size() * numCoarse;
    LogMe("Sending " + std::to_string(totalOrders) + " coarse scan orders...");

    OrderId testIdBase = 9900000;
    for (size_t s = 0; s < _testSymbols.size(); s++) {
        for (int p = 0; p < numCoarse; p++) {
            OrderId testId = testIdBase + s * 100 + p;
            TestOrderInfo info;
            info.symbol = _testSymbols[s];
            info.price = coarsePrices[p];
            _pendingTestOrders[testId] = info;
            api.placeOrder(testId, _testSymbols[s].c_str(), 1, coarsePrices[p], sideBuy, "LIME-ECN", props);
        }
        usleep(50000);  // 50ms between symbols
    }

    _testOrdersSendingComplete = true;
    LogMe("Coarse scan orders sent, waiting for responses...");
}

void EqtGT4::startBinarySearchPhase() {
    LogMe("Phase 2: Binary search for exact boundaries...");
    _binarySearchPhase = 2;
    _testOrdersSendingComplete = false;
    _pendingTestOrders.clear();

    // Initialize binary search state for each symbol based on coarse results
    for (const auto& sym : _testSymbols) {
        BinarySearchState state;
        state.symbol = sym;
        state.found = false;

        auto it = _testResults.find(sym);
        if (it != _testResults.end()) {
            int64_t maxAccepted = 0;
            int64_t minRejected = INT64_MAX;
            for (const auto& r : it->second) {
                if (r.second) maxAccepted = std::max(maxAccepted, r.first);
                else minRejected = std::min(minRejected, r.first);
            }

            if (maxAccepted > 0 && minRejected < INT64_MAX) {
                // We have a range to search
                state.low = maxAccepted;
                state.high = minRejected;
                state.current = (state.low + state.high) / 2;
                // Round to nearest $0.01 (100 scaled units)
                state.current = (state.current / 100) * 100;
            } else if (maxAccepted > 0) {
                // All passed - boundary is above our test range
                state.low = maxAccepted;
                state.high = maxAccepted;
                state.found = true;
            } else {
                // All rejected - boundary is below $1
                state.low = 0;
                state.high = minRejected;
                state.found = true;
            }
        } else {
            state.found = true;  // No results, skip
        }
        _binarySearchStates[sym] = state;
    }

    // Start binary search
    _currentSymbolIndex = 0;
    sendNextBinarySearchOrder();
}

void EqtGT4::sendNextBinarySearchOrder() {
    if (apiCtxTable.empty()) return;

    LimeApiContext* ctx = apiCtxTable.begin()->second;
    TradingApi& api = ctx->getApi();
    TradingApi::OrderProperties props;
    props.setTimeInForce(TradingApi::OrderProperties::timeInForceDay);

    // Find next symbol that needs testing
    while (_currentSymbolIndex < _testSymbols.size()) {
        const std::string& sym = _testSymbols[_currentSymbolIndex];
        BinarySearchState& state = _binarySearchStates[sym];

        // Check if converged (within $1 = 10000 scaled)
        if (!state.found && (state.high - state.low) > 10000) {
            // Send order at current price
            OrderId testId = 9800000 + _currentSymbolIndex * 1000 + (rand() % 1000);
            TestOrderInfo info;
            info.symbol = sym;
            info.price = state.current;
            _pendingTestOrders[testId] = info;

            api.placeOrder(testId, sym.c_str(), 1, state.current, sideBuy, "LIME-ECN", props);
            return;  // Wait for response
        } else {
            state.found = true;
            _currentSymbolIndex++;
        }
    }

    // All symbols done - print results
    _testOrdersSendingComplete = true;
    checkTestComplete();
}

void EqtGT4::processBinarySearchResponse(const std::string& symbol, int64_t price, bool accepted) {
    auto it = _binarySearchStates.find(symbol);
    if (it == _binarySearchStates.end()) return;

    BinarySearchState& state = it->second;

    if (accepted) {
        state.low = std::max(state.low, price);
    } else {
        state.high = std::min(state.high, price);
    }

    // Calculate next price to test
    state.current = (state.low + state.high) / 2;
    state.current = (state.current / 100) * 100;  // Round to $0.01

    // Check if converged
    if ((state.high - state.low) <= 10000) {
        state.found = true;
        _currentSymbolIndex++;
    }

    // Send next order
    sendNextBinarySearchOrder();
}

void EqtGT4::checkTestComplete() {
    if (!_testOrdersSendingComplete) return;
    if (!_pendingTestOrders.empty()) return;

    if (_binarySearchPhase == 1) {
        // Phase 1 complete - start binary search
        startBinarySearchPhase();
        return;
    }

    // Phase 2 complete - print final results
    _priceBoundaryTestMode = false;
    _testOrdersSendingComplete = false;
    _binarySearchPhase = 0;

    LogMe("========== PRICE BOUNDARY TEST RESULTS ==========");
    for (const auto& sym : _testSymbols) {
        auto it = _binarySearchStates.find(sym);
        if (it == _binarySearchStates.end()) continue;

        const BinarySearchState& state = it->second;
        ostringstream oss;
        oss << std::fixed << std::setprecision(2);

        if (state.low > 0 && state.high < INT64_MAX && state.high != state.low) {
            oss << sym << ": BOUNDARY $" << (state.low/10000.0) << " - $" << (state.high/10000.0)
                << " (max accepted: $" << (state.low/10000.0) << ")";
        } else if (state.low > 0) {
            oss << sym << ": MAX $" << (state.low/10000.0) << " (all tested OK)";
        } else {
            oss << sym << ": ALL REJECTED below $" << (state.high/10000.0);
        }
        LogMe(oss.str());
    }
    LogMe("=================================================");
}

void EqtGT4::dump() {
    LogMe("Dumping API context table (API ids):");
    for (ApiContextTable::iterator it = apiCtxTable.begin(); it != apiCtxTable.end(); ++it)
        LogMe(std::to_string((*it).first) + ",");
    LogMe("Dumping API eventId table (<API id, current eventId> pairs):");
    for (ApiIdTable::iterator it = apiIdTable.begin(); it != apiIdTable.end(); ++it)
        LogMe("(" + std::to_string((*it).first) + "," + std::to_string((*it).second) + "),");
}

void EqtGT4::dumpApiStats(LimeApiContext& ctx) {
    TradingApi& api = ctx.getApi();
    api.debugDumpStatistics(std::cout);
}

void EqtGT4::onStateChange(EnumSessionState::Enum newState) {
    if (newState == EnumSessionState::Established) {
		connectionAvailable = true;
    }
	connectionState.sessmgr.onStateChange(newState);
}

///////////////////////////////////////////////////////////
// Session Callbacks
////////////////////////////////////////////////////////////
void EqtGT4::onLoginFailed(const std::string& reason) {
    KT01_LOG_ERROR(__CLASS__, "Login failed: " + reason);
    connectionAvailable = false;
    onStateChange(EnumSessionState::Disconnected);

    if (!_hasNotifiedDisconnect) {
        Notification n = {};
        n.exch = _State.EXCHNAME;
        n.source = _State.SOURCE;
        n.org = _State.ORG;
        n.level = MsgLevel::ERROR;
        n.imsg = MsgType::ALERT;
        n.text = "LOGIN FAILED: " + reason;
        NotifierRest::instance().Notify(n);
        _hasNotifiedDisconnect = true;
    }

    // Exponential backoff: 15s, 30s, 60s, 120s, 240s, 300s cap (CFE pattern)
    int wait_times_ms[] = {15000, 30000, 60000, 120000, 240000, 300000};
    int attempt_index = std::min(_reconnectAttempt, 5);
    _reconnectBackoffMs = wait_times_ms[attempt_index];

    _lastDisconnectTime = std::chrono::steady_clock::now();
    _shouldReconnect = true;

    ostringstream oss;
    oss << "Will attempt reconnect in " << (_reconnectBackoffMs / 1000)
        << "s (attempt #" << (_reconnectAttempt + 1) << ")";
    LogMe(oss.str());
}

void EqtGT4::onLoginAccepted(uint64_t eventId) {
    LogMe("LOGIN ACCEPTED. EVENT ID: " + std::to_string(eventId));
    onStateChange(EnumSessionState::Established);

    // Always notify on successful login
    {
        Notification n = {};
        n.exch = _State.EXCHNAME;
        n.source = _State.SOURCE;
        n.org = _State.ORG;
        n.level = MsgLevel::INFO;
        n.imsg = MsgType::NOTICE;
        n.text = "LOGIN ACCEPTED. EventId=" + std::to_string(eventId);
        NotifierRest::instance().Notify(n);
    }

    // Reset reconnect tracking (CFE pattern)
    if (_reconnectAttempt > 0) {
        ostringstream oss;
        oss << "Reconnected successfully after " << _reconnectAttempt << " attempts";
        LogMe(oss.str());
    }
    _shouldReconnect = false;
    _reconnectAttempt = 0;
    _reconnectBackoffMs = 15000;
    _hasNotifiedDisconnect = false;
}

void EqtGT4::onLoginAccepted() {
    LogMe("LOGIN ACCEPTED. EVENT ID: 0 (no eventId provided). This is a legacy login without eventId support.");
    onStateChange(EnumSessionState::Established);

    // Always notify on successful login
    {
        Notification n = {};
        n.exch = _State.EXCHNAME;
        n.source = _State.SOURCE;
        n.org = _State.ORG;
        n.level = MsgLevel::INFO;
        n.imsg = MsgType::NOTICE;
        n.text = "LOGIN ACCEPTED (legacy)";
        NotifierRest::instance().Notify(n);
    }

    // Reset reconnect tracking (CFE pattern)
    if (_reconnectAttempt > 0) {
        ostringstream oss;
        oss << "Reconnected successfully after " << _reconnectAttempt << " attempts";
        LogMe(oss.str());
    }
    _shouldReconnect = false;
    _reconnectAttempt = 0;
    _reconnectBackoffMs = 15000;
    _hasNotifiedDisconnect = false;
}

void EqtGT4::onConnectionFailed(const std::string& reason) {
    KT01_LOG_ERROR(__CLASS__, "Connection failed: " + reason);
    connectionAvailable = false;
    onStateChange(EnumSessionState::Disconnected);

    if (!_hasNotifiedDisconnect) {
        Notification n = {};
        n.exch = _State.EXCHNAME;
        n.source = _State.SOURCE;
        n.org = _State.ORG;
        n.level = MsgLevel::ERROR;
        n.imsg = MsgType::ALERT;
        n.text = "CONNECTION FAILED: " + reason;
        NotifierRest::instance().Notify(n);
        _hasNotifiedDisconnect = true;
    }

    // Exponential backoff: 15s, 30s, 60s, 120s, 240s, 300s cap (CFE pattern)
    int wait_times_ms[] = {15000, 30000, 60000, 120000, 240000, 300000};
    int attempt_index = std::min(_reconnectAttempt, 5);
    _reconnectBackoffMs = wait_times_ms[attempt_index];

    _lastDisconnectTime = std::chrono::steady_clock::now();
    _shouldReconnect = true;

    ostringstream oss;
    oss << "Will attempt reconnect in " << (_reconnectBackoffMs / 1000)
        << "s (attempt #" << (_reconnectAttempt + 1) << ")";
    LogMe(oss.str());
}

void EqtGT4::onConnectionBusy() {
	connectionAvailable = false;
}

void EqtGT4::onConnectionAvailable() {
	connectionAvailable = true;
}

// When you need to look up secid from orderReqId
uint64_t EqtGT4::getSecidFromOrderReqId(uint64_t orderReqId) {
    if (_orderReqIdToSecIdMap.find(orderReqId) != _orderReqIdToSecIdMap.end()) {
        return _orderReqIdToSecIdMap[orderReqId];
    }
    return 0; // Default if not found
}
///////////////////////////////////////////////////////////
// API Callbacks
////////////////////////////////////////////////////////////
void EqtGT4::onOrderAccept(OrderId orderId, OrderId limeOrderId, const Listener::OrderAckAttributes& attributes, uint64_t eventId) {
    // Price boundary test mode - record result and skip logging/notification
    if (_priceBoundaryTestMode && _pendingTestOrders.count(orderId)) {
        auto& info = _pendingTestOrders[orderId];
        if (_binarySearchPhase == 2) {
            _pendingTestOrders.erase(orderId);
            processBinarySearchResponse(info.symbol, info.price, true);  // accepted
        } else {
            _testResults[info.symbol].push_back({info.price, true});  // accepted
            _pendingTestOrders.erase(orderId);
            checkTestComplete();
        }
        return;  // Don't log or notify
    }

    uint64_t reqId = orderId;
    auto itSentAccept = _sentOrders.find(reqId);
    if (itSentAccept != _sentOrders.end()) {
        itSentAccept->second.exchordid = limeOrderId;  // Update exchordid in cache
        KTN::OrderPod tmpOrd = itSentAccept->second;
        const std::string& marketClOrdId = attributes.marketClOrdId;
        char ClOrdID[marketClOrdId.size() + 1];
        std::strcpy(ClOrdID, marketClOrdId.c_str());
        int cbid = GetOrderCallback(ClOrdID);

        KTN::OrderPod* ord = _OrdPodPool.get();
        memset(ord->execid, 0, sizeof(ord->execid));
        ord->tradeDate = 0;
        ord->callbackid = cbid;
        ord->OrdStatus = KOrderStatus::NEW;
        ord->OrdState = KOrderState::WORKING;

		EquitySecDef def = SECMASTER_EQT->getSecDef(tmpOrd.secid);
        ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);

        ord->orderReqId = tmpOrd.orderReqId;  //Base63Codec::decode(ClOrdID);
        //now, we are tracking the orderReqId for the exchange order ID, since we can't rely on clordid since it changes
        SetOrderReqIdForExchExchId(orderId, ord->orderReqId);

		ord->secid = tmpOrd.secid;
        ord->exchordid = limeOrderId;
        ord->OrdSide = (attributes.side == sideBuy) ? KOrderSide::BUY : KOrderSide::SELL;

		ord->quantity = tmpOrd.quantity;
        ord->dispqty = tmpOrd.dispqty;
        ord->leavesqty = ord->quantity;
		ord->timestamp = getCurrentTimeNano();

        ord->OrdType = tmpOrd.OrdType;
        ord->OrdTif = tmpOrd.OrdTif;

        // Use exchange-adjusted price if available, then displayPrice fallback
        // Convert Lime ScaledPrice (dollars × 10^4) to internal (cents × 10^9)
        if (attributes.adjustedPrice != 0) {
            ord->price = EQTConverter::FromWire(attributes.adjustedPrice);
        } else if (attributes.displayPrice != 0) {
            ord->price = EQTConverter::FromWire(attributes.displayPrice);
        } else {
            ord->price = tmpOrd.price;
        }

        if (tmpOrd.OrdType == KOrderType::MARKET && _State.MEASURE) {
            ostringstream oss;
            oss << "onOrderAccept MARKET: adjustedPrice=" << attributes.adjustedPrice
                << " displayPrice=" << attributes.displayPrice
                << " final_price=" << ord->price;
            LogMe(oss.str());
        }

        ord->OrdOpenClose = KOrderOpenCloseInd::NONE;

        if (ord->OrdType == KOrderType::STOPLIMIT)
			ord->stopprice = tmpOrd.stopprice; // Use the stop price from the original order

        _qOrdsProc.push(*ord);
        _OrdPodPool.put(ord);

    }
}

void EqtGT4::onOrderFill(OrderId orderId, const Listener::FillInfo& fillInfo, uint64_t eventId) {
    // Price boundary test mode - skip processing (test orders get filled in simulator)
    if (_priceBoundaryTestMode && _pendingTestOrders.count(orderId)) {
        // Test order filled - count as accepted
        auto& info = _pendingTestOrders[orderId];
        if (_binarySearchPhase == 2) {
            _pendingTestOrders.erase(orderId);
            processBinarySearchResponse(info.symbol, info.price, true);  // accepted
        } else {
            _testResults[info.symbol].push_back({info.price, true});
            _pendingTestOrders.erase(orderId);
            checkTestComplete();
        }
        return;
    }

    // Resolve to original reqId if this was a replacement order
    uint64_t reqId = orderId;
    {
        auto itOrig = _origReqIdLookup.find(orderId);
        if (itOrig != _origReqIdLookup.end())
            reqId = itOrig->second;
    }
    auto itSentFill = _sentOrders.find(reqId);
    if (itSentFill != _sentOrders.end()) {
        KTN::OrderPod tmpOrd = itSentFill->second;

        // Get security ID from the map or the existing order
        uint64_t secId = getSecidFromOrderReqId(reqId);
        if (secId == 0) {
            secId = tmpOrd.secid;
        }

        // Check hedgeMap and send hedge orders if any
        bool sentHedge = HedgeComplex(reqId, fillInfo.lastShares,
            EQTConverter::FromWire(fillInfo.lastPrice),
            GetOrderCallback(tmpOrd.ordernum));

        // Create a new order pod for the fill
        KTN::OrderPod* ord = _OrdPodPool.get();

        // Basic order details
        ord->callbackid = GetOrderCallback(tmpOrd.ordernum); // Get callback ID if available
        ord->orderReqId = reqId;
        ord->secid = secId;
        //ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);
        EquitySecDef def = EquitySecMaster::instance().getSecDef(ord->secid);
        ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);
        ord->OrdType = tmpOrd.OrdType;
        ord->OrdTif = tmpOrd.OrdTif;

        // Convert Lime ScaledPrice (dollars × 10^4) to internal (cents × 10^9)
        ord->lastpx = EQTConverter::FromWire(fillInfo.lastPrice);
        ord->lastqty = fillInfo.lastShares;
        ord->timestamp = fillInfo.transactTimeNanos;

        // Copy over ClOrdID (order number) from the original order
        OrderUtils::fastCopy(ord->ordernum, tmpOrd.ordernum, sizeof(ord->ordernum));

        // Set order side from fill info
        ord->OrdSide = (fillInfo.side == sideBuy) ? KOrderSide::BUY : KOrderSide::SELL;

        // Set order status based on remaining quantity
        uint32_t leavesQty = fillInfo.leftQty;
        if (leavesQty > 0) {
            ord->OrdStatus = KOrderStatus::PARTIALLY_FILLED;
            ord->OrdState = KOrderState::WORKING;
        }
        else {
            ord->OrdStatus = KOrderStatus::FILLED;
            ord->OrdState = KOrderState::COMPLETE;
        }

        ord->leavesqty = leavesQty;
        
		ord->exchordid = tmpOrd.exchordid;
        // Use full execId directly
		OrderUtils::fastCopy(ord->execid, fillInfo.execId.c_str(), sizeof(ord->execid));
		// Still parse tradedate
        LimeOrderIdComponents comp = parseLimeOrderId(fillInfo.execId);
		ord->tradeDate = comp.date.empty() ? 0 : static_cast<uint32_t>(std::stoul(comp.date));

        // Determine fill type (default to outright)
        ord->OrdFillType = KOrderFillType::OUTRIGHT_FILL;

        // Set algorithm trigger if this was a hedge
        if (sentHedge) {
            ord->OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;
        }

        // Log the fill details
        if (_State.MEASURE) {
            ostringstream oss;
            oss << "[OK] onOrderFill: " << ord->ordernum << " REQID=" << reqId
                << " FILLTYPE=" << KOrderFillType::toString(ord->OrdFillType)
                << " SECID=" << ord->secid << " SIDE=" << KOrderSide::toString(ord->OrdSide)
                << " LASTQTY=" << ord->lastqty
                << " LASTPX=" << ord->lastpx << " LVSQTY=" << ord->leavesqty
                << " ORDSTATUS=" << KOrderStatus::toString(ord->OrdStatus)
                << " ORDSTATE=" << KOrderState::toString(ord->OrdState);
            LogMe(oss.str());
        }
        
        // Update net position for short sell tracking (SEC Reg SHO)
        // Only update if we already have a known position (learned from rejection).
        // Using operator[] would create a 0 entry for unknown positions, causing
        // subsequent sells to incorrectly use sideSellShort.
        auto itPos = _netPosition.find(ord->secid);
        if (itPos != _netPosition.end()) {
            int fillQty = static_cast<int>(fillInfo.lastShares);
            if (fillInfo.side == sideBuy || fillInfo.side == sideBuyToCover)
                itPos->second += fillQty;
            else
                itPos->second -= fillQty;
        }

        // Push the order to processing queue
        _qOrdsProc.push(*ord);
        _OrdPodPool.put(ord);

        // Clean up Lime reqId mapping when order completes
        if (ord->OrdState == KOrderState::COMPLETE) {
            auto itCleanup = _origReqIdLookup.find(orderId);
            if (itCleanup != _origReqIdLookup.end()) {
                _limeActiveReqId.erase(itCleanup->second);
                _origReqIdLookup.erase(itCleanup);
            }
            _sentOrders.erase(reqId);
            _ordernumToReqId.erase(std::string(tmpOrd.ordernum));
        }

    }
    else {
        KT01_LOG_ERROR(__CLASS__, "onOrderFill: Order not found in _sentOrders for reqId=" + std::to_string(reqId));
    }
}

void EqtGT4::onUSOptionsOrderFill(OrderId orderId, const Listener::USOptionsFillInfo& fillInfo, uint64_t eventId) {

}

void EqtGT4::onOrderCancel(OrderId orderId, uint64_t eventId) {
    // Resolve to original reqId if this was a replacement order
    uint64_t reqId = orderId;
    auto itOrig = _origReqIdLookup.find(orderId);
    if (itOrig != _origReqIdLookup.end()) {
        reqId = itOrig->second;
        _origReqIdLookup.erase(itOrig);
        _limeActiveReqId.erase(reqId);
    }
    auto itSentCancel = _sentOrders.find(reqId);
    if (itSentCancel != _sentOrders.end()) {
        KTN::OrderPod tmpOrd = itSentCancel->second;

        // Create a new order pod for the cancel
        KTN::OrderPod* ord = _OrdPodPool.get();
        memset(ord->execid, 0, sizeof(ord->execid));
        ord->tradeDate = 0;

        // Basic order details
        ord->callbackid = GetOrderCallback(tmpOrd.ordernum); // Get callback ID if available
        ord->orderReqId = reqId;
        ord->secid = tmpOrd.secid;
        //ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);
        EquitySecDef def = EquitySecMaster::instance().getSecDef(ord->secid);
        ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);

        // Set order status and state for cancellation
        ord->OrdStatus = KOrderStatus::CANCELED;
        ord->OrdState = KOrderState::COMPLETE;
        ord->OrdType = tmpOrd.OrdType;
        ord->OrdTif = tmpOrd.OrdTif;

        // Fill in remaining details from the original order
        ord->OrdSide = tmpOrd.OrdSide;
        ord->exchordid = tmpOrd.exchordid;
        ord->quantity = tmpOrd.quantity;
        ord->price = tmpOrd.price;
        ord->leavesqty = 0; // No leaves quantity since order is canceled
		ord->timestamp = getCurrentTimeNano();

        // Log the cancellation details
        if (_State.MEASURE) {
            ostringstream oss;
            oss << "[OK] onOrderCancel: " << ord->ordernum << " REQID=" << reqId
                << " SECID=" << ord->secid
                << " SIDE=" << KOrderSide::toString(ord->OrdSide)
                << " EXCHORDID=" << ord->exchordid
                << " ORDSTATUS=" << KOrderStatus::toString(ord->OrdStatus)
                << " ORDSTATE=" << KOrderState::toString(ord->OrdState);
            LogMe(oss.str());
        }

        // Push the order to processing queue
        _qOrdsProc.push(*ord);
        _OrdPodPool.put(ord);

        _sentOrders.erase(reqId);
        _ordernumToReqId.erase(std::string(tmpOrd.ordernum));
    }
    else {
        KT01_LOG_ERROR(__CLASS__, "onOrderCancel: Order not found in _sentOrders for reqId=" + std::to_string(reqId));
    }
}

void EqtGT4::onOrderPartialCancel(OrderId orderId, int leftQty, uint64_t eventId) {
    // Resolve to original reqId if this was a replacement order
    uint64_t reqId = orderId;
    {
        auto itOrig = _origReqIdLookup.find(orderId);
        if (itOrig != _origReqIdLookup.end())
            reqId = itOrig->second;
    }
    auto itSentPartCxl = _sentOrders.find(reqId);
    if (itSentPartCxl != _sentOrders.end()) {
        KTN::OrderPod tmpOrd = itSentPartCxl->second;

        // Create a new order pod for the partial cancel
        KTN::OrderPod* ord = _OrdPodPool.get();
        memset(ord->execid, 0, sizeof(ord->execid));
        ord->tradeDate = 0;

        // Basic order details
        ord->callbackid = GetOrderCallback(tmpOrd.ordernum);
        ord->orderReqId = reqId;
        ord->secid = tmpOrd.secid;
        EquitySecDef def = EquitySecMaster::instance().getSecDef(ord->secid);
        ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);

        // Set order status for partial cancellation (still working with reduced quantity)
        ord->OrdStatus = KOrderStatus::MODIFIED;
        ord->OrdState = KOrderState::WORKING;
        ord->OrdType = tmpOrd.OrdType;
        ord->OrdTif = tmpOrd.OrdTif;

        // Fill in remaining details from the original order
        ord->OrdSide = tmpOrd.OrdSide;
        ord->exchordid = tmpOrd.exchordid;
        ord->quantity = tmpOrd.quantity;
        ord->price = tmpOrd.price;

        // Update leaves quantity to the amount left after partial cancellation
        ord->leavesqty = leftQty;

        // Current timestamp
        ord->timestamp = time(NULL);

        // Log the partial cancellation details
        if (_State.MEASURE) {
            ostringstream oss;
            oss << "[OK] onOrderPartialCancel: " << ord->ordernum << " REQID=" << reqId
                << " SECID=" << ord->secid
                << " SIDE=" << KOrderSide::toString(ord->OrdSide)
                << " ORIGINAL_QTY=" << ord->quantity
                << " LEAVES_QTY=" << ord->leavesqty
                << " EXCHORDID=" << ord->exchordid
                << " ORDSTATUS=" << KOrderStatus::toString(ord->OrdStatus)
                << " ORDSTATE=" << KOrderState::toString(ord->OrdState);
            LogMe(oss.str());
        }

        // Push the order to processing queue
        _qOrdsProc.push(*ord);
        _OrdPodPool.put(ord);

    }
    else {
        KT01_LOG_ERROR(__CLASS__, "onOrderPartialCancel: Order not found in _sentOrders for reqId=" + std::to_string(reqId));
    }
}

void EqtGT4::onOrderReplace(OrderId orderId, OrderId replaceOrderId, OrderId limeReplaceOrderId, const Listener::OrderAckAttributes& attributes, uint64_t eventId) {
    // CFE pattern: use the ORIGINAL reqId to update the same order record in place.
    // Lime's cancel-replace uses different IDs, but internally we map back to the original.
    //
    // orderId        = the reqId we sent as "cancel this order" (original or previous Lime-facing ID)
    // replaceOrderId = the reqId we sent as "create this replacement" (from HandleModifyOrder)
    // limeReplaceOrderId = Lime's internal order ID for the replacement

    // Resolve to the true original reqId (handles modify-after-modify chain)
    uint64_t origReqId = orderId;
    auto itOrig = _origReqIdLookup.find(orderId);
    if (itOrig != _origReqIdLookup.end()) {
        origReqId = itOrig->second;
        _origReqIdLookup.erase(itOrig);
    }

    auto itSentReplace = _sentOrders.find(origReqId);
    if (itSentReplace != _sentOrders.end()) {
        KTN::OrderPod origOrd = itSentReplace->second;

        // Update Lime reqId mappings
        _limeActiveReqId[origReqId] = replaceOrderId;
        _origReqIdLookup[replaceOrderId] = origReqId;

        // Map Lime's internal order ID to our original reqId
        SetOrderReqIdForExchExchId(limeReplaceOrderId, origReqId);

        // Get the replacement order for updated price/quantity
        KTN::OrderPod replOrd = {};
        auto itReplOrd = _sentOrders.find(replaceOrderId);
        bool hasReplOrd = (itReplOrd != _sentOrders.end());
        if (hasReplOrd)
            replOrd = itReplOrd->second;

        // Build single MODIFIED event using original order's identity (like CFE onExecRptModify)
        KTN::OrderPod* ord = _OrdPodPool.get();
        memset(ord->execid, 0, sizeof(ord->execid));
        ord->tradeDate = 0;

        ord->callbackid = GetOrderCallback(origOrd.ordernum);
        ord->orderReqId = origReqId;  // Original reqId — updates _ords[origReqId] in place
        ord->secid = origOrd.secid;
        EquitySecDef def = EquitySecMaster::instance().getSecDef(ord->secid);
        ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);

        ord->OrdStatus = KOrderStatus::MODIFIED;
        ord->OrdState = KOrderState::WORKING;
        ord->OrdType = origOrd.OrdType;
        ord->OrdTif = origOrd.OrdTif;
        ord->OrdSide = origOrd.OrdSide;

        // Keep original exchordid (DB primary key — must not change)
        ord->exchordid = origOrd.exchordid;

        // Quantity/leaves from replacement order
        ord->quantity = hasReplOrd ? replOrd.quantity : origOrd.quantity;
        ord->leavesqty = ord->quantity;

        // Price: prefer Lime's adjustedPrice, fall back to replacement order, then original
        // Convert Lime ScaledPrice (dollars × 10^4) to internal (cents × 10^9)
        if (attributes.adjustedPrice != 0) {
            ord->price = EQTConverter::FromWire(attributes.adjustedPrice);
        } else if (hasReplOrd && replOrd.price != akl::Price{}) {
            ord->price = replOrd.price;
        } else {
            ord->price = origOrd.price;
        }

        // Copy original ordernum (same DB record, NEW → MODIFIED)
        OrderUtils::fastCopy(ord->ordernum, origOrd.ordernum, sizeof(ord->ordernum));
        OrderUtils::fastCopy(ord->origordernum, origOrd.origordernum, sizeof(ord->origordernum));

        ord->timestamp = getCurrentTimeNano();

        if (_State.MEASURE) {
            ostringstream oss;
            oss << "[OK] onOrderReplace: " << ord->ordernum << " ORIG_REQID=" << origReqId
                << " LIME_CANCEL_ID=" << orderId
                << " LIME_REPLACE_ID=" << replaceOrderId
                << " LIME_INTERNAL_ID=" << limeReplaceOrderId
                << " SECID=" << ord->secid
                << " SIDE=" << KOrderSide::toString(ord->OrdSide)
                << " QTY=" << ord->quantity
                << " PRICE=" << ord->price
                << " LEAVES_QTY=" << ord->leavesqty;
            LogMe(oss.str());
        }

        _qOrdsProc.push(*ord);
        _OrdPodPool.put(ord);

        // Update _sentOrders with new price/qty so subsequent callbacks see updated values
        itSentReplace->second.quantity = ord->quantity;
        itSentReplace->second.price = ord->price;
        itSentReplace->second.leavesqty = ord->leavesqty;
    }
    else {
        KT01_LOG_ERROR(__CLASS__, "onOrderReplace: Order not found in _sentOrders for origReqId=" + std::to_string(origReqId)
            + " (orderId=" + std::to_string(orderId) + ", replaceOrderId=" + std::to_string(replaceOrderId) + ")");
    }
}

void EqtGT4::onOrderReject(OrderId orderId, const std::string& reason, uint64_t eventId) {
    // Price boundary test mode - record result and skip logging/notification
    if (_priceBoundaryTestMode && _pendingTestOrders.count(orderId)) {
        auto& info = _pendingTestOrders[orderId];
        bool isPriceBoundary = (reason.find("price boundary") != std::string::npos);
        bool accepted = !isPriceBoundary;  // accepted if NOT price boundary reject
        if (_binarySearchPhase == 2) {
            _pendingTestOrders.erase(orderId);
            processBinarySearchResponse(info.symbol, info.price, accepted);
        } else {
            _testResults[info.symbol].push_back({info.price, accepted});
            _pendingTestOrders.erase(orderId);
            checkTestComplete();
        }
        return;  // Don't log or notify
    }

    // Auto-retry as short sell if Lime says position is flat/short (SEC Reg SHO)
    if (reason.find("must be marked short") != std::string::npos) {
        uint64_t reqId = orderId;
        auto itOrig = _origReqIdLookup.find(orderId);
        if (itOrig != _origReqIdLookup.end())
            reqId = itOrig->second;

        auto itSentRetry = _sentOrders.find(reqId);
        if (itSentRetry != _sentOrders.end()) {
            KTN::OrderPod tmpOrd = itSentRetry->second;

            // Learn that this secid has no long position
            _netPosition[tmpOrd.secid] = 0;

            // Generate new Lime orderId for retry
            OrderId retryId = OrderNums::instance().NextReqId();

            // Map retry ID back to original for callback routing (same pattern as cancel-replace)
            _origReqIdLookup[retryId] = reqId;
            _limeActiveReqId[reqId] = retryId;
            _orderReqIdToSecIdMap[retryId] = tmpOrd.secid;

            // Re-send with sideSellShort
            EquitySecDef secdef = EquitySecMaster::instance().getSecDef(tmpOrd.secid);
            std::string symbol = secdef.baseSymbol;
            std::string route = secdef.fixRoute.empty() ? "LIME-ECN" : secdef.fixRoute;
            LimeBrokerage::ScaledPrice scaledPrice = (tmpOrd.OrdType == KOrderType::MARKET) ?
                TradingApi::marketPrice :
                static_cast<LimeBrokerage::ScaledPrice>(EQTConverter::ToWire(tmpOrd.price));

            TradingApi::OrderProperties properties;
            switch (tmpOrd.OrdTif) {
            case KOrderTif::DAY:
                properties.setTimeInForce(TradingApi::OrderProperties::timeInForceDay);
                break;
            case KOrderTif::FAK:
                properties.setTimeInForce(TradingApi::OrderProperties::timeInForceIoc);
                break;
            case KOrderTif::FOK:
                properties.setTimeInForce(TradingApi::OrderProperties::timeInForceIoc);
                properties.setMinQty(tmpOrd.quantity);
                break;
            default:
                properties.setTimeInForce(TradingApi::OrderProperties::timeInForceDay);
                break;
            }

            LimeApiContext* ctx = apiCtxTable.begin()->second;
            TradingApi& api = ctx->getApi();

            try {
                CallStatus status = api.placeOrder(retryId, symbol, tmpOrd.quantity,
                    scaledPrice, sideSellShort, route, properties);

                LogMe("SHORT SELL RETRY: REQID=" + std::to_string(reqId)
                    + " RETRYID=" + std::to_string(retryId)
                    + " SYMBOL=" + symbol + " QTY=" + std::to_string(tmpOrd.quantity)
                    + " STATUS=" + std::to_string(status));
            } catch (const TradingApiException& exn) {
                LogMe("SHORT SELL RETRY FAILED: " + std::string(exn.what()));
            }

            return;  // Don't propagate reject to algo - retry replaces it
        }
    }

    // Map Lime orderId back to original reqId (handles retry/cancel-replace IDs)
    uint64_t reqId = orderId;
    auto itOrigReject = _origReqIdLookup.find(orderId);
    if (itOrigReject != _origReqIdLookup.end())
        reqId = itOrigReject->second;

    auto itSentReject = _sentOrders.find(reqId);
    if (itSentReject != _sentOrders.end()) {
        KTN::OrderPod tmpOrd = itSentReject->second;

        // Create a new order pod for the rejection
        KTN::OrderPod* ord = _OrdPodPool.get();
        memset(ord->execid, 0, sizeof(ord->execid));
        ord->tradeDate = 0;

        // Set basic order info
        ord->callbackid = GetOrderCallback(tmpOrd.ordernum);
        ord->orderReqId = reqId;
        ord->secid = tmpOrd.secid;
        //ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);
        EquitySecDef def = EquitySecMaster::instance().getSecDef(ord->secid);
        ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);

        // Set order status and state for rejection
        ord->OrdStatus = KOrderStatus::REJECTED;
        ord->OrdState = KOrderState::COMPLETE;
        ord->OrdType = tmpOrd.OrdType;
        ord->OrdTif = tmpOrd.OrdTif;

        // Fill in remaining details from the original order
        ord->OrdSide = tmpOrd.OrdSide;
        ord->exchordid = tmpOrd.exchordid;
        ord->quantity = tmpOrd.quantity;
        ord->price = tmpOrd.price;
        ord->leavesqty = 0;  // No leaves quantity for rejected order

        // Copy the rejection reason
        OrderUtils::fastCopy(ord->text, reason.c_str(), sizeof(ord->text));

        // Set order action to rejected
        ord->OrdAction = KOrderAction::ACTION_REJ;

        // Current timestamp
        ord->timestamp = time(NULL);

        // Log the rejection details
        if (_State.MEASURE) {
            ostringstream oss;
            oss << "[ERROR] onOrderReject: " << ord->ordernum << " REQID=" << reqId
                << " REASON=" << reason
                << " SECID=" << ord->secid
                << " SIDE=" << KOrderSide::toString(ord->OrdSide)
                << " ORDSTATUS=" << KOrderStatus::toString(ord->OrdStatus)
                << " ORDSTATE=" << KOrderState::toString(ord->OrdState);
            LogMe(oss.str());
        }

        // Send a notification about the rejection
        Notification n = {};
        n.exch = _State.EXCHNAME;
        n.source = _State.SOURCE;
        n.org = _State.ORG;
        n.level = MsgLevel::ERROR;
        n.imsg = MsgType::ALERT;
        {
            // Format: "Order rejected: AAPL@INET BUY $388.49 - Order price violates price boundary"
            double priceDisplay = static_cast<double>(ord->price) * def.displayFactor;
            ostringstream notifyMsg;
            notifyMsg << "Order rejected: " << def.symbol
                      << " " << KOrderSide::toString(ord->OrdSide)
                      << " $" << std::fixed << std::setprecision(2) << priceDisplay
                      << " - " << reason;
            n.text = notifyMsg.str();
        }
        NotifierRest::instance().Notify(n);

        // Push the order to processing queue
        if (ord->callbackid > 0)
            _qOrdsProc.push(*ord);

        _OrdPodPool.put(ord);

        _sentOrders.erase(reqId);
        _ordernumToReqId.erase(std::string(tmpOrd.ordernum));
    }
    else {
        KT01_LOG_ERROR(__CLASS__, "onOrderReject: Order not found in _sentOrders for reqId=" + std::to_string(reqId));
    }
}

void EqtGT4::onOrderCancelReject(OrderId orderId, const std::string& reason, uint64_t eventId) {
    // Map Lime orderId back to original reqId (handles retry/cancel-replace IDs)
    uint64_t reqId = orderId;
    auto itOrigCxlRej = _origReqIdLookup.find(orderId);
    if (itOrigCxlRej != _origReqIdLookup.end())
        reqId = itOrigCxlRej->second;

    auto itSentCxlRej = _sentOrders.find(reqId);
    if (itSentCxlRej != _sentOrders.end()) {
        KTN::OrderPod tmpOrd = itSentCxlRej->second;

        // Create a new order pod for the cancel rejection
        KTN::OrderPod* ord = _OrdPodPool.get();
        memset(ord->execid, 0, sizeof(ord->execid));
        ord->tradeDate = 0;

        // Basic order details
        ord->callbackid = GetOrderCallback(tmpOrd.ordernum);
        ord->orderReqId = reqId;
        ord->secid = tmpOrd.secid;
        //ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);
        EquitySecDef def = EquitySecMaster::instance().getSecDef(ord->secid);
        ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);

        ord->OrdState = KOrderState::COMPLETE;
        ord->OrdStatus = KOrderStatus::CXL_REJECT;
        ord->OrdAction = KOrderAction::ACTION_CXL_REJ;
        ord->OrdType = tmpOrd.OrdType;
        ord->OrdTif = tmpOrd.OrdTif;

        // Fill in remaining details from the original order
        ord->OrdSide = tmpOrd.OrdSide;
        ord->exchordid = tmpOrd.exchordid;
        ord->quantity = tmpOrd.quantity;
        ord->price = tmpOrd.price;
        ord->leavesqty = tmpOrd.leavesqty; // Leaves quantity remains unchanged

        // Copy the rejection reason
        OrderUtils::fastCopy(ord->text, reason.c_str(), sizeof(ord->text));

        // Current timestamp
        ord->timestamp = time(NULL);

        // Log the cancel rejection details
        if (_State.MEASURE) {
            ostringstream oss;
            oss << "[ERROR] onOrderCancelReject: " << ord->ordernum << " REQID=" << reqId
                << " REASON=" << reason
                << " SECID=" << ord->secid
                << " SIDE=" << KOrderSide::toString(ord->OrdSide)
                << " EXCHORDID=" << ord->exchordid
                << " ORDSTATUS=" << KOrderStatus::toString(ord->OrdStatus)
                << " ORDSTATE=" << KOrderState::toString(ord->OrdState);
            LogMe(oss.str());
        }

        // Send a notification about the cancel rejection
        Notification n = {};
        n.exch = _State.EXCHNAME;
        n.source = _State.SOURCE;
        n.org = _State.ORG;
        n.level = MsgLevel::ERROR;
        n.imsg = MsgType::ALERT;
        n.text = "Cancel order rejected: " + reason;
        NotifierRest::instance().Notify(n);

        // Push the order to processing queue
        if (ord->callbackid > 0)
            _qOrdsProc.push(*ord);

    }
    else {
        KT01_LOG_ERROR(__CLASS__, "onOrderCancelReject: Order not found in _sentOrders for reqId=" + std::to_string(reqId));
    }
}

void EqtGT4::onOrderCancelReplaceReject(OrderId orderId, OrderId replaceOrderId, const std::string& reason, uint64_t eventId) {
    uint64_t reqId = replaceOrderId;
    auto itSentCxlReplRej = _sentOrders.find(reqId);
    if (itSentCxlReplRej != _sentOrders.end()) {
        KTN::OrderPod tmpOrd = itSentCxlReplRej->second;

        // Create a new order pod for the cancel-replace rejection
        KTN::OrderPod* ord = _OrdPodPool.get();
        memset(ord->execid, 0, sizeof(ord->execid));
        ord->tradeDate = 0;

        // Basic order details
        ord->callbackid = GetOrderCallback(tmpOrd.ordernum);
        ord->orderReqId = reqId;
        ord->secid = tmpOrd.secid;
        //ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);
        EquitySecDef def = EquitySecMaster::instance().getSecDef(ord->secid);
        ord->OrdExch = KOrderExchange::Value(def.exchangeSymbol);

        // Set order status - it remains in its current state since cancel-replace was rejected
        ord->OrdStatus = KOrderStatus::CXL_REPL_REJECT;
        ord->OrdState = KOrderState::COMPLETE;  // The modify request is complete (rejected)
        ord->OrdAction = KOrderAction::ACTION_CXL_REPL_REJ;
        ord->OrdType = tmpOrd.OrdType;
        ord->OrdTif = tmpOrd.OrdTif;

        // Fill in remaining details from the original order
        ord->OrdSide = tmpOrd.OrdSide;
        ord->exchordid = tmpOrd.exchordid;
        ord->quantity = tmpOrd.quantity;
        ord->price = tmpOrd.price;
        ord->leavesqty = tmpOrd.leavesqty;  // Leaves quantity remains unchanged

        // Copy the rejection reason
        OrderUtils::fastCopy(ord->text, reason.c_str(), sizeof(ord->text));

        // Store the original order ID that was used in the replace request
        auto itOrigOrd = _sentOrders.find(orderId);
        if (itOrigOrd != _sentOrders.end()) {
            string origOrderNum = itOrigOrd->second.ordernum;
            OrderUtils::fastCopy(ord->origordernum, origOrderNum.c_str(), origOrderNum.length());
        }

        // Current timestamp
        ord->timestamp = time(NULL);
        ord->fillqty = 0;
        ord->leavesqty = 0;

        // Log the cancel-replace rejection details
        if (_State.MEASURE) {
            ostringstream oss;
            oss << "[ERROR] onOrderCancelReplaceReject: " << ord->ordernum
                << " REQID=" << reqId
                << " ORIG_ORDER_ID=" << orderId
                << " REPLACE_ORDER_ID=" << replaceOrderId
                << " REASON=" << reason
                << " SECID=" << ord->secid
                << " SIDE=" << KOrderSide::toString(ord->OrdSide)
                << " EXCHORDID=" << ord->exchordid
                << " ORDSTATUS=" << KOrderStatus::toString(ord->OrdStatus)
                << " ORDSTATE=" << KOrderState::toString(ord->OrdState);
            LogMe(oss.str());
        }

        // Send a notification about the cancel-replace rejection
        Notification n = {};
        n.exch = _State.EXCHNAME;
        n.source = _State.SOURCE;
        n.org = _State.ORG;
        n.level = MsgLevel::ERROR;
        n.imsg = MsgType::ALERT;
        n.text = "Cancel-Replace order rejected: " + reason;
        NotifierRest::instance().Notify(n);

        // Push the order to processing queue if there's a valid callback
        if (ord->callbackid > 0)
            _qOrdsProc.push(*ord);

        _OrdPodPool.put(ord);
    }
    else {
        KT01_LOG_ERROR(__CLASS__, "onOrderCancelReplaceReject: Order not found in _sentOrders for reqId=" + std::to_string(reqId));
    }
}

void EqtGT4::onManualOrder(OrderId orderId, const Listener::ManualOrderInfo& info, uint64_t eventId) {

}

void EqtGT4::onManualUSOptionsOrder(OrderId orderId, const Listener::ManualUSOptionsOrderInfo& info, uint64_t eventId) {

}

void EqtGT4::onManualOrderReplace(OrderId orderId, OrderId replaceOrderId, int quantity, ScaledPrice price, uint64_t eventId) {

}