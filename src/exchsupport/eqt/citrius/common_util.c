/*
 * common_util.c
 *
 *  Created on: May 28, 2010
 *      Author: val
 */

#include <stdio.h>
#include "common_util.h"
#include "limeq_def.h"
#ifdef __linux__
#include <arpa/inet.h>
#include "byteswap.h"
#endif
#ifdef WIN32
#include <winsock2.h>
#include <Windows.h>
#endif

#ifdef __cplusplus
extern "C" {
namespace LimeBrokerage {
#endif /* __cplusplus */

const int power10[] = {
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000
};
const int power10_size = sizeof(power10)/sizeof(power10[0]);

static const uint8_t _prime_numbers[] = {
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
    53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107,
    109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
    173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233,
    239, 241, 251
};
static const uint8_t _prime_numbers_size = sizeof(_prime_numbers)/sizeof(_prime_numbers[0]);

/*
 * Extract specific element from array of alements size sturct_size
 */
uint8_t* _get_element(void* array, int array_pos, size_t struct_size, size_t field_offset)
{
    return (uint8_t*)((uint8_t*)array+(array_pos * struct_size) + field_offset);
}
#define MAX_ELEMENTS 1024
static uint8_t divided_set[MAX_ELEMENTS];
/*
 * Calculate greatest common divider
 */
uint8_t greater_common_divider(void* set, size_t element_size, size_t element_offset, size_t nmemb)
{
    uint8_t res = 1;
    size_t es = element_size;
    size_t eo = element_offset;
    void* current_set = set;
    if (nmemb > MAX_ELEMENTS) return 1;

    for (int i = 0; i < _prime_numbers_size; i++) {
        bool all_divided = true;

        for (unsigned int j = 0; j < nmemb; j++) {

            uint8_t* element = _get_element(current_set, j, es, eo);
            if (_prime_numbers[i] > *element) {
                return res;
            }
            if (((*element) % _prime_numbers[i])  == 0) {
                divided_set[j] = (*element) / _prime_numbers[i];
            }
            else {
                all_divided = false;
            }
        }
        if (all_divided) {
            res *= _prime_numbers[i];
            current_set = divided_set;
            es = 1;
            eo = 0;
        }
    }
    return res;
}
/*
 * Normalize mantissa of the price point
 */
void normalize_mantissa(limeq_price* price, int8_t* min_exponent)
{
    //Normalize mantissa by removing trailing zeroes
    int zeroes = 0;
    for (int i = 1; i < power10_size; i++) {
        if ((price->mantissa % power10[i]) == 0) {
            zeroes = i;
        }
        else {
            break;
        }
    }
    price->mantissa /= power10[zeroes];
    price->exponent += zeroes;
    if (abs(price->exponent) > power10_size) {
        //Exponent can't be bigger than that
        int diff = price->exponent - power10_size;
        price->mantissa *= power10[diff];
        price->exponent += diff;
    }
    *min_exponent = MIN(*min_exponent, price->exponent);
}
/*
 * Normalize pricess to minimum exponent
 */
bool normalize_exponent(limeq_price* set, size_t nmemb, int8_t* min_exponent)
{
    for (unsigned int i = 0; i < nmemb; i++) {
        int8_t exp = set[i].exponent - *min_exponent;
        if (abs(exp) > power10_size) {
            printf("Exponent is too big %d\n", exp);
            return false;
        }
        if (exp < 0) {
            set[i].mantissa /= power10[exp * -1];
        }
        else {
            set[i].mantissa *= power10[exp];
        }
        set[i].exponent = *min_exponent;
    }
    return true;
}

static const char p_nyou_quote_conditions[] = {
    ' ', /* No Quote Condition */
    'E', /* Slow on the Bid due to LRP or GAP Quote */
    'F', /* Slow on the Bid due to LRP or GAP Quote */
    'U', /* Slow on the Bid and Ask due to LRP or GAP Quote */
    'W',  /* Slow Quote due to a Set Slow list on both the bid and offer sides */
};

static const char p_nyou_trading_status[] = {
    'P', /* Pre-Opening for messages sent before the stock is opened on a trade or quote */
    'O', /* The stock has opened or re-opened */
    'C', /* The stock was closed from the Closing template */
    'H', /* The stock is halted during a trading halt and has not resumed */
};

//static const char *nyou_data[] = { p_nyou_quote_conditions, p_nyou_trading_status };

/*
 * we only have 6 bits to pass this information
 */
uint8_t encode_nyou_quote_data(uint8_t quote_condition, uint8_t trading_status)
{
    uint8_t retVal = 0;
    switch(quote_condition)  {
    case ' ': /* No Quote Condition */
        retVal = 0;
        break;
    case 'E': /* Slow on the Bid due to LRP or GAP Quote */
        retVal = 1;
        break;
    case 'F': /* Slow on the Bid due to LRP or GAP Quote */
        retVal = 2;
        break;
    case 'U': /* Slow on the Bid and Ask due to LRP or GAP Quote */
        retVal = 3;
        break;
    case 'W':  /* Slow Quote due to a Set Slow list on both the bid and offer sides */
        retVal = 4;
        break;
    }

    switch (trading_status){
    case 'P': /* Pre-Opening for messages sent before the stock is opened on a trade or quote */
        retVal |= (0<<3);
        break;
    case 'O': /* The stock has opened or re-opened */
        retVal |= (1<<3);
        break;
    case 'C': /* The stock was closed from the Closing template */
        retVal |= (2<<3);
        break;
    case 'H': /* The stock is halted during a trading halt and has not resumed */
        retVal |= (3<<3);
        break;
    }
    return retVal;
}

int decode_nyou_quote_data(uint8_t encoded_data,
        uint8_t *p_quote_condition, uint8_t *p_trading_status)
{
    uint8_t encoded_quote_condition = encoded_data & 0x07;
    uint8_t encoded_trading_status = (encoded_data & 0x38) >> 3;
    *p_quote_condition = *p_trading_status = 0;

    if(encoded_quote_condition > sizeof(p_nyou_quote_conditions)) {
        return -1;
    }

    if(encoded_trading_status > sizeof(p_nyou_trading_status)) {
        return -1;
    }

    *p_quote_condition = p_nyou_quote_conditions[encoded_quote_condition];
    *p_trading_status = p_nyou_trading_status[encoded_trading_status];
    return 0;
}
/*
 * This function encodes 4 characters into 2 bytes
 * Each character assigned value from 0 - 15 and then encoded into higher or lower bits of either sales_cond or addtional_properties
 */
void encode_utdf_sales_conditions(uint8_t* sales_cond, uint8_t* additional_prop, char sales_conditions[4])
{
    *sales_cond = 0;
    *additional_prop = 0;

    //Lower 4 bits
    switch(sales_conditions[0]) {
    case '@':
        *sales_cond |= 15 << 0;
        break;
    case 'C':
        *sales_cond |= 14 << 0;
        break;
    case 'N':
        *sales_cond |= 13 << 0;
        break;
    case 'R':
        *sales_cond |= 12 << 0;
        break;
    case 'Y':
        *sales_cond |= 11 << 0;
        break;
    case ' ':
        *sales_cond |= 0 << 0;
        break;
    }

    //higher 4 bits
    switch(sales_conditions[1]) {
    case 'F':
        *sales_cond |= 15 << 4;
        break;
    case 'O':
        *sales_cond |= 14 << 4;
        break;
    case '4':
        *sales_cond |= 13 << 4;
        break;
    case '5':
        *sales_cond |= 12 << 4;
        break;
    case '6':
        *sales_cond |= 11 << 4;
        break;
    case '7':
        *sales_cond |= 10 << 4;
        break;
    case '8':
        *sales_cond |= 9 << 4;
        break;
    case '9':
        *sales_cond |= 8 << 4;
        break;
    case ' ':
        *sales_cond |= 0 << 4;
        break;
    }

    switch(sales_conditions[2]) {
    case 'T':
        *additional_prop |= 15 << 0;
        break;
    case 'L':
        *additional_prop |= 14 << 0;
        break;
    case 'Z':
        *additional_prop |= 13 << 0;
        break;
    case 'U':
        *additional_prop |= 12 << 0;
        break;
    case ' ':
        *sales_cond |= 0 << 0;
        break;
    }

    switch(sales_conditions[3]) {
    case '1':
        *additional_prop |= 15 << 4;
        break;
    case 'A':
        *additional_prop |= 14 << 4;
        break;
    case 'B':
        *additional_prop |= 13 << 4;
        break;
    case 'D':
        *additional_prop |= 12 << 4;
        break;
    case 'E':
        *additional_prop |= 11 << 4;
        break;
    case 'G':
        *additional_prop |= 10 << 4;
        break;
    case 'H':
        *additional_prop |= 9 << 4;
        break;
    case 'K':
        *additional_prop |= 8 << 4;
        break;
    case 'M':
        *additional_prop |= 7 << 4;
        break;
    case 'P':
        *additional_prop |= 6 << 4;
        break;
    case 'Q':
        *additional_prop |= 5 << 4;
        break;
    case 'S':
        *additional_prop |= 4 << 4;
        break;
    case 'V':
        *additional_prop |= 3 << 4;
        break;
    case 'W':
        *additional_prop |= 2 << 4;
        break;
    case 'X':
        *additional_prop |= 1 << 4;
        break;
    case ' ':
        *sales_cond |= 0 << 4;
        break;
    }
}

bool decode_utdf_sales_conditions(uint8_t utdf_sales_conditions,
        uint8_t additionalProperties,
        char sales_conditions[4]) {
    //Siac trade properties
    uint8_t byte_0 = utdf_sales_conditions & 0xF;
    uint8_t byte_1 = ((utdf_sales_conditions & 0xF0) >> 4);
    uint8_t byte_2 = (additionalProperties & 0xF);
    uint8_t byte_3 = ((additionalProperties & 0xF0) >> 4);
    bool res = true;
    switch (byte_0) {
    case 15:
        sales_conditions[0] = '@';
        break;
    case 14:
        sales_conditions[0] = 'C';
        break;
    case 13:
        sales_conditions[0] = 'N';
        break;
    case 12:
        sales_conditions[0] = 'R';
        break;
    case 11:
        sales_conditions[0] = 'Y';
        break;
    case 0:
        sales_conditions[0] = ' ';
        break;
    default:
        sales_conditions[0] = 13;
        res = false;
        break;
    }

    switch (byte_1) {
    case 15:
        sales_conditions[1] = 'F';
        break;
    case 14:
        sales_conditions[1] = 'O';
        break;
    case 13:
        sales_conditions[1] = '4';
        break;
    case 12:
        sales_conditions[1] = '5';
        break;
    case 11:
        sales_conditions[1] = '6';
        break;
    case 10:
        sales_conditions[1] = '7';
        break;
    case 9:
        sales_conditions[1] = '8';
        break;
    case 8:
        sales_conditions[1] = '9';
        break;
    case 0:
        sales_conditions[1] = ' ';
        break;
    default:
        sales_conditions[1] = 13;
        res = false;
        break;
    }

    switch (byte_2) {
    case 15:
        sales_conditions[2] = 'T';
        break;
    case 14:
        sales_conditions[2] = 'L';
        break;
    case 13:
        sales_conditions[2] = 'Z';
        break;
    case 12:
        sales_conditions[2] = 'U';
        break;
    case 0:
        sales_conditions[2] = ' ';
        break;
    default:
        sales_conditions[2] = 13;
        res = false;
        break;
    }

    switch (byte_3) {
    case 15:
        sales_conditions[3] = '1';
        break;
    case 14:
        sales_conditions[3] = 'A';
        break;
    case 13:
        sales_conditions[3] = 'B';
        break;
    case 12:
        sales_conditions[3] = 'D';
        break;
    case 11:
        sales_conditions[3] = 'E';
        break;
    case 10:
        sales_conditions[3] = 'G';
        break;
    case 9:
        sales_conditions[3] = 'H';
        break;
    case 8:
        sales_conditions[3] = 'K';
        break;
    case 7:
        sales_conditions[3] = 'M';
        break;
    case 6:
        sales_conditions[3] = 'P';
        break;
    case 5:
        sales_conditions[3] = 'Q';
        break;
    case 4:
        sales_conditions[3] = 'S';
        break;
    case 3:
        sales_conditions[3] = 'V';
        break;
    case 2:
        sales_conditions[3] = 'W';
        break;
    case 1:
        sales_conditions[3] = 'X';
        break;
    case 0:
        sales_conditions[3] = ' ';
        break;
    default:
        //      printf("Unknown byte 0 of trade conditions: %d = %c", byte_1, byte_1);
        sales_conditions[3] = 13;
        res = false;
        break;
    }
    return res;
}

void encode_siac_sales_conditions(uint8_t *reserved, uint8_t *additional_prop,
        char sales_conditions[SIAC_SALES_CONDITIONS_LEN]) {
    *reserved = 0;
    *additional_prop = 0;

    //Setting 0 byte of siac sale conditions
    switch (sales_conditions[0]) { //Enconding bit 0-3
    case '@':
        *reserved |= 15 << 0;
        break;
    case ' ':
        *reserved |= 14 << 0;
        break;
    case 'C':
        *reserved |= 13 << 0;
        break;
    case 'N':
        *reserved |= 12 << 0;
        break;
    case 'R':
        *reserved |= 11 << 0;
        break;
    default:
        *reserved |= 0 << 0;
        break;
    }
    //Setting 1st byte of siac sale conditions
    //This field occupy last 4 bits of reserve byte
    switch (sales_conditions[1]) {
    case ' ':
        *reserved |= 15 << 4;
        break;
    case 'F':
        *reserved |= 14 << 4;
        break;
    case 'O':
        *reserved |= 13 << 4;
        break;
    case '4':
        *reserved |= 12 << 4;
        break;
    case '5':
        *reserved |= 11 << 4;
        break;
    case '6':
        *reserved |= 10 << 4;
        break;
    case '7':
        *reserved |= 9 << 4;
        break;
    case '8':
        *reserved |= 8 << 4;
        break;
    case '9':
        *reserved |= 7 << 4;
        break;
    default:
        *reserved |= 0 << 4;
        break;

    }
    //3rd byte of siac sale conditions
    //This byte occupy 1st 4 bits of additional_prop byte
    switch (sales_conditions[2]) {
    case ' ':
        *additional_prop |= 15 << 0;
        break;
    case 'L':
        *additional_prop |= 14 << 0;
        break;
    case 'T':
        *additional_prop |= 13 << 0;
        break;
    case 'U':
        *additional_prop |= 12 << 0;
        break;
    case 'Z':
        *additional_prop |= 11 << 0;
        break;
    default:
        *additional_prop |= 0 << 0;
        break;

    }
    //4th byte of siac sale conditions
    //This byte occupy 2nd 4 bits of additional_prop byte
    switch (sales_conditions[3]) {
    case ' ':
        *additional_prop |= 15 << 4;
        break;
    case 'B':
        *additional_prop |= 14 << 4;
        break;
    case 'E':
        *additional_prop |= 13 << 4;
        break;
    case 'G':
        *additional_prop |= 12 << 4;
        break;
    case 'H':
        *additional_prop |= 11 << 4;
        break;
    case 'I':
        *additional_prop |= 10 << 4;
        break;
    case 'K':
        *additional_prop |= 9 << 4;
        break;
    case 'P':
        *additional_prop |= 8 << 4;
        break;
    case 'Q':
        *additional_prop |= 7 << 4;
        break;
    case 'X':
        *additional_prop |= 6 << 4;
        break;
    default:
        *additional_prop |= 0 << 4;
        break;

    }
}

bool decode_siac_sales_conditions(uint8_t siac_sales_conditions,
        uint8_t additionalProperties,
        char sales_conditions[SIAC_SALES_CONDITIONS_LEN]) {
    //Siac trade properties
    uint8_t byte_0 = siac_sales_conditions & 0xF;
    uint8_t byte_1 = ((siac_sales_conditions & 0xF0) >> 4);
    uint8_t byte_2 = (additionalProperties & 0xF);
    uint8_t byte_3 = ((additionalProperties & 0xF0) >> 4);
    bool res = true;
    switch (byte_0) {
    case 15:
        sales_conditions[0] = '@';
        break;
    case 14:
        sales_conditions[0] = ' ';
        break;
    case 13:
        sales_conditions[0] = 'C';
        break;
    case 12:
        sales_conditions[0] = 'N';
        break;
    case 11:
        sales_conditions[0] = 'R';
        break;
    default:
        //      printf("Unknown byte 0 of trade conditions: %d = %c", byte_0, byte_0);
        sales_conditions[0] = 13;
        res = false;
        break;
    }

    switch (byte_1) {
    case 15:
        sales_conditions[1] = ' ';
        break;
    case 14:
        sales_conditions[1] = 'F';
        break;
    case 13:
        sales_conditions[1] = 'O';
        break;
    case 12:
        sales_conditions[1] = '4';
        break;
    case 11:
        sales_conditions[1] = '5';
        break;
    case 10:
        sales_conditions[1] = '6';
        break;
    case 9:
        sales_conditions[1] = '7';
        break;
    case 8:
        sales_conditions[1] = '8';
        break;
    case 7:
        sales_conditions[1] = '9';
        break;
    default:
        //      printf("Unknown byte 1 of trade conditions: %d = %c", byte_1, byte_1);
        sales_conditions[1] = 13;
        res = false;
        break;
    }

    switch (byte_2) {
    case 15:
        sales_conditions[2] = ' ';
        break;
    case 14:
        sales_conditions[2] = 'L';
        break;
    case 13:
        sales_conditions[2] = 'T';
        break;
    case 12:
        sales_conditions[2] = 'U';
        break;
    case 11:
        sales_conditions[2] = 'Z';
        break;
    default:
        //      printf("Unknown byte 0 of trade conditions: %d = %c", byte_1, byte_1);
        sales_conditions[2] = 13;
        res = false;
        break;
    }

    switch (byte_3) {
    case 15:
        sales_conditions[3] = ' ';
        break;
    case 14:
        sales_conditions[3] = 'B';
        break;
    case 13:
        sales_conditions[3] = 'E';
        break;
    case 12:
        sales_conditions[3] = 'G';
        break;
    case 11:
        sales_conditions[3] = 'H';
        break;
    case 10:
        sales_conditions[3] = 'I';
        break;
    case 9:
        sales_conditions[3] = 'K';
        break;
    case 8:
        sales_conditions[3] = 'P';
        break;
    case 7:
        sales_conditions[3] = 'Q';
        break;
    case 6:
        sales_conditions[3] = 'X';
        break;
    case 5:
        sales_conditions[3] = 'M';
        break;
    case 4:
        sales_conditions[3] = 'V';
        break;
    case 3:
        sales_conditions[3] = '@';
        break;
    default:
        //printf("Unknown byte 0 of trade conditions: %d = %c", byte_1, byte_1);
        sales_conditions[3] = 13;
        res = false;
        break;
    }
    return res;
}

ENCODE_DEFINITION(leg_def_data, LEG_INFO_REQUEST_BITS_LIST);
DECODE_DEFINITION(leg_def_data, LEG_INFO_REQUEST_BITS_LIST);

ENCODE_DEFINITION(spread_data, SPREAD_DATA_BITS_LIST);
DECODE_DEFINITION(spread_data, SPREAD_DATA_BITS_LIST);

#ifndef __bswap_64
#define __bswap_64(x) (((int64_t)(ntohl((int)((x << 32) >> 32))) << 32) | (unsigned int)ntohl(((int)(x >> 32))))
#endif //__linux__

# if BYTE_ORDER == LITTLE_ENDIAN

#  define htobe64(x) __bswap_64 (x)
#  define htole64(x) (x)
#  define be64toh(x) __bswap_64 (x)
#  define le64toh(x) (x)
# else

#  define htobe64(x) (x)
#  define htole64(x) __bswap_64 (x)
#  define be64toh(x) (x)
#  define le64toh(x) __bswap_64 (x)
# endif

uint64_t lime_ntohll(uint64_t val) {
    return be64toh(val);
}
uint64_t lime_htonll(uint64_t val) {
    return htobe64(val);
}

#ifdef __cplusplus
} //extern C
} //namespace LimeBrokerage
#endif
