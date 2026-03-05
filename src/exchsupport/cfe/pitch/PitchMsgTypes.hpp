/*
 * PitchMsgTypes.hpp
 *
 *  Created on: Jan 1, 2018
 *      Author: sam
 */

#ifndef PITCHMSGTYPESMKTDATA_HPP_
#define PITCHMSGTYPESMKTDATA_HPP_

#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;
// HEX
namespace KTN
{
	namespace CFE
	{
		namespace Gap
		{
			enum MsgType : uint8_t
			{
				EnumGapLogin = 0x01,
				EnumGapLoginResponse = 0x02,
				EnumGapRequest = 0x03,
				EnumGapResponse = 0x04
			};

			static string toString(MsgType value)
			{
				switch (value)
				{
					case EnumGapLogin:
						return "GapLogin";
					case EnumGapLoginResponse:
						return "GapLoginResponse";
					case EnumGapRequest:
						return "GapRequest";
					case EnumGapResponse:
						return "GapResponse";
					default:
						return "UNKNOWN MSG TYPE";
				}
			}
		};

		namespace Spin
		{
			enum MsgType : uint8_t
			{
				EnumSpinLogin = 0x01,
				EnumSpinLoginResponse = 0x02,
				EnumSpinImageAvailable = 0x80,
				EnumSpinRequest = 0x81,
				EnumSpinResponse = 0x82,
				EnumSpinFinished = 0x83
			};

			static string toString(MsgType value)
			{
				switch (value)
				{
					case EnumSpinLogin:
						return "SpinLogin";
					case EnumSpinLoginResponse:
						return "SpinLoginResponse";
					case EnumSpinImageAvailable:
						return "SpinImageAvailable";
					case EnumSpinRequest:
						return "SpinRequest";
					case EnumSpinResponse:
						return "SpinResponse";
					case EnumSpinFinished:
						return "SpinFinished";
					default:
						return "UNKNOWN MSG TYPE";
				}
			}

			enum EnumLoginStatus : char
			{
				EnumLoginAccepted = 'A',
				EnumLoginNotAuthorized = 'N',
				EnumSessionInUse = 'B',
				EnumInvalidSession = 'S'
			};

			static string toString(EnumLoginStatus value)
			{
				switch (value)
				{
					case EnumLoginAccepted:
						return "LoginAccepted";
					case EnumLoginNotAuthorized:
						return "LoginNotAuthorized";
					case EnumSessionInUse:
						return "SessionInUse";
					case EnumInvalidSession:
						return "InvalidSession";
					default:
						return "UNKNOWN MSG TYPE: " + std::to_string(value);
				}
			}

			enum EnumSpinStatus : char
			{
				EnumSpinAccepted = 'A',
				EnumSpinOutOfRange = 'O',
				EnumSpinInProgress = 'S'
			};

			static string toString(EnumSpinStatus value)
			{
				switch (value)
				{
					case EnumSpinAccepted:
						return "SpinAccepted";
					case EnumSpinOutOfRange:
						return "SpinOutOfRange";
					case EnumSpinInProgress:
						return "SpinInProgress";
					default:
						return "UNKNOWN MSG TYPE";
				}
			}
		};

		namespace Pitch
		{
			enum MsgType : uint8_t
			{
				EnumTimeMsg = 0x20,
				EnumAddOrderLongMsg = 0x21,
				EnumAddOrderShortMsg = 0x22,
				EnumOrderExecutedMsg = 0x23,
				EnumOrderExecutedPriceSizeMsg = 0x24,
				EnumReduceSizeLongMsg = 0x25,
				EnumReduceSizeShortMsg = 0x26,
				EnumModifyOrderLongMsg = 0x27,
				EnumModifyOrderShortMsg = 0x28,
				EnumDeleteOrderMsg = 0x29,
				EnumTradeLongMsg = 0x2A,
				EnumTradeShortMsg = 0x2B,
				EnumTradeBreakMsg = 0x2C,
				EnumEndSessionMsg = 0x2D,
				EnumTradingStatusMsg = 0x31,
				EnumUnitClearMsg = 0x97,
				EnumTimeReferenceMsg = 0xB1,
				EnumMarketSnapshotShortMsg = 0xB2,
				EnumMarketSnapshotLongMsg = 0xB3,
				EnumSingleSideUpdateShortMsg = 0xB4,
				EnumSingleSideUpdateLongMsg = 0xB5,
				EnumTwoSideUpdateShortMsg = 0xB6,
				EnumTwoSideUpdateLongMsg = 0xB7,
				EnumTOPTradeMsg = 0xB8,
				EnumSettlementMsg = 0xB9,
				EnumEODSummaryMsg = 0xBA,
				EnumFuturesInstrumentDefinitionMsg = 0xBB,
				EnumTransactionBeginMsg = 0xBC,
				EnumTransactionEndMsg = 0xBD,
				EnumPriceLimitsMsg = 0xBE,
				EnumOpenInterestMsg = 0xD3,
				EnumFuturesVarianceSymbolMappingMsg = 0xFA
			};

			static string toString(uint16_t value)
			{
				switch (value)
				{
					case EnumTimeMsg:
						return "TimeMsg";
					case EnumAddOrderLongMsg:
						return "AddOrderLongMsg";
					case EnumAddOrderShortMsg:
						return "AddOrderShortMsg";
					case EnumOrderExecutedMsg:
						return "OrderExecutedMsg";
					case EnumOrderExecutedPriceSizeMsg:
						return "OrderExecutedPriceSizeMsg";
					case EnumReduceSizeLongMsg:
						return "ReduceSizeLongMsg";
					case EnumReduceSizeShortMsg:
						return "ReduceSizeShortMsg";
					case EnumModifyOrderLongMsg:
						return "ModifyOrderLongMsg";
					case EnumModifyOrderShortMsg:
						return "ModifyOrderShortMsg";
					case EnumDeleteOrderMsg:
						return "DeleteOrderMsg";
					case EnumTradeLongMsg:
						return "TradeLongMsg";
					case EnumTradeShortMsg:
						return "TradeShortMsg";
					case EnumTradeBreakMsg:
						return "TradeBreakMsg";
					case EnumEndSessionMsg:
						return "EndSessionMsg";
					case EnumTradingStatusMsg:
						return "TradingStatusMsg";
					case EnumUnitClearMsg:
						return "UnitClearMsg";
					case EnumTimeReferenceMsg:
						return "TimeReferenceMsg";
					case EnumMarketSnapshotShortMsg:
						return "MarketSnapshotShortMsg";
					case EnumMarketSnapshotLongMsg:
						return "MarketSnapshotLongMsg";
					case EnumSingleSideUpdateShortMsg:
						return "SingleSideUpdateShortMsg";
					case EnumSingleSideUpdateLongMsg:
						return "SingleSideUpdateLongMsg";
					case EnumTwoSideUpdateShortMsg:
						return "TwoSideUpdateShortMsg";
					case EnumTwoSideUpdateLongMsg:
						return "TwoSideUpdateLongMsg";
					case EnumTOPTradeMsg:
						return "TOPTradeMsg";
					case EnumSettlementMsg:
						return "Settlement Msg";
					case EnumEODSummaryMsg:
						return "EODSummary Msg";
					case EnumFuturesInstrumentDefinitionMsg:
						return "FuturesInstrumentDefinition Msg";
					case EnumTransactionBeginMsg:
						return "TransactionBegin Msg";
					case EnumTransactionEndMsg:
						return "TransactionEnd Msg";
					case EnumFuturesVarianceSymbolMappingMsg:
						return "FuturesVarianceSymbolMapping Msg";
					default: return to_string(value);
				}
			};

			// end namespaces:
		}
	}
}

#endif /* BATSMSGTYPES_HPP_ */
