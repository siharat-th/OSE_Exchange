//============================================================================
// Name        	: OrderRingBuffer.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Sep 21, 2023 12:53:30 PM
//============================================================================

#include <Maps/OrderRingBuffer.hpp>

OrderRingBuffer::OrderRingBuffer(size_t capacity) : capacity_(capacity) {
    keys_ = new uint64_t[capacity_];
    values_ = new KTN::Order[capacity_];
    valueCounts_ = new size_t[capacity_];
    for (size_t i = 0; i < capacity_; ++i) {
        valueCounts_[i] = 0;
    }
}

OrderRingBuffer::~OrderRingBuffer() {
    delete[] keys_;
    delete[] values_;
    delete[] valueCounts_;
}

void OrderRingBuffer::insert(const uint64_t& key, const KTN::Order& value) {
    size_t index = write_index_;
    keys_[index] = key;
    values_[index] = value;
    valueCounts_[index] = 1; // You are inserting a single value

    write_index_ = (write_index_ + 1) % capacity_;
}

bool OrderRingBuffer::lookup(const uint64_t& key, std::vector<KTN::Order>& values) const {
    for (size_t i = 0; i < capacity_; ++i) {
        if (keys_[i] == key) {
            values.assign(&values_[i], &values_[i] + valueCounts_[i]);
            return true;
        }
    }
    return false;
}

//void OrderRingBuffer::prune(const std::string& key) {
//    for (size_t i = 0; i < capacity_; ++i) {
//        if (keys_[i] == key) {
//            // Clear the vector associated with the key
//            values_[i].clear();
//            valueCounts_[i] = 0;
//            return;  // Assuming only one entry per key, remove this line if multiple entries are possible
//        }
//    }
//}

//void OrderRingBuffer::clear() {
//	keys_ = new std::string[capacity_];
//	values_ = new KTN::Order*[capacity_];
//	valueCounts_ = new size_t[capacity_];
//	for (size_t i = 0; i < capacity_; ++i) {
//		values_[i] = new KTN::Order[capacity_];  // Assuming a square matrix-like structure
//		valueCounts_[i] = 0;
//	}
//}
