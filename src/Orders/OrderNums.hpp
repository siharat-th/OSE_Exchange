//============================================================================
// Name        	: OrderNums.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Jan 19, 2024 1:07:59 PM
//============================================================================

#ifndef SRC_ORDERS_ORDERNUMS_HPP_
#define SRC_ORDERS_ORDERNUMS_HPP_

#include <string>
#include <vector>
#include <memory>

#include <KT01/Core/Macro.hpp>
#include <KT01/Core/Singleton.hpp>
#include <KT01/Core/Log.hpp>
#include <KT01/Core/Settings.hpp>

#include <Maps/SPSCRingBuffer.hpp>
using namespace KTN::Core;

#include <Orders/IdGen.h>
#include <Orders/OrderUtils.hpp>
using namespace KTN;

#include <KT01/Helpers/Base63.hpp>
using namespace KT01::Base63;

/**
 * @class OrderNums
 * @brief Singleton class for generating order numbers and request IDs.
 * @note We are making the orderids conform to the Base63 encoding scheme.
 *       This is a 63 character encoding scheme that is used in the CFE
 *      but will be useful for any echange that doesn't provide a similar mechanism
 *      to CME's OrderRequestID. Why? Because we can translate this between uint64_t and string easily
 *      and withouth loss of data in common uint64_t type.
 */
class OrderNums : public Singleton<OrderNums>
{
public:
	/**
	 * @brief Default constructor.
	 */
	OrderNums() : _qOrdIds(_capacity), _qReqIds(_capacity), _LOADED(false)
	{
		Load();
	}

	/**
	 * @brief Destructor.
	 */
	virtual ~OrderNums() {}

	/**
	 * @brief Generates the next order ID.
	 * @return The next order ID.
	 */
	string NextClOrdId()
	{
		string res = "";
		_qOrdIds.dequeue(res);
		return res;
	}

	/**
	 * @brief Generates the next order ID and copies it to the provided character array.
	 * @param ordernum The character array to copy the order ID to.
	 * @param len The length of the character array.
	 */
	void NextClOrdId(char *&ordernum, size_t len)
	{
		string res = "";
		_qOrdIds.dequeue(res);
		OrderUtils::fastCopy(ordernum, res.c_str(), 20);
	}

	/**
	 * @brief Generates the next request ID.  
	 * @return The next request ID.
	 * @note By passing in the clordid, we can enforce a linkage between the clorid and the request ID and vice versa.
	 */
	uint32_t GenOrderReqId(char* clordid)
	{
		uint64_t uniqueMessageID = Base63Codec::decode(clordid);
		return uniqueMessageID;
	}

	uint32_t GenOrderReqId(const std::string& clordid)
	{
		uint64_t uniqueMessageID = Base63Codec::decode(clordid.c_str());
		return uniqueMessageID;
	}

	/**
	 * @brief Generates the next request ID.
	 * @return The next request ID.
	 */
	uint64_t NextReqId()
	{
		uint64_t uniqueMessageID = 0;
		_qReqIds.dequeue(uniqueMessageID);
		return uniqueMessageID;
	}

	/**
	 * @brief Checks if the order numbers have been loaded.
	 * @return True if the order numbers have been loaded, false otherwise.
	 */
	bool isLoaded()
	{
		return _LOADED;
	}

	/**
	 * @brief Gets the size of the order ID queue.
	 * @return The size of the order ID queue.
	 */
	int OrdIdSize()
	{
		return _qOrdIds.size();
	}

	/**
	 * @brief Gets the size of the request ID queue.
	 * @return The size of the request ID queue.
	 */
	int ReqIdSize()
	{
		return _qReqIds.size();
	}

	void Refresh()
	{
		for (uint64_t i = _qOrdIds.size(); i < _capacity; i++)
		{
			auto arr = Base63Codec::encode(baseid);
			std::string s(arr.data(), arr.size());

			_qOrdIds.enqueue(s);
			//_qReqIds.enqueue(baseid); // TODO mwojcik reqIds don't seem to be pulled here on the CFE version?  Verify?
			baseid++;
		}
	}

private:
	/**
	 * @brief Pre-Loads the order numbers.
	 */

	void Load()
	{
		baseid = IdGen::ReqId24();

		for (uint64_t i = 0; i < _capacity; i++)
		{
			auto arr = Base63Codec::encode(baseid);
			std::string s(arr.data(), arr.size());

			_qOrdIds.enqueue(s);

			_qReqIds.enqueue(baseid);
			baseid++;
		}

		_LOADED = true;
	}

private:
	SPSCRingBuffer<string> _qOrdIds;
	SPSCRingBuffer<uint64_t> _qReqIds;

	static constexpr uint64_t _capacity = 2000 * 1000;

	bool _LOADED;
	string _IDBASE;
	int16_t _CNT;
	uint64_t _REQID;
	uint64_t baseid;

	/**
	 * @brief Generates the next order ID.
	 * @return The next order ID.
	 */
	inline string NextId()
	{
		_CNT++;
		char buf[20];
		sprintf(buf, "%s%06d", _IDBASE.c_str(), _CNT);
		string res = buf;
		return res;
	}
};

#endif /* SRC_ORDERS_ORDERNUMS_HPP_ */
