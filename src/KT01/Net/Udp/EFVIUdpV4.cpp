/*
 * EFVIFeed.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: sgaer
 */

#include <KT01/Net/Udp/EFVIUdpV4.hpp>

#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>

using namespace KT01::NET::UDP;
using namespace KT01::NET;

EFVIUdpV4::EFVIUdpV4(std::string feed, bool hwtimestamping,
					 std::string name, bool useCertNrConfigFile)
	: m_bRunning(false), MSG_COUNT(0),
	  _NAME(name), _servicename(name), m_vi(), m_mr(), m_pio(), m_dh(-1), m_pd(), m_sfc_mac(), m_sfc_mtu(0), m_pktbufs_p(nullptr), m_pktbufs_lifo_size(0), m_rxq_cushion(8), 
	  m_hw_timestamping(false), m_pktbufs_lifo(5000),
	  _useCertNrConfigFile(useCertNrConfigFile)
{
#if !AKL_TEST_MODE
	init();
	// CreateAndRun("ef-refill",1);

	// Create the refill thread...
	// pthread_t m_tidSub;
	// int rc = 0;
	// if ((rc = pthread_create(&m_tidSub, NULL, EFVIUdpV4::EntryPoint, this)) != 0)
	// {
	// 	perror("EFVI REFILL THREAD");
	// 	return;
	// }
#endif
}

// void *EFVIUdpV4::EntryPoint(void *pValue)
// {
// 	EFVIUdpV4 *pThis = (EFVIUdpV4 *)pValue;

// 	pthread_t current_thread = pthread_self();
// 	std::string thread_name = "ef-refill";

// 	int irc = pthread_setname_np(current_thread,
// 								 thread_name.c_str());
// 	if (irc != 0)
// 	{
// 		cout << "ERROR NAMING THREAD: " << thread_name << endl;
// 		exit(1);
// 	}

// 	int core = 1;
// 	cpu_set_t cpus;
// 	CPU_ZERO(&cpus);
// 	CPU_SET(core, &cpus);
// 	irc = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpus);
// 	if (irc != 0)
// 	{
// 		cout << "ERROR SETTING THREAD AFFINITY: " << core << endl;
// 		exit(1);
// 	}

// 	pThis->_ACTIVE.store(true, std::memory_order_relaxed);

// 	int MAX_CNT = pThis->MAX_MSG_COUNT;

// 	while (pThis->_ACTIVE.load(std::memory_order_acquire))
// 	{
// 		// if (pThis->MSG_COUNT.load(std::memory_order_acquire) >= MAX_CNT - 32)
// 		// {
// 		// 	pThis->maintain_rxq();

// 		// 	pThis->MSG_COUNT.store(0, std::memory_order_relaxed);
// 		// 	// cout << "BAM" << endl;
// 		// }

// 		usleep(10);
// 	}

// 	return NULL;
// }

EFVIUdpV4::~EFVIUdpV4()
{

	_ACTIVE.store(false, std::memory_order_relaxed);
	usleep(1000);

	if (::ef_vi_flush(&m_vi, dh()) == 0)
	{
		for (int i = 0; i < pbs_capacity(); ++i)
		{
			put_pktbuf(get_pktbuf(i));
		}
		::ef_memreg_free(&m_mr, dh());
		if (m_pktbufs_p)
		{
			// old:
			//::free(&m_pktbufs_p);
			::free(m_pktbufs_p);
			m_pktbufs_p = nullptr;
		}
	}
	else
	{
		std::cerr << "ef_vi_flush failed." << std::endl;
	}
	::ef_vi_free(&m_vi, dh());

	// i01 SFInterface base
	::ef_pd_free(&m_pd, m_dh);
	::ef_driver_close(m_dh);
}

void EFVIUdpV4::init()
{
	KT01_LOG_INFO(__CLASS__, "LOADING UDP SETTINGS FROM NetSettings.txt");
	NetSettings::Load(_sett, KT01::NET::Protocol::UDP, "NetSettings.txt");

	MSG_COUNT.store(0, std::memory_order_relaxed);
	MAX_MSG_COUNT = _sett.RxBatchSize;
	m_rxq_capacity = _sett.RxCapacity;
	OFFSET = EFVI_OFFSET;

	m_bStarted = false;
	m_bRunning = true;

	ostringstream oss;
	oss << "****RX CAPACITY=" << m_rxq_capacity << " MAX_MSG_COUNT=" << MAX_MSG_COUNT;
	KT01_LOG_INFO(__CLASS__, oss.str());

	MAX_MSG_COUNT = _sett.RxBatchSize;

	KT01_LOG_INFO(__CLASS__, oss.str());
	// until we do a/b:
	_intfname = _sett.IntfNameA;
	_feedAB = "A";

	ef_driver_open(&m_dh);
	KT01_LOG_INFO(__CLASS__, "EFVI DRIVER OPENED OK");

	ef_pd_alloc_by_name(&m_pd, m_dh, _intfname.c_str(), EF_PD_DEFAULT);
	KT01_LOG_INFO(__CLASS__, "EFVI PD ALLOCATED OK ON INTERFACE " + _intfname);

	enum ef_vi_flags vi_flags = EF_VI_FLAGS_DEFAULT;

	ef_vi_alloc_from_pd(&m_vi, m_dh, &m_pd, m_dh,
						-1, _sett.RxCapacity, 0, NULL, -1, vi_flags);

	KT01_LOG_INFO(__CLASS__, "VI ALLOCATED FROM PD OK");

	m_rx_prefix_len = ef_vi_receive_prefix_len(&m_vi);
	m_evq_capacity = ef_eventq_capacity(&m_vi);
	m_rxq_capacity = ef_vi_receive_capacity(&m_vi);
	m_txq_capacity = ef_vi_transmit_capacity(&m_vi);

	MSG_COUNT = 0;
	MAX_MSG_COUNT = _sett.RxBatchSize;

	oss.clear();
	oss.str("");
	oss << "****RX CAPACITY=" << m_rxq_capacity << " MAX_MSG_COUNT=" << MAX_MSG_COUNT;
	KT01_LOG_INFO(__CLASS__, oss.str());

	//	/* TODO FIXME: use get_hugepage_region with GHR_FALLBACK
	//	 *             since these pages will be locked in physical memory anyhow,
	//	 *             TLB misses shouldn't be an issue (with hugepages) */
	//	if (::posix_memalign( (void **)&m_pktbufs_p
	//						, SFCPktBuf::alloc_size()
	//						, pbs_capacity() * SFCPktBuf::alloc_size() ) != 0)
	//	{
	//		perror("posix_memalign");
	//		throw std::runtime_error("posix_memalign failed.");
	//	}

	int pktbufcap = ef_vi_receive_capacity(&m_vi);

	size_t alloc_size = pktbufcap * PKT_BUF_SIZE;

	alloc_size = ROUND_UP(alloc_size, huge_page_size);
	m_pktbufs_p = (SFCPktBuf *)mmap(NULL, alloc_size, PROT_READ | PROT_WRITE,
									MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);

	if (m_pktbufs_p == MAP_FAILED)
	{
		KT01_LOG_ERROR(__CLASS__, "mmap() failed. Are huge pages configured?");

		/* Allocate huge-page-aligned memory to give best chance of allocating
		 * transparent huge-pages.
		 */
		TEST(posix_memalign((void **)m_pktbufs_p, huge_page_size, pbs_capacity() * SFCPktBuf::alloc_size()) == 0);
	}

	//int allolen = pbs_capacity() * static_cast<int>(SFCPktBuf::alloc_size());

	TRY(ef_memreg_alloc(&m_mr, m_dh, &m_pd, m_dh,
						m_pktbufs_p, pbs_capacity() * static_cast<int>(SFCPktBuf::alloc_size())));
	KT01_LOG_INFO(__CLASS__, "EF_MEMREG ALLOC OK");

	int i = pbs_capacity();
	m_pktbufs_lifo.reserve(i);
	// m_pktbufs_lifo(pbs_capacity());
	// int pbprefix = SFCPktBuf::prefix_size() ;

	for (int i = 0; i < pbs_capacity(); ++i)
	{
		/* TODO: reference counting, possibly use shared_ptr. */
		SFCPktBuf *pb_p = get_pktbuf(i);
		pb_p->m_pod.m_addr = ::ef_memreg_dma_addr(&m_mr, i * PKT_BUF_SIZE);
		pb_p->m_pod.m_pool_id = i;
		pb_p->m_pod.m_data_size = 0; /* XXX: redundant with put_pktbuf() */
		put_pktbuf(pb_p);
	}

	////# of unfilled pkt_bufs
	//	int rxfill = ef_vi_receive_fill_level(&m_vi);
	////	//typically, 4095 - # unfilled
	//	int rxpbneeded = rxq_capacity() - rxfill;
	////	//# of slots available to push out
	int rxspace = ef_vi_receive_space(&m_vi); //= RXPBNEEDED
	int refillamount = rxspace;

	m_rxq_cushion = refillamount;

	fill_rxq(m_rxq_cushion);
}

void EFVIUdpV4::EfPrintStats()
{
	ostringstream oss;
	oss << "[CORE " << _sett.Affinity << " " << _intfname << "] " << _servicename << ": MSGCOUNT=" << MSG_COUNT;
	KT01_LOG_INFO(__CLASS__, oss.str());

	// let's show the rx ring health:
	int rxfill = ef_vi_receive_fill_level(&m_vi);
	// typically, 4095 - # unfilled
	int rxpbneeded = rxq_capacity() - rxfill;
	int rxspace = ef_vi_receive_space(&m_vi);

	ostringstream os2;
	os2 << "    ---> " << _servicename << ": RXFILL=" << rxfill << " RXSPACE=" << rxspace << " PB_NEEDED=" << rxpbneeded;
	KT01_LOG_INFO(__CLASS__, os2.str());

	_trk2.printMeasurements();
}

void EFVIUdpV4::printStatDetails(std::vector<int> &vec, std::string title)
{
	double dSum = 0;
	int iMin = 1000000;
	int iMax = 0;

	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] > iMax)
			iMax = vec[i];
		if (vec[i] < iMin)
			iMin = vec[i];
		dSum += vec[i];
	}

	double dAvg = dSum / vec.size();

	char buff[222];
	sprintf(buff, "%s MIN:%dns| MAX:%dns| AVG:%.2f| CNT:%d  (nanos)", title.c_str(), iMin, iMax, dAvg, (int)vec.size());
	KT01_LOG_INFO(__CLASS__, buff);
}

void EFVIUdpV4::poll()
{
	int maxevs = EF_VI_EVENT_POLL_MIN_EVS;
	//	int prelen = m_rx_prefix_len;

	// my friend i
	ef_event evs[maxevs];
	int n_ev = -1;
	n_ev = ef_eventq_poll(&m_vi, evs, static_cast<int>(sizeof(evs) / sizeof(evs[0])));

	//	cout << "GOT PACKET nev=" << n_ev << endl;
	//	usleep(1);

	if (n_ev > 0)
	{
		// Timestamp swts( Timestamp::now() );
		const uint64_t recvTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		for (int i = 0; i < n_ev; ++i)
		{
			switch (EF_EVENT_TYPE(evs[i]))
			{
			case EF_EVENT_TYPE_RX:
			{
				//					ef_handle_rx(EF_EVENT_RX_RQ_ID(evs[i]),
				//				EF_EVENT_RX_BYTES(evs[i]) - m_rx_prefix_len);
				//_trk2.startMeasurement("getPktBuf");
				SFCPktBuf *pb_p = get_pktbuf(EF_EVENT_RX_RQ_ID(evs[i]));
				//_trk2.stopMeasurement("getPktBuf");

				if (pb_p)
				{
					//						Timestamp hwts;
					//						int rxtsn = -ENODATA;
					//						unsigned timesync_flags = 0;
					//						if (m_hw_timestamping)
					//							rxtsn = ::ef_vi_receive_get_timestamp_with_sync_flags(
					//								  &m_vi
					//								, pb_p->data()
					//								, static_cast<KT01::Core::POSIXTimeSpec*>(&hwts)
					//								, &timesync_flags);
					//						bool hwts_clock_set = timesync_flags | EF_VI_SYNC_FLAG_CLOCK_SET;
					//						bool hwts_clock_in_sync = timesync_flags | EF_VI_SYNC_FLAG_CLOCK_IN_SYNC;
					char *buf = pb_p->data() + m_rx_prefix_len;
					int len = EF_EVENT_RX_BYTES(evs[i]) - m_rx_prefix_len;

					struct ethhdr *eth = (struct ethhdr *)buf;

					uint32_t multicastAddr;
					if (ntohs(eth->h_proto) == ETH_P_IP)
					{
						struct iphdr *ip = (struct iphdr *)(buf + sizeof(struct ethhdr));
						multicastAddr = ip->daddr;
					}
					else
					{
						multicastAddr = 0;
					}

					buf += OFFSET;
					len -= OFFSET;
					PREFETCH_RANGE(buf, len);
					// TODO FIXME: add jumbo frame support.
					//						if (UNLIKELY(rxtsn < 0))
					//							hwts = {0,0};
					// on_rx_cb(buf, len, swts, hwts, hwts_clock_set, hwts_clock_in_sync);
					on_rx_cb(buf, len, multicastAddr, recvTime);
					put_pktbuf(pb_p);
				}
				//
				//					char * buf = pb_p->data();// + OFFSET;
				//
				//					buf += OFFSET;
				//					len -= OFFSET;
				//
				//					PREFETCH_RANGE(buf, len);
				//
				//					on_rx_cb(buf, len);
				//
				//
				//					_trk2.startMeasurement("putPktBuf");
				//					put_pktbuf(pb_p);
				//					_trk2.stopMeasurement("putPktBuf");
			}
			break;
			default:
			{
				/* TODO FIXME: log error; unknown event type */
			}
			break;
			} // end switch
		} // end for

		MSG_COUNT++;
		maintain_rxq();
		//  cout << "MSGCOUNT=" << MSG_COUNT << "/" << MAX_MSG_COUNT << endl;
		//	  if (MSG_COUNT >= MAX_MSG_COUNT)
		//	  {
		//		  maintain_rxq();
		//
		//		  MSG_COUNT = 0;
		//	  }

		//  _trk2.stopMeasurement("efvi_poll");
	}

	// return n_ev;
}

bool EFVIUdpV4::apply_filter(const string &connid, const string &ipaddr, int port, bool isA)
{
	string intf = (isA) ? _sett.IntfA : _sett.IntfB;

	ostringstream os;
	os << "EFVIV3 ADDING FILTER " << connid << " on " << ipaddr << ":" << port
	   << " intf=" << intf << " [" << _intfname << "]";
	KT01_LOG_INFO(__CLASS__, os.str());

	// INIT UDP Filter Here.
	ef_filter_spec filter_spec;
	ef_filter_spec_init(&filter_spec, EF_FILTER_FLAG_NONE);

	int fd = 0;
	u_int yes = 1; /*** MODIFICATION TO ORIGINAL */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket filter");
		exit(1);
	}

	/* allow multiple sockets to use the same PORT number */
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
	{
		perror("Reusing ADDR failed");
		exit(1);
	}

	int n = 16 * 1024 * 1024;
	if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) == -1)
	{
		perror("Set RCV BUF FAILED");
		exit(1);
	}

	//
	/* use setsockopt() to request that the kernel join a multicast group */
	struct ip_mreq ip;
	ip.imr_multiaddr.s_addr = inet_addr(ipaddr.c_str());
	ip.imr_interface.s_addr = inet_addr(intf.c_str());
	if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &ip, sizeof(ip)) < 0)
	{
		perror("setsockopt");
		exit(1);
	}

	// ORIGINAL7
	// int port = atoi(connport.c_str());
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	inet_aton(ipaddr.c_str(), &(addr.sin_addr));
	addr.sin_port = htons(port);

	if (ef_filter_spec_set_ip4_local(&filter_spec, IPPROTO_UDP, addr.sin_addr.s_addr, addr.sin_port) != 0)
	{
		perror("ef filter spec new");
		cout << "EXITING" << endl;
		return false;
	}

	if (ef_vi_filter_add(&m_vi, m_dh, &filter_spec, NULL) != 0)
	{
		perror("ef_vi_filter_add");
		cout << "EXITING" << endl;
		return false;
	}

	return true;
}

SFCPktBuf *EFVIUdpV4::get_pktbuf(int id)
{
	/* TODO: reference counting, possibly use shared_ptr. */
	if (m_pktbufs_p && id >= 0 && id < pbs_capacity())
	{
		return reinterpret_cast<SFCPktBuf *>(
			reinterpret_cast<char *>(m_pktbufs_p) + id * SFCPktBuf::alloc_size());
	}
	else
	{
		return nullptr;
	}
}

SFCPktBuf *EFVIUdpV4::get_pktbuf()
{
	SFCPktBuf *ret = nullptr;
	/* TODO: reference counting, possibly use shared_ptr. */
	if (m_pktbufs_lifo.pop(ret) && ret)
	{
		--m_pktbufs_lifo_size;
		ret->m_pod.m_data_size = 0;
	}
	return ret;
}

bool EFVIUdpV4::put_pktbuf(SFCPktBuf *pb_p)
{
	/* TODO: reference counting, possibly use shared_ptr. */
	if (((char *)pb_p >= (char *)m_pktbufs_p) && ((char *)pb_p < ((char *)m_pktbufs_p + pbs_capacity() * SFCPktBuf::alloc_size())) && ((std::uintptr_t)pb_p % SFCPktBuf::alloc_size() == 0))
	{
		if (m_pktbufs_lifo.push(pb_p))
		{
			++m_pktbufs_lifo_size;
			return true;
		}
	}
	return false;
}

// void EFVIUdpV4::Run()
//{
//	_ACTIVE.store(true, std::memory_order_relaxed);
//
//	while(_ACTIVE.load(std::memory_order_acquire) == true)
//	{
//		if (MSG_COUNT >= MAX_MSG_COUNT)
//		{
//		  maintain_rxq();
//		  MSG_COUNT = 0;
//		}
//
//		usleep(100);
//	}
// }

void EFVIUdpV4::RefillRing()
{
	maintain_rxq();
}

void EFVIUdpV4::maintain_rxq()
{
	// _trk2.startMeasurement("fill_level");
	int rxfill = ef_vi_receive_fill_level(&m_vi);
	int rxpbneeded = rxq_capacity() - rxfill;
	//  _trk2.stopMeasurement("fill_level");

	//	m_rxq_cushion = rxpbneeded;

	// cout << "lifo=" << m_pktbufs_lifo_size << " rxcush=" << m_rxq_cushion << endl;

	if ((m_pktbufs_lifo_size < m_rxq_cushion) || (rxpbneeded < m_rxq_cushion)
		/* Receive queue size is within m_rxq_cushion of capacity. */
	)
	{

		m_rxq_cushion = 8;
		return; /* depletion not severe enough to warrant pushing */
	}
	else if (rxpbneeded < std::max(16, rxq_capacity() / 5))
	{
		m_rxq_cushion = 16; /* <20% depleted */
	}
	else if (rxpbneeded < std::max(32, rxq_capacity() / 4))
	{
		m_rxq_cushion = 32; /* 20% < depletion < 25% */
	}
	else if (rxpbneeded < std::max(64, rxq_capacity() / 3))
	{
		m_rxq_cushion = 64; /* 25% < depletion < 33% */
	}
	else if (rxpbneeded < std::max(128, rxq_capacity() / 2))
	{
		m_rxq_cushion = 128; /* 33% < depletion < 50% */
	}
	else
	{ /* serious depletion > 50% */
		m_rxq_cushion = std::max(128, rxq_capacity() / 10);
	}

	//  cout << "FILLING: " << m_rxq_cushion << endl;
	//_trk2.startMeasurement("fill_rxq");
	fill_rxq(m_rxq_cushion);
	//_trk2.stopMeasurement("fill_rxq");
}

int EFVIUdpV4::fill_rxq(int n)
{

	/* Remember that m_rxq_cushion is always bounded from above by the number of
	 * SFCPktBufs available due to get_pktbuf()'s behavior. */
	int ret = 0;
	for (int i = 0; i < m_rxq_cushion; ++i)
	{
		//_trk2.startMeasurement("getPktBufStack");
		SFCPktBuf *pb_p = get_pktbuf();
		//_trk2.stopMeasurement("getPktBufStack");

		// _trk2.startMeasurement("if_pb_p");
		if (pb_p && ef_vi_receive_init(&m_vi, pb_p->addr() + SFCPktBuf::prefix_size(), pb_p->pool_id()) == 0 /* will be -EAGAIN if rxq full */
		)
		{
			++ret;
			//    _trk2.stopMeasurement("if_pb_p");
		}
		else
		{
			//	 _trk2.stopMeasurement("if_pb_p");
			break;
		}
	}
	if (ret > 0)
	{
		//	 _trk2.startMeasurement("ef_push");
		ef_vi_receive_push(&m_vi);
		//	 _trk2.stopMeasurement("ef_push");
	}

	// int rxfill = ::ef_vi_receive_fill_level(&m_vi);
	// int rxpbneeded = rxq_capacity() - rxfill;

	//	cout << "AFTER FILL: NEEDED: " << rxpbneeded << " CAPACITY=" << rxq_capacity() << " FILL LVL=" << rxfill << endl;

	return ret;
}

// void EFVIFeedV3::on_rx_cb(char * buf, int len){}
