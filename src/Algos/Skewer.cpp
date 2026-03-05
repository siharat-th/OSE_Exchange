	/*
 * Skewer.hpp
 *
 *  Created on: Dec 27, 2023
 *      Author: sgaer
 */


#include "Skewer.hpp"

namespace KTN {

Skewer::Skewer(AlgoInitializer p): AlgoBaseV3(p), _QuoteOrdPool(100),_timer(*this)
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

	LogMe("INITING SLACK...", LogLevel::INFO);
	_SLACKER.InitAndRun();

	_TIMER_ELAPSED = false;
}

Skewer::~Skewer()
{
	if (_timer.Started())
		_timer.Stop();

	 if (_STATUS_ACTIVE.load(std::memory_order_relaxed) == true) {
		 _STATUS_ACTIVE.store(false, std::memory_order_relaxed);
		_statusthread.join();
	}
}

void Skewer::onLaunch(std::string json) {

	cout << json << endl;

	_cp = SkewerParams::parse(json);
	UpdateMetaParams(_cp.meta);

	LoadSpreadLegDefs(json);

	_statusthread = std::thread(&Skewer::StatusThread, this);

	onJson(json);
}

void Skewer::onJson(std::string json) {

	LogMe("*****" + m_Name + " PARAMS JSON", LogLevel::INFO);
	cout << "[" << __CLASS__ << " DEBUG] onJson: " << json << endl;;

	cout << "*****" + m_Name + " PARAMS JSON" << endl;
	cout << json << endl;

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

	if (_cp.meta.Enabled == 0 )
	{
		string ts = (_timer.Started()) ? "TRUE" : "FALSE";
		LogMe("ENABLED=FALSE. TIMER STARTED=" + ts);
		//we pause here, just in case...
		if (_timer.Started())
		{
			_timer.Pause(true);
		}

		_ENABLED = false;

		Enable(false);
		return;
	}
	_cp.StartTime 	= doc["StartTime"].GetString();
	_cp.EndTime 	= doc["EndTime"].GetString();

	_cp.IntervalSecs= doc["IntervalSecs"].GetInt();

	_cp.TotalQty 	= doc["TotalQty"].GetDouble();
	_cp.OrderQty 	= doc["OrderQty"].GetDouble();

	_cp.SqueezeRatio= doc["SqueezeRatio"].GetDouble();
	_cp.SqueezeQty 	= doc["SqueezeQty"].GetInt();
	_cp.Randomize 	= doc["Randomize"].GetBool();

	_cp.Aggressive 		= doc["Aggressive"].GetBool();
	_cp.SkewThreshold 	= doc["SkewThreshold"].GetDouble();
	_cp.SkewLevels 		= doc["SkewLevels"].GetInt();
	_cp.SkewMax 		= doc["SkewMax"].GetDouble();
	_cp.SkewMaxQty 		= doc["SkewMaxQty"].GetInt();


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


	string ts = (_timer.Started()) ? "TRUE" : "FALSE";
	LogMe("ENABLE=TRUE. TIMER STARTED=" + ts);



	if (!_timer.Started())
	{
		_timer.Start(_cp.StartTime, _cp.EndTime, _cp.IntervalSecs, 0);
	}
	else
	{

		LogMe("UPDATING TIMER INTERVAL");
		_timer.UpdateInterval(_cp.IntervalSecs);

		_timer.Pause(false);
	}

}

void Skewer::LoadSpreadLegDefs(string json)
{
//	_quoter.initialize( _cp.meta.Tag, _cp.meta.Strategy, _cp.meta.Source, _GUID,
//			_cp.meta.userinfo, _EXCH_CALLBACK_ID);


	_cp.LegCount = 0;

	for (auto ln : _LEGS)
	{
		if (ln.iscme)
		{
			RegisterCmeSecId(ln);
		}

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

		_quoter[_cp.LegCount].Construct(_EXCH_CALLBACK_ID, ln, _PARAMS);

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

void Skewer::OrderReceived(KTN::Order& incoming, int idx)
{
	//DBG_FILE_LINE
	cout << "INCOMING ORD:" +  OrderUtils::Print(incoming) << endl;
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

vector<string> Skewer::Symbols()
{
	vector<string> symbs;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument leg = _cp.Legs[i];
		symbs.push_back(leg.symbol);
	}

	return symbs;
}

void Skewer::Print()
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

	::AlgoBaseV3::Print();

	PrintMdMap();

	PrintBookMap();

	double skew = CalcSkew(_cp.Legs[0], KOrderSide::SELL );
	oss.str("");
	oss.clear();
	oss << "-------- " << _cp.Legs[0].symbol << ":  BOOK SKEW[S]=" << skew << "  --------";
	LogMe(oss.str(),LogLevel::INFO);

}

void Skewer::StatusThread()
{
	int secs = 2 * 1000 * 1000;
	_STATUS_ACTIVE.store(true, std::memory_order_relaxed);
	while(_STATUS_ACTIVE.load(std::memory_order_relaxed) == true)
	{
		ostringstream oss;
		for (int i = 0; i < _cp.LegCount; i++)
		{
			//first, let's get the quote- it's the cheapest thing to do
			instrument& leg = _cp.Legs[i];

			KOrderSide::Enum side = (leg.spreadmult > 0) ? KOrderSide::BUY : KOrderSide::SELL;
			DepthBook md = GetQuote(leg);

			double skew = CalcSkew(leg, side);

			double avgpx = _cp.AvgBuyPrice;
			// if (leg.spreadmult < 0)
			// 	_cp.AvgSellPrice;

			oss << leg.symbol << ": TOT=" << _cp.TotalExec
					<< " SKEW=" << std::setprecision(6)<< skew
			<< " Avg=" << std::setprecision(4) << avgpx;

			if (i < _cp.LegCount -1)
				oss << "; ";

		}
		AddReportingMsg(oss.str(), "INFO");
		usleep(secs);
	}
}

void Skewer::ReportStatus()
{
	ostringstream oss;
	for (int i = 0; i < _cp.LegCount; i++)
	{

		//first, let's get the quote- it's the cheapest thing to do
		instrument& leg = _cp.Legs[i];

		KOrderSide::Enum side = (leg.spreadmult > 0) ? KOrderSide::BUY : KOrderSide::SELL;
		//DepthBook md = GetQuote(leg);

		double skew = CalcSkew(leg, side);

		oss << leg.symbol << ": TOT=" << _cp.TotalExec << " SKEW=" << skew;

		if (i < _cp.LegCount -1)
			oss << "; ";

	}

//	std::string status = (_cp.meta.Enabled == 0) ? "DISABLED" : "ENABLED";
//
//	ostringstream oss;
//	oss << "POS: " << _cp.NetPosition ;
//
//	for (int i = 0; i < _cp.LegCount; i++)
//	{
//		instrument leg = _cp.Legs[i];
//		oss << "; " << leg.symbol << ":" << leg.totalexec;
//
//	}

	AddReportingMsg(oss.str(), "INFO");
}

void Skewer::AlertCheck()
{
	double d = .90;

	if (fabs(_cp.NetPosition) >= ( d *fabs(_cp.TotalQty)))
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

void Skewer::ResetPosition()
{
	LogMe("RESETTING POSITION!!!", LogLevel::INFO);
	_cp.NetPosition = 0;
	_cp.TotalExec = 0;
	_NETPOS = 0;
	_TOTALEXEC = 0;
	_CIRCUITBREAKER = false;

	_cp.Reset = false;
	_cp.meta.Enabled = false;

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

	//ik, if we reset, we will stop the timer
	_timer.Stop();

	Enable(false);
	_Ords.ClearInflight();
}

void Skewer::Restart()
{
}

void Skewer::Enable(bool bEnable)
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

		CancelAllOrders();
		ClearLegSentIds();

		string ts = (_timer.Started()) ? "TRUE" : "FALSE";
		LogMe("ENABLED=FALSE. TIMER STARTED=" + ts);
		//we pause here, just in case...
		if (_timer.Started())
		{
			_timer.Pause(true);
		}

		LogMe("GOT DISABLE COMMAND", LogLevel::WARN);

		ReportStatus();
	}
}

void Skewer::CancelAllOrders()
{
//	LogMe("CANCEL ALL DISABLED!!!! FOR TEST ONLY!!!", LogLevel::ERROR);
//	return;
	bCancelAllSent = true;

	vector<KTN::OrderPod> ords;
	//_Ords.GetByStratId(ords, _PARAMS.StratId);

	_Ords.GetByAlgoTrigger(ords, KOrderAlgoTrigger::ALGO_LEG);

	ostringstream oss;
	oss << "CXL ALL LEGS RESULT=" << ords.size() << " STRATID=" << _PARAMS.StratId;
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

		_Ords.onSentOrderCxl(cxl);
		icnt++;
		//LogMe("SENDING POD CXL: " + OrderUtils::Print(cxl));
		_ordersender->Send(cxl, KOrderAction::ACTION_CXL);
	}
	LogMe("CXL ALL SENT " + std::to_string(icnt) + " CANCELS!!!", LogLevel::WARN);

}

//calulation section....
void Skewer::onTimerStateChange(TimerState::Enum state)
{
	if (state == TimerState::WAIT)
		LogMe("TIMER STATE CHANGE: WAIT",LogLevel::INFO);
	if (state == TimerState::START)
		LogMe("BANG BANG!!! TIMER START EVENT!",LogLevel::INFO);
	if (state == TimerState::END)
		LogMe("TIMER STATE: END!",LogLevel::WARN);
}

void Skewer::onTimerInterval(TimerInterval::Enum interval)
{
	if (interval == TimerInterval::TRIGGERED)
		SendOnTimer();

//	if (interval == TimerInterval::MAXSECS)
//		SendOnExpired();

}

void Skewer::SendOnTimer()
{
	LogMe("TIMER INTERVAL TRIGGERED. CHECKING SKEW!",LogLevel::INFO);
	_TIMER_ELAPSED = true;

}

//We don't really need this anymore..
void Skewer::SendOnExpired()
{
}

void Skewer::CheckSqueeze(int index)
{
	if (_cp.SqueezeQty == 0 || _cp.SqueezeRatio == 0) return;
	if (!GotMktData()) return;

	vector<KTN::OrderPod> ords;
	instrument& leg = _cp.Legs[index];
	KOrderSide::Enum side = (leg.spreadmult > 0) ? KOrderSide::BUY : KOrderSide::SELL;
	if (!Working(ords, side, leg))
		return;

	DepthBook md;
	GetQuoteByIndex(index, md);

	int icnt = 0;
	for(auto& ord : ords)
	{
		if (Squeezed(ord.orderReqId))
			continue;

		int mktsize = (ord.OrdSide == KOrderSide::BUY) ? md.AskSize : md.BidSize;
		int price = (ord.OrdSide == KOrderSide::BUY) ? md.Ask : md.Bid;

		//double ratio = (ord.OrdSide == KOrderSide::BUY) ? (md.BidSize / (double)md.AskSize)
		//							: (md.AskSize / (double)md.BidSize);

//		if (ratio < _cp.SqueezeRatio)
//			continue;

		if (mktsize > _cp.SqueezeQty)
			continue;

		KTN::OrderPod neword = ord;
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

	string logmsg = "SQUEEZE MODS=" + std::to_string(icnt) //+ " -->ratio=" + std::to_string(ratio)
						+ " BIDSZ=" + std::to_string(md.BidSize)
						+ " ASKSZ=" + std::to_string(md.AskSize);
	LogMe(logmsg, LogLevel::INFO);
	AddReportingMsg(logmsg,"ALERT");


}

void Skewer::onCalculate() {

	if (!GoodToGo())
		return;

	//prevent multiple orders from being entered for same price
	QuoteState::Enum origstate = _QuotingState;
	ChangeQuoteState(QuoteState::Calculating);

	if (_TIMER_ELAPSED)
		CheckSkew();

	ChangeQuoteState(origstate);

}

void Skewer::onMktData(int index)
{
	CheckSqueeze(index);
}

void Skewer::CheckSkew()
{
	for (int i = 0; i < _cp.LegCount; i++)
	{

		//first, let's get the quote- it's the cheapest thing to do
		instrument& leg = _cp.Legs[i];

		KOrderSide::Enum side = (leg.spreadmult > 0) ? KOrderSide::BUY : KOrderSide::SELL;
		DepthBook md = GetQuote(leg);

		double skew = CalcSkew(leg, side);

		//for show/display
		_SKEW = skew;

//		ostringstream oss;
//		oss << leg.symbol << ": SKEW=" << skew << " vs " << _cp.SkewThreshold << ";"
//				<< " MKT=" << md.Bid << "@" << md.Ask << " " << md.BidSize << "/" << md.AskSize;
//
//		LogMe(oss.str());

		double testskew = (side == KOrderSide::BUY) ? skew : -1 * skew;

		if (testskew <= _cp.SkewThreshold || testskew >= _cp.SkewMax) continue;

		//let's roll

		LogMe("LET'S ROLL!", LogLevel::INFO);

		int price = (side == KOrderSide::BUY) ? md.Bid : md.Ask;// md.Ask : md.Bid;

		if (_cp.Aggressive)
			price =  (side == KOrderSide::BUY) ? md.Ask : md.Bid;

		int qty = (fabs(skew) >= _cp.SkewMax) ? _cp.SkewMaxQty : _cp.OrderQty;

		SendOnSkew(side, qty, price, leg);

	}
}

double Skewer::CalcSkew(const instrument& leg, KOrderSide::Enum side)
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

//	if (side == KOrderSide::SELL)
//		skew *= -1;

	return skew;
}

void Skewer::SendOnSkew(KOrderSide::Enum side, int qty, int price, instrument& leg)
{
	//RESET TIMERS FROM THIS POINT BC INTERVAL IS NOW RELATIVE
	_timer.ResetInterval();
	_TIMER_ELAPSED = false;

	DepthBook md = GetQuote(leg);

	//KOrderSide::Enum side = (leg.spreadmult > 0) ? KOrderSide::BUY : KOrderSide::SELL;
	//int price = (side == KOrderSide::BUY) ? md.Bid : md.Ask;

	if (!RiskCheckQty(_cp.OrderQty, _cp.TotalQty, _cp.TotalExec, side))
		return;

	//first check distance between bid/ask-- if > 1 tick, go to 1 tick above bid/below ask
	int bidask_spread = md.Ask - md.Bid;
	if (bidask_spread > leg.mintick){
		LogMe("PRICE ADJUSTMENT: SPREAD IS > MINTICK: BIDASK=" + std::to_string(bidask_spread) + " MINTICK=" + std::to_string(leg.mintick),LogLevel::WARN);
		price = (side == KOrderSide::BUY) ? GetAsk(leg.index) - leg.mintick : GetBid(leg.index) + leg.mintick;
		KTN::OrderPod ord = {};
		PrepareNewOrder(ord, leg, side, price, _cp.OrderQty);

		SendOrder(ord, leg);
		return;
	}

	//next, check to see if we instantly squeeze:
	if (_cp.SqueezeQty > 0)
	{
		double ratio = (side == KOrderSide::BUY) ? (md.BidSize / (double)md.AskSize)
							: (md.AskSize / (double)md.BidSize);

		int mktsize = (side == KOrderSide::BUY) ? md.AskSize : md.BidSize;

		if (mktsize < _cp.SqueezeQty)
		{
			price = (side == KOrderSide::BUY) ? md.Ask : md.Bid;
			KTN::OrderPod ord = {};
			PrepareNewOrder(ord, leg, side, price, _cp.OrderQty);

			SendOrder(ord, leg);
			_SqueezedOrds.push_back(ord.orderReqId);

			string logmsg = "onTimer SQUEEZE INSTANT->ratio=" + std::to_string(ratio)
						+ " BIDSZ=" + std::to_string(md.BidSize)
						+ " ASKSZ=" + std::to_string(md.AskSize);
			LogMe(logmsg, LogLevel::WARN);

			return;
		}
	}

	//if we get here, what we do is we check to see if we're already working more than x orders. If so, we just hit
	//the best quote with this order and leave the existing order to work.
	//theoretically, we can leave all them to work and squeeze, and we may do that,
	//but the idea is to accumulate

	vector<KTN::OrderPod> ords;
	Working(ords, side, leg);

	if (ords.size() > 0)
	{
		LogMe("FOUND " + std::to_string(ords.size()) + " ORDERS ON BOOK.");
		//*****Note: this will build orders at a price and squeeze ALL of them. Make sure this is what we want.
		//*****replace this with a setting:
		if (ords.size() >= 2)
		{
			LogMe("  -->HITTING BEST AND LEAVING " + std::to_string(ords.size())+ " ORDERS TO AGE!");
			price = (side == KOrderSide::BUY) ? md.Ask : md.Bid;
		}

		KTN::OrderPod ord = {};
		PrepareNewOrder(ord, leg, side, price, _cp.OrderQty);
		bool ok =  RiskCheckQty(ord.quantity, _cp.TotalQty, _cp.TotalExec, ord.OrdSide);
		if (ok) SendOrder(ord, leg);
		return;
	}



	KTN::OrderPod ord = {};
	PrepareNewOrder(ord, leg, side, price, _cp.OrderQty);
	SendOrder(ord, leg);

	ostringstream oss;
	oss << leg.symbol << " SENT SKEW ORDER: SKEW="
			<< _SKEW << " ORD QTY=" << qty << " PRICE=" << price;
	LogMe(oss.str(),LogLevel::INFO);

}

void Skewer::ModifyOrder(KTN::OrderPod& ord, instrument& leg, int price)
{
	KTN::OrderPod neword = ord;
	//_quoter[leg.index].Get(neword);
	string newid = _quoter[leg.index].NextId();
	OrderUtils::fastCopy(neword.ordernum, newid.c_str(), sizeof(neword.ordernum));
	neword.quantity = ord.leavesqty;
	neword.price = price;
	neword.OrdAction = KOrderAction::ACTION_MOD;
	neword.OrdStatus = KOrderStatus::sent_mod;
	neword.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_SQZ;

	SendOrder(neword, leg);
}

void Skewer::MoveAllToPrice(instrument& leg, KOrderSide::Enum& side, int price)
{
	vector<KTN::OrderPod> ords;
	Working(ords, side, leg);

	for (auto& ord : ords)
	{
		if (Squeezed(ord.orderReqId))
			continue;

		if (ord.OrdSide != side)
			continue;

		string orig = ord.ordernum;
		ModifyOrder(ord, leg, price);

		string logmsg = "MISSED SQUEEZE!!! MODIFYING " + orig + "->new-price=" + std::to_string(price);
		LogMe(logmsg, LogLevel::INFO);
	}
}

bool Skewer::Working(vector<KTN::OrderPod>& ords, KOrderSide::Enum side, instrument& leg)
{
	//bool found = false;
	ords.clear();

	_Ords.GetByAlgoTriggerSecId(ords, KOrderAlgoTrigger::ALGO_LEG, leg.secid);

	//cout << "WORKING GOT " << ords.size() << " FOR SECID=" << secid <<  " TRIG=" << KOrderAlgoTrigger::toString(trig) << endl;

	return (ords.size() > 0);
}



void Skewer::PrepareNewOrder(KTN::OrderPod &ord, const instrument& inst,
		KOrderSide::Enum side, int price, int qty)
{
	if (_MEASURE) _trk.startMeasurement("QuoteNextNew");
	_quoter[inst.index].NextNew(ord, side, 0, qty, price);
	if (_MEASURE) _trk.stopMeasurement("QuoteNextNew");
	_msgcnt[inst.index].news++;
}

void Skewer::PrepareModOrder(KTN::OrderPod &ord, const instrument& inst,
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

//Has the order in question already been modified by a squeeze?
bool Skewer::Squeezed(uint64_t reqid)
{
	for(uint64_t id : _SqueezedOrds)
	{
		if (id == reqid)
			return true;
	}

	return false;
}

void Skewer::ToggleCommand(int id)
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

void Skewer::QuickOrdPrint(const KTN::Order& ord, const instrument& inst)
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



void Skewer::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent)
{
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

	double newqty = ord.lastqty;

	_cp.TotalExec += newqty;
	_TOTALEXEC += newqty;
	_cp.NetPosition += newqty;

	if (ord.OrdSide == KOrderSide::BUY)
	{
		_cp.TotalBuys += ord.lastqty;
		_cp.TotalBuyPrice += (ord.lastqty * ord.lastpx);
		_cp.AvgBuyPrice = _cp.TotalBuyPrice / _cp.TotalBuys;
	}

	if (ord.OrdSide == KOrderSide::SELL)
	{
		_cp.TotalSells += ord.lastqty;
		_cp.TotalSellPrice += (ord.lastqty * ord.lastpx);
		_cp.AvgSellPrice = _cp.TotalSellPrice / _cp.TotalSells;
	}

	UpdateLegState(ord.orderReqId, ord.OrdState);
	//bool ok =  RiskCheckQty(ord.lastqty, _cp.TotalQty, _cp.TotalExec, ord.OrdSide);

	slackmsg msg = {};
	msg.color = LogLevel::INFO;
	msg.preview = "ORDER FILLED @ " + std::to_string(ord.lastpx);
	msg.text = KOrderAlgoTrigger::toString(ord.OrdAlgoTrig) + " ORDER FILLED: "
			+ KOrderSide::toString(ord.OrdSide)+ " " +  std::to_string(ord.lastqty)
			+ " @ " + std::to_string(ord.lastpx) + "\nTOTAL=" + std::to_string(_cp.TotalExec)
			+ "/" + std::to_string(_cp.TotalQty);
	_SLACKER.Send(msg);


	if (_cp.TotalExec >= _cp.TotalQty)
	{
		LogMe("DISABLING: QTY EXEC=" + std::to_string(_cp.TotalExec)
				+ " >= TOTAL ALLOWED:" + std::to_string(_cp.TotalQty),LogLevel::ERROR);
		Enable(false);

		if (_timer.Started())
			_timer.Stop();
	}

}

void Skewer::OrderMinUpdate(KTN::OrderPod& ord)
{
	UpdateLegState(ord.orderReqId, ord.OrdState);
}

void Skewer::UpdatePositionOnFill(const KTN::Order& ord)
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

bool Skewer::GoodToGo()
{
	if (!_ENABLED ) {
		return false;
	}

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


	if (_QuotingState == QuoteState::Calculating)
		return false;

	ChangeQuoteState(QuoteState::ReadyToSendOrders);

	return true;
}

bool Skewer::GotMktData()
{
	if (LIKELY(_GOT_DATA)) return true;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument leg = _cp.Legs[i];
		DepthBook sym = GetQuote(leg);
		if ((sym.Bid == 0 && sym.Ask == 0) || sym.BidSize == 0 || sym.AskSize == 0)
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
} /* namespace KTN */
