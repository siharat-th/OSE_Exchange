/*
 * byte_writer.hpp
 *
 *  Created on: Oct 21, 2016
 *      Author: sgaer
 */

#ifndef BYTE_WRITER2_HPP_
#define BYTE_WRITER2_HPP_

#pragma once

#include <algorithm>
#include <cassert>
#include <cstring>
#include <memory>
#include <inttypes.h>

namespace KTN {
namespace NET {

/**
 * @brief Class for writing bytes to a buffer.
 */
class ByteWriter {
 public:
  /**
   * @brief Default constructor.
   */
  ByteWriter()
  : data_(NULL),
    cur_(NULL),
    data_length_(0),
    cur_length_(NULL)
  {}

  /**
   * @brief Constructor that sets the initial buffer and length.
   * @param data Pointer to the buffer.
   * @param total_data_length Total length of the buffer.
   * @param output_length Pointer to the variable that will store the length of the written data.
   */
  ByteWriter(void* data, uint32_t total_data_length, uint32_t* output_length)
  : data_(static_cast<uint8_t*>(data)),
    cur_(static_cast<uint8_t*>(data)),
    data_length_(total_data_length),
    cur_length_(output_length)
  {}

  /**
   * @brief Destructor.
   */
  virtual ~ByteWriter() {}

  /**
   * @brief Sets the buffer and length for writing.
   * @param data Pointer to the buffer.
   * @param total_data_length Total length of the buffer.
   * @param output_length Pointer to the variable that will store the length of the written data.
   */
  void Set(void* data, uint32_t total_data_length, uint32_t* output_length) {
    data_ = static_cast<uint8_t*>(data);
    cur_  = static_cast<uint8_t*>(data);
    data_length_ = total_data_length;
    cur_length_ = output_length;
  }

  /**
   * @brief Writes a value to the buffer.
   * @tparam T The type of the value to write.
   * @param val The value to write.
   * @return True if the value was successfully written, false otherwise.
   */
  template<typename T>
  bool Write(const T& val) {
    if (data_length_ >= sizeof(T)) {
      data_length_ -= sizeof(T);
      memcpy(cur_, &val, sizeof(T));  // just to enforce that it must be POD
      cur_ += sizeof(T);
      if (cur_length_) {
        *cur_length_ = cur_ - data_;
      }
      return true;
    }
    return false;
  }

  /**
   * @brief Writes a block of data to the buffer.
   * @param val Pointer to the data to write.
   * @param val_length Length of the data to write.
   * @return True if the data was successfully written, false otherwise.
   */
  bool Write(const void* val, uint32_t val_length) {
    if (data_length_ >= val_length) {
      data_length_ -= val_length;
      memcpy(cur_, val, val_length);  // just to enforce that it must be POD
      cur_ += val_length;
      if (cur_length_) {
        *cur_length_ = (uint32_t)(cur_ - data_);
      }
      return true;
    }
    return false;
  }

  /**
   * @brief Moves the write position in the buffer by the specified number of bytes.
   * @param bytes The number of bytes to move the write position. Positive value moves forward, negative value moves backward.
   * @return True if the write position was successfully moved, false otherwise.
   */
  bool Seek(int32_t bytes) {
    if ((bytes > 0 && static_cast<int32_t>(data_length_) >= bytes) ||
        (bytes <= 0 && -bytes <= static_cast<int32_t>(bytes_written())))
    {
      data_length_ -= bytes;
      cur_ += bytes;
      if (cur_length_) {
        *cur_length_ = (uint32_t)(cur_ - data_);
      }
      return true;
    }
    return false;
  }

  /**
   * @brief Resets the write position to the beginning of the buffer.
   */
  void Reset() {
    Seek(-static_cast<int32_t>(bytes_written()));
  }

  /**
   * @brief Gets a pointer to the buffer.
   * @return Pointer to the buffer.
   */
  uint8_t* data() { return data_; }

  /**
   * @brief Gets the number of bytes that were written to the buffer.
   * @return The number of bytes written.
   */
  uint64_t bytes_written() { return (cur_ - data_); }

  /**
   * @brief Gets a pointer to the current write position in the buffer.
   * @return Pointer to the current write position.
   */
  uint8_t* cur() { return cur_; }

  /**
   * @brief Gets the number of bytes left to write in the buffer.
   * @return The number of bytes available for writing.
   */
  uint32_t bytes_available() { return data_length_; }

  /**
   * @brief Gets a pointer to the variable that stores the length of the written data.
   * @return Pointer to the variable that stores the length of the written data.
   */
  uint32_t* cur_length() { return cur_length_; }

 private:
  uint8_t* data_;  ///< Pointer to the buffer
  uint8_t* cur_;  ///< Pointer to the current write position in the buffer
  uint32_t data_length_;  ///< Total length of the buffer
  uint32_t* cur_length_;  ///< Pointer to the variable that stores the length of the written data

  /// DO NOT IMPLEMENT
  ByteWriter(const ByteWriter&);
  /// DO NOT IMPLEMENT
  ByteWriter& operator =(const ByteWriter&);
};

} ; // namespace Net
}  ;// namespace KTN

#endif /* BYTE_WRITER_HPP_ */
