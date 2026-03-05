//============================================================================
// Name        	: IL3Pool.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 25, 2023 10:47:03 AM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3POOL_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3POOL_HPP_

#pragma once

#pragma once
#include <string>
#include <vector>
#include <string>       // std::string
#include <iostream>
#include <sstream>

#include <unordered_map>

#include <KT01/Core/Macro.hpp>

#include <exchsupport/cme/il3/IL3Messages2.hpp>
using namespace KTN::CME::IL3;
using namespace std;

/**
 * @class NewOrdPool
 * @brief ILink3 New Order Pool pre-allocates a pool of NewOrderSingleMsg objects to be used by the ILink3 handler.
 */
class NewOrdPool
{
public:
    /**
     * @class NewOrdPool
     * @brief ILink3 New Order Pool pre-allocates a pool of NewOrderSingleMsg objects to be used by the ILink3 handler.
     */
    NewOrdPool()
    {
        pool.reserve(10000);
    }

    /**
     * @brief Add a NewOrderSingleMsg object to the pool.
     * @param msg The NewOrderSingleMsg object to add.
     */
    void add(NewOrderSingleMsg * msg)
    {
        pool.push_back(*msg);
        max = pool.size()-1;
    }

    /**
     * @brief Get a NewOrderSingleMsg object from the pool.
     * @return A pointer to the NewOrderSingleMsg object.
     */
    NewOrderSingleMsg* get()
    {
        count ++;
        if (UNLIKELY(count >= max))
        {
            count = 0;
        }
        return &pool[count];
    }

private:
	 // We could've chosen to use a std::array that would allocate the memory on the stack instead of the heap.
	// We would have to measure to see which one yields better performance.
	// It is good to have objects on the stack but performance starts getting worse as the size of the pool increases.

	std::vector<NewOrderSingleMsg> pool;
	size_t count = 0;
	size_t max = 0;
};

/**
    * @class ModOrdPool
    * @brief ILink3 Modify Order Pool pre-allocates a pool of OrderCancelReplaceRequestMsg objects to be used by the ILink3 handler.
    */
class ModOrdPool
{
public:
    /**
     * @brief Constructor for ModOrdPool class.
     * Initializes the ModOrdPool object and reserves memory for the pool.
     */
    ModOrdPool()
    {
        pool.reserve(10000);
    }

    /**
     * @brief Add an OrderCancelReplaceRequestMsg object to the pool.
     * @param msg The OrderCancelReplaceRequestMsg object to add.
     */
    void add(OrderCancelReplaceRequestMsg* msg)
    {
        pool.push_back(msg);
        max = pool.size() - 1;
    }

    /**
     * @brief Get an OrderCancelReplaceRequestMsg object from the pool.
     * @return A pointer to the OrderCancelReplaceRequestMsg object.
     */
    OrderCancelReplaceRequestMsg* get()
    {
        count++;
        if (UNLIKELY(count >= max))
        {
            count = 0;
        }
        return pool[count];
    }

private:
 // We could've chosen to use a std::array that would allocate the memory on the stack instead of the heap.
	// We would have to measure to see which one yields better performance.
	// It is good to have objects on the stack but performance starts getting worse as the size of the pool increases.
	std::vector<OrderCancelReplaceRequestMsg*> pool;
	size_t count = 0;
	size_t max = 0;

};

//template<typename MsgType>
struct MsgBatch
{
	KTN::CME::IL3::NewOrderSingleMsg* msg[10];
	uint64_t reqid[10];
	double ratio[10];
	int16_t taketicks[10];
	size_t len;
	uint8_t segid[10];


	int8_t callbackid[10];
	int8_t instindex[10];
};





#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3POOL_HPP_ */
