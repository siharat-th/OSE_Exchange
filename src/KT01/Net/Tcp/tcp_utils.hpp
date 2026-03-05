//============================================================================
// Name        	: tcp_utils.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 18, 2023 5:31:50 PM
//============================================================================

#ifndef SRC_KTNTCP_TCP_UTILS2_HPP_
#define SRC_KTNTCP_TCP_UTILS2_HPP_

#pragma once

#include <string>
#include <unistd.h>
#include <stddef.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <atomic>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>

#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include <ifaddrs.h>
#include <stdbool.h>
#include <netdb.h>
#include <pthread.h>



#include <cstdint>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <zf/zf.h>
#include <etherfabric/vi.h>
#include <etherfabric/pio.h>
#include <etherfabric/pd.h>
#include <etherfabric/memreg.h>
#include <etherfabric/capabilities.h>
#include <etherfabric/checksum.h>
#include <ci/tools.h>
#include <ci/tools/ipcsum_base.h>
#include <ci/tools/ippacket.h>
#include <ci/net/ethernet.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <net/if.h>
#include <arpa/inet.h>

#include <ifaddrs.h>


#define MTU                   1500
#define MAX_ETH_HEADERS       (14/*ETH*/ + 4/*802.1Q*/)
#define MAX_IP_TCP_HEADERS    (20/*IP*/ + 20/*TCP*/ + 12/*TCP options*/)
#define MAX_PACKET            (MTU + MAX_ETH_HEADERS)
#define MAX_MESSAGE           (MTU - MAX_IP_TCP_HEADERS)
#define UDP_HEADER_SIZE       (ETH_HLEN + sizeof(ci_ip4_hdr) + sizeof(ci_udp_hdr))
//#define N_RX_BUFS	256u
//#define N_TX_BUFS	128u
//#define FIRST_TX_BUF          0u
#define PKT_BUF_SIZE          2048
#define REFILL_BATCH          8

#define N_RX_BUFS             128u
#define N_TX_BUFS             1u
#define FIRST_TCP_TX_BUF      0u



template<typename T>
void print_args(T arg) {
    std::cout << arg << std::endl;
}

template<typename T, typename... Args>
void print_args(T arg, Args... args) {
    std::cout << arg << " ";
    print_args(args...);
}


struct pkt_buf {
  ef_addr           dma_addr;
  int               id;
  char              dma_start[1]  EF_VI_ALIGN(EF_VI_DMA_ALIGN);
};


//do we need these?
//EFVI DEFS:
/* Hardware delivers at most ef_vi_receive_buffer_len() bytes to each
 * buffer (default 1792), and for best performance buffers should be
 * aligned on a 64-byte boundary.  Also, RX DMA will not cross a 4K
 * boundary.  The I/O address space may be discontiguous at 4K boundaries.
 * So easiest thing to do is to make buffers always be 2K in size.
 */
#define PKT_BUF_SIZE         2048
#define PCKT_LEN 8192

/* Align address where data is delivered onto EF_VI_DMA_ALIGN boundary,
 * because that gives best performance.
 */
//#define RX_DMA_OFF           ROUND_UP(sizeof(struct pkt_buf), EF_VI_DMA_ALIGN)
//#define REFILL_BATCH_SIZE  16

//EFVI SEND:
#define MAX_UDP_PAYLEN	(1500 - sizeof(ci_ip4_hdr) - sizeof(ci_tcp_hdr))

#define BUF_SIZE        2048

/* This gives a frame len of 70, which is the same as:
**   eth + ip + tcp + tso + 4 bytes payload
*/
#define DEFAULT_PAYLOAD_SIZE  28
#define LOCAL_PORT            12345


#endif /* SRC_KTNTCP_TCP_UTILS_HPP_ */
