	/*
 * StackerTimer.hpp
 *
 *  Created on: Jan 22, 2024
 *      Author: sgaer
 */


#include "StackerTimer.hpp"

namespace KTN {

StackerTimer::StackerTimer(AlgoInitializer p): AlgoBaseV3(p), _QuoteOrdPool(100),_timer(*this)
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

	_mdh->SetHandleTrades(true);

}

StackerTimer::~StackerTimer()
{
	if (_timer.Started())
		_timer.Stop();
}

void StackerTimer::onLaunch(std::string json) {

	_cp = StackerTimerParams::parse(json);
	UpdateMetaParams(_cp.meta);

	LoadSpreadLegDefs(json);

	onJson(json);
}

void StackerTimer::onJson(std::string json) {

	//LogMe("*****" + m_Name + " PARAMS JSON", LogLevel::INFO);
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

	_cp.meta.Enabled = doc["Enabled"].GetInt();

	if (_cp.meta.Enabled == 0)
	{
		string ts = (_timer.Started()) ? "TRUE" : "FALSE";
		LogMe("ENABLED=FALSE. TIMER STARTED=" + ts);
		//we pause here, just in case...
		if (_timer.Started())
		{
			_timer.Pause(true);
		}

		_ENABLED = false;
		_CAN_TRADE = false;

		Enable(false);
		return;
	}

	_cp.CloseStartTime 	= doc["CloseStartTime"].GetString();
	_cp.CloseEndTime 	= doc["CloseEndTime"].GetString();
	_cp.CoverAtClose 	= doc["CoverAtClose"].GetBool();

	_cp.StackLevels 	= doc["StackLevels"].GetInt();
	_cp.StackOffsetTicks 	= doc["StackOffsetTicks"].GetInt();
	_cp.StackSide 		= doc["StackSide"].GetString();
	_cp.OrdSide 		= (_cp.StackSide == "B") ? KOrderSide::BUY : KOrderSide::SELL;

	_cp.LargeTradeQty 	= doc["LargeTradeQty"].GetInt();
	_cp.PullOnLargeTrade = doc["PullOnLargeTrade"].GetBool();

	_cp.PukeQty 	= doc["PukeQty"].GetInt();
	_cp.SqueezeRatio = doc["SqueezeRatio"].GetDouble();
	_cp.PukeRatio 	= doc["PukeRatio"].GetDouble();
	_cp.EnablePuke 	= doc["EnablePuke"].GetBool();

	_cp.TotalQty 	= doc["TotalQty"].GetInt();
	_cp.OrderQty 	= doc["OrderQty"].GetInt();

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

	if (_timer.Expired())
	{
		Enable(false);
		LogMe("ERROR: TIMER HAS EXPIRED!!!! NO STACKING ALLOWED!!!",LogLevel::ERROR);
		return;
	}

	Enable(true);
	_CIRCUITBREAKER = false;
	_CAN_TRADE = true;

	string ts = (_timer.Started()) ? "TRUE" : "FALSE";
	LogMe("ENABLE=TRUE. TIMER STARTED=" + ts);

	if (_cp.CoverAtClose)
	{
		if (!_timer.Started())
			_timer.Start(_cp.CloseStartTime, _cp.CloseEndTime);
		else
			_timer.Pause(false);
	}
	else
	{
		if (_timer.Started())
			_timer.Pause(false);
	}


}

void StackerTimer::LoadSpreadLegDefs(string json)
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

void StackerTimer::OrderReceived(KTN::Order& incoming, int idx)
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

vector<string> StackerTimer::Symbols()
{
	vector<string> symbs;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		instrument leg = _cp.Legs[i];
		symbs.push_back(leg.symbol);
	}

	return symbs;
}

void StackerTimer::Print()
{
	LogMe("****" + m_Name + " INFO ****", LogLevel::WARN);

	_Ords.Print();

	PrintMdMap();
}

void StackerTimer::ReportStatus()
{

}

void StackerTimer::AlertCheck()
{

}

void StackerTimer::ResetPosition()
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

void StackerTimer::Restart()
{
}

void StackerTimer::Enable(bool bEnable)
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
		_CIRCUITBREAKER = true;
		_ENABLED = false;
		_CAN_TRADE = false;
		//CancelAllOrders();
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

void StackerTimer::CancelAllOrders()
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

void StackerTimer::CancelOrder(const KTN::OrderPod& ord)
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

//calulation section....
void StackerTimer::onTimerStateChange(TimerState::Enum state)
{
	if (state == TimerState::WAIT)
		LogMe("TIMER STATE CHANGE: WAIT",LogLevel::INFO);
	if (state == TimerState::START){
		LogMe("BANG BANG!!! TIMER START EVENT!",LogLevel::INFO);
		//here- first, we cancel all the inferior priced stacks
		//then, we move BEST ORDER ONLY- if need be- to be on the bid until timer expires,
		//then cover balance
	}

	if (state == TimerState::END){
		LogMe("TIMER STATE: END!",LogLevel::WARN);
		//cover balance of unexecuted single order
	}
}

void StackerTimer::onTimerInterval(TimerInterval::Enum interval)
{
//	if (interval == TimerInterval::TRIGGERED)
//		SendOnTimer();

//	if (interval == TimerInterval::MAXSECS)
//		SendOnExpired();

}

void StackerTimer::SendStackOrder(int legindex, int price)
{
	LogMe("STACK ORDER SENDING!",LogLevel::INFO);


		KTN::OrderPod ord = {};

		instrument& leg = _cp.Legs[legindex];

		if(!RiskCheckQty(_cp.OrderQty, _cp.TotalQty, _cp.TotalExec, ord.OrdSide))
			return;

		PrepareNewOrder(ord, leg, _cp.OrdSide, price, _cp.OrderQty);
		SendOrder(ord, leg);


//		slackmsg msg = {};
//		msg.color = LogLevel::WARNING;
//		msg.preview = "ALERT: NEW STACK ORDER ENTERED";
//		msg.text = "SENDING NEW STACK " + KOrderAlgoTrigger::toString(ord.OrdAlgoTrig)
//				+ " ORDER: " + KOrderSide::toString(_cp.OrdSide) + " "
//				+ std::to_string(ord.quantity) + " @ " + std::to_string(ord.price);
//
//		_SLACKER.Send(msg);

}

//We don't really need this anymore..
void StackerTimer::SendOnExpired()
{
}

void StackerTimer::CheckSqueeze()
{
	for (int i = 0; i < _cp.LegCount; i++)
	{
		if (_cp.SqueezeRatio == 0)
			continue;

		//first, let's get the quote- it's the cheapest thing to do
		instrument& leg = _cp.Legs[i];

		KOrderSide::Enum side = (leg.spreadmult > 0) ? KOrderSide::BUY : KOrderSide::SELL;
		DepthBook md = GetQuote(leg);
		double ratio = (side == KOrderSide::BUY) ? (md.BidSize / (double)md.AskSize)
															: (md.AskSize / (double)md.BidSize);

		//next, if ratio < squeeze ratio, split/continue; we're not interested.
		if (ratio < _cp.SqueezeRatio) continue;

		//ok, if we get here, we know we have to do something.... if there are orders
		vector<KTN::OrderPod> ords;
		Working(ords, side, leg);

		if (ords.size() == 0) continue;

		//we know if we get here, we've got unfinished business, so let's roll.
		int price = (side == KOrderSide::BUY) ? md.Ask : md.Bid;

		for (auto& ord : ords)
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
			neword.price = price;
			neword.OrdAction = KOrderAction::ACTION_MOD;
			neword.OrdStatus = KOrderStatus::sent_mod;
			neword.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_SQZ;

			SendOrder(neword, leg);

			_SqueezedOrds.push_back(ord.orderReqId);

			string logmsg = "SQUEEZE MODIFYING " + orig + "->ratio=" + std::to_string(ratio)
						+ " BIDSZ=" + std::to_string(md.BidSize)
						+ " ASKSZ=" + std::to_string(md.AskSize)
						+ " NEWCLORDID=" + OrderUtils::toString(ord.ordernum);
			LogMe(logmsg, LogLevel::INFO);



			slackmsg msg = {};
			msg.color = LogLevel::WARN;
			msg.preview = "ALERT: SQUEEZE TRIGGERED";
			msg.text =logmsg;
			_SLACKER.Send(msg);
		}

	}
}

void StackerTimer::onCalculate() {

	if (!GoodToGo())
		return;

	//prevent multiple orders from being entered for same price
	QuoteState::Enum origstate = _QuotingState;
	ChangeQuoteState(QuoteState::Calculating);

	//this should take care of best bid/ask pukes
	if (_cp.EnablePuke)
		CheckPuke();

	for (int i = 0; i < _cp.LegCount; i++)
	{
		//get quote and some fundamentals
		instrument& leg = _cp.Legs[i];
		DepthBook md = GetQuote(leg);

		int secid = leg.secid;

		int mintick = leg.mintick;
		int mult =  (_cp.OrdSide == KOrderSide::BUY) ? -1 : 1;

		int pxadj = mult * (_cp.StackOffsetTicks * mintick);
		int nbbo = (_cp.OrdSide == KOrderSide::BUY) ? md.Bid : md.Ask;
		int startprice = nbbo + pxadj;


		bool sent = false;
		for(int lvl = 0 ; lvl < _cp.StackLevels; lvl++){
			int price = (lvl * mult * mintick) + startprice;

			//ok, if we get here, we know we have to do something.... if there are orders
			vector<KTN::OrderPod> ords;
			if(!_Ords.GetAtPrice(ords, secid, _cp.OrdSide,
					price, KOrderAlgoTrigger::ALGO_LEG))
			{
				SendStackOrder(i, price);
				sent = true;
			}
		}

		if (sent){
			//now cull the extras...
			vector<KTN::OrderPod> ords;
			Working(ords, _cp.OrdSide, leg);
			if (static_cast<int>(ords.size()) > _cp.StackLevels)
			{
				int i = 0;
				for(auto ord : ords)
				{
					i++;
					if (i > _cp.StackLevels)
					{
						ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_CXL;
						CancelOrder(ord);
					}
				}
			}
		}
	}

	ChangeQuoteState(origstate);
}

void StackerTimer::onTrade(int index, double price, double size, int aggressor)
{
	if (!GoodToGo())
		return;

	if (_cp.PullOnLargeTrade && size >= _cp.LargeTradeQty)
	{
		//LogMe("LARGE TRADE: " + std::to_string(size) + " @ " + std::to_string(price) + " AGG=" + std::to_string(aggressor), LogLevel::WARNING);
		instrument inst = _cp.Legs[index];
		PukeAtPrice(inst.secid, price, KOrderAlgoTrigger::ALGO_LARGE_TRADE_KILL);
	}
}

void StackerTimer::CheckPuke()
{
	for (int i = 0; i < _cp.LegCount; i++)
	{
		//get quote and some fundamentals
		instrument& leg = _cp.Legs[i];

		DepthBook md = GetQuote(leg);
		double ratio = (_cp.OrdSide == KOrderSide::BUY) ? (md.BidSize / md.AskSize)
															: (md.AskSize / md.BidSize);

		int price = (_cp.OrdSide == KOrderSide::BUY) ? md.Bid : md.Ask;

		if (ratio < _cp.PukeRatio){
			PukeAtPrice(leg.secid, price, KOrderAlgoTrigger::ALGO_PUKE);
		}

	}
}

void StackerTimer::PukeAtPrice(int32_t secid, int price, KOrderAlgoTrigger::Enum trig)
{
	vector<KTN::OrderPod> ords;
	if(_Ords.GetAtPrice(ords, secid, _cp.OrdSide,
						price, KOrderAlgoTrigger::ALGO_LEG))
	{
		LogMe(KOrderAlgoTrigger::toString(trig) + " PUKING AT " + std::to_string(price)
				+ " ORDS=" + std::to_string(ords.size()),LogLevel::WARN);
		for(auto ord : ords)
		{
			ord.OrdAlgoTrig = trig;
			CancelOrder(ord);
		}
	}
}

void StackerTimer::Working(vector<KTN::OrderPod>& ords, KOrderSide::Enum& side, const instrument& leg)
{
	//bool found = false;
	ords.clear();

	vector<KTN::OrderPod> temp;
	_Ords.GetByAlgoTrigger(temp, KOrderAlgoTrigger::ALGO_LEG);

	for(auto ord : temp)
	{
		if (ord.OrdState != KOrderState::WORKING || ord.orderReqId == 0)
			continue;
		ords.push_back(ord);
	}
}



void StackerTimer::PrepareNewOrder(KTN::OrderPod &ord, const instrument& inst,
		KOrderSide::Enum side, int price, int qty)
{
	if (_MEASURE) _trk.startMeasurement("QuoteNextNew");
	_quoter[inst.index].NextNew(ord, side, 0, qty, price);
	if (_MEASURE) _trk.stopMeasurement("QuoteNextNew");
	_msgcnt[inst.index].news++;
}

void StackerTimer::PrepareModOrder(KTN::OrderPod &ord, const instrument& inst,
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
bool StackerTimer::Squeezed(uint64_t reqid)
{
	for(uint64_t id : _SqueezedOrds)
	{
		if (id == reqid)
			return true;
	}

	return false;
}

void StackerTimer::ToggleCommand(int id)
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

void StackerTimer::QuickOrdPrint(const KTN::Order& ord, const instrument& inst)
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



void StackerTimer::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent)
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


void StackerTimer::OrderMinUpdate(KTN::OrderPod& ord)
{
	UpdateLegState(ord.orderReqId, ord.OrdState);
}

void StackerTimer::UpdatePositionOnFill(const KTN::Order& ord)
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

bool StackerTimer::GoodToGo()
{
	if (!_ENABLED || !_CAN_TRADE || _CIRCUITBREAKER ) {
		return false;
	}

	if (!_GOT_DATA)
	{
		for (int i = 0; i < _cp.LegCount; i++)
		{
			instrument leg = _cp.Legs[i];
			DepthBook sym = GetQuote(leg);
			if (sym.Bid == 0 && sym.Ask == 0)
			{
				ostringstream oss;
				oss << "DATA ERROR " << leg.symbol << ": " << sym.Bid << "@" <<  sym.Ask;
				LogMe(oss.str(), LogLevel::WARN);
				_GOT_DATA = false;
				return false;
			}
		}
		_GOT_DATA = true;
		LogMe("GOT DATA FOR ALL LEGS! GOOD TO GO.", LogLevel::INFO);
	}

	if (fabs(_cp.TotalExec) >= fabs(_cp.TotalQty))
	{
		_CIRCUITBREAKER = true;
		if (_ENABLED) Enable(false);
		usleep(2 * 1000 * 1000);

		ostringstream oss;
		oss << "*** TOTAL EXECUTED EXCEEDED: " << _cp.TotalExec << " vs MAX:" << _cp.TotalQty ;
		LogMe(oss.str(), LogLevel::ERROR);

		usleep(100000);
		AddReportingMsg(oss.str(), "ALERT");

		return false;
	}

	if (_timer.Expired())
		return false;

	if (!_ENABLED)
		return false;

	ChangeQuoteState(QuoteState::ReadyToSendOrders);

	return true;
}
} /* namespace KTN */
