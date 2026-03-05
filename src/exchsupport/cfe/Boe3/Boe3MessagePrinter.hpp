

#ifndef SRC_BOE3_BOE3MESSAGEPRINTER_HPP_
#define SRC_BOE3_BOE3MESSAGEPRINTER_HPP_

#pragma once

#include <sstream>
#include <string>
#include <cstring>
#include <exchsupport/cfe/Boe3/Boe3Includes.hpp>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace rapidjson;
using namespace std;

namespace KTN
{
    namespace CFE
    {
        namespace Boe3
        {

            class MessagePrinter
            {
            public:
                // Helper function to ensure a char array is null terminated.
                inline static std::string getSz(char *fld, int datalen)
                {
                    // Force null termination at datalen index.
                    fld[datalen] = '\0';
                    return std::string(fld);
                }

                static std::string generateJsonString(const NewOrderMsgV2 &msg, uint64_t customerOrderTime = 0)
                {
                    rapidjson::Document document;
                    rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
                    rapidjson::Value jsonObject(rapidjson::kObjectType);

                    // Add numeric fields
                    jsonObject.AddMember("StartOfMessage", rapidjson::Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", rapidjson::Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", rapidjson::Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", rapidjson::Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", rapidjson::Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", rapidjson::Value().SetUint(msg.SeqNum), allocator);

                    // Add character array and enum fields using helper functions
                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    AddEnumAsString(jsonObject, allocator, "Side", static_cast<char>(msg.Side));

                    jsonObject.AddMember("OrderQty", rapidjson::Value().SetUint(msg.OrderQty), allocator);

                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    AddCharArrayMember(jsonObject, allocator, "ClearingAccount", msg.ClearingAccount);

                    // Add price (assumed to be 64-bit integer type)
                    jsonObject.AddMember("Price", rapidjson::Value().SetInt64(msg.Price), allocator);

                    AddEnumAsString(jsonObject, allocator, "OrdType", static_cast<char>(msg.OrdType));
                    AddEnumAsString(jsonObject, allocator, "TimeInForce", static_cast<char>(msg.TimeInForce));

                    jsonObject.AddMember("MinQty", rapidjson::Value().SetUint(msg.MinQty), allocator);

                    AddCharArrayMember(jsonObject, allocator, "SecId", msg.SecId);

                    AddEnumAsString(jsonObject, allocator, "Capacity", msg.Capacity);
                    AddCharArrayMember(jsonObject, allocator, "Account", msg.Account);
                    AddCharArrayMember(jsonObject, allocator, "PreventMatch", msg.PreventMatch);

                    jsonObject.AddMember("ExpireTime", rapidjson::Value().SetUint(msg.ExpireTime), allocator);
                    jsonObject.AddMember("MaturityDate", rapidjson::Value().SetUint(msg.MaturityDate), allocator);

                    AddEnumAsString(jsonObject, allocator, "OpenClose", static_cast<char>(msg.OpenClose));
                    jsonObject.AddMember("CMTANumber", rapidjson::Value().SetUint(msg.CMTANumber), allocator);
                    jsonObject.AddMember("StopPx", rapidjson::Value().SetInt64(msg.StopPx), allocator);
                    jsonObject.AddMember("CustomGroupId", rapidjson::Value().SetUint(msg.CustomGroupId), allocator);

                    AddEnumAsString(jsonObject, allocator, "CtiCode", msg.CtiCode);
                    // For ManualOrderIndicator, following your NewOrderSingleMsg style we output it as a number
                    jsonObject.AddMember("ManualOrderIndicator", rapidjson::Value().SetUint(msg.ManualOrderIndicator), allocator);

                    AddCharArrayMember(jsonObject, allocator, "OEOID", msg.OEOID);
                    AddCharArrayMember(jsonObject, allocator, "FrequentTraderId", msg.FrequentTraderId);
                    AddEnumAsString(jsonObject, allocator, "CustOrderHandlingInst", msg.CustOrderHandlingInst);
                    AddCharArrayMember(jsonObject, allocator, "CountryCode", msg.CountryCode);

                    // Add CustomerOrderTime if provided (audit-only field, not in wire format)
                    if (customerOrderTime > 0) {
                        jsonObject.AddMember("CustomerOrderTime", rapidjson::Value().SetUint64(customerOrderTime), allocator);
                    }

                    // Convert JSON object to string
                    rapidjson::StringBuffer buffer;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);

                    return buffer.GetString();
                }

                static std::string generateJsonString(const ModifyOrderMsg &msg, uint64_t customerOrderTime = 0)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();

                    // Create the JSON object
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "OrigClOrdID", msg.OrigClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);

                    jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
                    jsonObject.AddMember("Price", Value().SetInt64(msg.Price), allocator);

                    AddEnumAsString(jsonObject, allocator, "OrdType", static_cast<char>(msg.OrdType));
                    AddEnumAsString(jsonObject, allocator, "CancelOrigOnReject", msg.CancelOrigOnReject);

                    jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx), allocator);
                    jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);

                    AddCharArrayMember(jsonObject, allocator, "OEOID", msg.OEOID);
                    AddCharArrayMember(jsonObject, allocator, "FrequentTraderId", msg.FrequentTraderId);
                    AddEnumAsString(jsonObject, allocator, "CustOrderHandlingInst", msg.CustOrderHandlingInst);

                    // Add CustomerOrderTime if provided (audit-only field, not in wire format)
                    if (customerOrderTime > 0) {
                        jsonObject.AddMember("CustomerOrderTime", Value().SetUint64(customerOrderTime), allocator);
                    }

                    // Convert the JSON object to a string
                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);

                    return buffer.GetString();
                }

                static std::string generateJsonString(const CancelOrderMsg &msg, uint64_t customerOrderTime = 0)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();

                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

                    AddCharArrayMember(jsonObject, allocator, "OrigClOrdID", msg.OrigClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);

                    jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
                    AddCharArrayMember(jsonObject, allocator, "OEOID", msg.OEOID);

                    // Add CustomerOrderTime if provided (audit-only field, not in wire format)
                    if (customerOrderTime > 0) {
                        jsonObject.AddMember("CustomerOrderTime", Value().SetUint64(customerOrderTime), allocator);
                    }

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);

                    return buffer.GetString();
                }

                static std::string generateJsonString(const MassCancelMsg &msg, uint64_t customerOrderTime = 0)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();

                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

                    AddCharArrayMember(jsonObject, allocator, "MassCancelId", msg.MassCancelId);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    AddCharArrayMember(jsonObject, allocator, "ProductName", msg.ProductName);
                    AddCharArrayMember(jsonObject, allocator, "MassCancelInst", msg.MassCancelInst);

                    jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
                    AddCharArrayMember(jsonObject, allocator, "OEOID", msg.OEOID);

                    // Add CustomerOrderTime if provided (audit-only field, not in wire format)
                    if (customerOrderTime > 0) {
                        jsonObject.AddMember("CustomerOrderTime", Value().SetUint64(customerOrderTime), allocator);
                    }

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);

                    return buffer.GetString();
                }

                static std::string generateJsonString(const PurgeOrdersMsg &msg, uint64_t customerOrderTime = 0)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();

                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

                    AddCharArrayMember(jsonObject, allocator, "MassCancelId", msg.MassCancelId);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    AddCharArrayMember(jsonObject, allocator, "ProductName", msg.ProductName);
                    AddCharArrayMember(jsonObject, allocator, "MassCancelInst", msg.MassCancelInst);

                    jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
                    AddCharArrayMember(jsonObject, allocator, "OEOID", msg.OEOID);

                    jsonObject.AddMember("CustomGroupIdCnt", Value().SetUint(msg.CustomGroupIdCnt), allocator);
                    jsonObject.AddMember("CustomGroupId", Value().SetUint(msg.CustomGroupId), allocator);

                    // Add CustomerOrderTime if provided (audit-only field, not in wire format)
                    if (customerOrderTime > 0) {
                        jsonObject.AddMember("CustomerOrderTime", Value().SetUint64(customerOrderTime), allocator);
                    }

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);

                    return buffer.GetString();
                }

                static std::string generateJsonString(const RiskResetMsg &msg, uint64_t customerOrderTime = 0)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

                    AddCharArrayMember(jsonObject, allocator, "RiskStatusId", msg.RiskStatusId);
                    AddCharArrayMember(jsonObject, allocator, "RiskReset", msg.RiskReset);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    AddCharArrayMember(jsonObject, allocator, "ProductName", msg.ProductName);

                    jsonObject.AddMember("CustomGroupId", Value().SetUint(msg.CustomGroupId), allocator);

                    // Add CustomerOrderTime if provided (audit-only field, not in wire format)
                    if (customerOrderTime > 0) {
                        jsonObject.AddMember("CustomerOrderTime", Value().SetUint64(customerOrderTime), allocator);
                    }

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const OrderAckMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();

                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);

                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
                    AddEnumAsString(jsonObject, allocator, "Side", static_cast<char>(msg.Side));
                    jsonObject.AddMember("Price", Value().SetInt64(msg.Price), allocator);

                    AddEnumAsString(jsonObject, allocator, "OrdType", static_cast<char>(msg.OrdType));
                    AddEnumAsString(jsonObject, allocator, "TimeInForce", static_cast<char>(msg.TimeInForce));
                    jsonObject.AddMember("MinQty", Value().SetUint(msg.MinQty), allocator);

                    AddCharArrayMember(jsonObject, allocator, "SecId", msg.SecId);

                    AddEnumAsString(jsonObject, allocator, "Capacity", msg.Capacity);
                    AddCharArrayMember(jsonObject, allocator, "Account", msg.Account);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    AddCharArrayMember(jsonObject, allocator, "ClearingAccount", msg.ClearingAccount);

                    jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
                    AddCharArrayMember(jsonObject, allocator, "PreventMatch", msg.PreventMatch);
                    jsonObject.AddMember("MaturityDate", Value().SetUint(msg.MaturityDate), allocator);
                    AddEnumAsString(jsonObject, allocator, "OpenClose", static_cast<char>(msg.OpenClose));
                    jsonObject.AddMember("LeavesQty", Value().SetUint(msg.LeavesQty), allocator);

                    AddEnumAsString(jsonObject, allocator, "BaseLiquidityIndicator", msg.BaseLiquidityIndicator);
                    jsonObject.AddMember("ExpireTime", Value().SetUint64(msg.ExpireTime), allocator);
                    AddEnumAsString(jsonObject, allocator, "SubLiquidityIndicator", msg.SubLiquidityIndicator);
                    jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx), allocator);
                    jsonObject.AddMember("CMTANumber", Value().SetUint(msg.CMTANumber), allocator);
                    AddEnumAsString(jsonObject, allocator, "CtiCode", msg.CtiCode);

                    jsonObject.AddMember("ManualOrderIndicator", Value().SetUint(msg.ManualOrderIndicator), allocator);
                    AddCharArrayMember(jsonObject, allocator, "OEOID", msg.OEOID);
                    AddCharArrayMember(jsonObject, allocator, "FrequentTraderId", msg.FrequentTraderId);
                    AddEnumAsString(jsonObject, allocator, "CustOrderHandlingInst", msg.CustOrderHandlingInst);
                    jsonObject.AddMember("RequestReceivedTime", Value().SetUint64(msg.RequestReceivedTime), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);

                    return buffer.GetString();
                }

                static std::string generateJsonString(const OrderRejectMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);

                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    AddEnumAsString(jsonObject, allocator, "OrderRejectReason", static_cast<char>(msg.OrderRejectReason));
                    AddCharArrayMember(jsonObject, allocator, "Text", msg.Text);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const OrderModifiedMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);

                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "OrigClOrdID", msg.OrigClOrdID);
                    jsonObject.AddMember("OrderID", Value().SetUint64(msg.OrderID), allocator);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    jsonObject.AddMember("Price", Value().SetInt64(msg.Price), allocator);
                    AddEnumAsString(jsonObject, allocator, "OrdType", static_cast<char>(msg.OrdType));
                    jsonObject.AddMember("OrderQty", Value().SetUint(msg.OrderQty), allocator);
                    jsonObject.AddMember("LeavesQty", Value().SetUint(msg.LeavesQty), allocator);
                    AddEnumAsString(jsonObject, allocator, "BaseLiquidityIndicator", msg.BaseLiquidityIndicator);
                    jsonObject.AddMember("StopPx", Value().SetInt64(msg.StopPx), allocator);
                    AddCharArrayMember(jsonObject, allocator, "FrequentTraderId", msg.FrequentTraderId);
                    AddEnumAsString(jsonObject, allocator, "CustOrderHandlingInst", msg.CustOrderHandlingInst);
                    jsonObject.AddMember("RequestReceivedTime", Value().SetUint64(msg.RequestReceivedTime), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const ModifyRejectedMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);

                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "OrigClOrdID", msg.OrigClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    AddEnumAsString(jsonObject, allocator, "ModifyRejectReason", static_cast<char>(msg.ModifyRejectReason));
                    AddCharArrayMember(jsonObject, allocator, "Text", msg.Text);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const OrderExecutionMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);

                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    jsonObject.AddMember("ExecId", Value().SetUint64(msg.ExecId), allocator);
                    jsonObject.AddMember("LastShares", Value().SetUint(msg.LastShares), allocator);
                    jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx), allocator);
                    jsonObject.AddMember("LeavesQty", Value().SetUint(msg.LeavesQty), allocator);

                    AddEnumAsString(jsonObject, allocator, "BaseLiquidityIndicator", msg.BaseLiquidityIndicator);
                    AddEnumAsString(jsonObject, allocator, "SubLiquidityIndicator", msg.SubLiquidityIndicator);
                    AddEnumAsString(jsonObject, allocator, "Side", static_cast<char>(msg.Side));

                    AddCharArrayMember(jsonObject, allocator, "SecId", msg.SecId);

                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    jsonObject.AddMember("MaturityDate", Value().SetUint(msg.MaturityDate), allocator);
                    AddCharArrayMember(jsonObject, allocator, "FeeCode", msg.FeeCode);
                    jsonObject.AddMember("TradeDate", Value().SetUint(msg.TradeDate), allocator);
                    jsonObject.AddMember("ClearingSize", Value().SetUint(msg.ClearingSize), allocator);
                    AddEnumAsString(jsonObject, allocator, "PendingStatus", msg.PendingStatus);
                    AddEnumAsString(jsonObject, allocator, "MultilegReportingType", static_cast<char>(msg.MultilegReportingType));
                    jsonObject.AddMember("SecondaryExecId", Value().SetUint64(msg.SecondaryExecId), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const OrderCancelledMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    AddEnumAsString(jsonObject, allocator, "CancelReason", static_cast<char>(msg.CancelReason));
                    jsonObject.AddMember("RequestReceivedTime", Value().SetUint64(msg.RequestReceivedTime), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const CancelRejectMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    AddEnumAsString(jsonObject, allocator, "CancelRejectReason", static_cast<char>(msg.CancelRejectReason));
                    AddCharArrayMember(jsonObject, allocator, "Text", msg.Text);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const MassCancelAckMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
                    AddCharArrayMember(jsonObject, allocator, "MassCancelId", msg.MassCancelId);
                    jsonObject.AddMember("CancelledOrderCount", Value().SetUint(msg.CancelledOrderCount), allocator);
                    jsonObject.AddMember("RequestReceivedTime", Value().SetUint64(msg.RequestReceivedTime), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const MassCancelRejectMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
                    AddCharArrayMember(jsonObject, allocator, "MassCancelId", msg.MassCancelId);
                    AddEnumAsString(jsonObject, allocator, "MassCancelRejectReason", static_cast<char>(msg.MassCancelRejectReason));
                    AddCharArrayMember(jsonObject, allocator, "Text", msg.Text);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const PurgeAckMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
                    AddCharArrayMember(jsonObject, allocator, "MassCancelId", msg.MassCancelId);
                    jsonObject.AddMember("PurgedOrderCount", Value().SetUint(msg.PurgedOrderCount), allocator);
                    jsonObject.AddMember("RequestReceivedTime", Value().SetUint64(msg.RequestReceivedTime), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const PurgeRejectedMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
                    AddCharArrayMember(jsonObject, allocator, "MassCancelId", msg.MassCancelId);
                    AddEnumAsString(jsonObject, allocator, "PurgeRejectReason", static_cast<char>(msg.PurgeRejectReason));
                    AddCharArrayMember(jsonObject, allocator, "Text", msg.Text);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const TradeCancelCorrectMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    jsonObject.AddMember("ExecRefId", Value().SetUint64(msg.ExecRefId), allocator);
                    AddEnumAsString(jsonObject, allocator, "Side", static_cast<char>(msg.Side));
                    AddEnumAsString(jsonObject, allocator, "BaseLiquidityIndicator", msg.BaseLiquidityIndicator);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    AddCharArrayMember(jsonObject, allocator, "ClearingAccount", msg.ClearingAccount);
                    jsonObject.AddMember("LastShares", Value().SetUint(msg.LastShares), allocator);
                    jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx), allocator);
                    jsonObject.AddMember("CorrectedPrice", Value().SetInt64(msg.CorrectedPrice), allocator);
                    jsonObject.AddMember("OrigTime", Value().SetUint64(msg.OrigTime), allocator);
                    AddCharArrayMember(jsonObject, allocator, "Symbol", msg.Symbol);
                    AddEnumAsString(jsonObject, allocator, "Capacity", msg.Capacity);
                    jsonObject.AddMember("MaturityDate", Value().SetUint(msg.MaturityDate), allocator);
                    AddEnumAsString(jsonObject, allocator, "OpenClose", static_cast<char>(msg.OpenClose));
                    jsonObject.AddMember("CMTANumber", Value().SetUint(msg.CMTANumber), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const TASRestatementMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);
                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);
                    jsonObject.AddMember("ExecId", Value().SetUint64(msg.ExecId), allocator);
                    AddEnumAsString(jsonObject, allocator, "Side", static_cast<char>(msg.Side));
                    jsonObject.AddMember("Price", Value().SetInt64(msg.Price), allocator);

                    AddCharArrayMember(jsonObject, allocator, "SecId", msg.SecId);

                    jsonObject.AddMember("MaturityDate", Value().SetUint(msg.MaturityDate), allocator);
                    jsonObject.AddMember("LastShares", Value().SetUint(msg.LastShares), allocator);
                    jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx), allocator);
                    AddCharArrayMember(jsonObject, allocator, "FeeCode", msg.FeeCode);
                    jsonObject.AddMember("TradeDate", Value().SetUint(msg.TradeDate), allocator);
                    jsonObject.AddMember("ClearingPrice", Value().SetInt64(msg.ClearingPrice), allocator);
                    AddCharArrayMember(jsonObject, allocator, "ClearingSymbol", msg.ClearingSymbol);
                    AddEnumAsString(jsonObject, allocator, "MultilegReportingType", msg.MultilegReportingType);
                    jsonObject.AddMember("SecondaryExecId", Value().SetUint64(msg.SecondaryExecId), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const VarianceRestatementMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    jsonObject.AddMember("TransactTime", Value().SetUint64(msg.TransactTime), allocator);

                    AddCharArrayMember(jsonObject, allocator, "ClOrdID", msg.ClOrdID);
                    AddCharArrayMember(jsonObject, allocator, "ClearingFirm", msg.ClearingFirm);

                    jsonObject.AddMember("ExecId", Value().SetUint64(msg.ExecId), allocator);
                    AddEnumAsString(jsonObject, allocator, "Side", static_cast<char>(msg.Side));
                    jsonObject.AddMember("Price", Value().SetInt64(msg.Price), allocator);

                    AddCharArrayMember(jsonObject, allocator, "SecId", msg.SecId);

                    jsonObject.AddMember("MaturityDate", Value().SetUint(msg.MaturityDate), allocator);
                    jsonObject.AddMember("LastShares", Value().SetUint(msg.LastShares), allocator);
                    jsonObject.AddMember("LastPx", Value().SetInt64(msg.LastPx), allocator);
                    AddCharArrayMember(jsonObject, allocator, "FeeCode", msg.FeeCode);
                    jsonObject.AddMember("TradeDate", Value().SetUint(msg.TradeDate), allocator);
                    jsonObject.AddMember("ClearingPrice", Value().SetInt64(msg.ClearingPrice), allocator);
                    AddCharArrayMember(jsonObject, allocator, "ClearingSymbol", msg.ClearingSymbol);
                    AddEnumAsString(jsonObject, allocator, "MultilegReportingType", msg.MultilegReportingType);
                    jsonObject.AddMember("SecondaryExecId", Value().SetUint64(msg.SecondaryExecId), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const ResetRiskAckMsg::BlockData &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

                    AddCharArrayMember(jsonObject, allocator, "RiskStatusId", msg.RiskStatusId);
                    AddEnumAsString(jsonObject, allocator, "RiskResetResult", msg.RiskResetResult);
                    jsonObject.AddMember("RequestReceivedTime", Value().SetUint64(msg.RequestReceivedTime), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const LoginRequestMsg &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    AddCharArrayMember(jsonObject, allocator, "SessionId", msg.SessionId);
                    AddCharArrayMember(jsonObject, allocator, "SessionSubId", msg.SessionSubId);
                    AddCharArrayMember(jsonObject, allocator, "Password", msg.Password);
                    AddEnumAsString(jsonObject, allocator, "ReplayUnspecifiedUnit", static_cast<char>(msg.ReplayUnspecifiedUnit));
                    jsonObject.AddMember("NumberOfUnits", Value().SetUint(msg.NumberOfUnits), allocator);
                    jsonObject.AddMember("UnitNumber", Value().SetUint(msg.UnitNumber), allocator);
                    jsonObject.AddMember("UnitSequence", Value().SetUint(msg.UnitSequence), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const LogoutRequestMsg &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const ClientHeartbeatMsg &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const LoginResponseMsg &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    AddEnumAsString(jsonObject, allocator, "LoginResponseStatus", static_cast<char>(msg.LoginResponseStatus));
                    AddCharArrayMember(jsonObject, allocator, "LoginResponseText", msg.LoginResponseText);
                    jsonObject.AddMember("ClientSeqNum", Value().SetUint(msg.ClientSeqNum), allocator);
                    jsonObject.AddMember("NbrUnits", Value().SetUint(msg.NbrUnits), allocator);
                    jsonObject.AddMember("UnitNumber", Value().SetUint(msg.UnitNumber), allocator);
                    jsonObject.AddMember("UnitSequence", Value().SetUint(msg.UnitSequence), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const ReplayCompleteMsg &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const LogoutResponseMsg &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);
                    AddEnumAsString(jsonObject, allocator, "LogoutReason", static_cast<char>(msg.LogoutReason));
                    AddCharArrayMember(jsonObject, allocator, "LogoutReasonText", msg.LogoutReasonText);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

                static std::string generateJsonString(const ServerHeartbeatMsg &msg)
                {
                    Document document;
                    Document::AllocatorType &allocator = document.GetAllocator();
                    Value jsonObject(kObjectType);

                    jsonObject.AddMember("StartOfMessage", Value().SetUint(msg.StartOfMessage), allocator);
                    jsonObject.AddMember("MessageLength", Value().SetUint(msg.MessageLength), allocator);
                    jsonObject.AddMember("MessageType", Value().SetUint(msg.MessageType), allocator);
                    jsonObject.AddMember("MatchingUnit", Value().SetUint(msg.MatchingUnit), allocator);
                    jsonObject.AddMember("Reserved", Value().SetUint(msg.Reserved), allocator);
                    jsonObject.AddMember("SeqNum", Value().SetUint(msg.SeqNum), allocator);

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    jsonObject.Accept(writer);
                    return buffer.GetString();
                }

            private:
                // Static helper function to handle char arrays without null terminators
                template <size_t N>
                static void AddCharArrayMember(Value &jsonObject, Document::AllocatorType &allocator, const char *key, const char (&charArray)[N])
                {
                    // Build string with only printable ASCII characters (0x20-0x7E)
                    // Skip non-printable chars, stop at null terminator or end of array
                    std::string result;
                    result.reserve(N);
                    for (size_t i = 0; i < N && charArray[i] != '\0'; ++i) {
                        if (charArray[i] >= 0x20 && charArray[i] <= 0x7E) {
                            result += charArray[i];
                        }
                    }
                    jsonObject.AddMember(Value().SetString(key, allocator), Value().SetString(result.c_str(), allocator), allocator);
                }

                // Helper for serializing enums as characters (not ASCII codes)
                static void AddEnumAsString(Value &jsonObject, Document::AllocatorType &allocator, const char *key, char enumValue)
                {
                    // Safety: Convert binary 0-9 to ASCII '0'-'9'
                    // CFE BOE3 protocol uses ASCII chars, but sometimes binary values appear
                    char asciiValue = enumValue;
                    if (enumValue >= 0 && enumValue <= 9) {
                        asciiValue = '0' + enumValue;
                    }
                    jsonObject.AddMember(Value().SetString(key, allocator), Value().SetString(std::string(1, asciiValue).c_str(), allocator), allocator);
                }
            };

        } // namespace utils
    } // namespace Boe3
} // namespace KTN

#endif /* SRC_BOE3_V3MESSAGEPRINTER_HPP_ */
