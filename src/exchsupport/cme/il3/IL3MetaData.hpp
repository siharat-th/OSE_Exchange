//============================================================================
// Name        	: IL3MetaData.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jul 4, 2023 11:02:53 AM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3METADATA_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3METADATA_HPP_

#pragma once
#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <bitset>
#include <stdint.h>

namespace KTN{
namespace CME{
namespace IL3{
namespace META{

// Negotiate500   BlockLength=76 id=500 semanticType=Negotiate
struct NegotiateMeta
{
	inline static const std::string description{"Negotiate"};
	inline static const std::string semanticType{"Negotiate"};
	inline static const std::size_t id{500};
	inline static const std::size_t blockLength{76};
};

// NegotiationResponse501   BlockLength=32 id=501 semanticType=NegotiationResponse
struct NegotiationResponseMeta
{
	inline static const std::string description{"NegotiationResponse"};
	inline static const std::string semanticType{"NegotiationResponse"};
	inline static const std::size_t id{501};
	inline static const std::size_t blockLength{32};
};

// NegotiationReject502   BlockLength=68 id=502 semanticType=NegotiationReject
struct NegotiationRejectMeta
{
	inline static const std::string description{"NegotiationReject"};
	inline static const std::string semanticType{"NegotiationReject"};
	inline static const std::size_t id{502};
	inline static const std::size_t blockLength{68};
};

// Establish503   BlockLength=132 id=503 semanticType=Establish
struct EstablishMeta
{
	inline static const std::string description{"Establish"};
	inline static const std::string semanticType{"Establish"};
	inline static const std::size_t id{503};
	inline static const std::size_t blockLength{132};
};

// EstablishmentAck504   BlockLength=38 id=504 semanticType=EstablishmentAck
struct EstablishmentAckMeta
{
	inline static const std::string description{"EstablishmentAck"};
	inline static const std::string semanticType{"EstablishmentAck"};
	inline static const std::size_t id{504};
	inline static const std::size_t blockLength{38};
};

// EstablishmentReject505   BlockLength=72 id=505 semanticType=EstablishmentReject
struct EstablishmentRejectMeta
{
	inline static const std::string description{"EstablishmentReject"};
	inline static const std::string semanticType{"EstablishmentReject"};
	inline static const std::size_t id{505};
	inline static const std::size_t blockLength{72};
};

// Sequence506   BlockLength=14 id=506 semanticType=Sequence
struct SequenceMeta
{
	inline static const std::string description{"Sequence"};
	inline static const std::string semanticType{"Sequence"};
	inline static const std::size_t id{506};
	inline static const std::size_t blockLength{14};
};

// Terminate507   BlockLength=67 id=507 semanticType=Terminate
struct TerminateMeta
{
	inline static const std::string description{"Terminate"};
	inline static const std::string semanticType{"Terminate"};
	inline static const std::size_t id{507};
	inline static const std::size_t blockLength{67};
};

// RetransmitRequest508   BlockLength=30 id=508 semanticType=RetransmitRequest
struct RetransmitRequestMeta
{
	inline static const std::string description{"RetransmitRequest"};
	inline static const std::string semanticType{"RetransmitRequest"};
	inline static const std::size_t id{508};
	inline static const std::size_t blockLength{30};
};

// Retransmission509   BlockLength=31 id=509 semanticType=Retransmission
struct RetransmissionMeta
{
	inline static const std::string description{"Retransmission"};
	inline static const std::string semanticType{"Retransmission"};
	inline static const std::size_t id{509};
	inline static const std::size_t blockLength{31};
};

// RetransmitReject510   BlockLength=75 id=510 semanticType=RetransmitReject
struct RetransmitRejectMeta
{
	inline static const std::string description{"RetransmitReject"};
	inline static const std::string semanticType{"RetransmitReject"};
	inline static const std::size_t id{510};
	inline static const std::size_t blockLength{75};
};

// NotApplied513   BlockLength=17 id=513 semanticType=NotApplied
struct NotAppliedMeta
{
	inline static const std::string description{"NotApplied"};
	inline static const std::string semanticType{"NotApplied"};
	inline static const std::size_t id{513};
	inline static const std::size_t blockLength{17};
};

// NewOrderSingle514   BlockLength=116 id=514 semanticType=D
struct NewOrderSingleMeta
{
	inline static const std::string description{"NewOrderSingle"};
	inline static const std::string semanticType{"D"};
	inline static const std::size_t id{514};
	inline static const std::size_t blockLength{116};
};

// OrderCancelReplaceRequest515   BlockLength=125 id=515 semanticType=G
struct OrderCancelReplaceRequestMeta
{
	inline static const std::string description{"OrderCancelReplaceRequest"};
	inline static const std::string semanticType{"G"};
	inline static const std::size_t id{515};
	inline static const std::size_t blockLength{125};
};

// OrderCancelRequest516   BlockLength=88 id=516 semanticType=F
struct OrderCancelRequestMeta
{
	inline static const std::string description{"OrderCancelRequest"};
	inline static const std::string semanticType{"F"};
	inline static const std::size_t id{516};
	inline static const std::size_t blockLength{88};
};

// MassQuote517   BlockLength=92 id=517 semanticType=i
struct MassQuoteMeta
{
	inline static const std::string description{"MassQuote"};
	inline static const std::string semanticType{"i"};
	inline static const std::size_t id{517};
	inline static const std::size_t blockLength{92};
};

// PartyDetailsDefinitionRequest518   BlockLength=147 id=518 semanticType=CX
struct PartyDetailsDefinitionRequestMeta
{
	inline static const std::string description{"PartyDetailsDefinitionRequest"};
	inline static const std::string semanticType{"CX"};
	inline static const std::size_t id{518};
	inline static const std::size_t blockLength{147};
};

// PartyDetailsDefinitionRequestAck519   BlockLength=159 id=519 semanticType=CY
struct PartyDetailsDefinitionRequestAckMeta
{
	inline static const std::string description{"PartyDetailsDefinitionRequestAck"};
	inline static const std::string semanticType{"CY"};
	inline static const std::size_t id{519};
	inline static const std::size_t blockLength{159};
};

// BusinessReject521   BlockLength=330 id=521 semanticType=j
struct BusinessRejectMeta
{
	inline static const std::string description{"BusinessReject"};
	inline static const std::string semanticType{"j"};
	inline static const std::size_t id{521};
	inline static const std::size_t blockLength{330};
};

// ExecutionReportNew522   BlockLength=209 id=522 semanticType=8
struct ExecutionReportNewMeta
{
	inline static const std::string description{"ExecutionReportNew"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{522};
	inline static const std::size_t blockLength{209};
};

// ExecutionReportReject523   BlockLength=467 id=523 semanticType=8
struct ExecutionReportRejectMeta
{
	inline static const std::string description{"ExecutionReportReject"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{523};
	inline static const std::size_t blockLength{467};
};

// ExecutionReportElimination524   BlockLength=202 id=524 semanticType=8
struct ExecutionReportEliminationMeta
{
	inline static const std::string description{"ExecutionReportElimination"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{524};
	inline static const std::size_t blockLength{202};
};

// ExecutionReportTradeOutright525   BlockLength=235 id=525 semanticType=8
struct ExecutionReportTradeOutrightMeta
{
	inline static const std::string description{"ExecutionReportTradeOutright"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{525};
	inline static const std::size_t blockLength{235};
};

// ExecutionReportTradeSpread526   BlockLength=230 id=526 semanticType=8
struct ExecutionReportTradeSpreadMeta
{
	inline static const std::string description{"ExecutionReportTradeSpread"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{526};
	inline static const std::size_t blockLength{230};
};

// ExecutionReportTradeSpreadLeg527   BlockLength=199 id=527 semanticType=8
struct ExecutionReportTradeSpreadLegMeta
{
	inline static const std::string description{"ExecutionReportTradeSpreadLeg"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{527};
	inline static const std::size_t blockLength{199};
};

// QuoteCancel528   BlockLength=52 id=528 semanticType=Z
struct QuoteCancelMeta
{
	inline static const std::string description{"QuoteCancel"};
	inline static const std::string semanticType{"Z"};
	inline static const std::size_t id{528};
	inline static const std::size_t blockLength{52};
};

// OrderMassActionRequest529   BlockLength=71 id=529 semanticType=CA
struct OrderMassActionRequestMeta
{
	inline static const std::string description{"OrderMassActionRequest"};
	inline static const std::string semanticType{"CA"};
	inline static const std::size_t id{529};
	inline static const std::size_t blockLength{71};
};

// OrderMassStatusRequest530   BlockLength=68 id=530 semanticType=AF
struct OrderMassStatusRequestMeta
{
	inline static const std::string description{"OrderMassStatusRequest"};
	inline static const std::string semanticType{"AF"};
	inline static const std::size_t id{530};
	inline static const std::size_t blockLength{68};
};

// ExecutionReportModify531   BlockLength=217 id=531 semanticType=8
struct ExecutionReportModifyMeta
{
	inline static const std::string description{"ExecutionReportModify"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{531};
	inline static const std::size_t blockLength{217};
};

// ExecutionReportStatus532   BlockLength=480 id=532 semanticType=8
struct ExecutionReportStatusMeta
{
	inline static const std::string description{"ExecutionReportStatus"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{532};
	inline static const std::size_t blockLength{480};
};

// OrderStatusRequest533   BlockLength=62 id=533 semanticType=H
struct OrderStatusRequestMeta
{
	inline static const std::string description{"OrderStatusRequest"};
	inline static const std::string semanticType{"H"};
	inline static const std::size_t id{533};
	inline static const std::size_t blockLength{62};
};

// ExecutionReportCancel534   BlockLength=214 id=534 semanticType=8
struct ExecutionReportCancelMeta
{
	inline static const std::string description{"ExecutionReportCancel"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{534};
	inline static const std::size_t blockLength{214};
};

// OrderCancelReject535   BlockLength=409 id=535 semanticType=9
struct OrderCancelRejectMeta
{
	inline static const std::string description{"OrderCancelReject"};
	inline static const std::string semanticType{"9"};
	inline static const std::size_t id{535};
	inline static const std::size_t blockLength{409};
};

// OrderCancelReplaceReject536   BlockLength=409 id=536 semanticType=9
struct OrderCancelReplaceRejectMeta
{
	inline static const std::string description{"OrderCancelReplaceReject"};
	inline static const std::string semanticType{"9"};
	inline static const std::size_t id{536};
	inline static const std::size_t blockLength{409};
};

// PartyDetailsListRequest537   BlockLength=20 id=537 semanticType=CF
struct PartyDetailsListRequestMeta
{
	inline static const std::string description{"PartyDetailsListRequest"};
	inline static const std::string semanticType{"CF"};
	inline static const std::size_t id{537};
	inline static const std::size_t blockLength{20};
};

// PartyDetailsListReport538   BlockLength=93 id=538 semanticType=CG
struct PartyDetailsListReportMeta
{
	inline static const std::string description{"PartyDetailsListReport"};
	inline static const std::string semanticType{"CG"};
	inline static const std::size_t id{538};
	inline static const std::size_t blockLength{93};
};

// ExecutionAck539   BlockLength=101 id=539 semanticType=BN
struct ExecutionAckMeta
{
	inline static const std::string description{"ExecutionAck"};
	inline static const std::string semanticType{"BN"};
	inline static const std::size_t id{539};
	inline static const std::size_t blockLength{101};
};

// RequestForQuote543   BlockLength=55 id=543 semanticType=R
struct RequestForQuoteMeta
{
	inline static const std::string description{"RequestForQuote"};
	inline static const std::string semanticType{"R"};
	inline static const std::size_t id{543};
	inline static const std::size_t blockLength{55};
};

// NewOrderCross544   BlockLength=74 id=544 semanticType=c
struct NewOrderCrossMeta
{
	inline static const std::string description{"NewOrderCross"};
	inline static const std::string semanticType{"c"};
	inline static const std::size_t id{544};
	inline static const std::size_t blockLength{74};
};

// MassQuoteAck545   BlockLength=350 id=545 semanticType=b
struct MassQuoteAckMeta
{
	inline static const std::string description{"MassQuoteAck"};
	inline static const std::string semanticType{"b"};
	inline static const std::size_t id{545};
	inline static const std::size_t blockLength{350};
};

// RequestForQuoteAck546   BlockLength=358 id=546 semanticType=b
struct RequestForQuoteAckMeta
{
	inline static const std::string description{"RequestForQuoteAck"};
	inline static const std::string semanticType{"b"};
	inline static const std::size_t id{546};
	inline static const std::size_t blockLength{358};
};

// ExecutionReportTradeAddendumOutright548   BlockLength=181 id=548 semanticType=8
struct ExecutionReportTradeAddendumOutrightMeta
{
	inline static const std::string description{"ExecutionReportTradeAddendumOutright"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{548};
	inline static const std::size_t blockLength{181};
};

// ExecutionReportTradeAddendumSpread549   BlockLength=187 id=549 semanticType=8
struct ExecutionReportTradeAddendumSpreadMeta
{
	inline static const std::string description{"ExecutionReportTradeAddendumSpread"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{549};
	inline static const std::size_t blockLength{187};
};

// ExecutionReportTradeAddendumSpreadLeg550   BlockLength=176 id=550 semanticType=8
struct ExecutionReportTradeAddendumSpreadLegMeta
{
	inline static const std::string description{"ExecutionReportTradeAddendumSpreadLeg"};
	inline static const std::string semanticType{"8"};
	inline static const std::size_t id{550};
	inline static const std::size_t blockLength{176};
};

// SecurityDefinitionRequest560   BlockLength=71 id=560 semanticType=c
struct SecurityDefinitionRequestMeta
{
	inline static const std::string description{"SecurityDefinitionRequest"};
	inline static const std::string semanticType{"c"};
	inline static const std::size_t id{560};
	inline static const std::size_t blockLength{71};
};

// SecurityDefinitionResponse561   BlockLength=429 id=561 semanticType=d
struct SecurityDefinitionResponseMeta
{
	inline static const std::string description{"SecurityDefinitionResponse"};
	inline static const std::string semanticType{"d"};
	inline static const std::size_t id{561};
	inline static const std::size_t blockLength{429};
};

// OrderMassActionReport562   BlockLength=114 id=562 semanticType=BZ
struct OrderMassActionReportMeta
{
	inline static const std::string description{"OrderMassActionReport"};
	inline static const std::string semanticType{"BZ"};
	inline static const std::size_t id{562};
	inline static const std::size_t blockLength{114};
};

// QuoteCancelAck563   BlockLength=351 id=563 semanticType=b
struct QuoteCancelAckMeta
{
	inline static const std::string description{"QuoteCancelAck"};
	inline static const std::string semanticType{"b"};
	inline static const std::size_t id{563};
	inline static const std::size_t blockLength{351};
};

//end spaces
} } } }
#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3METADATA_HPP_ */
