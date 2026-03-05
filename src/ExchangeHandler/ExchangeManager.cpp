//============================================================================
// Name        	: ExchangeManager.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 24, 2023 9:17:45 AM
//============================================================================

#include <ExchangeHandler/ExchangeManager.hpp>

ExchangeManager::ExchangeManager(tbb::concurrent_queue<KTN::OrderPod>& qords,
		const string& settingsfile) : _qords(qords)
{
	_ACTIVE.store(true, std::memory_order_release);

	std::vector<std::string> exchlist;
	Settings settings(settingsfile);

	std::string szlist = settings.getString("Exchanges");
	exchlist = StringSplitter::Split(szlist, ",");

	_SOURCE = settings.getString("Source");
	int affinity = settings.getInteger("Affinity");

	for (auto filename : exchlist)
	{
		KT01_LOG_INFO(__CLASS__, "LAUNCHING SESSION FROM " + filename);

		if (!file_exists(filename.c_str()))
		{
			KT01_LOG_ERROR(__CLASS__, "CANNOT FIND EXCHANGE CONFIG FILE: " + filename);
			KT01_LOG_ERROR(__CLASS__, "EXITING NOW. PLEASE CHECK THE 'ExchangeManager' SETTING IN MAIN CONFIG!");
			std::exit(1);
		}
		//single exch mode for now
		_filename = filename;
		break;
	}

	CreateAndRun("exch-mgr", affinity);
}

ExchangeManager::~ExchangeManager() {
}

void ExchangeManager::LogMe(std::string szMsg, LogLevel::Enum loglevel , bool Persist)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void ExchangeManager::Run()
{
	Settings settExch(_filename);
	string exch = settExch.getString("Exchange.Name");
	string proto = settExch.getString("Exchange.Protocol");

	if (exch.length() == 0)
	{
		for (int n = 0; n < 5; n++)
			KT01_LOG_ERROR(__CLASS__, "!!!FATAL: Exchange.Name SETTING NOT FOUND IN FILE " + _filename);

		std::exit(1);
	}

	LogMe("LAUNCHING EXCHANGE " + exch + " PROTOCOL: " + proto, LogLevel::INFO);


	//this is designed to be a very flexible approach to hosting exchange adapters
	//right now, single exchange, but we can adadpt this structure for multi-session/mutli-exch
	//for example, use a container HERE, so all exchanges created in the same thread,
	//or optionally, change this class to spawn a thread for each exchange.

	ExchangeBase2* sess;

	if (exch == "cme" || exch == "CME")
	{
		LogMe("ADDING EXCHANGE: CME_IL3" , LogLevel::INFO);
		//sess = new CmeAlgo(_qords, _filename, _SOURCE);
		sess = new CmeGT5(_qords, _filename, _SOURCE);
	}
	else if (exch == "cfe" || exch == "CFE")
	{
		KT01_LOG_INFO(__CLASS__, "ADDING EXCHANGE: CFE");
		sess = new KTN::CFE::CfeGT4(_qords, _filename, _SOURCE);
	}
	else if (exch == "eqt" || exch == "EQT")
	{
		KT01_LOG_INFO(__CLASS__, "ADDING EXCHANGE: EQT");
		sess = new KTN::EQT::EqtGT4(_qords, _filename, _SOURCE);
	}
	else if (exch == "ose" || exch == "OSE")
	{
		KT01_LOG_INFO(__CLASS__, "ADDING EXCHANGE: OSE");
		sess = new KTN::OSE::OseGT(_qords, _filename, _SOURCE);
	}
	else
	{
		KT01_LOG_ERROR(__CLASS__, "EXCHANGE NAME NOT FOUND: " + exch);
		return;
	}

	string threadname = exch + "-handl";
	pthread_setname_np(pthread_self(),threadname.c_str());

	_ACTIVE.store(true, std::memory_order_acquire);

	while(_ACTIVE.load(std::memory_order_acquire) == true)
	{
		sess->Poll();

		if (!_qcmds.empty())
		{
			Instruction inst = {};
			inst.isorder = false;
			while(_qcmds.try_pop(inst))
			{
				HandleCommand(inst, sess);
				inst = {};
			}
		}
	}

	LogMe("EXITING MAIN POLLING LOOP!!!!", LogLevel::ERROR);

}

void ExchangeManager::onCommand(const Instruction& cmd)
{
	_qcmds.push(cmd);
}

void ExchangeManager::HandleCommand(Instruction& cmd, ExchangeBase2* sess)
{
	if (cmd.isorder == true)
	{
		switch(cmd.command)
		{
		case ExchCmd::ORD_SEND:
			sess->Send(cmd.ord, cmd.ord.OrdAction);
			return;
		case ExchCmd::ORD_SEND_HDG_INSTRUCT:
			sess->SendHedgeInstruction(cmd.reqid, cmd.hdgs);
			return;
		case ExchCmd::ORD_SEND_WITH_HEDGE:
			sess->SendHedgeInstruction(cmd.reqid, cmd.hdgs);
			sess->Send(cmd.ord, cmd.ord.OrdAction);
			return;
		}
	}

	LogMe("SENDING TO SESS: " + ExchCmd::toString(cmd.command),LogLevel::INFO);
	sess->Command(cmd);
}