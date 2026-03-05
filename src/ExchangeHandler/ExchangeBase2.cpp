//============================================================================
// Name        	: ExchangeBase2.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2021-24 Katana Financial
// Date			: April 27, 2023 4:49:28 PM
//============================================================================

#include <ExchangeHandler/ExchangeBase2.hpp>

using namespace KTN;

ExchangeBase2::ExchangeBase2( const std::string& settingsfile, const std::string& source, 
	tbb::concurrent_queue<KTN::OrderPod>& qords, const std::string& exchname) 
	: _qOrdsProc(qords),
	_qCmds(32),
	_qAlgoCmds(32),
	_qRecvBufs(512),
	_OrdPodPool(512),
	_KtnBufs(256, 4096),
	_qHdgClOrdIDs(10000)
{
	_State.EXCHNAME = exchname;
	_State.MEASURE = true;
	_State.FILENAME = settingsfile;
	_State.SOURCE = source;
	_logsvc = new LogSvc(_State.EXCHNAME, _State.EXCHNAME);

	// hedge clordids- we pre-generate pairs of order req id's and clordids
	for (int i = 0; i < 10000; i++)
	{
		string nextclordid = OrderNums::instance().NextClOrdId();
		uint64_t newreqid = OrderNums::instance().GenOrderReqId(nextclordid);
		char *newclordid = new char[20];
		OrderUtils::fastCopy(newclordid, nextclordid.c_str(), 20);
		_qHdgClOrdIDs.enqueue(std::make_pair(newreqid, newclordid));
	}
	
}


ExchangeBase2::~ExchangeBase2() {
}

void ExchangeBase2::StopBase()
{

}
/**
 * AlgoJson reception IF Algo is integrated into handler.
 * @param json
 * @param cmd
 */
void ExchangeBase2::AlgoJson(string json, ExchCmd::Enum cmd)
{
	LogMe("ERROR: AlgoJson IS NOT IMPLEMENTED!", LogLevel::ERROR);
}

void ExchangeBase2::SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3> & hdg)
{
	LogMe("ERROR: SendWithHedgesV4 IS NOT IMPLEMENTED!", LogLevel::ERROR);
}

void ExchangeBase2::ClearMaps()
{
	LogMe("CLEAR MAPS NOT IMPLEMENTED FOR " + _State.EXCHNAME, LogLevel::WARN);
}

void ExchangeBase2::LogMe(const std::string& szMsg, LogLevel::Enum level)
{
//	LWT_LOG_ME(szMsg,"CMEALGO",iColor);
//	return;

	_logsvc->LogMe(szMsg, level);
}

void ExchangeBase2::ReportStatus(SessionStatus s)
{
	//_sessrpt->Report(s);
}

void ExchangeBase2::Reset()
{
	LogMe("RESET NOT IMPLEMENTED", LogLevel::ERROR);
}
void ExchangeBase2::Command(Instruction cmd)
{
	LogMe("GENERIC COMMAND NOT IMPLEMENTED", LogLevel::ERROR);
}

void ExchangeBase2::MsgSent(const std::string& symb, int cnt, int volume, int msgtype)
{
	if (_msgratio.find(symb) == _msgratio.end())
	{
		msgrate m = {};
		m.sent = 0;
		m.volume = 0;
		_msgratio[symb] = m;

		msgevents x = {};
		_msgevents[symb] = x;
	}

	if (cnt > 0)
		_msgratio[symb].sent += cnt;

	if (volume > 0)
		_msgratio[symb].volume += volume;

}

//void ExchangeBase2::HeartbeatReporter()
//{
//	_notifier->Heartbeat(_SOURCE, _EXCHNAME, _ORG);
//}
//
//void ExchangeBase2::PersistNotification(string msgtype, MsgLevel level, string text)
//{
//	KTN_NOTIFICATION msg = {};
//	msg.msgtype = msgtype;
//	msg.level = level;
//	msg.text = text;
//	msg.exch = _EXCHNAME;
//	msg.source = _SOURCE;
//	msg.imsg = MsgType::NOTIFY;
//
//	_notifier->Notify(msg);
//}

void ExchangeBase2::PrintLatencies( bool clear)
{
	LogMe("");
	LogMe("PERF TRACKER:", LogLevel::INFO);

	if (_State.MEASURE)
	{
		_State.MEASURE = false;
		_trk.printMeasurements();
		_State.MEASURE = true;
	}
	else
		LogMe("  ->MEASURE IS NOT ENABLED.");

	if (clear)
		_trk.clear();
}

void ExchangeBase2::PrintMsgRatios()
{
	LogMe("************* MESSAGING RATIOS ***************", LogLevel::WARN);

	for(auto x : _msgratio)
	{
		ostringstream oss;
		oss.imbue(std::locale(""));
		oss << fixed << std::setprecision(2);


		double ratio = x.second.sent / (double)x.second.volume;

		oss << x.first << ": RATIO=" << ratio << " SENT=" << x.second.sent << " VOLUME=" << x.second.volume;
		LogMe(oss.str(),LogLevel::WARN);
	}

	LogMe("*********************************************", LogLevel::WARN);
}

