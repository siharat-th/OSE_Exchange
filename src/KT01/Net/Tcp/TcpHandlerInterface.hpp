// TcpHandlerInterface.hpp
#ifndef SRC_KT01_NET_TCP_TCPHANDLERINTERFACE_HPP_
#define SRC_KT01_NET_TCP_TCPHANDLERINTERFACE_HPP_


#define FEED_OE 3
#define EFVI_OFFSET 42

#define CI_PAGE_SHIFT    16
#define CI_PAGE_SIZE     (1 << CI_PAGE_SHIFT)
#define CI_PAGE_MASK     (~((ci_uintptr_t) CI_PAGE_SIZE - 1))

#include <bits/stdc++.h>
using namespace std;

#include <KT01/Core/Macro.hpp>
#include <KT01/Core/PerformanceTracker.hpp>

#include <KT01/Net/ServiceDescriptor.hpp>
#include <KT01/Net/Tcp/TcpCallback.hpp>

#include <KT01/Net/NetSettings.hpp>
#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>

namespace KT01::NET::TCP {

/**
 * @brief The TcpHandlerInterface class is a shared abstract interface for TCP handlers.
 * It defines the methods that must be implemented by any TCP handler class.
 */

class TcpHandlerInterface {
public:
	virtual ~TcpHandlerInterface() = default;

	virtual bool Init(ServiceDescriptor& desc, bool isA) = 0;
	virtual void Subscribe(TcpCallback8t* cb) = 0;
	virtual void Stop() = 0;
	virtual void Poll() = 0;
	virtual void Send(char* buffer, size_t len, bool delegated) = 0;
	virtual int SendFastPath(char *buffer, size_t buffer_length) = 0;
	virtual int SendNormal(char *buffer, size_t buffer_length) = 0;
	virtual int Fd() const = 0;
	virtual int SegId() const = 0;

	// Reconnect methods
	virtual void SetReconnectConfig(bool auto_reconnect, int wait_ms) = 0;
	virtual void SetReconnectWaitTime(int wait_ms) = 0;
	virtual void Reconnect() = 0;
	virtual bool IsConnected() const = 0;
};

} // namespace KT01::NET::TCP

#endif
