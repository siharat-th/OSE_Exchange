//============================================================================
// Name        	: AppMessageFactoryFastV2.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 20, 2023 4:11:32 PM
//============================================================================

#include <exchsupport/cme/factory/AppMessageFactoryFastV2.hpp>

using namespace KTN::CME::IL3;

AppMessageFactoryFastV2::AppMessageFactoryFastV2()
{

}

AppMessageFactoryFastV2::~AppMessageFactoryFastV2() {
}

void AppMessageFactoryFastV2::Init(string settingsfile)
{
	//init the low latency timestamp service
	_tscns.init();
#if !AKL_TEST_MODE
	calibrationThread = std::thread([this]() {
		const char *threadName = "tscns-calib";
		pthread_setname_np(pthread_self(), threadName);
		while (true)
		{
			_tscns.calibrate();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	});
#endif

	EnumManualOrdIndReq MANUAL = EnumManualOrdIndReq_Manual;
	EnumManualOrdIndReq AUTO = EnumManualOrdIndReq_Automated;

	_sess.Load(settingsfile);

	_NEW_ATS = AppMessageGen::NewOrderTemplate(settingsfile, KOrderType::LIMIT, AUTO);
	_MOD_ATS = AppMessageGen::ModifyOrderTemplate(settingsfile, KOrderType::LIMIT, AUTO);
	_CXL_ATS = AppMessageGen::CancelOrderTemplate(settingsfile, AUTO);

	_NEW_MANUAL = AppMessageGen::NewOrderTemplate(settingsfile, KOrderType::LIMIT, MANUAL);
	_MOD_MANUAL = AppMessageGen::ModifyOrderTemplate(settingsfile, KOrderType::LIMIT, MANUAL);
	_CXL_MANUAL = AppMessageGen::CancelOrderTemplate(settingsfile, MANUAL);

	_NEW_ATS_MKT = AppMessageGen::NewOrderTemplate(settingsfile, KOrderType::MARKET, AUTO);
	_NEW_MANUAL_MKT = AppMessageGen::NewOrderTemplate(settingsfile, KOrderType::MARKET, MANUAL);

	_NEW_ATS_MKT_LIMIT = AppMessageGen::NewOrderTemplate(settingsfile, KOrderType::MARKET_LIMIT, AUTO);
	_NEW_MANUAL_MKT_LIMIT = AppMessageGen::NewOrderTemplate(settingsfile, KOrderType::MARKET_LIMIT, MANUAL);

	_NEW_ATS_STOP_LIMIT = AppMessageGen::NewOrderStopOrderTemplate(settingsfile, KOrderType::STOPLIMIT, AUTO);
	_NEW_MANUAL_STOP_LIMIT = AppMessageGen::NewOrderStopOrderTemplate(settingsfile, KOrderType::STOPLIMIT, MANUAL);

	_NEW_ATS_STOP = AppMessageGen::NewOrderStopOrderTemplate(settingsfile, KOrderType::STOP, AUTO);
	_NEW_MANUAL_STOP = AppMessageGen::NewOrderStopOrderTemplate(settingsfile, KOrderType::STOP, MANUAL);

}

//blank, pre-configured templates. Fill in critical data and send for encoding
void AppMessageFactoryFastV2::GetNewOrderTemplate(NewOrderSingleMsg*& msg)
{
	KTNBuf container = {};
	container.buffer = _NEW_ATS.buffer;

	msg = reinterpret_cast<NewOrderSingleMsg*>(container.buffer);
}

void AppMessageFactoryFastV2::GetModOrderTemplate(OrderCancelReplaceRequestMsg*& msg)
{
	KTNBuf container = {};
	container.buffer = _MOD_ATS.buffer;

	msg = reinterpret_cast<OrderCancelReplaceRequestMsg*>(container.buffer);
}

void AppMessageFactoryFastV2::GetCxlOrderTemplate(OrderCancelRequestMsg*& msg)
{
	KTNBuf container = {};
	container.buffer = _CXL_ATS.buffer;

	msg = reinterpret_cast<OrderCancelRequestMsg*>(container.buffer);
}


void AppMessageFactoryFastV2::NewOrderEncodeMan(const KTN::OrderPod& ord, uint64_t ordreqid,
		uint32_t seqnum, KTNBuf& container, bool logtoscreen=false)
{
	memset(container.buffer,0,150);

	switch (ord.OrdType) {
	    case KOrderType::LIMIT:
	        __builtin_memcpy(container.buffer, _NEW_MANUAL.buffer, _NEW_MANUAL.buffer_length);
	        break;

	    case KOrderType::MARKET:
	        std::cout << "MARKET ORDER IN APPMSGFACT LINE=" << __LINE__ << std::endl;
	        __builtin_memcpy(container.buffer, _NEW_MANUAL_MKT.buffer, _NEW_MANUAL_MKT.buffer_length);
	        break;

	    case KOrderType::MARKET_LIMIT:
			std::cout << "MARKET LIMIT ORDER IN APPMSGFACT LINE=" << __LINE__ << std::endl;
			__builtin_memcpy(container.buffer, _NEW_MANUAL_MKT_LIMIT.buffer, _NEW_MANUAL_MKT_LIMIT.buffer_length);
			break;

	    case KOrderType::STOPLIMIT:
	        std::cout << "STOP LIMIT ORDER IN APPMSGFACT LINE=" << __LINE__ << std::endl;
	        __builtin_memcpy(container.buffer, _NEW_MANUAL_STOP_LIMIT.buffer, _NEW_MANUAL_STOP_LIMIT.buffer_length);
	        break;

	    case KOrderType::STOP:
	        std::cout << "STOP ORDER IN APPMSGFACT LINE=" << __LINE__ << std::endl;
	        __builtin_memcpy(container.buffer, _NEW_MANUAL_STOP.buffer, _NEW_MANUAL_STOP.buffer_length);
	        break;

	    default:
	    	std::cout <<"UNKWON ORDER TYPE RECEIVED: CONVERTING TO LIMIT" << endl;
	    	__builtin_memcpy(container.buffer, _NEW_MANUAL.buffer, _NEW_MANUAL.buffer_length);
	        break;
	}


	NewOrderSingleMsg* msg = reinterpret_cast<NewOrderSingleMsg*>(container.buffer);

	if (LIKELY(ord.OrdType == KOrderType::LIMIT))
		msg->Price = PriceConverters::ToMantissaFromPrice(ord.price);
	else
		msg->Price = IL3::PRICE_NULL;


	msg->StopPx = IL3::PRICE_NULL;
	if (UNLIKELY(ord.OrdType == KOrderType::STOPLIMIT ||ord.OrdType == KOrderType::STOP)){
		msg->StopPx = PriceConverters::ToMantissaFromPrice(ord.stopprice);
		msg->Price = PriceConverters::ToMantissaFromPrice(ord.stopprice);
	}

	msg->OrderQty = ord.quantity;

	msg->DisplayQty = (ord.dispqty == 0) ? IL3::UINT32_NULL : ord.dispqty;
	msg->MinQty = KTN::CME::IL3::UINT32_NULL;

	if (UNLIKELY(ord.OrdTif == KOrderTif::FAK)){

		msg->DisplayQty = IL3::UINT32_NULL;
		msg->MinQty = (ord.minqty > 0) ? ord.minqty : IL3::UINT32_NULL;

		cout << "[Encode NewOrder] " << KOrderTif::toString(ord.OrdTif) << ": MinQty=" << ord.minqty << endl;
	}

	msg->SecurityID = ord.secid;
	msg->Side = (ord.OrdSide == KOrderSide::BUY) ? EnumSideReq::EnumSideReq_Buy : EnumSideReq_Sell;

	strncpy(msg->ClOrdID, ord.ordernum, sizeof(msg->ClOrdID));

	msg->SendingTimeEpoch = _tscns.rdns();

	msg->SeqNum = seqnum;

	msg->OrderRequestID = ordreqid;

	switch (ord.OrdTif)
	{
	case KOrderTif::DAY:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		break;
	case KOrderTif::GTC:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_GoodTillCancel;
		break;
	case KOrderTif::FAK:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_FillAndKill;
		break;
	default:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		break;
	}

	container.buffer_length =  msg->msgSize;

	if (logtoscreen)
	{
		cout << MessagePrinter::generateJsonString(*msg);
	}

}

uint16_t AppMessageFactoryFastV2::NewOrderEncodeAutoV42(char* clordid, uint32_t qty, akl::Price px, int32_t secid,
		KOrderSide::Enum side, KOrderTif::Enum tif, uint64_t ordreqid,
		uint32_t seqnum, char* &buf)
{
	static const size_t bufferSize = 144;
	PREFETCH_RANGE(_NEW_ATS.buffer, bufferSize);
	__builtin_memcpy(buf, _NEW_ATS.buffer, bufferSize);

	NewOrderSingleMsg* msg = reinterpret_cast<NewOrderSingleMsg*>(buf);

	msg->Price = PriceConverters::ToMantissaFromPrice(px);
	msg->OrderQty = qty;
	//msg->DisplayQty = qty;
	msg->DisplayQty = IL3::UINT32_NULL;
	msg->SecurityID = secid;
	msg->Side = (side == KOrderSide::BUY) ? EnumSideReq::EnumSideReq_Buy : EnumSideReq_Sell;

	fastCopy2(msg->ClOrdID, clordid, sizeof(msg->ClOrdID));

	msg->SendingTimeEpoch = _tscns.rdns();
	msg->SeqNum = seqnum;
	msg->OrderRequestID = ordreqid;

	switch (tif)
	{
	case KOrderTif::DAY:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		break;
	case KOrderTif::GTC:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_GoodTillCancel;
		break;
	case KOrderTif::FAK:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_FillAndKill;
		break;
	default:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		break;
	}

	return bufferSize;
}

uint16_t AppMessageFactoryFastV2::NewOrderMsgEncode(NewOrderSingleMsg* msg, char* &buf)
{
	static const size_t bufferSize = 144;
	PREFETCH_RANGE(reinterpret_cast<char*>(msg), bufferSize);

	msg->SendingTimeEpoch = _tscns.rdns();//_ts.Current();

	buf = reinterpret_cast<char*>(msg);

	return bufferSize;
}

// uint16_t AppMessageFactoryFastV2::ModifyOrderMsgEncode(OrderCancelReplaceRequestMsg* msg, char* &buf)
// {
// 	static const size_t bufferSize = 144;
// 	PREFETCH_RANGE(reinterpret_cast<char*>(msg), bufferSize);

// 	msg->SendingTimeEpoch = _tscns.rdns();//_ts.Current();

// 	buf = reinterpret_cast<char*>(msg);

// 	return bufferSize;
// }


void AppMessageFactoryFastV2::ModifyOrderEncodeMan(const KTN::OrderPod& ord, uint32_t seqnum, KTNBuf& container)
{
	memset(container.buffer,0,150);
	__builtin_memcpy(container.buffer, _MOD_MANUAL.buffer, _MOD_MANUAL.buffer_length);

	OrderCancelReplaceRequestMsg* msg = reinterpret_cast<OrderCancelReplaceRequestMsg*>(container.buffer);

	msg->Price = PriceConverters::ToMantissaFromPrice(ord.price);

	msg->OrderQty = ord.quantity;
	msg->DisplayQty = ord.quantity;
	msg->SecurityID = ord.secid;
	msg->Side = (ord.OrdSide == KOrderSide::BUY) ? EnumSideReq::EnumSideReq_Buy : EnumSideReq_Sell;
	msg->SeqNum = seqnum;
	msg->SendingTimeEpoch = _tscns.rdns();

	msg->OrderRequestID = ord.orderReqId;
	msg->OrderID = ord.exchordid;

	if (LIKELY(msg->OFMOverride == EnumOFMOverrideReq::EnumOFMOverrideReq_Enabled))
	{
		msg->OrderQty = ord.leavesqty + ord.fillqty;
		msg->DisplayQty = msg->OrderQty;
	}
	
	
	msg->DisplayQty = (ord.dispqty == 0) ? IL3::UINT32_NULL : ord.dispqty;
	msg->MinQty = KTN::CME::IL3::UINT32_NULL;

	if (UNLIKELY(ord.OrdTif == KOrderTif::FAK)){
		msg->DisplayQty = IL3::UINT32_NULL;
		msg->MinQty = ord.minqty;
	}

	switch (ord.OrdTif)
	{
	case KOrderTif::DAY:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		break;
	case KOrderTif::GTC:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_GoodTillCancel;
		break;
	// case KOrderTif::FAK:
	// 	msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_FillAndKill;
	// 	break;
	default:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		break;
	}



	strncpy(msg->ClOrdID, ord.ordernum, sizeof(msg->ClOrdID));

	msg->OrdType = (ord.OrdType == KOrderType::MARKET) ? EnumOrderTypeReq::EnumOrderTypeReq_MarketWithProtection : EnumOrderTypeReq::EnumOrderTypeReq_Limit;
	msg->StopPx = IL3::PRICE_NULL;

	int iMsgSize = msg->msgSize;
	container.buffer_length =  iMsgSize;
}

uint16_t AppMessageFactoryFastV2::ModifyOrderEncodeAutoV4(const KTN::OrderPod& ord, uint32_t seqnum,
		char* &buf)
{
	static const size_t bufferSize = 145;
	PREFETCH_RANGE(_MOD_ATS.buffer, _MOD_ATS.buffer_length);
	__builtin_memcpy(buf, _MOD_ATS.buffer, bufferSize);

	OrderCancelReplaceRequestMsg* msg = reinterpret_cast<OrderCancelReplaceRequestMsg*>(buf);
	msg->Price = PriceConverters::ToMantissaFromPrice(ord.price);
	msg->OrderQty = ord.quantity;
	msg->DisplayQty = ord.dispqty;
	msg->SecurityID = ord.secid;
	msg->Side = (ord.OrdSide == KOrderSide::BUY) ? EnumSideReq::EnumSideReq_Buy : EnumSideReq_Sell;

	// if (LIKELY(msg->OFMOverride == EnumOFMOverrideReq::EnumOFMOverrideReq_Enabled))
	// {
	// 	msg->OrderQty = ord.leavesqty + ord.fillqty;
	// 	msg->DisplayQty = msg->OrderQty;
	// }

	switch (ord.OrdTif)
	{
	case KOrderTif::DAY:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		break;
	case KOrderTif::GTC:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_GoodTillCancel;
		break;
	// case KOrderTif::FAK:
	// 	msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_FillAndKill;
	// 	break;
	default:
		msg->TimeInForce = EnumTimeInForce::EnumTimeInForce_Day;
		break;
	}

	msg->SeqNum = seqnum;
	msg->OrderRequestID = ord.orderReqId;
	msg->OrderID = ord.exchordid;

	msg->SendingTimeEpoch =  _tscns.rdns();//_ts.Current();//
	fastCopy2(msg->ClOrdID, ord.ordernum, sizeof(msg->ClOrdID));

	return bufferSize;
}



void AppMessageFactoryFastV2::CancelOrderEncodeMan(const KTN::OrderPod& ord, uint32_t seqnum, KTNBuf& container)
{
	//container.buffer = _CXL_MANUAL.buffer;

	memset(container.buffer,0,150);
	__builtin_memcpy(container.buffer, _CXL_MANUAL.buffer, _CXL_MANUAL.buffer_length);

	OrderCancelRequestMsg* msg = reinterpret_cast<OrderCancelRequestMsg*>(container.buffer);

	msg->SecurityID = ord.secid;
	msg->Side = (ord.OrdSide == KOrderSide::BUY) ? EnumSideReq::EnumSideReq_Buy : EnumSideReq_Sell;
	msg->SeqNum = seqnum;
	msg->SendingTimeEpoch = _tscns.rdns();

	msg->OrderRequestID = ord.orderReqId;
	msg->OrderID = ord.exchordid;
	msg->PartyDetailsListReqID = _sess.PartyListId;

	strncpy(msg->ClOrdID, ord.ordernum, sizeof(msg->ClOrdID));

	int iMsgSize = msg->msgSize;// sizeof(OrderCancelRequestMsg);

	container.buffer_length =  iMsgSize;

}

void AppMessageFactoryFastV2::CancelOrderEncodeAuto(const KTN::OrderPod& ord, uint32_t seqnum, KTNBuf& container)
{
	memset(container.buffer,0,150);
	__builtin_memcpy(container.buffer, _CXL_ATS.buffer, _CXL_ATS.buffer_length);

	OrderCancelRequestMsg* msg = reinterpret_cast<OrderCancelRequestMsg*>(container.buffer);

	msg->SecurityID = ord.secid;
	msg->Side = (ord.OrdSide == KOrderSide::BUY) ? EnumSideReq::EnumSideReq_Buy : EnumSideReq_Sell;
	msg->SeqNum = seqnum;
	msg->SendingTimeEpoch = _tscns.rdns();
	msg->OrderRequestID = ord.orderReqId;

	msg->OrderID = ord.exchordid;
	msg->PartyDetailsListReqID = _sess.PartyListId;

	strncpy(msg->ClOrdID, ord.ordernum, sizeof(msg->ClOrdID));

	int iMsgSize = msg->msgSize;// sizeof(OrderCancelRequestMsg);

	container.buffer_length =  iMsgSize;

}





