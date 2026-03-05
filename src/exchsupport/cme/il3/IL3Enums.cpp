//============================================================================
// Name        	: IL3Enums.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 11, 2023 1:00:13 PM
//============================================================================

#include "IL3Enums.hpp"

//
//
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumAvgPxInd e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumBooleanFlag e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumBooleanNULL e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumClearingAcctType e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumCmtaGiveUpCD e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumCustOrdHandlInst e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumCustOrderCapacity e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumDKReason e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumExecAckStatus e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumExecMode e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumExecReason e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumExecTypTrdCxl e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumExpCycle e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumFTI e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumKeepAliveLapsed e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumListUpdAct e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumManualOrdInd e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumManualOrdIndReq e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumMassActionOrdTyp e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumMassActionResponse e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumMassActionScope e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumMassCancelTIF e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumMassCxlReqTyp e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumMassStatusOrdTyp e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumMassStatusReqTyp e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumMassStatusTIF e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumOFMOverrideReq e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumOrdStatusTrd e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumOrdStatusTrdCxl e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumOrderEventType e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumOrderStatus e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumOrderType e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumOrderTypeReq e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumPartyDetailRole e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumQuoteAckStatus e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumQuoteCxlStatus e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumQuoteCxlTyp e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumQuoteTyp e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumRFQSide e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumReqResult e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumSLEDS e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumSMPI e) {
//	os<<static_cast<char>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumSecRspTyp e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumShortSaleType e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumSideNULL e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumSideReq e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumSideTimeInForce e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumSplitMsg e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumTimeInForce e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
//std::ostream& operator <<(std::ostream& os, KTN::CME::IL3::EnumTradeAddendum e) {
//	os<<static_cast<std::uint64_t>(e);
//	return os;
//}
