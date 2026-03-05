/*
 * netops.hpp
 *
 *  Created on: Oct 21, 2016
 *      Author: sgaer
 */

#ifndef NETOPS_HPP_
#define NETOPS_HPP_

#include <inttypes.h>
#include <stddef.h>

namespace LWT {
namespace NET {

// Convert from host to network byte order
uint16_t ToNetworkUInt16(uint16_t value);

// Convert from host to network byte order
uint32_t ToNetworkUInt32(uint32_t value);

// Convert from netwoek to host byte order
uint16_t ToHostUInt16(uint16_t value);

// Convert from netwoek to host byte order
uint32_t ToHostUInt32(uint32_t value);

namespace detail {
template <typename T>
T Swap2(T value) {
  // this is the freebsd implementation -- is this faster?
  // u_char *s = (u_char *)&x;
  // return (uint32_t)(s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3]);

  // we need the bytes
  union {
    T pod;
    char bytes[sizeof(T)];
  } input, output;
  input.pod = value;
  output.bytes[0] = input.bytes[1];
  output.bytes[1] = input.bytes[0];
  return output.pod;
};
template <typename T>
T Swap4(T value) {
  // we need the bytes
  union {
    T pod;
    char bytes[sizeof(T)];
  } input, output;
  input.pod = value;
  output.bytes[0] = input.bytes[3];
  output.bytes[1] = input.bytes[2];
  output.bytes[2] = input.bytes[1];
  output.bytes[3] = input.bytes[0];
  return output.pod;
};
template <typename T>
T Swap8(T value) {
  // we need the bytes
  union {
    T pod;
    char bytes[sizeof(T)];
  } input, output;
  input.pod = value;
  output.bytes[0] = input.bytes[7];
  output.bytes[1] = input.bytes[6];
  output.bytes[2] = input.bytes[5];
  output.bytes[3] = input.bytes[4];
  output.bytes[4] = input.bytes[3];
  output.bytes[5] = input.bytes[2];
  output.bytes[6] = input.bytes[1];
  output.bytes[7] = input.bytes[0];
  return output.pod;
};

template <typename T, size_t SIZE> struct SwapEndianStruct;
template <typename T> struct SwapEndianStruct<T, 1> { inline static T Swap(T val) { return val;        } };
template <typename T> struct SwapEndianStruct<T, 2> { inline static T Swap(T val) { return Swap2(val); } };
template <typename T> struct SwapEndianStruct<T, 4> { inline static T Swap(T val) { return Swap4(val); } };
template <typename T> struct SwapEndianStruct<T, 8> { inline static T Swap(T val) { return Swap8(val); } };

}
template <typename T>
T SwapEndian(T value) {
  return detail::SwapEndianStruct<T, sizeof(T)>::Swap(value);
}

}  // namespace Net
}  // namespace LWT


#endif /* NETOPS_HPP_ */
