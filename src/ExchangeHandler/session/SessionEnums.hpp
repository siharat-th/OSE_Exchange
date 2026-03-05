//============================================================================
// Name        	: SessionEnums.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 15, 2023 9:53:30 AM
//============================================================================

#ifndef SRC_SESSION_SESSIONENUMS_HPP_
#define SRC_SESSION_SESSIONENUMS_HPP_

#include <string>

namespace KTN{
namespace Session{

struct EnumSessionState
{
    enum Enum : int
	{
        /// Session is disconnected.
        Disconnected,

		StartingLoginProcess,

        /// Session is waiting for the [Negotiation Response](https://www.cmegroup.com/confluence/display/EPICSANDBOX/Negotiation+Response) message from the exchange.
        AwaitNegotiationResponse,

		Negotiated,

        /// Session is waiting for the [Establishment Acknowledgment](https://www.cmegroup.com/confluence/display/EPICSANDBOX/Establishment+Acknowledgment) message from the exchange.
        AwaitEstablishmentAck,

        /// Session is waiting for the confirming [Terminate](https://www.cmegroup.com/confluence/display/EPICSANDBOX/Terminate) message from the exchange.
        AwaitConfirmingTerminate,

        /// Session is fully established.
        Established,

        /// Session is waiting for response and processing of Retransmission Request
        WaitingForRetransmission,

		/// Session is terminating the connection
		Terminating
    };

    static std::string toString(int state)
    	{
    		switch(state) {
    			case Disconnected:
    				return "Disconnected";

    			case AwaitNegotiationResponse:
    				return "AwaitNegotiationResponse";

    			case AwaitEstablishmentAck:
    				return "AwaitEstablishmentAck";

    			case AwaitConfirmingTerminate:
    				return "AwaitConfirmingTerminate";

    			case Established:
					return "Established";

    			case Negotiated:
    				return "Negotiated";

				case WaitingForRetransmission:
					return "WaitingForRetransmission";

				case Terminating:
					return "Terminating";

    			default:
    				return "INVALID";
    		}
    	}

};


}}

#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3SESSIONENUMS_HPP_ */
