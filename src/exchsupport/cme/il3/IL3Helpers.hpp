/*
 * IL3Helpers.h
 *
 *  Created on: Jan 24, 2025
 *      Author: rocky
 */

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3HELPERS_H_
#define SRC_EXCHSUPPORT_CME_IL3_IL3HELPERS_H_

#include <iostream>
#include <unordered_map>

#include <ExchangeHandler/ExchStructs.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>
#include <exchsupport/cme/il3/IL3Messages2.hpp>
#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
using namespace KTN::NET;

#include <Orders/OrderEnumsV2.hpp>

namespace KTN{
namespace CME{
namespace IL3{

using namespace KTN::ORD;

class IL3Helpers {
public:

	enum MsgLengths : int
	{
		MAX_SESS_MSG_ID = 513,
		SOFH_SIZE = 4,
		SBE_HEADER_SIZE = 8,
		MSG_HDR_SIZE = 12,
		DATA_FIELD_LEN = 2,
		GROUP_SIZE_LEN = 3,
		MSGTYPE_NEW = 514,
		MSGTYPE_MOD = 515,
		MSGTYPE_CXL = 516,
		LEN_NEW = 132,
		LEN_MOD = 133,
		LEN_CXL = 88
	};

	static uint32_t extractSeqNum(const void* buffer) {
	    // Cast the buffer to a pointer to uint32_t and dereference it.
	    // Only do this if you know the buffer is correctly aligned!
	    return *reinterpret_cast<const uint32_t*>(buffer);
	}

    static std::string toString(EnumOrderStatus status) {
        static const std::unordered_map<EnumOrderStatus, std::string> statusMap = {
            {EnumOrderStatus_New, "New"},
            {EnumOrderStatus_PartiallyFilled, "Partially Filled"},
            {EnumOrderStatus_Filled, "Filled"},
            {EnumOrderStatus_Cancelled, "Cancelled"},
            {EnumOrderStatus_Replaced, "Replaced"},
            {EnumOrderStatus_Rejected, "Rejected"},
            {EnumOrderStatus_Expired, "Expired"},
            {EnumOrderStatus_Undefined, "Undefined"}
        };

        auto it = statusMap.find(status);
        return (it != statusMap.end()) ? it->second : "Unknown";
    }
    
    static KOrderStatus::Enum translate(EnumOrderStatus status) {
           static const std::unordered_map<EnumOrderStatus, KOrderStatus::Enum> statusMap = {
               {EnumOrderStatus_New, KOrderStatus::NEW},
               {EnumOrderStatus_PartiallyFilled, KOrderStatus::PARTIALLY_FILLED},
               {EnumOrderStatus_Filled, KOrderStatus::FILLED},
               {EnumOrderStatus_Cancelled, KOrderStatus::CANCELED},
               {EnumOrderStatus_Replaced, KOrderStatus::MODIFIED},
               {EnumOrderStatus_Rejected, KOrderStatus::REJECTED}
           };

           auto it = statusMap.find(status);
           return (it != statusMap.end()) ? it->second : KOrderStatus::UNKNOWN;
       }


    static int SkipGroupAndAdvance(uint8_t* buf, int msgbytes)
    {
    	auto nofills = reinterpret_cast<IL3::groupSize*>(buf);

    	buf += sizeof(IL3::groupSize);
    	msgbytes += sizeof(IL3::groupSize);

    	buf += (nofills->blockLength * nofills->numInGroup);
    	msgbytes += (nofills->blockLength * nofills->numInGroup);

    	return msgbytes;
    }
    /*
     * @brief: Convenience function to calculate message length for "live" fully formed messages.
     * Particularly helpful for messages with repeating groups
     * For those that don't simply return msg.buffer_length
     *
     *
     */
    static int GetLiveBytes(KTNBuf8t msg)
	{
    	int msgbytes = msg.block_length;

    	switch (msg.msgtype)
		{
			 case ExecutionReportTradeOutrightMsg::id:
			 {
				auto buf = msg.buffer;

				buf += ExecutionReportTradeOutrightMsg::blockLength;
				msgbytes = ExecutionReportTradeOutrightMsg::blockLength;
				//2 groups here; we pass msgbytes and increment and pass back
				int nbrgrps = 2;
				for (int i = 0; i < nbrgrps; i++)
					msgbytes = SkipGroupAndAdvance(buf, msgbytes);

				return msgbytes;

			 }break;

			 case ExecutionReportTradeSpreadMsg::id:
			 {
				auto buf = msg.buffer;

				buf += ExecutionReportTradeSpreadMsg::blockLength;
				msgbytes = ExecutionReportTradeSpreadMsg::blockLength;
				//3 groups here; we pass msgbytes and increment and pass back
				int nbrgrps = 3;
				for (int i = 0; i < nbrgrps; i++)
					msgbytes = SkipGroupAndAdvance(buf, msgbytes);

				return msgbytes;

			 }break;

			 case ExecutionReportTradeSpreadLegMsg::id:
			 {
				auto buf = msg.buffer;

				//auto msg = reinterpret_cast<ExecutionReportTradeSpreadLegMsg::BlockData*>(buf);
				buf += ExecutionReportTradeSpreadLegMsg::blockLength;
				msgbytes = ExecutionReportTradeSpreadLegMsg::blockLength;
				//2 groups here; we pass msgbytes and increment and pass back
				int nbrgrps = 2;
				for (int i = 0; i < nbrgrps; i++)
					msgbytes = SkipGroupAndAdvance(buf, msgbytes);

				return msgbytes;

			 }break;

			 case PartyDetailsDefinitionRequestAckMsg::id:
			 {
				msgbytes = PartyDetailsDefinitionRequestAckMsg::blockLength;
				msgbytes += GROUP_SIZE_LEN;
				msgbytes += GROUP_SIZE_LEN;

				msgbytes += 23; // first group: NoPartyDetails; always 1 group
				msgbytes += 2; // second groupt: NoTrdRegPublications; always 1 group
				return msgbytes;
			 } break;

			 case PartyDetailsListReportMsg::id:
			 {
				auto buf = msg.buffer;

				//auto msg = reinterpret_cast<PartyDetailsListReportMsg::BlockData*>(buf);
				buf += PartyDetailsListReportMsg::blockLength;
				msgbytes = PartyDetailsListReportMsg::blockLength;

				//this is not a fixed size, so we have to get it. we can prob make this all better.
				//auto noPartyDets = reinterpret_cast<IL3::groupSize*>(buf);
				buf += sizeof(IL3::groupSize);
				msgbytes += sizeof(IL3::groupSize);


				//2 groups here; we pass msgbytes and increment and pass back
				int nbrgrps = 1;
				for (int i = 0; i < nbrgrps; i++)
					msgbytes = SkipGroupAndAdvance(buf, msgbytes);

				return msgbytes;

			 }break;

			 case OrderMassActionReportMsg::id:
			 {
				 return msg.msg_length;
//				//we have to do a calculation here for msgbytes...
//				auto temp = reinterpret_cast<OrderMassActionReportMsg::BlockData*>(msg.buffer);
//				msgbytes = OrderMassActionReportMsg::blockLength;
//				temp += OrderMassActionReportMsg::blockLength;
//
//				auto noords = reinterpret_cast<IL3::groupSize*>(temp);
//				temp += sizeof(IL3::groupSize);
//				msgbytes += sizeof(IL3::groupSize);
//
//				uint8_t nbrords = noords->numInGroup;
//
//				cout << "[LiveBytes OrdMassAction] numInGroup=" << (int)nbrords << " msgbytes=" << msgbytes << endl;
//
//				if (nbrords > 0)
//				{
//					int AffOrdsBlockLen = 32;
//					msgbytes += (nbrords * AffOrdsBlockLen);
//					temp += (nbrords * AffOrdsBlockLen);
//
//					cout << "[LiveBytes OrdMassAction] numInGroup=" << nbrords << " msgbytes=" << msgbytes << endl;
//				}
//
//				cout << "[LiveBytes OrdMassAction] returning msgbytes=" << msgbytes << endl;
//				return msgbytes;
			 } break;

			 default:
				 //so here, we know we don't have a message with a repeating group. We can simply return
				 //the block length. This is DIFFERENT from the buffer_length.
				return msgbytes;
		}
	}
};


//end namespace;
}}}

#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3HELPERS_H_ */
