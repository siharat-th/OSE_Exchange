#ifndef SRC_CFE_AUDITPROCESSOR_HPP_
#define SRC_CFE_AUDITPROCESSOR_HPP_
#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <pthread.h>
#include <unistd.h>  // For usleep
#include <memory>    // For std::unique_ptr
#include <set>
using namespace std;

#include <ExchangeHandler/audit/IAuditProcessor.hpp>
using namespace KTN;

#include <KT01/Core/Log.hpp>
using namespace KT01::Core;

#include <exchsupport/cfe/Boe3/Boe3Includes.hpp>
#include <exchsupport/cfe/Boe3/Boe3MessagePrinter.hpp>


class CfeAuditProcessor : public IAuditProcessor {
    public:
    std::string process(char* buf, int templateId, uint64_t customerOrderTime = 0) override
    {
        std::string json = "";

        switch (static_cast<KTN::CFE::Boe3::AppMsgType>(templateId)) {
            case KTN::CFE::Boe3::AppMsgType::ModifyOrder: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::ModifyOrderMsg*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg, customerOrderTime);
            } break;

            case KTN::CFE::Boe3::AppMsgType::CancelOrder: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::CancelOrderMsg*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg, customerOrderTime);
            } break;

            case KTN::CFE::Boe3::AppMsgType::MassCancelOrder: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::MassCancelMsg*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg, customerOrderTime);
            } break;

            case KTN::CFE::Boe3::AppMsgType::PurgeOrders: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::PurgeOrdersMsg*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg, customerOrderTime);
            } break;

            case KTN::CFE::Boe3::AppMsgType::ResetRisk: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::RiskResetMsg*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg, customerOrderTime);
            } break;

            case KTN::CFE::Boe3::AppMsgType::NewOrderV2: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::NewOrderMsgV2*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg, customerOrderTime);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::OrderAck: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::OrderAckMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::OrderRejected: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::OrderRejectMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::OrderModified: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::OrderModifiedMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::ModifyRejected: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::ModifyRejectedMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::OrderExecution: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::OrderExecutionMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::OrderCancelled: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::OrderCancelledMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::CancelRejected: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::CancelRejectMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::MassCancelAck: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::MassCancelAckMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::MassCancelReject: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::MassCancelRejectMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::PurgeAck: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::PurgeAckMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::PurgeReject: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::PurgeRejectedMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::TradeCancelCorrect: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::TradeCancelCorrectMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::TASRestatement: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::TASRestatementMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            case KTN::CFE::Boe3::AppMsgType::VarianceRestatement: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::VarianceRestatementMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            // case AppMsgType::QuoteUpdateAck: {
            //     auto msg = reinterpret_cast<QuoteUpdateAckMsg*>(buf);
            //     json = Boe3::MessagePrinter::generateJsonString(*msg);
            // } break;
    
            // case AppMsgType::QuoteUpdateReject: {
            //     auto msg = reinterpret_cast<QuoteUpdateRejectMsg*>(buf);
            //     json = Boe3::MessagePrinter::generateJsonString(*msg);
            // } break;
    
            // case AppMsgType::QuoteRestated: {
            //     auto msg = reinterpret_cast<QuoteRestatedMsg*>(buf);
            //     json = Boe3::MessagePrinter::generateJsonString(*msg);
            // } break;
    
            // case AppMsgType::QuoteExecution: {
            //     auto msg = reinterpret_cast<QuoteExecutionMsg*>(buf);
            //     json = Boe3::MessagePrinter::generateJsonString(*msg);
            // } break;
    
            // case AppMsgType::QuoteCancelled: {
            //     auto msg = reinterpret_cast<QuoteCancelledMsg*>(buf);
            //     json = Boe3::MessagePrinter::generateJsonString(*msg);
            // } break;
    
            // case AppMsgType::TASQuoteRestatement: {
            //     auto msg = reinterpret_cast<TASQuoteRestatementMsg*>(buf);
            //     json = Boe3::MessagePrinter::generateJsonString(*msg);
            // } break;
    
            // case AppMsgType::VarianceQuoteRestatement: {
            //     auto msg = reinterpret_cast<VarianceQuoteRestatementMsg*>(buf);
            //     json = Boe3::MessagePrinter::generateJsonString(*msg);
            // } break;
    
            case KTN::CFE::Boe3::AppMsgType::ResetRiskAck: {
                auto msg = reinterpret_cast<KTN::CFE::Boe3::ResetRiskAckMsg::BlockData*>(buf);
                json = KTN::CFE::Boe3::MessagePrinter::generateJsonString(*msg);
            } break;
    
            default:
                json = "{}"; // or log unrecognized templateId
				cout << "[CfeAuditProcessor] Unrecognized templateId: " << templateId << endl;
                break;
        }
    
        return json;
    }
    
    };
    




#endif /* SRC_CMEAUDITPROCESSOR_HPP_ */