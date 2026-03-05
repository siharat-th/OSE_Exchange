/*
 * CfeMktDataHandler.cpp
 *
 *  Created on: Sep 27, 2016
 *  Massively updated on: Dec 7 2023
 *      Author: sgaer
 */

#include "CfeMktDataHandler.hpp"
#include <algorithm>
#include <cctype>
#include <KtnEfvi/EFVIFeedV3.h>
//#include <KtnEfvi/SocketFeed.h>
using i01::net::EFVIFeedV3;

// CfeMktDataHandler * m_pThisHandler;

CfeMktDataHandler::CfeMktDataHandler(const std::string &name, int core)
	: m_Me("CfeHandler")
	, _FEED(nullptr)
	, core(core)
	, name(name)
{
}

CfeMktDataHandler::~CfeMktDataHandler()
{
    if (_FEED)
    {
        LogMe("Stopping EFVI feed...");
        _FEED->Stop();
       
        LogMe("Deleting EFVI feed...");
        delete _FEED;
        _FEED = nullptr;
        LogMe("EFVI feed deleted");
    }
    
}

void CfeMktDataHandler::Stop()
{
	LogMe("STOPPING EFVI MAIN FEED");
	if (_FEED)
	{
		//_FEED->Stop();
	}

	_mp.Stop();

	LogMe("FEEDS STOPPED. EXITING");
	std::exit(0);
}

void CfeMktDataHandler::LogMe(std::string szMsg)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void CfeMktDataHandler::Start()
{
	ostringstream os;
	os << "****CFE KATANA MDS STARTING****";
	LogMe(os.str());

	_settings.Init();
	
	_roots = _settings.RootSymbols;
	_chids = _settings.Channels;

	LogMe("STARTING MESSAGE PROCESSOR");
	//_mp.Start();

	Settings settings;
	settings.Load("DataSettings.txt", "DataSettings");

	string dir = settings.getString("Cfe.ConfigDir");
	if (!dir.empty() && dir.back() != '/')
    	dir += '/';

	string chanfile = settings.getString("Cfe.ChannelFile");
	string datacenter = settings.getString("Cfe.DataCenter");
	string feed = settings.getString("Cfe.Feed");

	_USE_PITCH = settings.getBoolean("Cfe.UsePitch");
	if (_USE_PITCH)
		LogMe("OK: CFE PITCH ENABLED");
	else
		KT01_LOG_WARN(__CLASS__, "CFE PITCH DISABLED");


	_USE_TOP = settings.getBoolean("Cfe.UseTop");
	if (_USE_TOP)
		LogMe("OK: CFE TOP ENABLED");
	else
		KT01_LOG_WARN(__CLASS__, "CFE TOP DISABLED");


	// if (!dir.empty() && dir.back() != '/')
	// 	dir += '/';

	KTN::CFE::CfeChannels _Channels;
	_Channels.Load(dir + chanfile);

	//std::string transport = settings.getString("Cfe.Transport");
	//bool useSocketFeed = false;
	//if (!transport.empty())
	//{
	//	std::string normalized = transport;
	//	std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char c) {
	//		return static_cast<char>(std::tolower(c));
	//	});
	//	useSocketFeed = (normalized == "socket");
	//}

	//if (useSocketFeed)
	//{
	//	LogMe("INITIALIZING SocketFeed TRANSPORT");
	//	_FEED = new SocketFeed(*this);
	//}
	//else
	//{
	//	LogMe("INITIALIZING EFVIFeed TRANSPORT");
	//	_FEED = new EFVIFeedV3(*this, false);
	//}

	LogMe("INITIALIZING EFVIFeed TRANSPORT");
	_FEED = new EFVIFeedV3(*this, false, core, name);

	for (auto chid : _chids)
	{
		if (_USE_PITCH)
		{
			LogMe("STARTING UDP EFVI FOR PITCH UNIT ID " + chid);
			KTN::CFE::Channel mychannel = _Channels.GetChannel("PITCH", datacenter, atoi(chid.c_str()), feed);
			_FEED->AddFilter(mychannel.ip, mychannel.port, mychannel.feed);
		}

		if (_USE_TOP)
		{
			LogMe("STARTING UDP EFVI FOR TOP UNIT ID " + chid);
			KTN::CFE::Channel mychannel = _Channels.GetChannel("TOP", datacenter, atoi(chid.c_str()), feed);
			_FEED->AddFilter(mychannel.ip, mychannel.port, mychannel.feed);
		}
	}

	if (_FEED)
	{
		LogMe("STARTING EFVI FEED");
		_FEED->Start();
		LogMe("EFVI FEED STARTED");
	}
}

void CfeMktDataHandler::MenuChoice(int choice)
{
	ostringstream oss;
	oss << "GOT MENU CHOICE " << choice;
	LogMe(oss.str());

	if (choice == 11)
		PrintStats();
}

void CfeMktDataHandler::Subscribe(KTN::AlgoMktDataCallback *subscriber)
{
	_mp.Subscribe(subscriber);
}

void CfeMktDataHandler::AddSymbol(const std::string &symbol, int index, uint32_t secId)
{
	_mp.AddSymbol(symbol, index, secId);
}

void CfeMktDataHandler::onRecv(char *buf, int len, int segid)
{
	_mp.Process(buf, len);
}

void CfeMktDataHandler::PrintStats()
{
	LogMe("EFVI_FEED SEQ NUMBERS:");
	if (_FEED)
	{
		_FEED->printSeqs();
	}
}
