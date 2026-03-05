#ifndef LIMEQ_DEF_H_
#define LIMEQ_DEF_H_

#include "common_util.h"
#ifdef __linux__
#include <inttypes.h>
#include <stdbool.h>
#include <endian.h>
#include <time.h>
#endif

#ifdef __cplusplus
extern "C" {
namespace LimeBrokerage {
#endif /* __cplusplus */

// Common constants.
static const int HEADER_BYTES = 3;
static const char PAD_CHAR = ' ';
static const char NUM_PAD_CHAR = '0';

static const int LIMEQ_MAJOR_VER = 2;
static const int LIMEQ_MINOR_VER = 0;
static const int QUOTE_SERVER_PORT = 7047;
static const int MAX_SUBSCRIPTION_REQUEST = 16*1024;

#define SYMBOL_LEN 21
#define LIME_SYMBOL_LEN 8

typedef enum
{
    AUTH_REQUEST = 'l',
    LOGIN_REQUEST = 'L',
    LOGIN_RESPONSE = 'E',
    LOGOUT_REQUEST = 'O',
    HEARTBEAT = 'H',
    SUBSCRIPTION_REQUEST = 'S',
    SUBSCRIPTION_REPLY = 'P',
    BOOK_REBUILD = 'R',
    ORDER = 'A',
    TRADE = 'T',
    QUOTE_SOURCE_CONTROL = 'M',
    LIMEQ_CONTROL = 'Q',
    TRADING_ACTION = 'X',
    MOD_EXECUTION = 'W',
    ORDER_REPLACE = 'D',
    CLI_MESSAGE = 'K',
    IMBALANCE_MESSAGE = 'I',
    OPTION_ATTRIBUTES = 'B',
    SECURITY_DEFINITION = 'Y',
    TRADE_CANCEL_CORRECT = 'Z',
    SYMBOL_STATUS = 'N',
    FEATURE_CONTROL = 'F',
    FEATURE_CONTROL_RESPONSE = 'G',
    INDEX = 'J',
    SPREAD_DEFINITION = 'C',
    OPTION_ANALYTICS = 'U',
    OPTION_SHOCKED_PNL = 'V',
    PRICE_BAND = '0',
    ROUND_LOT_TOB = '1',
    AUCTION_UPDATE = '2',
    AUCTION_SUMMARY = '3',
    EXT_ORDER = '!',
    EXT_MOD_EXECUTION = '@',
    EXT_ORDER_REPLACE = '#',
    EXT_TRADE = '^',

    // Lower case values are reserved for message types internal
    // to Citrius, used in the quote request queue.
    SYMBOL_ID_MAPPING = 'a',
    QBM_CONTROL_MSG = 'b',
    CLEAR_BOOK_MESSAGE = 'c',
    CLEAR_BOOK_ITER = 'd',
    CLEAR_BOOK_BY_SYMBOL_RANGE_MESSAGE = 'e',
    SUMMARY_DATA = 'f',
    SUBSCRIPTION_DONE = 'g',
    SWEEP_THROUGH_BOOK_MESSAGE = 's',
    INTERNAL_SUBSCRIPTION_REQUEST = 'r', // used internally by the QuoteServer
    SYMBOL_INIT = 'i',
    BOVESPA_THEORETICAL_PRICE_UPDATE = 'o',
    BOVESPA_THEORETICAL_PRICE_LIST_CLEAR = 'p',
    BOOK_DIVIDER = 'e',
    MESSAGE_NONE = 'n',
    REBUILD_VENUE_HASH = 'h',
    REBUILD_SYMBOL_HASH = 'y',
    TRADE_WITH_BROKER_IDS = 't',

    // Optional field messages
    OPTIONAL_FIELDS_BOUNDARY = 200,
    OPTIONAL_EXCHANGE_ORDER_ID = 201,
    OPTIONAL_EXCHANGE_ORDER_IDS_ORDER_REPLACE = 202,
    OPTIONAL_BROKER_IDS = 203,
    OPTIONAL_USEC_TIMESTAMP = 204,
    OPTIONAL_USEC_TIMESTAMP_ORDER_REPLACE = 205
} limeq_message_type;

typedef enum _limeq_control_code
{
    SLOW_MESSAGE_PROCESSING = 'S',
    MESSAGE_DROPPED = 'D',
    START_OF_SESSION = 'B',
    END_OF_SESSION = 'E',
    QUOTE_SOURCE_CONNECTED = 'C',
    QUOTE_SOURCE_DISCONNECTED = 'X',
    EXTENDED_LIMEQ_CONTROL_MSG = 'T'
    //QUOTE_SOURCE_REWIND_START = 'R',
    //QUOTE_SOURCE_REWIND_END = 'W'
} limeq_control_code;

typedef enum
{
    IO_STATS = 'I',
    MSG_COUNT = 'M',
    CHANNEL_STATUS = 'C'
} limeq_control_subtype;

typedef enum _trading_action_code
{
    REG_SHO_UPDATE = '0',
    OPENING_DELAY = '1',
    TRADING_HALT = '2',
    TRADING_RESUME = '3',
    NO_OPEN_NO_RESUME = '4',
    PRICE_INDICATION = '5',
    TRADING_RANGE_INDICATION = '6',
    MARKET_IMBALANCE_BUY = '7',
    MARKET_IMBALANCE_SELL = '8',
    MARKET_ON_CLOSE_IMBALALNCE_BUY = '9',
    MARKET_ON_CLOSE_IMBALALNCE_SELL = 'A',
    NO_MARKET_IMBALANCE = 'C',
    NO_MARKET_ON_CLOSE_IMBALANCE = 'D',
    QUOTE_RESUME = 'Q',
    EXCHANGE_TRADING_HALT = 'V',
    EXCHANGE_QUOTE_RESUME = 'R',
    TRADING_PAUSE = 'P'
} trading_action_code;

typedef enum short_sale_restriction_code {
    SHORT_SALE_RESTRICTION_ACTIVATED = 'a',
    SHORT_SALE_RESTRICTION_IN_EFFECT = 'e',
    SHORT_SALE_RESTRICTION_CONTINUED = 'c',
    SHORT_SALE_RESTRICTION_DEACTIVATED = 'd',
    SHORT_SALE_RESTRICTION_NOT_IN_EFFECT = 'n',
    TRADING_STATUS_UPDATE = 't'
} short_sale_restriction_code;

typedef enum _limeq_feature_index
{
    FEATURE_INDEX_TRADE_REPLAY = 0,
    FEATURE_INDEX_TRADES_ONLY = 1,
    FEATURE_INDEX_TOP_OF_BOOK = 2,
    FEATURE_INDEX_PRICE_AGGREGATED = 3,
    FEATURE_INDEX_FILTER_MMID = 4,
    FEATURE_INDEX_SNAPSHOT_BOOK = 5,
    FEATURE_INDEX_ORDER_VISIBILITY = 6,
    FEATURE_INDEX_BROKER_IDS = 7,
    FEATURE_INDEX_USEC_TIMESTAMP = 8,
    FEATURE_INDEX_MAX = 9,
    FEATURE_INDEX_RATE_LIMIT = 10, // supported only by Options Analytics feed
    FEATURE_INDEX_ENABLE_NAGLE = 11,
    FEATURE_INDEX_ROUND_LOT_TOB = 12,
    FEATURE_INDEX_DISABLE_IMPLIED = 13,
    FEATURE_INDEX_EXTENDED_QUOTE = 14,
    FEATURE_INDEX_ENABLE_IPO_AND_UPLISTED = 15 // allow subscriptions to (IPOs, uplistings etc)
} limeq_feature_index;

typedef enum _limeq_feature_code
{
    FEATURE_TRADE_REPLAY = 1 << FEATURE_INDEX_TRADE_REPLAY,
    FEATURE_TRADES_ONLY = 1 << FEATURE_INDEX_TRADES_ONLY,
    FEATURE_TOP_OF_BOOK = 1 << FEATURE_INDEX_TOP_OF_BOOK,
    FEATURE_PRICE_AGGREGATED = 1 << FEATURE_INDEX_PRICE_AGGREGATED,
    FEATURE_FILTER_MMID = 1 << FEATURE_INDEX_FILTER_MMID,
    FEATURE_SNAPSHOT_BOOK = 1 << FEATURE_INDEX_SNAPSHOT_BOOK,
    FEATURE_ORDER_VISIBILITY = 1 << FEATURE_INDEX_ORDER_VISIBILITY,
    FEATURE_BROKER_IDS = 1 << FEATURE_INDEX_BROKER_IDS,
    FEATURE_USEC_TIMESTAMP = 1 << FEATURE_INDEX_USEC_TIMESTAMP,
    FEATURE_RATE_LIMIT = 1 << FEATURE_INDEX_RATE_LIMIT,
    FEATURE_ENABLE_NAGLE = 1 << FEATURE_INDEX_ENABLE_NAGLE,
    FEATURE_ROUND_LOT_TOB = 1 << FEATURE_INDEX_ROUND_LOT_TOB,
    FEATURE_DISABLE_IMPLIED = 1 << FEATURE_INDEX_DISABLE_IMPLIED,
    FEATURE_EXTENDED_QUOTE = 1 << FEATURE_INDEX_EXTENDED_QUOTE,
    FEATURE_ENABLE_ENABLE_IPO_AND_UPLISTED = 1 << FEATURE_INDEX_ENABLE_IPO_AND_UPLISTED
} limeq_feature_code;

#define MSG_LEN_SIZE 2

#define HEADER_LEN (sizeof(uint16_t) + sizeof(uint8_t))
#define LIMEQ_HEADER  \
    uint16_t msg_len; \
    uint8_t msg_type;

typedef struct limeq_header
{
    LIMEQ_HEADER;
} limeq_header;

typedef enum _app_type
{
    GUI = 'G',
    JAVA_API = 'J',
    CPP_API = 'N',
    RAW_SOCKET = 'R',
    LIMEQ_REPEATER = 'L',
    CLI_SESSION = 'C'
} app_type;

#define UNAME_LEN 32
#define PASSWD_LEN 32

typedef enum auth_types
{
    CLEAR_TEXT = 0
} auth_types;

#define AUTH_TYPE_POS 0
#define AUTH_TYPE_LEN 2
#define HOST_ID_LEN 6

typedef struct login_request_msg
{
    LIMEQ_HEADER;
    //struct {
    uint8_t auth_type : AUTH_TYPE_LEN;
    uint8_t expressSession : 1;
    //} flags;
    char uname[UNAME_LEN];
    char passwd[PASSWD_LEN];
    uint8_t host_id[HOST_ID_LEN];
    uint8_t reserved; /* was "login_flags" */
    uint8_t session_type;
    uint8_t heartbeat_interval;
    uint8_t timeout_interval;
    uint8_t ver_major;
    uint8_t ver_minor;
} login_request_msg;

#define TOKEN_LEN 168
typedef struct auth_request_msg
{

    LIMEQ_HEADER;
    uint8_t session_type;
    uint8_t heartbeat_interval;
    uint8_t timeout_interval;
    uint8_t ver_1;
    uint8_t ver_2;
    uint8_t ver_3;
    char reserved;
    uint16_t revis;
    char uname[UNAME_LEN];
    char token[TOKEN_LEN];
} auth_request_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(auth_request_msg) == (12+UNAME_LEN + TOKEN_LEN));
#endif

typedef enum _reject_reason_code
{
    INVALID_UNAME_PASSWORD = 'A',
    VERSION_MISMATCH = 'V',
    REWIND = 'R',
    ADMIN_DISABLED = 'D',
    ACCOUNT_DISABLED = 'T',
    INSUFFICIENT_PRIVILEGE = 'I',
    INVALID_IP = 'N',
    LOGIN_FAILED = 'F',
    EXCEEDED_MAX_SESSIONS = 'M',
    LOGIN_SUCCEEDED = 'S',
    INVALID_SERVER_GROUP = 'G',
    INVALID_AUTH_METHOD = 'E',
    INVALID_TOKEN_SIGNATURE= 's',
    USERNAME_MISMATCH = 'm',
    IP_ADDRESS_MISMATCH = 'i',
    MARKET_DATA_SERVICE_MISMATCH = 'q',
    AUTH_TOKEN_EXPIRED = 'x',
    DUPLICATE_TOKEN = 'd'
} reject_reason_code;

typedef struct login_response_msg
{
    LIMEQ_HEADER;
    uint8_t ver_major;
    uint8_t ver_minor;
    uint8_t heartbeat_interval;
    uint8_t timeout_interval;
    uint8_t response_code;
} login_response_msg;

typedef struct logout_request_msg
{
    LIMEQ_HEADER;
    uint8_t reserved;
} logout_request_msg;

typedef struct heartbeat_msg
{
    LIMEQ_HEADER;
    uint8_t reserved;
} heartbeat_msg;

#define QUOTE_SOURCE_NAME_LEN 4
#define MMID_LEN 5

typedef char quote_source_id_t[QUOTE_SOURCE_NAME_LEN];
typedef char mmid_t[MMID_LEN];

// Must match enum SymbolType in quoteSystemApi.h
enum subscribe_symbol_type {
    // Symbol is 21 characters: OCC Symbol(6)+Expiration YYMMDD(6)+[P|C](1)+Strike(8).
    // Subscribes to the unique option series with the specified OCC Symbol, expiration, and strike.
    SUBSCRIBE_SYMBOL_TYPE_NORMAL, // 0
    // Symbol is 21 characters: UNDERLYING(6)+Expiration YYMMDD(6)+[P|C](1)+Strike(8).
    // Subscribes to all options with the specified underlying, expiration, and strike.
    SUBSCRIBE_SYMBOL_TYPE_UNDERLYING_ATTRIBUTES, // 1
    // Symbol is an OCC Symbol (OPRA root):
    // Subscribes to all options with the specified class.
    SUBSCRIBE_SYMBOL_TYPE_CLASS, // 2
    // Symbol is an underlying security:
    // Subscribes to all options with the specified underlying.
    SUBSCRIBE_SYMBOL_TYPE_UNDERLYING, // 3
    // Symbol is an index.
    // Used to subscribe all to just indexes.
    SUBSCRIBE_SYMBOL_TYPE_INDEX // 4
};

// Must match enum SubscriptionType in quoteSystemApi.h
enum subscribe_type {
    SUBSCRIBE_TYPE_MARKET_DATA = 1<<0,    // Also used for analytics by Options Analytics feed
    SUBSCRIBE_TYPE_SHOCKED_VALUES = 1<<1, // Only supported by Options Analytics feed
    SUBSCRIBE_TYPE_ATTRIBUTES = 1<<2
};

//
// subscription_request_msg 'flags' has this format:
//
// Bit    0: unsubscribe
// Bits 1-3: symbolType {normal, underlying_attributes, class, underlying, index}
// Bit  4-6: SubscriptionTypeMask {MarketData | Attributes}
// Bit    7: Reserved2
//
#define SUBSCRIBE_TYPE_MASK_BIT_OFFSET 4
#define SUBSCRIBE_TYPE_MASK_ALL (0xf<<SUBSCRIBE_TYPE_MASK_BIT_OFFSET)
#define SYMBOL_TYPE_MASK_BIT_OFFSET 1
#define SYMBOL_TYPE_MASK_ALL (0x7<<SYMBOL_TYPE_MASK_BIT_OFFSET)

//
// Definitions for subscription_request_msg 'flags'.
//
enum subscription_flags
{
    SUBSCRIPTION_FLAG_UNSUBSCRIBE = 1, // 0=subscribe, 1=unsubscribe
    SUBSCRIPTION_FLAG_MARKET_DATA = (SUBSCRIBE_TYPE_MARKET_DATA<<SUBSCRIBE_TYPE_MASK_BIT_OFFSET), // subscribe for quotes&trades (or analytics from Options Analytics feed)
    SUBSCRIPTION_FLAG_SHOCKED_VALUES = (SUBSCRIBE_TYPE_MARKET_DATA<<SUBSCRIBE_TYPE_SHOCKED_VALUES), // subscribe for shocked values (Options Analytics only)
    SUBSCRIPTION_FLAG_ATTRIBUTES = (SUBSCRIBE_TYPE_ATTRIBUTES<<SUBSCRIBE_TYPE_MASK_BIT_OFFSET), // subscribe for attributes
};

// This message is backward compatible with subscription_request_msg.
// We're migrating away from use of bit fields for better portability.
typedef struct subscription_request_msg
{
    LIMEQ_HEADER;
    uint8_t flags;
    quote_source_id_t qsid;
    uint16_t num_symbols;
    //char *sub_symbols;
} subscription_request_msg;

typedef enum _subscription_outcome
{
    LOAD_ALLOCATION_EXCEEDED = 'L',
    MAX_NUMBER_OF_CLIENTS_EXCEEDED = 'M',
    MAX_NUMBER_OF_SYMBOLS_EXCEEDED = 'S',
    QUOTE_SOURCE_NOT_FOUND = 'Q',
    SUBSCRIPTION_SUCCESSFUL = 'P',
    LICENSE_ALLOCATION_EXCEEDED = 'A',
    NO_LICENSE_FOR_QUOTE_SOURCE = 'N',
    SUBSCRIBE_ALL_DISABLED = 'D',
    LICENSE_IP_VALIDATION_FAILED = 'I'
} subscription_outcome;

typedef struct subscription_reply_msg
{
    LIMEQ_HEADER;
    uint8_t outcome;
    quote_source_id_t qsid;
} subscription_reply_msg;

typedef enum book_rebuild_action
{
    BOOK_REBUILD_STARTED = 0,
    BOOK_REBUILD_ENDED = 1
} book_rebuild_action;

typedef enum valid_symbol_indicator
{
    VALID_SYMBOL = 0,
    INVALID_SYMBOL = 1
} valid_symbol_indicator;

typedef struct book_rebuild_msg
{
    LIMEQ_HEADER;
    uint8_t action : 1;
    uint8_t symbol_indicator : 1;
    uint8_t symbol_len : 6;
    quote_source_id_t quote_source_id;
    uint32_t symbol_index;
    char symbol[SYMBOL_LEN];
    uint8_t symbol_type;
    uint16_t channel_id;
} book_rebuild_msg;

// ORDER: Only 2 bits allocated for quote_side, value can't be bigger than 3.
typedef enum quote_side
{
    NONE = 0,
    BUY = 1,
    SELL = 2
} quote_side;

#define QUOTE_SIDE_POS 0
#define QUOTE_SIDE_LEN 2

typedef enum quote_flags {
    QUOTE_FLAGS_TOP_OF_BOOK = 1<<0,
    QUOTE_FLAGS_NBBO = 1<<1,
    QUOTE_FLAGS_PRICE_AGGR = 1<<2
} quote_flags;

// 36 bytes
#define COMMON_DATA                       \
    LIMEQ_HEADER;                         \
    uint8_t side : QUOTE_SIDE_LEN;        \
    uint8_t feed_specific_properties : 6; \
    union {                               \
        uint8_t sales_conditions;         \
        uint8_t quote_flags;              \
        uint8_t byte;                     \
    } u;                                  \
    int8_t price_exponent;                \
    uint8_t additional_prop;              \
    mmid_t mmid;                          \
    uint32_t timestamp;                   \
    uint32_t order_id;                  \
    uint32_t shares;                      \
    int32_t price_mantissa;               \
    quote_source_id_t quote_source_id;    \
    uint32_t symbol_index;

typedef struct common_data
{
    COMMON_DATA;
} common_data;

#define EXCHANGE_ORDER_ID_LEN 8

typedef struct order_msg
{
    COMMON_DATA;
    uint32_t nanoseconds;
    uint64_t exchangeOrderId;
} order_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT((sizeof(order_msg) % 8)==0);
#endif

typedef enum trade_msg_flags_e
{
    TRADE_MSG_FLAGS_HIGH_TICK = 1 << 0,
    TRADE_MSG_FLAGS_LOW_TICK = 1 << 1,
    TRADE_MSG_FLAGS_CANCELLED = 1 << 2,
    TRADE_MSG_FLAGS_CORRECTED = 1 << 3,
    TRADE_MSG_FLAGS_PREVIOUS_SESSION_LAST_TRADE = 1 << 4,
    TRADE_MSG_FLAGS_LAST_TICK = 1 << 5,
    TRADE_MSG_FLAGS_OPEN_TICK = 1 << 6,
    TRADE_MSG_FLAGS_CLOSE_TICK = 1 << 7
} trade_msg_flags_t;

typedef struct trade_msg
{
    COMMON_DATA;//36
    uint32_t total_volume;//40
    uint8_t flags;
    char pad[3];//44
    uint32_t nanoseconds;//48
    uint8_t sale_condition[4];//52
    uint32_t upper_trade_id; //56
} trade_msg;

// Keep in mind that this struct will be padded by compiler
// to 16 bytes.
typedef struct trade_info {
    int32_t price_mantissa;
    uint32_t shares;
    uint32_t timestamp;
    int8_t price_exp;
} trade_info_t;

typedef struct trade_cancel_correct_msg
{
    COMMON_DATA;
    uint32_t updated_total_volume;
    struct {
        int32_t price_mantissa;
        int8_t price_exp;
        uint32_t shares;
        uint32_t timestamp;
        uint32_t nanoseconds;
    } updated_trade;
    uint32_t nanoseconds;
} trade_cancel_correct_msg;

typedef struct mod_execution_msg
{
    COMMON_DATA;
    uint32_t total_volume;
    uint32_t shares_executed;
    int32_t trade_price_mantissa;
    int8_t trade_price_exponent;
    uint8_t trade_flags;
    uint16_t pad;
    uint32_t trade_id;
    uint32_t nanoseconds;
    uint32_t upper_trade_id;
    uint64_t exchange_order_id;
} mod_execution_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT((sizeof(mod_execution_msg) % 8)==0);
#endif

typedef struct order_replace_msg
{
    COMMON_DATA;
    uint32_t old_order_id; //40
    int32_t old_mantissa;
    int8_t old_exponent;
    char old_additional_prop;
    uint16_t pad; //48
    uint32_t nanoseconds;
    uint32_t pad2; //56
    uint64_t old_exchange_order_id; //64
    uint64_t new_exchange_order_id; //72
} order_replace_msg;

typedef struct ext_order_msg
{
    COMMON_DATA;
    uint32_t nanoseconds;
    uint64_t exchangeOrderId;
    uint64_t citrius_timestamp;
    uint64_t exchange_seq_num;
} ext_order_msg;

typedef struct ext_mod_execution_msg
{
    COMMON_DATA;
    uint32_t total_volume;
    uint32_t shares_executed;
    int32_t trade_price_mantissa;
    int8_t trade_price_exponent;
    uint8_t trade_flags;
    uint16_t pad;
    uint32_t trade_id;
    uint32_t nanoseconds;
    uint32_t upper_trade_id;
    uint64_t exchange_order_id;
    uint64_t citrius_timestamp;
    uint64_t exchange_seq_num;
} ext_mod_execution_msg;

typedef struct ext_order_replace_msg
{
    COMMON_DATA;
    uint32_t old_order_id; //40
    int32_t old_mantissa;
    int8_t old_exponent;
    char old_additional_prop;
    uint16_t pad; //48
    uint32_t nanoseconds;
    uint32_t pad2; //56
    uint64_t old_exchange_order_id; //64
    uint64_t new_exchange_order_id; //72
    uint64_t citrius_timestamp;
    uint64_t exchange_seq_num;
} ext_order_replace_msg;

typedef struct ext_trade_msg
{
    COMMON_DATA;//36
    uint32_t total_volume;//40
    uint8_t flags;
    char pad[3];//44
    uint32_t nanoseconds;//48
    uint8_t sale_condition[4];//52
    uint32_t upper_trade_id; //56
    uint64_t citrius_timestamp;
    uint64_t exchange_seq_num;
} ext_trade_msg;

typedef struct quote_source_ctl_msg
{
    LIMEQ_HEADER;
    uint8_t message_code;
    quote_source_id_t qsid;
} quote_source_ctl_msg;

#define LIMEQ_CONTROL_COMMON_DATA      \
    LIMEQ_HEADER;                      \
    uint8_t code;                      \
    quote_source_id_t quote_source_id; \
    uint16_t common_data_len;          \
    uint16_t sub_type;

typedef struct limeq_control_common_data
{
    LIMEQ_CONTROL_COMMON_DATA;
}limeq_control_common_data;

typedef struct limeq_control_msg
{
    LIMEQ_CONTROL_COMMON_DATA;
    char byte_array[4];
} limeq_control_msg;

typedef struct channel_status_msg_s
{
    LIMEQ_CONTROL_COMMON_DATA;
    uint8_t pad_1;
    uint8_t status_type;
    uint16_t channel_id;
    uint32_t skipped_messages;
    uint32_t timestamp;
} channel_status_msg_t;

typedef enum channel_status_type_s {
    CHANNEL_STATUS_TYPE_LOSS
}channel_status_type_t;

typedef enum imbalance_type {
    IMBALANCE_TYPE_UNKNOWN = 0,
    IMBALANCE_TYPE_OPEN = 1, // ARCA, NYSE
    IMBALANCE_TYPE_MARKET = 2, // ARCA
    IMBALANCE_TYPE_HALT = 3, // ARCA
    IMBALANCE_TYPE_CLOSE = 4, // ARCA, NYSE
    IMBALANCE_TYPE_NO = 5 // NYSE
} imbalance_type_t;

typedef struct
{
    LIMEQ_HEADER;
    mmid_t exchange_id;
    uint8_t side : QUOTE_SIDE_LEN;
    uint8_t reserved : 6;
    int8_t price_exponent;
    uint8_t imbalance_type;
    uint8_t pad;
    uint32_t timestamp;
    uint32_t shares;
    int32_t price_mantissa;
    quote_source_id_t quote_source_id;
    uint32_t symbol_index;
    int32_t total_imbalance;
    int32_t market_imbalance;
    uint32_t feed_specific_data;
    uint32_t nanoseconds;
    uint32_t closing_clearing_price_numerator;
} imbalance_msg ;

#define OPTION_SYMBOL_LEN 6
#define OCC_NAME_LEN OPTION_SYMBOL_LEN

// New symbology option name, which is also ASCII encoded option attributes.
typedef struct option_name {
    char symbol[OPTION_SYMBOL_LEN]; // Option class, blank padded.
    char expiration_year[2]; // YY
    char expiration_month[2]; // MM
    char expiration_day[2]; // DD
    char put_call; // 'P' or 'C'
    char strike_price[5]; // Whole number
    char strike_decimal[3]; // Fraction
} option_name_t;

#define OPTION_NAME_LEN 21
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(option_name_t)==OPTION_NAME_LEN);
#endif /* __cplusplus */

#define OPRA_NAME_LEN 5
typedef char opra_name_t[OPRA_NAME_LEN];

//
// Options Attribute Message.
//
typedef struct option_attributes_message
{
    LIMEQ_HEADER;
    char reserved1;
    quote_source_id_t quote_source_id;
    char underlying_name[OPTION_SYMBOL_LEN];
    option_name_t option_name;
    opra_name_t opra_name;
    uint32_t open_interest;
    uint32_t symbol_index;
    uint16_t quoting_exchanges;
    uint16_t prev_quoting_exchanges; // exchanges quoting on previous day
    uint32_t underlying_index;
    uint32_t series_index;
} option_attributes_message_t;

#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(option_attributes_message_t)==(8+OPTION_SYMBOL_LEN+OPTION_NAME_LEN+OPRA_NAME_LEN+20));
#endif /* __cplusplus */

enum limeq_quote_alert_flag_e
{
    LIMEQ_QUOTE_ALERT_FLAG_PASS_THROUGH_STATE = 1 << 0
};

typedef struct quote_alert_msg
{
    LIMEQ_HEADER;
    uint8_t halted_state;
    uint32_t timestamp;
    uint32_t symbol_index;
    quote_source_id_t quote_source_id;
    uint8_t flags;
    uint8_t short_sale_restriction_indicator;
    uint8_t reserve[2];
    uint32_t nanoseconds;
} quote_alert_msg;

typedef struct index_msg {
    LIMEQ_HEADER;
    int8_t value_exponent;
    int32_t value_mantissa;
    quote_source_id_t quote_source_id;
    uint32_t symbol_index;
    uint32_t timestamp;
    uint32_t nanoseconds;
} index_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(index_msg) == 24);
#endif

typedef struct price_band_msg
{
    LIMEQ_HEADER;
    int8_t up_price_exponent;
    uint32_t up_price_mantissa;
    int8_t down_price_exponent;
    char pad[3];
    uint32_t down_price_mantissa;
    uint32_t timestamp;
    uint32_t effective_timestamp;
    uint32_t symbol_index;
    quote_source_id_t quote_source_id;
    //uint32_t nanoseconds_;
} price_band_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(price_band_msg) == 32);
#endif

#define SEC_DEF_MSG_PAD_SIZE 3

typedef struct security_definition_msg {
    LIMEQ_HEADER;
    uint8_t sec_def_type;
    uint32_t timestamp;
    uint32_t symbol_index;
    quote_source_id_t quote_source_id;
    // Common fields
    uint32_t common_fields_len;
    int32_t previous_day_closing_price_mant;
    int8_t previous_day_closing_price_exp;
    // Actual feed specific message to follow.
    char msg[SEC_DEF_MSG_PAD_SIZE];
    uint32_t nanoseconds;
} security_definition_msg_t;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(security_definition_msg_t) == 32);
#endif

typedef struct auction_update_message
{
    LIMEQ_HEADER;
    uint32_t symbol_index;
    uint32_t timestamp;
    quote_source_id_t quote_source_id;
    uint8_t auction_type;
    int32_t reference_price_mant;
    int8_t reference_price_exp;
    uint32_t buy_shares;
    uint32_t sell_shares;
    int32_t indicative_price_mant;
    int8_t indicative_price_exp;
    int32_t auction_only_price_mant;
    int8_t auction_only_price_exp;
} auction_update_message_t;

typedef struct auction_summary_message
{
    LIMEQ_HEADER;
    uint32_t symbol_index;
    uint32_t timestamp;
    quote_source_id_t quote_source_id;
    uint8_t auction_type;
    int32_t price_mant;
    int8_t price_exp;
    uint32_t shares;
} auction_summary_message_t;

//CLI_MESSAGE
#define CLI_MESSAGE_LEN_LEN 4
typedef struct cli_message
{
    LIMEQ_HEADER;
    uint8_t padding;
    uint16_t cli_msg_len;
//  char *cli_msg;
} cli_message;

#define FEATURE_SPECIFIC_DATA_SIZE 64
typedef struct feature_control_message {
    LIMEQ_HEADER;
    uint8_t padding;
    uint16_t feature_code;
    char feature_specific_data[FEATURE_SPECIFIC_DATA_SIZE];
} feature_control_message;

typedef enum feature_control_response_e
{
    FEATURE_CONTROL_RESPONSE_ENABLED,
    FEATURE_CONTROL_RESPONSE_DISABLED
} feature_control_response_t;

typedef struct feature_control_response_msg {
    LIMEQ_HEADER;
    uint8_t pad;
    uint16_t feature_code;
    uint8_t response;
    char pad2;
} feature_control_response_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(feature_control_response_msg) == 8);
#endif

typedef enum {
    SYMBOL_STATUS_FLAGS_INVALID_HIGH = 1 << 0,
    SYMBOL_STATUS_FLAGS_INVALID_LOW = 1 << 1
} symbol_status_flags_t;

typedef struct symbol_status_msg {
    LIMEQ_HEADER;
    int8_t high_trade_price_exponent;
    uint32_t symbol_index;
    int32_t high_trade_price_mantissa;
    int32_t low_trade_price_mantissa;
    quote_source_id_t quote_source_id;
    int8_t low_trade_price_exponent;
    uint8_t exchange_id;
    uint8_t symbol_type;
    int8_t open_exponent;
    uint16_t close_date;
    int8_t close_exponent;
    int8_t last_exponent;
    int32_t open_mantissa;
    int32_t close_mantissa;
    int32_t last_mantissa;
} symbol_status_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(symbol_status_msg) == 40);
#endif

//Spread Info Message
//This message conveys spread info
#define UNDERLYING_SYMBOL_NAME_LEN 6
#define MAX_LEGS 5

typedef struct spread_def_msg spread_def_msg_t;
typedef struct spread_def_data spread_def_data_t;

typedef uint64_t leg_def_data_t;
typedef uint16_t spread_data_t;
/**
 * The biggest value of this enum can't be bigger than SPREAD_REQUEST_LEG_TYPE_BITS
 * Using SPREAD_LEG_TYPE_EQUITY as a bigest value add values before it
 */
enum leg_type {
    SPREAD_LEG_TYPE_PUT,
    SPREAD_LEG_TYPE_CALL,
    SPREAD_LEG_TYPE_EQUITY
};
typedef enum leg_type leg_type_t;

/**
 * The biggest value of this enum can't be bigger than SPREAD_REQUEST_SIDE_BITS
 * Using SPREAD_SIDE_SELL_SHORT as a bigest value add values before it
 */
enum leg_side {
    SPREAD_SIDE_BUY = 1,
    SPREAD_SIDE_SELL = 2,
    SPREAD_SIDE_SELL_SHORT = 3
};
typedef enum leg_side leg_side_t;
#define BASE_YEAR 2000

#define LEG_INFO_REQUEST_BITS_LIST(_) \
    _(uint8_t, SPREAD_REQUEST_PAD, 1) \
    _(uint8_t, SPREAD_REQUEST_LEG_TYPE_BITS, 2) \
    _(uint8_t, SPREAD_REQUEST_EXPIRATION_YEAR_BITS, 8) \
    _(uint8_t, SPREAD_REQUEST_EXPIRATION_MONTH_BITS, 4) \
    _(uint8_t, SPREAD_REQUEST_EXPIRATION_DAY_BITS, 5) \
    _(uint8_t, SPREAD_REQUEST_STRIKE_MANTISSA_SIGN_BITS, 1) \
    _(uint32_t, SPREAD_REQUEST_STRIKE_MANTISSA_BITS, 31) \
    _(uint8_t, SPREAD_REQUEST_SIDE_BITS, 2) \
    _(uint16_t, SPREAD_REQUEST_RATIO_BTIS, 10)

BITOP_DEFINITION(leg_def_data, LEG_INFO_REQUEST_BITS_LIST);
ENCODE_DEBUG_DECLARATION(leg_def_data, LEG_INFO_REQUEST_BITS_LIST);
DECODE_DEBUG_DECLARATION(leg_def_data, LEG_INFO_REQUEST_BITS_LIST);

#ifndef __cplusplus
BITOP_COMPILE_TIME_ASSERT(leg_def_data, LEG_INFO_REQUEST_BITS_LIST);
#endif

#define SPREAD_DATA_BITS_LIST(_) \
        _(uint8_t, SPREAD_DATA_PAD, 1) \
        _(uint8_t, SPREAD_DATA_NUM_LEGS_BITS, 3) \
        _(uint8_t, SPREAD_DATA_RATIO_GCD, 8) \
        _(uint8_t, SPREAD_DATA_COMMON_EXPONENT_SIGN, 1) \
        _(uint8_t, SPREAD_DATA_COMMON_EXPONENT, 3)

BITOP_DEFINITION(spread_data, SPREAD_DATA_BITS_LIST);

#ifndef __cplusplus
BITOP_COMPILE_TIME_ASSERT(spread_data, SPREAD_DATA_BITS_LIST);
COMPILE_TIME_ASSERT(SPREAD_LEG_TYPE_EQUITY < (1<<SPREAD_REQUEST_LEG_TYPE_BITS));
COMPILE_TIME_ASSERT(SPREAD_SIDE_SELL_SHORT < (1<<SPREAD_REQUEST_SIDE_BITS));
#endif

struct spread_def_data {
        spread_data_t spread_common_data; //2 BYTES
        char underlying_name[UNDERLYING_SYMBOL_NAME_LEN]; //6 BYTES
        //char spread_name[8];
        leg_def_data_t legs[MAX_LEGS]; //8*5 = 40 BYTES
};
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(spread_def_data_t) == 48);
#endif

struct spread_def_msg {
        LIMEQ_HEADER;
        char pad;
        quote_source_id_t quote_source_id;
        spread_def_data_t spread_info;
        char spread_name[SYMBOL_LEN];
        char pad1[3];
};
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(spread_def_msg_t) == 80);
#endif

// Boundary message used to signal the end of optional field messages.
typedef struct optional_fields_boundary_msg {
    LIMEQ_HEADER;
    char pad;
} optional_fields_boundary_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(optional_fields_boundary_msg) == 4);
#endif

// Optional field messages.
typedef struct optional_exchange_order_id_msg {
    LIMEQ_HEADER;
    char exchange_order_id[EXCHANGE_ORDER_ID_LEN];
    char pad;
} optional_exchange_order_id_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(optional_exchange_order_id_msg) == 12);
#endif

typedef struct optional_exchange_order_ids_order_replace_msg {
    LIMEQ_HEADER;
    char old_exchange_order_id[EXCHANGE_ORDER_ID_LEN];
    char new_exchange_order_id[EXCHANGE_ORDER_ID_LEN];
    char pad;
} optional_exchange_order_ids_order_replace_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(optional_exchange_order_ids_order_replace_msg) == 20);
#endif

#define BROKER_ID_BUYER_LEN 8
#define BROKER_ID_SELLER_LEN 8

typedef struct optional_broker_ids_msg {
    LIMEQ_HEADER;
    char buyer_id[BROKER_ID_BUYER_LEN];
    char seller_id[BROKER_ID_SELLER_LEN];
    char pad;
} optional_broker_ids_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(optional_broker_ids_msg) == 20);
#endif

typedef struct optional_usec_timestamp_msg {
    LIMEQ_HEADER;
    char pad;
    uint16_t usec_timestamp;
    char pad2[2];
} optional_usec_timestamp_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(optional_usec_timestamp_msg) == 8);
#endif

typedef struct optional_usec_timestamp_order_replace_msg {
    LIMEQ_HEADER;
    char pad;
    uint16_t old_usec_timestamp;
    uint16_t new_usec_timestamp;
} optional_usec_timestamp_order_replace_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(optional_usec_timestamp_order_replace_msg) == 8);
#endif

typedef struct option_analytics_msg {
    LIMEQ_HEADER;
    char pad;
    uint32_t timestamp;
    uint32_t symbol_index;
    int32_t delta_scaled;
    int32_t gamma_scaled;
    int32_t theta_scaled;
    int32_t vega_scaled;
    uint32_t volatility;
    uint32_t option_price_scaled;
    uint32_t underlying_price_scaled;
} option_analytics_msg;
#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(option_analytics_msg) == 40);
#endif

typedef struct option_shocked_pnl_msg {
    LIMEQ_HEADER;
    char pad;
    uint32_t timestamp;
    uint32_t symbol_index;
    uint32_t neg_shocked_values_scaled[5];
    uint32_t pos_shocked_values_scaled[5];
    uint32_t volatility;
    uint32_t option_price_scaled;
    uint32_t underlying_price_scaled;
    int8_t max_negative_shock;
    int8_t max_positive_shock;
    int8_t reserved1;
    int8_t reserved2;
} option_shocked_pnl_msg;

typedef struct RoundLotTob_s {
    LIMEQ_HEADER;
    uint8_t             side;
    quote_source_id_t   quoteSourceId;
    uint64_t            timestamp;
    uint32_t            symbolIndex;
    uint32_t            priceAggregatedTobMantissa;
    uint32_t            priceAggregatedTobSize;
    uint32_t            oddLotAggregatedTobMantissa;
    uint32_t            oddLotAggregatedTobSize;
    uint32_t            oddLotFilteredTobMantissa;
    uint32_t            oddLotFilteredTobSize;
    int8_t              priceAggregatedTobExponent;
    int8_t              oddLotAggregatedTobExponent;
    int8_t              oddLotFilteredTobExponent;
    char                pad;
} RoundLotTobMsg;

typedef enum security_type_s {
    SECURITY_TYPE_EQUITY = 1,
    SECURITY_TYPE_OPTION = 1<<1,
    SECURITY_TYPE_FUTURES = 1<<2,
    SECURITY_TYPE_INDEX = 1<<3,
    SECURITY_TYPE_MAX = 1<<4
} security_type_t;

#ifndef __cplusplus
COMPILE_TIME_ASSERT(sizeof(option_shocked_pnl_msg) == 68);
#endif

#ifdef __cplusplus
} //extern "C"
} // namespace LimeBrokerage
#endif /* __cplusplus */

#endif /* LIMEQ_DEF_H_ */
