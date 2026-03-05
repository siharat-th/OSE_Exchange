//============================================================================
// Name        	: OrderHashTable.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 2, 2023 7:18:03 PM
//============================================================================

#include <Maps/OrderHashTable.hpp>



OrderHashTable::OrderHashTable(size_t initial_size)
: size_(initial_size), count_(0), table_(initial_size)//, hash_function_()
{

}

OrderHashTable::~OrderHashTable() {
	// TODO Auto-generated destructor stub
}

void OrderHashTable::insert(const string &key, const KTN::Order &value)
{
	if (count_ >= size_ * LOAD_FACTOR) {
	        resize_table();
	}
	//size_t index = hash_function_(key.data(), key.size()) % size_;
    size_t index = hash_function_(key) % size_;
    for (auto& entry : table_[index]) {
        if (entry.first == key) {
            entry.second = value;
            return;
        }
    }
    table_[index].emplace_back(key, value);
    count_++;
}

bool OrderHashTable::lookup(const string &key, KTN::Order &value) const {

	//size_t index = hash_function_(key.data(), key.size()) % size_;
	size_t index = hash_function_(key) % size_;
//	cout << "SEARCHING FOR " << key << endl;
	for (const auto& entry : table_[index]) {
		if (entry.first == key) {
			value = entry.second;
			value.isFound = true;
//			cout << "**FOUND " << key << " RETURNING TRUE" << endl;
//			cout << OrderUtils::Print(value) << endl;
			return true;
		}
	}

	value.isFound = false;
	value.exchordid = 0;


	//cout << "!! NOT FOUND " << key << " RETURNING FALSE" << endl;
	return false;
}

bool OrderHashTable::all(vector<KTN::Order>& result) {

	bool found = false;
	for (const auto& bucket : table_) {
	    for (const auto& entry : bucket) {
	      if (entry.second.leavesqty > 0) {
	        result.push_back(entry.second);
	        found = true;
	      }
	    }
	  }
	return found;
}


void OrderHashTable::erase(const string &key) {
	//size_t index = hash_function_(key.data(), key.size()) % size_;
	size_t index = hash_function_(key) % size_;
    auto& bucket = table_[index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->first == key) {
            bucket.erase(it);
            count_ --;
            return;
        }
    }
}

void OrderHashTable::clear() {
    table_.clear();
    count_ = 0;
}

void OrderHashTable::resize_table() {
    // Double the size of the hash table
    size_t new_size = size_ * 2;
    std::vector<std::vector<std::pair<string, KTN::Order>>> new_table(new_size);

    // Rehash all of the existing entries
    for (auto& bucket : table_) {
        for (auto& entry : bucket) {
        	//size_t index = hash_function_(entry.first.data(), entry.first.size()) % size_;
            size_t index = hash_function_(entry.first) % new_size;
            new_table[index].emplace_back(entry);
        }
    }

    // Update the hash table and size
    table_ = std::move(new_table);
    size_ = new_size;
}
