//============================================================================
// Name        	: SpinClient.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Dec 10, 2024 10:45:48 AM
//============================================================================

#ifndef SRC_DATAHANDLERS_CFESPININCLIENT_HPP_
#define SRC_DATAHANDLERS_CFESPININCLIENT_HPP_

#pragma once
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <unordered_map>
#include <set>
#include <functional>

#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_queue.h>

#include <KT01/Core/Log.hpp>
#include <KT01/Core/NamedThread.hpp>

#include <Settings/DataProcSettings.hpp>

#include <KT01/Net/byte_writer.hpp>
#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/helpers.hpp>

// spin server
#include <KT01/Net/helpers.hpp>
#include <KT01/Net/Tcp/TcpCallback.hpp>
#include <KT01/Net/Tcp/TcpHandlerInterface.hpp>
#include <KT01/Net/Tcp/StandardTcpV1.hpp>
#include <KT01/Net/Tcp/TcpHandlerFactory.hpp>

// book building
#include <KT01/DataStructures/DepthBook.hpp>
#include <KT01/DataStructures/LimitOrderBook.hpp>
#include <KT01/DataStructures/MDOrder.hpp>

#include <KT01/SecDefs/CfeSecMaster.hpp>

#include <exchsupport/cfe/pitch/PitchMessages.hpp>
#include <exchsupport/cfe/pitch/PitchMsgTypes.hpp>

#include <exchsupport/cfe/helpers/CfeHelpers.hpp>

#include <exchsupport/cfe/spin/SpinMessages.hpp>
#include <exchsupport/cfe/spin/SpinCallback.hpp>
#include <exchsupport/cfe/config/CfeSpinConfig.hpp>

namespace KTN
{
	namespace CFE
	{
		namespace Spin
		{

			struct DebugState
			{
				bool DebugEnabled;
				int DebugSecid;
				int DebugSpreadSecid;
				string DebugSymbol;
				string DebugSpreadSymbol;
				bool DebugMbp;
				bool DebugMbo;
				bool DebugCertMode;
				bool DebugInstDefs;
				bool DebugSpin;
				bool DebugSpinBooks;
				bool DebugSpinImage;
			};

			class SpinClient : public KT01::NET::TCP::TcpCallback8t
			{
			public:
				using byte_ptr = uint8_t*;

				SpinClient(SpinCallback *spinCallback);
				virtual ~SpinClient();
				void Start();
				void Stop();

				bool Connect();
				void Disconnect();
				bool IsConnected() const { return _Connected.load(std::memory_order_relaxed); }

				void SendSpinRequest(uint32_t seq);

				uint32_t CurrentSpinImageSeq() const { return _CURRENT_SPIN_SEQ; }
				bool IsSpinning() const { return _SPINNING; }

				/**
				 * @brief Callback function for receiving TCP data.
				 * @param data The received data.
				 * @param len The length of the data.
				 * @param segid The segment ID.
				 * @param index The index.
				 * @return The number of bytes processed.
				 */
				uint16_t onRecv(uint8_t* data, int len, int segid, int index) override;

			private:
				void LogMe(std::string szMsg);

				void LoadDebugSettings();

				// thread functions: producer/consumer
				void SpinThread();
				void ConsumerFunction();

				// sendin tcp messages
				void Send(char *buffer, size_t len);
				void SendLogin(string username, string password, string subid);
				void SendHeartbeat();

				// message processing
				void MsgHandler(byte_ptr buf, int len);
				int ProcessBuffer(byte_ptr buf, int len);
				int ProcessMessage(int iTemplateid, byte_ptr buf, int iMsgLen);

				// spin session messages
				int ProcessLoginResponseMsg(byte_ptr data, size_t len);
				int ProcessSpinImageAvailableMsg(byte_ptr data, size_t len);
				int ProcessSpinResponseMsg(byte_ptr data, size_t len);
				int ProcessSpinFinishedMsg(byte_ptr data, size_t len);

				// spin messages to reconstruct the book
				int ProcessTimeMsg(byte_ptr data, size_t len);
				int ProcessAddOrderLongMsg(byte_ptr data, size_t len);
				int ProcessAddOrderShortMsg(byte_ptr data, size_t len);
				int ProcessTradingStatusMsg(byte_ptr data, size_t len);
				int ProcessTimeReferenceMsg(byte_ptr data, size_t len);
				int ProcessSettlementMsg(byte_ptr data, size_t len);
				//int ProcessFuturesInstrumentDefinitionMsg(byte_ptr data, size_t len);
				int ProcessPriceLimitsMsg(byte_ptr data, size_t len);

				inline void msgcnt()
				{
					_msgcount++;

					if (_msgcount % 10000 == 0 || (_msgcount % 1000 == 0 && _msgcount <= 10000))
					{
						ostringstream xxx;
						xxx << "MSG COUNT:" << _msgcount;
						LogMe(xxx.str());
					}
				}

			private:
				SpinCallback& _spinCallback;
				DebugState dbg;
				string _DATA_CENTER;

				std::shared_ptr<KT01::SECDEF::CFE::CfeSecMaster> _secmaster;

				std::atomic<bool> _bActive;
				std::atomic<bool> _SPIN_THREAD_ACTIVE;

				std::atomic<bool> _Connected;

				// we have a spin thread for the polling function, and a consumer thread to process the data
				std::thread _spinthread;
				std::thread _hbthread;

				KT01::NET::TCP::TcpHandlerInterface *_tcpHandler = nullptr;

				std::vector<uint8_t> _tcpRemainder;

				std::atomic<uint32_t> _SEQA;
				std::atomic<uint32_t> _SEQ_OUT;

				std::atomic<bool> _SPINNING;
				std::atomic<uint32_t> _CURRENT_SPIN_SEQ;

				std::unordered_map<int, LimitOrderBook> _lob;
				std::unordered_map<string, LimitOrderBook> _lob_temp;

				NamedThread consumerThread;
				DataProcSettings _settings;

				tbb::concurrent_queue<std::pair<byte_ptr, int>> _qspin;
				tbb::concurrent_queue<std::pair<char *, int>> _qsend;
				tbb::concurrent_queue<std::pair<char *, int>> _qsuspense;

				// legacy vars
				long long _msgcount;

				uint32_t _SPIN_ORDERS_COUNT;
				uint32_t _SPIN_ORDERS_PROCESSED;

				uint32_t _SPIN_START_SEQ;
				uint32_t _SPIN_END_SEQ;

				// debug misc
				long _spinImageCnt;
				const int _spinDbgImageCnt = 2;

				//easy output of secdefs to file for cert:
				std::unordered_map<string, KT01::SECDEF::CFE::CfeSecDef> _secdef_map;

				struct ConnConfig
				{
					std::string ip;
					int port = 0;
					std::string username;
					std::string password;
					std::string subid;
					std::string interfaceName;
					std::string interfaceIp;
					bool loaded = false;
				} _connConfig;
			};

			//============================================================================
		} // namespace Spin
	} // namespace CFE
} // namespace KTN

#endif /* SRC_DATAHANDLERS_SpinClient_HPP_ */
