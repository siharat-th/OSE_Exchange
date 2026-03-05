//============================================================================
// Name        	: PLMonitor.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 5, 2023 9:39:06 AM
//============================================================================

#include <AlgoEngines/PLMonitor.hpp>

PLMonitor::PLMonitor(AlgoPLCallback& sender, string guid)
	: _cb(sender),_ACTIVE(false),_GUID(guid)
{
	 _rpt = std::unique_ptr<AlgoPLReporter>(new AlgoPLReporter(_GUID));
}

PLMonitor::~PLMonitor() {

}

void PLMonitor::LogMe(std::string szMsg, LogLevel::Enum level) {
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void PLMonitor::Start() {
	LogMe("STARTING PL MONITOR THREAD", LogLevel::INFO);
	if (!_ACTIVE) {
		_ACTIVE = true;
		_calcthread = std::thread(&PLMonitor::calculation_thread, this);
		// Get the native handle of the thread
		pthread_t threadHandle = _calcthread.native_handle();

		// Set the name of the thread using pthread_setname_np
		pthread_setname_np(threadHandle, "algo-pl-mon");
	}
}

void PLMonitor::Stop() {
	LogMe("STOPPING PL MONITOR THREAD", LogLevel::ERROR);
	if (_ACTIVE.load(std::memory_order_relaxed) == true) {
		_ACTIVE.store(false, std::memory_order_relaxed);
		_calcthread.join();
	}
}

void PLMonitor::Register(Position& pos)
{
	_qpos.push(pos);
}

void PLMonitor::onMarketData(const DepthBook &md) {
	_qmd.push(md);
}


//this is an add/update function
void PLMonitor::Prepare(Position& pos, bool isnew) {

	LogMe("PLM ADDING POSITION WITH ID=" + pos.posid);
	double spreadprice = 0;

	for(auto kv : pos.legs)
	{
		instrument leg = kv.second.leg;
		ostringstream oss;
		oss << "   LEG " << leg.index <<": SYMBOL=" << leg.symbol;
		LogMe(oss.str());

		// int posmult = (pos.legs[i].netpos > 0) ? 1 : -1;
		pos.spreadpos = leg.spreadmult * kv.second.netpos;

		double legpx = leg.spreadmult * abs(leg.pricemult) * kv.second.price;
		spreadprice += legpx;
	}

	if (isnew)
	{
		pos.erased = false;
		pos.status = KPosStatus::MONITOR_PL;
		pos.totpl = 0;
		pos.price = spreadprice;
		pos.risk.elapsedMinutes = 0;

		pos.entrytime = getCurrentTime();
	}

	//pos.guid = _GUID;

	pos.targetprice = spreadprice + pos.risk.ProfitTaking;
	pos.stoprice = spreadprice + pos.risk.StopLoss;

	pos.risk.entrytime_us = getCurrentTimeInMicroseconds();
	pos.risk.ttl_us = minutesToMicroseconds(pos.risk.MinutesToLive);
	pos.risk.expiretime_us = pos.risk.entrytime_us + pos.risk.ttl_us;

}


void PLMonitor::AdjustRisk(EngineCommand cmd)
{
	_qengine.push(cmd);
}

void PLMonitor::Adjust(Position& p, EngineCommand& cmd)
{
	for (const auto &pair : cmd.items)
	{
//		const std::string& item = pair.first;
		KEngineItem::Enum item = pair.first;
		const std::string& value = pair.second;

		/* int ProfitTaking;
		 int StopLoss;
		 int MinutesToLive;*/

		LogMe("SPREAD " + p.algoid + " FOUND RISK ADJUSTMENT FOR " + KEngineItem::toString(item) + "=" + value, LogLevel::WARN);
		// Check if a specific key exists
		if (item == KEngineItem::PROFIT_TAKING)
		{
			int pt = stoi(value);
			p.risk.ProfitTaking = pt;
			p.targetprice = p.price + pt;
		}
		if (item == KEngineItem::STOP_LOSS)
		{
			int sl = stoi(value);
			p.risk.StopLoss = sl;
			p.stoprice = p.price + sl;
		}
		if (item == KEngineItem::MINUTES_TO_LIVE)
		{
			int ml = stoi(value);
			p.risk.MinutesToLive = ml;
			p.risk.ttl_us = minutesToMicroseconds(p.risk.MinutesToLive);
			p.risk.expiretime_us = p.risk.entrytime_us + p.risk.ttl_us;

		}
		if (item == KEngineItem::PUSH_TIME)
		{
			int mins = stoi(value);
			p.risk.MinutesToLive += mins;
			p.risk.ttl_us = minutesToMicroseconds(p.risk.MinutesToLive);
			p.risk.expiretime_us = p.risk.entrytime_us + p.risk.ttl_us;
		}
	}

	LogMe(p.algoid + " POSITION RISK ADJUSTED:", LogLevel::INFO);
	ostringstream oss;
	oss << "   ProfitTaking=" << p.risk.ProfitTaking
		<< " StopLoss=" << p.risk.StopLoss
		<< " MinutesToLive=" << p.risk.MinutesToLive;
	LogMe(oss.str(), LogLevel::INFO);

}

void PLMonitor::calculation_thread() {

	std::unordered_map<int, DepthBook> quotes;
	std::unordered_map<string, Position> positions;


	int sleepus = PLMonitor::_SLEEPUS;

	_ACTIVE.store(true, std::memory_order_relaxed);
	_PRINTFLAG.store(false, std::memory_order_relaxed);

	while(_ACTIVE.load(std::memory_order_relaxed) == true)
	{
		if (_PRINTFLAG.load(std::memory_order_relaxed) == true)
		{
			LogMe("GOT PRINTFLAG=TRUE. POSITION COUNT=" + std::to_string(positions.size()));
			_PRINTFLAG.store(false, std::memory_order_relaxed);
			vector<Position> vp;
			for(auto kv : positions){
				vp.push_back(kv.second);
			}

			print_positions(vp);

		}

		if (!_qengine.empty())
		{
			EngineCommand cmd = {};
			while(_qengine.try_pop(cmd))
			{
				if (positions.find(cmd.spreadid) == positions.end())
				{
					LogMe("CANNOT FIND POSITION WITH GUID=" + cmd.spreadid, LogLevel::ERROR);
					continue;
				}

				if (cmd.Command == KEngineCommand::MARKET)
				{
					LogMe("GOT COVER (GO TO MKT) COMMAND FOR " + cmd.spreadid);

					Position p = positions[cmd.spreadid];
					p.status = KPosStatus::COVERED;

					_cb.onPL(p, AlgoPLCallback::MKT);
					_rpt->Report(p);

					positions.erase(p.posid);
				}

				if (cmd.Command == KEngineCommand::CANCEL)
				{
					LogMe("GOT CANCEL COMMAND. REMOVING " + cmd.spreadid + "FROM WATCH");

					Position p = positions[cmd.spreadid];
					p.status = KPosStatus::CANCELED;
					_rpt->Report(p);

					positions.erase(cmd.spreadid);
				}

				if (cmd.Command == KEngineCommand::RISK)
					Adjust(positions[cmd.spreadid], cmd);


			}
		}

		if (!_qmd.empty())
		{
			DepthBook md = {};
			while(_qmd.try_pop(md))
			{
				quotes[md.SecId] = md;
			}
		}

		if (!_qpos.empty())
		{
			Position p = {};
			while(_qpos.try_pop(p))
			{
				bool isnew = (positions.find(p.posid) == positions.end()) ? true : false;
				Prepare(p, isnew);
				positions[p.posid] = p;
			}
		}


		if (positions.size() == 0)
		{
			usleep(1000);
			continue;
		}

		for  (auto& pos : positions)
		{
			double temp = 0;
			double spreadprice = 0;
			size_t chksum = 0;
			for(auto kv : pos.second.legs)
			{
				PositionLeg& posleg = kv.second;

				int secid = posleg.leg.secid;
//				cout << "SECID=" << secid << " " << quotes[secid].symbol <<  " QUOTE: B:"
//						<< quotes[secid].bid << "  A:" << quotes[secid].ask << endl;


				if (quotes.find(secid) == quotes.end())
				{
					//LogMe("NEED DATA FOR " + posleg.leg.symbol, LogLevel::WARN);
					break;
				}

				if (!GotMktData(quotes[secid]))
					break;

				chksum ++;

				posleg.bid = quotes[secid].Bid;
				posleg.ask = quotes[secid].Ask;



//				if (posleg.bid == 0)
//					posleg.bid = posleg.ask;
//
//				if (posleg.ask == 0)
//					posleg.ask = posleg.bid;

				double plprice = (posleg.netpos > 0) ? posleg.bid : posleg.ask;

				posleg.calc();
				temp += posleg.pl;

				double legpx = posleg.leg.spreadmult * abs(posleg.leg.pricemult) * plprice;
				spreadprice += legpx;
//				cout << fixed << "POS TOT=" << _positions.size() << " LEGS=" << pos.second.numlegs << "] POSLEG: B=" << posleg.bid << " A=" << posleg.ask
//						<< " PX=" << posleg.price <<" NETPOS=" << posleg.netpos
//						<< " PL=" << temp << " SPMUL=" << posleg.leg.spreadmult << " PXMUL=" << posleg.leg.pricemult << endl;
			}

			if (chksum != pos.second.legs.size())
			{
				//LogMe("DATA INCOMPLETE FOR SPRD " + pos.first, LogLevel::WARN);
				pos.second.status = KPosStatus::WAITING;
				_rpt->Report(pos.second);
				usleep(100000);
				continue;
			}
			else
			{
				if (pos.second.status == KPosStatus::WAITING)
					pos.second.status = KPosStatus::MONITOR_PL;
			}

			pos.second.totpl = temp;
			pos.second.mktprice = spreadprice;

			pos.second.plper = pos.second.totpl / abs(pos.second.spreadpos);


			_rpt->Report(pos.second);

//			cout << pos.second.id << " PL=" << pos.second.totpl << " PT=" << pos.second.risk.ProfitTaking
//					<< " SL=" << pos.second.risk.StopLoss
//					<< " SPPRICE=" << pos.second.lastprice << " POSPRICE=" << pos.second.price
//					<< " STATUS=" << KPosStatus::toString(pos.second.status) << endl;

			if (pos.second.status !=  KPosStatus::MONITOR_PL)
			{
				usleep(sleepus);
				continue;
			}

			int64_t ttl = getTimeToLive(pos.second.risk.entrytime_us, pos.second.risk.ttl_us);

			pos.second.risk.timeleft_us = ttl;
			pos.second.risk.seconds_left = ttl / 1000000;


			if (pos.second.risk.timeleft_us <= 0)
			{
				LogMe(pos.second.algoid + " POS TIMED OUT!!", LogLevel::WARN);
				if (!isTimeInCmeRange()){
					LogMe("TIME IS NOT IN CME TIME RANGE. CANNOT COVER!!!", LogLevel::WARN);
				}
				Position p = pos.second;
				p.status = KPosStatus::TIMED_OUT;

				_cb.onPL(p, AlgoPLCallback::TIMERHIT);
				_rpt->Report(p);

				positions.erase(pos.first);

			}

			if (pos.second.plper >= pos.second.risk.ProfitTaking)
			{
				if (!isTimeInCmeRange()){
					LogMe("PROFIT TARGET HIT: TIME IS NOT IN CME TIME RANGE. CANNOT COVER!!!", LogLevel::WARN);
				}

				Position p = pos.second;
				p.status = KPosStatus::PROFIT_HIT;

				_cb.onPL(p, AlgoPLCallback::PROFITTAKE);
				_rpt->Report(p);

				positions.erase(pos.first);


			}

			if (pos.second.plper <= pos.second.risk.StopLoss)
			{
				if (!isTimeInCmeRange()){
					LogMe("STOP LOSS HIT: TIME IS NOT IN CME TIME RANGE. CANNOT COVER!!!", LogLevel::WARN);
				}
				Position p = pos.second;
				p.status = KPosStatus::STOP_HIT;

				_cb.onPL(p, AlgoPLCallback::STOPLOSS);
				_rpt->Report(p);

				positions.erase(pos.first);

			}

		}

		usleep(sleepus);

	}
}


void PLMonitor::Print()
{
	_PRINTFLAG.store(true, std::memory_order_relaxed);
}

void PLMonitor::print_positions(vector<Position> vpos)
{
	ostringstream oss;
	oss << "***PLMON RECORDS COUNT=" << vpos.size();
	LogMe(oss.str(), LogLevel::INFO);

	if (vpos.size() == 0)
		return;

	for  (auto& pos : vpos)
	{
		ostringstream oss;

		auto pl = pos;
		oss << "ID=" << pl.algoid << ": PL=" << pl.totpl << " pt=" << pl.risk.ProfitTaking
				<< " sl=" << pl.risk.StopLoss << " ttl=" << pl.risk.MinutesToLive;
		LogMe(oss.str(), LogLevel::INFO);

		for(auto kv : pl.legs)
		{
			auto pos = kv.second;
			int plprice = (pos.side == KOrderSide::BUY) ? pos.ask : pos.bid;
			oss.clear();
			oss.str("");
			oss << "  -->" << pos.leg.symbol << " " << KOrderSide::toString(pos.side)
					<< " " << pos.netpos <<" @ " << pos.price << " PL=" << pos.pl
					<< " (" << plprice << ")";
			LogMe(oss.str());
		}

	}

}

bool PLMonitor::GotMktData(DepthBook& sym)
{
	if ((sym.Bid == 0 && sym.Ask == 0) || (sym.Bid > sym.Ask) || sym.BidSize == 0 || sym.AskSize == 0)
		return false;

	return true;
}
