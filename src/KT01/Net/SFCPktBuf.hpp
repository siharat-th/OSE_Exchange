#pragma once

#include <cstddef>
#include <algorithm>

#include <unistd.h>
#include <cstddef>
#include <etherfabric/memreg.h>


namespace KT01 { namespace NET {

/**
 * @brief Represents a packet buffer used in the SFC network library.
 */
class SFCPktBuf {
public:
    /**
     * @brief Returns the size of the prefix in the packet buffer.
     * @return The size of the prefix.
     */
    static constexpr size_t prefix_size() { return offsetof(SFCPktBufPOD, m_dma_buf); }

    /**
     * @brief Returns the capacity of the packet buffer.
     * @return The capacity of the packet buffer.
     */
    static constexpr size_t capacity() { return s_capacity - prefix_size(); }

    /**
     * @brief Returns the allocation size of the packet buffer.
     * @return The allocation size of the packet buffer.
     */
    static constexpr size_t alloc_size() { return s_capacity; }

    // EFVIFeedV2*	 vi_owner()    { return m_pod.m_vi_owner; }

    /**
     * @brief Returns the address of the packet buffer.
     * @return The address of the packet buffer.
     */
    ef_addr& addr() { return m_pod.m_addr; }

    /**
     * @brief Returns the pool ID of the packet buffer.
     * @return The pool ID of the packet buffer.
     */
    int pool_id() { return m_pod.m_pool_id; }

    /**
     * @brief Returns the size of the packet buffer.
     * @return The size of the packet buffer.
     */
    size_t size() { return m_pod.m_data_size; }

    /**
     * @brief Returns a constant pointer to the data in the packet buffer.
     * @return A constant pointer to the data in the packet buffer.
     */
    const char* data() const { return m_pod.m_dma_buf; }

    /**
     * @brief Returns a pointer to the data in the packet buffer.
     * @return A pointer to the data in the packet buffer.
     */
    char* data() { return m_pod.m_dma_buf; }

    /**
     * @brief Returns a constant pointer to the secondary data in the packet buffer.
     * @return A constant pointer to the secondary data in the packet buffer.
     */
    const char* data2() const { return m_pod.m_dma_buf_p; }

    /**
     * @brief Returns a pointer to the secondary data in the packet buffer.
     * @return A pointer to the secondary data in the packet buffer.
     */
    char* data2() { return m_pod.m_dma_buf_p; }

    /**
     * @brief Returns a constant pointer to the data at the specified index in the packet buffer.
     * @param n The index of the data.
     * @return A constant pointer to the data at the specified index in the packet buffer.
     */
    const char* operator[](int n) const { return data() + n; }

    /**
     * @brief Returns a pointer to the data at the specified index in the packet buffer.
     * @param n The index of the data.
     * @return A pointer to the data at the specified index in the packet buffer.
     */
    char* operator[](int n) { return data() + n; }

    /**
     * @brief Represents the Packet Buffer POD (Plain Old Data) structure.
     */
    struct SFCPktBufPOD {
        /// Which SFCVirtualInterface owns this SFCPktBuf, for forward compatibility with RSS, etc.
        // EFVIFeedV2*  m_vi_owner;
        ef_addr m_addr;
        int m_pool_id;
        size_t m_data_size;
        char* m_dma_buf_p;
        char m_dma_buf[1] __attribute__((aligned(EF_VI_DMA_ALIGN)));
    } m_pod;

private:
    static const int s_capacity = 2048;
    static_assert(s_capacity < 4096, "s_capacity must be smaller than pagesize (4096)");

    // friend class EFVIFeedV2;
};

} }
