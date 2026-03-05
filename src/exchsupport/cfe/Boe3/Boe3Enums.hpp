#ifndef BOE3ENUMS_HPP_
#define BOE3ENUMS_HPP_

#pragma once

#include <cstdint>
#include <string>
#include <map>

namespace KTN
{
    namespace CFE
    {
        namespace Boe3
        {
            
            enum STARTMSG : uint16_t
            {
                B0E3 = 58288
            };

            enum MATCHUINT : uint8_t
            {
                VX_MAIN = 1,
                IBHY = 2,
                FBT = 3, 
                NA = 4
            };

            enum MsgSizes : uint16_t
            {
                HEADER = 12
            };

            enum MsgTypeRange: uint16_t
            {
                SESSION_MSG_MIN = 1,
                SESSION_MSG_MAX = 504,

                APP_MSG_MIN = 1000,
                APP_MSG_MAX = 2000
            };

            // SessMsg enum.
            enum SessMsgType : uint16_t
            {
                LoginRequest = 1,
                LogoutRequest = 2,
                ClientHeartbeat = 3,
                LoginResponse = 501,
                ReplayComplete = 502,
                LogoutResponse = 503,
                ServerHeartbeat = 504
            };

            // Converts SessMsg to its string representation.
            std::string toString(SessMsgType value);

            

            // AppMsg enum.
            enum AppMsgType : uint16_t
            {
                //NewOrder = 1001,
                ModifyOrder = 1002,
                CancelOrder = 1003,
                MassCancelOrder = 1004,
                PurgeOrders = 1005,
                QuoteUpdate = 1006,
                ResetRisk = 1007,
                NewOrderV2 = 1008,
                OrderAck = 1501,
                OrderRejected = 1502,
                OrderModified = 1503,
                ModifyRejected = 1504,
                OrderExecution = 1505,
                OrderCancelled = 1506,
                CancelRejected = 1507,
                MassCancelAck = 1508,
                MassCancelReject = 1509,
                PurgeAck = 1510,
                PurgeReject = 1511,
                TradeCancelCorrect = 1512,
                TASRestatement = 1513,
                VarianceRestatement = 1514,
                QuoteUpdateAck = 1515,
                QuoteUpdateReject = 1516,
                QuoteRestated = 1517,
                QuoteExecution = 1518,
                QuoteCancelled = 1519,
                TASQuoteRestatement = 1520,
                VarianceQuoteRestatement = 1521,
                ResetRiskAck = 1522
            };

            // Converts AppMsg to its string representation.
            std::string toString(AppMsgType value);

            //Replay enum.
            enum EnumReplayUnspecifiedUnit : char
            {
                Fail = 'F',
                Replay_From_Zero = 'R',
                Skip = 'S'
            };


            //Login Response enum.
            enum EnumLoginResponseStatus : char
            {
                Accepted = 'A', // Accepted
                Session_In_Use = 'B', // Session In Use
                Disabled = 'D', // Disabled
                Invalid_Unit = 'I', // Invalid Unit
                Invalid_Message = 'M', // Invalid Message
                Not_Authorized = 'N', // Not Authorized
                Sequence_Ahead = 'Q', // Sequence Ahead
                Invalid_Session = 'S'  // Invalid Session
            };

            std::string toString(EnumLoginResponseStatus value);

            enum EnumLogoutReason : char
            {
                User_Requested = 'U',// = User Requested
                Administrative = 'A',// = Administrative
                Protocol_Violation = '!'// = Protocol Violation
            };

            std::string toString(EnumLogoutReason value);

            // Order-related enums.
            enum EnumOrderSide : char
            {
                BUY = '1',
                SELL = '2'
            };

            enum EnumOrderType : char
            {
                Market = '1',
                Limit = '2',
                StopLimit = '4'
            };

            enum EnumOrderTIF : char
            {
                Day = '0',
                GTC = '1',
                IOC = '3',
                FOK = '4',
                GTD = '6'
            };

            // Quote result enum.
            enum QuoteResult : uint8_t
            {
                ACCEPT = 0,
                CANCEL = 1,
                REJECT = 2,
                MODIFIED = 3
            };

            enum EnumManualOrderIndicator : char
            {
                MANUAL = 'Y',
                AUTO = 'N'
            };

            enum EnumOrderOpenClose : char
            {
                OPEN = 'O',
                CLOSE = 'C',
                NONE = 'N'
            };

            enum EnumMultilegReportType : char
            {
                OUTRIGHT = '1',
                SPREAD_LEG = '2',
                SPREAD = '3'
            };
          
            std::string to_string(EnumMultilegReportType value);

        enum EnumOrderReasonCode : char {
            Admin                                  = 'A',
            UnknownMaturityDate                    = 'B',
            UnknownProductName                     = 'C',
            DuplicateIdentifier                    = 'D',
            Halted                                 = 'H',
            IncorrectDataCenter                    = 'I',
            OrderRateThresholdExceeded             = 'K',
            LiquidityAvailableExceedsOrderSize     = 'M',
            RanOutOfLiquidity                      = 'N',
            ClOrdIdMismatch                        = 'O',
            CantModifyPendingOrder                 = 'P',
            UserRequested                          = 'U',
            WouldWash                              = 'V',
            OrderExpired                           = 'X',
            SymbolNotSupported                     = 'Y',
            UnforeseenReason                       = 'Z',
            OnlyCloseTransactionsAccepted          = 'c',
            RiskManagementEfidOrCustomGroupIdLevel = 'f',
            OrderPersisted                         = 'h',
            MarketAccessRiskLimitExceeded          = 'm',
            RiskManagementConfigInsufficient       = 'n',
            MaxOpenOrdersCountExceeded             = 'o',
            RiskManagementProductLevel             = 's',
            OrderReceivedByCFEDuringReplay         = 'y',
            SessionEnd                             = 'z'
        };

        std::string toString(EnumOrderReasonCode value);
         


            

            // Functions for mapping quote reason codes to human-readable text.
            std::string getQuoteReason(char code);
            std::string getQuoteReason(const std::string &code);
            std::string getQuoteOrderStatus(const std::string &status);
            bool isQuoteOK(const std::string &status);

            // Message string functions (these can be used for both session and application messages).
            std::string getMessageType(uint16_t msgtype);
            std::map<uint16_t, std::string> getMessageMap();

            // Functions for message reasons.
            std::string getMessageReason(char reason);
            std::string getMessageReason(const std::string &reason);

        } // namespace Boe3
    } // namespace CFE
} // namespace KTN

#endif // V3MSGTYPES_HPP_
