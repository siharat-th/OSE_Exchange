#include <KT01/Net/Tcp/StandardTcpV1.hpp>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include "StandardTcpV1.hpp"

using namespace KT01::NET::TCP;

StandardTcpV1::StandardTcpV1(TcpCallback8t* cb)
	: _callback(cb), _epoll_fd(-1), _fd(-1), _segid(-1), _loopcounter(0),
	  _connected(false), _running(false), _auto_reconnect(true),
	  _reconnect_wait_ms(15000), _saved_isA(false)  // 15 seconds (initial wait time)
{
	_should_reconnect.store(false, std::memory_order_relaxed);
}

StandardTcpV1::~StandardTcpV1() {
	Stop();
}

bool StandardTcpV1::Init(ServiceDescriptor& desc, bool isA) {
	(void)isA;

	// Save descriptor and isA for reconnect
	_saved_descriptor = desc;
	_saved_isA = isA;
	_should_reconnect.store(false, std::memory_order_relaxed);

	_fd = _socket.CreateSocket(desc);
	if (_fd < 0) {
		KT01_LOG_ERROR(__CLASS__, "StandardTcpV1: failed to connect");
		return false;
	}

	_epoll_fd = epoll_create1(0);
	if (_epoll_fd < 0) {
		KT01_LOG_ERROR(__CLASS__, "StandardTcpV1: failed to create epoll fd");
		close(_fd);
		_fd = -1;
		return false;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = _fd;

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _fd, &ev) < 0) {
		KT01_LOG_ERROR(__CLASS__, "StandardTcpV1: failed to add fd to epoll");
		close(_epoll_fd);
		close(_fd);
		_epoll_fd = -1;
		_fd = -1;
		return false;
	}

	_segid = desc.iSource;
	_connected = true;
	_running.store(true);

	LogMe("StandardTcpV1 OK: connection established. FD=" + std::to_string(_fd));
	return true;
}

void StandardTcpV1::Subscribe(TcpCallback8t* cb) {
	_callback = cb;
}

void StandardTcpV1::Stop() {
	// if (!_running.exchange(false)) return;
	_running.store(false);

	if (_epoll_fd >= 0) {
		close(_epoll_fd);
		_epoll_fd = -1;
	}

	if (_fd >= 0) {
		close(_fd);
		_fd = -1;
	}
}

void StandardTcpV1::Poll() {
	// Check if reconnect is needed (must be at top!)
	if (_should_reconnect.load(std::memory_order_relaxed) && !_running.load()) {
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
			now - _last_disconnect_time).count();

		if (elapsed >= _reconnect_wait_ms) {
			LogMe("Attempting auto-reconnect...");
			Reconnect();
		}
		return;  // Not ready to poll yet
	}

	if (!_running.load()) return;

	struct epoll_event events[4];
	int nfds = epoll_wait(_epoll_fd, events, 4, 0);

	for (int i = 0; i < nfds; ++i) {
		if (events[i].data.fd == _fd && (events[i].events & EPOLLIN)) {
			tcp_poll();
		}
	}
}

// void StandardTcpV1::tcp_poll() {
// 	char buf[8192];

// 	while (true) {
// 		ssize_t n = ::recv(_fd, buf, sizeof(buf), 0);
// 		if (n <= 0) break;

// 		if (_callback) {
// 			// Cast char* to uint8_t* and pass segid/index
// 			_callback->onRecv(reinterpret_cast<uint8_t*>(buf), n, _segid, 0);  // index = 0 unless you track it
// 		}
// 	}
// }

void StandardTcpV1::tcp_poll() {
	//this is for the edge case of large packets in spin:
	char buf[65535];  // 64KB

	while (true)
	{
		const ssize_t n = ::recv(_fd, buf, sizeof(buf), 0);

		if (n > 0)
		{
			if (_callback)
			{
				// Append new data to any pending data
				size_t original_size = pending_data.size();
				pending_data.resize(original_size + n);
				memcpy(pending_data.data() + original_size, buf, n);

				// Process the combined data
				const int consumed = _callback->onRecv(pending_data.data(), pending_data.size(), _segid, 0);

				// Keep any unprocessed data
				if (consumed == 0)
				{
					// Do nothing, leave data in pending_data
				}
				else if (consumed < static_cast<int>(pending_data.size()))
				{
					// Move unprocessed data to beginning of buffer
					memmove(pending_data.data(), pending_data.data() + consumed,
					pending_data.size() - consumed);
					pending_data.resize(pending_data.size() - consumed);
				}
				else
				{
					pending_data.clear();
				}
				// cout << "[StandardTcpV1 WARNING] recv() n=" << n << ", consumed=" << consumed
				//      << ", pending=" << pending_data.size() << endl;
			}
			else
			{
				KT01_LOG_ERROR(__CLASS__, "StandardTcpV1: callback not set");
				break;
			}
		}

		if (n == 0)
		{
			// Connection closed
			KT01_LOG_ERROR(__CLASS__, "StandardTcpV1: connection closed");

			// Record disconnect time
			_last_disconnect_time = std::chrono::steady_clock::now();
			_should_reconnect.store(_auto_reconnect, std::memory_order_relaxed);

			pending_data.clear();
			cleanup_socket();

			if (_auto_reconnect)
			{
				LogMe("Connection closed. Will attempt reconnect in " +
					  std::to_string(_reconnect_wait_ms/1000) + " seconds");
			}
			break;
		}

		if (n < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// No data available, continue polling
				break;
			}
			else
			{
				KT01_LOG_ERROR(__CLASS__, "StandardTcpV1: recv error");

				// Record disconnect time and trigger reconnect (same as connection closed case)
				_last_disconnect_time = std::chrono::steady_clock::now();
				_should_reconnect.store(_auto_reconnect, std::memory_order_relaxed);

				pending_data.clear();
				cleanup_socket();

				if (_auto_reconnect)
				{
					LogMe("Connection error. Will attempt reconnect in " +
						  std::to_string(_reconnect_wait_ms/1000) + " seconds");
				}
				break;
			}
		}
    }
}

void StandardTcpV1::Send(char* buffer, size_t len, bool delegated) {
	(void)delegated;
	ssize_t sent = ::send(_fd, buffer, len, 0);
	if (sent < 0) {
		KT01_LOG_ERROR(__CLASS__, "EFVITcpV1: send error");
	}
}

int StandardTcpV1::SendFastPath(char* buffer, size_t buffer_length) {
	return ::send(_fd, buffer, buffer_length, 0);
}

int StandardTcpV1::SendNormal(char* buffer, size_t buffer_length) {
	return ::send(_fd, buffer, buffer_length, 0);
}

int StandardTcpV1::Fd() const {
	return _fd;
}

int StandardTcpV1::SegId() const {
	return _segid;
}

void StandardTcpV1::LogMe(const std::string& msg) {
	KT01_LOG_INFO(__CLASS__, msg);
}

void StandardTcpV1::cleanup_socket() {
    if (_fd >= 0) {
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _fd, nullptr); // remove from epoll
        close(_fd);
        _fd = -1;
        cout << "[DEBUG] tcp_poll_2: socket closed and removed from epoll" << endl;
    }
	if (_epoll_fd >= 0) {
        close(_epoll_fd);
        _epoll_fd = -1;
    }
    _running.store(false);
}

void StandardTcpV1::SetReconnectConfig(bool auto_reconnect, int wait_ms) {
	_auto_reconnect = auto_reconnect;
	_reconnect_wait_ms = wait_ms;
	LogMe("Reconnect config: auto=" + std::to_string(auto_reconnect) +
		  ", wait=" + std::to_string(wait_ms) + "ms");
}

void StandardTcpV1::SetReconnectWaitTime(int wait_ms) {
	_reconnect_wait_ms = wait_ms;
	LogMe("Reconnect wait time updated to " + std::to_string(wait_ms/1000) + "s");
}

void StandardTcpV1::Reconnect() {
	LogMe("Reconnecting to " + _saved_descriptor.address + ":" +
		  std::to_string(_saved_descriptor.port));

	_should_reconnect.store(false, std::memory_order_relaxed);

	if (Init(_saved_descriptor, _saved_isA)) {
		LogMe("TCP reconnection successful. Waiting for login...");
	} else {
		KT01_LOG_ERROR(__CLASS__, "Reconnection failed. Will retry in " +
					   std::to_string(_reconnect_wait_ms/1000) + "s");
		_should_reconnect.store(true, std::memory_order_relaxed);
		_last_disconnect_time = std::chrono::steady_clock::now();
	}
}

bool StandardTcpV1::IsConnected() const {
	return _connected && _running.load();
}