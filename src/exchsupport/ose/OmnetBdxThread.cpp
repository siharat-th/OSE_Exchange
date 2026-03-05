//============================================================================
// Name        	: OmnetBdxThread.cpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
//============================================================================

#include "OmnetBdxThread.hpp"
#include <KT01/Core/Log.hpp>
#include <KT01/SecDefs/OseSecMaster.hpp>
#include <Orders/OrderEnumsV2.hpp>
#include <cstring>
#include <chrono>

using namespace KT01::SECDEF::OSE;

#undef __CLASS__
#define __CLASS__ "OmnetBdxThread"

// Helper: swap network-order int64_t to host order
static inline int64_t SwapInt64(const void* src)
{
	int64_t result;
	swapint64_t(&result, const_cast<void*>(src));
	return result;
}

namespace KTN::OSE {

OmnetBdxThread::OmnetBdxThread(SPSCRingBuffer<KTN::OrderPod>& responseQueue,
                                 const OseSessionSettings& sett)
	: _responseQueue(responseQueue)
	, _sett(sett)
	, _active(false)
{
}

OmnetBdxThread::~OmnetBdxThread()
{
	Stop();
}

bool OmnetBdxThread::Start()
{
	// Login with separate session for broadcast polling
	if (!_session.Login(_sett, _sett.ForceLogin))
	{
		KT01_LOG_ERROR(__CLASS__, "Failed to login BDX session");
		return false;
	}

	// Subscribe to all broadcasts
	_session.SubscribeAll();

	// Start polling thread
	_active.store(true, std::memory_order_release);
	_thread = std::thread(&OmnetBdxThread::Run, this);

	KT01_LOG_INFO(__CLASS__, "BDX polling thread started");
	return true;
}

void OmnetBdxThread::Stop()
{
	_active.store(false, std::memory_order_release);

	if (_thread.joinable())
		_thread.join();

	_session.Logout();
	KT01_LOG_INFO(__CLASS__, "BDX polling thread stopped");
}

void OmnetBdxThread::Run()
{
	char evbuf[MAX_RESPONSE_SIZE];
	size_t evlen;

	while (_active.load(std::memory_order_acquire))
	{
		evlen = sizeof(evbuf);

		// Poll for events (non-blocking with READEV_OPTMSK_MANY)
		int status = _session.ReadEvent(evbuf, evlen, OMNI_EVTTYP_ALL, READEV_OPTMSK_MANY);

		if (status == OMNIAPI_SUCCESS)
		{
			// Check broadcast type
			if (evlen >= sizeof(broadcast_type_t))
			{
				broadcast_type_t* btype = (broadcast_type_t*)evbuf;

				if (btype->central_module_c == 'B' && btype->server_type_c == 'O')
				{
					uint16_t txnum;
					PUTSHORT(txnum, btype->transaction_number_n);

					if (txnum == 5) // BO5 - Firm Order Book
					{
						ParseBO5(evbuf, evlen);
					}
					else if (_sett.DebugRecvBytes)
					{
						KT01_LOG_INFO(__CLASS__, "BDX: BO" + std::to_string(txnum) +
						              " len=" + std::to_string(evlen));
					}
				}
				else if (btype->central_module_c == 'B' && btype->server_type_c == 'N')
				{
					ParseNetworkEvent(evbuf, evlen);
				}
				else if (_sett.DebugRecvBytes)
				{
					KT01_LOG_INFO(__CLASS__, "BDX: " +
					              std::string(1, btype->central_module_c) +
					              std::string(1, btype->server_type_c) +
					              " len=" + std::to_string(evlen));
				}
			}
		}
		else if (status == OMNIAPI_NOT_FOUND)
		{
			// No events available
			if (_sett.BdxPollIntervalUs > 0)
				std::this_thread::sleep_for(std::chrono::microseconds(_sett.BdxPollIntervalUs));
			else
				std::this_thread::yield();
		}
		else if (status == OMNIAPI_NOT_LOGGED_IN)
		{
			KT01_LOG_ERROR(__CLASS__, "Forced logout detected!");
			_active.store(false);
			break;
		}
		else if (status == OMNIAPI_OVERFLOW)
		{
			KT01_LOG_ERROR(__CLASS__, "Event buffer overflow - broadcasts may have been lost");
		}
	}
}

void OmnetBdxThread::ParseBO5(const char* buf, size_t len)
{
	if (len < sizeof(broadcast_hdr_t))
		return;

	const broadcast_hdr_t* hdr = (const broadcast_hdr_t*)buf;
	uint16_t items;
	PUTSHORT(items, hdr->items_n);

	if (_sett.DebugAppMsgs)
	{
		KT01_LOG_INFO(__CLASS__, "BO5 received: len=" + std::to_string(len) +
		              " items=" + std::to_string(items));
	}

	// Walk the VIM sub-items
	const uint8_t* ptr = (const uint8_t*)buf + sizeof(broadcast_hdr_t);
	const uint8_t* end = (const uint8_t*)buf + len;

	// Accumulate order info across sub-items
	KTN::OrderPod ord;
	memset(&ord, 0, sizeof(ord));
	bool hasOrderInfo = false;
	bool hasReturnInfo = false;
	bool hasChangeInfo = false;
	int32_t transAck = 0;
	uint8_t changeReason = 0;

	for (uint16_t i = 0; i < items && ptr < end; ++i)
	{
		if (ptr + sizeof(sub_item_hdr_t) > end)
			break;

		const sub_item_hdr_t* subhdr = (const sub_item_hdr_t*)ptr;
		uint16_t namedStruct;
		uint16_t subSize;
		PUTSHORT(namedStruct, subhdr->named_struct_n);
		PUTSHORT(subSize, subhdr->size_n);

		const uint8_t* data = ptr + sizeof(sub_item_hdr_t);

		if (_sett.DebugRecvBytes)
		{
			KT01_LOG_INFO(__CLASS__, "  BO5 sub[" + std::to_string(i) + "]: struct=" +
			              std::to_string(namedStruct) + " size=" + std::to_string(subSize));
		}

		switch (namedStruct)
		{
		case 34901: // segment_instance_number — skip
			break;

		case 34904: // order_return_info — tx ack + order number
		{
			if (data + sizeof(order_return_info_t) <= end)
			{
				const order_return_info_t* info = (const order_return_info_t*)data;
				PUTLONG(transAck, info->trans_ack_i);
				memcpy(&ord.exchordid, &info->order_number_u, sizeof(quad_word));
				hasReturnInfo = true;
			}
			break;
		}

		case 34005: // hv_order_trans — new order
		{
			if (data + sizeof(hv_order_trans_t) <= end)
			{
				const hv_order_trans_t* ot = (const hv_order_trans_t*)data;

				// Extract order fields
				int32_t price;
				PUTLONG(price, ot->order_var.premium_i);
				int64_t qty = SwapInt64(&ot->order_var.mp_quantity_i);

				ord.price = akl::Price::FromShifted((int64_t)price);
				ord.quantity = (uint32_t)qty;
				ord.leavesqty = (uint32_t)qty;
				ord.OrdSide = (ot->order_var.bid_or_ask_c == 1) ? KOrderSide::BUY : KOrderSide::SELL;
				ord.OrdStatus = KOrderStatus::NEW;
				ord.OrdState = KOrderState::WORKING;

				// Extract order request ID from ex_client
				ord.orderReqId = strtoull(ot->order_var.ex_client_s, nullptr, 10);

				// Extract secid (orderbook_id) from series via OseSecMaster
				series_t s = ot->series;
				PUTSHORT(s.commodity_n, s.commodity_n);
				PUTSHORT(s.expiration_date_n, s.expiration_date_n);
				uint64_t seriesKey = ((uint64_t)s.country_c << 56) |
				                     ((uint64_t)s.market_c << 48) |
				                     ((uint64_t)s.instrument_group_c << 40) |
				                     ((uint64_t)s.modifier_c << 32) |
				                     ((uint64_t)s.commodity_n << 16) |
				                     ((uint64_t)s.expiration_date_n);
				ord.secid = OseSecMaster::instance().GetOrderbookIdBySeriesKey(seriesKey);

				hasOrderInfo = true;
			}
			break;
		}

		case 34010: // hv_alter_trans — order altered
		{
			if (data + sizeof(hv_alter_trans_t) <= end)
			{
				const hv_alter_trans_t* at = (const hv_alter_trans_t*)data;

				int32_t price;
				PUTLONG(price, at->order_var.premium_i);
				int64_t qty = SwapInt64(&at->order_var.mp_quantity_i);
				int64_t balance = SwapInt64(&at->balance_quantity_i);

				ord.price = akl::Price::FromShifted((int64_t)price);
				ord.quantity = (uint32_t)qty;
				ord.leavesqty = (uint32_t)balance;
				ord.OrdSide = (at->order_var.bid_or_ask_c == 1) ? KOrderSide::BUY : KOrderSide::SELL;
				ord.OrdStatus = KOrderStatus::MODIFIED;
				ord.OrdState = KOrderState::WORKING;
				memcpy(&ord.exchordid, &at->order_number_u, sizeof(quad_word));
				ord.orderReqId = strtoull(at->order_var.ex_client_s, nullptr, 10);

				hasOrderInfo = true;
			}
			break;
		}

		case 34011: // delete_trans — order deleted/cancelled
		{
			if (data + sizeof(delete_trans_t) <= end)
			{
				const delete_trans_t* dt = (const delete_trans_t*)data;
				memcpy(&ord.exchordid, &dt->order_number_u, sizeof(quad_word));
				ord.OrdSide = (dt->bid_or_ask_c == 1) ? KOrderSide::BUY : KOrderSide::SELL;
				ord.OrdStatus = KOrderStatus::CANCELED;
				ord.OrdState = KOrderState::COMPLETE;
				ord.leavesqty = 0;

				hasOrderInfo = true;
			}
			break;
		}

		case 34902: // order_change_combined — quantity change (fill/execution)
		{
			if (data + sizeof(order_change_combined_t) <= end)
			{
				const order_change_combined_t* oc = (const order_change_combined_t*)data;
				int64_t newQty = SwapInt64(&oc->mp_quantity_i);
				int64_t totalVol = SwapInt64(&oc->total_volume_i);
				changeReason = oc->change_reason_c;

				// change_reason: 1=new, 2=change, 3=delete, 4=fill, 5=partial fill
				if (changeReason == 4 || changeReason == 5)
				{
					// This is a fill/execution
					ord.lastqty = ord.quantity - (uint32_t)newQty; // executed qty
					ord.leavesqty = (uint32_t)newQty;
					ord.fillqty = ord.quantity - (uint32_t)newQty;

					if (newQty == 0)
					{
						ord.OrdStatus = KOrderStatus::FILLED;
						ord.OrdState = KOrderState::COMPLETE;
					}
					else
					{
						ord.OrdStatus = KOrderStatus::PARTIALLY_FILLED;
						ord.OrdState = KOrderState::WORKING;
					}
				}

				hasChangeInfo = true;
			}
			break;
		}

		case 34009: // alter_trans — simple alter
		{
			if (data + sizeof(alter_trans_t) <= end)
			{
				const alter_trans_t* at = (const alter_trans_t*)data;
				int32_t price;
				PUTLONG(price, at->order_var.premium_i);
				int64_t qty = SwapInt64(&at->order_var.mp_quantity_i);

				ord.price = akl::Price::FromShifted((int64_t)price);
				ord.quantity = (uint32_t)qty;
				ord.OrdSide = (at->order_var.bid_or_ask_c == 1) ? KOrderSide::BUY : KOrderSide::SELL;
				ord.OrdStatus = KOrderStatus::MODIFIED;
				ord.OrdState = KOrderState::WORKING;
				memcpy(&ord.exchordid, &at->order_number_u, sizeof(quad_word));

				hasOrderInfo = true;
			}
			break;
		}

		default:
			// Skip unknown sub-items
			break;
		}

		// Advance to next sub-item
		ptr += subSize;
	}

	// Check transaction ack for rejection
	if (hasReturnInfo && transAck < 0)
	{
		ord.OrdStatus = KOrderStatus::REJECTED;
		ord.OrdState = KOrderState::COMPLETE;
		ord.rejreason = (uint16_t)(-transAck);
		snprintf(ord.text, sizeof(ord.text), "OMnet reject: %d", transAck);
		hasOrderInfo = true;
	}

	// Push to response queue if we have meaningful order info
	if (hasOrderInfo || hasChangeInfo)
	{
		ord.recvTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
		    std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		_responseQueue.enqueue(ord);

		if (_sett.DebugAppMsgs)
		{
			KT01_LOG_INFO(__CLASS__, "BO5 → response: status=" + std::to_string((int)ord.OrdStatus) +
			              " side=" + std::to_string((int)ord.OrdSide) +
			              " qty=" + std::to_string(ord.quantity) +
			              " leaves=" + std::to_string(ord.leavesqty) +
			              " changeReason=" + std::to_string(changeReason));
		}
	}
}

void OmnetBdxThread::ParseNetworkEvent(const char* buf, size_t len)
{
	if (len < sizeof(broadcast_type_t))
		return;

	broadcast_type_t* btype = (broadcast_type_t*)buf;
	uint16_t txnum;
	PUTSHORT(txnum, btype->transaction_number_n);

	switch (txnum)
	{
	case 1: // BN1 - Network status
		KT01_LOG_INFO(__CLASS__, "BN1: Network status event");
		break;
	case 4: // BN4 - Forced logout
		KT01_LOG_ERROR(__CLASS__, "BN4: Network gateway forced logout!");
		_active.store(false);
		break;
	default:
		if (_sett.DebugSession)
			KT01_LOG_INFO(__CLASS__, "BN" + std::to_string(txnum) + " event");
		break;
	}
}

} // namespace KTN::OSE
