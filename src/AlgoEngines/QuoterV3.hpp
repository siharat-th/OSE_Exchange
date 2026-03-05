//============================================================================
// Name        	: QuoterV3.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Nov 10, 2023 4:11:48 PM
//============================================================================

#ifndef SRC_ALGOENGINES_QUOTETERV3_HPP_
#define SRC_ALGOENGINES_QUOTETERV3_HPP_
#pragma once

#include <queue>
#include <cstdint>

#include <Orders/IdGen.h>

#include <AlgoSupport/algo_structs.hpp>
#include <AlgoSupport/AlgoHdgEncoder.hpp>
#include <KT01/Core/Macro.hpp>
#include <KT01/Core/Log.hpp>
#include <KT01/Core/MemPool.hpp>
#include <KT01/Core/StringSplitter.hpp>

using namespace KTN::Core;

#include <exchsupport/cme/il3/IL3Includes.hpp>
#include <exchsupport/cme/factory/AppMessageFactoryFastV2.hpp>
#include <exchsupport/cme/il3/IL3Types.hpp>
#include <exchsupport/cme/il3/IL3Composites.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>
#include <exchsupport/cme/il3/IL3Headers.hpp>
#include <exchsupport/cme/il3/IL3Messages2.hpp>
#include <exchsupport/cme/il3/IL3AppHelpers.hpp>
#include <exchsupport/cme/il3/IL3Pool.hpp>
using namespace KTN::CME::IL3;

#include <Orders/Order.hpp>
#include <Orders/OrderPod.hpp>
#include <Orders/OrderNums.hpp>
using namespace KTN;

#include <Maps/SPSCRingBuffer.hpp>
using namespace KTN::Core;
//#include <tbb/concurrent_queue.h>
//using namespace tbb;


/**
 * @class QuoterV3
 * @brief Represents a QuoterV3 class that manages quoting orders and functions.
 */
class QuoterV3 {
public:
	/**
  * @brief Default constructor for QuoterV3.
  */
	QuoterV3();

	/**
  * @brief Destructor for QuoterV3.
  */
	virtual ~QuoterV3();

	/**
  * @brief Constructs a QuoterV3 object.
  * @param cbid The cbid value.
  * @param inst The instrument value.
  * @param meta The algometa value.
  */
	void Construct(int cbid, instrument inst, algometa meta);

	/**
  * @brief Gets the OrderPod object.
  * @param ord The OrderPod object to get.
  */
	void Get(KTN::OrderPod& ord);

	/**
  * @brief Retrieve a new OrderPod object.
  * @param ord The OrderPod object to create.
  * @param side The KOrderSide value.
  * @param exchordid The exchordid value.
  * @param qty The quantity value.
  * @param price The price value.
  */
	void NextNew(KTN::OrderPod& ord, KOrderSide::Enum side, uint64_t exchordid, uint32_t qty, int price );

	/**
  * @brief Modifies an existing OrderPod object.
  * @param ord The OrderPod object to modify.
  * @param side The KOrderSide value.
  * @param reqid The reqid value.
  * @param exchordid The exchordid value.
  * @param qty The quantity value.
  * @param price The price value.
  */
	void NextMod(KTN::OrderPod& ord, KOrderSide::Enum side, uint64_t reqid, uint64_t exchordid, uint32_t qty, int price );

	/**
  * @brief Creates a new blank Order object.
  * @return The new blank Order object.
  */
	inline KTN::Order NewBlank()
	{
		return _NewTEMP;
	}

	/**
  * @brief Gets the template Order object.
  * @return The template Order object.
  */
	inline KTN::Order GetTemplate()
	{
		return _BaseTEMP;
	}

	/**
  * @brief Gets the total count.
  * @return The total count.
  */
	inline int Total()
	{
		return _DEQ_CNT;
	}

	/**
  * @brief Generates the next ID.
  * @return The next ID.  Way faster now using OrderNums singleton.
  */
	inline string NextId()
	{
		_CNT++;
		return OrderNums::instance().NextClOrdId(); // TODO mwojcik double check this
		
		// char buf[20];
		// sprintf(buf,"%s%06d",_IDBASE.c_str(),_CNT);
		// string res = buf;
		// return res;
	}

	inline void SetStartingReqId(uint64_t reqid)
	{
		_REQ_ID = reqid;
	}

	inline size_t GetOrderQueueSize()
	{
		return _qOrds.size();
	}

	inline size_t GetOrderQueueCapacity()
	{
		return _qOrds.capacity();
	}

	/**
  * @brief Fills the queue.
  */
	void FillQueue();

private:
	/**
  * @brief Logs the message.
  * @param szMsg The message to log.
  * @param iColor The color value.
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

	/**
  * @brief Creates the ID base.
  */
	inline void CreateIdBase()
	{
		_CNT = 0;
		string uuid = IdGen::UUID(6);

		char buf[20];
		sprintf(buf,  "%03d:%s",_PARAMS.StratId, uuid.c_str());

		//_IDBASE = buf;
	}

	/**
  * @brief Generates the next ReqId.
  * @return The next ReqId.
  */
	inline uint64_t NextReqId()
	{
		//we do this for a reason. First, we have about 1,000,000 possible reqids.
		//next, we provide for 24 hedges to easily calculate new ids for autohedging etc.
		_REQ_ID+=25;
		return _REQ_ID;
	}

	/**
  * @brief Gets the last ReqId.
  * @return The last ReqId.
  */
	inline uint64_t LastReqId()
	{
		return _REQ_ID;
	}

	/**
  * @brief Increments the ReqId.
  * @param nbrToInc The number to increment.
  */
	inline void IncReqId(int nbrToInc)
	{
		_REQ_ID += nbrToInc;
	}

private:
	SPSCRingBuffer<KTN::OrderPod> _qOrds;

	std::unordered_map<string, uint64_t > _clOrdIdMap;
	std::unordered_map<uint64_t, string > _reqIdMap;

	int MAX_OBJECTS = 20000;

	uint64_t _REQ_ID { 1 };
	uint16_t _HDG_REQ_ID;

	KTN::Order _BaseTEMP;
	KTN::Order _NewTEMP;
	KTN::Order _ModTEMP;
	instrument _inst;
	algometa _PARAMS;

	int _CALLBACK_ID;
	string _SOURCE;
	string _TAG;

	//string _IDBASE;
	int16_t _CNT;

	int16_t _DEQ_CNT;
};

#endif /* SRC_ALGOENGINES_QUOTETEMPLATE_HPP_ */
