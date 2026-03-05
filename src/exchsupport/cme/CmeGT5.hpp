//============================================================================
// Name        	: CmeGT5: 5th Generation CmeHandler. Better, faster, stronger.
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2024 Katana Financial
// Date			: Feb 9, 2024 6:24:37 PM
//============================================================================

#ifndef SRC_EXCHANGEHANDLERS_CMEGT5_HPP_
#define SRC_EXCHANGEHANDLERS_CMEGT5_HPP_

//#define ELIM_TESTING 1

#pragma once

#include <ExchangeHandler/ExchangeBase2.hpp>
using namespace KTN;

//note, we do this here in case we ever want an exchange WITHOUT an audit trail
#include <ExchangeHandler/audit/AuditTrailWriter.hpp>
#include <exchsupport/cme/audit/CmeAuditProcessor.hpp>


#include <exchsupport/cme/il3/IL3Includes.hpp>
#include <exchsupport/cme/il3/IL3Helpers.hpp>
#include <KT01/SecDefs/CmeSecMasterV2.hpp>
#include <exchsupport/cme/msgw/CmeMsgw.hpp>
#include <akl/PriceConverters.hpp>

#ifdef ELIM_TESTING
#include <exchsupport/cme/factory/ExecRptTestFactory.hpp>
#endif

using namespace KT01::SECDEF::CME;
using namespace KT01::MSGW;

using namespace KTN;
using namespace KTN::Session;
using namespace KTN::NET;
using namespace KTN::Core;



namespace KTN { namespace CME {

/**
 * @brief CmeGT5 class represents the 5th Generation CmeHandler.
 * Low latency order template creation; memory pool usage and custom maps and memory management.
 * It provides better, faster, and stronger functionality for handling CME exchange iLink3 Messages.
 */
class CmeGT5 : public ExchangeBase2, public TcpCallback8t
{
private:
	using byte_ptr = uint8_t*;

	vector<int> _SEGS;
	static constexpr int _MAX_SEGS = 8;
	ConnState _CX[_MAX_SEGS];
	
public:
	/**
  * @brief Constructor for CmeGT5 class.
  * @param qords The concurrent queue for order pods.
  * @param settingsfile The path to the settings file.
  * @param source The source of the CME exchange.
  */
	explicit CmeGT5(tbb::concurrent_queue<KTN::OrderPod>& qords,
			const std::string& settingsfile, const std::string& source);
	
	/**
  * @brief Destructor for CmeGT5 class.
  */
	virtual ~CmeGT5();

  /**
   * @brief Start the audit trail writer.
   * @param dir The directory path for storing the audit trail files.
   * @param sessid The session ID.
   * @param firmid The firm ID.
   * @param core The core number.
   */
  void StartAuditTrail(std::string dir, std::string sessid, std::string firmid, int core);

    /**
  * @brief Start the CmeGT5 handler.
  */
	void Start();

	/**
  * @brief Stop the CmeGT5 handler.
  */
	void Stop();

	/**
  * @brief Clear the maps used by the CmeGT5 handler.
  */
	void ClearMaps();

	/**
  * @brief Reset the CmeGT5 handler.
  */
	void Reset();

	/**
  * @brief Execute a command on the CmeGT5 handler.
  * @param cmd The instruction command to execute.
  */
	void Command(Instruction cmd);

	/**
  * @brief Print the latencies.
  * @param clear Flag indicating whether to clear the latencies after printing.
  */
	void PrintLatencies(bool clear);

	/**
  * @brief Poll for new messages.
  */
	void Poll();

	/**
  * @brief Processing thread for handling messages.
  */
	void ProcessingThread();

	/**
  * @brief Callback function for receiving data.
  * @param data The received data.
  * @param len The length of the data.
  * @param segid The segment ID.
  * @param index The index.
  * @return The number of bytes processed.
  */
	uint16_t onRecv(byte_ptr data, int len, int segid, int index) override;

	virtual void onConnectionClosed(int segid, int index) override;

  uint16_t ProcessRecvBuffer(byte_ptr buf, int len, int segid, int index, const uint64_t recvTime);
	/**
  * @brief Send an order.
  * @param ord The order to send.
  * @param action The action to perform.
  */
	void Send(KTN::OrderPod& ord, int action);

	/**
  * @brief Send a mass action.
  * @param ord The order to perform the mass action on.
  */
	void SendMassAction(KTN::Order& ord);

	/**
  * @brief Send a hedge instruction.
  * @param ordereqid The order request ID.
  * @param hdg The vector of spread hedge instructions.
  */
	void SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3>& hdg);

	/**
  * @brief Send a warm message.
  * @param index The index.
  */
	void SendWarm(int index);

	/**
  * @brief Print the session map.
  * @param conns The array of connection states.
  */
	void PrintSessionMap(ConnState(&conns)[_MAX_SEGS]);


private:

    /**
     * @brief Sets the last sent time for a connection state.
     * 
     * @param conn The connection state.
     */
    void setLastSentTime(ConnState& conn);

    /**
     * @brief Performs complex hedging based on order request ID, last quantity, last price, and callback ID.
     * 
     * @param ordreqid The order request ID.
     * @param lastqty The last quantity.
     * @param lastpx The last price.
     * @param cbid The callback ID.
     * @return True if the hedging is successful, false otherwise.
     */
    bool HedgeComplex(uint64_t ordreqid, int lastqty, int64_t lastpx, uint8_t cbid);

    /**
     * @brief Processes a vector of unique pointers to KTNBuf objects.
     * 
     * @param vbufs The vector of unique pointers to KTNBuf objects.
     */
    void ProcessHdgBuf(std::vector<std::unique_ptr<KTNBuf>>& vbufs);

    /**
     * @brief Processes a session message for a given connection state and message.
     * 
     * @param conn The connection state.
     * @param msg The session message.
     */
    void ProcessSessionMessage(ConnState& conn, KTNBuf8t &msg);

    /**
     * @brief Handles the state change event for a given connection state.
     * 
     * @param newState The new state.
     * @param conn The connection state.
     */
    void onStateChange(EnumSessionState::Enum newState, ConnState& conn);

    /**
     * @brief Handles the negotiate response message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onNegotiateResponseMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the negotiate reject message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onNegotiateRejectMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the establish acknowledgement message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onEstablishAckMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the establish reject message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onEstablishRejectMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the sequence message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onSequenceMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the terminate message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onTerminateMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the retransmission message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onRetransmissionMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the retransmit reject message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onRetransmitRejectMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the not applied message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onNotAppliedMsg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);


	//application msgs:
    /**
     * @brief Processes an application message for a given connection state and message.
     * 
     * @param conn The connection state.
     * @param msg The application message.
     */
    int ProcessApplicationMessage(ConnState& conn, KTNBuf8t &msg, bool ignore_seqs=false);
    /**
     * @brief Handles the execution report new message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onExecRptNew(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime);

    /**
     * @brief Handles the execution report modify message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onExecRptModify(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime);

    /**
     * @brief Handles the execution report cancel message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onExecRptCancel(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime);

    /**
     * @brief Processes the execution report trade outright message for a given buffer, message length, segment ID, and index.
     * 
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     * @param index The index.
     * @return The number of bytes processed.
     */
    int onExecRptTradeOutright(byte_ptr buf, int iMsgLen, int segid, int index, uint64_t recvTime);

    /**
     * @brief Processes the execution report trade spread message for a given buffer, message length, segment ID, and index.
     * 
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     * @param index The index.
     * @return The number of bytes processed.
     */
    int onExecRptTradeSpread(byte_ptr buf, int iMsgLen, int segid, int index, uint64_t recvTime);

    /**
     * @brief Processes the execution report trade spread leg message for a given buffer, message length, segment ID, and index.
     * 
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     * @param index The index.
     * @return The number of bytes processed.
     */
    int onExecRptTradeSpreadLeg(byte_ptr buf, int iMsgLen, int segid, int index, uint64_t recvTime);

    /**
     * @brief Handles the execution report reject message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onExecRptReject(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime);

    /**
     * @brief Handles the execution report elimination message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onExecRptElimination(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime);

    /**
     * @brief Handles the order mass action report message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    int onOrderMassActionReport(byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the order cancel reject message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     *  @return The number of bytes processed.
     */
    void onOrderCancelReject(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime);

    /**
     * @brief Handles the order cancel replace reject message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onOrderCancelReplaceReject(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime);

    /**
     * @brief Handles the party details definition request acknowledgement message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    int onPartyDetailsDefinitionRequestAck(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the party details list report message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    int onPartyDetailsListReport(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the business reject message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onBusinessReject(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the execution report status message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onExecRptStatus(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the execution acknowledgement message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onExecutionAck(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the trade addendum outright message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onTradeAddendumOutright(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the trade addendum spread message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onTradeAddendumSpread(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);

    /**
     * @brief Handles the trade addendum spread leg message for a given connection state, buffer, message length, and segment ID.
     * 
     * @param conn The connection state.
     * @param buf The buffer containing the message.
     * @param iMsgLen The length of the message.
     * @param segid The segment ID.
     */
    void onTradeAddendumSpreadLeg(ConnState& conn, byte_ptr buf, int iMsgLen, int segid);


    /**
     * @brief Utility Function to Check SeqNum for connection to detect gaps
     */
    bool CheckIncomingSeq(ConnState& conn, KTNBuf8t &msg);

    /**
        * @brief Utility Function for checking Fault Tolerance Indicator vs. State.USE_PRIMARY
        *
        * @param fti The EnumFTI indicating passed from the session message
        * @param seg the segment id
        * @param fn The message wanting to check this
        */
    inline bool CheckFaultTolerance(EnumFTI fti, int segid, string msg_name)
    {
      //bool res = false;
    	//standard TFFT matrix here; 2 vars with 2 possible values
    	//True/True
    	if (LIKELY(fti == EnumFTI::EnumFTI_Primary && _State.USE_PRIMARY))
    		return true;

    	//False/False
    	if (UNLIKELY(fti == EnumFTI::EnumFTI_Backup && !_State.USE_PRIMARY))
    		return true;

    	//False/True
    	if (UNLIKELY(fti == EnumFTI::EnumFTI_Backup && _State.USE_PRIMARY))
		{
			ostringstream oss;
			oss << "**SEG " << segid << " " << msg_name <<": FTI set to FTI_BACKUP but State.USE_PRIMARY set to TRUE. Failover is needed!";
			LogMe(oss.str(), LogLevel::ERROR);
			return false;
		}

    	//True/False
    	if (UNLIKELY(fti == EnumFTI::EnumFTI_Primary && !_State.USE_PRIMARY))
		{
			ostringstream oss;
			oss << "**SEG " << segid << " " << msg_name <<": FTI set to FTI_PRIMARY but State.USE_PRIMARY set to FALSE. Failover is needed!";
			LogMe(oss.str(), LogLevel::ERROR);
			return false;
		}

    	//default
    	return true;

    }

private:

	void SessionInit(ConnState& conn, int index);
	void SessionLogin(ConnState& conn);
	void Dispatch(ConnState& conn, bool checkhb);

	bool checkSessionIsConnected(ConnState& conn, int segid);
	bool CheckForHeartbeat(ConnState& conn);
	void CheckEvents(ConnState (&conns)[_MAX_SEGS]);


	//Session and Tcp
	bool StartTcpConnection(ConnState& conn, bool primary=true);
	void NegotiateSession(ConnState& conn);
	void EstablishSession(ConnState& conn);
	void SendSequenceMsg(ConnState& conn, EnumKeepAliveLapsed lapsed);
	void SendTerminateMsg(ConnState& conn, string reason, int errorcode);
	void SendRetransmitRequestMsg(ConnState& conn, int fromseq, int toseq, uint64_t lastuuid=IL3::UINT64_NULL);


	void SendPartyDetailsDefRequest(ConnState& conn);
	void SendPartyDetailsListRequest(ConnState& conn);
	void SendMassCxl(ConnState& conn, KTN::Order ord, int seg);
	void SendMassOrderStatusReq(ConnState& conn, EnumMassStatusReqTyp reqtype, int segid);

	int SkipGroupAndAdvance(byte_ptr& buf, int msgbytes);

	void Display();
	void Banner(string msg);

	inline void genAudit(KTNBuf*& x, byte_ptr buf, size_t iMsgLen, uint8_t segid, int msgtype)
	{
		
		__builtin_memcpy(x->buffer, buf, iMsgLen);
		x->buffer_length = iMsgLen;
		x->segid = segid;
		x->msgtype = msgtype;
		x->dir = 1;
		
	}

	inline int GetConnIndex(ConnState (&conns)[_MAX_SEGS], int segid)
	{
		for(size_t i = 0; i < _SEGS.size(); i++)
		{
			if (conns[i].segid == segid)
				return i;
		}
		return -1;
	}

  inline KOrderType::Enum ConvertOrderType(EnumOrderType ordtype)
  {
    switch (ordtype)
    {
      case EnumOrderType::EnumOrderType_Limit:
        return KOrderType::LIMIT;
      case EnumOrderType::EnumOrderType_MarketWithProtection:
        return KOrderType::MARKET;
      case EnumOrderType::EnumOrderType_MarketWithLeftoverAsLimit:
        return KOrderType::MARKET_LIMIT;
      case EnumOrderType::EnumOrderType_StopLimit:
        return KOrderType::STOPLIMIT;
      default:
        return KOrderType::LIMIT;
    }
  }


private:
	CmeSessionSettings _sett;

	//convenience thread takes work off critical path
	std::thread _procthread;
	std::atomic<bool> _PROC_THREAD_ACTIVE;

	CmeMsgw _msgw;

	tbb::concurrent_queue<KTN::OrderPod>& _qOrdsProc;
	tbb::concurrent_queue<KTNBuf> _qHdgProc;

	SessionMessageFactory _sessfact;
	AppMessageFactoryFastV2 _fastfact;

	SPSCRingBuffer<Instruction> _qCmds;
	SPSCRingBuffer<Instruction> _qAlgoCmds;

	//	SPSCRingBuffer<KTNBuf8t> _qRecvBufs;
	tbb::concurrent_queue<KTNBuf8t> _qRecvBufs;
	std::vector<uint8_t> _tcpRemainders[_MAX_SEGS];
	//SPSCRingBuffer<std::pair<byte_ptr, int>> _qRecvBufs;

	OrderPodPool _OrdPodPool;
	BufferPool _KtnBufs;
	//ObjectPool<char*> _HdgBufs;

	SPSCRingBuffer<std::pair<uint64_t,char*>> _qHdgClOrdIds;
	vector<SpreadHedgeV3> _sprd_hdgs;
	vector<KTNBuf*> _hdgbufs;

	//this is a crutch, sorry... but Spread Leg doesn't send back order request id...
	//this is needed to give us a callback id
	//TODO: Use a circular buffer - VERY EASY TO DO!! SO DO IT.
	std::unordered_map<uint64_t, uint64_t> _cme_to_callback;

	//map cert secid to symbol
	std::unordered_map<long, string> _certIdMap;
	std::unordered_map<string, long> _certIdSymbolMap;

	template<bool SPIN_CORE>
	void runProcessingThreadLoop()
	{
		_PROC_THREAD_ACTIVE.store(true, std::memory_order_relaxed);
		LogMe("OK: STARTING PROCESSING THREAD", LogLevel::OK);

		std::vector<uint8_t> payload;

		while(_PROC_THREAD_ACTIVE.load(std::memory_order_relaxed) == true)
		{
			if (!_qRecvBufs.empty())
			{
				KTNBuf8t ktnbuf;
				while (_qRecvBufs.try_pop(ktnbuf))
				{
					const uint64_t recvQueuePopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
						std::chrono::high_resolution_clock::now().time_since_epoch()).count();

					LOGINFO("[CmeGT5] Processing message recvTime={} recvQPopTime={} recv2QPop={} size={} segid={} index={}",
						ktnbuf.recvTime, recvQueuePopTime, recvQueuePopTime - ktnbuf.recvTime, ktnbuf.buffer_length, ktnbuf.segid, ktnbuf.index);

					std::vector<uint8_t> &tcpRemainder = _tcpRemainders[ktnbuf.index];
					int consumed;
					if (!tcpRemainder.empty())
					{
						payload.insert(payload.end(), tcpRemainder.begin(), tcpRemainder.end());
						payload.insert(payload.end(), ktnbuf.buffer, ktnbuf.buffer + ktnbuf.buffer_length);
						consumed = ProcessRecvBuffer(payload.data(), payload.size(), ktnbuf.segid, ktnbuf.index, ktnbuf.recvTime);
						if (consumed < static_cast<int>(payload.size()))
						{
							tcpRemainder.assign(payload.begin() + consumed, payload.end());
						}
						else
						{
							tcpRemainder.clear();
						}
					}
					else
					{
						//payload.assign(ktnbuf.buffer, ktnbuf.buffer + ktnbuf.buffer_length);
						consumed = ProcessRecvBuffer(ktnbuf.buffer, ktnbuf.buffer_length, ktnbuf.segid, ktnbuf.index, ktnbuf.recvTime);
						if (consumed < static_cast<int>(ktnbuf.buffer_length))
						{
							tcpRemainder.clear();
							tcpRemainder.insert(tcpRemainder.end(),ktnbuf.buffer + consumed, ktnbuf.buffer + ktnbuf.buffer_length);
							//tcpRemainder.assign(payload.begin() + consumed, payload.end());
						}
						else
						{
							tcpRemainder.clear();
						}
					}



					payload.clear();
					// No manual delete needed — destructor handles it
				}
			}


			// if(!_qHdgProc.empty()){
			// 	std::vector<std::unique_ptr<KTNBuf>> vbufs;
			// 	KTNBuf buf;
			// 	while (_qHdgProc.try_pop(buf)) {
			// 		//vbufs.push_back(std::make_unique<KTNBuf>(buf));

			// 		 auto bufPtr = std::make_unique<KTNBuf>(buf);
			// 		//_KtnBufs.put(bufPtr.get()); // Use the raw pointer for _KtnBufs.put
			// 		vbufs.push_back(std::move(bufPtr)); // Move unique_ptr into vector
			// 	}

			// 	ProcessHdgBuf(vbufs);
			// }

			if (_State.CHECK_EVENT_LOOP_COUNT >= _State.CHECK_EVENTS_TRIG){
				CheckEvents(_CX);
				_State.CHECK_EVENT_LOOP_COUNT = 0;
			}

			if constexpr (!SPIN_CORE)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}
		}
	}

};

} }

#endif /* SRC_EXCHANGEHANDLERS_CMEFIX_HPP_ */
