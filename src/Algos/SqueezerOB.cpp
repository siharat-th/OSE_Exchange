/*
 * MktMaker.cpp
 *
 *  Created on: Apr 24, 2021
 *      Author: sgaer
 */

#include <Algos/SqueezerOB.hpp>

namespace KTN {

SqueezerOB::SqueezerOB(AlgoInitializer p)
		: AlgoBaseV3(p)
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

SqueezerOB::~SqueezerOB() {

}

void SqueezerOB::onLaunch(std::string json) {
	//load json params to your custom param struct or whatevs
	_cp = SqueezerParams::parse(json);
	UpdateMetaParams(_cp.meta);

	_mdh->SetHandleTrades(true);

	//OB is always enabled....
	//for an OB algos, we send the json to onJson b/c we have an order
	_cp.meta.Enabled = 1;
	LogMe("INITIAL SQUEEZE- SENDING onJSON FROM onLaunch...", LogLevel::INFO);
	onJson(json);

}

void SqueezerOB::onJson(std::string json) {
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

	//check if we already own this symbol:
	bool foundleg = false;
	for(int i = 0; i < _cp.LegCount; i++)
	{
		if (incoming.symbol == _cp.Legs[i].symbol)
		{
			incoming.instindex = i;
			foundleg = true;
			break;
		}
	}

	if (!foundleg)
	{
		instrument ln = {};
		AddLeg(incoming, ln);
	}

	_mdh->SetHandleTrades(true);


#ifdef DEBUG_TESTING
	ostringstream oss;
	oss << "INCOMING ORD: " << OrderUtils::Print(incoming) << "\nLEG COUNT=" << _cp.LegCount;
	LogMe(oss.str(), LogLevel::WARNING);
#endif

	_CIRCUITBREAKER = false;
	_CAN_TRADE = true;

	for (int i = 0; i < _cp.LegCount; i++)
	{
		if (incoming.symbol == _cp.Legs[i].symbol)
		{
			incoming.instindex = _cp.Legs[i].index;
			incoming.mktsegid = _cp.Legs[i].mktsegid;

			SqzOrdParam sq = {};
			sq.sqzprice = (int)first_element["SqzPrice"].GetDouble();
			sq.sqzqty = (int)first_element["SqzQty"].GetDouble();

			//new-ish
			sq.sqzratio = first_element["SqzRatio"].GetDouble();
			sq.largetrade = first_element["LargeTradeQty"].GetInt();

			sq.side = incoming.OrdSide;
			sq.index = i;
			sq.symbol = _cp.Legs[i].symbol;
			sq.secid = _cp.Legs[i].secid;

			if (sq.sqzprice == 0)
			{
				if (sq.side == KOrderSide::BUY)
					sq.sqzprice = GetAsk(_cp.Legs[i].index);
				if (sq.side == KOrderSide::SELL)
					sq.sqzprice = GetBid(_cp.Legs[i].index);
			}

			LogMe("SQZ ORDER RECEIVED: SIDE=" + KOrderSide::toString(sq.side) + " SQZPX=" + std::to_string(sq.sqzprice));

			KTN::OrderPod ord = {};
			_quoter[i].NextNew(ord, incoming.OrdSide, ord.exchordid,
					incoming.quantity, incoming.price);

			_ordersender->Send(ord, KOrderAction::ACTION_NEW);
			_Ords.onSentOrderFast(ord);
			_sqzMap[ord.orderReqId] = sq;
			_reqMap[ord.ordernum] = ord.orderReqId;

			break;
		}
	}
}


void SqueezerOB::Print()
{
	LogMe("\n****" + m_Name + "****\n", LogLevel::INFO);
	PrintMdMap();

	_trk.printMeasurements();

	ostringstream oss;
	oss << "***SQZMAP SIZE=" << _sqzMap.size();

	LogMe(oss.str(), LogLevel::INFO);

	if (_sqzMap.size() > 0)
	{
		int i = 0;
		for(auto kv : _sqzMap)
		{
			ostringstream oss;
			oss <<"  " << i << ": CLORDID=" << kv.first << " " << KOrderSide::toString(kv.second.side)
					<< " SYMB=" << kv.second.symbol << " PX=" << kv.second.sqzprice;
			LogMe(oss.str());
		}
	}

	::AlgoBaseV3::Print();

}

void SqueezerOB::OrderMinUpdate(KTN::OrderPod& ord)
{

	if (ord.OrdState == KOrderState::COMPLETE)
	{
		if (_sqzMap.find(ord.orderReqId) != _sqzMap.end())
				_sqzMap.erase(ord.orderReqId);
	}

}

void SqueezerOB::OrderReceived(KTN::Order& incoming, int idx)
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

		LogMe("SQZ SENT MANUAL MOD ORDER: " + OrderUtils::Print(ord), LogLevel::WARN);
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
		if (_sqzMap.find(ord.orderReqId) != _sqzMap.end())
			_sqzMap.erase(ord.orderReqId);
		//MapSqz(ord, KOrderAction::ACTION_CXL, *sqz);
	}


	ChangeQuoteState(QuoteState::SentOrders);
}

void SqueezerOB::MapSqz(const KTN::OrderPod & ord, int action, const SqzOrdParam& sq)
{
	_sqzMap[ord.orderReqId] = sq;

	if (action == KOrderAction::ACTION_CXL)
	{
		if (_sqzMap.find(ord.orderReqId) != _sqzMap.end())
			_sqzMap.erase(ord.orderReqId);
		return;
	}

	_sqzMap[ord.orderReqId] = sq;

}

void SqueezerOB::ReportStatus()
{
	//std::string status = (_cp.meta.Enabled == 0) ? "DISABLED" : "ENABLED";

}

void SqueezerOB::ResetPosition()
{
	LogMe("RESETTING POSITION!!!", LogLevel::INFO);
	_CIRCUITBREAKER = false;
	ReportStatus();
}

void SqueezerOB::Restart()
{

}

void SqueezerOB::Enable(bool bEnable)
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


void SqueezerOB::onCalculate() {
}

void SqueezerOB::onMktData(int index)
{
	if (!GoodToGo(index))
			return;
	if (_sqzMap.size() == 0)
			return;
	//_trk.startMeasurement("SqzCheck");
	for (auto it = _sqzMap.begin(); it != _sqzMap.end(); ++it)
	{
		if (it->second.index == index)
		{
			bool triggered = CheckSqz(index, it->second);
			if (triggered)
			{
				SqueezeSender(index, it->first, it->second.sqzprice, it->second.trig);
				_sqzMap.erase(it->first);
			}
		}
	}
	//_trk.stopMeasurement("SqzCheck");
}


void SqueezerOB::onTrade(int index, double price, double size, int aggressor)
{
	if (!GoodToGo(index))
		return;
	if (_sqzMap.size() == 0)
		return;

	for (auto it = _sqzMap.begin(); it != _sqzMap.end(); ++it)
	{
		if (it->second.index == index)
		{
			if (size >= it->second.largetrade && price == it->second.sqzprice)
			{

				SqueezeSender(index, it->first, it->second.sqzprice, KOrderAlgoTrigger::ALGO_SQZ_LARGETRD);
				_sqzMap.erase(it->first);
				//LogMe("LARGE TRADE ALERT!!! SQUEEZING!",LogLevel::WARNING);
			}
		}
	}

}


bool SqueezerOB::CheckSqz(int index, SqzOrdParam& sq)
{
	if (!GoodToGo(index))
		return false;

	//instrument& leg = _cp.Legs[index];
	DepthBook md = {};
	GetQuoteByIndex(index, md);


	if (sq.side == KOrderSide::BUY)
	{
//		ostringstream oss;
//			oss << "CHECKING BUY SQUEEZING ON ORDER " << ord.ordernum + " SYMB=" << ord.symbol << " SIDE="
//					<< ord.side << " SZQPRICE=" << sq.sqzprice << " SQZ_QTY=" << sq.sqzqty << " vs " << md.AskSize << " @ " << md.Ask;
//
//			LogMe(oss.str());


		if (md.Ask >= sq.sqzprice)
		{
			if (md.AskSize < sq.sqzqty){
				sq.trig = KOrderAlgoTrigger::ALGO_SQZ_QTY;
				return true;
			}

			if (md.Ask > sq.sqzprice){
				sq.trig = KOrderAlgoTrigger::ALGO_SQZ_PRICEJUMP;
				return true;
			}
		}
	}

	if (sq.side == KOrderSide::SELL)
	{
//		ostringstream oss;
//		oss << "CHECKING SELL SQUEEZING ON ORDER " << ord.ordernum + " SYMB=" << ord.symbol << " SIDE="
//				<< ord.side << " SZQPRICE=" << sq.sqzprice << " SQZ_QTY=" << sq.sqzqty << " vs " << md.AskSize << " @ " << md.Ask;
//
//		LogMe(oss.str());
	//	LogMe("QTY SQZ!! BIDSIZE=" + std::to_string(md.BidSize) + " vs " + std::to_string(sq.sqzqty));

		if (md.Bid <= sq.sqzprice)
		{
			if (md.BidSize < sq.sqzqty){
				sq.trig = KOrderAlgoTrigger::ALGO_SQZ_QTY;
				return true;
			}

			if (md.Bid < sq.sqzprice)
			{
				sq.trig = KOrderAlgoTrigger::ALGO_SQZ_PRICEJUMP;
				return true;
			}

		}
	}

	return false;
}

void SqueezerOB::SqueezeSender(int index, uint64_t reqid, int sqz_price, KOrderAlgoTrigger::Enum trig)
{
	KTN::OrderPod inc = {};
	_Ords.Get(inc, reqid);

	if (inc.exchordid == 0)
	{
		LogMe("ERROR ON ALGO MOD!! THIS ORDER CAN NOT BE MODIFIED!!! NOT FOUND: "
				+ OrderUtils::toString(inc.ordernum), LogLevel::ERROR);
		return;
	}

	KTN::OrderPod ord = {};
	_quoter[index].NextMod(ord, inc.OrdSide, reqid, inc.exchordid, inc.quantity, sqz_price);

	ord.OrdAlgoTrig = trig;
	//here we use the good old send method-- may convert later
	_ordersender->Send(ord, KOrderAction::ACTION_MOD);
	_Ords.onSentOrderFast(ord);

	LogMe("SENT SQZ! TRIG=" + KOrderAlgoTrigger::toString(ord.OrdAlgoTrig),LogLevel::WARN);

}

void SqueezerOB::ToggleCommand(int id)
{

}


void SqueezerOB::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) {

	_Ords.minOrderExecution(ord, hedgeSent);

	ostringstream oss;
	oss << "CmeID=" <<  ord.exchordid << " SECID=" << ord.secid << " SIDE="
			<< ord.OrdSide << " QTY=" << ord.lastqty << " PX=" << ord.lastpx;
	LogMe("**MINFILL FOR: " + oss.str(), LogLevel::INFO);

//	_bQuoteInFlight = _Ords.HasInflight();

	//double newqty = ord.lastqty;

	ChangeQuoteState(QuoteState::ReadyToRefresh);

	if (ord.OrdStatus == KOrderStatus::FILLED)
	{
		if (_sqzMap.find(ord.orderReqId) != _sqzMap.end())
			_sqzMap.erase(ord.orderReqId);
	}
}


bool SqueezerOB::GoodToGo(int index)
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

bool SqueezerOB::GotMktData(int index)
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
			LogMe(oss.str(), LogLevel::WARN);
			AddReportingMsg(oss.str(),"ALERT");
		}
		_cp.Legs[index].gotdata = false;
		return false;
	}

	_cp.Legs[index].gotdata = true;

	return true;
}

void SqueezerOB::AddLeg(const KTN::Order& ord, instrument& ln)
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

} /* namespace KTN */
