//============================================================================
// Name        	: Waves.cpp
// Author      	: sgaer
// Version     	:
// Copyright   	: Copyright (C) 2024 Katana Financial
// Date			: Apr 17, 2024 11:33:56 AM
//============================================================================


#include <Algos/Waves.hpp>

namespace KTN {

Waves::Waves(AlgoInitializer p)
		: AlgoBaseV3(p),_hdgpool(5000), _QuoteOrdPool(625)
{
	_HANDLER_STARTED = false;
	_USE_REPORTING_MSGS = true;
	_SHOW_QUOTES = false;
	_ENABLED = false;
	_QuotingState = QuoteState::TimerTick;
	_GOT_DATA = false;
	_PL_STARTED = false;
	_CNT = 0;
	_GOT_CNT = 0;
	_TESTID = 0;

	_WATCH_TRADES = false;
	_HDGTYPE = HedgeType::COMPLEX;
}

Waves::~Waves() {
	if (_STATUS_ACTIVE.load(std::memory_order_relaxed) == true) {
			 _STATUS_ACTIVE.store(false, std::memory_order_relaxed);
			_statusthread.join();
		}
}

void Waves::onLaunch(std::string json) {
	//load json params to your custom param struct or whatevs
	_cp = WavesParams::parse(json);
	UpdateMetaParams(_cp.meta);

	_rows = 0;
	_cols = 0;
	_cp.LegCount = 0;

	BuildMatrix(_cp.Product, _cp.StartMonth, _cp.StartYear, _cp.NumMonths);

	ostringstream oss;
	oss << "CREATED GRAPH WITH " << _rows << " ROWS AND " << _cols << " COLS. TOTAL LEGS=" << _cp.LegCount;
	LogMe(oss.str(),LogLevel::INFO);

	_ENABLED = false;

	_statusthread = std::thread(&Waves::StatusThread, this);

	onJson(json);
}


void Waves::BuildMatrix(string product, string month,int year, int nummonths)
{
	int iStep = 1;

	MatrixGen mtx;
	//mtx.GenOutrightMatrix(product, month, year, nummonths);
	mtx.GenSpreadMatrix(product, month, year, nummonths);

	map<pair<int, int>, std::string> outmap = mtx.GetGraphMap();
	//uint8_t idx = 0;

	_rows = 0;
	_cols = 0;

	for (int n = 0; n < nummonths; n+=iStep)
	{
//		string symbol = outmap[std::make_pair(n,n)];
//		RegisterLegByRowCol(n, n, KOrderProdType::FUTURE, symbol);
		_rows ++;

		for (int j = n+iStep; j < nummonths; j+=iStep)
		{
			_cols++;
			string symbol = outmap[std::make_pair(n,j)];
			RegisterLegByRowCol(n, j, KOrderProdType::SPREAD, symbol);
		}
	}

}

void Waves::RegisterLegByRowCol(int n, int j, KOrderProdType::Enum prodtype,
		string symbol)
{
	//string symbol = graph[std::make_pair(n,j)];

	instrument ln = LegParamParser::GetCmeLeg(symbol, prodtype, _cp.LegCount);

	RegisterCmeSecId(ln);


	ln.callbackid = _EXCH_CALLBACK_ID;

	ln.row = n;
	ln.col = j;
	ln.istas = false;
	ln.index = _cp.LegCount;

	_LEGS.push_back(ln);

	_cp.curveMatrix[n][j].exists = true;
	_cp.curveMatrix[n][j].index = _cp.LegCount;

	_quoter[_cp.LegCount].Construct(_EXCH_CALLBACK_ID, ln, _PARAMS);
	_cp.Legs[ln.index] = ln;
	_cp.LegSecIdMap[ln.secid] = ln;

	_msgcnt[_cp.LegCount] = {};

	_cp.LegCount++;

	int cnt = _cp.LegCount;
	ostringstream oss;
	oss << "ADDED MATRIX LEG [" << ln.row << "," << ln.col <<"]: IDX=" << ln.index <<" SYMB: " << ln.symbol
			<< " SECID=" << ln.secid << " CBID=" << _EXCH_CALLBACK_ID << " INDEX=" << ln.index << " TOTAL=" << cnt;
	LogMe(oss.str(),LogLevel::INFO);

}

void Waves::onJson(std::string json) {

	//cout << "[" << __CLASS__ << " DEBUG] onJson: " << json << endl;;

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
		LogMe("ENABLED=FALSE");

		_ENABLED = false;
		_CAN_TRADE = false;

		Enable(false);
		return;
	}

	_cp.BidEdge = doc["BidEdge"].GetInt();
	_cp.AskEdge = doc["AskEdge"].GetInt();

	_cp.OrderQty = doc["OrderQty"].GetDouble();
	_cp.TotalQty = doc["TotalQty"].GetDouble();

	_cp.SendBids = doc["SendBids"].GetBool();
	_cp.SendAsks = doc["SendAsks"].GetBool();

	_cp.MinWidth = doc["MinWidth"].GetInt();
	_cp.MaxWidth = doc["MaxWidth"].GetInt();

	_cp.Doubles = doc["Doubles"].GetBool();
	_cp.Triples = doc["Triples"].GetBool();
	_cp.Quads = doc["Quads"].GetBool();

	_cp.SqueezeQty = (int)doc["SqueezeQty"].GetDouble();
	_cp.SqueezeRatio = doc["SqueezeRatio"].GetDouble();


	_cp.WavgThreshold = doc["WavgThreshold"].GetDouble();

	_cp.TestMode = doc["TestMode"].GetBool();

	_cp.Reset = false;
	if (doc.HasMember("ResetPosition"))
		_cp.Reset = doc["ResetPosition"].GetBool();

	if (_cp.Reset)
	{
		LogMe("*** GOT RESET COMMAND!!! ***", LogLevel::WARN);
		ResetPosition();
		return;
	}


	_SENTCNT = 0;
	_OK_TO_TRADE = true;
	ChangeQuoteState(QuoteState::ReadyToRefresh);

	_GROUPCNT ++;
	_SPRDID = IdGen::UUID(6);
	_CIRCUITBREAKER = false;
	_CAN_TRADE = true;

	UpdateMetaParams(_cp.meta);

	Enable(true);
	_CIRCUITBREAKER = false;
	_CAN_TRADE = true;

}


void Waves::onEngineJson(EngineCommand& cmd)
{
}


void Waves::Print()
{
	LogMe("\n****" + m_Name + "****\n", LogLevel::INFO);
	::AlgoBaseV3::Print();

	PrintMdMap();

	LogMe("-----INSTRUMENT MATRIX-----", LogLevel::INFO);
	for (int i = 0; i < _cp.LegCount; i++)
	{
		//std::pair<int,int> pr = _cp.IndexToPathMap[i];

		instrument leg = _cp.Legs[i];

		string istas = (_cp.Legs[i].istas) ? "TRUE" : "FALSE";

		ostringstream oss;
		oss << "[" << leg.row << "," << leg.col << "] " << leg.symbol;
		LogMe(oss.str());
	}

	for (int i = 0; i < _cp.MAX_SIZE; i++){
		for(int j= i; j < _cp.MAX_SIZE; j++)
		{
			if (!_cp.curveMatrix[i][j].exists)
				continue;
			ostringstream oss;
			oss << "CURVE: i=" << i << " j=" << j << " INDEX=" << (int)_cp.curveMatrix[i][j].index
					<< " SYMB=" << _cp.Legs[ _cp.curveMatrix[i][j].index].symbol;
			LogMe(oss.str(),LogLevel::INFO);
		}
	}

	DoublePrint();
}


void Waves::StatusThread()
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

			int secid = leg.secid;

			int bq = _Ords.GetSideExposure(secid, KOrderSide::BUY, KOrderAlgoTrigger::ALGO_LEG);
			int sq = _Ords.GetSideExposure(secid, KOrderSide::SELL, KOrderAlgoTrigger::ALGO_LEG);

			oss << leg.symbol << ": TOT=" << _cp.TotalExec <<" OPEN B=" << bq << " S=" << sq;

			if (i < _cp.LegCount -1)
				oss << "; ";
		}
		AddReportingMsg(oss.str(), "INFO");
		usleep(secs);
	}
}

void Waves::ReportStatus()
{
	ostringstream oss;
	for (int i = 0; i < _cp.LegCount; i++)
	{

		//first, let's get the quote- it's the cheapest thing to do
		instrument& leg = _cp.Legs[i];

		//KOrderSide::Enum side = (leg.spreadmult > 0) ? KOrderSide::BUY : KOrderSide::SELL;
		DepthBook md = GetQuote(leg);

		oss << leg.symbol << ": TOT=" << _cp.TotalExec;

		if (i < _cp.LegCount -1)
			oss << "; ";
	}

	AddReportingMsg(oss.str(), "INFO");

}

void Waves::ResetPosition()
{
	LogMe("RESETTING POSITION!!!", LogLevel::INFO);

	Enable(false);
	_cp.TotalExec = 0;


	for(int i = 0; i < _cp.LegCount; i++)
	{
		instrument& leg = _cp.Legs[i];
		leg.lastSentBuyPrice = 0;
		leg.lastBuyId = 0;
		leg.buyState = KOrderState::INFLIGHT;
		leg.lastSentBuyPrice = 0;
		leg.lastBuyId = 0;
		leg.buyState = KOrderState::INFLIGHT;
	}

	ReportStatus();
}

void Waves::Restart()
{
//	LogMe("RESTARTING ALGO...", LogLevel::INFO);
//	Enable(false);
//	usleep(1000000);
//	Enable(true);

}

void Waves::Enable(bool bEnable)
{
	_ENABLED = bEnable;
	_PARAMS.Enabled = _ENABLED;

	if (_ENABLED)
	{
		ReportStatus();
		ChangeQuoteState(QuoteState::ReadyToRefresh);

		for (int i = 0; i < _cp.LegCount; i++)
		{
			instrument& leg = _cp.Legs[i];
			//LogMe("RESETTING SLOP VARIABLES", LogLevel::WARNING);
			leg.lastAsk = 0;
			leg.lastBid = 0;
			leg.lastSentSellPrice = 0;
			leg.lastSentBuyPrice = 0;
		}
		LogMe("ENABLED!",LogLevel::INFO);
		//_Ords.ClearInflight();
		//_Ords.ClearDeadOrders();
	}
	else
	{
		_ENABLED = false;
		ChangeQuoteState(QuoteState::Disabled);
		CancelAllOrders();

		LogMe("GOT DISABLE COMMAND", LogLevel::WARN);

		ReportStatus();
	}
}

void Waves::onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid,KOrderSide::Enum side,
					uint32_t qty, int32_t price, uint16_t instindex)
{
	/*
	 * Here, we got notified by an exchange handler that it has hedged a fill.
	 * We need to generate an OrderPod new order that corresponds
	 * and send it to the OrderManager.
	 */

	int index = -1;

	for(int i = 0; i < _cp.LegCount; i++)
	{
		if (_cp.Legs[i].secid == secid)
		{
			index = i;
			break;
		}
	}

	if (index < 0)
	{
		string szerr = "ALGO onHedgeOrderSent: ERROR! CANNOT FIND INDEX FOR SECID=" + std::to_string(secid);
		LogMe(szerr,LogLevel::ERROR);
		AddReportingMsg(szerr, "ALERT");
		return;
	}


	ChangeHedgeState(HedgeState::ActiveHedges);

	ostringstream oss;
	oss << "ALGO onHedgeOrderSent: " << KOrderSide::toString(side)
							<< " " <<  qty << " s=" <<  _cp.LegSecIdMap[secid].symbol
							<< " @ " << price << " idx=" << index
							<< " secid=" << secid //<< " symb=" << _cp.Legs[index].symbol//<< " clordid=" << clordid
							<< " reqid=" << reqid;
	LogMe(oss.str(),LogLevel::INFO);


	OrderPod ord = {};
	_quoter[index].NextNew(ord, side, 0, qty, price);

//	ord.secid = secid;
//	ord.price = price;
//	ord.quantity = qty;
//	ord.leavesqty = qty;


	ord.orderReqId = reqid;
	ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_HEDGE;
	OrderUtils::fastCopy(ord.ordernum, clordid, sizeof(ord.ordernum));

	//LogMe("SENDING HDG TO V5: " + OrderUtils::Print(ord));

	_Ords.onSentOrderFast(ord);

}

void Waves::onOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
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


void Waves::onCalculate() {

	if (!GoodToGo()) return;

	if (_Ords.HasInflight())
	{
		//cout << "INFLIGHT=TRUE" << endl;
		return;
	}


	for (int n = 0; n < _cp.NumMonths; n++){
		for(int j = n+1; j < _cp.NumMonths; j++){

			if (_cp.Doubles && ((j - n) == 2))
			{
				//cout << "SENDING n=" << n << " j=" << j << endl;
				if (DoubleCalc(n,j))
					return;
			}
		}
	}

}

bool Waves::DoubleCalc(int n, int j)
{
	int index = _cp.curveMatrix[n][j].index;
	//double
	DepthBook dbl = {};
	GetQuoteByIndex(index, dbl);

	if (dbl.Bid == dbl.Ask)
		return false;

	DepthBook md1 = {};
	GetQuoteByIndex(_cp.curveMatrix[n][n+1].index, md1);

	DepthBook md2 = {};
	GetQuoteByIndex(_cp.curveMatrix[n+1][j].index, md2);

	double wavg = WavgBidAsk(md1) + WavgBidAsk(md2);

	bool sentbid = false;
	bool sentask = false;

//	ostringstream oss;
//	oss << _cp.Legs[index].symbol << "(" << n << "," << j << "): "
//			<< _cp.Legs[_cp.curveMatrix[n][n+1].index].symbol << "("
//			<< n << "," << n+1 << ")"
//			<< "+" << _cp.Legs[_cp.curveMatrix[n+1][j].index].symbol << "("
//						<< n+1 << "," << j << ")"
//			<<" WAVG=" << wavg << " | DBL BID=" << dbl.Bid <<" ASK=" << dbl.Ask;


	if (_cp.SendBids && wavg >= (dbl.Ask + _cp.WavgThreshold)
			&& (dbl.Ask != _cp.Legs[index].lastSentBuyPrice))
	{
		//ahaha buy - but we sell hedges AT ASK and squeeze!!!
		SpreadHedgeV3 hdg1 = GetHedgeLeg(_cp.curveMatrix[n][n+1].index, KOrderSide::SELL, md1.Ask, -1);
		SpreadHedgeV3 hdg2 = GetHedgeLeg(_cp.curveMatrix[n+1][j].index, KOrderSide::SELL, md2.Ask, -1);
		_hdgVecS.clear();
		_hdgVecS.push_back(hdg1);
		_hdgVecS.push_back(hdg2);

		//if(UNLIKELY(_cp.TestMode)){
			//LogMe(oss.str());
			//LogMe(_cp.Legs[index].symbol +" BAM! SEND BUY @ " + std::to_string(dbl.Ask),LogLevel::INFO);
		//}
		//else
			_cp.Legs[index].lastSentBuyPrice = dbl.Ask;

		if (!_cp.TestMode){
			sentbid = SendQuote(_cp.Legs[index], KOrderSide::BUY, dbl.Ask, _cp.OrderQty, _hdgVecS);
			if (sentbid) return true;
		}

	}

	if (_cp.SendAsks && wavg <= (dbl.Bid - _cp.WavgThreshold)
			&& (dbl.Bid != _cp.Legs[index].lastSentSellPrice))
	{
		//ahaha SELL- But we buy hedges AT BID and squeeze!!!
		SpreadHedgeV3 hdg1 = GetHedgeLeg(_cp.curveMatrix[n][n+1].index, KOrderSide::BUY, md1.Bid, 1);
		SpreadHedgeV3 hdg2 = GetHedgeLeg(_cp.curveMatrix[n+1][j].index, KOrderSide::BUY, md2.Bid, 1);
		_hdgVecB.clear();
		_hdgVecB.push_back(hdg1);
		_hdgVecB.push_back(hdg2);

		//if(UNLIKELY(_cp.TestMode)){
			//LogMe(oss.str());
			//LogMe(_cp.Legs[index].symbol +" BAM! SEND SELL @" + std::to_string(dbl.Bid),LogLevel::WARNING);
		//}
		//else
			_cp.Legs[index].lastSentSellPrice = dbl.Bid;

		if (!_cp.TestMode){
			sentask = SendQuote(_cp.Legs[index], KOrderSide::SELL, dbl.Bid, _cp.OrderQty, _hdgVecB);
			if (sentask) return true;
		}
	}

	return false;
}

void Waves::DoublePrint()
{
	LogMe("---------- WAVG PRINTER: DOUBLES ---------", LogLevel::INFO);
	for (int n = 0; n < _cp.NumMonths; n++)
	{
		for(int j = n+1; j < _cp.NumMonths; j++)
		{
			if (_cp.Doubles && ((j - n) == 2))
			{
				int index = _cp.curveMatrix[n][j].index;
				//double
				DepthBook dbl = {};
				GetQuoteByIndex(index, dbl);

				if (dbl.Bid == dbl.Ask)
					continue;



				DepthBook md1 = {};
				GetQuoteByIndex(_cp.curveMatrix[n][n+1].index, md1);

				DepthBook md2 = {};
				GetQuoteByIndex(_cp.curveMatrix[n+1][j].index, md2);

				double wavg = WavgBidAsk(md1) + WavgBidAsk(md2);

				//bool sentbid = false;
				//bool sentask = false;

				ostringstream oss;
				oss << _cp.Legs[index].symbol << "(" << n << "," << j << "): "
						<< _cp.Legs[_cp.curveMatrix[n][n+1].index].symbol << "("
						<< n << "," << n+1 << ")"
						<< "+" << _cp.Legs[_cp.curveMatrix[n+1][j].index].symbol << "("
									<< n+1 << "," << j << ")"
						<<" WAVG=" << wavg << " | DBL BID=" << dbl.Bid <<" ASK=" << dbl.Ask;

				LogMe(oss.str());
			}
		}
	}

	LogMe("---------------------------------");
}


SpreadHedgeV3 Waves::GetHedgeLeg(int index, KOrderSide::Enum side, int32_t price, int spreadmult)
{
	SpreadHedgeV3 hdg = *_hdgpool.get();
	hdg.hdgtype = _HDGTYPE;
	hdg.edge = _cp.BidEdge;
	hdg.OrdSide = side;
	hdg.price = price;
	hdg.instindex = _cp.Legs[index].index;
	hdg.secid = _cp.Legs[index].secid;
	hdg.mktsegid = _cp.Legs[index].mktsegid;
	hdg.spreadmult = spreadmult;
	hdg.ratio = 1;
	hdg.sqz = true;
	hdg.cbid = _EXCH_CALLBACK_ID;

	return hdg;
}

void Waves::onMktData(int index)
{
	CheckSqueeze(index);
}


void Waves::onTrade(int index, double price, double size, int aggressor)
{

}


bool Waves::SendQuote(instrument& leg, KOrderSide::Enum side,
		int price, int qty, vector<SpreadHedgeV3>& hdgPath)
{
	if (!GoodToGo()) return false;

//	if (side == KOrderSide::BUY && price == leg.lastSentBuyPrice)
//		return false;
//
//	if (side == KOrderSide::SELL && price == leg.lastSentSellPrice)
//		return false;
//
	KTN::OrderPod ord = *_QuoteOrdPool.get();
	bool sentorders = false;

	if(PrepareQuotingOrder(ord, leg, side, price, qty))
	{
		if (RiskCheckQty(ord.quantity, _cp.TotalQty, _cp.TotalExec, ord.OrdSide))
		{
			//LogMe("WAVE SEND W HEDGE: " + OrderUtils::Print(ord), LogLevel::WARNING);
			OrderWithHedge oh = {};
			oh.ord = ord;
			oh.hdgs = hdgPath;
			SendOrderWithHedge(oh, leg);
			//this changes quote state to SentOrders
			//SendOrder(ord, leg);

			//LogMe("HDG SEND: " + std::to_string(ord.orderReqId));
			//_ordersender->SendHedgeInstruction(ord.orderReqId, hdgPath);

			sentorders = true;
			ChangeQuoteState(QuoteState::SentOrders);
		}
	}
	else{
		int lastsentpx = (side == KOrderSide::BUY) ? leg.lastSentBuyPrice : leg.lastSentSellPrice;
		LogMe("PREP QUOTING ORDER FOR " + leg.symbol + " FAILED: " + KOrderSide::toString(side)
		+ " PX=" + std::to_string(price) + " LAST SENT=" + std::to_string(lastsentpx), LogLevel::WARN);
	}

	_QuoteOrdPool.put(&ord);

	return sentorders;
}

bool Waves::PrepareQuotingOrder(KTN::OrderPod &ord, instrument& inst,
		KOrderSide::Enum side, int price, int qty)
{
	bool found = false;
	if (_MEASURE) _trk.startMeasurement("PrepOrdV5Search");
	found = _Ords.GetBySecIdSideStrat(ord, inst.secid, side, _PARAMS.StratId);
	if (_MEASURE) _trk.stopMeasurement("PrepOrdV5Search");

	int quoteqty = fabs(inst.spreadmult) * qty;

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

//bool Waves::RiskCheckQty(int qty, KOrderSide::Enum side)
//{
//	if (qty + _cp.TotalExec <= _cp.TotalQty)
//		return true;
//
//	ostringstream bbb;
//	bbb << "POTENTIAL EXPOSURE: " << KOrderSide::toString(side) << " QTY + TOTAL_EXEC > TOTAL_QTY!: QTY=" << qty << " EXEC=" << _cp.TotalExec << " ORDER NOT SENT!";
//	LogMe(bbb.str(), LogLevel::ERROR);
//	_ENABLED = false;
//	_OK_TO_TRADE = false;
////	_SLACKER.Send(_GUID + " ALERT: " + KOrderSide::toString(side) + " REJECTED", "SPRDR RISK", bbb.str(),LogLevel::ERROR, true);
//	return false;
//
//}


void Waves::ToggleCommand(int id)
{
	if (id == 1)
	{
		LogMe("****ORDER BOOK PRINT****",LogLevel::INFO);
		_Ords.Print();
	}
}


void Waves::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent)
{

	if (ord.OrdFillType == KOrderFillType::SPREAD_LEG_FILL)
	{
		_Ords.minOrderExecution(ord, hedgeSent);
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

		//UpdateLegState(ord.orderReqId, ord.OrdState);

		CancelAllOrders();
		RiskCheckQty(ord.lastqty, _cp.TotalQty, _cp.TotalExec, ord.OrdSide);
		return;
	}

	if (ord.OrdAlgoTrig == KOrderAlgoTrigger::ALGO_HEDGE)
	{
		if (ord.OrdStatus == KOrderStatus::PARTIALLY_FILLED)
			return;

		ChangeHedgeState(HedgeState::NoHedges);
	}
}

bool Waves::GotMktData()
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


bool Waves::GoodToGo()
{
	if (!_ENABLED ) return false;

	if (!GotMktData()) return false;

	if (_QuotingState == QuoteState::SentOrders)
		return false;

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


void Waves::CancelAllOrders()
{
	vector<KTN::OrderPod> ords;
	_Ords.GetByAlgoTrigger(ords, KOrderAlgoTrigger::ALGO_LEG);

	ostringstream oss;
	oss << "CXL ALL RESULT=" << ords.size() << " STRATID=" << _PARAMS.StratId;
	LogMe(oss.str(),LogLevel::WARN);

	for (auto ord : ords)
	{
		//KTN::Order cxl = _quoter.QuoteOrderCancel(ord, "CXL");

		KTN::OrderPod cxl = {};
		_quoter[ord.instindex].Get(cxl);

		//OrderUtils::Copy(ord.origclordid, incoming.ordernum);
		cxl.orderReqId = ord.orderReqId;
		cxl.exchordid = ord.exchordid;
		cxl.callbackid = _EXCH_CALLBACK_ID;
		cxl.OrdAction = KOrderAction::ACTION_CXL;
		cxl.OrdStatus = KOrderStatus::sent_cxl;
		cxl.OrdState = KOrderState::INFLIGHT;
		cxl.OrdAlgoTrig = KOrderAlgoTrigger::ORDERBOOK;
		cxl.OrdExch = KOrderExchange::CME;
		cxl.OrdSide = ord.OrdSide;

		_Ords.onSentOrderCxl(cxl);

	//	LogMe("SENDING POD CXL: " + OrderUtils::Print(cxl));
		_ordersender->Send(cxl, KOrderAction::ACTION_CXL);
		//_Ords.onSentOrderCxl(cxl);
	}

	//LogMe("!!! SENT CANCEL ALL!!!", LogLevel::WARNING);

	//_Ords.ClearInflight();

}


void Waves::CheckSqueeze()
{
	for (int i = 0; i < _cp.LegCount; i++)
	{
		if (_cp.SqueezeQty == 0)
			continue;

		CheckSqueeze(i);
	}
}

void Waves::CheckSqueeze(int index)
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

		int mktsize = (ord.OrdSide == KOrderSide::BUY) ? md.AskSize : md.AskSize;
		int price = (ord.OrdSide == KOrderSide::BUY) ? md.Ask : md.Bid;

		if (md.Ask - md.Bid > 1)
			price = (ord.OrdSide == KOrderSide::BUY) ? md.Ask-1 : md.Bid+1;

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

bool Waves::Squeezed(uint64_t reqid)
{
	for(uint64_t id : _SqueezedOrds)
	{
		if (id == reqid)
			return true;
	}

	return false;
}

bool Waves::Working(vector<KTN::OrderPod>& ords, instrument& leg, KOrderAlgoTrigger::Enum trig)
{
	//bool found = false;
	ords.clear();

	_Ords.GetByAlgoTriggerSecId(ords, trig, leg.secid);

	//cout << "WORKING GOT " << ords.size() << " FOR SECID=" << secid <<  " TRIG=" << KOrderAlgoTrigger::toString(trig) << endl;

	return (ords.size() > 0);
}

void Waves::OrderReceived(KTN::Order& incoming, int idx)
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


} /* namespace KTN */
