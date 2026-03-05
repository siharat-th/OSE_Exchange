	/*
 * Stacker.hpp
 *
 *  Created on: Jan 22, 2024
 *      Author: sgaer
 */


#include "Stacker.hpp"

namespace KTN {

Stacker::Stacker(AlgoInitializer p): AlgoBaseV3(p), _QuoteOrdPool(1000),_HdgPool(1000)
{
	_HANDLER_STARTED = false;
	//_cp;
	_cp.TotalExec = 0;
	bCancelAllSent = false;
	_USE_REPORTING_MSGS = true;
	_SHOW_QUOTES = false;
	_ENABLED = false;
	_QUOTEID = 0;
	_LASTQUOTEID = 0;
	_CNT = 0;
	_QuotingState = QuoteState::TimerTick;
	_alloCount = 0,
	_WATCH_TRADES = true;

	LogMe("INITING SLACK...", LogLevel::INFO);
	_SLACKER.InitAndRun();

	ChangeHedgeState(HedgeState::NoHedges);

	if (p.orders.size() > 0)
	{
		LogMe("[INFO] Stacker::Stacker() - ***Found " + std::to_string(p.orders.size()) + " LIVE orders to initialize with.");

		for (auto ord : p.orders)
		{
			ord.orderReqId =  OrderNums::instance().NextReqId();
			cout << "[Stacker DEBUG] Found order: " << ord.ordernum << " CMEID=" << ord.exchordid << " REQID=" << ord.orderReqId
			<< " SECID=" << ord.secid << " PRICE=" << ord.price << " QTY=" << ord.quantity << " LVS=" << ord.leavesqty << endl;
			_Ords.onSentOrderFast(ord);
			minOrderAck(ord);
		}
	}

}

Stacker::~Stacker()
{
	if (_STATUS_ACTIVE.load(std::memory_order_relaxed) == true) {
		 _STATUS_ACTIVE.store(false, std::memory_order_relaxed);
		_statusthread.join();
	}

	if (_STACK_ACTIVE.load(std::memory_order_relaxed) == true) {
		_STACK_ACTIVE.store(false, std::memory_order_relaxed);
		_stackthread.join();
	}
}

void Stacker::onLaunch(std::string json)
{
	_cp = StackerParams::parse(json);
	UpdateMetaParams(_cp.meta);

	LoadSpreadLegDefs(json);

	_statusthread = std::thread(&Stacker::StatusThread, this);
	_stackthread = std::thread(&Stacker::StackThread, this);

	_mdh->SetHandleTrades(true);

	onJson(json);
}

void Stacker::onJson(std::string json) {

	cout << "[" << __CLASS__ << " DEBUG] onJson: " << json << endl;
	
	Document d;

	if (d.Parse(json.c_str()).HasParseError()) {
		int e = (unsigned)d.GetErrorOffset();
		ostringstream oss;
		oss << "JSON OFFSET ERROR AT " << e ;
		LogMe(oss.str(),LogLevel::ERROR);
	}

	if (!d.IsArray() || d.Empty())
	{
		LogMe("ALGO JSON OBJECT MUST BE POPULATED AND AN ARRAY!!!!", LogLevel::ERROR);
		LogMe(json, LogLevel::ERROR);
		return;
	}

	const auto& doc = d[0];

	_cp.meta.Enabled = doc["Enabled"].GetInt();

	if (_cp.meta.Enabled == 0)
	{
		_ENABLED = false;
		Enable(false);
		return;
	}

	_cp.SendBids 	= doc["SendBids"].GetBool();
	_cp.SendAsks 	= doc["SendAsks"].GetBool();

	_cp.BidLevels 	= doc["BidLevels"].GetInt();
	_cp.AskLevels 	= doc["AskLevels"].GetInt();
	_cp.OffsetLevels = doc["OffsetLevels"].GetInt();

	_cp.AutoHedge = doc["AutoHedge"].GetBool();
	_cp.Edge = doc["Edge"].GetInt();

	_cp.LargeTradeQty 	= doc["LargeTradeQty"].GetInt();
	_cp.PullOnLargeTrade = doc["PullOnLargeTrade"].GetBool();

	_cp.TradeOnLargeQty = doc["TradeOnLargeQty"].GetBool();
	_cp.SkewThreshold = doc["SkewThreshold"].GetDouble();
	_cp.SkewLevels = doc["SkewLevels"].GetInt();

	_cp.SqueezeRatio = doc["SqueezeRatio"].GetDouble();
	_cp.SqueezeQty = doc["SqueezeQty"].GetDouble();

	_cp.EnablePuke =  doc["EnablePuke"].GetBool();
	_cp.PukeQty 	= doc["PukeQty"].GetInt();
	_cp.PukeRatio 	= doc["PukeRatio"].GetDouble();
	_cp.PukePctPriority = doc["PukePctPriority"].GetDouble();

	_cp.EnableRestack = doc["EnableRestack"].GetBool();
	_cp.RestackQty = doc["RestackQty"].GetInt();
	_cp.RestackTopLevel = doc["RestackTopLevel"].GetBool();

	_cp.TotalQty 	= doc["TotalQty"].GetDouble();
	_cp.OrderQty 	= doc["OrderQty"].GetDouble();

	_cp.LargeOrdQty = (doc.HasMember("LargeOrdQty")) ? doc["LargeOrdQty"].GetInt() : 5 * _cp.OrderQty;

	_cp.MaxOpenBuys = doc["MaxOpenBuys"].GetInt();
	_cp.MaxOpenSells = doc["MaxOpenSells"].GetInt();

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument& leg = _cp.Legs[i];
		leg.activeQuoting = true;
		leg.lastAsk = 0;
		leg.lastBid = 0;
		leg.lastSentSellPrice= 0;
		leg.lastSentBuyPrice = 0;
	}

	_cp.Reset = false;
	if (doc.HasMember("ResetPosition"))
		_cp.Reset = doc["ResetPosition"].GetBool();

	if (_cp.Reset)
	{
		LogMe("*** GOT RESET COMMAND!!! ***", LogLevel::WARN);
		ResetPosition();
		return;
	}

	UpdateMetaParams(_cp.meta);

	Enable(true);

}

void Stacker::LoadSpreadLegDefs(string json)
{
	_cp.LegCount = 0;

	for (auto ln : _LEGS)
	{
		if (ln.iscme)
			RegisterCmeSecId(ln);
		

		ln.callbackid = _EXCH_CALLBACK_ID;
		ln.lastBuyId = 0;
		ln.lastSellId = 0;
		ln.buyState = KOrderState::UNKNOWN;
		ln.sellState = KOrderState::UNKNOWN;
		ln.activeQuoting = true;
		ln.lastAsk = 0;
		ln.lastBid = 0;
		ln.lastSentSellPrice= 0;
		ln.lastSentBuyPrice = 0;
		ln.index = _cp.LegCount;
		ln.tif = KOrderTif::GTC;

		_quoter[_cp.LegCount].Construct(_EXCH_CALLBACK_ID, ln, _PARAMS);

		ConstructHedge(ln);

		Order ord = _quoter[_cp.LegCount].GetTemplate();
		_Ords.RegisterOrderTemplate(ord);

		_cp.Legs[ln.index] = ln;
		_cp.LegSecIdMap[ln.secid] = ln;

		_msgcnt[_cp.LegCount] = {};
		_cp.LegCount++;


		ostringstream lll;
		lll << "ADDED LEG " << ln.index <<" SYMBOL: " << ln.symbol
			<<	"PRODTYPE=" << ln.prodtype << " SECID=" << ln.secid << " MINTICK=" << ln.mintick
			<< " LEGS=" << _cp.LegCount;
		LogMe(lll.str(),LogLevel::INFO);

	}

	_NUMLEGS = _cp.LegCount;
}

//Construct a blank hedge object
void Stacker::ConstructHedge(instrument& inst)
{
	SpreadHedgeV3 hdg;
	int legindex = inst.index;

	hdg.hdgtype = HedgeType::COMPLEX;
	hdg.edge = _cp.Edge; //in ticks i.e. 1 for crude, 25 for ES...

	hdg.instindex = legindex;
	hdg.secid = _cp.Legs[legindex].secid;
	hdg.mktsegid = _cp.Legs[legindex].mktsegid;
	hdg.spreadmult = _cp.Legs[legindex].spreadmult;
	hdg.ratio = 1; //stacker ratio always 1. for now

	_hdg[legindex] = hdg;
}

void Stacker::OrderReceived(KTN::Order& incoming, int idx)
{
	LogMe("INCOMING ORD:" +  OrderUtils::Print(incoming), LogLevel::WARN);

	if (idx == -1)
	{
		for (int i = 0; i < _cp.LegCount; i++)
		{
			if (incoming.symbol == _cp.Legs[i].symbol)
			{
				idx = 1;
				break;
			}
		}
	}

	string action = incoming.action;

#ifdef DEBUG_TESTING
	LogMe("ON ORDER RECEIVED: " + OrderUtils::Print(incoming), LogLevel::WARNING);
#endif

	if (action == "NEW")
	{
		LogMe("ERROR: MULTISPREADER DOES NOT ACCEPT NEW ORDERS FROM OUTSIDE ALGO\nCANCEL IS THE ONLY ACTIONS PERMITTED", LogLevel::ERROR);
		return;
	}

	if (action == "MOD")
	{
		KTN::OrderPod ord = {};
		//_Ords.GetByExchId(ord, incoming.exchordid);

		if (!_Ords.GetByExchId(ord, incoming.exchordid))
		{
			LogMe("ORDER WITH CMEID=" + std::to_string(incoming.exchordid) +
					" NOT FOUND IN ALGO BOOK.",LogLevel::ERROR);
			return;
		}

//		ord.orderReqId++;
		ord.stratid = _PARAMS.StratId;

		string ordid = _quoter[ord.instindex].NextId();
		OrderUtils::fastCopy(ord.ordernum, ordid.c_str(), sizeof(ord.ordernum));

		ord.quantity = incoming.quantity;
		ord.price = incoming.price;
		ord.callbackid = _EXCH_CALLBACK_ID;
		ord.OrdAction = KOrderAction::ACTION_MOD;
		ord.OrdStatus = KOrderStatus::sent_mod;
		ord.OrdState = KOrderState::INFLIGHT;
		ord.OrdExch = KOrderExchange::CME;

		_Ords.onSentOrderMod(ord);
		_ordersender->Send(ord, KOrderAction::ACTION_MOD);
	}

	if (action == "CXL")
	{
		KTN::OrderPod ord = {};
		if (!_Ords.GetByExchId(ord, incoming.exchordid))
		{
			LogMe("ORDER WITH CMEID=" + std::to_string(incoming.exchordid) +
					" NOT FOUND IN ALGO BOOK.",LogLevel::ERROR);
			return;
		}

		//ord.orderReqId++;
		ord.stratid = _PARAMS.StratId;

		string ordid = _quoter[ord.instindex].NextId();
		OrderUtils::fastCopy(ord.ordernum, ordid.c_str(), sizeof(ord.ordernum));

		ord.callbackid = _EXCH_CALLBACK_ID;
		ord.OrdAction = KOrderAction::ACTION_CXL;
		ord.OrdStatus = KOrderStatus::sent_cxl;
		ord.OrdState = KOrderState::INFLIGHT;
		ord.OrdExch = KOrderExchange::CME;

		_Ords.onSentOrderCxl(ord);
		_ordersender->Send(ord, KOrderAction::ACTION_CXL);
	}


	ChangeQuoteState(QuoteState::SentOrders);
}

vector<string> Stacker::Symbols()
{
	vector<string> symbs;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument leg = _cp.Legs[i];
		symbs.push_back(leg.symbol);
	}

	return symbs;
}

void Stacker::Print()
{
	LogMe("****" + m_Name + " INFO ****", LogLevel::WARN);

	_Ords.Print();

	ostringstream oss;
	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument& leg = _cp.Legs[i];
		DepthBook md = GetQuote(leg);

		int secid = leg.secid;
		int bq = _Ords.GetSideExposure(secid, KOrderSide::BUY, KOrderAlgoTrigger::ALGO_LEG);
		int sq = _Ords.GetSideExposure(secid, KOrderSide::SELL, KOrderAlgoTrigger::ALGO_LEG);

		oss << leg.symbol << ": TOT=" << _cp.TotalExec <<" OPEN B=" << bq << " S=" << sq;

		LogMe(oss.str());
	}

	PrintMdMap();

	PrintBookMap();
}

void Stacker::StatusThread()
{
	int secs = 2 * 1000 * 1000;
	_STATUS_ACTIVE.store(true, std::memory_order_relaxed);
	while(_STATUS_ACTIVE.load(std::memory_order_relaxed) == true)
	{
		ostringstream oss;
		for (int i = 0; i < _cp.LegCount; i++)
		{
			instrument& leg = _cp.Legs[i];
			DepthBook md = GetQuote(leg);

			int bq = _Ords.GetSideExposure(leg.secid, KOrderSide::BUY, KOrderAlgoTrigger::ALGO_LEG);
			int sq = _Ords.GetSideExposure(leg.secid, KOrderSide::SELL, KOrderAlgoTrigger::ALGO_LEG);

			oss << leg.symbol << ": TOT=" << _cp.TotalExec <<" OPEN B=" << bq << " S=" << sq;

			if (i < _cp.LegCount -1)
				oss << "; ";
		}
		AddReportingMsg(oss.str(), "INFO");
		usleep(secs);
	}
}

void Stacker::ReportStatus()
{
	ostringstream oss;
	for (int i = 0; i < _cp.LegCount; i++)
	{

		//first, let's get the quote- it's the cheapest thing to do
		instrument& leg = _cp.Legs[i];

		//KOrderSide::Enum side = (leg.spreadmult > 0) ? KOrderSide::BUY : KOrderSide::SELL;
		DepthBook md = GetQuote(leg);

		oss << leg.symbol << ": TOT=" << _cp.TotalExec << " SENT=" << _cp.TotalSent;

		if (i < _cp.LegCount -1)
			oss << "; ";
	}

	AddReportingMsg(oss.str(), "INFO");
}

void Stacker::AlertCheck()
{

}

void Stacker::ResetPosition()
{
	LogMe("RESETTING POSITION", LogLevel::INFO);
	//CancelAllOrders();
	_cp.TotalExec = 0;
	_cp.NetPosition = 0;
	_NETPOS = 0;
	_TOTALEXEC = 0;

	_cp.Reset = false;
	_cp.meta.Enabled = false;
	_cp.TotalSent = 0;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument& leg = _cp.Legs[i];
		leg.position = 0;
		leg.effective = 0;
		leg.lastAsk = 0;
		leg.lastBid = 0;
		leg.lastSentSellPrice = 0;
		leg.lastSentBuyPrice = 0;
		leg.totalexec = 0;
	}

	Enable(false);
	_Ords.ClearInflight();
}

void Stacker::Restart()
{
}

void Stacker::Enable(bool bEnable)
{
	_ENABLED = bEnable;
	_PARAMS.Enabled = _ENABLED;

	if (_ENABLED)
	{
		ReportStatus();

		bCancelAllSent = false;

		for (int i = 0; i < _cp.LegCount; i++)
		{
			instrument& leg = _cp.Legs[i];
			//LogMe("RESETTING SLOP VARIABLES", LogLevel::WARNING);
			leg.lastAsk = 0;
			leg.lastBid = 0;
			leg.lastSentSellPrice = 0;
			leg.lastSentBuyPrice = 0;
		}

		_Ords.ClearInflight();
		_Ords.ClearDeadOrders();

		LogMe("ENABLED!",LogLevel::INFO);
		//AddReportingMsg("ENABLED", "STATE");

		ChangeQuoteState(QuoteState::ReadyToSendOrders);
	}
	else
	{
		ChangeQuoteState(QuoteState::Disabled);
		_ENABLED = false;

		ClearLegSentIds();

		LogMe("GOT DISABLE COMMAND", LogLevel::WARN);
		//AddReportingMsg("DISABLED", "STATE");

		ReportStatus();
	}
}

void Stacker::CancelAllOrders()
{
//	LogMe("CANCEL ALL DISABLED!!!! FOR TEST ONLY!!!", LogLevel::ERROR);
//	return;
	bCancelAllSent = true;

	vector<KTN::OrderPod> ords;
	//_Ords.GetByStratId(ords, _PARAMS.StratId);

	_Ords.GetByAlgoTrigger(ords, KOrderAlgoTrigger::ALGO_LEG);

	ostringstream oss;
	oss << "CXL ALL ALGO_LEGS RESULT=" << ords.size() << " STRATID=" << _PARAMS.StratId;
	LogMe(oss.str(),LogLevel::WARN);

	int icnt = 0;
	for (auto ord : ords)
	{
		//KTN::Order cxl = _quoter.QuoteOrderCancel(ord, "CXL");

		if (ord.OrdAlgoTrig != KOrderAlgoTrigger::ALGO_LEG)
			continue;

		KTN::OrderPod cxl = {};
		_quoter[ord.instindex].Get(cxl);

		cxl.orderReqId = ord.orderReqId;
		cxl.exchordid = ord.exchordid;
		cxl.callbackid = _EXCH_CALLBACK_ID;
		cxl.OrdAction = KOrderAction::ACTION_CXL;
		cxl.OrdStatus = KOrderStatus::sent_cxl;
		cxl.OrdState = KOrderState::INFLIGHT;
		cxl.OrdAlgoTrig = ord.OrdAlgoTrig;
		cxl.OrdExch = KOrderExchange::CME;
		cxl.OrdSide = ord.OrdSide;
		cxl.quantity = ord.quantity;
		cxl.secid = ord.secid;
		cxl.price = ord.price;
		cxl.leavesqty = ord.leavesqty;
		cxl.isManual = false;

		_Ords.onSentOrderCxl(cxl);
		icnt++;
		LogMe("SENDING POD CXL: " + OrderUtils::Print(cxl));
		_ordersender->Send(cxl, KOrderAction::ACTION_CXL);
	}
	LogMe("CXL ALL SENT " + std::to_string(icnt) + " CANCELS!!!", LogLevel::WARN);

}

void Stacker::CancelOrder(const KTN::OrderPod& ord)
{

	KTN::OrderPod cxl = {};
	_quoter[ord.instindex].Get(cxl);

	cxl.orderReqId = ord.orderReqId;
	cxl.exchordid = ord.exchordid;
	cxl.callbackid = _EXCH_CALLBACK_ID;
	cxl.OrdAction = KOrderAction::ACTION_CXL;
	cxl.OrdStatus = KOrderStatus::sent_cxl;
	cxl.OrdState = KOrderState::INFLIGHT;
	cxl.OrdAlgoTrig = ord.OrdAlgoTrig;
	cxl.OrdExch = KOrderExchange::CME;
	cxl.OrdSide = ord.OrdSide;
	cxl.quantity = ord.quantity;
	cxl.secid = ord.secid;
	cxl.price = ord.price;
	cxl.leavesqty = ord.leavesqty;

	_Ords.onSentOrderCxl(cxl);

	_ordersender->Send(cxl, KOrderAction::ACTION_CXL);

	LogMe("CXL ORD SENT " + KOrderAlgoTrigger::toString(cxl.OrdAlgoTrig) + " CANCEL!!!", LogLevel::WARN);

}


bool Stacker::SendStackOrders(int legindex, int price, KOrderSide::Enum side, bool largetrade)
{
//	if(!RiskCheckQty(_cp.OrderQty, _cp.TotalQty, _cp.TotalExec, side))
//		return false;

	if (_Ords.HasInflight())
	{
		//LogMe("ORDERS IN FLIGHT. NOT STACKING.");
		return false;
	}

	int secid = _cp.Legs[legindex].secid;

	if (!largetrade)
	{
		vector<KTN::OrderPod> ords;
		//NOTE: Especially for  
		if(_Ords.GetAtPrice(ords, secid, side, price, KOrderAlgoTrigger::ALGO_LEG))
			return false;
	}

	//--- RISK CHECK FOR STACKS IS EXPOSURE BASED:---
	
	int qty = _Ords.GetSideExposure(secid, side, KOrderAlgoTrigger::ALGO_LEG);
	int max = (side == KOrderSide::BUY) ? _cp.MaxOpenBuys : _cp.MaxOpenSells;

	cout << "[DEBUG] RISK CHECK: SIDE=" << KOrderSide::toString(side) << " QTY=" << qty << " MAX=" << max << endl;

	if (qty >= max){
		string msg = "DISABLED: ORD QTY > MAX: " + KOrderSide::toString(side) + " EXPOSURE=" + std::to_string(qty)
						+ " MAX=" + std::to_string(max);
		LogMe(msg,LogLevel::ERROR);
		AddReportingMsg(msg,"ALERT");
		Enable(false);
		return false;
	}
	//-----------------------------------------------

	instrument& leg = _cp.Legs[legindex];

	//int ordqty = (largetrade) ? _cp.LargeOrdQty : _cp.OrderQty;

	KTN::OrderPod* ord = _QuoteOrdPool.get();
	PrepareNewOrder(*ord, leg, side, price, _cp.OrderQty);
	ord->OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;
	ord->OrdState = KOrderState::INFLIGHT;

	SendOrder(*ord, leg);
	_cp.TotalSent += ord->quantity;

	if (!_cp.AutoHedge)
		return true;

	//buy order -> hedge = sell, so we ADD the edge to the price to
	//SELL higher; conversely sell -> hedge = buy, so we SUBTRACT
	int mult = (side == KOrderSide::BUY) ? 1 : -1;
	int edge = _cp.Edge * mult;

	SpreadHedgeV3 hdg;

	hdg.hdgtype = HedgeType::COMPLEX;
	hdg.edge = _cp.Edge; //in ticks i.e. 1 for crude, 25 for ES...
	hdg.OrdSide = (side == KOrderSide::BUY) ? KOrderSide::SELL : KOrderSide::BUY;
	hdg.price = price + edge;
	hdg.instindex = legindex;
	hdg.secid = _cp.Legs[legindex].secid;
	hdg.mktsegid = _cp.Legs[legindex].mktsegid;
	hdg.spreadmult = _cp.Legs[legindex].spreadmult;
	hdg.ratio = 1; //stacker ratio always 1. for now
	hdg.sqz = leg.squeeze;

	vector<SpreadHedgeV3> hdgPath;
	hdgPath.push_back(hdg);

	_ordersender->SendHedgeInstruction(ord->orderReqId, hdgPath);

	return true;
}

void Stacker::onCalculate()
{
}

void Stacker::onMktData(int index)
{
	CheckSqueeze(index);

	if (_cp.SendAsks)
		CheckPuke(index, false);
	if (_cp.SendBids)
		CheckPuke(index, true);

	//moved to worker thread
	//CheckStack(index);
}

void Stacker::onTrade(int index, double price, double size, int aggressor)
{
	if (!GoodToGo())return;

	if (aggressor == 0)return;

	if (_cp.PullOnLargeTrade && size >= _cp.LargeTradeQty)
	{
		instrument inst = _cp.Legs[index];
		KOrderSide::Enum side = (aggressor == 2) ? KOrderSide::BUY : KOrderSide::SELL;

		PukeAtPrice(inst.secid, price, side, KOrderAlgoTrigger::ALGO_LARGE_TRADE_KILL,0,0);

		LogMe("***LARGE TRADE: " + std::to_string(size) + " @ " + std::to_string(price) + " AGG=" + std::to_string(aggressor), LogLevel::WARN);

		//PukeAtPrice(inst.secid, price, side, KOrderAlgoTrigger::ALGO_LARGE_TRADE_KILL,0,0);

		if (_cp.TradeOnLargeQty)
		{
			double skew = CalcSkew(inst);

			if (_cp.SendBids && aggressor == 1 && skew > _cp.SkewThreshold)
			{
				if (SendStackOrders(index, price, KOrderSide::BUY, true))
					LogMe("SENT BUY ON LARGE TRADE: PRICE=" + std::to_string(price));
				else
					LogMe("(RISK)DID NOT SEND BUY ON LARGE TRADE: PRICE=" + std::to_string(price));
			}

			if (_cp.SendAsks && aggressor == 2 && skew < _cp.SkewThreshold)
			{
				if(SendStackOrders(index, price, KOrderSide::SELL, true))
					LogMe("SENT SELL ON LARGE TRADE: PRICE=" + std::to_string(price));
				else
					LogMe("(RISK)DID NOT SEND BUY ON LARGE TRADE: PRICE=" + std::to_string(price));
			}
		}
	}


}

void Stacker::StackThread()
{
	LogMe("STACK THREAD STARTING!!!",LogLevel::INFO);
	_STACK_ACTIVE.store(true, std::memory_order_relaxed);

	while (_STACK_ACTIVE.load(std::memory_order_relaxed) == true)
	{
		for(int i = 0; i < _cp.LegCount; i++)
		{
			CheckStack(i);
			//usleep(1000);
		}
		usleep(1000);
	}
}

void Stacker::CheckStack(int index)
{
	if (!GoodToGo())return;

	instrument& leg = _cp.Legs[index];

	//we use the top level quote b/c the book updates less frequently and incremental is faster
	//more accurate quote


//	bookmd book = {};
//	GetBook(leg, book);

	//note: let's not use the depth book from snapshots- it's slow and relatively useless for our purposes
	DepthBook md = {};
	GetQuoteByIndex(index, md);


	int bidlevels = 0;
	int asklevels = 0;
	for (int lvl = _cp.OffsetLevels; lvl < 10; lvl++)
	{

		//cout << "LVL=" << lvl << " " << " BSIZE=" << book.Bids[lvl].size << " RESQTY=" << _cp.RestackQty << endl;
		if (_cp.SendBids)
		{
			if (bidlevels >= _cp.BidLevels)
				continue;
			bidlevels ++;

			int price = (-1 * lvl * leg.mintick) + md.Bid;
//			int price = book.Bids[lvl].price;

			//this prevents endless fight between stack and puke
			if (lvl == 0)
			{

				if (md.BidSize <= _cp.PukeQty)
					continue;

				double ratio = md.BidSize / md.AskSize;
				if (ratio <= _cp.PukeRatio)
					continue;

				price = md.Bid;

			}

			SendStackOrders(index, price, KOrderSide::BUY, false);
		}

		if (_cp.SendAsks)
		{
			if (asklevels >= _cp.AskLevels)
				continue;
			asklevels++;

			int price = (lvl * leg.mintick) + md.Ask;
			//int price = book.Asks[lvl].price;

			if (lvl == 0)
			{
				DepthBook md = {};
				GetQuoteByIndex(index, md);

				if (md.AskSize <= _cp.PukeQty)
					continue;

				//if ratio is <= puke ratio, then we don't want to enter quote
				//opposite of CheckPuke boolean, so continue loop
				double ratio = md.AskSize / md.BidSize;
				if (ratio <= _cp.PukeRatio)
					continue;

				price = md.Ask;
			}

			SendStackOrders(index, price, KOrderSide::SELL, false);
		}
	}

}

void Stacker::CheckPuke()
{
	for (int i = 0; i < _cp.LegCount; i++){
		CheckPuke(i, true);
		CheckPuke(i, false);
	}
}

bool Stacker::CheckPuke(int index, bool isBid)
{
	if (!_GOT_DATA)
		return false;

	if (_cp.EnablePuke == false)
		return false;

	instrument& leg = _cp.Legs[index];
	DepthBook md = {};
	GetQuoteByIndex(index, md);

	KOrderSide::Enum side = (isBid) ? KOrderSide::BUY : KOrderSide::SELL;

	double ratio = (side == KOrderSide::BUY) ? (md.BidSize / (double)md.AskSize)
											 : (md.AskSize / (double)md.BidSize);
	int size = (side == KOrderSide::BUY) ? md.BidSize : md.AskSize;

	if (ratio >= _cp.PukeRatio && size >= _cp.PukeQty)
	    return false;

	int32_t price = (side == KOrderSide::BUY) ? md.Bid : md.Ask;

	return PukeAtPrice(leg.secid, price, side, KOrderAlgoTrigger::ALGO_PUKE,ratio, size);

}

bool Stacker::PukeAtPrice(int32_t secid, int price, KOrderSide::Enum side,
		KOrderAlgoTrigger::Enum trig, double ratio, int size)
{
	vector<KTN::OrderPod> ords;
	if(!_Ords.GetAtPrice(ords, secid, side, price, KOrderAlgoTrigger::ALGO_LEG)){
		//cout << "GOT " << ords.size() << " ORDS AT " << price << " FOR SIDE=" << KOrderSide::toString(side) << endl;
		return false;
	}

	//cout << "GOT " << ords.size() << " ORDS AT " << price << " FOR SIDE=" << KOrderSide::toString(side) << endl;

	for(auto& ord : ords)
	{
		if (Canceled(ord.exchordid))
			continue;
		_CxlOrds.push_back(ord.exchordid);

		ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_PUKE;
		CancelOrder(ord);
	}

	ostringstream oss;
	oss << KOrderAlgoTrigger::toString(KOrderAlgoTrigger::ALGO_LEG) << " PUKING AT "
			<< price << " ORDS=" << ords.size() << " RATIO=" << std::fixed
			<< std::setprecision(2) << ratio << " SIZE=" << size;
	LogMe(oss.str(),LogLevel::WARN);

	AddReportingMsg(oss.str(),"ALERT");

	return true;
}

void Stacker::CheckSqueeze()
{
	for (int i = 0; i < _cp.LegCount; i++)
	{
		if (_cp.SqueezeRatio == 0)
			continue;

		CheckSqueeze(i);
	}
}

void Stacker::CheckSqueeze(int index)
{
	if (_cp.SqueezeQty == 0) return;
	if (!GotMktData()) return;

	vector<KTN::OrderPod> ords;
	instrument& leg = _cp.Legs[index];
	if (!Working(ords, leg, KOrderAlgoTrigger::ALGO_HEDGE))
		return;

	DepthBook md = {};
	GetQuoteByIndex(index, md);

	int icnt = 0;
	for(auto& ord : ords)
	{
		if (Squeezed(ord.orderReqId))
			continue;

		double ratio = (ord.OrdSide == KOrderSide::BUY) ? (md.BidSize / (double)md.AskSize)
																: (md.AskSize / (double)md.BidSize);
		int mktsize = (ord.OrdSide == KOrderSide::BUY) ? md.AskSize : md.AskSize;

		if (ratio < _cp.SqueezeRatio && mktsize > _cp.SqueezeQty) return;
		int price = (ord.OrdSide == KOrderSide::BUY) ? md.Ask : md.Bid;

		//PrepareModOrder(ord, leg, side, price, ord.leavesqty);
		KTN::OrderPod neword = ord;
		//_quoter[leg.index].Get(neword);
		string newid = _quoter[leg.index].NextId();
		OrderUtils::fastCopy(neword.ordernum, newid.c_str(), sizeof(neword.ordernum));
		neword.quantity = ord.leavesqty;
		neword.orderReqId = ord.orderReqId;
		neword.price = price;
		neword.OrdAction = KOrderAction::ACTION_MOD;
		neword.OrdStatus = KOrderStatus::sent_mod;
		neword.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_SQZ;

		SendOrder(neword, leg);
		_SqueezedOrds.push_back(ord.orderReqId);
		icnt++;
	}

	if (icnt == 0)
		return;

	string logmsg = "SQUEEZE MODS=" + std::to_string(icnt)
						+ " BIDSZ=" + std::to_string(md.BidSize)
						+ " ASKSZ=" + std::to_string(md.AskSize);
	LogMe(logmsg, LogLevel::INFO);
	AddReportingMsg(logmsg,"ALERT");

}

double Stacker::CalcSkew(const instrument& leg)
{
	DepthBook book = GetBook(leg);

	int bidsize = 0;
	int asksize = 0;

	int levels = (_cp.SkewLevels == 0) ? 10 : _cp.SkewLevels;

	for (int i = 0; i < levels; i ++)
	{
		bidsize += book.Bids[i].size;
		asksize += book.Asks[i].size;
	}

	double skew =  std::log10(bidsize) - std::log10(asksize);

	return skew;
}

bool Stacker::Squeezed(uint64_t reqid)
{
	for(uint64_t id : _SqueezedOrds)
	{
		if (id == reqid)
			return true;
	}

	return false;
}

bool Stacker::Working(vector<KTN::OrderPod>& ords, instrument& leg, KOrderAlgoTrigger::Enum trig)
{
	//bool found = false;
	ords.clear();

	_Ords.GetByAlgoTriggerSecId(ords, trig, leg.secid);

	return (ords.size() > 0);
}



void Stacker::PrepareNewOrder(KTN::OrderPod &ord, const instrument& inst,
		KOrderSide::Enum side, int price, int qty)
{
	if (_MEASURE) _trk.startMeasurement("QuoteNextNew");
	_quoter[inst.index].NextNew(ord, side, 0, qty, price);
	if (_MEASURE) _trk.stopMeasurement("QuoteNextNew");
	_msgcnt[inst.index].news++;
}

void Stacker::PrepareModOrder(KTN::OrderPod &ord, const instrument& inst,
		KOrderSide::Enum side, int price, int qty)
{

	if (ord.OrdState != KOrderState::WORKING)
		return;

	if (price == ord.price){
		LogMe("MOD ORDER IS PRICE IS SAME AS ORIG. NOT MODIFYING!",LogLevel::ERROR);
		return;
	}

	if (_MEASURE) _trk.startMeasurement("QuoteNextMod");
		_quoter[inst.index].NextMod(ord, side, ord.orderReqId, ord.exchordid, qty, price);
	if (_MEASURE) _trk.stopMeasurement("QuoteNextMod");

	_msgcnt[inst.index].mods++;
}


void Stacker::ToggleCommand(int id)
{
	if (id == 94)
	{
		if (_MEASURE)
		{
			LogMe("DISABLING LATENCY MEASURING", LogLevel::WARN);
			_MEASURE = false;
		}
		else
		{
			LogMe("ENABLING LATENCY MEASURING", LogLevel::WARN);
			_MEASURE = true;
		}
	}

	if (id == 555)
	{
		LogMe("GOT CANCEL ALL ALGO ORDERS COMMAND!",LogLevel::INFO);
		CancelAllOrders();
	}

}

void Stacker::QuickOrdPrint(const KTN::Order& ord, const instrument& inst)
{
	DepthBook md = GetQuote(inst);
	ostringstream oss;
	oss << "**QUOTE " << inst.symbol << " SIDE=" << ord.OrdSide << " SYMB=" << ord.symbol << " PRICE=" << ord.price
		<< " QTY=" << ord.quantity << " ORDPX=" << ord.price << " QUOTE=" << md.Bid << "@" << md.Ask;

	LogMe(oss.str(), LogLevel::INFO);

//	vector<SpreadHedge> hdglist = _hedgemap[ord.ordernum];
//	ostringstream oyy;
//	oyy << ord.ordernum << ": " << KOrderSide::toString(ord.OrdSide) << " " << ord.symbol << " @ " << ord.price;
//	LogMe("     HEDGE LIST FOR " + oyy.str(), LogLevel::INFO);
//	for (auto hdg : hdglist)
//	{
//		ostringstream oss;
//		oss << "     " << hdg.inst.symbol << " " << hdg.side << " @ " << hdg.price << " prec=" << hdg.tickprecision;
//		LogMe(oss.str());
//	}
}



void Stacker::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent)
{
	if (ord.OrdFillType == KOrderFillType::SPREAD_LEG_FILL)
	{
		_Ords.minOrderExecSpreadLeg(ord);
		return;
	}

	_Ords.minOrderExecution(ord, hedgeSent);

	_cp.TotalSent -= ord.lastqty;
	if (_cp.TotalSent < 0)
		_cp.TotalSent = 0;

	ostringstream oss;
	oss << KOrderAlgoTrigger::toString(ord.OrdAlgoTrig) << " CmeID=" <<  ord.exchordid << " SECID=" << ord.secid << " SIDE="
			<< ord.OrdSide << " QTY=" << ord.lastqty << " PX=" << ord.lastpx;
	LogMe("**ALGO GOT FILL: " + oss.str(), LogLevel::INFO);
	AddReportingMsg(oss.str(),"FILL");

	double newqty = ord.lastqty;

	_cp.TotalExec += newqty;
	_TOTALEXEC += newqty;
	_cp.NetPosition += newqty;

	UpdateLegState(ord.orderReqId, ord.OrdState);
	//bool ok =  RiskCheckQty(ord.lastqty, _cp.TotalQty, _cp.TotalExec, ord.OrdSide);

	if (_cp.TotalExec >= _cp.TotalQty)
	{
		LogMe("DISABLING: QTY EXEC=" + std::to_string(_cp.TotalExec)
				+ " >= TOTAL ALLOWED:" + std::to_string(_cp.TotalQty),LogLevel::ERROR);
		Enable(false);
	}
}

void Stacker::minOrderCancel(KTN::OrderPod& ord)
{
	OrderMinUpdate(ord);
	_Ords.minOrderCancel(ord);
	_bQuoteInFlight = _Ords.HasInflight();
	if(!_bQuoteInFlight)
		ChangeQuoteState(QuoteState::ReadyToSendOrders);

	if (ord.OrdAlgoTrig != KOrderAlgoTrigger::ALGO_LEG)
		return;

	_cp.TotalSent -= _cp.OrderQty;
	if (_cp.TotalSent < 0)
		_cp.TotalSent = 0;

	LogMe("OnCxl- TotalSent=" + std::to_string(_cp.TotalSent));
}

void Stacker::minOrderCancelReject(KTN::OrderPod& ord, const string& text)
{
	OrderMinUpdate(ord);
	_Ords.minOrderReject(ord, text);
	LogMe("CXL REJECT RECEIVED: " + text,LogLevel::ERROR);
	AddReportingMsg("CXL REJECT RECEIVED: " + text, "ALERT");

	if (ord.OrdAlgoTrig != KOrderAlgoTrigger::ALGO_LEG)
			return;

	_cp.TotalSent -= _cp.OrderQty;
	if (_cp.TotalSent < 0)
		_cp.TotalSent = 0;

	LogMe("OnRej- TotalSent=" + std::to_string(_cp.TotalSent));
}

void Stacker::minOrderCancelReplaceReject(KTN::OrderPod& ord, const string& text)
{
	OrderMinUpdate(ord);
	_Ords.minOrderReject(ord, text);
	LogMe("CXL REPLACE REJECT RECEIVED: " + text,LogLevel::ERROR);
	AddReportingMsg("CXL REPLACE REJECT RECEIVED: " + text, "ALERT");

}

void Stacker::onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid,KOrderSide::Enum side,
					uint32_t qty, int32_t price, uint16_t instindex)
{
	/*
	 * Here, we got notified by an exchange handler that it has hedged a fill.
	 * We need to generate an OrderPod new order that corresponds
	 * and send it to the OrderManager.
	 */

	//just in case, we can do a lookup here; not really fast path anymore
	int index = _cp.LegSecIdMap[secid].index;

	ChangeHedgeState(HedgeState::ActiveHedges);

	ostringstream oss;
	oss << "ALGO onHdg: " << KOrderSide::toString(side)
							<< " " <<  qty << " " <<  _cp.LegSecIdMap[secid].symbol
							<< " @ " << price //<< " index=" << index
							//<< " secid=" << secid << " clordid=" << clordid
							<< " reqid=" << reqid;
	LogMe(oss.str(),LogLevel::INFO);

	OrderPod ord = {};
	_quoter[index].NextNew(ord, side, 0, qty, price);

	ord.orderReqId = reqid;
	ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_HEDGE;
	OrderUtils::fastCopy(ord.ordernum, clordid, sizeof(ord.ordernum));

	//LogMe("SENDING HDG TO V5: " + OrderUtils::Print(ord));

	_Ords.onSentOrderFast(ord);

}

void Stacker::onOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
				uint32_t qty, int32_t price, KOrderAlgoTrigger::Enum trig)
{
	int instindex = _cp.LegSecIdMap[secid].index;

	ostringstream oss;
	oss << "ALGO GOT " + KOrderAlgoTrigger::toString(trig)
			+ " ORDER SENT: " << KOrderSide::toString(side)
							<< " " <<  qty << " " <<  _cp.LegSecIdMap[secid].symbol
							<< " @ " << price << " index=" << instindex
							<< " secid=" << secid << " clordid=" << clordid << " reqid=" << reqid;
	LogMe(oss.str(),LogLevel::INFO);

	OrderPod ord = {};
	_quoter[instindex].NextNew(ord, side, 0, qty, price);

	ord.orderReqId = reqid;
	ord.OrdAlgoTrig = trig;
	OrderUtils::fastCopy(ord.ordernum, clordid, sizeof(ord.ordernum));

	LogMe("SENDING " + KOrderAlgoTrigger::toString(ord.OrdAlgoTrig) + " TO V5: " + OrderUtils::Print(ord));

	_Ords.onSentOrderFast(ord);
}


void Stacker::OrderMinUpdate(KTN::OrderPod& ord)
{
	UpdateLegState(ord.orderReqId, ord.OrdState);
}

void Stacker::UpdatePositionOnFill(const KTN::Order& ord)
{
	_cp.NetPosition = 0;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument& leg = _cp.Legs[i];
		if (strcmp(ord.symbol, leg.symbol.c_str()) == 0)
		{
			leg.totalexec += ord.lastqty;
			leg.position += ord.OrdSide * ord.lastqty;
			leg.effective = leg.position * 1/leg.spreadmult;
		}
		_cp.NetPosition += leg.effective;
	}

	_NETPOS = _cp.NetPosition;

	//if we are above max pos, we will shut off; the above to relieve max exposure setting if we get a fill that does so.
	AlertCheck();

}

bool Stacker::GotMktData()
{
	if (LIKELY(_GOT_DATA)) return true;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument leg = _cp.Legs[i];
		DepthBook md = GetQuote(leg);
		if ((md.Bid == 0 && md.Ask == 0) || (md.Bid > md.Ask) || md.BidSize == 0 || md.AskSize == 0)
		{
			_DATA_TRIES ++;
			if (_DATA_TRIES < 10 || _DATA_TRIES % 1000 == 0)
			{
				ostringstream oss;
				oss <<_DATA_TRIES << ": DATA ERROR " << leg.symbol << ": " << md.Bid << "@" <<  md.Ask << " " << md.BidSize << " x " <<  md.AskSize;
				LogMe(oss.str(), LogLevel::WARN);
				AddReportingMsg(oss.str(),"ALERT");
			}
			_GOT_DATA = false;

			return false;
		}
	}
	_GOT_DATA = true;
	LogMe("GOT DATA FOR ALL LEGS! GOOD TO GO.", LogLevel::INFO);

	AddReportingMsg("GOT DATA FOR ALL LEGS! GOOD TO GO.", "ALERT");

	return true;
}

bool Stacker::GoodToGo()
{
	if (!_ENABLED ) return false;

	if (!GotMktData()) return false;


	if (fabs(_cp.TotalExec) >= fabs(_cp.TotalQty))
	{
		if (_ENABLED) Enable(false);
		usleep(2 * 1000 * 1000);

		ostringstream oss;
		oss << "*** TOTAL EXECUTED EXCEEDED: " << _cp.TotalExec << " vs MAX:" << _cp.TotalQty ;
		LogMe(oss.str(), LogLevel::ERROR);

		usleep(100000);
		AddReportingMsg(oss.str(), "ALERT");

		return false;
	}


//	ChangeQuoteState(QuoteState::ReadyToSendOrders);
	_QuotingState = QuoteState::ReadyToSendOrders;

	return true;
}
} /* namespace KTN */
