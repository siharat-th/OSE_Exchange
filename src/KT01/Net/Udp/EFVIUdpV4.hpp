/*
 * EFVIFeed.h
 *
 *  Created on: Mar 1, 2017
 *  V4 updated 2/13/2024
 *      Author: sgaer
 */

#ifndef EFVIFEEDV4_H_
#define EFVIFEEDV4_H_

#pragma once

#include <unistd.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <cstdint>
#include <functional>
#include <sstream>

#include <netinet/in.h>
#include <etherfabric/vi.h>
#include <etherfabric/pd.h>
#include <etherfabric/pio.h>
#include <etherfabric/memreg.h>

// Note: this is the solfarflare std file:
#include <KT01/Net/efvi/efvi_utils.h>

#include <boost/lockfree/stack.hpp>

#include <KT01/Core/Macro.hpp>
#include <KT01/Core/PerformanceTracker.hpp>

#include <KT01/Net/feed_structs.hpp>

#include <KT01/Core/Time.hpp>
#include <KT01/Net/SFCPktBuf.hpp>
using namespace KT01::NET;
using namespace KT01::Core;

#include <KT01/Net/NetSettings.hpp>
using namespace KT01::NET;

#include <KT01/Net/Udp/UdpCallback.hpp>

using namespace std;

namespace KT01::NET::UDP
{
	#define EFVI_OFFSET 42
	#define PKT_BUF_SIZE 2048

	// EFVI DEFS:
	/* Hardware delivers at most ef_vi_receive_buffer_len() bytes to each
	 * buffer (default 1792), and for best performance buffers should be
	 * aligned on a 64-byte boundary.  Also, RX DMA will not cross a 4K
	 * boundary.  The I/O address space may be discontiguous at 4K boundaries.
	 * So easiest thing to do is to make buffers always be 2K in size.
	 */

	class EFVIUdpV4
	{
	public:
		typedef KT01::Core::Timestamp Timestamp;
		// using EfViCallback = void (*)(char*, int);

		enum class FilterIPProto : int
		{
			UDP = ::IPPROTO_UDP,
			TCP = ::IPPROTO_TCP
		};

		explicit EFVIUdpV4(std::string feed, bool hwtimestamping,
						   std::string name, bool useCertNrConfigFile = false);

		virtual ~EFVIUdpV4();

		bool apply_filter(const string &connid, const string &ipaddr, int port, bool isA);
		void poll();

		// void Run() override;

		void EfPrintStats();
		void RefillRing();

		void testticket();
		int Source();

	private:
		void printStatDetails(std::vector<int> &vec, std::string title);

		void init();

		//static void *EntryPoint(void *);

		// V3
		/* Callbacks: */
		/// Received packet callback.
		//  The data buffer will contain the complete packet, including all headers
		//  (Ethernet, IP, UDP/TCP, etc.).
		virtual void on_rx_cb(const char *data, const int len, const uint32_t address, const uint64_t recvTime) = 0; //, const Timestamp& swts, const Timestamp& hwts = {0,0}, bool hwts_clock_set = false, bool hwts_clock_in_sync = false) = 0;
														//	/// Received discarded packet callback.
														//	//  A received packet was discarded (and why).
														//	virtual void on_rx_discard_cb(const char * data, int len, unsigned discard_type, const Timestamp& swts, const Timestamp& hwts = {0,0}, bool hwts_clock_set = false, bool hwts_clock_in_sync = false) {}
														//	//  TODO FIXME: support sending packets!
														//	/// Sent packet callback.
														//	//  This is called after a packet has left the wire.
														//	virtual void on_tx_cb(int qid, const char * data, int len, const Timestamp& swts, const Timestamp& hwts = {0,0}, bool hwts_clock_set = false, bool hwts_clock_in_sync = false) {}
														//	/// Optional on_final_event_cb, called after all polled events have been
														//	//  processed.  This is useful to, for example, ensure that all available
														//	//  packets in the event queue have been processed before taking action.
														//	virtual void on_poll_complete_cb(int num_events) {}

		int64_t timespec_diff_ns(struct timespec a, struct timespec b);

	private:
		std::atomic<bool> _ACTIVE;
		PerformanceTracker _trk2;
		// UdpCallbackV3&	 rxCallback;
		bool _useCertNrConfigFile;

		NetSettings _sett;

		std::string _NAME;
		bool m_bStarted;
		bool m_bRunning;
		int unit_;

		std::atomic<uint64_t> MSG_COUNT;
		int MAX_MSG_COUNT;
		int OFFSET;

		std::string _servicename;
		std::string _intfname;

		string _feedAB;

		int source_;
		int port_;

		int rx_prefix_len;

		//*************i01 **************

		/* Capacities: */
		int evq_capacity() const { return m_evq_capacity; }
		int rxq_capacity() const { return m_rxq_capacity; }
		int txq_capacity() const { return m_txq_capacity; }
		/// Number of `SFCPktBuf`s allocated.
		int pbs_capacity() const { return m_rxq_capacity + m_txq_capacity; }

		ef_vi &vi() { return m_vi; }
		ef_memreg &mr() { return m_mr; }
		ef_pio &pio() { return m_pio; }
		ef_driver_handle &dh() { return m_dh; }
		ef_pd &pd() { return m_pd; }

		ef_driver_handle m_dh;
		struct ef_pd m_pd;
		struct ef_vi m_vi;
		struct ef_memreg m_mr;
		struct ef_pio m_pio;

		char m_sfc_mac[7];
		unsigned m_sfc_mtu;

		int m_evq_capacity;
		int m_rxq_capacity;
		int m_txq_capacity;

		int m_rx_prefix_len;
		int m_pktlen_offset;

		uint64_t m_n_ht_events;

		/// \internal Pointer to start of region containing `SFCPktBuf`s for this VI.
		SFCPktBuf *m_pktbufs_p;
		/// \internal Reclamation stack for freed/available `SFCPktBuf`s.
		boost::lockfree::stack<SFCPktBuf *> m_pktbufs_lifo;
		/// \internal Number of unused `SFCPktBuf`s available.
		std::atomic<int> m_pktbufs_lifo_size;
		/// \internal Next rxq maintenance will try to add at least this many
		//  `SFCPktBuf`s.from `m_pktbufs_lifo` to the receive queue.
		int m_rxq_cushion;
		/// \internal Is hardware timestamping enabled?
		bool m_hw_timestamping;
		/// \internal Returns the `SFCPktBuf` at index `id`.
		SFCPktBuf *get_pktbuf(int id);
		/// \internal Returns a reclaimed `SFCPktBuf`.
		SFCPktBuf *get_pktbuf();
		/// \internal Reclaims `SFCPktBuf` at `pb`.
		bool put_pktbuf(SFCPktBuf *pb_p);
		/// \internal Returns `SFCPktBuf`s to receive queue via
		//`ef_vi_receive_{init,push}` as needed if receive queue is low.
		void maintain_rxq();
		/// \internal Adds up to `n` SFCPktBufs to rxq, returns number of SFCPktBufs
		//  actually pushed onto rxq.
		int fill_rxq(int n);

	private:
		EFVIUdpV4() = delete;
		EFVIUdpV4(const EFVIUdpV4 &) = delete;
		EFVIUdpV4 &operator=(const EFVIUdpV4 &) = delete;
	};

} // namespace KT01::NET::UDP

#endif /* EFVIFEED_H_ */
