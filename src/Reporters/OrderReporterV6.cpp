//============================================================================
// Name        	: OrderReporterV6.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 23, 2023 3:04:50 PM
//============================================================================

#include <Reporters/OrderReporterV6.hpp>
#include <iostream>
#include <zmq.h>

using namespace KTN::REPORTER;

OrderReporterV6::OrderReporterV6(string owner)
{
	Settings settings("Settings.txt");
	_ACCT = settings.getString("Account");
	_KEY = settings.getString("Key");
	_SRC = settings.getString("Source");
	_ORG = settings.getString("Org");

	_SHOW = settings.getBoolean("Orders.ShowReports");

	CreateAndRun(owner, 2);
}


OrderReporterV6::~OrderReporterV6()
{
}

void OrderReporterV6::UpdateMetaParams(const algometa& meta)
{
	_PARAMS = meta;

}

void OrderReporterV6::UpdateUGOMap(uint64_t reqid, UserInfo ugo)
{
	_ugoMap[reqid] = ugo;
}

void OrderReporterV6::UpdateTagMap(uint64_t reqid, string tag)
{
	_tagmap[reqid] = tag;
}

void OrderReporterV6::Report(const KTN::OrderPod& ord)
{
	_qOrds.push(ord);
}

//Note: this will be a bit slower than the normal report, but it will
// allow you to send the order with the metadata.  This is useful for
// order recovery and other things.
void OrderReporterV6::ReportWithMeta(const KTN::OrderPod& ord, const algometa& meta)
{
	KTN::OrderPod ordWithMeta = ord;
	ordWithMeta.hasMeta = true;

	_algometaMap[ordWithMeta.orderReqId] = meta;

	_qOrds.push(ordWithMeta);
}

void OrderReporterV6::Stop()
{
	if (_ACTIVE.load(std::memory_order_relaxed) == true) {
		_ACTIVE.store(false, std::memory_order_relaxed);
	 }
}

void OrderReporterV6::Run()
{
	Settings settings("Settings.txt");
	string ip = settings.getString("ArgoServer");
	int port = settings.getInteger("ArgoPushPort");
	string endpoint = "tcp://" + ip + ":" + std::to_string(port); 


	void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_PUSH);

    // Log connection attempt
    std::ostringstream zzz;
    KT01_LOG_OK("OrderReporterV6", "OK: Connecting to ZMQ endpoint: " + endpoint);

    // Connect to the endpoint
    int rc = zmq_connect(socket, endpoint.c_str());
    if (rc != 0) {
        KT01_LOG_ERROR("OrderReporterV6", "Failed to connect to ZMQ endpoint: " + endpoint);
        zmq_close(socket);
        zmq_ctx_destroy(context);
        std::exit(EXIT_FAILURE);
    }

	int MAX_ORDS = 100;

	_ACTIVE.store(true, std::memory_order_relaxed);
	
	while(_ACTIVE.load(std::memory_order_relaxed) == true)
	{
		//drain the entire queue and then process/group your json
		std::vector<OrderPod> ords;
		OrderPod ord = {};
		while (_qOrds.try_pop(ord))
		{
			ords.push_back(ord);
		}

		if (ords.size() > 0)
		{
			rapidjson::Document jsonDoc;
			jsonDoc.SetObject();

			rapidjson::Value devices(rapidjson::Type::kArrayType);

			std::vector<OrderPod> ordtemp;
			std::vector<std::string> json_ready_to_send;

			for (auto ord : ords)
			{
				if (ord.secid <= 0)
				{
					KT01_LOG_WARN("OrderReporterV6", "DID NOT SEND ORDER JSON! SECID=0. PASS.");
					KT01_LOG_WARN("OrderReporterV6", OrderUtils::Print(ord));
					continue;
				}
				auto now = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
				auto epoch = now.time_since_epoch();
				auto ssboe = std::chrono::duration_cast<std::chrono::microseconds>(epoch).count();
				ord.timestamp = ssboe;
				//ord.OrdExch = KOrderExchange::CME;

				ordtemp.push_back(ord);
				if ((int)ordtemp.size() >= MAX_ORDS)
				{
					std::string szJson = FormatOrdersJson(ordtemp);
					if (szJson != "")
						json_ready_to_send.push_back(szJson);
					ordtemp.clear();
				}
			}

			//if any are left:
			std::string szJson = FormatOrdersJson(ordtemp);
			if (szJson != "")
				json_ready_to_send.push_back(szJson);

			for (auto json :json_ready_to_send )
			{
				//send zmq via ReporterBase:
				LOGINFO("[OrderReporterV6] SENDING ORDER JSON: {}", json);
				int sent = zmq_send(socket, json.c_str(), json.size(), 0);
                if (sent == -1) {
                    LOGERROR("[OrderReporterV6] ZMQ REPORTER FAILED TO SEND: {}", json);
                }

				if (_SHOW)
					cout << "[OrderReporter DEBUG] OK res=" << sent << " json="  << json << endl;

				std::this_thread::sleep_for(std::chrono::microseconds(50));
			}

			ords.clear();
		}

		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	for(int i = 0; i < 10; i++)
		KT01_LOG_ERROR("OrderReporterV6","KILLING REPORTER BASE THREAD! EXITING APPLICATION!");
    
	std::exit(1);
}

std::string OrderReporterV6::FormatOrdersJson(std::vector<KTN::OrderPod> & ords)
{
	StringBuffer s;
	s.Clear();

	Writer<StringBuffer> writer(s);

	writer.StartObject();

	writer.Key("header");
	writer.String("orders");

	writer.Key("version");
	writer.String("V3");

	writer.Key("source");
	writer.String(_SRC.c_str());

	std::string hdr("orders");
	writer.String(hdr.c_str());

	writer.StartArray();

	int count = 0;
	for (auto ord : ords)
	{
		string transtime = GetTimeFormatted(ord.timestamp);

		//defaulting here 
		algometa meta = _PARAMS;

		if (ord.hasMeta)
		{
			if (_algometaMap.find(ord.orderReqId) != _algometaMap.end())
			{
				meta = _algometaMap[ord.orderReqId];
			}
			else
			{
				// if (OrderRecoveryService::instance().CheckMeta(ord.cmeid, meta) == false)
				// {
				// 	KT01_LOG_WARN("OrderReporterV6", "HASMETA=TRUE AND META NOT FOUND. PASS.");
				// 	KT01_LOG_WARN("OrderReporterV6", OrderUtils::Print(ord));
				// 	continue;
				// }
				
				KT01_LOG_ERROR("OrderReporterV6", "HASMETA=TRUE AND META NOT FOUND. PASS.");
				KT01_LOG_ERROR("OrderReporterV6", OrderUtils::Print(ord));
				continue;
			}
		}

		//fyi- this is here for algos and making it quicker to format messages. we don't really need.
		UserInfo ugo = meta.userinfo;
		if (meta.userinfo.org == "")
		{
			if(_ugoMap.find(ord.orderReqId) != _ugoMap.end())
			{
				ugo = _ugoMap[ord.orderReqId];
			}
			else
			{
				ugo.user = "MISSING";
				ugo.org = _ORG;
				ugo.groupname = _ORG;
			}
		}

		string tag = meta.Tag;

		if (_tagmap.find(ord.orderReqId) != _tagmap.end())
		{
			tag = _tagmap[ord.orderReqId];
		}

		count ++;

		writer.StartObject();

			writer.Key("user");
			writer.String(ugo.user.c_str());

			writer.Key("org");
			writer.String(ugo.org.c_str());

			writer.Key("groupname");
			writer.String(ugo.groupname.c_str());

			writer.Key("algoid");
			writer.String(meta.UniqueId.c_str());

			string action = LegacyAction(ord);
			writer.Key("action");
			writer.String(action.c_str());

			string exchid = OrderUtils::ConvertCmeId(ord.exchordid);
			writer.Key("exchordid");
			writer.String(exchid.c_str());

			writer.Key("strategy");
			writer.String(meta.Strategy.c_str());

			writer.Key("stratid");
			writer.Int(ord.stratid);

			writer.Key("ordernum");
			writer.String(ord.ordernum);

			writer.Key("origclordid");
			if ((KOrderExchange::isEquityVenue(ord.OrdExch) || ord.OrdExch == KOrderExchange::OSE) && strlen(ord.origordernum) > 0)
				writer.String(ord.origordernum);
			else
				writer.String(ord.ordernum);

			writer.Key("exchange");
			writer.String(KOrderExchange::toString(ord.OrdExch).c_str());			

			std::string product, symbol;
			KOrderProdType::Enum prodtype;
			switch (ord.OrdExch)
			{
				case KOrderExchange::CME:
				{
					const auto& def = CmeSecMaster::instance().getSecDef(ord.secid);
					product = def.product;
					symbol = def.symbol;
					prodtype = def.prodtype;
					break;
				}
				case KOrderExchange::CFE:
				{
					const auto& def = CfeSecMaster::instance().getSecDef(ord.secid);
					product = def.product;
					symbol = def.symbol;
					prodtype = def.prodtype;
					break;
				}
				case KOrderExchange::INET:
				case KOrderExchange::ARCP:
				case KOrderExchange::BYXB:
				case KOrderExchange::BZXB:
				case KOrderExchange::EDGAB:
				case KOrderExchange::EDGAX:
				{
					EquitySecDef def = EquitySecMaster::instance().getSecDef(ord.secid);
					product = def.productCode;
					symbol = def.symbol;
					prodtype = def.prodtype;
					break;
				}
				case KOrderExchange::OSE:
				{
					auto def = KT01::SECDEF::OSE::OseSecMaster::instance().getSecDef(static_cast<uint32_t>(ord.secid));
					product = def.product;
					symbol = def.symbol;
					prodtype = def.prodtype;
					break;
				}
				default:
					product = "UNKNOWN";
					symbol = "UNKNOWN";
					prodtype = KOrderProdType::FUTURE;
					break;
			}

			writer.Key("product");
			writer.String(product.c_str());

			writer.Key("symbol");
			writer.String(symbol.c_str());

			writer.Key("prodtype");
			writer.String(KOrderProdType::toString(prodtype).c_str());

			writer.Key("secid");
			writer.Int(ord.secid);

			

			writer.Key("side");
			writer.String(KOrderSide::toString(ord.OrdSide).c_str());

			writer.Key("price");
			writer.Double(ord.price);

			writer.Key("quantity");
			writer.Int(ord.quantity);

			writer.Key("leavesqty");
			writer.Int(ord.leavesqty);

			writer.Key("fillqty");
			writer.Int(ord.fillqty);

			writer.Key("lastqty");
			writer.Int(ord.lastqty);

			writer.Key("lastpx");
			writer.Double(ord.lastpx);

			writer.Key("maxshow");
			writer.Int(ord.dispqty);

			writer.Key("status");
			writer.String(KOrderStatus::toString(ord.OrdStatus).c_str());

			string ordertag = OrderTag(ord);
			writer.Key("tag");
			writer.String(ordertag.c_str());

			writer.Key("tag2");
			writer.String(meta.Tag.c_str());

			writer.Key("source");
			writer.String(_SRC.c_str());

			writer.Key("execid");
			writer.String(ord.execid);

			string text = RejectText(ord);
			writer.Key("text");
			writer.String(text.c_str());

			writer.Key("rpt");
			writer.Int(0);

			string state = LegacyState(ord);
			writer.Key("state");
			writer.String(state.c_str());

			writer.Key("ssboe");
			writer.Int64(ord.timestamp);

			writer.Key("iRptType");
			writer.Int((int)ord.OrdAction);

			writer.Key("filltype");
			writer.Int((int)ord.OrdFillType);

			writer.Key("reporttype");
			string rpttype = LegacyRptType(ord);
			writer.String(rpttype.c_str());

			string blank = "";
			writer.Key("reference");
			writer.String(blank.c_str());

			writer.Key("stoppx");
			if (ord.OrdType == KOrderType::STOPLIMIT || ord.OrdType == KOrderType::STOP)
				writer.Double(ord.stopprice);
			else
				writer.Double(0);

			writer.Key("ordtype");
			string ordtype = KOrderType::toString(ord.OrdType);
			writer.String(ordtype.c_str());

			writer.Key("tif");
			string ordtif = KOrderTif::toString(ord.OrdTif);
			writer.String(ordtif.c_str());


			writer.Key("KOrdSide");
			writer.String(KOrderSide::toString(ord.OrdSide).c_str());

			writer.Key("KOrdType");
			writer.String(KOrderType::toString(ord.OrdType).c_str());

			writer.Key("KOrdState");
			writer.String(KOrderState::toString(ord.OrdState).c_str());

			writer.Key("KOrdExch");
			writer.String(KOrderExchange::toString(ord.OrdExch).c_str());

			writer.Key("KOrdProdType");
			writer.String(KOrderProdType::toString(prodtype).c_str());

			writer.Key("KOrdFillType");
			writer.String(KOrderFillType::toString(ord.OrdFillType).c_str());

			writer.Key("KOrdStatus");
			writer.String(KOrderStatus::toString(ord.OrdStatus).c_str());

			writer.Key("KOrdTif");
			writer.String(KOrderTif::toString(ord.OrdTif).c_str());

			writer.Key("KOrdAction");
			writer.String(KOrderAction::toString(ord.OrdAction).c_str());

			writer.Key("KOrdPutCall");
			string na = "NA";
			writer.String(na.c_str());

			writer.Key("KOrdAlgoTrigger");
			writer.String(KOrderAlgoTrigger::toString(ord.OrdAlgoTrig).c_str());

			writer.Key("openclose");
			writer.String(KOrderOpenCloseInd::toString(ord.OrdOpenClose).c_str());

			writer.Key("tradedate");
			writer.Uint(ord.tradeDate);

			writer.Key("timestamp");
			writer.Int64(ord.timestamp);

			writer.Key("transacttime");
			writer.String(transtime.c_str());


		writer.EndObject();

	}

	writer.EndArray();
	writer.EndObject();

	//string szPrefix = "JSAR";
	std::string res =  s.GetString() ;

	if (count == 0)
	{
		KT01_LOG_WARN("OrderReporterV6", "NO ORDERS TO REPORT. PASS.");
		return "";
	}

	return res;
}

string OrderReporterV6::LegacyRptType(const KTN::OrderPod& ord)
{
	switch (ord.OrdStatus)
	{
		case KOrderStatus::NEW:
			return "NEWORDER";

		case KOrderStatus::CANCELED:
			return "CXLORDER";

		case KOrderStatus::MODIFIED:
			return "MODORDER";

		case KOrderStatus::PARTIALLY_FILLED:
		case KOrderStatus::FILLED:
			return "FILLREPORT";

		case KOrderStatus::REJECTED:
			return"NEWORDER";

		default:
			return "UNKOWN";
	}
}

string OrderReporterV6::LegacyAction(const KTN::OrderPod& ord)
{
	switch (ord.OrdStatus)
	{
		case KOrderStatus::NEW:
			return "ORDACK";

		case KOrderStatus::CANCELED:
			return "CXLORDER";

		case KOrderStatus::MODIFIED:
			return "ORDCXLREPL";

		case KOrderStatus::PARTIALLY_FILLED:
		case KOrderStatus::FILLED:
			return "ORDEXEC";

		case KOrderStatus::REJECTED:
			return"ORDREJCTED";

		default:
			return "UNKOWN";
	}
}

string OrderReporterV6::LegacyState(const KTN::OrderPod& ord)
{
	switch (ord.OrdState)
	{
		case KOrderState::WORKING:
		case KOrderState::INFLIGHT:
			return "ACTIVE";

		case KOrderState::COMPLETE:
			return "COMPLETE";

		default:
			return "COMPLETE";
	}
}

string OrderReporterV6::OrderTag(const KTN::OrderPod& ord)
{
	string ordertag = _PARAMS.Tag;

	switch(ord.OrdAlgoTrig)
	{
	case KOrderAlgoTrigger::ALGO_LEG:
			ordertag = _PARAMS.Tag;
			break;
	case KOrderAlgoTrigger::ALGO_HEDGE:
			ordertag = "HDG";
			break;
	default:
		ordertag = KOrderAlgoTrigger::toString(ord.OrdAlgoTrig);
	}

//
//
//	if (ord.OrdAlgoTrig == KOrderAlgoTrigger::ALGO_HEDGE)
//		ordertag = "HDG";
//	if (ord.OrdAlgoTrig == KOrderAlgoTrigger::ALGO_STOPLOSS)
//		ordertag = "ALGOSTP";
//	if (ord.OrdAlgoTrig == KOrderAlgoTrigger::ORDERBOOK)
//		ordertag = "ORDBOOK";
//	if (ord.OrdAlgoTrig == KOrderAlgoTrigger::ALGO_SQZ)

	return ordertag;
}

string OrderReporterV6::RejectText(const KTN::OrderPod& ord)
{
	string reason = "";
	if (ord.OrdAction == KOrderAction::ACTION_REJ)
		reason = ErrorCodes::getOrderRejectMessage(ord.rejreason);

	if (ord.OrdAction == KOrderAction::ACTION_CXL_REJ)
		reason = ErrorCodes::getCxlRejectMessage(ord.rejreason);

	if (ord.OrdAction == KOrderAction::ACTION_CXL_REPL_REJ)
		reason = ErrorCodes::getCxlReplaceRejectMessage(ord.rejreason);

	return reason;

}

string OrderReporterV6::GetTimeFormatted(uint64_t ssboe)
{
    // Convert microseconds to a duration
    std::chrono::microseconds us(ssboe);

    // Convert microseconds to time_point
    std::chrono::system_clock::time_point timePoint(std::chrono::duration_cast<std::chrono::system_clock::duration>(us));

    // Convert time_point to time in local time zone
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm tm;

    localtime_r(&time, &tm);

    // Format the time with microsecond resolution
    ostringstream otime;
    otime << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
          << "." << std::setw(6) << std::setfill('0') << ssboe % 1000000;

    string res = otime.str();
    return res;
}

