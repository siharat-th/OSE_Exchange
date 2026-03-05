/*
 * Boe3Msgs.hpp
 *
 *  Created on: May 22, 2021
 *      Author: samg
 */

#ifndef SRC_BOE3_V3APPLICATIONMESSAGES_HPP_
#define SRC_BOE3_V3APPLICATIONMESSAGES_HPP_

#pragma once

#include <cstdint> // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring> // For string operations (e.g., strncpy)
#include <bitset>
#include <stdint.h>

#include <exchsupport/cfe/Boe3/Boe3Enums.hpp>
#include <exchsupport/cfe/Boe3/Boe3Types.hpp>

namespace KTN
{
	namespace CFE
	{
		namespace Boe3
		{
			// id = 1008 blockLength = 143
			struct NewOrderMsgV2
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				char ClOrdID[20];
				EnumOrderSide Side;
				uint32_t OrderQty;
				// EFID: Default from port if not supplied
				char ClearingFirm[4];
				char ClearingAccount[4];
				// Four (4) implied decimal prices. So *100 for VIX
				PRICE Price;
				EnumOrderType OrdType;
				EnumOrderTIF TimeInForce;
				uint32_t MinQty;
				SECID8 SecId;
				//char SecId[8];
				// Capacity 'C' = Customer; 'F' = Firm
				char Capacity;
				char Account[16];
				char PreventMatch[3];
				DATETIME ExpireTime;
				DATE MaturityDate ;
				EnumOrderOpenClose OpenClose;
				uint32_t CMTANumber;
				PRICE StopPx;
				uint16_t CustomGroupId;
				char CtiCode;
				EnumManualOrderIndicator ManualOrderIndicator;
				char OEOID[18];
				char FrequentTraderId[6];
				char CustOrderHandlingInst;
				char CountryCode[2];

				void Construct()
				{
					StartOfMessage = static_cast<uint16_t>(STARTMSG::B0E3);
					MessageType = static_cast<uint16_t>(AppMsgType::NewOrderV2);
					MatchingUnit = static_cast<uint8_t>(MATCHUINT::VX_MAIN);
					Reserved = 0;
			
					memset(SecId, 0, 8);  // zero out all 8 bytes
					
					// Set default values
					OrderQty = 0;
					Price = 0;
					MinQty = 0x00;
					ExpireTime = 0;
					MaturityDate = 0;
					CMTANumber = 0;
					StopPx = 0;
					CustomGroupId = 0;
					MaturityDate = 0;
					CtiCode = 0;
					ManualOrderIndicator = EnumManualOrderIndicator::MANUAL;
					memset(ClearingFirm, '\0', sizeof(ClearingFirm));
					memset(PreventMatch, '\0', sizeof(PreventMatch));
					OpenClose = EnumOrderOpenClose::NONE;
					OrdType = EnumOrderType::Limit;
					TimeInForce = EnumOrderTIF::Day;

					memcpy(CountryCode, "US", 2);
					CustOrderHandlingInst = '\0';

				}
			} __attribute__((packed));

			// id = 1002 blockLength = 104
			struct ModifyOrderMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				char ClOrdID[20];
				char OrigClOrdID[20];
				char ClearingFirm[4];
				uint32_t OrderQty;
				PRICE Price;
				EnumOrderType OrdType;
				char CancelOrigOnReject;
				PRICE StopPx;
				EnumManualOrderIndicator ManualOrderIndicator;
				char OEOID[18];
				char FrequentTraderId[6];
				char CustOrderHandlingInst;

				void Construct()
				{
					StartOfMessage = static_cast<uint16_t>(STARTMSG::B0E3);
					MessageType = static_cast<uint16_t>(AppMsgType::ModifyOrder);
					MatchingUnit = static_cast<uint8_t>(MATCHUINT::VX_MAIN);
					Reserved = 0;
					SeqNum = 0;
					
					memset(ClOrdID, '\0', sizeof(ClOrdID));
					memset(OrigClOrdID, '\0', sizeof(OrigClOrdID));
					memset(ClearingFirm, '\0', sizeof(ClearingFirm));
					CustOrderHandlingInst = '\0';
				}

			} __attribute__((packed));

			// id = 1003 blockLength = 55
			struct CancelOrderMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				char OrigClOrdID[20];
				char ClearingFirm[4];
				EnumManualOrderIndicator ManualOrderIndicator;
				char OEOID[18];

				void Construct()
				{
					StartOfMessage = static_cast<uint16_t>(STARTMSG::B0E3);
					MessageType = static_cast<uint16_t>(AppMsgType::CancelOrder);
					MatchingUnit = static_cast<uint8_t>(MATCHUINT::VX_MAIN);
					Reserved = 0;
					SeqNum = 0;
			
					memset(ClearingFirm, '\0', sizeof(ClearingFirm));
					memset(OrigClOrdID, '\0', sizeof(OrigClOrdID));
				}
			} __attribute__((packed));

			// id = 1004 blockLength = 77
			struct MassCancelMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				char MassCancelId[20];
				char ClearingFirm[4];
				char ProductName[6];
				char MassCancelInst[16];
				EnumManualOrderIndicator ManualOrderIndicator;
				char OEOID[18];

				void Construct()
				{
					StartOfMessage = static_cast<uint16_t>(STARTMSG::B0E3);
					MessageLength = 75;
					MessageType = static_cast<uint16_t>(AppMsgType::MassCancelOrder);
					MatchingUnit = static_cast<uint8_t>(MATCHUINT::VX_MAIN);
					Reserved = 0;

					memset(ClearingFirm, '\0', sizeof(ClearingFirm));
					memset(ProductName, '\0', sizeof(ProductName));
					memset(MassCancelId, '\0', sizeof(MassCancelId));

				}
			} __attribute__((packed));

			// id = 1005 blockLength = 80
			struct PurgeOrdersMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				char MassCancelId[20];
				char ClearingFirm[4];
				char ProductName[6];
				char MassCancelInst[16];
				EnumManualOrderIndicator ManualOrderIndicator;
				char OEOID[18];
				uint8_t CustomGroupIdCnt;
				uint16_t CustomGroupId;

				void Construct()
				{
					StartOfMessage = static_cast<uint16_t>(STARTMSG::B0E3);
					MessageLength = 78;
					MessageType = static_cast<uint16_t>(AppMsgType::PurgeOrders);
					MatchingUnit = static_cast<uint8_t>(MATCHUINT::VX_MAIN);
					Reserved = 0;
					
				}

			} __attribute__((packed));

			// id=1006 blockLength = 81
			struct QuoteUpdateMessageMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				char QuoteUpdateID[16];
				char ClearingFirm[4];
				char ClearingAccount[4];
				uint32_t CmtaNumber;
				char Account[16];
				uint16_t CustomGroupID;
				char Capacity;
				char CtiCode;
				EnumManualOrderIndicator ManualOrderIndicator;
				char OEOID[18];
				char SizeModifier;
				uint8_t QuoteCnt;

				void Construct()
				{
					StartOfMessage = static_cast<uint16_t>(STARTMSG::B0E3);
					MessageType = static_cast<uint16_t>(AppMsgType::QuoteUpdate);
					MatchingUnit = static_cast<uint8_t>(MATCHUINT::VX_MAIN);
					Reserved = 0;
				}
			} __attribute__((packed));

			// blocklength = 24
			struct GroupQuoteUpdateOrderMsg
			{
				char SecId[6];
				EnumOrderSide Side;
				EnumOrderOpenClose OpenClose;
				PRICE Price;
				uint32_t OrderQty;
				uint32_t ScratchPad;
			} __attribute__((packed));

			// id=1007 blockLength = 48
			struct RiskResetMsg
			{
				uint16_t StartOfMessage;
				uint16_t MessageLength;
				uint16_t MessageType;
				uint8_t MatchingUnit;
				uint8_t Reserved;
				uint32_t SeqNum;
				char RiskStatusId[16];
				char RiskReset[8];
				char ClearingFirm[4];
				char ProductName[6];
				uint16_t CustomGroupId;

				void Construct()
				{
					StartOfMessage = static_cast<uint16_t>(STARTMSG::B0E3);
					MessageType = static_cast<uint16_t>(AppMsgType::ResetRisk);
					MatchingUnit = static_cast<uint8_t>(MATCHUINT::VX_MAIN);
					Reserved = 0;
				}
			} __attribute__((packed));

			struct OrderAckMsg
			{
				inline static const std::size_t id{1501};
				inline static const std::size_t blockLength{160};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char ClOrdID[20];
					uint64_t OrderID;
					EnumOrderSide Side;
					PRICE Price;
					EnumOrderType OrdType;
					EnumOrderTIF TimeInForce;
					uint32_t MinQty;
					SECID8 SecId;
					char Capacity;
					char Account[16];
					char ClearingFirm[4];
					char ClearingAccount[4];
					uint32_t OrderQty;
					char PreventMatch[3];
					DATE MaturityDate;
					EnumOrderOpenClose OpenClose;
					uint32_t LeavesQty;
					char BaseLiquidityIndicator;
					DATETIME ExpireTime;
					char SubLiquidityIndicator;
					PRICE StopPx;
					uint32_t CMTANumber;
					char CtiCode;
					EnumManualOrderIndicator ManualOrderIndicator;
					char OEOID[18];
					uint32_t CumQty;
					char FrequentTraderId[6];
					char CustOrderHandlingInst;
					DATETIME RequestReceivedTime;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct OrderRejectMsg
			{
				inline static const std::size_t id{1502};
				inline static const std::size_t blockLength{105};
				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char ClOrdID[20];
					char ClearingFirm[4];
					EnumOrderReasonCode OrderRejectReason;
					char Text[60];
				} __attribute__((packed));
				BlockData *blockData_{nullptr};
			};

			struct OrderModifiedMsg
			{

				inline static const std::size_t id{1503};
				inline static const std::size_t blockLength{113};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					uint64_t TransactTime;
					char ClOrdID[20];
					char OrigClOrdID[20];
					uint64_t OrderID;
					char ClearingFirm[4];
					PRICE Price;
					EnumOrderType OrdType;
					uint32_t OrderQty;
					uint32_t LeavesQty;
					char BaseLiquidityIndicator;
					PRICE StopPx;
					char FrequentTraderId[6];
					char CustOrderHandlingInst;
					DATETIME RequestReceivedTime;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct ModifyRejectedMsg
			{
				inline static const std::size_t id{1504};
				inline static const std::size_t blockLength{125};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					uint64_t TransactTime;
					char ClOrdID[20];
					char OrigClOrdID[20];
					char ClearingFirm[4];
					EnumOrderReasonCode ModifyRejectReason;
					char Text[60];
				} __attribute__((packed));
				BlockData *blockData_{nullptr};
			};

			struct OrderExecutionMsg
			{
				inline static const std::size_t id{1505};
				inline static const std::size_t blockLength{99};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					uint64_t TransactTime;
					char ClOrdID[20];
					uint64_t ExecId;
					uint32_t LastShares;
					PRICE LastPx;
					uint32_t LeavesQty;
					char BaseLiquidityIndicator;
					char SubLiquidityIndicator;
					EnumOrderSide Side;
					SECID8 SecId;
					char ClearingFirm[4];
					DATE MaturityDate;
					char FeeCode[2];
					DATE TradeDate;
					uint32_t ClearingSize;
					char PendingStatus;
					EnumMultilegReportType MultilegReportingType;
					uint64_t SecondaryExecId;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct OrderCancelledMsg
			{
				inline static const std::size_t id{1506};
				inline static const std::size_t blockLength{53};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char ClOrdID[20];
					char ClearingFirm[4];
					EnumOrderReasonCode CancelReason;
					DATETIME RequestReceivedTime;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct CancelRejectMsg
			{
				inline static const std::size_t id{1507};
				inline static const std::size_t blockLength{105};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char ClOrdID[20];
					char ClearingFirm[4];
					EnumOrderReasonCode CancelRejectReason;
					char Text[60];
				} __attribute__((packed));
			};

			struct MassCancelAckMsg
			{
				inline static const std::size_t id{1508};
				inline static const std::size_t blockLength{52};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char MassCancelId[20];
					uint32_t CancelledOrderCount;
					DATETIME RequestReceivedTime;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct MassCancelRejectMsg
			{
				inline static const std::size_t id{1509};
				inline static const std::size_t blockLength{101};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char MassCancelId[20];
					EnumOrderReasonCode MassCancelRejectReason;
					char Text[60];
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct PurgeAckMsg
			{
				inline static const std::size_t id{1510};
				inline static const std::size_t blockLength{52};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char MassCancelId[20];
					uint32_t PurgedOrderCount;
					DATETIME RequestReceivedTime;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct PurgeRejectedMsg
			{
				inline static const std::size_t id{1511};
				inline static const std::size_t blockLength{101};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char MassCancelId[20];
					EnumOrderReasonCode PurgeRejectReason;
					char Text[60];
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct TradeCancelCorrectMsg
			{
				inline static const std::size_t id{1512};
				inline static const std::size_t blockLength{107};
				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char ClOrdID[20];
					uint64_t ExecRefId;
					EnumOrderSide Side;
					char BaseLiquidityIndicator;
					char ClearingFirm[4];
					char ClearingAccount[4];
					uint32_t LastShares;
					PRICE LastPx;
					PRICE CorrectedPrice;
					DATETIME OrigTime;
					char Symbol[8];
					char Capacity;
					DATE MaturityDate;
					EnumOrderOpenClose OpenClose;
					uint32_t CMTANumber;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct TASRestatementMsg
			{
				inline static const std::size_t id{1513};
				inline static const std::size_t blockLength{112};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char ClOrdID[20];
					char ClearingFirm[4];
					uint64_t ExecId;
					EnumOrderSide Side;
					PRICE Price;
					SECID8 SecId;
					DATE MaturityDate;
					uint32_t LastShares;
					PRICE LastPx;
					char FeeCode[2];
					DATE TradeDate;
					PRICE ClearingPrice;
					char ClearingSymbol[8];
					char MultilegReportingType;
					uint64_t SecondaryExecId;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct VarianceRestatementMsg
			{
				inline static const std::size_t id{1514};
				inline static const std::size_t blockLength{112};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char ClOrdID[20];
					char ClearingFirm[4];
					uint64_t ExecId;
					EnumOrderSide Side;
					PRICE Price;
					SECID8 SecId;
					DATE MaturityDate;
					uint32_t LastShares;
					PRICE LastPx;
					char FeeCode[2];
					DATE TradeDate;
					PRICE ClearingPrice;
					char ClearingSymbol[8];
					char MultilegReportingType;
					uint64_t SecondaryExecId;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct QuoteUpdateAckNsg
			{
				inline static const std::size_t id{1515};
				inline static const std::size_t blockLength{45};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char QuoteUpdateID[16];
					DATETIME RequestReceivedTime;
					uint8_t QuoteCnt;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct QuoteUpdateAckGroupNsg
			{
				inline static const std::size_t blockLength{10};

				struct BlockData
				{
					uint64_t OrderID;
					char QuoteResult;
					char SubLiquidityInd;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct QuoteUpdateRejectedNsg
			{
				inline static const std::size_t id{1516};
				inline static const std::size_t blockLength{37};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char QuoteUpdateID[16];
					char QuoteRejectReason;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct QuoteRestatedNsg
			{
				inline static const std::size_t id{1517};
				inline static const std::size_t blockLength{64};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char QuoteUpdateID[16];
					uint64_t OrderID;
					uint32_t LeavesQty;
					PRICE WorkingPrice;
					char SecId[6];
					EnumOrderSide Side;
					char RestatementReason;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct QuoteExecutionNsg
			{
				inline static const std::size_t id{1518};
				inline static const std::size_t blockLength{83};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char QuoteUpdateID[16];
					uint64_t OrderID;
					uint64_t ExecId;
					char QuoteSymbol[6];
					char ClearingFirm[4];
					uint32_t LastShares;
					PRICE LastPx;
					uint32_t LeavesQty;
					EnumOrderSide Side;
					char BaseLiquidityInd;
					char SubLiquidityInd;
					char FeeCode[2];
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct QuoteCancelledNsg
			{
				inline static const std::size_t id{1519};
				inline static const std::size_t blockLength{52};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char QuoteUpdateID[16];
					uint64_t OrderID;
					char SecId[6];
					EnumOrderSide Side;
					char CancelReason;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct TASQuoteRestatementNsg
			{
				inline static const std::size_t id{1520};
				inline static const std::size_t blockLength{66};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char QuoteUpdateID[16];
					uint64_t ExecId;
					char SecId[6];
					char ClearingSymbol[8];
					PRICE ClearingPrice;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct VarianceQuoteRestatementNsg
			{
				inline static const std::size_t id{1521};
				inline static const std::size_t blockLength{66};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					DATETIME TransactTime;
					char QuoteUpdateID[16];
					uint64_t ExecId;
					char SecId[6];
					char ClearingSymbol[8];
					PRICE ClearingPrice;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

			struct ResetRiskAckMsg
			{
				inline static const std::size_t id{1522};
				inline static const std::size_t blockLength{37};

				struct BlockData
				{
					uint16_t StartOfMessage;
					uint16_t MessageLength;
					uint16_t MessageType;
					uint8_t MatchingUnit;
					uint8_t Reserved;
					uint32_t SeqNum;
					char RiskStatusId[16];
					char RiskResetResult;
					DATETIME RequestReceivedTime;
				} __attribute__((packed));

				BlockData *blockData_{nullptr};
			};

		} /* namespace Boe3 */
	} /* namespace CFE */
} /* namespace KTN */

#endif /* SRC_BOE3_V3SESSIONMESSAGES_HPP_ */
