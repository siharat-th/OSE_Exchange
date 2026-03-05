/*
 * EFVIFeedV3.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: sgaer
 */

#include <KtnEfvi/EFVIFeedV3.h>

namespace i01 {
namespace net {

EFVIFeedV3::EFVIFeedV3(UdpCallback& cb, bool hwtimestamping, int core, const std::string &strategyId)
			: FeedHandler("EFVIFeedV3", cb)
			, _bRunning(false)
			, _MSG_COUNT(0)
			, m_vi()
			, m_mr()
			, m_pio()
			, m_dh(-1)
			, m_pd()
			, m_sfc_mac()
			, m_sfc_mtu(0)
			, m_pktbufs_p(nullptr)
			, m_pktbufs_lifo_size(0)
			, m_rxq_cushion(8)
			, m_hw_timestamping(hwtimestamping)
			, m_pktbufs_lifo(5000)
			, m_uses_mmap(false)
			, m_alloc_size(0)
			, strategyId(strategyId)
{
	//Legacy settings style
	CmeMdpSettings st;
	st.Init(_MDPFILE);

	_AFFINITY = core;

	_szInterfaceA = st.InterfaceA;
	_szInterfaceB = st.InterfaceB;

	_szInterfaceNameA = st.InterfaceNameA;
	_szInterfaceNameB = st.InterfaceNameB;

	//until we do a/b:
	_intfname = _szInterfaceNameA;
	_feedAB = "A";

	_RX_CAP = st.RxCapacity;
	_RX_BATCH = st.RxBatchSize;

	_MSG_COUNT = 0;
	_MAX_MSG_COUNT = _RX_BATCH;

	_OFFSET = EFVI_OFFSET;

	_bStarted = false;
	_bRunning = true;

	init();
}

EFVIFeedV3::~EFVIFeedV3() {
	LogMe("EFVIFeedV3 destructor starting...");
	
	// Stop thread first
	_bRunning = false;
	
	// Wait for thread to finish
	if (_rx_thread_id != 0) {
		LogMe("Waiting for EFVI RX thread to finish...");
		void* thread_result;
		int join_result = pthread_join(_rx_thread_id, &thread_result);
		if (join_result == 0) {
			LogMe("EFVI RX thread joined successfully");
		} else {
			LogMe("WARNING: pthread_join failed, rc=" + std::to_string(join_result));
		}
		_rx_thread_id = 0;
	}

	if (::ef_vi_flush(&m_vi, dh()) == 0)
	{
		LogMe("ef_vi_flush success, cleaning up packet buffers...");
		
		for (int i = 0; i < pbs_capacity(); ++i)
		{ 
			put_pktbuf(get_pktbuf(i)); 
		}
		
		::ef_memreg_free(&m_mr, dh());
		
		// **Fix this section - Use the correct method according to allocation method**
		if (m_pktbufs_p)
		{
			if (m_uses_mmap) {
				// Use munmap for memory allocated with mmap
				LogMe("Unmapping memory allocated with mmap...");
				if (munmap(m_pktbufs_p, m_alloc_size) != 0) {
					perror("munmap failed");
					LogMe("ERROR: munmap failed");
				} else {
					LogMe("Memory unmapped successfully");
				}
			} else {
				 // Use free for memory allocated with posix_memalign
				LogMe("Freeing memory allocated with posix_memalign...");
				::free(m_pktbufs_p);
				LogMe("Memory freed successfully");
			}
			m_pktbufs_p = nullptr;
		}
	} else { 
		LogMe("ERROR: ef_vi_flush failed"); 
	}
	
	::ef_vi_free(&m_vi, dh());
	::ef_pd_free(&m_pd, m_dh);
	::ef_driver_close(m_dh);
	
	LogMe("EFVIFeedV3 destructor completed");
}

void EFVIFeedV3::AddFilter(const string& ipaddress, int port, const string& feedab)
{
	createEfviFeedFilter(ipaddress, port, feedab);
}

void EFVIFeedV3::init()
{
	ef_driver_open(&m_dh);
	LogMe("EFVI DRIVER OPENED OK");

	ef_pd_alloc_by_name(&m_pd, m_dh, _intfname.c_str(), EF_PD_DEFAULT);
	LogMe("EFVI PD ALLOCATED OK ON INTERFACE " + _intfname);

	enum ef_vi_flags vi_flags = EF_VI_FLAGS_DEFAULT;

	ef_vi_alloc_from_pd(&m_vi, m_dh, &m_pd, m_dh,
							  -1, _RX_CAP, 0, NULL, -1, vi_flags);

	LogMe("VI ALLOCATED FROM PD OK");

	m_rx_prefix_len = ef_vi_receive_prefix_len(&m_vi);
	m_rxq_capacity = _RX_CAP;
	m_txq_capacity = ef_vi_transmit_capacity(&m_vi);

	_MSG_COUNT = 0;
	_MAX_MSG_COUNT = _RX_BATCH;

	ostringstream oss;
	oss << "****RX CAPACITY=" << m_rxq_capacity << " MAX__MSG_COUNT=" << _MAX_MSG_COUNT;
	LogMe(oss.str());

	int pktbufcap = ef_vi_receive_capacity(&m_vi);
	size_t alloc_size = pktbufcap * PKT_BUF_SIZE;
	
	alloc_size = ROUND_UP(alloc_size, huge_page_size);
	m_alloc_size = alloc_size;  // Store size for later use

	// Try mmap first
	m_pktbufs_p = (SFCPktBuf*)mmap(NULL, alloc_size, PROT_READ | PROT_WRITE,
					   MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);

	if (m_pktbufs_p == MAP_FAILED) {
		LogMe("mmap() failed. Are huge pages configured? Trying posix_memalign...");
		m_uses_mmap = false;

		 // Use posix_memalign instead
		if (posix_memalign((void**)&m_pktbufs_p, huge_page_size, pbs_capacity() * SFCPktBuf::alloc_size()) != 0) {
			perror("posix_memalign failed");
			LogMe("ERROR: Failed to allocate packet buffers");
			throw std::runtime_error("Failed to allocate packet buffers");
		}
		LogMe("Using posix_memalign for packet buffers");
	} else {
		m_uses_mmap = true;
		LogMe("Using mmap for packet buffers");
	}

	TRY(ef_memreg_alloc(&m_mr, m_dh, &m_pd, m_dh,
				 m_pktbufs_p, pbs_capacity() * static_cast<int>(SFCPktBuf::alloc_size())));

	LogMe("EF_MEMREG ALLOC OK");

	int i = pbs_capacity();
	m_pktbufs_lifo.reserve(i);

    for (int i = 0; i < pbs_capacity(); ++i)
    {
        SFCPktBuf * pb_p = get_pktbuf(i);
        pb_p->m_pod.m_addr = ef_memreg_dma_addr(&m_mr, i * PKT_BUF_SIZE);
        pb_p->m_pod.m_pool_id = i;
        pb_p->m_pod.m_data_size = 0;
        put_pktbuf(pb_p);
    }

	int rxfill = ef_vi_receive_fill_level(&m_vi);
	int rxpbneeded = rxq_capacity() - rxfill;
	int rxspace = ef_vi_receive_space(&m_vi);
	int refillamount = rxspace;

	m_rxq_cushion = refillamount;
	fill_rxq(m_rxq_cushion);
}

void EFVIFeedV3::Start() {
	if (_thread_started.exchange(true)) {
		LogMe("WARNING: Thread already started");
		return;
	}

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	_bRunning = true;

	LogMe("Creating EFVI RX thread...");
	
	int rc = pthread_create(&_rx_thread_id, &attr, EFVIFeedV3::EntryPointRx, this);
	if (rc != 0) {
		perror("EFVI Thread creation failed");
		LogMe("ERROR: Failed to create EFVI RX thread, rc=" + std::to_string(rc));
		_bRunning = false;
		_thread_started = false;
		return;
	}
	
	pthread_attr_destroy(&attr);
	LogMe("EFVI RX thread created successfully");
}

void EFVIFeedV3::Stop() {
	LogMe("EFVI FEED STOPPING");
	_bRunning = false;
	usleep(10000); // Wait for thread to stop
}

void EFVIFeedV3::printSeqs() {

	ostringstream oss;
	oss << "[CORE " << _AFFINITY << " " << _intfname << "] " << "cfe-efmd" << ": MSGCOUNT=" << _MSG_COUNT;
	LogMe(oss.str());


	//let's show the rx ring health:

	int rxfill = ef_vi_receive_fill_level(&m_vi);
	//typically, 4095 - # unfilled
	int rxpbneeded = rxq_capacity() - rxfill;
	int rxspace = ef_vi_receive_space(&m_vi);

	ostringstream os2;
	os2 <<"    ---> " <<  "cfe-efmd" << ": RXFILL=" << rxfill << " RXSPACE=" << rxspace << " PB_NEEDED=" << rxpbneeded ;
	LogMe(os2.str());
}

void * EFVIFeedV3::EntryPointRx(void* pValue)
{
	EFVIFeedV3 *pThis = (EFVIFeedV3 *)pValue;

	if (!pThis) {
		std::cerr << "ERROR: NULL pointer passed to EntryPointRx" << std::endl;
		pthread_exit(nullptr);
		return nullptr;
	}

	pthread_attr_t attr;
	cpu_set_t cpus;
	pthread_attr_init(&attr);

	ostringstream os;
	LOGINFO("***STARTING {} EFVI THREAD ON CORE {} INTF:{}", "cfe-efmd", pThis->strategyId, pThis->_AFFINITY, pThis->_intfname);

	CPU_ZERO(&cpus);
	CPU_SET(pThis->_AFFINITY, &cpus);

	pthread_t current_thread = pthread_self();
	
	// Try to set CPU affinity but don't exit if failed
	if (pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpus) != 0) {
		perror("pthread_setaffinity_np failed");
		pThis->LogMe("WARNING: Failed to set CPU affinity, continuing anyway");
	}

	std::string szName = pThis->strategyId + "-cfemd";

	// Try to set thread name but don't exit if failed
	int irc = pthread_setname_np(current_thread, szName.c_str());
	if (irc != 0)
	{
		perror("pthread_setname_np failed");
		pThis->LogMe("WARNING: Failed to set thread name, continuing anyway");
		// **Removed exit(1)!**
	}

	try {
		pThis->LogMe("Starting polling loop...");
		pThis->Poll();
	} catch (const std::exception& e) {
		pThis->LogMe("ERROR in Poll(): " + std::string(e.what()));
	} catch (...) {
		pThis->LogMe("ERROR: Unknown exception in Poll()");
	}

	pThis->LogMe("EFVI RX Thread exiting");
	pthread_exit(NULL);
	return NULL;
}

void EFVIFeedV3::Reset() {
	//m_TAS_SENTa = false;
}

void EFVIFeedV3::Poll() {
	_bRunning = true;

	while (_bRunning) {
		int res = poll();
	}
}


int EFVIFeedV3::poll()
{
	if (!_bRunning) return 0;

	int maxevs =  EF_VI_EVENT_POLL_MIN_EVS;
	int prelen = m_rx_prefix_len;

	//my friend i
	ef_event evs[maxevs];
	int n_ev = -1;
	n_ev = ef_eventq_poll(&m_vi, evs, static_cast<int>(sizeof(evs) / sizeof(evs[0])));

	if( n_ev > 0 )
	{
		//Timestamp swts( Timestamp::now() );
		for (int i = 0; i < n_ev; ++i)
		{
			switch(EF_EVENT_TYPE(evs[i]))
			{
				case EF_EVENT_TYPE_RX:
					ef_handle_rx(EF_EVENT_RX_RQ_ID(evs[i]),
									EF_EVENT_RX_BYTES(evs[i]) - prelen);
					break;
			  default: {
			  /* TODO FIXME: log error; unknown event type */
			  } break;
			} //end switch

	  } //end for

	  _MSG_COUNT++;
	  maintain_rxq();
	}

	return n_ev;
}

void EFVIFeedV3::createEfviFeedFilter(string ipaddress, int port, string feedab)
{
	string intf = (feedab == "A") ? _szInterfaceA : _szInterfaceB;

	ostringstream os;
	os << "SETTING EFVI UDP FILTER: IP=" << ipaddress << ":" << port << " intf=" << intf << " [" << _intfname << "]";
	LogMe(os.str());

	//INIT UDP Filter Here.
	ef_filter_spec filter_spec;
	ef_filter_spec_init(&filter_spec, EF_FILTER_FLAG_NONE);

	int fd = 0;
	u_int yes = 1;
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket filter");
		LogMe("ERROR: Failed to create socket for filter");
		return;
	}

	// Use RAII-style cleanup for socket
	auto cleanup_socket = [&fd]() { 
		if (fd >= 0) { 
			close(fd); 
			fd = -1; 
		} 
	};

	/* allow multiple sockets to use the same PORT number */
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		perror("Reusing ADDR failed");
		LogMe("ERROR: Failed to set SO_REUSEADDR");
		cleanup_socket();
		return;
	}

	int n = 16 * 1024 * 1024;
	if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) == -1) {
		perror("Set RCV BUF FAILED");
		LogMe("ERROR: Failed to set receive buffer size");
		cleanup_socket();
		return;
	}

	/* use setsockopt() to request that the kernel join a multicast group */
	struct ip_mreq ip;
	ip.imr_multiaddr.s_addr = inet_addr(ipaddress.c_str());
	ip.imr_interface.s_addr = inet_addr(intf.c_str());
	
	if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &ip, sizeof(ip)) < 0) {
		perror("setsockopt IP_ADD_MEMBERSHIP");
		LogMe("ERROR: Failed to join multicast group");
		cleanup_socket();
		return;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	
	if (inet_aton(ipaddress.c_str(), &(addr.sin_addr)) == 0) {
		LogMe("ERROR: Invalid IP address: " + ipaddress);
		cleanup_socket();
		return;
	}
	addr.sin_port = htons(port);

	if (ef_filter_spec_set_ip4_local(&filter_spec, IPPROTO_UDP, addr.sin_addr.s_addr, addr.sin_port) != 0) {
		perror("ef filter spec set");
		LogMe("ERROR: Failed to set filter spec");
		cleanup_socket();
		return;
	}

	if (ef_vi_filter_add(&m_vi, m_dh, &filter_spec, NULL) != 0) {
		perror("ef_vi_filter_add");
		LogMe("ERROR: Failed to add VI filter");
		cleanup_socket();
		return;
	}

	// Close socket - we don't need it after adding the filter
	cleanup_socket();
	
	LogMe("EFVI UDP FILTER ADDED SUCCESSFULLY");
}

void EFVIFeedV3::ef_handle_rx(int pkt_buf_i, int len)
{
	SFCPktBuf * pb_p = get_pktbuf(pkt_buf_i);

	char * buf = pb_p->data();// + _OFFSET;

	buf += _OFFSET;
	len -= _OFFSET;

	PREFETCH_RANGE(buf, len);

	callback().onRecv(buf, len, 0);

	 put_pktbuf(pb_p);
}


void EFVIFeedV3::ef_handle_rx_discard(int pkt_buf_i, int len, int discard_type)
{
	ostringstream oss;
	oss << "ERROR: dicard type=" << discard_type;
	LogMe(oss.str());

  if( /* accept_discard_pkts */ 1 ) {

    ef_handle_rx(pkt_buf_i, len);
  }

}


void EFVIFeedV3::ef_handle_batched_rx( int pkt_buf_i)
{

}


SFCPktBuf* EFVIFeedV3::get_pktbuf(int id)
{
    /* TODO: reference counting, possibly use shared_ptr. */
    if (m_pktbufs_p && id >= 0 && id < pbs_capacity())
    {
        return reinterpret_cast<SFCPktBuf*>(
            reinterpret_cast<char*>(m_pktbufs_p) + id * SFCPktBuf::alloc_size() );
    } else { return nullptr; }
}

SFCPktBuf* EFVIFeedV3::get_pktbuf()
{
    SFCPktBuf* ret = nullptr;
    /* TODO: reference counting, possibly use shared_ptr. */
    if (m_pktbufs_lifo.pop(ret) && ret)
    {
        --m_pktbufs_lifo_size;
        ret->m_pod.m_data_size = 0;
    }
    return ret;
}

bool EFVIFeedV3::put_pktbuf(SFCPktBuf* pb_p)
{
    /* TODO: reference counting, possibly use shared_ptr. */
    if ( ((char*)pb_p >= (char*)m_pktbufs_p)
      && ((char*)pb_p < ((char*)m_pktbufs_p + pbs_capacity() * SFCPktBuf::alloc_size()))
      && ((std::uintptr_t)pb_p % SFCPktBuf::alloc_size() == 0)
       )
    {
        //if (m_pktbufs_lifo.bounded_push(pb_p))
        if (m_pktbufs_lifo.push(pb_p))
        {
            ++m_pktbufs_lifo_size;
            return true;
        }
    }
    return false;
}

void EFVIFeedV3::maintain_rxq()
{
    /* TODO: add statistics collection */
    int rxfill = ::ef_vi_receive_fill_level(&m_vi);
    int rxpbneeded = rxq_capacity() - rxfill;

    if ( (m_pktbufs_lifo_size < m_rxq_cushion)
       ||(rxpbneeded < m_rxq_cushion)
         /* Receive queue size is within m_rxq_cushion of capacity. */
        )
    {
        m_rxq_cushion = 8;
        return; /* depletion not severe enough to warrant pushing */
    } else if (rxpbneeded < std::max(16, rxq_capacity() / 5)) {
        m_rxq_cushion = 16; /* <20% depleted */
    } else if (rxpbneeded < std::max(32, rxq_capacity() / 4)) {
        m_rxq_cushion = 32; /* 20% < depletion < 25% */
    } else if (rxpbneeded < std::max(64, rxq_capacity() / 3)) {
        m_rxq_cushion = 64; /* 25% < depletion < 33% */
    } else if (rxpbneeded < std::max(128, rxq_capacity() / 2)) {
        m_rxq_cushion = 128; /* 33% < depletion < 50% */
    } else { /* serious depletion > 50% */
        m_rxq_cushion = std::max(128, rxq_capacity() / 10);
    }

  //  cout << "FILLING: " << m_rxq_cushion << endl;
    fill_rxq(m_rxq_cushion);
}

int EFVIFeedV3::fill_rxq(int n)
{
    /* Remember that m_rxq_cushion is always bounded from above by the number of
     * SFCPktBufs available due to get_pktbuf()'s behavior. */
    int ret = 0;
    for (int i = 0; i < m_rxq_cushion; ++i)
    {
        SFCPktBuf * pb_p = get_pktbuf();
        if ( pb_p
          && ef_vi_receive_init(&m_vi
              , pb_p->addr() + SFCPktBuf::prefix_size()
              , pb_p->pool_id()) == 0 /* will be -EAGAIN if rxq full */
           )
        {
            ++ret;
        } else { break; }
    }
    if (ret > 0)
    { ef_vi_receive_push(&m_vi); }

    return ret;
}

} // namespace net
} // namespace i01
