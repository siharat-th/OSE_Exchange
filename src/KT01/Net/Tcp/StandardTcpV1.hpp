#ifndef SRC_KTNTCP_STDTCPV1_HPP_
#define SRC_KTNTCP_STDTCPV1_HPP_

#pragma once

#include <KT01/Net/Tcp/TcpHandlerInterface.hpp>
#include <KT01/Net/Tcp/TcpSocket.hpp>

#include <atomic>
#include <string>

namespace KT01
{
    namespace NET
    {
        namespace TCP
        {

            /**
             * @brief A simple TCP handler using standard sockets and raw epoll.
             */
            class StandardTcpV1 : public TcpHandlerInterface
            {
            public:
                explicit StandardTcpV1(TcpCallback8t *cb);
                virtual ~StandardTcpV1();

                bool Init(ServiceDescriptor &desc, bool isA) override;
                void Subscribe(TcpCallback8t *cb) override;
                void Stop() override;
                void Poll() override;
                void Send(char *buffer, size_t len, bool delegated) override;
                int SendFastPath(char *buffer, size_t buffer_length) override;
                int SendNormal(char *buffer, size_t buffer_length) override;
                int Fd() const override;
                int SegId() const override;

                // Reconnect configuration
                void SetReconnectConfig(bool auto_reconnect, int wait_ms) override;
                void SetReconnectWaitTime(int wait_ms) override;
                void Reconnect() override;
                bool IsConnected() const override;

            private:
                void LogMe(const std::string &msg);
                void cleanup_socket();
                void tcp_poll();

            private:
                TcpCallback8t *_callback;

                int _epoll_fd;
                int _fd;
                int _segid;
                uint64_t _loopcounter;
                bool _connected;

                std::atomic<bool> _running;

                TcpSocket _socket;
                std::vector<uint8_t> pending_data;
                NetSettings _settings;

                // Reconnect members
                bool _auto_reconnect;
                int _reconnect_wait_ms;
                std::chrono::steady_clock::time_point _last_disconnect_time;
                ServiceDescriptor _saved_descriptor;
                bool _saved_isA;
                std::atomic<bool> _should_reconnect;
            };

        } // namespace TCP
    } // namespace NET
} // namespace KTN

#endif /* SRC_KTNTCP_EFVITCPV1_HPP_ */
