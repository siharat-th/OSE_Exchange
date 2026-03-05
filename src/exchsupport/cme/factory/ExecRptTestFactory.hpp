//============================================================================
// Name        	: ExecRptTestFactory.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 20, 2023 4:11:32 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_EXECRPTTESTFACTORY_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_EXECRPTTESTFACTORY_HPP_
#pragma once

#include <cassert>
#include <memory>
#include <KT01/Core/Settings.hpp>

#include <exchsupport/cme/prices/PriceConverters.hpp>
#include <exchsupport/cme/settings/CmeSessionSettings.hpp>

#include <exchsupport/cme/il3/IL3Types.hpp>
#include <exchsupport/cme/il3/IL3Composites.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>

#include <exchsupport/cme/il3/IL3Headers.hpp>
#include <exchsupport/cme/il3/IL3Messages2.hpp>
#include <exchsupport/cme/il3/IL3AppHelpers.hpp>

#include <KT01/Net/byte_writer.hpp>
#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
using namespace KTN::NET;

#include <KT01/Net/helpers.hpp>

#include <KT01/Core/PerformanceTracker.hpp>

#include <Orders/Order.hpp>
#include <Orders/IdGen.h>
using namespace KTN;

namespace KTN
{
	namespace CME
	{
		namespace IL3
		{
			/**
			 * @brief This class is used to generate CME IL3 application messages and provide pre-filled templates for them.
			 *
			 */
			class TestFactory
			{
			private:
				using byte_ptr = uint8_t *;

			public:
			static std::pair<byte_ptr,int> BuildExecRptElimination(
				const ExecutionReportNewMsg::BlockData* msg,
				int segid)
			{
				// 1) allocate and writer for SOH+SBE headers
				constexpr uint32_t CAP = 1600;
				char* buffer = new char[CAP];
				uint32_t bytesWritten = 0;
				ByteWriter writer(buffer, CAP, &bytesWritten);
			
				// 2) payload pointer
				auto* bd = reinterpret_cast<ExecutionReportEliminationMsg::BlockData*>(writer.cur());
			
				// 3) zero & fill every field
				std::memset(bd, 0, sizeof(*bd));
				bd->SeqNum               = msg->SeqNum;
				bd->UUID                 = msg->UUID;
				std::strncpy(bd->ExecID,   msg->ExecID, sizeof(bd->ExecID));
				std::strncpy(bd->SenderID, msg->SenderID, sizeof(bd->SenderID));
				std::strncpy(bd->ClOrdID,  msg->ClOrdID, sizeof(bd->ClOrdID));
				bd->PartyDetailsListReqID = msg->PartyDetailsListReqID;
				bd->OrderID               = msg->OrderID;
				bd->Price                 = msg->Price;
				bd->StopPx                = msg->StopPx;
				bd->TransactTime          = msg->TransactTime;
				bd->SendingTimeEpoch      = msg->SendingTimeEpoch;
				bd->OrderRequestID        = msg->OrderRequestID;
				bd->CrossID               = msg->CrossID;
				bd->HostCrossID           = msg->HostCrossID;
				std::strncpy(bd->Location, msg->Location, sizeof(bd->Location));
				bd->SecurityID            = msg->SecurityID;
				bd->CumQty                = 0;
				
				bd->OrderQty              = msg->OrderQty;
				bd->MinQty                = msg->MinQty;
				bd->DisplayQty            = msg->DisplayQty;
				bd->ExpireDate            = msg->ExpireDate;
				bd->OrdType               = msg->OrdType;
				bd->Side                  = msg->Side;
				bd->TimeInForce           = msg->TimeInForce;
				bd->ManualOrderIndicator  = msg->ManualOrderIndicator;
				bd->PossRetransFlag       = msg->PossRetransFlag;
				bd->CrossType             = msg->CrossType;
				bd->SplitMsg              = msg->SplitMsg;
				bd->ExecutionMode         = msg->ExecutionMode;
				bd->LiquidityFlag         = msg->LiquidityFlag;
				bd->ManagedOrder          = msg->ManagedOrder;
				bd->ShortSaleType         = msg->ShortSaleType;
				bd->DiscretionPrice       = msg->DiscretionPrice;
				bd->ReservationPrice      = msg->ReservationPrice;
				bd->PriorityIndicator     = msg->PriorityIndicator;
			
				// 4) cast and return fixed length
				byte_ptr ubuf = reinterpret_cast<byte_ptr>(buffer);
				return { ubuf, static_cast<int>(ExecutionReportEliminationMsg::blockLength) };
			}
			};

		}
	}
}
#endif /* SRC_EXCHSUPPORT_CME_IL3_APPMESSAGEFACTORY_HPP_ */
