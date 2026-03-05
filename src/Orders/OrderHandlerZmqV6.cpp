/*
 * OrderHandlerZmqV6.cpp
 *
 *  Created on: Apr 18, 2021
 *      Author: sgaer
 */

#include <Orders/OrderHandlerZmqV6.hpp>

namespace KTN{

OrderHandlerZmqV6::OrderHandlerZmqV6(IExchangeSender& exch)
		:_Ords(1, "OrdZmq"),_ordsender(exch)
{
	Settings settings("Settings.txt");

	_SOURCE = settings.getString("Source");
	_ACCOUNT = settings.getString("Account");
	_KEY = settings.getString("Key");
	
	_SHOW_MSGS = settings.getBoolean("Orders.ShowMsgs");
	_SESSION_ENABLED = settings.getBoolean("Services.Exchanges");

	//ZeroMQ Init
	initZmq(settings.getString("ArgoServer"), settings.getInteger("ArgoPort"));

	//Register the callback with the exchange sender
	_EXCH_CALLBACK_ID = _ordsender.RegisterCallback(this);

	//_REQ_ID = OrderNums::instance().NextReqId();
	//KT01_LOG_INFO(__CLASS__, "Initial reqId set to {}", _REQ_ID);

	algometa meta = {};
	meta.UniqueId = "ORDBOOK";
	meta.StratId = 0;
	meta.Strategy = "000";
	_Ords.UpdateMetaParams(meta);

	KT01_LOG_INFO(__CLASS__, "OK: Loaded OrderHandlerZmqV6 with SOURCE=" + _SOURCE + " GUID=" + _GUID + " CALLBACK=" + std::to_string(_EXCH_CALLBACK_ID));

	const tbb::concurrent_unordered_map<uint64_t, KTN::Order> &apiOrders = OrderRecoveryService::instance().GetApiOrders();
	for (tbb::concurrent_unordered_map<uint64_t, KTN::Order>::const_iterator it = apiOrders.begin(); it != apiOrders.end(); ++it)
	{
		KTN::Order ord = it->second;

		ord.orderReqId = OrderNums::instance().GenOrderReqId(ord.ordernum);

		if (ord.OrdAlgoTrig == KOrderAlgoTrigger::ORDERBOOK)
		{
			_clOrdIdMap[ord.ordernum] = ord.orderReqId;
			OrderPod pod = {};
			OrderUtils::Transpose(pod, ord, ord.ordernum);
			pod.callbackid = _EXCH_CALLBACK_ID;
			pod.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;
			UserInfo u = {};
			u.groupname = ord.groupname;
			u.user = ord.user;
			u.org = ord.org;
			_Ords.onSentOrderOrdbook(pod, u);
			_ugoMap[pod.orderReqId] = u;

			pod.OrdState = KOrderState::STATUS_UPDATE;
			if (ProcessOrderStatus(pod))
			{
				//_Ords.minOrderAck(pod);
				_Ords.minOrderAddInState(pod);
			}
			else
			{
				LOGWARN("[OrderHandlerZmqV6] Constructor: ProcessOrderStatus failed for order {}", pod.ordernum);
			}
		}
	}
}

OrderHandlerZmqV6::~OrderHandlerZmqV6()
{
	_Ords.Stop();
	_ACTIVE.store(false);
}

void OrderHandlerZmqV6::Stop()
{
	if (_ACTIVE.load(std::memory_order_relaxed) == true)
		_ACTIVE.store(false);
}

void OrderHandlerZmqV6::onHeartbeat(int msgseqnum){}
void OrderHandlerZmqV6::onSessionStatus(string exch, int mktsegid, int protocol,
		string desc, string state) {}


void OrderHandlerZmqV6::minOrderAck(KTN::OrderPod& ord)
{
	if (_SHOW_MSGS)
		cout << "[orderzmq DEBUG] minOrderAck ORDERNUM=" << ord.ordernum << " REQID=" << ord.orderReqId << " EXCHID=" << ord.exchordid
		<< " CLORDID=" << ord.ordernum << " SECID=" << ord.secid << " ORDTYPE=" << KOrderType::toString(ord.OrdType) << endl;

	if (ord.orderReqId == 0)
	{
		if (ord.exchordid > 0)
			ord.orderReqId = FindReqId(ord.exchordid);
		else
			ord.orderReqId = FindReqId(ord.ordernum);

		if (ord.orderReqId ==0)
		{
			KT01_LOG_ERROR(__CLASS__, "ORDER CXL: " + std::string(ord.ordernum) + " NOT FOUND IN ORDERS. ORDREQID=0");
			return;
		}
	}

	//checks to see if this is a status update or a retransmit and handles accordingly
	if (!ProcessOrderStatus(ord))
		return;
	
	_Ords.minOrderAck(ord);
}
void OrderHandlerZmqV6::minOrderCancel(KTN::OrderPod& ord)
{
	if (_SHOW_MSGS)
		cout << "[orderzmq DEBUG] minOrderCancel ORDERNUM=" << ord.ordernum << " REQID=" << ord.orderReqId << " EXCHID=" << ord.exchordid << endl;

	if (ord.orderReqId == 0)
	{
		if (ord.exchordid > 0)
			ord.orderReqId = FindReqId(ord.exchordid);
		else
			ord.orderReqId = FindReqId(ord.ordernum);

		if (ord.orderReqId ==0)
		{
			KT01_LOG_ERROR(__CLASS__, "ORDER CXL: " + std::string(ord.ordernum) + " NOT FOUND IN ORDERS. ORDREQID=0");
			return;
		}
	}

	//checks to see if this is a status update or a retransmit and handles accordingly
	if (!ProcessOrderStatus(ord))
	{
		if (_SHOW_MSGS)
			cout << "[orderzmq DEBUG] minOrderCancel: ProcessOrderStatus failed for order " << ord.ordernum << endl;
		return;
	}
		
	_Ords.minOrderCancel(ord);

	{
		std::lock_guard<std::mutex> lock(reqToPrevOrderMutex);
		reqIdToPrevOrderMap.erase(ord.orderReqId);
	}
}

void OrderHandlerZmqV6::minOrderModify(KTN::OrderPod& ord)
{
	if (ord.orderReqId == 0)
	{
		if (ord.exchordid > 0)
			ord.orderReqId = FindReqId(ord.exchordid);
		else
			ord.orderReqId = FindReqId(ord.ordernum);

		if (ord.orderReqId ==0)
		{
			KT01_LOG_ERROR(__CLASS__, "ORDER CXL: " + std::string(ord.ordernum) + " NOT FOUND IN ORDERS. ORDREQID=0");
			return;
		}
	}

	//checks to see if this is a status update or a retransmit and handles accordingly
	if (!ProcessOrderStatus(ord))
		return;

	_Ords.minOrderModify(ord);

	{
		std::lock_guard<std::mutex> lock(reqToPrevOrderMutex);
		reqIdToPrevOrderMap.erase(ord.orderReqId);
	}
}

//leaving execution to individual algos...
void OrderHandlerZmqV6::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent)
{
	if (ord.orderReqId == 0)
	{
		if (ord.exchordid > 0)
			ord.orderReqId = FindReqId(ord.exchordid);
		else
			ord.orderReqId = FindReqId(ord.ordernum);

		if (ord.orderReqId ==0)
		{
			KT01_LOG_ERROR(__CLASS__, "ORDER EXEC: " + std::string(ord.ordernum) + " NOT FOUND IN ORDERS");
			return;
		}
	}

	if (ord.OrdFillType == KOrderFillType::SPREAD_LEG_FILL)
	{
		cout << "[OrderZmq OK] SPREAD LEG FILL: " << std::string(ord.ordernum) << " REQID=" << ord.orderReqId
			<< " EXCHID=" << ord.exchordid << " SECID=" << ord.secid << endl;

		if (ord.exchordid == 0)
		{
			KTN::OrderPod temp = {};
			if (!_Ords.GetByOrdernum(temp, ord.ordernum))
			{
				ostringstream oss;
				oss << "NO ORDER " << ord.ordernum << " FOUND IN ORDMGR MAP. UNABLE TO MODIFY THIS ORDER.";
				KT01_LOG_ERROR(__CLASS__, oss.str());
			}

			ord.exchordid = temp.exchordid;
		}

		_Ords.minOrderExecSpreadLeg(ord);
		return;
	}

	//checks to see if this is a status update or a retransmit and handles accordingly
	if (!ProcessOrderStatus(ord))
		return;


	cout << "[orderzmq WARNING] EXEC: " << ord.ordernum << " REQID=" << ord.orderReqId << " EXCHID=" << ord.exchordid
		<< " CLORDID=" << ord.ordernum << " SECID=" << ord.secid << " LASTPX=" << ord.lastpx
		<< " LASTQTY=" << ord.lastqty << endl;

	_Ords.minOrderExecution(ord, hedgeSent);

	if (ord.leavesqty <= 0)
	{
		std::lock_guard<std::mutex> lock(reqToPrevOrderMutex);
		reqIdToPrevOrderMap.erase(ord.orderReqId);
	}
}

void OrderHandlerZmqV6::minOrderReject(KTN::OrderPod& ord, const string& text)
{
	if (ord.orderReqId == 0)
	{
		if (ord.exchordid > 0)
			ord.orderReqId = FindReqId(ord.exchordid);
		else
			ord.orderReqId = FindReqId(ord.ordernum);

		if (ord.orderReqId ==0)
		{
			KT01_LOG_ERROR(__CLASS__, "ORDER CXL: " + std::string(ord.ordernum) + " NOT FOUND IN ORDERS. ORDREQID=0");
			return;
		}
	}

	if (ord.OrdState == KOrderState::STATUS_UPDATE || ord.possRetransmit == true)
	{
		//check and see if we have this order's request id and meta info, otherwise, fill it in:
		if (!AddOrderFromStatusRequest(ord))
		{
			LogMe("ORDER REJECT: " + std::string(ord.ordernum) + " NOT FOUND IN ORDERS", LogLevel::ERROR);
			return;
		}
	}

	_Ords.minOrderReject(ord, text);
}

void OrderHandlerZmqV6::minOrderCancelReject(KTN::OrderPod& ord, const string& text)
{
	KTN::OrderPod prevOrd;
	{
		std::lock_guard<std::mutex> lock(reqToPrevOrderMutex);
		std::unordered_map<uint64_t, KTN::OrderPod>::iterator it = reqIdToPrevOrderMap.find(ord.orderReqId);
		if (it != reqIdToPrevOrderMap.end())
		{
			prevOrd = it->second;
			ord.OrdStatus = prevOrd.OrdStatus;
			reqIdToPrevOrderMap.erase(it);
		}
		else
		{
			LOGWARN("[OrderHandlerZmqV6] minOrderCancelReject: No previous order found for reqId {}", ord.orderReqId);
		}
	}

	_Ords.minOrderCancelReject(ord, text);
}

void OrderHandlerZmqV6::minOrderCancelReplaceReject(KTN::OrderPod& ord, const string& text)
{
	KTN::OrderPod prevOrd;
	{
		std::lock_guard<std::mutex> lock(reqToPrevOrderMutex);
		std::unordered_map<uint64_t, KTN::OrderPod>::iterator it = reqIdToPrevOrderMap.find(ord.orderReqId);
		if (it != reqIdToPrevOrderMap.end())
		{
			prevOrd = it->second;
			reqIdToPrevOrderMap.erase(it);
		}
		else
		{
			LOGWARN("[OrderHandlerZmqV6] minOrderCancelReplaceReject: No previous order found for reqId {}", ord.orderReqId);
			return;
		}
	}
	OrderUtils::fastCopy(prevOrd.ordernum, ord.ordernum, sizeof(prevOrd.ordernum));
	_Ords.minOrderCancelReplaceReject(prevOrd, text);
}

void OrderHandlerZmqV6::onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid,KOrderSide::Enum side,
			uint32_t qty, int32_t price, uint16_t instindex)
{
	LogMe("CALLBACK HEDGE ORDER SENT NOT IMPLMENTED",LogLevel::ERROR);
}

void OrderHandlerZmqV6::onOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
				uint32_t qty, int32_t price, KOrderAlgoTrigger::Enum trig)
{
	//Use this callback to add an order into the order book so we can process it later
}

void OrderHandlerZmqV6::RegisterOrderWithOrderbook(Order& ord)
{
	cout << "[orderzmq DEBUG] RegisterOrder ORDERNUM=" << ord.ordernum << " REQID=" << ord.orderReqId 
			<< " SECID=" << ord.secid << endl;

	OrderPod pod = {};
	OrderUtils::fastCopy(pod.ordernum, ord.ordernum, sizeof(pod.ordernum));
	pod.secid = ord.secid;
	pod.OrdSide = ord.OrdSide;
	pod.quantity = ord.quantity;
	pod.price = ord.price;
	pod.OrdAction = KOrderAction::ACTION_NEW;
	pod.OrdStatus = KOrderStatus::sent_new;
	pod.OrdExch = ord.OrdExch;
	pod.exchordid = ord.exchordid;
	pod.quantity = ord.quantity;
	pod.leavesqty = ord.leavesqty;
	pod.fillqty = ord.fillqty;
	pod.fillqty = ord.fillqty;
	pod.hasMeta = false;

	//these items we make sure are synced with the order book
	pod.callbackid = _EXCH_CALLBACK_ID;
	pod.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;
	pod.orderReqId = ord.orderReqId;

	if (!AddOrderFromStatusRequest(pod))
	{
		_exchIdMap[pod.exchordid] = pod.orderReqId;
		_reqIdMap[pod.orderReqId] = pod.ordernum;
		_clOrdIdMap[pod.ordernum] = pod.orderReqId;
		_tagmap[pod.orderReqId] = ord.tag;
		_algoidmap[pod.orderReqId] = ord.algoid;

		UserInfo u = {};
		u.groupname = ord.groupname;
		u.user = ord.user;
		u.org = ord.org;
		_ugoMap[pod.orderReqId] = u;

		_Ords.onSentOrderOrdbook(pod, _ugoMap[pod.orderReqId]);
	}
	
	if (_SHOW_MSGS)
		cout << "[orderzmq DEBUG] RegisterOrder: AddFromStatus succeeded for order " << pod.ordernum << endl;
	
}

void OrderHandlerZmqV6::LogMe(std::string szMsg, LogLevel::Enum loglevel , bool persist)
{
	string me = "OrdsZmqV6|" + _SOURCE;
	switch (loglevel)
	{
		case LogLevel::ERROR:
			KT01_LOG_ERROR(me, szMsg);
			break;
		case LogLevel::WARN:
			KT01_LOG_WARN(me, szMsg);
			break;
		case LogLevel::INFO:
			KT01_LOG_INFO(me, szMsg);
			break;
		case LogLevel::DEBUG:
			KT01_LOG_DEBUG(me, szMsg);
			break;
		default:
			KT01_LOG_INFO(me, szMsg);
	}
	
}

void OrderHandlerZmqV6::Print()
{
	LogMe("PRINTING ZMQ ORDER BOOK");
	_Ords.Print();
}

void OrderHandlerZmqV6::GetOrdersBySecId(std::vector<KTN::OrderPod> &ords, int32_t secId)
{
	_Ords.GetBySecId(ords, secId);
}

void OrderHandlerZmqV6::initZmq(std::string ArgoServer, int ArgoPort)
{
	_zmq = new ZmqSubscriberV5(this, ArgoServer, ArgoPort,
			"ordzmqV6", "Settings.txt","Orders.Topics");
}

void OrderHandlerZmqV6::onPubMsg(std::string szTopic, std::string szJson)
{
	//cout << "[OrderZmq DEBUG] onPubMsg: " << szTopic << " " << szJson << endl;
	if (szTopic.compare("ORDERS") == 0)
		onJson(szJson);
}

void OrderHandlerZmqV6::onJson(const std::string& json)
{
	//cout << "[OrderZmq DEBUG] onJson: " << json << endl;

	if (!ValidHeader(json))
		return;

	if (_SHOW_MSGS){
		LogMe("OrdHandV4 onJson:", LogLevel::WARN);
		cout << "[OrderZmq DEBUG] " << json << endl;
	}

	HandleOrders(json);
}

bool OrderHandlerZmqV6::ValidHeader(const std::string& json)
{
	JsonHeader hdr = JsonOrderDecoders::CheckJsonHeader(json);

	if (hdr.isEmpty)
	{
		LogMe("JSON HEADER EMPTY. CANNOT PROCESS. V2!.",LogLevel::ERROR, true);

		return false;
	}

	if (hdr.source.compare(_SOURCE) != 0)
	{
		//LogMe("SOURCE OF INCOMING [" + hdr.source + "] DOESN'T MATCH CURRENT SOURCE OF " + _SOURCE, LogLevel::WARNING);

		return false;
	}

	if (!hdr.hasUGO)
	{
		LogMe("ORDER MUST CONTAIN USER/GROUP/ORG", LogLevel::ERROR, true);
		LogMe(json, LogLevel::ERROR, true);
		return false;
	}

	return true;
}

void OrderHandlerZmqV6::HandleOrders(const string& json)
{
	std::vector<KTN::Order> ords = JsonOrderDecoders::DecodeOrder(json);

	//cout << "[OrderZmq WARNING] HandleOrders ORDERS SIZE=" << ords.size() << endl;

	for (auto ord : ords)
	{
		if (ord.source != _SOURCE)
			continue;

		UserInfo u = {};
		u.groupname = ord.groupname;
		u.user = ord.user;
		u.org = ord.org;

		if (ord.action == "CXLALL" || ord.action == "CXLALL_SYMB" || ord.action == "CXLALL_INSTRUCTIONS")
		{
			LogMe("HANDLING " + ord.action + " SYMBOL=" + ord.symbol);
			HandleCancelAll(ord);
			continue;
		}

		ord.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;

		if (ord.exchange == "CME" || ord.exchange == "cme")
			ord.OrdExch = KOrderExchange::CME;
		else if (ord.exchange == "CFE" || ord.exchange == "cfe")
			ord.OrdExch = KOrderExchange::CFE;
		else if (ord.exchange == "OSE" || ord.exchange == "ose")
			ord.OrdExch = KOrderExchange::OSE;
		else
		{
			EquitySecDef eqtDef = EquitySecMaster::instance().getSecDef(ord.symbol);
			if (eqtDef.secid > 0)
				ord.OrdExch = KOrderExchange::Value(eqtDef.exchangeSymbol);
			else
				ord.OrdExch = KOrderExchange::OTHER;
		}


		if (ord.OrdExch == KOrderExchange::CME)
		{
			CmeSecDef def = CmeSecMaster::instance().getSecDef(ord.symbol);
			ord.secid = def.secid;
			ord.product = def.product;
			ord.OrdProdType = def.prodtype;
			ord.OrdPutCall = KOrderPutCall::UNKNOWN;
			ord.mktsegid = def.msgw;
		}

		if (ord.OrdExch == KOrderExchange::CFE)
		{
			CfeSecDef def = CfeSecMaster::instance().getSecDef(ord.symbol);
			ord.secid = def.secid;
			ord.product = def.product;
			ord.OrdProdType = def.prodtype;
			ord.OrdPutCall = KOrderPutCall::UNKNOWN;
			ord.mktsegid = 1;

			cout << "[HandleOrders DEBUG] CFE ORDER: SYMBOL=" << ord.symbol << " SECID=" << ord.secid
					<< " PRODTYPE=" << KOrderProdType::toString(ord.OrdProdType) << " BASE64=" << def.b63_secid << endl;
		}

		if (ord.OrdExch == KOrderExchange::OSE)
		{
			OseSecDef def = OseSecMaster::instance().getSecDef(ord.symbol);
			ord.secid = def.orderbook_id;
			ord.product = def.product;
			ord.OrdProdType = def.prodtype;
			ord.OrdPutCall = KOrderPutCall::UNKNOWN;

			cout << "[HandleOrders DEBUG] OSE ORDER: SYMBOL=" << ord.symbol << " SECID=" << ord.secid
					<< " PRODTYPE=" << KOrderProdType::toString(ord.OrdProdType) << endl;
		}

		if (KOrderExchange::isEquityVenue(ord.OrdExch))
		{
			EquitySecDef def = EquitySecMaster::instance().getSecDef(ord.symbol);
			ord.secid = def.secid;
			ord.product = def.productCode;
			ord.OrdProdType = def.prodtype;
			ord.OrdPutCall = KOrderPutCall::UNKNOWN;
		}

		if (!ord.isManual)
		{
			LogMe("WARNING: AUTO ORDER RECEIVED FROM FRONT END!!!");
		}
		

		if (ord.action.compare("NEW") == 0)
		{
			HandleNewOrder(ord);
		}

		if (ord.action.compare("MOD") == 0)
			HandleModifyOrder(ord);

		if (ord.action.compare("CXL") == 0)
		{
			HandleCancelOrder(ord.ordernum, ord.user);
		}
	}
}

void OrderHandlerZmqV6::SendOrder(KTN::OrderPod& ord)
{
	if (_SHOW_MSGS){
		LogMe("ORDZMQ SENDING " + KOrderAction::toString(ord.OrdAction) + ":"
				+ OrderUtils::Print(ord));

		cout << "[OrderZmq] " << OrderUtils::Print(ord) << endl;
	}

	_Ords.onSentOrderOrdbook(ord, _ugoMap[ord.orderReqId]);

	if (_SESSION_ENABLED){
		_ordsender.Send(ord, ord.OrdAction);
	}
	else
	{
		LogMe("EXCHANGE SESSIONS NOT ENABLED", LogLevel::ERROR);
		LogMe(OrderUtils::Print(ord));
	}
}

void OrderHandlerZmqV6::SendCommand(Instruction& cmd)
{
//	LogMe("ORDZMQ SENDING " + KOrderAction::toString(ord.OrdAction) + ":"
//			+ OrderUtils::Print(ord));

	if (_SESSION_ENABLED){
		_ordsender.ExchCommand(cmd);
	}
	else
	{
		LogMe("EXCHANGE SESSIONS NOT ENABLED", LogLevel::ERROR);
	}
}

void OrderHandlerZmqV6::HandleNewOrder(const KTN::Order& ord)
{
	OrderPod pod = {};

	string cl = NextOrdId();
	OrderUtils::fastCopy(pod.ordernum, cl.c_str(), strlen(cl.c_str()));

	OrderUtils::Transpose(pod, ord, cl.c_str());

	pod.orderReqId = OrderNums::instance().GenOrderReqId(cl);

	//set this here... for cfe. ugh.
	//pod.origqty = ord.quantity;

	pod.stratid = 0;
	pod.leavesqty = ord.quantity;
	pod.fillqty = 0;
	pod.isManual = ord.isManual;
	pod.callbackid = _EXCH_CALLBACK_ID;

	pod.OrdStatus = KOrderStatus::sent_new;
	pod.OrdAction = KOrderAction::ACTION_NEW;

	strncpy(pod.text, ord.user.c_str(), sizeof(pod.text) - 1);

	//_ords[ord.orderReqId] = pod;
	_reqIdMap[pod.orderReqId] = pod.ordernum;
	_clOrdIdMap[pod.ordernum] = pod.orderReqId;
	_tagmap[pod.orderReqId] = ord.tag;

	UserInfo u = {};
	u.groupname = ord.groupname;
	u.user = ord.user;
	u.org = ord.org;
	_ugoMap[pod.orderReqId] = u;

	SendOrder(pod);
}

void OrderHandlerZmqV6::HandleModifyOrder(const KTN::Order& incoming)
{
	std::string ordernum = incoming.ordernum;
	//int dQty = incoming.leavesqty;
	//int dPrice = incoming.price;
	//int dMaxShow = 0.0;

	if (_clOrdIdMap.find(ordernum) == _clOrdIdMap.end())
	{
		char buff[255];
		sprintf(buff,"NO ORDER %s FOUND IN CLORDID->REQID MAP. UNABLE TO MODIFY THIS ORDER.", ordernum.c_str());
		LogMe(buff,LogLevel::ERROR, true);
		return;
	}

	uint64_t reqid = _clOrdIdMap[ordernum];
	//_clOrdIdMap.erase(ordernum);

	KTN::OrderPod ord = {};
	//_Ords.GetByOrdernum(ord, incoming.ordernum);

	if (!_Ords.GetByOrdernum(ord, incoming.ordernum))
	{
		ostringstream oss;
		oss << "NO ORDER " << ordernum << " FOUND IN ORDMGR MAP. UNABLE TO MODIFY THIS ORDER.";
		LogMe(oss.str(),LogLevel::ERROR, true);
	}

	KTN::OrderPod neword = ord;

	if (neword.fillqty > 0 && neword.OrdStatus == KOrderStatus::PARTIALLY_FILLED)
	{
		LogMe("NOTICE: YOU ARE ABOUT TO MODIFY A PARTIALLY FILLED ORDER. IFM RULES WILL APPLY IF ENABLED", LogLevel::WARN);
		if (_SHOW_MSGS)
			cout << "[OrderZmq DEBUF] NOTICE: YOU ARE ABOUT TO MODIFY A PARTIALLY FILLED ORDER. FILLQTY=" << neword.fillqty << " INCOMING (NEW) QTY=" << incoming.quantity << endl;
	}
	

	string newordid = NextOrdId();
	OrderUtils::fastCopy(neword.ordernum, newordid.c_str(), strlen(newordid.c_str()));
	//note- this gets ingnored for cme, cfe uses it, many other exchanges use it too.
	OrderUtils::fastCopy(neword.origordernum, ordernum.c_str(), strlen(ordernum.c_str()));

	neword.orderReqId = reqid;
	
	//this is really for CFE mods which are inane.
	//neword.origqty = ord.quantity;
	neword.fillqty = ord.fillqty;
	//neword.origlvs = ord.leavesqty;
	
	neword.callbackid = _EXCH_CALLBACK_ID;

	neword.price = incoming.price;

	neword.quantity = incoming.quantity;
	neword.leavesqty = incoming.leavesqty;  // TODO potential race condition causing leaves qty to be inaccurate if the modify gets rejeced with an in-flight fill?

	neword.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;
	neword.OrdOpenClose = ord.OrdOpenClose;

	neword.isManual = true;
	neword.stratid = 0;
	neword.stopprice = incoming.stoppx;

	neword.dispqty = incoming.dispqty;
	neword.minqty = incoming.minqty;

	neword.OrdStatus = KOrderStatus::sent_mod;
	neword.OrdAction = KOrderAction::ACTION_MOD;

	strncpy(neword.text, incoming.user.c_str(), sizeof(neword.text) - 1);

	//_ords[neword.orderReqId] = neword;
	_reqIdMap[neword.orderReqId] = neword.ordernum;
	_clOrdIdMap[neword.ordernum] = neword.orderReqId;
	_tagmap[neword.orderReqId] = incoming.tag;

	{
		std::lock_guard<std::mutex> lock(reqToPrevOrderMutex);
		reqIdToPrevOrderMap[neword.orderReqId] = ord;
	}

	SendOrder(neword);
}

void OrderHandlerZmqV6::HandleCancelOrder(const std::string &ordernum, const std::string &user)
{
	if (_clOrdIdMap.find(ordernum) == _clOrdIdMap.end())
	{
		ostringstream oss;
		oss << "NO ORDER " << ordernum << " FOUND IN CLORDID->REQID MAP. UNABLE TO CANCEL THIS ORDER.";
		LogMe(oss.str(),LogLevel::ERROR, true);
		return;
	}

	uint64_t reqid = _clOrdIdMap[ordernum];
	//_clOrdIdMap.erase(ordernum);


	//OrderPod ord = _ords[reqid];
	KTN::OrderPod ord = {};
	if (!_Ords.GetByOrdernum(ord, ordernum))
	{
		ostringstream oss;
		oss << "NO ORDER " << ordernum << " FOUND IN ORDMGR MAP. UNABLE TO CANCEL THIS ORDER.";
		LogMe(oss.str(),LogLevel::ERROR, true);
	}

	OrderPod neword = ord;

	string newordid = NextOrdId();
	OrderUtils::fastCopy(neword.ordernum, newordid.c_str(), strlen(newordid.c_str()));
	//note- this gets ingnored for cme, cfe uses it, many other exchanges use it too.
	OrderUtils::fastCopy(neword.origordernum, ordernum.c_str(), strlen(ordernum.c_str()));

	neword.callbackid = _EXCH_CALLBACK_ID;
	neword.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;
	neword.OrdOpenClose = ord.OrdOpenClose;
	neword.isManual = true;
	neword.stratid = 0;
	neword.OrdStatus = KOrderStatus::sent_cxl;
	neword.OrdAction = KOrderAction::ACTION_CXL;
	neword.orderReqId = reqid;

	strncpy(neword.text, user.c_str(), sizeof(neword.text) - 1);

	//_ords[neword.orderReqId] = neword;
	_reqIdMap[neword.orderReqId] = neword.ordernum;
	_clOrdIdMap[neword.ordernum] = neword.orderReqId;

	{
		std::lock_guard<std::mutex> lock(reqToPrevOrderMutex);
		reqIdToPrevOrderMap[neword.orderReqId] = ord;
	}

	SendOrder(neword);
}


void OrderHandlerZmqV6::HandleCancelAll(KTN::Order& ord)
{
	LogMe("RECEIVED MASS CANCEL: " + ord.action, LogLevel::INFO);
	Instruction cmd = {};

	if (ord.action == "CXLALL")
		cmd.command = ExchCmd::CXL_ALL_SEGS;

    if (ord.action == "CXLALL_SYMB")
	{
    	ostringstream oss;
    	oss << "MASS CANCEL SYMBOL=" << ord.symbol;
		LogMe(oss.str(), LogLevel::INFO);
		cmd.command = ExchCmd::CXL_ALL_SYMB;
		cmd.cmd_symbol = ord.symbol;
	}

	DBG_FILE_LINE
	if (ord.action == "CXLALL_INSTRUCTIONS")
	{
		DBG_FILE_LINE
		cmd.command = ExchCmd::CXL_ALL_INSTRUCTIONS;
		cmd.cmd_symbol = ord.symbol;
		cmd.cmd_text = ord.text;
	}

	DBG_FILE_LINE



	SendCommand(cmd);
}


// std::string OrderHandlerZmqV6::NextOrdId()
// {
// 	return OrderNums::instance().NextClOrdId();
// }


// void OrderHandlerZmqV6::LoadOrdersFromApi()
// {
// 	LogMe("[INFO] Loading orders from RestAPI.");
// 	auto ords = OrderApiFunctions::LoadOrdersFromApi(_SOURCE,"");
	
// 	for (auto ord : ords)
// 	{
// 		uint32_t reqid = OrderNums::instance().GenOrderReqId(ord.ordernum);
// 		ord.orderReqId = reqid;

// 		//I want to load all orders that are in the order book, even for algos in case they are not running
// 		// and we get a cancel/redirect
// 		_apiorders[ord.exchordid] = ord;
// 		_apiExchOrdMap[ord.ordernum] = ord.exchordid;

// 		// TODO: recheck again
// 		_clOrdIdMap[ord.ordernum] = ord.orderReqId;
// 		OrderPod pod = {};
// 		OrderUtils::Transpose(pod, ord, ord.ordernum);
// 		pod.callbackid = _EXCH_CALLBACK_ID;
// 		pod.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;
// 		UserInfo u = {};
// 		u.groupname = ord.groupname;
// 		u.user = ord.user;
// 		u.org = ord.org;
// 		//_Ords.onSentOrderOrdbook(pod, u);
// 		_ugoMap[pod.orderReqId] = u;
// 		_Ords.onSentOrderOrdbook(pod, _ugoMap[pod.orderReqId]);
// 	}
// 	//RegisterOrderWithOrderbook(ord);
	
// 	if (_SHOW_MSGS)
// 	{
// 		cout << OrderUtils::PrintOrdersTable(ords) << endl;
// 	}

// 	LogMe("[OrdZmq OK] RestApi Orders Loaded: " + std::to_string(_apiorders.size()));
// }



}//end namespace

