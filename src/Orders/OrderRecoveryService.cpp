#include <Orders/OrderRecoveryService.hpp>
using namespace KTN::REPORTER;

void OrderRecoveryService::Init(const std::string& settingsfilename) {

    Settings settings(settingsfilename);
    _source = settings.getString("Source");
   
   _reporter.emplace("OrdRecovery");

    algometa meta = {};
	meta.UniqueId = "ORDBOOK";
	meta.StratId = 0;
	meta.Strategy = "000";
	_reporter->UpdateMetaParams(meta);
}

void OrderRecoveryService::LoadFromApi() {
    auto ords = OrderApiFunctions::LoadOrdersFromApi(_source, "");
    for (const auto& ord : ords) {
        _apiorders[ord.exchordid] = ord;
    }

    cout << OrderUtils::PrintOrdersTable(ords) << endl;
}

bool OrderRecoveryService::HasOrder(uint64_t exchordid) {
    return _apiorders.find(exchordid) != _apiorders.end();
}

bool OrderRecoveryService::GetOrder(uint64_t exchordid, KTN::Order& out) {
    auto it = _apiorders.find(exchordid);
    if (it == _apiorders.end()) return false;
    out = it->second;
    return true;
}

void OrderRecoveryService::RemoveOrder(uint64_t exchordid) {
    _apiorders.unsafe_erase(exchordid);
}

// void OrderRecoveryService::AddSuspenseOrder(const std::string& guid, const KTN::OrderPod& ord) {
//     _suspenseOrds[guid][ord.cmeid] = ord;
// }

// std::vector<KTN::OrderPod> OrderRecoveryService::GetSuspenseOrders(const std::string& guid) {
//     std::vector<KTN::OrderPod> out;
//     auto it = _suspenseOrds.find(guid);
//     if (it != _suspenseOrds.end()) {
//         for (const auto& kv : it->second)
//             out.push_back(kv.second);
//     }
//     return out;
// }

// void OrderRecoveryService::RemoveSuspenseOrder(const std::string& guid, uint64_t cmeid) {
//     _suspenseOrds[guid].erase(cmeid);
// }

void OrderRecoveryService::UpdateOrder(KTN::OrderPod& ord) 
{
    string tag = "";
    UserInfo ugo;

    //KOrderState::Enum origstate = ord.OrdState;


    if (_apiorders.find(ord.exchordid) != _apiorders.end()) {
        KTN::Order apiord = _apiorders[ord.exchordid];
        cout << "[OrderReoveryService OK] Found order in API orders: " << apiord.ordernum << endl;
        tag = apiord.tag;
        
        ugo.user = apiord.user;
        ugo.org = apiord.org;
        ugo.groupname = apiord.groupname;

        _reporter->UpdateUGOMap(ord.orderReqId, ugo);

        if (!tag.empty()) 
            _reporter->UpdateTagMap(ord.orderReqId, tag);

        KTN::OrderPod neword = ord;
        OrderUtils::UpdateFromApi(neword, apiord);

        neword.leavesqty = ord.leavesqty;
        if (neword.leavesqty < neword.quantity)
        {
            neword.fillqty = neword.quantity - neword.leavesqty;
        }

        if (ord.leavesqty > 0 && ord.leavesqty < ord.quantity)
        {
            neword.OrdStatus = KOrderStatus::PARTIALLY_FILLED;
        }
        
        //take into account STATUS_UPDATE...
        if (ord.leavesqty > 0)
            neword.OrdState = KOrderState::WORKING;
        else
            neword.OrdState = KOrderState::COMPLETE;
      
        //update api order fields 
        OrderUtils::Transpose(apiord, neword);
        _apiorders[ord.exchordid] = apiord;

        cout << "[OrderRecoveryService OK] Updating order: " << OrderUtils::Print(apiord) << endl;

        algometa meta = {};
        meta.StratId = apiord.stratid;
        meta.Strategy = apiord.strategy;
        meta.UniqueId = apiord.algoid;
        meta.Tag = tag;
        meta.userinfo = ugo;

        _metamap[ord.exchordid] = meta;

       neword.hasMeta = true;
       cout << "[OrderRecoveryService OK] Sending order to reporter: " << OrderUtils::Print(neword) << endl;
       ReportOrder(neword, meta);

       // Update the incoming order so its changes are relected in the sender
       ord = neword;

    }
    else{
        KT01_LOG_ERROR(__CLASS__, "Order not found in API orders. Unable to report order.");
        return;
    }
}

void OrderRecoveryService::ReportOrder(KTN::OrderPod& ord, algometa& meta) 
{
    ord.hasMeta =true;
    cout << "[OrderRecoveryService OK] Reporting order: " << OrderUtils::Print(ord) << endl;
    _reporter->ReportWithMeta(ord, meta);
}

void OrderRecoveryService::Stop() {
    _reporter->Stop();
}
