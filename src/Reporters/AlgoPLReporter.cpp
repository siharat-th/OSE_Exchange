//============================================================================
// Name        	: AlgoPLReporter.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 23, 2023 3:04:50 PM
//============================================================================

#include <Reporters/AlgoPLReporter.hpp>
#include <iostream>

using namespace KTN::REPORTER;

AlgoPLReporter::AlgoPLReporter(string guid)
	: ReporterBase("ALGOPLRPT", 1000000), _qPos(256),_ssboe_us(0),_GUID(guid)
{
	Start();
}


AlgoPLReporter::~AlgoPLReporter()
{
}

void AlgoPLReporter::StopWork()
{
    Stop();
}


void AlgoPLReporter::Report(Position& data)
{
	_qPos.push(data);
}

void AlgoPLReporter::DoWork()
{
	//std::vector<Position> msgs;
	//uint64_t hbus = _hb_interval * 1000 * 1000;
	uint64_t ssboe = getSsboeUs();
	string updtime = getTimeString();

	std::unordered_map<string, Position> posmap;

	if (!_qPos.empty())
	{
		Position pos = {};
		//_qMsgs.try_dequeue(msg);
		while(_qPos.try_pop(pos))
		{
			pos.algoid = _GUID;
			pos.ssboe = ssboe;
			pos.updatetime = updtime;
			//msgs.push_back(pos);
			posmap[pos.posid] = pos;
		}
	}

	for(auto kv : posmap)
	{
		Position pos = kv.second;
		string szjson = PositionJsonConverter::toJsonString(pos);
		SendJson(szjson);
		//cout << "ALGOPLRPT REPORTER TO BASE: "  << szjson << endl;
		usleep(10);
	}
}

