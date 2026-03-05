//============================================================================
// Name        	: EFVITcpV5.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 14, 2023 12:16:20 PM
//============================================================================

#include <KT01/Net/Tcp/EFVITcpV5.hpp>

using namespace KT01::NET::TCP;

EFVITcpV5::EFVITcpV5(TcpCallback8t *callback) : _callback(callback), _fd(0), _segid(0)
{
}

EFVITcpV5::~EFVITcpV5()
{
}

void EFVITcpV5::LogMe(std::string szMsg)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void EFVITcpV5::Subscribe(TcpCallback8t *callback)
{
	_callback = callback;
}

bool EFVITcpV5::Init(ServiceDescriptor &feed, bool isA = true)
{
	LogMe("LOADING TCP SETTINGS FROM NetSettings.txt");
	NetSettings::Load(_sett, KT01::NET::Protocol::TCP, "NetSettings.txt");

	_SEGID = feed.iSource;
	_INDEX = feed.index;

	OFFSET = EFVI_OFFSET;

	last_recv_val = 0;
	last_send_val = 0;

	if (_sett.UseCtpio)
		LogMe("**** USING CTPIO ******");

	feed.localinterface = (isA) ? _sett.IntfA : _sett.IntfB;
	feed.localinterfacename = (isA) ? _sett.IntfNameA : _sett.IntfNameB;
	feed.feedtype = FEED_OE;

	if (_sett.Debug)
		LOGWARN("[EFVITcpV5] Init | feed.localinterface: {}", feed.localinterface);
	

	// PollNet Start
	const char *err = client.init(feed.localinterfacename.c_str());
	if (err)
	{
		ostringstream oss;
		oss << "CLIENT INIT ERROR: " << err << endl;
		perror("CLIENT_INIT");
		KT01_LOG_ERROR(__CLASS__, oss.str());
		std::exit(1);
	}

	ostringstream oss;
	oss << "TCPV5 CONNECTING TO " << feed.address << ":" << feed.port
		<< " ON " << feed.localinterfacename;
	LogMe(oss.str());
	err = client.connect(feed.address.c_str(), feed.port);

	if (err)
	{
		cout << "client.connect ERRROR: " << err << endl;
		return false;
	}
	else
		LogMe("TCP5 EFVI TCP CONNECTION TO " + feed.address + " OK");

	_fd = feed.index + 1;
	_segid = feed.index;
	_index = feed.index;

	return true;
}

void EFVITcpV5::Stop()
{
	ostringstream oss;
	oss << "RECEIVED STOP FOR FD=" << _fd;
	LogMe(oss.str());

	bye();
}

void EFVITcpV5::poll()
{
	client.poll(*this);
}

bool EFVITcpV5::bye()
{
	client.poll(*this);
	if (!conn_ || conn_->isClosed())
		return true;
	conn_->sendFin();
	return false;
}

void EFVITcpV5::onConnectionRefused()
{
	// put us in a bad state so we CANT send until restart.
	_fd = -1;
	KT01_LOG_ERROR(__CLASS__,"****" + std::to_string(_segid) + " TCP CONNECTION REFUSED!!!");
}

void EFVITcpV5::onConnectionEstablished(TcpConn &conn)
{
	conn_ = &conn;

	if (_sett.Debug)
		LOGWARN("[EFVITcpV5] onConnectionEstablished | sendable: {}", conn.getSendable());
}

uint32_t EFVITcpV5::onData(TcpConn &conn, uint8_t *data, uint32_t size)
{
	if (_sett.Debug)
		LOGWARN("[EFVITcpV5] onData | got size: {}", size);


	int32_t remaining_size = size;
	uint32_t processed_size = 0;

	//	char* result = new char[4096];
	//	__builtin_memcpy(result, data, remaining_size);
	//
	//	if (LIKELY(_callback))
	//		processed_size =_callback->onRecv(result, remaining_size, _SEGID, _INDEX);

	processed_size = _callback->onRecv(data, remaining_size, _SEGID, _INDEX);

	remaining_size = size - processed_size;

	if (remaining_size < 0)
		remaining_size = 0;

	//	while (processed_size < size) {
	//		//Packet recv_pack;
	//		//_trk.startMeasurement("EFVIRecv");
	//		char* result = new char[4096];
	//		__builtin_memcpy(result, data, remaining_size);
	//		uint8_t* processed_data = data;// + processed_size;
	//
	//	//	_trk.stopMeasurement("EFVIRecv");
	//
	//		// Pass processed data to _callback.onReceive()
	////		cout << "******TCP5 TO CB=" << remaining_size << endl;
	//		processed_size +=_callback->onRecv((char*)processed_data, remaining_size, _SEGID);
	//	//	cout << "******TCP5 CB TOTAL PROCESSED=" << processed_size << endl;
	//
	//		data += processed_size;
	//		remaining_size -= processed_size;
	//
	//		if (processed_size >= size)
	//			break;
	//	}
	//DBG_FILE_LINE
	if (_sett.Debug)
	{
		LOGWARN("[EFVITcpV5] onData | processed_size: {} | remaining_size: {}", processed_size, remaining_size);
	}

	return remaining_size;

	//
	//	while (size >= sizeof(Packet)) {
	//	  const Packet& recv_pack = *(const Packet*)data;
	//	//  auto lat = now - recv_pack.ts;
	//
	//	  cout << "recv val: " << recv_pack.val << endl;//<< " latency: " << lat << endl;
	////	  if (recv_pack.val != ++last_recv_val)
	////	  {
	////		cout << "invalid recv val: " << recv_pack.val << ", last_recv_val: " << last_recv_val << endl;
	////		exit(1);
	////	  }
	//	  data += sizeof(Packet);
	//	  size -= sizeof(Packet);
	//	}
	//	_callback->onRecv((char*)data, size, _SEGID);
	//
	////	conn.setUserTimer(0, 1 * 1000); // reset 10 secs recv timeout
	//	cout << "onData RETURNING " << size << endl;
	//	return size;
}

void EFVITcpV5::onConnectionReset(TcpConn &conn)
{
	ostringstream oss;
	oss << "SEG " << _segid << " TCP: onConnectionReset";
	LogMe(oss.str());
}

void EFVITcpV5::onConnectionClosed(TcpConn &conn)
{
	ostringstream oss;
	oss << "SEG " << _segid << " TCP: onConnectionClosed" << endl;
	LogMe(oss.str());

	if (LIKELY(_callback != nullptr))
	{
		_callback->onConnectionClosed(_SEGID, _INDEX);
	}
}

void EFVITcpV5::onFin(TcpConn &conn, uint8_t *data, uint32_t size)
{
	ostringstream oss;
	oss << "SEG " << _segid << " TCP: onFin, remaining data size:" << size << endl;
	conn.sendFin();
	LogMe(oss.str());
}
void EFVITcpV5::onConnectionTimeout(TcpConn &conn)
{
	ostringstream oss;
	oss << "SEG " << _segid << " TCP: onConnectionTimeout, established: " << conn.isEstablished() << endl;
	LogMe(oss.str());
}

void EFVITcpV5::onMoreSendable(TcpConn &conn)
{
}

void EFVITcpV5::onUserTimeout(TcpConn &conn, uint32_t timer_id)
{
}

void EFVITcpV5::Poll()
{
	poll();
}

void EFVITcpV5::Send(char *buffer, size_t buffer_length, bool send_delegated)
{
	uint32_t result = 0;
	if (UNLIKELY(_fd <= 0))
	{
		KT01_LOG_ERROR(__CLASS__,"TCPV5 CANNOT SEND WITH FD <= 0!");
		return;
	}

	if (send_delegated)
	{
		result = SendFastPath(buffer, buffer_length);
		cout << "[SendMaster DEBUG] SEND RESULT=" << result << endl;
		if (result != 0)
		{
			ostringstream oss;
			oss << "[SendMaster DEBUG]: SENDING ERROR. RC=" << result;
			LogMe(oss.str());
		}
		return;
	}

	if (_sett.Debug)
		LOGWARN("[EFVITcpV5] SendMaster | len={}", buffer_length);

	result = SendNormal(buffer, buffer_length);

	if (_sett.Debug)
		LOGWARN("[EFVITcpV5] SendMaster | result_code={}", result);
	
	if (result != buffer_length)
	{
		ostringstream oss;
		oss << "SendMaster(Normal): SENDING ERROR. RC=" << result << " BUFLEN=" << buffer_length;
		KT01_LOG_ERROR(__CLASS__, oss.str());
	}
}

int EFVITcpV5::SendNormal(char *buffer, size_t buffer_length)
{
	int rc = client.getConn().send(buffer, buffer_length, false);

	if (_sett.Debug)
		LOGWARN("[EFVITcpV5] SendNormal | result_code={}", rc);

	return rc;
}

int EFVITcpV5::SendFastPath(char *buffer, size_t buffer_length)
{	
	int rc = client.getConn().send(buffer, buffer_length, false);

	if (_sett.Debug)
		LOGWARN("[EFVITcpV5] SendFastPath | result_code={}", rc);

	return rc;
	
}

void EFVITcpV5::SendWarm()
{
	client.getConn().send_warmup_to_conn();
	//	client.send_warmup();
}
