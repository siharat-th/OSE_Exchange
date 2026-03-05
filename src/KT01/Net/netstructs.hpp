//============================================================================
// Name        	: netstructs.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 11, 2023 1:18:34 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_NET_NETSTRUCTS_HPP_
#define SRC_EXCHSUPPORT_CME_NET_NETSTRUCTS_HPP_

#pragma once

#include <stdio.h>
# include <sys/types.h>
# include <iostream>
# include <vector>
# include <map>

namespace KTN {
namespace NET{

/**
 * @brief Structure representing a KTN binary message container
 */
struct KTN_MSG
{
    uint64_t uuid;       // Larger data types first to minimize padding
    uint32_t buffer_length;
    int num_msgs;
    int segid;
    uint32_t seqnum;
    uint16_t fd;
    uint16_t msgtype;
    int16_t dir;
    int16_t slot;
    bool isA;
    char* buffer;        // Pointer at the end to reduce alignment issues
} __attribute__((aligned(64), packed));

#pragma pack(push, 1)  // Disable padding
/**
 * @brief Structure representing a KTN buffer.
 */
struct KTNBuf
{
    size_t buffer_length;  // Frequently accessed, placed first
    char* buffer;          // Pointer to the data buffer
    char buf[1536];       // Cache-friendly buffer (1536 = 24 * 64)
    size_t num_msgs;      // Frequently accessed, placed next
    uint64_t customerOrderTime;  // Customer order time for audit trail
    uint32_t seqnum;
    uint16_t msgtype;
    int16_t dir;
    uint8_t segid;
    uint8_t index;
    bool audit_trail;
    // uint64_t key;       // Uncomment if needed, place before buf for alignment

    KTNBuf() : buffer_length(0), buffer(buf), customerOrderTime(0) {}  // Constructor to initialize buffer and buffer_length
} __attribute__((aligned(64)));  // Align the struct to 64 bytes
#pragma pack(pop)  // Restore default packing


struct RecoveredMessage {
    uint32_t seqnum = 0;
    uint16_t msgtype = 0;
    uint16_t msg_length = 0;
    std::vector<uint8_t> payload;

    bool poss_dupe = false;
    bool prev_uuid_dupe = false;
};

// #pragma pack(push, 1)  // Disable padding
// /**
//  * @brief Structure representing a KTN buffer with 8-bit data.
//  */
// struct KTNBuf8t
// {
//     size_t buffer_length;  // Frequently accessed, placed first
//     uint8_t* buffer;          // Pointer to the data buffer
//     uint8_t buf[1536];       // Cache-friendly buffer (1536 = 24 * 64)
//     size_t num_msgs;      // Frequently accessed, placed next
//     uint32_t seqnum;
//     uint16_t msgtype;
//     uint16_t block_length; //size of basic message block, not including header or repeating groups
//     uint16_t msg_length;
//     int16_t dir;
//     uint8_t segid;
//     uint8_t index;
//     bool audit_trail;
//     bool poss_dupe;
//     bool prev_uuid_dupe;
//     // uint64_t key;       // Uncomment if needed, place before buf for alignment

//     KTNBuf8t() : buffer(buf), buffer_length(0) {}  // Constructor to initialize buffer and buffer_length
// };// __attribute__((aligned(64)));  // Align the struct to 64 bytes
// #pragma pack(pop)  // Restore default packing

/**
 * @brief Structure representing a simple KTN buffer.
 */
struct KTNSimp
{
    char buffer[132];
    uint16_t buffer_length;
    KTNSimp() : buffer_length(0) {}
} __attribute__((aligned(64)));

}
}

#endif /* SRC_EXCHSUPPORT_CME_NET_NETSTRUCTS_HPP_ */
