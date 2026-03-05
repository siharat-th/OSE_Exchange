//============================================================================
// Name        	: ExchangeBase2.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2021-24 Katana Financial
// Date			: April 27, 2023 4:49:28 PM
//============================================================================


#ifndef SRC_EXCHANGEHANDLERS_EXCHANGEBASE2_HPP_
#define SRC_EXCHANGEHANDLERS_EXCHANGEBASE2_HPP_

#include <ExchangeHandler/audit/AuditTrailWriter.hpp>
#include <KT01/Core/BufferPool.hpp>
#include <KT01/Core/FixedSizeMap.hpp>
#include <Maps/HashMap.hpp>
#include <Maps/SpreadHedgeCircBuff.hpp>
#pragma once

#include <ExchangeHandler/ExchangePch.hpp>
#include <ExchangeHandler/session/SessionStateManager.hpp>


namespace KTN{


struct ConnState
{
    /**
* @brief Represents the connection state of a connection.
*/
    int index; /**< The index of the connection. */
    int segid; /**< The segment ID of the connection. */
    //EFVITcpV5* tcpHandler; /**< The TCP handler of the connection. */
    KT01::NET::TCP::TcpHandlerInterface* tcpHandler = nullptr;
    KTN::Session::SessionStateManager sessmgr; /**< The session state manager of the connection. */
    bool is_reconnecting = false; /**< Flag to prevent state changes during reconnect */
    bool needs_reset_reconnect = false; /**< Flag indicating sequence mismatch requiring reset */
    int reset_retry_count = 0; /**< Number of reset attempts */
    int reset_backoff_ms = 30000; /**< Backoff time in milliseconds (exponential, start 30s) */

    // Reconnect tracking fields
    bool has_notified_disconnect = false; /**< Track if disconnect notification was sent */
    int reconnect_attempt = 0; /**< Track reconnect attempt count */
    int reconnect_backoff_ms = 15000; /**< Reconnect backoff time in ms (exponential, start 15s) */

    static bool OkToSend(EnumSessionState::Enum state)
    {
    	if (LIKELY(state == EnumSessionState::Established )|| UNLIKELY(state ==EnumSessionState::WaitingForRetransmission))
    		return true;
    	return false;
    }

	using OrderSeqRecord = std::pair<KTN::OrderPod, uint32_t>;
	static constexpr int MAX_ORDER_RECORDS = 1024;
	static_assert (MAX_ORDER_RECORDS > 0 && (MAX_ORDER_RECORDS & (MAX_ORDER_RECORDS - 1)) == 0,
		"MAX_ORDER_RECORDS must be greater than 0 and a power of two");
	static constexpr int SEQ_MASK = MAX_ORDER_RECORDS - 1;
	OrderSeqRecord *outgoingOrderRecords = new OrderSeqRecord[MAX_ORDER_RECORDS];

	void AddOrderRecord(const KTN::OrderPod &order, const uint32_t seq)
	{
		const int idx = seq & SEQ_MASK;
		outgoingOrderRecords[idx] = std::make_pair(order, seq);
	}

	const OrderSeqRecord GetOrderRecord(const uint32_t seq) const
	{
		const int idx = seq & SEQ_MASK;
		return outgoingOrderRecords[idx];
	}
};

/**
* @class ExchangeBase2
* @brief Represents the base class for an exchange handler.
*/
class ExchangeBase2
{
public:
    /**
     * @brief Constructor for ExchangeBase2.
     * @param settingsfile The path to the settings file.
     * @param source The source of the exchange.
     * @param exchname The name of the exchange.
     */
    ExchangeBase2(const std::string& settingsfile, const std::string& source, tbb::concurrent_queue<KTN::OrderPod>& qords, const std::string& exchname);

    /**
     * @brief Destructor for ExchangeBase2.
     */
    virtual ~ExchangeBase2();

    /**
     * @brief Gets the name of the exchange.
     * @return The name of the exchange.
     */
    inline const std::string& Name() const
    	{
    		return _State.EXCHNAME;
    	}

    /**
     * @brief Stops the exchange.
     */
    virtual void Stop() = 0;

    /**
     * @brief Sends an order.
     * @param order The order to send.
     * @param action The action to perform.
     */
    virtual void Send(KTN::OrderPod& order, int action) = 0;

    /**
     * @brief Sends a mass action.
     * @param order The order to send.
     */
    virtual void SendMassAction(KTN::Order& order) = 0;

    /**
     * @brief Sends a hedge instruction.
     * @param ordereqid The order request ID.
     * @param hdg The vector of spread hedge instructions.
     */
    virtual void SendHedgeInstruction(uint64_t ordereqid, const std::vector<SpreadHedgeV3>& hdg);

    /**
     * @brief Sends an algorithmic command in JSON format.
     * @param json The JSON string.
     * @param cmd The command type.
     */
    virtual void AlgoJson(std::string json, ExchCmd::Enum cmd);

    /**
     * @brief Clears the maps.
     */
    virtual void ClearMaps();

    /**
     * @brief Prints the latencies.
     * @param clear Indicates whether to clear the latencies after printing.
     */
    virtual void PrintLatencies(bool clear);

    /**
     * @brief Prints the message ratios.
     */
    virtual void PrintMsgRatios();

    /**
     * @brief Resets the exchange.
     */
    virtual void Reset();

    /**
     * @brief Executes a command.
     * @param command The command to execute.
     */
    virtual void Command(Instruction command);

    /**
     * @brief Polls the exchange for data
     */
    virtual void Poll()
    	{
    		LogMe("POLL NOT IMPLEMENTED!");
    	};

    /**
     * @brief Gets the settings file path.
     * @return The settings file path.
     */
    std::string SettFile()
    	{
    		return _State.FILENAME;
    	}

protected:
    /**
     * @brief Logs a message.
     * @param szMsg The message to log.
     * @param iColor The color of the log message.
     */
    void LogMe(const std::string& szMsg, LogLevel::Enum level =LogLevel::INFO);

    /**
     * @brief Changes the name of the exchange.
     * @param owner The owner of the exchange.
     * @param source The source of the exchange.
     */
    void ChangeName(const std::string& owner, const std::string& source);

    /**
     * @brief Reports the status of the session.
     * @param s The session status.
     */
    void ReportStatus(SessionStatus s);

    /**
     * @brief Logs a message indicating that a message was sent.
     * @param symb The symbol of the message.
     * @param cnt The count of the message.
     * @param volume The volume of the message.
     * @param event The event of the message.
     */
    void MsgSent(const std::string& symb, int cnt, int volume, int event);

    /**
     * @brief Stops the base exchange.
     */
    void StopBase();


    inline int8_t GetOrderCallback(char* clordid)
    {
        if (_clordid_to_callback.find(clordid) == 255)
            return 0;
        return _clordid_to_callback.find(clordid);
    }

    inline void SetOrderCallback(char* clordid, int8_t cbid)
    {
        _clordid_to_callback.insert(clordid, cbid);
    }

    inline uint64_t GetOrderReqIdFromExchId(uint64_t exchid)
    {
        auto it = _exchid_to_ordreqid.find(exchid);
        if (it != _exchid_to_ordreqid.end())
            return it->second;
        return 0;
    }

    inline void SetOrderReqIdForExchExchId(uint64_t exchid, uint64_t ordreqid)
    {
        _exchid_to_ordreqid[exchid] = ordreqid;
    }

    /**
     * @brief Fast copy function for copying memory.
     * @param dest The destination memory address.
     * @param src The source memory address.
     * @param length The length of the memory to copy.
     */
    inline void fastCopy2(char* dest, const char* src, size_t length)
    {
		size_t i = 0;
		for (; i + 4 <= length; i += 4) {
			dest[i] = src[i];
			dest[i + 1] = src[i + 1];
			dest[i + 2] = src[i + 2];
			dest[i + 3] = src[i + 3];
		}
		for (; i < length; ++i) {
			dest[i] = src[i];
		}
	}



    /**
     * @brief Checks if a heartbeat is needed.
     * @return True if a heartbeat is needed, false otherwise.
     */
    inline bool needhb()
	{
		bool checkhb = false;
		if (_State.LOOPER > _State.LOOP_TRIG)
		{
			checkhb = true;
			_State.LOOPER = 0;
		}

		return checkhb;
	}

protected:
    ExchState _State;
    LogSvc* _logsvc;

    std::unique_ptr<AuditTrailWriter> _audit;

	//convenience thread takes work off critical path
	std::thread _procthread;
	std::atomic<bool> _PROC_THREAD_ACTIVE;

	tbb::concurrent_queue<KTN::OrderPod>& _qOrdsProc;
	tbb::concurrent_queue<KTNBuf> _qHdgProc;
	tbb::concurrent_queue<KTNBuf8t> _qRetrans;
   
    std::unordered_map<std::string, msgrate> _msgratio;
    std::unordered_map<std::string, msgevents> _msgevents;
    HashMap<uint64_t, std::vector<KTN::OrderPod>, 1000> _hedgeOrds;
    SpreadHedgeCircBuff _hedgeMap;
    PerformanceTracker _trk;

    SPSCRingBuffer<Instruction> _qCmds;
	SPSCRingBuffer<Instruction> _qAlgoCmds;
	SPSCRingBuffer<KTNBuf8t> _qRecvBufs;

	OrderPodPool _OrdPodPool;
	BufferPool _KtnBufs;

    SPSCRingBuffer<std::pair<uint64_t,char*>> _qHdgClOrdIDs;
	vector<SpreadHedgeV3> _sprd_hdgs;
	vector<KTNBuf*> _hdgbufs;

    KTN::Core::FixedSizeMap<1024> _clordid_to_callback;

    struct order_info
    {
        uint64_t orderReqId;
        uint64_t exchId;
        uint64_t secId;
    };

    using OrderMapType = tbb::concurrent_unordered_map<std::string, order_info>;
    OrderMapType clordIdToReqIdMap;
    std::unordered_map<uint64_t, uint64_t> _exchid_to_ordreqid;
};



//NOTE: I kept this here for reference. I will remove it later. This is the original code before I documented it above
/*class ExchangeBase2
{
public:
	ExchangeBase2( const std::string& settingsfile, const std::string& source, const std::string& exchname);
	virtual ~ExchangeBase2();

	inline const std::string& Name() const
	{
		return _State.EXCHNAME;
	}

	virtual void Stop() = 0;

	virtual void Send(KTN::OrderPod&, int action)=0;
	virtual void SendMassAction(KTN::Order&) = 0;
	virtual void SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3> & hdg);
	virtual void AlgoJson(string json, ExchCmd::Enum cmd);

	virtual void ClearMaps();

	virtual void PrintLatencies(bool clear);
	virtual void PrintMsgRatios();

	virtual void Reset();
	virtual void Command(Instruction command);

	virtual void Poll()
	{
		LogMe("POLL NOT IMPLEMENTED!");
	};


	string SettFile()
	{
		return _State.FILENAME;
	}


protected:
    void LogMe(const std::string& szMsg, int iColor=LOG_WHITE);
    void ChangeName(const string& owner, const string& source);
//	void HeartbeatReporter();
//	void PersistNotification(string msgtype, MsgLevel level, string msg);
	void ReportStatus(SessionStatus s);

	void MsgSent(string symb, int cnt, int volume, int event);
	void StopBase();

	inline void fastCopy2(char* dest, const char* src, size_t length) {
		size_t i = 0;
		for (; i + 4 <= length; i += 4) {
			dest[i] = src[i];
			dest[i + 1] = src[i + 1];
			dest[i + 2] = src[i + 2];
			dest[i + 3] = src[i + 3];
		}
		for (; i < length; ++i) {
			dest[i] = src[i];
		}
	}

	inline bool needhb()
	{
		bool checkhb = false;
		if (_State.LOOPER > _State.LOOP_TRIG)
		{
			checkhb = true;
			_State.LOOPER = 0;
		}

		return checkhb;
	}

protected:
	ExchState _State;
	LogSvc * _logsvc;

	std::unordered_map<std::string, msgrate> _msgratio;
	std::unordered_map<std::string, msgevents> _msgevents;

	//map hedge orders to sent orders for instant hedging. almost instant.
	//std::unordered_map<uint64_t, KTN::Order> _hedgeOrds;
	HashMap<uint64_t, vector<KTN::OrderPod>, 1000> _hedgeOrds;
	//HashMap<uint64_t, vector<SpreadHedgeV3>, 1000> _hedgeMap;

	SpreadHedgeCircBuff _hedgeMap;

	PerformanceTracker _trk;

};*/

}

#endif /* SRC_EXCHANGEHANDLERS_EXCHANGEBASE_HPP_ */
