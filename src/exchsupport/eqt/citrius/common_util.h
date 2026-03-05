
/*
 * bitop.h
 *
 *  Created on: May 3, 2010
 *      Author: val
 */

#ifndef BITOP_H_
#define BITOP_H_
#include "limetypes.hh"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
namespace LimeBrokerage {
#endif /* __cplusplus */

#ifndef MIN
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))
#endif

#ifndef MAX
#define MAX(X, Y)  ((X) > (Y) ? (X) : (Y))
#endif

enum BIT_FIELD_SIGN { BIT_FIELD_POSITIVE_SIGN, BIT_FIELD_NEGATIVE_SIGN };

// Used to validate sizeof() structures,
// e.g. COMPILE_TIME_ASSERT(sizeof(book_item)==2*DCACHE_LINE_SIZE)
#define CONCAT(x,y) x##y
#define _CT_ASSERT(expr,unique) char CONCAT(constraint,unique)[(expr)?1:-1]
#define COMPILE_TIME_ASSERT(expr) _CT_ASSERT(expr,__LINE__)

/*
 * This portion defines set of macros that allow bit manipulation.
 * It allow packing and unpacking set of fields into one set of
 * contiguous bits.
 */

//Decode function list of parameters declaration
#define DECODE_DECLARATION_BODY(type, name, value) \
    , type* name##_val

//Decode function declaration
#define DECODE_DECLARATION(name, list) \
    int name##_decode(name##_t* __result \
        list(DECODE_DECLARATION_BODY) \
        )

#define DECODE_DEBUG_DECLARATION(name, list) \
    int name##_decode_debug(name##_t* __result \
        list(DECODE_DECLARATION_BODY) \
        )

//Individual value decoding
#define DECODE_DEFINITION_BODY(type, name, value) \
    __shift_bits -= name; \
    __tmp = *__result >> __shift_bits; \
    __tmp &= (__one << name) - 1; \
    *name##_val = __tmp; \
    __tmp = 0;

#define DECODE_BODY(name, list) \
    { \
        name##_t __tmp = 0;\
        name##_t __one = 1;\
        int __shift_bits = sizeof(name##_t) * 8; \
        list(DECODE_DEFINITION_BODY) \

//Decode function definition
#define DECODE_DEFINITION(name, list) \
    DECODE_DECLARATION(name, list) \
    DECODE_BODY(name, list) \
        return 0; \
    } \

#define POPULATE_RES_STRING(type, name, value) \
    __pos += sprintf(&__output_str[__pos], "%d, ", (int) (* name##_val));

#define PRINT_RESULTS(function, name, list) \
    int __pos = 0; \
    char __output_str[1024*10]; \
    list(POPULATE_RES_STRING) \
    printf("%s: %lld = %s\n", #function, (long long int)(*__result), __output_str); \
    return 0; \

#define DECODE_DEBUG_DEFINITION(name, list) \
    DECODE_DEBUG_DECLARATION(name, list)\
    DECODE_BODY(name, list) \
    PRINT_RESULTS(decode, name, list) \
    } \

//Encode function list of parameters declaration
#define ENCODE_DECLARATION_BODY(type, name, value) \
    , type* name##_val

//Encode function declaration
#define ENCODE_DECLARATION(name, list) \
    int name##_encode(name##_t* __result \
        list(ENCODE_DECLARATION_BODY) \
        )

#define ENCODE_DEBUG_DECLARATION(name, list) \
    int name##_encode_debug(name##_t* __result \
        list(ENCODE_DECLARATION_BODY) \
    )
//Individual value encoding
#define ENCODE_DEFINITION_BODY(type, name, value) \
    __shift_bits -= name; \
    __tmp = *name##_val; \
    __tmp = __tmp << __shift_bits; \
    *__result |= __tmp;

#define ENCODE_BODY(name, list) \
    { \
        name##_t __tmp = 0; \
        int __shift_bits = sizeof(name##_t) * 8; \
        list(ENCODE_DEFINITION_BODY) \

//Encode function definition
#define ENCODE_DEFINITION(name, list) \
    ENCODE_DECLARATION(name, list) \
    ENCODE_BODY(name, list) \
    return 0; \
    }

#define ENCODE_DEBUG_DEFINITION(name, list) \
    ENCODE_DEBUG_DECLARATION(name, list) \
    ENCODE_BODY(name, list) \
    PRINT_RESULTS(encode, name, list) \
    return 0; \
    }

//Allow to sum value of all the bitfields
#define BITFIELD_SUM(type, name, value) \
    + name

//Define individual bit value pairs for enum declaration
#define BITOP_DEFINITION_BODY(type, name, value) \
    name = value,

/*
 * This macro defines set of fields that can be packed into
 * data structure identified by 'name'. It declare enum of bit fields passed as a list.
 * It checks that sum of the
 * bits can fit into the data structure and identifies functions to
 * encode/decode fields.
 * It defines 2 functions:
 * int 'name'_decode(name* _output, type1* var1, type2* var2);
 * int 'name'_encode(name* _output, type1* var1, type2* var2);
 * Total number of parameters in each function depends on total number of elements
 * in definition list *
 * (see spread_manager.c|h for usage example)
 */
#define BITOP_DEFINITION(name, list) \
    enum name##_bits { \
        list(BITOP_DEFINITION_BODY) \
    }; \
    typedef enum name##_bits name##_bits_t; \
    ENCODE_DECLARATION(name, list); \
    DECODE_DECLARATION(name, list);

#define BITOP_COMPILE_TIME_ASSERT(name, list) \
    COMPILE_TIME_ASSERT((sizeof(name##_t) * 8) == \
            0 \
            list(BITFIELD_SUM) \
            );

typedef struct limeq_price {
    int32_t mantissa;
    int8_t exponent;
} limeq_price;

/*
 * This function finds greater common divider accross array of 1 byte integers
 * set -  is array of elements element_size each that contains uint8_t number to calculate greater_common_divider for
 * element_size - is size of the each element in the set
 * element_offset - position of required uint8_t field in the element
 * nmemb - number of elements in the set
 */
uint8_t greater_common_divider(void* set, size_t element_size, size_t element_offset, size_t nmemb);

/*
 * Normalize mantissa of the price
 * Divide all the mantissas by max power of 10 possible
 * Calculate minimum exponent while doing it
 */
void normalize_mantissa(limeq_price* price, int8_t* min_exponent);

/*
 * Recalculate set of prices based on minimum exponent
 */
bool normalize_exponent(limeq_price* set, size_t nmemb, int8_t* min_exponent);

/**
 * Siac specific function shared by API and QS
 */

#define SIAC_SALES_CONDITIONS_LEN 4
#define SIAC_LONG_MSG_SALE_CONDITIONS 1
#define UTDF_LONG_MSG_SALE_CONDITIONS 2
bool decode_siac_sales_conditions(uint8_t siac_sales_conditions, uint8_t additionalProperties, char sales_conditions[SIAC_SALES_CONDITIONS_LEN]);
void encode_siac_sales_conditions(uint8_t *reserved, uint8_t *additional_prop, char sales_conditions[SIAC_SALES_CONDITIONS_LEN]);

bool decode_utdf_sales_conditions(uint8_t siac_sales_conditions, uint8_t additionalProperties, char sales_conditions[4]);
void encode_utdf_sales_conditions(uint8_t *reserved, uint8_t *additional_prop, char sales_conditions[4]);

typedef enum quote_feed_specific_s {
    QUOTE_FEED_SPECIFIC_NONE,
    QUOTE_FEED_SPECIFIC_NYOU,
    QUOTE_FEED_SPECIFIC_MAX
}quote_feed_specific_t;

/*
 * NYOU specific function
 */

typedef enum nyou_data_type_s {
    NYOU_DATA_QUOTE_CONDITION,
    NYOU_DATA_TRADING_STATUS,
    NYOU_DATA_MAX
} nyou_data_type_t;

uint8_t encode_nyou_quote_data(uint8_t quote_condition, uint8_t trading_status);
int decode_nyou_quote_data(uint8_t encoded_data, uint8_t *, uint8_t *);

/**
 * 64 bit hton and ntoh
 */
uint64_t lime_ntohll(uint64_t);
uint64_t lime_htonll(uint64_t);

#ifdef __cplusplus
} //extern "C"
} // namespace LimeBrokerage
#endif /* __cplusplus */
#endif /* BITOP_H_ */

