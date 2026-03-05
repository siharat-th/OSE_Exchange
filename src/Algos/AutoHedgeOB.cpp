/*
 * AutoHedgeOB.hpp
 *
 *  Created on: May 24, 2024
 *      Author: sgaer
 */

#include <Algos/AutoHedgeOB.hpp>

namespace KTN {

AutoHedgeOB::AutoHedgeOB(AlgoInitializer p)
		: AlgoBaseV3(p), _hdgpool(1000)
{
	_HANDLER_STARTED = false;
	_USE_REPORTING_MSGS = true;
	_ENABLED = false;
	_QuotingState = QuoteState::TimerTick;
	_GOT_DATA = false;
	_CNT = 0;
	_WATCH_TRADES = true;
	_mdh->SetHandleTrades(true);
}

AutoHedgeOB::~AutoHedgeOB() {

}

void AutoHedgeOB::onLaunch(std::string json) {
	//load json params to your custom param struct or whatevs
	_cp = AutoHedgeOB2Params::parse(json);
	UpdateMetaParams(_cp.meta);

	_mdh->SetHandleTrades(false);
	_CIRCUITBREAKER = false;
	_CAN_TRADE = true;

	//OB is always enabled....
	//for an OB algos, we send the json to onJson b/c we have an order
	_cp.meta.Enabled = 1;
	LogMe("INITIAL SQUEEZE- SENDING onJSON FROM onLaunch...", LogLevel::INFO);
	onJson(json);

}

void AutoHedgeOB::onJson(std::string json) {
	_ENABLED = true;

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

	_GROUPCNT ++;
	_SPRDID = IdGen::UUID(6);

	const auto& first_element = d[0];
	const auto& first_order = first_element["Orders"][0];

	KTN::Order incoming = OrderParser::parseorder(first_order);
	//Get Order Inst/Add Leg with new function
	instrument leg = {};
	int i = AddLeg(incoming.symbol, leg);
	incoming.instindex = i;
	incoming.mktsegid = _cp.Legs[i].mktsegid;


#ifdef DEBUG_TESTING
	ostringstream oss;
	oss << "INCOMING ORD: " << OrderUtils::Print(incoming) << "\nLEG COUNT=" << _cp.LegCount;
	LogMe(oss.str(), LogLevel::WARN);
#endif


	KTN::OrderPod ord = {};
	_quoter[leg.index].NextNew(ord, incoming.OrdSide, ord.exchordid,
			incoming.quantity, incoming.price);

	ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;

	//Get Hedge Inst/Add Leg with new function
	AutoHedgeOB2Params p = _cp;
	p.HdgRatio = first_element["HdgRatio"].GetDouble();
	p.HdgSymbol = first_element["HdgSymbol"].GetString();
	p.HdgIndex = AddLeg(p.HdgSymbol, p.HedgeLeg);
	p.HdgSide = (ord.OrdSide == KOrderSide::BUY) ? KOrderSide::SELL : KOrderSide::BUY;
	p.TakeTicks = first_element["TakeTicks"].GetInt();
	_hdgMap[ord.orderReqId] = p;

	SendOrder(ord, leg);
	_reqMap[ord.ordernum] = ord.orderReqId;

}


void AutoHedgeOB::Print()
{
	LogMe("\n****" + m_Name + "****\n", LogLevel::INFO);
	PrintMdMap();

	_trk.printMeasurements();

	ostringstream oss;
	oss << "***HDGMAP SIZE=" << _hdgMap.size();

	LogMe(oss.str(), LogLevel::INFO);

	if (_hdgMap.size() > 0)
	{
		int i = 0;
		for(auto kv : _hdgMap)
		{
			ostringstream oss;
			oss <<"  " << i << ": REQID=" << kv.first << " "
					<< KOrderSide::toString(kv.second.HdgSide)
					<< " SYMB=" << kv.second.HdgSymbol << " RATIO=" << kv.second.HdgRatio;
			LogMe(oss.str());
		}
	}

	::AlgoBaseV3::Print();

}

void AutoHedgeOB::OrderMinUpdate(KTN::OrderPod& ord)
{
	if (ord.OrdState == KOrderState::COMPLETE)
	{
		if (_hdgMap.find(ord.orderReqId) != _hdgMap.end())
			_hdgMap.erase(ord.orderReqId);
	}

}

void AutoHedgeOB::OrderReceived(KTN::Order& incoming, int idx)
{
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
	LogMe("ON ORDER RECEIVED: " + OrderUtils::Print(incoming), LogLevel::WARN);
#endif

	if (action == "NEW")
	{
		return;
	}

	if (action == "MOD")
	{
		if (incoming.exchordid == 0)
		{
			LogMe("ERROR ON ALGO MOD!! THIS ORDER CAN NOT BE MODIFIED!!! NOT FOUND: "
					+ OrderUtils::toString(incoming.origclordid), LogLevel::ERROR);
			return;
		}

		KTN::OrderPod ord = {};
		_quoter[idx].Get(ord);

		_Ords.GetByExchId(ord, incoming.exchordid);

		ord.quantity = incoming.quantity;
		ord.price = incoming.price;
		ord.callbackid = _EXCH_CALLBACK_ID;
		ord.OrdAction = KOrderAction::ACTION_MOD;
		ord.OrdStatus = KOrderStatus::sent_mod;
		ord.OrdState = KOrderState::INFLIGHT;
		//ord.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;
		ord.OrdExch = KOrderExchange::CME;

		_Ords.onSentOrderMod(ord);
		_ordersender->Send(ord, KOrderAction::ACTION_MOD);

		LogMe("AUTOHDG SENT MANUAL MOD ORDER: " + OrderUtils::Print(ord), LogLevel::WARN);
		_ordersender->Send(ord, KOrderAction::ACTION_MOD);
		_Ords.onSentOrderFast(ord);
	}

	if (action == "CXL")
	{
		KTN::OrderPod ord = {};
		_quoter[idx].Get(ord);

		_Ords.GetByExchId(ord, incoming.exchordid);

		if (ord.exchordid == 0)
		{
			LogMe("ERROR ON ALGO CXL!! THIS ORDER CAN NOT BE CANCELED!!! EXCHID NOT FOUND: "
				+ std::to_string(ord.exchordid), LogLevel::ERROR);
			return;
		}

		//OrderUtils::Copy(ord.origclordid, incoming.ordernum);
		/*ord.orderReqId = inc.orderReqId;
		ord.exchordid = inc.exchordid;*/
		ord.callbackid = _EXCH_CALLBACK_ID;
		ord.OrdAction = KOrderAction::ACTION_CXL;
		ord.OrdStatus = KOrderStatus::sent_cxl;
		ord.OrdState = KOrderState::INFLIGHT;
		//ord.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;
		ord.OrdExch = KOrderExchange::CME;

		_Ords.onSentOrderCxl(ord);
		_ordersender->Send(ord, KOrderAction::ACTION_CXL);
		if (_hdgMap.find(ord.orderReqId) != _hdgMap.end())
			_hdgMap.erase(ord.orderReqId);

	}


	ChangeQuoteState(QuoteState::SentOrders);
}

void AutoHedgeOB::ReportStatus()
{
	//std::string status = (_cp.meta.Enabled == 0) ? "DISABLED" : "ENABLED";

}

void AutoHedgeOB::ResetPosition()
{
	LogMe("RESETTING POSITION!!!", LogLevel::INFO);
	_CIRCUITBREAKER = false;
	ReportStatus();
}

void AutoHedgeOB::Restart()
{

}

void AutoHedgeOB::Enable(bool bEnable)
{
	_ENABLED = bEnable;

	if (_ENABLED)
	{
		ReportStatus();
		ChangeQuoteState(QuoteState::ReadyToRefresh);
	}
	else
	{
		ReportStatus();
		LogMe("GOT DISABLE COMMAND!", LogLevel::WARN);
		_ENABLED = false;
	}
}


void AutoHedgeOB::onCalculate() {
}

void AutoHedgeOB::onMktData(int index)
{
	if (!GoodToGo(index))
			return;
}


void AutoHedgeOB::onTrade(int index, double price, double size, int aggressor)
{
	if (!GoodToGo(index))
		return;
}

bool AutoHedgeOB::SendHedge(uint64_t reqid, int fillqty, AutoHedgeOB2Params& p)
{
	instrument leg = _cp.Legs[p.HdgIndex];

	if (!_cp.Legs[leg.index].gotdata)
	{
		ostringstream oss;
		oss << "AUTOHEDGE ERROR!!! " << p.HdgSymbol << " HAS NO MKT DATA! INDEX=" << leg.index
				<< " SECID=" << leg.secid;
		LogMe(oss.str(),LogLevel::ERROR);
		return false;
	}

	DepthBook md;
	GetQuoteByIndex(leg.index, md);

	int price = (p.HdgSide == KOrderSide::BUY) ? md.Ask : md.Bid;
	//this should round....
	//int origqty = (int)(p.HdgRatio * fillqty);

	double hdgqty = p.HdgRatio * fillqty + p.HdgRemainder;
	int qty = static_cast<int>(std::floor(hdgqty));
	p.HdgRemainder = hdgqty - qty;


	if (qty != hdgqty)
	{
		LogMe("HDG QUANTITY ADJUSTED FROM " + std::to_string(hdgqty)
				+ " TO " + std::to_string(qty) + "| TOTAL REMAINDER=" + std::to_string(p.HdgRemainder));
	}

	if (qty == 0)
	{
		LogMe("HEDGE NOT SENT: HEDGE QTY=0 | TOTAL REMAINDER=" + std::to_string(p.HdgRemainder));
		return true;
	}


	if (p.TakeTicks > 0)
	{
		int ticks = leg.mintick * p.TakeTicks;
		price += (p.HdgSide * ticks);
	}

	KTN::OrderPod ord = {};
	_quoter[leg.index].NextNew(ord, p.HdgSide, ord.exchordid,
			qty, price);
	ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_HEDGE;


	p.HdgSent += ord.quantity;

	_Ords.onSentOrderFast(ord);

	SendOrder(ord, leg);

	LogMe("SENT AUTOHDG!!! TRIG=" + KOrderAlgoTrigger::toString(ord.OrdAlgoTrig)
			+ " PRICE=" + std::to_string(ord.price) + " QTY=" + std::to_string(ord.quantity)
			+ " REMAINDER="
			+ std::to_string(p.HdgRemainder),LogLevel::WARN);

	return true;
}

void AutoHedgeOB::ToggleCommand(int id)
{

}


void AutoHedgeOB::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) {

	_Ords.minOrderExecution(ord, hedgeSent);

	if (ord.OrdFillType == KOrderFillType::SPREAD_LEG_FILL)
		return;

	ostringstream oss;
	oss << KOrderAlgoTrigger::toString(ord.OrdAlgoTrig) << " SECID=" << ord.secid << " SIDE="
			<< ord.OrdSide << " QTY=" << ord.lastqty << " PX=" << ord.lastpx <<" CmeID=" <<  ord.exchordid ;
	LogMe("**MINFILL FOR: " + oss.str(), LogLevel::INFO);

	if (ord.OrdAlgoTrig == KOrderAlgoTrigger::ALGO_HEDGE){
		ChangeQuoteState(QuoteState::ReadyToRefresh);
		return;
	}

	if (_hdgMap.find(ord.orderReqId) != _hdgMap.end())
	{
		SendHedge(ord.orderReqId, ord.lastqty, _hdgMap[ord.orderReqId]);

		if (ord.OrdStatus == KOrderStatus::FILLED)
		{
			if (_hdgMap.find(ord.orderReqId) != _hdgMap.end())
				_hdgMap.erase(ord.orderReqId);
		}
	}

	ChangeQuoteState(QuoteState::ReadyToRefresh);
}


bool AutoHedgeOB::GoodToGo(int index)
{
	//define what you do when you calculate...
//	if (!_ENABLED)
//		return false;
//
//	if (!_CAN_TRADE)
//		return false;

	if (_cp.Legs[index].gotdata)
		return true;

	return GotMktData(index);

//	int bid = GetBid(index);
//	int ask = GetAsk(index);
//	if (bid != 0 && ask !=0)
//	{
//		_cp.Legs[index].gotdata = true;
//		return true;
//	}
//	return false;

}

bool AutoHedgeOB::GotMktData(int index)
{
	instrument leg = _cp.Legs[index];

	if (leg.gotdata) return true;

	DepthBook sym = GetQuote(leg);
	if ((sym.Bid == 0 && sym.Ask == 0) || sym.BidSize == 0 || sym.AskSize == 0)
	{
		_DATA_TRIES ++;
		if (_DATA_TRIES < 10 || _DATA_TRIES % 1000 == 0)
		{
			ostringstream oss;
			oss <<_DATA_TRIES << ": DATA ERROR " << leg.symbol << ": " << sym.Bid << "@" <<  sym.Ask << " " << sym.BidSize << " x " <<  sym.AskSize;
			LogMe(oss.str());
			LogMe(oss.str(), LogLevel::WARN);
			AddReportingMsg(oss.str(),"ALERT");
		}
		_cp.Legs[index].gotdata = false;
		return false;
	}

	_cp.Legs[index].gotdata = true;

	return true;
}

void AutoHedgeOB::AddLeg(const KTN::Order& ord, instrument& ln)
{
	ln = LegParamParser::GetCmeLeg(ord.symbol, ord.OrdProdType, _cp.LegCount);

	LogMe("JSON LOADED LEG: " + ln.Print(), LogLevel::WARN);

	_cp.LegSecIdMap[ln.secid] = ln;

	ln.callbackid = _EXCH_CALLBACK_ID;

	RegisterCmeSecId(ln);

	_quoter[_cp.LegCount].Construct(_EXCH_CALLBACK_ID, ln, _PARAMS);

	_cp.Legs[ln.index] = ln;
	_SymbolIndexMap[ln.symbol] = ln.index;
	_cp.LegSecIdMap[ln.secid] = ln;

	_cp.LegCount++;

	ostringstream lll;
	lll << "ADDED LEG " << ln.index <<" SYMBOL: " << ln.symbol << " SEGID=" << (int)ln.mktsegid
		<<	"PRODTYPE=" << ln.prodtype << " SECID=" << ln.secid << " MINTICK=" << ln.mintick
		<< " LEGS=" << _cp.LegCount;
	LogMe(lll.str(),LogLevel::INFO);
}

int AutoHedgeOB::AddLeg(string symbol, instrument& ln)
{

	//bool foundleg = false;
	for(int i = 0; i < _cp.LegCount; i++){
		if (symbol == _cp.Legs[i].symbol)
			return i;
	}

	//if we get here, we need to add

	CmeSecDef def = CmeSecMaster::instance().getSecDef(symbol);
	ln = LegParamParser::GetCmeLeg(symbol, def.prodtype, _cp.LegCount);

	LogMe("JSON LOADED LEG VIA SYMBOL: " + ln.Print(), LogLevel::WARN);

	_cp.LegSecIdMap[ln.secid] = ln;

	ln.callbackid = _EXCH_CALLBACK_ID;

	RegisterCmeSecId(ln);

	_quoter[_cp.LegCount].Construct(_EXCH_CALLBACK_ID, ln, _PARAMS);

	_cp.Legs[ln.index] = ln;
	_SymbolIndexMap[ln.symbol] = ln.index;
	_cp.LegSecIdMap[ln.secid] = ln;

	_cp.LegCount++;

	ostringstream lll;
	lll << "ADDED LEG " << ln.index <<" SYMBOL: " << ln.symbol << " SEGID=" << (int)ln.mktsegid
		<<	"PRODTYPE=" << ln.prodtype << " SECID=" << ln.secid << " MINTICK=" << ln.mintick
		<< " LEGS=" << _cp.LegCount;
	LogMe(lll.str(),LogLevel::INFO);

	return ln.index;
}

} /* namespace KTN */
