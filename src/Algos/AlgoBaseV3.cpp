/*
 * AlgoBaseV3.cpp
 *
 *  Created on: Apr 24, 2021
 *      Author: sgaer
 */

#include <Algos/AlgoBaseV3.hpp>
#include <exchsupport/cfe/mktdata/CfeMktDataHandler.hpp>

namespace KTN {

//AlgoBaseV3 * m_pThisAlgo3;

AlgoBaseV3::AlgoBaseV3(AlgoInitializer params)
	: m_Name(params.name), _GUID(params.guid),
	  _ordersender(params.ordersender), _MKT_DATA_CPU(params.cpu),
	  _LEGS(params.legs),
	  _Ords(1, params.guid),
	  _GROUPCNT(0),
	  _CNT(0),
	  _ALLSYMBOLS(false),
	  _RPT_ACTIVE(false),
	  _MKT_DATA_OK(false),
	  _CME_BY_SECID(false),
	  _DATA_TRIES(0)
	 // _WATCH_TRADES(params.watch_trades),_WATCH_VOLUME(params.watch_volume)
{
	m_Name = _GUID;
	string szsource = _GUID;
	string owner = m_Name;
	_logsvc = new LogSvc(owner, szsource);

	_settings = Settings("Settings.txt");

#if !AKL_TEST_MODE
	_algorpt = new AlgoV3Reporter();
#endif

	_CORE = _settings.getInteger("AlgoAffinity");
	_PARAMS.Source = _settings.getString("Source");
	_ORG = _settings.getString("Org");
	//_MEASURE = CmeSettingsHelper::GetBool("IL3.MeasureLatency");

	RegisterCallbackId();

	ostringstream oss;
	oss << "HELLO! MY GUID IS " << _GUID << "  CPU=" << _MKT_DATA_CPU << " CALLBACK=" << (int)_EXCH_CALLBACK_ID;
	LogMe(oss.str(),LogLevel::INFO);

	for (std::vector<instrument>::const_iterator it = params.legs.begin(); it != params.legs.end(); ++it)
	{
		if (it->exchEnum == KTN::ORD::KOrderExchange::Enum::CME)
		{
			_msgw.LoadProductGatewayFile();
			break;
		}
	}

	if (params.legs.size() > 0)
	{
		if (params.legs[0].exchEnum == KTN::ORD::KOrderExchange::Enum::CME)
		{
			_mdh = new KtnCmeMdpV5(params.guid, _MKT_DATA_CPU, _WATCH_TRADES);
			LOGINFO("[AlgoBaseV3] Created CME MktDataHandler for Algo {}", _GUID);
		}
		else if (params.legs[0].exchEnum == KTN::ORD::KOrderExchange::Enum::CFE)
		{
			_mdh = new CfeMktDataHandler(params.guid, _MKT_DATA_CPU);
			LOGINFO("[AlgoBaseV3] Created CFE MktDataHandler for Algo {}", _GUID);
		}
		else
		{
			LOGERROR("[AlgoBaseV3] Unsupported exchange {} for market data handler", static_cast<int>(params.legs[0].exchEnum));
			return; // TODO mwojcik how to handle?
		}
	}

	_mdh->Subscribe(this);

	_HedgingState = HedgeState::NoHedges;
}

AlgoBaseV3::~AlgoBaseV3() {
	LogMe("SHUTTING DOWN ALGO");
	_logsvc->Stop();
	_logsvc->Join();
	_RPT_ACTIVE = false;
}

void AlgoBaseV3::StartMarketDataHandler()
{
#if !AKL_TEST_MODE
	if (_mdh)
	{
		_mdh->Start();
	}
#endif
}

void AlgoBaseV3::Poll()
{

}

void AlgoBaseV3::Shutdown()
{
	LogMe("SHUTDOWN COMMAND: STOPPING ALL THREADS!!!", LogLevel::ERROR);
	_Ords.Stop();
	_RPT_ACTIVE = false;
	_ACTIVE.store(false, std::memory_order_relaxed);
	usleep(10000);
}

void AlgoBaseV3::LogMe(std::string szMsg, LogLevel::Enum level)
{
//	LWT_LOG_ME(szMsg,_GUID,iColor);
//	return;
	_LOG.msg = szMsg;
	_LOG.loglevel = level;
	_logsvc->LogMe(_LOG);
}

void AlgoBaseV3::Notify(string msg, MsgLevel::Enum level, MsgType::Enum msgtype)
{
	Notification n = {};
	n.exch = "ALGO";
	n.source = _PARAMS.Source;
	n.org = _ORG;
	n.level = level;
	n.imsg = msgtype;
	n.text = msg;

	NotifierRest::instance().Notify(n);
}

void AlgoBaseV3::GenIds(string prefix)
{

}

void AlgoBaseV3::RegisterCallbackId()
{
	_EXCH_CALLBACK_ID = _ordersender->RegisterCallback(this);

	ostringstream oss;
	oss << "****ALGO " + _GUID + " CALLBACK ID=" << _EXCH_CALLBACK_ID;
	for(int i = 0; i < 10; i++)
		LogMe(oss.str(), LogLevel::INFO);
}


void AlgoBaseV3::onEngineJson(EngineCommand& cmd)
{
	LogMe("ENGINE COMMAND IS NOT IMPLEMENTED!! ERROR!!!", LogLevel::ERROR);
	return;
}

void AlgoBaseV3::onAlgoMessage(const std::string &json)
{

}

void AlgoBaseV3::RegisterParams(std::string json)
{
	Document d;

	if (d.Parse(json.c_str()).HasParseError()) {
		int e = (unsigned)d.GetErrorOffset();
		ostringstream oss;
		oss << "JSON OFFSET ERROR AT " << e ;
		LogMe(oss.str(),LogLevel::ERROR);
	}

	if (!d.IsArray())
	{
		LogMe("ALGO JSON OBJECT MUST BE AN ARRAY!!!!", LogLevel::ERROR);
		return;
	}

	for (Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
	{

		_PARAMS.userinfo.user = (*itr)["user"].GetString();
		_PARAMS.userinfo.org = (*itr)["org"].GetString();

		if ((*itr).HasMember("groupname"))
			_PARAMS.userinfo.user = (*itr)["groupname"].GetString();
		else
			_PARAMS.userinfo.groupname = _PARAMS.userinfo.org;

		LogMe(_PARAMS.UniqueId + " ALGO REGISTERED USER: " + UserInfo::Print(_PARAMS.userinfo), LogLevel::INFO);

	}
}

void AlgoBaseV3::ToggleCommand(int id)
{
	if (id == 1)
	{
		_Ords.Print();
	}
}

void AlgoBaseV3::Restart()
{
	LogMe("RESTART NOT ENABLED FOR " + _GUID, LogLevel::WARN);
}

void AlgoBaseV3::CancelAllOrders()
{
	LogMe("CANCEL ALL ORDERS NOT ENABLED FOR " + _GUID, LogLevel::WARN);
}

void AlgoBaseV3::ChangeQuoteState(const QuoteState::Enum& newstate)
{
	//QuoteState::Enum oldstate = _QuotingState;
	_QuotingState = newstate;

//	ostringstream oss;
//	oss << "QUOTING STATE CHANGED FROM " << QuoteState::toString(oldstate) << " TO "
//			<< QuoteState::toString(_QuotingState);
//	LogMe(oss.str(), LogLevel::WARN);

}

void AlgoBaseV3::ChangeHedgeState(const HedgeState::Enum& newstate)
{
	//HedgeState::Enum oldstate = _HedgingState;
	_HedgingState = newstate;

//	ostringstream oss;
//	oss << "HEDGE STATE CHANGED FROM " << HedgeState::toString(oldstate) << " TO "
//			<< HedgeState::toString(_HedgingState);
//	LogMe(oss.str(), LogLevel::WARN);

}

void AlgoBaseV3::AddReportingMsg(string msg, string msgtype)
{
//	auto gmsg = new genericstats();
//	gmsg->text = msg;
//	gmsg->msgtype = msgtype;
	_mystats.enabled = _PARAMS.Enabled;
	_mystats.text = msg;
	_mystats.msgtype = msgtype;

#if !AKL_TEST_MODE
	_algorpt->Report(_mystats);
#endif
	//_MSGS.enqueue(gmsg);
}

void AlgoBaseV3::PrintMdMap()
{
	LogMe("--- ALGO MARKET DATA ----",LogLevel::INFO);


	for (int i = 0; i < _mdh->Obm.Size(); i++)
	{
		DepthBook md = _mdh->Obm.books[i];
		instrument inst = _SECIDMAP[md.SecId];
		ostringstream oy;
		oy << fixed;
		oy  << "  " << i <<"." << inst.exch << " | " << inst.symbol
			<< ": " << md.Bid << " @ " << md.Ask << " "
			<< md.BidSize << " X " << md.AskSize
			<< " |LAST=" << md.Last << "(" << md.LastSize << ") AGGR=" << (int)md.LastAgressor;

		LogMe(oy.str());
	}

	ostringstream wtf;
	wtf << "  COUNT=" << _mdh->Obm.Size() << ":";
	LogMe(wtf.str());

	LogMe("---------------------",LogLevel::INFO);

}

void AlgoBaseV3::PrintBookMap()
{
	for (int i = 0; i < _mdh->Obm.Size(); i++)
	{
		DepthBook md = _mdh->Obm.books[i];
		instrument inst = _SECIDMAP[md.SecId];
		PrintBook(inst);
	}

}

void AlgoBaseV3::PrintBook(instrument inst)
{
	ostringstream oss;
	oss << "------------ " << inst.symbol << " DEPTH ----------------";
	LogMe(oss.str(),LogLevel::INFO);

	int col = 8;
	int lvl = 5;

	oss.str("");
	oss.clear();
	oss << std::left << std::setw(lvl) << "Lvl" << " | "
		<< std::setw(col) << "Bid" << " | "
		<< std::setw(col) << "BidSz" << " | "
		<< std::setw(col) << "Ask" << " | "
		<< std::setw(col) << "AskSz";
	LogMe(oss.str(), LogLevel::INFO);

	oss.str("");
	oss.clear();
	oss << std::setw(lvl) << "-----" << " | "
			<< std::setw(col) << "--------"  << " | "
			<< std::setw(col) << "--------"  << " | "
			<< std::setw(col) << "--------"  << " | "
			<< std::setw(col) << "--------" ;
	LogMe(oss.str(), LogLevel::INFO);

	DepthBook book = {};
	GetBook(inst, book);

	for(int j = 0; j < 10; j++)
	{
		pxl bid = book.Bids[j];
		pxl ask = book.Asks[j];

		oss.str("");
		oss.clear();
		oss  << std::setw(lvl) << j << " | "
		<< std::setw(col) << bid.px << " | "
		<< std::setw(col) << bid.size << " | "
		<< std::setw(col) << ask.px << " | "
		<< std::setw(col) << ask.size ;
		LogMe(oss.str());
	}

}


void AlgoBaseV3::Print()
{
	_Ords.Print();

	_mdh->PrintStats();
}

void AlgoBaseV3::ResetPosition()
{
}

void AlgoBaseV3::OrderReceived(KTN::Order& ord, int index=0)
{
	LogMe("OrderReceived NOT IMPLEMENTED FOR THIS ALGO!", LogLevel::ERROR);
}

void AlgoBaseV3::OrderMinUpdate(KTN::OrderPod& ord)
{
	//LogMe("OrderMinUpdate NOT IMPLEMENTED FOR THIS ALGO!", LogLevel::ERROR);
}


void AlgoBaseV3::onMktDataBidAsk(const DepthBook & md)
{
	if (md.Bid == 0 && md.Ask == 0)
		return;
		
	onMktData(md.Index);
	onCalculate();
}


void AlgoBaseV3::onMktDataTrade(const DepthBook & md)
{
	if (md.Bid == 0 && md.Ask == 0)
		return;

	onTrade(md.Index, md.Last, md.LastSize, md.LastAgressor);
	onCalculate();
}

void AlgoBaseV3::onMktDataSecurityStatus(int secid, BookSecurityStatus secstatus, BookSecurityTradingEvent event, const uint64_t recvTime)
{
	//your code here
}

void AlgoBaseV3::onKillswitchUpdate(const bool state)
{
	KT01_LOG_ERROR("AlgoBaseV3", "onKillswitchUpdate NOT IMPLEMENTED FOR THIS ALGO!");
}

void AlgoBaseV3::DisableRisk()
{
	KT01_LOG_ERROR("AlgoBaseV3", "DisableRisk NOT IMPLEMENTED FOR THIS ALGO!");
}

void AlgoBaseV3::TryStop()
{

}

DepthBook& AlgoBaseV3::findBySecId(int id)
{
  
  int index =  _mdh->Obm.IndexOf(id);
  if (_mdh->Obm.IndexOf(id) >= 0)
  {
	return _mdh->Obm.books[index];
  }
  
  static DepthBook blank;
  return blank;
}

int AlgoBaseV3::indexBySecId(int id) {

  return _mdh->Obm.IndexOf(id);
}

void AlgoBaseV3::RegisterCmeSecId(instrument & inst)
{
	CmeSecDef def = CmeSecMaster::instance().getSecDef(inst.symbol);

	inst.secid = def.secid;
	inst.product = def.product;
	inst.prodtype = def.prodtype;
	inst.mintick = def.mintick;
	//we use size here to determine the next index - it is zero based.
	inst.mdindex = _mdh->Obm.Size();

	//TODO: options support if/when
	inst.mktsegid = _msgw.getProdSegId(inst.product, true);

	LOGINFO("LEG {} MKT SEG ID={} SECID={} PRODTYPE={}", inst.symbol, inst.mktsegid, inst.secid, KOrderProdType::toString(inst.prodtype));
	LOGINFO("*******CME: {} {}", inst.label, inst.symbol);

	_mdh->AddSymbol(inst.symbol, inst.mdindex, inst.secid);

	_LEGMAP[inst.symbol] = inst;
	_SECIDMAP[inst.secid] = inst;

	LOGINFO("REGISTERING SECID {} AT ALGO HANDLER.  COUNT={}", inst.secid, _mdh->Obm.Size());
}

void AlgoBaseV3::RegisterCfeSecId(instrument & inst)
{
	CfeSecDef def = CfeSecMaster::instance().getSecDef(inst.symbol);

	inst.secid = def.secid;
	inst.product = def.product;
	inst.prodtype = def.prodtype;
	inst.mintick = def.mintick;
	//we use size here to determine the next index - it is zero based.
	//inst.mdindex = _mdh->Obm.Size();

	// TODO mwojcik will this work?
	inst.mktsegid = 1;//_msgw.getProdSegId(inst.product, true);

	LOGINFO("LEG {} MKT SEG ID={} SECID={} PRODTYPE={}", inst.symbol, inst.mktsegid, inst.secid, KOrderProdType::toString(inst.prodtype));
	LOGINFO("*******CME: {} {}", inst.label, inst.symbol);

	//_mdh->AddSymbol(inst.symbol, inst.mdindex);
	_mdh->AddSymbol(inst.symbol, 0, inst.secid);
	_LEGMAP[inst.symbol] = inst;
	_SECIDMAP[inst.secid] = inst;

	//LOGINFO("REGISTERING SECID {} AT ALGO HANDLER.  COUNT={}", inst.secid, _mdh->Obm.Size());
}

DepthBook& AlgoBaseV3::GetQuote(const instrument & inst)
{
	int index = indexBySecId(inst.secid);
	if (UNLIKELY(index == -1))
	{
		static DepthBook blank;
		return blank;
	}
	return _mdh->Obm.books[index];

}

bool AlgoBaseV3::GetQuote(const instrument & inst, DepthBook& md)
{
	int index = indexBySecId(inst.secid);
	if (UNLIKELY(index == -1))
		return false;

	md = _mdh->Obm.books[index];

	return true;
}

bool AlgoBaseV3::GetQuoteByIndex(int index, DepthBook& md)
{
	md = _mdh->Obm.books[index];
	return true;
}


DepthBook& AlgoBaseV3::GetBook(const instrument & inst)
{
	int index = indexBySecId(inst.secid);
	if (UNLIKELY(index == -1))
	{
		static DepthBook blank;
		return blank;
	}
	return _mdh->Obm.books[index];

}

bool AlgoBaseV3::GetBook(const instrument & inst, DepthBook& book)
{
	int index = indexBySecId(inst.secid);
	if (UNLIKELY(index == -1))
		return false;

	book = _mdh->Obm.books[index];

	return true;
}


void AlgoBaseV3::UpdateMetaParams(algometa meta)
{
	_PARAMS = meta;
	_PARAMS.Tag = meta.Tag;
	_PARAMS.HedgeTag = "HDG";
	_PARAMS.StratId = meta.StratId;
	_GUID = meta.UniqueId;

	_mystats.name = _GUID;
	_mystats.uniqueid = _GUID;
	_mystats.source = _PARAMS.Source;
	_mystats.name = _PARAMS.UniqueId;
	_mystats.templatename = _PARAMS.TemplateName;
	_mystats.exch = "CME";

	_Ords.UpdateMetaParams(meta);

	std::string state = (meta.Enabled) ? "ENABLED" : "DISABLED";
	LogLevel::Enum level = (meta.Enabled) ? LogLevel::INFO : LogLevel::ERROR;

	ostringstream oss;
	oss << "***** " + meta.TemplateName + ": STATE INCOMING IS " << state  << " STRAT=" << meta.Strategy ;
	LogMe(oss.str(), level);
	std::string status = oss.str();
}

////Poll the queue for any buffer to send directly ....
//// I think this can be better.
//void AlgoBaseV3::Poll(vector<KTN::OrderPod>& ords)
//{
//	while (!_qSendOrds.empty())
//	{
//		KTN::OrderPod ord = {};
//		if (_qSendOrds.dequeue(ord)){
//			ords.push_back(ord);
//		}
//	}
//}

void AlgoBaseV3::onTrade(int index, double price, double size, int aggressor) {

}

void AlgoBaseV3::onMktData(int index) {
	//utility/convenience
}

void AlgoBaseV3::onStateChange(int index) {
	//utility/convenience
}


bool AlgoBaseV3::RiskCheckQty(int qty, int totalqty, int execqty, KOrderSide::Enum side)
{
	if (qty + execqty <= totalqty)
		return true;

	ostringstream bbb;
	bbb << "POTENTIAL EXPOSURE: " << KOrderSide::toString(side)
			<< " QTY + TOTAL_EXEC > TOTAL_QTY!: QTY=" << qty << " TOT_EXEC=" << execqty << "VS  MAX=" << totalqty << " ORDER NOT SENT!";
	LogMe(bbb.str(), LogLevel::ERROR);
	if (_ENABLED) Enable(false);
	//_SLACKER.Send(_GUID + " ALERT: " + KOrderSide::toString(side) + " REJECTED", "SPRDR RISK", bbb.str(),LogLevel::ERROR, true);
	return false;

}

//order sending: native or OrderPod- don't know, but can also use a queue etc...
void AlgoBaseV3::SendOrder(KTN::OrderPod& ord, instrument& leg)
{
	//Risk check before sending
	ChangeQuoteState(QuoteState::SentOrders);

	_Ords.onSentOrderFast(ord);
	//we are going to elect to just send the buffer for new and mods...
	_ordersender->Send(ord, ord.OrdAction);

	if (ord.OrdSide == KOrderSide::BUY){
		leg.lastSentBuyPrice = ord.price;
		leg.lastBuyId = ord.orderReqId;
		leg.buyState = KOrderState::INFLIGHT;
	}
	else{
		leg.lastSentSellPrice = ord.price;
		leg.lastSellId = ord.orderReqId;
		leg.sellState = KOrderState::INFLIGHT;
	}
}

void AlgoBaseV3::SendOrderWithHedge(OrderWithHedge& oh, instrument& leg)
{
	//Risk check before sending
	ChangeQuoteState(QuoteState::SentOrders);

	_Ords.onSentOrderFast(oh.ord);
	//we are going to elect to just send the buffer for new and mods...
	_ordersender->SendOrderWithHedge(oh);

	if (oh.ord.OrdSide == KOrderSide::BUY){
		leg.lastSentBuyPrice = oh.ord.price;
		leg.lastBuyId = oh.ord.orderReqId;
		leg.buyState = KOrderState::INFLIGHT;
	}
	else{
		leg.lastSentSellPrice = oh.ord.price;
		leg.lastSellId = oh.ord.orderReqId;
		leg.sellState = KOrderState::INFLIGHT;
	}

}

//session callback impl

//callback defaults

const std::string& AlgoBaseV3::getGuid() const
{
	return _GUID;
}

void AlgoBaseV3::onHeartbeat(int msgseqnum){}
void AlgoBaseV3::onSessionStatus(string exch, int mktsegid, int protocol,
		string desc, string state) {}

void AlgoBaseV3::onSpreadLegOrderExecution(KTN::Order& ord)
{
	LogMe("SLEG:" + OrderUtils::Print(ord));
}


void AlgoBaseV3::minOrderAck(KTN::OrderPod& ord)
{
	_Ords.minOrderAck(ord);
	_bQuoteInFlight = _Ords.HasInflight();
	if(!_bQuoteInFlight)
		ChangeQuoteState(QuoteState::ReadyToSendOrders);
}
void AlgoBaseV3::minOrderCancel(KTN::OrderPod& ord)
{
	OrderMinUpdate(ord);
	_Ords.minOrderCancel(ord);
	_bQuoteInFlight = _Ords.HasInflight();
	if(!_bQuoteInFlight)
		ChangeQuoteState(QuoteState::ReadyToSendOrders);

}
void AlgoBaseV3::minOrderModify(KTN::OrderPod& ord)
{
	OrderMinUpdate(ord);
	_Ords.minOrderModify(ord);
	_bQuoteInFlight = _Ords.HasInflight();
	if(!_bQuoteInFlight)
		ChangeQuoteState(QuoteState::ReadyToSendOrders);
}

//leaving execution to individual algos...
void AlgoBaseV3::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent)
{
}

void AlgoBaseV3::minOrderReject(KTN::OrderPod& ord, const string& text)
{
	OrderMinUpdate(ord);
	_Ords.minOrderReject(ord, text);
	LogMe("REJECT RECEIVED: " + text,LogLevel::ERROR);
	if (ord.OrdAction == KOrderAction::ACTION_REJ){
		Enable(false);
		return;
	}
	LogMe("CXL REJECTED- ALGO STILL ACTIVE!", LogLevel::ERROR);
}

void AlgoBaseV3::minOrderCancelReject(KTN::OrderPod& ord, const string& text)
{
	OrderMinUpdate(ord);
	_Ords.minOrderReject(ord, text);
	LogMe("CXL REJECT RECEIVED: " + text,LogLevel::ERROR);
}

void AlgoBaseV3::minOrderCancelReplaceReject(KTN::OrderPod& ord, const string& text)
{
	OrderMinUpdate(ord);
	_Ords.minOrderReject(ord, text);
	LogMe("CXL REPLACE REJECT RECEIVED: " + text,LogLevel::ERROR);
}

void AlgoBaseV3::onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid,KOrderSide::Enum side,
			uint32_t qty, int32_t price, uint16_t instindex)
{
	LogMe("CALLBACK HEDGE ORDER SENT NOT IMPLMENTED",LogLevel::ERROR);
}

void AlgoBaseV3::onOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
				uint32_t qty, int32_t price, KOrderAlgoTrigger::Enum trig)
{
	LogMe("CALLBACK ORDER SENT NOT IMPLMENTED",LogLevel::ERROR);
}


} /* namespace KTN */
