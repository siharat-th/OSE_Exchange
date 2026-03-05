#ifndef CONF_HPP
#define CONF_HPP

#include <cstdint>

namespace KT01 {
namespace NET {
namespace TCP {

//============================================================================
/*
NOTE ON SendBuf1K:
Whether to use 1024 bytes for a DMA send buffer. 
If set to false, 2048 bytes is used. 
ef_vi requires that a DMA buffer be in a 4096 aligned block, and as typical MTU 
is 1500 bytes, using 2048 sized buffer will have 25% memory wasted. 
So setting `SendBuf1K` to true will maximize memory utilization but reducing the 
max SMSS to 960 bytes

IMPORTANT: CME will send packets of 1460 bytes (1500 with TCP header, therefore 1500, 
which is MTU) for bulk messages (i.e. lots of fills). 

So we should NOT enable this, even though it’s technically more efficient. 

We may want to look at this for CFE however. 

*/

struct EfviConfig
{
    static constexpr bool UseCtpio = true;
    static constexpr bool Debug = true;
    static constexpr uint32_t ConnSendBufCnt = 512;
    static constexpr bool SendBuf1K = false;
    static constexpr uint32_t ConnRecvBufSize = 4096;
    static constexpr uint32_t MaxConnCnt = 1;
    static constexpr uint32_t MaxTimeWaitConnCnt = 2;
    static constexpr uint32_t RecvBufCnt = 1024;
    static constexpr uint32_t SynRetries = 3;
    static constexpr uint32_t TcpRetries = 10;
    static constexpr uint32_t DelayedAckMS = 0;
    static constexpr uint32_t MinRtoMS = 5;
    static constexpr uint32_t MaxRtoMS = 10;
    static constexpr bool WindowScaleOption = false;
    static constexpr bool TimestampOption = false;
    static constexpr int CongestionControlAlgo = 0;
    static constexpr uint32_t UserTimerCnt = 2;

    struct UserData
    {
        virtual ~UserData() = default;  // Add at least one member
    };
};

}  // namespace TCP
} // namespace NET
}  // namespace KTN

#endif  // CONF_HPP
