//============================================================================
// Name        	: IL3MetaMap.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jul 4, 2023 11:27:49 AM
//============================================================================

#include <exchsupport/cme/il3/IL3MetaMap.hpp>

namespace KTN {
namespace CME {
namespace IL3 {
namespace META {

IL3MetaMap::IL3MetaMap()
{
}

// Function to populate the metadata map
void IL3MetaMap::init() {
    // Negotiate500   BlockLength=76 id=500 semanticType=Negotiate
    metadataMap[500] = { "Negotiate", "Negotiate", 500, 76 };

    // NegotiationResponse501   BlockLength=32 id=501 semanticType=NegotiationResponse
    metadataMap[501] = { "NegotiationResponse", "NegotiationResponse", 501, 32 };

    // NegotiationReject502   BlockLength=68 id=502 semanticType=NegotiationReject
    metadataMap[502] = { "NegotiationReject", "NegotiationReject", 502, 68 };

    // Establish503   BlockLength=132 id=503 semanticType=Establish
    metadataMap[503] = { "Establish", "Establish", 503, 132 };

    // EstablishmentAck504   BlockLength=38 id=504 semanticType=EstablishmentAck
    metadataMap[504] = { "EstablishmentAck", "EstablishmentAck", 504, 38 };

    // EstablishmentReject505   BlockLength=72 id=505 semanticType=EstablishmentReject
    metadataMap[505] = { "EstablishmentReject", "EstablishmentReject", 505, 72 };

    // Sequence506   BlockLength=14 id=506 semanticType=Sequence
    metadataMap[506] = { "Sequence", "Sequence", 506, 14 };

    // Terminate507   BlockLength=67 id=507 semanticType=Terminate
    metadataMap[507] = { "Terminate", "Terminate", 507, 67 };

    // RetransmitRequest508   BlockLength=30 id=508 semanticType=RetransmitRequest
    metadataMap[508] = { "RetransmitRequest", "RetransmitRequest", 508, 30 };

    // Retransmission509   BlockLength=31 id=509 semanticType=Retransmission
    metadataMap[509] = { "Retransmission", "Retransmission", 509, 31 };

    // RetransmitReject510   BlockLength=75 id=510 semanticType=RetransmitReject
    metadataMap[510] = { "RetransmitReject", "RetransmitReject", 510, 75 };

    // NotApplied513   BlockLength=17 id=513 semanticType=NotApplied
    metadataMap[513] = { "NotApplied", "NotApplied", 513, 17 };

    // NewOrderSingle514   BlockLength=116 id=514 semanticType=D
    metadataMap[514] = { "NewOrderSingle", "D", 514, 116 };

    // OrderCancelReplaceRequest515   BlockLength=125 id=515 semanticType=G
    metadataMap[515] = { "OrderCancelReplaceRequest", "G", 515, 125 };

    // OrderCancelRequest516   BlockLength=88 id=516 semanticType=F
    metadataMap[516] = { "OrderCancelRequest", "F", 516, 88 };

    // MassQuote517   BlockLength=92 id=517 semanticType=i
    metadataMap[517] = { "MassQuote", "i", 517, 92 };

    // PartyDetailsDefinitionRequest518   BlockLength=147 id=518 semanticType=CX
    metadataMap[518] = { "PartyDetailsDefinitionRequest", "CX", 518, 147 };

    // PartyDetailsDefinitionRequestAck519   BlockLength=159 id=519 semanticType=CY
    metadataMap[519] = { "PartyDetailsDefinitionRequestAck", "CY", 519, 159 };

    // BusinessReject521   BlockLength=330 id=521 semanticType=j
    metadataMap[521] = { "BusinessReject", "j", 521, 330 };

    // ExecutionReportNew522   BlockLength=209 id=522 semanticType=8
    metadataMap[522] = { "ExecutionReportNew", "8", 522, 209 };

    // ExecutionReportReject523   BlockLength=467 id=523 semanticType=8
    metadataMap[523] = { "ExecutionReportReject", "8", 523, 467 };

    // ExecutionReportElimination524   BlockLength=202 id=524 semanticType=8
    metadataMap[524] = { "ExecutionReportElimination", "8", 524, 202 };

    // ExecutionReportTradeOutright525   BlockLength=235 id=525 semanticType=8
    metadataMap[525] = { "ExecutionReportTradeOutright", "8", 525, 235 };

    // ExecutionReportTradeSpread526   BlockLength=230 id=526 semanticType=8
    metadataMap[526] = { "ExecutionReportTradeSpread", "8", 526, 230 };

    // ExecutionReportTradeSpreadLeg527   BlockLength=199 id=527 semanticType=8
    metadataMap[527] = { "ExecutionReportTradeSpreadLeg", "8", 527, 199 };

    // QuoteCancel528   BlockLength=52 id=528 semanticType=Z
    metadataMap[528] = { "QuoteCancel", "Z", 528, 52 };

    // OrderMassActionRequest529   BlockLength=71 id=529 semanticType=CA
    metadataMap[529] = { "OrderMassActionRequest", "CA", 529, 71 };

    // OrderMassStatusRequest530   BlockLength=68 id=530 semanticType=AF
    metadataMap[530] = { "OrderMassStatusRequest", "AF", 530, 68 };

    // ExecutionReportModify531   BlockLength=217 id=531 semanticType=8
    metadataMap[531] = { "ExecutionReportModify", "8", 531, 217 };

    // ExecutionReportStatus532   BlockLength=480 id=532 semanticType=8
    metadataMap[532] = { "ExecutionReportStatus", "8", 532, 480 };

    // OrderStatusRequest533   BlockLength=62 id=533 semanticType=H
    metadataMap[533] = { "OrderStatusRequest", "H", 533, 62 };

    // ExecutionReportCancel534   BlockLength=214 id=534 semanticType=8
    metadataMap[534] = { "ExecutionReportCancel", "8", 534, 214 };

    // OrderCancelReject535   BlockLength=409 id=535 semanticType=9
    metadataMap[535] = { "OrderCancelReject", "9", 535, 409 };

    // OrderCancelReplaceReject536   BlockLength=409 id=536 semanticType=9
    metadataMap[536] = { "OrderCancelReplaceReject", "9", 536, 409 };

    // PartyDetailsListRequest537   BlockLength=20 id=537 semanticType=CF
    metadataMap[537] = { "PartyDetailsListRequest", "CF", 537, 20 };

    // PartyDetailsListReport538   BlockLength=93 id=538 semanticType=CG
    metadataMap[538] = { "PartyDetailsListReport", "CG", 538, 93 };

    // ExecutionAck539   BlockLength=101 id=539 semanticType=BN
    metadataMap[539] = { "ExecutionAck", "BN", 539, 101 };

    // RequestForQuote543   BlockLength=55 id=543 semanticType=R
    metadataMap[543] = { "RequestForQuote", "R", 543, 55 };

    // NewOrderCross544   BlockLength=74 id=544 semanticType=c
    metadataMap[544] = { "NewOrderCross", "c", 544, 74 };

    // MassQuoteAck545   BlockLength=350 id=545 semanticType=b
    metadataMap[545] = { "MassQuoteAck", "b", 545, 350 };

    // RequestForQuoteAck546   BlockLength=358 id=546 semanticType=b
    metadataMap[546] = { "RequestForQuoteAck", "b", 546, 358 };

    // ExecutionReportTradeAddendumOutright548   BlockLength=181 id=548 semanticType=8
    metadataMap[548] = { "ExecutionReportTradeAddendumOutright", "8", 548, 181 };

    // ExecutionReportTradeAddendumSpread549   BlockLength=187 id=549 semanticType=8
    metadataMap[549] = { "ExecutionReportTradeAddendumSpread", "8", 549, 187 };

    // ExecutionReportTradeAddendumSpreadLeg550   BlockLength=176 id=550 semanticType=8
    metadataMap[550] = { "ExecutionReportTradeAddendumSpreadLeg", "8", 550, 176 };

    // SecurityDefinitionRequest560   BlockLength=71 id=560 semanticType=c
    metadataMap[560] = { "SecurityDefinitionRequest", "c", 560, 71 };

    // SecurityDefinitionResponse561   BlockLength=429 id=561 semanticType=d
    metadataMap[561] = { "SecurityDefinitionResponse", "d", 561, 429 };

    // OrderMassActionReport562   BlockLength=114 id=562 semanticType=BZ
    metadataMap[562] = { "OrderMassActionReport", "BZ", 562, 114 };

    // QuoteCancelAck563   BlockLength=351 id=563 semanticType=b
    metadataMap[563] = { "QuoteCancelAck", "b", 563, 351 };
}

} // namespace META
} // namespace IL3
} // namespace CME
} // namespace KTN
