/*
 * OrderApiFunctions.hpp
 *
 *  Created on: Mar 5, 2025
 *      Author: sgaer
 */

 #ifndef ORDERAPIFUNCTIONS_HPP_
 #define ORDERAPIFUNCTIONS_HPP_
 
 #pragma once
 
 
 #include <Orders/Order.hpp>
 #include <Orders/OrderPod.hpp>
 #include <Orders/IdGen.h>
 #include <Orders/OrderUtils.hpp>
 #include <Orders/OrderManagerV5.hpp>
 
 #include <Json/JsonOrderDecoders.hpp>
 
 #include <exchsupport/cme/msgw/CmeMsgw.hpp>
 using namespace KT01::MSGW;
 using namespace KTN::CME::IL3;

#include <KT01/SecDefs/CfeSecMaster.hpp>
#include <KT01/SecDefs/CmeSecMasterV2.hpp>
#include <KT01/SecDefs/EquitySecMaster.hpp>
using namespace KT01::SECDEF::CME;
using namespace KT01::SECDEF::CFE;
using namespace KT01::SECDEF::EQT;
 
 #include <Rest/RestApi.hpp>
 #include <Rest/JsonUrlReader.hpp>
 #include <Rest/JsonUrlPoster.hpp>
 
 using namespace std;


class OrderApiFunctions
{
public:

    /**
     * @brief Loads orders from the RestAPI.
     * @return A vector of orders.
     */
    
    static std::vector<KTN::Order> LoadOrdersFromApi( const string& source, const string& guid, const string& org="AKL")
    {
        string url = RestApi::ORDS_GET;
        //bool _SHOW_MSGS = false;

        int seq = 0;
        
        ostringstream oss;
        oss << url << "?org=" << org << "&seq=" << seq;
        string json = JsonUrlReader::Get(oss.str());

        std::vector<KTN::Order> result;
        std::vector<KTN::Order> ords = JsonOrderDecoders::DecodeOrdersFromRestApi(json);
        
        // Remove any item whose source does not match _SOURCE
        auto new_end = std::remove_if(
            ords.begin(), 
            ords.end(),
            [&source](const KTN::Order& item) { return item.source != source; }
        );
        ords.erase(new_end, ords.end());

        if (guid != "")
        {
            new_end = std::remove_if(
                ords.begin(), 
                ords.end(),
                [&guid](const KTN::Order& item) { return item.guid != guid; }
            );
            ords.erase(new_end, ords.end());
        }

        for(auto & ord : ords)
        {
            if (ord.OrdAlgoTrig == KOrderAlgoTrigger::UNKNOWN)
                ord.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;
                
            if (ord.exchange == "CME" || ord.exchange == "cme")
                ord.OrdExch = KOrderExchange::CME;
            else if (ord.exchange == "CFE" || ord.exchange == "cfe")
                ord.OrdExch = KOrderExchange::CFE;
            else
            {
                EquitySecDef eqtDef = EquitySecMaster::instance().getSecDef(ord.symbol);
                if (eqtDef.secid > 0)
                    ord.OrdExch = KOrderExchange::Value(eqtDef.exchangeSymbol);
                else
                    ord.OrdExch = KOrderExchange::OTHER;
            }
    
    
            if (ord.OrdExch == KOrderExchange::CME)
            {
                CmeSecDef def = CmeSecMaster::instance().getSecDef(ord.symbol);
                ord.secid = def.secid;
                ord.product = def.product;
                ord.OrdProdType = def.prodtype;
                ord.OrdPutCall = KOrderPutCall::UNKNOWN;
                ord.mktsegid = def.msgw;
            }
    
            if (ord.OrdExch == KOrderExchange::CFE)
            {
                CfeSecDef def = CfeSecMaster::instance().getSecDef(ord.symbol);
                ord.secid = def.secid;
                ord.product = def.product;
                ord.OrdProdType = def.prodtype;
                ord.OrdPutCall = KOrderPutCall::UNKNOWN;
                ord.mktsegid = 1;
    
                LOGINFO("[HandleOrders DEBUG] CFE ORDER: SYMBOL={} SECID={} PRODTYPE={} BASE64={}", ord.symbol, ord.secid,
                    KOrderProdType::toString(ord.OrdProdType), def.b63_secid);
            }

            if (KOrderExchange::isEquityVenue(ord.OrdExch))
            {
                EquitySecDef def = EquitySecMaster::instance().getSecDef(ord.symbol);
                ord.secid = def.secid;
                ord.product = def.productCode;
                ord.OrdProdType = def.prodtype;
                ord.OrdPutCall = KOrderPutCall::UNKNOWN;
            }

            result.push_back(ord);
            
            // if (_SHOW_MSGS)
            // {
            // 	cout << "[OrdZmq DEBUG] RestApi Order: " << ord.ordernum << " SOURCE=" << ord.source
            // 		<< " EXCHID=" << ord.cmeid <<" SYMBOL=" << ord.symbol
            // 		<< " SIDE=" << KOrderSide::toString(ord.OrdSide) << " LVSQTY=" << ord.leavesqty
            // 		<< " PX=" << ord.price << " TAG=" << ord.tag << endl << " TRIG=" << KOrderAlgoTrigger::toString(ord.OrdAlgoTrig) << endl;
            // }
        }

        // if (_SHOW_MSGS)
        // {
        //     cout << OrderUtils::PrintOrdersTable(result) << endl;
        //     cout << "[OrdZmq DEBUG] RestApi Orders Loaded: " << result.size() << endl;
        // }

        return result;
    }

};


#endif /* ORDERAPIFUNCTIONS_HPP_ */