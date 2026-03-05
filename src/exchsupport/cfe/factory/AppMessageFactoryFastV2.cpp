//============================================================================
// Name        	: AppMessageFactoryFastV2.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 20, 2023 4:11:32 PM
//============================================================================

#include <exchsupport/cfe/factory/AppMessageFactoryFastV2.hpp>

using namespace KTN::CFE::Boe3;

AppMessageFactoryFastV2::AppMessageFactoryFastV2()
{
}

AppMessageFactoryFastV2::~AppMessageFactoryFastV2()
{
}

void AppMessageFactoryFastV2::Init(string settingsfile)
{
	// init the low latency timestamp service
	_tscns.init();
	EnumManualOrderIndicator MANUAL = EnumManualOrderIndicator::MANUAL;
	EnumManualOrderIndicator AUTO = EnumManualOrderIndicator::AUTO;

	_sess.Load(settingsfile);
	string tag50_man = _sess.Tag50_Manual;
	string tag50_auto = _sess.Tag50_Auto;

	_NEW_ATS = AppMessageGen::NewOrderTemplate(settingsfile, tag50_auto, KOrderType::LIMIT, AUTO);
	_MOD_ATS = AppMessageGen::ModifyOrderTemplate(settingsfile, tag50_auto, KOrderType::LIMIT, AUTO);
	_CXL_ATS = AppMessageGen::CancelOrderTemplate(settingsfile, tag50_auto, AUTO);

	_NEW_MANUAL = AppMessageGen::NewOrderTemplate(settingsfile, tag50_man, KOrderType::LIMIT, MANUAL);
	_MOD_MANUAL = AppMessageGen::ModifyOrderTemplate(settingsfile, tag50_man, KOrderType::LIMIT, MANUAL);
	_CXL_MANUAL = AppMessageGen::CancelOrderTemplate(settingsfile, tag50_man, MANUAL);

	_NEW_ATS_MKT = AppMessageGen::NewOrderTemplate(settingsfile, tag50_auto, KOrderType::MARKET, AUTO);
	_NEW_MANUAL_MKT = AppMessageGen::NewOrderTemplate(settingsfile, tag50_man, KOrderType::MARKET, MANUAL);

	_NEW_ATS_STOP_LIMIT = AppMessageGen::NewOrderTemplate(settingsfile, tag50_auto, KOrderType::STOPLIMIT, AUTO);
	_NEW_MANUAL_STOP_LIMIT = AppMessageGen::NewOrderTemplate(settingsfile, tag50_man, KOrderType::STOPLIMIT, MANUAL);
}

// blank, pre-configured templates. Fill in critical data and send for encoding
void AppMessageFactoryFastV2::GetNewOrderTemplate(NewOrderMsgV2 *&msg)
{
	KTNBuf container = {};
	container.buffer = _NEW_ATS.buffer;

	msg = reinterpret_cast<NewOrderMsgV2 *>(container.buffer);
}

void AppMessageFactoryFastV2::GetModOrderTemplate(ModifyOrderMsg *&msg)
{
	KTNBuf container = {};
	container.buffer = _MOD_ATS.buffer;

	msg = reinterpret_cast<ModifyOrderMsg *>(container.buffer);
}

void AppMessageFactoryFastV2::GetCxlOrderTemplate(CancelOrderMsg *&msg)
{
	KTNBuf container = {};
	container.buffer = _CXL_ATS.buffer;

	msg = reinterpret_cast<CancelOrderMsg *>(container.buffer);
}

void AppMessageFactoryFastV2::NewOrderEncodeMan(const KTN::OrderPod &ord, uint32_t seqnum, const char *oeoid,
												KTNBuf &container, bool logtoscreen = false)
{
	memset(container.buffer, 0, sizeof(NewOrderMsgV2));

	switch (ord.OrdType)
	{
	case KOrderType::LIMIT:
		//std::cout << "[AppMsgFact DEBUG] LIMIT ORDER IN APPMSGFACT LINE=" << __LINE__ << std::endl;
		__builtin_memcpy(container.buffer, _NEW_MANUAL.buffer, _NEW_MANUAL.buffer_length);
		break;

	case KOrderType::MARKET:
	case KOrderType::MARKET_LIMIT:
		//std::cout << "[AppMsgFact DEBUG] MARKET ORDER IN APPMSGFACT LINE=" << __LINE__ << std::endl;
		__builtin_memcpy(container.buffer, _NEW_MANUAL_MKT.buffer, _NEW_MANUAL_MKT.buffer_length);
		break;

	case KOrderType::STOPLIMIT:
	case KOrderType::STOP:
		//std::cout << "[AppMsgFact DEBUG]  STOP LIMIT ORDER IN APPMSGFACT LINE=" << __LINE__ << std::endl;
		__builtin_memcpy(container.buffer, _NEW_MANUAL_STOP_LIMIT.buffer, _NEW_MANUAL_STOP_LIMIT.buffer_length);
		break;

	default:
		//std::cout << "[AppMsgFact DEBUG] UNKWON ORDER TYPE RECEIVED: CONVERTING TO LIMIT" << endl;
		__builtin_memcpy(container.buffer, _NEW_MANUAL.buffer, _NEW_MANUAL.buffer_length);
		break;
	}

	NewOrderMsgV2 *msg = reinterpret_cast<NewOrderMsgV2 *>(container.buffer);

	if (LIKELY(ord.OrdType == KOrderType::LIMIT))
	{
		int64_t wire_price = CFEConverter::ToWire(ord.price);
		msg->Price = wire_price;
		std::cout << "[NEW BUILD v2025-01-15] CFEConverter::ToWire() | Internal Price (shifted)="
		          << ord.price.AsShifted() << " | Wire Price (unshifted)=" << wire_price << std::endl;
	}
	else
		msg->Price = 0x00;

	msg->StopPx = 0x00;
	if (UNLIKELY(ord.OrdType == KOrderType::STOPLIMIT || ord.OrdType == KOrderType::STOP))
	{
		msg->StopPx = CFEConverter::ToWire(ord.stopprice);
		msg->Price =  CFEConverter::ToWire(ord.stopprice);
	}

	msg->OrderQty = ord.quantity;

	msg->MinQty = 0x00;

	if (UNLIKELY(ord.OrdTif == KOrderTif::IOC))
	{
		msg->MinQty = ord.minqty;
		msg->MinQty = (ord.minqty > 0) ? ord.minqty : 0x00;

		cout << "[Encode NewOrder] " << KOrderTif::toString(ord.OrdTif) << ": MinQty=" << ord.minqty << endl;
	}

	std::array<char, 6> encoded = Base63Codec::encode(ord.secid);
	memset(msg->SecId, 0, sizeof(msg->SecId));  // zero out all 8 bytes
	__builtin_memcpy(msg->SecId, encoded.data(), encoded.size());  // copy 6 characters

	if (oeoid != nullptr)
	{
		memset(msg->OEOID, '\0', sizeof(msg->OEOID));
		strncpy(msg->OEOID, oeoid, sizeof(msg->OEOID));
	}

	//cout << "[AppMsgFact DEBUG] SECID=" << ord.secid << " ENCODED=" << encoded.data() << " MSG=" << msg->SecId << " PRICE=" << msg->Price << endl;

	msg->Side = (ord.OrdSide == KOrderSide::BUY) ? EnumOrderSide::BUY : EnumOrderSide::SELL;

	
	fastCopy2(msg->ClOrdID, ord.ordernum, sizeof(msg->ClOrdID));

	msg->SeqNum = seqnum;

	switch (ord.OrdTif)
	{
	case KOrderTif::DAY:
		msg->TimeInForce = EnumOrderTIF::Day;
		break;
	case KOrderTif::GTC:
		msg->TimeInForce = EnumOrderTIF::GTC;
		break;
	case KOrderTif::FOK:
		msg->TimeInForce = EnumOrderTIF::FOK;
		break;
	case KOrderTif::IOC:
	case KOrderTif::FAK:
		msg->TimeInForce = EnumOrderTIF::IOC;
		break;
	default:
		msg->TimeInForce = EnumOrderTIF::Day;
		break;
	}

	if (ord.OrdOpenClose != KOrderOpenCloseInd::NONE)
	{
		msg->OpenClose = (ord.OrdOpenClose == KOrderOpenCloseInd::OPEN) ? EnumOrderOpenClose::OPEN : EnumOrderOpenClose::CLOSE;
	}
	

	container.buffer_length = sizeof(NewOrderMsgV2);
}

uint16_t AppMessageFactoryFastV2::NewOrderEncodeAuto(char *clordid, uint32_t qty, akl::Price px, uint64_t secid,
													 KOrderSide::Enum side, KOrderTif::Enum tif, uint32_t seqnum, char *&buf)
{
	static const size_t bufferSize = sizeof(NewOrderMsgV2);
	PREFETCH_RANGE(_NEW_ATS.buffer, bufferSize);
	__builtin_memcpy(buf, _NEW_ATS.buffer, bufferSize);

	NewOrderMsgV2 *msg = reinterpret_cast<NewOrderMsgV2 *>(buf);

	msg->Price = CFEConverter::ToWire(px);
	msg->OrderQty = qty;
	
	std::array<char, 6> encoded = Base63Codec::encode(secid);
	memset(msg->SecId, 0, sizeof(msg->SecId));  // zero out all 8 bytes
	__builtin_memcpy(msg->SecId, encoded.data(), encoded.size());  // copy 6 characters

	msg->Side = (side == KOrderSide::BUY) ? EnumOrderSide::BUY : EnumOrderSide::SELL;

	msg->SeqNum = seqnum;

	switch (tif)
	{
	case KOrderTif::DAY:
		msg->TimeInForce = EnumOrderTIF::Day;
		break;
	case KOrderTif::GTC:
		msg->TimeInForce = EnumOrderTIF::GTC;
		break;
	case KOrderTif::FOK:
		msg->TimeInForce = EnumOrderTIF::FOK;
		break;
	case KOrderTif::IOC:
	case KOrderTif::FAK:
		msg->TimeInForce = EnumOrderTIF::IOC;
		break;
	default:
		msg->TimeInForce = EnumOrderTIF::Day;
		break;
	}

	fastCopy2(msg->ClOrdID, clordid, sizeof(msg->ClOrdID));

	return bufferSize;
}

void AppMessageFactoryFastV2::ModifyOrderEncodeMan(uint32_t qty, akl::Price price, const char *newordernum, char *origordernum,
	uint32_t seqnum, KOrderType::Enum ordtype, const char *oeoid, KTNBuf &container)
{
	memset(container.buffer, 0, sizeof(ModifyOrderMsg));
	__builtin_memcpy(container.buffer, _MOD_MANUAL.buffer, _MOD_MANUAL.buffer_length);

	ModifyOrderMsg *msg = reinterpret_cast<ModifyOrderMsg *>(container.buffer);

	msg->Price = CFEConverter::ToWire(price);
	msg->OrderQty = qty;
	msg->SeqNum = seqnum;

	fastCopy2(msg->ClOrdID, newordernum, sizeof(msg->ClOrdID));
	fastCopy2(msg->OrigClOrdID, origordernum, sizeof(msg->OrigClOrdID));

	msg->OrderQty = qty;

	msg->OrdType = (ordtype == KOrderType::MARKET) ? EnumOrderType::Market : EnumOrderType::Limit;
	msg->StopPx = 0x00;

	if (oeoid != nullptr)
	{
		memset(msg->OEOID, '\0', sizeof(msg->OEOID));
		strncpy(msg->OEOID, oeoid, sizeof(msg->OEOID));
	}

	int iMsgLen = sizeof(ModifyOrderMsg);
	msg->MessageLength = iMsgLen - 2;

	container.buffer_length = iMsgLen;
}

uint16_t AppMessageFactoryFastV2::ModifyOrderEncodeAuto(uint32_t qty, akl::Price price, const char *newordernum, char *origordernum,
														uint32_t seqnum, char *&buf)
{
	static const size_t bufferSize = sizeof(ModifyOrderMsg);
	PREFETCH_RANGE(_MOD_ATS.buffer, _MOD_ATS.buffer_length);
	__builtin_memcpy(buf, _MOD_ATS.buffer, bufferSize);

	ModifyOrderMsg *msg = reinterpret_cast<ModifyOrderMsg *>(buf);

	msg->Price = CFEConverter::ToWire(price);
	msg->OrderQty = qty;
	msg->SeqNum = seqnum;

	fastCopy2(msg->ClOrdID, newordernum, sizeof(msg->ClOrdID));
	fastCopy2(msg->OrigClOrdID, origordernum, sizeof(msg->OrigClOrdID));

	return bufferSize;
}

void AppMessageFactoryFastV2::CancelOrderEncodeMan(char *origordernum, uint32_t seqnum, const char *oeoid, KTNBuf &container)
{
	memset(container.buffer, 0, 150);
	__builtin_memcpy(container.buffer, _CXL_MANUAL.buffer, _CXL_MANUAL.buffer_length);

	CancelOrderMsg *msg = reinterpret_cast<CancelOrderMsg *>(container.buffer);

	fastCopy2(msg->OrigClOrdID, origordernum, sizeof(msg->OrigClOrdID));
	msg->SeqNum = seqnum;

	if (oeoid != nullptr)
	{
		memset(msg->OEOID, '\0', sizeof(msg->OEOID));
		strncpy(msg->OEOID, oeoid, sizeof(msg->OEOID));
	}

	container.buffer_length = sizeof(CancelOrderMsg);
}

void AppMessageFactoryFastV2::CancelOrderEncodeAuto(char *origordernum, uint32_t seqnum, KTNBuf &container)
{
	memset(container.buffer, 0, 150);
	__builtin_memcpy(container.buffer, _CXL_ATS.buffer, _CXL_ATS.buffer_length);

	CancelOrderMsg *msg = reinterpret_cast<CancelOrderMsg *>(container.buffer);

	fastCopy2(msg->OrigClOrdID, origordernum, sizeof(msg->OrigClOrdID));
	msg->SeqNum = seqnum;

	container.buffer_length = sizeof(CancelOrderMsg);
}
