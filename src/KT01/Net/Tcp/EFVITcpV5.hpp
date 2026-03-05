#ifndef SRC_KTNTCP_EFVITCPV5_HPP_
#define SRC_KTNTCP_EFVITCPV5_HPP_

#pragma once

#include <KT01/Net/Tcp/TcpHandlerInterface.hpp>

#include <KT01/Net/Tcp/EFVIConf.hpp>
#include <external/pollnet/efvitcp/TcpClient.h>
using namespace efvitcp;

namespace KT01
{
  namespace NET
  {
    namespace TCP
    {

      /**
       * @class EFVITcpV5
       * @brief Represents the EFVITcpV5 class. This is our main TCP class. This class is a true EFVI TCP implementation.
       */
      class EFVITcpV5 : public TcpHandlerInterface
      {
      public:
        /**
         * @brief Constructs an EFVITcpV5 object with the specified callback.
         * @param callback The callback object.
         */
        EFVITcpV5(TcpCallback8t *callback);

        /**
         * @brief Destroys the EFVITcpV5 object.
         */
        virtual ~EFVITcpV5();

        /**
         * @brief Initializes the EFVITcpV5 object with the specified service descriptor and flag.
         * @param desc The service descriptor.
         * @param isA The flag indicating whether it is A.
         * @return True if initialization is successful, false otherwise.
         */
        bool Init(ServiceDescriptor &desc, bool isA) override;

        /**
         * @brief Subscribes to the EFVITcpV5 object with the specified callback.
         * @param callback The callback object.
         */
        void Subscribe(TcpCallback8t *callback) override;

        /**
         * @brief Stops the EFVITcpV5 object.
         */
        void Stop() override;

        /**
         * @brief Polls the EFVITcpV5 object.
         */
        void Poll() override;

        /**
         * @brief Sends the master data with the specified buffer and buffer length.
         * @param buffer The buffer containing the data to send.
         * @param buffer_length The length of the buffer.
         * @param send_delegated The flag indicating whether to send delegated.
         */
        void Send(char *buffer, size_t buffer_length, bool send_delegated) override;

        /**
         * @brief Sends the data using the fast path with the specified buffer and buffer length.
         * @param buffer The buffer containing the data to send.
         * @param buffer_length The length of the buffer.
         * @return The number of bytes sent.
         */
        int SendFastPath(char *buffer, size_t buffer_length) override;

        /**
         * @brief Sends the data using the normal path with the specified buffer and buffer length.
         * @param buffer The buffer containing the data to send.
         * @param buffer_length The length of the buffer.
         * @return The number of bytes sent.
         */
        int SendNormal(char *buffer, size_t buffer_length) override;

        /**
         * @brief Sends the warm data.
         */
        void SendWarm();

        /**
         * @brief Gets the file descriptor.
         * @return The file descriptor.
         */
        inline int Fd() const override
        {
          return _fd;
        }

        /**
         * @brief Gets the segment ID.
         * @return The segment ID.
         */
        inline int SegId() const override
        {
          return _segid;
        }

        // Reconnect methods (stub implementation)
        void SetReconnectConfig(bool auto_reconnect, int wait_ms) override {}
        void SetReconnectWaitTime(int wait_ms) override {}
        void Reconnect() override {}
        bool IsConnected() const override { return true; }

        using TcpClient = efvitcp::TcpClient<EfviConfig>;
        using TcpConn = TcpClient::Conn;

        /**
         * @brief Polls the EFVITcpV5 object.
         */
        void poll();

        /**
         * @brief Sends the bye message.
         * @return True if sending the bye message is successful, false otherwise.
         */
        bool bye();

        /**
         * @brief Handles the connection refused event.
         */
        void onConnectionRefused();

        /**
         * @brief Handles the connection established event.
         * @param conn The TCP connection.
         */
        void onConnectionEstablished(TcpConn &conn);

        /**
         * @brief Handles the data event.
         * @param conn The TCP connection.
         * @param data The data received.
         * @param size The size of the data.
         * @return The number of bytes processed.
         */
        uint32_t onData(TcpConn &conn, uint8_t *data, uint32_t size);

        /**
         * @brief Handles the connection reset event.
         * @param conn The TCP connection.
         */
        void onConnectionReset(TcpConn &conn);

        /**
         * @brief Handles the connection closed event.
         * @param conn The TCP connection.
         */
        void onConnectionClosed(TcpConn &conn);

        /**
         * @brief Handles the FIN event.
         * @param conn The TCP connection.
         * @param data The data received.
         * @param size The size of the data.
         */
        void onFin(TcpConn &conn, uint8_t *data, uint32_t size);

        /**
         * @brief Handles the connection timeout event.
         * @param conn The TCP connection.
         */
        void onConnectionTimeout(TcpConn &conn);

        /**
         * @brief Handles the more sendable event.
         * @param conn The TCP connection.
         */
        void onMoreSendable(TcpConn &conn);

        /**
         * @brief Handles the user timeout event.
         * @param conn The TCP connection.
         * @param timer_id The timer ID.
         */
        void onUserTimeout(TcpConn &conn, uint32_t timer_id);


        bool isConnectionEstablished() const
        {
            return conn_ != nullptr && conn_->isEstablished();
        }

      private:
        /**
         * @brief Logs the message with the specified color.
         * @param szMsg The message to log.
         * @param iColor The color of the log message.
         */
        void LogMe(std::string szMsg);

      private:
        //*** POLLNET ****
        TcpClient client;
        TcpConn *conn_ = nullptr;
        int64_t last_recv_val = 0;
        int64_t last_send_val = 0;

        int _SEGID;
        int _INDEX;

        TcpCallback8t *_callback;
        PerformanceTracker _trk;

        bool _efvi_enabled;
        bool _delsent;

        int _fd;
        int _segid;
        int _index;
        uint64_t _loopcounter;

        NetSettings _sett;

        int OFFSET;

      private:
        EFVITcpV5(const EFVITcpV5 &) = delete;
        EFVITcpV5 &operator=(const EFVITcpV5 &) = delete;
      };

    } // namespace TCP
  } // namespace NET
} // namespace KTN

#endif /* SRC_KTNTCP_EFVITCPV5_HPP_ */
