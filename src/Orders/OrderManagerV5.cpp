//============================================================================
// Name        	: OrderManagerV5.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 24, 2023 11:16:58 AM
//============================================================================

#include "OrderManagerV5.hpp"

OrderManagerV5::OrderManagerV5(int core, string owner)://_secmaster(sec),
	_CORE(core),_inflight_count(0),_qOrdProcessed(20000),
	_rpt(false),_spreadlegs(false),_version(5)
{
	_owner = owner;
	_NAME = owner + "/ORDV" + std::to_string(_version);
	_ORDS_ACTIVE.store(false, std::memory_order_relaxed);
	_PARAMSFLAG.store(false, std::memory_order_relaxed);
}

OrderManagerV5::~OrderManagerV5() {}

void OrderManagerV5::UpdateMetaParams(const algometa& meta)
{
	_PARAMS = meta;

#if !AKL_TEST_MODE
	//Here, we check if the thread is running- if it is, we split,
	//otherwise, create and run the thread--
	//So- you MUST call updateMetaParams to run the ordermanager...
	 if(_ORDS_ACTIVE.load(std::memory_order_acquire) == true)
		 return;

	 CreateAndRun("OrdMgrV5", _CORE );
#endif
}

void OrderManagerV5::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void OrderManagerV5::Stop()
{
	_ORDS_ACTIVE.store(false, std::memory_order_relaxed);
}

int OrderManagerV5::Count()
{
	return _ords.size();
}

/**
 * This function is new in V5 to handle OrderPod conversion.
 * Going forward we will only use the large format order for reporting back
 * to database/monitoring systems that are not critical path
 */
void OrderManagerV5::RegisterOrderTemplate(Order& ord)
{
	std::pair<int, int> key = std::make_pair(ord.callbackid, ord.secid);
	_templates[key] = ord;
	LogMe("TEMPLATE FOR " + OrderUtils::toString(ord.symbol) + " REGISTERED");
}

void OrderManagerV5::Clear()
{
    _ords.clear();
    _CLEARFLAG.store(true, std::memory_order_relaxed);
}

void OrderManagerV5::ClearDeadOrders()
{
	_ords.cleardead();
}

void OrderManagerV5::Print()
{
	vector<KTN::OrderPod> ords = GetAll();
	int i = 0;

	LogMe("---- ORDER MANAGER V" + std::to_string(_version) + " ----------",LogLevel::INFO);
	ostringstream oss;
	oss << "  COUNT=" << ords.size()  << " Tot=" << Count() << " ****";
	LogMe(oss.str(),LogLevel::WARN);

	for(auto ord : ords)
	{
		Order temp = GetFromTemp(ord);
		i++;
		ostringstream oss;
		oss << "  " << i << ". " << KOrderSide::toString(ord.OrdSide) << " "
				<< ord.quantity << "[LVS=" << ord.leavesqty << "] "
				<< temp.symbol << " @ " << ord.price << " TAG=" << temp.tag
				<< " ORD=" << ord.ordernum << " CMEID=" << ord.exchordid
				<< " STATUS=" << KOrderStatus::toString(ord.OrdStatus)
				<< " STATE=" << KOrderState::toString(ord.OrdState)
				<< " ALGO=" << KOrderAlgoTrigger::toString(ord.OrdAlgoTrig)
				<< " REQID=" << ord.orderReqId
				<< " SECID=" << ord.secid
				<< " STRATID=" << ord.stratid;
		LogMe(oss.str());
	}

	LogMe("--------------------------------\n",LogLevel::INFO);
}

KTN::OrderPod OrderManagerV5::GetOrder(uint64_t reqid) {
	auto ord = _ords.get(reqid);
	return *ord;
}

bool OrderManagerV5::Exists(uint64_t reqid) {
	return _ords.find(reqid);
}

bool OrderManagerV5::HasInflight(const string& tag)
{
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	if (_ords.at(i)->OrdState == KOrderState::INFLIGHT)
	// 		return true;
	// }

	return false;
}

bool OrderManagerV5::HasInflight()
{
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	if (_ords.at(i)->OrdState == KOrderState::INFLIGHT)
	// 		return true;
	// }
	return false;
}

void OrderManagerV5::ClearInflight()
{

	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	if (_ords.at(i)->OrdState == KOrderState::INFLIGHT)
	// 		_ords.erase(_ords.at(i)->orderReqId);
	// }

}

vector<KTN::OrderPod> OrderManagerV5::GetAll()
{
	vector<KTN::OrderPod> orders;

	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	if (_ords.at(i)->OrdState == KOrderState::UNKNOWN
	// 			&& _ords.at(i)->OrdStatus == KOrderStatus::UNKNOWN)
	// 		continue;
	// 	orders.push_back(*_ords.at(i));
	// }
	return orders;

}

vector<KTN::OrderPod> OrderManagerV5::GetWorking()
{
	vector<KTN::OrderPod> orders;

	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	if (_ords.at(i)->leavesqty <= 0) continue;
	// 	if (_ords.at(i)->OrdState != KOrderState::WORKING &&
	// 			_ords.at(i)->OrdState != KOrderState::INFLIGHT) continue;
	// 	orders.push_back(*_ords.at(i));
	// }
	return orders;

}

void OrderManagerV5::GetBySecIdSideStrat(vector<KTN::OrderPod>& orders, int32_t secid,
		const KOrderSide::Enum side, int stratid)
{
// 	for(int i = 0; i < _ords.size(); i++)
// 	{
// 		if (_ords.at(i)->OrdState != KOrderState::WORKING &&
// 					_ords.at(i)->OrdState != KOrderState::INFLIGHT) continue;
// 		if (_ords.at(i)->secid != static_cast<uint64_t>(secid)) continue;
// 		if (_ords.at(i)->stratid != stratid ) continue;
// 	//	if (_ords.at(i)->leavesqty <= 0) continue;
// 		if (_ords.at(i)->OrdSide != side) continue;
// 		orders.push_back(*_ords.at(i));
// 	}
}

void OrderManagerV5::GetByAlgoTrigger(vector<KTN::OrderPod>& orders, const KOrderAlgoTrigger::Enum trig)
{
	//note: for this, I only want ACTIVE/WORKING orders, so if we need to modify/cancel etc...
// 	for(int i = 0; i < _ords.size(); i++)
// 	{
// //		if (_ords.at(i)->OrdState != KOrderState::WORKING &&
// //					_ords.at(i)->OrdState != KOrderState::INFLIGHT) continue;
// 		if (_ords.at(i)->OrdState != KOrderState::WORKING) continue;
// 		if (_ords.at(i)->OrdAlgoTrig != trig) continue;
// 		if (_ords.at(i)->orderReqId == 0) continue;
// 		orders.push_back(*_ords.at(i));
// 	}
}

void OrderManagerV5::GetByAlgoTriggerSecId(vector<KTN::OrderPod>& orders, const KOrderAlgoTrigger::Enum trig, int32_t secid)
{
	//note: for this, I only want ACTIVE/WORKING orders, so if we need to modify/cancel etc...
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	if (_ords.at(i)->OrdState != KOrderState::WORKING) continue;
	// 	if (_ords.at(i)->secid != static_cast<uint64_t>(secid)) continue;
	// 	if (_ords.at(i)->OrdAlgoTrig != trig) continue;
	// 	if (_ords.at(i)->orderReqId == 0) continue;
	// 	orders.push_back(*_ords.at(i));
	// }
}

void OrderManagerV5::GetByAlgoTriggerPrice(vector<KTN::OrderPod>& orders, const KOrderAlgoTrigger::Enum trig, int32_t secid, int32_t price)
{
	//note: for this, I only want ACTIVE/WORKING orders, so if we need to modify/cancel etc...
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	if (_ords.at(i)->orderReqId == 0) continue;
	// 	if (_ords.at(i)->OrdState != KOrderState::WORKING) continue;
	// 	if (_ords.at(i)->secid != static_cast<uint64_t>(secid)) continue;
	// 	if (_ords.at(i)->price != price) continue;
	// 	if (_ords.at(i)->OrdAlgoTrig != trig) continue;

	// 	orders.push_back(*_ords.at(i));
	// }
}

bool OrderManagerV5::GetBySecIdSideStrat(KTN::OrderPod& ord, int32_t secid,
		const KOrderSide::Enum side, int stratid)
{
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	KTN::OrderPod* currentOrder = _ords.at(i);

	// 	// 2. Store the pointer in a local variable to avoid repeated function calls.
	// 	if (currentOrder->OrdState != KOrderState::WORKING &&
	// 		currentOrder->OrdState != KOrderState::INFLIGHT) continue;

	// 	if (currentOrder->secid != static_cast<uint64_t>(secid)) continue;
	// 	if (currentOrder->stratid != stratid) continue;
	// 	// if (currentOrder->leavesqty <= 0) continue;

	// 	// 10. Use bitwise operations for state checking.
	// 	if (currentOrder->OrdSide != side) continue;

	// 	ord  =  *currentOrder;
	// 	return true;

	// }
	return false;
}

bool OrderManagerV5::GetAtPrice(vector<KTN::OrderPod>& orders, int32_t secid, KOrderSide::Enum side, int32_t price,
			KOrderAlgoTrigger::Enum trig)
{
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	KTN::OrderPod* currentOrder = _ords.at(i);
	// 	if (currentOrder->orderReqId == 0) continue;
	// 	if (currentOrder->OrdState != KOrderState::WORKING &&
	// 			currentOrder->OrdState != KOrderState::INFLIGHT) continue;
	// 	if (currentOrder->secid != static_cast<uint64_t>(secid)) continue;
	// 	if (currentOrder->OrdSide != side) continue;
	// 	if (currentOrder->price != price) continue;
	// 	if (currentOrder->OrdAlgoTrig != trig) continue;

	// 	orders.push_back(*_ords.at(i));
	// }

	return (orders.size() > 0);
}

void OrderManagerV5::GetBySecId(vector<KTN::OrderPod>& ords, int32_t secid)
{
	return _ords.getBySecId(ords, secid);
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	KTN::OrderPod *currentOrder = _ords.at(i);
	// 	if (currentOrder->secid != static_cast<uint64_t>(secid)) continue;
	// 	if (currentOrder->OrdState != KOrderState::WORKING &&
	// 			currentOrder->OrdState != KOrderState::INFLIGHT) continue;
	// 	if (currentOrder->orderReqId == 0) continue;
	// 	ords.push_back(*currentOrder);
	// }
}

int OrderManagerV5::GetSideExposure(int32_t secid, KOrderSide::Enum side, KOrderAlgoTrigger::Enum trig)
{
	int qty = 0;
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	KTN::OrderPod* currentOrder = _ords.at(i);
	// 	if (currentOrder->orderReqId == 0) continue;
	// 	if (currentOrder->OrdState != KOrderState::WORKING &&
	// 			currentOrder->OrdState != KOrderState::INFLIGHT) continue;
	// 	if (currentOrder->secid != static_cast<uint64_t>(secid)) continue;
	// 	if (currentOrder->OrdSide != side) continue;
	// 	if (currentOrder->OrdAlgoTrig != trig) continue;

	// 	qty += _ords.at(i)->leavesqty;
	// }

	return qty;
}

void OrderManagerV5::GetByStratId(vector<KTN::OrderPod>& ords, int stratid)
{
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	if (_ords.at(i)->OrdState != KOrderState::WORKING &&
	// 				_ords.at(i)->OrdState != KOrderState::INFLIGHT) continue;
	// 	if (_ords.at(i)->stratid != stratid ) continue;
	// 	ords.push_back(*_ords.at(i));
	// }
}

bool OrderManagerV5::GetByOrdernum(KTN::OrderPod& ord, const string& ordernum)
{
	return _ords.getByOrderNum(ord, ordernum);
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	if (_ords.at(i)->ordernum == ordernum){
	// 		ord = *_ords.at(i);
	// 		return true;
	// 	}
	// }

	//return false;
}


bool OrderManagerV5::GetByExchId(KTN::OrderPod& ord, uint64_t exchordid)
{
	return _ords.getByExchOrdId(ord, exchordid);
	// for(int i = 0; i < _ords.size(); i++)
	// {
	// 	if (_ords.at(i)->exchordid == exchordid ){
	// 		ord = *_ords.at(i);
	// 		return true;
	// 	}
	// }
	// return false;
}

void OrderManagerV5::Get(KTN::OrderPod& ord, uint64_t reqid)
{
	ord = *_ords.get(reqid);
}

void OrderManagerV5::onSentOrderFast(KTN::OrderPod& ord) {
    _ords[ord.orderReqId] = ord;
	_ords.updateOrderNum(ord);
    _inflight_count ++;
}

void OrderManagerV5::onSentOrderOrdbook(KTN::OrderPod& ord, UserInfo& info) {
    _ords[ord.orderReqId] = ord;
	_ords.updateOrderNum(ord);
    _ugomap[ord.orderReqId] = info;
}

void OrderManagerV5::onSentOrderNew(KTN::OrderPod& ord) {
	_ords[ord.orderReqId] = ord;
	_ords.updateOrderNum(ord);
	_inflight_count ++;
}

void OrderManagerV5::onSentOrderMod(KTN::OrderPod& ord) {
   _ords[ord.orderReqId] = ord;
	_ords.updateOrderNum(ord);
   _inflight_count ++;
}

void OrderManagerV5::onSentOrderCxl(KTN::OrderPod& ord) {
    _ords[ord.orderReqId] = ord;
	_ords.updateOrderNum(ord);
    _inflight_count ++;
}

void OrderManagerV5::minOrderAddInState(KTN::OrderPod& ord)
{
	_ords[ord.orderReqId] = ord;
	_ords.updateOrderNum(ord);
	if (ord.exchordid > 0)
	{
		_ords.updateExchOrdId(ord, ord.exchordid);
	}

	LogMe("Order " + std::string(ord.ordernum) + " added 'in-state' to the manager. STATUS=" + KOrderStatus::toString(ord.OrdStatus)
			+ " STATE=" + KOrderState::toString(ord.OrdState) + " CMEID=" + std::to_string(ord.exchordid));
}

void OrderManagerV5::minOrderUpdate(KTN::OrderPod& ord, KOrderStatus::Enum status, KOrderState::Enum state)
{
    auto& tgt = _ords[ord.orderReqId];

    // Core state
    tgt.OrdStatus = status;
    tgt.OrdState = state;
    if (ord.exchordid > 0) tgt.exchordid = ord.exchordid;
    tgt.leavesqty = ord.leavesqty;
    if (ord.OrdState == KOrderState::STATUS_UPDATE) tgt.fillqty = ord.fillqty;
    tgt.timestamp = ord.timestamp;
    if (ord.OrdType > 0) tgt.OrdType = ord.OrdType;
    tgt.OrdFillType = KOrderFillType::UNKNOWN;

    // Additional fields based on OrderUtils::Update
    if (ord.quantity > 0) tgt.quantity = ord.quantity;
    if (ord.orderReqId > 0) tgt.orderReqId = ord.orderReqId;
    if (ord.OrdAction > 0) tgt.OrdAction = ord.OrdAction;
    if (tgt.price == 0) tgt.price = ord.price;
    if (tgt.stopprice == 0) tgt.stopprice = ord.stopprice;
    if (ord.OrdAlgoTrig > 0) tgt.OrdAlgoTrig = ord.OrdAlgoTrig;
    if (ord.dispqty > 0) tgt.dispqty = ord.dispqty;
    if (ord.minqty > 0 ) tgt.minqty = ord.minqty;

    ProcessOrder(tgt);
}

void OrderManagerV5::minOrderAck(KTN::OrderPod& ord)
{
	minOrderUpdate(ord, KOrderStatus::NEW, KOrderState::WORKING);
	if (ord.exchordid > 0)
	{
		_ords.updateExchOrdId(ord, ord.exchordid);
	}
}
void OrderManagerV5::minOrderCancel(KTN::OrderPod& ord)
{
	minOrderUpdate(ord, KOrderStatus::CANCELED, KOrderState::COMPLETE);
}
void OrderManagerV5::minOrderModify(KTN::OrderPod& ord)
{
	minOrderUpdate(ord, KOrderStatus::MODIFIED, KOrderState::WORKING);

	// Apply modified price (minOrderUpdate only sets price when tgt.price==0,
	// but modify needs to overwrite the existing price with the new one)
	if (ord.price != akl::Price{}) {
		_ords[ord.orderReqId].price = ord.price;
	}
}

//leaving execution to individual algos...
void OrderManagerV5::minOrderExecution(KTN::OrderPod& ord, bool hedgeSent)
{
	if (ord.OrdFillType == KOrderFillType::SPREAD_LEG_FILL)
	{
		minOrderExecSpreadLeg(ord);
		return;
	}

	cout << "[OrderManagerV5 WARNING] minOrderExecution: " << OrderUtils::Print(ord) << endl;

	if (_ords.get(ord.orderReqId) == nullptr)
	{
		ostringstream oss;
		oss << "***ORPHAN FILL: ORD MGR V4 CANNOT FIND ORDER FOR REQ=" << ord.orderReqId
				<< " EXCHID=" << ord.exchordid << " SECID=" << ord.secid;
		LogMe(oss.str(), LogLevel::ERROR);
		return;
	}

	//need to figure out if exchange has cumqty or not...
	//if not, we need to add the lastqty to cumqty...
	//_ords[ord.orderReqId].fillqty += ord.lastqty;

	auto& tgt = _ords[ord.orderReqId];

	tgt.lastpx = ord.lastpx;
	tgt.lastqty = ord.lastqty;
	//tgt.quantity = ord.quantity;
	tgt.fillqty += ord.lastqty;

	tgt.leavesqty = ord.leavesqty;
	tgt.timestamp = ord.timestamp;
	tgt.OrdStatus = ord.OrdStatus;
	tgt.OrdState = ord.OrdState;
	tgt.OrdFillType = ord.OrdFillType;
	OrderUtils::fastCopy(tgt.execid, ord.execid, sizeof(ord.execid));

	ProcessOrder(tgt);
}


void OrderManagerV5::minOrderExecSpreadLeg(KTN::OrderPod& ord)
{
	//note: we already know this comes without a reqid, so...
	ord.quantity = ord.lastqty; //this is the leg qty, not the spread qty

	KTN::OrderPod spreadOrd;
	if (_ords.getByExchOrdId(spreadOrd, ord.exchordid))
	{
		ord.orderReqId = spreadOrd.orderReqId;
	}

	ord.OrdFillType = KOrderFillType::SPREAD_LEG_FILL;
	ProcessOrder(ord);
}

void OrderManagerV5::minOrderReject(KTN::OrderPod& ord, const string& text)
{
	ord.OrdFillType = KOrderFillType::UNKNOWN;
	ProcessRejectedOrder(ord, text);
}

void OrderManagerV5::minOrderCancelReject(KTN::OrderPod& ord, const string& text)
{
	KTN::OrderPod *ordCopyPtr = _ords.get(ord.orderReqId);
	if (ordCopyPtr != nullptr)
	{
		// If we have the order in the map, it could still be working.  Need to notify the UI about new clordid.
		ordCopyPtr->OrdStatus = ord.OrdStatus;
		KTN::OrderPod ordCopy = *ordCopyPtr;
		ordCopy.OrdStatus = ord.OrdStatus;
		ordCopy.OrdAction = ord.OrdAction;
		ProcessRejectedOrder(ordCopy, text);
	}
	else
	{
		ord.OrdFillType = KOrderFillType::UNKNOWN;
		ProcessRejectedOrder(ord, text);
	}
}

void OrderManagerV5::minOrderCancelReplaceReject(KTN::OrderPod& ord, const string& text)
{
	ord.OrdFillType = KOrderFillType::UNKNOWN;

	auto& tgt = _ords[ord.orderReqId];
	tgt.price = ord.price;
	tgt.quantity = ord.quantity;
	tgt.leavesqty = ord.leavesqty;
	tgt.OrdStatus = ord.OrdStatus;
	tgt.OrdState = ord.OrdState;

	ProcessRejectedOrder(ord, text);
}

void OrderManagerV5::ProcessOrderID(uint64_t ordid)
{
	//_qOrdId.enqueue(ordid);
}

void OrderManagerV5::ProcessOrder(OrderPod &ord)
{
	_qOrdProcessed.enqueue(ord);
}

void OrderManagerV5::ProcessSentOrder(OrderPod &ord)
{
	LogMe("PROCESS SENT ORDER IS DEPRECATED!");
}

void OrderManagerV5::ProcessRejectedOrder(OrderPod &ord, const string& text)
{
	_qOrdProcessed.enqueue(ord);
}


void OrderManagerV5::Run()
{
	OrderReporterV6 proc = OrderReporterV6(_owner);
	proc.UpdateMetaParams(_PARAMS);

	_ORDS_ACTIVE.store(true, std::memory_order_relaxed);

	while (_ORDS_ACTIVE.load(std::memory_order_acquire))
	{
		if (_qOrdProcessed.size() > 0)
		{
			OrderPod ord = {};
			while (_qOrdProcessed.dequeue(ord))
			{
				OrderPod* base = _ords.get(ord.orderReqId);
				if (UNLIKELY(base == nullptr))
					continue;

				OrderPod ordcopy = *base;

				// For cancel/replace reject handling: copy values if we have a valid base
				if (ord.OrdAction == KOrderAction::ACTION_CXL_REPL_REJ ||
					ord.OrdAction == KOrderAction::ACTION_CXL_REJ)
				{
					if (ordcopy.exchordid > 0)
					{
						ord.leavesqty = ordcopy.leavesqty;
						ord.fillqty  = ordcopy.fillqty;
						// ord.price     = ordcopy.price;
						// ord.quantity  = ordcopy.quantity;
						ord.exchordid     = ordcopy.exchordid;
					}
					else
					{
						LOGWARN("[OrderManagerV5] Run: Can't find req ID {} for reject", ord.orderReqId);
					}
				}

				if (ord.OrdFillType > 0 && ord.OrdFillType == KOrderFillType::SPREAD_LEG_FILL)
				{
					// Handle spread leg fills
					OrderPod legord = ordcopy;
					OrderUtils::Update(legord, ord);

					// Copy necessary fields from the original order
					legord.OrdAlgoTrig = ordcopy.OrdAlgoTrig;
					legord.stratid     = ordcopy.stratid;
					legord.OrdType     = ordcopy.OrdType;
					legord.OrdTif      = ordcopy.OrdTif;
					legord.OrdSide     = ord.OrdSide;
					legord.price       = ord.lastpx;
					legord.quantity    = ord.lastqty;
					legord.lastpx      = ord.lastpx;
					legord.lastqty     = ord.lastqty;
					legord.fillqty     = ord.lastqty;
					legord.secid       = ord.secid;
					legord.OrdFillType = KOrderFillType::SPREAD_LEG_FILL;
					legord.OrdState    = KOrderState::COMPLETE;

					OrderUtils::fastCopy(legord.execid, ord.execid, sizeof(legord.execid));
					OrderUtils::fastCopy(legord.ordernum, ordcopy.ordernum, sizeof(legord.ordernum));

					proc.Report(legord);
				}
				else
				{
					// 2025-07-01 mwojcik: I don't think we need this, as the order from the queue should already have all the info in the map?
					// I believe this also causes duplicated fills, which overwrites other fills and causes them to be missed.
					//OrderUtils::Update(ordcopy, ord);
					//ord = ordcopy;

					if (ord.stratid == 0)
					{
						auto it = _ugomap.find(ord.orderReqId);
						if (it != _ugomap.end())
							proc.UpdateUGOMap(ord.orderReqId, it->second);
					}

					proc.Report(ord);

					if (ord.OrdState == KOrderState::COMPLETE &&
						ord.OrdFillType == KOrderFillType::OUTRIGHT_FILL)
					{
						_ords.erase(ord.orderReqId); // still safe here if only Run() erases
					}
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}
