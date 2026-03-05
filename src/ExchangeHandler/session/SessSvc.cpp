//============================================================================
// Name        	: SessSvc.cpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Feb 3, 2023 1:50:50 PM
//============================================================================

#include <ExchangeHandler/session/SessSvc.hpp>

SessSvc::SessSvc(string sessid, string firmid, int segid, int keepalive, bool dbg)
: _DIR("MsgStorage/"), _keepalive(keepalive), _sessid(sessid), _firmid(firmid), _segid(segid),_DBG(dbg)
{
	ostringstream oss;
	oss << _sessid <<"-" << firmid << "-" << segid << ".STATE";
	_FILENAME =  oss.str();

	ostringstream ss;
	ss << "seg" << segid << "-sesslog";

	CreateAndRun(ss.str(),2);
}

SessSvc::~SessSvc() {
	_ACTIVE.store(false, std::memory_order_relaxed);

}

void SessSvc::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}


void SessSvc::Start()
{

}

void SessSvc::Persist(const SessStateMsg& msg)
{
	_MSGS.push(msg);
}

void SessSvc::Run()
{
	std::string filename = _DIR + "/" + _FILENAME;
	std::fstream file;
	file.open(filename, std::ios::out | std::ios::in);

	if (!file) {
	    LogMe("SESS SVC: File not found: " + filename + ". Creating a new file.", LogLevel::WARN);

	    file.open(filename, std::ios::out);
	    if (!file) {
	        LogMe("SESS SVC: Error creating file: " + filename + ". Exiting SESS SVC thread.", LogLevel::ERROR);
	        std::exit(1);
	        return;
	    }
	}

	LogMe("OPENING SESS STATE FILE=" + filename, LogLevel::INFO);

	int isleepus = 10000;

	_ACTIVE.store(true, std::memory_order_relaxed);
	while (_ACTIVE.load(std::memory_order_relaxed)) {
	    SessStateMsg msg = {};
	    while (_MSGS.try_pop(msg)) {
	        // Close the file if it's already open
	        if (file.is_open()) {
	            file.close();
	        }
	        // Reopen the file in truncation mode to clear its content
	        file.open(filename, std::ios::out | std::ios::trunc);
	        if (!file) {
	            LogMe("SESS SVC: Error opening file for sess svc: " + filename, LogLevel::ERROR);
	            return;
	        }

	        std::string sztf = (msg.negotiated) ? "True" : "False";
	        file << "FirmID=" << msg.firmid << "\n";
	        file << "SessID=" << msg.sessid << "\n";
	        file << "SegID=" << msg.segid << "\n";
	        file << "Negotiated=" << sztf << "\n";
	        file << "UUID=" << msg.uuid << "\n";
	        file << "MsgSeqIn=" << msg.seqin << "\n";
	        file << "MsgSeqOut=" << msg.seqout << "\n";
	        file << "PreviousUUID=" << msg.previousuuid << "\n";
	        file << "PrevMsgSeqIn=" << msg.prevseqin << "\n";
	        file << "PrevMsgSeqOut=" << msg.prevseqout << "\n";
	        file << "SessionCreationTime="  << msg.sessioncreated << "\n";
	        file << "LastReqId=" << msg.lastreqid << "\n";
	        file.flush();

	        if (_DBG)
	            std::cout << "[SessSvc] File Saved: " << filename
	                      << " In=" << msg.seqin << " Out=" << msg.seqout << std::endl;
	    }
	    usleep(isleepus);
	}

	if (file.is_open()) {
		file.close();
		std::cout << filename << ": File closed." << std::endl;
	}

}

