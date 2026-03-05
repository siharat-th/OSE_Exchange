/*
 * MutliSpreader2.cpp
 *
 *  Created on: Apr 14, 2023
 *      Author: sgaer
 */

#include "MultiSpreaderV3.hpp"

namespace KTN {

MultiSpreaderV3::MultiSpreaderV3(AlgoInitializer p): AlgoBaseV3(p), _QuoteOrdPool(100)
{
	_HANDLER_STARTED = false;
	
	_cp.NetPosition = 0;
	_cp.TotalExec = 0;
	bCancelAllSent = false;
	_USE_REPORTING_MSGS = true;
	_SHOW_QUOTES = false;
	_ENABLED = false;
	_QUOTEID = 0;
	_LASTQUOTEID = 0;
	_CNT = 0;
	_QuotingState = QuoteState::TimerTick;
	_alloCount = 0;
}

MultiSpreaderV3::~MultiSpreaderV3() {
}

void MultiSpreaderV3::onLaunch(std::string json) {

	cout << "[" << __CLASS__ << " DEBUG] onJson: " << json << endl;;
	//this can be better but for now huge improvement
	_cp = MS2Params::parse(json);
	UpdateMetaParams(_cp.meta);

	LoadSpreadLegDefs(json);

	onJson(json);
}

void MultiSpreaderV3::onJson(std::string json) {

	LogMe("*****" + m_Name + " PARAMS JSON", LogLevel::INFO);
	cout << "[" << __CLASS__ << " DEBUG] onJson: " << json << endl;;

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

	_cp.Reset = false;
	if (doc.HasMember("ResetPosition"))
	{
		_cp.Reset = doc["ResetPosition"].GetBool();

		if (_cp.Reset)
		{
			LogMe("*** GOT RESET POSITION COMMAND!!! ***", LogLevel::WARN);
			ResetPosition();
			_cp.Reset = false;
			_cp.meta.Enabled = false;
			return;
		}
	}

	_cp.meta.Enabled = doc["Enabled"].GetInt();

	if (_cp.meta.Enabled != 1)
	{
		Enable(false);
		return;
	}
	_cp.MaxPosition = doc["MaxPosition"].GetDouble();
	_cp.TotalQty = doc["TotalQty"].GetDouble();

	_cp.OrderQty = doc["OrderQty"].GetDouble();

	_cp.Slop = doc["Slop"].GetDouble();

	_cp.ReloadOnFill = doc["ReloadOnFill"].GetBool();

	_cp.SendBids = doc["SendBids"].GetBool();
	_cp.SendAsks = doc["SendAsks"].GetBool();

	_cp.SpreadBid = doc["SpreadBid"].GetDouble();
	_cp.SpreadAsk = doc["SpreadAsk"].GetDouble();

	_cp.BuyDisc = doc["BuyDisc"].GetDouble();
	_cp.SellDisc = doc["SellDisc"].GetDouble();
	_cp.HedgeStop = doc["HedgeStop"].GetDouble();

	_cp.SqueezeQty =  (doc.HasMember("SqueezeQty")) ? doc["SqueezeQty"].GetInt() : 75;
	_cp.SqueezeRatio =  (doc.HasMember("SqueezeRatio")) ? doc["SqueezeRatio"].GetDouble() : 2.5;

	//_cp.ZeroPlus = (doc.HasMember("ZeroPlus") ? doc["ZeroPlus"].GetBool() : false);
	_cp.WavgPlus = (doc.HasMember("WavgPlus") ? doc["WavgPlus"].GetBool() : false);
	_cp.WavgThreshold = (doc.HasMember("WavgThreshold") ? doc["WavgThreshold"].GetDouble() : 0.0);
	_cp.ExchFee = (doc.HasMember("ExchFee") ? doc["ExchFee"].GetDouble() : 0.0);

	_cp.MinLeanQty = doc["MinLeanQty"].GetInt();

	vector<instrument> legtemp = LegParamParser::ParseSpreadLegs( json);
	for(int i = 0; i < _cp.LegCount; i++)
	{
		if (_cp.Legs[i].symbol == legtemp[i].symbol)
		{
			_cp.Legs[i].activeQuoting = legtemp[i].activeQuoting;
			if (_cp.Legs[i].activeQuoting)
			{
				LogMe("LEG " + _cp.Legs[i].symbol + " ACTIVE_QUOTING ENABLED!!!", LogLevel::INFO );
			}
		}
	}

	UpdateMetaParams(_cp.meta);

	if (_cp.meta.Enabled != 0)
	{
		Enable(true);
//		//_bQuoteInFlight = false;
//		_CIRCUITBREAKER = false;
//		//LogMe("***STRATEGY=" + _PARAMS.Strategy, LogLevel::INFO);
//		_CAN_TRADE = true;
//		onCalculate();
	}
	else
	{
//		_CIRCUITBREAKER = true;
//		_ENABLED = false;
//		_CAN_TRADE = false;
		Enable(false);
	}
}

void MultiSpreaderV3::LoadSpreadLegDefs(string json)
{
	bool has_cme_products = false;
	_cp.LegCount = 0;

	for (auto ln : _LEGS)
	{
		if (ln.iscme)
		{
			RegisterCmeSecId(ln);
			has_cme_products=true;
		}

		ln.callbackid = _EXCH_CALLBACK_ID;

		ln.lastBuyId = 0;
		ln.lastSellId = 0;
		ln.buyState = KOrderState::UNKNOWN;
		ln.sellState = KOrderState::UNKNOWN;

		_quoter[_cp.LegCount].Construct(_EXCH_CALLBACK_ID, ln, _PARAMS);

		Order ord = _quoter[_cp.LegCount].GetTemplate();
		_Ords.RegisterOrderTemplate(ord);

		_cp.Legs[ln.index] = ln;
		_cp.LegSecIdMap[ln.secid] = ln;

		_msgcnt[_cp.LegCount] = {};
		_cp.LegCount++;

		ostringstream lll;
		lll << "ADDED LEG " << ln.index <<" SYMBOL: " << ln.symbol
			<< " PRODTYPE=" << ln.prodtype << " SECID=" << ln.secid << " MINTICK=" << ln.mintick
			<< " LEGS=" << _cp.LegCount;
		LogMe(lll.str(),LogLevel::INFO);
	}

	_NUMLEGS = _cp.LegCount;


	if (has_cme_products )
	{
		instrument* legs = new instrument[_cp.LegCount];
		for(int i = 0; i < _cp.LegCount; i++)
			legs[i] = _cp.Legs[i];
		_pricer.Load(legs, _cp.LegCount);
		_pricer.LoadOrders(_EXCH_CALLBACK_ID, _PARAMS);

	}

}

void MultiSpreaderV3::OrderReceived(KTN::Order& incoming, int idx)
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

	if (action == "NEW")
	{
		LogMe("ERROR: MULTISPREADER DOES NOT ACCEPT NEW ORDERS FROM OUTSIDE ALGO\nCANCEL IS THE ONLY ACTIONS PERMITTED", LogLevel::ERROR);
		return;
	}

	if (action == "MOD")
	{

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
	}


	ChangeQuoteState(QuoteState::SentOrders);
}

vector<string> MultiSpreaderV3::Symbols()
{
	vector<string> symbs;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument leg = _cp.Legs[i];
		symbs.push_back(leg.symbol);
	}

	return symbs;
}

void MultiSpreaderV3::Print()
{
	LogMe("****" + m_Name + " INFO ****", LogLevel::WARN);

	_Ords.Print();

	ostringstream oss;
	oss << "POSITION: " << _cp.NetPosition ;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument leg = _cp.Legs[i];
		oss << " | " << leg.symbol << ":" << leg.totalexec;
	}
	LogMe(oss.str(),LogLevel::INFO);


	LogMe("----- MESSAGING -----",LogLevel::INFO);
	oss.str("");
	for(int i = 0; i < _cp.LegCount; i++)
	{
		instrument leg = _cp.Legs[i];
		oss << "  " << leg.symbol << "[" << i << "]: NEW=" << _msgcnt[i].news
				<< " MODS=" << _msgcnt[i].mods << "\n";
	}
	LogMe(oss.str());
	LogMe("---------------------",LogLevel::INFO);


	if (_MEASURE)
	{
		_MEASURE = false;

		LogMe("-----LATENCY STATS: ALGO " + _GUID + "------",LogLevel::INFO);
		_trk.printMeasurements();
		_trk.clear();
		_MEASURE = true;
		LogMe("---------------------",LogLevel::INFO);
	}


	PrintMdMap();

	::AlgoBaseV3::Print();

}

void MultiSpreaderV3::ReportStatus()
{

	std::string status = (_cp.meta.Enabled == 0) ? "DISABLED" : "ENABLED";

	ostringstream oss;
	oss << "POS: " << _cp.NetPosition ;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument leg = _cp.Legs[i];
		oss << "; " << leg.symbol << ":" << leg.totalexec;

	}

	AddReportingMsg(oss.str(), "INFO");
}

void MultiSpreaderV3::AlertCheck()
{
	double d = .90;

	if (fabs(_cp.NetPosition) >= ( d *fabs(_cp.MaxPosition)))
	{
		//ostringstream oss;
		//oss << "NET POSITION > " << (d*100) << "% OF MAX:\n" << _cp.NetPosition << " vs. " << _cp.MaxPosition;

//		ostringstream pss;
//		pss << (d*100) << "% POSITION: " << _cp.NetPosition << " " << _cp.meta.Symbol << "." << _cp.meta.Exch << " MktMaker";

		//_SLACKER.Send(pss.str(), _cp.meta.Symbol + "." + _cp.meta.Exch + " MktMaker POS ALERT", oss.str(), LogLevel::WARNING, true);

		ChangeQuoteState(QuoteState::MaxExposure);
		return;
	}
}

void MultiSpreaderV3::ResetPosition()
{
	LogMe("RESETTING POSITION!!!", LogLevel::INFO);
	_cp.NetPosition = 0;
	_cp.TotalExec = 0;
	_NETPOS = 0;
	_TOTALEXEC = 0;

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

void MultiSpreaderV3::Restart()
{
}

void MultiSpreaderV3::Enable(bool bEnable)
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

		ChangeQuoteState(QuoteState::ReadyToSendOrders);
	}
	else
	{
		ChangeQuoteState(QuoteState::Disabled);
		_ENABLED = false;
		_CAN_TRADE = false;
		CancelAllOrders();
		ClearLegSentIds();
		ReportStatus();

		LogMe("GOT DISABLE COMMAND", LogLevel::WARN);
	}
}

void MultiSpreaderV3::CancelAllOrders()
{
	bCancelAllSent = true;

	vector<KTN::OrderPod> ords;

	_Ords.GetByAlgoTrigger(ords, KOrderAlgoTrigger::ALGO_LEG);

	ostringstream oss;
	oss << "CXL ALL LEGS RESULT=" << ords.size() << " STRATID=" << _PARAMS.StratId;
	LogMe(oss.str(),LogLevel::WARN);

	int icnt = 0;
	for (auto ord : ords)
	{
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

		_Ords.onSentOrderCxl(cxl);
		icnt++;

		LogMe("SENDING POD CXL: " + OrderUtils::Print(cxl));
		_ordersender->Send(cxl, KOrderAction::ACTION_CXL);

	}

	LogMe("CXL ALL SENT " + std::to_string(icnt) + " CANCELS!!!", LogLevel::WARN);

}

void MultiSpreaderV3::onCalculate() {

	if (!GotMktData())
		return;

	//if (_MEASURE) _trk.startMeasurement("HdgStopChk");
	if (_cp.HedgeStop > 0 && _HedgingState == HedgeState::ActiveHedges)
	{
		if (_cp.HedgeStop > 0)
			CheckHedgeStops();

		if (_cp.SqueezeRatio > 0)
			CheckSqueeze();
	}

	if (!GoodToGo())
		return;

	//prevent multiple orders from being entered for same price
	QuoteState::Enum origstate = _QuotingState;
	ChangeQuoteState(QuoteState::Calculating);

	//and now see if we're ok to keep quoting....
	bool anybidchanged = false;
	bool anyaskchanged = false;

	//if (_MEASURE) _trk.startMeasurement("onCalc");
	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument& leg = _cp.Legs[i];
		int mktbid = GetBid(leg.index);
		int mktask = GetAsk(leg.index);

		//cout << "[MulitSpread DEBUG] " << leg.symbol << " BID=" << mktbid << " ASK=" << mktask << " index=" << leg.index << " i=" << i << endl;

		leg.bidChanged = false;
		leg.askChanged = false;

		//now let's see if the bid has changed enough for us to move:
		double bid_diff = mktbid - leg.lastBid;
		double ask_diff = mktask - leg.lastAsk;

		double slop = _cp.Slop * leg.mintick;

		//cout << leansym.symbol << " BIDDIFF=" << bid_diff << " ASKDIFF=" << ask_diff << " LASTBID=" << leg.lastBid << " LASTASK=" << leg.lastAsk << endl;

		if (fabs(bid_diff) >= slop && (leg.lastBid != mktbid))
		{
			leg.lastBid = mktbid;
			leg.bidChanged = true;
			anybidchanged = true;
			//cout <<"** BIDCHANGE=TRUE" << endl;
		}
		
		if (fabs(ask_diff) >= slop  && (leg.lastAsk != mktask))
		{
			leg.lastAsk = mktask;
			leg.askChanged = true;
			anyaskchanged = true;
			//cout <<"** ASKCHANGE=TRUE" << endl;
		}
		
	}
	//if (_MEASURE) _trk.stopMeasurement("onCalc");

	if (anybidchanged == false && anyaskchanged == false)
	{
		ChangeQuoteState(origstate);
		return;
	}

	//now if anything has changed in our quote tree, let's requote all legs:
	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument& leg = _cp.Legs[i];
		if (leg.activeQuoting)
			QuoteGenerator(leg, anybidchanged, anyaskchanged);

		leg.bidChanged = false;
		leg.askChanged = false;
	}
	
}

void MultiSpreaderV3::QuoteGenerator(instrument& leg, bool bidChange, bool askChange)
{
	QuoteState::Enum origstate = _QuotingState;
	ChangeQuoteState(QuoteState::Calculating);

	//SendBids and SendAsks is from the SPREAD LEVEL perspective.
	//so, you need to be able to send actively quoted legs for any side that's
	//respective of the Spread-level's side. I don't fucking know anymore. FIX THIS YOU IDIOT.

	/*
	 * Buy spread: iSide = 1 -> send buys;  iSide = -1 -> send sells;
	 *
	 * Sell spread iSide = 1 -> send sells; iSide = -1 -> send buys
	 */

	// ostringstream oss;
	// oss << "[MultiSpread DEBUG] LEG " << leg.index << " QUOTER: " << leg.symbol << " spreadBid=" << _cp.SpreadBid
	// 	<< " spreadAsk=" << _cp.SpreadAsk << " buyDisc=" << _cp.BuyDisc << " sellDisc=" << _cp.SellDisc;
	// cout << oss.str() << endl;
//	LogMe(oss.str());

	//double qty = _cp.QuoteSizes[0];
	bool sentbid = false;
	bool sentask = false;

	if (_cp.SendBids)
		sentbid = SendQuote(leg, true, _cp.SpreadBid, _cp.BuyDisc);

	if (_cp.SendAsks)
		sentask = SendQuote(leg, false, _cp.SpreadAsk, _cp.SellDisc);
		
	if (!sentbid && !sentask)
		ChangeQuoteState(origstate);
}

bool MultiSpreaderV3::SendQuote(instrument& leg, bool isbid, int edge, int ticks)
{
	_pricer.CalculateSide3(_imp[leg.index], leg.index, leg.spreadmult, isbid, edge, ticks,
			 _mdh->Obm.books, _cp.TestMode);
	bool sendimp = false;

	if(_imp[leg.index].OrderSide == KOrderSide::BUY && _imp[leg.index].price != leg.lastSentBuyPrice)
		sendimp = true;
	else if(_imp[leg.index].OrderSide == KOrderSide::SELL && _imp[leg.index].price != leg.lastSentSellPrice)
		sendimp = true;
		
	if (!sendimp) return false;

	if (!CheckPositionAndQty()) return false;
	
	bool sentorders = false;
	KTN::OrderPod ord = *_QuoteOrdPool.get();
	
	if (!PrepareQuotingOrder(ord, leg, _imp[leg.index].OrderSide, _imp[leg.index].price,
			_cp.OrderQty )){
		_QuoteOrdPool.put(&ord);
		return false;
	}
	
	//this changes quote state to SentOrders
	SendOrder(ord, leg);
	
	_ordersender->SendHedgeInstruction(ord.orderReqId, _imp[leg.index].hdgPath);
	
	sentorders = true;
	
	_QuoteOrdPool.put(&ord);
	
	return sentorders;
}

bool MultiSpreaderV3::PrepareQuotingOrder(KTN::OrderPod &ord, const instrument& inst,
		KOrderSide::Enum side, int price, int qty)
{

	bool found = false;
	if (_MEASURE) _trk.startMeasurement("PrepOrdV5Search");
	found = _Ords.GetBySecIdSideStrat(ord, inst.secid, side, _PARAMS.StratId);
	if (_MEASURE) _trk.stopMeasurement("PrepOrdV5Search");

	int quoteqty = fabs(inst.spreadmult) * qty;

	if (!RiskCheckQty(quoteqty, _cp.TotalQty, _cp.TotalExec, side))
				return false;

	if (found && ord.orderReqId > 0)
	{
		if (ord.OrdState != KOrderState::WORKING)
			return false;

		if (price == ord.price)
			return false;

		//if (_MEASURE) _trk.startMeasurement("QuoteNextMod");
			_quoter[inst.index].NextMod(ord, side, ord.orderReqId, ord.exchordid, quoteqty, price);
		//if (_MEASURE) _trk.stopMeasurement("QuoteNextMod");

		_msgcnt[inst.index].mods++;

		return true;
	}
	else
	{
		//if (_MEASURE) _trk.startMeasurement("QuoteNextNew");
		_quoter[inst.index].NextNew(ord, side, 0, quoteqty, price);
		//if (_MEASURE) _trk.stopMeasurement("QuoteNextNew");
		_msgcnt[inst.index].news++;
		return true;
	}

	return false;
}


void MultiSpreaderV3::ToggleCommand(int id)
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

	if (id == 71 || id == 72)
	{
		if (id == 71)
			_SHOW_QUOTES = true;

		if (id == 72)
			_SHOW_QUOTES = false;

		if (id != 71 && id != 72)
			return;

		string tf = (_SHOW_QUOTES) ? "TRUE" : "FALSE";
		LogMe("SHOW QUOTES SET TO " + tf, LogLevel::INFO);
	}


}

void MultiSpreaderV3::QuickOrdPrint(const KTN::Order& ord, const instrument& inst)
{
	DepthBook legsym = GetQuote(inst);
	ostringstream oss;
	oss << "**QUOTE " << inst.symbol << " SIDE=" << ord.OrdSide << " SYMB=" << ord.symbol << " PRICE=" << ord.price
		<< " QTY=" << ord.quantity << " ORDPX=" << ord.price << " QUOTE=" << legsym.Bid << "@" << legsym.Ask;

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



void MultiSpreaderV3::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent)
{
	//if it's a spread leg fill we don't really care here
	if (ord.OrdFillType == KOrderFillType::SPREAD_LEG_FILL)
	{
		_Ords.minOrderExecSpreadLeg(ord);
		return;
	}

	_Ords.minOrderExecution(ord, hedgeSent);


	ostringstream oss;
	oss << KOrderAlgoTrigger::toString(ord.OrdAlgoTrig) << " CmeID=" <<  ord.exchordid << " SECID=" << ord.secid << " SIDE="
			<< ord.OrdSide << " QTY=" << ord.lastqty << " PX=" << ord.lastpx;
	LogMe("**ALGO GOT FILL: " + oss.str(), LogLevel::INFO);


	if (ord.OrdAlgoTrig == KOrderAlgoTrigger::ALGO_LEG )
	{
		ChangeQuoteState(QuoteState::MonitorPL);

		//here, we are just caring about how many times we execute the spread...
		_cp.TotalExec ++;// += newqty;
		_TOTALEXEC ++; //+= newqty;

		UpdateLegState(ord.orderReqId, ord.OrdState);

		CancelAllOrders();
		CheckPositionAndQty();
		return;
	}

	if (ord.OrdAlgoTrig == KOrderAlgoTrigger::ALGO_HEDGE)
	{
		if (ord.OrdStatus == KOrderStatus::PARTIALLY_FILLED)
			return;

		ChangeHedgeState(HedgeState::NoHedges);
	}
}

void MultiSpreaderV3::onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid,KOrderSide::Enum side,
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
	oss << "ALGO GOT HEDGE SENT: " << KOrderSide::toString(side)
							<< " " <<  qty << " " <<  _cp.LegSecIdMap[secid].symbol
							<< " @ " << price << " index=" << index
							<< " secid=" << secid << " clordid=" << clordid << " reqid=" << reqid;
	LogMe(oss.str(),LogLevel::INFO);


	OrderPod ord = {};
	_quoter[index].NextNew(ord, side, 0, qty, price);

	ord.orderReqId = reqid;
	ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_HEDGE;
	OrderUtils::fastCopy(ord.ordernum, clordid, sizeof(ord.ordernum));

	LogMe("SENDING HDG TO V5: " + OrderUtils::Print(ord));

	_Ords.onSentOrderFast(ord);

}

void MultiSpreaderV3::onOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
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

void MultiSpreaderV3::OrderMinUpdate(KTN::OrderPod& ord)
{
	UpdateLegState(ord.orderReqId, ord.OrdState);
}

void MultiSpreaderV3::UpdatePositionOnFill(const KTN::Order& ord)
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

void MultiSpreaderV3::CheckHedgeStops()
{
	vector<KTN::OrderPod> ords;
	_Ords.GetByAlgoTrigger(ords, KOrderAlgoTrigger::ALGO_HEDGE);

	if (ords.size() == 0) return;

	for(auto ord : ords)
	{
		int mult = (int)ord.OrdSide;
		int mktprice = (mult == 1) ? GetAsk(ord.instindex) : GetBid(ord.instindex);
		int stopprice = ord.price + (mult * _cp.HedgeStop);


		if ((mktprice - stopprice) * mult >= 0) {
		    // stop me out
		    // Action for both BUY and SELL is handled here
//			if (_MEASURE) _trk.startMeasurement("QuotePoolGet");
//					KTN::OrderPod hdg = *_QuoteOrdPool.get();
//			if (_MEASURE) _trk.stopMeasurement("QuotePoolGet");

			KTN::OrderPod hdg = {};
			_quoter[ord.instindex].NextMod(hdg, ord.OrdSide, ord.orderReqId, ord.exchordid, ord.leavesqty, mktprice);

			hdg.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_STOPLOSS;
			_Ords.onSentOrderFast(hdg);

			_ordersender->Send(hdg, hdg.OrdAction);

			ostringstream oss;
			oss << KOrderAlgoTrigger::toString(hdg.OrdAlgoTrig) << " STOP ELECTED: SIDE=" << KOrderSide::toString(hdg.OrdSide)
					<< " ORIG=" << ord.price << " STOP=" << mktprice << " NEW_PX=" << hdg.price;
			LogMe(oss.str(),LogLevel::WARN);
		}
	}

}

bool MultiSpreaderV3::GoodToGo()
{
	if (!_ENABLED )
		return false;

	if (!GotMktData()) return false;

//	if (fabs(_cp.NetPosition) >= fabs(_cp.MaxPosition))
//	{
//		if (_ENABLED) Enable(false);
//		usleep(2 * 1000 * 1000);
//
//		ostringstream oss;
//		oss << "*** NET POSITION EXCEEDED: " << _cp.NetPosition << " vs MAX:" << _cp.MaxPosition ;
//		LogMe(oss.str(), LogLevel::ERROR);
//
//		AddReportingMsg(oss.str(),"ALERT");
//
//		return false;
//	}
//
//	if (fabs(_cp.TotalExec) >= fabs(_cp.TotalQty))
//	{
//		if (_ENABLED) Enable(false);
//		usleep(2 * 1000 * 1000);
//
//		ostringstream oss;
//		oss << "*** TOTAL EXECUTED EXCEEDED: " << _cp.TotalExec << " vs MAX:" << _cp.TotalQty ;
//		LogMe(oss.str(), LogLevel::ERROR);
//
//		usleep(100000);
//		AddReportingMsg(oss.str(), "ALERT");
//
//		return false;
//	}

	ChangeQuoteState(QuoteState::ReadyToSendOrders);

	return true;
}

bool MultiSpreaderV3::GotMktData()
{
	if (LIKELY(_GOT_DATA)) return true;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument leg = _cp.Legs[i];
		DepthBook sym = GetQuote(leg);
		if ((sym.Bid == 0 && sym.Ask == 0) || (sym.Bid > sym.Ask) || sym.BidSize == 0 || sym.AskSize == 0)
		{
			_DATA_TRIES ++;
			if (_DATA_TRIES < 10 || _DATA_TRIES % 1000 == 0)
			{
				ostringstream oss;
				oss <<_DATA_TRIES << ": DATA ERROR " << leg.symbol << ": " << sym.Bid << "@" <<  sym.Ask << " " << sym.BidSize << " x " <<  sym.AskSize;
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

bool MultiSpreaderV3::CheckPositionAndQty()
{
	if (fabs(_cp.NetPosition) >= fabs(_cp.MaxPosition))
	{
		//_algostate.CIRCUITBREAKER = true;
		if (_ENABLED) Enable(false);
		usleep(2 * 1000 * 1000);

		ostringstream oss;
		oss << "*** NET POSITION EXCEEDED: " << _cp.NetPosition << " vs MAX:" << _cp.MaxPosition ;
		LogMe(oss.str(), LogLevel::ERROR);

		AddReportingMsg(oss.str(),"ALERT");

		return false;
	}

	if (fabs(_cp.TotalExec) >= fabs(_cp.TotalQty))
	{
		//_algostate.CIRCUITBREAKER = true;
		if (_ENABLED) Enable(false);
		usleep(2 * 1000 * 1000);

		ostringstream oss;
		oss << "*** TOTAL EXECUTED EXCEEDED: " << _cp.TotalExec << " vs MAX:" << _cp.TotalQty ;
		LogMe(oss.str(), LogLevel::ERROR);

		usleep(100000);
		AddReportingMsg(oss.str(), "ALERT");

		return false;
	}
	return true;
}

//squeezer logic
void MultiSpreaderV3::CheckSqueeze()
{
	for (int i = 0; i < _cp.LegCount; i++)
	{
		if (_cp.SqueezeRatio == 0)
			continue;

		CheckSqueeze(i);
	}
}

void MultiSpreaderV3::CheckSqueeze(int index)
{
	if (_cp.SqueezeRatio == 0) return;
	if (!GotMktData()) return;

	//first, let's get the quote- it's the cheapest thing to do
	instrument& leg = _cp.Legs[index];
	KOrderSide::Enum side = (leg.spreadmult > 0) ? KOrderSide::BUY : KOrderSide::SELL;
	DepthBook md;
	GetQuoteByIndex(index, md);

	double ratio = (side == KOrderSide::BUY) ? (md.BidSize / (double)md.AskSize)
														: (md.AskSize / (double)md.BidSize);

	int mktsize = (side == KOrderSide::BUY) ? md.AskSize : md.AskSize;

	//next, if ratio < squeeze ratio, split/continue; we're not interested.
	if (ratio < _cp.SqueezeRatio && mktsize > _cp.SqueezeQty) return;

	//ok, if we get here, we know we have to do something.... if there are orders
	vector<KTN::OrderPod> ords;
	_Ords.GetByAlgoTrigger(ords, KOrderAlgoTrigger::ALGO_HEDGE);

	if (ords.size() == 0) return;

	//we know if we get here, we've got unfinished business, so let's roll.
	int price = (side == KOrderSide::BUY) ? md.Ask : md.Bid;

	int icnt = 0;
	for(auto& ord : ords)
	{
		if (Squeezed(ord.orderReqId))
			continue;

		string orig = ord.ordernum;

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

	string logmsg = "SQUEEZE MODS=" + std::to_string(icnt) + " -->ratio=" + std::to_string(ratio)
						+ " BIDSZ=" + std::to_string(md.BidSize)
						+ " ASKSZ=" + std::to_string(md.AskSize);
	LogMe(logmsg, LogLevel::INFO);
	AddReportingMsg(logmsg,"ALERT");

}

bool MultiSpreaderV3::Squeezed(uint64_t reqid)
{
	for(uint64_t id : _SqueezedOrds)
	{
		if (id == reqid)
			return true;
	}

	return false;
}

} /* namespace KTN */
