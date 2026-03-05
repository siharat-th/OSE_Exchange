//============================================================================
// Name        	: AlgoV3Reporter.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 31, 2023 3:04:50 PM
//============================================================================

#include <Reporters/AlgoV3Reporter.hpp>
#include <iostream>

using namespace KTN::REPORTER;

AlgoV3Reporter::AlgoV3Reporter() : ReporterBase("ALGO3RPT", 10000), _qMsgs(1000), _ssboe_us(0)
{
	Settings settings("Settings.txt");
	_ACCT = settings.getString("Account");
	_KEY = settings.getString("Key");

	Start();
}


AlgoV3Reporter::~AlgoV3Reporter()
{
}

void AlgoV3Reporter::Report(const genericstats& msg)
{
	_qMsgs.push(msg);
//	_qMsgs.enqueue(msg);
}

void AlgoV3Reporter::StopWork()
{
	Stop();
}

void AlgoV3Reporter::DoWork()
{
	std::vector<genericstats> msgs;
	uint64_t hbus = _hb_interval * 1000 * 1000;
	uint64_t ssboe = getSsboeUs();
	string updtime = getTimeString();

	if (!_qMsgs.empty())
	{
		genericstats msg = {};
		//_qMsgs.try_dequeue(msg);
		while(_qMsgs.try_pop(msg))
		{
			msg.ssboe = ssboe;
			msg.updatetime = updtime;
			msgs.push_back(msg);

			_stats[msg.uniqueid] = msg;
		}

	}

	if (msgs.size() == 0 && (ssboe - _ssboe_us > hbus))
	{
		//update ssboe
		_ssboe_us = ssboe;

		for(auto kv : _stats)
		{
			genericstats gmsg = kv.second;

			if (gmsg.enabled == false)
				continue;

			gmsg.msgtype = "HB";
			gmsg.ssboe = ssboe;
			gmsg.updatetime = updtime;

			SendMessage(gmsg, ssboe, updtime);
		}
	}
	else
	{
		for (genericstats  gmsg : msgs)
			SendMessage(gmsg, ssboe, updtime);
	}
}

void AlgoV3Reporter::SendMessage(const genericstats& gmsg, uint64_t ssboe, const string& updtime)
{
	string szjson = AlgoJson::FormatAlgoStats(gmsg.msgtype, gmsg.source,
						gmsg.enabled,gmsg.templatename,gmsg.uniqueid,gmsg.text,
						gmsg.name, gmsg.symbol, gmsg.exch,
						gmsg.position, gmsg.totalexec);
	SendJson(szjson);
	usleep(10);
//#ifdef DEBUG_TESTING
//	cout << "ALGOV3 REPORTER TO BASE: "  << szjson << endl;
//#endif
}


