/*
 * EFVIFeedV3.h
 *
 *  Created on: Mar 1, 2017
 *      Author: sgaer
 */

#ifndef EFVIFEEDV3_H_
#define EFVIFEEDV3_H_

#include <unistd.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <atomic>

#include <boost/lockfree/stack.hpp>

// networking/ip addr stuff:
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include <netinet/in.h>
#include <etherfabric/vi.h>
#include <etherfabric/pd.h>
#include <etherfabric/pio.h>
#include <etherfabric/memreg.h>

// Note: this is the solfarflare std file:
#include <KT01/Net/efvi/utils_efvi.h>
#include <poll.h>
#include <linux/if_ether.h>

#include <KT01/Net/feed_defines.hpp>
#include <KT01/Net/feed_structs.hpp>
#include <Settings/CmeMdpSettings.h>
using namespace KTN::EFVI;

#include <KT01/Net/FeedHandler.hpp>

#include <exchsupport/cme/mktdata/ChannelStructsV2.hpp>

#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>
#include <KT01/Core/Macro.hpp>
#include <KT01/Core/NamedThread.hpp>
using namespace KT01::Core;
using namespace KTN::Feeds::CME;
using namespace std;

#include <KT01/Net/Udp/UdpCallback.hpp>
// using namespace KTN::EFVI;

#include <KT01/Core/Time.hpp>
#include <KT01/Net/SFCPktBuf.hpp>
//using namespace i01::net;
//using namespace i01::core;

using namespace KT01::NET;
using namespace KT01::Core;

#define POLL_BATCH_SIZE 2

/* Hardware delivers at most ef_vi_receive_buffer_len() bytes to each
 * buffer (default 1792), and for best performance buffers should be
 * aligned on a 64-byte boundary.  Also, RX DMA will not cross a 4K
 * boundary.  The I/O address space may be discontiguous at 4K boundaries.
 * So easiest thing to do is to make buffers always be 2K in size.
 */
#define PKT_BUF_SIZE 2048

/* Align address where data is delivered onto EF_VI_DMA_ALIGN boundary,
 * because that gives best performance.
 */
#define RX_DMA_OFF ROUND_UP(sizeof(struct ef_pkt_buf), EF_VI_DMA_ALIGN)

namespace i01
{
	namespace net
	{

		class EFVIFeedV3 : public FeedHandler
		{
		public:
			/*typedef i01::core::Timestamp Timestamp;*/
			typedef KT01::Core::Timestamp Timestamp;

			enum class FilterIPProto : int
			{
				UDP = ::IPPROTO_UDP,
				TCP = ::IPPROTO_TCP
			};

			explicit EFVIFeedV3(UdpCallback &cb, bool hwtimestamping, int core, const std::string &strategyId);
			~EFVIFeedV3() override;

			void AddFilter(const string& ipaddress, int port, const string& feedab) override;

			void printSeqs() override;

			void Start() override;
			void Stop() override;
			void Reset() override;

			
			void Poll();
			int poll();

			void init();
			void initEfvi();

			void RefillRing();

			void testticket();

			int Source();

		private:
			void createEfviFeedFilter(string ipaddress, int port, string feedab);

			void ef_handle_rx(int pkt_buf_i, int len);
			void ef_handle_rx_discard(int pkt_buf_i, int len, int discard_type);
			void ef_handle_batched_rx(int pkt_buf_i);
			void ef_refill_rx_ring(); //(struct app_EFVI* res);

			void hexdump(const void *pv, int len);
			int64_t timespec_diff_ns(struct timespec a, struct timespec b);

			static void *EntryPointRx(void *pValue);

			CmeMdpSettings _MdpSettings;
			const std::string _MDPFILE = "MdpSettings.txt";

			int _AFFINITY;

			std::string _szInterfaceA;
			std::string _szInterfaceB;

			std::string _szInterfaceNameA;
			std::string _szInterfaceNameB;

			int _RX_CAP;
			int _RX_BATCH;

			std::string _Me;

		private:
			Configuration _Channels;

			bool _bStarted;
			bool _bRunning;
			int unit_;

			int _MSG_COUNT;
			int _MAX_MSG_COUNT;

			std::string _intfname;

			string _feedAB;

			int _source;
			int _port;

			int _OFFSET;

			int rx_prefix_len;

			 // Thread management
			pthread_t _rx_thread_id = 0;
			std::atomic<bool> _thread_started{false};

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
			
			// Memory management tracking
			bool m_uses_mmap = false;
			size_t m_alloc_size = 0;
			
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

			std::string strategyId;

		private:
			EFVIFeedV3() = delete;
			EFVIFeedV3(const EFVIFeedV3 &) = delete;
			EFVIFeedV3 &operator=(const EFVIFeedV3 &) = delete;
		};

	} /* namespace Csm */
} /* namespace LWT */

#endif /* EFVIFEED_H_ */
