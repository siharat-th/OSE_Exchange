/*
 * feed_defines.hpp
 *
 *  Created on: Feb 27, 2017
 *      Author: sgaer
 */

#ifndef FEED_DEFINES_HPP_
#define FEED_DEFINES_HPP_


#define AUDIN = 1;
#define AUDOUT = -1;

#define CSM_STATUS_PREOPEN 21
#define CSM_STATUS_MKTOPEN 17


#define CSM_L2_SNAPSHOT_FULL_REFRESH 17
#define CSM_L2_INCREMENTAL_REFRESH 18
#define CSM_L2_SECURITY_STATUS 19
#define CSM_L1_CURRENT_MARKET_UPDATE 12

#define CSM_TEST_SECID  470478434

#define MAXEVENTS 64

#define FEED_L1 1
#define FEED_L2 2
#define FEED_OE 3

#define EFVI_OFFSET 42

//0 = Fatal, 1 = Error, 2 = Warning, 3 = Info, 4 = Trace, 5 = Debug
#define LOG_FATAL 0
#define LOG_ERROR 1
#define LOG_WARNING 2
#define LOG_INFO 3
#define LOG_TRACE 4
#define LOG_DEBUG 5

#define VXT_CLASSKEY		1180728636
#define VX_CLASSKEY			141007877


//using namespace LWT::Csm::Dirty;

//FEB 2017
#define VXT_TEST_ID 478449848

//#define DBG_CAPTURE_TIME { PerformanceCounter::Current(&lat_); g_times[g_trace_count] = (now - g_last_md_timestamp).WholeNanoseconds(); g_dbug_lines[g_trace_count] = DBG_FILE_LINE; ++g_trace_count; }

#define DBG_FILE_LINE  cout << __FILE__ << " " << __LINE__ << endl;
#define DBG  cout << __LINE__ << endl;


//EFVI DEFS:
/* Hardware delivers at most ef_vi_receive_buffer_len() bytes to each
 * buffer (default 1792), and for best performance buffers should be
 * aligned on a 64-byte boundary.  Also, RX DMA will not cross a 4K
 * boundary.  The I/O address space may be discontiguous at 4K boundaries.
 * So easiest thing to do is to make buffers always be 2K in size.
 */
#define PKT_BUF_SIZE         2048

/* Align address where data is delivered onto EF_VI_DMA_ALIGN boundary,
 * because that gives best performance.
 */
//#define RX_DMA_OFF           ROUND_UP(sizeof(struct pkt_buf), EF_VI_DMA_ALIGN)
#define REFILL_BATCH_SIZE  16



#endif /* FEED_DEFINES_HPP_ */
