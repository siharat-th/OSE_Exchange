//============================================================================
// Name        	: AuditTrailWriter.cpp
// Author      	: SG
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: July 3, 2023 1:50:50 PM
//============================================================================

#include <ExchangeHandler/audit/AuditTrailWriter.hpp>

AuditTrailWriter::AuditTrailWriter(std::unique_ptr<IAuditProcessor> processor, BufferPool &sendBufferPool)
: _processor(std::move(processor)), _CORE(1),
/*_MQ(1000),_BUFQ(1000),*/ sendBufferPool(sendBufferPool)
{
	_ACTIVE.store(false, std::memory_order_relaxed);
}

AuditTrailWriter::~AuditTrailWriter() {
	KT01_LOG_INFO(__CLASS__, "DESTRUCTOR CALLED, setting _ACTIVE to false");
	_ACTIVE.store(false, std::memory_order_relaxed);
}

void AuditTrailWriter::Start(string dir, string sessid, string firmid, int core)
{
	_FIRMID = firmid;
	_SESSID = sessid;
	_CORE = core;

	if (!dir.empty() && dir.back() != '/') {
		dir = dir + '/';
	}
	_DIR = dir;

	KT01_LOG_INFO(__CLASS__, "STARTING AUDIT WRITER IN DIR: " + _DIR,_me);

	CreateAndRun("auditsvc", _CORE);
}

void AuditTrailWriter::InitFileForSeg(int segid)
{
	Key key = {};
	key.segid = segid;
	key.firmid = _FIRMID;
	key.sessid = _SESSID;

	if (_logmap.find(key) == _logmap.end())
	{
		std::ostringstream oss;
		oss << _SESSID << "-" << _FIRMID << "-" << (int)segid;
		KT01_LOG_INFO(__CLASS__, "NEED NEW FILE FOR KEY " + oss.str(), "AuditTrailWriter");
		CreateAuditFile( _SESSID, _FIRMID, segid);
	}
}

void AuditTrailWriter::CreateAuditFile(string sessid, string firmid, int segid)
{
	Key key = {};
	key.sessid = sessid;
	key.firmid = firmid;
	key.segid = segid;

	// Get the current system time
	auto currentTime = std::chrono::system_clock::now();

	// Convert the system time to a time_t object
	std::time_t currentTimeT = std::chrono::system_clock::to_time_t(currentTime);

	// Convert the time_t object to a tm struct
	std::tm* timeInfo = std::localtime(&currentTimeT);

	// Format the date and time
	std::ostringstream tss;
	tss << std::put_time(timeInfo, "%Y%m%d%H%M%S");
	std::string formattedDateTime = tss.str();

	ostringstream oss;
	oss << sessid << "-" << firmid << "-" << segid;// << "_";// << tss.str();// << ".log";

	KT01_LOG_INFO(__CLASS__,"MAPPING FILE: " + oss.str());

	size_t maxSize = 1048576;            // Maximum size of a log file in bytes (10 MB in this example)
	bool rotateDaily = true;            // Set to 'true' for daily rotation, 'false' for size-based rotation

	// Create and initialize the logger
	Key newKey = {_SESSID, _FIRMID, segid};
	_logmap[key].Init(_DIR, oss.str(), maxSize, rotateDaily);

}

bool AuditTrailWriter::fileExists(string filename)
{
	 std::ifstream file(filename);
	 return file.good();
}

bool AuditTrailWriter::fileIsOpen(string filename)
{
	std::ifstream file(filename);

	// Check if the file is open
	if (!file.is_open()) {
		ostringstream oss;
		oss << "Failed to open the file: " << filename;
		KT01_LOG_INFO(__CLASS__,oss.str(), _me);
		return false;
	}

	return true;
}

void AuditTrailWriter::Write(KTNBuf* msg)
{
//	_MQ.enqueue(msg);
	//_MQ.push(msg);
	msgQueue.push(msg);
}

// void AuditTrailWriter::Write(char* buf, int msgtype, int segid)
// {
// 	//_BUFQ.enqueue(std::make_tuple(buf,msgtype, segid));
// 	_BUFQ.push(std::make_tuple(buf,msgtype, segid));
// }

void AuditTrailWriter::Write(KTNBuf8t &&msg)
{
	msgQueue.push(std::move(msg));
}

void AuditTrailWriter::Run() {

    string firmid = _FIRMID;
    string sessid = _SESSID;
    // Sleep for 500 milliseconds
    //longer means less contention on the queue. why there's contention on concurrent queue I have no idea.
    int isleepus = 10000;// * 1000;
   // int isleepus = 20000 * 1000;

	_ACTIVE.store(true, std::memory_order_relaxed);
	// while(_ACTIVE.load(std::memory_order_relaxed) == true){

    //     if(_BUFQ.empty())
    //     {
    //     	usleep(isleepus);
    //     	continue;
    //     }

	// 	std::tuple<char*, int, int> tup;
	// 	while(_BUFQ.try_pop(tup))
	// 	{
	// 		KTNBuf* msg = new KTNBuf();
	// 		msg->buffer = std::get<0>(tup);
	// 		msg->msgtype = std::get<1>(tup);
	// 		msg->segid = std::get<2>(tup);
	// 		ProcessLine(msg->buffer, msg->msgtype, (int)msg->segid);
	// 		usleep(1000);
	// 	}

    //     usleep(isleepus);
    // }

	while (_ACTIVE.load(std::memory_order_relaxed) == true)
	{
		if (msgQueue.empty())
		{
			usleep(isleepus);
			continue;
		}

		MessageVariantType msgVariant;
		while (msgQueue.try_pop(msgVariant))
		{
			if (std::holds_alternative<KTNBuf *>(msgVariant))
			{
				KTNBuf *msg = std::get<KTNBuf *>(msgVariant);
				ProcessLine(msg->buffer, msg->msgtype, static_cast<int>(msg->segid), msg->customerOrderTime);
				sendBufferPool.put(msg);
				usleep(1000);
			}
			else if (std::holds_alternative<KTNBuf8t>(msgVariant))
			{
				KTNBuf8t &msg = std::get<KTNBuf8t>(msgVariant);
				ProcessLine(reinterpret_cast<char *>(msg.buffer), msg.msgtype, static_cast<int>(msg.segid));
				usleep(1000);
			}
			else
			{
				KT01_LOG_ERROR(__CLASS__, "Unknown variant type in audit message queue!");
			}
		}

		usleep(isleepus);
	}

    KT01_LOG_INFO(__CLASS__,"CLOSING AUDIT FILES AND EXITING AUDIT WRITER!");

}

bool AuditTrailWriter::KeyExists(int segid)
{
	// Key to search for
	Key searchKey = {_SESSID, _FIRMID, segid};

	// Searching for the key
	auto it = _keySet.find(searchKey);

	if (it != _keySet.end()) {
		return true;
	} else {
		return false;
	}

}

void AuditTrailWriter::ProcessLine(char* buf, int iTemplateid, int segid, uint64_t customerOrderTime)
{
	if (segid == 0)
	{
		KT01_LOG_INFO(__CLASS__, "SEGID OF 0 SENT!!! NO AUDIT SAVED FOR TEMPLATE " +
				std::to_string(iTemplateid));
	}

	Key key = {};
	key.segid = segid;
	key.firmid = _FIRMID;
	key.sessid = _SESSID;

	if (_logmap.find(key) == _logmap.end())
	{
		std::ostringstream oss;
		oss << _SESSID << "-" << _FIRMID << "-" << (int)segid;
		KT01_LOG_INFO(__CLASS__, "NEED NEW FILE FOR KEY " + oss.str());
		CreateAuditFile( _SESSID, _FIRMID, segid);
	}

	std::string json = process(buf, iTemplateid, customerOrderTime);

	_logmap[key].log(json);

}

std::string AuditTrailWriter::process(char* buf, int iTemplateid, uint64_t customerOrderTime) {
    return _processor->process(buf, iTemplateid, customerOrderTime);
}
