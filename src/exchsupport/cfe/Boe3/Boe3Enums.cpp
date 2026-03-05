#include <exchsupport/cfe/Boe3/Boe3Enums.hpp>
#include <sstream>

namespace KTN {
namespace CFE {
namespace Boe3 {

//------------------------------------------------------
// SessMsgType toString Implementation
//------------------------------------------------------
std::string toString(SessMsgType value)
{
    switch (value)
    {
        case SessMsgType::LoginRequest:    return "LoginRequest";
        case SessMsgType::LogoutRequest:   return "LogoutRequest";
        case SessMsgType::ClientHeartbeat: return "ClientHeartbeat";
        case SessMsgType::LoginResponse:   return "LoginResponse";
        case SessMsgType::ReplayComplete:  return "ReplayComplete";
        case SessMsgType::LogoutResponse:  return "LogoutResponse";
        case SessMsgType::ServerHeartbeat: return "ServerHeartbeat";
        default:                       return "Unknown SessMsgType";
    }
}

//------------------------------------------------------
// AppMsgType toString Implementation
//------------------------------------------------------
std::string toString(AppMsgType value)
{
    switch (value)
    {
        //case AppMsgType::NewOrder:                return "NewOrder";
        case AppMsgType::ModifyOrder:             return "ModifyOrder";
        case AppMsgType::CancelOrder:             return "CancelOrder";
        case AppMsgType::MassCancelOrder:         return "MassCancelOrder";
        case AppMsgType::PurgeOrders:             return "PurgeOrders";
        case AppMsgType::QuoteUpdate:             return "QuoteUpdate";
        case AppMsgType::ResetRisk:               return "ResetRisk";
        case AppMsgType::NewOrderV2:              return "NewOrderV2";
        case AppMsgType::OrderAck:                return "OrderAck";
        case AppMsgType::OrderRejected:           return "OrderRejected";
        case AppMsgType::OrderModified:           return "OrderModified";
        case AppMsgType::ModifyRejected:          return "ModifyRejected";
        case AppMsgType::OrderExecution:          return "OrderExecution";
        case AppMsgType::OrderCancelled:          return "OrderCancelled";
        case AppMsgType::CancelRejected:          return "CancelRejected";
        case AppMsgType::MassCancelAck:           return "MassCancelAck";
        case AppMsgType::MassCancelReject:        return "MassCancelReject";
        case AppMsgType::PurgeAck:                return "PurgeAck";
        case AppMsgType::PurgeReject:             return "PurgeReject";
        case AppMsgType::TradeCancelCorrect:      return "TradeCancelCorrect";
        case AppMsgType::TASRestatement:          return "TASRestatement";
        case AppMsgType::VarianceRestatement:     return "VarianceRestatement";
        case AppMsgType::QuoteUpdateAck:          return "QuoteUpdateAck";
        case AppMsgType::QuoteUpdateReject:       return "QuoteUpdateReject";
        case AppMsgType::QuoteRestated:           return "QuoteRestated";
        case AppMsgType::QuoteExecution:          return "QuoteExecution";
        case AppMsgType::QuoteCancelled:          return "QuoteCancelled";
        case AppMsgType::TASQuoteRestatement:     return "TASQuoteRestatement";
        case AppMsgType::VarianceQuoteRestatement:return "VarianceQuoteRestatement";
        case AppMsgType::ResetRiskAck:            return "ResetRiskAck";
        default:                              return "Unknown AppMsgType";
    }
}

//------------------------------------------------------
// QuoteResult toString Implementation
//------------------------------------------------------
std::string toString(QuoteResult value)
{
    switch (value)
    {
        case QuoteResult::ACCEPT:   return "ACCEPT";
        case QuoteResult::CANCEL:   return "CANCEL";
        case QuoteResult::REJECT:   return "REJECT";
        case QuoteResult::MODIFIED: return "MODIFIED";
        default:                  return "UNKNOWN QuoteResult";
    }
}

//------------------------------------------------------
// Quote Reason Conversions
//------------------------------------------------------
std::string getQuoteReason(char code)
{
    switch (code)
    {
        case 'C': return "Invalid EFID (Clearing Firm)";
        case 'D': return "Invalid WashId";
        case 'F': return "Not Enabled For Quotes";
        case 'I': return "Incorrect Data Center";
        case 'K': return "Message Rate Threshold";
        case 'L': return "Invalid Quote Cnt";
        case 'M': return "Symbols not on same matching engine";
        case 'O': return "Invalid Manual Order Indicator";
        case 'Q': return "Invalid QuoteUpdateId";
        case 'R': return "Futures root does not match across quotes";
        case 'S': return "Symbol not found";
        case 'W': return "Invalid WashPreventType";
        case 'a': return "Admin";
        case 'c': return "Invalid Capacity";
        case 'e': return "Invalid OEOID";
        case 'f': return "Risk Mgmt EFID or CustomGroup ID Level";
        case 'i': return "Invalid CTI Code";
        case 'm': return "Invalid WashMethod";
        case 'o': return "Invalid open/close";
        case 'p': return "Risk Management product level";
        case 'r': return "Invalid Remove";
        case 's': return "Invalid Side";
        case 'u': return "Symbol range unreachable";
        case 'y': return "Quote received by CFE during replay";
        case 'z': return "Invalid SizeModifier";
        default:  return "UNKNOWN QUOTE REASON";
    }
}

std::string getQuoteReason(const std::string &code)
{
    if (code.empty())
        return "UNKNOWN QUOTE REASON";
    return getQuoteReason(code[0]);
}

std::string getQuoteOrderStatus(const std::string &status)
{
    if (status.empty())
        return "UNKNOWN";
    char code = status[0];
    switch (code)
    {
        case 'A': return "NEW";
        case 'L': return "MODIFIED";
        case 'R': return "MODIFIED";
        case 'N': return "NEW";
        case 'D': return "NEW";
        case 'd': return "MODIFIED";
        case 'U': return "CANCELLED";
        case 'a': return "REJECTED";
        case 'P': return "REJECTED";
        case 'f': return "REJECTED";
        case 'S': return "REJECTED";
        case 'p': return "REJECTED";
        case 's': return "REJECTED";
        case 'n': return "REJECTED";
        case 'u': return "REJECTED";
        default:  return "UNKNOWN";
    }
}

bool isQuoteOK(const std::string &status)
{
    if (status.empty())
        return false;
    char code = status[0];
    // Consider these codes as NOT OK.
    switch (code)
    {
        case 'a':
        case 'P':
        case 'f':
        case 'S':
        case 'p':
        case 's':
        case 'n':
        case 'u':
            return false;
        default:
            return true;
    }
}

//------------------------------------------------------
// Message String Functions
//------------------------------------------------------
std::string getMessageType(uint16_t msgtype)
{
    switch (msgtype)
    {
        case static_cast<uint16_t>(AppMsgType::OrderAck):                 return "OrderAck";
        case static_cast<uint16_t>(AppMsgType::OrderRejected):            return "OrderRejected";
        case static_cast<uint16_t>(AppMsgType::OrderModified):            return "OrderModified";
        case static_cast<uint16_t>(AppMsgType::ModifyRejected):           return "ModifyRejected";
        case static_cast<uint16_t>(AppMsgType::OrderCancelled):           return "OrderCancelled";
        case static_cast<uint16_t>(AppMsgType::CancelRejected):           return "CancelRejected";
        case static_cast<uint16_t>(AppMsgType::OrderExecution):           return "OrderExecution";
        case static_cast<uint16_t>(AppMsgType::TradeCancelCorrect):       return "TradeCancelCorrect";
        case static_cast<uint16_t>(AppMsgType::TASRestatement):           return "TASRestatement";
        case static_cast<uint16_t>(AppMsgType::VarianceRestatement):      return "VarianceRestatement";
        case static_cast<uint16_t>(AppMsgType::QuoteUpdateAck):           return "QuoteUpdateAck";
        case static_cast<uint16_t>(AppMsgType::QuoteUpdateReject):        return "QuoteUpdateReject";
        case static_cast<uint16_t>(AppMsgType::QuoteRestated):            return "QuoteRestated";
        case static_cast<uint16_t>(AppMsgType::QuoteCancelled):           return "QuoteCancelled";
        case static_cast<uint16_t>(AppMsgType::QuoteExecution):           return "QuoteExecution";
        case static_cast<uint16_t>(AppMsgType::TASQuoteRestatement):      return "TASQuoteRestatement";
        case static_cast<uint16_t>(SessMsgType::LoginResponse):           return "LoginResponse";
        case static_cast<uint16_t>(SessMsgType::LogoutResponse):          return "LogoutResponse";
        case static_cast<uint16_t>(SessMsgType::ReplayComplete):          return "ReplayComplete";
        case static_cast<uint16_t>(SessMsgType::LoginRequest):            return "LoginRequest";
        case static_cast<uint16_t>(SessMsgType::LogoutRequest):           return "LogoutRequest";
        case static_cast<uint16_t>(SessMsgType::ClientHeartbeat):         return "ClientHeartbeat";
        default:                                                    return "Unknown Message Type";
    }
}

std::map<uint16_t, std::string> getMessageMap()
{
    std::map<uint16_t, std::string> msgmap = {
        {static_cast<uint16_t>(AppMsgType::OrderAck),                 "OrderAck"},
        {static_cast<uint16_t>(AppMsgType::OrderRejected),            "OrderRejected"},
        {static_cast<uint16_t>(AppMsgType::OrderModified),            "OrderModified"},
        {static_cast<uint16_t>(AppMsgType::ModifyRejected),           "ModifyRejected"},
        {static_cast<uint16_t>(AppMsgType::OrderCancelled),           "OrderCancelled"},
        {static_cast<uint16_t>(AppMsgType::CancelRejected),           "CancelRejected"},
        {static_cast<uint16_t>(AppMsgType::OrderExecution),           "OrderExecution"},
        {static_cast<uint16_t>(AppMsgType::TradeCancelCorrect),       "TradeCancelCorrect"},
        {static_cast<uint16_t>(AppMsgType::TASRestatement),           "TASRestatement"},
        {static_cast<uint16_t>(AppMsgType::VarianceRestatement),      "VarianceRestatement"},
        {static_cast<uint16_t>(SessMsgType::LoginResponse),           "LoginResponse"},
        {static_cast<uint16_t>(SessMsgType::LogoutResponse),          "LogoutResponse"},
        {static_cast<uint16_t>(SessMsgType::ReplayComplete),          "ReplayComplete"},
        {static_cast<uint16_t>(AppMsgType::QuoteUpdateAck),           "QuoteUpdateAck"},
        {static_cast<uint16_t>(AppMsgType::QuoteUpdateReject),        "QuoteUpdateReject"},
        {static_cast<uint16_t>(AppMsgType::QuoteRestated),            "QuoteRestated"},
        {static_cast<uint16_t>(AppMsgType::QuoteCancelled),           "QuoteCancelled"},
        {static_cast<uint16_t>(AppMsgType::QuoteExecution),           "QuoteExecution"},
        {static_cast<uint16_t>(AppMsgType::TASQuoteRestatement),      "TASQuoteRestatement"}
    };
    return msgmap;
}

//------------------------------------------------------
// Message Reason Conversions
//------------------------------------------------------
std::string getMessageReason(char reason)
{
    switch (reason)
    {
        case 'A': return "Admin";
        case 'B': return "UnknownMaturityDate";
        case 'C': return "Unknown Product Name";
        case 'D': return "Duplicate Identifier";
        case 'H': return "Halted";
        case 'I': return "Incorrect Data Center";
        case 'J': return "Too late to cancel";
        case 'K': return "Order rate threshold exceeded";
        case 'M': return "Liquidity available exceeds order size";
        case 'N': return "Ran out of liquidity to execute against";
        case 'O': return "ClOrdID doesn't match a known order";
        case 'P': return "Can't modify an order that is pending";
        case 'U': return "User requested";
        case 'V': return "Would wash";
        case 'X': return "Order expired";
        case 'Y': return "Symbol not supported";
        case 'Z': return "Unforeseen reason";
        case 'h': return "Order persisted";
        case 'f': return "Risk management firm level or customer group ID level";
        case 'm': return "Market access risk limit exceeded";
        case 'n': return "Risk management configuration is insufficient";
        case 'o': return "Max open orders count exceeded";
        case 's': return "Risk management product level";
        case 'y': return "Order received by CFE during replay";
        case 'z': return "Session end";
        default:  return "UNKNOWN REASON";
    }
}

std::string getMessageReason(const std::string &reason)
{
    if (reason.empty())
        return "UNKNOWN REASON";
    return getMessageReason(reason[0]);
}


std::string toString(EnumLoginResponseStatus value)
{
    switch (value)
    {
        case EnumLoginResponseStatus::Accepted:         return "Login Accepted";
        case EnumLoginResponseStatus::Session_In_Use:   return "Session In Use";
        case EnumLoginResponseStatus::Disabled:         return "Session is disabled";
        case EnumLoginResponseStatus::Invalid_Unit:     return "Invalid Unit";
        case EnumLoginResponseStatus::Invalid_Message:  return "Invalid Message";
        case EnumLoginResponseStatus::Not_Authorized:   return "Not Authorized";
        case EnumLoginResponseStatus::Sequence_Ahead:   return "Sequence Ahead";
        case EnumLoginResponseStatus::Invalid_Session:  return "Invalid Session";
        default:                                        return "Unknown Login Response Status";
    }
}

std::string toString(EnumLogoutReason value)
{
    switch (value)
    {
        case EnumLogoutReason::User_Requested:          return "User Requested";
        case EnumLogoutReason::Administrative:          return "Administrative";
        case EnumLogoutReason::Protocol_Violation:      return "Protocol Violation";
        default:                                        return "Unknown Logout Reason";
    }
}

std::string to_string(EnumMultilegReportType value)
{
    switch (value)
    {
        case EnumMultilegReportType::OUTRIGHT: return "OUTRIGHT";
        case EnumMultilegReportType::SPREAD_LEG: return "SPREAD_LEG";
        case EnumMultilegReportType::SPREAD: return "SPREAD";
        default: return "UNKNOWN";
    }
}

std::string toString(EnumOrderReasonCode value) {
    switch (value) {
        case Admin:                                  return "Admin";
        case UnknownMaturityDate:                    return "Unknown maturity date";
        case UnknownProductName:                     return "Unknown product name";
        case DuplicateIdentifier:                    return "Duplicate identifier (e.g., ClOrdId)";
        case Halted:                                 return "Halted";
        case IncorrectDataCenter:                    return "Incorrect data center";
        case OrderRateThresholdExceeded:             return "Order rate threshold exceeded";
        case LiquidityAvailableExceedsOrderSize:     return "Liquidity available exceeds order size";
        case RanOutOfLiquidity:                      return "Ran out of liquidity to execute against";
        case ClOrdIdMismatch:                        return "ClOrdId doesn't match a known order";
        case CantModifyPendingOrder:                 return "Can't modify an order that is pending";
        case UserRequested:                          return "User requested";
        case WouldWash:                              return "Would wash";
        case OrderExpired:                           return "Order expired";
        case SymbolNotSupported:                     return "Symbol not supported";
        case UnforeseenReason:                       return "Unforeseen reason";
        case OnlyCloseTransactionsAccepted:          return "Only Close transactions accepted";
        case RiskManagementEfidOrCustomGroupIdLevel: return "Risk management EFID level or custom group Id level";
        case OrderPersisted:                         return "Order persisted";
        case MarketAccessRiskLimitExceeded:          return "Market access risk limit exceeded";
        case RiskManagementConfigInsufficient:       return "Risk management configuration is insufficient";
        case MaxOpenOrdersCountExceeded:             return "Max open orders count exceeded";
        case RiskManagementProductLevel:             return "Risk management product level";
        case OrderReceivedByCFEDuringReplay:         return "Order received by CFE during replay";
        case SessionEnd:                             return "Session end";
        default:                                     return "Unknown";
    }
}


} // namespace Boe3
} // namespace CFE
} // namespace KTN
