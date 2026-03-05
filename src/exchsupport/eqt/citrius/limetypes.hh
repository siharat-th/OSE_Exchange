#ifndef LIME_TYPES_HH
#define LIME_TYPES_HH

#if defined (_WIN32)
#include <BaseTsd.h>

typedef SSIZE_T ssize_t;
typedef INT8 int8_t;
typedef UINT8 uint8_t;
typedef INT16 int16_t;
typedef UINT16 uint16_t;
typedef INT32 int32_t;
typedef UINT32 uint32_t;
typedef INT64 int64_t;
typedef UINT64 uint64_t;

#elif defined (__linux__)
#include <stdint.h>
#include <stdbool.h>
#endif

#endif // LIME_TYPES_HH
