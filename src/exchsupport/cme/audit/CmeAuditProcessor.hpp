#ifndef SRC_CMEAUDITPROCESSOR_HPP_
#define SRC_CMEAUDITPROCESSOR_HPP_
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

#include <exchsupport/cme/il3/IL3Types.hpp>
#include <exchsupport/cme/il3/IL3Composites.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>
#include <exchsupport/cme/il3/IL3Messages2.hpp>

#include <exchsupport/cme/il3/IL3MessagePrinter.hpp>

#include <exchsupport/cme/il3/IL3MetaMap.hpp>
#include <exchsupport/cme/il3/IL3MetaData.hpp>

using namespace KTN::CME;
using namespace KTN::CME::IL3;


class CmeAuditProcessor : public IAuditProcessor {
    public:
    std::string process(char* buf, int templateId, uint64_t customerOrderTime = 0) override
    {
        (void)customerOrderTime; // Not used for CME audit currently
        string json ="";
        
    
        switch (templateId) {
        case META::NewOrderSingleMeta::id: {
            auto msg = reinterpret_cast<NewOrderSingleMsg*>(buf);
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
        case META::OrderCancelReplaceRequestMeta::id: {
            auto msg = reinterpret_cast<OrderCancelReplaceRequestMsg*>(buf);
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
        case META::OrderCancelRequestMeta::id: {
            auto msg = reinterpret_cast<OrderCancelRequestMsg*>(buf);
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
        case META::PartyDetailsDefinitionRequestMeta::id: {
            auto msg = reinterpret_cast<PartyDetailsDefinitionRequestMsg*>(buf);
            buf += META::PartyDetailsDefinitionRequestMeta::blockLength;
    
            auto noparties = reinterpret_cast<KTN::CME::IL3::groupSize*>(buf);
            buf += sizeof(KTN::CME::IL3::groupSize);
    
            vector<GrpPartyDetail> vParts;
            for (size_t i = 0; i < noparties->numInGroup; i++) {
                auto partydet =
                        reinterpret_cast<GrpPartyDetail*>(buf);
                buf += 22; //blocklen
                //deref the pointer for the vec
                vParts.push_back(*partydet);
            }
    
            json = IL3::MessagePrinter::generateJsonString(*msg, vParts);
        }
            break;
        case META::OrderMassActionRequestMeta::id: {
                auto msg = reinterpret_cast<OrderMassActionRequestMsg*>(buf);
                buf += META::OrderMassActionRequestMeta::blockLength;
                json = IL3::MessagePrinter::generateJsonString(*msg);
            }
            break;
        case ExecutionReportNewMsg::id: {
            auto msg = reinterpret_cast<ExecutionReportNewMsg::BlockData*>(buf);
            buf += ExecutionReportNewMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportModifyMsg::id: {
            auto msg = reinterpret_cast<ExecutionReportModifyMsg::BlockData*>(buf);
            buf += ExecutionReportModifyMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportCancelMsg::id: {
            auto msg = reinterpret_cast<ExecutionReportCancelMsg::BlockData*>(buf);
            buf += ExecutionReportCancelMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportRejectMsg::id: {
            auto msg = reinterpret_cast<ExecutionReportRejectMsg::BlockData*>(buf);
            buf += ExecutionReportRejectMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportTradeOutrightMsg::id: {
            auto msg =
                    reinterpret_cast<ExecutionReportTradeOutrightMsg::BlockData*>(buf);
            buf += ExecutionReportTradeOutrightMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportTradeSpreadMsg::id: {
            auto msg =
                    reinterpret_cast<ExecutionReportTradeSpreadMsg::BlockData*>(buf);
            buf += ExecutionReportTradeSpreadMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportTradeSpreadLegMsg::id: {
            auto msg =
                    reinterpret_cast<ExecutionReportTradeSpreadLegMsg::BlockData*>(buf);
            buf += ExecutionReportTradeSpreadLegMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case PartyDetailsDefinitionRequestAckMsg::id: {
            auto msg =
                    reinterpret_cast<PartyDetailsDefinitionRequestAckMsg::BlockData*>(buf);
            buf += PartyDetailsDefinitionRequestAckMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case PartyDetailsListReportMsg::id: {
            auto msg = reinterpret_cast<PartyDetailsListReportMsg::BlockData*>(buf);
            buf += PartyDetailsListReportMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case OrderMassActionReportMsg::id: {
            auto msg = reinterpret_cast<OrderMassActionReportMsg::BlockData*>(buf);
            buf += OrderMassActionReportMsg::blockLength;
    
            auto noords = reinterpret_cast<KTN::CME::IL3::groupSize*>(buf);
            buf += sizeof(KTN::CME::IL3::groupSize);
    
            vector<GrpAffectedOrdersMassActionReport::BlockData> vOrds;
            for (size_t i = 0; i < noords->numInGroup; i++) {
                auto afford =
                        reinterpret_cast<GrpAffectedOrdersMassActionReport::BlockData*>(buf);
                buf += 32;//noords->blockLength;
                //deref the pointer for the vec
                vOrds.push_back(*afford);
            }
    
            json = IL3::MessagePrinter::generateJsonString(*msg, vOrds);
    
            return json;
        }
            break;
    
        case BusinessRejectMsg::id: {
            auto msg = reinterpret_cast<BusinessRejectMsg::BlockData*>(buf);
            buf += BusinessRejectMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case OrderCancelReplaceRejectMsg::id: {
            auto msg =
                    reinterpret_cast<OrderCancelReplaceRejectMsg::BlockData*>(buf);
            buf += OrderCancelReplaceRejectMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case OrderCancelRejectMsg::id: {
            auto msg = reinterpret_cast<OrderCancelRejectMsg::BlockData*>(buf);
            buf += OrderCancelRejectMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportTradeAddendumOutrightMsg::id: {
            auto msg =
                    reinterpret_cast<ExecutionReportTradeAddendumOutrightMsg::BlockData*>(buf);
            buf += ExecutionReportTradeAddendumOutrightMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportTradeAddendumSpreadMsg::id: {
            auto msg =
                    reinterpret_cast<ExecutionReportTradeAddendumSpreadMsg::BlockData*>(buf);
            buf += ExecutionReportTradeAddendumSpreadMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportTradeAddendumSpreadLegMsg::id: {
            auto msg =
                    reinterpret_cast<ExecutionReportTradeAddendumSpreadLegMsg::BlockData*>(buf);
            buf += ExecutionReportTradeAddendumSpreadLegMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportEliminationMsg::id: {
            auto msg =
                    reinterpret_cast<ExecutionReportEliminationMsg::BlockData*>(buf);
            buf += ExecutionReportEliminationMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case META::OrderMassStatusRequestMeta::id: {
            auto msg = reinterpret_cast<OrderMassStatusRequestMsg*>(buf);
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        case ExecutionReportStatusMsg::id: {
            auto msg =
                    reinterpret_cast<ExecutionReportStatusMsg::BlockData*>(buf);
            buf += ExecutionReportStatusMsg::blockLength;
            json = IL3::MessagePrinter::generateJsonString(*msg);
        }
            break;
    
        default: {
            string err = "AUDITV3: INVALID MSGTYPE RECEIVED-> TEMPLATE=" + std::to_string(templateId);
            KT01_LOG_INFO(__CLASS__, err);
            json = IL3::MessagePrinter::generateJsonError(err);
        }
            break;
    
        }
    
        return json;
    }
    };
    




#endif /* SRC_CMEAUDITPROCESSOR_HPP_ */