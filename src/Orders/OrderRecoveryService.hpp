//============================================================================
// Name        	: OrderRecoveryService.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 13,2025 10:16:58 AM
//============================================================================

#ifndef SRC_ORDERS_ORDERRECOVERYSERVICE_HPP_
#define SRC_ORDERS_ORDERRECOVERYSERVICE_HPP_

#pragma once



#include <KT01/Core/Singleton.hpp>

#include <Orders/Order.hpp>
#include <Orders/OrderPod.hpp>
#include <Orders/OrderUtils.hpp>

#include <Reporters/OrderReporterV6.hpp>
#include <AlgoSupport/algo_structs.hpp>
#include <Orders/OrderApiFunctions.hpp>
#include <KT01/Core/Settings.hpp>

#include <optional>
#include <tbb/concurrent_unordered_map.h>


using namespace KTN::REPORTER;


class OrderRecoveryService : public Singleton<OrderRecoveryService> {
    friend class Singleton<OrderRecoveryService>;

private:
    std::string _source;

    tbb::concurrent_unordered_map<uint64_t, KTN::Order> _apiorders; // cmeid -> Order
    tbb::concurrent_unordered_map<uint64_t, algometa> _metamap;
    //tbb::concurrent_unordered_map<std::string, std::map<uint64_t, KTN::OrderPod>> _suspenseOrds; // algoid -> cmeid -> OrderPod
    //tbb::concurrent_unordered_map<uint64_t, UserInfo> _ugoMap;

    std::optional<OrderReporterV6> _reporter;


    OrderRecoveryService() {}

public:
    void Init(const std::string& settingsfilename);

    void LoadFromApi(); // fills _apiorders

    bool HasOrder(uint64_t cmeid);
    bool GetOrder(uint64_t cmeid, KTN::Order& out);
    void RemoveOrder(uint64_t cmeid);

    //void AddSuspenseOrder(const std::string& guid, const KTN::OrderPod& ord);
    //std::vector<KTN::OrderPod> GetSuspenseOrders(const std::string& guid);
    //void RemoveSuspenseOrder(const std::string& guid, uint64_t cmeid);

    void UpdateOrder(KTN::OrderPod& ord);
    void ReportOrder(KTN::OrderPod& ord, algometa& meta);

    bool CheckMeta(uint64_t cmeid, algometa& meta) {    
        auto it = _metamap.find(cmeid);
        if (it != _metamap.end()) 
        {
            meta = it->second;
            return true;
        }
        return false;;
    }

    void Stop();

    tbb::concurrent_unordered_map<uint64_t, KTN::Order> &GetApiOrders() { return _apiorders; }
};


#endif /* SRC_ORDERS_ORDERRECOVERYSERVICE_HPP_ */
