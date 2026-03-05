//============================================================================
// Name        	: QuoterV3.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 10, 2023 4:11:48 PM
//============================================================================

#include <AlgoEngines/QuoterV3.hpp>

QuoterV3::QuoterV3() : _qOrds(30000)
{
	_HDG_REQ_ID = IdGen::ReqId16();
}

QuoterV3::~QuoterV3() {
	// TODO Auto-generated destructor stub
}

void QuoterV3::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void QuoterV3::Get(OrderPod& ord)
{
//	_qOrds.dequeue(ord);
	_qOrds.try_pop(ord);

}

void QuoterV3::NextNew(KTN::OrderPod& ord, KOrderSide::Enum side, uint64_t exchordid,
		uint32_t qty, int price )
{
//	_qOrds.dequeue(ord);
	_qOrds.try_pop(ord);

	ord.OrdSide = side;
	//ord.exchordid = exchordid;
	ord.quantity = qty;
	ord.price = price;

	ord.OrdAction = KOrderAction::ACTION_NEW;
	ord.OrdStatus = KOrderStatus::sent_new;
}

void QuoterV3::NextMod(KTN::OrderPod& ord, KOrderSide::Enum side, uint64_t reqid, uint64_t exchordid,
		uint32_t qty, int price )
{
//	_qOrds.dequeue(ord);
	_qOrds.try_pop(ord);

	ord.OrdSide = side;
	ord.exchordid = exchordid;
	ord.orderReqId = reqid;
	ord.quantity = qty;
	ord.price = price;
	ord.OrdAction = KOrderAction::ACTION_MOD;
	ord.OrdStatus = KOrderStatus::sent_mod;
}

void QuoterV3::Construct(int cbid, instrument inst, algometa meta)
{
	_inst = inst;
	_PARAMS = meta;
	_SOURCE = meta.Source;
	_TAG = meta.Tag;

	_CALLBACK_ID = cbid;

	// CreateIdBase();
	// LogMe("NEW ID BASE=" + _IDBASE,LogLevel::WARNING);

	KTN::Order base = {};

	base.source = meta.Source;
	base.tag = meta.Tag;
	base.strategy = meta.Strategy;
	base.stratid = meta.StratId;
	base.guid = meta.UniqueId;
	base.algoid = meta.UniqueId;

	base.callbackid = _CALLBACK_ID;
	base.instindex = inst.index;

	base.isManual = false;

	base.dispqty = 0;
	base.minqty = 0;
	base.state = "ACTIVE";

	base.OrdExch = inst.exchEnum;
	base.OrdTif = (inst.tif == KOrderTif::GTC) ? KOrderTif::GTC : KOrderTif::DAY;
	base.OrdType = KOrderType::LIMIT;
	base.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;

	OrderUtils::FillFields(base, meta.userinfo, meta.UniqueId);

	//instrument level
	OrderUtils::Copy(base.symbol, inst.symbol.c_str());
	base.secid = inst.secid;
	base.mintick = inst.mintick;
	base.exchange = inst.exch;
	base.OrdProdType = inst.prodtype;
	base.mktsegid = inst.mktsegid;
	base.product = inst.product;

	_BaseTEMP = base;

	_NewTEMP = base;
	_NewTEMP.OrdAction = KOrderAction::ACTION_NEW;
	_NewTEMP.OrdStatus = KOrderStatus::sent_new;
	_NewTEMP.OrdState = KOrderState::INFLIGHT;
	_NewTEMP.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;

	_ModTEMP = base;
	_ModTEMP.OrdAction = KOrderAction::ACTION_MOD;
	_ModTEMP.OrdStatus = KOrderStatus::sent_mod;
	_ModTEMP.OrdState = KOrderState::INFLIGHT;
	_ModTEMP.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;


	// //32 bit
	// uint32_t uniqueMessageID = IdGen::ReqId24();
	// _REQ_ID = IdGen::encodeRequestID(_CALLBACK_ID, uniqueMessageID);
	// int res = IdGen::extractAlgoBaseID(_REQ_ID);

	FillQueue();
}

void QuoterV3::FillQueue()
{
	LogMe("*** V3 BUILDING ORDER RING BUFF QUEUE " + _inst.symbol,LogLevel::WARN);

	//_qOrds.reset();

	for(int i = _qOrds.size(); i < MAX_OBJECTS-1; i++)
	{
		KTN::OrderPod ord = {};

		string ordernum = OrderNums::instance().NextClOrdId();

		OrderUtils::fastCopy(ord.ordernum, ordernum.c_str(), sizeof(ord.ordernum));

		ord.orderReqId = OrderNums::instance().GenOrderReqId(ord.ordernum);

		_clOrdIdMap[ord.ordernum] = ord.orderReqId;
		_reqIdMap[ord.orderReqId] = ord.ordernum;

		ord.stratid = _PARAMS.StratId;
		ord.callbackid = _CALLBACK_ID;
		ord.instindex = _inst.index;
		ord.secid = _inst.secid;
		ord.mktsegid = _inst.mktsegid;

		ord.dispqty = KTN::CME::IL3::UINT32_NULL;

		ord.isManual = false;

		ord.OrdExch = _inst.exchEnum;
		ord.OrdTif = (_inst.tif == KOrderTif::GTC) ? KOrderTif::GTC : KOrderTif::DAY;
		ord.OrdType = ORD::KOrderType::LIMIT;
		ord.OrdState = KOrderState::INFLIGHT;

		//these will change
		ord.OrdAction = KOrderAction::ACTION_NEW;
		ord.OrdStatus = KOrderStatus::sent_new;

		//this changes for hedge/etc
		ord.OrdAlgoTrig = KOrderAlgoTrigger::ALGO_LEG;

//		_qOrds.enqueue(ord);
		_qOrds.push(ord);
	#if !AKL_TEST_MODE
		usleep(1);
	#endif
	}

}

