//============================================================================
// Name        	: AppMessageGen.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 20, 2023 4:11:32 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CFE_BOE3_APPMESSAGEGEN_HPP_
#define SRC_EXCHSUPPORT_CFE_BOE3_APPMESSAGEGEN_HPP_
#pragma once

#include <exchsupport/cfe/Boe3/Boe3MessageIncludes.hpp>
#include <exchsupport/cfe/settings/CfeBoe3Settings.hpp>

#include <KT01/Net/byte_writer.hpp>
#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
#include <KT01/Net/helpers.hpp>
using namespace KTN::NET;

#include <KT01/Core/PerformanceTracker.hpp>
#include <Orders/Order.hpp>
#include <Orders/IdGen.h>
using namespace KTN;

namespace KTN
{
	namespace CFE
	{
		namespace Boe3
		{
			/**
			 * @brief This class is used to generate CME IL3 application messages and provide pre-filled templates for them.
			 *
			 */
			class AppMessageGen
			{
			public:
				static KTNBuf NewOrderTemplate(const string &settingsfile, const string &oeo_id, KOrderType::Enum ordtype, EnumManualOrderIndicator manual_indicator)
				{
					CfeBoe3Settings sess;
					sess.Load(settingsfile);

					char *buffer = new char[1600];
					uint32_t buffer_length = 1600;
					uint32_t bytes_written = 0;

					ByteWriter writer(buffer, buffer_length, &bytes_written);

					// NewOrder * msg = {};
					NewOrderMsgV2 *msg = (NewOrderMsgV2 *)writer.cur();
					msg->Construct();

					strncpy(msg->ClearingFirm, sess.SessionSettings.EFID.c_str(), sizeof(msg->ClearingFirm));
					// note- we don't have to fill in clearing account as we can set that at the port level with CFE
					memset(msg->ClearingAccount, '\0', sizeof(msg->ClearingAccount));
					msg->TimeInForce = EnumOrderTIF::Day;
					msg->MinQty = 0;


					msg->Capacity = sess.Capacity[0];
					msg->CtiCode = sess.CTI[0];

					memset(msg->Account, '\0', sizeof(msg->Account));
					strncpy(msg->Account, sess.Account.c_str(), strlen(sess.Account.c_str()));
					strncpy(msg->PreventMatch, sess.PreventMatchInstruction.c_str(), strlen(sess.PreventMatchInstruction.c_str()));

					msg->ManualOrderIndicator = manual_indicator;
					memset(msg->OEOID, '\0', sizeof(msg->OEOID));
					strncpy(msg->OEOID, oeo_id.c_str(), strlen(oeo_id.c_str()));

					msg->CustomGroupId = 0;
					memset(msg->FrequentTraderId, '\0', sizeof(msg->FrequentTraderId));
					strncpy(msg->FrequentTraderId, sess.FrequentTraderId.c_str(), strlen(sess.FrequentTraderId.c_str()));

					// NOTE: CustOrderHandling inst = NUL; defaults to Port Defalut of 'Y" - electronic
					// I put this here to answer the question of what it is
					// msg->CustOrderHandlingInst = '\0';
					
					// country code V2
					memcpy(msg->CountryCode, "US", 2);

					// order type
					switch (ordtype)
					{
					case KOrderType::LIMIT:
						msg->OrdType = EnumOrderType::Limit;
						break;

					case KOrderType::MARKET:
					case KOrderType::MARKET_LIMIT:
						msg->OrdType = EnumOrderType::Market;
						break;

					case KOrderType::STOPLIMIT:
					case KOrderType::STOP:
						msg->OrdType = EnumOrderType::StopLimit;
						break;
					}

					int iMsgLen = sizeof(NewOrderMsgV2);
					msg->MessageLength = iMsgLen - 2;

					KTNBuf container = {};
					container.buffer = buffer;
					container.buffer_length = iMsgLen;

					return container;
				};

				static KTNBuf CancelOrderTemplate(const string &settingsfile, const string &oeo_id, EnumManualOrderIndicator manual_indicator)
				{
					CfeBoe3Settings sess;
					sess.Load(settingsfile);

					char *buffer = new char[1600];
					uint32_t buffer_length = 1600;
					uint32_t bytes_written = 0;

					ByteWriter writer(buffer, buffer_length, &bytes_written);

					CancelOrderMsg *msg = (CancelOrderMsg *)writer.cur();
					msg->Construct();

					strncpy(msg->ClearingFirm, sess.SessionSettings.EFID.c_str(), strlen(sess.SessionSettings.EFID.c_str()));

					msg->ManualOrderIndicator = manual_indicator;
					memset(msg->OEOID, '\0', sizeof(msg->OEOID));
					strncpy(msg->OEOID, oeo_id.c_str(), strlen(oeo_id.c_str()));

					int iMsgLen = sizeof(CancelOrderMsg);
					msg->MessageLength = iMsgLen - 2;

					KTNBuf container = {};
					container.buffer = buffer;
					container.buffer_length = iMsgLen;

					return container;
				};

				static KTNBuf ModifyOrderTemplate(const string &settingsfile, const string &oeo_id, KOrderType::Enum ordtype, EnumManualOrderIndicator manual_indicator)
				{
					CfeBoe3Settings sess;
					sess.Load(settingsfile);

					char *buffer = new char[1600];
					uint32_t buffer_length = 1600;
					uint32_t bytes_written = 0;

					ByteWriter writer(buffer, buffer_length, &bytes_written);

					ModifyOrderMsg *msg = (ModifyOrderMsg *)writer.cur();
					msg->Construct();

					strncpy(msg->ClearingFirm, sess.SessionSettings.EFID.c_str(), strlen(sess.SessionSettings.EFID.c_str()));


					msg->CancelOrigOnReject = 'Y';

					msg->OrderQty = 0;
					msg->Price = 0;
					msg->StopPx = 0;

					msg->ManualOrderIndicator = manual_indicator;

					memset(msg->OEOID, '\0', sizeof(msg->OEOID));
					strncpy(msg->OEOID, oeo_id.c_str(), sizeof(msg->OEOID));

					memset(msg->FrequentTraderId, '\0', sizeof(msg->FrequentTraderId));
					strncpy(msg->FrequentTraderId, sess.FrequentTraderId.c_str(), sizeof(msg->FrequentTraderId));

					memset(msg->FrequentTraderId, '\0', sizeof(msg->FrequentTraderId));
					strncpy(msg->FrequentTraderId, sess.FrequentTraderId.c_str(), strlen(sess.FrequentTraderId.c_str()));

					// order type
					switch (ordtype)
					{
					case KOrderType::LIMIT:
						msg->OrdType = EnumOrderType::Limit;
						break;

					case KOrderType::MARKET:
						msg->OrdType = EnumOrderType::Market;
						break;

					case KOrderType::STOPLIMIT:
						msg->OrdType = EnumOrderType::StopLimit;
						break;
					}

					int iMsgLen = sizeof(ModifyOrderMsg);
					msg->MessageLength = iMsgLen - 2;

					KTNBuf container = {};
					container.buffer = buffer;
					container.buffer_length = iMsgLen;

					return container;
				};

				static KTNBuf MassCancelEncode(const string& efid, const string &oeo_id, EnumManualOrderIndicator manual_indicator,
											   string clordid, int32_t seqnum, string root = "VX", string massinst="FBN")
				{
					KTNBuf container = {};

					uint32_t buffer_length = sizeof(container.buf);
					uint32_t bytes_written = 0;

					ByteWriter writer(container.buf, buffer_length, &bytes_written);

					MassCancelMsg *msg = (MassCancelMsg *)writer.cur();
					msg->Construct();

					msg->SeqNum = seqnum;

					strncpy(msg->MassCancelId, clordid.c_str(), sizeof(msg->MassCancelId));

					if (massinst == "FBNSP")
					{
						string cxlid = "01234";
						strncpy(msg->MassCancelId, cxlid.c_str() , sizeof(msg->MassCancelId));
					}

					if (massinst == "FBNSC")
					{
						string cxlid = "12345";
						strncpy(msg->MassCancelId, cxlid.c_str() , sizeof(msg->MassCancelId));
					}

					if (massinst == "FBNC")
					{
						string cxlid = "1223456345";
						strncpy(msg->MassCancelId, cxlid.c_str() , sizeof(msg->MassCancelId));
					}

					if (massinst == "FSLB")
					{
						string cxlid = "34567";
						strncpy(msg->MassCancelId, cxlid.c_str() , sizeof(msg->MassCancelId));
					}


					strncpy(msg->ClearingFirm, efid.c_str(), sizeof(msg->ClearingFirm));
					strncpy(msg->ProductName, root.c_str(), sizeof(msg->ProductName));
			
					// std::string massinst = "ABNBC";// + ackstyle;
					// ALWAYS CANCEL ALL. FOR NOW.
					//string massinst = "ABN";
					
					cout << "[MassCancelEncode OK] massinst=" << massinst << endl;
					
					memset(msg->MassCancelInst, '\0', sizeof(msg->MassCancelInst));
					strncpy(msg->MassCancelInst, massinst.c_str(), sizeof(msg->MassCancelInst));

					msg->ManualOrderIndicator = manual_indicator;
					memset(msg->OEOID, '\0', sizeof(msg->OEOID));
					strncpy(msg->OEOID, oeo_id.c_str(), sizeof(msg->OEOID));
					int iMsgLen = sizeof(MassCancelMsg);
					msg->MessageLength = iMsgLen - 2;

					container.buffer_length = iMsgLen;

					// NetHelpers::hexdump(container.buffer, msg->msgSize);
					return container;
				};

				static KTNBuf RiskResetEncode(const string& efid, string clordid, int32_t seqnum, string root = "VX", string riskreset="SFC")
				{
					KTNBuf container = {};

					uint32_t buffer_length = sizeof(container.buf);
					uint32_t bytes_written = 0;

					ByteWriter writer(container.buf, buffer_length, &bytes_written);

					RiskResetMsg *msg = (RiskResetMsg *)writer.cur();
					msg->Construct();

					msg->SeqNum = seqnum;

					strncpy(msg->RiskStatusId, clordid.c_str(), sizeof(msg->RiskStatusId));
					strncpy(msg->ClearingFirm, efid.c_str(), sizeof(msg->ClearingFirm));
					strncpy(msg->ProductName, root.c_str(), sizeof(msg->ProductName));
					strncpy(msg->RiskReset, riskreset.c_str(), sizeof(msg->RiskReset));

					int iMsgLen = sizeof(RiskResetMsg);
					msg->MessageLength = iMsgLen - 2;

					container.buffer_length = iMsgLen;

					// NetHelpers::hexdump(container.buffer, msg->msgSize);
					return container;
				};
			};
		} /* namespace Boe3 */
	} /* namespace CFE */
} /* namespace KTN */
#endif /* SRC_EXCHSUPPORT_CFE_BOE3_APPMESSAGEFACTORY_HPP_ */
