//============================================================================
// Name        	: IL3MessagePrinter.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 30, 2023 11:46:51 AM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3MESSAGEPRINTER_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3MESSAGEPRINTER_HPP_
#pragma once

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <bitset>
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>

#include <KT01/Core/Macro.hpp>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace rapidjson;
using namespace std;

#include <exchsupport/cme/il3/IL3Types.hpp>
#include <exchsupport/cme/il3/IL3Composites.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>
#include <exchsupport/cme/il3/IL3Messages2.hpp>

namespace KTN{
namespace CME{
namespace IL3{


/**
 * @class MessagePrinter
 * @brief Audit Trail and Textual Representation of IL3 Messages
 */

class MessagePrinter
{
public:
	static std::string generateJsonString(const NewOrderSingleMsg & msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the variables to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(msg.templateId_), allocator);
		jsonObject.AddMember("Price", Value().SetInt64(msg.Price), allocator);
		jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);

		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);

		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx), allocator);

		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);

		jsonObject.AddMember("MinQty", Value().SetUint(msg.MinQty), allocator);
		jsonObject.AddMember("DisplayQty", Value().SetUint(msg.DisplayQty), allocator);
		jsonObject.AddMember("ExpireDate", Value().SetUint(msg.ExpireDate), allocator);

		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);


		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("ExecInst", Value().SetUint(msg.ExecInst), allocator);
		//jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), allocator), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);
		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetUint(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetUint(msg.ShortSaleType), allocator);
		jsonObject.AddMember("DiscretionPrice", Value().SetInt64(msg.DiscretionPrice), allocator);
		jsonObject.AddMember("ReservationPrice", Value().SetInt64(msg.ReservationPrice), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();

	}

	static std::string generateJsonString(const OrderCancelReplaceRequestMsg& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(msg.templateId_), allocator);
		jsonObject.AddMember("Price", Value().SetInt64(msg.Price), allocator);
		jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);

        AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
        AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);

		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		//AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("MinQty", Value().SetUint(msg.MinQty), allocator);
		jsonObject.AddMember("DisplayQty", Value().SetUint(msg.DisplayQty), allocator);
		jsonObject.AddMember("ExpireDate", Value().SetUint(msg.ExpireDate), allocator);

		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("OFMOverride", Value().SetUint(msg.OFMOverride), allocator);
		jsonObject.AddMember("ExecInst", Value().SetUint(msg.ExecInst), allocator);
		//jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), allocator).Move(), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);
		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetUint(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetUint(msg.ShortSaleType), allocator);
		jsonObject.AddMember("DiscretionPrice", Value().SetInt64(msg.DiscretionPrice), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const OrderCancelRequestMsg& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(msg.templateId_), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);

        AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
        AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);

		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		//AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("OrigOrderUser", Value(msg.OrigOrderUser, allocator).Move(), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const PartyDetailsDefinitionRequestMsg& msg,
			const std::vector<GrpPartyDetail>& partyDetails)
//			const std::vector<GrpTrdRegPublication>& trdRegPublications)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(msg.templateId_), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.Memo);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("ListUpdateAction", Value().SetString(std::string(1, msg.ListUpdateAction).c_str(), 1, allocator), allocator);

		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

		AddCharArrayMember(jsonObject, allocator, "Memo", msg.Memo);
		AddCharArrayMember(jsonObject, allocator, "AvgPxGroupID", msg.AvgPxGroupID);

		jsonObject.AddMember("SelfMatchPreventionID", Value().SetUint64(msg.SelfMatchPreventionID), allocator);

		AddEnumAsString(jsonObject, allocator, "CmtaGiveupCD", msg.CmtaGiveupCD);

		jsonObject.AddMember("CustOrderCapacity", Value().SetUint(msg.CustOrderCapacity), allocator);
		jsonObject.AddMember("ClearingAccountType", Value().SetUint(msg.ClearingAccountType), allocator);

		AddEnumAsString(jsonObject, allocator, "SelfMatchPreventionInstruction", msg.SelfMatchPreventionInstruction);

		jsonObject.AddMember("AvgPxIndicator", Value().SetUint(msg.AvgPxIndicator), allocator);
		jsonObject.AddMember("ClearingTradePriceType", Value().SetUint(msg.ClearingTradePriceType), allocator);

		AddEnumAsString(jsonObject, allocator, "CustOrderHandlingInst", msg.CustOrderHandlingInst);

		jsonObject.AddMember("Executor", Value(msg.Executor), allocator);
		jsonObject.AddMember("IDMShortCode", Value(msg.IDMShortCode), allocator);

//		// Add partyDetails as an array
		Value partyDetailsArray(kArrayType);
		for (const auto& partyDetail : partyDetails)
		{
			Value partyDetailObject(kObjectType);
			partyDetailObject.AddMember("PartyDetailID", Value(partyDetail.PartyDetailID, allocator).Move(), allocator);
			partyDetailObject.AddMember("PartyDetailRole", Value().SetUint(partyDetail.PartyDetailRole), allocator);
			partyDetailsArray.PushBack(partyDetailObject, allocator);
		}
		jsonObject.AddMember("PartyDetails", partyDetailsArray, allocator);
//
//		// Add trdRegPublications as an array
//		Value trdRegPublicationsArray(kArrayType);
//		for (const auto& trdRegPublication : trdRegPublications) {
//			Value trdRegPublicationObject(kObjectType);
//			trdRegPublicationObject.AddMember("TrdRegPublicationType", Value().SetUint(trdRegPublication.TrdRegPublicationType), allocator);
//			trdRegPublicationObject.AddMember("TrdRegPublicationReason", Value().SetUint(trdRegPublication.TrdRegPublicationReason), allocator);
//			trdRegPublicationsArray.PushBack(trdRegPublicationObject, allocator);
//		}
//		jsonObject.AddMember("TrdRegPublications", trdRegPublicationsArray, allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const PartyDetailsDefinitionRequestAckMsg::BlockData& msg)
//		const std::vector<GrpPartyDetails519>& partyDetails,
//		const std::vector<GrpTrdRegPublications519>& trdRegPublications)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);
		jsonObject.AddMember("TemplateId", Value().SetUint(PartyDetailsDefinitionRequestAckMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);

		AddCharArrayMember(jsonObject, allocator, "Memo", msg.Memo);
		AddCharArrayMember(jsonObject, allocator, "AvgPxGroupID", msg.AvgPxGroupID);

		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("SelfMatchPreventionID", Value().SetUint64(msg.SelfMatchPreventionID), allocator);
		jsonObject.AddMember("PartyDetailRequestStatus", Value().SetUint(msg.PartyDetailRequestStatus), allocator);
		jsonObject.AddMember("CustOrderCapacity", Value().SetUint(msg.CustOrderCapacity), allocator);
		jsonObject.AddMember("ClearingAccountType", Value().SetUint(msg.ClearingAccountType), allocator);
		jsonObject.AddMember("SelfMatchPreventionInstruction", Value().SetUint(msg.SelfMatchPreventionInstruction), allocator);
		jsonObject.AddMember("AvgPxIndicator", Value().SetUint(msg.AvgPxIndicator), allocator);
		jsonObject.AddMember("ClearingTradePriceType", Value().SetUint(msg.ClearingTradePriceType), allocator);

		jsonObject.AddMember("CmtaGiveupCD", Value().SetString(std::string(1, msg.CmtaGiveupCD).c_str(), 1, allocator), allocator);
		jsonObject.AddMember("CustOrderHandlingInst", Value().SetString(std::string(1, msg.CustOrderHandlingInst).c_str(), 1, allocator), allocator);


		jsonObject.AddMember("ListUpdateAction", Value().SetUint(msg.ListUpdateAction), allocator);
		jsonObject.AddMember("PartyDetailDefinitionStatus", Value().SetUint(msg.PartyDetailDefinitionStatus), allocator);
		jsonObject.AddMember("Executor", Value().SetUint64(msg.Executor), allocator);
		jsonObject.AddMember("IDMShortCode", Value().SetUint64(msg.IDMShortCode), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);


		// Add partyDetails as an array
//		Value partyDetailsArray(kArrayType);
//		for (const auto& partyDetail : partyDetails) {
//			Value partyDetailObject(kObjectType);
//			const GrpPartyDetails519::BlockData& blockData = *(partyDetail.blockData_);
//			partyDetailObject.AddMember("PartyDetailID", Value(blockData.PartyDetailID, allocator).Move(), allocator);
//			partyDetailObject.AddMember("PartyDetailRole", Value().SetUint(blockData.PartyDetailRole), allocator);
//			partyDetailsArray.PushBack(partyDetailObject, allocator);
//		}
//		jsonObject.AddMember("partyDetails", partyDetailsArray, allocator);
//
//		// Add trdRegPublications as an array
//		Value trdRegPublicationsArray(kArrayType);
//		for (const auto& trdRegPublication : trdRegPublications) {
//			Value trdRegPublicationObject(kObjectType);
//			const GrpTrdRegPublications519::BlockData& blockData = *(trdRegPublication.blockData_);
//			trdRegPublicationObject.AddMember("TrdRegPublicationType", Value().SetUint(blockData.TrdRegPublicationType), allocator);
//			trdRegPublicationObject.AddMember("TrdRegPublicationReason", Value().SetUint(blockData.TrdRegPublicationReason), allocator);
//			trdRegPublicationsArray.PushBack(trdRegPublicationObject, allocator);
//		}
//		jsonObject.AddMember("trdRegPublications", trdRegPublicationsArray, allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const BusinessRejectMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		jsonObject.AddMember("TemplateId", Value().SetUint(BusinessRejectMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);

		AddCharArrayMember(jsonObject, allocator, "Text", msg.Text);

		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("BusinessRejectRefID", Value().SetUint64(msg.BusinessRejectRefID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("RefSeqNum", Value().SetUint(msg.RefSeqNum), allocator);
		jsonObject.AddMember("RefTagID", Value().SetUint(msg.RefTagID), allocator);
		jsonObject.AddMember("BusinessRejectReason", Value().SetUint(msg.BusinessRejectReason), allocator);

		AddCharArrayMember(jsonObject, allocator, "RefMsgType", msg.RefMsgType);

		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);


		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const ExecutionReportNewMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportNewMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);

		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("Price", Value().SetInt64(msg.Price.mantissa), allocator);
		jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx.mantissa), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("CrossID", Value().SetUint64(msg.CrossID), allocator);
		jsonObject.AddMember("HostCrossID", Value().SetUint64(msg.HostCrossID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
		jsonObject.AddMember("MinQty", Value().SetUint(msg.MinQty), allocator);
		jsonObject.AddMember("DisplayQty", Value().SetUint(msg.DisplayQty), allocator);
		jsonObject.AddMember("ExpireDate", Value().SetUint(msg.ExpireDate), allocator);
		jsonObject.AddMember("DelayDuration", Value().SetUint(msg.DelayDuration), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);

		jsonObject.AddMember("Side", Value().SetInt((int)msg.Side), allocator);
		jsonObject.AddMember("TimeInForce", Value().SetInt(msg.TimeInForce), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetInt(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);
		jsonObject.AddMember("CrossType", Value().SetUint(msg.CrossType), allocator);
		jsonObject.AddMember("ExecInst", Value().SetUint(msg.ExecInst), allocator);

//		jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), 1, allocator), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);

		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetUint(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetUint(msg.ShortSaleType), allocator);
		jsonObject.AddMember("DelayToTime", Value().SetUint64(msg.DelayToTime), allocator);
		jsonObject.AddMember("DiscretionPrice", Value().SetInt64(msg.DiscretionPrice.mantissa), allocator);
		jsonObject.AddMember("ReservationPrice", Value().SetInt64(msg.ReservationPrice.mantissa), allocator);
		jsonObject.AddMember("PriorityIndicator", Value().SetUint(msg.PriorityIndicator), allocator);


		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const ExecutionReportRejectMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportRejectMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Text", msg.Text);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("Price", Value().SetInt64(msg.Price.mantissa), allocator);
		jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx.mantissa), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("CrossID", Value().SetUint64(msg.CrossID), allocator);
		jsonObject.AddMember("HostCrossID", Value().SetUint64(msg.HostCrossID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
		jsonObject.AddMember("MinQty", Value().SetUint(msg.MinQty), allocator);
		jsonObject.AddMember("DisplayQty", Value().SetUint(msg.DisplayQty), allocator);
		jsonObject.AddMember("OrdRejReason", Value().SetUint(msg.OrdRejReason), allocator);
		jsonObject.AddMember("ExpireDate", Value().SetUint(msg.ExpireDate), allocator);
		jsonObject.AddMember("DelayDuration", Value().SetUint(msg.DelayDuration), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);
		jsonObject.AddMember("CrossType", Value().SetUint(msg.CrossType), allocator);
		jsonObject.AddMember("ExecInst", Value().SetUint(msg.ExecInst), allocator);
		//jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), 1, allocator), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);

		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetUint(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetUint(msg.ShortSaleType), allocator);
		jsonObject.AddMember("DelayToTime", Value().SetUint64(msg.DelayToTime), allocator);
		jsonObject.AddMember("DiscretionPrice", Value().SetInt64(msg.DiscretionPrice.mantissa), allocator);
		jsonObject.AddMember("ReservationPrice", Value().SetInt64(msg.ReservationPrice.mantissa), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const ExecutionReportEliminationMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportEliminationMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("Price", Value().SetInt64(msg.Price.mantissa), allocator);
		jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx.mantissa), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("CrossID", Value().SetUint64(msg.CrossID), allocator);
		jsonObject.AddMember("HostCrossID", Value().SetUint64(msg.HostCrossID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("CumQty", Value().SetUint(msg.CumQty), allocator);
		jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
		jsonObject.AddMember("MinQty", Value().SetUint(msg.MinQty), allocator);
		jsonObject.AddMember("DisplayQty", Value().SetUint(msg.DisplayQty), allocator);
		jsonObject.AddMember("ExpireDate", Value().SetUint(msg.ExpireDate), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("CrossType", Value().SetUint(msg.CrossType), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);
		//jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), 1, allocator), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);
		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetUint(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetUint(msg.ShortSaleType), allocator);
		jsonObject.AddMember("DiscretionPrice", Value().SetInt64(msg.DiscretionPrice.mantissa), allocator);
		jsonObject.AddMember("ReservationPrice", Value().SetInt64(msg.ReservationPrice.mantissa), allocator);
		jsonObject.AddMember("PriorityIndicator", Value().SetUint(msg.PriorityIndicator), allocator);


		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const ExecutionReportTradeOutrightMsg::BlockData& msg)
			//const std::vector<GrpOutrightFillEvent::BlockData>& fillEvents,
			//const std::vector<GrpOutrightOrderEvent::BlockData>& orderEvents)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportTradeOutrightMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx.mantissa), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("Price", Value().SetInt64(msg.Price.mantissa), allocator);
		jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx.mantissa), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("SecExecID", Value().SetUint64(msg.SecExecID), allocator);
		jsonObject.AddMember("CrossID", Value().SetUint64(msg.CrossID), allocator);
		jsonObject.AddMember("HostCrossID", Value().SetUint64(msg.HostCrossID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
		jsonObject.AddMember("LastQty", Value().SetUint(msg.LastQty), allocator);
		jsonObject.AddMember("CumQty", Value().SetUint(msg.CumQty), allocator);
		jsonObject.AddMember("MDTradeEntryID", Value().SetUint(msg.MDTradeEntryID), allocator);
		jsonObject.AddMember("SideTradeID", Value().SetUint(msg.SideTradeID), allocator);
		jsonObject.AddMember("TradeLinkID", Value().SetUint(msg.TradeLinkID), allocator);
		jsonObject.AddMember("LeavesQty", Value().SetUint(msg.LeavesQty), allocator);
		jsonObject.AddMember("TradeDate", Value().SetUint(msg.TradeDate), allocator);
		jsonObject.AddMember("ExpireDate", Value().SetUint(msg.ExpireDate), allocator);
		jsonObject.AddMember("OrdStatus", Value().SetUint(msg.OrdStatus), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("AggressorIndicator", Value().SetUint(msg.AggressorIndicator), allocator);
		jsonObject.AddMember("CrossType", Value().SetUint(msg.CrossType), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);

		//jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), 1, allocator), allocator);
		//jsonObject.AddMember("ExecutionMode", Value().SetUint(msg.ExecutionMode), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);

		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetUint(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetUint(msg.ShortSaleType), allocator);
		jsonObject.AddMember("Ownership", Value().SetUint(msg.Ownership), allocator);
		jsonObject.AddMember("DiscretionPrice", Value().SetInt64(msg.DiscretionPrice.mantissa), allocator);
		jsonObject.AddMember("TrdType", Value().SetUint(msg.TrdType), allocator);

		//jsonObject.AddMember("ExecRestatementReason", Value().SetUint(msg.ExecRestatementReason), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecRestatementReason", msg.ExecutionMode);

		jsonObject.AddMember("SettlDate", Value().SetUint(msg.SettlDate), allocator);
		jsonObject.AddMember("MaturityDate", Value().SetUint(msg.MaturityDate), allocator);
		jsonObject.AddMember("CalculatedCcyLastQty", Value().SetInt64(msg.CalculatedCcyLastQty.mantissa), allocator);
		jsonObject.AddMember("GrossTradeAmt", Value().SetInt64(msg.GrossTradeAmt.mantissa), allocator);
		jsonObject.AddMember("BenchmarkPrice", Value().SetInt64(msg.BenchmarkPrice.mantissa), allocator);
		jsonObject.AddMember("ReservationPrice", Value().SetInt64(msg.ReservationPrice.mantissa), allocator);
		jsonObject.AddMember("PriorityIndicator", Value().SetUint(msg.PriorityIndicator), allocator);
		jsonObject.AddMember("DisplayLimitPrice", Value().SetInt64(msg.DisplayLimitPrice.mantissa), allocator);


//		// Add the GrpOutrightFillEvent vector
//		Value fills(kArrayType);
//		for (const GrpOutrightFillEvent::BlockData& fillData : fillEvents) {
//			Value fillObject(kObjectType);
//			fillObject.AddMember("FillPx", Value().SetInt64(fillData.FillPx.mantissa), allocator);
//			fillObject.AddMember("FillQty", Value().SetUint(fillData.FillQty), allocator);
//			fillObject.AddMember("FillExecID", Value(fillData.FillExecID, allocator).Move(), allocator);
//			fillObject.AddMember("FillYieldType", Value().SetUint(fillData.FillYieldType), allocator);
//			fills.PushBack(fillObject, allocator);
//		}
//		jsonObject.AddMember("Fills", fills, allocator);
//
//		// Add the GrpOutrightOrderEvent vector
//		Value ords(kArrayType);
//		for (const GrpOutrightOrderEvent::BlockData& eventData : orderEvents) {
//			Value eventObject(kObjectType);
//			eventObject.AddMember("OrderEventPx", Value().SetInt64(eventData.OrderEventPx.mantissa), allocator);
//			eventObject.AddMember("OrderEventText", Value(eventData.OrderEventText, allocator).Move(), allocator);
//			eventObject.AddMember("OrderEventExecID", Value().SetUint(eventData.OrderEventExecID), allocator);
//			eventObject.AddMember("OrderEventQty", Value().SetUint(eventData.OrderEventQty), allocator);
//			eventObject.AddMember("OrderEventType", Value().SetUint(eventData.OrderEventType), allocator);
//			eventObject.AddMember("OrderEventReason", Value().SetUint(eventData.OrderEventReason), allocator);
//			eventObject.AddMember("ContraGrossTradeAmt", Value().SetInt64(eventData.ContraGrossTradeAmt.mantissa), allocator);
//			eventObject.AddMember("ContraCalculatedCcyLastQty", Value().SetInt64(eventData.ContraCalculatedCcyLastQty.mantissa), allocator);
//			ords.PushBack(eventObject, allocator);
//		}
//		jsonObject.AddMember("Orders", ords, allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const ExecutionReportTradeSpreadMsg::BlockData& msg)
//			const std::vector<GrpSpreadFillEvent::BlockData>& fillEvents,
//			const std::vector<GrpSpreadFillLegEvent::BlockData>& legEvents,
//			const std::vector<GrpSpreadFillOrderEvent::BlockData>& orderEvents)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportTradeSpreadMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx.mantissa), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("Price", Value().SetInt64(msg.Price.mantissa), allocator);
		jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx.mantissa), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("SecExecID", Value().SetUint64(msg.SecExecID), allocator);
		jsonObject.AddMember("CrossID", Value().SetUint64(msg.CrossID), allocator);
		jsonObject.AddMember("HostCrossID", Value().SetUint64(msg.HostCrossID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
		jsonObject.AddMember("LastQty", Value().SetUint(msg.LastQty), allocator);
		jsonObject.AddMember("CumQty", Value().SetUint(msg.CumQty), allocator);
		jsonObject.AddMember("MDTradeEntryID", Value().SetUint(msg.MDTradeEntryID), allocator);
		jsonObject.AddMember("SideTradeID", Value().SetUint(msg.SideTradeID), allocator);
		jsonObject.AddMember("LeavesQty", Value().SetUint(msg.LeavesQty), allocator);
		jsonObject.AddMember("TradeDate", Value().SetUint(msg.TradeDate), allocator);
		jsonObject.AddMember("ExpireDate", Value().SetUint(msg.ExpireDate), allocator);
		jsonObject.AddMember("OrdStatus", Value().SetUint(msg.OrdStatus), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("AggressorIndicator", Value().SetUint(msg.AggressorIndicator), allocator);
		jsonObject.AddMember("CrossType", Value().SetUint(msg.CrossType), allocator);
		jsonObject.AddMember("TotalNumSecurities", Value().SetUint(msg.TotalNumSecurities), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);

		//jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), 1, allocator), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);

		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetUint(msg.ShortSaleType), allocator);


//		// Add the GrpSpreadFillEvent vector
//		Value fills(kArrayType);
//		for (const GrpSpreadFillEvent::BlockData& fillData : fillEvents) {
//			Value fillObject(kObjectType);
//			fillObject.AddMember("FillPx", Value().SetInt64(fillData.FillPx.mantissa), allocator);
//			fillObject.AddMember("FillQty", Value().SetUint(fillData.FillQty), allocator);
//			fillObject.AddMember("FillExecID", Value(fillData.FillExecID, allocator).Move(), allocator);
//			fillObject.AddMember("FillYieldType", Value().SetUint(fillData.FillYieldType), allocator);
//			fills.PushBack(fillObject, allocator);
//		}
//		jsonObject.AddMember("Fills", fills, allocator);
//
//		// Add the GrpSpreadFillLegEvent vector
//		Value legs(kArrayType);
//		for (const GrpSpreadFillLegEvent::BlockData& legEventData : legEvents) {
//			Value legEventObject(kObjectType);
//			legEventObject.AddMember("LegExecID", Value().SetUint64(legEventData.LegExecID), allocator);
//			legEventObject.AddMember("LegLastPx", Value().SetInt64(legEventData.LegLastPx.mantissa), allocator);
//			legEventObject.AddMember("LegSecurityID", Value().SetInt(legEventData.LegSecurityID), allocator);
//			legEventObject.AddMember("LegTradeID", Value().SetUint(legEventData.LegTradeID), allocator);
//			legEventObject.AddMember("LegLastQty", Value().SetUint(legEventData.LegLastQty), allocator);
//			legEventObject.AddMember("LegSide", Value().SetUint(legEventData.LegSide), allocator);
//			legs.PushBack(legEventObject, allocator);
//		}
//		jsonObject.AddMember("LegFills", legs, allocator);
//
//		// Add the GrpSpreadFillOrderEvent vector
//		Value orders(kArrayType);
//		for (const GrpSpreadFillOrderEvent::BlockData& orderEventData : orderEvents) {
//			Value orderEventObject(kObjectType);
//			orderEventObject.AddMember("OrderEventPx", Value().SetInt64(orderEventData.OrderEventPx.mantissa), allocator);
//			orderEventObject.AddMember("OrderEventText", Value(orderEventData.OrderEventText, allocator).Move(), allocator);
//			orderEventObject.AddMember("OrderEventExecID", Value().SetUint(orderEventData.OrderEventExecID), allocator);
//			orderEventObject.AddMember("OrderEventQty", Value().SetUint(orderEventData.OrderEventQty), allocator);
//			orderEventObject.AddMember("OrderEventType", Value().SetUint(orderEventData.OrderEventType), allocator);
//			orderEventObject.AddMember("OrderEventReason", Value().SetUint(orderEventData.OrderEventReason), allocator);
//			orders.PushBack(orderEventObject, allocator);
//		}
//		jsonObject.AddMember("Orders", orders, allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const ExecutionReportTradeSpreadLegMsg::BlockData& msg)
//			const std::vector<GrpSpreadLegFillEvent::BlockData>& fillEvents,
//			const std::vector<GrpSpreadLegFillOrderEvent::BlockData>& orderEvents)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportTradeSpreadLegMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("Volatility", Value().SetInt64(msg.Volatility.mantissa), allocator);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx.mantissa), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("UnderlyingPx", Value().SetInt64(msg.UnderlyingPx.mantissa), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("SecExecID", Value().SetUint64(msg.SecExecID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("OptionDelta", Value().SetInt(msg.OptionDelta.mantissa), allocator);
		jsonObject.AddMember("TimeToExpiration", Value().SetInt(msg.TimeToExpiration.mantissa), allocator);
		jsonObject.AddMember("RiskFreeRate", Value().SetInt(msg.RiskFreeRate.mantissa), allocator);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("LastQty", Value().SetUint(msg.LastQty), allocator);
		jsonObject.AddMember("CumQty", Value().SetUint(msg.CumQty), allocator);
		jsonObject.AddMember("SideTradeID", Value().SetUint(msg.SideTradeID), allocator);
		jsonObject.AddMember("TradeDate", Value().SetUint(msg.TradeDate), allocator);
		jsonObject.AddMember("OrdStatus", Value().SetUint(msg.OrdStatus), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("SettlDate", Value().SetUint(msg.SettlDate), allocator);
		jsonObject.AddMember("CalculatedCcyLastQty", Value().SetInt64(msg.CalculatedCcyLastQty.mantissa), allocator);
		jsonObject.AddMember("GrossTradeAmt", Value().SetInt64(msg.GrossTradeAmt.mantissa), allocator);


//		// Add the GrpSpreadLegFillEvent vector
//		Value fills(kArrayType);
//		for (const GrpSpreadLegFillEvent::BlockData& fillData : fillEvents) {
//			Value fillObject(kObjectType);
//			fillObject.AddMember("FillPx", Value().SetInt64(fillData.FillPx.mantissa), allocator);
//			fillObject.AddMember("FillQty", Value().SetUint(fillData.FillQty), allocator);
//			fillObject.AddMember("FillExecID", Value(fillData.FillExecID, allocator).Move(), allocator);
//			fillObject.AddMember("FillYieldType", Value().SetUint(fillData.FillYieldType), allocator);
//			fills.PushBack(fillObject, allocator);
//		}
//		jsonObject.AddMember("Fills", fills, allocator);
//
//		// Add the GrpSpreadLegFillOrderEvent vector
//		Value orders(kArrayType);
//		for (const GrpSpreadLegFillOrderEvent::BlockData& orderEventData : orderEvents) {
//			Value orderEventObject(kObjectType);
//			orderEventObject.AddMember("OrderEventPx", Value().SetInt64(orderEventData.OrderEventPx.mantissa), allocator);
//			orderEventObject.AddMember("OrderEventText", Value(orderEventData.OrderEventText, allocator).Move(), allocator);
//			orderEventObject.AddMember("OrderEventExecID", Value().SetUint(orderEventData.OrderEventExecID), allocator);
//			orderEventObject.AddMember("OrderEventQty", Value().SetUint(orderEventData.OrderEventQty), allocator);
//			orderEventObject.AddMember("OrderEventType", Value().SetUint(orderEventData.OrderEventType), allocator);
//			orderEventObject.AddMember("OrderEventReason", Value().SetUint(orderEventData.OrderEventReason), allocator);
//			orders.PushBack(orderEventObject, allocator);
//		}
//		jsonObject.AddMember("Orders", orders, allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const OrderMassActionRequestMsg& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(msg.templateId_), allocator);
		jsonObject.AddMember("msgSize", Value().SetUint(msg.msgSize), allocator);
		jsonObject.AddMember("enodingType_", Value().SetUint(msg.enodingType_), allocator);
		jsonObject.AddMember("blockLength_", Value().SetUint(msg.blockLength_), allocator);
		jsonObject.AddMember("templateId_", Value().SetUint(msg.templateId_), allocator);
		jsonObject.AddMember("schemaId_", Value().SetUint(msg.schemaId_), allocator);
		jsonObject.AddMember("version_", Value().SetUint(msg.version_), allocator);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("SecurityGroup", Value(msg.SecurityGroup, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("MassActionScope", Value().SetUint(msg.MassActionScope), allocator);
		jsonObject.AddMember("MarketSegmentID", Value().SetUint(msg.MarketSegmentID), allocator);
		jsonObject.AddMember("MassCancelRequestType", Value().SetUint(msg.MassCancelRequestType), allocator);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);

		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("OrigOrderUser", Value(msg.OrigOrderUser, allocator).Move(), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}



	static std::string generateJsonString(const OrderMassActionReportMsg::BlockData& msg,
			const std::vector<GrpAffectedOrdersMassActionReport::BlockData>& affectedOrders)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		jsonObject.AddMember("TemplateId", Value().SetUint(OrderMassActionReportMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("MassActionReportID", Value().SetUint64(msg.MassActionReportID), allocator);
		jsonObject.AddMember("SecurityGroup", Value(msg.SecurityGroup, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("DelayDuration", Value().SetInt(msg.DelayDuration), allocator);
		jsonObject.AddMember("MassActionResponse", Value().SetUint(msg.MassActionResponse), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("MassActionScope", Value().SetUint(msg.MassActionScope), allocator);
		jsonObject.AddMember("TotalAffectedOrders", Value().SetUint(msg.TotalAffectedOrders), allocator);
		jsonObject.AddMember("LastFragment", Value().SetUint(msg.LastFragment), allocator);
		jsonObject.AddMember("MassActionRejectReason", Value().SetInt(msg.MassActionRejectReason), allocator);
		jsonObject.AddMember("MarketSegmentID", Value().SetInt(msg.MarketSegmentID), allocator);
		jsonObject.AddMember("MassCancelRequestType", Value().SetUint(msg.MassCancelRequestType), allocator);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);
		jsonObject.AddMember("LiquidityFlag", Value().SetInt(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("DelayToTime", Value().SetUint64(msg.DelayToTime), allocator);
		jsonObject.AddMember("OrigOrderUser", Value(msg.OrigOrderUser, allocator).Move(), allocator);
		jsonObject.AddMember("CancelText", Value(msg.CancelText, allocator).Move(), allocator);


		// Add the GrpAffectedOrdersMassActionReport vector
		Value affectedOrdersArray(kArrayType);

		if (affectedOrders.size() > 0)
		{
			for (const GrpAffectedOrdersMassActionReport::BlockData orderData : affectedOrders) {
				Value orderObject(kObjectType);
				orderObject.AddMember("OrigCIOrdID", Value(orderData.OrigCIOrdID, allocator).Move(), allocator);
				orderObject.AddMember("AffectedOrderID", Value().SetUint64(orderData.AffectedOrderID), allocator);
				orderObject.AddMember("CxlQuantity", Value().SetUint(orderData.CxlQuantity), allocator);
				affectedOrdersArray.PushBack(orderObject, allocator);
			}
			jsonObject.AddMember("Orders", affectedOrdersArray, allocator);
		}
		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const ExecutionReportModifyMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportModifyMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("Price", Value().SetInt64(msg.Price.mantissa), allocator);
		jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx.mantissa), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("CrossID", Value().SetUint64(msg.CrossID), allocator);
		jsonObject.AddMember("HostCrossID", Value().SetUint64(msg.HostCrossID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
		jsonObject.AddMember("CumQty", Value().SetUint(msg.CumQty), allocator);
		jsonObject.AddMember("LeavesQty", Value().SetUint(msg.LeavesQty), allocator);
		jsonObject.AddMember("MinQty", Value().SetUint(msg.MinQty), allocator);
		jsonObject.AddMember("DisplayQty", Value().SetUint(msg.DisplayQty), allocator);
		jsonObject.AddMember("ExpireDate", Value().SetUint(msg.ExpireDate), allocator);
		jsonObject.AddMember("DelayDuration", Value().SetUint(msg.DelayDuration), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);
		jsonObject.AddMember("CrossType", Value().SetUint(msg.CrossType), allocator);
		jsonObject.AddMember("ExecInst", Value().SetUint(msg.ExecInst), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);
		//jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), 1, allocator), allocator);
		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetUint(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetUint(msg.ShortSaleType), allocator);
		jsonObject.AddMember("DelayToTime", Value().SetUint64(msg.DelayToTime), allocator);
		jsonObject.AddMember("DiscretionPrice", Value().SetInt64(msg.DiscretionPrice.mantissa), allocator);
		jsonObject.AddMember("PriorityIndicator", Value().SetUint(msg.PriorityIndicator), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const ExecutionReportStatusMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportStatusMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Text", msg.Text);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("Price", Value().SetInt64(msg.Price.mantissa), allocator);
		jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx.mantissa), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("OrdStatusReqID", Value().SetUint64(msg.OrdStatusReqID), allocator);
		jsonObject.AddMember("MassStatusReqID", Value().SetUint64(msg.MassStatusReqID), allocator);
		jsonObject.AddMember("CrossID", Value().SetUint64(msg.CrossID), allocator);
		jsonObject.AddMember("HostCrossID", Value().SetUint64(msg.HostCrossID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
		jsonObject.AddMember("CumQty", Value().SetUint(msg.CumQty), allocator);
		jsonObject.AddMember("LeavesQty", Value().SetUint(msg.LeavesQty), allocator);
		jsonObject.AddMember("MinQty", Value().SetUint(msg.MinQty), allocator);
		jsonObject.AddMember("DisplayQty", Value().SetUint(msg.DisplayQty), allocator);
		jsonObject.AddMember("ExpireDate", Value().SetUint(msg.ExpireDate), allocator);
		jsonObject.AddMember("OrdStatus", Value().SetString(std::string(1, msg.OrdStatus).c_str(), 1, allocator), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("LastRptRequested", Value().SetUint(msg.LastRptRequested), allocator);
		jsonObject.AddMember("CrossType", Value().SetUint(msg.CrossType), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);

		//jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), 1, allocator), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);

		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetUint(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetUint(msg.ShortSaleType), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const OrderStatusRequestMsg& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(msg.templateId_), allocator);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrdStatusReqID", Value().SetUint64(msg.OrdStatusReqID), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const OrderMassStatusRequestMsg& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(msg.templateId_), allocator);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("MassStatusReqID", Value().SetUint64(msg.MassStatusReqID), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		AddCharArrayMember(jsonObject, allocator, "SecurityGroup", msg.SecurityGroup);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetUint(msg.SecurityID), allocator);
		jsonObject.AddMember("MassStatusReqType", Value().SetUint(msg.MassStatusReqType), allocator);
		jsonObject.AddMember("OrdStatusReqType", Value().SetUint(msg.OrdStatusReqType), allocator);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("MarketSegmentID", Value().SetUint(msg.MarketSegmentID), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const ExecutionReportCancelMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportCancelMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("Price", Value().SetInt64(msg.Price.mantissa), allocator);
		jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx.mantissa), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		jsonObject.AddMember("CrossID", Value().SetUint64(msg.CrossID), allocator);
		jsonObject.AddMember("HostCrossID", Value().SetUint64(msg.HostCrossID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
		jsonObject.AddMember("CumQty", Value().SetUint(msg.CumQty), allocator);
		jsonObject.AddMember("MinQty", Value().SetUint(msg.MinQty), allocator);
		jsonObject.AddMember("DisplayQty", Value().SetUint(msg.DisplayQty), allocator);
		jsonObject.AddMember("ExpireDate", Value().SetUint(msg.ExpireDate), allocator);
		jsonObject.AddMember("DelayDuration", Value().SetUint(msg.DelayDuration), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("Side", Value().SetUint(msg.Side), allocator);
		jsonObject.AddMember("TimeInForce", Value().SetUint(msg.TimeInForce), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);
		//jsonObject.AddMember("ExecRestatementReason", Value().SetUint(msg.ExecRestatementReason), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecRestatementReason", msg.ExecutionMode);
		jsonObject.AddMember("CrossType", Value().SetUint(msg.CrossType), allocator);
		jsonObject.AddMember("ExecInst", Value().SetUint(msg.ExecInst), allocator);

		//jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), 1, allocator), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);

		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetUint(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetUint(msg.ShortSaleType), allocator);
		jsonObject.AddMember("DelayToTime", Value().SetUint64(msg.DelayToTime), allocator);
		jsonObject.AddMember("DiscretionPrice", Value().SetInt64(msg.DiscretionPrice.mantissa), allocator);
		jsonObject.AddMember("ReservationPrice", Value().SetInt64(msg.ReservationPrice.mantissa), allocator);
		jsonObject.AddMember("PriorityIndicator", Value().SetUint(msg.PriorityIndicator), allocator);
		jsonObject.AddMember("OrigOrderUser", Value(msg.OrigOrderUser, allocator).Move(), allocator);
		jsonObject.AddMember("CancelText", Value(msg.CancelText, allocator).Move(), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const OrderCancelRejectMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(OrderCancelRejectMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Text", msg.Text);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("CxlRejReason", Value().SetUint(msg.CxlRejReason), allocator);
		jsonObject.AddMember("DelayDuration", Value().SetUint(msg.DelayDuration), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);
		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("DelayToTime", Value().SetUint64(msg.DelayToTime), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const OrderCancelReplaceRejectMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(OrderCancelReplaceRejectMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Text", msg.Text);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("OrderRequestID", Value().SetUint64(msg.OrderRequestID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("CxlRejReason", Value().SetUint(msg.CxlRejReason), allocator);
		jsonObject.AddMember("DelayDuration", Value().SetUint(msg.DelayDuration), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetUint(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);
		jsonObject.AddMember("LiquidityFlag", Value().SetUint(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("DelayToTime", Value().SetUint64(msg.DelayToTime), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const PartyDetailsListRequestMsg& msg,
			const std::vector<GrpRequestingPartyIDs>& requestingPartyIDs,
			const std::vector<GrpPartyIDs>& partyIDs)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(msg.templateId_), allocator);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

		// Add the groups to the JSON object
		Value requestingPartyIDsArray(kArrayType);
		for (const auto& requestingPartyID : requestingPartyIDs)
		{
			Value requestingPartyIDObject(kObjectType);
			requestingPartyIDObject.AddMember("RequestingPartyID", Value(requestingPartyID.RequestingPartyID, allocator).Move(), allocator);
			requestingPartyIDObject.AddMember("RequestingPartyIDSource", Value().SetString(&requestingPartyID.RequestingPartyIDSource, 1, allocator), allocator);
			requestingPartyIDObject.AddMember("RequestingPartyRole", Value().SetString(&requestingPartyID.RequestingPartyRole, 1, allocator), allocator);
			requestingPartyIDsArray.PushBack(requestingPartyIDObject, allocator);
		}
		jsonObject.AddMember("RequestingPartyIDs", requestingPartyIDsArray, allocator);

		Value partyIDsArray(kArrayType);
		for (const auto& partyID : partyIDs)
		{
			Value partyIDObject(kObjectType);
			partyIDObject.AddMember("PartyID", Value().SetUint64(partyID.PartyID), allocator);
			partyIDObject.AddMember("PartyIDSource", Value().SetString(&partyID.PartyIDSource, 1, allocator), allocator);
			partyIDObject.AddMember("PartyRole", Value().SetUint(partyID.PartyRole), allocator);
			partyIDsArray.PushBack(partyIDObject, allocator);
		}
		jsonObject.AddMember("PartyIDs", partyIDsArray, allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const PartyDetailsListReportMsg::BlockData& msg)
//			const std::vector<GrpPartyDetailsListReport::BlockData>& partyDetails,
//			const std::vector<GrpTrdRegPublicationsListReport::BlockData>& trdRegPublications)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(PartyDetailsListReportMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("AvgPxGroupID", Value(msg.AvgPxGroupID, allocator).Move(), allocator);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("PartyDetailsListReportID", Value().SetUint64(msg.PartyDetailsListReportID), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("SelfMatchPreventionID", Value().SetUint64(msg.SelfMatchPreventionID), allocator);
		jsonObject.AddMember("TotNumParties", Value().SetUint(msg.TotNumParties), allocator);
		jsonObject.AddMember("RequestResult", Value().SetInt(msg.RequestResult), allocator);
		jsonObject.AddMember("LastFragment", Value().SetInt(msg.LastFragment), allocator);
		jsonObject.AddMember("CustOrderCapacity", Value().SetInt(msg.CustOrderCapacity), allocator);
		jsonObject.AddMember("ClearingAccountType", Value().SetInt(msg.ClearingAccountType), allocator);
		jsonObject.AddMember("SelfMatchPreventionInstruction", Value().SetInt(msg.SelfMatchPreventionInstruction), allocator);
		jsonObject.AddMember("AvgPxIndicator", Value().SetInt(msg.AvgPxIndicator), allocator);
		jsonObject.AddMember("ClearingTradePriceType", Value().SetInt(msg.ClearingTradePriceType), allocator);
		jsonObject.AddMember("CmtaGiveupCD", Value().SetString(std::string(1, msg.CmtaGiveupCD).c_str(), 1, allocator), allocator);
		jsonObject.AddMember("CustOrderHandlingInst", Value().SetString(std::string(1, msg.CustOrderHandlingInst).c_str(), 1, allocator), allocator);
		jsonObject.AddMember("Executor", Value().SetUint64(msg.Executor), allocator);
		jsonObject.AddMember("IDMShortCode", Value().SetUint64(msg.IDMShortCode), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetInt(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);

//		// Add the groups to the JSON object
//		Value partyDetailsArray(kArrayType);
//		for (const auto& partyDetail : partyDetails)
//		{
//			Value partyDetailObject(kObjectType);
//			partyDetailObject.AddMember("PartyDetailID", Value(partyDetail.PartyDetailID, allocator).Move(), allocator);
//			partyDetailObject.AddMember("PartyDetailRole", Value().SetInt(partyDetail.PartyDetailRole), allocator);
//			partyDetailsArray.PushBack(partyDetailObject, allocator);
//		}
//		jsonObject.AddMember("PartyDetailIDs", partyDetailsArray, allocator);
//
//		Value trdRegPublicationsArray(kArrayType);
//		for (const auto& trdRegPublication : trdRegPublications)
//		{
//			Value trdRegPublicationObject(kObjectType);
//			trdRegPublicationObject.AddMember("TrdRegPublicationType", Value().SetUint(trdRegPublication.TrdRegPublicationType), allocator);
//			trdRegPublicationObject.AddMember("TrdRegPublicationReason", Value().SetUint(trdRegPublication.TrdRegPublicationReason), allocator);
//			trdRegPublicationsArray.PushBack(trdRegPublicationObject, allocator);
//		}
//		jsonObject.AddMember("TrdRegPublications", trdRegPublicationsArray, allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const ExecutionAckMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionAckMsg::id), allocator);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("ExecAckStatus", Value().SetInt(msg.ExecAckStatus), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("SecExecID", Value().SetUint64(msg.SecExecID), allocator);
		jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx.mantissa), allocator);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("LastQty", Value().SetUint(msg.LastQty), allocator);
		jsonObject.AddMember("DKReason", Value().SetString(std::string(1, msg.DKReason).c_str(), 1, allocator), allocator);
		jsonObject.AddMember("Side", Value().SetInt(msg.Side), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetInt(msg.ManualOrderIndicator), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


	static std::string generateJsonString(const ExecutionReportTradeAddendumOutrightMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportTradeAddendumOutrightMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx.mantissa), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("SecExecID", Value().SetUint64(msg.SecExecID), allocator);
		jsonObject.AddMember("OrigSecondaryExecutionID", Value().SetUint64(msg.OrigSecondaryExecutionID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("LastQty", Value().SetUint(msg.LastQty), allocator);
		jsonObject.AddMember("SideTradeID", Value().SetUint(msg.SideTradeID), allocator);
		jsonObject.AddMember("OrigSideTradeID", Value().SetUint(msg.OrigSideTradeID), allocator);
		jsonObject.AddMember("TradeDate", Value().SetUint(msg.TradeDate), allocator);
		jsonObject.AddMember("OrdStatus", Value().SetString(std::string(1, msg.OrdStatus).c_str(), 1, allocator), allocator);
		jsonObject.AddMember("ExecType", Value().SetString(std::string(1, msg.ExecType).c_str(), 1, allocator), allocator);
		jsonObject.AddMember("Side", Value().SetInt(msg.Side), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetInt(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetInt(msg.PossRetransFlag), allocator);

		AddEnumAsString(jsonObject, allocator, "ExecInst", msg.ExecInst);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);
		jsonObject.AddMember("LiquidityFlag", Value().SetInt(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetInt(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetInt(msg.ShortSaleType), allocator);

		jsonObject.AddMember("DiscretionPrice", Value().SetInt64(msg.LastPx.mantissa), allocator);
		jsonObject.AddMember("TrdType", Value().SetUint(msg.TrdType), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecRestatementReason", msg.ExecRestatement);
		jsonObject.AddMember("SettleDate", Value().SetUint(msg.SettlDate), allocator);
		jsonObject.AddMember("MaturityDate", Value().SetUint(msg.MaturityDate), allocator);
		jsonObject.AddMember("CalculatedCcyLastQty", Value().SetInt64(msg.CalculatedCcyLastQty.mantissa), allocator);
		jsonObject.AddMember("GrossTradeAmt", Value().SetInt64(msg.GrossTradeAmt.mantissa), allocator);
		jsonObject.AddMember("BenchmarkPrice", Value().SetInt64(msg.BenchmarkPrice.mantissa), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const ExecutionReportTradeAddendumSpreadMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportTradeAddendumSpreadMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx.mantissa), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("SecExecID", Value().SetUint64(msg.SecExecID), allocator);
		jsonObject.AddMember("OrigSecondaryExecutionID", Value().SetUint64(msg.OrigSecondaryExecutionID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("MDTradeEntryID", Value().SetUint(msg.MDTradeEntryID), allocator);
		jsonObject.AddMember("LastQty", Value().SetUint(msg.LastQty), allocator);
		jsonObject.AddMember("SideTradeID", Value().SetUint(msg.SideTradeID), allocator);
		jsonObject.AddMember("OrigSideTradeID", Value().SetUint(msg.OrigSideTradeID), allocator);
		jsonObject.AddMember("TradeDate", Value().SetUint(msg.TradeDate), allocator);
		jsonObject.AddMember("OrdStatus", Value().SetString(std::string(1, msg.OrdStatus).c_str(), 1, allocator), allocator);
		jsonObject.AddMember("ExecType", Value().SetString(std::string(1, msg.ExecType).c_str(), 1, allocator), allocator);
		AddEnumAsString(jsonObject, allocator, "OrdType", msg.OrdType);
		jsonObject.AddMember("Side", Value().SetInt(msg.Side), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetInt(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetInt(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("TotalNumSecurities", Value().SetUint(msg.TotalNumSecurities), allocator);
		jsonObject.AddMember("SplitMsg", Value().SetUint(msg.SplitMsg), allocator);

		//jsonObject.AddMember("ExecutionMode", Value().SetString(std::string(1, msg.ExecutionMode).c_str(), 1, allocator), allocator);
		AddEnumAsString(jsonObject, allocator, "ExecutionMode", msg.ExecutionMode);

		jsonObject.AddMember("LiquidityFlag", Value().SetInt(msg.LiquidityFlag), allocator);
		jsonObject.AddMember("ManagedOrder", Value().SetInt(msg.ManagedOrder), allocator);
		jsonObject.AddMember("ShortSaleType", Value().SetInt(msg.ShortSaleType), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonString(const ExecutionReportTradeAddendumSpreadLegMsg::BlockData& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("TemplateId", Value().SetUint(ExecutionReportTradeAddendumSpreadLegMsg::id), allocator);
		jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
		jsonObject.AddMember("UUID", Value().SetUint64(msg.UUID), allocator);
		jsonObject.AddMember("ExecID", Value(msg.ExecID, allocator).Move(), allocator);
		AddCharArrayMember(jsonObject, allocator, "SenderID", msg.SenderID);
		AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
		jsonObject.AddMember("PartyDetailsListReqID", Value().SetUint64(msg.PartyDetailsListReqID), allocator);
		jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx.mantissa), allocator);
		jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
		jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
		jsonObject.AddMember("SendingTimeEpoch", Value().SetUint64(msg.SendingTimeEpoch), allocator);
		jsonObject.AddMember("SecExecID", Value().SetUint64(msg.SecExecID), allocator);
		jsonObject.AddMember("OrigSecondaryExecutionID", Value().SetUint64(msg.OrigSecondaryExecutionID), allocator);
		AddCharArrayMember(jsonObject, allocator, "Location", msg.Location);
		jsonObject.AddMember("SecurityID", Value().SetInt(msg.SecurityID), allocator);
		jsonObject.AddMember("LastQty", Value().SetUint(msg.LastQty), allocator);
		jsonObject.AddMember("SideTradeID", Value().SetUint(msg.SideTradeID), allocator);
		jsonObject.AddMember("OrigSideTradeID", Value().SetUint(msg.OrigSideTradeID), allocator);
		jsonObject.AddMember("TradeDate", Value().SetUint(msg.TradeDate), allocator);

		jsonObject.AddMember("OrdStatus", Value().SetString(std::string(1, msg.OrdStatus).c_str(), 1, allocator), allocator);

		jsonObject.AddMember("ExecType", Value().SetString(std::string(1, msg.ExecType).c_str(), 1, allocator), allocator);
		jsonObject.AddMember("ManualOrderIndicator", Value().SetInt(msg.ManualOrderIndicator), allocator);
		jsonObject.AddMember("PossRetransFlag", Value().SetInt(msg.PossRetransFlag), allocator);
		jsonObject.AddMember("Side", Value().SetInt(msg.Side), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}

	static std::string generateJsonError(const string& msg)
	{
		Document document;
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create the JSON object
		Value jsonObject(kObjectType);

		// Add the fields to the JSON object
		jsonObject.AddMember("OrdStatus", Value().SetString(msg.c_str(), msg.length(), allocator), allocator);

		// Convert the JSON object to a string
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonObject.Accept(writer);

		return buffer.GetString();
	}


private:
    // Static helper function to handle char arrays without null terminators
	template <size_t N>
	static void AddCharArrayMember(Value& jsonObject, Document::AllocatorType& allocator, const char* key, const char (&charArray)[N]) {
	    std::string trimmedString(charArray);  // Convert to std::string, auto-trims at first '\0'
	    jsonObject.AddMember(Value().SetString(key, allocator), Value().SetString(trimmedString.c_str(), allocator), allocator);
	}

    // Helper for serializing enums as characters (not ASCII codes)
	static void AddEnumAsString(Value& jsonObject, Document::AllocatorType& allocator, const char* key, char enumValue) {
		jsonObject.AddMember(Value().SetString(key, allocator), Value().SetString(std::string(1, enumValue).c_str(), allocator), allocator);
	}

};

} } }
#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3MESSAGEPRINTER_HPP_ */
