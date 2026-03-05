//============================================================================
// Name        	: CfeGt4 Boe3 Order Handler
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2024 Katana Financial
// Date			: Feb 9, 2024 6:24:37 PM
//============================================================================

#ifndef SRC_EXCHANGEHANDLERS_CFEGT4_HPP_
#define SRC_EXCHANGEHANDLERS_CFEGT4_HPP_

#pragma once

#include <ExchangeHandler/ExchangeBase2.hpp>
using namespace KTN;

// #include <exchsupport/cme/audit/AuditTrailWriterV3.hpp>
#include <exchsupport/cfe/Boe3/Boe3Includes.hpp>
#include <exchsupport/cfe/Boe3/Boe3MessagePrinter.hpp>
#include <KT01/Core/FixedSizeMap.hpp>
using namespace KTN::Core;

#include <exchsupport/cfe/audit/CfeAuditProcessor.hpp>

#include <exchsupport/cme/il3/IL3Enums.hpp>

namespace KTN
{
  namespace CFE
  {

    /**
     * @brief CfeGT4 class represents the 5th Generation CmeHandler.
     * Low latency order template creation; memory pool usage and custom maps and memory management.
     * It provides better, faster, and stronger functionality for handling CME exchange iLink3 Messages.
     */
    class CfeGT4 : public ExchangeBase2, public TcpCallback8t
    {
    private:
      using byte_ptr = uint8_t *;

      ConnState _CX;

    public:
      /**
       * @brief Constructor for CfeGT4 class.
       * @param qords The concurrent queue for order pods.
       * @param settingsfile The path to the settings file.
       * @param source The source of the CME exchange.
       */
      explicit CfeGT4(tbb::concurrent_queue<KTN::OrderPod> &qords,
                      const std::string &settingsfile, const std::string &source);

      /**
       * @brief Destructor for CfeGT4 class.
       */
      virtual ~CfeGT4();

      /**
       * @brief Start the audit trail writer.
       * @param dir The directory path for storing the audit trail files.
       * @param sessid The session ID.
       * @param firmid The firm ID.
       * @param core The core number.
       */
      void StartAuditTrail(std::string dir, std::string sessid, std::string firmid, int core);

      /**
       * @brief Start the CfeGT4 handler.
       */
      void Start();

      /**
       * @brief Stop the CfeGT4 handler.
       */
      void Stop();

      /**
       * @brief Clear the maps used by the CfeGT4 handler.
       */
      void ClearMaps();

      /**
       * @brief Reset the CfeGT4 handler.
       */
      void Reset();

      /**
       * @brief Execute a command on the CfeGT4 handler.
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

      /**
       * @brief Send an order.
       * @param ord The order to send.
       * @param action The action to perform.
       */
      void Send(KTN::OrderPod &ord, int action);

      /**
       * @brief Send a mass action.
       * @param ord The order to perform the mass action on.
       */
      void SendMassAction(KTN::Order &ord);

      /**
       * @brief Send a hedge instruction.
       * @param ordereqid The order request ID.
       * @param hdg The vector of spread hedge instructions.
       */
      void SendHedgeInstruction(uint64_t ordereqid, const vector<SpreadHedgeV3> &hdg);

      /**
       * @brief Send a warm message.
       * @param index The index.
       */
      void SendWarm(int index);

      /**
       * @brief Print the session map.
       * @param conns The array of connection states.
       */
      void PrintSessionMap(ConnState &conn);

    private:
      /**
       * @brief Sets the last sent time for a connection state.
       *
       * @param conn The connection state.
       */
      void setLastSentTime(ConnState &conn);

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
      void ProcessHdgBuf(std::vector<std::unique_ptr<KTNBuf>> &vbufs);

      /**
       * @brief Processes a session message for a given connection state and message.
       *
       * @param conn The connection state.
       * @param msg The session message.
       */
      void ProcessSessionMessage(ConnState &conn, KTNBuf8t &msg);

      /**
       * @brief Handles the state change event for a given connection state.
       *
       * @param newState The new state.
       * @param conn The connection state.
       */
      void onStateChange(EnumSessionState::Enum newState, ConnState &conn);

      // CFE Session Messages
      /**
       * @brief A Login Response message is sent in response to a Login Request message.
       * @param conn The connection state.
       * @param buf The buffer containing the message.
       * @param iMsgLen The length of the message.
       * @param segid The segment ID. Always 1 for CFE.
       */
      void onLoginReponseMsg(ConnState &conn, byte_ptr buf, int iMsgLen, int segid);

      void onLogoutResponseMsg(ConnState &conn, byte_ptr buf, int iMsgLen, int segid);

      void onReplayCompleteMsg(ConnState &conn, byte_ptr buf, int iMsgLen, int segid);

      void onHeartbeatMsg(ConnState &conn, byte_ptr buf, int iMsgLen, int segid);

      // application msgs:
      /**
       * @brief Processes an application message for a given connection state and message.
       *
       * @param conn The connection state.
       * @param msg The application message.
       */
      int ProcessApplicationMessage(ConnState &conn, KTNBuf8t &msg, bool ignore_seqs = false);

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
       * @return The number of bytes processed.
       */
      void onExecRptTrade(byte_ptr buf, int iMsgLen, int segid, uint64_t recvTime);

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
       * @brief Handles the order mass cxl report message for a given connection state, buffer, message length, and segment ID.
       *
       * @param buf The buffer containing the message.
       * @param iMsgLen The length of the message.
       * @param segid The segment ID.
       */
      void onMassCancelAck(byte_ptr buf, int iMsgLen, int segid);

      /**
       * @brief Handles the order mass cxl reject report message for a given connection state, buffer, message length, and segment ID.
       *
       * @param buf The buffer containing the message.
       * @param iMsgLen The length of the message.
       * @param segid The segment ID.
       */
      void onMassCancelReject(byte_ptr buf, int iMsgLen, int segid);

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
       * @brief Handles the trade cancel correction message for a given connection state, buffer, message length, and segment ID.
       *
       * @param buf The buffer containing the message.
       * @param iMsgLen The length of the message.
       * @param segid The segment ID.
       */
      void onTradeCancelCorrect(byte_ptr buf, int iMsgLen, int segid);



      /**
       * @brief Handles the risk reset message for a given connection state, buffer, message length, and segment ID.
       *
       * @param buf The buffer containing the message.
       * @param iMsgLen The length of the message.
       * @param segid The segment ID.
       */
      void onRiskResetAck(byte_ptr buf, int iMsgLen, int segid);

      /**
       * @brief Handles the TAS restatement message for a given connection state, buffer, message length, and segment ID.
       *
       * @param buf The buffer containing the message.
       * @param iMsgLen The length of the message.
       * @param segid The segment ID.
       */
      void onTASRestatement(byte_ptr buf, int iMsgLen, int segid);

      /**
       * @brief Utility Function to Check SeqNum for connection to detect gaps
       */
      bool CheckIncomingSeq(ConnState &conn, KTNBuf8t &msg);

      /**
       * @brief Utility Function for checking Fault Tolerance Indicator vs. State.USE_PRIMARY
       *
       * @param fti The EnumFTI indicating passed from the session message
       * @param seg the segment id
       * @param fn The message wanting to check this
       */
      inline bool CheckFaultTolerance(KTN::CME::IL3::EnumFTI fti, int segid, string msg_name)
      {
        //bool res = false;
        // standard TFFT matrix here; 2 vars with 2 possible values
        // True/True
        if (LIKELY(fti == KTN::CME::IL3::EnumFTI::EnumFTI_Primary && _State.USE_PRIMARY))
          return true;

        // False/False
        if (UNLIKELY(fti == KTN::CME::IL3::EnumFTI::EnumFTI_Backup && !_State.USE_PRIMARY))
          return true;

        // False/True
        if (UNLIKELY(fti == KTN::CME::IL3::EnumFTI::EnumFTI_Backup && _State.USE_PRIMARY))
        {
          ostringstream oss;
          oss << "**SEG " << segid << " " << msg_name << ": FTI set to FTI_BACKUP but State.USE_PRIMARY set to TRUE. Failover is needed!";
          LogMe(oss.str());
          return false;
        }

        // True/False
        if (UNLIKELY(fti == KTN::CME::IL3::EnumFTI::EnumFTI_Primary && !_State.USE_PRIMARY))
        {
          ostringstream oss;
          oss << "**SEG " << segid << " " << msg_name << ": FTI set to FTI_PRIMARY but State.USE_PRIMARY set to FALSE. Failover is needed!";
          LogMe(oss.str());
          return false;
        }

        // default
        return true;
      }

      //Per CFE spec:
      /*
       * Changes in OrderQty result in an adjustment of the current order's OrderQty. The new OrderQty does
        not directly replace the current order's LeavesQty. Rather, a delta is computed from the current
        OrderQty and the replacement OrderQty. This delta is then applied to the current LeavesQty. If the
        resulting LeavesQty is less than or equal to zero, the order is cancelled.

        Right now, system-wide, our "replacement quentity" is defined as the LeavesQty.

        The CFE "math" for the "delta" shortcut is:

          orderqty = (desired leavesqty - origleavesqty) + origqty
          
          where:
            desired leavesqty = the new value of leavesqty
            origleavesqty = the current order's leaves quantity
            origqty = the current order's order quantity
        */

    //   inline uint32_t CfeOrderModDelta(KTN::OrderPod &ord)
    //   {
    //     int32_t qty = ord.quantity;

    //     int32_t desired_lvs = ord.quantity;
    //     int32_t orig_lvs = ord.origlvs;
    //     int32_t orig_qty = ord.origqty;

    //     int32_t delta = desired_lvs - orig_lvs;
      
    //     qty = orig_qty + delta;

    //     return qty;
    //   }

    private:
      void SessionInit(ConnState &conn, int index);
      void SessionLogin(ConnState &conn);
      void Dispatch(ConnState &conn, bool checkhb);

      bool checkSessionIsConnected(ConnState &conn, int segid);
      bool CheckForHeartbeat(ConnState &conn);
      void CheckEvents(ConnState &conn);

      // Session and Tcp
      void StartTcpConnection(ConnState &conn, bool primary = true);
      void StartSessionManager(ConnState &conn);
      // void EstablishSession(ConnState& conn) {};
      void SendHeartbeatMsg(ConnState &conn);
      void SendLogoutMsg(ConnState &conn);

      void SendMassCxl(ConnState &conn, KOrderAction::Enum action, string root = "", string inst = "");
      void SendRiskReset(ConnState &conn);

      void Display();
      void Banner(string msg);

      inline void genAudit(KTNBuf *&x, byte_ptr buf, size_t iMsgLen, uint8_t segid, int msgtype)
      {

        __builtin_memcpy(x->buffer, buf, iMsgLen);
        x->buffer_length = iMsgLen;
        x->segid = segid;
        x->msgtype = msgtype;
        x->dir = 1;
      }

    private:
      CfeBoe3Settings _sett;

      CFE::Boe3::SessionMessageFactory _sessfact;
      CFE::Boe3::AppMessageFactoryFastV2 _fastfact;

      // ObjectPool<char*> _HdgBufs;
    };

  }
}

#endif /* SRC_EXCHANGEHANDLERS_CMEFIX_HPP_ */
