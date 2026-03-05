//============================================================================
// Name        	: OrderUtils.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 3, 2023 2:14:36 PM
//============================================================================

#ifndef SRC_ORDERS_ORDERUTILS_HPP_
#define SRC_ORDERS_ORDERUTILS_HPP_
#pragma once
#include <string>
#include <charconv> // required for std::to_chars
#include <Orders/Order.hpp>
#include <Orders/OrderPod.hpp>
using namespace KTN;

#include <akl/PriceConverters.hpp>


class OrderUtils
{
public:

	static string ConvertCmeId(uint64_t exchordid)
	{
		ostringstream eee;
		eee << exchordid;
		return eee.str();
	}

	static void fastCopy(char* dest, const char* src, size_t length) {
		memset(dest, 0,length);
		for (size_t i = 0; i < length; ++i) {
			if (src[i] == '\0')
				break;  // Stop copying if we reach the end of the source string
			dest[i] = src[i];
		}
	}

	static void Copy(char* dest, const char* src) {

		for (size_t i = 0; i < strlen(src); ++i) {
			if (src[i] == '\0')
				break;  // Stop copying if we reach the end of the source string
			dest[i] = src[i];
		}
	}

	static string toString(const char* src)//, size_t length)
	{
		string res = src;
		return res;
	}

	static void toChar(char* dest, uint64_t src, size_t bufferSize) {
		auto result = std::to_chars(dest, dest + bufferSize, src);
		if (result.ec == std::errc()) {
			// Manually add null terminator
			*result.ptr = '\0';
		} else {
			// Handle error (e.g., set dest[0] = '\0' or report failure)
			if (bufferSize > 0) {
				dest[0] = '\0';
			}
		}
	}
	

	static std::string Print(const KTN::Order& ord)
	{
		ostringstream oss;
		oss << fixed;
		oss << "USER="<< ord.user << "|ORG=" << ord.org << "|GROUP=" << ord.groupname <<  "|ALGOID=" << ord.algoid <<  "|CLORDID="<< ord.ordernum
				<< "|GUID=" << ord.guid << "|SIDE="<< KOrderSide::toString(ord.OrdSide)
				<< "|QTY="<< ord.quantity  << "|SYMB="<< ord.symbol << "|EXCH=" << ord.exchange
				<< "|PRODTYPE=" << KOrderProdType::toString(ord.OrdProdType)
				<< "|SECID=" << ord.secid
				<< "|PRICE="<< ord.price
				<< "|EXCHORDID="<< ord.exchordid <<  "|ORIGCLORDID=" << ord.origclordid <<  "|TTIME="<< ord.transacttime
				<< "|LVSQTY="<< ord.leavesqty << "|FILLQTY="<< ord.fillqty
				<< "|DISPQTY="<< ord.dispqty
				<< "|MINQTY="<< ord.minqty
				<< "|LASTPX=" << ord.lastpx << "|LASTQTY=" << ord.lastqty
				<< "|STATUS="<< KOrderStatus::toString(ord.OrdStatus)
				<< "|STATE="<< ord.state
				<< "|TAG="<< ord.tag << "|TAG2="<< ord.tag2 << "|SOURCE=" << ord.source
				<< "|REF=" << ord.reference
				<< "|STOPPX=" << ord.stoppx
				<< "|SECID=" << ord.secid << "|MKTSEGID=" << ord.mktsegid << "|CALLBACKID=" << ord.callbackid
				<< "|OrdSide=" << KOrderSide::toString(ord.OrdSide)
				<< "|OrdExch=" << KOrderExchange::toString(ord.OrdExch)
				<< "|OrdProdType=" << KOrderProdType::toString(ord.OrdProdType)
				<< "|OrdType=" << KOrderType::toString(ord.OrdType)
				<< "|OrdFillType=" << KOrderFillType::toString(ord.OrdFillType)
				<< "|OrdStatus=" << KOrderStatus::toString(ord.OrdStatus)
				<< "|OrdTif=" << KOrderTif::toString(ord.OrdTif)
				<< "|OrdPutCall=" << KOrderPutCall::toString(ord.OrdPutCall)
				<< "|OrdAction=" << KOrderAction::toString(ord.OrdAction)
				<< "|OrdState=" << KOrderState::toString(ord.OrdState)
				<< "|OrdAlgoTrig=" << KOrderAlgoTrigger::toString(ord.OrdAlgoTrig)
				<< "|INST_INDEX=" << ord.instindex
		<< "|ReqId=" << ord.orderReqId
		<< "|StratID=" << ord.stratid;

		return oss.str();
	}

	// This function assumes that your Order type and enum conversion functions 
    // (e.g. KOrderSide::toString, KOrderType::toString, etc.) are defined elsewhere.
    static std::string PrintOrdersTable(const std::vector<Order>& orders) {
        // Define header names exactly as required.
        std::vector<std::string> headers = {
            "USER", "TIF", "STATUS", "CLORDID", "OrdSide", "QTY", "SYMB", "SECID", "PRICE",
            "OrdType", "CMEID", "LVSQTY", 
            "TAG", "OrdTif", "ReqId", "StratID"
        };
    
        // Build a row for each order using the field mapping described above.
        std::vector<std::vector<std::string>> rows;
        for (const auto &ord : orders) {
            std::vector<std::string> row;
            row.push_back(ord.user);
            row.push_back(KOrderTif::toString(ord.OrdTif));
            row.push_back(KOrderStatus::toString(ord.OrdStatus));
            row.push_back(ord.ordernum);
            row.push_back(KOrderSide::toString(ord.OrdSide));
            row.push_back(std::to_string(ord.quantity));
            row.push_back(ord.symbol);
            row.push_back(std::to_string(ord.secid));
            row.push_back(std::to_string(ord.price));
            row.push_back(KOrderType::toString(ord.OrdType));
            row.push_back(std::to_string(ord.exchordid));
            row.push_back(std::to_string(ord.leavesqty));
            // row.push_back(std::to_string(ord.fillqty));
            // row.push_back(std::to_string(ord.dispqty));
            // row.push_back(std::to_string(ord.minqty));
            // row.push_back(std::to_string(ord.lastpx));
            // row.push_back(std::to_string(ord.lastqty));
            row.push_back(ord.tag);
            //row.push_back(ord.source);
            row.push_back(KOrderTif::toString(ord.OrdTif));
            row.push_back(std::to_string(ord.orderReqId));
            row.push_back(std::to_string(ord.stratid));
    
            // Only add row if it has the expected number of fields.
            if (row.size() == headers.size())
                rows.push_back(row);
        }
    
        // Determine the maximum width for each column.
        size_t numCols = headers.size();
        std::vector<size_t> colWidths(numCols, 0);
        for (size_t i = 0; i < numCols; i++) {
            colWidths[i] = headers[i].length();
            for (const auto &row : rows) {
                colWidths[i] = std::max(colWidths[i], row[i].length());
            }
        }
    
        // Build a horizontal border line.
        std::ostringstream oss;
        oss << "+";
        for (size_t i = 0; i < numCols; i++) {
            oss << std::string(colWidths[i] + 2, '-') << "+";
        }
        std::string border = oss.str();
    
        // Build the header row.
        oss.str("");
        oss.clear();
        oss << "|";
        for (size_t i = 0; i < numCols; i++) {
            oss << " " << std::left << std::setw(colWidths[i]) << headers[i] << " |";
        }
        std::string headerRow = oss.str();
    
        // Assemble the table with a border above and below the header, then each data row.
        std::ostringstream table;
        table << border << "\n" << headerRow << "\n" << border << "\n";
        for (const auto &row : rows) {
            oss.str("");
            oss.clear();
            oss << "|";
            for (size_t i = 0; i < numCols; i++) {
                oss << " " << std::left << std::setw(colWidths[i]) << row[i] << " |";
            }
            table << oss.str() << "\n";
        }
        table << border;
    
        return table.str();
    }

	static std::string Print(const KTN::OrderPod& ord)
	{
		ostringstream oss;
		oss << fixed;
		oss 	<< " QTY="<< ord.quantity
				<< "|SECID=" << ord.secid
				<< "|CALLBACKID=" << (int)ord.callbackid
				<< "|EXCHORDID="<< ord.exchordid
				<< "|CLORDID=" << ord.ordernum
				<< "|PRICE="<< ord.price
				<< "|SIDE=" << KOrderSide::toString(ord.OrdSide)
				<< "|LVS="<< ord.leavesqty
				<< "|FILL="<< ord.fillqty
				<< "|DISP="<< ord.dispqty
				<< "|MIN="<< ord.minqty
				<< "|MKTSEGID=" << (int)ord.mktsegid
				<< "|STATUS=" << KOrderStatus::toString(ord.OrdStatus)
				<< "|ALGO=" << KOrderAlgoTrigger::toString(ord.OrdAlgoTrig)
				<< "|ACTION=" << KOrderAction::toString(ord.OrdAction)
				<< "|ORDREQID=" << ord.orderReqId
				<< "|TSTAMP=" << ord.timestamp
				<< "|EXCH=" << KOrderExchange::toString(ord.OrdExch)				
				<< "|ORDTYPE=" << KOrderType::toString(ord.OrdType)
				<< "|TIF=" << KOrderTif::toString(ord.OrdTif)
				<< "|STATE=" << KOrderState::toString(ord.OrdState)
				<< "|STRATID=" << ord.stratid
				<< "|INSTIDX=" << (int)ord.instindex;
		return oss.str();
	}

	static std::string QuickPrint(const KTN::Order& ord)
	{
		ostringstream oss;
		oss << fixed;
		oss << " CLORDID="<< ord.ordernum << "|SIDE="<< KOrderSide::toString(ord.OrdSide)
				<< "|QTY="<< ord.quantity  << "|SYMB="<< ord.symbol
				<< "|SECID=" << ord.secid
				<< "|PRICE="<< ord.price << "|ORDTYPE=" << KOrderType::toString(ord.OrdType)
				<< "|EXCHORDID="<< ord.exchordid <<  "|ORIGCLORDID=" << ord.origclordid
				<< "|STRATID=" << ord.stratid << "|ACTION=" << KOrderAction::toString(ord.OrdAction);

		return oss.str();
	}


	static void FillFields(Order & ord, UserInfo u, std::string algoid="")
	{
		ord.user = u.user;
		ord.org = u.org;
		ord.groupname = u.groupname;
		if (algoid.length() > 0)
			ord.algoid = algoid;
	}

	static void SetV2Fields(KTN::Order & ord, KOrderAction::Enum action, KOrderStatus::Enum status,
			bool inflight, bool ishedge=false, int callbackid=0)
	{
		//ord.OrdSide = (ord.side == "B") ? KOrderSide::BUY : KOrderSide::SELL;

		ord.OrdExch = ORD::KOrderExchange::CME;

		//ord.OrdProdType = ORD::KOrderProdType::Value(inst.prodtype);
		//fix this for FAK and STOPS ETC:
		ord.OrdTif = ORD::KOrderTif::DAY;
		//ord.OrdType = (ord.ordtype == "MARKET") ? ORD::KOrderType::MARKET : ORD::KOrderType::LIMIT;

		ord.OrdAction = action;
		ord.OrdStatus = status;
		ord.OrdState = (inflight) ?  KOrderState::INFLIGHT : KOrderState::WORKING;
		ord.OrdAlgoTrig = (!ishedge) ? KOrderAlgoTrigger::ALGO_LEG : KOrderAlgoTrigger::ALGO_HEDGE;

		ord.callbackid = callbackid;
	}

	static UserInfo FillUserInfo(Order ord)
	{
		UserInfo u = {};
		u.user = ord.user;
		u.org = ord.org;
		u.groupname = ord.groupname;

		return u;
	}

	static void Transpose(Order& dest, const OrderPod& src)
	{
		dest.OrdStatus = src.OrdStatus;
		dest.OrdState = src.OrdState;
		dest.exchordid = src.exchordid;
		dest.OrdAction = src.OrdAction;
		dest.OrdAlgoTrig = src.OrdAlgoTrig;

		dest.orderReqId = src.orderReqId;

		if (dest.exchordid == 0)
			dest.exchordid = src.exchordid;
		
		if (src.quantity > 0)
		dest.quantity = src.quantity;
		
		dest.leavesqty = src.leavesqty;
		dest.timestamp = src.timestamp;
		
		if (dest.price == 0)
			dest.price = src.price;
		
		if (dest.stoppx == 0)
			dest.stoppx = src.stopprice;

		dest.lastpx = src.lastpx;
		dest.lastqty = src.lastqty;

		dest.minqty = src.minqty;
		dest.dispqty = src.dispqty;

		dest.openclose = KOrderOpenCloseInd::toString(src.OrdOpenClose);
	}

	//legacy old order to order pod:
	static void Transpose(OrderPod& pod, const Order& ord, string clordid, bool certnr=false)
	{
		pod.orderReqId = ord.orderReqId;
		pod.exchordid = ord.exchordid;
		OrderUtils::fastCopy(pod.ordernum, clordid.c_str(), sizeof(ord.ordernum));

		if (pod.secid == 0)
			pod.secid = ord.secid;
	
		if (pod.stratid == 0)
			pod.stratid = ord.stratid;
		
		pod.callbackid = -1;

		pod.isManual = true;
		pod.mktsegid = ord.mktsegid;

		pod.OrdAction = ord.OrdAction;
		pod.OrdAlgoTrig = ord.OrdAlgoTrig;
		pod.OrdExch = ord.OrdExch;
		pod.OrdSide = ord.OrdSide;
		pod.OrdState = ord.OrdState;
		pod.OrdStatus = ord.OrdStatus;
		pod.OrdTif = ord.OrdTif;
		pod.OrdType = ord.OrdType;
		pod.OrdOpenClose = KOrderOpenCloseInd::Value(ord.openclose);

		pod.quantity = ord.quantity;
		pod.leavesqty = ord.leavesqty;
		pod.fillqty = ord.fillqty;
		pod.dispqty = ord.dispqty;
		pod.minqty = ord.minqty;

		if (pod.price == 0)
			pod.price = ord.price;
		if (pod.stopprice == 0)
		{
			pod.stopprice = ord.stoppx;
			if (ord.price == 0)
				pod.price = ord.stoppx;
		}
			
	}


	static void Update(OrderPod& dest, const OrderPod& src)
	{
		dest.quantity = src.quantity;
		dest.leavesqty = src.leavesqty;
		dest.timestamp = src.timestamp;
		dest.OrdStatus = src.OrdStatus;
		dest.OrdState = src.OrdState;
		dest.orderReqId = src.orderReqId;
		if (dest.exchordid == 0)
			dest.exchordid = src.exchordid;
		
		dest.OrdAction = src.OrdAction;

		if (dest.price == 0)
			dest.price = src.price;
		
		if (dest.stopprice == 0)
			dest.stopprice = src.stopprice;

		dest.OrdAlgoTrig = src.OrdAlgoTrig;
		dest.dispqty = src.dispqty;
		dest.minqty = src.minqty;
	}

	static void UpdateFromApi(OrderPod& pod,  Order& apiord)
	{
		apiord.orderReqId = pod.orderReqId;

		if (pod.exchordid == 0)
			pod.exchordid = apiord.exchordid;
		
		if (pod.secid == 0)
			pod.secid = apiord.secid;

		OrderUtils::fastCopy(pod.ordernum, apiord.ordernum, sizeof(apiord.ordernum));

		pod.OrdAction = apiord.OrdAction;
		pod.OrdAlgoTrig = apiord.OrdAlgoTrig;
		pod.OrdExch = apiord.OrdExch;
		pod.OrdSide = apiord.OrdSide;
		pod.OrdTif = apiord.OrdTif;
		pod.OrdType = apiord.OrdType;

		pod.stratid = apiord.stratid;
		pod.quantity = apiord.quantity;
		if (pod.price == 0)
			pod.price = apiord.price;
		
		if (pod.stopprice == 0)
			pod.stopprice = apiord.stoppx;
			
		pod.isManual = apiord.isManual;
		
		pod.dispqty = apiord.dispqty;
		pod.minqty = apiord.minqty;
		

		if (pod.leavesqty > 0)
		{
			apiord.leavesqty = pod.leavesqty;
			apiord.fillqty = pod.fillqty;
			
			pod.OrdState = KOrderState::WORKING;
		}

		//TODO mwojcik
		//pod.OrdOpenClose = apiord.OrdOpenClose;
	}


};

#endif /* SRC_ORDERS_ORDERUTILS_HPP_ */
