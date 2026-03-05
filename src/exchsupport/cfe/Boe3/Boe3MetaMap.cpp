#include  <exchsupport/cfe/Boe3/Boe3MetaMap.hpp>

using namespace KTN::CFE::Boe3::Meta;

BOE3MetaMap::BOE3MetaMap() {
}

void BOE3MetaMap::init() {
    // CFE message metadata entries with blockLength = 0

    // Login and Heartbeat messages
    metadataMap[1]   = { "LoginRequest", "LoginRequest", 1, 0 };
    metadataMap[2]   = { "LogoutRequest", "LogoutRequest", 2, 0 };
    metadataMap[3]   = { "ClientHeartbeat", "ClientHeartbeat", 3, 0 };
    metadataMap[501] = { "LoginResponse", "LoginResponse", 501, 0 };
    metadataMap[502] = { "ReplayComplete", "ReplayComplete", 502, 0 };
    metadataMap[503] = { "LogoutResponse", "LogoutResponse", 503, 0 };
    metadataMap[504] = { "ServerHeartbeat", "ServerHeartbeat", 504, 0 };

    // Order and Quote related messages
    //metadataMap[1001] = { "NewOrder", "NewOrder", 1001, 143 };
    metadataMap[1002] = { "ModifyOrder", "ModifyOrder", 1002, 104 };
    metadataMap[1003] = { "CancelOrder", "CancelOrder", 1003, 55 };
    metadataMap[1004] = { "MassCancelOrder", "MassCancelOrder", 1004, 77 };
    metadataMap[1005] = { "PurgeOrders", "PurgeOrders", 1005, 80 };
    metadataMap[1006] = { "QuoteUpdate", "QuoteUpdate", 1006, 81 };
    metadataMap[1007] = { "ResetRisk", "ResetRisk", 1007, 48 };
    metadataMap[1008] = { "NewOrderV2", "NewOrderV2", 1008, 143 };

    // Execution and Acknowledgment messages
    metadataMap[1501] = { "OrderAck", "OrderAck", 1501, 160 };
    metadataMap[1502] = { "OrderRejected", "OrderRejected", 1502, 0 };
    metadataMap[1503] = { "OrderModified", "OrderModified", 1503, 0 };
    metadataMap[1504] = { "ModifyRejected", "ModifyRejected", 1504, 0 };
    metadataMap[1505] = { "OrderExecution", "OrderExecution", 1505, 0 };
    metadataMap[1506] = { "OrderCancelled", "OrderCancelled", 1506, 0 };
    metadataMap[1507] = { "CancelRejected", "CancelRejected", 1507, 0 };
    metadataMap[1508] = { "MassCancelAck", "MassCancelAck", 1508, 0 };
    metadataMap[1509] = { "MassCancelReject", "MassCancelReject", 1509, 0 };
    metadataMap[1510] = { "PurgeAck", "PurgeAck", 1510, 0 };
    metadataMap[1511] = { "PurgeReject", "PurgeReject", 1511, 0 };

    // Additional execution and restatement messages
    metadataMap[1512] = { "TradeCancelCorrect", "TradeCancelCorrect", 1512, 0 };
    metadataMap[1513] = { "TASRestatement", "TASRestatement", 1513, 0 };
    metadataMap[1514] = { "VarianceRestatement", "VarianceRestatement", 1514, 0 };
    metadataMap[1515] = { "QuoteUpdateAck", "QuoteUpdateAck", 1515, 0 };
    metadataMap[1516] = { "QuoteUpdateReject", "QuoteUpdateReject", 1516, 0 };
    metadataMap[1517] = { "QuoteRestated", "QuoteRestated", 1517, 0 };
    metadataMap[1518] = { "QuoteExecution", "QuoteExecution", 1518, 0 };
    metadataMap[1519] = { "QuoteCancelled", "QuoteCancelled", 1519, 0 };
    metadataMap[1520] = { "TASQuoteRestatement", "TASQuoteRestatement", 1520, 0 };
    metadataMap[1521] = { "VarianceQuoteRestatement", "VarianceQuoteRestatement", 1521, 0 };
    metadataMap[1522] = { "ResetRiskAck", "ResetRiskAck", 1522, 0 };
}


