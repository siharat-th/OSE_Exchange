//============================================================================
// Name        	: CustomHFTMap.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 27, 2023 7:34:08 PM
//============================================================================

#include <Maps/CustomHFTMap.hpp>

CustomHFTMap::CustomHFTMap()// : index_(0)
{
	// for (size_t i = 0; i < capacity_; i++)
	// {
	// 	values_[i] = new OrderPod();
	// 	keys_[i] = 0;
	// }
}

KTN::OrderPod &CustomHFTMap::operator[](uint64_t key)
{
	return map[key];
	// for (size_t i = 0; i < index_; i++)
	// {
	// 	if (keys_[i] == key)
	// 	{
	// 		return *(values_[i]);
	// 	}
	// }

	// keys_[index_] = key;
	// values_[index_] = new OrderPod(); // Initialize a new Order

	// if (index_ >= capacity_)
	// 	index_ = 0;

	// index_++;

	// // Return a reference to the newly inserted value
	// return *(values_[index_ - 1]);
}

void CustomHFTMap::clear()
{
	map.clear();
	// for (size_t i = 0; i < capacity_; i++)
	// {
	// 	values_[i] = new OrderPod();
	// 	keys_[i] = 0;
	// }
	// index_ = 0;
}

void CustomHFTMap::cleardead()
{
	// for (size_t i = 0; i < index_; i++)
	// {
	// 	if (values_[i]->leavesqty == 0 || values_[i]->OrdState == KOrderState::COMPLETE)
	// 	{
	// 		values_[i] = new OrderPod();
	// 		keys_[i] = 0;
	// 	}
	// }
}

void CustomHFTMap::erase(uint64_t key)
{
	// for (size_t i = 0; i < index_; i++)
	// {
	// 	if (keys_[i] == key)
	// 	{
	// 		keys_[i] = 0;
	// 		values_[i] = new OrderPod();
	// 		return;
	// 	}
	// }
}

bool CustomHFTMap::find(uint64_t key) const
{
	return map.find(key) != map.cend();
	// for (size_t i = 0; i < index_; i++)
	// {
	// 	//	DBG_FILE_LINE
	// 	if (keys_[i] == key)
	// 	{
	// 		return true;
	// 	}
	// }
	// return false;
}

// void CustomHFTMap::insert(uint64_t key, OrderPod *value)
// {
// 	for (size_t i = 0; i < index_; i++)
// 	{
// 		if (keys_[i] == key)
// 		{
// 			values_[i] = value;
// 			return;
// 		}
// 	}

// 	keys_[index_] = key;
// 	values_[index_] = value;
// 	index_++;

// 	if (index_ >= capacity_)
// 		index_ = 0;
// }

KTN::OrderPod *CustomHFTMap::get(uint64_t key)
{
	OrderMapType::iterator it = map.find(key);
	if (it != map.end())
	{
		return &(it->second);
	}
	// for (size_t i = 0; i < index_; i++)
	// {
	// 	if (keys_[i] == key)
	// 	{
	// 		// Check if the order is valid before returning it
	// 		if (LIKELY(values_[i]->quantity > 0))
	// 		{
	// 			return values_[i];
	// 		}
	// 		else
	// 		{
	// 			// If the order is not valid, return a pointer to a default-constructed Order
	// 			static OrderPod defaultOrder;KTN
	// 			return &defaultOrder;
	// 		}
	// 	}
	// }

	// Key not found, return a pointer to a default-constructed Order
	static KTN::OrderPod defaultOrder;
	return &defaultOrder;
}

bool CustomHFTMap::getByOrderNum(KTN::OrderPod &ord, const std::string &ordernum)
{
	OrderNumMapType::const_iterator it = orderNumMap.find(ordernum);
	if (it != orderNumMap.end())
	{
		uint64_t orderReqId = it->second;
		OrderMapType::iterator orderIt = map.find(orderReqId);
		if (orderIt != map.end())
		{
			ord = orderIt->second;
			return true;
		}
	}
	// for (auto &pair : map)
	// {
	// 	if (pair.second.ordernum == ordernum){
	// 		ord = pair.second;
	// 		return true;
	// 	}
	// }
	return false;
}

bool CustomHFTMap::getByExchOrdId(KTN::OrderPod &ord, uint64_t exchordid)
{
	ExchOrdIdMapType::const_iterator it = exchOrdIdMap.find(exchordid);
	if (it != exchOrdIdMap.end())
	{
		uint64_t orderReqId = it->second;
		OrderMapType::iterator orderIt = map.find(orderReqId);
		if (orderIt != map.end())
		{
			ord = orderIt->second;
			return true;
		}
	}

	// for (auto &pair : map)
	// {
	// 	if (pair.second.exchordid == exchordid ){
	// 		ord = pair.second;
	// 		return true;
	// 	}
	// }
	return false;
}

void CustomHFTMap::getBySecId(std::vector<KTN::OrderPod> &ords, int32_t secid)
{
	for (auto &pair : map)
	{
		if (pair.second.secid == secid)
		{
			ords.push_back(pair.second);
		}
	}
}

void CustomHFTMap::updateOrderNum(KTN::OrderPod &ord)
{
	if (ord.ordernum[0] != '\0')
	{
		orderNumMap[std::string(ord.ordernum)] = ord.orderReqId;
	}
}

void CustomHFTMap::updateExchOrdId(KTN::OrderPod &ord, uint64_t exchordid)
{
	exchOrdIdMap[exchordid] = ord.orderReqId;
}


