//============================================================================
// Name        	: IL3Enums.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 31, 2023 7:45:30 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3ENUMS_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3ENUMS_HPP_

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <ostream>

namespace KTN{
namespace CME{
namespace IL3{
enum EnumAvgPxInd : uint8_t {
    EnumAvgPxInd_NoAveragePricing = 0,
    EnumAvgPxInd_TradeIsPartOfAveragePriceGroup = 1,
    EnumAvgPxInd_TradeIsPartOfNotionalValueAveragePriceGroup = 3
};

enum EnumBooleanFlag : uint8_t {
    EnumBooleanFlag_False = 0,
    EnumBooleanFlag_True = 1
};

enum EnumBooleanNULL : uint8_t {
    EnumBooleanNULL_False = 0,
    EnumBooleanNULL_True = 1
};

enum EnumClearingAcctType : uint8_t {
    EnumClearingAcctType_Customer = 0,
    EnumClearingAcctType_Firm = 1
};

enum EnumCmtaGiveUpCD : char {
    EnumCmtaGiveUpCD_GiveUp = 'G',
    EnumCmtaGiveUpCD_SGXoffset = 'S'
};

enum EnumCustOrdHandlInst : char {
    EnumCustOrdHandlInst_FCMprovidedscreen = 'C',
    EnumCustOrdHandlInst_Otherprovidedscreen = 'D',
    EnumCustOrdHandlInst_FCMAPIorFIX = 'G',
    EnumCustOrdHandlInst_AlgoEngine = 'H',
    EnumCustOrdHandlInst_DeskElectronic = 'W',
    EnumCustOrdHandlInst_ClientElectronic = 'Y'
};

enum EnumCustOrderCapacity : uint8_t {
    EnumCustOrderCapacity_MemberTradingForOwnAccount = 1,
    EnumCustOrderCapacity_MemberFirmTradingForProprietaryAccount = 2,
    EnumCustOrderCapacity_MemberTradingForAnotherMember = 3,
    EnumCustOrderCapacity_AllOther = 4
};

enum EnumDKReason : char {
    EnumDKReason_UnknownSecurity = 'A',
    EnumDKReason_WrongSide = 'B',
    EnumDKReason_QuantityExceedsOrder = 'C',
    EnumDKReason_NoMatchingOrder = 'D',
    EnumDKReason_PriceExceedsLimit = 'E',
    EnumDKReason_CalculationDifference = 'F',
    EnumDKReason_NoMatchingExecutionReport = 'G',
    EnumDKReason_Other = 'Z'
};

enum EnumExecAckStatus : uint8_t {
    EnumExecAckStatus_Accepted = 1,
    EnumExecAckStatus_Rejected = 2
};

enum EnumExecMode : char {
    EnumExecMode_Aggressive = 'A',
    EnumExecMode_Passive = 'P'
};

enum EnumExecRestateReason : uint8_t {
    EnumExecRestateReason_MarketExchangeOption = 8,
    EnumExecRestateReason_CancelledNotBest = 9,
	EnumExecRestateReason_CancelOnDisconnect = 100,
	EnumExecRestateReason_SelfMatchPreventionOldestOrderCancelled = 103,
	EnumExecRestateReason_CancelOnGlobexCreditControlsViolation = 104,
	EnumExecRestateReason_CancelFromFirmsoft = 105,
	EnumExecRestateReason_CancelFromRiskManagementAPI = 106,
	EnumExecRestateReason_SelfMatchPreventionNewestOrderCancelled = 107,
	EnumExecRestateReason_CancelDueToVolQuotedOptionOrderRestedQtyLessThanMinOrderSize = 108
};

enum EnumExecTypTrdCxl : char {
    EnumExecTypTrdCxl_TradeCorrection = 'G',
    EnumExecTypTrdCxl_TradeCancel = 'H'
};

enum EnumExpCycle : uint8_t {
    EnumExpCycle_ExpireOnTradingSessionClose = 0,
    EnumExpCycle_ExpirationAtGivenDate = 2
};

enum EnumFTI : uint8_t {
    EnumFTI_Backup = 0,
    EnumFTI_Primary = 1
};

enum EnumKeepAliveLapsed : uint8_t {
    EnumKeepAliveLapsed_NotLapsed = 0,
    EnumKeepAliveLapsed_Lapsed = 1
};

enum EnumListUpdAct : char {
    EnumListUpdAct_Add = 'A',
    EnumListUpdAct_Delete = 'D'
};

enum EnumManualOrdInd : uint8_t {
    EnumManualOrdInd_Automated = 0,
    EnumManualOrdInd_Manual = 1
};

enum EnumManualOrdIndReq : uint8_t {
    EnumManualOrdIndReq_Automated = 0,
    EnumManualOrdIndReq_Manual = 1
};

enum EnumMassActionOrdTyp : char {
    EnumMassActionOrdTyp_Limit = '2',
    EnumMassActionOrdTyp_StopLimit = '4'
};

enum EnumMassActionResponse : uint8_t {
    EnumMassActionResponse_Rejected = 0,
    EnumMassActionResponse_Accepted = 1
};

enum EnumMassActionScope : uint8_t {
    EnumMassActionScope_Instrument = 1,
    EnumMassActionScope_All = 7,
    EnumMassActionScope_MarketSegmentID = 9,
    EnumMassActionScope_InstrumentGroup = 10,
    EnumMassActionScope_QuoteSetID = 100
};

enum EnumMassCancelTIF : uint8_t {
    EnumMassCancelTIF_Day = 0,
    EnumMassCancelTIF_GoodTillCancel = 1,
    EnumMassCancelTIF_GoodTillDate = 6
};

enum EnumMassCxlReqTyp : uint8_t {
    EnumMassCxlReqTyp_SenderSubID = 100,
    EnumMassCxlReqTyp_Account = 101
};

enum EnumMassStatusOrdTyp : uint8_t {
    EnumMassStatusOrdTyp_SenderSubID = 100,
    EnumMassStatusOrdTyp_Account = 101
};

enum EnumMassStatusReqTyp : uint8_t {
    EnumMassStatusReqTyp_Instrument = 1,
    EnumMassStatusReqTyp_InstrumentGroup = 3,
    EnumMassStatusReqTyp_MarketSegment = 100
};

enum EnumMassStatusTIF : uint8_t {
    EnumMassStatusTIF_Day = 0,
    EnumMassStatusTIF_GTC = 1,
    EnumMassStatusTIF_GTD = 6
};

enum EnumOFMOverrideReq : uint8_t {
    EnumOFMOverrideReq_Disabled = 0,
    EnumOFMOverrideReq_Enabled = 1
};

enum EnumOrdStatusTrd : uint8_t {
    EnumOrdStatusTrd_PartiallyFilled = 1,
    EnumOrdStatusTrd_Filled = 2
};

enum EnumOrdStatusTrdCxl : char {
    EnumOrdStatusTrdCxl_TradeCorrection = 'G',
    EnumOrdStatusTrdCxl_TradeCancel = 'H'
};

enum EnumOrderEventType : uint8_t {
    EnumOrderEventType_PartiallyFilled = 4,
    EnumOrderEventType_Filled = 5
};

enum EnumOrderStatus : char {
    EnumOrderStatus_New = '0',
    EnumOrderStatus_PartiallyFilled = '1',
    EnumOrderStatus_Filled = '2',
    EnumOrderStatus_Cancelled = '4',
    EnumOrderStatus_Replaced = '5',
    EnumOrderStatus_Rejected = '8',
    EnumOrderStatus_Expired = 'C',
    EnumOrderStatus_Undefined = 'U'
};

enum EnumOrderType : char {
    EnumOrderType_MarketWithProtection = '1',
    EnumOrderType_Limit = '2',
    EnumOrderType_StopLimit = '4',
    EnumOrderType_MarketWithLeftoverAsLimit = 'K'
};

enum EnumOrderTypeReq : char {
    EnumOrderTypeReq_MarketWithProtection = '1',
    EnumOrderTypeReq_Limit = '2',
    EnumOrderTypeReq_StopWithProtection = '3',
    EnumOrderTypeReq_StopLimit = '4',
    EnumOrderTypeReq_MarketWithLeftoverAsLimit = 'K'
};

enum EnumPartyDetailRole : uint16_t {
    EnumPartyDetailRole_ExecutingFirm = 1,
    EnumPartyDetailRole_CustomerAccount = 24,
    EnumPartyDetailRole_TakeUpFirm = 96,
    EnumPartyDetailRole_Operator = 118,
    EnumPartyDetailRole_TakeUpAccount = 1000
};

enum EnumQuoteAckStatus : uint8_t {
    EnumQuoteAckStatus_Accepted = 0,
    EnumQuoteAckStatus_Rejected = 5
};

enum EnumQuoteCxlStatus : uint8_t {
    EnumQuoteCxlStatus_CancelPerInstrument = 1,
    EnumQuoteCxlStatus_CancelPerInstrumentGroup = 3,
    EnumQuoteCxlStatus_CancelAllQuotes = 4,
    EnumQuoteCxlStatus_Rejected = 5,
    EnumQuoteCxlStatus_CancelPerQuoteSet = 100
};

enum EnumQuoteCxlTyp : uint8_t {
    EnumQuoteCxlTyp_CancelPerInstrument = 1,
    EnumQuoteCxlTyp_CancelPerInstrumentGroup = 3,
    EnumQuoteCxlTyp_CancelAllQuotes = 4,
    EnumQuoteCxlTyp_CancelPerQuoteSet = 100
};

enum EnumQuoteTyp : uint8_t {
    EnumQuoteTyp_Tradeable = 1
};

enum EnumRFQSide : uint8_t {
    EnumRFQSide_Buy = 1,
    EnumRFQSide_Sell = 2,
    EnumRFQSide_Cross = 8
};

enum EnumReqResult : uint8_t {
    EnumReqResult_ValidRequest = 0,
    EnumReqResult_NoDataFoundThatMatchedSelectionCriteria = 2,
    EnumReqResult_NotAuthorizedToRetrieveData = 3,
    EnumReqResult_DataTemporarilyUnavailable = 4
};

enum EnumSLEDS : uint8_t {
    EnumSLEDS_TradeClearingAtExecutionPrice = 0,
    EnumSLEDS_TradeClearingAtAlternateClearingPrice = 1
};

enum EnumSMPI : char{
    EnumSMPI_CancelNewest = 'N',
    EnumSMPI_CancelOldest = 'O'
};

enum EnumSecRspTyp : uint8_t {
    EnumSecRspTyp_AcceptSecurityProposalAsIs = 1,
    EnumSecRspTyp_AcceptSecurityProposalWithRevisions = 2,
    EnumSecRspTyp_RejectSecurityProposal = 5
};

enum EnumShortSaleType : uint8_t {
    EnumShortSaleType_LongSell = 0,
    EnumShortSaleType_ShortSaleWithNoExemptionSESH = 1,
    EnumShortSaleType_ShortSaleWithExemptionSSEX = 2,
    EnumShortSaleType_UndisclosedSellInformationNotAvailableUNDI = 3
};

enum EnumSideNULL : uint8_t {
    EnumSideNULL_Buy = 1,
    EnumSideNULL_Sell = 2
};

enum EnumSideReq : uint8_t {
    EnumSideReq_Buy = 1,
    EnumSideReq_Sell = 2
};

enum EnumSideTimeInForce : uint8_t {
    EnumSideTimeInForce_Day = 0,
    EnumSideTimeInForce_FAK = 3
};

enum EnumSplitMsg : uint8_t {
    EnumSplitMsg_SplitMessageDelayed = 0,
    EnumSplitMsg_OutOfOrderMessageDelayed = 1,
    EnumSplitMsg_CompleteMessageDelayed = 2
};

enum EnumTimeInForce : uint8_t {
    EnumTimeInForce_Day = 0,
    EnumTimeInForce_GoodTillCancel = 1,
    EnumTimeInForce_FillAndKill = 3,
    EnumTimeInForce_FillOrKill = 4,
    EnumTimeInForce_GoodTillDate = 6
};

enum EnumTradeAddendum : uint8_t {
    EnumTradeAddendum_PartiallyFilled = 4,
    EnumTradeAddendum_Filled = 5,
    EnumTradeAddendum_TradeCancel = 100,
    EnumTradeAddendum_TradeCorrection = 101
};

enum EnumExecInst : uint8_t {
    ExecInst_AON = 0,         // All Or None
    ExecInst_OB = 1,          // Only Best
    ExecInst_NH = 2,          // Not Held
    ExecInst_Reserved1 = 3,
    ExecInst_Reserved2 = 4,
    ExecInst_Reserved3 = 5,
    ExecInst_Reserved4 = 6,
    ExecInst_Reserved5 = 7
};


}
}
}
//
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::AvgPxInd e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::BooleanFlag e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::BooleanNULL e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ClearingAcctType e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::CmtaGiveUpCD e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::CustOrdHandlInst e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::CustOrderCapacity e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::DKReason e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ExecAckStatus e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ExecMode e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ExecReason e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ExecTypTrdCxl e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ExpCycle e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::FTI e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::KeepAliveLapsed e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ListUpdAct e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ManualOrdInd e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ManualOrdIndReq e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::MassActionOrdTyp e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::MassActionResponse e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::MassActionScope e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::MassCancelTIF e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::MassCxlReqTyp e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::MassStatusOrdTyp e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::MassStatusReqTyp e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::MassStatusTIF e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::OFMOverrideReq e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::OrdStatusTrd e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::OrdStatusTrdCxl e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::OrderEventType e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::OrderStatus e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::OrderType e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::OrderTypeReq e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::PartyDetailRole e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::QuoteAckStatus e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::QuoteCxlStatus e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::QuoteCxlTyp e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::QuoteTyp e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::RFQSide e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ReqResult e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::SLEDS e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::SMPI e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::SecRspTyp e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::ShortSaleType e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::SideNULL e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::SideReq e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::SideTimeInForce e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::SplitMsg e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::TimeInForce e);
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::TradeAddendum e);




#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3ENUMS_HPP_ */
