/*
 * MktMaker.cpp
 *
 *  Created on: Apr 24, 2021
 *      Author: sgaer
 */

#include <Algos/PLWatcher.hpp>

namespace KTN {

PLWatcher::PLWatcher(AlgoInitializer p)
		: AlgoBaseV3(p)
{
	_HANDLER_STARTED = false;
	
	_USE_REPORTING_MSGS = true;
	_ENABLED = false;
	_QuotingState = QuoteState::TimerTick;
	_GOT_DATA = false;
	_CNT = 0;
	_WATCH_TRADES = true;

	_CIRCUITBREAKER = false;
	_CAN_TRADE = true;
}

PLWatcher::~PLWatcher()
{
//	if (_STATUS_ACTIVE.load(std::memory_order_relaxed) == true) {
//		 _STATUS_ACTIVE.store(false, std::memory_order_relaxed);
//		_statusthread.join();
//	}

//	if (_CHECKER_ACTIVE.load(std::memory_order_relaxed) == true) {
//		_CHECKER_ACTIVE.store(false, std::memory_order_relaxed);
//		_checkerthread.join();
//	}

	_plm->Stop();
}

void PLWatcher::onLaunch(std::string json) {
	_cp = PLWatcherParams::parse(json);
	UpdateMetaParams(_cp.meta);

	_mdh->SetHandleTrades(false);

	//OB is always enabled....
	//for an OB algos, we send the json to onJson b/c we have an order
	_cp.meta.Enabled = 1;

//	_statusthread = std::thread(&PLWatcher::StatusThread, this);
//	_checkerthread = std::thread(&PLWatcher::CheckPositions, this);

	_plm = new PLMonitor(*this, _GUID);
	_plm->Start();

	onJson(json);
}

void PLWatcher::onJson(std::string json) {
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

	const auto& first_element = d[0];

	string plid = "PL-" + IdGen::UUID(6);

	Position pos = {};
	pos.algoid = _GUID;
	pos.posid = plid;
	pos.risk.ProfitTaking = first_element["ProfitTaking"].GetDouble();
	pos.risk.StopLoss = first_element["StopLoss"].GetDouble();
	pos.risk.MinutesToLive = first_element["MinutesToLive"].GetInt();
	pos.source = first_element["Source"].GetString();

	if (!first_element.HasMember("Orders") || !first_element["Orders"].IsArray()) {
		LogMe("EACH ELEMENT MUST CONTAIN 'Orders' ARRAY!!!!", LogLevel::ERROR);
		LogMe(json, LogLevel::ERROR);
		return;
	}

	const auto& first_order = first_element["Orders"][0];
	KTN::Order incoming = OrderParser::parseorder(first_order);

	const auto& orders = first_element["Orders"];
	for (SizeType i = 0; i < orders.Size(); i++) {
		const auto& order = orders[i];

		KTN::Order incoming = OrderParser::parseorder(order);
		//check if we already own this symbol:
		bool foundleg = false;
		int legindex = -1;
		for(int i = 0; i < _cp.LegCount; i++)
		{
			if (incoming.symbol == _cp.Legs[i].symbol)
			{
				legindex = i;
				foundleg = true;
				break;
			}
		}

		if (!foundleg)
		{
			instrument ln = {};
			AddLeg(incoming, ln);

			ln.spreadmult = 1;
			ln.pricemult = 1;

			legindex = ln.index;
		}

		incoming.instindex = _cp.Legs[legindex].index;
		incoming.mktsegid = _cp.Legs[legindex].mktsegid;

		cout << "LEG INDEX=" << legindex << " SEGID=" << (int)incoming.mktsegid << endl;

		PositionLeg pleg = {};

		pleg.leg = _cp.Legs[legindex];
		pleg.leg.spreadmult = incoming.OrdSide;
		pleg.index = legindex;
		pleg.price = incoming.price;
		pleg.netpos = incoming.OrdSide * incoming.quantity;
		pleg.side = incoming.OrdSide;

		pos.addleg(pleg);

	}

	LogMe("RECEIVED PL TO ADD TO WATCH:");

	ostringstream oss;
	oss << pos.posid << ": pt=" << pos.risk.ProfitTaking
			<< " sl=" << pos.risk.StopLoss << " ttl=" << pos.risk.MinutesToLive;
	LogMe(oss.str(), LogLevel::INFO);

	for(auto kv : pos.legs)
	{
		auto leg = kv.second;
		oss.clear();
		oss.str("");
		oss << "  -->" << leg.leg.symbol << " " << KOrderSide::toString(leg.side)
				<< " " << leg.netpos <<" @ " << leg.price;
		LogMe(oss.str());
	}

	_plm->Register(pos);
}

void PLWatcher::onEngineJson(EngineCommand& cmd)
{
	LogMe("APPLYING ENGINE COMMAND!");
	_plm->AdjustRisk(cmd);
}

void PLWatcher::Print()
{
	LogMe("\n****" + m_Name + "****\n", LogLevel::INFO);
	PrintMdMap();

	//_trk.printMeasurements();

	::AlgoBaseV3::Print();


	_plm->Print();
}

void PLWatcher::OrderMinUpdate(KTN::OrderPod& ord)
{
	/*if (ord.OrdState == KOrderState::COMPLETE)
	{
		if (_sqzMap.find(ord.orderReqId) != _sqzMap.end())
				_sqzMap.erase(ord.orderReqId);
	}*/

}

void PLWatcher::OrderReceived(KTN::Order& incoming, int idx)
{
	//SqzOrdParam* sqz = dynamic_cast<SqzOrdParam*>(&params);

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
		return;
//		KTN::OrderPod ord = {};
//		_quoter[idx].NextNew(ord, incoming.OrdSide, ord.exchordid,
//				incoming.quantity, incoming.price);
//
//		_ordersender->Send(ord, KOrderAction::ACTION_NEW);
//		_Ords.onSentOrderFast(ord);
//		MapSqz(ord, KOrderAction::ACTION_NEW, *sqz);
//		_reqMap[ord.ordernum] = ord.orderReqId;

//#ifdef DEBUG_TESTING
//		LogMe("SQZ SENT NEW ORDER: " + OrderUtils::Print(ord), LogLevel::WARNING);
//#endif
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

		LogMe("PLWATCH SENT MANUAL MOD ORDER: " + OrderUtils::Print(ord), LogLevel::WARN);
		_ordersender->Send(ord, KOrderAction::ACTION_MOD);
		_Ords.onSentOrderFast(ord);
		//MapSqz(ord, KOrderAction::ACTION_MOD, NULL);
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

		/*if (_sqzMap.find(ord.orderReqId) != _sqzMap.end())
			_sqzMap.erase(ord.orderReqId);*/
		//MapSqz(ord, KOrderAction::ACTION_CXL, *sqz);
	}


	ChangeQuoteState(QuoteState::SentOrders);
}



void PLWatcher::ReportStatus()
{
	//std::string status = (_cp.meta.Enabled == 0) ? "DISABLED" : "ENABLED";

}

void PLWatcher::ResetPosition()
{
	LogMe("RESETTING POSITION!!!", LogLevel::INFO);
	_CIRCUITBREAKER = false;
	ReportStatus();
}

void PLWatcher::Restart()
{

}

void PLWatcher::Enable(bool bEnable)
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
	}
}


void PLWatcher::onCalculate() {
}

void PLWatcher::onMktData(int index)
{
	if (!GoodToGo())
			return;
	DepthBook md;
	GetQuoteByIndex(index, md);
	_plm->onMarketData(md);
}


void PLWatcher::onTrade(int index, double price, double size, int aggressor)
{
//	return;
//
//	if (!GoodToGo(index))
//		return;
//	if (_sqzMap.size() == 0)
//		return;
//
//	for (auto it = _sqzMap.begin(); it != _sqzMap.end(); ++it)
//	{
//		if (it->second.index == index)
//		{
//			if (size >= it->second.largetrade && price == it->second.sqzprice)
//			{
//				SqueezeSender(index, it->first, it->second.sqzprice);
//				_sqzMap.erase(it->first);
//				LogMe("LARGE TRADE ALERT!!! SQUEEZING!",LogLevel::WARNING);
//			}
//		}
//	}

}

void PLWatcher::onPL(Position& pos, ProfitEvent event)
{
	for(auto kv : pos.legs)
	{
		KTN::OrderPod ord = {};
		PositionLeg posleg = kv.second;

		DepthBook md;
		//bool got = GetQuote(posleg.leg, md);


		KOrderSide::Enum side = (posleg.side == KOrderSide::BUY) ? KOrderSide::SELL : KOrderSide::BUY;
		int price = (side == KOrderSide::BUY) ? md.Ask : md.Bid;

		_quoter[posleg.index].NextNew(ord, side, 0, fabs(posleg.netpos), price);
		ord.OrdAlgoTrig = (event == ProfitEvent::PROFITTAKE) ? KOrderAlgoTrigger::ALGO_PROFIT
				: KOrderAlgoTrigger::ALGO_STOPLOSS;

		if (event == ProfitEvent::TIMERHIT)
			ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_TIMEREXP;

		if (event == ProfitEvent::MKT)
			ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_GOTO_MKT;

		_ordersender->Send(ord, KOrderAction::ACTION_NEW);
		_Ords.onSentOrderFast(ord);

//		ostringstream oss;
//		oss << KOrderAlgoTrigger::toString(ord.OrdAlgoTrig) << ": " <<
//		Notify()
	}
}


void PLWatcher::ToggleCommand(int id)
{

}


void PLWatcher::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) {

	_Ords.minOrderExecution(ord, hedgeSent);

	ostringstream oss;
	oss << "CmeID=" <<  ord.exchordid << " SECID=" << ord.secid << " SIDE="
			<< ord.OrdSide << " QTY=" << ord.lastqty << " PX=" << ord.lastpx;
	LogMe("**PLWATCH FILL FOR: " + oss.str(), LogLevel::INFO);

//	_bQuoteInFlight = _Ords.HasInflight();

	//double newqty = ord.lastqty;

	ChangeQuoteState(QuoteState::ReadyToRefresh);

	/*if (ord.OrdStatus == KOrderStatus::FILLED)
	{
		if (_sqzMap.find(ord.orderReqId) != _sqzMap.end())
			_sqzMap.erase(ord.orderReqId);
	}*/
}

void PLWatcher::AddLeg(const KTN::Order& ord, instrument& ln)
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

bool PLWatcher::GoodToGo()
{
	if (!_ENABLED ) {
		return false;
	}

	if (!GotMktData()) return false;

	ChangeQuoteState(QuoteState::ReadyToSendOrders);

	return true;
}

bool PLWatcher::GotMktData()
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
