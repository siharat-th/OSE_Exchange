//============================================================================
// Name        	: SessErrors.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 26, 2023 12:35:30 PM
//============================================================================

#include "IL3ErrorCodes.hpp"

using namespace KTN::CME::IL3;

const std::map<int, std::string> ErrorCodes::sessionErrorMessages = {
	{0, "HMACNotAuthenticated: failed authentication because identity is not recognized"},
	{1, "HMACNotAvailable: HMAC component is not responding (5sec)"},
	{3, "InvalidTimestamp: Timestamp is not greater than the one last used or value sent by the client is out of acceptable range (MIN, MAX)"},
	{4, "RequiredHMACSignatureMissing: empty bytes in HMACSignature field"},
	{5, "RequiredAccessKeyIDMissing: empty bytes in AccessKeyID field"},
	{6, "RequiredSessionMissing: empty bytes in Session field"},
	{7, "RequiredFirmMissing: empty bytes in Firm field"},
	{9, "RequiredRequestTimestampMissing: null value in RequestTimestamp field"},
	{10, "SessionBlocked: session and firm are not authorized for this port"},
	{11, "InvalidKeepAliveInterval: value is out of acceptable range (MIN, MAX)"},
	{12, "InvalidAccessKeyID: contains non-printable ASCII character"},
	{13, "InvalidSession: contains non-printable ASCII character"},
	{14, "InvalidFirm: contains non-printable ASCII character"},
	{15, "Volume Controls - exceeding TPS limit as defined for volume controls (reject action)"},
	{16, "SplitMessageRejected - Messages queued due to split message penalty being rejected because of logout or disconnect"},
	{17, "SplitMessageQueue - Reached threshold of messages queued due to split message penalty"},
	{18, "RequiredTradingSystemNameMissing: empty bytes in TradingSystemName"},
	{19, "RequiredTradingSystemVersionMissing: empty bytes in TradingSystemVersion"},
	{20, "RequiredTradingSystemVendorMissing: empty bytes in TradingSystemVendor"},
	{21, "RequiredKeepAliveIntervalMissing: null value in KeepAliveInterval field"},
	{23, "InvalidTradingSystemName: contains non-printable ASCII character"},
	{24, "InvalidTradingSystemVersion: contains non-printable ASCII character"},
	{25, "InvalidTradingSystemVendor: contains non-printable ASCII character"},
	{27, "DesignatedBackup - Using Designated backup before designated primary not allowed"},
	{28, "NegotiateNotAllowed - Not allowed to negotiate on backup when established on primary"}
};

std::string ErrorCodes::getSessionErrorMessage(int errorCode) {
    if (sessionErrorMessages.find(errorCode) != sessionErrorMessages.end()) {
        return "Session Error Message:" + sessionErrorMessages.at(errorCode) + "(" + std::to_string(errorCode) + ")";
    } else {
        return "Unknown Session Error code: " + std::to_string(errorCode);
    }
}


const std::map<int, std::string> ErrorCodes::busRejectErrorMessages = {
    {0, "Other"},
    {1, "Unknown ID (unknown PartyDetailsListReqID being referenced in business message)"},
    {2, "Unknown Security"},
    {3, "Unsupported Message Type (for example using messages specific to cash markets for futures)"},
    {5, "Required Field Missing"},
    {6, "Not Authorized"},
    {8, "Throttle Limit Exceeded (volume controls)"},
    {100, "Value is incorrect (out of range) for this tag (for example using a value outside defined range of integer)"},
    {101, "Incorrect data format for value (for example using ASCII character outside 32-126 in string field)"},
    {102, "Rejected due to Kill Switch"},
    {103, "Rejected due to Risk Management API"},
    {104, "Rejected due to Globex Credit Controls"},
    {105, "Not Authorized to trade Deliverable Swap Futures"},
    {106, "Not Authorized to trade Interest Rate Swap Futures"},
    {107, "Rejected due to Inline Credit Controls"},
    {108, "Invalid PartyDetailsListReqID (reusing already existing PartyDetailsListReqID while creating Party Details Definition Request)"},
    {109, "Incoming message could not be decoded"},
    {110, "Same repeating group entry appears more than once"},
    {111, "Exceeded maximum number of allowable Party Details Definition Requests"},
    {112, "Technical Error in Registering Party Details"},
    {113, "Rejected due to Cross Venue Risk"},
    {114, "Order Status Not Available"},
    {115, "Enum Not Supported"},
    {116, "Order Status Not Found"},
    {117, "Mass Order Status Cannot be Completed"},
    {118, "Exceeded Maximum Number of Allowable RequestingPartyID's in Party Details List Request"},
    {119, "Duplicate Order Threshold Exceeded"},
    {120, "On-Demand Message Rejected Due to Corresponding PartyDetailsDefinitionRequest Being Rejected"},
    {121, "Message Rejected Since PartyDetailsListReqID Does Not Match with Corresponding PartyDetailsDefinitionRequest as part of On-demand Message"},
    {122, "Party Details Definition Request sent to MSGW Being Rejected Due to Corresponding Business Message Being Rejected"},
    {123, "Another Message Sent in Between Party Details Definition Request and Business Message when using On-demand administrative information"},
    {124, "Cannot Have More Than One In-Flight Mass Order Status Request in Progress"},
    {125, "Exceeded Maximum Number of In-Flight Order Status Requests"},
    {126, "Cannot Have More Than One In-Flight Party Details List Request in Progress"},
    {127, "Party Details List Request is Missing Requesting Party ID and Party ID"},
    {128, "Party Details List Request cannot contain both RequestingPartyID and PartyID"},
    {129, "Party Details Definition Request Being Rejected Since Another Message was sent in Between On-Demand Message"},
    {130, "Buy Side Firm ID Does Not Match Sell Side Firm ID in New Order Cross"},
    {131, "Message Type Not Supported on Backup Instance"},
    {132, "New Order Cross Does Not Contain Buy Side Followed by Sell Side"},
    {133, "Duplicate ClOrdID: X not allowed"},
    {134, "Value Provided Does Not Match With Original Trade (EU Bilateral Reject)"},
    {135, "Combination of Order ID & SecExecID not found (EU Bilateral Reject)"},
    {136, "Non Zero Limit Price Not Allowed for eFix Instruments"},
    {137, "Messages queued due to latency floor being rejected because of session terminate or disconnect prior to release from latency floor"},
    {138, "Message rejected due to internal technical error"},
    {139, "Max Order Size Exceeded"},
    {140, "Not Authorized to Submit GFS Orders (Only FAK/FOK Allowed)"},
    {141, "Invalid On-Behalf Information if less than 6 characters (i.e. invalid tag 9937-OrigOrderUser)"},
    {142, "Invalid On-Behalf Information if both sender comp ID's are the same value (i.e. tag 9937-OrigOrderUser cannot be same as sending session and firm)"},
    {143, "Invalid On-Behalf Information if both GFID's are different (i.e. GFID does not match between tag 9937- OrigOrderUser and PartyDetailRole=1)"},
    {144, "Quotes cannot be blocked for instrument or quote set on-behalf"},
    {145, "Quote rejected: Blocked from quoting this instrument 'cannot quote group: xx'"},
    {146, "Cancel On Behalf feature not available"},
    {147, "Not Authorized to cancel for OrigOrderUser: xxxxxx"},
    {148, "CancelOnBehalf not allowed on iLink2 order or quote"}
};

std::string ErrorCodes::getBusRejectErrorMessage(int errorCode) {
    if (busRejectErrorMessages.find(errorCode) != busRejectErrorMessages.end()) {
        return  "Business Reject: " + busRejectErrorMessages.at(errorCode) + "(" +  std::to_string(errorCode) + ")";
    } else {
        return "Unknown Business Reject Error code: " + std::to_string(errorCode);
    }
}


const std::map<int, std::string> ErrorCodes::terminateCodes = {
    {0, "Finished: session is being terminated without finalization"},
    {2, "Unnegotiated: Sending any message when session has not been Negotiated"},
    {3, "NotEstablished: Sending any message (except Negotiate) when session has not been established"},
    {4, "AlreadyNegotiated: NegotiationResponse was already sent; Negotiate was redundant"},
    {5, "NegotiationInProgress: Previous Negotiate still being processed; Wait for NegotiationResponse or timeout"},
    {6, "AlreadyEstablished: EstablishmentAck was already sent; Establish was redundant"},
    {7, "EstablishInProgress: Previous Establish still being processed; Wait for EstablishmentAck or timeout"},
    {8, "CMEAdministeredPortClosure: due to invalid number of Negotiate/Establish attempts being exceeded"},
    {9, "Volume Controls - exceeding TPS limit as defined for volume controls (logout action)"},
    {10, "InvalidNextSeqNo - value is not greater than the one last used for same UUID or value sent by the client is out of acceptable range (MIN, MAX)"},
    {11, "InvalidMsgSeqNo - value is lower than the last one used for the same UUID or value is not initialized to 1 at the beginning of the week or value sent by the client is out of acceptable range (MIN, MAX)"},
    {12, "InvalidLastSeqNo - value is lower than the last one used for the same UUID or value sent by the client is out of acceptable range (MIN, MAX)"},
    {13, "InvalidUUID: UUID value does not match current UUID or value sent by the client is out of acceptable range (MIN, MAX)"},
    {14, "InvalidTimestamp: Timestamp value does not match with RequestTimestamp sent by CME or value sent by the client is out of acceptable range (MIN, MAX)"},
    {15, "RequiredUUIDMisssing: null value in UUID field"},
    {16, "RequiredRequestTimestampMissing: null value in RequestTimestamp field"},
    {17, "RequiredCodeMisssing: null value in Code field"},
    {18, "InvalidSOFH: Invalid message length or invalid encoding type specified"},
    {19, "DecodingError: Incoming message could not be decoded"},
    {20, "KeepAliveIntervalLapsed: KeepAliveInterval has lapsed without any response so terminating session"},
    {21, "RequiredNextSeqNoMissing: null value in NextSeqNo field"},
    {22, "RequiredKeepAliveIntervalLapsedMissing: null value in KeepAliveIntervalLapsed field"},
    {23, "Non-Negotiate/Establish message received when not Negotiated/Established"},
    {24, "TerminateInProgress: Previous Terminate still being processed; Wait for Terminate or timeout"},
    {25, "Other: any other error condition not mentioned above"},
    {26, "DisconnectFromPrimary: Backup session will be terminated as well"},
    {27, "TerminatedFromPrimary: Backup session will be terminated as well"},
    {28, "RejectSplitMsgsQueued: Messages queued due to split message penalty being rejected because of terminate or disconnect"},
    {29, "ExceededMaxNoOfSplitMsgsQueued: Reached threshold of messages queued due to split message penalty"}
};

std::string ErrorCodes::getTerminateMessage(int errorCode) {
    if (terminateCodes.find(errorCode) != terminateCodes.end()) {
        return "Termination: " + terminateCodes.at(errorCode) + "(" + std::to_string(errorCode) + ")";
    } else {
        return "Unknown Termination Code: " + std::to_string(errorCode);
    }
}

const std::map<int, std::string> ErrorCodes::orderRejectErrorCodes = {
    {3, "Orders may not be entered while the market is closed"},
    {1003, "Orders may not be entered while the market is paused"},
    {1011, "FIX field incorrect"},
    {1012, "Price must be greater than zero"},
    {1013, "Invalid order qualifier"},
    {1014, "The user is not authorized to trade"},
    {2013, "Market price orders not supported by opposite limit"},
    {2019, "Order's GTD Expire Date is before the current (or next, if not currently in a session) trading session end date"},
    {7000, "Order rejected"},
    {2046, "Disclosed Quantity cannot be greater than total or remaining qty"},
    {2047, "Order contract is unknown"},
    {2051, "The Order was submitted with a different side than the requesting Cancel"},
    {2058, "Stop price maxi-mini must be greater than or equal to trigger price"},
    {2059, "Stop price maxi-mini must be smaller than or equal to trigger price"},
    {2060, "Sell order stop price must be below the last trade price"},
    {2061, "Buy order stop price must be above the last trade price"},
    {2115, "Order quantity is outside of the allowable range"},
    {2130, "Order type not permitted while the market is in Post Close/Pre-Open (PCP)"},
    {2137, "Order price is outside the limits"},
    {2179, "Order price is outside bands"},
    {2311, "Order type not permitted for group"},
    {2500, "Instrument has a request for cross in progress"},
    {2501, "Order Quantity too low"},
    {4999, "Order not allowed on Instruments without a valid Value Date"},
    {7027, "Order type not permitted while the market is reserved"},
    {7028, "Order session date is in the past"},
    {7029, "Orders may not be entered while the market is forbidden"},
    {7613, "Disclosed quantity cannot be smaller than the minimum quantity"},
    {7614, "Self Match Prevention ID is not associated with the given Firm ID"}
};

std::string ErrorCodes::getOrderRejectMessage(int errorCode) {
	if (orderRejectErrorCodes.find(errorCode) != orderRejectErrorCodes.end()) {
		return orderRejectErrorCodes.at(errorCode);
	} else {
		return "Unknown OrderReject error code: " + std::to_string(errorCode);
	}
}

const std::map<int, std::string> ErrorCodes::cxlReplaceRejectErrorCodes = {
    {3, "Orders may not be entered while the market is closed"},
    {1003, "Orders may not be entered while the market is paused"},
    {1012, "Price must be greater than zero"},
    {1013, "Invalid order qualifier"},
    {1014, "The user is not authorized to trade"},
    {2013, "Market price orders not supported by opposite limit"},
    {2019, "Order's GTD Expire Date is before the current (or next, if not currently in a session) trading session end date"},
    {2045, "This order is not on the book"},
    {2046, "Disclosed Quantity cannot be greater than total or remaining qty"},
    {2047, "Order contract is unknown"},
    {2048, "The order was submitted with a different SenderCompID than the requesting cancel"},
    {2058, "Stop price max-min must be greater than or equal to trigger price"},
    {2059, "Stop price max-min must be smaller than or equal to trigger price"},
    {2060, "Sell order stop price must be below the last trade price"},
    {2061, "Buy order stop price must be above the last trade price"},
    {2100, "The modify was submitted on a different product than the original order"},
    {2101, "Attempt to modify an order with a different in-flight-fill mitigation status than the first modification"},
    {2102, "Attempt to modify an order with a different SenderCompID than the original order"},
    {2115, "Order quantity is outside of the allowable range"},
    {2130, "Order type not permitted while the market is in Post Close/Pre-Open (PCP)"},
    {2137, "Order price is outside the limits"},
    {2179, "Order price is outside bands"},
    {2311, "Order type not permitted for group"},
    {2500, "Instrument has a request for cross in progress"},
    {2501, "Order Quantity too low"},
    {5102, "Modification of orders from Iceberg to Non-Iceberg & vice-versa is not allowed"},
    {7000, "Order rejected"},
    {7016, "This order is not on the book"},
    {7024, "Order cannot be modified or canceled while the market is in No Cancel"},
    {7027, "Order type not permitted while the market is reserved"},
    {7028, "Order session date is in the past"},
    {7029, "Orders may not be entered while the market is forbidden"},
    {7613, "Disclosed quantity cannot be smaller than the minimum quantity"},
    {7614, "Self Match Prevention ID is not associated with the given Firm ID"}
};

std::string ErrorCodes::getCxlReplaceRejectMessage(int errorCode) {
	if (cxlReplaceRejectErrorCodes.find(errorCode) != cxlReplaceRejectErrorCodes.end()) {
		return cxlReplaceRejectErrorCodes.at(errorCode);
	} else {
		return "Unknown CxlReplReject error code: " + std::to_string(errorCode);
	}
}

const std::map<int, std::string> ErrorCodes::cxlRejectErrorCodes = {
    {1003, "Orders may not be canceled while the market is closed"},
    {1003, "Orders may not be canceled while the market is paused"},
    {2045, "This order is not on the book"},
    {2048, "The order was submitted with a different SenderCompID than the requesting cancel"},
    {2051, "The order was submitted with a different side than the requesting Cancel"},
    {7024, "Order cannot be modified or canceled while the market is in No Cancel"},
    {7027, "Order type not permitted while the market is reserved"},
    {7029, "Orders may not be entered while the market is forbidden"}
};

std::string ErrorCodes::getCxlRejectMessage(int errorCode) {
	if (cxlRejectErrorCodes.find(errorCode) != cxlRejectErrorCodes.end()) {
		return cxlRejectErrorCodes.at(errorCode);
	} else {
		return "Unknown CxlReject error code: " + std::to_string(errorCode);
	}
}

const std::map<int, std::string> ErrorCodes::retransmitRejectCodes = {
    {0, "OutOfRange: FromSeqNo + Count is beyond the range of sequence numbers"},
    {1, "InvalidUUID: The specified UUID is unknown or is not authorized for the requester to access"},
    {2, "RequestLimitExceeded: The message count exceeds maximum retransmission size of single request"},
    {3, "RetransmitAlreadyInProgress: Prior Retransmit still in progress so cannot send a new one"},
    {4, "InvalidTimestamp: Value sent by the client in RequestTimestamp is out of acceptable range (MIN, MAX)"},
    {5, "InvalidFromSeqNo: Value sent by the client in FromSeqNo is out of acceptable range (MIN, MAX)"},
    {6, "RequiredUUIDMissing: null value in UUID field"},
    {7, "RequiredRequestTimestampMissing: null value in RequestTimestamp field"},
    {8, "RequiredFromSeqNoMissing: null value in FromSeqNo field"},
    {9, "RequiredMsgCountMissing: null value in MsgCount field"}
};

std::string ErrorCodes::getRetransmitRejectMessage(int errorCode) {
	if (retransmitRejectCodes.find(errorCode) != retransmitRejectCodes.end()) {
		return retransmitRejectCodes.at(errorCode);
	} else {
		return "Unknown error code: " + std::to_string(errorCode);
	}
}
