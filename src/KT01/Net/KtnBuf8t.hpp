#ifndef KTN_NET_KTNBUF8T_HPP
#define KTN_NET_KTNBUF8T_HPP

#pragma once
#include <cassert>
#include <cstdint>
#include <cstring>
#include <utility>

namespace KTN
{
namespace NET
{
#pragma pack(push, 1)

struct alignas(64) KTNBuf8t
{
	static constexpr size_t kInlineSize = 3200; // 1536;
	static_assert(kInlineSize % 64 == 0, "kInlineSize must be cache-line aligned");

	size_t buffer_length = 0;
	uint8_t *buffer = nullptr;
	uint8_t buf[kInlineSize];

	size_t num_msgs = 0;
	uint64_t recvTime = 0;
    uint64_t customerOrderTime = 0;
	uint32_t seqnum = 0;
	uint16_t msgtype = 0;
	uint16_t block_length = 0;
	uint16_t msg_length = 0;
	int16_t dir = 0;
	uint8_t segid = 0;
	uint8_t index = 0;
	bool audit_trail = false;
	bool poss_dupe = false;
	bool prev_uuid_dupe = false;
	bool owns_buffer = false;

	// Default constructor
	KTNBuf8t() : buffer_length(0), buffer(buf), owns_buffer(false)
	{
	}

	// Constructor from source data
	KTNBuf8t(const uint8_t *src, size_t len) : buffer_length(len)
	{
		if (len <= kInlineSize)
		{
			buffer = buf;
			memcpy(buf, src, len);
			owns_buffer = false;
		}
		else
		{
			buffer = new uint8_t[len];
			memcpy(buffer, src, len);
			owns_buffer = true;
		}
	}

	// Move constructor
	KTNBuf8t(KTNBuf8t &&other) noexcept
	{
		moveFrom(std::move(other));
	}

	// Move assignment
	KTNBuf8t &operator=(KTNBuf8t &&other) noexcept
	{
		if (this != &other)
		{
			cleanup();
			moveFrom(std::move(other));
		}
		return *this;
	}

	// Destructor
	~KTNBuf8t()
	{
		cleanup();
	}

	// Deleted copy constructor
	KTNBuf8t(const KTNBuf8t &) = delete;

	// Deleted copy assignment
	KTNBuf8t &operator=(const KTNBuf8t &) = delete;

	void CloneFrom(const KTNBuf8t &src)
	{
		buffer_length = src.buffer_length;
		if (buffer_length <= kInlineSize)
		{
			memcpy(buf, src.buffer, buffer_length);
			buffer = buf;
			owns_buffer = false;
		}
		else
		{
			buffer = new uint8_t[buffer_length];
			memcpy(buffer, src.buffer, buffer_length);
			owns_buffer = true;
		}

		seqnum = src.seqnum;
		msgtype = src.msgtype;
		block_length = src.block_length;
		msg_length = src.msg_length;
		segid = src.segid;
		index = src.index;
		dir = src.dir;
		audit_trail = src.audit_trail;
		poss_dupe = src.poss_dupe;
		prev_uuid_dupe = src.prev_uuid_dupe;
		recvTime = src.recvTime;
	}

  private:
	void cleanup()
	{
		if (owns_buffer && buffer && buffer != buf)
		{
			delete[] buffer;
		}
		buffer = nullptr;
		buffer_length = 0;
		owns_buffer = false;
		recvTime = 0;
	}

	void moveFrom(KTNBuf8t &&other)
	{
		if (!other.owns_buffer)
		{
			memcpy(buf, other.buf, other.buffer_length);
			buffer = buf;
			buffer_length = other.buffer_length;
			owns_buffer = false;
		}
		else
		{
			buffer = other.buffer;
			buffer_length = other.buffer_length;
			owns_buffer = true;
		}

		num_msgs = other.num_msgs;
        customerOrderTime = other.customerOrderTime;
		seqnum = other.seqnum;
		msgtype = other.msgtype;
		block_length = other.block_length;
		msg_length = other.msg_length;
		dir = other.dir;
		segid = other.segid;
		index = other.index;
		audit_trail = other.audit_trail;
		poss_dupe = other.poss_dupe;
		prev_uuid_dupe = other.prev_uuid_dupe;
		recvTime = other.recvTime;

		// Invalidate source
		other.buffer = nullptr;
		other.buffer_length = 0;
		other.owns_buffer = false;
	}
};

#pragma pack(pop)

} // namespace NET
} // namespace KTN

#endif // KTN_NET_KTNBUF8T_HPP