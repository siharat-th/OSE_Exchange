

/*     Type definitions of OMnet records.		*/
/*     Version 4.1-1430					*/
/*     Created  Thu Dec  1 07:07:46 2022		*/

/*     Definitions of sub structures			*/


#ifndef __OMEX_OSE_H
#define __OMEX_OSE_H
#if !(defined(INTLIST) && defined(QUADWORDS))
typedef   struct { char quad_word[8]; } quad_word;
#else
#define quad_word quadword
#endif


#include "om_inttypes.h"
#include "oal_align_packed.h"

/*   TRANSACTION TYPE = EB11  */
/*   NAMED STRUCT = 50000  */

typedef struct series
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} series_t;


typedef struct transaction_type
{
   char central_module_c;
   char server_type_c;
   uint16_t transaction_number_n;
} transaction_type_t;


typedef struct broadcast_type
{
   char central_module_c;
   char server_type_c;
   uint16_t transaction_number_n;
} broadcast_type_t;


/*   NAMED STRUCT = 50016  */

typedef struct trading_code
{
   char country_id_s [2];
   char ex_customer_s [5];
   char user_id_s [5];
} trading_code_t;


typedef struct time_spec
{
   uint32_t tv_sec;
   int32_t tv_nsec;
} time_spec_t;


typedef struct transaction_type_low
{
   transaction_type_t transaction_type;
} transaction_type_low_t;


typedef struct transaction_type_high
{
   transaction_type_t transaction_type;
} transaction_type_high_t;


typedef struct series_fields_used
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} series_fields_used_t;


typedef struct partition_low
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} partition_low_t;


typedef struct partition_high
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} partition_high_t;


typedef struct order_var
{
   int64_t mp_quantity_i;
   int32_t premium_i;
   uint32_t block_n;
   uint16_t time_validity_n;
   uint16_t exch_order_type_n;
   char ex_client_s [16];
   char customer_info_s [15];
   uint8_t open_close_req_c;
   uint8_t bid_or_ask_c;
   uint8_t ext_t_state_c;
   uint8_t order_type_c;
   uint8_t stop_condition_c;
} order_var_t;


typedef struct ex_user_code
{
   char country_id_s [2];
   char ex_customer_s [5];
   char user_id_s [5];
} ex_user_code_t;


/*   NAMED STRUCT = 50002  */

typedef struct give_up_member
{
   char country_id_s [2];
   char ex_customer_s [5];
   char filler_1_s;
} give_up_member_t;


typedef struct party
{
   char country_id_s [2];
   char ex_customer_s [5];
   char filler_1_s;
} party_t;


typedef struct old_series
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} old_series_t;


typedef struct new_series
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} new_series_t;


typedef struct da24
{
   uint8_t country_c;
   char name_s [32];
   char exchange_short_s [4];
   char country_id_s [2];
   char tz_exchange_s [40];
   char master_clh_id_s [12];
   char country_s [2];
   char date_implementation_s [8];
   char filler_3_s [3];
} da24_t;


/*   BROADCAST TYPE = BO93  */

typedef struct ext_mass_quote_update
{
   broadcast_type_t broadcast_type;
   trading_code_t trading_code;
   time_spec_t execution_timestamp;
   quad_word order_number_u;
   uint32_t sequence_number_u;
   char exchange_info_s [32];
   char ex_client_s [16];
   char filler_2_s [2];
   uint16_t items_n;
   struct	
   {	
      series_t series;
      quad_word order_number_buy_u;
      int32_t buy_price_i;
      int64_t buy_quantity_u;
      quad_word order_number_sell_u;
      int32_t sell_price_i;
      int64_t sell_quantity_u;
   } item [10];
} ext_mass_quote_update_t;


/*   BROADCAST TYPE = BO96  */

typedef struct mass_quote_update
{
   broadcast_type_t broadcast_type;
   trading_code_t trading_code;
   time_spec_t execution_timestamp;
   quad_word order_number_u;
   uint32_t sequence_number_u;
   uint16_t items_n;
   char acc_type_c;
   char filler_1_s;
   struct	
   {	
      series_t series;
      int32_t buy_price_i;
      int64_t buy_quantity_u;
      int32_t sell_price_i;
      int64_t sell_quantity_u;
   } item [36];
} mass_quote_update_t;


typedef struct server_partition
{
   char server_name_s [20];
   transaction_type_low_t transaction_type_low;
   transaction_type_high_t transaction_type_high;
   series_fields_used_t series_fields_used;
   partition_low_t partition_low;
   partition_high_t partition_high;
   int32_t event_type_i;
} server_partition_t;


typedef struct order
{
   series_t series;
   trading_code_t trading_code;
   order_var_t order_var;
   ex_user_code_t ex_user_code;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   uint32_t order_index_u;
   uint16_t transaction_number_n;
   uint8_t change_reason_c;
   char filler_1_s;
} order_t;


typedef struct stop_series
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} stop_series_t;


typedef struct order_no_id
{
   series_t series;
   int64_t mp_quantity_i;
   int32_t premium_i;
   uint32_t block_n;
   uint16_t exch_order_type_n;
   uint8_t bid_or_ask_c;
   char filler_1_s;
} order_no_id_t;


typedef struct trd_rpt_cust
{
   party_t party;
   char ex_client_s [16];
   char customer_info_s [15];
   char exchange_info_s [32];
   uint8_t open_close_req_c;
   uint16_t exch_order_type_n;
   give_up_member_t give_up_member;
   char filler_2_s [2];
} trd_rpt_cust_t;


typedef struct da53
{
   series_t series;
   char corp_action_code_s [2];
   uint8_t corp_action_type_c;
   uint8_t corp_action_status_c;
   uint8_t corp_action_level_c;
   char filler_3_s [3];
} da53_t;


typedef struct originator_trading_code
{
   char country_id_s [2];
   char ex_customer_s [5];
   char user_id_s [5];
} originator_trading_code_t;


typedef struct trd_rpt_part
{
   party_t party;
   char ex_client_s [16];
   char customer_info_s [15];
   char exchange_info_s [32];
   uint8_t open_close_req_c;
} trd_rpt_part_t;


typedef struct add_tm_combo_item
{
   series_t series;
   uint16_t ratio_n;
   char op_if_buy_c;
   char op_if_sell_c;
} add_tm_combo_item_t;


typedef struct answer_at_the_money_item
{
   series_t series;
   uint16_t expiration_date_n;
   char filler_2_s [2];
   int32_t atm_price_i;
   int32_t base_price_i;
} answer_at_the_money_item_t;


typedef struct answer_bi73_signals_sent_item
{
   series_t series;
   int32_t info_type_i;
   char business_date_s [8];
   char clearing_date_s [8];
   char sent_date_s [8];
   char sent_time_s [6];
   uint16_t seq_num_srm_n;
} answer_bi73_signals_sent_item_t;


typedef struct answer_bi7_signals_sent_item
{
   series_t series;
   int32_t info_type_i;
   char business_date_s [8];
   char clearing_date_s [8];
   char sent_date_s [8];
   char sent_time_s [6];
   uint16_t seq_num_srm_n;
} answer_bi7_signals_sent_item_t;


typedef struct answer_broker_item
{
   char user_id_s [5];
   uint8_t program_trader_c;
   uint16_t cst_id_n;
   uint16_t usr_id_n;
   uint16_t items_n;
   struct	
   {	
      series_t series;
   } item [100];
} answer_broker_item_t;


typedef struct answer_central_group_item
{
   char central_group_s [12];
   uint16_t leg_number_n;
   uint8_t sort_type_c;
   char filler_1_s;
   char long_ins_id_s [32];
} answer_central_group_item_t;


typedef struct answer_clearing_date_item
{
   series_t series;
   char clearing_date_s [8];
   char next_clearing_date_s [8];
   char prev_clearing_date_s [8];
   char tra_cl_next_day_c;
   char filler_3_s [3];
} answer_clearing_date_item_t;


typedef struct answer_converted_series_item
{
   uint16_t adjust_ident_n;
   char filler_2_s [2];
   int32_t contract_size_i;
   int32_t price_quot_factor_i;
   old_series_t old_series;
   new_series_t new_series;
} answer_converted_series_item_t;


typedef struct answer_corp_action_da53_item
{
   series_t series;
   char corp_action_code_s [2];
   uint8_t corp_action_type_c;
   uint8_t corp_action_status_c;
   uint8_t corp_action_level_c;
   char filler_3_s [3];
} answer_corp_action_da53_item_t;


typedef struct answer_currency_item
{
   uint16_t sec_rel_primary_n;
   uint16_t third_rel_primary_n;
   char base_cur_s [3];
   char pri_unit_s [15];
   char sec_unit_s [15];
   char third_unit_s [15];
   char pri_not_s [5];
   char sec_not_s [5];
   char third_not_s [5];
   uint8_t acc_as_pay_c;
} answer_currency_item_t;


typedef struct answer_deal_source_item
{
   int64_t ds_attribute_q;
   int16_t deal_source_n;
   char desc_long_s [128];
   char desc_abbreviated_s [32];
   char filler_2_s [2];
} answer_deal_source_item_t;


typedef struct answer_exchange_da24_item
{
   da24_t da24;
} answer_exchange_da24_item_t;


typedef struct answer_ext_mass_quote_update_item
{
   ext_mass_quote_update_t ext_mass_quote_update;
} answer_ext_mass_quote_update_item_t;


typedef struct answer_instrument_item
{
   series_t series;
   uint32_t min_show_vol_u;
   uint16_t hidden_vol_meth_n;
   uint16_t pub_inf_id_n;
   char int_id_s [8];
   char name_s [32];
   uint8_t maintain_positions_c;
   uint8_t traded_c;
   uint8_t post_trade_proc_c;
   uint8_t pos_handling_c;
   uint8_t directed_trade_information_c;
   uint8_t public_deal_information_c;
   uint8_t pricing_method_c;
   uint8_t settlement_type_c;
} answer_instrument_item_t;


typedef struct answer_instrument_group_item
{
   uint8_t instrument_group_c;
   char name_s [32];
   char ing_id_s [3];
   uint8_t group_type_c;
   uint8_t tailor_made_c;
   uint8_t option_type_c;
   uint8_t option_style_c;
   uint8_t warrant_c;
   uint8_t repo_type_c;
   uint8_t buy_sell_back_c;
   uint8_t future_styled_c;
   uint8_t is_exclusive_opening_sell_c;
   uint8_t knock_variant_c;
   uint8_t binary_variant_c;
   uint8_t option_variant_c;
   uint8_t physical_delivery_c;
   uint8_t forward_style_c;
   uint8_t swap_style_c;
   uint8_t maturity_c;
   char group_short_name_s [15];
   uint8_t overnight_index_swap_c;
} answer_instrument_group_item_t;


typedef struct answer_instrument_status_item
{
   series_t series;
   uint16_t state_number_n;
   uint16_t state_level_e;
} answer_instrument_status_item_t;


typedef struct answer_limit_item
{
   series_t series;
   int32_t upper_limit_i;
   int32_t lower_limit_i;
   int32_t reference_premium_i;
   uint16_t price_source_rule_n;
   char filler_2_s [2];
} answer_limit_item_t;


typedef struct answer_market_item
{
   uint16_t normal_trading_days_n;
   uint16_t normal_settl_days_n;
   uint16_t normal_clearing_days_n;
   uint8_t country_c;
   uint8_t market_c;
   char name_s [32];
   char mar_id_s [5];
   uint8_t market_type_c;
   uint8_t index_market_c;
   char bic_code_s [15];
   char mic_code_s [8];
   char filler_2_s [2];
} answer_market_item_t;


typedef struct answer_mass_quote_update_item
{
   mass_quote_update_t mass_quote_update;
} answer_mass_quote_update_item_t;


typedef struct answer_mm_protection_item
{
   int64_t quantity_protection_q;
   int64_t delta_protection_q;
   int32_t exposure_time_interval_i;
   int32_t frozen_time_i;
   uint16_t commodity_n;
   char country_id_s [2];
   char ex_customer_s [5];
   uint8_t include_futures_c;
   char filler_2_s [2];
} answer_mm_protection_item_t;


typedef struct answer_non_trading_days_item
{
   uint8_t country_c;
   uint8_t market_c;
   char date_non_trading_s [8];
   uint8_t closed_for_trading_c;
   uint8_t closed_for_settlement_c;
   uint8_t closed_for_clearing_c;
   char filler_3_s [3];
} answer_non_trading_days_item_t;


typedef struct answer_partition_item
{
   server_partition_t server_partition;
} answer_partition_item_t;


typedef struct answer_prel_settlement_item
{
   series_t series;
   int32_t settl_price_i;
   char settlement_date_s [8];
   uint8_t settlement_price_type_c;
   char date_and_time_s [14];
   char filler_1_s;
} answer_prel_settlement_item_t;


typedef struct answer_price_vola_settl_item
{
   series_t series;
   int32_t last_price_i;
   int32_t settle_price_i;
   int32_t actual_vol_i;
   int32_t theo_price_i;
   int32_t current_vwv_i;
   int32_t calc_vwv_i;
   int32_t prev_vwv_i;
   int32_t risk_free_rate_i;
   int32_t ulg_price_i;
   int32_t dividend_yield_i;
   int32_t dividend_sum_i;
} answer_price_vola_settl_item_t;


typedef struct answer_ref_price_trans_item
{
   series_t series;
   int32_t reference_price_i;
   int32_t reserved_i;
   char date_and_time_s [14];
   char filler_2_s [2];
} answer_ref_price_trans_item_t;


typedef struct answer_state_type_item
{
   uint16_t state_type_number_n;
   char state_type_name_s [20];
   char name_s [32];
   uint8_t country_c;
   uint8_t market_c;
} answer_state_type_item_t;


typedef struct answer_stop_order_item
{
   quad_word order_number_u;
   party_t party;
   order_t order;
   stop_series_t stop_series;
   time_spec_t timestamp_created;
   time_spec_t timestamp_in;
   int32_t limit_premium_i;
   int64_t total_volume_i;
   int64_t display_quantity_i;
} answer_stop_order_item_t;


typedef struct series_next
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} series_next_t;


typedef struct answer_tot_equil_prices_item
{
   series_t series;
   int64_t equilibrium_quantity_i;
   int32_t equilibrium_price_i;
   int32_t best_bid_premium_i;
   int32_t best_ask_premium_i;
   int64_t best_bid_quantity_i;
   int64_t best_ask_quantity_i;
   uint8_t matching_price_type_c;
   char filler_3_s [3];
} answer_tot_equil_prices_item_t;


typedef struct answer_tot_ob_item
{
   quad_word order_number_u;
   uint32_t sequence_number_u;
   uint32_t ob_position_u;
   uint8_t combo_mark_c;
   char filler_3_s [3];
   order_no_id_t order_no_id;
   party_t party;
} answer_tot_ob_item_t;


typedef struct answer_tot_ob_id_item
{
   quad_word order_number_u;
   uint32_t sequence_number_u;
   uint32_t ob_position_u;
   uint8_t combo_mark_c;
   char filler_3_s [3];
   order_t order;
} answer_tot_ob_id_item_t;


typedef struct answer_tot_order_item
{
   quad_word order_number_u;
   uint32_t sequence_number_u;
   uint32_t ob_position_u;
   uint8_t combo_mark_c;
   uint8_t order_category_c;
   char filler_2_s [2];
   party_t party;
   order_t order;
   int64_t total_volume_i;
   int64_t display_quantity_i;
   int64_t orig_shown_quantity_i;
   int64_t orig_total_volume_i;
   time_spec_t timestamp_in;
   time_spec_t timestamp_created;
} answer_tot_order_item_t;


typedef struct answer_trade_report_types_item
{
   int64_t initial_trr_min_value_u;
   char trc_id_s [10];
   char trr_id_s [4];
   char condition_s [32];
   uint8_t authorized_c;
   uint8_t ext_t_state_c;
   uint8_t allow_interbank_c;
   uint8_t allow_within_participant_c;
   uint8_t cbo_trade_report_c;
   uint8_t allow_non_std_settlement_c;
   uint8_t time_of_agree_req_c;
   uint8_t time_of_agree_gran_c;
   uint8_t allow_delayed_c;
   char filler_1_s;
} answer_trade_report_types_item_t;


typedef struct answer_trade_statistics_item
{
   series_t series;
   int32_t bid_premium_i;
   int32_t ask_premium_i;
   int32_t opening_price_i;
   int32_t settle_price_i;
   int32_t last_price_i;
   int32_t high_price_i;
   int32_t low_price_i;
   int64_t volume_today_i;
   int64_t volume_yesterday_i;
   int64_t turnaround_yesterday_u;
   int64_t turnaround_today_u;
   int64_t open_balance_u;
   int64_t revised_open_balance_u;
   int32_t volatility_i;
   int32_t underlying_price_i;
   int32_t corr_opening_price_i;
   int32_t corr_high_price_i;
   int32_t corr_low_price_i;
   int32_t corr_last_price_i;
   uint8_t bid_theo_c;
   uint8_t ask_theo_c;
   char filler_2_s [2];
} answer_trade_statistics_item_t;


typedef struct answer_trading_state_item
{
   char state_name_s [20];
   uint16_t state_number_n;
   uint16_t iss_def_warning_interval_n;
   uint16_t iss_def_num_of_warnings_n;
   uint16_t state_type_number_n;
   uint8_t continues_matching_c;
   uint8_t trading_end_c;
   uint8_t price_quotation_required_c;
   uint8_t market_orders_allowed_c;
   uint8_t fill_or_kill_allowed_c;
   uint8_t fill_and_kill_allowed_c;
   uint8_t edited_ob_changes_avail_c;
   uint8_t ob_changes_avail_c;
   uint8_t end_of_clearing_day_c;
   uint8_t state_priority_c;
   uint8_t fill_and_store_c;
   uint8_t all_or_none_c;
   uint8_t imb_orders_allwd_c;
   uint8_t mtl_orders_rlot_allwd_c;
   uint8_t allow_hidd_aggr_c;
   uint8_t best_limit_c;
   uint8_t market_bid_c;
   uint8_t price_stabilization_c;
   uint8_t short_sell_c;
   uint8_t allow_cp_mar_c;
   uint8_t allow_cp_lim_c;
   uint8_t allow_undisclosed_qty_c;
   uint8_t sso_orders_allowed_c;
   uint8_t stop_orders_allowed_c;
   uint8_t good_till_session_c;
   uint8_t priority_ranking_c;
   uint8_t fix_state_number_c;
   char filler_1_s;
} answer_trading_state_item_t;


typedef struct answer_trd_report_item
{
   trading_code_t trading_code;
   transaction_type_t transaction_type;
   quad_word order_number_u;
   series_t series;
   order_var_t order_var;
   party_t party;
   uint32_t sequence_number_u;
   char exchange_info_s [32];
   give_up_member_t give_up_member;
   char settlement_date_s [8];
   char time_of_agreement_date_s [8];
   char time_of_agreement_time_s [6];
   uint8_t deferred_publication_c;
   char filler_1_s;
} answer_trd_report_item_t;


typedef struct answer_trd_report_party_item
{
   trading_code_t trading_code;
   transaction_type_t transaction_type;
   quad_word order_number_u;
   series_t series;
   order_var_t order_var;
   party_t party;
   char exchange_info_s [32];
   give_up_member_t give_up_member;
   char settlement_date_s [8];
   char time_of_agreement_date_s [8];
   char time_of_agreement_time_s [6];
   uint8_t deferred_publication_c;
   char filler_1_s;
} answer_trd_report_party_item_t;


typedef struct answer_underlying_adjustment_item
{
   int64_t no_of_shares_issued_q;
   int64_t free_float_q;
   uint16_t adjust_ident_n;
   uint16_t commodity_n;
   char date_adjust_s [8];
   char date_conversion_s [8];
   uint8_t deal_price_modifier_c;
   uint8_t contract_size_modifier_c;
   uint8_t strike_price_modifier_c;
   uint8_t contracts_modifier_c;
   uint8_t und_price_modifier_c;
   uint8_t so_strike_price_modifier_c;
   uint8_t so_contract_size_modifier_c;
   uint8_t so_deal_price_modifier_c;
   int32_t deal_price_mod_factor_i;
   int32_t contr_size_mod_factor_i;
   int32_t strike_price_mod_factor_i;
   int32_t contracts_mod_factor_i;
   int32_t und_price_mod_factor_i;
   int32_t so_strike_price_mod_factor_i;
   int32_t so_contr_size_mod_factor_i;
   int32_t so_deal_price_mod_factor_i;
   int32_t pqf_mod_factor_i;
   int32_t so_pqf_mod_factor_i;
   uint16_t new_commodity_n;
   uint16_t so_commodity_n;
   uint8_t pqf_modifier_c;
   uint8_t so_pqf_modifier_c;
   uint8_t country_c;
   uint8_t market_c;
   uint8_t so_country_c;
   uint8_t so_market_c;
   uint8_t adjusted_c;
   uint8_t spinoff_c;
   uint8_t is_adjust_same_series_c;
   uint8_t price_info_prev_day_c;
   char no_of_shares_issued_op_c;
   char free_float_op_c;
   uint16_t items_n;
   char filler_2_s [2];
   struct	
   {	
      series_t series;
      int32_t contract_share_i;
      int32_t amount_ratio_i;
   } delivery_change [20];
} answer_underlying_adjustment_item_t;


typedef struct answer_underlying_indices_item
{
   series_t series;
   int32_t bid_premium_i;
   int32_t ask_premium_i;
   int32_t closing_price_i;
   int32_t opening_price_i;
   int32_t high_price_i;
   int32_t low_price_i;
   int32_t last_price_i;
   int32_t ref_price_i;
   int32_t change_previous_i;
   int32_t change_yesterday_i;
   int32_t points_of_movement_i;
   int64_t turnover_u;
   int64_t best_bid_volume_u;
   int64_t best_ask_volume_u;
   char date_s [8];
   char ext_time_s [6];
   uint8_t undisclosed_bid_volume_c;
   uint8_t undisclosed_ask_volume_c;
   char filler_2_s [2];
   char reserved_2_s [2];
} answer_underlying_indices_item_t;


typedef struct answer_user_type_info_item
{
   transaction_type_t transaction_type;
   uint8_t trans_or_bdx_c;
   char filler_3_s [3];
} answer_user_type_info_item_t;


typedef struct answer_valid_sector_code_item
{
   char sector_code_s [4];
   char description_s [40];
} answer_valid_sector_code_item_t;


typedef struct at_the_money_update_item
{
   series_t series;
   uint16_t expiration_date_n;
   char filler_2_s [2];
   int32_t atm_price_i;
   int32_t base_price_i;
} at_the_money_update_item_t;


typedef struct block_order_response_item
{
   int32_t transaction_status_i;
   int32_t trans_ack_i;
   uint8_t item_number_c;
   char filler_3_s [3];
} block_order_response_item_t;


typedef struct block_order_trans_item
{
   series_t series;
   order_var_t order_var;
   int64_t total_volume_i;
} block_order_trans_item_t;


typedef struct user_code
{
   char country_id_s [2];
   char ex_customer_s [5];
   char user_id_s [5];
} user_code_t;


typedef struct block_order_trans_p_item
{
   series_t series;
   order_var_t order_var;
   int64_t total_volume_i;
} block_order_trans_p_item_t;


typedef struct block_price_trans_item
{
   series_t series;
   quad_word order_number_bid_u;
   quad_word order_number_ask_u;
   int32_t bid_premium_i;
   int32_t ask_premium_i;
   int64_t bid_quantity_i;
   int64_t ask_quantity_i;
   int64_t bid_total_volume_i;
   int64_t ask_total_volume_i;
   uint32_t block_n;
   uint16_t time_validity_n;
   uint8_t order_type_c;
   char ex_client_s [16];
   uint8_t delta_quantity_c;
} block_price_trans_item_t;


typedef struct block_price_trans_p_item
{
   series_t series;
   quad_word order_number_bid_u;
   quad_word order_number_ask_u;
   int32_t bid_premium_i;
   int32_t ask_premium_i;
   int64_t bid_quantity_i;
   int64_t ask_quantity_i;
   int64_t bid_total_volume_i;
   int64_t ask_total_volume_i;
   uint32_t block_n;
   uint16_t time_validity_n;
   uint8_t order_type_c;
   char ex_client_s [16];
   uint8_t delta_quantity_c;
} block_price_trans_p_item_t;


typedef struct da28
{
   char central_group_s [12];
   uint16_t segment_number_n;
   uint16_t items_n;
   struct	
   {	
      char long_ins_id_s [32];
      uint16_t leg_number_n;
      uint8_t sort_type_c;
      char filler_1_s;
   } item [30];
} da28_t;


typedef struct account
{
   char country_id_s [2];
   char ex_customer_s [5];
   char account_id_s [16];
   char filler_1_s;
} account_t;


typedef struct orig_series
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} orig_series_t;


typedef struct match_id
{
   uint64_t execution_event_nbr_u;
   uint32_t match_group_nbr_u;
   uint32_t match_item_nbr_u;
} match_id_t;


typedef struct countersign_code
{
   char country_id_s [2];
   char ex_customer_s [5];
   char user_id_s [5];
} countersign_code_t;


typedef struct pos_account
{
   char country_id_s [2];
   char ex_customer_s [5];
   char account_id_s [16];
   char filler_1_s;
} pos_account_t;


typedef struct combo_series
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} combo_series_t;


typedef struct combo_trade_report_trans_item
{
   series_t series;
   int64_t mp_quantity_i;
   int32_t premium_i;
   uint32_t block_n;
   char settlement_date_s [8];
   char time_of_agreement_date_s [8];
   char time_of_agreement_time_s [6];
   uint8_t deferred_publication_c;
   char filler_1_s;
   trd_rpt_cust_t bid_side;
   trd_rpt_cust_t ask_side;
} combo_trade_report_trans_item_t;


typedef struct corp_action_update_bu53_item
{
   da53_t da53;
} corp_action_update_bu53_item_t;


typedef struct prio_crossing
{
   int64_t mp_quantity_i;
   int32_t premium_i;
   give_up_member_t buy_give_up_member;
   give_up_member_t sell_give_up_member;
   uint32_t block_n;
   uint16_t exch_order_type_n;
   char buy_customer_info_s [15];
   char sell_customer_info_s [15];
   char buy_ex_client_s [16];
   char sell_ex_client_s [16];
   char exchange_info_s [32];
   uint8_t order_type_c;
   char filler_3_s [3];
} prio_crossing_t;


typedef struct whose
{
   trading_code_t trading_code;
   char ex_client_s [16];
} whose_t;


typedef struct equil_price_update_item
{
   series_t series;
   int64_t equilibrium_quantity_i;
   int32_t equilibrium_price_i;
   int32_t best_bid_premium_i;
   int32_t best_ask_premium_i;
   int64_t best_bid_quantity_i;
   int64_t best_ask_quantity_i;
   uint8_t matching_price_type_c;
   char filler_3_s [3];
} equil_price_update_item_t;


typedef struct exchange_rate_answer_item
{
   series_t series;
   int32_t rate_i;
} exchange_rate_answer_item_t;


typedef struct ext_mass_quote_trans_item
{
   series_t series;
   quad_word order_number_buy_u;
   int32_t buy_price_i;
   int64_t buy_quantity_u;
   quad_word order_number_sell_u;
   int32_t sell_price_i;
   int64_t sell_quantity_u;
} ext_mass_quote_trans_item_t;


typedef struct ext_mass_quote_update_item
{
   series_t series;
   quad_word order_number_buy_u;
   int32_t buy_price_i;
   int64_t buy_quantity_u;
   quad_word order_number_sell_u;
   int32_t sell_price_i;
   int64_t sell_quantity_u;
} ext_mass_quote_update_item_t;


typedef struct instrument_status_info_item
{
   series_t series;
   uint16_t seconds_to_state_change_n;
   uint16_t state_number_n;
   char warning_msg_s [80];
   uint16_t state_level_e;
   char actual_start_date_s [8];
   char actual_start_time_s [6];
   char next_planned_start_date_s [8];
   char next_planned_start_time_s [6];
   char filler_2_s [2];
} instrument_status_info_item_t;


typedef struct limit_change_item
{
   series_t series;
   int32_t upper_limit_i;
   int32_t lower_limit_i;
   int32_t reference_premium_i;
   uint16_t price_source_rule_n;
   char filler_2_s [2];
} limit_change_item_t;


typedef struct mass_quote_trans_item
{
   series_t series;
   int32_t buy_price_i;
   int64_t buy_quantity_u;
   int32_t sell_price_i;
   int64_t sell_quantity_u;
} mass_quote_trans_item_t;


typedef struct mass_quote_update_item
{
   series_t series;
   int32_t buy_price_i;
   int64_t buy_quantity_u;
   int32_t sell_price_i;
   int64_t sell_quantity_u;
} mass_quote_update_item_t;


typedef struct message_information_item
{
   char text_line_s [80];
} message_information_item_t;


typedef struct da87
{
   int64_t quantity_protection_q;
   int64_t delta_protection_q;
   int32_t exposure_time_interval_i;
   int32_t frozen_time_i;
   uint16_t commodity_n;
   char country_id_s [2];
   char ex_customer_s [5];
   uint8_t include_futures_c;
   char filler_2_s [2];
} da87_t;


typedef struct multi_leg_order_insert_item
{
   series_t series;
   int64_t quantity_i;
   int32_t premium_i;
   uint8_t bid_or_ask_c;
   uint8_t calculate_quantity_method_c;
   char filler_2_s [2];
} multi_leg_order_insert_item_t;


typedef struct multi_leg_order_insert_p_item
{
   series_t series;
   int64_t quantity_i;
   int32_t premium_i;
   uint8_t bid_or_ask_c;
   uint8_t calculate_quantity_method_c;
   char filler_2_s [2];
} multi_leg_order_insert_p_item_t;


typedef struct da18
{
   uint8_t country_c;
   uint8_t market_c;
   char date_non_trading_s [8];
   uint8_t closed_for_trading_c;
   uint8_t closed_for_settlement_c;
   uint8_t closed_for_clearing_c;
   char filler_3_s [3];
} da18_t;


typedef struct upper_level_series
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} upper_level_series_t;


typedef struct tick_size
{
   int32_t step_size_i;
   int32_t lower_limit_i;
   int32_t upper_limit_i;
} tick_size_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34903  */

typedef struct order_change_separate
{
   series_t series;
   quad_word order_number_u;
   int64_t mp_quantity_i;
   int64_t total_volume_i;
   uint8_t bid_or_ask_c;
   uint8_t change_reason_c;
   char ex_client_s [16];
   char customer_info_s [15];
   char filler_3_s [3];
   originator_trading_code_t originator_trading_code;
   time_spec_t execution_timestamp;
} order_change_separate_t;


typedef struct search_series
{
   uint8_t country_c;
   uint8_t market_c;
   uint8_t instrument_group_c;
   uint8_t modifier_c;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
} search_series_t;


typedef struct ref_price_change_item
{
   series_t series;
   int32_t reference_price_i;
   int32_t reserved_i;
   char date_and_time_s [14];
   char filler_2_s [2];
} ref_price_change_item_t;


typedef struct settle_price_update_item
{
   series_t series;
   int32_t settle_price_i;
   char settlement_date_s [8];
   uint8_t settlement_price_type_c;
   char date_and_time_s [14];
   char filler_1_s;
} settle_price_update_item_t;


typedef struct bid_side
{
   trd_rpt_part_t trd_rpt_part;
} bid_side_t;


typedef struct ask_side
{
   trd_rpt_part_t trd_rpt_part;
} ask_side_t;


typedef struct da54
{
   char sector_code_s [4];
   char description_s [40];
} da54_t;


/*   TRANSACTION TYPE = DC3  */

typedef struct add_tm_combo
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t no_of_legs_n;
   char filler_2_s [2];
   add_tm_combo_item_t item [4];
} add_tm_combo_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34009  */

typedef struct alter_trans
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   order_var_t order_var;
} alter_trans_t;


/*   TRANSACTION TYPE = DI3  */

typedef struct answer_add_tm_combo
{
   transaction_type_t transaction_type;
   series_t series;
} answer_add_tm_combo_t;


/*   TRANSACTION TYPE = MA68  */

typedef struct answer_at_the_money
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_at_the_money_item_t item [2500];
} answer_at_the_money_t;


/*   TRANSACTION TYPE = UA20  */

typedef struct answer_bi73_signals_sent
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_bi73_signals_sent_item_t item [1000];
} answer_bi73_signals_sent_t;


/*   TRANSACTION TYPE = UA9  */

typedef struct answer_bi7_signals_sent
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_bi7_signals_sent_item_t item [1000];
} answer_bi7_signals_sent_t;


/*   TRANSACTION TYPE = MA99  */

typedef struct answer_block_size
{
   transaction_type_t transaction_type;
   int32_t max_block_order_size_i;
   int32_t max_block_price_size_i;
} answer_block_size_t;


/*   TRANSACTION TYPE = DA6  */

typedef struct answer_broker
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   char country_id_s [2];
   char ex_customer_s [5];
   char filler_1_s;
   uint16_t items_n;
   answer_broker_item_t item [50];
} answer_broker_t;


/*   TRANSACTION TYPE = UA12  */

typedef struct answer_business_date
{
   transaction_type_t transaction_type;
   char omex_version_s [16];
   char business_date_s [8];
   char utc_date_s [8];
   char utc_time_s [6];
   char tz_variable_s [40];
   char filler_2_s [2];
   int32_t utc_offset_i;
} answer_business_date_t;


/*   TRANSACTION TYPE = DA28  */

typedef struct answer_central_group
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_central_group_item_t item [1000];
} answer_central_group_t;


/*   TRANSACTION TYPE = CA68  */

typedef struct answer_clearing_date
{
   transaction_type_t transaction_type;
   partition_low_t partition_low;
   partition_high_t partition_high;
   char omex_version_s [16];
   char business_date_s [8];
   uint16_t items_n;
   char filler_2_s [2];
   answer_clearing_date_item_t item [1000];
} answer_clearing_date_t;


/*   TRANSACTION TYPE = DA15  */

typedef struct answer_converted_series
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_converted_series_item_t item [100];
} answer_converted_series_t;


/*   TRANSACTION TYPE = DA53  */

typedef struct answer_corp_action_da53
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_corp_action_da53_item_t item [1000];
} answer_corp_action_da53_t;


/*   TRANSACTION TYPE = DA33  */

typedef struct answer_currency
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_currency_item_t item [100];
} answer_currency_t;


/*   TRANSACTION TYPE = DA46  */

typedef struct answer_deal_source
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_deal_source_item_t item [100];
} answer_deal_source_t;


/*   TRANSACTION TYPE = DA24  */

typedef struct answer_exchange_da24
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_exchange_da24_item_t item [100];
} answer_exchange_da24_t;


/*   TRANSACTION TYPE = MA93  */

typedef struct answer_ext_mass_quote_update
{
   transaction_type_t transaction_type;
   uint16_t items_n;
   char filler_2_s [2];
   answer_ext_mass_quote_update_item_t item [100];
} answer_ext_mass_quote_update_t;


/*   TRANSACTION TYPE = IA21, IA22, MA123, MA507  */

typedef struct answer_hdr
{
   transaction_type_t transaction_type;
   uint16_t items_n;
   uint16_t size_n;
} answer_hdr_t;


/*   TRANSACTION TYPE = DA22, DA3  */

typedef struct answer_instrument
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_instrument_item_t item [100];
} answer_instrument_t;


/*   TRANSACTION TYPE = DA8  */

typedef struct answer_instrument_group
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_instrument_group_item_t item [100];
} answer_instrument_group_t;


/*   TRANSACTION TYPE = UA15  */

typedef struct answer_instrument_status
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_instrument_status_item_t item [1000];
} answer_instrument_status_t;


/*   TRANSACTION TYPE = MA1  */

typedef struct answer_limit
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t items_n;
   char filler_2_s [2];
   answer_limit_item_t item [1000];
} answer_limit_t;


/*   TRANSACTION TYPE = DA23, DA7  */

typedef struct answer_market
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_market_item_t item [100];
} answer_market_t;


/*   TRANSACTION TYPE = MA96  */

typedef struct answer_mass_quote_update
{
   transaction_type_t transaction_type;
   uint16_t items_n;
   char filler_2_s [2];
   answer_mass_quote_update_item_t item [47];
} answer_mass_quote_update_t;


/*   TRANSACTION TYPE = CA10  */

typedef struct answer_missing_trade_hdr
{
   transaction_type_t transaction_type;
   char filler_2_s [2];
   uint16_t items_n;
} answer_missing_trade_hdr_t;


/*   TRANSACTION TYPE = DA87  */

typedef struct answer_mm_protection
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_mm_protection_item_t item [500];
} answer_mm_protection_t;


/*   TRANSACTION TYPE = TA70, TA71  */

typedef struct answer_next_series_hdr
{
   transaction_type_t transaction_type;
   series_t next_series;
   uint16_t segment_number_n;
   uint16_t items_n;
   uint16_t size_n;
   char filler_2_s [2];
} answer_next_series_hdr_t;


/*   TRANSACTION TYPE = DA18  */

typedef struct answer_non_trading_days
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_non_trading_days_item_t item [100];
} answer_non_trading_days_t;


/*   TRANSACTION TYPE = UA1  */

typedef struct answer_partition
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_partition_item_t item [100];
} answer_partition_t;


/*   TRANSACTION TYPE = IA17  */

typedef struct answer_prel_settlement
{
   transaction_type_t transaction_type;
   uint16_t items_n;
   uint16_t segment_number_n;
   answer_prel_settlement_item_t item [1500];
} answer_prel_settlement_t;


/*   TRANSACTION TYPE = RA62  */

typedef struct answer_price_vola_settl
{
   transaction_type_t transaction_type;
   uint16_t items_n;
   uint16_t segment_number_n;
   uint8_t settlement_price_type_c;
   char filler_3_s [3];
   answer_price_vola_settl_item_t item [1100];
} answer_price_vola_settl_t;


/*   TRANSACTION TYPE = IA2147  */

typedef struct answer_ref_price_trans
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_ref_price_trans_item_t item [1700];
} answer_ref_price_trans_t;


/*   TRANSACTION TYPE = DA120, DA121, DA122, DA123, DA124, DA125, DA126, DA90, UA14  */

typedef struct answer_segment_hdr
{
   transaction_type_t transaction_type;
   uint16_t items_n;
   uint16_t size_n;
   uint16_t segment_number_n;
   char filler_2_s [2];
} answer_segment_hdr_t;


/*   TRANSACTION TYPE = DA76  */

typedef struct answer_state_type
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_state_type_item_t item [100];
} answer_state_type_t;


/*   TRANSACTION TYPE = MA47, MA48, MA49  */

typedef struct answer_stop_order
{
   transaction_type_t transaction_type;
   series_t series;
   uint32_t order_index_u;
   uint16_t items_n;
   char filler_2_s [2];
   answer_stop_order_item_t item [300];
} answer_stop_order_t;


/*   TRANSACTION TYPE = IB12  */

typedef struct answer_tot_equil_prices
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint8_t instance_c;
   uint8_t instance_next_c;
   series_next_t series_next;
   uint16_t items_n;
   char filler_2_s [2];
   answer_tot_equil_prices_item_t item [1230];
} answer_tot_equil_prices_t;


/*   TRANSACTION TYPE = MA42  */

typedef struct answer_tot_ob
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   uint16_t items_n;
   uint8_t bid_or_ask_c;
   char filler_1_s;
   answer_tot_ob_item_t item [1000];
} answer_tot_ob_t;


/*   TRANSACTION TYPE = MA41  */

typedef struct answer_tot_ob_id
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   uint16_t items_n;
   uint8_t bid_or_ask_c;
   char filler_1_s;
   answer_tot_ob_id_item_t item [399];
} answer_tot_ob_id_t;


/*   TRANSACTION TYPE = MA32, MA43, MA44, MA50, MA8  */

typedef struct answer_tot_order
{
   transaction_type_t transaction_type;
   series_t series;
   uint32_t order_index_u;
   uint16_t items_n;
   char filler_2_s [2];
   answer_tot_order_item_t item [296];
} answer_tot_order_t;


/*   TRANSACTION TYPE = DA45  */

typedef struct answer_trade_report_types
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_trade_report_types_item_t item [200];
} answer_trade_report_types_t;


/*   TRANSACTION TYPE = IA42  */

typedef struct answer_trade_statistics
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_trade_statistics_item_t item [500];
} answer_trade_statistics_t;


/*   TRANSACTION TYPE = DA29  */

typedef struct answer_trading_state
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_trading_state_item_t item [100];
} answer_trading_state_t;


/*   TRANSACTION TYPE = MA78  */

typedef struct answer_trd_report
{
   transaction_type_t transaction_type;
   series_t series;
   uint32_t order_index_u;
   uint16_t items_n;
   char filler_2_s [2];
   answer_trd_report_item_t item [300];
} answer_trd_report_t;


/*   TRANSACTION TYPE = MA80  */

typedef struct answer_trd_report_party
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   uint16_t items_n;
   uint8_t bid_or_ask_c;
   char filler_1_s;
   answer_trd_report_party_item_t item [300];
} answer_trd_report_party_t;


/*   TRANSACTION TYPE = DA14  */

typedef struct answer_underlying_adjustment
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_underlying_adjustment_item_t item [100];
} answer_underlying_adjustment_t;


/*   TRANSACTION TYPE = IA12  */

typedef struct answer_underlying_indices
{
   transaction_type_t transaction_type;
   uint16_t items_n;
   uint16_t segment_number_n;
   answer_underlying_indices_item_t item [635];
} answer_underlying_indices_t;


/*   TRANSACTION TYPE = DA30  */

typedef struct answer_user_type_info
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   char ust_id_s [5];
   uint8_t ext_or_int_c;
   uint8_t is_trader_c;
   uint8_t program_trader_c;
   uint8_t trader_authorization_c;
   uint8_t hide_firm_order_trade_c;
   char filler_2_s [2];
   answer_user_type_info_item_t item [100];
} answer_user_type_info_t;


/*   TRANSACTION TYPE = DA54  */

typedef struct answer_valid_sector_code
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   answer_valid_sector_code_item_t item [200];
} answer_valid_sector_code_t;


/*   TRANSACTION TYPE = UI1  */

typedef struct application_status
{
   transaction_type_t transaction_type;
   series_t series;
   int32_t application_status_i;
} application_status_t;


/*   BROADCAST TYPE = BO68  */

typedef struct at_the_money_update
{
   broadcast_type_t broadcast_type;
   uint16_t items_n;
   char filler_2_s [2];
   at_the_money_update_item_t item [50];
} at_the_money_update_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34810  */

typedef struct basic_order
{
   int32_t premium_i;
   int64_t quantity_i;
   uint32_t block_n;
   uint16_t time_validity_n;
   uint16_t exch_order_type_n;
   uint8_t order_type_c;
   uint8_t bid_or_ask_c;
   char filler_2_s [2];
} basic_order_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34815  */

typedef struct basic_order_update
{
   int32_t premium_i;
   int64_t quantity_i;
   int64_t balance_quantity_i;
   uint16_t time_validity_n;
   uint16_t exch_order_type_n;
   uint8_t delta_quantity_c;
   char filler_3_s [3];
} basic_order_update_t;


/*   TRANSACTION TYPE = BD70, BD71, TA70, TA71  */
/*   NAMED STRUCT = 34401  */

typedef struct basic_trade_ticker
{
   series_t series;
   time_spec_t timestamp_match;
   time_spec_t time_of_publication;
   uint64_t execution_event_nbr_u;
   uint32_t match_group_nbr_u;
   int64_t deal_quantity_i;
   int32_t deal_price_i;
   uint16_t state_number_n;
   uint16_t segment_number_n;
   uint8_t aggressive;
   char filler_3_s [3];
} basic_trade_ticker_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34909  */

typedef struct bb_change_separate
{
   quad_word order_number_u;
   int64_t mp_quantity_i;
   uint8_t change_reason_c;
   char filler_3_s [3];
} bb_change_separate_t;


/*   BROADCAST TYPE = BO99  */

typedef struct block_order_response
{
   broadcast_type_t broadcast_type;
   quad_word order_number_u;
   uint8_t items_c;
   char filler_3_s [3];
   block_order_response_item_t item [100];
} block_order_response_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34006  */

typedef struct block_order_trans
{
   transaction_type_t transaction_type;
   series_t series;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   uint8_t items_c;
   char filler_3_s [3];
   block_order_trans_item_t item [100];
} block_order_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34106  */

typedef struct block_order_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   uint8_t items_c;
   char filler_3_s [3];
   block_order_trans_p_item_t item [100];
} block_order_trans_p_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34007  */

typedef struct block_price_trans
{
   transaction_type_t transaction_type;
   series_t series;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   char customer_info_s [15];
   uint8_t items_c;
   block_price_trans_item_t item [14];
} block_price_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34107  */

typedef struct block_price_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   char customer_info_s [15];
   uint8_t items_c;
   block_price_trans_p_item_t item [14];
} block_price_trans_p_t;


/*   TRANSACTION TYPE = BD2, BD6, BD70, BD71, BI81, BO16, BO17, BO25, BO5, EB10, EB11  */

typedef struct broadcast_hdr
{
   broadcast_type_t broadcast_type;
   uint16_t items_n;
   uint16_t size_n;
} broadcast_hdr_t;


/*   TRANSACTION TYPE = BU120, BU121, BU122, BU123, BU124, BU125, BU126, BU90  */

typedef struct broadcast_segment_hdr
{
   broadcast_type_t broadcast_type;
   uint16_t items_n;
   uint16_t size_n;
   uint16_t segment_number_n;
   char filler_2_s [2];
} broadcast_segment_hdr_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34013  */

typedef struct broker_trans
{
   transaction_type_t transaction_type;
   series_t series;
   order_var_t order_var;
   party_t party;
   char exchange_info_s [32];
} broker_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34113  */

typedef struct broker_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   order_var_t order_var;
   party_t party;
   char exchange_info_s [32];
} broker_trans_p_t;


/*   BROADCAST TYPE = BU28  */

typedef struct central_group_update
{
   broadcast_type_t broadcast_type;
   uint16_t chg_type_n;
   char filler_2_s [2];
   da28_t da28;
} central_group_update_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34816  */

typedef struct centre_point_order
{
   int64_t minimum_quantity_i;
   uint8_t mid_tick_c;
   uint8_t preference_only_c;
   uint8_t single_fill_minimum_quantity_c;
   char filler_1_s;
} centre_point_order_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34802  */

typedef struct clearing_info
{
   give_up_member_t give_up_member;
   char ex_client_s [16];
   uint8_t open_close_req_c;
   char filler_1_s;
} clearing_info_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 5  */

typedef struct cl_bt_settlement_inst
{
   int32_t original_trade_number_i;
   uint32_t trade_reject_sec_u;
   uint8_t allow_trade_reject_c;
   char clearer_code_s [15];
   char bic_code_s [15];
   char cust_info_s [32];
   char filler_1_s;
} cl_bt_settlement_inst_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 6  */

typedef struct cl_commission_api
{
   int64_t commission_value_q;
   int32_t commission_amount_i;
   uint64_t adj_trade_price_q;
   uint8_t commission_format_c;
   char filler_3_s [3];
} cl_commission_api_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 33  */

typedef struct cl_counterparty_tlx
{
   party_t party;
} cl_counterparty_tlx_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 3  */

typedef struct cl_trade_base_api
{
   trading_code_t trading_code;
   series_t series;
   give_up_member_t give_up_member;
   quad_word order_number_u;
   int32_t sequence_number_i;
   int32_t trade_number_i;
   int32_t deal_price_i;
   int64_t trade_quantity_i;
   account_t account;
   char customer_info_s [15];
   uint8_t bought_or_sold_c;
   uint8_t deal_source_c;
   uint8_t open_close_req_c;
   uint8_t trade_type_c;
   uint8_t le_state_c;
   user_code_t user_code;
   char created_date_s [8];
   char created_time_s [6];
   char asof_date_s [8];
   char asof_time_s [6];
   char modified_date_s [8];
   char modified_time_s [6];
   uint8_t trade_state_c;
   uint8_t attention_c;
   int32_t deal_number_i;
   uint32_t global_deal_no_u;
   int32_t orig_trade_number_i;
   orig_series_t orig_series;
   char exchange_info_s [32];
   uint32_t big_attention_u;
   char clearing_date_s [8];
   time_spec_t execution_timestamp;
   uint8_t trade_venue_c;
   uint8_t instance_c;
   uint16_t exch_order_type_n;
   party_t party;
   uint16_t trade_rep_code_n;
   uint16_t state_number_n;
   match_id_t match_id;
} cl_trade_base_api_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 4  */

typedef struct cl_trade_bt_api
{
   char isin_code_s [12];
   uint16_t dec_in_quantity_n;
   uint16_t dec_in_price_n;
   uint16_t premium_unit_n;
   char filler_2_s [2];
   uint64_t consideration_u;
   char settlement_date_s [8];
   party_t party;
   int32_t conv_factor_u;
   int32_t box_number_i;
   uint64_t end_consideration_u;
   int32_t clean_value_i;
   int32_t dirty_market_price_i;
} cl_trade_bt_api_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 70  */

typedef struct cl_trade_cancel_trade_api
{
   uint32_t trade_reject_sec_u;
} cl_trade_cancel_trade_api_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 68  */

typedef struct cl_trade_fixed_income_api
{
   int32_t corresponding_yield_price_i;
   int64_t consideration_q;
   uint16_t deferred_time_n;
   char settlement_date_s [8];
   char filler_2_s [2];
} cl_trade_fixed_income_api_t;


/*   TRANSACTION TYPE = BD6  */
/*   NAMED STRUCT = 40  */

typedef struct cl_trade_ise_api
{
   uint32_t previous_bbo_u;
   uint32_t trade_number_u;
   uint8_t order_category_c;
   uint8_t account_type_c;
   char broker_id_s [5];
   uint8_t trade_venue_c;
} cl_trade_ise_api_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 20  */

typedef struct cl_trade_secur_part
{
   countersign_code_t countersign_code;
   new_series_t new_series;
   party_t party;
   pos_account_t pos_account;
   combo_series_t combo_series;
   int64_t nbr_held_q;
   int64_t nbr_written_q;
   int64_t total_held_q;
   int64_t total_written_q;
   int32_t ext_seq_nbr_i;
   int32_t ext_status_i;
   int64_t rem_quantity_i;
   int64_t quantity_i;
   uint32_t ext_trade_number_u;
   uint32_t orig_ext_trade_number_u;
   int32_t residual_i;
   int32_t give_up_number_i;
   int32_t commission_i;
   int32_t combo_deal_price_i;
   char clearing_date_s [8];
   char passthrough_s [32];
   char ex_client_s [16];
   char ext_trade_fee_type_c;
   uint8_t give_up_state_c;
   char reserved_2_s [2];
   uint8_t orig_trade_type_c;
   uint8_t open_close_c;
   char reserved_1_c;
   uint8_t client_category_c;
   uint8_t instigant_c;
   uint8_t cab_price_ind_c;
   char filler_2_s [2];
} cl_trade_secur_part_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 44  */

typedef struct cl_trade_sgx_api
{
   party_t party;
   series_t combo_series;
   int32_t combo_deal_price_i;
   int64_t combo_trade_quantity_i;
   int32_t box_number_i;
} cl_trade_sgx_api_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 67  */

typedef struct cl_trade_trade_report_api
{
   char time_of_agreement_date_s [8];
   char time_of_agreement_time_s [6];
   char filler_2_s [2];
} cl_trade_trade_report_api_t;


/*   TRANSACTION TYPE = BD6, CA10  */
/*   NAMED STRUCT = 21  */

typedef struct cl_trade_warrants_api
{
   uint64_t consideration_u;
   party_t party;
   int32_t ext_status_i;
} cl_trade_warrants_api_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34016  */

typedef struct combo_acc_trans
{
   transaction_type_t transaction_type;
   series_t series;
   order_var_t order_var;
   char exchange_info_s [32];
   quad_word order_number_u;
   give_up_member_t give_up_member;
} combo_acc_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34116  */

typedef struct combo_acc_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   order_var_t order_var;
   char exchange_info_s [32];
   quad_word order_number_u;
   give_up_member_t give_up_member;
} combo_acc_trans_p_t;


/*   TRANSACTION TYPE = MO77  */

typedef struct combo_trade_report_trans
{
   transaction_type_t transaction_type;
   series_t series;
   uint8_t ext_t_state_c;
   char filler_1_s;
   uint16_t items_n;
   combo_trade_report_trans_item_t item [6];
} combo_trade_report_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34907  */

typedef struct combo_trans_part
{
   transaction_type_t transaction_type;
   series_t series;
   order_var_t order_var;
   char exchange_info_s [32];
   give_up_member_t give_up_member;
} combo_trans_part_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34908  */

typedef struct combo_trans_part_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   order_var_t order_var;
   char exchange_info_s [32];
   give_up_member_t give_up_member;
} combo_trans_part_p_t;


/*   TRANSACTION TYPE = YC2  */

typedef struct commit_block_participant_request
{
   transaction_type_t transaction_type;
   series_t series;
   char ex_customer_s [5];
   char country_id_s [2];
   uint8_t block_or_unblock_c;
} commit_block_participant_request_t;


/*   TRANSACTION TYPE = YC1  */

typedef struct commit_block_ptlg_request
{
   transaction_type_t transaction_type;
   series_t series;
   char ptl_id_s [32];
   uint8_t block_or_unblock_c;
   char filler_3_s [3];
} commit_block_ptlg_request_t;


/*   TRANSACTION TYPE = YC4  */

typedef struct commit_mass_cancel_participant_request
{
   transaction_type_t transaction_type;
   series_t series;
   char ex_customer_s [5];
   char country_id_s [2];
   char filler_1_s;
} commit_mass_cancel_participant_request_t;


/*   TRANSACTION TYPE = YC3  */

typedef struct commit_mass_cancel_ptlg_request
{
   transaction_type_t transaction_type;
   series_t series;
   char ptl_id_s [32];
} commit_mass_cancel_ptlg_request_t;


/*   BROADCAST TYPE = BU53  */

typedef struct corp_action_update_bu53
{
   broadcast_type_t broadcast_type;
   uint16_t chg_type_n;
   uint16_t items_n;
   corp_action_update_bu53_item_t item [50];
} corp_action_update_bu53_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34028  */

typedef struct cppx_confirmation_trans
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   prio_crossing_t prio_crossing;
} cppx_confirmation_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34125  */

typedef struct cppx_confirmation_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   quad_word order_number_u;
   prio_crossing_t prio_crossing;
} cppx_confirmation_trans_p_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34023  */

typedef struct cppx_initiation_trans
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   prio_crossing_t prio_crossing;
} cppx_initiation_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34123  */

typedef struct cppx_initiation_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   quad_word order_number_u;
   prio_crossing_t prio_crossing;
} cppx_initiation_trans_p_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO4, MO40, MO44, MO74  */
/*   NAMED STRUCT = 34011  */

typedef struct delete_trans
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   whose_t whose;
   uint8_t bid_or_ask_c;
   char customer_info_s [15];
   char exchange_info_s [32];
} delete_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO388, MO424, MO428  */
/*   NAMED STRUCT = 34111  */

typedef struct delete_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   quad_word order_number_u;
   whose_t whose;
   uint8_t bid_or_ask_c;
   char customer_info_s [15];
   char exchange_info_s [32];
} delete_trans_p_t;


/*   TRANSACTION TYPE = BI81, UA14  */
/*   NAMED STRUCT = 35003  */

typedef struct destination_item
{
   series_t series;
   uint8_t destination_level_c;
   char filler_3_s [3];
} destination_item_t;


/*   TRANSACTION TYPE = BI81, UA14  */
/*   NAMED STRUCT = 35004  */

typedef struct document_url
{
   uint8_t items_c;
   char url_link_s [255];
} document_url_t;


/*   BROADCAST TYPE = BO10  */

typedef struct equil_price_update
{
   broadcast_type_t broadcast_type;
   uint16_t items_n;
   char filler_2_s [2];
   equil_price_update_item_t item [27];
} equil_price_update_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 50004  */

typedef struct exchange_info
{
   char exchange_info_s [32];
} exchange_info_t;


/*   TRANSACTION TYPE = IA119  */

typedef struct exchange_rate_answer
{
   transaction_type_t transaction_type;
   uint16_t segment_number_n;
   uint16_t items_n;
   exchange_rate_answer_item_t item [3500];
} exchange_rate_answer_t;


/*   TRANSACTION TYPE = II19  */

typedef struct exchange_rate_query
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} exchange_rate_query_t;


/*   BROADCAST TYPE = BI83  */

typedef struct exchange_rate_update
{
   broadcast_type_t broadcast_type;
   series_t series;
   int32_t rate_i;
} exchange_rate_update_t;


/*   TRANSACTION TYPE = BD70, TA70  */
/*   NAMED STRUCT = 34402  */

typedef struct extended_trade_ticker
{
   uint16_t trade_condition_n;
   uint16_t deal_info_n;
} extended_trade_ticker_t;


/*   TRANSACTION TYPE = MO93  */

typedef struct ext_mass_quote_trans
{
   transaction_type_t transaction_type;
   series_t series;
   char exchange_info_s [32];
   char ex_client_s [16];
   char filler_2_s [2];
   uint16_t items_n;
   ext_mass_quote_trans_item_t item [10];
} ext_mass_quote_trans_t;


/*   TRANSACTION TYPE = BO5, MA123, MA507  */
/*   NAMED STRUCT = 34845  */

typedef struct ext_quote_update
{
   series_t series;
   quad_word order_number_u;
   uint8_t bid_or_ask_c;
   uint8_t change_reason_c;
   char filler_2_s [2];
} ext_quote_update_t;


/*   TRANSACTION TYPE = BD70, TA70  */
/*   NAMED STRUCT = 34404  */

typedef struct fixed_income_trade_ticker
{
   int32_t corresponding_yield_price_i;
} fixed_income_trade_ticker_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34801  */

typedef struct free_text
{
   char customer_info_s [15];
   char filler_1_s;
} free_text_t;


/*   TRANSACTION TYPE = BD70, BD71, TA70, TA71  */
/*   NAMED STRUCT = 34405  */

typedef struct half_trade_ticker
{
   trading_code_t trading_code;
   int64_t trade_quantity_i;
   uint32_t block_n;
   uint8_t bid_or_ask_c;
   uint8_t deal_source_c;
   char filler_2_s [2];
} half_trade_ticker_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO33, MO43, MO99  */
/*   NAMED STRUCT = 34010  */

typedef struct hv_alter_trans
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   order_var_t order_var;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   int64_t total_volume_i;
   uint8_t delta_quantity_c;
   char filler_3_s [3];
   int64_t balance_quantity_i;
} hv_alter_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO417, MO427  */
/*   NAMED STRUCT = 34110  */

typedef struct hv_alter_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   quad_word order_number_u;
   order_var_t order_var;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   int64_t total_volume_i;
   uint8_t delta_quantity_c;
   char filler_3_s [3];
   int64_t balance_quantity_i;
} hv_alter_trans_p_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO31  */
/*   NAMED STRUCT = 34005  */

typedef struct hv_order_trans
{
   transaction_type_t transaction_type;
   series_t series;
   order_var_t order_var;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   int64_t total_volume_i;
} hv_order_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO415  */
/*   NAMED STRUCT = 34105  */

typedef struct hv_order_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   order_var_t order_var;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   int64_t total_volume_i;
} hv_order_trans_p_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34001  */

typedef struct hv_price_2_trans
{
   transaction_type_t transaction_type;
   series_t series;
   give_up_member_t give_up_member;
   quad_word order_number_bid_u;
   quad_word order_number_ask_u;
   int32_t bid_premium_i;
   int32_t ask_premium_i;
   int64_t bid_quantity_i;
   int64_t ask_quantity_i;
   int64_t bid_total_volume_i;
   int64_t ask_total_volume_i;
   uint32_t block_n;
   uint16_t time_validity_n;
   char ex_client_s [16];
   uint8_t order_type_c;
   char customer_info_s [15];
   char exchange_info_s [32];
   char filler_2_s [2];
} hv_price_2_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34101  */

typedef struct hv_price_2_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   give_up_member_t give_up_member;
   quad_word order_number_bid_u;
   quad_word order_number_ask_u;
   int32_t bid_premium_i;
   int32_t ask_premium_i;
   int64_t bid_quantity_i;
   int64_t ask_quantity_i;
   int64_t bid_total_volume_i;
   int64_t ask_total_volume_i;
   uint32_t block_n;
   uint16_t time_validity_n;
   char ex_client_s [16];
   uint8_t order_type_c;
   char customer_info_s [15];
   char exchange_info_s [32];
   char filler_2_s [2];
} hv_price_2_trans_p_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34818  */

typedef struct inactive_order
{
   uint8_t inactive_c;
   char filler_3_s [3];
} inactive_order_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34025  */

typedef struct indicative_quote
{
   series_t series;
   int64_t buy_quantity_u;
   int64_t sell_quantity_u;
   int32_t buy_price_i;
   int32_t sell_price_i;
   uint8_t bid_quote_action;
   uint8_t ask_quote_action;
   char filler_2_s [2];
} indicative_quote_t;


/*   BROADCAST TYPE = BI9  */

typedef struct info_heartbeat
{
   broadcast_type_t broadcast_type;
   uint8_t heartbeat_interval_c;
   uint8_t instance_c;
   uint8_t tot_instances_c;
   char description_s [40];
   char filler_1_s;
} info_heartbeat_t;


/*   BROADCAST TYPE = BI7  */

typedef struct info_ready
{
   broadcast_type_t broadcast_type;
   int32_t info_type_i;
   series_t series;
   char business_date_s [8];
   char sent_date_s [8];
   char sent_time_s [6];
   char clearing_date_s [8];
   uint16_t seq_num_srm_n;
} info_ready_t;


/*   BROADCAST TYPE = BI41  */

typedef struct instrument_status_info
{
   broadcast_type_t broadcast_type;
   uint16_t items_n;
   char filler_2_s [2];
   instrument_status_info_item_t item [9];
} instrument_status_info_t;


/*   TRANSACTION TYPE = BD2, BO16, BO17, BU120, BU121, BU122, BU123, BU124, BU125, BU126, BU90, CA10, DA120, DA121, DA122, DA123, DA124, DA125, DA126, DA90, DC90, EB10, EB11, IA21, IA22, MA123, MA507, TA70, TA71, UA14  */

typedef struct item_hdr
{
   uint16_t items_n;
   uint16_t size_n;
} item_hdr_t;


/*   BROADCAST TYPE = BL51, BL52  */

typedef struct limit_change
{
   broadcast_type_t broadcast_type;
   uint16_t items_n;
   char filler_2_s [2];
   limit_change_item_t item [50];
} limit_change_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34803  */

typedef struct linked_order_leg
{
   series_t series;
   int32_t premium_i;
   int64_t quantity_i;
   uint32_t block_n;
   uint8_t order_type_c;
   uint8_t bid_or_ask_c;
   char filler_2_s [2];
} linked_order_leg_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34831  */

typedef struct linked_order_leg_number
{
   uint8_t leg_number;
   char filler_3_s [3];
} linked_order_leg_number_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34024  */

typedef struct long_stop_order_trans
{
   transaction_type_t transaction_type;
   series_t series;
   order_var_t order_var;
   stop_series_t stop_series;
   int32_t limit_premium_i;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   int64_t total_volume_i;
} long_stop_order_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34124  */

typedef struct long_stop_order_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   order_var_t order_var;
   stop_series_t stop_series;
   int32_t limit_premium_i;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   int64_t total_volume_i;
} long_stop_order_trans_p_t;


/*   TRANSACTION TYPE = BD2, IA21, IA22  */
/*   NAMED STRUCT = 33034  */

typedef struct market_info_base
{
   int32_t opening_price_i;
   int32_t high_price_i;
   int32_t low_price_i;
   int32_t last_price_i;
   int64_t volume_u;
   int64_t turnover_u;
   uint32_t number_of_deals_u;
   char hhmmss_s [6];
   char trend_indicator_c;
   uint8_t deal_source_c;
} market_info_base_t;


/*   TRANSACTION TYPE = BD2, IA21, IA22  */
/*   NAMED STRUCT = 33040  */

typedef struct market_info_index
{
   int32_t opening_price_i;
   int32_t high_price_i;
   int32_t low_price_i;
   int32_t last_price_i;
   int32_t closing_price_i;
   int32_t change_previous_i;
   int32_t change_yesterday_i;
   int32_t points_of_movement_i;
   char date_time_of_dist_s [14];
   char date_time_of_comp_s [14];
} market_info_index_t;


/*   TRANSACTION TYPE = BD2, IA21, IA22  */
/*   NAMED STRUCT = 33047  */

typedef struct market_info_oseu
{
   int32_t ref_price_i;
} market_info_oseu_t;


/*   TRANSACTION TYPE = BD2  */
/*   NAMED STRUCT = 33043  */

typedef struct market_info_reason
{
   uint8_t edited_price_info_reason_c;
   char filler_3_s [3];
} market_info_reason_t;


/*   TRANSACTION TYPE = BD2  */
/*   NAMED STRUCT = 33038  */

typedef struct market_info_series
{
   series_t series;
   int32_t reserved_i;
   uint8_t all_or_none_c;
   char filler_3_s [3];
} market_info_series_t;


/*   BROADCAST TYPE = BO38  */

typedef struct market_maker_protection_info
{
   broadcast_type_t broadcast_type;
   trading_code_t trading_code;
   series_t series;
   int64_t calc_quantity_protection_q;
   int64_t calc_delta_protection_q;
} market_maker_protection_info_t;


/*   TRANSACTION TYPE = MO96  */

typedef struct mass_quote_trans
{
   transaction_type_t transaction_type;
   series_t series;
   char exchange_info_short [16];
   char ex_client_s [16];
   char filler_2_s [2];
   uint16_t items_n;
   mass_quote_trans_item_t item [36];
} mass_quote_trans_t;


/*   TRANSACTION TYPE = BI81, UA14  */
/*   NAMED STRUCT = 35001  */

typedef struct message_core_info
{
   uint32_t sequence_number_u;
   uint8_t message_information_type_c;
   char message_source_s [80];
   char yyyymmdd_s [8];
   char hhmmss_s [6];
   uint8_t message_priority_c;
   char message_header_s [80];
   uint8_t update_status_note_c;
   char filler_3_s [3];
} message_core_info_t;


/*   TRANSACTION TYPE = BI81, UA14  */
/*   NAMED STRUCT = 35002  */

typedef struct message_information
{
   uint16_t items_n;
   char filler_2_s [2];
   message_information_item_t item [10];
} message_information_t;


/*   BROADCAST TYPE = BU87  */

typedef struct mm_protection_update
{
   broadcast_type_t broadcast_type;
   uint16_t chg_type_n;
   char filler_2_s [2];
   da87_t da87;
} mm_protection_update_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34918  */

typedef struct mp_trade_price
{
   series_t series;
   quad_word order_number_u;
   uint8_t bid_or_ask_c;
   uint8_t deal_source_c;
   uint16_t trade_condition_n;
   int32_t deal_price_i;
   int64_t deal_quantity_i;
   uint8_t ext_t_state_c;
   uint8_t opposing_deal_source_c;
   uint16_t exch_order_type_n;
   quad_word opposing_order_number_u;
} mp_trade_price_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34833  */

typedef struct multi_leg_order_insert
{
   transaction_type_t transaction_type;
   series_t series;
   int32_t premium_i;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   char customer_info_s [15];
   char ex_client_s [16];
   uint8_t open_close_req_c;
   uint8_t multi_leg_price_type_c;
   uint8_t order_type_c;
   uint8_t items_c;
   char filler_1_s;
   multi_leg_order_insert_item_t item [5];
} multi_leg_order_insert_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34834  */

typedef struct multi_leg_order_insert_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   int32_t premium_i;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   char customer_info_s [15];
   char ex_client_s [16];
   uint8_t open_close_req_c;
   uint8_t multi_leg_price_type_c;
   uint8_t order_type_c;
   uint8_t items_c;
   char filler_1_s;
   multi_leg_order_insert_p_item_t item [5];
} multi_leg_order_insert_p_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34832  */

typedef struct multi_leg_order_leg_number
{
   uint8_t leg_number;
   char filler_3_s [3];
} multi_leg_order_leg_number_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34906  */

typedef struct multi_order_response
{
   int32_t transaction_status_i;
   int32_t trans_ack_i;
   uint8_t item_number_c;
   char filler_3_s [3];
} multi_order_response_t;


/*   BROADCAST TYPE = BU18  */

typedef struct non_trading_days_update_bu18
{
   broadcast_type_t broadcast_type;
   uint16_t chg_type_n;
   char filler_2_s [2];
   da18_t da18;
} non_trading_days_update_bu18_t;


/*   TRANSACTION TYPE = BU122, BU123, DA122, DA123  */
/*   NAMED STRUCT = 37103  */

typedef struct ns_block_size
{
   int64_t maximum_size_u;
   uint32_t minimum_size_n;
   uint32_t block_n;
   uint8_t lot_type_c;
   char filler_3_s [3];
} ns_block_size_t;


/*   TRANSACTION TYPE = BU122, BU123, DA122, DA123  */
/*   NAMED STRUCT = 37104  */

typedef struct ns_calc_rule
{
   uint32_t accr_intr_round_u;
   uint32_t clean_pr_round_u;
   uint16_t yield_conv_n;
   uint16_t ex_coupon_n;
   uint8_t accr_intr_ud_c;
   uint8_t clean_pr_ud_c;
   uint8_t day_count_conv_c;
   uint8_t eom_count_conv_c;
   uint8_t set_start_consid_c;
   uint8_t set_end_consid_c;
   uint8_t calculation_conv_c;
   uint8_t ex_coupon_calc_type_c;
} ns_calc_rule_t;


/*   TRANSACTION TYPE = BU126, DA126  */
/*   NAMED STRUCT = 37308  */

typedef struct ns_combo_series_leg
{
   series_t series;
   uint16_t ratio_n;
   char op_if_buy_c;
   char op_if_sell_c;
} ns_combo_series_leg_t;


/*   TRANSACTION TYPE = BU120, BU121, DA120, DA121  */
/*   NAMED STRUCT = 37203  */

typedef struct ns_coupon_dates
{
   char date_coupdiv_s [8];
   char date_booksclose_s [8];
   uint32_t dividend_i;
} ns_coupon_dates_t;


/*   TRANSACTION TYPE = BU120, BU121, BU122, BU123, BU124, BU125, DA120, DA121, DA122, DA123, DA124, DA125  */
/*   NAMED STRUCT = 37001  */

typedef struct ns_delta_header
{
   int64_t download_ref_number_q;
   time_spec_t full_answer_timestamp;
   uint8_t full_answer_c;
   char filler_3_s [3];
} ns_delta_header_t;


/*   TRANSACTION TYPE = BU120, BU121, DA120, DA121  */
/*   NAMED STRUCT = 37202  */

typedef struct ns_fixed_income
{
   int64_t nominal_value_q;
   uint32_t coupon_interest_i;
   uint16_t dec_in_nominal_n;
   uint16_t coupon_settlement_days_n;
   uint16_t coupon_frequency_n;
   uint16_t rate_determ_days_n;
   char date_release_s [8];
   char date_termination_s [8];
   char date_dated_s [8];
   char date_proceed_s [8];
   uint8_t fixed_income_type_c;
   uint8_t day_calc_rule_c;
   char filler_2_s [2];
} ns_fixed_income_t;


/*   TRANSACTION TYPE = BU122, BU123, DA122, DA123  */
/*   NAMED STRUCT = 37101  */

typedef struct ns_inst_class_basic
{
   series_t series;
   upper_level_series_t upper_level_series;
   int32_t price_quot_factor_i;
   int32_t contract_size_i;
   int32_t redemption_value_i;
   int32_t undisclosed_min_ord_val_i;
   int32_t opt_min_ord_val_i;
   int32_t opt_min_trade_val_i;
   uint16_t derivate_level_n;
   uint16_t dec_in_strike_price_n;
   uint16_t dec_in_contr_size_n;
   uint16_t rnt_id_n;
   uint16_t virt_commodity_n;
   uint16_t settlement_days_n;
   uint8_t settl_day_unit_c;
   char inc_id_s [14];
   char name_s [32];
   char trc_id_s [10];
   char base_cur_s [3];
   uint8_t traded_c;
   uint8_t price_unit_premium_c;
   uint8_t price_unit_strike_c;
   uint8_t trd_cur_unit_c;
   uint8_t db_operation_c;
   char filler_3_s [3];
} ns_inst_class_basic_t;


/*   TRANSACTION TYPE = BU122, BU123, DA122, DA123  */
/*   NAMED STRUCT = 37127  */

typedef struct ns_inst_class_oat
{
   uint8_t limit_order_allwd_c;
   uint8_t market_orders_allowed_c;
   uint8_t fill_or_kill_allowed_c;
   uint8_t fill_and_kill_allowed_c;
   uint8_t fill_and_store_c;
   uint8_t all_or_none_c;
   uint8_t imb_orders_allwd_c;
   uint8_t mtl_orders_rlot_allwd_c;
   uint8_t allow_hidd_aggr_c;
   uint8_t best_limit_c;
   uint8_t market_bid_c;
   uint8_t price_stabilization_c;
   uint8_t short_sell_c;
   uint8_t short_sell_validation_c;
   uint8_t allow_cp_mar_c;
   uint8_t allow_cp_lim_c;
   uint8_t allow_undisclosed_qty_c;
   uint8_t sso_orders_allowed_c;
   uint8_t stop_orders_allowed_c;
   uint8_t good_till_session_c;
   uint8_t priority_ranking_c;
   char filler_3_s [3];
} ns_inst_class_oat_t;


/*   TRANSACTION TYPE = BU122, BU123, DA122, DA123  */
/*   NAMED STRUCT = 37105  */

typedef struct ns_inst_class_secur
{
   int32_t exerc_limit_i;
   uint16_t dec_in_deliv_n;
   uint16_t cleared_dec_in_qty_n;
   uint16_t dec_in_fixing_n;
   uint8_t exerc_limit_unit_c;
   char settl_cur_id_s [32];
   char csd_id_s [12];
   uint8_t fixing_req_c;
} ns_inst_class_secur_t;


/*   TRANSACTION TYPE = BU124, BU125, BU126, DA124, DA125, DA126  */
/*   NAMED STRUCT = 37301  */

typedef struct ns_inst_series_basic
{
   series_t series;
   uint16_t step_size_multiple_n;
   char ins_id_s [32];
   char long_ins_id_s [32];
   char date_last_trading_s [8];
   char time_last_trading_s [6];
   char date_first_trading_s [8];
   char time_first_trading_s [6];
   uint8_t series_status_c;
   uint8_t suspended_c;
   uint8_t traded_in_click_c;
   uint8_t db_operation_c;
   uint8_t trade_reporting_only_c;
   uint8_t traded_c;
} ns_inst_series_basic_t;


/*   TRANSACTION TYPE = BU124, BU125, DA124, DA125  */
/*   NAMED STRUCT = 37302  */

typedef struct ns_inst_series_basic_single
{
   upper_level_series_t upper_level_series;
   int32_t contract_size_i;
   int32_t price_quot_factor_i;
   uint16_t state_number_n;
   uint16_t ex_coupon_n;
   char isin_code_s [12];
   char settlement_date_s [8];
   char first_settlement_date_s [8];
   char date_notation_s [8];
   uint8_t deliverable_c;
   char effective_exp_date_s [8];
   uint8_t ext_info_source_c;
   uint8_t participant_defined_c;
   char filler_1_s;
} ns_inst_series_basic_single_t;


/*   TRANSACTION TYPE = BU125, DA125  */
/*   NAMED STRUCT = 37306  */

typedef struct ns_inst_series_bo
{
   char isin_code_old_s [12];
   uint8_t tm_template_c;
   uint8_t tm_series_c;
   uint8_t accept_collateral_c;
   char filler_1_s;
} ns_inst_series_bo_t;


/*   TRANSACTION TYPE = BU124, BU125, BU126, DA124, DA125, DA126  */
/*   NAMED STRUCT = 37310  */

typedef struct ns_inst_series_id
{
   int32_t orderbook_id_i;
} ns_inst_series_id_t;


/*   TRANSACTION TYPE = BU124, BU125, DA124, DA125  */
/*   NAMED STRUCT = 37303  */

typedef struct ns_inst_series_power
{
   char date_delivery_start_s [8];
   char date_delivery_stop_s [8];
} ns_inst_series_power_t;


/*   TRANSACTION TYPE = BU90, DA90, DC90  */
/*   NAMED STRUCT = 37801  */

typedef struct ns_pre_trade_limit
{
   int32_t order_rate_limit_i;
   char ptl_suffix_s [16];
   char country_id_s [2];
   char ex_customer_s [5];
   char spons_user_name_s [32];
   char sponsored_client_country_id_s [2];
   char sponsored_client_ex_customer_s [5];
   int16_t warning_breach_lvl_n;
   int16_t not_breach_lvl_n;
   uint8_t enable_warn_email_c;
   uint8_t enable_not_email_c;
   uint8_t enable_breach_email_c;
   uint8_t db_operation_c;
   uint8_t intraday_c;
   char valid_from_date_s [8];
   uint8_t enable_restr_instr_c;
   uint8_t enable_def_user_c;
   char filler_3_s [3];
} ns_pre_trade_limit_t;


/*   TRANSACTION TYPE = BU90, DA90  */
/*   NAMED STRUCT = 37805  */

typedef struct ns_pre_trade_limit_id
{
   char ptl_id_s [32];
} ns_pre_trade_limit_id_t;


/*   TRANSACTION TYPE = BU90, DA90, DC90  */
/*   NAMED STRUCT = 37804  */

typedef struct ns_pre_trade_limit_not
{
   char not_email_addr_s [128];
   char valid_from_date_s [8];
} ns_pre_trade_limit_not_t;


/*   TRANSACTION TYPE = BU90, DA90, DC90  */
/*   NAMED STRUCT = 37803  */

typedef struct ns_pre_trade_limit_param
{
   series_t series;
   int64_t max_order_size_q;
   int64_t open_buy_q;
   int64_t open_sell_q;
   int64_t traded_bought_q;
   int64_t traded_sold_q;
   int64_t traded_net_q;
   int64_t total_buy_q;
   int64_t total_sell_q;
   int64_t total_net_buy_q;
   int64_t total_net_sell_q;
   uint8_t pre_trade_limit_param_unit_c;
   char valid_from_date_s [8];
   char filler_3_s [3];
} ns_pre_trade_limit_param_t;


/*   TRANSACTION TYPE = BU90, DA90, DC90  */
/*   NAMED STRUCT = 37802  */

typedef struct ns_pre_trade_limit_user
{
   user_code_t user_code;
   char valid_from_date_s [8];
} ns_pre_trade_limit_user_t;


/*   TRANSACTION TYPE = BU122, BU123, DA122, DA123  */
/*   NAMED STRUCT = 37102  */

typedef struct ns_price_tick
{
   tick_size_t tick_size;
   uint16_t dec_in_premium_n;
   char is_fractions_c;
   char filler_1_s;
} ns_price_tick_t;


/*   TRANSACTION TYPE = BU120, BU121, BU122, BU123, BU124, BU125, DA120, DA121, DA122, DA123, DA124, DA125  */
/*   NAMED STRUCT = 37002  */

typedef struct ns_remove
{
   series_t series;
} ns_remove_t;


/*   TRANSACTION TYPE = BU120, BU121, DA120, DA121  */
/*   NAMED STRUCT = 37201  */

typedef struct ns_underlying_basic
{
   uint16_t commodity_n;
   uint16_t linked_commodity_n;
   uint16_t state_number_n;
   uint16_t dec_in_price_n;
   char com_id_s [6];
   char isin_code_s [12];
   char name_s [32];
   char base_cur_s [3];
   uint8_t deliverable_c;
   uint8_t underlying_type_c;
   uint8_t price_unit_c;
   uint8_t underlying_status_c;
   char underlying_issuer_s [6];
   char sector_code_s [4];
   uint8_t virtual_c;
   char country_id_s [2];
   char ext_provider_c;
   char external_id_s [40];
   uint8_t cur_unit_c;
   uint8_t db_operation_c;
   char filler_3_s [3];
} ns_underlying_basic_t;


/*   TRANSACTION TYPE = BD2, IA21, IA22  */
/*   NAMED STRUCT = 33031  */

typedef struct ob_levels_closing
{
   int32_t closing_price_i;
   int64_t open_balance_u;
} ob_levels_closing_t;


/*   TRANSACTION TYPE = IQ21, IQ22  */
/*   NAMED STRUCT = 33002  */

typedef struct ob_levels_id
{
   series_t series;
   uint32_t block_n;
} ob_levels_id_t;


/*   TRANSACTION TYPE = IA21, IA22  */
/*   NAMED STRUCT = 33032  */

typedef struct ob_levels_next_query
{
   uint16_t segment_number_n;
   uint8_t instance_c;
   uint8_t instance_next_c;
   series_next_t series_next;
} ob_levels_next_query_t;


/*   TRANSACTION TYPE = IQ21, IQ22  */
/*   NAMED STRUCT = 33020  */

typedef struct ob_levels_query_data
{
   uint16_t segment_number_n;
   char filler_2_s [2];
} ob_levels_query_data_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34902  */

typedef struct order_change_combined
{
   int64_t mp_quantity_i;
   int64_t total_volume_i;
   uint8_t item_number_c;
   uint8_t bid_or_ask_c;
   uint8_t change_reason_c;
   char filler_1_s;
} order_change_combined_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34919  */

typedef struct order_chg_sep_trans_ack
{
   int32_t trans_ack_i;
   order_change_separate_t order_change_separate;
} order_chg_sep_trans_ack_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34917  */

typedef struct order_info
{
   time_spec_t timestamp_in;
   time_spec_t timestamp_created;
   quad_word order_number_u;
   party_t party;
   order_t order;
   int64_t total_volume_i;
   int64_t display_quantity_i;
   int64_t orig_total_volume_i;
   int64_t orig_shown_quantity_i;
   uint32_t order_state_u;
} order_info_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34921  */

typedef struct order_leg_trade_info
{
   series_t series;
   match_id_t match_id;
   quad_word order_number_u;
   int32_t trade_price_i;
   int64_t trade_quantity_i;
   uint8_t item_number_c;
   uint8_t deal_source_c;
   uint8_t bid_or_ask_c;
   char filler_1_s;
} order_leg_trade_info_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34804  */

typedef struct order_owner
{
   trading_code_t owner;
} order_owner_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34905  */

typedef struct order_price_change
{
   series_t series;
   quad_word order_number_u;
   int32_t premium_i;
   time_spec_t execution_timestamp;
   uint8_t bid_or_ask_c;
   uint8_t change_reason_c;
   char filler_2_s [2];
} order_price_change_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34904  */

typedef struct order_return_info
{
   int32_t trans_ack_i;
   quad_word order_number_u;
   originator_trading_code_t originator_trading_code;
   time_spec_t execution_timestamp;
} order_return_info_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34913  */

typedef struct order_state
{
   uint32_t order_state_u;
} order_state_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34910  */

typedef struct order_status
{
   char exchange_info_s [32];
   char customer_info_s [15];
   uint8_t open_close_req_c;
   int32_t premium_i;
   party_t party;
   int64_t orig_shown_quantity_i;
   int64_t orig_total_volume_i;
   int64_t rem_quantity_i;
   uint16_t transaction_number_n;
   uint16_t exch_order_type_n;
   give_up_member_t give_up_member;
   char ex_client_s [16];
   uint8_t order_type_c;
   char filler_3_s [3];
} order_status_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34819  */

typedef struct order_submitter
{
   trading_code_t submitter;
} order_submitter_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34920  */

typedef struct order_trade_info
{
   match_id_t match_id;
   int32_t trade_price_i;
   int64_t trade_quantity_i;
   uint8_t item_number_c;
   uint8_t deal_source_c;
   uint8_t bid_or_ask_c;
   char filler_1_s;
} order_trade_info_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34922  */

typedef struct order_trade_info_asx
{
   quad_word opposing_order_number_u;
   uint16_t trade_condition_n;
   uint16_t exch_order_type_n;
   uint8_t ext_t_state_c;
   uint8_t opposing_deal_source_c;
   uint8_t aggressive_c;
   uint8_t bid_or_ask_c;
} order_trade_info_asx_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34004  */

typedef struct order_trans
{
   transaction_type_t transaction_type;
   series_t series;
   order_var_t order_var;
   char exchange_info_s [32];
   give_up_member_t give_up_member;
} order_trans_t;


typedef struct ose_exchange_info
{
   char acc_type_c;
   char tr_purpose_flag_c;
   char customer_use_s [20];
   char blank_s [10];
} ose_exchange_info_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34813  */

typedef struct peg_order
{
   int32_t limit_price_i;
   int16_t peg_price_offset_n;
   uint8_t peg_type_c;
   char filler_1_s;
} peg_order_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34002  */

typedef struct price_2_trans
{
   transaction_type_t transaction_type;
   series_t series;
   int32_t bid_premium_i;
   int32_t ask_premium_i;
   quad_word order_number_bid_u;
   quad_word order_number_ask_u;
   int64_t bid_quantity_i;
   int64_t ask_quantity_i;
   uint32_t block_n;
   uint16_t time_validity_n;
   char ex_client_s [16];
   char filler_2_s [2];
} price_2_trans_t;


/*   TRANSACTION TYPE = BO16, BO17, IA21, IA22  */
/*   NAMED STRUCT = 33051  */

typedef struct price_depth
{
   int32_t price_i;
   uint8_t price_depth_position_c;
   uint8_t price_depth_operator_c;
   uint8_t bid_or_ask_c;
   char filler_1_s;
} price_depth_t;


/*   TRANSACTION TYPE = BO16, BO17  */
/*   NAMED STRUCT = 33056  */

typedef struct price_depth_delete_levels
{
   uint8_t price_depth_position_c;
   uint8_t levels_to_delete_c;
   uint8_t bid_or_ask_c;
   char filler_1_s;
} price_depth_delete_levels_t;


/*   TRANSACTION TYPE = BO16, BO17, IA21, IA22  */
/*   NAMED STRUCT = 33050  */

typedef struct price_depth_series
{
   series_t series;
   uint32_t synchronization_number_u;
   uint32_t block_n;
   uint8_t premium_levels_c;
   uint8_t demands_populated_c;
   char filler_2_s [2];
   uint32_t reserved1_u;
} price_depth_series_t;


/*   TRANSACTION TYPE = BO16, BO17, IA21, IA22  */
/*   NAMED STRUCT = 33052  */

typedef struct price_depth_with_volume
{
   int32_t price_i;
   int64_t quantity_u;
   uint8_t hidden_quantity_c;
   uint8_t price_depth_position_c;
   uint8_t price_depth_operator_c;
   uint8_t bid_or_ask_c;
} price_depth_with_volume_t;


/*   TRANSACTION TYPE = BO16, BO17, IA21, IA22  */
/*   NAMED STRUCT = 33053  */

typedef struct price_depth_with_volume_orders
{
   int32_t price_i;
   int64_t quantity_u;
   uint32_t no_of_orders_u;
   uint8_t hidden_quantity_c;
   uint8_t price_depth_position_c;
   uint8_t price_depth_operator_c;
   uint8_t bid_or_ask_c;
} price_depth_with_volume_orders_t;


/*   TRANSACTION TYPE = EB11  */
/*   NAMED STRUCT = 50014  */

typedef struct price_item
{
   int64_t price_q;
   uint16_t dec_in_price_n;
   char filler_2_s [2];
} price_item_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34003  */

typedef struct price_trans
{
   transaction_type_t transaction_type;
   series_t series;
   order_var_t order_var;
   quad_word order_number_u;
   give_up_member_t give_up_member;
} price_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34103  */

typedef struct price_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   order_var_t order_var;
   quad_word order_number_u;
   give_up_member_t give_up_member;
} price_trans_p_t;


/*   TRANSACTION TYPE = EB10  */
/*   NAMED STRUCT = 20057  */

typedef struct price_vola_settl_item
{
   series_t series;
   int32_t last_price_i;
   int32_t settle_price_i;
   int32_t actual_vol_i;
   int32_t theo_price_i;
   int32_t current_vwv_i;
   int32_t calc_vwv_i;
   int32_t prev_vwv_i;
   int32_t risk_free_rate_i;
   int32_t ulg_price_i;
   int32_t implied_vol_i;
   uint8_t settlement_price_type_c;
   char filler_3_s [3];
} price_vola_settl_item_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34020  */

typedef struct prio_crossing_trans
{
   transaction_type_t transaction_type;
   series_t series;
   prio_crossing_t prio_crossing;
} prio_crossing_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34118  */

typedef struct prio_crossing_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   prio_crossing_t prio_crossing;
} prio_crossing_trans_p_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34814  */

typedef struct quantity_condition
{
   int64_t minimum_quantity_i;
} quantity_condition_t;


/*   TRANSACTION TYPE = TR71  */

typedef struct query_amended_trades
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_amended_trades_t;


/*   TRANSACTION TYPE = MM68  */

typedef struct query_at_the_money
{
   transaction_type_t transaction_type;
   series_t series;
   series_t search_series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_at_the_money_t;


/*   TRANSACTION TYPE = UQ20  */

typedef struct query_bi73_signals_sent
{
   transaction_type_t transaction_type;
   search_series_t search_series;
   uint16_t segment_number_n;
   char business_date_s [8];
   char clearing_date_s [8];
   uint16_t seq_num_srm_n;
} query_bi73_signals_sent_t;


/*   TRANSACTION TYPE = UQ9  */

typedef struct query_bi7_signals_sent
{
   transaction_type_t transaction_type;
   search_series_t search_series;
   uint16_t segment_number_n;
   char business_date_s [8];
   uint16_t seq_num_srm_n;
} query_bi7_signals_sent_t;


/*   TRANSACTION TYPE = UQ14  */

typedef struct query_bi81_broadcasts_sent
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   uint8_t message_information_type_c;
   uint8_t message_priority_c;
   char date_s [8];
   uint32_t from_sequence_number_u;
   uint32_t to_sequence_number_u;
   search_series_t search_series;
   uint8_t update_status_note_c;
   char filler_3_s [3];
} query_bi81_broadcasts_sent_t;


/*   TRANSACTION TYPE = MQ99  */

typedef struct query_block_size
{
   transaction_type_t transaction_type;
   series_t series;
} query_block_size_t;


/*   TRANSACTION TYPE = DQ6  */

typedef struct query_broker
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char country_id_s [2];
   char ex_customer_s [5];
   char filler_3_s [3];
} query_broker_t;


/*   TRANSACTION TYPE = UQ12  */

typedef struct query_business_date
{
   transaction_type_t transaction_type;
} query_business_date_t;


/*   TRANSACTION TYPE = DQ28  */

typedef struct query_central_group
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_central_group_t;


/*   TRANSACTION TYPE = CQ68  */

typedef struct query_clearing_date
{
   transaction_type_t transaction_type;
   series_t series;
   search_series_t search_series;
} query_clearing_date_t;


/*   TRANSACTION TYPE = DQ126  */

typedef struct query_combo
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_combo_t;


/*   TRANSACTION TYPE = DQ15  */

typedef struct query_converted_series
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   uint16_t adjust_ident_n;
} query_converted_series_t;


/*   TRANSACTION TYPE = DQ53  */

typedef struct query_corp_action
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_corp_action_t;


/*   TRANSACTION TYPE = DQ33  */

typedef struct query_currency
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_currency_t;


/*   TRANSACTION TYPE = DQ46  */

typedef struct query_deal_source
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_deal_source_t;


/*   TRANSACTION TYPE = DQ120, DQ121, DQ122, DQ123, DQ124, DQ125  */

typedef struct query_delta
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
   int64_t download_ref_number_q;
   time_spec_t full_answer_timestamp;
} query_delta_t;


/*   TRANSACTION TYPE = DQ24  */

typedef struct query_exchange_dq24
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_exchange_dq24_t;


/*   TRANSACTION TYPE = MH93  */

typedef struct query_ext_mass_quote_update
{
   transaction_type_t transaction_type;
   series_t series;
   party_t party;
   uint32_t sequence_first_u;
   uint32_t sequence_last_u;
} query_ext_mass_quote_update_t;


/*   TRANSACTION TYPE = IQ21, IQ22  */

typedef struct query_hdr
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t items_n;
   uint16_t size_n;
} query_hdr_t;


/*   TRANSACTION TYPE = DQ22, DQ3  */

typedef struct query_instrument
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_instrument_t;


/*   TRANSACTION TYPE = DQ8  */

typedef struct query_instrument_group
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_instrument_group_t;


/*   TRANSACTION TYPE = UQ15  */

typedef struct query_instrument_status
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   uint16_t state_level_e;
} query_instrument_status_t;


/*   TRANSACTION TYPE = ML1  */

typedef struct query_limit_trans
{
   transaction_type_t transaction_type;
   series_t series;
   uint8_t only_this_series_c;
   char filler_3_s [3];
} query_limit_trans_t;


/*   TRANSACTION TYPE = DQ23, DQ7  */

typedef struct query_market
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_market_t;


/*   TRANSACTION TYPE = MH96  */

typedef struct query_mass_quote_update
{
   transaction_type_t transaction_type;
   series_t series;
   party_t party;
   uint32_t sequence_first_u;
   uint32_t sequence_last_u;
} query_mass_quote_update_t;


/*   TRANSACTION TYPE = CQ10  */

typedef struct query_missing_trade
{
   transaction_type_t transaction_type;
   series_t series;
   int32_t sequence_first_i;
   int32_t sequence_last_i;
   char date_s [8];
} query_missing_trade_t;


/*   TRANSACTION TYPE = DQ87  */

typedef struct query_mm_protection
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_mm_protection_t;


/*   TRANSACTION TYPE = DQ18  */

typedef struct query_non_trading_days
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_non_trading_days_t;


/*   TRANSACTION TYPE = MH123  */

typedef struct query_order_history_server
{
   transaction_type_t transaction_type;
   series_t series;
   char yyyymmdd_s [8];
   quad_word order_number_u;
   uint32_t sequence_first_u;
   uint32_t sequence_last_u;
   uint8_t originates_from_trade_c;
   char filler_3_s [3];
} query_order_history_server_t;


/*   TRANSACTION TYPE = MA123, MA507  */
/*   NAMED STRUCT = 34133  */

typedef struct query_order_history_server_next
{
   series_t next_series;
   uint32_t sequence_number_next_u;
} query_order_history_server_next_t;


/*   TRANSACTION TYPE = MH507  */

typedef struct query_order_history_server_p
{
   transaction_type_t transaction_type;
   series_t series;
   party_t party;
   char yyyymmdd_s [8];
   quad_word order_number_u;
   uint32_t sequence_first_u;
   uint32_t sequence_last_u;
   uint8_t originates_from_trade_c;
   char filler_3_s [3];
} query_order_history_server_p_t;


/*   TRANSACTION TYPE = UQ1  */

typedef struct query_partition
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_partition_t;


/*   TRANSACTION TYPE = II17  */

typedef struct query_prel_settlement
{
   transaction_type_t transaction_type;
   series_t series;
   char settlement_date_s [8];
   uint16_t segment_number_n;
   uint8_t settlement_price_type_c;
   char filler_1_s;
} query_prel_settlement_t;


/*   TRANSACTION TYPE = DQ90  */

typedef struct query_pre_trade_limit
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_pre_trade_limit_t;


/*   TRANSACTION TYPE = RQ62  */

typedef struct query_price_vola_settl
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char date_s [8];
   char filler_2_s [2];
} query_price_vola_settl_t;


/*   TRANSACTION TYPE = IQ2147  */

typedef struct query_ref_price_trans
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_ref_price_trans_t;


/*   TRANSACTION TYPE = DQ76  */

typedef struct query_state_type
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_state_type_t;


/*   TRANSACTION TYPE = IQ12  */

typedef struct query_tot_equil_prices
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_tot_equil_prices_t;


/*   TRANSACTION TYPE = MQ7  */

typedef struct query_tot_ob
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   uint8_t bid_or_ask_c;
   uint8_t only_this_series_c;
   char filler_2_s [2];
} query_tot_ob_t;


/*   TRANSACTION TYPE = MQ32, MQ416, MQ47, MQ48, MQ49, MQ5, MQ50, MQ78, MQ8, MQ9  */

typedef struct query_tot_order
{
   transaction_type_t transaction_type;
   series_t series;
   whose_t whose;
   uint32_t order_index_u;
} query_tot_order_t;


/*   TRANSACTION TYPE = MQ392, MQ393, MQ432, MQ433, MQ434  */

typedef struct query_tot_order_p
{
   transaction_type_t transaction_type;
   series_t series;
   whose_t whose;
   uint32_t order_index_u;
} query_tot_order_p_t;


/*   TRANSACTION TYPE = MQ80  */

typedef struct query_tot_party
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   uint8_t bid_or_ask_c;
   char filler_3_s [3];
} query_tot_party_t;


/*   TRANSACTION TYPE = DQ45  */

typedef struct query_trade_report_types
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_trade_report_types_t;


/*   TRANSACTION TYPE = IQ42  */

typedef struct query_trade_statistics
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char date_s [8];
   char filler_2_s [2];
} query_trade_statistics_t;


/*   TRANSACTION TYPE = TR70  */

typedef struct query_trade_ticker
{
   transaction_type_t transaction_type;
   series_t series;
   series_t search_series;
   time_spec_t timestamp;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_trade_ticker_t;


/*   TRANSACTION TYPE = DQ29  */

typedef struct query_trading_state
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_trading_state_t;


/*   TRANSACTION TYPE = DQ14  */

typedef struct query_underlying_adjustment
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char date_adjust_s [8];
   char filler_2_s [2];
} query_underlying_adjustment_t;


/*   TRANSACTION TYPE = DQ30  */

typedef struct query_user_type_info
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_user_type_info_t;


/*   TRANSACTION TYPE = DQ54  */

typedef struct query_valid_sector_code
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t segment_number_n;
   char filler_2_s [2];
} query_valid_sector_code_t;


/*   TRANSACTION TYPE = MO39  */

typedef struct quote_request_vol
{
   transaction_type_t transaction_type;
   series_t series;
   uint32_t block_n;
   uint8_t bid_or_ask_c;
   char filler_3_s [3];
   int64_t mp_quantity_i;
} quote_request_vol_t;


/*   BROADCAST TYPE = MI4  */

typedef struct quote_request_vol_info
{
   broadcast_type_t broadcast_type;
   series_t series;
   user_code_t user_code;
   uint32_t block_n;
   uint8_t bid_or_ask_c;
   char filler_3_s [3];
   int64_t mp_quantity_i;
} quote_request_vol_info_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34949  */

typedef struct ranking_time
{
   time_spec_t timestamp_ranking;
} ranking_time_t;


/*   BROADCAST TYPE = BI2147  */

typedef struct ref_price_change
{
   broadcast_type_t broadcast_type;
   uint16_t items_n;
   char filler_2_s [2];
   ref_price_change_item_t item [35];
} ref_price_change_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34812  */

typedef struct reserve_order
{
   int64_t display_quantity_i;
   int64_t original_display_quantity_i;
} reserve_order_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34901  */

typedef struct segment_instance_number
{
   uint16_t segment_number_n;
   uint8_t instance_c;
   char filler_1_s;
   uint32_t sequence_number_u;
   trading_code_t trading_code;
} segment_instance_number_t;


/*   BROADCAST TYPE = BI63  */

typedef struct settle_price_update
{
   broadcast_type_t broadcast_type;
   uint16_t items_n;
   char filler_2_s [2];
   settle_price_update_item_t item [30];
} settle_price_update_t;


/*   TRANSACTION TYPE = DC87  */

typedef struct set_mm_protection
{
   transaction_type_t transaction_type;
   series_t series;
   da87_t da87;
} set_mm_protection_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34808  */

typedef struct single_order_insert
{
   transaction_type_t transaction_type;
   series_t series;
   uint16_t items_n;
   uint16_t size_n;
} single_order_insert_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34809  */

typedef struct single_order_update
{
   transaction_type_t transaction_type;
   series_t series;
   quad_word order_number_u;
   uint8_t bid_or_ask_c;
   char filler_3_s [3];
   uint16_t items_n;
   uint16_t size_n;
} single_order_update_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO41  */
/*   NAMED STRUCT = 34017  */

typedef struct stop_order_trans
{
   transaction_type_t transaction_type;
   series_t series;
   order_var_t order_var;
   stop_series_t stop_series;
   int32_t limit_premium_i;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   int64_t total_volume_i;
} stop_order_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO425  */
/*   NAMED STRUCT = 34117  */

typedef struct stop_order_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   order_var_t order_var;
   stop_series_t stop_series;
   int32_t limit_premium_i;
   give_up_member_t give_up_member;
   char exchange_info_s [32];
   int64_t total_volume_i;
} stop_order_trans_p_t;


/*   TRANSACTION TYPE = BD2, BD6, BD70, BD71, BI81, BO16, BO17, BO25, BO5, BU120, BU121, BU122, BU123, BU124, BU125, BU126, BU90, CA10, DA120, DA121, DA122, DA123, DA124, DA125, DA126, DA90, DC90, EB10, EB11, IA21, IA22, IQ21, IQ22, MA123, MA507, TA70, TA71, UA14  */

typedef struct sub_item_hdr
{
   uint16_t named_struct_n;
   uint16_t size_n;
} sub_item_hdr_t;


/*   BROADCAST TYPE = BI1  */

typedef struct suspend_resume_trading
{
   broadcast_type_t broadcast_type;
   uint16_t commodity_n;
   uint8_t on_off_c;
   char filler_1_s;
} suspend_resume_trading_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34807  */

typedef struct time_in_force
{
   uint16_t time_validity_n;
   char filler_2_s [2];
} time_in_force_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34014  */

typedef struct tm_trade_rpt_trans
{
   transaction_type_t transaction_type;
   series_t series;
   order_var_t order_var;
   party_t party;
   uint16_t commodity_n;
   uint16_t expiration_date_n;
   int32_t strike_price_i;
   char exchange_info_s [32];
} tm_trade_rpt_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO75  */
/*   NAMED STRUCT = 34021  */

typedef struct trade_report_1_trans
{
   transaction_type_t transaction_type;
   series_t series;
   order_var_t order_var;
   party_t party;
   char exchange_info_s [32];
   give_up_member_t give_up_member;
   char settlement_date_s [8];
   char time_of_agreement_date_s [8];
   char time_of_agreement_time_s [6];
   uint8_t deferred_publication_c;
   char filler_1_s;
} trade_report_1_trans_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO459  */
/*   NAMED STRUCT = 34119  */

typedef struct trade_report_1_trans_p
{
   transaction_type_t transaction_type;
   series_t series;
   trading_code_t trading_code;
   order_var_t order_var;
   party_t party;
   char exchange_info_s [32];
   give_up_member_t give_up_member;
   char settlement_date_s [8];
   char time_of_agreement_date_s [8];
   char time_of_agreement_time_s [6];
   uint8_t deferred_publication_c;
   char filler_1_s;
} trade_report_1_trans_p_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507, MO76  */
/*   NAMED STRUCT = 34022  */

typedef struct trade_report_2_trans
{
   transaction_type_t transaction_type;
   series_t series;
   int64_t mp_quantity_i;
   int32_t premium_i;
   uint32_t block_n;
   char settlement_date_s [8];
   char time_of_agreement_date_s [8];
   char time_of_agreement_time_s [6];
   uint8_t ext_t_state_c;
   uint8_t deferred_publication_c;
   trd_rpt_cust_t bid_side;
   trd_rpt_cust_t ask_side;
} trade_report_2_trans_t;


/*   TRANSACTION TYPE = BD70, TA70  */
/*   NAMED STRUCT = 34403  */

typedef struct trade_report_trade_ticker
{
   uint8_t trade_report_type;
   char settlement_date_s [8];
   char time_of_agreement_date_s [8];
   char time_of_agreement_time_s [6];
   uint8_t outside_info_spread_c;
} trade_report_trade_ticker_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34018  */

typedef struct trade_report_trans
{
   transaction_type_t transaction_type;
   series_t series;
   int64_t mp_quantity_i;
   int32_t premium_i;
   uint32_t block_n;
   uint8_t ext_t_state_c;
   char filler_3_s [3];
   bid_side_t bid_side;
   ask_side_t ask_side;
} trade_report_trans_t;


/*   TRANSACTION TYPE = BD71, TA71  */
/*   NAMED STRUCT = 34406  */

typedef struct trade_ticker_amend
{
   uint64_t execution_event_nbr_u;
   uint32_t match_group_nbr_u;
   uint8_t trade_state_c;
   char filler_3_s [3];
} trade_ticker_amend_t;


/*   TRANSACTION TYPE = DC90  */

typedef struct trans_segment_hdr
{
   transaction_type_t transaction_type;
   uint16_t items_n;
   uint16_t size_n;
   uint16_t segment_number_n;
   char filler_2_s [2];
} trans_segment_hdr_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34838  */

typedef struct trigger_on_session_order
{
   int16_t session_type_id_n;
   char filler_2_s [2];
} trigger_on_session_order_t;


/*   TRANSACTION TYPE = BO25, BO5, MA123, MA507  */
/*   NAMED STRUCT = 34811  */

typedef struct trigger_order
{
   stop_series_t stop_series;
   int32_t limit_premium_i;
   uint8_t stop_condition_c;
   char filler_3_s [3];
} trigger_order_t;


/*   BROADCAST TYPE = BI73  */

typedef struct undo_info_ready
{
   broadcast_type_t broadcast_type;
   int32_t info_type_i;
   series_t series;
   char business_date_s [8];
   char clearing_date_s [8];
   char sent_date_s [8];
   char sent_time_s [6];
   uint16_t seq_num_srm_n;
} undo_info_ready_t;


/*   BROADCAST TYPE = BU54  */

typedef struct valid_sector_code_update_bu54
{
   broadcast_type_t broadcast_type;
   uint16_t chg_type_n;
   char filler_2_s [2];
   da54_t da54;
} valid_sector_code_update_bu54_t;


#include "oal_align_reset.h"

#endif
#ifndef __ALL_MESSAGES_H
#define __ALL_MESSAGES_H

#ifndef _BAS_MESSAGES_H_
#define _BAS_MESSAGES_H_

/******************************************************************************
Module: bas_messages.h

Message definitions generated 2022-12-01 07:07:27 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define GEN_SHUT_REQ_NEWP               100001
#define GEN_SECFAIL                    -100002
#define GEN_NOPRSENTRY                 -100003
#define GEN_INVCMD                     -100005
#define GEN_AMBIGCMD                   -100007
#define GEN_INVATTRVAL                 -100008
#define GEN_NOATTRVAL                  -100010
#define GEN_TRUNCATTRVAL               -100011
#define GEN_MOREATTRVAL                -100013
#define GEN_NOCOMMAND                  -100014
#define GEN_ERRPARSE                   -100016
#define GEN_MISSENVVAR                 -100018
#define GEN_VALNOTVALID                -100020
#define GEN_MIBFULL                    -100021
#define GEN_CONTINUE_EXECUTION          100023
#define GEN_NOVALUE                    -100025
#define GEN_INVARG                     -100027
#define GEN_CMDTIMEOUT                 -100029
#define GEN_TOOMANYARG                 -100031
#define GEN_NOLOGFILE                  -100033
#define GEN_MISSARG                    -100035
#define GEN_INVARGVAL                  -100037
#define GEN_NODEFFILE                  -100038
#define GEN_TOHIGHINST                 -100040
#define GEN_PRCNOTDEF                  -100042
#define GEN_NOEXCHNAME                 -100044
#define GEN_PRCALRINMIB                -100046
#define GEN_NOCMDTHREAD                -100048
#define GEN_DEFVALTOOLONG              -100050
#define GEN_TRUNC                      -100051
#define GEN_ENVVARNOTDEF                100053
#define GEN_BADPRIORITY                -100054
#define GEN_NOSUBSYSCFG                -100056
#define GEN_FOPENERR                   -100058
#define GEN_LOGFOPENERR                -100060
#define GEN_ENVTOONESTED               -100062
#define GEN_PRCNOTEXIST                -100064
#define GEN_PRCBUSYCMD                 -100066
#define GEN_PRCBADCMDMODE              -100068
#define GEN_NODYNMEM                   -100070
#define GEN_CMDLOST                    -100072
#define GEN_NULLINP                    -100073
#define GEN_DATTIMINV                  -100075
#define GEN_DAYMONINV                  -100077
#define GEN_IMPLNOTEST                 -100079
#define GEN_INTERNALERROR              -100081
#define GEN_INVSEGMENTNR               -100083
#define GEN_INSFMEM                    -100084
#define GEN_BUGCHECK                   -100086
#define GEN_INVTRANSSIZE               -100088
#define GEN_INVTRANSTYPE               -100090
#define GEN_NOTFOU                     -100092
#define GEN_FTP_ERROR                  -100094
#define GEN_FTP_OK                      100095
#define GEN_MF_FAILUNMAP               -100096
#define GEN_MF_FNOTUNMP                -100098
#define GEN_MF_MAPFAILED               -100100
#define GEN_MF_SYNCFAILED              -100102
#define GEN_MF_MTXFAILED               -100104
#define GEN_MF_NOSUCHFILE              -100106
#define GEN_MF_EACCESS                 -100108
#define GEN_FCLOSEERR                  -100110
#define GEN_OK                          100111
#define GEN_BUFOVF                     -100112
#define GEN_IO_FAILURE                 -100114
#define GEN_IO_NOSUCHFILE              -100116
#define GEN_IO_NOACCESS                -100118
#define GEN_IO_INVRESOURCE             -100120
#define GEN_IO_BUFTOSMALL              -100122
#define GEN_NOT_SUPPORTED              -100124
#define GEN_ABORT_MESSAGES             -100126
#define GEN_RT_ERRALLOMEM              -105000
#define GEN_RT_ERRFACNAM               -105002
#define GEN_RT_ERRDATPTYP              -105004
#define GEN_RT_KEYTYPNOTSUP            -105006
#define GEN_RT_PROTOCOLERR             -105008
#define GEN_RT_TRUNCATED               -105010
#define GEN_RT_FLAGOUTRANG             -105012
#define GEN_RT_INVEVTNAM               -105014
#define GEN_RT_NOTALLOW                -105016
#define GEN_RT_ABORT                   -105018
#define GEN_RT_TOOMANYDATP             -105020
#define GEN_RT_FLAGMISS                -105022
#define GEN_RT_INVDATPLEN              -105024
#define GEN_RT_NOTINIT                 -105026
#define GEN_RT_UNKPARNAME              -105028
#define GEN_RT_PARCNT                  -105030
#define GEN_RT_BADPARAM                -105032
#define GEN_RT_INTOVF                  -105034
#define GEN_RT_INVTXNUMBER             -105036
#define GEN_RT_INVTXTYPE               -105038
#define GEN_SEC_OK                      120001
#define GEN_SEC_NYI                    -120003
#define GEN_SEC_FAIL                   -120004
#define GEN_SEC_INVID                  -120006
#define GEN_SEC_INVIDENT               -120008
#define GEN_SEC_TABFULL                -120010
#define GEN_SEC_FILTOOSMALL            -120012
#define GEN_SEC_SECTOOSMALL            -120014
#define GEN_LCK_OK                      125001
#define GEN_LCK_NYI                    -125003
#define GEN_LCK_INVID                  -125004
#define GEN_LCK_TABFULL                -125006
#define GEN_TIM_CTRY_NOT_FOUND         -130000
#define GEN_TIM_NOT_INIT               -130002
#define GEN_TIM_NULL                    130003
#define GEN_TIM_DELTA_NOT_ALLOWED       130005
#define GEN_TIM_TZ_NOT_SET             -130006
#define GEN_PRICE_NORMAL                131001
#define GEN_PRICE_NYI                  -131003
#define GEN_PRICE_BADFRAC1             -131004
#define GEN_PRICE_BADFRAC2             -131006
#define GEN_PRICE_BADFRACVAL           -131008
#define GEN_PRICE_BADFRACDIV           -131010
#define GEN_PRICE_INVFRACBAS           -131012
#define GEN_PRICE_INVFMTNO             -131014
#define GEN_PRICE_FMTREQ               -131016
#define GEN_PRICE_INVDECPRICE          -131018
#define OMNI_SYNCHPOINT                -145000
#define OMNI_SECURITY                  -145002
#define OMNI_LOGIN_FAIL                -145004
#define OMNI_NOT_NOTIFIED              -145005
#define OMNI_DUPL_LOGIN                -145006
#define OMNI_UNKNOWNBYBE               -145008
#define OMNI_OAPIOLD                   -145010
#define OMNI_TIMED_OUT                 -145011
#define GEN_RTR_ERRALLOMEM             -165000
#define GEN_RTR_ERRFACNAM              -165002
#define GEN_RTR_ERRDATPTYP             -165004
#define GEN_RTR_KEYTYPNOTSUP           -165006
#define GEN_RTR_PROTOCOLERR            -165008
#define GEN_RTR_TRUNCATED              -165010
#define GEN_RTR_FLAGOUTRANG            -165012
#define GEN_RTR_INVEVTNAM              -165014
#define GEN_RTR_NOTALLOW               -165016
#define GEN_RTR_ABORT                  -165018
#define GEN_RTR_TOOMANYDATP            -165020
#define GEN_RTR_FLAGMISS               -165022
#define GEN_RTR_INVDATPLEN             -165024
#define GEN_RTR_NOTINIT                -165026
#define GEN_RTR_UNKPARNAME             -165028
#define GEN_RTR_PARCNT                 -165030
#define GEN_RTR_BADPARAM               -165032
#define GEN_RTR_INTOVF                 -165034
#define GEN_RTR_INVTXNUMBER            -165036
#define GEN_RTR_INVTXTYPE              -165038
#define GEN_RTR_SRVABOTX               -165040
#define GEN_RTR_NOSRVAVL                165041
#define GEN_RTR_INVFLAGS               -165042
#define GEN_RTR_INVCHANNEL             -165044
#define GEN_RTR_INVFACNAM              -165046
#define GEN_RTR_FAILED                 -165048
#define GEN_TPS_NOHOST                 -166000
#define GEN_TPS_ERRLOCLNAME            -166002
#define GEN_TPS_BADIPADDR              -166004
#define GEN_TPS_ERRHOSTNAME            -166006
#define GEN_TPS_ERROPENSOCK            -166008
#define GEN_TPS_ERRTHREAD              -166010
#define GEN_TPS_ERRTCPCONN             -166012
#define GEN_TPS_INVHANDLE              -166014
#define GEN_TPS_INVBUFFER              -166016
#define GEN_TPS_NOCALLBACK             -166018
#define GEN_TPS_INSMEM                 -166020
#define GEN_TPS_COMMERR                -166022
#define GEN_TPS_INSBUFF                -166024
#define GEN_TPS_INVSEM                 -166026
#define GEN_TPS_ERRBIND                -166028
#define GEN_TPS_ERRLSTN                -166030
#define GEN_TPS_INVOCVER               -166032
#define GEN_TPS_INVMODE                -166034
#define GEN_TPS_EOF                    -166036
#define BAS_HASH_FEWBUCKETS            -167000
#define BAS_HASH_BADTYPE               -167002
#define BAS_HASH_ITEMEXISTS            -167003
#define BAS_HASH_BADLISTCMD            -167004
#define BAS_HASH_NOCMPFUN              -167006
#define BAS_HASH_NOHASHFUN             -167008
#define BAS_HASH_MALFUNC               -167010
#define BAS_CMD_NOCOMMAND              -168000
#define BAS_CMD_INVALIDCMD             -168002
#define BAS_CMD_TOOLONGITEM            -168004
#define BAS_CMD_NOATTRVAL              -168006
#define BAS_CMD_TOOLONGAVAL            -168008
#define BAS_CMD_NOPRSENTRY             -168010
#define BAS_CMD_AMBIGCMD               -168012
#define BAS_CMD_TRUNCATTRVAL           -168014
#define BAS_CMD_CMDFILEERR             -168016
#define BAS_CMD_INVARG                 -168017
#define OAL_ILLEGAL_PARAMETER          -169000
#define OAL_OVERFLOW                   -169002
#define OAL_ERROR                      -169004
#define BAS_MDF_VERSION                 198001

/******************************************************************************
*end*of* bas_messages.h
*******************************************************************************/

#endif /* _BAS_MESSAGES_H_ */


#ifndef _CDB_MESSAGES_H_
#define _CDB_MESSAGES_H_

/******************************************************************************
Module: cdb_messages.h

Message definitions generated 2022-12-01 07:07:29 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define CDB_SUCCCOMP                    300001
#define CDB_NORMAL                      300003
#define CDB_NOMAREXIST                 -300005
#define CDB_NOCOUEXIST                 -300007
#define CDB_NOMSUEXIST                 -300009
#define CDB_INSFMEM                    -300010
#define CDB_SQLDECLERR                 -300012
#define CDB_SQLERROR                   -300014
#define CDB_UNTRTNO                    -300016
#define CDB_ILLEXPYEAR                 -300018
#define CDB_NOTBLMATCH                 -300020
#define CDB_USRNOTEXIST                -300022
#define CDB_WRONGACCTYPE               -300024
#define CDB_NOVALIDUST                 -300026
#define CDB_NOVALIDUSR                 -300028
#define CDB_USRNOTLEG                  -300030
#define CDB_SECMISMTC                  -300032
#define CDB_USER_TIMEOUT               -300034
#define CDB_USER_LOST                  -300036
#define CDB_OLDTIMESEC                 -300038
#define CDB_MNUNOTEXIST                -300040
#define CDB_ILLEXPMONTH                -300042
#define CDB_DBRETRY                    -300044
#define CDB_VECFULL                    -300046
#define CDB_BUGCHECK                   -300048
#define CDB_TOOMANYUSERS               -300050
#define CDB_TOOMANYSESSION             -300052
#define CDB_NUMSESSIONTOOLOW           -300054
#define CDB_PARSESSNOTALLOWED          -300056
#define CDB_NOICOEXIST                  300057
#define CDB_NOITUEXIST                  300059
#define CDB_NOLTFEXIST                  300061
#define CDB_NOPQREXIST                  300063
#define CDB_NOTRFEXIST                  300065
#define CDB_NOVMSEXIST                  300067
#define CDB_NOVOLEXIST                  300069
#define CDB_NOPSCEXIST                  300071
#define CDB_NOCOPEXIST                  300073
#define CDB_NOCBSEXIST                 -300075
#define CDB_NOCOMEXIST                 -300077
#define CDB_NOIDBEXIST                 -300079
#define CDB_NOIDLEXIST                 -300081
#define CDB_NOINSEXIST                 -300083
#define CDB_NOINTEXIST                 -300085
#define CDB_NOLITEXIST                 -300087
#define CDB_NOLTTEXIST                 -300089
#define CDB_NOLTUEXIST                 -300091
#define CDB_NOTRLEXIST                 -300093
#define CDB_NOTRSEXIST                 -300095
#define CDB_NOTSFEXIST                 -300097
#define CDB_NOTRTEXIST                 -300099
#define CDB_NOTZOEXIST                 -300101
#define CDB_NOUSREXIST                 -300103
#define CDB_NOUSTEXIST                 -300105
#define CDB_NOVBSEXIST                 -300107
#define CDB_NOVPTEXIST                 -300109
#define CDB_NOMNUACCESS                -300111
#define CDB_USRNOTLOG                  -300113
#define CDB_NOPREVINA                  -300115
#define CDB_NOCLHEXIST                 -300117
#define CDB_NOMEMEXIST                 -300119
#define CDB_NOINCEXIST                 -300121
#define CDB_NOCLIEXIST                 -300123
#define CDB_NOBBOEXIST                 -300125
#define CDB_NOCRSEXIST                 -300127
#define CDB_NOSPREXIST                 -300129
#define CDB_NOTRCEXIST                 -300131
#define CDB_NOTRREXIST                 -300133
#define CDB_NOOBLEXIST                 -300135
#define CDB_NOLFAEXIST                 -300137
#define CDB_NONODEXIST                 -300139
#define CDB_NOLNOEXIST                 -300141
#define CDB_NOISSEXIST                  300143
#define CDB_NOCSSEXIST                  300145
#define CDB_NOPCOEXIST                  300147
#define CDB_NOPICEXIST                  300149
#define CDB_NOPISEXIST                  300151
#define CDB_NOPRDEXIST                  300153
#define CDB_NOPSREXIST                  300155
#define CDB_NOPUREXIST                  300157
#define CDB_NOUCLEXIST                  300159
#define CDB_NOWINEXIST                  300161
#define CDB_NOVPDEXIST                  300163
#define CDB_NOISDEXIST                  300165
#define CDB_NOISCEXIST                  300167
#define CDB_NOCOGEXIST                  300169
#define CDB_NOFMLEXIST                  300171
#define CDB_NOSYIEXIST                  300173
#define CDB_NOOBKEXIST                  300175
#define CDB_NOTSREXIST                  300177
#define CDB_NOEDGEXIST                  300179
#define CDB_NOSPGEXIST                  300181
#define CDB_NOPDPEXIST                  300183
#define CDB_NOFOVEXIST                  300185
#define CDB_NOCLPEXIST                  300187
#define CDB_NOCLHUSR                   -300189
#define CDB_NOAFTEXIST                 -300191
#define CDB_NOATYEXIST                 -300193
#define CDB_NOADCEXIST                 -300195
#define CDB_NOCSBEXIST                 -300197
#define CDB_NOEXCEXIST                 -300199
#define CDB_NOFASEXIST                 -300201
#define CDB_NOFTBEXIST                 -300203
#define CDB_NONTDEXIST                 -300205
#define CDB_NOPEVEXIST                 -300207
#define CDB_NOPFTEXIST                 -300209
#define CDB_NOTRUEXIST                 -300211
#define CDB_INVCOMMAND                 -300213
#define CDB_NOSYMEXIST                 -300215
#define CDB_NOACCEXIST                 -300217
#define CDB_NOSBSEXIST                 -300219
#define CDB_WRONGWEEK                  -300220
#define CDB_SLEC_INSERR                -300222
#define CDB_SLEC_EXCMISSING            -300224
#define CDB_SLEC_DIFFCODE              -300226
#define CDB_SLEC_DIFFISIN              -300228
#define CDB_NODLVEXIST                 -300229
#define CDB_NOCOMSPEC                  -300230
#define CDB_ERRLONGCOM                 -300232
#define CDB_NOMARSPEC                  -300234
#define CDB_ERRLONGMAR                 -300236
#define CDB_NOINGSPEC                  -300238
#define CDB_ERRLONGING                 -300240
#define CDB_ERRLONGINS                 -300242
#define CDB_NOSTRKSPEC                 -300244
#define CDB_ERRLONGSTRK                -300246
#define CDB_NOSTRKDECSPEC              -300248
#define CDB_ERRLONGSTRKDEC             -300250
#define CDB_NOCONTRSPEC                -300252
#define CDB_ERRLONGCONTR               -300254
#define CDB_NOCONTRDECSPEC             -300256
#define CDB_ERRLONGCONTRDEC            -300258
#define CDB_NOEXPSPEC                  -300260
#define CDB_ERRLONGEXP                 -300262
#define CDB_ERRLONGLTD                 -300264
#define CDB_ERRLONGLTT                 -300266
#define CDB_ERRLONGISIN                -300268
#define CDB_ERRLONGIMPLD               -300270
#define CDB_ERRLONGIMPLT               -300272
#define CDB_NOOPERSPEC                 -300274
#define CDB_ERRLONGOPER                -300276
#define CDB_ERRLONGDLVSTART            -300278
#define CDB_ERRLONGDLVSTOP             -300280
#define CDB_ERRLONGSTART               -300282
#define CDB_ERRLONGEND                 -300284
#define CDB_ERRLONGSYMB                -300286
#define CDB_ERRLONGMTH                 -300288
#define CDB_ERRLONGSTR                 -300290
#define CDB_ERRLONGEXC                 -300292
#define CDB_ERRLONGSTATUS              -300294
#define CDB_ERRLONGLISTING             -300296
#define CDB_ERRLONGTRINCL              -300298
#define CDB_ERRLONGISTRADED            -300300
#define CDB_ERRTRINCL                  -300302
#define CDB_ERRISTRADED                -300304
#define CDB_WRONGINPUT                  300305
#define CDB_NOSERIES                    300307
#define CDB_NOINSTRUMENT                300309
#define CDB_NOUNDERLYING                300311
#define CDB_NOCOMBO                     300313
#define CDB_NOUSER                      300315
#define CDB_MARCLOSED                   300317
#define CDB_COUCLOSED                   300319
#define CDB_NOTRM                       300321
#define CDB_NOMARKET                    300323
#define CDB_NOINSTGROUP                 300325
#define CDB_UNEXPECTED                 -300326
#define CDB_WRONGSEGM                  -300328
#define CDB_NOINSTCLASS                 300329
#define CDB_NOBASEDFOUND               -300330
#define CDB_NOACCOUNT                   300331
#define CDB_NOSYMBOL                    300333
#define CDB_WRONGEXP                   -300334
#define CDB_ILLEGTRD                   -300336
#define CDB_WRONGCOM                   -300338
#define CDB_DUMMY                      -300340
#define CDB_RTRERROR                   -300342
#define CDB_ABORT                      -300344
#define CDB_ILLCHANNEL                 -300346
#define CDB_NOSRVAVAIL                 -300348
#define CDB_INSERTFAILED               -300350
#define CDB_REMOVEFAILED               -300352
#define CDB_NOTFOUNDDB                 -300354
#define CDB_NOTFOUNDMEM                -300356
#define CDB_UCLEXCEED                  -300358
#define CDB_NOTAREPO                   -300360
#define CDB_NOTMPLFOUND                -300362
#define CDB_STARTWRONG                 -300364
#define CDB_STARTNOTSETTL              -300366
#define CDB_ENDWRONG                   -300368
#define CDB_ENDNOTSETTL                -300370
#define CDB_WRONGMONEYPAR              -300372
#define CDB_WRONGNOOFSUB               -300374
#define CDB_WRONGBUYSELLBACK           -300376
#define CDB_NOINCFOUND                 -300378
#define CDB_REPOALREXIST                300379
#define CDB_REPOTOOLONG                -300380
#define CDB_LOADFAILED                 -300382
#define CDB_STARTNOTVALID              -300384
#define CDB_ENDNOTVALID                -300386
#define CDB_OUTRSPECMISSING            -300388
#define CDB_STARTBEFISSUE              -300390
#define CDB_ENDAFTERMATUR              -300392
#define CDB_SERIESALREXIST              300393
#define CDB_TMNOTALLOWED               -300394
#define CDB_INSEMPTY                   -300396
#define CDB_NOCTDPRICE                 -300398
#define CDB_INSNOTFOUND                -300400
#define CDB_ENDAFTEREQMATUR            -300402
#define CDB_NOTENOUGHLEGS              -300404
#define CDB_COMBONOTTM                 -300406
#define CDB_NOCONVFACTOR               -300408
#define CDB_ADDISIN                     300409
#define CDB_STARTTOEARLY               -300410
#define CDB_ENDTOEARLY                 -300412
#define CDB_PASSWLENTOOSHORT           -300414
#define CDB_PASSWINHISTORY             -300416
#define CDB_BOXALREXIST                 300417
#define CDB_NOTEMPLATEBOX              -300418
#define CDB_TMBOXNOTALLOWED            -300420
#define CDB_TOO_MANY_TM_COMBOS         -300421
#define CDB_COMBODIFFPART              -300422
#define CDB_CURRISNOTEQ                -300424
#define CDB_NODSCEXIST                 -300425
#define CDB_TRANSNOTINLTT              -300426
#define CDB_TRANSNOTOPEN               -300428
#define CDB_PRCQUOTISNOTEQ             -300430
#define CDB_HPFILENOTFOUND             -300432
#define CDB_HPOUTPUTFILEWRONG          -300434
#define CDB_DICFILENOTFOUND             300435
#define CDB_SEGMENTFULL                 300437
#define CDB_MAXTWOLEGEXCEEDED          -300438
#define CDB_MAXTHREELEGEXCEEDED        -300440
#define CDB_LASTTRADINGISPASSED        -300442
#define CDB_CSTNOTAUTH                 -300444
#define CDB_FIRSTTRADINGINFUTURE       -300446
#define CDB_OPERNOTVALID               -300448
#define CDB_STATUSNOTVALID             -300450
#define CDB_WRONGUST                   -300452
#define CDB_WRONGMAC                   -300454
#define CDB_WRONGAPPL                  -300456
#define CDB_WRONGRWACCESS              -300458
#define CDB_WRONGMENUACCESS            -300460
#define CDB_NOTAUTHMAC                 -300462
#define CDB_NOTAUTHUSR                 -300464
#define CDB_USERALREXIST               -300466
#define CDB_MACALREXIST                -300468
#define CDB_ERRLONGFTD                 -300470
#define CDB_ERRLONGFTT                 -300472
#define CDB_CUSTNOTAUTH                -300474
#define CDB_NOTSETTL                   -300476
#define CDB_TMINSTNOTALLOWED           -300478
#define CDB_EXPNOTSETTL                -300480
#define CDB_EXPDATEWRONG               -300482
#define CDB_WRONGNOOFSTRIP             -300484
#define CDB_MAXPTLGUSERSEXCEEDED       -300486
#define PD_ACTIVE                       305001
#define PD_EXPISNTD                    -305002
#define PD_LTRISNTD                     305003
#define PD_ALREADYRESUMED               305005
#define PD_ALREADYSUSP                  305007
#define PD_ALREADYEXIST                 305009
#define PD_NOTEXIST                     305011
#define PD_SGNTXSTART                   305013
#define PD_BUFOVFLOW                   -305015
#define PD_UNEXPECTED                  -305016
#define PD_HIGHISINNOTCORR             -305018
#define PD_LOWISINNOTCORR              -305020
#define PD_LOWERISHIGHER               -305022
#define PD_ISINOLAP                    -305024
#define PD_ISINMISS                    -305026
#define PD_SEGMZERO                    -305028
#define PD_INVONOFF                    -305030
#define PD_SERCODEEMPTY                -305032
#define PD_INGCODEZERO                 -305034
#define PD_MARCODEZERO                 -305036
#define PD_OPERNOTVALID                -305038
#define PD_DELNOTVALID                 -305040
#define PD_ISINNOTUNIQUE               -305042
#define PD_WRONGIMPLFORMAT             -305044
#define PD_WRONGEXPFORMAT              -305046
#define PD_WRONGLASTFORMAT             -305048
#define PD_ORIGSERNOTFOUND             -305050
#define PD_UPPSERNOTFOUND              -305052
#define PD_EXTSERIESTRT                -305054
#define PD_NOTACTIVE                   -305056
#define PD_NOTFOUND                    -305058
#define PD_FUTDELETE                   -305060
#define PD_NOTAUTHORIZED               -305062
#define PD_ILLKEYVALUE                 -305064
#define PD_SPACESINKEY                 -305066
#define PD_ILLSEARCHVALUE              -305068
#define PD_FIELDISEMPTY                -305070
#define PD_COMCODEALREXIST             -305072
#define PD_COMIDALREXIST               -305074
#define PD_INGCODEALREXIST             -305076
#define PD_INGIDALREXIST               -305078
#define PD_INTCODEALREXIST             -305080
#define PD_INTIDALREXIST               -305082
#define PD_INCCODEALREXIST             -305084
#define PD_INCIDALREXIST               -305086
#define PD_INSCODEALREXIST             -305088
#define PD_INSIDALREXIST               -305090
#define PD_INSMUSTBESAME               -305092
#define PD_MARCODEALREXIST             -305094
#define PD_MARIDALREXIST               -305096
#define PD_EXCCODEALREXIST             -305098
#define PD_EXCIDALREXIST               -305100
#define PD_CBGCODEALREXIST             -305102
#define PD_CBGIDALREXIST               -305104
#define PD_CBTCODEALREXIST             -305106
#define PD_CBTIDALREXIST               -305108
#define PD_CBCCODEALREXIST             -305110
#define PD_CBCIDALREXIST               -305112
#define PD_CBSCODEALREXIST             -305114
#define PD_CBSIDALREXIST               -305116
#define PD_WRONGPERIOD                 -305118
#define PD_EXPNOTLEG                   -305120
#define PD_DELNOTFOUND                 -305122
#define PD_TIMEISNULL                  -305124
#define PD_NOTEXACT                    -305126
#define PD_TIMEISPASSED                -305128
#define PD_INCNOTTRADED                -305130
#define PD_WRONGISIN                   -305132
#define PD_COMILLSTATUS                -305134
#define PD_ILLPASSW                    -305136
#define PD_INSILLSTATUS                -305138
#define PD_EXPISPASSED                 -305140
#define PD_STRIKEISZERO                -305142
#define PD_STRIKENOTZERO               -305144
#define PD_IDNOTCREATED                -305146
#define PD_OPRANOTUNIQUE               -305148
#define PD_OPRASTRKNOTUNIQUE           -305150
#define PD_REMOVENOTALLWD              -305152
#define PD_FIELDEXCEEDED               -305154
#define PD_DOUBLEMENU                  -305156
#define PD_DOUBLEMENUTXT               -305158
#define PD_MNUNOTCHGABLE               -305160
#define PD_USRNOTFOUND                 -305162
#define PD_ACTNOTFOUND                 -305164
#define PD_MTXNOTFOUND                 -305166
#define PD_MACNOTFOUND                 -305168
#define PD_MNUNOTFOUND                 -305170
#define PD_BONDPARAM                   -305172
#define PD_POWPARAM                    -305174
#define PD_POWSTARTGTSTOP              -305176
#define PD_DOUBLECUPDATE               -305178
#define PD_BOTHWINSUM                  -305180
#define PD_SUMAFTERWIN                 -305182
#define PD_STOPAFTERREST               -305184
#define PD_DOUBLECOM                   -305186
#define PD_DOUBLESUMDATE               -305188
#define PD_DOUBLEWINDATE               -305190
#define PD_DOUBLESTOPDATE              -305192
#define PD_DOUBLERESTDATE              -305194
#define PD_DOUBLEINS                   -305196
#define PD_DOUBLESIZE                  -305198
#define PD_DOUBLEMODIFIER              -305200
#define PD_DOUBLESPREAD                -305202
#define PD_DOUBLELOWER                 -305204
#define PD_DOUBLEUPPER                 -305206
#define PD_LOWERGEUPPER                -305208
#define PD_LOWEROVRLAP                 -305210
#define PD_UPPEROVRLAP                 -305212
#define PD_DOUBLEMONTH                 -305214
#define PD_DOUBLEPERIOD                -305216
#define PD_DOUBLEORDERNO               -305218
#define PD_ILLEGMONTH                  -305220
#define PD_ILLEGPERIOD                 -305222
#define PD_DOUBLEEXP                   -305224
#define PD_WRONGEXP                    -305226
#define PD_WRONGNOT                    -305228
#define PD_WRONGLIFETIME               -305230
#define PD_DOUBLELEG                   -305232
#define PD_SERIEISPASSED               -305234
#define PD_LTDISPASSED                 -305236
#define PD_TOOFEWLEGS                  -305238
#define PD_CBCNOTTRADED                -305240
#define PD_DOUBLEINC                   -305242
#define PD_DOUBLEWIN                   -305244
#define PD_DOUBLEPEV                   -305246
#define PD_SERIESNOTEXIST              -305248
#define PD_DISCEXCEED                  -305250
#define PD_DOUBLEFVL                   -305252
#define PD_FVLNOTEXIST                 -305254
#define PD_DOUBLEFTB                   -305256
#define PD_FEEVALEXCEED                -305258
#define PD_LIMITSEARCH                 -305260
#define PD_ILLEGMONTHNAME              -305262
#define PD_ILLEGPERIODNAME             -305264
#define PD_DOUBLELGP                   -305266
#define PD_DOUBLETRU                   -305268
#define PD_WROPEN                      -305270
#define PD_WRNET                       -305272
#define PD_DOUBLECHA                   -305274
#define PD_DOUBLECLI                   -305276
#define PD_EXCWCNOTALLWD               -305278
#define PD_DOUBLESPR                   -305280
#define PD_SPRCHKLEG                   -305282
#define PD_TRRCODEEMPTY                -305284
#define PD_CONDEMPTY                   -305286
#define PD_SPRCHKEMPTY                 -305288
#define PD_PUBINFEMPTY                 -305290
#define PD_DOUBLETRC                   -305292
#define PD_DOUBLEPRDDATE               -305294
#define PD_ACCNUMALREXIST              -305296
#define PD_DOUBLETRT                   -305298
#define PD_WRACTTIME                   -305300
#define PD_WRCONTROP                   -305302
#define PD_WRCONTRFACSUB               -305304
#define PD_WRCONTRFAC                  -305306
#define PD_DOUBLEISD                   -305308
#define PD_ISDNOTEQ                    -305310
#define PD_WRSERIES                    -305312
#define PD_DOUBLEINSID                 -305314
#define PD_DOUBLESOINSID               -305316
#define PD_OLDINSNOTEXIST              -305318
#define PD_CODEISEQ                    -305320
#define PD_SOCODEISEQ                  -305322
#define PD_INSINPREV                   -305324
#define PD_PASSEDDATE                  -305326
#define PD_ALREXIST                    -305328
#define PD_NOCHGALW                    -305330
#define PD_EXCEEDCONTR                 -305332
#define PD_NAMEISEQ                    -305334
#define PD_SOOLDNAMEISEQ               -305336
#define PD_SONEWNAMEISEQ               -305338
#define PD_WRINFSRC                    -305340
#define PD_INFSRCNACT                  -305342
#define PD_WRINFTYPE                   -305344
#define PD_WRONGATTR                   -305346
#define PD_WRONGGENATTR                -305348
#define PD_WRONGDERATTR                -305350
#define PD_BOTHATTR                    -305352
#define PD_BOTHCOMEXP                  -305354
#define PD_ISSALREXIST                 -305356
#define PD_ISSNOSERIES                 -305358
#define PD_DOUBLEMNA                   -305360
#define PD_DOUBLEINT                   -305362
#define PD_DOUBLECBT                   -305364
#define PD_FROMTOP                     -305366
#define PD_FROMTOR                     -305368
#define PD_FROMTOI                     -305370
#define PD_FRPBLANK                    -305372
#define PD_FRPNOTBLANK                 -305374
#define PD_FRRBLANK                    -305376
#define PD_FRRNOTBLANK                 -305378
#define PD_WHPBLANK                    -305380
#define PD_WHPNOTBLANK                 -305382
#define PD_WHRBLANK                    -305384
#define PD_WHRNOTBLANK                 -305386
#define PD_OTHQBLANK                   -305388
#define PD_OTHBNOTBLANK                -305390
#define PD_OTHBBLANK                   -305392
#define PD_OTHQNOTBLANK                -305394
#define PD_DOUBLPFT                    -305396
#define PD_BUYSELL                     -305398
#define PD_RESP                        -305400
#define PD_ISNNODIG                    -305402
#define PD_MAXISIN                     -305404
#define PD_WRONGCBA                    -305406
#define PD_NODLVFOUND                  -305408
#define PD_NODLVEXPFOUND               -305410
#define PD_IFILL                       -305412
#define PD_INSDUP                      -305414
#define PD_NOSTORE                     -305416
#define PD_SGNTOMGRP                   -305418
#define PD_SGNTOMSER                   -305420
#define PD_SGNTOMNEWSER                -305422
#define PD_SGNTOMSTRK                  -305424
#define PD_SGNUNDZERO                  -305426
#define PD_SGNONGTX                    -305428
#define PD_EXPINVDAY                   -305430
#define PD_EXPINVWEEK                  -305432
#define PD_EXPINVMTH                   -305434
#define PD_ISNCODEALREXIST             -305436
#define PD_ISNIDALREXIST               -305438
#define PD_TRRCODEALREXIST             -305440
#define PD_TRRIDALREXIST               -305442
#define PD_COMCODEZERO                 -305444
#define PD_DEFMUSTBEOPEN               -305446
#define PD_MUSTBEOPEN                  -305448
#define PD_NOFLTITEM                   -305450
#define PD_DOUBLEQUA                   -305452
#define PD_DOUBLEBAS                   -305454
#define PD_DOUBLEOTHBAS                -305456
#define PD_DOUBLEOTHQUA                -305458
#define PD_NOPVTITEM                   -305460
#define PD_NOLITITEM                   -305462
#define PD_CBTANDINT                   -305464
#define PD_NOMACITEM                   -305466
#define PD_NOMNUITEM                   -305468
#define PD_DOUBLESBS                   -305470
#define PD_ERRFROMATY                  -305472
#define PD_DOUBLEPERCODE               -305474
#define PD_DOUBLEPERNAME               -305476
#define PD_ILLEGDATE                   -305478
#define PD_LTTSTATUSUNL                -305480
#define PD_LTTSTARTTIMEEMPTY           -305482
#define PD_LTTSTARTTIMENOTEMPTY        -305484
#define PD_LTTSTOPTIMEEMPTY            -305486
#define PD_LTTSTOPTIMENOTEMPTY         -305488
#define PD_NOISSSERIES                 -305490
#define PD_INCANDINS                   -305492
#define PD_NOEXPINS                    -305494
#define PD_INVPQREXP                   -305496
#define PD_VOLLSTEMPTY                 -305498
#define PD_HLSTEMPTY                   -305500
#define PD_HDLMEMPTY                   -305502
#define PD_TIMBEFALEMPTY               -305504
#define PD_TIMBETWALEMPTY              -305506
#define PD_CSTNOTALLWD                 -305508
#define PD_VOLANDLST                   -305510
#define PD_WRONGCONTR                  -305512
#define PD_NOPFTITEM                   -305514
#define PD_ALLFEEVALEMPTY              -305516
#define PD_PQRDELNOTVALID              -305518
#define PD_UNDALREADYRESUMED           -305520
#define PD_UNDALREADYSUSP              -305522
#define PD_CSTALREADYRESUMED           -305524
#define PD_CSTALREADYSUSP              -305526
#define PD_WACCNOTALLOWED              -305528
#define PD_INSDEL                      -305530
#define PD_EXPWEEKEMPTY                -305532
#define PD_EXPMTHEMPTY                 -305534
#define PD_CBRSTPEXCEED                -305536
#define PD_CBRINCSTEPS                 -305538
#define PD_NOCLIITEM                   -305540
#define PD_INSISDELETED                -305542
#define PD_OBLDOUBLE                   -305544
#define PD_OBLRWDOUBLE                 -305546
#define PD_ISINCODEEMPTY               -305548
#define PD_DOUBLENOD                   -305550
#define PD_DOUBLEFAC                   -305552
#define PD_USERLOCK                    -305554
#define PD_SLECCOMINCOMPL              -305556
#define PD_ISINTSYSEMPTY               -305558
#define PD_MODIFIEREMPTY               -305560
#define PD_ISSIDENTEMPTY               -305562
#define PD_COGCODEALREXIST             -305564
#define PD_COGIDALREXIST               -305566
#define PD_OBKCODEALREXIST             -305568
#define PD_OBKIDALREXIST               -305570
#define PD_DOUBLEFML                   -305572
#define PD_POSORTRDEXIST               -305574
#define PD_NOCLAVAIL                   -305576
#define PD_PMMANDCGM                   -305578
#define PD_DOUBLECGM                   -305580
#define PD_GUARDPRICEEMPTY             -305582
#define PD_VALINTTYPEEMPTY             -305584
#define PD_VALINTBASEEMPTY             -305586
#define PD_GROSSMARGPROPEMPTY          -305588
#define PD_DELIVPROPEMPTY              -305590
#define PD_DOUBLEREG                   -305592
#define PD_DOUBLEREGDESC               -305594
#define PD_NOACC                       -305596
#define PD_NOACCACT                    -305598
#define PD_NOACRACC                    -305600
#define PD_NOACRACCACT                 -305602
#define PD_NOING                       -305604
#define PD_NOINGACT                    -305606
#define PD_NOCLH                       -305608
#define PD_NOCLHACT                    -305610
#define PD_NOORG                       -305612
#define PD_NOORGACT                    -305614
#define PD_NOCCCST                     -305616
#define PD_NOCCCSTACT                  -305618
#define PD_NOMAR                       -305620
#define PD_NOMARACT                    -305622
#define PD_NOMSU                       -305624
#define PD_NOMSUACT                    -305626
#define PD_NOINS                       -305628
#define PD_NOINSACT                    -305630
#define PD_NOUPPINS                    -305632
#define PD_NOUPPINSACT                 -305634
#define PD_NOFIXINS                    -305636
#define PD_NOFIXINSACT                 -305638
#define PD_NOINT                       -305640
#define PD_NOINTACT                    -305642
#define PD_NOUPPINT                    -305644
#define PD_NOUPPINTACT                 -305646
#define PD_NOCBC                       -305648
#define PD_NOCBCACT                    -305650
#define PD_NOCBG                       -305652
#define PD_NOCBGACT                    -305654
#define PD_NOCBR                       -305656
#define PD_NOCBRACT                    -305658
#define PD_NOCBT                       -305660
#define PD_NOCBTACT                    -305662
#define PD_NOMNU                       -305664
#define PD_NOMNUACT                    -305666
#define PD_NOATYPE                     -305668
#define PD_NOATYPEACT                  -305670
#define PD_NOCIS                       -305672
#define PD_NOCISACT                    -305674
#define PD_NOCOM                       -305676
#define PD_NOCOMACT                    -305678
#define PD_NONEWCOM                    -305680
#define PD_NONEWCOMACT                 -305682
#define PD_NOBRD                       -305684
#define PD_NOBRDACT                    -305686
#define PD_NOPRICBA                    -305688
#define PD_NOPRICBAACT                 -305690
#define PD_NOSECCBA                    -305692
#define PD_NOSECCBAACT                 -305694
#define PD_NODLSCBA                    -305696
#define PD_NODLSCBAACT                 -305698
#define PD_NOALTCOM                    -305700
#define PD_NOALTCOMACT                 -305702
#define PD_NOINC                       -305704
#define PD_NOINCACT                    -305706
#define PD_NOUPPINC                    -305708
#define PD_NOUPPINCACT                 -305710
#define PD_NOACCTYP                    -305712
#define PD_NOACCTYPACT                 -305714
#define PD_NOEXC                       -305716
#define PD_NOEXCACT                    -305718
#define PD_NOCUR                       -305720
#define PD_NOCURACT                    -305722
#define PD_NOCST                       -305724
#define PD_NOCSTACT                    -305726
#define PD_NOPAYCST                    -305728
#define PD_NOPAYCSTACT                 -305730
#define PD_NOCC1CST                    -305732
#define PD_NOCC1CSTACT                 -305734
#define PD_NOCC2CST                    -305736
#define PD_NOCC2CSTACT                 -305738
#define PD_NOUST                       -305740
#define PD_NOUSTACT                    -305742
#define PD_NOUSR                       -305744
#define PD_NOUSRACT                    -305746
#define PD_NOLAN                       -305748
#define PD_NOLANACT                    -305750
#define PD_NOCOU                       -305752
#define PD_NOCOUACT                    -305754
#define PD_NOBSS                       -305756
#define PD_NOBSSACT                    -305758
#define PD_NOEXP                       -305760
#define PD_NOEXPACT                    -305762
#define PD_NOLST                       -305764
#define PD_NOLSTACT                    -305766
#define PD_NOSPS                       -305768
#define PD_NOSPSACT                    -305770
#define PD_NOVPT                       -305772
#define PD_NOVPTACT                    -305774
#define PD_NOVPD                       -305776
#define PD_NOVPDACT                    -305778
#define PD_NOOBL                       -305780
#define PD_NOOBLACT                    -305782
#define PD_NONTD                       -305784
#define PD_NONTDACT                    -305786
#define PD_NOCCL                       -305788
#define PD_NOCCLACT                    -305790
#define PD_NOCCM                       -305792
#define PD_NOCCMACT                    -305794
#define PD_NOSRU                       -305796
#define PD_NOSRUACT                    -305798
#define PD_NOTRT                       -305800
#define PD_NOTRTACT                    -305802
#define PD_NOLIT                       -305804
#define PD_NOLITACT                    -305806
#define PD_NOSNS                       -305808
#define PD_NOSNSACT                    -305810
#define PD_NOTZO                       -305812
#define PD_NOTZOACT                    -305814
#define PD_NOVMS                       -305816
#define PD_NOVMSACT                    -305818
#define PD_NOVOL                       -305820
#define PD_NOVOLACT                    -305822
#define PD_NOINF                       -305824
#define PD_NOINFACT                    -305826
#define PD_NOADC                       -305828
#define PD_NOADCACT                    -305830
#define PD_NOFTB                       -305832
#define PD_NOFTBACT                    -305834
#define PD_NOFRO                       -305836
#define PD_NOFROACT                    -305838
#define PD_NOQUA                       -305840
#define PD_NOQUAACT                    -305842
#define PD_NOOTHQUA                    -305844
#define PD_NOOTHQUAACT                 -305846
#define PD_NOBAS                       -305848
#define PD_NOBASACT                    -305850
#define PD_NOOTHBAS                    -305852
#define PD_NOOTHBASACT                 -305854
#define PD_NOFLT                       -305856
#define PD_NOFLTACT                    -305858
#define PD_NOAFT                       -305860
#define PD_NOAFTACT                    -305862
#define PD_NOPFT                       -305864
#define PD_NOPFTACT                    -305866
#define PD_NOPEV                       -305868
#define PD_NOPEVACT                    -305870
#define PD_NOPOD                       -305872
#define PD_NOPODACT                    -305874
#define PD_NOPRY                       -305876
#define PD_NOPRYACT                    -305878
#define PD_NOEXT                       -305880
#define PD_NOEXTACT                    -305882
#define PD_NOTAX                       -305884
#define PD_NOTAXACT                    -305886
#define PD_NOBNK                       -305888
#define PD_NOBNKACT                    -305890
#define PD_NOCSS                       -305892
#define PD_NOCSSACT                    -305894
#define PD_NOCSB                       -305896
#define PD_NOCSBACT                    -305898
#define PD_NOTRA                       -305900
#define PD_NOTRAACT                    -305902
#define PD_NOPRD                       -305904
#define PD_NOPRDACT                    -305906
#define PD_NOFRT                       -305908
#define PD_NOFRTACT                    -305910
#define PD_NOWHN                       -305912
#define PD_NOWHNACT                    -305914
#define PD_NOSYM                       -305916
#define PD_NOSYMACT                    -305918
#define PD_NOSBS                       -305920
#define PD_NOSBSACT                    -305922
#define PD_NODLVINT                    -305924
#define PD_NODLVINTACT                 -305926
#define PD_NOWIN                       -305928
#define PD_NOWINACT                    -305930
#define PD_NOPUR                       -305932
#define PD_NOPURACT                    -305934
#define PD_NOBBO                       -305936
#define PD_NOBBOACT                    -305938
#define PD_NOCRS                       -305940
#define PD_NOCRSACT                    -305942
#define PD_NOTRC                       -305944
#define PD_NOTRCACT                    -305946
#define PD_NOTRR                       -305948
#define PD_NOTRRACT                    -305950
#define PD_NOSPR                       -305952
#define PD_NOSPRACT                    -305954
#define PD_NOPSR                       -305956
#define PD_NOPSRACT                    -305958
#define PD_NOBASINS                    -305960
#define PD_NOBASINSACT                 -305962
#define PD_NOOTHINS                    -305964
#define PD_NOOTHINSACT                 -305966
#define PD_NOFATY                      -305968
#define PD_NOFATYACT                   -305970
#define PD_NOWCC                       -305972
#define PD_NOWCCACT                    -305974
#define PD_NOLVMS                      -305976
#define PD_NOLVMSACT                   -305978
#define PD_NOHVMS                      -305980
#define PD_NOHVMSACT                   -305982
#define PD_NORNT                       -305984
#define PD_NOCHGORD                    -305986
#define PD_NOFRMMNU                    -305988
#define PD_NOFRMMNUACT                 -305990
#define PD_NOLFA                       -305992
#define PD_NOLFAACT                    -305994
#define PD_NOLNO                       -305996
#define PD_NOLNOACT                    -305998
#define PD_NONOD                       -306000
#define PD_NONODACT                    -306002
#define PD_NOSRI                       -306004
#define PD_NOSRIACT                    -306006
#define PD_NOTDS                       -306008
#define PD_NOTDSACT                    -306010
#define PD_NOTDP                       -306012
#define PD_NOTDPACT                    -306014
#define PD_INVULTIMO                   -306016
#define PD_INVALWCREATE                -306018
#define PD_INVCALCLTDNOT               -306020
#define PD_INVCALCSTARTNOT             -306022
#define PD_INVCALCSTOPSTART            -306024
#define PD_INVCALCEXPSTOP              -306026
#define PD_EXPMOVBACK                  -306028
#define PD_EXPMOVFORW                  -306030
#define PD_LTDMOVBACK                  -306032
#define PD_LTDMOVFORW                  -306034
#define PD_LTDNOTMOVBACK               -306036
#define PD_SETTLMOVBACK                -306038
#define PD_SETTLMOVFORW                -306040
#define PD_NOTMOVBACK                  -306042
#define PD_NOTMOVFORW                  -306044
#define PD_DLVSTARTMOVBACK             -306046
#define PD_DLVSTARTMOVFORW             -306048
#define PD_DLVSTOPMOVBACK              -306050
#define PD_DLVSTOPMOVFORW              -306052
#define PD_STARTMOVBACK                -306054
#define PD_STARTMOVFORW                -306056
#define PD_STARTNOTMOVBACK             -306058
#define PD_STOPMOVBACK                 -306060
#define PD_STOPMOVFORW                 -306062
#define PD_NOOPN                       -306064
#define PD_NOOPNACT                    -306066
#define PD_NOCOP                       -306068
#define PD_NOCOPACT                    -306070
#define PD_OPENONMAR                   -306072
#define PD_COPNOTEMPTY                 -306074
#define PD_COPPRICEEMPTY               -306076
#define PD_NOPMMCST                    -306078
#define PD_NOPMMCSTACT                 -306080
#define PD_NOPRMEXC                    -306082
#define PD_NOPRMEXCACT                 -306084
#define PD_NOCOG                       -306086
#define PD_NOCOGACT                    -306088
#define PD_NOFML                       -306090
#define PD_NOFMLACT                    -306092
#define PD_NOFOV                       -306094
#define PD_NOFOVACT                    -306096
#define PD_NOOBK                       -306098
#define PD_NOOBKACT                    -306100
#define PD_NOPDP                       -306102
#define PD_NOPDPACT                    -306104
#define PD_NOSYI                       -306106
#define PD_NOSYIACT                    -306108
#define PD_NOTSR                       -306110
#define PD_NOTSRACT                    -306112
#define PD_NOEDG                       -306114
#define PD_NOEDGACT                    -306116
#define PD_NOSPG                       -306118
#define PD_NOSPGACT                    -306120
#define PD_NOLRE                       -306122
#define PD_NOLREACT                    -306124
#define PD_NOLCO                       -306126
#define PD_NOLCOACT                    -306128
#define PD_ACCASPAYEMPTY               -306130
#define PD_ACCBDXDEALEMPTY             -306132
#define PD_ACCCHKLEGEMPTY              -306134
#define PD_ACCESSTYPEEMPTY             -306136
#define PD_ACCIDEMPTY                  -306138
#define PD_ACCMAXLEGEMPTY              -306140
#define PD_ACCMINQEMPTY                -306142
#define PD_ACCNOEMPTY                  -306144
#define PD_ACCPUBINFEMPTY              -306146
#define PD_ACCSPRCHKEMPTY              -306148
#define PD_ACREMPTY                    -306150
#define PD_ACRACCIDEMPTY               -306152
#define PD_ACTFROMEMPTY                -306154
#define PD_ACTIVEEMPTY                 -306156
#define PD_ACTPASSEMPTY                -306158
#define PD_ADCIDEMPTY                  -306160
#define PD_AFTIDEMPTY                  -306162
#define PD_ATYIDEMPTY                  -306164
#define PD_ALARMEMPTY                  -306166
#define PD_APPLEVENTEMPTY              -306168
#define PD_BASEDONEMPTY                -306170
#define PD_BBOIDEMPTY                  -306172
#define PD_BESTPREMPTY                 -306174
#define PD_BLOCKEMPTY                  -306176
#define PD_BNKACCEMPTY                 -306178
#define PD_BNKIDEMPTY                  -306180
#define PD_BSSIDEMPTY                  -306182
#define PD_BUYSELLEMPTY                -306184
#define PD_CBAIDEMPTY                  -306186
#define PD_CBCIDEMPTY                  -306188
#define PD_CBGCODEEMPTY                -306190
#define PD_CBGIDEMPTY                  -306192
#define PD_CBRIDEMPTY                  -306194
#define PD_CBTIDEMPTY                  -306196
#define PD_CCLIDEMPTY                  -306198
#define PD_CCLUNITEMPTY                -306200
#define PD_CCMIDEMPTY                  -306202
#define PD_CHGIDEMPTY                  -306204
#define PD_CISIDEMPTY                  -306206
#define PD_CHAIDEMPTY                  -306208
#define PD_CLASSEMPTY                  -306210
#define PD_CLEAREDEMPTY                -306212
#define PD_CLEXPEMPTY                  -306214
#define PD_CLHIDEMPTY                  -306216
#define PD_CLOSALLWEMPTY               -306218
#define PD_CLOSESTSEMPTY               -306220
#define PD_COMIDEMPTY                  -306222
#define PD_COMBILLEMPTY                -306224
#define PD_CONTRSZEMPTY                -306226
#define PD_CONTROPEMPTY                -306228
#define PD_CONTRSHREMPTY               -306230
#define PD_CONTRSIZOPEMPTY             -306232
#define PD_CONVDATEEMPTY               -306234
#define PD_COUIDEMPTY                  -306236
#define PD_COUPONEMPTY                 -306238
#define PD_CROSSPREMPTY                -306240
#define PD_CRSIDEMPTY                  -306242
#define PD_CSBIDEMPTY                  -306244
#define PD_CSSIDEMPTY                  -306246
#define PD_CSTIDEMPTY                  -306248
#define PD_CURIDEMPTY                  -306250
#define PD_CUSTCODEEMPTY               -306252
#define PD_DAILYMTHEMPTY               -306254
#define PD_DATEEMPTY                   -306256
#define PD_DAYSEMPTY                   -306258
#define PD_DEALOPEMPTY                 -306260
#define PD_DEFACCEMPTY                 -306262
#define PD_DEFPOSEMPTY                 -306264
#define PD_DELIVEMPTY                  -306266
#define PD_DELIVINSEMPTY               -306268
#define PD_DELNTDEMPTY                 -306270
#define PD_DERIVEMPTY                  -306272
#define PD_DLSCBAIDEMPTY               -306274
#define PD_DLSDATEEMPTY                -306276
#define PD_DLVINTIDEMPTY               -306278
#define PD_DLVLISTEMPTY                -306280
#define PD_DLVSTARTNTDEMPTY            -306282
#define PD_DLVSTOPNTDEMPTY             -306284
#define PD_EVNOEMPTY                   -306286
#define PD_EXCCODEEMPTY                -306288
#define PD_EXCIDEMPTY                  -306290
#define PD_EXCLEMPTY                   -306292
#define PD_EXPFREQEMPTY                -306294
#define PD_EXPDATEEMPTY                -306296
#define PD_EXPIDEMPTY                  -306298
#define PD_EXPNTDEMPTY                 -306300
#define PD_EXPPERIODEMPTY              -306302
#define PD_FACILITYEMPTY               -306304
#define PD_FACTYPEMPTY                 -306306
#define PD_FASTMARCREMPTY              -306308
#define PD_FASTMARTIMEEMPTY            -306310
#define PD_FIELDCEMPTY                 -306312
#define PD_FILLKILLEMPTY               -306314
#define PD_FIRMEMPTY                   -306316
#define PD_FLTIDEMPTY                  -306318
#define PD_FROMEMPTY                   -306320
#define PD_FROMMENUEMPTY               -306322
#define PD_FTBIDEMPTY                  -306324
#define PD_GENSEREMPTY                 -306326
#define PD_GROUPSETTLEMPTY             -306328
#define PD_INCIDEMPTY                  -306330
#define PD_INDEXEMPTY                  -306332
#define PD_INFALWDEMPTY                -306334
#define PD_INGIDEMPTY                  -306336
#define PD_INSIDEMPTY                  -306338
#define PD_INTBDXDEALEMPTY             -306340
#define PD_INTCHKLEGEMPTY              -306342
#define PD_INTERPEMPTY                 -306344
#define PD_INTERPREMPTY                -306346
#define PD_INTIDEMPTY                  -306348
#define PD_INTMAXLEGEMPTY              -306350
#define PD_INTNOTTRADED                -306352
#define PD_INTSPRCHKEMPTY              -306354
#define PD_IPADDREMPTY                 -306356
#define PD_ISINASSEMPTY                -306358
#define PD_ISAPIEMPTY                  -306360
#define PD_ISCLOSEEMPTY                -306362
#define PD_ISFRACTIONSEMPTY            -306364
#define PD_ISEXCLEMPTY                 -306366
#define PD_ISMMEMPTY                   -306368
#define PD_ISNIDEMPTY                  -306370
#define PD_ISNLOWEMPTY                 -306372
#define PD_ISNHIGHEMPTY                -306374
#define PD_ISNETEMPTY                  -306376
#define PD_ISOIEMPTY                   -306378
#define PD_ISOPENEMPTY                 -306380
#define PD_ISSDATEEMPTY                -306382
#define PD_ISSUEMARKEMPTY              -306384
#define PD_ISTRADEREMPTY               -306386
#define PD_ISTRANSEMPTY                -306388
#define PD_ISTRTRDEVTEMPTY             -306390
#define PD_LANIDEMPTY                  -306392
#define PD_LEGNOEMPTY                  -306394
#define PD_LFAIDEMPTY                  -306396
#define PD_LNOIDEMPTY                  -306398
#define PD_LIFETIMEEMPTY               -306400
#define PD_LITIDEMPTY                  -306402
#define PD_LOWLSTEMPTY                 -306404
#define PD_LSTIDEMPTY                  -306406
#define PD_LTRNTDEMPTY                 -306408
#define PD_MANDPREMPTY                 -306410
#define PD_MANDVOLEMPTY                -306412
#define PD_MARIDEMPTY                  -306414
#define PD_MAXCOMBOLEGEMPTY            -306416
#define PD_MAXTIMEEMPTY                -306418
#define PD_MAXUNITEMPTY                -306420
#define PD_MAXVOLEMPTY                 -306422
#define PD_MAXWAITEMPTY                -306424
#define PD_MEMTYPEMPTY                 -306426
#define PD_MENUEMPTY                   -306428
#define PD_MENUTYPEEMPTY               -306430
#define PD_MENUNOEMPTY                 -306432
#define PD_MINHOLDEMPTY                -306434
#define PD_MINQEMPTY                   -306436
#define PD_MINTIMEEMPTY                -306438
#define PD_MINUNITEMPTY                -306440
#define PD_MINVOLEMPTY                 -306442
#define PD_MINWAITEMPTY                -306444
#define PD_MONTHCODEEMPTY              -306446
#define PD_MONTHNAMEEMPTY              -306448
#define PD_MONTHNOEMPTY                -306450
#define PD_MPORCLEMPTY                 -306452
#define PD_NAMEEMPTY                   -306454
#define PD_NEARESTEMPTY                -306456
#define PD_NEWINSEMPTY                 -306458
#define PD_NOCYCLEMPTY                 -306460
#define PD_NODENAMEEMPTY               -306462
#define PD_NODENUMEMPTY                -306464
#define PD_NOOFDLVEMPTY                -306466
#define PD_NOOFINITEMPTY               -306468
#define PD_NOOFLEGSEMPTY               -306470
#define PD_NOSERIESEMPTY               -306472
#define PD_NOTNTDEMPTY                 -306474
#define PD_NOTPERIODEMPTY              -306476
#define PD_NTDIDEMPTY                  -306478
#define PD_OBLIDEMPTY                  -306480
#define PD_OBLRIGHTSEMPTY              -306482
#define PD_OLDINSEMPTY                 -306484
#define PD_ORDNOEMPTY                  -306486
#define PD_OPENCLOSEEMPTY              -306488
#define PD_OPENSTSEMPTY                -306490
#define PD_OPIFBUYEMPTY                -306492
#define PD_OPIFSELLEMPTY               -306494
#define PD_ORDEREMPTY                  -306496
#define PD_ORGIDEMPTY                  -306498
#define PD_OVRBIDEMPTY                 -306500
#define PD_PASSWORDEMPTY               -306502
#define PD_PAYTYPEEMPTY                -306504
#define PD_PEVIDEMPTY                  -306506
#define PD_PFTIDEMPTY                  -306508
#define PD_PHYSADDREMPTY               -306510
#define PD_PODIDEMPTY                  -306512
#define PD_POSTYPEMPTY                 -306514
#define PD_PRDDATEEMPTY                -306516
#define PD_PRDEVEMPTY                  -306518
#define PD_PRDIDEMPTY                  -306520
#define PD_PRDOMNETEMPTY               -306522
#define PD_PRDVENDOREMPTY              -306524
#define PD_PREMUNITEMPTY               -306526
#define PD_PRICEUNITEMPTY              -306528
#define PD_PRICEQEMPTY                 -306530
#define PD_PRICBAIDEMPTY               -306532
#define PD_PRIDAYSEMPTY                -306534
#define PD_PRIMMARKEMPTY               -306536
#define PD_PRINOTEMPTY                 -306538
#define PD_PRIUNITEMPTY                -306540
#define PD_PROPEMPTY                   -306542
#define PD_PSRIDEMPTY                  -306544
#define PD_PUBIDEMPTY                  -306546
#define PD_PURIDEMPTY                  -306548
#define PD_QREQBDXEMPTY                -306550
#define PD_RACCESSEMPTY                -306552
#define PD_WACCESSEMPTY                -306554
#define PD_RATIOEMPTY                  -306556
#define PD_REALTIMEEMPTY               -306558
#define PD_REFNTDEMPTY                 -306560
#define PD_REJACCEMPTY                 -306562
#define PD_REJTIMOUTEMPTY              -306564
#define PD_RESPEMPTY                   -306566
#define PD_RISKEMPTY                   -306568
#define PD_RNTEMPTY                    -306570
#define PD_SBSIDEMPTY                  -306572
#define PD_SBSSTATUSEMPTY              -306574
#define PD_SECCBAIDEMPTY               -306576
#define PD_SECDAYSEMPTY                -306578
#define PD_SEGMNOEMPTY                 -306580
#define PD_SETTLDAYSEMPTY              -306582
#define PD_SIZEEMPTY                   -306584
#define PD_SHORTNAMEEMPTY              -306586
#define PD_SOSTRKPROPEMPTY             -306588
#define PD_SOSTRKPRFACEMPTY            -306590
#define PD_SOCONTRSZOPEMPTY            -306592
#define PD_SOCONTRSZFACEMPTY           -306594
#define PD_SODEALPROPEMPTY             -306596
#define PD_SODEALPRFACEMPTY            -306598
#define PD_SNSIDEMPTY                  -306600
#define PD_SNSUNDEREMPTY               -306602
#define PD_SNSNOOFDECEMPTY             -306604
#define PD_SNSDECEMPTY                 -306606
#define PD_SNSISSID1EMPTY              -306608
#define PD_SNSISSCODEEMPTY             -306610
#define PD_SNSSIGNEMPTY                -306612
#define PD_SNSSIGNCODEEMPTY            -306614
#define PD_SPREADEMPTY                 -306616
#define PD_SPRIDEMPTY                  -306618
#define PD_SPSIDEMPTY                  -306620
#define PD_SRUIDEMPTY                  -306622
#define PD_SRUONEHIGH                  -306624
#define PD_SRUONELOW                   -306626
#define PD_SRUDTIMENOTEMPTY            -306628
#define PD_STARTPROCEMPTY              -306630
#define PD_STATUSEMPTY                 -306632
#define PD_STEPEMPTY                   -306634
#define PD_STEPSFROMUNDEMPTY           -306636
#define PD_STEPSINCYCEMPTY             -306638
#define PD_STOPTIMEEMPTY               -306640
#define PD_STOPTYPEEMPTY               -306642
#define PD_STIMULIEMPTY                -306644
#define PD_STLNTDEMPTY                 -306646
#define PD_STRIKECODEEMPTY             -306648
#define PD_STREXISTEMPTY               -306650
#define PD_STROPEMPTY                  -306652
#define PD_STRUNITEMPTY                -306654
#define PD_SUBSYSEMPTY                 -306656
#define PD_SYMIDEMPTY                  -306658
#define PD_TAXIDEMPTY                  -306660
#define PD_TIMEUNITEMPTY               -306662
#define PD_TIMEBTWTRADEEMPTY           -306664
#define PD_TOEMPTY                     -306666
#define PD_TRADDATEEMPTY               -306668
#define PD_TRADDAYSEMPTY               -306670
#define PD_TRADEDEMPTY                 -306672
#define PD_TRADTIMEEMPTY               -306674
#define PD_TRADTYPEMPTY                -306676
#define PD_TRCIDEMPTY                  -306678
#define PD_TRDREPRULEEMPTY             -306680
#define PD_TRRIDEMPTY                  -306682
#define PD_TRTIDEMPTY                  -306684
#define PD_TSSIDEMPTY                  -306686
#define PD_TZOIDEMPTY                  -306688
#define PD_TYPEEMPTY                   -306690
#define PD_ULTIMO1EMPTY                -306692
#define PD_ULTIMO2EMPTY                -306694
#define PD_USEMODDUPLEMPTY             -306696
#define PD_CALCLTDNOTEMPTY             -306698
#define PD_CALCSTARTNOTEMPTY           -306700
#define PD_CALCSTOPSTARTEMPTY          -306702
#define PD_CALCEXPSTOPEMPTY            -306704
#define PD_UNDOPEMPTY                  -306706
#define PD_UPPIDEMPTY                  -306708
#define PD_UPPINTIDEMPTY               -306710
#define PD_USRIDEMPTY                  -306712
#define PD_USTIDEMPTY                  -306714
#define PD_VALIDLASTEMPTY              -306716
#define PD_VPDIDEMPTY                  -306718
#define PD_XAXISEMPTY                  -306720
#define PD_YAXISEMPTY                  -306722
#define PD_VOLIDEMPTY                  -306724
#define PD_WCCIDEMPTY                  -306726
#define PD_WHENEMPTY                   -306728
#define PD_WINIDEMPTY                  -306730
#define PD_DESCEMPTY                   -306732
#define PD_MSTCLHIDEMPTY               -306734
#define PD_BASCURIDEMPTY               -306736
#define PD_EXTIDEMPTY                  -306738
#define PD_EXTIDNOTEMPTY               -306740
#define PD_MAXSPANEMPTY                -306742
#define PD_PONESPREMPTY                -306744
#define PD_PSECSPREMPTY                -306746
#define PD_PREGEMPTY                   -306748
#define PD_PALLEMPTY                   -306750
#define PD_HVSTPEMPTY                  -306752
#define PD_WVSTPEMPTY                  -306754
#define PD_FLRTSTPEMPTY                -306756
#define PD_PCDAYEMPTY                  -306758
#define PD_ERTRDEMPTY                  -306760
#define PD_SWTRDEMPTY                  -306762
#define PD_FIXBCEMPTY                  -306764
#define PD_PRIMPBEMPTY                 -306766
#define PD_SECPBEMPTY                  -306768
#define PD_CORREMPTY                   -306770
#define PD_ITRHIGHEMPTY                -306772
#define PD_ITRACREMPTY                 -306774
#define PD_ITRMAXEMPTY                 -306776
#define PD_CALCMEMPTY                  -306778
#define PD_PRINTPEMPTY                 -306780
#define PD_BVTIMEMPTY                  -306782
#define PD_HMAXEMPTY                   -306784
#define PD_NEGTIMEMPTY                 -306786
#define PD_VOLUSDEMPTY                 -306788
#define PD_USRNAMEEMPTY                -306790
#define PD_FASTMARKEMPTY               -306792
#define PD_MAXOMNTRTEMPTY              -306794
#define PD_BRDTYPEEMPTY                -306796
#define PD_DISCUNITEMPTY               -306798
#define PD_DISCOUNTEMPTY               -306800
#define PD_PRCALCMEMPTY                -306802
#define PD_DLVSTARTEMPTY               -306804
#define PD_DLVSTOPEMPTY                -306806
#define PD_STARTDATEEMPTY              -306808
#define PD_STOPDATEEMPTY               -306810
#define PD_AUTOEMPTY                   -306812
#define PD_MSUIDEMPTY                  -306814
#define PD_SUPTRPEMPTY                 -306816
#define PD_SUPBPEMPTY                  -306818
#define PD_SUPLTREMPTY                 -306820
#define PD_SUPITREMPTY                 -306822
#define PD_SUPUNCRSEMPTY               -306824
#define PD_PRINFEMPTY                  -306826
#define PD_VOLINFEMPTY                 -306828
#define PD_DELIVMARGEMPTY              -306830
#define PD_PAYMARGEMPTY                -306832
#define PD_USEDIVIDEMPTY               -306834
#define PD_OPENINTEMPTY                -306836
#define PD_ENABLEOPENEMPTY             -306838
#define PD_BESTPROPENEMPTY             -306840
#define PD_OPNIDEMPTY                  -306842
#define PD_OPENTIMEEMPTY               -306844
#define PD_CLOSETIMEEMPTY              -306846
#define PD_COPLESSEMPTY                -306848
#define PD_COPCHGEMPTY                 -306850
#define PD_COPIDEMPTY                  -306852
#define PD_OPENALLSTOPEMPTY            -306854
#define PD_COGCODEEMPTY                -306856
#define PD_COGIDEMPTY                  -306858
#define PD_OBKCODEEMPTY                -306860
#define PD_OBKIDEMPTY                  -306862
#define PD_TSRIDEMPTY                  -306864
#define PD_EDGIDEMPTY                  -306866
#define PD_SPGIDEMPTY                  -306868
#define PD_FMLIDEMPTY                  -306870
#define PD_FOVIDEMPTY                  -306872
#define PD_PDPIDEMPTY                  -306874
#define PD_SYIIDEMPTY                  -306876
#define PD_PRMEXCIDEMPTY               -306878
#define PD_MINBBOEMPTY                 -306880
#define PD_MKTORDEMPTY                 -306882
#define PD_LMTMKTEMPTY                 -306884
#define PD_OPNRECEMPTY                 -306886
#define PD_MAXFML                      -306888
#define PD_SRIIDEMPTY                  -306890
#define PD_BIDNOTASKEMPTY              -306892
#define PD_BIDANDASKEMPTY              -306894
#define PD_ASKNOTBIDEMPTY              -306896
#define PD_ASKANDBIDEMPTY              -306898
#define PD_SPREADFROMEMPTY             -306900
#define PD_MINBIDPRICEEMPTY            -306902
#define PD_MINASKPRICEEMPTY            -306904
#define PD_PRICEEMPTY                  -306906
#define PD_ABSPEREMPTY                 -306908
#define PD_MINSPREADEMPTY              -306910
#define PD_MAXSPREADEMPTY              -306912
#define PD_ROUNDINGEMPTY               -306914
#define PD_TDPIDEMPTY                  -306916
#define PD_TDSIDEMPTY                  -306918
#define PD_DELAYLESSCOP                -306920
#define PD_DOUBLERISKP                 -306922
#define PD_ISSINFUTURE                 -306924
#define PD_LREIDEMPTY                  -306926
#define PD_LCOIDEMPTY                  -306928
#define PD_REGNAMEEMPTY                -306930
#define PD_REFTOACC                    -306932
#define PD_REFTOACR                    -306934
#define PD_REFTOCBC                    -306936
#define PD_REFTOCBS                    -306938
#define PD_REFTOCBT                    -306940
#define PD_REFTOCCL                    -306942
#define PD_REFTOCLH                    -306944
#define PD_REFTOCLI                    -306946
#define PD_REFTOCOM                    -306948
#define PD_REFTOCSL                    -306950
#define PD_REFTOCSS                    -306952
#define PD_REFTOCST                    -306954
#define PD_REFTOCUP                    -306956
#define PD_REFTOEXC                    -306958
#define PD_REFTOFVL                    -306960
#define PD_REFTOICO                    -306962
#define PD_REFTOILU                    -306964
#define PD_REFTOINC                    -306966
#define PD_REFTOINS                    -306968
#define PD_REFTOINT                    -306970
#define PD_REFTOISN                    -306972
#define PD_REFTOISS                    -306974
#define PD_REFTOITU                    -306976
#define PD_REFTOLGP                    -306978
#define PD_REFTOLIT                    -306980
#define PD_REFTOLIU                    -306982
#define PD_REFTOLNO                    -306984
#define PD_REFTOLTT                    -306986
#define PD_REFTOLTU                    -306988
#define PD_REFTOMAC                    -306990
#define PD_REFTOMAR                    -306992
#define PD_REFTOMNA                    -306994
#define PD_REFTOORG                    -306996
#define PD_REFTOPAYCST                 -306998
#define PD_REFTOPEV                    -307000
#define PD_REFTOPFT                    -307002
#define PD_REFTOPQR                    -307004
#define PD_REFTOPUR                    -307006
#define PD_REFTOSYM                    -307008
#define PD_REFTOTAX                    -307010
#define PD_REFTOTRC                    -307012
#define PD_REFTOUSR                    -307014
#define PD_REFTOMNU                    -307016
#define PD_REFTOOBL                    -307018
#define PD_REFTOCOG                    -307020
#define PD_REFTOFML                    -307022
#define PD_REFTOFOV                    -307024
#define PD_REFTOOBK                    -307026
#define PD_REFTOPDP                    -307028
#define PD_REFTOSYI                    -307030
#define PD_REFTOTSR                    -307032
#define PD_REFTOSRI                    -307034
#define PD_REFTOTDS                    -307036
#define PD_REFTOTDP                    -307038
#define PD_REFTOLRE                    -307040
#define PD_REFTOLCO                    -307042
#define PD_SYSERR                      -307044
#define PD_INSFMEM                     -307046
#define PD_ERRNOEXP                    -307048
#define PD_ERRNOMAR                    -307050
#define PD_ERRNOINT                    -307052
#define PD_ERRNOINC                    -307054
#define PD_ERRNOCCL                    -307056
#define PD_FLDTYPENOTEX                -307058
#define PD_FLDNOTEX                    -307060
#define PD_WRONGVERSION                -307062
#define PD_VPCNOINIT                   -307064
#define PD_VPCNOANS                    -307066
#define PD_SCODENOTCREATED             -307068
#define PD_MCODENOTCREATED             -307070
#define PD_WRUPDNOTALLOWED             -307072
#define PD_UPDNOTBOTHSYM               -307074
#define PD_OMDUNOTEVT                  -307076
#define PD_ILLEGFAC                    -307078
#define PD_ILLSETTLPROP                -307080
#define PD_SERISTRADED                 -307082
#define PD_ALLOWUNINTEMPTY             -307084
#define PD_WRONGUNDATTR                -307086
#define PD_WRONGEXPATTR                -307088
#define PD_WRONGDEDATTR                -307090
#define PD_QRYONNNOTSUPP               -307092
#define PD_COMBODEALEMPTY              -307094
#define PD_COUNTSIGNEMPTY              -307096
#define PD_NOCLP                       -307098
#define PD_NOCLPACT                    -307100
#define PD_CLPIDEMPTY                  -307102
#define PD_NETTMTHEMPTY                -307104
#define PD_DIFFCLP                     -307106
#define PD_STEPNOTMULT                 -307108
#define PD_LOWZEROMISS                 -307110
#define PD_ISSNOOPRA                   -307112
#define PD_SONOOPRA                    -307114
#define PD_SOISSOPRAISEQ               -307116
#define PD_SOOPRAISEQ                  -307118
#define PD_ISSOPRAISEQ                 -307120
#define PD_ISSOPRAEXIST                -307122
#define PD_SOOPRAEXIST                 -307124
#define PD_NEWINSIDEXIST               -307126
#define PD_NEWINSCODEEXIST             -307128
#define PD_SOINSIDEXIST                -307130
#define PD_SOINSCODEEXIST              -307132
#define PD_CLPCSSIDEMPTY               -307134
#define PD_MMLCKTIME                   -307136
#define PD_BLKRSPTIME                  -307138
#define PD_BLKMINSIZE                  -307140
#define PD_ISSTYPEEMPTY                -307142
#define PD_NOTALLSPG                   -307144
#define PD_TOOMANYSPG                  -307146
#define PD_DOUBLESPG                   -307148
#define PD_DOUBLEEDG                   -307150
#define PD_SYISYMEXIST                 -307152
#define PD_TYPEISTRADED                -307154
#define PD_CBTEXISTS                   -307156
#define PD_MARNOTTRADED                -307158
#define PD_CLASSISTRADED               -307160
#define PD_IMPLNOTINFUT                -307162
#define PD_INTRADAYNOTALLWD            -307164
#define PD_TOTWCNOTALLWD               -307166
#define PD_CLQRYFAIL                   -307168
#define PD_INFOQRYFAIL                 -307170
#define PD_IDCHANGED                    307171
#define PD_INCCHANGED                   307173
#define PD_EXPCHANGED                   307175
#define PD_LTTCHANGED                   307177
#define PD_STRCHANGED                   307179
#define PD_ISNCHANGED                   307181
#define PD_NORELCHANGED                 307183
#define PD_ISSUELSTONLY                -307184
#define PD_ACTIVELSTONLY               -307186
#define PD_LSTCHGNOTALLOWED            -307188
#define PD_BOTHUNDNOTALLOWED           -307190
#define PD_NEWMARNOTALLOWED            -307192
#define PD_DECISNOTEQ                  -307194
#define PD_TSEIDEMPTY                  -307196
#define PD_TIMEEMPTY                   -307198
#define PD_STATEEMPTY                  -307200
#define PD_DOUBLETIME                  -307202
#define PD_NOTSE                       -307204
#define PD_NOTSEACT                    -307206
#define PD_CSTATEWR                    -307208
#define PD_PSTATEWR                    -307210
#define PD_ASTATEWR                    -307212
#define PD_RSTATEWR                    -307214
#define PD_ILLSTARTPROC                -307216
#define PD_STARTINTOVR                 -307218
#define PD_LTTIDEMPTY                  -307220
#define PD_NOLTT                       -307222
#define PD_NOLTTACT                    -307224
#define PD_VALIDFROMEMPTY              -307226
#define PD_VALIDUNTILEMPTY             -307228
#define PD_SOMODEMPTY                  -307230
#define PD_NOINSTEMPLFOUND             -307232
#define PD_NOINCTEMPLFOUND             -307234
#define PD_NOTMCOMFOUND                -307236
#define PD_BOTHNEWSPEMPTY              -307238
#define PD_TSEDIFFNTD                  -307240
#define PD_ONETEMPALLWD                -307242
#define PD_CALCLTDEXPEMPTY             -307244
#define PD_INVLTDCORR                  -307246
#define PD_OPRAINDEXEMPTY              -307248
#define PD_DOUBLEEXC                   -307250
#define PD_ACTCRSEMPTY                 -307252
#define PD_SSTIDEMPTY                  -307254
#define PD_VOLINTEMPTY                 -307256
#define PD_AGGRPAYEMPTY                -307258
#define PD_RESERVSTATE                 -307260
#define PD_HALFDAYEMPTY                -307262
#define PD_NOSST                       -307264
#define PD_NOSSTACT                    -307266
#define PD_CNTRSZISNOTEQ               -307268
#define PD_WREXPOP                     -307270
#define PD_WREXPFAC                    -307272
#define PD_REFTOSBS                    -307274
#define PD_SWBIDEMPTY                  -307276
#define PD_REFINSEMPTY                 -307278
#define PD_LOCKREFEMPTY                -307280
#define PD_DOUBLESWB                   -307282
#define PD_NOREFINS                    -307284
#define PD_NOREFINSACT                 -307286
#define PD_NOSWB                       -307288
#define PD_SWBALRSUSP                  -307290
#define PD_SWBALRRESUMED               -307292
#define PD_NOTLOCKED                   -307294
#define PD_CEGIDEMPTY                  -307296
#define PD_DOUBLECEG                   -307298
#define PD_MATURITYEMPTY               -307300
#define PD_FCOUPONEMPTY                -307302
#define PD_CUPFREQEMPTY                -307304
#define PD_NOOUTRREFINS                -307306
#define PD_NOOUTRREFINSACT             -307308
#define PD_NOOUTRINS                   -307310
#define PD_NOOUTRINSACT                -307312
#define PD_OUTRREFINSEMPTY             -307314
#define PD_OUTRINSEMPTY                -307316
#define PD_IPRIDEMPTY                  -307318
#define PD_NOIPR                       -307320
#define PD_NOIPRACT                    -307322
#define PD_NOREFSPEC                   -307324
#define PD_REFSPEC                     -307326
#define PD_DIFFREFSPEC                 -307328
#define PD_NOTSUSPENDED                -307330
#define PD_REFMUSTBESWAP               -307332
#define PD_REFISSWAP                   -307334
#define PD_OUTRREFISSWAP               -307336
#define PD_LEGMUSTBESWAP               -307338
#define PD_OUTRLEGISSWAP               -307340
#define PD_LEGEQREF                    -307342
#define PD_OLEGEQREF                   -307344
#define PD_OLEGEQOREF                  -307346
#define PD_DOUBLEOSWB                  -307348
#define PD_DOUBLEGNO                   -307350
#define PD_PRYTYPEEMPTY                -307352
#define PD_PRYCONVEMPTY                -307354
#define PD_USECUSIPEMPTY               -307356
#define PD_WRONGCUSIP                  -307358
#define PD_OBLEVNOT1OR5                -307360
#define PD_CUSIPTOLONG                 -307362
#define PD_WRLTTTIME                   -307364
#define PD_REFEXISTASREF               -307366
#define PD_LEGEXISTASREF               -307368
#define PD_REFEXISTASLEG               -307370
#define PD_LEGEXISTASLEG               -307372
#define PD_ERRINFRAC                   -307374
#define PD_REFTOSWB                    -307376
#define PD_MARORDACTWRONG              -307378
#define PD_MARORDACTNOTDEF             -307380
#define PD_COUPAFTMAT                  -307382
#define PD_SERIESEXCEED                -307384
#define PD_LOCKEDSWAP                  -307386
#define PD_SWAPNOTINBOX                -307388
#define PD_DESIDEMPTY                  -307390
#define PD_REFTODES                    -307392
#define PD_DGRIDEMPTY                  -307394
#define PD_REFTODGR                    -307396
#define PD_UPPEREMPTY                  -307398
#define PD_VOLCHGEMPTY                 -307400
#define PD_VDSIDEMPTY                  -307402
#define PD_PMMOBLTYPEEMPTY             -307404
#define PD_TRDIDEMPTY                  -307406
#define PD_INFFEEDIDEMPTY              -307408
#define PD_DEPOSIDEMPTY                -307410
#define PD_HEDGEEXCIDEMPTY             -307412
#define PD_HEDGECSTIDEMPTY             -307414
#define PD_DOUBLEEXTID                 -307416
#define PD_NODES                       -307418
#define PD_NODESACT                    -307420
#define PD_NOVDS                       -307422
#define PD_NOVDSACT                    -307424
#define PD_NODHB                       -307426
#define PD_NODHBACT                    -307428
#define PD_DHBIDEMPTY                  -307430
#define PD_DEPOSTYPEEMPTY              -307432
#define PD_DECISDIFF                   -307434
#define PD_NODGR                       -307436
#define PD_ISINMISSING                  307437
#define PD_ALRSIGNALLED                -307438
#define PD_REFTOTSE                    -307440
#define PD_NOORV                       -307442
#define PD_NOORVACT                    -307444
#define PD_ORVIDEMPTY                  -307446
#define PD_HEDGEALGEMPTY               -307448
#define PD_NOMTH                       -307450
#define PD_NOMTHACT                    -307452
#define PD_MTHIDEMPTY                  -307454
#define PD_NOPDR                       -307456
#define PD_NOPDRACT                    -307458
#define PD_PDRIDEMPTY                  -307460
#define PD_BOTHCOMCIS                  -307462
#define PD_DOUBLECIS                   -307464
#define PD_NOATR                       -307466
#define PD_NOATRACT                    -307468
#define PD_NOLAI                       -307470
#define PD_NOLAIACT                    -307472
#define PD_NOPRL                       -307474
#define PD_NOPRLACT                    -307476
#define PD_NOCIB                       -307478
#define PD_NOCIBACT                    -307480
#define PD_NOTIR                       -307482
#define PD_NOTIRACT                    -307484
#define PD_NOIMS                       -307486
#define PD_NOIMSACT                    -307488
#define PD_NOICS                       -307490
#define PD_NOICSACT                    -307492
#define PD_ATRIDEMPTY                  -307494
#define PD_AATIDEMPTY                  -307496
#define PD_DOUBLEAAT                   -307498
#define PD_NOAAT                       -307500
#define PD_NOAATACT                    -307502
#define PD_PRFORMEMPTY                 -307504
#define PD_STRFORMEMPTY                -307506
#define PD_CABFORMEMPTY                -307508
#define PD_SETTLUNITEMPTY              -307510
#define PD_CIBIDEMPTY                  -307512
#define PD_ICSIDEMPTY                  -307514
#define PD_IMSIDEMPTY                  -307516
#define PD_PRIOEMPTY                   -307518
#define PD_CRRATEEMPTY                 -307520
#define PD_DSRATIOEMPTY                -307522
#define PD_AORBEMPTY                   -307524
#define PD_CHARGEEMPTY                 -307526
#define PD_LAIIDEMPTY                  -307528
#define PD_REFTOLAI                    -307530
#define PD_MORETHANONE                 -307532
#define PD_DOUBLEINST                  -307534
#define PD_TIERIDEMPTY                 -307536
#define PD_EXPNOEMPTY                  -307538
#define PD_DOUBLETIER                  -307540
#define PD_TIEREMPTY                   -307542
#define PD_DOUBLEICS                   -307544
#define PD_DOUBLEIMS                   -307546
#define PD_PRLIDEMPTY                  -307548
#define PD_EXTPROVEMPTY                -307550
#define PD_EXTERNIDEMPTY               -307552
#define PD_LTUNITEMPTY                 -307554
#define PD_INTRATEEMPTY                -307556
#define PD_EXPISSPEC                   -307558
#define PD_EXPNOTSPEC                  -307560
#define PD_DOUBLETDP                   -307562
#define PD_DOUBLETDS                   -307564
#define PD_INTTRDREPRULEEMPTY          -307566
#define PD_NOMTM                       -307568
#define PD_NOMTMACT                    -307570
#define PD_ATTRRULEEMPTY               -307572
#define PD_ONLYONEDEFSAX               -307574
#define PD_WRONGDATEFORMAT             -307576
#define PD_NOSID                       -307578
#define PD_NOSIDACT                    -307580
#define PD_SIDIDEMPTY                  -307582
#define PD_CURRFORMEMPTY               -307584
#define PD_REFTOATY                    -307586
#define PD_REFTOWIN                    -307588
#define PD_EXCEEDFORMAT                -307590
#define PD_EXCEEDPRFORMAT              -307592
#define PD_EXCEEDCABFORMAT             -307594
#define PD_COMBOMTHEMPTY               -307596
#define PD_WRONGSTRFORMAT              -307598
#define PD_CBOMINLEGSEXCEED            -307600
#define PD_RFQPRICEMPTY                -307602
#define PD_CLOSINGTRDPREMPTY           -307604
#define PD_STRIKEEXCEEDED              -307606
#define PD_FIXINSIDEMPTY               -307608
#define PD_FIXINSIDNOTEMPTY            -307610
#define PD_MMRESPTYPEEMPTY             -307612
#define PD_TRDAUTHEMPTY                -307614
#define PD_NOLEGSDEFINED               -307616
#define PD_NORRU                       -307618
#define PD_NORRUACT                    -307620
#define PD_NOGMS                       -307622
#define PD_NOGMSACT                    -307624
#define PD_NOGCT                       -307626
#define PD_NOGCTACT                    -307628
#define PD_GCTIDEMPTY                  -307630
#define PD_MAXYEMPTY                   -307632
#define PD_FBBIDEMPTY                  -307634
#define PD_FUTINSEMPTY                 -307636
#define PD_REFTOFBB                    -307638
#define PD_FUTMUSTBEFB                 -307640
#define PD_LEGMUSTBEFBLEG              -307642
#define PD_LEGEQFUT                    -307644
#define PD_OUTRLEGISWRONG              -307646
#define PD_OLEGEQFB                    -307648
#define PD_SEVCHFOUND                  -307650
#define PD_NOCONVPRICE                 -307652
#define PD_DOUBLEFBB                   -307654
#define PD_DOUBLEOFBB                  -307656
#define PD_NOCHFOUND                   -307658
#define PD_DIFFCTDSPEC                 -307660
#define PD_DIFFCONVSPEC                -307662
#define PD_FBEXISTASFB                 -307664
#define PD_LEGEXISTASFB                -307666
#define PD_FBEXISTASLEG                -307668
#define PD_NOFBB                       -307670
#define PD_NOCONVFACTOR                -307672
#define PD_REFTOGCG                    -307674
#define PD_DOUBLEGCT                   -307676
#define PD_GMSIDEMPTY                  -307678
#define PD_DOUBLENOMTR                 -307680
#define PD_RRUIDEMPTY                  -307682
#define PD_HIDDVOLMETHEMPTY            -307684
#define PD_DOUBLERRU                   -307686
#define PD_MORPEMPTY                   -307688
#define PD_MORPSPEC                    -307690
#define PD_STARTDATESPEC               -307692
#define PD_STOPDATESPEC                -307694
#define PD_REPOGENSPEC                 -307696
#define PD_RRUIDSPEC                   -307698
#define PD_GMSIDSPEC                   -307700
#define PD_UNDERLEXIST                 -307702
#define PD_SERIESEXIST                 -307704
#define PD_CLASSEXIST                  -307706
#define PD_DUPLUNDERL                  -307708
#define PD_DUPLSERIES                  -307710
#define PD_DUPLISIN                    -307712
#define PD_NOTMPLINC                   -307714
#define PD_NOTMPLCOM                   -307716
#define PD_NOTMPLINS                   -307718
#define PD_ISSDATEWRONG                -307720
#define PD_MATURDATEWRONG              -307722
#define PD_DATEDDATEWRONG              -307724
#define PD_MATURDATEEMPTY              -307726
#define PD_DATEDDATEEMPTY              -307728
#define PD_FIRSTCOUPWRONG              -307730
#define PD_LASTCOUPWRONG               -307732
#define PD_FIRSTCOUPEMPTY              -307734
#define PD_COUPFREQEMPTY               -307736
#define PD_GCGROUPEMPTY                -307738
#define PD_FITWRONG                    -307740
#define PD_NOMINALISZERO               -307742
#define PD_REDEMPISZERO                -307744
#define PD_WRONGDCR                    -307746
#define PD_NOCTDPRICE                  -307748
#define PD_CTDPRICESPEC                -307750
#define PD_PSPIDEMPTY                  -307752
#define PD_NOPSP                       -307754
#define PD_NOPSPACT                    -307756
#define PD_DEFCOLLIDEMPTY              -307758
#define PD_NODEF                       -307760
#define PD_NODEFACT                    -307762
#define PD_REFTOCDC                    -307764
#define PD_REFTOPSP                    -307766
#define PD_DOUBLEDEF                   -307768
#define PD_WRONGDEF                    -307770
#define PD_NODEFSPEC                   -307772
#define PD_FUTBNOTINBOX                -307774
#define PD_INSLONGALREXIST             -307776
#define PD_CBSLONGALREXIST             -307778
#define PD_STARTTERMEMPTY              -307780
#define PD_ENDTERMEMPTY                -307782
#define PD_TERMLENEMPTY                -307784
#define PD_TERMLENSPEC                 -307786
#define PD_INSIDEXISTASLONG            -307788
#define PD_INSLONGEXISTASID            -307790
#define PD_TMREPONOTALLOWED            -307792
#define PD_SWBTYPEEMPTY                -307794
#define PD_FUTBOXTYPEEMPTY             -307796
#define PD_REFMUSTBETFORW              -307798
#define PD_CHFOUND                     -307800
#define PD_INVFULLDEPTH                -307802
#define PD_DESEXCEMPTY                 -307804
#define PD_PMMOBLTYPEINV               -307806
#define PD_SERIESSTATUSINV             -307808
#define PD_DEPOSTYPEINV                -307810
#define PD_OPERATIONINV                -307812
#define PD_INVIMPLDATE                 -307814
#define PD_WRONGDGRIMPLDATE            -307816
#define PD_INVHEDGEMEMBER              -307818
#define PD_WKPIDEMPTY                  -307820
#define PD_NOWKP                       -307822
#define PD_NOWKPACT                    -307824
#define PD_REFTOWKP                    -307826
#define PD_INVOBLEVEL                  -307828
#define PD_INVEXTFULLDEPTH             -307830
#define PD_INVINTFULLDEPTH             -307832
#define PD_INVDAYINWEEK                -307834
#define PD_INVWEEKINMONTH              -307836
#define PD_RATIOEXCEEDED               -307838
#define PD_NOMCSDATA                   -307840
#define PD_MCSCHKFAILED                -307842
#define PD_DIFFUNDERL                  -307844
#define PD_ILLRATIO                    -307846
#define PD_NOCBX                       -307848
#define PD_CBXIDEMPTY                  -307850
#define PD_BOXTYPEEMPTY                -307852
#define PD_REFTOCBX                    -307854
#define PD_DOUBLEOLEG                  -307856
#define PD_BOXALRSUSP                  -307858
#define PD_BOXALRRESUMED               -307860
#define PD_LEGMUSTBEOPSWPLEG           -307862
#define PD_NOCBS                       -307864
#define PD_NOCBSACT                    -307866
#define PD_SORTTYPEEMPTY               -307868
#define PD_WRONGPRFORMAT               -307870
#define PD_NEGVALNOTALLOWED            -307872
#define PD_NOCCR                       -307874
#define PD_NOCCRACT                    -307876
#define PD_CCRIDEMPTY                  -307878
#define PD_ACCRINTUDEMPTY              -307880
#define PD_CLEANPRUDEMPTY              -307882
#define PD_DAYCCONVEMPTY               -307884
#define PD_EOMCCONVEMPTY               -307886
#define PD_CALCCEMPTY                  -307888
#define PD_SNSFREETEXTEMPTY            -307890
#define PD_SNSNOOFSUBEMPTY             -307892
#define PD_SNSTERMPARAMEMPTY           -307894
#define PD_LONGSNSIDEMPTY              -307896
#define PD_TERMSNSIDEMPTY              -307898
#define PD_INCLOPTPREMEMPTY            -307900
#define PD_NOPRF                       -307902
#define PD_NOPRFACT                    -307904
#define PD_PRFIDEMPTY                  -307906
#define PD_OPTMIDPREMPTY               -307908
#define PD_GROSSORNETEMPTY             -307910
#define PD_DOUBLEPRF                   -307912
#define PD_TERMIDSPEC                  -307914
#define PD_NOIXV                       -307916
#define PD_NOIXVACT                    -307918
#define PD_IXVIDEMPTY                  -307920
#define PD_DOUBLEIXDATE                -307922
#define PD_NOCRO                       -307924
#define PD_NOCROACT                    -307926
#define PD_CROIDEMPTY                  -307928
#define PD_IXDATEEMPTY                 -307930
#define PD_IXVALEMPTY                  -307932
#define PD_IXVIDNOTEMPTY               -307934
#define PD_IXVALNOTEMPTY               -307936
#define PD_LAGISEMPTY                  -307938
#define PD_LAGISNOTEMPTY               -307940
#define PD_DECINDEXNOTEMPTY            -307942
#define PD_PROTCOUPNOTEMPTY            -307944
#define PD_PROTREDEMPTNOTEMPTY         -307946
#define PD_ILLSTARTHOUR                -307948
#define PD_ILLSTOPHOUR                 -307950
#define PD_ILLCASCHOUR                 -307952
#define PD_ILLSTARTMIN                 -307954
#define PD_ILLSTOPMIN                  -307956
#define PD_ILLCASCMIN                  -307958
#define PD_COUNTERPINFOEMPTY           -307960
#define PD_CURTYPEMPTY                 -307962
#define PD_CURTYPNOTEMPTY              -307964
#define PD_REFTOGCC                    -307966
#define PD_DELTAALLOCTIMESPEC          -307968
#define PD_TZISEMPTY                   -307970
#define PD_NOOFSERISZERO               -307972
#define PD_NOOFSEREXCEED               -307974
#define PD_NOOFUSRISZERO               -307976
#define PD_NOGWY                       -307978
#define PD_NOGWYACT                    -307980
#define PD_PQFOPEMPTY                  -307982
#define PD_SOPQFOPEMPTY                -307984
#define PD_CLASSLIMGTTOTAL             -307986
#define PD_WRONGCLASSQUERY             -307988
#define PD_WRONGCLASSTRANS             -307990
#define PD_LOWERLIMWRONG               -307992
#define PD_UPPERLIMWRONG               -307994
#define PD_STEPSIZEWRONG               -307996
#define PD_MTMPRICEWRONG               -307998
#define PD_NOCMI                       -308000
#define PD_NOCMIACT                    -308002
#define PD_CMIIDEMPTY                  -308004
#define PD_NOCSC                       -308006
#define PD_NOCSCACT                    -308008
#define PD_CSCIDEMPTY                  -308010
#define PD_CMIFORMATEMPTY              -308012
#define PD_LIFETYPEEMPTY               -308014
#define PD_AMOUNTEMPTY                 -308016
#define PD_PASSAGGREMPTY               -308018
#define PD_REFTOCSC                    -308020
#define PD_DOUBLECMI                   -308022
#define PD_LINKTYPEEMPTY               -308024
#define PD_DOUBLETZ                    -308026
#define PD_IMPLIMMNOTALLOWED           -308028
#define PD_LTDAFTERCLOSENOTALLOWED     -308030
#define PD_TOOMANYCBX                  -308032
#define PD_WRONGHOLDTIME               -308034
#define PD_MAXCOUPON                   -308036
#define PD_ONELINKAGE                  -308038
#define PD_NOSAR                       -308040
#define PD_NOSARACT                    -308042
#define PD_SARIDEMPTY                  -308044
#define PD_NODPR                       -308046
#define PD_NODPRACT                    -308048
#define PD_DPRIDEMPTY                  -308050
#define PD_NORIT                       -308052
#define PD_NORITACT                    -308054
#define PD_RITIDEMPTY                  -308056
#define PD_NORATEINS                   -308058
#define PD_NORATEINSACT                -308060
#define PD_RATEINSIDEMPTY              -308062
#define PD_REFTORIT                    -308064
#define PD_REFTOPOD                    -308066
#define PD_SECACCOUNTEMPTY             -308068
#define PD_CASHACCOUNTEMPTY            -308070
#define PD_DOUBLEACC                   -308072
#define PD_DOUBLERIT                   -308074
#define PD_REFTOCSA                    -308076
#define PD_REFTOSEC                    -308078
#define PD_FROMGEQTO                   -308080
#define PD_DOUBLEFROM                  -308082
#define PD_DOUBLETO                    -308084
#define PD_FROMOVRLAP                  -308086
#define PD_RITIDIDEMPTY                -308088
#define PD_RITIDNOTEMPTY               -308090
#define PD_TOOMANYACCOUNTS             -308092
#define PD_DOUBLEBIC                   -308094
#define PD_WRONGBIC                    -308096
#define PD_WRONGACC                    -308098
#define PD_BCDATEAFTCOUP               -308100
#define PD_BCDATEAFTMAT                -308102
#define PD_COUPBEFISS                  -308104
#define PD_BCDATEBEFISS                -308106
#define PD_COUPBEFDATED                -308108
#define PD_BCDATEBEFDATED              -308110
#define PD_INVMINIOBLEVEL              -308112
#define PD_GROUPTYPEEMPTY              -308114
#define PD_OPTTYPEEMPTY                -308116
#define PD_OPTSTYLEEMPTY               -308118
#define PD_REPOTYPEEMPTY               -308120
#define PD_SYNTHTYPEEMPTY              -308122
#define PD_MARTYPEEMPTY                -308124
#define PD_AVPERIODEMPTY               -308126
#define PD_SETTLCUREMPTY               -308128
#define PD_NOSETTLCUR                  -308130
#define PD_NOSETTLCURACT               -308132
#define PD_HOLIDAYADJEMPTY             -308134
#define PD_AGTIDEMPTY                  -308136
#define PD_NOAGT                       -308138
#define PD_NOAGTACT                    -308140
#define PD_EFTIDEMPTY                  -308142
#define PD_NOEFT                       -308144
#define PD_NOEFTACT                    -308146
#define PD_AGREEMENTEMPTY              -308148
#define PD_EFTSTIMULIEMPTY             -308150
#define PD_EXTFEETYPEEMPTY             -308152
#define PD_DOUBLEEXTFEE                -308154
#define PD_DOUBLEAGREEMENT             -308156
#define PD_CPNINTRSTISZERO             -308158
#define PD_CPNINTRSTNOTZERO            -308160
#define PD_COUPONSEXIST                -308162
#define PD_COUPONSNOTEXIST             -308164
#define PD_DAYCALCRULEWRONG            -308166
#define PD_COUPDATEWRONG               -308168
#define PD_BOOKSCLOSEDATEWRONG         -308170
#define PD_LOWERSTRIKEGEUPPER          -308172
#define PD_LOWEREXPGEUPPER             -308174
#define PD_DOUBLEUPPERSTRIKE           -308176
#define PD_DOUBLEUPPEREXP              -308178
#define PD_DOUBLELOWERSTRIKE           -308180
#define PD_DOUBLELOWEREXP              -308182
#define PD_LOWERSTRIKEOVRLAP           -308184
#define PD_LOWEREXPOVRLAP              -308186
#define PD_EXPLEZERO                   -308188
#define PD_LIMITNOTNEG                 -308190
#define PD_REFTOCIB                    -308192
#define PD_REFTOSID                    -308194
#define PD_REFTOPRL                    -308196
#define PD_NOSPL                       -308198
#define PD_NOSPLACT                    -308200
#define PD_NOEDL                       -308202
#define PD_NOEDLACT                    -308204
#define PD_SPLIDEMPTY                  -308206
#define PD_EDLIDEMPTY                  -308208
#define PD_PRICELIMITUNITEMPTY         -308210
#define PD_GWTIDEMPTY                  -308212
#define PD_LIMITTOSMALL                -308214
#define PD_NOGWT                       -308216
#define PD_NOGWTACT                    -308218
#define PD_UNLIMORLIMIT                -308220
#define PD_SETACTION                   -308222
#define PD_REFTOGWY                    -308224
#define PD_NOTLIMITTOHIGH              -308226
#define PD_ITMHIGHDITM                 -308228
#define PD_ATMHIGHOTM                  -308230
#define PD_STARTDATENOTEMPTY           -308232
#define PD_STOPDATENOTEMPTY            -308234
#define PD_OPTTYPENOTEMPTY             -308236
#define PD_OPTSTYLENOTEMPTY            -308238
#define PD_FUTSTYLEDSPEC               -308240
#define PD_WARRANTSPEC                 -308242
#define PD_REPOTYPENOTEMPTY            -308244
#define PD_BUYSELLBACKSPEC             -308246
#define PD_SYNTHTYPENOTEMPTY           -308248
#define PD_NONTRADREFSPEC              -308250
#define PD_CONTRSIZOPNOTEMPTY          -308252
#define PD_PQFOPNOTEMPTY               -308254
#define PD_BINDEALSNOTYES              -308256
#define PD_BILATCLNOTYES               -308258
#define PD_STARTTERMWRONG              -308260
#define PD_EXCCODEEXCEED               -308262
#define PD_MARCODEEXCEED               -308264
#define PD_INGCODEEXCEED               -308266
#define PD_CBGCODEEXCEED               -308268
#define PD_COMCODEEXCEED               -308270
#define PD_DOUBLEMARORDACT             -308272
#define PD_DIFFLTTPARAM                -308274
#define PD_DIFFIMSCHARGE               -308276
#define PD_DIFFICSCREDITRATE           -308278
#define PD_MAXLEGEXCEED                -308280
#define PD_DOUBLEDEFINED               -308282
#define PD_REFTOSSS                    -308284
#define PD_ECBIDEMPTY                  -308286
#define PD_NOECB                       -308288
#define PD_NOECBACT                    -308290
#define PD_SSSIDEMPTY                  -308292
#define PD_NOSSS                       -308294
#define PD_NOSSSACT                    -308296
#define PD_REFTOECB                    -308298
#define PD_STATEREFORLAST              -308300
#define PD_NORRD                       -308302
#define PD_NORRDACT                    -308304
#define PD_RRDIDEMPTY                  -308306
#define PD_NORSP                       -308308
#define PD_NORSPACT                    -308310
#define PD_RSPIDEMPTY                  -308312
#define PD_DOUBLERRD                   -308314
#define PD_REFTORRD                    -308316
#define PD_REFTORUP                    -308318
#define PD_RUPIDEMPTY                  -308320
#define PD_RUNNUMBEREMPTY              -308322
#define PD_NORUP                       -308324
#define PD_NORUPACT                    -308326
#define PD_CLOSEONLYNOTALLWD           -308328
#define PD_DOUBLESTRESSMAR             -308330
#define PD_NOTINDATAGEN                -308332
#define PD_DOUBLEBASICSCEN             -308334
#define PD_PQRPUBIDEMPTY               -308336
#define PD_EFTALLEMPTY                 -308338
#define PD_NODSC                       -308340
#define PD_NODSCACT                    -308342
#define PD_REFTOEFT                    -308344
#define PD_PERNOLIM                    -308346
#define PD_WRONGREFPRICEFORMAT         -308348
#define PD_ENQREQEMPTY                 -308350
#define PD_COMPREQEMPTY                -308352
#define PD_GWYILLCHAR                  -308354
#define PD_GWYGENERICEXIST             -308356
#define PD_TMNOTALLWDFROMPD            -308358
#define PD_REFTOAUR                    -308360
#define PD_AURIDEMPTY                  -308362
#define PD_NOAUR                       -308364
#define PD_NOAURACT                    -308366
#define PD_TRTNOTAUTH                  -308368
#define PD_DOUBLEAUR                   -308370
#define PD_HCTIDEMPTY                  -308372
#define PD_NOHCT                       -308374
#define PD_NOHCTACT                    -308376
#define PD_HAIRCREMPTY                 -308378
#define PD_NOCVR                       -308380
#define PD_NOCVRACT                    -308382
#define PD_NOCOR                       -308384
#define PD_NOCORACT                    -308386
#define PD_ACCEPTCOLLWRONG             -308388
#define PD_CVRIDEMPTY                  -308390
#define PD_CORIDEMPTY                  -308392
#define PD_COLLTYPEEMPTY               -308394
#define PD_NOATY                       -308396
#define PD_FRBIDEMPTY                  -308398
#define PD_NOFRB                       -308400
#define PD_NOFRBACT                    -308402
#define PD_NOCONSBNK                   -308404
#define PD_NOCONSBNKACT                -308406
#define PD_CONSBNKIDEMPTY              -308408
#define PD_PVMIDEMPTY                  -308410
#define PD_NOPVM                       -308412
#define PD_NOPVMACT                    -308414
#define PD_VMAIDEMPTY                  -308416
#define PD_NOVMA                       -308418
#define PD_NOVMAACT                    -308420
#define PD_CRMIDEMPTY                  -308422
#define PD_NOCRM                       -308424
#define PD_NOCRMACT                    -308426
#define PD_COLLACCEMPTY                -308428
#define PD_INTERACCEMPTY               -308430
#define PD_VOLADAYSEMPTY               -308432
#define PD_PVMMANYROWS                 -308434
#define PD_DOUBLEPVM                   -308436
#define PD_VOLCORRMISS                 -308438
#define PD_DOUBLEVMA                   -308440
#define PD_DOUBLECRM                   -308442
#define PD_XYMISSMATCH                 -308444
#define PD_VALNOTONE                   -308446
#define PD_REFTOPSR                    -308448
#define PD_XYTOOSMALL                  -308450
#define PD_INVTZTRANSITION             -308452
#define PD_NOOAT                       -308454
#define PD_NOOATACT                    -308456
#define PD_NOPQC                       -308458
#define PD_NOPQCACT                    -308460
#define PD_OATIDEMPTY                  -308462
#define PD_DOUBLEPQC                   -308464
#define PD_PQCIDEMPTY                  -308466
#define PD_EXTLARGERINT                -308468
#define PD_REFTOWIG                    -308470
#define PD_WIGIDEMPTY                  -308472
#define PD_NOWIG                       -308474
#define PD_NOWIGACT                    -308476
#define PD_DOUBLEWIG                   -308478
#define PD_REFTOTDG                    -308480
#define PD_TDGIDEMPTY                  -308482
#define PD_NOTDG                       -308484
#define PD_NOTDGACT                    -308486
#define PD_DOUBLETDG                   -308488
#define PD_RECTPOS                     -308490
#define PD_KEEPOSCHANGED               -308492
#define PD_GIVERECTEMPTY               -308494
#define PD_DOUBLEDRE                   -308496
#define PD_REFTODRE                    -308498
#define PD_HCTOUTRANGE                 -308500
#define PD_REFTOFRB                    -308502
#define PD_NEWCSTIDEMPTY               -308504
#define PD_INVSWIFTMEM                 -308506
#define PD_NOTMPLCST                   -308508
#define PD_DERIVLEVWRONG               -308510
#define PD_INSILLSUSPEND               -308512
#define PD_INVALIDTIME                 -308514
#define PD_SSTNOTEMPTY                 -308516
#define PD_ACTOLOTEMPTY                -308518
#define PD_LOTTYPEEMPTY                -308520
#define PD_LONORDACTEMPTY              -308522
#define PD_DOUBLELOTTYPE               -308524
#define PD_MAXLOTEXCEED                -308526
#define PD_ODDLOTNOTONE                -308528
#define PD_ROUNDNOTFOUND               -308530
#define PD_ROUNDLTODD                  -308532
#define PD_ROUNDMISSING                -308534
#define PD_BLOCKNOTMULT                -308536
#define PD_DATEFEMPTY                  -308538
#define PD_DATEFNOTEMPTY               -308540
#define PD_TRADINGCEXIST               -308542
#define PD_GROUPEXISTASCOMBO           -308544
#define PD_COMBOEXISTASGROUP           -308546
#define PD_NORMALNOTMULT               -308548
#define PD_DELTAVALUEEMPTY             -308550
#define PD_HALFSTEPSEMPTY              -308552
#define PD_ENABLEDELTAWRONG            -308554
#define PD_ENABLEHALFWRONG             -308556
#define PD_HALFABOVEEXCABOVE           -308558
#define PD_HALFBELOWEXCBELOW           -308560
#define PD_FTTIMENOTEMPTY              -308562
#define PD_FTTIMEEMPTY                 -308564
#define PD_LTTIMENOTEMPTY              -308566
#define PD_LTTIMEEMPTY                 -308568
#define PD_FTDATEBEFORECRE             -308570
#define PD_FTDMOVBACK                  -308572
#define PD_FTDMOVFORW                  -308574
#define PD_TRADEDINCLICKWRONG          -308576
#define PD_CLASSISTRADEDCLK            -308578
#define PD_INTNOTTRADEDCLK             -308580
#define PD_SERISTRADEDCLK              -308582
#define PD_INCNOTTRADEDCLK             -308584
#define PD_INSNOTYETTRADED             -308586
#define PD_FIRSTDATEAFTER              -308588
#define PD_FIRSTTIMEAFTER              -308590
#define PD_MAXDIFFLASTEXCEED           -308592
#define PD_MAXDIFFFIRSTEXCEED          -308594
#define PD_PRICEISMISSING              -308596
#define PD_MORETHANONELEVELSPEC        -308598
#define PD_SIGNALPARTLY                -308600
#define PD_SPECIFYONELEVEL             -308602
#define PD_RCLASSEMPTY                 -308604
#define PD_RCADJEMPTY                  -308606
#define PD_REFTORSP                    -308608
#define PD_DOUBLERRC                   -308610
#define PD_NOVALIDRNKCLASS             -308612
#define PD_LTMIDEMPTY                  -308614
#define PD_NOLTM                       -308616
#define PD_NOLTMACT                    -308618
#define PD_NOLTMFOUND                  -308620
#define PD_INVALIDRRC                  -308622
#define PD_DOUBLERUP                   -308624
#define PD_OPTVAREMPTY                 -308626
#define PD_KNOCKVAREMPTY               -308628
#define PD_BINARYVAREMPTY              -308630
#define PD_SWAPSTYLEEMPTY              -308632
#define PD_FORWSTYLEEMPTY              -308634
#define PD_FORWSTYLEWRONG              -308636
#define PD_SWAPSTYLEWRONG              -308638
#define PD_KNOCKVARWRONG               -308640
#define PD_BINARYVARWRONG              -308642
#define PD_TRROVERMAX                  -308644
#define PD_REFTOLTM                    -308646
#define PD_MARIDXDIFF                  -308648
#define PD_LTMIDXDIFF                  -308650
#define PD_NOVIRTCOM                   -308652
#define PD_NOVIRTCOMACT                -308654
#define PD_NOLINKCOM                   -308656
#define PD_NOLINKCOMACT                -308658
#define PD_NOVSC                       -308660
#define PD_NOVSCACT                    -308662
#define PD_NOPID                       -308664
#define PD_NOPIDACT                    -308666
#define PD_DOUBLECBC                   -308668
#define PD_BOTHINGCBG                  -308670
#define PD_REFTOSTS                    -308672
#define PD_SECCODEEMPTY                -308674
#define PD_CRPCODEEMPTY                -308676
#define PD_PIDIDEMPTY                  -308678
#define PD_PARTALREXISTS               -308680
#define PD_REFTOCPA                    -308682
#define PD_COMINCINSEMPTY              -308684
#define PD_COMINCINSNOTEMPTY           -308686
#define PD_STARTNOTVALID               -308688
#define PD_ENDBEFORESTART              -308690
#define PD_RECORDBEFORESTART           -308692
#define PD_AFFECTLINKWRONG             -308694
#define PD_NOCAC                       -308696
#define PD_NOCACACT                    -308698
#define PD_MINSIZEWRONG                -308700
#define PD_MAXSIZEWRONG                -308702
#define PD_INVOBLEVELBIG               -308704
#define PD_INVMINIOBLEVELBIG           -308706
#define PD_SESSNOEMPTY                 -308708
#define PD_DOUBLESESSNO                -308710
#define PD_WRONGVIRTUAL                -308712
#define PD_LINKISSAME                  -308714
#define PD_LINKISLINKED                -308716
#define PD_COMISLINKED                 -308718
#define PD_DOUBLECRP                   -308720
#define PD_VIRTINCNOTALLWD             -308722
#define PD_VIRTCBCNOTALLWD             -308724
#define PD_VIRTININC                   -308726
#define PD_VIRTINCBC                   -308728
#define PD_EXRALREXIST                 -308730
#define PD_EXRCURTYPE                  -308732
#define PD_INDEXCOMEMPTY               -308734
#define PD_WRONGMARATTR                -308736
#define PD_WRONGINGATTR                -308738
#define PD_WRONGCBGATTR                -308740
#define PD_ABBRNOTUNIQUE               -308742
#define PD_STOCKCODENOTUNIQUE          -308744
#define PD_SEQNOTUNIQUE                -308746
#define PD_NOCOMCODELEFT               -308748
#define PD_STKHPNAMEEMPTY              -308750
#define PD_STKSHNAMEEMPTY              -308752
#define PD_STKWHNISSDTEEMPTY           -308754
#define PD_STKSECTYPEEMPTY             -308756
#define PD_STKEXPDATEEMPTY             -308758
#define PD_STKPAREMPTY                 -308760
#define PD_STKPARCUREMPTY              -308762
#define PD_STKLOTEMPTY                 -308764
#define PD_STKCOUEMPTY                 -308766
#define PD_STKTRDCUREMPTY              -308768
#define PD_STKMKTWRONG                 -308770
#define PD_STKISINEMPTY                -308772
#define PD_STKPARCDEEMPTY              -308774
#define PD_STKWIWRONG                  -308776
#define PD_STKSECTCEMPTY               -308778
#define PD_STKTRADEDWRONG              -308780
#define PD_STKCODEEMPTY                -308782
#define PD_STKIBMCDEEMPTY              -308784
#define PD_STKRMASTEREMPTY             -308786
#define PD_STKCMASTEREMPTY             -308788
#define PD_STKIMASTEREMPTY             -308790
#define PD_STKDMASTEREMPTY             -308792
#define PD_STKCMASTERWRONG             -308794
#define PD_STKIMASTERWRONG             -308796
#define PD_STKDMASTERWRONG             -308798
#define PD_STKINVSCHWRONG              -308800
#define PD_STKSECUATTRWRONG            -308802
#define PD_STKCHGIDENTEMPTY            -308804
#define PD_HP_NOACTION                  308805
#define PD_STKDELIDENTEMPTY            -308806
#define PD_STKAMENDCDEWRONG            -308808
#define PD_NOPLSTATE                   -308810
#define PD_WHNISSDTEWRONG              -308812
#define PD_NOWICODE                    -308814
#define PD_LINKNOTALLWD                -308816
#define PD_LINKISVIRTUAL               -308818
#define PD_IMMISSINSWRONG              -308820
#define PD_IMMISSCOMWRONG              -308822
#define PD_ISSENDDATEWRONG             -308824
#define PD_WARNINGINTWRONG             -308826
#define PD_ORDACTEMPTY                 -308828
#define PD_NOCORPLEV                   -308830
#define PD_IMMISSWRONG                 -308832
#define PD_CRPWRONG                    -308834
#define PD_CRPTYPEEMPTY                -308836
#define PD_MINGTMAX                    -308838
#define PD_REFTOSST                    -308840
#define PD_HIDDOATNOTALLWD             -308842
#define PD_HIDDCBTNOTALLWD             -308844
#define PD_MINVALGTMAX                 -308846
#define PD_MINVOLGTMAX                 -308848
#define PD_SETTLMACTEMPTY              -308850
#define PD_CRPCODEWRONG                -308852
#define PD_NOEXD                       -308854
#define PD_NOEXDACT                    -308856
#define PD_EXDIDEMPTY                  -308858
#define PD_REFTOEXD                    -308860
#define PD_DOUBLEEXCEPTDATE            -308862
#define PD_CHGATTREMPTY                -308864
#define PD_ACPNAMEEMPTY                -308866
#define PD_ACPIDEMPTY                  -308868
#define PD_DOUBLECLP                   -308870
#define PD_ONEDEFAULT                  -308872
#define PD_NODEFAULT                   -308874
#define PD_ALLCHARSET                  -308876
#define PD_REPCHAR                     -308878
#define PD_REQALPHA                    -308880
#define PD_REQDIGIT                    -308882
#define PD_REQEXT                      -308884
#define PD_REQDIC                      -308886
#define PD_STARTBEFIMPL                -308888
#define PD_ISSENDPASSED                -308890
#define PD_ILLEGTIME                   -308892
#define PD_NOASC                       -308894
#define PD_NOASCACT                    -308896
#define PD_ASCIDEMPTY                  -308898
#define PD_NODSTSST                    -308900
#define PD_NODSTSSTACT                 -308902
#define PD_DSTSSTIDEMPTY               -308904
#define PD_NOTRISST                    -308906
#define PD_NOTRISSTACT                 -308908
#define PD_TRISSTIDEMPTY               -308910
#define PD_DOUBLEDSTSST                -308912
#define PD_REFTOASC                    -308914
#define PD_NOMOVINS                    -308916
#define PD_NOMOVINSACT                 -308918
#define PD_ASCTIMEOUTEMPTY             -308920
#define PD_PRACTSECNOTALLWD            -308922
#define PD_TRDCURUNITEMPTY             -308924
#define PD_CURUNITEMPTY                -308926
#define PD_NOSLI                       -308928
#define PD_NOSLIACT                    -308930
#define PD_SLIIDEMPTY                  -308932
#define PD_REFTOSLI                    -308934
#define PD_DOUBLEMAR                   -308936
#define PD_REFTOUST                    -308938
#define PD_DOUBLEENDSTATE              -308940
#define PD_NOENDSTATE                  -308942
#define PD_WRONGCSTATTR                -308944
#define PD_WRONGOLDPASSW               -308946
#define PD_ILLOLDPASSW                 -308948
#define PD_ILLCROSSMARKET              -308950
#define PD_STKLOTEXCEEDED              -308952
#define PD_STKLOTNOTNUM                -308954
#define PD_STKPARNOTNUM                -308956
#define PD_STKRMASTERNOTNUM            -308958
#define PD_STKCMASTERNOTNUM            -308960
#define PD_STKIMASTERNOTNUM            -308962
#define PD_STKDMASTERNOTNUM            -308964
#define PD_REFNUMBZERO                 -308966
#define PD_REMOVEINTRNOTALLWD          -308968
#define PD_WRONGOWNMEM                 -308970
#define PD_COLDRESTEMPTY               -308972
#define PD_NOAGR                       -308974
#define PD_NOAGRACT                    -308976
#define PD_AGRIDEMPTY                  -308978
#define PD_REFTOAGR                    -308980
#define PD_DOUBLEAGR                   -308982
#define PD_BOTHAGRCLASS                -308984
#define PD_NONPRACTSECNOTALLWD         -308986
#define PD_PRACTSECMIXNOTALLWD         -308988
#define PD_PASSWCHSETEMPTY             -308990
#define PD_ILLSSSSTART                 -308992
#define PD_ILLSSSTIMESEQ               -308994
#define PD_COMLINKILLSTATUS            -308996
#define PD_LINKEDDELISTED              -308998
#define PD_TRDDISSEMPTY                -309000
#define PD_MAXEXTCOUPON                -309002
#define PD_OPIFSELLWRONG               -309004
#define PD_OPPIDEMPTY                  -309006
#define PD_NOOPP                       -309008
#define PD_NOOPPACT                    -309010
#define PD_DIOIDEMPTY                  -309012
#define PD_NODIO                       -309014
#define PD_NODIOACT                    -309016
#define PD_BLKCROIDEMPTY               -309018
#define PD_DOUBLEPOD                   -309020
#define PD_ORIGINNOTSPEC               -309022
#define PD_NOLSS                       -309024
#define PD_NOLSSACT                    -309026
#define PD_NOSLP                       -309028
#define PD_NOSLPACT                    -309030
#define PD_LSSNOTEMPTY                 -309032
#define PD_LSSIDEMPTY                  -309034
#define PD_DOUBLESST                   -309036
#define PD_SLPIDEMPTY                  -309038
#define PD_REFTOLSS                    -309040
#define PD_REFTOICS                    -309042
#define PD_DOUBLEREX                   -309044
#define PD_ACCTIDEMPTY                 -309046
#define PD_ACCTIDNOTEMPTY              -309048
#define PD_OBJIDEMPTY                  -309050
#define PD_OBJTYPEWRONG                -309052
#define PD_SHRTSPSIDEMPTY              -309054
#define PD_SHRTSRUIDEMPTY              -309056
#define PD_NOSHRTSPS                   -309058
#define PD_NOSHRTSPSACT                -309060
#define PD_NOSHRTSRU                   -309062
#define PD_NOSHRTSRUACT                -309064
#define PD_NOAQR                       -309066
#define PD_NOAQRACT                    -309068
#define PD_AQRIDEMPTY                  -309070
#define PD_AQRFREQWRONG                -309072
#define PD_NOOFEXPWRONG                -309074
#define PD_NOOFOTMWRONG                -309076
#define PD_NOOFITMWRONG                -309078
#define PD_MDCINTERVWRONG              -309080
#define PD_AQRSTARTWRONG               -309082
#define PD_AQRSTARTEXCEEDED            -309084
#define PD_AQRINTERVWRONG              -309086
#define PD_AQRINTERVEXCEEDED           -309088
#define PD_NOOFCYCLEWRONG              -309090
#define PD_AQRCYCLEWAITWRONG           -309092
#define PD_AQRCYCLEWAITEXCEEDED        -309094
#define PD_EXQRTIMEWRONG               -309096
#define PD_AQRCHECKWRONG               -309098
#define PD_REFTOCEG                    -309100
#define PD_SECLENDTYPEEMPTY            -309102
#define PD_NORPG                       -309104
#define PD_NORPGACT                    -309106
#define PD_REFTOBNK                    -309108
#define PD_RPTVISIBEMPTY               -309110
#define PD_RPTFILEPREFEMPTY            -309112
#define PD_RPTFILEEXTEMPTY             -309114
#define PD_RPTSCHEDEMPTY               -309116
#define PD_RPTTYPEEMPTY                -309118
#define PD_RTMIDEMPTY                  -309120
#define PD_PARAMNOEMPTY                -309122
#define PD_PARAMNAMEEMPTY              -309124
#define PD_PARAMTYPEEMPTY              -309126
#define PD_REFTORTM                    -309128
#define PD_PARAMINPVEMPTY              -309130
#define PD_DOUBLEPARAM                 -309132
#define PD_WRONGPARAM                  -309134
#define PD_EXCPARAMMISSING             -309136
#define PD_RPTIDEMPTY                  -309138
#define PD_RPTEVENTEMPTY               -309140
#define PD_NORTM                       -309142
#define PD_NORTMACT                    -309144
#define PD_NORPT                       -309146
#define PD_NORPTACT                    -309148
#define PD_RPTWHENEMPTY                -309150
#define PD_REFTORPT                    -309152
#define PD_RPTSPECNOTUNIQUE            -309154
#define PD_RPTSPECDESCEMPTY            -309156
#define PD_RPTSUBSYSEMPTY              -309158
#define PD_RPTSUBSYSNOTEMPTY           -309160
#define PD_RDTIDEMPTY                  -309162
#define PD_RDTIDNOTEMPTY               -309164
#define PD_PRDIDNOTEMPTY               -309166
#define PD_DOUBLERPI                   -309168
#define PD_RPTPRODWRONG                -309170
#define PD_RPGIDEMPTY                  -309172
#define PD_REFTORPG                    -309174
#define PD_BOTHRPGREPSPEC              -309176
#define PD_RPGINRPG                    -309178
#define PD_DOUBLERPG                   -309180
#define PD_RDTTIMEEMPTY                -309182
#define PD_RDTDAYNOEMPTY               -309184
#define PD_RDTBOTHTIMEDEF              -309186
#define PD_DOUBLERDT                   -309188
#define PD_REFTORDT                    -309190
#define PD_NORDT                       -309192
#define PD_NORDTACT                    -309194
#define PD_NOTAUTHORIZEDCLH            -309196
#define PD_REFTOCSB                    -309198
#define PD_CLHISCHANGED                -309200
#define PD_NAMEDWRONG                  -309202
#define PD_WRONGPARAM2                 -309204
#define PD_OTTIMEEMPTY                 -309206
#define PD_RDTISEMPTY                  -309208
#define PD_ACTONEMPTY                  -309210
#define PD_ACTONWRONG                  -309212
#define PD_WRONGVALIDUNTIL             -309214
#define PD_DOUBLELTT                   -309216
#define PD_LTTUNLIMNOTALLWD            -309218
#define PD_LTTOVERLAPPING              -309220
#define PD_WRLTTTIMENONE               -309222
#define PD_SHRTTERMUNITEMPTY           -309224
#define PD_RTMFILENOTUNIQUE            -309226
#define PD_REFTOCBA                    -309228
#define PD_NOREFPRICE                  -309230
#define PD_ERROPENFILE                 -309232
#define PD_NOOFINSWRONG                -309234
#define PD_WRONGCLASSTXQ               -309236
#define PD_WRONGCLASSTXR               -309238
#define PD_EXCIDNOTREQ                 -309240
#define PD_EXCIDREQFORTDG              -309242
#define PD_TDPTDGEMPTY                 -309244
#define PD_CONVSERIESEXIST             -309246
#define PD_BOTHINTABT                  -309248
#define PD_REFTOABT                    -309250
#define PD_NOABT                       -309252
#define PD_NOABTACT                    -309254
#define PD_ABTIDEMPTY                  -309256
#define PD_INVMDSTARTEND               -309258
#define PD_CLEARDAYSEMPTY              -309260
#define PD_ERRCLOSECLEARING            -309262
#define PD_ERROPENTRADING              -309264
#define PD_REFTOMPR                    -309266
#define PD_MPRIDEMPTY                  -309268
#define PD_NOMPR                       -309270
#define PD_NOMPRACT                    -309272
#define PD_DOUBLECST                   -309274
#define PD_PPPERCEMPTY                 -309276
#define PD_PPPERCEXCEEDED              -309278
#define PD_MM1PERCEMPTY                -309280
#define PD_MM2PERCEMPTY                -309282
#define PD_MM3PERCEMPTY                -309284
#define PD_MM4PERCEMPTY                -309286
#define PD_MM5PERCEMPTY                -309288
#define PD_MM1PERCEXCEEDED             -309290
#define PD_MMALLWARNEMPTY              -309292
#define PD_MM2PERCEXCEEDED             -309294
#define PD_MM3PERCEXCEEDED             -309296
#define PD_MM4PERCEXCEEDED             -309298
#define PD_MM5PERCEXCEEDED             -309300
#define PD_MMALLWARNEXCEEDED           -309302
#define PD_RANKEXCEEDED                -309304
#define PD_RANKEMPTY                   -309306
#define PD_MATCHPRIONOTALLWD           -309308
#define PD_INVALIDVERSION              -309310
#define PD_WCCALRCHILD                 -309312
#define PD_HOLDINGNOTALLWD             -309314
#define PD_NOWRACCESS                  -309316
#define PD_COUNTERSNOTALLWD            -309318
#define PD_ALRCOUNTERSIGNED            -309320
#define PD_NOCEG                       -309322
#define PD_NOCPA                       -309324
#define PD_NOCSA                       -309326
#define PD_NOSEC                       -309328
#define PD_NODLV                       -309330
#define PD_NODRE                       -309332
#define PD_NOICO                       -309334
#define PD_NOISN                       -309336
#define PD_NOISS                       -309338
#define PD_NOITU                       -309340
#define PD_NOMNA                       -309342
#define PD_NOPQR                       -309344
#define PD_COUNTERSIMMNOTALLWD         -309346
#define PD_DELHOLDINGNOTALLWD          -309348
#define PD_COUNTERSINSNOTALLWD         -309350
#define PD_NOCDC                       -309352
#define PD_NOGCG                       -309354
#define PD_UPDHOLDINGDELNOTALLWD       -309356
#define PD_NEWINSISWRONG               -309358
#define PD_SOINSISWRONG                -309360
#define PD_REFTOCBG                    -309362
#define PD_ACCCOMBOFIRST               -309364
#define PD_NOSTY                       -309366
#define PD_NOSTYACT                    -309368
#define PD_STYIDEMPTY                  -309370
#define PD_NOPTM                       -309372
#define PD_NOPTMACT                    -309374
#define PD_PTMIDEMPTY                  -309376
#define PD_FROMEXPEMPTY                -309378
#define PD_STPMULTEMPTY                -309380
#define PD_DOUBLEFROMEXP               -309382
#define PD_MULTIPLEWRONG               -309384
#define PD_FROMEXP1MISSING             -309386
#define PD_WRONGCUSTATTRRULE           -309388
#define PD_WRONGACCATTRRULE            -309390
#define PD_WRONGREADATTRRULE           -309392
#define PD_WRONGDIARYATTRRULE          -309394
#define PD_WRONGATYATTRRULE            -309396
#define PD_WRONGAFTATTRRULE            -309398
#define PD_WRONGCSBATTRRULE            -309400
#define PD_WRONGCCCSTATTRRULE          -309402
#define PD_WRONGANETATTRRULE           -309404
#define PD_WRONGCURATTRRULE            -309406
#define PD_WRONGCURCONVATTRRULE        -309408
#define PD_WRONGRMNATTRRULE            -309410
#define PD_WRONGINVTATTRRULE           -309412
#define PD_WRONGNATIONATTRRULE         -309414
#define PD_WRONGACCTEXTATTRRULE        -309416
#define PD_WRONGEXTACCATTRRULE         -309418
#define PD_WRONGFORHOLDATTRRULE        -309420
#define PD_BOTHINTMAR                  -309422
#define PD_BOTHINTMAREMPTY             -309424
#define PD_NODREACT                    -309426
#define PD_DREIDEMPTY                  -309428
#define PD_NORST                       -309430
#define PD_NORSTACT                    -309432
#define PD_RSTIDEMPTY                  -309434
#define PD_DOUBLERST                   -309436
#define PD_LTDAFTEREXP                 -309438
#define PD_LTDAFTEREFFEXP              -309440
#define PD_PREMUNITNOTEQ               -309442
#define PD_CURRUNITNOTEQ               -309444
#define PD_REFTOSTY                    -309446
#define PD_ZEROFIXEMPTY                -309448
#define PD_NOSCR                       -309450
#define PD_NOSCRACT                    -309452
#define PD_NOMCO                       -309454
#define PD_NOMCOACT                    -309456
#define PD_DOUBLEATY                   -309458
#define PD_REFTOMCO                    -309460
#define PD_WRONGNDSCRPARAM             -309462
#define PD_WRONGLTDSCRPARAM            -309464
#define PD_WRONGMCOACCOUNT             -309466
#define PD_MCOIDEMPTY                  -309468
#define PD_SCRONLYFOROPT               -309470
#define PD_NOMBS                       -309472
#define PD_NOMBSACT                    -309474
#define PD_MBSIDEMPTY                  -309476
#define PD_REFTOMBS                    -309478
#define PD_STKMBSEMPTY                 -309480
#define PD_ULRIDEMPTY                  -309482
#define PD_LOWCOMEMPTY                 -309484
#define PD_UPPCOMEMPTY                 -309486
#define PD_STOCKFROMEMPTY              -309488
#define PD_STOCKTOEMPTY                -309490
#define PD_LOWERCOMGEUPPER             -309492
#define PD_DOUBLESEQNO                 -309494
#define PD_FROMNOTALNUM                -309496
#define PD_TONOTALNUM                  -309498
#define PD_NOALNUMINSTOCK              -309500
#define PD_ULRNOTFOUND                 -309502
#define PD_REFTOULR                    -309504
#define PD_SEQNOEMPTY                  -309506
#define PD_NOULR                       -309508
#define PD_NOVOM                       -309510
#define PD_NOVOMACT                    -309512
#define PD_VOMIDEMPTY                  -309514
#define PD_OLDINSISCHANGED             -309516
#define PD_NEWINSISCHANGED             -309518
#define PD_SOINSISCHANGED              -309520
#define PD_MINIINCIDEMPTY              -309522
#define PD_NETRATIOEMPTY               -309524
#define PD_NETTINGINCALREADYEXIST      -309526
#define PD_INCALREADYEXIST             -309528
#define PD_NOCPNCST                    -309530
#define PD_NOCPNCSTACT                 -309532
#define PD_CPNCSTIDEMPTY               -309534
#define PD_CPNEXCIDEMPTY               -309536
#define PD_INCNETTINGSAME              -309538
#define PD_REFTOCPN                    -309540
#define PD_ACTIVEINSEXIST              -309542
#define PD_NOITEMFOUND                 -309544
#define PD_COMIDWRONG                  -309546
#define PD_NEWCOMIDEMPTY               -309548
#define PD_INCIDWRONG                  -309550
#define PD_TDPIDWRONG                  -309552
#define PD_PURIDWRONG                  -309554
#define PD_PURNOTEQ                    -309556
#define PD_POSIDEMPTY                  -309558
#define PD_NOCPN                       -309560
#define PD_KEEPOSHANDLCHANGED          -309562
#define PD_CSTINTSUSP                  -309564
#define PD_INVONOFFSUSP                -309566
#define PD_SKEWUPISWRONG               -309568
#define PD_SKEWDOWNISWRONG             -309570
#define PD_INVPRIORITY                 -309572
#define PD_SPREADUNITEMPTY             -309574
#define PD_ILLSESSALLOWED              -309576
#define PD_ACCMANUSER                  -309578
#define PD_USTALREADYRESUMED           -309580
#define PD_USTALREADYSUSP              -309582
#define PD_INVSUSPENDRESUME            -309584
#define PD_VAGIDEMPTY                  -309586
#define PD_NOVAG                       -309588
#define PD_NOVAGACT                    -309590
#define PD_USGIDEMPTY                  -309592
#define PD_NOUSG                       -309594
#define PD_NOUSGACT                    -309596
#define PD_INVADDUSG                   -309598
#define PD_INVDELUSG                   -309600
#define PD_MANTRDUSG                   -309602
#define PD_SLCIDEMPTY                  -309604
#define PD_NOSLC                       -309606
#define PD_NOSLCACT                    -309608
#define PD_WRONGOWNUSG                 -309610
#define PD_INVDAYCOUNTCONV             -309612
#define PD_INVBUSINESSDAYCONV          -309614
#define PD_INVRESETDAYTYPE             -309616
#define PD_INVPAYMENTSET               -309618
#define PD_ROLLOVERDATECALCFAILED      -309620
#define PD_INVFIXETFLOAYVALUE          -309622
#define PD_INVNBROFSWAPLEGS            -309624
#define PD_INVNBROFFRALEGS             -309626
#define PD_INVROLLOVERPERIOD           -309628
#define PD_INVSETTLEMENTDAYS           -309630
#define PD_PAYINADVANCENOTIRS          -309632
#define PD_INVSETTLEMENTDAYSADVANCE    -309634
#define PD_INVTRONOTRADED              -309636
#define PD_INVTCXTNOTRADED             -309638
#define PD_INVTRONOTRADEDCXT           -309640
#define PD_MANRESETINS                 -309642
#define PD_MANRESETHOLIDAY             -309644
#define PD_INVRESETDAYS                -309646
#define PD_NOTEMPTYRESETINS            -309648
#define PD_NOTEMPTYRESETHOLIDAY        -309650
#define PD_NOTZERORESETDAYS            -309652
#define PD_NOTEMPTYRESETTYPE           -309654
#define PD_INVLEGNO                    -309656
#define PD_INVSLCITEMS                 -309658
#define PD_SETTLHOLEMPTY               -309660
#define PD_NEGSETTLEMENTDAYS           -309662
#define PD_INVSLCGROUP                 -309664
#define PD_RESETHOLIDAYNOTFOUND        -309666
#define PD_SETTLEHOLIDAYNOTFOUND       -309668
#define PD_INVLEGNBRSWAP               -309670
#define PD_INVSWPSTYLCR                -309672
#define PD_INVLEGNBRFRA                -309674
#define PD_INVLEGTYPEFRA               -309676
#define PD_AIHIDEMPTY                  -309678
#define PD_REFTOAIH                    -309680
#define PD_NOAIH                       -309682
#define PD_NOAIHACT                    -309684
#define PD_REFTOVAG                    -309686
#define PD_CVREMPTY                    -309688
#define PD_NORFR                       -309690
#define PD_NORFRACT                    -309692
#define PD_RFRIDEMPTY                  -309694
#define PD_DETERMDATEEMPTY             -309696
#define PD_FROMDATEEMPTY               -309698
#define PD_RATEEMPTY                   -309700
#define PD_REFTORFR                    -309702
#define PD_DOUBLERFR                   -309704
#define PD_NOLOT                       -309706
#define PD_NOLOTACT                    -309708
#define PD_LOTIDEMPTY                  -309710
#define PD_LOTTERYDATEEMPTY            -309712
#define PD_PAYOUTDATEEMPTY             -309714
#define PD_REFTOLOT                    -309716
#define PD_DOUBLELOT                   -309718
#define PD_STFIDEMPTY                  -309720
#define PD_NOSTF                       -309722
#define PD_NOSTFACT                    -309724
#define PD_SFIIDEMPTY                  -309726
#define PD_NOSFI                       -309728
#define PD_NOSFIACT                    -309730
#define PD_CONVFROMEMPTY               -309732
#define PD_CONVTHROUGHEMPTY            -309734
#define PD_DOUBLECON                   -309736
#define PD_NOLEADCST                   -309738
#define PD_NOLEADCSTACT                -309740
#define PD_NOARRCST                    -309742
#define PD_NOARRCSTACT                 -309744
#define PD_NOCORRVPT                   -309746
#define PD_NOCORRVPTACT                -309748
#define PD_SSSTYPEEMPTY                -309750
#define PD_REFTOCIS                    -309752
#define PD_BOTHPRIONOTALLWD            -309754
#define PD_NOIAOSSS                    -309756
#define PD_NOIAOSSSACT                 -309758
#define PD_NOIACSSS                    -309760
#define PD_NOIACSSSACT                 -309762
#define PD_NOBBAOSSS                   -309764
#define PD_NOBBAOSSSACT                -309766
#define PD_NOBBACSSS                   -309768
#define PD_NOBBACSSSACT                -309770
#define PD_NOFIXSSS                    -309772
#define PD_NOFIXSSSACT                 -309774
#define PD_CREINCNOTALLWD              -309776
#define PD_TMPLCOMEMPTY                -309778
#define PD_NOFALLBSSS                  -309780
#define PD_NOFALLBSSSACT               -309782
#define PD_EXTINFSOURCEWRONG           -309784
#define PD_UNDERSCOREATEND             -309786
#define PD_RPTROWEMPTY                 -309788
#define PD_EXTPMLOGEMPTY               -309790
#define PD_EXTPOBLOGEMPTY              -309792
#define PD_EXTPOUTPLOGEMPTY            -309794
#define PD_EXTPPRCCALCEMPTY            -309796
#define PD_EXTPMSGVALEMPTY             -309798
#define PD_EXTPRECVALEMPTY             -309800
#define PD_EXTPOBEMPTY                 -309802
#define PD_EXTPMATCHEMPTY              -309804
#define PD_EXTPOBPROCEMPTY             -309806
#define PD_EXTPAUCHEMPTY               -309808
#define PD_EXTPCBCALCEMPTY             -309810
#define PD_EXTPSESSCHGEMPTY            -309812
#define PD_CBSIDEMPTY                  -309814
#define PD_TOFAGREEGRANEMPTY           -309816
#define PD_PGRIDEMPTY                  -309818
#define PD_NOPPL                       -309820
#define PD_NOPPLACT                    -309822
#define PD_PPLIDEMPTY                  -309824
#define PD_NOPLR                       -309826
#define PD_NOPLRACT                    -309828
#define PD_PLRIDEMPTY                  -309830
#define PD_REFTOPLR                    -309832
#define PD_BOTHPLRPRL                  -309834
#define PD_DOUBLEPLR                   -309836
#define PD_MAXQTYWRONG                 -309838
#define PD_MAXVALWRONG                 -309840
#define PD_MAXQTYEMPTY                 -309842
#define PD_MAXVALEMPTY                 -309844
#define PD_DOUBLEABT                   -309846
#define PD_NOTAUTHORIZEDINS            -309848
#define PD_NOOFADDSEREXCEED            -309850
#define PD_FEECOFFNOTALLWD             -309852
#define PD_EXTSETTLTYPEMPTY            -309854
#define PD_STARTRULESPEC               -309856
#define PD_ENDRULESPEC                 -309858
#define PD_NOENDWHNSTART               -309860
#define PD_LTDTOCLOSMATUR              -309862
#define PD_NOTTI                       -309864
#define PD_NOTTIACT                    -309866
#define PD_TTIIDEMPTY                  -309868
#define PD_REFTOTTI                    -309870
#define PD_REFTOMMP                    -309872
#define PD_NOMMP                       -309874
#define PD_INVQPROTVALUE               -309876
#define PD_INVDPROTVALUE               -309878
#define PD_QPROTVALUETOOLOW            -309880
#define PD_DPROTVALUETOOLOW            -309882
#define PD_INVINCLFUTURES              -309884
#define PD_RESUMENOTALLWD              -309886
#define PD_SUSPRESUMENOTALLWD          -309888
#define PD_LOGOUTNOTALLWD              -309890
#define PD_SETMAXLIMITNOTALLWD         -309892
#define PD_NODTI                       -309894
#define PD_NODTIACT                    -309896
#define PD_DTIIDEMPTY                  -309898
#define PD_NODFP                       -309900
#define PD_NODFPACT                    -309902
#define PD_DFPIDEMPTY                  -309904
#define PD_DOUBLEDFP                   -309906
#define PD_WRONGTIMEDELAY              -309908
#define PD_VOLATILITYEMPY              -309910
#define PD_INVMARMIN                   -309912
#define PD_MINSPREADGTMAX              -309914
#define PD_REFTOPGR                    -309916
#define PD_DWWRONGSUM                  -309918
#define PD_PCPOINTEXCEED               -309920
#define PD_NOPGR                       -309922
#define PD_NOPGRACT                    -309924
#define PD_NORSC                       -309926
#define PD_NORSCACT                    -309928
#define PD_RSCIDEMPTY                  -309930
#define PD_REFTORSC                    -309932
#define PD_PMAIDEMPTY                  -309934
#define PD_REFTOPMA                    -309936
#define PD_NOPMA                       -309938
#define PD_NOPMAACT                    -309940
#define PD_CFMIDEMPTY                  -309942
#define PD_REFTOCFM                    -309944
#define PD_NOCFM                       -309946
#define PD_NOCFMACT                    -309948
#define PD_PRCRVIDEMPTY                -309950
#define PD_DOUBCRVDEF                  -309952
#define PD_NOCRV                       -309954
#define PD_REFTOCRV                    -309956
#define PD_CRVIDEMPTY                  -309958
#define PD_NOCRVACT                    -309960
#define PD_NOCRDACT                    -309962
#define PD_NOPCO                       -309964
#define PD_REFTOPCO                    -309966
#define PD_PCOIDEMPTY                  -309968
#define PD_NOPCOACT                    -309970
#define PD_PCYRSEMPTY                  -309972
#define PD_DOUBLEPCO                   -309974
#define PD_NOUPPCCC                    -309976
#define PD_NOUPPCCCACT                 -309978
#define PD_OVERLPCNOTODD               -309980
#define PD_NOCCC                       -309982
#define PD_CIRCDEPCCCNOTALLOWED        -309984
#define PD_REFTOCCC                    -309986
#define PD_CCCIDEMPTY                  -309988
#define PD_NOCCCACT                    -309990
#define PD_MARGMETHEMPTY               -309992
#define PD_PC1NOMINORMAX               -309994
#define PD_PC2NOMINORMAX               -309996
#define PD_PC3NOMINORMAX               -309998
#define PD_STARTENDBOTHORNONE          -310000
#define PD_SHORTNUMEXCEEDED            -310002
#define PD_REFTOSTF                    -310004
#define PD_DERIVFROMEMPTY              -310006
#define PD_DOUBLESTF                   -310008
#define PD_NOTOP                       -310010
#define PD_NOTOPACT                    -310012
#define PD_REFTOTOP                    -310014
#define PD_TOPIDEMPTY                  -310016
#define PD_DOUBLETOL                   -310018
#define PD_TOLIDEMPTY                  -310020
#define PD_LISTTYPEEMPTY               -310022
#define PD_SORTITEMEMPTY               -310024
#define PD_NOTOL                       -310026
#define PD_REFTOTOL                    -310028
#define PD_LISTHDREMPTY                -310030
#define PD_EXPTIMENEG                  -310032
#define PD_FROZENTIMENEG               -310034
#define PD_MAXEXPLTMINEXP              -310036
#define PD_NOSTRKUPWHNCALIB            -310038
#define PD_NOSTRKDWNWHNCALIB           -310040
#define PD_NOSTRKEXTWHNCALIB           -310042
#define PD_NOMINEXPWHNCALIB            -310044
#define PD_NOMAXEXPWHNCALIB            -310046
#define PD_TTCBEMPTY                   -310048
#define PD_TTLEGEMPTY                  -310050
#define PD_REPOLTDISPASSED             -310052
#define PD_FROMTOLISTED                -310054
#define PD_NOSTA                       -310056
#define PD_NOSTAACT                    -310058
#define PD_STAIDEMPTY                  -310060
#define PD_DOUBLESTA                   -310062
#define PD_CSDEMPTY                    -310064
#define PD_CSDIDENTEMPTY               -310066
#define PD_NOEMB                       -310068
#define PD_NOEMBACT                    -310070
#define PD_EMBIDEMPTY                  -310072
#define PD_EXCMEMTYPEEMPTY             -310074
#define PD_DOUBLEEMB                   -310076
#define PD_REFTOEMB                    -310078
#define PD_REFTOSLC                    -310080
#define PD_NOPRSOURCEINS               -310082
#define PD_AUTOFIXEMPTY                -310084
#define PD_NOPRSOURCEINSACT            -310086
#define PD_NOMRG                       -310088
#define PD_NOMRGACT                    -310090
#define PD_NOFIX                       -310092
#define PD_MRGIDEMPTY                  -310094
#define PD_NOFIXACT                    -310096
#define PD_REFTOMRG                    -310098
#define PD_NOPPR                       -310100
#define PD_FIXIDEMPTY                  -310102
#define PD_NOPPRACT                    -310104
#define PD_DOUBLEFIX                   -310106
#define PD_PPRIDEMPTY                  -310108
#define PD_APPIDEMPTY                  -310110
#define PD_REFTOPPR                    -310112
#define PD_TARGCOMPEMPTY               -310114
#define PD_NODHG                       -310116
#define PD_PORTNUMEMPTY                -310118
#define PD_NODHGACT                    -310120
#define PD_FIXVEREMPTY                 -310122
#define PD_DHGIDEMPTY                  -310124
#define PD_CONNTYPEEMPTY               -310126
#define PD_REFTODHG                    -310128
#define PD_DROPFILTEREMPTY             -310130
#define PD_NOOMS                       -310132
#define PD_DROPFILTERNOTEMPTY          -310134
#define PD_NOOMSACT                    -310136
#define PD_DOUBLEULR                   -310138
#define PD_OMSIDEMPTY                  -310140
#define PD_USECOMPRWRONG               -310142
#define PD_REFTOOMS                    -310144
#define PD_RESETSEQWRONG               -310146
#define PD_NOEXR                       -310148
#define PD_WRONGABTATTR                -310150
#define PD_NOEXRACT                    -310152
#define PD_BOTHABTCLASS                -310154
#define PD_EXRIDEMPTY                  -310156
#define PD_SECIDEMPTY                  -310158
#define PD_REFTOEXR                    -310160
#define PD_ISINEMPTY                   -310162
#define PD_NOCCO                       -310164
#define PD_STFALREXISTS                -310166
#define PD_NOCCOACT                    -310168
#define PD_LONGFREEEMPTY               -310170
#define PD_CCOIDEMPTY                  -310172
#define PD_LISTDATEEMPTY               -310174
#define PD_REFTOCCO                    -310176
#define PD_ISSPRICEEMPTY               -310178
#define PD_NOCCG                       -310180
#define PD_NOITEMS                     -310182
#define PD_NOCCGACT                    -310184
#define PD_LONGINSEXIST                -310186
#define PD_CCGIDEMPTY                  -310188
#define PD_INSIDEXIST                  -310190
#define PD_REFTOCCG                    -310192
#define PD_NOTIE                       -310194
#define PD_NOTIEACT                    -310196
#define PD_TIEIDEMPTY                  -310198
#define PD_REFTOTIE                    -310200
#define PD_NOIRM                       -310202
#define PD_NOIRMACT                    -310204
#define PD_IRMIDEMPTY                  -310206
#define PD_REFTOIRM                    -310208
#define PD_NOWRM                       -310210
#define PD_NOWRMACT                    -310212
#define PD_WRMIDEMPTY                  -310214
#define PD_REFTOWRM                    -310216
#define PD_MINMATGTMAXMAT              -310218
#define PD_TIERNOEMPTY                 -310220
#define PD_DOUBLECCG                   -310222
#define PD_DATENUMBERNOTEMPTY          -310224
#define PD_MAXNUMBEREMPTY              -310226
#define PD_MAXDATEEMPTY                -310228
#define PD_NOTDPITEM                   -310230
#define PD_CCO1OR2EMPTY                -310232
#define PD_INVALIDVOLA                 -310234
#define PD_HIGHMAXEMPTY                -310236
#define PD_DAYSYEAREMPTY               -310238
#define PD_POINTSNOTODD                -310240
#define PD_PASSEDMAXDATE               -310242
#define PD_CIRCDEPWRMNOTALLOWED        -310244
#define PD_CCONOTCCG                   -310246
#define PD_POINTSPCNOTODD              -310248
#define PD_MAXDATENOTEMPTY             -310250
#define PD_CRERSKFILEEMPTY             -310252
#define PD_CREVECFILEEMPTY             -310254
#define PD_DOUBLEMRG                   -310256
#define PD_DISCMETHEMPTY               -310258
#define PD_NOCURVDEFITEM               -310260
#define PD_PRIONOTZERO                 -310262
#define PD_PRADOUBLEDEF                -310264
#define PD_PRAOTHERINT                 -310266
#define PD_PRACODEEXIST                -310268
#define PD_PRAINTINMARKET              -310270
#define PD_PRANOCODE                   -310272
#define PD_NOPRA                       -310274
#define PD_NOPRAACT                    -310276
#define PD_PRAIDEMPTY                  -310278
#define PD_REFTOPRA                    -310280
#define PD_PRAOTHERMAR                 -310282
#define PD_PRADESCRNOTUNIQUE           -310284
#define PD_PRACODENOTUNIQUE            -310286
#define PD_PUBYLDCRVEMPTY              -310288
#define PD_CRERSKCUBEMPTY              -310290
#define PD_INTRTYPENOTRIGHT            -310292
#define PD_MONTHLYSETTLNOTALLWD        -310294
#define PD_STRESSMARGCALCEMPTY         -310296
#define PD_STRESSMARGNOTCONSEC         -310298
#define PD_BASESCENEMPTY               -310300
#define PD_BASESCENNOTCONSEC           -310302
#define PD_CALCSTRESSNOTNEG            -310304
#define PD_PRANOTUNIQUE                -310306
#define PD_MARSCENEMPTY                -310308
#define PD_MARSCENNOTCONSEC            -310310
#define PD_INVSLCCOM                   -310312
#define PD_NOREFIXSTART                -310314
#define PD_NOREFIXCPN                  -310316
#define PD_ADDDIVNOTALLWD              -310318
#define PD_NOPDH                       -310320
#define PD_NOPDHACT                    -310322
#define PD_PDHIDEMPTY                  -310324
#define PD_REFTOPDH                    -310326
#define PD_NORSG                       -310328
#define PD_NORSGACT                    -310330
#define PD_RSGIDEMPTY                  -310332
#define PD_REFTORSG                    -310334
#define PD_NOSRG                       -310336
#define PD_NOSRGACT                    -310338
#define PD_SRGIDEMPTY                  -310340
#define PD_REFTOSRG                    -310342
#define PD_PDWWRONGSUM                 -310344
#define PD_SIZEMINGTMAX                -310346
#define PD_PCSIDEMPTY                  -310348
#define PD_DOUBLEPCS                   -310350
#define PD_MORETHANONEPCS              -310352
#define PD_NOPCS                       -310354
#define PD_NOPCSACT                    -310356
#define PD_CONTRELIGPCSEMPTY           -310358
#define PD_SUPERPRODAREAIDNOTUNIQUE    -310360
#define PD_REFTOPCS                    -310362
#define PD_NOHKP                       -310364
#define PD_NOHKPACT                    -310366
#define PD_HKPIDEMPTY                  -310368
#define PD_DELDAYSNOTZERO              -310370
#define PD_TIERDOUBLE                  -310372
#define PD_CREDITRATETOLARGE           -310374
#define PD_MARGDAYSEMPTY               -310376
#define PD_FIXTRRTYPEEMPTY             -310378
#define PD_BALANCEANDSTRICTEMPTY       -310380
#define PD_BALANCEACCEMPTY             -310382
#define PD_ONESYSTEMCLHALLOWED         -310384
#define PD_ISSNOTCLEARING              -310386
#define PD_ISSCONVNOTEQ                -310388
#define PD_PRCMETHEMPTY                -310390
#define PD_TOPNOTADDED                 -310392
#define PD_EPCALCRULEEMPTY             -310394
#define PD_NOTACCARPRA                 -310396
#define PD_NOIFFTDNTD                  -310398
#define PD_NOIFDLVSTARTNTD             -310400
#define PD_NOIFDLVSTOPNTD              -310402
#define PD_NOIFSTARTNTD                -310404
#define PD_NOIFSTOPNTD                 -310406
#define PD_INVALIDACTTIME              -310408
#define PD_DELIVSTARTAFTERSTOP         -310410
#define PD_USRIDWRONGCHAR              -310412
#define PD_MARCRVEMPTY                 -310414
#define PD_MARCRVFILLEDIN              -310416
#define PD_MARSECCRVFORW               -310418
#define PD_CRVTENOREMPTY               -310420
#define PD_DISCCRVNOTEMPTY             -310422
#define PD_DISCCRVEMPTY                -310424
#define PD_DISCCRVNOTDISC              -310426
#define PD_MARVALBASEEMPTY             -310428
#define PD_CRVTYPEEMPTY                -310430
#define PD_NOINTRINS                   -310432
#define PD_NOINTRINSACT                -310434
#define PD_NOSPONSCST                  -310436
#define PD_NOSPONSCSTACT               -310438
#define PD_SPONSCSTEMPTY               -310440
#define PD_SPONSEXCEMPTY               -310442
#define PD_NOSPONSCLICST               -310444
#define PD_NOSPONSCLICSTACT            -310446
#define PD_SPONSCLICSTEMPTY            -310448
#define PD_SPONSCLIEXCEMPTY            -310450
#define PD_SPONSCLIALRINPTL            -310452
#define PD_USRALRINPTL                 -310454
#define PD_RISKPROFILEPARAMEMPTY       -310456
#define PD_USRWRONGCST                 -310458
#define PD_PTLIDEMPTY                  -310460
#define PD_REFTOPTL                    -310462
#define PD_NOPTL                       -310464
#define PD_DOUBLEUSER                  -310466
#define PD_SGSIDEMPTY                  -310468
#define PD_REFTOSGS                    -310470
#define PD_NOSGS                       -310472
#define PD_NOSGSACT                    -310474
#define PD_DOUBLESGS                   -310476
#define PD_SHRTSRUSPSIDEMPTY           -310478
#define PD_SHRTLENEMPTY                -310480
#define PD_REFTORMS                    -310482
#define PD_SCALEFACEMPTY               -310484
#define PD_RMSIDEMPTY                  -310486
#define PD_NORMS                       -310488
#define PD_MARSECCRVEMPTY              -310490
#define PD_NODEPINS                    -310492
#define PD_NODEPINSACT                 -310494
#define PD_DEPINSEMPTY                 -310496
#define PD_DEPINSNOTEMPTY              -310498
#define PD_CONSTRMETHWRONG             -310500
#define PD_CRVTENORNOTEMPTY            -310502
#define PD_DEPINSNOTDEPOSIT            -310504
#define PD_FORWARDSTARTNOTEMPTY        -310506
#define PD_FORWARDSTARTLENZERO         -310508
#define PD_OISWRONG                    -310510
#define PD_NOTINTEGERYEARDAYS          -310512
#define PD_BOTHOTCENDANDSEPMARVAL      -310514
#define PD_CRVFORW                     -310516
#define PD_KFDIDEMPTY                  -310518
#define PD_REFTOKFD                    -310520
#define PD_NOKFD                       -310522
#define PD_NOKFDACT                    -310524
#define PD_KFNIDEMPTY                  -310526
#define PD_NOKFN                       -310528
#define PD_NOKFNACT                    -310530
#define PD_DOUBLEFOOTNOTE              -310532
#define PD_KOIIDEMPTY                  -310534
#define PD_NOKOI                       -310536
#define PD_NOKOIACT                    -310538
#define PD_DOUBLEOPENPERIOD            -310540
#define PD_INVALIDDATE                 -310542
#define PD_INVALIDFIRSTRESET           -310544
#define PD_INVALIDFIRSTFIX1            -310546
#define PD_INVALIDRESET1               -310548
#define PD_INVALIDFIRSTFIX2            -310550
#define PD_INVALIDRESET2               -310552
#define PD_INVALIDFIRSTFIX3            -310554
#define PD_INVALIDRESET3               -310556
#define PD_INVALIDFIRSTFIX4            -310558
#define PD_INVALIDRESET4               -310560
#define PD_COHIDEMPTY                  -310562
#define PD_NOCOH                       -310564
#define PD_NOCOHACT                    -310566
#define PD_REFTOCOH                    -310568
#define PD_ACCEPTCOLLEMPTY             -310570
#define PD_ALLOWCOLLEMPTY              -310572
#define PD_DOUBLETIMETOMAT             -310574
#define PD_REFTOHCT                    -310576
#define PD_ACCTTYPEEMPTY               -310578
#define PD_PTNIDEMPTY                  -310580
#define PD_NOPTN                       -310582
#define PD_NOPTNACT                    -310584
#define PD_DOUBLEPTN                   -310586
#define PD_EMAILEMPTY                  -310588
#define PD_UNITISEMPTY                 -310590
#define PD_SPONSUSEREMPTY              -310592
#define PD_NOSPONSUSR                  -310594
#define PD_NOSPONSUSRACT               -310596
#define PD_WRONGSPONSUSR               -310598
#define PD_WRONGUNIT                   -310600
#define PD_NOCRW                       -310602
#define PD_SPONSNOTAUTH                -310604
#define PD_DELPTNNOTALLWD              -310606
#define PD_DELPTUNOTALLWD              -310608
#define PD_DELPTPNOTALLWD              -310610
#define PD_DELPTLNOTALLWD              -310612
#define PD_SPONSNOTTHESAME             -310614
#define PD_REFTOCRW                    -310616
#define PD_CRWIDEMPTY                  -310618
#define PD_NOCRWACT                    -310620
#define PD_ILLPRLIMSUP                 -310622
#define PD_ILLPRLIMSUPTYPE             -310624
#define PD_FROMPRICEDIFF               -310626
#define PD_NORPS                       -310628
#define PD_NORPSACT                    -310630
#define PD_RPSIDEMPTY                  -310632
#define PD_REFTORPS                    -310634
#define PD_PRICESOURCEEMPTY            -310636
#define PD_DOUBLERPS                   -310638
#define PD_NODYNPLR                    -310640
#define PD_NODYNPLRACT                 -310642
#define PD_DYNPLRIDEMPTY               -310644
#define PD_NODYNRPS                    -310646
#define PD_NODYNRPSACT                 -310648
#define PD_PLRWRONGDEF                 -310650
#define PD_SPONSUSRCHANGED             -310652
#define PD_ENAWARNCHANGED              -310654
#define PD_ENANOTCHANGED               -310656
#define PD_ENABRCHANGED                -310658
#define PD_ADDPTUNOTALLWD              -310660
#define PD_ADDPTPNOTALLWD              -310662
#define PD_LCOUPONEMPTY                -310664
#define PD_DAYCALCEMPTY                -310666
#define PD_DAYCALCWRONG                -310668
#define PD_ISSCURIDEMPTY               -310670
#define PD_NOISSCUR                    -310672
#define PD_FLOATFIXEDEMPTY             -310674
#define PD_FLOATFIXEDWRONG             -310676
#define PD_GWCIDEMPTY                  -310678
#define PD_PRIKEYEMPTY                 -310680
#define PD_VALUEEMPTY                  -310682
#define PD_GWCDUPLICATE                -310684
#define PD_NOGWC                       -310686
#define PD_NOGWCACT                    -310688
#define PD_TMPLINCIDEMPTY              -310690
#define PD_WRONGTEMPL                  -310692
#define PD_RICCODEEMPTY                -310694
#define PD_PTLIDWRONG                  -310696
#define PD_PTPUNITCHANGED              -310698
#define PD_COLLHANDLWRONG              -310700
#define PD_REFTOVCU                    -310702
#define PD_VCUIDEMPTY                  -310704
#define PD_NOVCU                       -310706
#define PD_NOVCUACT                    -310708
#define PD_DOUBLEVCU                   -310710
#define PD_RPSMUSTBESAME               -310712
#define PD_UNITVALNOTALLWD             -310714
#define PD_NOKIV                       -310716
#define PD_NOKTR                       -310718
#define PD_INVALIDREPAYM1              -310720
#define PD_INVALIDREPAYM2              -310722
#define PD_INVALIDREPAYM3              -310724
#define PD_INVALIDREPAYM4              -310726
#define PD_KIVIDEMPTY                  -310728
#define PD_INDEXDATEEMPTY              -310730
#define PD_INDEXDATENOTUNIQUE          -310732
#define PD_KTRIDEMPTY                  -310734
#define PD_TAXTYPEEMPTY                -310736
#define PD_CSHFLOWSTSEMPTY             -310738
#define PD_CONVCODEEMPTY               -310740
#define PD_TAXEMPTY                    -310742
#define PD_LOANTYPEEMPTY               -310744
#define PD_REPAYMPROFEMPTY             -310746
#define PD_REPAYMPROFSPECEMPTY         -310748
#define PD_PRINCIXLINKEMPTY            -310750
#define PD_CUPTYPEEMPTY                -310752
#define PD_BONDTYPEEMPTY               -310754
#define PD_FIRSTISSDATEEMPTY           -310756
#define PD_ISSUERTYPEEMPTY             -310758
#define PD_SECPLACEMEMPTY              -310760
#define PD_YIELDAVRGREMPTY             -310762
#define PD_TURNOVRGREMPTY              -310764
#define PD_GOVGUAREMPTY                -310766
#define PD_INVALIDREPAYM5              -310768
#define PD_INVALIDREPAYM6              -310770
#define PD_INVALIDREPAYM7              -310772
#define PD_INVALIDREPAYM8              -310774
#define PD_INVALIDREPAYM9              -310776
#define PD_INVALIDREPAYM10             -310778
#define PD_INVALIDREPAYM11             -310780
#define PD_INVALIDREPAYM12             -310782
#define PD_CASHFLOWNOTNO               -310784
#define PD_YIELDCALCNOTNO              -310786
#define PD_DRAWPRICEEMPTY              -310788
#define PD_NOOFOPENPEREMPTY            -310790
#define PD_LASTDUEDATEEMPTY            -310792
#define PD_DRAWPRICEWRONG              -310794
#define PD_TURNOVRGRNOTTR              -310796
#define PD_COUPONTYPEWRONG             -310798
#define PD_TURNOVRGRTR                 -310800
#define PD_REPAYMPROFSPECWRONG         -310802
#define PD_DRAWPRICENOTEMPTY           -310804
#define PD_INTRSBASESPRNOTEMPTY        -310806
#define PD_FLOATDATENOTEMPTY           -310808
#define PD_FLOATDATEEMPTY              -310810
#define PD_INTRSBASECAPEMPTY           -310812
#define PD_INTRSBASECAPNOTEMPTY        -310814
#define PD_CIBORSTRIKENOTEMPTY         -310816
#define PD_FIRSTINTPAYMYWRONG          -310818
#define PD_INTRSBASESPREMPTY           -310820
#define PD_FIRSTISSAFTLAST             -310822
#define PD_OPENPERGTDURINY             -310824
#define PD_LOANTYPEINFOSPEC            -310826
#define PD_OPENPEREMPTY                -310828
#define PD_NEWISSNOMEMPTY              -310830
#define PD_FIRSTOPENEMPTY              -310832
#define PD_LASTOPENEMPTY               -310834
#define PD_INTDEBTPRICEEMPTY           -310836
#define PD_INTDEBTPRICENOTEMPTY        -310838
#define PD_INTDEBTRATEEMPTY            -310840
#define PD_INTDEBTRATENOTEMPTY         -310842
#define PD_SERIALLOANRATIOEMPTY        -310844
#define PD_SERIALLOANRATIONOTEMPTY     -310846
#define PD_SERIALLOANNOREPEMPTY        -310848
#define PD_SERIALLOANNOREPNOTEMPTY     -310850
#define PD_BULLETLOANNOREPEMPTY        -310852
#define PD_BULLETLOANNOREPNOTEMPTY     -310854
#define PD_DRAWINGDATEEMPTY            -310856
#define PD_DRAWINGDATENOTEMPTY         -310858
#define PD_FIRSTDEBTSETTLDATEEMPTY     -310860
#define PD_FIRSTDEBTSETTLDATENOTEMPTY  -310862
#define PD_PUBLDATEEMPTY               -310864
#define PD_PUBLDATENOTEMPTY            -310866
#define PD_NOOFDEFERREDWRONG           -310868
#define PD_CALLPRICEEMPTY              -310870
#define PD_YIELDCALCNOTYES             -310872
#define PD_BONDTYPEWRONG               -310874
#define PD_LASTISSDATEEMPTY            -310876
#define PD_CALCDATEEMPTY               -310878
#define PD_COUPREPAYMDATEEMPTY         -310880
#define PD_FIRSTAFTLAST                -310882
#define PD_LASTAFTCALC                 -310884
#define PD_CALCAFTDRAW                 -310886
#define PD_DRAWAFTPUBL                 -310888
#define PD_PUBLAFTCUPREPAYM            -310890
#define PD_FIRSTISSUENOTINCR           -310892
#define PD_LASTISSUENOTINCR            -310894
#define PD_CALCNOTINCR                 -310896
#define PD_DRAWNOTINCR                 -310898
#define PD_PUBLNOTINCR                 -310900
#define PD_REPAYMNOTINCR               -310902
#define PD_INCIDEXIST                  -310904
#define PD_INSIDEXISTFROMUND           -310906
#define PD_FIRSTTRADEDATEEMPTY         -310908
#define PD_TRADINGUNITEMPTY            -310910
#define PD_DAYSXCOUPONWRONG            -310912
#define PD_PTLUSREMPTY                 -310914
#define PD_MAXLIMITEXCEEDED            -310916
#define PD_FXMIDEMPTY                  -310918
#define PD_NOFXM                       -310920
#define PD_NOFXMACT                    -310922
#define PD_NOFXINT                     -310924
#define PD_NOFXINTACT                  -310926
#define PD_NOGBLCUR                    -310928
#define PD_NOGBLCURACT                 -310930
#define PD_NOPRHOLDINT                 -310932
#define PD_NOPRHOLDINTACT              -310934
#define PD_PERCENTILEWRONG             -310936
#define PD_REFTOFXM                    -310938
#define PD_PFGIDEMPTY                  -310940
#define PD_ATGIDEMPTY                  -310942
#define PD_NOATG                       -310944
#define PD_NOATGACT                    -310946
#define PD_REFTOATG                    -310948
#define PD_DOUBLEATG                   -310950
#define PD_ACCUMLVLEMPTY               -310952
#define PD_PRODAGGRLVLEMPTY            -310954
#define PD_PRODEFFLVLEMPTY             -310956
#define PD_LEVELEMPTY                  -310958
#define PD_LOWERLIMITEMPTY             -310960
#define PD_UPPERLIMITEMPTY             -310962
#define PD_ACCUMBASEEMPTY              -310964
#define PD_ACCUMPEREMPTY               -310966
#define PD_EFFECTAFTIDEMPTY            -310968
#define PD_EFFDUREMPTY                 -310970
#define PD_DOUBLEPFT                   -310972
#define PD_DOUBLELEVEL                 -310974
#define PD_REFTOPFG                    -310976
#define PD_NOPFG                       -310978
#define PD_NOPFTITEMS                  -310980
#define PD_NOPFGLADDER                 -310982
#define PD_INSTRDATEEMPTY              -310984
#define PD_RPGISCONNECTED              -310986
#define PD_DEFUSRALRINPTL              -310988
#define PD_ENADEFUSRCHANGED            -310990
#define PD_LINKEDISSAME                -310992
#define PD_LINKEDISLINKED              -310994
#define PD_CISISLINKED                 -310996
#define PD_MAXPTLUSREXCEEDED           -310998
#define PD_MAXPTLPTPEXCEEDED           -311000
#define PD_MAXPTNEXCEEDED              -311002
#define PD_LONGUNDTOOLONG              -311004
#define PD_TIMEVALUEBASEEMPTY          -311006
#define PD_SPOTDELAYTYPEEMPTY          -311008
#define PD_STARTOFMARGINPEREMPTY       -311010
#define PD_MTMSTYLEEMPTY               -311012
#define PD_PCGIDEMPTY                  -311014
#define PD_NOPCG                       -311016
#define PD_NOPCGACT                    -311018
#define PD_REFTOPCG                    -311020
#define PD_DOUBLEPCG                   -311022
#define PD_STRIDEMPTY                  -311024
#define PD_NOSTR                       -311026
#define PD_NOSTRACT                    -311028
#define PD_REFTOSTR                    -311030
#define PD_NOOFINSEMPTY                -311032
#define PD_STRIPRANGEEMPTY             -311034
#define PD_SPLITRULEEMPTY              -311036
#define PD_RISKTYPEEMPTY               -311038
#define PD_PCYIDEMPTY                  -311040
#define PD_NOPCY                       -311042
#define PD_NOPCYACT                    -311044
#define PD_REFTOPCY                    -311046
#define PD_DOUBLEPCY                   -311048
#define PD_NOHEACST                    -311050
#define PD_NOHEACSTACT                 -311052
#define PD_NOCRGCST                    -311054
#define PD_NOCRGCSTACT                 -311056
#define PD_NOCPYEXC                    -311058
#define PD_NOCPYEXCACT                 -311060
#define PD_CISISFULL                   -311062
#define PD_FXCIDEMPTY                  -311064
#define PD_NOFXC                       -311066
#define PD_NOFXCACT                    -311068
#define PD_REFTOFXC                    -311070
#define PD_TENIDEMPTY                  -311072
#define PD_NOTEN                       -311074
#define PD_NOTENACT                    -311076
#define PD_TENORTYPEEMPTY              -311078
#define PD_DOUBLETEN                   -311080
#define PD_DOUBLECUR                   -311082
#define PD_FTPNOPWD                    -311084
#define PD_EOTIFADJONLYDAILY           -311086
#define PD_STARTSTOPFROMUPPER          -311088
#define PD_USEFTDREFONLYDAILY          -311090
#define PD_EXPCLOSED                   -311092
#define PD_ONLYCALDAYSWITHUNITMONTH    -311094
#define PD_NOAREACOM                   -311096
#define PD_NOAREACOMACT                -311098
#define PD_AREACOMIDEMPTY              -311100
#define PD_NOSYSTEMCOM                 -311102
#define PD_NOSYSTEMCOMACT              -311104
#define PD_SYSTEMCOMIDEMPTY            -311106
#define PD_NOEPA                       -311108
#define PD_REFTOEPA                    -311110
#define PD_PENDINGNOTALLWD             -311112
#define PD_NOIXC                       -311114
#define PD_IXCIDEMPTY                  -311116
#define PD_INDEXINSIDEMPTY             -311118
#define PD_NOIDXINS                    -311120
#define PD_NOIDXINSACT                 -311122
#define PD_INDEXALREXIST               -311124
#define PD_REFTOIXC                    -311126
#define PD_ICFIDEMPTY                  -311128
#define PD_NOICF                       -311130
#define PD_NOICFACT                    -311132
#define PD_MSEIDEMPTY                  -311134
#define PD_MSECODEEMPTY                -311136
#define PD_NOMSE                       -311138
#define PD_NOMSEACT                    -311140
#define PD_EXTFULLHOLDBWRONG           -311142
#define PD_PRICESOURCESAME             -311144
#define PD_PRICESOURCEISLINKED         -311146
#define PD_ONLYONELOTALLOWED           -311148
#define PD_ONLYROUNDLOTALLOWED         -311150
#define PD_INSISLINKED                 -311152
#define PD_SPREADTOLEMPTY              -311154
#define PD_LTPTOLEMPTY                 -311156
#define PD_PRICESOURCEDIFFUNDERLYING   -311158
#define PD_NOIOC                       -311160
#define PD_NOIOCACT                    -311162
#define PD_IOCIDEMPTY                  -311164
#define PD_DOUBLEIOC                   -311166
#define PD_PROTOCOLEMPTY               -311168
#define PD_PROTOCOLNOTEMPTY            -311170
#define PD_MULTICASTEMPTY              -311172
#define PD_MULTICASTNOTEMPTY           -311174
#define PD_FAMILYNAMEEMPTY             -311176
#define PD_PRIMARYNODEEMPTY            -311178
#define PD_PRIMARYINSTEMPTY            -311180
#define PD_STANDBYNODEEMPTY            -311182
#define PD_STANDBYINSTEMPTY            -311184
#define PD_NOOCG                       -311186
#define PD_NOOCGACT                    -311188
#define PD_PRIMARYNODEEXIST            -311190
#define PD_STANDBYNODEEXIST            -311192
#define PD_REFTOIOC                    -311194
#define PD_STANDBYSAMEASPRIMARY        -311196
#define PD_PRIMARYBADPAIR              -311198
#define PD_STANDBYBADPAIR              -311200
#define PD_ONLYONEDEFIOC               -311202
#define PD_WRONGPORTNUMBER             -311204
#define PD_PORTNOTUNIQUE               -311206
#define PD_APPLIDNOTUNIQUE             -311208
#define PD_DOUBLEAPPL                  -311210
#define PD_FREEFLOATGTSHAREDISSUES     -311212
#define PD_FREEFLOATRATIOTOLARGE       -311214
#define PD_FREEFLOATSHARESZERO         -311216
#define PD_FREEFLOATZERO               -311218
#define PD_DELIVSERIENOTCLEARED        -311220
#define PD_NODELIVINS                  -311222
#define PD_NODELIVINSACT               -311224
#define PD_STRIPMONTHNOTACT            -311226
#define PD_NOGBLCRVCUR                 -311228
#define PD_NOGBLCRVCURACT              -311230
#define PD_SUBMETHODMARGEMPTY          -311232
#define PD_LAMBDAWEIGHTEDWRONG         -311234
#define PD_LAMBDAEQUALWRONG            -311236
#define PD_MAXNUMBERTOSMALL            -311238
#define PD_RANKINGACCCALCWRONG         -311240
#define PD_TOTACCCALCTOBIG             -311242
#define PD_SUBMETHODCCAREMPTY          -311244
#define PD_RANKINGNOTALLLOWED          -311246
#define PD_LAMBDANOTALLOWED            -311248
#define PD_REFTOPQC                    -311250
#define PD_KNOCKSTYLESPEC              -311252
#define PD_PQCANDLSTIDEMPTY            -311254
#define PD_PQCANDLSTIDSPEC             -311256
#define PD_MINVOLPQRNOTEMPTY           -311258
#define PD_MINVOLPQREMPTY              -311260
#define PD_ECLIDEMPTY                  -311262
#define PD_REFTOECL                    -311264
#define PD_NOECL                       -311266
#define PD_DELECLNOTALLWD              -311268
#define PD_MISSINGCONFIG               -311270
#define PD_WRONGCONFIGVAL              -311272
#define PD_UNITDIFFERFROMEXC           -311274
#define PD_NOTEQUITYINDEX              -311276
#define PD_MAXCHGEMPTY                 -311278
#define PD_WRLIDEMPTY                  -311280
#define PD_NOWRL                       -311282
#define PD_NOWRLACT                    -311284
#define PD_REFTOWRL                    -311286
#define PD_NOSIS                       -311288
#define PD_NOSISACT                    -311290
#define PD_SISIDEMPTY                  -311292
#define PD_REFTOSIS                    -311294
#define PD_SISTIMEEMPTY                -311296
#define PD_CHAEMPTY                    -311298
#define PD_DOUBLESISTIME               -311300
#define PD_DOUBLECLA                   -311302
#define PD_NOPFC                       -311304
#define PD_NOPFCACT                    -311306
#define PD_NOINH                       -311308
#define PD_NOINHACT                    -311310
#define PD_NOREF                       -311312
#define PD_NOREFACT                    -311314
#define PD_PFCIDEMPTY                  -311316
#define PD_PENCOMP                     -311318
#define PD_INHIDEMPTY                  -311320
#define PD_REFIDEMPTY                  -311322
#define PD_REFTOINH                    -311324
#define PD_DOUBLEINH                   -311326
#define PD_REFTOREF                    -311328
#define PD_DOUBLEREF                   -311330
#define PD_REFTOPFC                    -311332
#define PD_DOUBLEPFC                   -311334
#define PD_STARTTIME                   -311336
#define PD_ENDTIME                     -311338
#define PD_OFFSETNUMBER                -311340
#define PD_STARTTIMEGREATENDTIME       -311342
#define PD_NOESP                       -311344
#define PD_NOESPACT                    -311346
#define PD_REFTOESP                    -311348
#define PD_ESPIDEMPTY                  -311350
#define PD_NOCSE                       -311352
#define PD_NOCSEACT                    -311354
#define PD_NODEFCSE                    -311356
#define PD_NODEFCSEACT                 -311358
#define PD_CSEIDEMPTY                  -311360
#define PD_FROMTIMEEMPTY               -311362
#define PD_REFTOCSE                    -311364
#define PD_DOUBLECSETIME               -311366
#define PD_STARTDATEGREATERENDDATE     -311368
#define PD_NOHSE                       -311370
#define PD_NOHSEACT                    -311372
#define PD_HSEIDEMPTY                  -311374
#define PD_REFTOHSE                    -311376
#define PD_DEFCSEORRECORD              -311378
#define PD_SHARESOP                    -311380
#define PD_FREEFLOATOP                 -311382
#define PD_MIDMINMAX                   -311384
#define PD_MIDPOINTTRADESINFO          -311386
#define PD_EXTSETTLPARAM               -311388
#define PD_NOCLG                       -311390
#define PD_NOCLGACT                    -311392
#define PD_CLGIDEMPTY                  -311394
#define PD_VAGLIMIT                    -311396
#define PD_INDSECURLIMIT               -311398
#define PD_REFTOCLG                    -311400
#define PD_DOUBLECLG                   -311402
#define PD_NODFU                       -311404
#define PD_NODFUACT                    -311406
#define PD_NOPCC                       -311408
#define PD_NOPCCACT                    -311410
#define PD_DFUIDEMPTY                  -311412
#define PD_PCCIDEMPTY                  -311414
#define PD_REFTODFU                    -311416
#define PD_REFTOPCC                    -311418
#define PD_DOUBLERRDID                 -311420
#define PD_STRKOREXPCHANGED            -311422
#define PD_NEWSTRKNOTALLWD             -311424
#define PD_NEWSTRKNOTSPEC              -311426
#define PD_NEWEXPNOTALLWD              -311428
#define PD_NEWEXPNOTSPEC               -311430
#define PD_SOSTRKNOTALLWD              -311432
#define PD_SOSTRKNOTSPEC               -311434
#define PD_SOEXPNOTALLWD               -311436
#define PD_SOEXPNOTSPEC                -311438
#define PD_UPDISSNOTALLWD              -311440
#define PD_NOTARRP                     -311442
#define PD_NOTARRPACT                  -311444
#define PD_CLONEFEEISWRONG             -311446
#define PD_STRKPROPWRONG               -311448
#define PD_DEALPROPWRONG               -311450
#define PD_CONTRSZOPWRONG              -311452
#define PD_PRCQUOTOPWRONG              -311454
#define PD_CONTROPWRONG                -311456
#define PD_UNDPROPWRONG                -311458
#define PD_UNDMARGPROPWRONG            -311460
#define PD_CONSTCONTRWRONG             -311462
#define PD_INHISINWRONG                -311464
#define PD_ADJSAMEISWRONG              -311466
#define PD_PRINFOPREVDAYWRONG          -311468
#define PD_NOOFSHARESOPWRONG           -311470
#define PD_FREEFLOATOPWRONG            -311472
#define PD_ISNEWWRONG                  -311474
#define PD_ISSPINOFFWRONG              -311476
#define PD_SOMODIFIERWRONG             -311478
#define PD_EXPDATEOPWRONG              -311480
#define PD_MOVETRDSTATWRONG            -311482
#define PD_DOUBLEOLDINSID              -311484
#define PD_REFINCSAMEASINC             -311486
#define PD_REFINCCIRCULAR              -311488
#define PD_NOREFINC                    -311490
#define PD_NOREFINCACT                 -311492
#define PD_PREVCBOUNCRNOTALLWD         -311494
#define PD_LPPRALGEMPTY                -311496
#define PD_CYCLEPRICELEVNOTZERO        -311498
#define PD_SPTIDEMPTY                  -311500
#define PD_INTSPTIDEMPTY               -311502
#define PD_NOATMBASECALC               -311504
#define PD_NOSPT                       -311506
#define PD_NOSPTACT                    -311508
#define PD_SPRCHECKEMPTY               -311510
#define PD_REFTOSPT                    -311512
#define PD_REFTOTRR                    -311514
#define PD_PTLUNITNOTQ                 -311516
#define PD_REQFULFILRESPRANGE          -311518
#define PD_GWYIDEMPTY                  -311520
#define PD_GWYADDREMPTY                -311522
#define PD_DOUBLEPARTITION             -311524
#define PD_GWYPORTNBREMPTY             -311526
#define PD_ACCTYPEMPTY                 -311528
#define PD_PTLEMAILDIFF                -311530
#define PD_ECLEMAILDIFF                -311532
#define PD_NOPRM                       -311534
#define PD_NOPRMACT                    -311536
#define PD_PRMIDEMPTY                  -311538
#define PD_REFTOPRM                    -311540
#define PD_ILLPREVENTMETH              -311542
#define PD_NOPREVENTMETH               -311544
#define PD_GWYPORT0ALREXIST            -311546
#define PD_GWYCLASSEMPTY               -311548
#define PD_GWYIDWRONGNAME              -311550
#define PD_INSTANCEEMPTY               -311552
#define PD_PARTEMPTY                   -311554
#define PD_ONLYONEPARTALLWD            -311556
#define PD_MAX2OUCHALLWD               -311558
#define PD_INSTANCEDIFFER              -311560
#define PD_PARTITIONDIFFER             -311562
#define PD_PORTDIFFER                  -311564
#define PD_OUCHGWYIDWRONGNAME          -311566
#define PD_GWYNODEPORTEXIST            -311568
#define PD_USERNAMEEXCEEDED            -311570
#define PD_PASSWEXCEEDED               -311572
#define PD_CITIDEMPTY                  -311574
#define PD_NOCIT                       -311576
#define PD_NOCITACT                    -311578
#define PD_REFTOCIT                    -311580
#define PD_ONEEMPTYSST                 -311582
#define CDB_CGEN_UNDZERO               -314001
#define CDB_CGEN_TOMNYCALL             -314002
#define CDB_CGEN_TOMNYPUT              -314004
#define CDB_CGEN_TOMNYFRWD             -314006
#define CDB_CGEN_COMBCRE                314007
#define CDB_CGEN_COMBEXI                314009
#define CDB_CGEN_NEWUNDER               314011
#define CDB_CGEN_ALTUNDER               314013
#define CDB_CGEN_UNDPRICE               314015
#define CDB_CGEN_NUMCOMB                314017
#define CDB_CGEN_DATEIS                 314019
#define CDB_CGEN_MON                    314021
#define CDB_CGEN_TUE                    314023
#define CDB_CGEN_WED                    314025
#define CDB_CGEN_THU                    314027
#define CDB_CGEN_FRI                    314029
#define CDB_CGEN_SAT                    314031
#define CDB_CGEN_SUN                    314033
#define CDB_SLEC_LOGDEF                 315001
#define CDB_SLEC_NOSTORE                315003
#define CDB_SLEC_STORE                  315005
#define CDB_SLEC_INCNOTFOUND           -315006
#define CDB_SLEC_UPPINSNOTFOUND        -315008
#define CDB_SLEC_INSIDFOUND            -315010
#define CDB_SLEC_ISINFOUND             -315012
#define CDB_SLEC_BINFOUND              -315014
#define CDB_MXLD_ACR_CM_MISS           -320000
#define CDB_MXLD_ACC_INSERT             320001
#define CDB_MXLD_ACC_DELETE             320003
#define CDB_MXLD_COM_DELETE             320005
#define CDB_MXLD_COM_EXISTED            320007
#define CDB_MXLD_COM_INSERT             320009
#define CDB_MXLD_COM_UPDATE             320011
#define CDB_MXLD_INC_DELETE             320013
#define CDB_MXLD_INC_EXISTED            320015
#define CDB_MXLD_INC_INSERT             320017
#define CDB_MXLD_INC_UPDATE             320019
#define CDB_MXLD_INS_DELETE             320021
#define CDB_MXLD_INS_EXISTED            320023
#define CDB_MXLD_INS_INSERT             320025
#define CDB_MXLD_INS_UPDATE             320027
#define CDB_MXLD_SYM_INSERT             320029
#define CDB_MXLD_SYM_DELETE             320031
#define CDB_MXLD_SYM_IGNORED            320033
#define CDB_MXLD_USR_DELETE             320035
#define CDB_MXLD_USR_INSERT             320037
#define CDB_MXLD_USR_UPDATE             320039
#define CDB_MXLD_USR_SUSPEND            320041
#define CDB_MXLD_CST_DELETE             320043
#define CDB_MXLD_CST_INSERT             320045
#define CDB_MXLD_ACRONYMREAD            320047
#define CDB_MXLD_BADGEREAD              320049
#define CDB_MXLD_DELMARK                320051
#define CDB_MXLD_MFIRMREAD              320053
#define CDB_MXLD_OPTSYMREAD             320055
#define CDB_MXLD_OPTSERREAD             320057
#define CDB_MXLD_REGTRADREAD            320059
#define CDB_MXLD_SPECREAD               320061
#define CDB_MXLD_COMPL_OK               320063
#define CDB_SGEN_NON_INT_MODE          -325000
#define CDB_SGEN_ABORT_RTR_WAIT        -325002
#define CDB_SGEN_UNDZERO               -325003
#define CDB_SGEN_TOOMANY_CUR_SER       -325004
#define CDB_SGEN_TOOMANY_CUR_GRP       -325006
#define CDB_SGEN_TOOMANY_SERIES        -325008
#define CDB_SGEN_TOOMANY_STRIKES       -325010
#define CDB_SGEN_ICACHE                -325012
#define CDB_SGEN_IGMT                  -325014
#define CDB_SGEN_IFILL                 -325016
#define CDB_SGEN_IINCR                 -325018
#define CDB_SGEN_IMAX                  -325020
#define CDB_SGEN_SERIES_CREATED         325021
#define CDB_SGEN_SERIES_EXISTED         325023
#define CDB_SGEN_NEWUNDER               325025
#define CDB_SGEN_NEW_INST_CLASS         325027
#define CDB_SGEN_UNDPRICE               325029
#define CDB_SGEN_NUMSERIES              325031
#define CDB_SGEN_DATEIS                 325033
#define CDB_SGEN_NO_NEW_SERIES          325035
#define CDB_SGEN_NOISA                  325037
#define CDB_SGEN_NOISN                  325039
#define CDB_SGEN_IWRONG                 325041
#define CDB_SGEN_IGET                   325043
#define CDB_VPCGWY_OMLOGINGIVEUP       -330000
#define CDB_VPCGWY_MQLOGINGIVEUP       -330002
#define CDB_VPCGWY_NOSYSRES            -330004
#define CDB_VPCGWY_RECOVFILEERR        -330006
#define CDB_VPCGWY_MQDEADMSG           -330008
#define CDB_VPCGWY_ADDFAIL             -330010
#define CDB_VPCGWY_OMQRYFAIL           -330012
#define CDB_VPCGWY_INTERR              -330014
#define CDB_VPCGWY_COMERR              -330016
#define CDB_VPCGWY_OMTRANSERR          -330018
#define CDB_VPCGWY_OMLOGINFAIL         -330019
#define CDB_VPCGWY_OMLOGOUTFAIL        -330021
#define CDB_VPCGWY_MQCONNFAIL          -330023
#define CDB_VPCGWY_MQOPENFAIL          -330025
#define CDB_VPCGWY_MQCLOSEFAIL         -330027
#define CDB_VPCGWY_MQDISCFAIL          -330029
#define CDB_VPCGWY_MQPUTFAIL           -330031
#define CDB_VPCGWY_MQGETFAIL           -330033
#define CDB_VPCGWY_MQTRANSMSG          -330035
#define CDB_VPCGWY_OMLOGINSUC           330037
#define CDB_VPCGWY_OMLOGOUTSUC          330039
#define CDB_VPCGWY_MQCONNSUC            330041
#define CDB_VPCGWY_MQOPENSUC            330043
#define CDB_VPCGWY_MQCLOSESUC           330045
#define CDB_VPCGWY_MQDISCSUC            330047
#define CDB_VPCGWY_MQDQEMPTY            330049
#define CDB_VPCGWY_MQTRANSQEMPTY        330051
#define CDB_VPCGWY_ACTSERIES            330053
#define CDB_VPCGWY_VPCINERR             330055
#define CDB_VPCGWY_OMBRREC              330057
#define CDB_VPCGWY_EXIT                 330059
#define CDB_VPCGWY_START                330061
#define CDB_MDF_VERSION                 335001

/******************************************************************************
*end*of* cdb_messages.h
*******************************************************************************/

#endif /* _CDB_MESSAGES_H_ */


#ifndef _DE_MESSAGES_H_
#define _DE_MESSAGES_H_

/******************************************************************************
Module: de_messages.h

Message definitions generated 2022-12-01 07:07:38 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define DE_DUPLICATE_TOKEN             -800002
#define DE_UNKNOWN_TOKEN               -800004
#define DE_INVALID_ORDERBOOK           -800006
#define DE_INVALID_SIDE                -800008
#define DE_INVALID_TIF                 -800010
#define DE_INVALID_ORDER_NUMBER        -800012
#define DE_CAN_NOT_CANCEL              -800014
#define DE_THROTTLING                  -800015
#define DE_INVALID_MASS_CANCEL_SCOPE   -800016
#define DE_INVALID_UNDERLYING          -800018

/******************************************************************************
*end*of* de_messages.h
*******************************************************************************/

#endif /* _DE_MESSAGES_H_ */


#ifndef _GEN_MESSAGES_H_
#define _GEN_MESSAGES_H_

/******************************************************************************
Module: gen_messages.h

Message definitions generated 2022-12-01 07:07:29 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define GEN_CDC_OK                      110001
#define GEN_CDC_MEM_FULL               -110002
#define GEN_CDC_MEM_NOTFOU             -110003
#define GEN_CDC_TRC_NOTDEF             -110005
#define GEN_CDC_INS_NOTFOU             -110007
#define GEN_CDC_INT_NOTFOU             -110009
#define GEN_CDC_LIU_NOTFOU             -110011
#define GEN_CDC_LTT_NOTFOU             -110013
#define GEN_CDC_MAR_NOTFOU             -110015
#define GEN_CDC_USR_NOTFOU             -110017
#define GEN_CDC_CLI_NOTFOU             -110019
#define GEN_CDC_VPT_NOTFOU             -110021
#define GEN_CDC_INT_CLOSED             -110023
#define GEN_CDC_ILL_TRT_IN_USR         -110025
#define GEN_CDC_NO_MATCH               -110027
#define GEN_CDC_INT_FULL               -110028
#define GEN_CDC_INS_FULL               -110030
#define GEN_CDC_LTT_FULL               -110032
#define GEN_CDC_LIU_FULL               -110034
#define GEN_CDC_PQR_FULL               -110036
#define GEN_CDC_USR_FULL               -110038
#define GEN_CDC_MAR_FULL               -110040
#define GEN_CDC_RIT_FULL               -110042
#define GEN_CDC_VPT_FULL               -110044
#define GEN_CDC_CLI_FULL               -110046
#define GEN_CDC_INT_SIZE_FULL          -110048
#define GEN_CDC_USR_TRT_ID_FULL        -110050
#define GEN_CDC_NOT_INIT               -110051
#define GEN_CDC_NOT_LOCKED             -110053
#define GEN_CDC_NOT_UPDATER            -110055
#define GEN_CDC_LAST_TRADE             -110057
#define GEN_CDC_MAR_LOCKED             -110059
#define GEN_CDC_ILL_COMMODITY          -110061
#define GEN_CDC_CBS_FULL               -110062
#define GEN_CDC_VMS_FULL               -110064
#define GEN_CDC_ICO_FULL               -110066
#define GEN_CDC_USR_NAME_NOTFOU        -110068
#define GEN_CDC_NEAR_MAX_LIMIT         -110069
#define GEN_CDC_TABLE_NOT_FOUND        -110071
#define GEN_CDC_TRT_FULL               -110072
#define GEN_CDC_TRT_NOTFOU             -110073
#define GEN_CDC_PQR_NOTFOU             -110075
#define GEN_CDC_USR_SUSP               -110077
#define GEN_CDC_SYM_FULL               -110078
#define GEN_CDC_SYM_NOTFOU             -110079
#define GEN_CDC_CST_SUSP               -110081
#define GEN_CDC_VOL_FULL               -110082
#define GEN_CDC_NOT_VPT                -110083
#define GEN_CDC_NOT_VPD                -110085
#define GEN_CDC_VPD_FULL               -110086
#define GEN_CDC_VPD_NOTFOU             -110087
#define GEN_CDC_MKT_NOTVAL             -110089
#define GEN_CDC_BBO_NOTFOU             -110091
#define GEN_CDC_COM_NOTFOU             -110093
#define GEN_CDC_CRS_NOTFOU             -110095
#define GEN_CDC_INC_NOTFOU             -110097
#define GEN_CDC_SPR_NOTFOU             -110099
#define GEN_CDC_TRC_NOTFOU             -110101
#define GEN_CDC_BBO_FULL               -110102
#define GEN_CDC_COM_FULL               -110104
#define GEN_CDC_CRS_FULL               -110106
#define GEN_CDC_INC_FULL               -110108
#define GEN_CDC_SPR_FULL               -110110
#define GEN_CDC_TRC_FULL               -110112
#define GEN_CDC_OBL_FULL               -110114
#define GEN_CDC_OBL_NOTFOU             -110115
#define GEN_CDC_MSU_FULL               -110116
#define GEN_CDC_MSU_NOTFOU             -110117
#define GEN_CDC_COG_FULL               -110118
#define GEN_CDC_COG_NOTFOU             -110119
#define GEN_CDC_FML_FULL               -110120
#define GEN_CDC_FML_NOTFOU             -110121
#define GEN_CDC_OBK_FULL               -110122
#define GEN_CDC_OBK_NOTFOU             -110123
#define GEN_CDC_TSR_FULL               -110124
#define GEN_CDC_TSR_NOTFOU             -110125
#define GEN_CDC_EDG_FULL               -110126
#define GEN_CDC_EDG_NOTFOU             -110127
#define GEN_CDC_SPG_FULL               -110128
#define GEN_CDC_SPG_NOTFOU             -110129
#define GEN_CDC_PDP_FULL               -110130
#define GEN_CDC_PDP_NOTFOU             -110131
#define GEN_CDC_FOV_FULL               -110132
#define GEN_CDC_FOV_NOTFOU             -110133
#define GEN_CDC_TSE_FULL               -110134
#define GEN_CDC_TSE_NOTFOU             -110135
#define GEN_CDC_DES_FULL               -110136
#define GEN_CDC_DES_NOTFOU             -110137
#define GEN_CDC_EID_FULL               -110138
#define GEN_CDC_EID_NOTFOU             -110139
#define GEN_CDC_UST_FULL               -110140
#define GEN_CDC_UST_NOTFOU             -110141
#define GEN_CDC_EXC_FULL               -110142
#define GEN_CDC_EXC_NOTFOU             -110143
#define GEN_CDC_VDS_FULL               -110144
#define GEN_CDC_VDS_NOTFOU             -110145
#define GEN_CDC_DGR_FULL               -110146
#define GEN_CDC_DGR_NOTFOU             -110147
#define GEN_CDC_DHB_FULL               -110148
#define GEN_CDC_DHB_NOTFOU             -110149
#define GEN_CDC_SST_FULL               -110150
#define GEN_CDC_SST_NOTFOU             -110151
#define GEN_CDC_PRL_FULL               -110152
#define GEN_CDC_PRL_NOTFOU             -110153
#define GEN_CDC_SERIES_STOPPED         -110155
#define GEN_CDC_INV_BLOCK              -110157
#define GEN_CDC_INV_QUANTITY           -110159
#define GEN_CDC_INV_PREMIUM            -110161
#define GEN_CDC_INV_OPEN_CLOSE_REQ     -110163
#define GEN_CDC_CRSCLOSE               -110165
#define GEN_CDC_INVOPENCLOSE           -110167
#define GEN_CDC_MATCH_VALIDITY         -110169
#define GEN_CDC_INVUSER                -110171
#define GEN_CDC_MAX_QUANTITY           -110173
#define GEN_CDC_MIN_OF_SIL             -110175
#define GEN_CDC_MAR_CLOSED             -110177
#define GEN_CDC_BEFORE_TRD_TIME        -110179
#define GEN_CDC_AFTER_TRD_TIME         -110181
#define GEN_CDC_BEFORE_TX_TIME         -110183
#define GEN_CDC_AFTER_TX_TIME          -110185
#define GEN_CDC_CIB_FULL               -110186
#define GEN_CDC_CIB_NOTFOU             -110187
#define GEN_CDC_SID_FULL               -110188
#define GEN_CDC_SID_NOTFOU             -110189
#define GEN_CDC_NTD_FULL               -110190
#define GEN_CDC_NTD_NOTFOU             -110191
#define GEN_CDC_NO_HEDGE_PRICE         -110193
#define GEN_CDC_CRF_FULL               -110194
#define GEN_CDC_CRF_NOTFOU             -110195
#define GEN_CDC_CBX_FULL               -110196
#define GEN_CDC_CBX_NOTFOU             -110197
#define GEN_CDC_WKP_FULL               -110198
#define GEN_CDC_WKP_NOTFOU             -110199
#define GEN_CDC_MAX_MEM_QUANTITY       -110201
#define GEN_CDC_CRO_FULL               -110202
#define GEN_CDC_CRO_NOTFOU             -110203
#define GEN_CDC_CCR_FULL               -110204
#define GEN_CDC_CCR_NOTFOU             -110205
#define GEN_CDC_IXV_FULL               -110206
#define GEN_CDC_IXV_NOTFOU             -110207
#define GEN_CDC_VBS_FULL               -110208
#define GEN_CDC_AEX_FULL               -110210
#define GEN_CDC_AEX_NOTFOU             -110211
#define GEN_CDC_CSA_FULL               -110212
#define GEN_CDC_CSA_NOTFOU             -110213
#define GEN_CDC_SEC_FULL               -110214
#define GEN_CDC_SEC_NOTFOU             -110215
#define GEN_CDC_POD_FULL               -110216
#define GEN_CDC_POD_NOTFOU             -110217
#define GEN_CDC_CUP_FULL               -110218
#define GEN_CDC_CUP_NOTFOU             -110219
#define GEN_CDC_ING_FULL               -110220
#define GEN_CDC_ING_NOTFOU             -110221
#define GEN_CDC_EDL_FULL               -110222
#define GEN_CDC_EDL_NOTFOU             -110223
#define GEN_CDC_SPL_FULL               -110224
#define GEN_CDC_SPL_NOTFOU             -110225
#define GEN_CDC_VSP_FULL               -110226
#define GEN_CDC_VSP_NOTFOU             -110227
#define GEN_CDC_PRICE_NOTINLIM         -110229
#define GEN_CDC_SSS_FULL               -110230
#define GEN_CDC_SSS_NOTFOU             -110231
#define GEN_CDC_ECB_FULL               -110232
#define GEN_CDC_ECB_NOTFOU             -110233
#define GEN_CDC_CBS_NOT_READY          -110235
#define GEN_CDC_OAT_FULL               -110236
#define GEN_CDC_OAT_NOTFOU             -110237
#define GEN_CDC_PQC_FULL               -110238
#define GEN_CDC_PQC_NOTFOU             -110239
#define GEN_CDC_TRC_MEM_NOTFOUND       -110241
#define GEN_CDC_CPA_FULL               -110242
#define GEN_CDC_CPA_NOTFOU             -110243
#define GEN_CDC_ASC_FULL               -110244
#define GEN_CDC_ASC_NOTFOU             -110245
#define GEN_CDC_NOT_TRADED             -110247
#define GEN_CDC_BEFORE_FIRST           -110249
#define GEN_CDC_AFTER_LAST             -110251
#define GEN_CDC_PID_FULL               -110252
#define GEN_CDC_PID_NOTFOU             -110253
#define GEN_CDC_ONLY_IN_EQ_CBO         -110255
#define GEN_CDC_TRC_INS_NOTFOUND       -110257
#define GEN_CDC_TRC_SST_NOTFOUND       -110259
#define GEN_CDC_TRC_UST_NOTFOUND       -110261
#define GEN_CDC_CAC_FULL               -110262
#define GEN_CDC_CAC_NOTFOU             -110263
#define GEN_CDC_CUR_FULL               -110264
#define GEN_CDC_CUR_NOTFOU             -110265
#define GEN_CDC_OPP_FULL               -110266
#define GEN_CDC_OPP_NOTFOU             -110267
#define GEN_CDC_DIO_FULL               -110268
#define GEN_CDC_DIO_NOTFOU             -110269
#define GEN_CDC_RPT_FULL               -110270
#define GEN_CDC_RPT_NOTFOU             -110271
#define GEN_CDC_RTM_FULL               -110272
#define GEN_CDC_RTM_NOTFOU             -110273
#define GEN_CDC_CRP_FULL               -110274
#define GEN_CDC_CRP_NOTFOU             -110275
#define GEN_CDC_MPR_FULL               -110276
#define GEN_CDC_MPR_NOTFOU             -110277
#define GEN_CDC_MPM_FULL               -110278
#define GEN_CDC_MPM_NOTFOU             -110279
#define GEN_CDC_ONLY_TRADE_REPORTING   -110281
#define GEN_CDC_CSE_FULL               -110282
#define GEN_CDC_CSE_NOTFOU             -110283
#define GEN_CDC_HSE_FULL               -110284
#define GEN_CDC_HSE_NOTFOU             -110285
#define GEN_LIST_NAME_NO_MARKET        -135000
#define GEN_LIST_NAME_NO_NAME          -135002
#define GEN_LIST_NAME_NO_TYPE          -135004
#define GEN_LIST_NAME_NOT_AST          -135006
#define GEN_LIST_NAME_TOO_SHORT        -135008
#define GEN_LIST_NAME_DUPL             -135010
#define GEN_LIST_NAME_DUPL_TYP         -135012
#define OC_BIND_FAIL                   -140000
#define OC_BRX_NOT_ALL                 -140001
#define OC_CALLCONN                     140003
#define OC_CLIENT_CONNECT               140005
#define OC_ERROR                       -140006
#define OC_FATAL                       -140008
#define OC_INFO                         140009
#define OC_INVARGS                     -140010
#define OC_INVCHN_TYPE                 -140012
#define OC_INVLEVEL                    -140014
#define OC_INVSRVNAM                   -140016
#define OC_MSG_XMT                      140017
#define OC_NO_EVENTS                    140019
#define OC_NORMAL                       140021
#define OC_PROTOCOLERR                 -140022
#define OC_READ_PRG                     140023
#define OC_SRV_ONLINE                   140025
#define OC_TX_ABORT_INPROG             -140026
#define OC_TX_ALLSRVSHA                -140028
#define OC_TX_ALLSRVSTA                -140030
#define OC_TX_ALRSHADOW                -140032
#define OC_TX_CH_ACTIVE                -140034
#define OC_TX_CH_NOTACTIVE             -140036
#define OC_TX_COMMIT                    140037
#define OC_TX_CONNOTSET                -140038
#define OC_TX_DATTYPDIF                -140040
#define OC_TX_DCLNOTTERM               -140041
#define OC_TX_DELAYTERM                 140043
#define OC_TX_EXINOTCON                -140044
#define OC_TX_INVACCESS                -140046
#define OC_TX_INVFACNAM                -140048
#define OC_TX_INVTXBUFLEN              -140050
#define OC_TX_KEYLENDIF                -140052
#define OC_TX_KEYOFFDIF                -140054
#define OC_TX_KEYOUTREC                -140056
#define OC_TX_KEYRANOVE                -140058
#define OC_TX_MSG_PEND                 -140060
#define OC_TX_NOACCESS                 -140062
#define OC_TX_NOACP                    -140064
#define OC_TX_NONDEQ_MSG                140065
#define OC_TX_NONTRM_READ              -140066
#define OC_TX_NONTRM_SRVCOM            -140067
#define OC_TX_NOSRV_FOR_KEY            -140068
#define OC_TX_NOT_OPEN                 -140070
#define OC_TX_NOTACTIVE                -140072
#define OC_TX_NOTSAMCLU                -140074
#define OC_TX_NOTSAMNOD                -140076
#define OC_TX_NRSEGSDIF                -140078
#define OC_TX_OUTST_EVTSDWN            -140080
#define OC_TX_REQABORT                 -140082
#define OC_TX_REQCALLUN                -140084
#define OC_TX_SHUTCALL                 -140085
#define OC_TX_SHUTDOWN                 -140087
#define OC_TX_START                     140089
#define OC_TX_STARTREC                  140091
#define OC_TX_TOOMANSHA                -140092
#define OC_TX_TRUNCATE                 -140094
#define OC_TX_TWONODALR                -140096
#define OC_TX_UPPLESLOW                -140098
#define OC_TX_VOTE                      140099
#define OC_TRNS_UNSUPP                 -140100
#define OC_TX_JOUOVERFL                -140102
#define OC_TX_JOUFILFUL                -140104
#define OC_TX_JOUFILERR                -140106
#define GEN_TRADE_NOSUBS                175001
#define GEN_SHR_DVPCOMPLETEDNOCANCEL   -197000
#define GEN_SHR_DVPRECALLEDNOCANCEL    -197002
#define GEN_SHR_DVPCANCELLEDNOCANCEL   -197004
#define GEN_SHR_NEEDSAUTHORIZATION      197005
#define GEN_SHR_INVFIXEDINCOMETYPE     -197006
#define GEN_SHR_ILLEGALARGUMENT        -197008
#define GEN_SHR_INVPREMIUMUNIT         -197010
#define GEN_SHR_FIXEDINCOMEPRODUCT     -197012
#define GEN_SHR_FIXEDINCOMEPRICETYPE   -197014
#define GEN_SHR_INVSERIES              -197016
#define GEN_SHR_NOTTMTEMPLATE          -197018
#define GEN_SHR_NOTTMSERIES            -197020
#define GEN_SHR_INTERROR               -197022
#define GEN_SHR_SERIESMISMATCH         -197024
#define GEN_SHR_INVINSID               -197026
#define GEN_SHR_ETRMULTRECEIVE          197027
#define GEN_MDF_VERSION                 199001

/******************************************************************************
*end*of* gen_messages.h
*******************************************************************************/

#endif /* _GEN_MESSAGES_H_ */


#ifndef _IN_MESSAGES_H_
#define _IN_MESSAGES_H_

/******************************************************************************
Module: in_messages.h

Message definitions generated 2022-12-01 07:07:30 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define IDL_BSSDATANOTFOU              -505000
#define IDL_BSSNOTFOU                  -505002
#define IDL_BSSTABFULL                 -505004
#define IDL_COMDATANOTFOU              -505006
#define IDL_COMNOTFOU                  -505008
#define IDL_COMTABFULL                 -505010
#define IDL_EXCDATANOTFOU              -505012
#define IDL_EXCNOTFOU                  -505014
#define IDL_EXCTABFULL                 -505016
#define IDL_INCDATANOTFOU              -505018
#define IDL_INCNOTFOU                  -505020
#define IDL_INCTABFULL                 -505022
#define IDL_INSDATANOTFOU              -505024
#define IDL_INSNOTFOU                  -505026
#define IDL_INSTABFULL                 -505028
#define IDL_INTDATANOTFOU              -505030
#define IDL_INTNOTFOU                  -505032
#define IDL_INTTABFULL                 -505034
#define IDL_MARDATANOTFOU              -505036
#define IDL_MARNOTFOU                  -505038
#define IDL_MARTABFULL                 -505040
#define IDL_SYMDATANOTFOU              -505042
#define IDL_SYMNOTFOU                  -505044
#define IDL_SYMTABFULL                 -505046
#define IDL_TSSDATANOTFOU              -505048
#define IDL_TSSNOTFOU                  -505050
#define IDL_TSSTABFULL                 -505052
#define IDL_VMSDATANOTFOU              -505054
#define IDL_VMSNOTFOU                  -505056
#define IDL_VMSTABFULL                 -505058
#define IDL_VOLDATANOTFOU              -505060
#define IDL_VOLNOTFOU                  -505062
#define IDL_VOLTABFULL                 -505064
#define IDL_BBODATANOTFOU              -505066
#define IDL_BBONOTFOU                  -505068
#define IDL_BBOTABFULL                 -505070
#define IDL_TRCDATANOTFOU              -505072
#define IDL_TRCNOTFOU                  -505074
#define IDL_TRCTABFULL                 -505076
#define IDL_OBKDATANOTFOU              -505078
#define IDL_OBKNOTFOU                  -505080
#define IDL_OBKTABFULL                 -505082
#define IDL_AEXDATANOTFOU              -505084
#define IDL_AEXNOTFOU                  -505086
#define IDL_AEXTABFULL                 -505088
#define IDL_TSEDATANOTFOU              -505090
#define IDL_TSENOTFOU                  -505092
#define IDL_TSETABFULL                 -505094
#define IDL_SSTDATANOTFOU              -505096
#define IDL_SSTNOTFOU                  -505098
#define IDL_SSTTABFULL                 -505100
#define IDL_PTDDATANOTFOU              -505102
#define IDL_PTDNOTFOU                  -505104
#define IDL_PTDTABFULL                 -505106
#define IDL_IPRDATANOTFOU              -505108
#define IDL_IPRNOTFOU                  -505110
#define IDL_IPRTABFULL                 -505112
#define IDL_EIDDATANOTFOU              -505114
#define IDL_EIDNOTFOU                  -505116
#define IDL_EIDTABFULL                 -505118
#define IDL_DHBDATANOTFOU              -505120
#define IDL_DHBNOTFOU                  -505122
#define IDL_DHBTABFULL                 -505124
#define IDL_DESDATANOTFOU              -505126
#define IDL_DESNOTFOU                  -505128
#define IDL_DESTABFULL                 -505130
#define IDL_DGRDATANOTFOU              -505132
#define IDL_DGRNOTFOU                  -505134
#define IDL_DGRTABFULL                 -505136
#define IDL_VDSDATANOTFOU              -505138
#define IDL_VDSNOTFOU                  -505140
#define IDL_VDSTABFULL                 -505142
#define IDL_MEMDATANOTFOU              -505144
#define IDL_MEMNOTFOU                  -505146
#define IDL_MEMTABFULL                 -505148
#define IDL_CBXDATANOTFOU              -505150
#define IDL_CBXNOTFOU                  -505152
#define IDL_CBXTABFULL                 -505154
#define IDL_PSPDATANOTFOU              -505156
#define IDL_PSPNOTFOU                  -505158
#define IDL_PSPTABFULL                 -505160
#define IDL_USRDATANOTFOU              -505162
#define IDL_USRNOTFOU                  -505164
#define IDL_USRTABFULL                 -505166
#define IDL_CBSDATANOTFOU              -505168
#define IDL_CBSNOTFOU                  -505170
#define IDL_CBSTABFULL                 -505172
#define IDL_SBSDATANOTFOU              -505174
#define IDL_SBSNOTFOU                  -505176
#define IDL_SBSTABFULL                 -505178
#define IDL_CORRUPTED                  -505180
#define IDL_CONSISTENT                  505181
#define IDL_WRONGLCK                   -505182
#define IDL_CCRDATANOTFOU              -505184
#define IDL_CCRNOTFOU                  -505186
#define IDL_CCRTABFULL                 -505188
#define IDL_IXVDATANOTFOU              -505190
#define IDL_IXVNOTFOU                  -505192
#define IDL_IXVTABFULL                 -505194
#define IDL_CUPDATANOTFOU              -505196
#define IDL_CUPNOTFOU                  -505198
#define IDL_CUPTABFULL                 -505200
#define IDL_INGDATANOTFOU              -505202
#define IDL_INGNOTFOU                  -505204
#define IDL_INGTABFULL                 -505206
#define IDL_NTDDATANOTFOU              -505208
#define IDL_NTDNOTFOU                  -505210
#define IDL_NTDTABFULL                 -505212
#define IDL_CIBDATANOTFOU              -505214
#define IDL_CIBNOTFOU                  -505216
#define IDL_CIBTABFULL                 -505218
#define IDL_COGDATANOTFOU              -505220
#define IDL_COGNOTFOU                  -505222
#define IDL_COGTABFULL                 -505224
#define IDL_DSCDATANOTFOU              -505226
#define IDL_DSCNOTFOU                  -505228
#define IDL_DSCTABFULL                 -505230
#define IDL_CURNOTFOU                  -505232
#define IDL_CURDATANOTFOU              -505234
#define IDL_CURTABFULL                 -505236
#define IDL_LITNOTFOU                  -505238
#define IDL_LITDATANOTFOU              -505240
#define IDL_LITTABFULL                 -505242
#define IDL_ISSDATANOTFOU              -505244
#define IDL_ISSNOTFOU                  -505246
#define IDL_ISSTABFULL                 -505248
#define IDL_TRTDATANOTFOU              -505250
#define IDL_TRTNOTFOU                  -505252
#define IDL_TRTTABFULL                 -505254
#define IDL_LTUDATANOTFOU              -505256
#define IDL_LTUNOTFOU                  -505258
#define IDL_LTUTABFULL                 -505260
#define IDL_SSSDATANOTFOU              -505262
#define IDL_SSSNOTFOU                  -505264
#define IDL_SSSTABFULL                 -505266
#define IDL_OATDATANOTFOU              -505268
#define IDL_OATNOTFOU                  -505270
#define IDL_OATTABFULL                 -505272
#define IDL_SCRDATANOTFOU              -505274
#define IDL_SCRNOTFOU                  -505276
#define IDL_SCRTABFULL                 -505278
#define IDL_PRLDATANOTFOU              -505280
#define IDL_PRLNOTFOU                  -505282
#define IDL_PRLTABFULL                 -505284
#define IDL_RPSDATANOTFOU              -505286
#define IDL_RPSNOTFOU                  -505288
#define IDL_RPSTABFULL                 -505290
#define INFO_OPERATORLOG               -510000
#define INFO_RESTART_OPERATION          510001
#define INFO_SXBUFFUL                  -510002
#define INFO_SXCSUMERR                 -510004
#define INFO_SXINVEXCCODE              -510006
#define INFO_SXMSGLENERR               -510008
#define INFO_SXMSGUNK                  -510010
#define INFO_SXNOOMX                   -510012
#define INFO_SXRTRYEXH                 -510014
#define INFO_BADTRDSYNCH               -510016
#define INFO_NOTRDSYNCH                -510018
#define INFO_BADORDSYNCH               -510020
#define INFO_INITNOTPASSED             -510022
#define INFO_PRIMTOSOON                -510024
#define INFO_NOTONETOONEMP             -510026
#define INFO_PANIC                     -510028
#define INFO_SERIESNOTFOU              -510030
#define INFO_PRIMARY                    510031
#define INFO_WAITPRIM                   510033
#define INFO_PRIMFOUND                  510035
#define INFO_MODDEPTH                   510037
#define INFO_NOMODDEPTH                 510039
#define INFO_NEWDEAL                    510041
#define INFO_NEWTRADE                   510043
#define INFO_DELDEAL                    510045
#define INFO_SKIPDEAL                   510047
#define INFO_SKIPORDER                  510049
#define INFO_TRDSYNCH                   510051
#define INFO_ORDSYNCH                   510053
#define INFO_DEALNOTFOU                -510055
#define INFO_DUPDEAL                   -510057
#define INFO_DUPORDER                  -510059
#define INFO_ORDERNOTFOU               -510061
#define INFO_SPREADUNSAFE              -510063
#define INFO_INDEXDUNSAFE              -510065
#define INFO_OK                         510067
#define INFO_NYI                        510069
#define INFO_BUGCHECK                  -510070
#define INFO_INSFMEM                   -510072
#define INFO_ABORT                     -510074
#define INFO_SUCCESS                    510075
#define INFO_NOINFO                    -510077
#define INFO_TODAYNOTAVAIL             -510079
#define INFO_BADSEG                    -510080
#define INFO_INVDATEFORM               -510082
#define INFO_INVFROMTODATE             -510084
#define INFO_NOTINMEMDB                 510085
#define INFO_NOINFOUV                  -510087
#define INFO_NOINFOINS                 -510089
#define INFO_NODEFSERVICES             -510091
#define INFO_MARGINQUOTESUPD           -510093
#define INFO_MARGINQUOTESVIEWED        -510095
#define INFO_IVBUFLEN                  -510096
#define INFO_FILENOTFOUND              -510098
#define INFO_NO_DATA_RETRY             -510099
#define INFO_FORCEDROLL                -510100
#define INFO_DB_RETRY_EXHAUSTED        -510102
#define INFO_TRAVERSEND                -510104
#define INFO_RTRERROR                  -510106
#define INFO_PRIMGROUPSHUT             -510108
#define INFO_COMPLETE                   510109
#define INFO_SSLNOCLOSE                -510111
#define INFO_SSLWVERISON               -510112
#define INFO_SSLNOLOGFILE              -510113
#define INFO_SSLNOTLOGENA              -510115
#define INFO_SSLNOMOUNT                -510116
#define INFO_SSLNOCHANNEL              -510117
#define INFO_SSLCALLBCKREG             -510118
#define INFO_SSLRICNOTOPEN             -510119
#define INFO_SSLNODISPATCH             -510120
#define INFO_SSLDISCONNECT             -510122
#define INFO_SSLRECONNECT               510123
#define INFO_SSLRICICLOSED              510125
#define INFO_SSLRICINFO                 510127
#define INFO_SSLRICOK                   510129
#define INFO_SSLRICSTALE               -510131
#define INFO_SSLEVUNK                   510133
#define INFO_SSLDISMOUNT               -510135
#define INFO_SSLNOWALIDD               -510136
#define INFO_NOTAWAYMARKET             -510138
#define INFO_NOSERIESTERM               510139
#define INFO_OPERNOTACC                -510140
#define INFO_OPERNYI                   -510142
#define INFO_UNKNOWNOPER               -510144
#define INFO_NEWFIRSTDEAL               510145
#define INFO_DELFIRSTDEAL               510147
#define INFO_DELLASTDEAL                510149
#define INFO_NOTALLOCATED              -510150
#define INFO_NOHOLDBACK                 510151
#define INFO_RETRANSNOTHAND            -510153
#define INFO_ONEINVUNDERLYING          -510155
#define INFO_MODULENOTINIT             -510156
#define INFO_PTDNOTUNIQUE              -510157
#define INFO_INCNOTFOUND               -510159
#define INFO_NOREFTOTIMER              -510161
#define INFO_MODBEST                    510163
#define INFO_CINTNOTFOU                 510165
#define INFO_INSNOTFOU                  510167
#define INFO_BO31NOTADDED               510169
#define INFO_BO35NOTADDED               510171
#define INFO_BD2NOTADDED                510173
#define INFO_MISSINGTIMERREF            510175
#define INFO_HBINTTOSHORT               510177
#define INFO_IPRNOTFOU                 -510178
#define INFO_BO3XNOTADDED               510179
#define INFO_INSNOTADDED                510181
#define INFO_CINTNOTADDED               510183
#define INFO_NOHDLYIELDDIFF            -510184
#define INFO_FUNCNOTSUP                -510186
#define INFO_HBBDCLATE                 -510188
#define INFO_ABORTSTATEREADY           -510190
#define INFO_ABORTNOMATCH              -510192
#define INFO_DATALCKBYSIGNAL           -510194
#define INFO_NOVALUNDERLYING           -510195
#define INFO_ONEINSNOOFFPRICE          -510196
#define INFO_ILLORDERTYPE               510197
#define INFO_PATCHNODEALS              -510198
#define INFO_PATCHDEALGONE             -510200
#define INFO_PATCHVOLERROR             -510202
#define INFO_DUALOPRALINEUSE            510203
#define INFO_TSISCOMBO                  510205
#define INFO_COLFAILNOSOURCE           -510206
#define INFO_NOITEMS                   -510208
#define INFO_INVALIDPRICESOURCE        -510210
#define INFO_MISSINGGROUP              -510212
#define INFO_DIRTYFUTURE               -510214
#define INFO_DIRTYWI                   -510216
#define INFO_DIRTYMONEY                -510218
#define INFO_INDICATIVEPRICETYPE       -510220
#define INFO_INVALIDPRICETYPE          -510222
#define INFO_CALCRULENOTAPLUND         -510224
#define INFO_NOTFOU                    -510226
#define INFO_ABORTSTATEUNDO            -510228
#define INFO_INVSUBITEMHDRSIZE         -510230
#define INFO_INVSUBITEMHDR             -510232
#define INFO_SUBITEMDROPPED            -510234
#define INFO_INVNAMEDSTRUCT            -510236
#define INFO_INVVITSIZE                -510238
#define INFO_INVVIQSIZE                -510240
#define INFO_INVVITITEMS               -510242
#define INFO_INVVIQITEMS               -510244
#define INFO_INVSUBITEMCOM             -510246
#define INFO_INVSUBITEMSTATUS          -510248
#define INFO_NOVALSUBITEMS             -510250
#define INFO_ONEINVSUBITEM             -510252
#define INFO_ONOFFOUTRANGE             -510254
#define INFO_WRNGEXTPRV                -510255
#define INFO_INVPRICE                  -510256
#define INFO_INVVOLUME                 -510258
#define INFO_INVMESSAGE                -510260
#define INFO_NOTISSUER                 -510262
#define INFO_INTSYN                    -510264
#define INFO_INTSEM                    -510266
#define INFO_BDCSYN                    -510268
#define INFO_BDCSEM                    -510270
#define INFO_ERRORCODE                 -510272
#define INFO_INVINSTRUMENT             -510274
#define INFO_INVSETTLETYPE             -510276
#define INFO_INVDEALSOURCE             -510278
#define INFO_INSNOTALLOWED             -510280
#define INFO_NOTRDYTRYLATER            -510282
#define INFO_HISDATENOTALWD            -510284
#define INFO_INVCRULEUSEWCALC          -510286
#define INFO_INVTIMEFORM               -510288
#define INFO_INVAONFIELD               -510290
#define INFO_INVTRENDIND               -510292
#define INFO_INVOPTRDHOLIDAY           -510294
#define INFO_TOMANYAGVLEVELS           -510296
#define INFO_AGVLEVELDROPPED           -510298
#define INFO_INDEXVALIDFAIL            -510300
#define INFO_INVINDEXOPER              -510302
#define INFO_RESETIXHILOFAIL           -510304
#define INFO_INVACCEXCHRATE            -510306
#define INFO_TXKEEPALIVE                510307
#define INFO_INDEXCALCFAIL             -510308
#define INFO_INDEXJOURNALFAIL          -510310
#define INFO_INDEXINDICESLOGFAIL       -510312
#define INFO_NOTSUPEAS                 -510314
#define INFO_ONEFMPUNDERLYING          -510316
#define INFO_STRTRUNCATED              -510317
#define INFO_SLAEXCEEDED               -510319
#define INFO_SPINVFORTHEOALG           -510320
#define INFO_SPINVFORALG               -510322
#define INFO_SPTOMANYMATCHES           -510324
#define INFO_SPINVALGO                 -510326
#define INFO_SPINVALGOFORPROD          -510328
#define INFO_SPINVASLOCKED             -510330
#define INFO_SPGENERICERROR            -510332
#define INFO_SPINVMANUALLYFLAG         -510334
#define INFO_SPMIXFUTOPT               -510336
#define INFO_SPNOTALLOPT               -510338
#define INFO_SPINVCLEARINGDATE         -510340
#define INFO_SPRULEOUTOFRANGE          -510342
#define INFO_SPOPTEXPANDMAXLIMIT       -510344
#define INFO_SPINVALGOPARAM            -510346
#define INFO_EMERGSPLOCKED             -510348
#define INFO_SPINVSOURCE               -510350
#define INFO_SPMIXDERUPP               -510352
#define INFO_SPNOUPPERLP               -510354
#define INFO_SPCANCALCINTRINSIC        -510356
#define INFO_SPSAVECALCINPROG          -510358
#define INFO_SPSAVELCK                 -510360
#define INFO_SPSAVENEWCDAY             -510362
#define INFO_SEGFAULTW1                -510364
#define INFO_SEGFAULTW2                -510366
#define INFO_WRONGCHILDSTATE           -510368
#define INFO_TXDUPITEM                 -510370
#define INFO_IS109PROTOCOL             -510372
#define INFO_NOTAUTHORIZEDINS          -510373
#define INFO_BO6SEQNBRDROP              510375
#define INFO_SQLERROR                  -510376
#define INFO_OBPOSITIONERROR           -510378
#define INFO_BLACKLISTED               -510380
#define INFO_INVREFPRICETYPE           -510382
#define PMI_GENRSPABN                  -515000
#define PMI_MALLOCERR                  -515002
#define PMI_LISTRSPABN                 -515004
#define PMI_INITRSPABN                 -515006
#define PMI_NOFREEBLOCK                -515008
#define PMI_NOINCFOUND                 -515010
#define PMI_NOINSFOUND                 -515012
#define PMI_NOUNDFOUND                 -515014
#define PMI_PMISTSABN                  -515015
#define PMI_QANSWOVERFLOW              -515016
#define INREU_INVMSGFMT                -520000
#define INREU_JOKE                     -520002
#define INREU_CFGCMDLNG                -520004
#define INREU_NOCFGFIL                 -520006
#define INREU_NOCRSREF                 -520008
#define INREU_NORICDEF                 -520010
#define INREU_NOPARENTDE               -520012
#define INREU_NOCOMCODDEF              -520014
#define INREU_INVBSTDEF                -520016
#define INREU_ONEBSTDEF                -520018
#define INREU_OMXRECDEF                -520020
#define INREU_NOOMXREC                 -520022
#define INREU_OMXNOTDEF                -520024
#define INREU_OMXNOFLD                 -520026
#define INREU_OMXDEF                   -520028
#define INREU_OMXONEFLD                -520030
#define INREU_OMXOVRLAP                -520032
#define INREU_OMXDBLFLDNO              -520034
#define INREU_INVBDXTYP                -520036
#define INREU_CONNFAIL                 -520038
#define INREU_DCLRCF                   -520040
#define INREU_DCLRTF                   -520042
#define INREU_ISINLEN                  -520044
#define INREU_INVSTSMSG                -520046
#define INREU_ASYINTERR                -520048
#define INREU_ASYDEVACT                -520050
#define INREU_ASYNOVLDCNTX             -520052
#define INREU_ASYNOSTOP                -520054
#define INREU_INVEXPFLD                -520056
#define INREU_INVINSTTYPFLD            -520058
#define INREU_INVTIMFLD                -520060
#define INREU_INVDATFLD                -520062
#define INREU_INVRECSIZ                -520064
#define INREU_BDXNOTDEF                -520066
#define INREU_REUSTS                   -520068
#define INREU_REUPROTERR               -520070
#define INREU_INVCRF                   -520072
#define INREU_NOSERIEDEF               -520074
#define INREU_NOINDEXDEF               -520076
#define INREU_NORMAL                    520077
#define INREU_WRNGSEQNBR               -520079
#define OPRA_CACH_INS_FULL             -525000
#define OPRA_CACH_CINT_FULL            -525002
#define OPRA_CACH_GRP_FULL             -525004
#define OPRA_CACH_EXC_FULL             -525006
#define OPRA_CACH_INS_NOT_RDY          -525008
#define OPRA_CACH_CINT_NOT_RDY         -525010
#define OPRA_CACH_GROUP_NOT_RDY        -525012
#define OPRA_CACH_EXC_NOT_RDY          -525014
#define OPRA_CACH_INSNOTFOU            -525016
#define IN_CQATS_FILEOPRFAILED         -530001
#define IN_CQATS_NORMAL                 530003
#define IN_CQATS_FEED_UNRELIABLE       -530004
#define IN_CQATS_NEW_UNKNOWN_MESSAGE    530005
#define IN_CQATS_UNEXP_SEQUENCE_NUMBER -530007
#define IN_CQATS_CORRECTION_FAILURE    -530009
#define IN_CQATS_CANCEL_FAILURE        -530011
#define IN_ULFEED_FILEOPRFAILED        -535001
#define IN_ULFEED_NORMAL                535003
#define IN_ULFEED_FEED_UNRELIABLE      -535004
#define IN_ULFEED_NEW_UNK_MESSAGE       535005
#define IN_ULFEED_UNEXP_SEQ_NUMBER     -535007
#define IN_ULFEED_CORRECTION_FAILURE   -535009
#define IN_ULFEED_CANCEL_FAILURE       -535011
#define IN_ULFEED_COMNOTFND             535013
#define IN_MDF_VERSION                  540001

/******************************************************************************
*end*of* in_messages.h
*******************************************************************************/

#endif /* _IN_MESSAGES_H_ */


#ifndef _ME_MESSAGES_H_
#define _ME_MESSAGES_H_

/******************************************************************************
Module: me_messages.h

Message definitions generated 2022-12-01 07:07:30 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define ME_MDF_VERSION                  401001
#define ME_COUNTERS_NOT_INIT           -405000
#define ME_INFORMATION                  405001
#define ME_SUCCESS                      405003
#define ME_PANIC                       -405004
#define ME_INSFMEM                     -405006
#define ME_BUGCHECK                    -405007
#define ME_QUOTA                       -405009
#define ME_IVBUFLEN                    -405010
#define ME_ABORT                       -405012
#define ME_COMMIT_LOG_UNKNOWN          -405014
#define ME_INVINT                      -405016
#define ME_NOHIT                       -405017
#define ME_LISEMPTY                    -405019
#define ME_OVERFLOW                    -405021
#define ME_RECEXCLEN                   -405022
#define ME_WRNGFTYP                    -405024
#define ME_OPLOG_FULL                  -405026
#define ME_FILE_NOT_FOUND              -405028
#define ME_NOT_ALLW_ALTER_ORDER        -405029
#define ME_NOT_EXP_RESULT              -405031
#define ME_INV_USR                     -405033
#define ME_FILE_LOCKED                 -405035
#define ME_EOF                          405037
#define ME_BUFFER_SMALL                -405039
#define ME_QUEUE_ALMOST_FULL            405041
#define ME_NO_PQR_IN_UND               -405042
#define ME_MARKET_MAKER_PROTECTION     -405044
#define ME_MARKET_MAKER_PROTECTION_SET  405045
#define ME_MARKET_MAKER_TRADE           405047
#define ME_MM_VOLUME_PROTECTION        -405048
#define ME_MM_DELTA_PROTECTION         -405050
#define ME_MM_PROTECTION_LIST_END       405051
#define ME_NOT_YET_INITIALIZED         -405052
#define ME_INFO_ERROR_STATE            -410000
#define ME_INFO_LOG_TOO_SMALL          -410002
#define ME_INFO_NO_TIMESTAMP           -410004
#define ME_INFO_LOG_UNKNOWN            -410006
#define ME_INFO_LOG_WRONG_SIZE         -410008
#define ME_INFO_TOO_MANY_DEALS         -410010
#define ME_INFO_POSS_DUPL              -410011
#define ME_INFO_TOO_MANY_LOGS          -410012
#define ME_INFO_UNDEFINED_STATE        -410014
#define ME_INFO_DEAL_DELAYED           -410015
#define ME_LOG_ALREADY_INIT             415001
#define ME_LOG_NOT_INITIALIZED         -415002
#define ME_LOG_NOT_READ_MODE           -415004
#define ME_LOG_NOT_WRITE_MODE          -415006
#define ME_LOG_END_OF_FILE             -415007
#define ME_LOG_UNDEFINED_MODE          -415008
#define ME_MATCH_ALRLOAD               -420001
#define ME_MATCH_DEFMAX                -420003
#define ME_MATCH_DISCARDED             -420005
#define ME_MATCH_ILL_CRS               -420007
#define ME_MATCH_MIN_QUANTITY          -420009
#define ME_MATCH_PREMIUM_REQ           -420011
#define ME_MATCH_TIMEOUT               -420013
#define ME_MATCH_INV_ALTER             -420015
#define ME_MATCH_NOT_IMPL              -420017
#define ME_MATCH_OVERBID               -420019
#define ME_MATCH_UNKNOWN               -420021
#define ME_MATCH_BID_ASK               -420023
#define ME_MATCH_VALIDITY              -420025
#define ME_MATCH_MARBOUNCE             -420027
#define ME_MATCH_PREMIUM               -420029
#define ME_MATCH_STOPPED               -420031
#define ME_MATCH_AONBOUNCE             -420033
#define ME_MATCH_BLOCK                 -420035
#define ME_MATCH_COMBO_MAX_LEGS        -420037
#define ME_MATCH_COMBO_PART            -420039
#define ME_MATCH_COMBO_SAVE            -420041
#define ME_MATCH_INV_COMBO_SIZE        -420043
#define ME_MATCH_QUANTITY              -420045
#define ME_MATCH_ORDER_EXISTS          -420047
#define ME_MATCH_NOT_AT_MARKET         -420049
#define ME_MATCH_LOW_VOLUME            -420051
#define ME_MATCH_BROKER_SPREAD         -420053
#define ME_MATCH_ILL_INTERBANK         -420055
#define ME_MATCH_ACC_PREMIUM           -420057
#define ME_MATCH_ACC_ORDER             -420059
#define ME_MATCH_COMBO_LEG             -420061
#define ME_MATCH_LEG_PRICE_NOT_0       -420063
#define ME_MATCH_LEG_NOT_FOUND         -420065
#define ME_MATCH_QUANT_NE_LEG          -420067
#define ME_MATCH_COMBO_SPREAD          -420069
#define ME_MATCH_SER_IS_EXPIRED        -420071
#define ME_MATCH_VOLUME_REQ            -420073
#define ME_MATCH_CROSS_NO_STDCOMB      -420074
#define ME_MATCH_ILL_ORDER_TYPE        -420076
#define ME_MATCH_MKT_ORDER_PRICE       -420078
#define ME_MATCH_NOCLS_PRM             -420079
#define ME_MATCH_FILL_BAL              -420081
#define ME_MATCH_COP_LESS              -420083
#define ME_MATCH_COP_VAR               -420085
#define ME_MATCH_PRE_TIME              -420087
#define ME_MATCH_INV_EXP               -420089
#define ME_MATCH_CROSS_PRICE           -420091
#define ME_MATCH_BLOCK_MAX_LEGS        -420093
#define ME_MATCH_BLOCK_SERIES          -420095
#define ME_MATCH_LMT_NOT_VAL           -420097
#define ME_MATCH_MKT_NOT_VAL           -420099
#define ME_MATCH_KIL_NOT_VAL           -420101
#define ME_MATCH_EXISTS_AS_STD_COMBO   -420103
#define ME_MATCH_EXISTS_AS_REV_STD_CBS -420105
#define ME_MATCH_TM_COMBO_EXISTS       -420107
#define ME_MATCH_REV_TM_COMBO_EXISTS   -420109
#define ME_MATCH_TOO_MANY_TM_COMBOS    -420111
#define ME_MATCH_ILL_RATIO             -420113
#define ME_MATCH_ILL_BUY_SELL_OP       -420115
#define ME_MATCH_COMBO_DIFF_CONTR_SIZE -420117
#define ME_MATCH_NO_BB_WHEN_TM_CBS_P   -420119
#define ME_MATCH_DUPL_SERIES           -420121
#define ME_MATCH_NOT_AUTHORIZED        -420123
#define ME_MATCH_NOTHEO_PRM            -420125
#define ME_MATCH_MIXED_COM             -420127
#define ME_MATCH_MEM_CRS_NOT_ALLOWED   -420129
#define ME_MATCH_PRICE_LIMIT           -420131
#define ME_MATCH_INV_STP_COND          -420133
#define ME_MATCH_INV_HIDDEN            -420135
#define ME_MATCH_INV_SHOWN             -420137
#define ME_MATCH_NOT_AUTH              -420139
#define ME_MATCH_SHOWN_TOO_SMALL       -420141
#define ME_MATCH_AONNOTVALID           -420143
#define ME_MATCH_INV_DELTA             -420145
#define ME_MATCH_FOK_NOT_WKUP_OWN      -420147
#define ME_MATCH_AON_CROSS_PRICE       -420149
#define ME_MATCH_LEG_PREMIUM           -420151
#define ME_MATCH_NO_WILD_CARD          -420153
#define ME_MATCH_INV_OP_CLS_REQ        -420155
#define ME_MATCH_INV_ORIGIN            -420157
#define ME_MATCH_INV_ACCOUNT           -420159
#define ME_MATCH_INV_CTI_CODE          -420161
#define ME_MATCH_NOT_AT_BEST           -420163
#define ME_MATCH_PRICE_LIMIT_LEG       -420165
#define ME_MATCH_PRICE_DEV_LEG         -420167
#define ME_MATCH_PRICE_LEG             -420169
#define ME_MATCH_PRICE_RANGE           -420171
#define ME_MATCH_NOT_TIME_YET          -420173
#define ME_MATCH_NO_ACTION              420175
#define ME_MATCH_ORD_NOT_FOU           -420177
#define ME_MATCH_CB_LIMIT_LAST         -420179
#define ME_MATCH_CB_LIMIT_REF          -420181
#define ME_MATCH_PERC_OR_ABS           -420183
#define ME_MATCH_BLMT_ORDER_PRICE      -420184
#define ME_MATCH_BLMT_NOT_VAL          -420185
#define ME_MATCH_BLMT_NO_PREM          -420187
#define ME_MATCH_NOT_IN_WORKUP         -420189
#define ME_MATCH_IN_WORKUP             -420191
#define ME_MATCH_INS_FILL_OR_KILL      -420193
#define ME_MATCH_SST_FILL_OR_KILL      -420195
#define ME_MATCH_USR_FILL_OR_KILL      -420197
#define ME_MATCH_INS_FILL_AND_KILL     -420199
#define ME_MATCH_SST_FILL_AND_KILL     -420201
#define ME_MATCH_USR_FILL_AND_KILL     -420203
#define ME_MATCH_INS_FILL_AND_STORE    -420205
#define ME_MATCH_SST_FILL_AND_STORE    -420207
#define ME_MATCH_USR_FILL_AND_STORE    -420209
#define ME_MATCH_INS_LIMIT_ORD         -420211
#define ME_MATCH_SST_LIMIT_ORD         -420213
#define ME_MATCH_USR_LIMIT_ORD         -420215
#define ME_MATCH_INS_MARKET_ORD        -420217
#define ME_MATCH_SST_MARKET_ORD        -420219
#define ME_MATCH_USR_MARKET_ORD        -420221
#define ME_MATCH_INS_ALL_OR_NONE       -420223
#define ME_MATCH_SST_ALL_OR_NONE       -420225
#define ME_MATCH_USR_ALL_OR_NONE       -420227
#define ME_MATCH_INV_INT_HIDDEN        -420229
#define ME_MATCH_OID_REQUIRED          -420231
#define ME_MATCH_ODD_LOT_EXP           -420233
#define ME_MATCH_ILL_ORDER_TYPE_SST    -420235
#define ME_MATCH_ILL_ORDER_TYPE_INT    -420237
#define ME_MATCH_ILL_ORDER_TYPE_USR    -420239
#define ME_MATCH_MIN_VOL_TRC           -420241
#define ME_MATCH_MAX_VOL_TRC           -420243
#define ME_MATCH_INS_IMBALANCE         -420245
#define ME_MATCH_SST_IMBALANCE         -420247
#define ME_MATCH_USR_IMBALANCE         -420249
#define ME_MATCH_INS_MTL_ROUND_LOT     -420251
#define ME_MATCH_SST_MTL_ROUND_LOT     -420253
#define ME_MATCH_USR_MTL_ROUND_LOT     -420255
#define ME_MATCH_INS_MTL_COMBO_R_LOT   -420257
#define ME_MATCH_SST_MTL_COMBO_R_LOT   -420259
#define ME_MATCH_USR_MTL_COMBO_R_LOT   -420261
#define ME_MATCH_INS_ODD_LOT           -420263
#define ME_MATCH_SST_ODD_LOT           -420265
#define ME_MATCH_USR_ODD_LOT           -420267
#define ME_MATCH_INS_LIMIT_ODD_LOT     -420269
#define ME_MATCH_SST_LIMIT_ODD_LOT     -420271
#define ME_MATCH_USR_LIMIT_ODD_LOT     -420273
#define ME_MATCH_INS_MARKET_ODD_LOT    -420275
#define ME_MATCH_SST_MARKET_ODD_LOT    -420277
#define ME_MATCH_USR_MARKET_ODD_LOT    -420279
#define ME_MATCH_INS_MTL_ODD_LOT       -420281
#define ME_MATCH_SST_MTL_ODD_LOT       -420283
#define ME_MATCH_USR_MTL_ODD_LOT       -420285
#define ME_MATCH_INS_HIDDEN_AGGRESSIVE -420287
#define ME_MATCH_SST_HIDDEN_AGGRESSIVE -420289
#define ME_MATCH_USR_HIDDEN_AGGRESSIVE -420291
#define ME_MATCH_INS_HIDDEN_PASSIVE    -420293
#define ME_MATCH_SST_HIDDEN_PASSIVE    -420295
#define ME_MATCH_USR_HIDDEN_PASSIVE    -420297
#define ME_MATCH_OVERRIDE_CRS          -420299
#define ME_MATCH_MATCHED_CRS_PRICE     -420301
#define ME_MATCH_BID_ORDER             -420303
#define ME_MATCH_ASK_ORDER             -420305
#define ME_MATCH_FORCE_ORDER           -420307
#define ME_MATCH_ILL_ORDER_TYPE_TX     -420309
#define ME_MATCH_MIN_BLK_SIZE          -420311
#define ME_MATCH_MAX_BLK_SIZE          -420313
#define ME_MATCH_INS_DECR_SHOWN_NOHIDD -420315
#define ME_MATCH_INS_DECR_SHOWN        -420317
#define ME_MATCH_INS_DECR_HIDD         -420319
#define ME_MATCH_INS_INCR_SHOWN        -420321
#define ME_MATCH_INS_INCR_HIDD         -420323
#define ME_MATCH_INS_CLIENT            -420325
#define ME_MATCH_INS_PRICE_IMPR        -420327
#define ME_MATCH_INS_PRICE_DISIMPR     -420329
#define ME_MATCH_INS_BLOCK             -420331
#define ME_MATCH_INS_TIME_EXT          -420333
#define ME_MATCH_INS_TIME_DECR         -420335
#define ME_MATCH_INS_ORDER_TYPE        -420337
#define ME_MATCH_INS_EXCH_ORDER_TYPE   -420339
#define ME_MATCH_ALTER_BALANCE         -420341
#define ME_MATCH_SST_DECR_SHOWN_NOHIDD -420343
#define ME_MATCH_SST_DECR_SHOWN        -420345
#define ME_MATCH_SST_DECR_HIDD         -420347
#define ME_MATCH_SST_INCR_SHOWN        -420349
#define ME_MATCH_SST_INCR_HIDD         -420351
#define ME_MATCH_SST_CLIENT            -420353
#define ME_MATCH_SST_PRICE_IMPR        -420355
#define ME_MATCH_SST_PRICE_DISIMPR     -420357
#define ME_MATCH_SST_BLOCK             -420359
#define ME_MATCH_SST_TIME_EXT          -420361
#define ME_MATCH_SST_TIME_DECR         -420363
#define ME_MATCH_SST_ORDER_TYPE        -420365
#define ME_MATCH_SST_EXCH_ORDER_TYPE   -420367
#define ME_MATCH_CRS_PRIO_NO_ORDER     -420369
#define ME_MATCH_SST_PST_ORDER         -420371
#define ME_MATCH_INS_PST_ORDER         -420373
#define ME_MATCH_USR_PST_ORDER         -420375
#define ME_MATCH_SST_SHORT_ORDER       -420377
#define ME_MATCH_INS_SHORT_ORDER       -420379
#define ME_MATCH_USR_SHORT_ORDER       -420381
#define ME_MATCH_SST_MB_ORDER          -420383
#define ME_MATCH_INS_MB_ORDER          -420385
#define ME_MATCH_USR_MB_ORDER          -420387
#define ME_MATCH_SST_BL_ORDER          -420389
#define ME_MATCH_INS_BL_ORDER          -420391
#define ME_MATCH_USR_BL_ORDER          -420393
#define ME_MATCH_LAST_PRICE            -420395
#define ME_MATCH_MB_SELL               -420397
#define ME_MATCH_MB_ORDER_TYPE         -420399
#define ME_MATCH_SH_BUY                -420401
#define ME_MATCH_SH_TKO                -420403
#define ME_MATCH_MTL_NO_OPPOSING       -420405
#define ME_MATCH_COMBO_PRACTICE        -420407
#define ME_MATCH_COMBO_EQTY_NOT_MULT   -420409
#define ME_MATCH_RELOAD_NOT_DONE       -420410
#define ME_MATCH_FOK_FAK_NOT_ALLOWD    -420411
#define ME_MATCH_VOL_VAL_TRC           -420413
#define ME_MATCH_BEST_LIMIT_REQ        -420415
#define ME_MATCH_SER_WILD_CARD         -420417
#define ME_MATCH_INV_ACTIVATION        -420419
#define ME_MATCH_INV_CLEARING_PART     -420421
#define ME_MATCH_INV_E_ORD_TYPE_PRIO   -420423
#define ME_MATCH_INV_ALWAYS_INACTIVE   -420425
#define ME_MATCH_ILL_EXCH_ORDER_TYPE   -420426
#define ME_MATCH_MM_PRIO_LEVEL_EXC     -420427
#define ME_MATCH_STOP_PREMIUM          -420429
#define ME_MATCH_SST_SSO_OPEN_END      -420431
#define ME_MATCH_INS_SSO_OPEN_END      -420433
#define ME_MATCH_USR_SSO_OPEN_END      -420435
#define ME_MATCH_SST_SSO_CLOSE_END     -420437
#define ME_MATCH_INS_SSO_CLOSE_END     -420439
#define ME_MATCH_USR_SSO_CLOSE_END     -420441
#define ME_MATCH_SST_SSO               -420443
#define ME_MATCH_INS_SSO               -420445
#define ME_MATCH_USR_SSO               -420447
#define ME_MATCH_NO_SST_FOR_SSO        -420449
#define ME_MATCH_SSO_SPEC_NO_TYPE      -420451
#define ME_MATCH_SST_EXT_T_NOT_VALID   -420453
#define ME_MATCH_SST_EXT_T_STATE       -420455
#define ME_MATCH_INS_EXT_T_STATE       -420457
#define ME_MATCH_INS_STOP_ORD          -420459
#define ME_MATCH_SST_STOP_ORD          -420461
#define ME_MATCH_USR_STOP_ORD          -420463
#define ME_MATCH_INS_CRS_SPREAD_MAR    -420465
#define ME_MATCH_SST_CRS_SPREAD_MAR    -420467
#define ME_MATCH_USR_CRS_SPREAD_MAR    -420469
#define ME_MATCH_INS_CRS_SPREAD_LIM    -420471
#define ME_MATCH_SST_CRS_SPREAD_LIM    -420473
#define ME_MATCH_USR_CRS_SPREAD_LIM    -420475
#define ME_MATCH_INS_UNDISCLOSED_QTY   -420477
#define ME_MATCH_SST_UNDISCLOSED_QTY   -420479
#define ME_MATCH_USR_UNDISCLOSED_QTY   -420481
#define ME_MATCH_MIN_UNDISCLOSED       -420483
#define ME_MATCH_CP_NO_HIDDEN          -420485
#define ME_MATCH_CP_LIMIT              -420487
#define ME_MATCH_NO_CP_PRICE           -420489
#define ME_MATCH_MIN_VOLUME_MATCH      -420491
#define ME_MATCH_MIN_VOLUME_FIX        -420493
#define ME_MATCH_FIX_PREMIUM_DIFF      -420495
#define ME_MATCH_UNDSC_NO_HIDDEN       -420497
#define ME_MATCH_UNDSC_NO_MARKET       -420499
#define ME_MATCH_INTCRSONUSER          -420501
#define ME_MATCH_MUST_BE_QUOTE         -420503
#define ME_MATCH_ILL_NAMED_STRUCTURE   -420505
#define ME_MATCH_INCONSISTENT_VIM      -420507
#define ME_MATCH_ILL_QUOTE_ACTION      -420509
#define ME_MATCH_ILL_PRICE_QUANTITY    -420511
#define ME_MATCH_INV_ORDER_CAPACITY    -420513
#define ME_MATCH_INV_ALT_CLEARING      -420515
#define ME_MATCH_MAX_QUANTITY_OSA      -420517
#define ME_MATCH_DUPL_NAMED_STRUCT     -420519
#define ME_MATCH_MAND_NAMED_STRUCT     -420521
#define ME_MATCH_LINKED_ORDER_CONT     -420523
#define ME_MATCH_LINKED_ORDER_DUPL_SER -420525
#define ME_MATCH_LINKED_ORDER_MIN_LEGS -420527
#define ME_MATCH_LINKED_ORDER_MAX_LEGS -420529
#define ME_MATCH_INV_ALTER_LINKED      -420531
#define ME_MATCH_NOT_SAME_LOTS         -420533
#define ME_MATCH_ONLY_ONE_ORDER        -420535
#define ME_MATCH_INV_DELTA_QUANTITY    -420537
#define ME_MATCH_HOLD_IN_MARKET        -420539
#define ME_MATCH_USER_NO_TRADE         -420541
#define ME_MATCH_IPMO_RESTRICTIONS     -420543
#define ME_MATCH_MULTI_PRICE_TYPE      -420545
#define ME_MATCH_NO_COMBO_IN_MULTI     -420547
#define ME_MATCH_MULTI_ORDER_DUPL_SER  -420549
#define ME_MATCH_PRICE_TYPE_SAME_SIDE  -420551
#define ME_MATCH_INV_CALC_QUANTITY     -420553
#define ME_MATCH_PRICE_TYPE_SAME_UNIT  -420555
#define ME_MATCH_MULTI_ORDER_MIN_LEGS  -420557
#define ME_MATCH_MULTI_ORDER_MAX_LEGS  -420559
#define ME_MATCH_MULTI_ORDER_CONT      -420561
#define ME_MATCH_PRICE_TYPE_ILL_UNIT   -420563
#define ME_MATCH_PRICE_TYPE_ILL_PQF    -420565
#define ME_MATCH_ORDER_SIZE_TOO_LARGE  -420567
#define ME_MATCH_USR_RISK_BREACHED     -420569
#define ME_MATCH_MAX_ORDER_RATE        -420571
#define ME_MATCH_ORDER_SUBMISSION      -420573
#define ME_MATCH_NO_SPONSOR            -420575
#define ME_MATCH_MUST_BE_ORDER         -420577
#define ME_MATCH_ORDER_VALUE_TOO_LARGE -420579
#define ME_MATCH_ORDER_SIZE_GT_AVAILQ  -420581
#define ME_MATCH_NO_EXCHANGE_INFO      -420583
#define ME_MATCH_CLIENT_CAT_INVALID    -420585
#define ME_MATCH_EXCLIENT_BLANK        -420587
#define ME_MATCH_ILL_PREMIUM_FIXED     -420589
#define ME_MATCH_NO_FIXED_PRICE        -420591
#define ME_MATCH_FIXED_PRICE_COMBO     -420593
#define ME_MATCH_ILL_TIME_FIXED_MARKET -420595
#define ME_MATCH_TRP_FIXED_LINKED_LTP  -420597
#define ME_MATCH_SST_PR_ORDER          -420599
#define ME_MATCH_INS_PR_ORDER          -420601
#define ME_MATCH_USR_PR_ORDER          -420603
#define ME_MATCH_ILL_PR_VALUE          -420605
#define ME_MATCH_SH_COMPRICE           -420607
#define ME_MATCH_MKT_BLMT_NOPRC_EXIST  -420609
#define ME_MATCH_SH_ORDTYPE            -420611
#define ME_MATCH_SH_LTP_NEUFALL        -420613
#define ME_MATCH_RFQ_NOT_LEGITIMATE    -420615
#define ME_MATCH_RFQ_RESPONSE_ILLEGAL  -420617
#define ME_MATCH_MIN_VALUE_MIDPOINT    -420619
#define ME_MATCH_SST_MIDPOINT          -420621
#define ME_MATCH_SH_ILL_COMBO          -420623
#define ME_MATCH_FOK_NOT_ALLOWD        -420625
#define ME_MATCH_MAX_VALUE_MIDPOINT    -420627
#define ME_MATCH_INS_GOOD_TILL_SESSION -420629
#define ME_MATCH_SST_GOOD_TILL_SESSION -420631
#define ME_MATCH_USR_GOOD_TILL_SESSION -420633
#define ME_MATCH_TAR_INST_NOT_TRADED   -420635
#define ME_MATCH_MAX_MEMBER_ORDERS     -420637
#define ME_MATCH_REFERENCE_PRICE       -420639
#define ME_MATCH_INV_TOKEN             -420641
#define ME_MATCH_THROTTLING            -420643
#define ME_MATCH_ILL_TRT_IN_USR        -420645
#define ME_OB_INV_HANDLE               -425000
#define ME_OB_NO_RESOURCE              -425002
#define ME_OB_NOT_COSTUMER             -425003
#define ME_OB_NOT_INIT                 -425004
#define ME_OB_NOT_LOCKED               -425006
#define ME_OB_NOT_SERIES               -425007
#define ME_OB_OLD_CUSTOMER             -425009
#define ME_OB_OLD_SERIES               -425011
#define ME_OB_END                       425013
#define ME_OB_TENTATIVE                 425015
#define ME_OB_NOT_CUSTOMER             -425017
#define ME_OB_NOT_UPDATER              -425018
#define ME_OB_FM_FAIL                  -425020
#define ME_OB_SER_COMBO_PART           -425021
#define ME_RTR_ERROR                   -430001
#define ME_RTR_ALRCRE                  -430003
#define ME_RTR_NULLIOB                 -430005
#define ME_RTR_NODST                   -430007
#define ME_RTR_TXTIMOUT                -430009
#define ME_RTR_RCVTIMOUT               -430011
#define ME_QUERY_CUST_IST              -435001
#define ME_QUERY_NOT_OWN               -435003
#define ME_QUERY_CUST_UND              -435005
#define ME_QUERY_OID_NOTFND            -435007
#define ME_QUERY_FAIL                  -435008
#define ME_QUERY_ONLY_THIS_SER         -435009
#define ME_QUERY_NOT_AUTH              -435011
#define ME_QUERY_DATAINCOMPLETE        -435012
#define ME_QUEUE_LAST_ENTRY             440001
#define ME_QUEUE_EMPTY                 -440003
#define ME_QUEUE_FULL                  -440005
#define ME_QUEUE_NOT_INIT              -440007
#define ME_QUEUE_SIZE_ERROR            -440008
#define ME_QUEUE_NOT_REMOVED           -440009
#define ME_STAT_INSFMEM                -445000
#define ME_STAT_INT_OVRFLW             -445002
#define ME_PBLOG_NO_LOGB_NAME          -450000
#define ME_TXSRV_TIMEOUT               -455001
#define ME_TXSRV_NOT_CONN              -455003
#define ME_MXABS_BAD_ATTRIBUTE         -460000
#define ME_MXABS_OK                     460001
#define ME_MXABS_INVHANDLE             -460003
#define ME_MXABS_INVLEN                -460005
#define ME_MXABS_NOTFND                -460007
#define ME_MXABS_NYI                   -460009
#define ME_MXABS_NOOP4CLIENT           -460011
#define ME_MXABS_NOACCESS              -460013
#define ME_MXABS_NOACTION              -460015
#define ME_MXABS_NOREPORT              -460017
#define ME_MXABS_ALROPEN               -460019
#define ME_MXABS_INVEXERPT             -460021
#define ME_MXABS_SUBJ2DLL              -460023
#define ME_MXABS_INVTXCODE             -460024
#define ME_MXABS_ZEROQTY               -460025
#define ME_MXABS_BADQTY4REM            -460027
#define ME_MXABS_SYNCINPROG            -460029
#define ME_MXABS_DLLINPROG             -460031
#define ME_MXABS_ALRRES                -460033
#define ME_MXABS_NOTRES                -460035
#define ME_MXABS_TRADERSUSP            -460037
#define ME_MXABS_CXLREPLMENT           -460039
#define ME_MXABS_OPMISMATCH            -460041
#define ME_MXABS_ALERT                 -460043
#define ME_MXABS_OK_BUT_WAIT            460045
#define ME_MXAOF_INTERR                -465000
#define ME_MXAOF_COMERR                -465002
#define ME_MXAOF_NOLOG                 -465004
#define ME_MXAOF_MSG_REROUTED          -465005
#define ME_MXAOF_MSG_IGNORED           -465006
#define ME_MXAOF_SEQ_NO_TO_AOF          465007
#define ME_MXAOF_SEQ_NO_FROM_AOF        465009
#define ME_MXFLT_INTERR                -470000
#define ME_MXFLT_COMERR                -470002
#define ME_MXFLT_NOLOG                 -470004
#define ME_MXFLT_DATAERR               -470006
#define ME_MXFLT_HANDLERR              -470008
#define ME_MXFLT_NOTSPEC               -470010
#define ME_MXFLT_NOTFOUND              -470012
#define ME_MXFLT_FILTERR               -470014
#define ME_TRP_ILL_MIXED_INV_PRICES    -475001
#define ME_TRP_ILL_MIXED_PRICE_TYPES   -475003
#define ME_TRP_ILL_NO_COMBO_LEGS       -475005
#define ME_TRP_NON_EXISTING            -475007
#define ME_TRP_NOT_ALLOWED             -475009
#define ME_TRP_NOT_ALLOWED_TO_ANSWER   -475011
#define ME_TRP_NOT_ALLOWED_TO_EXE      -475013
#define ME_TRP_QUOTE_MISSING           -475015
#define ME_TRP_STATE_NOT_ACCEPTING     -475017
#define ME_TRP_STATE_NOT_IN_TRADING    -475019
#define ME_TRP_TOO_LONG_ACCEPT_TIMER   -475021
#define ME_TRP_TOO_LONG_TRADE_TIMER    -475023
#define ME_TRP_VOLUME_TOO_SMALL        -475025
#define ME_TRP_MAX_NO_COMBO_LEGS       -475027
#define ME_TRP_EXE_PREMIUM_DIFF        -475029
#define ME_TRP_NO_REPLY                -475031
#define ME_TRP_COMBO_NOT_ALLOWED       -475033
#define ME_TRP_QUANTITY_MISMATCH       -475035
#define ME_TRP_ACCOUNT                 -475037
#define ME_TRP_INV_DEALSOURCE          -475039
#define ME_TRP_INV_VAILDATE_ACCOUNT    -475041
#define ME_TRP_CRS_PRIO_SPREAD         -475043
#define ME_TRP_CRS_PRIO_SPREAD_TIME    -475045
#define ME_TRP_CRS_PRIO_NOT_ALLOWED    -475047
#define ME_TRP_TRP_INI_NOT_ALLOWED     -475049
#define ME_TRP_TRP_INI_INTERBANK       -475051
#define ME_TRP_TRP_INI_MIN_VAL         -475053
#define ME_TRP_TRP_FOR_NOT_ALLOWED     -475055
#define ME_TRP_TRP_INV_DATE            -475057
#define ME_TRP_TRP_INV_RATE            -475059
#define ME_TRP_TRP_INV_BOQ             -475061
#define ME_TRP_TRP_COMB_NOT_ALLOWED    -475063
#define ME_TRP_TRP_MIN_VAL             -475065
#define ME_TRP_TRP_MAX_VAL             -475067
#define ME_TRP_TRP_COMB_LEGS           -475069
#define ME_TRP_TRP_INV_PARTY           -475071
#define ME_TRP_TRP_INV_EXTEND_PRICE    -475073
#define ME_TRP_CPPX_NO_MAX_TIME        -475074
#define ME_TRP_CPPX_NOT_INIT           -475076
#define ME_TRP_CPPX_MAX_TIME_PASSED    -475078
#define ME_TRP_CPPX_INV_SERIES         -475080
#define ME_TRP_CPPX_INV_QUANTITY       -475082
#define ME_TRP_CPPX_INV_PARTICIPANT    -475084
#define ME_TRP_NON_STD_SETTL_ALLOWED   -475085
#define ME_TRP_TRP_SINGLE_NOT_ALLOWED  -475087
#define ME_TRP_TRP_PURPOSE             -475089
#define ME_TRP_TRP_PURPOSE_SET         -475091
#define ME_TRP_INV_ACCOUNT_TYPE        -475093
#define ME_OHS_INVMEMBER               -485000
#define ME_OHS_INVPARTY                -485002
#define ME_OHS_INVTIME                 -485004
#define ME_OHS_TXNOTHANDLED            -485006
#define ME_OHS_INVTRANSSIZE            -485008
#define ME_OHS_INVBIDASK               -485010
#define ME_OHS_INVINSTANCE             -485012
#define ME_OHS_DATAINCOMPLETE          -485014
#define ME_OHS_INVBDXTYPE              -485016
#define ME_OHS_INVORDERNBR             -485018
#define ME_OHS_DATAPURGED               485019
#define ME_OHS_INVSERIES                485021
#define ME_OHS_DATANOTREADY             485023
#define ME_OHS_DATAINCOMPLETE_NORECOV  -485024

/******************************************************************************
*end*of* me_messages.h
*******************************************************************************/

#endif /* _ME_MESSAGES_H_ */


#ifndef _MP_MESSAGES_H_
#define _MP_MESSAGES_H_

/******************************************************************************
Module: mp_messages.h

Message definitions generated 2022-12-01 07:07:30 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define MP_MDF_VERSION                  401001
#define MP_COUNTERS_NOT_INIT           -405000
#define MP_INFORMATION                  405001
#define MP_SUCCESS                      405003
#define MP_PANIC                       -405004
#define MP_INSFMEM                     -405006
#define MP_BUGCHECK                    -405007
#define MP_QUOTA                       -405009
#define MP_IVBUFLEN                    -405010
#define MP_ABORT                       -405012
#define MP_COMMIT_LOG_UNKNOWN          -405014
#define MP_INVINT                      -405016
#define MP_NOHIT                       -405017
#define MP_LISEMPTY                    -405019
#define MP_OVERFLOW                    -405021
#define MP_RECEXCLEN                   -405022
#define MP_WRNGFTYP                    -405024
#define MP_OPLOG_FULL                  -405026
#define MP_FILE_NOT_FOUND              -405028
#define MP_NOT_ALLW_ALTER_ORDER        -405029
#define MP_NOT_EXP_RESULT              -405031
#define MP_INV_USR                     -405033
#define MP_FILE_LOCKED                 -405035
#define MP_EOF                          405037
#define MP_BUFFER_SMALL                -405039
#define MP_QUEUE_ALMOST_FULL            405041
#define MP_NO_PQR_IN_UND               -405042
#define MP_MARKET_MAKER_PROTECTION     -405044
#define MP_MARKET_MAKER_PROTECTION_SET  405045
#define MP_MARKET_MAKER_TRADE           405047
#define MP_MM_VOLUME_PROTECTION        -405048
#define MP_MM_DELTA_PROTECTION         -405050
#define MP_MM_PROTECTION_LIST_END       405051
#define MP_NOT_YET_INITIALIZED         -405052
#define MP_INFO_ERROR_STATE            -410000
#define MP_INFO_LOG_TOO_SMALL          -410002
#define MP_INFO_NO_TIMESTAMP           -410004
#define MP_INFO_LOG_UNKNOWN            -410006
#define MP_INFO_LOG_WRONG_SIZE         -410008
#define MP_INFO_TOO_MANY_DEALS         -410010
#define MP_INFO_POSS_DUPL              -410011
#define MP_INFO_TOO_MANY_LOGS          -410012
#define MP_INFO_UNDEFINED_STATE        -410014
#define MP_INFO_DEAL_DELAYED           -410015
#define MP_LOG_ALREADY_INIT             415001
#define MP_LOG_NOT_INITIALIZED         -415002
#define MP_LOG_NOT_READ_MODE           -415004
#define MP_LOG_NOT_WRITE_MODE          -415006
#define MP_LOG_END_OF_FILE             -415007
#define MP_LOG_UNDEFINED_MODE          -415008
#define MP_MATCH_ALRLOAD               -420001
#define MP_MATCH_DEFMAX                -420003
#define MP_MATCH_DISCARDED             -420005
#define MP_MATCH_ILL_CRS               -420007
#define MP_MATCH_MIN_QUANTITY          -420009
#define MP_MATCH_PREMIUM_REQ           -420011
#define MP_MATCH_TIMEOUT               -420013
#define MP_MATCH_INV_ALTER             -420015
#define MP_MATCH_NOT_IMPL              -420017
#define MP_MATCH_OVERBID               -420019
#define MP_MATCH_UNKNOWN               -420021
#define MP_MATCH_BID_ASK               -420023
#define MP_MATCH_VALIDITY              -420025
#define MP_MATCH_MARBOUNCE             -420027
#define MP_MATCH_PREMIUM               -420029
#define MP_MATCH_STOPPED               -420031
#define MP_MATCH_AONBOUNCE             -420033
#define MP_MATCH_BLOCK                 -420035
#define MP_MATCH_COMBO_MAX_LEGS        -420037
#define MP_MATCH_COMBO_PART            -420039
#define MP_MATCH_COMBO_SAVE            -420041
#define MP_MATCH_INV_COMBO_SIZE        -420043
#define MP_MATCH_QUANTITY              -420045
#define MP_MATCH_ORDER_EXISTS          -420047
#define MP_MATCH_NOT_AT_MARKET         -420049
#define MP_MATCH_LOW_VOLUME            -420051
#define MP_MATCH_BROKER_SPREAD         -420053
#define MP_MATCH_ILL_INTERBANK         -420055
#define MP_MATCH_ACC_PREMIUM           -420057
#define MP_MATCH_ACC_ORDER             -420059
#define MP_MATCH_COMBO_LEG             -420061
#define MP_MATCH_LEG_PRICE_NOT_0       -420063
#define MP_MATCH_LEG_NOT_FOUND         -420065
#define MP_MATCH_QUANT_NE_LEG          -420067
#define MP_MATCH_COMBO_SPREAD          -420069
#define MP_MATCH_SER_IS_EXPIRED        -420071
#define MP_MATCH_VOLUME_REQ            -420073
#define MP_MATCH_CROSS_NO_STDCOMB      -420074
#define MP_MATCH_ILL_ORDER_TYPE        -420076
#define MP_MATCH_MKT_ORDER_PRICE       -420078
#define MP_MATCH_NOCLS_PRM             -420079
#define MP_MATCH_FILL_BAL              -420081
#define MP_MATCH_COP_LESS              -420083
#define MP_MATCH_COP_VAR               -420085
#define MP_MATCH_PRE_TIME              -420087
#define MP_MATCH_INV_EXP               -420089
#define MP_MATCH_CROSS_PRICE           -420091
#define MP_MATCH_BLOCK_MAX_LEGS        -420093
#define MP_MATCH_BLOCK_SERIES          -420095
#define MP_MATCH_LMT_NOT_VAL           -420097
#define MP_MATCH_MKT_NOT_VAL           -420099
#define MP_MATCH_KIL_NOT_VAL           -420101
#define MP_MATCH_EXISTS_AS_STD_COMBO   -420103
#define MP_MATCH_EXISTS_AS_REV_STD_CBS -420105
#define MP_MATCH_TM_COMBO_EXISTS       -420107
#define MP_MATCH_REV_TM_COMBO_EXISTS   -420109
#define MP_MATCH_TOO_MANY_TM_COMBOS    -420111
#define MP_MATCH_ILL_RATIO             -420113
#define MP_MATCH_ILL_BUY_SELL_OP       -420115
#define MP_MATCH_COMBO_DIFF_CONTR_SIZE -420117
#define MP_MATCH_NO_BB_WHEN_TM_CBS_P   -420119
#define MP_MATCH_DUPL_SERIES           -420121
#define MP_MATCH_NOT_AUTHORIZED        -420123
#define MP_MATCH_NOTHEO_PRM            -420125
#define MP_MATCH_MIXED_COM             -420127
#define MP_MATCH_MEM_CRS_NOT_ALLOWED   -420129
#define MP_MATCH_PRICE_LIMIT           -420131
#define MP_MATCH_INV_STP_COND          -420133
#define MP_MATCH_INV_HIDDEN            -420135
#define MP_MATCH_INV_SHOWN             -420137
#define MP_MATCH_NOT_AUTH              -420139
#define MP_MATCH_SHOWN_TOO_SMALL       -420141
#define MP_MATCH_AONNOTVALID           -420143
#define MP_MATCH_INV_DELTA             -420145
#define MP_MATCH_FOK_NOT_WKUP_OWN      -420147
#define MP_MATCH_AON_CROSS_PRICE       -420149
#define MP_MATCH_LEG_PREMIUM           -420151
#define MP_MATCH_NO_WILD_CARD          -420153
#define MP_MATCH_INV_OP_CLS_REQ        -420155
#define MP_MATCH_INV_ORIGIN            -420157
#define MP_MATCH_INV_ACCOUNT           -420159
#define MP_MATCH_INV_CTI_CODE          -420161
#define MP_MATCH_NOT_AT_BEST           -420163
#define MP_MATCH_PRICE_LIMIT_LEG       -420165
#define MP_MATCH_PRICE_DEV_LEG         -420167
#define MP_MATCH_PRICE_LEG             -420169
#define MP_MATCH_PRICE_RANGE           -420171
#define MP_MATCH_NOT_TIME_YET          -420173
#define MP_MATCH_NO_ACTION              420175
#define MP_MATCH_ORD_NOT_FOU           -420177
#define MP_MATCH_CB_LIMIT_LAST         -420179
#define MP_MATCH_CB_LIMIT_REF          -420181
#define MP_MATCH_PERC_OR_ABS           -420183
#define MP_MATCH_BLMT_ORDER_PRICE      -420184
#define MP_MATCH_BLMT_NOT_VAL          -420185
#define MP_MATCH_BLMT_NO_PREM          -420187
#define MP_MATCH_NOT_IN_WORKUP         -420189
#define MP_MATCH_IN_WORKUP             -420191
#define MP_MATCH_INS_FILL_OR_KILL      -420193
#define MP_MATCH_SST_FILL_OR_KILL      -420195
#define MP_MATCH_USR_FILL_OR_KILL      -420197
#define MP_MATCH_INS_FILL_AND_KILL     -420199
#define MP_MATCH_SST_FILL_AND_KILL     -420201
#define MP_MATCH_USR_FILL_AND_KILL     -420203
#define MP_MATCH_INS_FILL_AND_STORE    -420205
#define MP_MATCH_SST_FILL_AND_STORE    -420207
#define MP_MATCH_USR_FILL_AND_STORE    -420209
#define MP_MATCH_INS_LIMIT_ORD         -420211
#define MP_MATCH_SST_LIMIT_ORD         -420213
#define MP_MATCH_USR_LIMIT_ORD         -420215
#define MP_MATCH_INS_MARKET_ORD        -420217
#define MP_MATCH_SST_MARKET_ORD        -420219
#define MP_MATCH_USR_MARKET_ORD        -420221
#define MP_MATCH_INS_ALL_OR_NONE       -420223
#define MP_MATCH_SST_ALL_OR_NONE       -420225
#define MP_MATCH_USR_ALL_OR_NONE       -420227
#define MP_MATCH_INV_INT_HIDDEN        -420229
#define MP_MATCH_OID_REQUIRED          -420231
#define MP_MATCH_ODD_LOT_EXP           -420233
#define MP_MATCH_ILL_ORDER_TYPE_SST    -420235
#define MP_MATCH_ILL_ORDER_TYPE_INT    -420237
#define MP_MATCH_ILL_ORDER_TYPE_USR    -420239
#define MP_MATCH_MIN_VOL_TRC           -420241
#define MP_MATCH_MAX_VOL_TRC           -420243
#define MP_MATCH_INS_IMBALANCE         -420245
#define MP_MATCH_SST_IMBALANCE         -420247
#define MP_MATCH_USR_IMBALANCE         -420249
#define MP_MATCH_INS_MTL_ROUND_LOT     -420251
#define MP_MATCH_SST_MTL_ROUND_LOT     -420253
#define MP_MATCH_USR_MTL_ROUND_LOT     -420255
#define MP_MATCH_INS_MTL_COMBO_R_LOT   -420257
#define MP_MATCH_SST_MTL_COMBO_R_LOT   -420259
#define MP_MATCH_USR_MTL_COMBO_R_LOT   -420261
#define MP_MATCH_INS_ODD_LOT           -420263
#define MP_MATCH_SST_ODD_LOT           -420265
#define MP_MATCH_USR_ODD_LOT           -420267
#define MP_MATCH_INS_LIMIT_ODD_LOT     -420269
#define MP_MATCH_SST_LIMIT_ODD_LOT     -420271
#define MP_MATCH_USR_LIMIT_ODD_LOT     -420273
#define MP_MATCH_INS_MARKET_ODD_LOT    -420275
#define MP_MATCH_SST_MARKET_ODD_LOT    -420277
#define MP_MATCH_USR_MARKET_ODD_LOT    -420279
#define MP_MATCH_INS_MTL_ODD_LOT       -420281
#define MP_MATCH_SST_MTL_ODD_LOT       -420283
#define MP_MATCH_USR_MTL_ODD_LOT       -420285
#define MP_MATCH_INS_HIDDEN_AGGRESSIVE -420287
#define MP_MATCH_SST_HIDDEN_AGGRESSIVE -420289
#define MP_MATCH_USR_HIDDEN_AGGRESSIVE -420291
#define MP_MATCH_INS_HIDDEN_PASSIVE    -420293
#define MP_MATCH_SST_HIDDEN_PASSIVE    -420295
#define MP_MATCH_USR_HIDDEN_PASSIVE    -420297
#define MP_MATCH_OVERRIDE_CRS          -420299
#define MP_MATCH_MATCHED_CRS_PRICE     -420301
#define MP_MATCH_BID_ORDER             -420303
#define MP_MATCH_ASK_ORDER             -420305
#define MP_MATCH_FORCE_ORDER           -420307
#define MP_MATCH_ILL_ORDER_TYPE_TX     -420309
#define MP_MATCH_MIN_BLK_SIZE          -420311
#define MP_MATCH_MAX_BLK_SIZE          -420313
#define MP_MATCH_INS_DECR_SHOWN_NOHIDD -420315
#define MP_MATCH_INS_DECR_SHOWN        -420317
#define MP_MATCH_INS_DECR_HIDD         -420319
#define MP_MATCH_INS_INCR_SHOWN        -420321
#define MP_MATCH_INS_INCR_HIDD         -420323
#define MP_MATCH_INS_CLIENT            -420325
#define MP_MATCH_INS_PRICE_IMPR        -420327
#define MP_MATCH_INS_PRICE_DISIMPR     -420329
#define MP_MATCH_INS_BLOCK             -420331
#define MP_MATCH_INS_TIME_EXT          -420333
#define MP_MATCH_INS_TIME_DECR         -420335
#define MP_MATCH_INS_ORDER_TYPE        -420337
#define MP_MATCH_INS_EXCH_ORDER_TYPE   -420339
#define MP_MATCH_ALTER_BALANCE         -420341
#define MP_MATCH_SST_DECR_SHOWN_NOHIDD -420343
#define MP_MATCH_SST_DECR_SHOWN        -420345
#define MP_MATCH_SST_DECR_HIDD         -420347
#define MP_MATCH_SST_INCR_SHOWN        -420349
#define MP_MATCH_SST_INCR_HIDD         -420351
#define MP_MATCH_SST_CLIENT            -420353
#define MP_MATCH_SST_PRICE_IMPR        -420355
#define MP_MATCH_SST_PRICE_DISIMPR     -420357
#define MP_MATCH_SST_BLOCK             -420359
#define MP_MATCH_SST_TIME_EXT          -420361
#define MP_MATCH_SST_TIME_DECR         -420363
#define MP_MATCH_SST_ORDER_TYPE        -420365
#define MP_MATCH_SST_EXCH_ORDER_TYPE   -420367
#define MP_MATCH_CRS_PRIO_NO_ORDER     -420369
#define MP_MATCH_SST_PST_ORDER         -420371
#define MP_MATCH_INS_PST_ORDER         -420373
#define MP_MATCH_USR_PST_ORDER         -420375
#define MP_MATCH_SST_SHORT_ORDER       -420377
#define MP_MATCH_INS_SHORT_ORDER       -420379
#define MP_MATCH_USR_SHORT_ORDER       -420381
#define MP_MATCH_SST_MB_ORDER          -420383
#define MP_MATCH_INS_MB_ORDER          -420385
#define MP_MATCH_USR_MB_ORDER          -420387
#define MP_MATCH_SST_BL_ORDER          -420389
#define MP_MATCH_INS_BL_ORDER          -420391
#define MP_MATCH_USR_BL_ORDER          -420393
#define MP_MATCH_LAST_PRICE            -420395
#define MP_MATCH_MB_SELL               -420397
#define MP_MATCH_MB_ORDER_TYPE         -420399
#define MP_MATCH_SH_BUY                -420401
#define MP_MATCH_SH_TKO                -420403
#define MP_MATCH_MTL_NO_OPPOSING       -420405
#define MP_MATCH_COMBO_PRACTICE        -420407
#define MP_MATCH_COMBO_EQTY_NOT_MULT   -420409
#define MP_MATCH_RELOAD_NOT_DONE       -420410
#define MP_MATCH_FOK_FAK_NOT_ALLOWD    -420411
#define MP_MATCH_VOL_VAL_TRC           -420413
#define MP_MATCH_BEST_LIMIT_REQ        -420415
#define MP_MATCH_SER_WILD_CARD         -420417
#define MP_MATCH_INV_ACTIVATION        -420419
#define MP_MATCH_INV_CLEARING_PART     -420421
#define MP_MATCH_INV_E_ORD_TYPE_PRIO   -420423
#define MP_MATCH_INV_ALWAYS_INACTIVE   -420425
#define MP_MATCH_ILL_EXCH_ORDER_TYPE   -420426
#define MP_MATCH_MM_PRIO_LEVEL_EXC     -420427
#define MP_MATCH_STOP_PREMIUM          -420429
#define MP_MATCH_SST_SSO_OPEN_END      -420431
#define MP_MATCH_INS_SSO_OPEN_END      -420433
#define MP_MATCH_USR_SSO_OPEN_END      -420435
#define MP_MATCH_SST_SSO_CLOSE_END     -420437
#define MP_MATCH_INS_SSO_CLOSE_END     -420439
#define MP_MATCH_USR_SSO_CLOSE_END     -420441
#define MP_MATCH_SST_SSO               -420443
#define MP_MATCH_INS_SSO               -420445
#define MP_MATCH_USR_SSO               -420447
#define MP_MATCH_NO_SST_FOR_SSO        -420449
#define MP_MATCH_SSO_SPEC_NO_TYPE      -420451
#define MP_MATCH_SST_EXT_T_NOT_VALID   -420453
#define MP_MATCH_SST_EXT_T_STATE       -420455
#define MP_MATCH_INS_EXT_T_STATE       -420457
#define MP_MATCH_INS_STOP_ORD          -420459
#define MP_MATCH_SST_STOP_ORD          -420461
#define MP_MATCH_USR_STOP_ORD          -420463
#define MP_MATCH_INS_CRS_SPREAD_MAR    -420465
#define MP_MATCH_SST_CRS_SPREAD_MAR    -420467
#define MP_MATCH_USR_CRS_SPREAD_MAR    -420469
#define MP_MATCH_INS_CRS_SPREAD_LIM    -420471
#define MP_MATCH_SST_CRS_SPREAD_LIM    -420473
#define MP_MATCH_USR_CRS_SPREAD_LIM    -420475
#define MP_MATCH_INS_UNDISCLOSED_QTY   -420477
#define MP_MATCH_SST_UNDISCLOSED_QTY   -420479
#define MP_MATCH_USR_UNDISCLOSED_QTY   -420481
#define MP_MATCH_MIN_UNDISCLOSED       -420483
#define MP_MATCH_CP_NO_HIDDEN          -420485
#define MP_MATCH_CP_LIMIT              -420487
#define MP_MATCH_NO_CP_PRICE           -420489
#define MP_MATCH_MIN_VOLUME_MATCH      -420491
#define MP_MATCH_MIN_VOLUME_FIX        -420493
#define MP_MATCH_FIX_PREMIUM_DIFF      -420495
#define MP_MATCH_UNDSC_NO_HIDDEN       -420497
#define MP_MATCH_UNDSC_NO_MARKET       -420499
#define MP_MATCH_INTCRSONUSER          -420501
#define MP_MATCH_MUST_BE_QUOTE         -420503
#define MP_MATCH_ILL_NAMED_STRUCTURE   -420505
#define MP_MATCH_INCONSISTENT_VIM      -420507
#define MP_MATCH_ILL_QUOTE_ACTION      -420509
#define MP_MATCH_ILL_PRICE_QUANTITY    -420511
#define MP_MATCH_INV_ORDER_CAPACITY    -420513
#define MP_MATCH_INV_ALT_CLEARING      -420515
#define MP_MATCH_MAX_QUANTITY_OSA      -420517
#define MP_MATCH_DUPL_NAMED_STRUCT     -420519
#define MP_MATCH_MAND_NAMED_STRUCT     -420521
#define MP_MATCH_LINKED_ORDER_CONT     -420523
#define MP_MATCH_LINKED_ORDER_DUPL_SER -420525
#define MP_MATCH_LINKED_ORDER_MIN_LEGS -420527
#define MP_MATCH_LINKED_ORDER_MAX_LEGS -420529
#define MP_MATCH_INV_ALTER_LINKED      -420531
#define MP_MATCH_NOT_SAME_LOTS         -420533
#define MP_MATCH_ONLY_ONE_ORDER        -420535
#define MP_MATCH_INV_DELTA_QUANTITY    -420537
#define MP_MATCH_HOLD_IN_MARKET        -420539
#define MP_MATCH_USER_NO_TRADE         -420541
#define MP_MATCH_IPMO_RESTRICTIONS     -420543
#define MP_MATCH_MULTI_PRICE_TYPE      -420545
#define MP_MATCH_NO_COMBO_IN_MULTI     -420547
#define MP_MATCH_MULTI_ORDER_DUPL_SER  -420549
#define MP_MATCH_PRICE_TYPE_SAME_SIDE  -420551
#define MP_MATCH_INV_CALC_QUANTITY     -420553
#define MP_MATCH_PRICE_TYPE_SAME_UNIT  -420555
#define MP_MATCH_MULTI_ORDER_MIN_LEGS  -420557
#define MP_MATCH_MULTI_ORDER_MAX_LEGS  -420559
#define MP_MATCH_MULTI_ORDER_CONT      -420561
#define MP_MATCH_PRICE_TYPE_ILL_UNIT   -420563
#define MP_MATCH_PRICE_TYPE_ILL_PQF    -420565
#define MP_MATCH_ORDER_SIZE_TOO_LARGE  -420567
#define MP_MATCH_USR_RISK_BREACHED     -420569
#define MP_MATCH_MAX_ORDER_RATE        -420571
#define MP_MATCH_ORDER_SUBMISSION      -420573
#define MP_MATCH_NO_SPONSOR            -420575
#define MP_MATCH_MUST_BE_ORDER         -420577
#define MP_MATCH_ORDER_VALUE_TOO_LARGE -420579
#define MP_MATCH_ORDER_SIZE_GT_AVAILQ  -420581
#define MP_MATCH_NO_EXCHANGE_INFO      -420583
#define MP_MATCH_CLIENT_CAT_INVALID    -420585
#define MP_MATCH_EXCLIENT_BLANK        -420587
#define MP_MATCH_ILL_PREMIUM_FIXED     -420589
#define MP_MATCH_NO_FIXED_PRICE        -420591
#define MP_MATCH_FIXED_PRICE_COMBO     -420593
#define MP_MATCH_ILL_TIME_FIXED_MARKET -420595
#define MP_MATCH_TRP_FIXED_LINKED_LTP  -420597
#define MP_MATCH_SST_PR_ORDER          -420599
#define MP_MATCH_INS_PR_ORDER          -420601
#define MP_MATCH_USR_PR_ORDER          -420603
#define MP_MATCH_ILL_PR_VALUE          -420605
#define MP_MATCH_SH_COMPRICE           -420607
#define MP_MATCH_MKT_BLMT_NOPRC_EXIST  -420609
#define MP_MATCH_SH_ORDTYPE            -420611
#define MP_MATCH_SH_LTP_NEUFALL        -420613
#define MP_MATCH_RFQ_NOT_LEGITIMATE    -420615
#define MP_MATCH_RFQ_RESPONSE_ILLEGAL  -420617
#define MP_MATCH_MIN_VALUE_MIDPOINT    -420619
#define MP_MATCH_SST_MIDPOINT          -420621
#define MP_MATCH_SH_ILL_COMBO          -420623
#define MP_MATCH_FOK_NOT_ALLOWD        -420625
#define MP_MATCH_MAX_VALUE_MIDPOINT    -420627
#define MP_MATCH_INS_GOOD_TILL_SESSION -420629
#define MP_MATCH_SST_GOOD_TILL_SESSION -420631
#define MP_MATCH_USR_GOOD_TILL_SESSION -420633
#define MP_MATCH_TAR_INST_NOT_TRADED   -420635
#define MP_OB_INV_HANDLE               -425000
#define MP_OB_NO_RESOURCE              -425002
#define MP_OB_NOT_COSTUMER             -425003
#define MP_OB_NOT_INIT                 -425004
#define MP_OB_NOT_LOCKED               -425006
#define MP_OB_NOT_SERIES               -425007
#define MP_OB_OLD_CUSTOMER             -425009
#define MP_OB_OLD_SERIES               -425011
#define MP_OB_END                       425013
#define MP_OB_TENTATIVE                 425015
#define MP_OB_NOT_CUSTOMER             -425017
#define MP_OB_NOT_UPDATER              -425018
#define MP_OB_FM_FAIL                  -425020
#define MP_OB_SER_COMBO_PART           -425021
#define MP_RTR_ERROR                   -430001
#define MP_RTR_ALRCRE                  -430003
#define MP_RTR_NULLIOB                 -430005
#define MP_RTR_NODST                   -430007
#define MP_RTR_TXTIMOUT                -430009
#define MP_RTR_RCVTIMOUT               -430011
#define MP_QUERY_CUST_IST              -435001
#define MP_QUERY_NOT_OWN               -435003
#define MP_QUERY_CUST_UND              -435005
#define MP_QUERY_OID_NOTFND            -435007
#define MP_QUERY_FAIL                  -435008
#define MP_QUERY_ONLY_THIS_SER         -435009
#define MP_QUERY_NOT_AUTH              -435011
#define MP_QUERY_DATAINCOMPLETE        -435012
#define MP_QUEUE_LAST_ENTRY             440001
#define MP_QUEUE_EMPTY                 -440003
#define MP_QUEUE_FULL                  -440005
#define MP_QUEUE_NOT_INIT              -440007
#define MP_QUEUE_SIZE_ERROR            -440008
#define MP_QUEUE_NOT_REMOVED           -440009
#define MP_STAT_INSFMEM                -445000
#define MP_STAT_INT_OVRFLW             -445002
#define MP_PBLOG_NO_LOGB_NAME          -450000
#define MP_TXSRV_TIMEOUT               -455001
#define MP_TXSRV_NOT_CONN              -455003
#define MP_MXABS_BAD_ATTRIBUTE         -460000
#define MP_MXABS_OK                     460001
#define MP_MXABS_INVHANDLE             -460003
#define MP_MXABS_INVLEN                -460005
#define MP_MXABS_NOTFND                -460007
#define MP_MXABS_NYI                   -460009
#define MP_MXABS_NOOP4CLIENT           -460011
#define MP_MXABS_NOACCESS              -460013
#define MP_MXABS_NOACTION              -460015
#define MP_MXABS_NOREPORT              -460017
#define MP_MXABS_ALROPEN               -460019
#define MP_MXABS_INVEXERPT             -460021
#define MP_MXABS_SUBJ2DLL              -460023
#define MP_MXABS_INVTXCODE             -460024
#define MP_MXABS_ZEROQTY               -460025
#define MP_MXABS_BADQTY4REM            -460027
#define MP_MXABS_SYNCINPROG            -460029
#define MP_MXABS_DLLINPROG             -460031
#define MP_MXABS_ALRRES                -460033
#define MP_MXABS_NOTRES                -460035
#define MP_MXABS_TRADERSUSP            -460037
#define MP_MXABS_CXLREPLMENT           -460039
#define MP_MXABS_OPMISMATCH            -460041
#define MP_MXABS_ALERT                 -460043
#define MP_MXABS_OK_BUT_WAIT            460045
#define MP_MXAOF_INTERR                -465000
#define MP_MXAOF_COMERR                -465002
#define MP_MXAOF_NOLOG                 -465004
#define MP_MXAOF_MSG_REROUTED          -465005
#define MP_MXAOF_MSG_IGNORED           -465006
#define MP_MXAOF_SEQ_NO_TO_AOF          465007
#define MP_MXAOF_SEQ_NO_FROM_AOF        465009
#define MP_MXFLT_INTERR                -470000
#define MP_MXFLT_COMERR                -470002
#define MP_MXFLT_NOLOG                 -470004
#define MP_MXFLT_DATAERR               -470006
#define MP_MXFLT_HANDLERR              -470008
#define MP_MXFLT_NOTSPEC               -470010
#define MP_MXFLT_NOTFOUND              -470012
#define MP_MXFLT_FILTERR               -470014
#define MP_QXSRV_ILL_MIXED_INV_PRICES  -475001
#define MP_QXSRV_ILL_MIXED_PRICE_TYPES -475003
#define MP_QXSRV_ILL_NO_COMBO_LEGS     -475005
#define MP_QXSRV_NON_EXISTING          -475007
#define MP_QXSRV_NOT_ALLOWED           -475009
#define MP_QXSRV_NOT_ALLOWED_TO_ANSWER -475011
#define MP_QXSRV_NOT_ALLOWED_TO_EXE    -475013
#define MP_QXSRV_QUOTE_MISSING         -475015
#define MP_QXSRV_STATE_NOT_ACCEPTING   -475017
#define MP_QXSRV_STATE_NOT_IN_TRADING  -475019
#define MP_QXSRV_TOO_LONG_ACCEPT_TIMER -475021
#define MP_QXSRV_TOO_LONG_TRADE_TIMER  -475023
#define MP_QXSRV_VOLUME_TOO_SMALL      -475025
#define MP_QXSRV_MAX_NO_COMBO_LEGS     -475027
#define MP_QXSRV_EXE_PREMIUM_DIFF      -475029
#define MP_QXSRV_NO_REPLY              -475031
#define MP_QXSRV_COMBO_NOT_ALLOWED     -475033
#define MP_QXSRV_QUANTITY_MISMATCH     -475035
#define MP_QXSRV_ACCOUNT               -475037
#define MP_QXSRV_INV_DEALSOURCE        -475039
#define MP_QXSRV_INV_VAILDATE_ACCOUNT  -475041
#define MP_QXSRV_CRS_PRIO_SPREAD       -475043
#define MP_QXSRV_CRS_PRIO_SPREAD_TIME  -475045
#define MP_QXSRV_CRS_PRIO_NOT_ALLOWED  -475047
#define MP_QXSRV_TRP_INI_NOT_ALLOWED   -475049
#define MP_QXSRV_TRP_INI_INTERBANK     -475051
#define MP_QXSRV_TRP_INI_MIN_VAL       -475053
#define MP_QXSRV_TRP_FOR_NOT_ALLOWED   -475055
#define MP_QXSRV_TRP_INV_DATE          -475057
#define MP_QXSRV_TRP_INV_RATE          -475059
#define MP_QXSRV_TRP_INV_BOQ           -475061
#define MP_QXSRV_TRP_COMB_NOT_ALLOWED  -475063
#define MP_QXSRV_TRP_MIN_VAL           -475065
#define MP_QXSRV_TRP_MAX_VAL           -475067
#define MP_QXSRV_TRP_COMB_LEGS         -475069
#define MP_QXSRV_TRP_INV_PARTY         -475071
#define MP_QXSRV_TRP_INV_EXTEND_PRICE  -475073
#define MP_QXSRV_CPPX_NO_MAX_TIME      -475074
#define MP_QXSRV_CPPX_NOT_INIT         -475076
#define MP_QXSRV_CPPX_MAX_TIME_PASSED  -475078
#define MP_QXSRV_CPPX_INV_SERIES       -475080
#define MP_QXSRV_CPPX_INV_QUANTITY     -475082
#define MP_QXSRV_CPPX_INV_PARTICIPANT  -475084
#define MP_QXSRV_NON_STD_SETTL_ALLOWED -475085
#define MP_QXSRV_INVQTYINCR            -475086
#define MP_OHS_INVMEMBER               -485000
#define MP_OHS_INVPARTY                -485002
#define MP_OHS_INVTIME                 -485004
#define MP_OHS_TXNOTHANDLED            -485006
#define MP_OHS_INVTRANSSIZE            -485008
#define MP_OHS_INVBIDASK               -485010
#define MP_OHS_INVINSTANCE             -485012
#define MP_OHS_DATAINCOMPLETE          -485014
#define MP_OHS_INVBDXTYPE              -485016
#define MP_OHS_INVORDERNBR             -485018
#define MP_OHS_DATAPURGED               485019
#define MP_OHS_INVSERIES                485021
#define MP_OHS_DATANOTREADY             485023
#define MP_OHS_DATAINCOMPLETE_NORECOV  -485024

/******************************************************************************
*end*of* mp_messages.h
*******************************************************************************/

#endif /* _MP_MESSAGES_H_ */


#ifndef _OMN_MESSAGES_H_
#define _OMN_MESSAGES_H_

/******************************************************************************
Module: omn_messages.h

Message definitions generated 2022-12-01 07:07:28 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define OMN_GENERIC_DBG                 200001
#define OMN_GENERIC_SUCCESS             200003
#define OMN_GENERIC_INFO                200005
#define OMN_GENERIC_WARN               -200007
#define OMN_GENERIC_ERROR              -200008
#define OMN_GENERIC_FATAL              -200010
#define OSA_RELOGIN_NOT_ALLOWED        -201000
#define OMN_REGISTRATION_NOT_ALLOWED    202001
#define OMN_USER_NOT_FOUND              202003
#define OMN_USERTYPE_NOT_FOUND          202005
#define OMN_FAILED_TO_BUILD_VERTABLE    202007
#define OMN_NODEID_NOT_FOUND            202009
#define OMN_OSA_SEC_OMDU_INVLOGIN      -204000
#define OMN_OSA_SEC_INVLOGIN_1         -204002
#define OMN_OSA_SEC_INVLOGIN_2         -204004
#define OMN_OSA_SEC_INVLOGIN_3         -204005
#define OMN_OSA_SEC_INVLOGIN_4          204007
#define OMN_OSA_SEC_INVLOGIN_5          204009
#define OMN_OSA_SEC_INVLOGIN_6          204011
#define OMN_OSA_SEC_INVLOGIN_7          204013
#define OMN_OSA_SEC_INVLOGIN_8          204015
#define OMN_OSA_SEC_INVLOGIN_9          204017
#define OMN_OSA_SEC_INVLOGIN_10         204019
#define OMN_OSA_SEC_INVLOGIN_11         204021
#define OMN_OSA_SEC_INVLOGIN_12         204023
#define OMN_OSA_SEC_INVLOGIN_13         204025
#define OMN_OSA_SEC_INVLOGIN_15         204027
#define OMN_OSA_SEC_INVLOGIN_16        -204028
#define OMN_OSA_SEC_INVLOGIN_17         204029
#define OMN_OSA_SEC_INVLOGIN_19         204031
#define OMN_OSA_SEC_INVLOGIN_20         204033
#define OMN_OSA_SEC_INVLOGIN_21         204035
#define OMN_OSA_SEC_INVLOGIN_22         204037
#define OMN_OSA_SEC_INVLOGIN_23         204039
#define OMN_OSA_SEC_INVLOGIN_24         204041
#define OMN_OSA_SEC_INVLOGIN_25         204043
#define OMN_OSA_SEC_INVLOGIN_26         204045
#define OMN_OSA_SEC_INVLOGIN_27         204047
#define OMN_OSA_SEC_INVLOGIN_28        -204049
#define OMN_OSA_SEC_INVLOGIN_29         204051
#define OMN_OSA_SEC_INVLOGIN_30        -204052
#define OMN_OSA_SEC_INVLOGIN_31        -204054
#define OMN_OSA_SEC_INVLOGIN_32        -204056
#define OMN_OSA_SEC_INVLOGIN_33        -204058
#define OMN_OSA_SEC_INVLOGIN_34        -204060
#define OMN_OSA_SEC_INVLOGIN_35        -204062
#define OMN_OSA_SEC_SET_SRC_FAILED     -204064
#define OMN_OSA_SEC_SET_PASSW_FAILED   -204066
#define OMN_OSA_SEC_SECVIO_1           -204068
#define OMN_OSA_SEC_SECVIO_2           -204070
#define OMN_OSA_SEC_OAPIOLD            -204072
#define OMN_OSA_SEC_INVNODEID_2        -204074
#define OMN_OSA_SEC_INVNODEID_3        -204076
#define OMN_OSA_SEC_INVNODEID_4        -204078
#define OMN_OSA_SEC_INVNODEID_8        -204080
#define OMN_OSA_SEC_INVNODEID_9        -204082
#define OMN_OSA_SEC_INVNODEID_10       -204084
#define OMN_OSA_SEC_INVNODEID_11       -204086
#define OMN_OSA_SEC_IN_STARTUP_MODE     204087
#define OMN_OSA_SEC_IN_FAILOVER_MODE    204089
#define OMN_OSA_SEC_QUERY_SELF          204091
#define OMN_OSA_SEC_INV_OQ62            204093
#define OMN_OSA_SEC_INV_OQ6X            204095
#define OMN_OSA_SEC_INV_OQ50            204097
#define OMN_OSA_SEC_INV_NQ10            204099
#define OMN_OSA_SEC_INV_NQ11            204101
#define OMN_OSA_SEC_INV_QRY             204103
#define OMN_OSA_SEC_INV_SETPWD_TRX_1   -204104
#define OMN_OSA_SEC_INV_SETPWD_TRX_2   -204106
#define DUGWY_NOCFGFILE                -207001
#define DUGWY_INTERR                   -207002
#define DUGWY_NORMTSRVNODES            -207004
#define DUGWY_NORMTSRV                 -207006
#define DUGWY_NOMBRNODES               -207008
#define DUGWY_INFGRPEXISTS             -207010
#define DUGWY_NOINFGRP                 -207012
#define DUGWY_NOFILTER                 -207014
#define DUGWY_CFGSTRLNG                -207016
#define DUGWY_NOSRVTCPIP               -207018
#define DUGWY_NOMEMBER                 -207020
#define DUGWY_INVGWYMSG                -207022
#define DUGWY_INVGWYVRS                -207024
#define DUGWY_INSUFMEM                 -207026
#define DUGWY_ABORT                    -207028
#define DUGWY_NOOPMAP                  -207029
#define DUGWY_EOFFND                    207031
#define DUGWY_CLTQUETOLONG             -207033
#define DUGWY_UNKNOWN                   207035
#define DUGWY_NOPREFNODES              -207037
#define DUGWY_DUPCFGNODES              -207039
#define DUGWY_INVCFGBLOCK              -207041
#define DUGWY_CMDNOTALLOW              -207043
#define DUGWY_SERVERNOTFOUND           -207045
#define DUGWY_INVCONFIG                -207046
#define DUGWY_NOSRVGROUPS              -207047
#define DUGWY_NOMEMBERS                -207049
#define OMDU_OPENSOCK                  -208000
#define OMDU_DATALOST                  -208002
#define OMDU_LINKLOST                  -208004
#define OMDU_SENDERLOST                -208006
#define OMDU_SENDERRESTARTED           -208008
#define OMDU_NONODADDR                 -208010
#define OMDU_NONODENAME                -208012
#define OMDU_NORDACCESS                -208014
#define OMDU_NOTTWOCALLBACK            -208016
#define OMDU_INSFMEM                   -208018
#define OMDU_INVTHRCNTX                -208020
#define OMDU_CLTPRTERR                 -208022
#define OMDU_CLTLNKLOST                -208024
#define OMDU_INVVALSYNTAX              -208026
#define OMDU_TOMUCHDATA                -208028
#define OMDU_INVVERSTR                 -208030
#define OMDU_WRGVERS                   -208032
#define OMDU_INFOBJNAMWLD              -208034
#define OMDU_SNDNAMLNG                 -208036
#define OMDU_FAILTODCLSRV              -208038
#define OMDU_NOUPDFLG                  -208040
#define OMDU_XCHN                      -208042
#define OMDU_ETHCOMPRD                 -208044
#define OMDU_ETHCOMPXTA                -208046
#define OMDU_INFCLSNOTVLD              -208048
#define OMDU_INTERR                    -208050
#define OMDU_SNDRUNAVL                 -208052
#define OMDU_RETRNSNOTFND              -208054
#define OMDU_SNDRCFG                   -208056
#define OMDU_TOLNGPNDRCVQUE            -208058
#define OMDU_NOTRUNNING                -208060
#define OMDU_THRSTALLED                -208062
#define OMDU_NORCVRCLT                 -208064
#define OMDU_SCRPTNOTENA               -208066
#define OMDU_MAXINFCLS                 -208068
#define OMDU_NOINFCLS                  -208070
#define OMDU_INVINFCLS                 -208072
#define OMDU_NOVLDADRSTR               -208074
#define OMDU_FLOODED                   -208075
#define OMDU_NOTFLOODED                -208077
#define OMDU_MAXSUB                    -208079
#define OMDU_NOMOREMSGS                -208081
#define OMDU_MOREMSGS                  -208083
#define OMDU_XTASIZENOLL               -208084
#define OMDU_CLTQUETOLNG               -208086
#define OMDU_NOGSTHR                   -208088
#define OMDU_GBLSECUPD                 -208090
#define CACHE_QRYNOCFG                 -209000
#define CACHE_QRYNOAVAIL               -209002
#define CACHE_OVFLOW                   -209004
#define CACHE_COMERR                   -209006
#define CACHE_CFGERR                   -209008
#define CACHE_INTERR                   -209010
#define CACHE_MAXCLI                   -209012
#define CACHE_OKIDOKI                   209013
#define CACHE_FLTREQLOAD               -209015
#define CACHE_BDXNOAVAIL               -209017
#define CACHE_TRXNOAVAIL               -209019
#define CACHE_INSTRCLSREQ              -209021
#define OFS_FUNCTION_IS_OFF             209501
#define OFS_QUERY_INPUT_DATA_LEVEL      209503
#define OFS_QUERY_INPUT_DATA_VARIANT    209505
#define OFS_QUERY_INPUT_SAVE_ACTION     209507
#define OFS_QUERY_FILE_NAME             209509
#define OFS_QUERY_INPUT_FILE_NAME       209511
#define OFS_QUERY_INPUT_DATA_AUTHORITY  209513
#define OFS_QUERY_ANSWER_BUFF_SMALL    -209515
#define OFS_QUERY_SEGMENT_NBR_ZERO      209517
#define OFS_QUERY_PUT_ERROR_1           209519
#define OFS_QUERY_PUT_ERROR_2           209521
#define OFS_QUERY_PUT_ERROR_3           209523
#define OFS_QUERY_PUT_READ_RESULT      -209524
#define OFS_QUERY_FILE_NOT_EXIST        209525
#define OFS_SERVER_INTERNAL_ERROR_1    -209526
#define OFS_SERVER_INTERNAL_ERROR_2    -209528
#define OFS_INVALID_QUERY              -209530
#define OFS_NO_MORE_DATA                209531
#define OFS_FILE_DELETE_SUCCEEDED      -209533
#define OFS_OSS_POSIX_STAT_FAILED      -209535
#define OFS_OSS_POSIX_MKDIR_FAILED     -209537
#define OFS_OSS_POSIX_OPENDIR_FAILED   -209539
#define OFS_OSS_POSIX_ROPEN_FAILED     -209541
#define OFS_OSS_POSIX_READ_FAILED      -209543
#define OFS_OSS_POSIX_WOPEN_FAILED     -209545
#define OFS_OSS_POSIX_WRITE_FAILED     -209547
#define OFS_OSS_POSIX_RM_FAILED        -209549
#define OFS_OSS_POSIX_STD_EXCEPTION    -209551
#define OFS_OSS_POSIX_OTH_EXCEPTION    -209553
#define DBSRV_SS_ERROR                 -210500
#define DBSRV_SS_NORMAL                 210501
#define DBSRV_SS_BUFFEROVF             -210502
#define DBSRV_RMS_RNF                  -210504
#define DBSRV_RMS_EOF                  -210506
#define DBSRV_UNKNOWN_TBL_REQ_TYPE     -210508

/******************************************************************************
*end*of* omn_messages.h
*******************************************************************************/

#endif /* _OMN_MESSAGES_H_ */


#ifndef _CL_MESSAGES_H_
#define _CL_MESSAGES_H_

/******************************************************************************
Module: cl_messages.h

Message definitions generated 2022-12-01 07:07:31 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define CL_OK                           700001
#define CL_CMD_QUEUE                    700003
#define CL_COMPLETED                    700005
#define CL_NOQTY                        700007
#define CL_NYI                         -700009
#define CL_INVACCOUNT                  -700011
#define CL_STATEMISMAT                 -700013
#define CL_STATENOTEND                 -700015
#define CL_STATEEND                    -700017
#define CL_PSSTOPUSR                   -700019
#define CL_PSSTOPRSTRT                 -700021
#define CL_PSALRSTOP                   -700023
#define CL_BUFOVFLW                    -700025
#define CL_REQHOLDING                  -700027
#define CL_INVID                       -700028
#define CL_INVMEMBER                   -700030
#define CL_ERRORTBS                    -700032
#define CL_DB_LOCKED                   -700034
#define CL_ILLVERNOUPD                 -700036
#define CL_INVLOGICAL                  -700038
#define CL_NOSERORVAL                  -700040
#define CL_NODEAL                      -700042
#define CL_INVACCORQTY                 -700044
#define CL_INVDEALSOURCE               -700046
#define CL_NODEALPRICE                 -700048
#define CL_INVPOSREQ                   -700050
#define CL_INCBUYSELL                  -700052
#define CL_INVTIME                     -700054
#define CL_INVFIXING                   -700056
#define CL_PRODEVTEXIST                -700058
#define CL_PRODEVTACTIV                -700060
#define CL_FTBNOTFND                   -700062
#define CL_NOFTBORFUNC                 -700064
#define CL_INVOPER                     -700066
#define CL_NOQTYSUPPL                  -700068
#define CL_TRDNOTFND                   -700070
#define CL_TOOMUCHQTY                  -700072
#define CL_NOPOSFND                    -700074
#define CL_TRDNOTSPEC                  -700076
#define CL_TRDSPEC                     -700078
#define CL_EXENOTSUP                   -700080
#define CL_INVEXEREQ                   -700082
#define CL_INVTRANSTYPE                -700084
#define CL_INVTRANSSIZE                -700086
#define CL_PROTOCOLERRE                -700088
#define CL_PROTOCOLERRD                -700090
#define CL_NOITEMS                     -700092
#define CL_TXNOTHANDLED                -700094
#define CL_INTXNDENIED                 -700096
#define CL_INVTRANSRULE                -700098
#define CL_INVTRADESTATE               -700100
#define CL_INCORRECTSUM                -700102
#define CL_INVACCESSW                  -700104
#define CL_INVACCESSR                  -700106
#define CL_INVACCESSE                  -700108
#define CL_POSNOTSUFCLO                -700110
#define CL_INVSTAFORREV                -700112
#define CL_REMNOTQTY                   -700114
#define CL_INVTRDNUM                   -700116
#define CL_TRDNOTTRANS                 -700118
#define CL_INVNBRTRD                   -700120
#define CL_REMTOBIG                    -700122
#define CL_INVTRAIL                    -700124
#define CL_TRANSNOTNEW                 -700126
#define CL_TRANSNOTPLAIN               -700128
#define CL_ADJACCPOS                   -700130
#define CL_INVSERIES                   -700132
#define CL_INVSERORVAL                 -700134
#define CL_INVBASE                     -700136
#define CL_INVULSERIES                 -700138
#define CL_ERRCREACC                   -700140
#define CL_INVACCSTATE                 -700142
#define CL_NOACCCHANGE                 -700144
#define CL_INVACCOUNTE                 -700146
#define CL_INVACCTYP                   -700148
#define CL_INVACCFEETYP                -700150
#define CL_INVCUSTBANK                 -700152
#define CL_INVCOUNTSIGN                -700154
#define CL_ACCISDEFAULT                -700156
#define CL_TRANSANDSINK                -700158
#define CL_INVCHARINACC                -700160
#define CL_ACCOUNT_STATE               -700162
#define CL_ACCOUNT_DUPL                -700164
#define CL_PRODNOTFND                  -700166
#define CL_INVCHGTYP                   -700168
#define CL_INVPSSTATUS                 -700170
#define CL_TMPPRODSTAT                 -700172
#define CL_PRODSTATNOGOOD              -700174
#define CL_LASTSTATE                   -700176
#define CL_FUNCNOTFND                  -700178
#define CL_FUNCTABOVF                  -700180
#define CL_FUNCONLYTRD                 -700182
#define CL_FUNCONLYPOS                 -700184
#define CL_FUNCONLYFLOW                -700186
#define CL_DUPDELITEM                  -700188
#define CL_DELITEMNFND                 -700190
#define CL_PRDDATENFND                 -700192
#define CL_INVWHEN                     -700194
#define CL_FIM_ERROR                   -700196
#define CL_NOPEVRULES                  -700198
#define CL_INVPEVCLASS                 -700200
#define CL_INVXAXISREP                 -700202
#define CL_DECIMLOST                   -700204
#define CL_INVNBRDEC                   -700206
#define CL_PRODNOTSERIES               -700208
#define CL_INVCONTRVAL                 -700210
#define CL_INVCONTRSIZ                 -700212
#define CL_INVSTRIKEPRICE              -700214
#define CL_INVEVTTYPE                  -700216
#define CL_INCPEVDATA                  -700218
#define CL_UNBALDVP                    -700220
#define CL_PANIC                       -700222
#define CL_INVPROGNAME                 -700224
#define CL_INVPROGPARAM                -700226
#define CL_NEWFAILED                   -700228
#define CL_INVLESTATE                  -700230
#define CL_BADEXEQTY                   -700232
#define CL_BADASSIGN                   -700234
#define CL_BADEXEREQ                   -700236
#define CL_NOTRDDEL                    -700238
#define CL_ONLYBINDEALS                -700240
#define CL_TRANSEXIST                  -700242
#define CL_BWNOMATCH                   -700244
#define CL_INVNBRSHARES                -700246
#define CL_INVCOMMODITY                -700248
#define CL_NOSUCHFILE                  -700250
#define CL_ERROPNFIL                   -700252
#define CL_ERRCREFIL                   -700254
#define CL_ERRCLOFIL                   -700256
#define CL_ERRFLUSHFIL                 -700258
#define CL_ERRWRITEFIL                 -700260
#define CL_ERREADFIL                   -700262
#define CL_ENDOFILE                    -700263
#define CL_BUFTOOSMALL                 -700264
#define CL_BADFILFMT                   -700266
#define CL_BADLNKID                    -700268
#define CL_INVALIDLNK                  -700270
#define CL_BADSEQNBR                   -700272
#define CL_DUPLINCACHE                 -700274
#define CL_UNKREPORT                   -700276
#define CL_INVPEVAUTO                  -700278
#define CL_NOIP5DEAL                   -700280
#define CL_INVQTYPARAM                 -700282
#define CL_RTR_ERROR                   -700284
#define CL_INVMASTERPOSCHG             -700286
#define CL_RECOVFAIL                   -700288
#define CL_GDNNOTNEXT                  -700290
#define CL_PRODEVTEXISTSER             -700292
#define CL_NOISSCONVMOD                -700293
#define CL_SRVREJECT                   -700295
#define CL_NOSRVAVAIL                  -700297
#define CL_SERNOTFND                   -700298
#define CL_INVNOMVAL                   -700300
#define CL_INVEXEREQACCTYPE            -700302
#define CL_INVEXEREQACCPROP            -700304
#define CL_UNEXPTRDUPD                 -700306
#define CL_INVAPTQTY                   -700308
#define CL_RRNOTHOLDING                -700310
#define CL_INVEXEREQSTATE              -700312
#define CL_INVEXEREQTYPE               -700314
#define CL_INVPRDEVTREQSTATE           -700316
#define CL_INVEXEREQDATE               -700318
#define CL_INVPRDEVTREQDATE            -700320
#define CL_INVROLLSTATE                -700322
#define CL_INVROLLBACKTYPE             -700324
#define CL_INVROLLBACKPARAMS           -700326
#define CL_SAMEUSER                    -700328
#define CL_INVCANC                     -700330
#define CL_INVREFPRICE                 -700332
#define CL_INVOUTRVAL                  -700334
#define CL_INVSSTNUM                   -700336
#define CL_INVCDCUPD                   -700338
#define CL_DEALNOTSTORED               -700340
#define CL_ROLLNOTENABLED              -700342
#define CL_PROTOCOLERRT                -700344
#define CL_NOTRADEFORAB                -700346
#define CL_NODEALFORAB                 -700348
#define CL_INCONSISTENCYINGIVEUP       -700350
#define CL_KEYTABOFL                   -700352
#define CL_TRDCLDRCHNG                 -700354
#define CL_INVSUBSEVNT                 -700356
#define CL_INVINSTRTYP                 -700358
#define CL_NODUALPOS                   -700360
#define CL_ILLTRNSTIMEINTERVAL         -700362
#define CL_NOTRNSTIMEOBJ               -700364
#define CL_SQLERROR                    -700366
#define CL_NOMATCHOK                   -700368
#define CL_CDCAGTFULL                  -700370
#define CL_NOTHANDLED                   700371
#define CL_NOAGREEMENT                 -700372
#define CL_AUTHBADSEQNBR               -700374
#define CL_AUTHNOENTRY                 -700376
#define CL_AUTHOBJNOTFOUND             -700378
#define CL_TRADESNOTRECOVERED          -700380
#define CL_PEVSERIESEXP                -700382
#define CL_PEVINVREQDATE               -700384
#define CL_FETCHNEXT                   -700386
#define CL_NOTHOLDING                  -700388
#define CL_INVREJECTCONFIRM            -700390
#define CL_OK_ERRLOG                    700391
#define CL_INVALIDPOSOBJ               -700393
#define CL_POSNOTINMAP                 -700395
#define CL_WRONGPOSINMAP               -700397
#define CL_WRONGPSSTATE                -700398
#define CL_TIMEOUT                      700399
#define CL_NOTREALTIME                 -700400
#define CL_OLDPOSINDB                  -700402
#define CL_ILLEXTNONCTDGUP             -700404
#define CL_ILLEXTPASTEXP               -700406
#define CL_ILLEXTTRANSGUP              -700408
#define CL_EXCEPTIONALEVENT             700409
#define CL_INVPOSALGORITHM             -700410
#define CL_INVREQPOSUPD                -700412
#define CL_INVEXEQTY                   -700414
#define CL_RNGERR                      -700415
#define CL_TRDNOGENSTYLEERR            -700416
#define CL_HOLDINGRCTTRD                700417
#define CL_HOLDINGRCTDEAL               700419
#define CL_PENDINGTMDEAL                700421
#define CL_INVFACILITY                 -700422
#define CL_NOCLOGW                     -700424
#define CL_CLHACCNEWCNTRBLANK          -700426
#define CL_NOCLHACCNEWCNTR             -700428
#define CL_FIXINGNOTFOUND              -700430
#define CL_INVCPNLOTS                  -700432
#define CL_ACCTYPENOTCOL               -700434
#define CL_CLEANPRICEDIFF              -700436
#define CL_INVCLEANPRICE               -700437
#define CL_INVDATE                     -700438
#define CL_INVDAYCALCRULE              -700440
#define CL_INCONSPARTY                 -700442
#define CL_NOSYSTEMCLH                 -700444
#define CL_SERINDELIVERY               -700446
#define CL_INVRISKSCALE                -700448
#define CL_INTERNALERROR               -700450
#define CL_PAYMSERMISSING              -700452
#define CL_HOLDINGDEAL                  700453
#define CL_SQLEOS                      -700454
#define CL_NOACCOUNT                   -700455
#define CL_NOTFOUND                     700457
#define CL_BADCONFIG                   -700459
#define CL_INVALIDCONF                 -700461
#define CL_CACHEINVALID                -700463
#define CL_INVPARTCONF                 -700464
#define CL_STALECDCDATA                -700466
#define CL_BADPSSTATE                  -700468
#define CL_DLVFLWNOTFND                -700470
#define CL_SERVERLOST                  -700472
#define CL_CMD_BDX                      700473
#define CL_INVSRVCMD                   -700474
#define CL_UNEXPTXN                    -700476
#define CL_UNEXPBDX                    -700478
#define CL_UNEXPEVENT                  -700480
#define CL_POSSDATALOSS                -700482
#define CL_DUPLICATE                    700483
#define CL_INVCMDCFG                   -700484
#define CL_OMN_FACILITY_BEGIN           704001
#define CL_OMN_NORMAL                   704003
#define CL_OMN_REQHOLDING               704005
#define CL_OMN_BATNTRN                  704007
#define CL_OMN_NOINFO                   704009
#define CL_OMN_BADARGS                 -704010
#define CL_OMN_UKNWNQRY                -704012
#define CL_OMN_SNDFAIL                 -704014
#define CL_OMN_ALRMOV                  -704016
#define CL_OMN_INVBALISS               -704018
#define CL_OMN_INVBALOWN               -704020
#define CL_OMN_INVCOUNTRY              -704022
#define CL_OMN_INVISSOWN               -704024
#define CL_OMN_INVTRANSNUM             -704026
#define CL_OMN_INVVERNO                -704028
#define CL_OMN_RTRERROR                -704030
#define CL_OMN_TXALRSTORED             -704032
#define CL_OMN_BADPOSUPD               -704034
#define CL_OMN_BADSIZE                 -704036
#define CL_OMN_BADSEG                  -704038
#define CL_OMN_BADSEQ                  -704040
#define CL_OMN_ALRCANC                 -704042
#define CL_OMN_DACEMPTY                -704044
#define CL_OMN_INVACCOUNT              -704046
#define CL_OMN_INVBIDASK               -704048
#define CL_OMN_INVDACACC               -704050
#define CL_OMN_INVDEALSRC              -704052
#define CL_OMN_INVINSTRTYP             -704054
#define CL_OMN_INVITMCNT               -704056
#define CL_OMN_INVMARKET               -704058
#define CL_OMN_INVPRICE                -704060
#define CL_OMN_INVQUANT                -704062
#define CL_OMN_INVSERIES               -704064
#define CL_OMN_INVTRANSTYPE            -704066
#define CL_OMN_NODEALFND               -704068
#define CL_OMN_INVTRDID                -704070
#define CL_OMN_INVMEMBER               -704072
#define CL_OMN_INVCHGREQ               -704074
#define CL_OMN_INVACCESSW              -704076
#define CL_OMN_VALIDERR                -704078
#define CL_OMN_STORERR                 -704080
#define CL_OMN_PRODEVTEXIST            -704082
#define CL_OMN_PRODSTATNOGOOD          -704084
#define CL_OMN_INVACCORQTY             -704086
#define CL_OMN_FUNCONLYTRD             -704088
#define CL_OMN_INVACCESSR              -704090
#define CL_OMN_INVACCESSE              -704092
#define CL_OMN_NOQTYSUPPL              -704094
#define CL_OMN_TRDNOTFND               -704096
#define CL_OMN_TOOMUCHQTY              -704098
#define CL_OMN_NOPOSFND                -704100
#define CL_OMN_TRDNOTSPEC              -704102
#define CL_OMN_TRDSPEC                 -704104
#define CL_OMN_EXENOTSUP               -704106
#define CL_OMN_INVEXEREQ               -704108
#define CL_OMN_INVACCOUNTE             -704110
#define CL_OMN_INVACCSTATE             -704112
#define CL_OMN_INVTRADESTATE           -704114
#define CL_OMN_REMTOBIG                -704116
#define CL_OMN_INVID                   -704118
#define CL_OMN_POSNOTSUFCLO            -704120
#define CL_OMN_INCORRECTSUM            -704122
#define CL_OMN_BADXBALVER              -704124
#define CL_OMN_NOXBALVER               -704126
#define CL_OMN_SIGALRREC               -704128
#define CL_OMN_ILLTRNSTIME             -704130
#define CL_OMN_DB_LOCKED               -704132
#define CL_OMN_INVTIME                 -704134
#define CL_OMN_TXNOTHANDLED            -704136
#define CL_OMN_INTXNDENIED             -704138
#define CL_OMN_INVOPER                 -704140
#define CL_OMN_INTERR                  -704142
#define CL_OMN_INVEXCONF               -704144
#define CL_OMN_INVEXREJECT             -704146
#define CL_OMN_INVTRDCONF              -704148
#define CL_OMN_UNKREPORT               -704150
#define CL_OMN_INVMASTERPOSCHG         -704152
#define CL_OMN_INCBUYSELL              -704154
#define CL_OMN_REMNOTQTY               -704156
#define CL_OMN_INVXACCOP               -704158
#define CL_OMN_ABHOLDTRADE             -704160
#define CL_OMN_INVCMTA                 -704161
#define CL_OMN_INVACCTYPE              -704163
#define CL_OMN_INVOPENCLOSE            -704165
#define CL_OMN_CTRSALRPEND             -704166
#define CL_OMN_CRECTRSALRPEND          -704168
#define CL_OMN_DELCTRSALRPEND          -704170
#define CL_OMN_MODCTRSALRPEND          -704172
#define CL_OMN_CTRSFAILED              -704174
#define CL_OMN_NOPENDCTRS              -704175
#define CL_OMN_INVTRADCODE             -704176
#define CL_OMN_ABPENDAFACTRS           -704178
#define CL_OMN_NODEALSTATE             -704180
#define CL_OMN_BADDEALSTATECHANGE      -704182
#define CL_OMN_TMSERNOTDEF             -704184
#define CL_OMN_INVRCTINTRMTRD          -704186
#define CL_OMN_INVRCTINTRMDEAL         -704188
#define CL_OMN_INVRCTAPTDEAL           -704190
#define CL_OMN_NOTANAPTDEAL            -704192
#define CL_OMN_APTRDNOTFND             -704194
#define CL_OMN_INVTRDTYPRCT            -704196
#define CL_OMN_INVACCEDITRA            -704197
#define CL_OMN_INVACCEDITRAN           -704199
#define CL_OMN_INVACCEDIDATD           -704201
#define CL_OMN_INVACCEDIDATE           -704203
#define CL_OMN_TRANSANDSINK            -704204
#define CL_OMN_TRANSNOAUTONET          -704206
#define CL_OMN_NOACCHNGPROP            -704208
#define CL_OMN_NOMODACCDEL             -704210
#define CL_OMN_NOACCHANGEDEL           -704212
#define CL_OMN_INVBNDQUO               -704214
#define CL_OMN_INVPDBNDQUO             -704216
#define CL_OMN_NOPDMATCH               -704218
#define CL_OMN_PHYSNOTREG              -704219
#define CL_OMN_INVDELIVQTY             -704220
#define CL_OMN_INVCSDACC               -704222
#define CL_OMN_TOOFEWITEMS             -704224
#define CL_OMN_BADDCSEQNBR             -704226
#define CL_OMN_INVPOSREQ               -704228
#define CL_OMN_RECDELIV                -704230
#define CL_OMN_TOOMUCHREG              -704232
#define CL_OMN_PDINHOLD                -704234
#define CL_OMN_DELDISTDONE             -704236
#define CL_OMN_INVSTAFORREV            -704238
#define CL_OMN_TRANSNOTNEW             -704240
#define CL_OMN_MEMSUSPCL               -704242
#define CL_OMN_INVTRANSRULE            -704244
#define CL_OMN_INVOUTRVAL              -704246
#define CL_OMN_TOOMANYLEGS             -704248
#define CL_OMN_TRADEMUSTBEACTIVE       -704250
#define CL_OMN_TRADETYPEINVALID        -704252
#define CL_OMN_INVGIVEUPACCESS         -704254
#define CL_OMN_NOTENOUGHREMAINING      -704256
#define CL_OMN_INVGIVEUPACC            -704258
#define CL_OMN_INVGIVEUPACCSTATE       -704260
#define CL_OMN_INVGIVEUPACCTYPE        -704262
#define CL_OMN_GIVEUPNOTHOLDING        -704264
#define CL_OMN_NOTCORRECTSUM           -704266
#define CL_OMN_NOREJECTACCESS          -704268
#define CL_OMN_NOCONFIRMACCESS         -704270
#define CL_OMN_HOLDINGGIVEUP           -704272
#define CL_OMN_HOLDINGRCTTRD           -704274
#define CL_OMN_MARKSTILOP              -704276
#define CL_OMN_INCONSDEALSRC           -704278
#define CL_OMN_INCONSINSTIG            -704280
#define CL_OMN_INCONSTRADETYPE         -704282
#define CL_OMN_INCTRDTYPORDEASRC       -704284
#define CL_OMN_INCONSBUYSELL           -704286
#define CL_OMN_CLSEQNBRTOOLOW          -704288
#define CL_OMN_CLSEQNBRTOOHIGH         -704290
#define CL_OMN_INVTRDDATA              -704292
#define CL_OMN_INVEXTTRDNUM            -704294
#define CL_OMN_GIVEUPNOINST            -704296
#define CL_OMN_RECTNOREV               -704298
#define CL_OMN_ONLYONEDEAL             -704300
#define CL_OMN_INVCLDATE               -704302
#define CL_OMN_INVACCID                -704304
#define CL_OMN_INVCABIND               -704306
#define CL_OMN_INVCLICAT               -704308
#define CL_OMN_INVEXTTRDFEETYP         -704310
#define CL_OMN_INVEXTDEALNUM           -704312
#define CL_OMN_INVQTY                  -704314
#define CL_OMN_TRANSEXIST              -704316
#define CL_OMN_ACCINVALID              -704318
#define CL_OMN_ACCSTATENCHANGE         -704320
#define CL_OMN_RECURSIVEORIG           -704322
#define CL_OMN_INVORIEXTTRDNUM         -704324
#define CL_OMN_INVACCLVL               -704326
#define CL_OMN_INVSUMFLG               -704328
#define CL_OMN_TRANSNOTPLAIN           -704330
#define CL_OMN_EXTRDNBRNOTUNIQUE       -704332
#define CL_OMN_INVWILDSEARCH           -704334
#define CL_OMN_NODEALPRICE             -704336
#define CL_OMN_UNBALQTY                -704338
#define CL_OMN_INVRESIDUAL             -704340
#define CL_OMN_INVINSTIGANT            -704342
#define CL_OMN_INVSTIMEVT              -704344
#define CL_OMN_INVTRADETYPE            -704346
#define CL_OMN_NOTALLACCTYPE           -704348
#define CL_OMN_INVFINALFLG             -704350
#define CL_OMN_INVFIXLABEL             -704352
#define CL_OMN_IDMTMNOTCOMPLETE        -704354
#define CL_OMN_TRADETOOOLD             -704356
#define CL_OMN_CLRDATECLOSED           -704358
#define CL_OMN_CLRDATENOTCLOSED        -704360
#define CL_OMN_CANCNOTALLNONBIN        -704362
#define CL_OMN_CANCNOTALLINST          -704364
#define CL_OMN_CANCNOTALLWUINST        -704366
#define CL_OMN_CANCNOTALLBOX           -704368
#define CL_OMN_CANCTOLATE              -704370
#define CL_OMN_CANCNOTALLNONBIL        -704372
#define CL_OMN_RCTNOTALL               -704374
#define CL_OMN_NOANNULDEAL              704375
#define CL_OMN_CANTNETGUPEXISTS        -704377
#define CL_OMN_COVERCALLNOTPENDING     -704378
#define CL_OMN_INVCOVERCALL            -704380
#define CL_OMN_INVCOVERCALLACCESS      -704382
#define CL_OMN_INVACCOUNTSTATE         -704384
#define CL_OMN_INVACCOUNTSTATEDEL      -704386
#define CL_OMN_NOACCCHANGE             -704388
#define CL_OMN_ACCISDEFAULT            -704390
#define CL_OMN_COVCALLREQDELIV         -704392
#define CL_OMN_COVERCALLISPENDING      -704394
#define CL_OMN_INVGIVUPINTRMTRD        -704396
#define CL_OMN_INVPROPCHAIN            -704398
#define CL_OMN_POSNOTALLOWED           -704400
#define CL_OMN_INVDLVPROP              -704402
#define CL_OMN_INVTRNSTRDTYP           -704404
#define CL_OMN_INVCOVERQTY             -704406
#define CL_OMN_NOMATCHOK               -704408
#define CL_OMN_TRDCABPRICE             -704410
#define CL_OMN_INVPRICEFORMAT          -704412
#define CL_OMN_INVCABPRICEFORMAT       -704414
#define CL_OMN_RCTDSERNOTACTIVE        -704416
#define CL_OMN_INVDENYQTY              -704418
#define CL_OMN_INVTRDITEMNUM           -704420
#define CL_OMN_TRPNOTFOUND             -704422
#define CL_OMN_PARTYTRPNOTFOUND        -704424
#define CL_OMN_INVAUTH                 -704426
#define CL_OMN_ALREADYAUTH             -704428
#define CL_OMN_ILLTRANSUSER            -704430
#define CL_OMN_ILLCOMMODITY            -704432
#define CL_OMN_INVINTMEMBER            -704434
#define CL_OMN_MEMSUSPTRD              -704436
#define CL_OMN_INV2STEPMTMREQ          -704438
#define CL_OMN_INSNOTTRADED            -704440
#define CL_OMN_TRADEDATEPASSED         -704442
#define CL_OMN_INSISSUSP               -704444
#define CL_OMN_NOMATCHSESSION          -704446
#define CL_OMN_INVUSER                 -704448
#define CL_OMN_MEMNOTNOVATE            -704450
#define CL_OMN_INVTIMEVALIDITY         -704452
#define CL_OMN_PARTYNOTNOVATE          -704454
#define CL_OMN_INVNOVINS               -704456
#define CL_OMN_INVSETTLEDATE           -704458
#define CL_OMN_INVOPSETTLE             -704460
#define CL_OMN_MATDATEPASSED           -704462
#define CL_OMN_EXPDATEPASSED           -704464
#define CL_OMN_INVTRTYPE               -704466
#define CL_OMN_INVTRAUTH               -704468
#define CL_OMN_INVTRCAT                -704470
#define CL_OMN_FACEZERO                -704472
#define CL_OMN_FACENEG                 -704474
#define CL_OMN_FACETOBIG               -704476
#define CL_OMN_YIELDNEG                -704478
#define CL_OMN_YIELDTOBIG              -704480
#define CL_OMN_YIELDANDCONSNOTZERO     -704482
#define CL_OMN_INVTICKSIZE             -704484
#define CL_OMN_CONSIDTOBIG             -704486
#define CL_OMN_TRPALRDCANCEL           -704488
#define CL_OMN_SSIALRDCANCEL           -704490
#define CL_OMN_TRPNOVATED              -704492
#define CL_OMN_INVTRPSTATE             -704494
#define CL_OMN_SUBSTATEPENDCANCEL      -704496
#define CL_OMN_ALREADYNOVATED          -704498
#define CL_OMN_ILLNOVREQ               -704500
#define CL_OMN_INVPARTY                -704502
#define CL_OMN_INVTRPSEARCH            -704504
#define CL_OMN_INVBICCODE              -704506
#define CL_OMN_INVNOVATIONPARAM        -704508
#define CL_OMN_INVSSIPARAM             -704510
#define CL_OMN_INVBUYSSIPARAM          -704512
#define CL_OMN_INVSELLSSIPARAM         -704514
#define CL_OMN_INVAGRPARAM             -704516
#define CL_OMN_CHGPCIDNOTALLW          -704518
#define CL_OMN_CHGPCUSNOTALLW          -704520
#define CL_OMN_CHGACIDNOTALLW          -704522
#define CL_OMN_CHGACUSNOTALLW          -704524
#define CL_OMN_CHGACCNOTALLW           -704526
#define CL_OMN_CHGSERNOTALLW           -704528
#define CL_OMN_CHGFVALNOTALLW          -704530
#define CL_OMN_CHGCONSNOTALLW          -704532
#define CL_OMN_CHGYLDNOTALLW           -704534
#define CL_OMN_CHGSETTDTNOTALLW        -704536
#define CL_OMN_CHGTRDDTNOTALLW         -704538
#define CL_OMN_CHGNAMENOTALLW          -704540
#define CL_OMN_CHGBOSNOTALLW           -704542
#define CL_OMN_CHGNOVNOTALLW           -704544
#define CL_OMN_CHGUSESNOTALLW          -704546
#define CL_OMN_CHGUSEANOTALLW          -704548
#define CL_OMN_CHGTRPCNOTALLW          -704550
#define CL_OMN_INVASOF                 -704552
#define CL_OMN_INVCASHACC              -704554
#define CL_OMN_INVSECACC               -704556
#define CL_OMN_INVDVPACC               -704558
#define CL_OMN_INVPARTINFO             -704560
#define CL_OMN_INVNAME                 -704562
#define CL_OMN_YIELDZERO               -704564
#define CL_OMN_INVCONFREJ              -704566
#define CL_OMN_INVSETTLEDOMCUR         -704568
#define CL_OMN_INVOPETRP               -704570
#define CL_OMN_LETTERCONF              -704572
#define CL_OMN_INVBUYCURRENCY          -704574
#define CL_OMN_INVSELLCURRENCY         -704576
#define CL_OMN_INVCURRENCY             -704578
#define CL_OMN_BUYSELLCUREQUAL         -704580
#define CL_OMN_AGRNOTFOUND             -704582
#define CL_OMN_INVAGRTYPE              -704584
#define CL_OMN_INVCANCEL               -704586
#define CL_OMN_INVFORMAT               -704588
#define CL_OMN_AGREEXISTING            -704590
#define CL_OMN_INVFITYPE               -704592
#define CL_OMN_INVCALCRULE             -704594
#define CL_OMN_AMOUNTZERO              -704596
#define CL_OMN_AMOUNTTOBIG             -704598
#define CL_OMN_AMOUNTNEG               -704600
#define CL_OMN_RATENEG                 -704602
#define CL_OMN_RATEZERO                -704604
#define CL_OMN_INVCASHTYPE             -704606
#define CL_OMN_CHGAMNTNOTALLW          -704608
#define CL_OMN_CHGIRATNOTALLW          -704610
#define CL_OMN_SETTLEPASSEDMATDATE     -704612
#define CL_OMN_SETTLEPASSEDEXPDATE     -704614
#define CL_OMN_NOYIELDRTN              -704616
#define CL_OMN_EXRATEZERO              -704618
#define CL_OMN_BUYAMOUNTZERO           -704620
#define CL_OMN_SELLAMOUNTZERO          -704622
#define CL_OMN_INVMETHOD               -704624
#define CL_OMN_CHGBOSAMTNOTALLW        -704626
#define CL_OMN_CHGBOSCURNOTALLW        -704628
#define CL_OMN_CHGBOSSSINOTALLW        -704630
#define CL_OMN_CHGSELLSINOTALLW        -704632
#define CL_OMN_CHGSTLDOMNOTALLW        -704634
#define CL_OMN_CHGMTHDNOTALLW          -704636
#define CL_OMN_SSIEXISTING             -704638
#define CL_OMN_SSINOTFOUND             -704640
#define CL_OMN_INVINSTLEVEL            -704642
#define CL_OMN_MISMATCHINSTLEVEL       -704644
#define CL_OMN_INVNETONTYPE            -704646
#define CL_OMN_NETINPROG               -704648
#define CL_OMN_INVINTPARTY             -704650
#define CL_OMN_CHGAGRTNOTALLW          -704652
#define CL_OMN_CHGINSTTNOTALLW         -704654
#define CL_OMN_AGRALRDCANCEL           -704656
#define CL_OMN_TRPNETTED               -704658
#define CL_OMN_INVNOVYLDCONS           -704660
#define CL_OMN_EXRATENEG               -704662
#define CL_OMN_CHGERATNOTALLW          -704664
#define CL_OMN_DEALPRICENEG            -704666
#define CL_OMN_CONSIDZERO              -704668
#define CL_OMN_CONSIDNEG               -704670
#define CL_OMN_INVTERMDATE             -704672
#define CL_OMN_TERMBEFORESETTLE        -704674
#define CL_OMN_INVRATEINDEX            -704676
#define CL_OMN_INVRATEFIXDATE          -704678
#define CL_OMN_RATEFIXAFTERSETTLE      -704680
#define CL_OMN_INVDAYCOUNT             -704682
#define CL_OMN_CHGDATETERMNOTALLW      -704684
#define CL_OMN_CHGNOTIONALNOTALLW      -704686
#define CL_OMN_CHGFIXRATNOTALLW        -704688
#define CL_OMN_CHGFRIDXNOTALLW         -704690
#define CL_OMN_CHGFIXDATENOTALLW       -704692
#define CL_OMN_CHGDCCONVNOTALLW        -704694
#define CL_OMN_CHGDEALPRICENOTALLW     -704696
#define CL_OMN_CHGQUANTITYNOTALLW      -704698
#define CL_OMN_CHGCURRNOTALLW          -704700
#define CL_OMN_INVFIXING               -704702
#define CL_OMN_DEALPRICEZERO           -704704
#define CL_OMN_CHGCASHRNOTALLW         -704706
#define CL_OMN_CHGUCONSNOTALLW         -704708
#define CL_OMN_CHGUSETTDTNOTALLW       -704710
#define CL_OMN_UCONSIDTOBIG            -704712
#define CL_OMN_CASHRATENEG             -704714
#define CL_OMN_CASHRATETOBIG           -704716
#define CL_OMN_MARGINRATIONEG          -704718
#define CL_OMN_INVUSETTLEDATE          -704720
#define CL_OMN_USETTLEPASSEDMATDATE    -704722
#define CL_OMN_OBJTYPNOTSSI            -704724
#define CL_OMN_OBJTYPNOTAGR            -704726
#define CL_OMN_OPNOTALLMATCHTRP        -704728
#define CL_OMN_FIXSEREXIST             -704730
#define CL_OMN_MISSFIXSERIES           -704732
#define CL_OMN_INVSELLSETTLEINST       -704734
#define CL_OMN_INVBUYSETTLEINST        -704736
#define CL_OMN_SSIBLANK                -704738
#define CL_OMN_FIXEXIST                -704740
#define CL_OMN_INVRATEINDEXPERIOD      -704742
#define CL_OMN_SELLSETTLEINST          -704744
#define CL_OMN_CONSZEROPAYFALSE        -704746
#define CL_OMN_INVPAYSETTLE            -704748
#define CL_OMN_CHGPAYSETTLENOTALLW     -704750
#define CL_OMN_CHGSETTLEFORCURNOTALLW  -704752
#define CL_OMN_INVSETTLEFORCUR         -704754
#define CL_OMN_INVCHARINMSG            -704756
#define CL_OMN_INVRENEGOPER            -704758
#define CL_OMN_INVRENEGTYPE            -704760
#define CL_OMN_NOTOWNERTRP             -704762
#define CL_OMN_INVFRATYPE              -704764
#define CL_OMN_CHGFRATYPENOTALLW       -704766
#define CL_OMN_MARGINRATIOTOBIG        -704768
#define CL_OMN_MEMNOTSWIFT             -704770
#define CL_OMN_PARTYNOTSWIFT           -704772
#define CL_OMN_INVEFFDATE              -704774
#define CL_OMN_EFFNOTSETTLEDATE        -704776
#define CL_OMN_UFACEVALUETOBIG         -704778
#define CL_OMN_INVUFACEVALUE           -704780
#define CL_OMN_INVUCONSIDERATION       -704782
#define CL_OMN_DEDGENMSG               -704784
#define CL_OMN_INVMSGTYPE              -704786
#define CL_OMN_MSGTYPMNYI              -704788
#define CL_OMN_INVRATEFLOAT            -704790
#define CL_OMN_MISSRATEINDEX           -704792
#define CL_OMN_INVRATEINDEXFIXED       -704794
#define CL_OMN_INITRATENEG             -704796
#define CL_OMN_INVROPERIOD             -704798
#define CL_OMN_INVRODAY                -704800
#define CL_OMN_INVFIXEDORFLOAT         -704802
#define CL_OMN_INVUPFPAYER             -704804
#define CL_OMN_NOUPFDATE               -704806
#define CL_OMN_INVUPFDATE              -704808
#define CL_OMN_NOPUPFAMT               -704810
#define CL_OMN_INVUPFAMT               -704812
#define CL_OMN_NOUPFCUR                -704814
#define CL_OMN_INVUPFCUR               -704816
#define CL_OMN_INVCONDITION            -704818
#define CL_OMN_INVCONDCONF             -704820
#define CL_OMN_INVRATERESET            -704822
#define CL_OMN_INVPAYMENTSET           -704824
#define CL_OMN_INVFRD                  -704826
#define CL_OMN_INVIRSWAPTYPE           -704828
#define CL_OMN_INVRENEGCANCEL          -704830
#define CL_OMN_INVRENEGAUTH            -704832
#define CL_OMN_INVRENEGRECTIFY         -704834
#define CL_OMN_INVFLOSTADAT            -704836
#define CL_OMN_BADFLOSTADAT            -704838
#define CL_OMN_INVFLOENDDAT            -704840
#define CL_OMN_BADENDSTODAT            -704842
#define CL_OMN_INVFLOFIXDAT            -704844
#define CL_OMN_BADFLOFIXDAT            -704846
#define CL_OMN_INVFLOSETDAT            -704848
#define CL_OMN_INVFLOPARTY             -704850
#define CL_OMN_INVFLOOP                -704852
#define CL_OMN_INVFLOTYP               -704854
#define CL_OMN_INVFLOTERM              -704856
#define CL_OMN_INVFLODATSEQ            -704858
#define CL_OMN_INVFIRSTSTART           -704860
#define CL_OMN_INVLASTEND              -704862
#define CL_OMN_INVBUSDAYCONV           -704864
#define CL_OMN_INVRESETDAYS            -704866
#define CL_OMN_CHGUPFPAYERNOTALLW      -704868
#define CL_OMN_CHGUPFDATENOTALLW       -704870
#define CL_OMN_CHGUPFAMTNOTALLW        -704872
#define CL_OMN_CHGUPFCURNOTALLW        -704874
#define CL_OMN_CHGBUSDAYCONVNOTALLW    -704876
#define CL_OMN_CHGRATERESETNOTALLW     -704878
#define CL_OMN_CHGRESETDAYSNOTALLW     -704880
#define CL_OMN_CHGPAYMENTSETNOTALLW    -704882
#define CL_OMN_CHGSPREADNOTALLW        -704884
#define CL_OMN_CHGINITIRNOTALLW        -704886
#define CL_OMN_CHGFRODATENOTALLW       -704888
#define CL_OMN_CHGROPERIODNOTALLW      -704890
#define CL_OMN_CHGRODAYNOTALLW         -704892
#define CL_OMN_CHGFIXEDORFLOATNOTALLW  -704894
#define CL_OMN_CHGPARTYPAYNOTALLW      -704896
#define CL_OMN_INVPRINCEXCH            -704898
#define CL_OMN_INVPRINCEXCHDATE        -704900
#define CL_OMN_INVTERMSTATE            -704902
#define CL_OMN_EFFDATELESSSETTLEDATE   -704904
#define CL_OMN_EFFDATELARGERSETTLEDATE -704906
#define CL_OMN_FIRSTTRANSDATE          -704908
#define CL_OMN_SECONDTRANSDATE         -704910
#define CL_OMN_RENEGNOTFOUND           -704912
#define CL_OMN_MEMNOTFULWLD            -704914
#define CL_OMN_AUTHORIZE                704915
#define CL_OMN_AUTHORIZE_UPDATE        -704916
#define CL_OMN_INVFLOWSTATE            -704918
#define CL_OMN_SWAPTERMNOTFOUND        -704920
#define CL_OMN_ALREADYCONF             -704922
#define CL_OMN_INVLEGNUMBER            -704924
#define CL_OMN_INVSWAPTERMOPER         -704926
#define CL_OMN_INVFULLTERM             -704928
#define CL_OMN_INVTERMPAYER            -704930
#define CL_OMN_INVTERMAGREEDATE        -704932
#define CL_OMN_TERMAGREELESSSETTLEDATE -704934
#define CL_OMN_TERMAGREENOTSETTLEDATE  -704936
#define CL_OMN_INVTERMPAYDATE          -704938
#define CL_OMN_TERMPAYLESSSETTLEDATE   -704940
#define CL_OMN_TERMPAYNOTSETTLEDATE    -704942
#define CL_OMN_TERMPAYAMOUNTNEG        -704944
#define CL_OMN_TERMPAYAMOUNTZERO       -704946
#define CL_OMN_INVTERMINFO             -704948
#define CL_OMN_NOTAMOUNTMISSING        -704950
#define CL_OMN_INVNOTAMOUNT            -704952
#define CL_OMN_NOTAMOUNTTOOBIG         -704954
#define CL_OMN_INVSWAPTERMRECTIFY      -704956
#define CL_OMN_INVSWAPTERMCANCEL       -704958
#define CL_OMN_INVSWAPTERMAUTH         -704960
#define CL_OMN_INVSWAPTERMREJECT       -704962
#define CL_OMN_TRPNOTMATCHED           -704964
#define CL_OMN_INVREJECTRENEG          -704966
#define CL_OMN_INVCHGACCUNWIND         -704968
#define CL_OMN_FACEZEROUNWIND          -704970
#define CL_OMN_NOVTRPRENEG             -704972
#define CL_OMN_AUTHINVUSER             -704974
#define CL_OMN_AUTHOBJDIFF             -704976
#define CL_OMN_INVFACEVALUE            -704978
#define CL_OMN_DUPDELFLOWNOTALLW       -704980
#define CL_OMN_DELFLOWNOTFOUND         -704982
#define CL_OMN_MEMPARTYEQUAL           -704984
#define CL_OMN_PERIODSWAPTOOLONG       -704986
#define CL_OMN_PERIODFRATOOLONG        -704988
#define CL_OMN_FLOWAMOUNTZERO          -704990
#define CL_OMN_FLOWAMOUNTTOOBIG        -704992
#define CL_OMN_FLOWAMOUNTNEG           -704994
#define CL_OMN_INVSELLSETTLESSI        -704996
#define CL_OMN_INVSELLSETTLEPAY        -704998
#define CL_OMN_RATEFIXAFTERPAY         -705000
#define CL_OMN_INVMEMFEETYPE           -705002
#define CL_OMN_MEMFEENOTFOUND          -705004
#define CL_OMN_INVMEMFEEOP             -705006
#define CL_OMN_INVMEMFEECHNGACC        -705008
#define CL_OMN_INVMEMFEECHNGEFBD       -705010
#define CL_OMN_INVMEMFEECHNGEF         -705012
#define CL_OMN_INVMEMFEECHNGUPFRONT    -705014
#define CL_OMN_INVFIRSTBILLDATE        -705016
#define CL_OMN_INVENTRFEEBILLDATE      -705018
#define CL_OMN_ENTRFEEZERO             -705020
#define CL_OMN_ENTRFEENEG              -705022
#define CL_OMN_UPFRONTFEEZERO          -705024
#define CL_OMN_UPFRONTFEENEG           -705026
#define CL_OMN_FLOWRATENEG             -705028
#define CL_OMN_FLOWRATEZERO            -705030
#define CL_OMN_FLOWCONSIDNEG           -705032
#define CL_OMN_FLOWCONSIDZERO          -705034
#define CL_OMN_INVCONF                 -705036
#define CL_OMN_QUANTITYZERO            -705038
#define CL_OMN_QUANTITYTOOBIG          -705040
#define CL_OMN_QUANTITYNEG             -705042
#define CL_OMN_INVFROMTERMAGREEDATE    -705044
#define CL_OMN_INVTOTERMAGREEDATE      -705046
#define CL_OMN_FROMAFTERTOTERMDATE     -705048
#define CL_OMN_INVFROMEFFDATE          -705050
#define CL_OMN_INVTOEFFDATE            -705052
#define CL_OMN_FROMAFTERTOEFFDATE      -705054
#define CL_OMN_FLOWCONSIDTOOBIG        -705056
#define CL_OMN_NOTAMOUNTNEG            -705058
#define CL_OMN_TERMPAYAMOUNTTOOBIG     -705060
#define CL_OMN_RECTIFYNOTALLOWED       -705062
#define CL_OMN_INVPARTYACCOUNT         -705064
#define CL_OMN_INVPARTYACCSTATE        -705066
#define CL_OMN_INVPROXYACCOUNT         -705068
#define CL_OMN_INVPROXYACCSTATE        -705070
#define CL_OMN_CASHTRNSFRGRPEXIST      -705072
#define CL_OMN_CHGCASHCODENOTALLW      -705074
#define CL_OMN_CHGPRXYACCNOTALLW       -705076
#define CL_OMN_CHGPARTYACCNOTALLW      -705078
#define CL_OMN_CASHGRPNOTFOUND         -705080
#define CL_OMN_CASHGRPNOTSAME          -705082
#define CL_OMN_FACETOSMALL             -705084
#define CL_OMN_UPFAMOUNTTOOBIG         -705086
#define CL_OMN_CONFNOTNEEDED           -705088
#define CL_OMN_INVTRPSUBSTATE          -705090
#define CL_OMN_INVRATESPREAD           -705092
#define CL_OMN_INVSSISECACC            -705094
#define CL_OMN_INVSSICASHACC           -705096
#define CL_OMN_UNWINDFACEVALPOS        -705098
#define CL_OMN_INVUNWINDCONS           -705100
#define CL_OMN_REPOCNCLFRISTTXDATE     -705102
#define CL_OMN_REPOCNCLSECONDTXDATE    -705104
#define CL_OMN_REPOCNCLUNWIND          -705106
#define CL_OMN_REPOCNCLEXTNOVATED      -705108
#define CL_OMN_YIELDNOTININTRVL        -705110
#define CL_OMN_EXCHGRATENOTININTRVL    -705112
#define CL_OMN_DEALPRICENOTININTRVL    -705114
#define CL_OMN_INTRATENOTININTRVL      -705116
#define CL_OMN_FIXINTRATENOTININTRVL   -705118
#define CL_OMN_CASHRATENOTININTRVL     -705120
#define CL_OMN_INIINTRATENOTININTRVL   -705122
#define CL_OMN_INVEXEREQACCTYPE        -705124
#define CL_OMN_INVTERMSEARCH           -705126
#define CL_OMN_OPERBLANK               -705128
#define CL_OMN_PROBBLANK               -705130
#define CL_OMN_INVEXTTSTATE            -705132
#define CL_OMN_FIXNOTININTRVL          -705134
#define CL_OMN_GIVEUPNOTALLOWED        -705136
#define CL_OMN_INVFEEREBATE            -705138
#define CL_OMN_INVNOVYLD               -705140
#define CL_OMN_NOSWAPFLOW              -705142
#define CL_OMN_INVSPREAD               -705144
#define CL_OMN_INVHOLDDEALTSTATE       -705146
#define CL_OMN_INVORIGHOLDDEAL         -705148
#define CL_OMN_INVNEWHOLDDEAL          -705150
#define CL_OMN_FLWFIXINTRATNOTININTRVL -705152
#define CL_OMN_INVNOVTODAY             -705154
#define CL_OMN_INVNOVFUTURE            -705156
#define CL_OMN_NOVTODAYCLOSED          -705158
#define CL_OMN_NOVFUTURECLOSED         -705160
#define CL_OMN_NOPRINCEXCHDATE         -705162
#define CL_OMN_PRINCEXRATENEG          -705164
#define CL_OMN_PRINCEXRATEZERO         -705166
#define CL_OMN_PRINCEXCHRATENOTININTRV -705168
#define CL_OMN_INVXCURSWAPTYPE         -705170
#define CL_OMN_INVMEMOBJOWN            -705172
#define CL_OMN_INVCHRINKEY             -705174
#define CL_OMN_INVOBJVER               -705176
#define CL_OMN_HOLDINGDEAL             -705178
#define CL_OMN_CHGPRINCEXDATENOTALLW   -705180
#define CL_OMN_CHGPRINCEXRATENOTALLW   -705182
#define CL_OMN_CHGPRINCEXCHNOTALLW     -705184
#define CL_OMN_CHGFIRSTHOLIDAYNOTALLW  -705186
#define CL_OMN_CHGSECONDHOLIDAYNOTALLW -705188
#define CL_OMN_CHGAPPLYHOLIDAYNOTALLW  -705190
#define CL_OMN_CHGCURRENCYNOTALLW      -705192
#define CL_OMN_INVSTATEHOLIDAY         -705194
#define CL_OMN_STATEHOLIDAYEQUAL       -705196
#define CL_OMN_STATEHOLIDAYBLANK       -705198
#define CL_OMN_INVFIRSTTRANSDATE       -705200
#define CL_OMN_INVACCFEEASSOCSAME      -705202
#define CL_OMN_INVACCFEEASSOCOVER      -705204
#define CL_OMN_INVFIXDATECONSID        -705206
#define CL_OMN_INVNOVUNWIND            -705208
#define CL_OMN_FULLTERMALREXIST        -705210
#define CL_OMN_SECNOTAMOUNTMISSING     -705212
#define CL_OMN_INVSECNOTAMOUNT         -705214
#define CL_OMN_SECNOTAMOUNTTOOBIG      -705216
#define CL_OMN_SECNOTAMOUNTNEG         -705218
#define CL_OMN_BADFLOSETDAT            -705220
#define CL_OMN_NONBINARYDEAL           -705222
#define CL_OMN_INVACCALLNOV            -705224
#define CL_OMN_INVEXCRCARCAL           -705226
#define CL_OMN_INVFIRSTCURRENCY        -705228
#define CL_OMN_INVSECCURRENCY          -705230
#define CL_OMN_CURRENCYNOTMATCH        -705232
#define CL_OMN_FEEREBATECCLEVEL        -705234
#define CL_OMN_NOTMATCHPART            -705236
#define CL_OMN_INVTRANSDATENOV         -705238
#define CL_OMN_ACCOUNTNOTNOVATE        -705240
#define CL_OMN_INVTRANSCASHACC         -705242
#define CL_OMN_INVRCTCOMPFLOW          -705244
#define CL_OMN_MATCHSWAPNOTCANC        -705246
#define CL_OMN_NOVTRPNOTCANC           -705248
#define CL_OMN_NETTEDTRPNOTCANC        -705250
#define CL_OMN_NONDOMCURCANCSETTLEDATE -705252
#define CL_OMN_NOOPRCANCREQ            -705254
#define CL_OMN_TRMCLOSED               -705256
#define CL_OMN_TRMCLOSEDTXN            -705258
#define CL_OMN_INVOPAFTERSETTLE        -705260
#define CL_OMN_INVOPAFTEREFFDATE       -705262
#define CL_OMN_INVOPAFTERSECTRANSDATE  -705264
#define CL_OMN_RCTPAYCALCNOTALLW       -705266
#define CL_OMN_RCTFIXINGNOTALLW        -705268
#define CL_OMN_TERMAGREEAFTERTERMDATE  -705270
#define CL_OMN_MATCHAGREEMENTNOTCANC   -705272
#define CL_OMN_ACCTYPNOTTRD            -705274
#define CL_OMN_ACCTYPNOTPOS            -705276
#define CL_OMN_ACCTYPPLAINEW           -705278
#define CL_OMN_ACCTYPBAD               -705280
#define CL_OMN_ONLYOWNERCANCEL         -705282
#define CL_OMN_INVCLHDATECANCEL        -705284
#define CL_OMN_CONREJONLYPARTY         -705286
#define CL_OMN_AUTHDONE                -705288
#define CL_OMN_INSTNOTTRADEDCLICK      -705290
#define CL_OMN_FIRSTTRADETIMENOTPASSED -705292
#define CL_OMN_INVOLDDATE              -705294
#define CL_OMN_INVNEWDATE              -705296
#define CL_OMN_DATENOTMATCHTERM        -705298
#define CL_OMN_INVTYPEOFDATE           -705300
#define CL_OMN_DATENOTMATCHUNWIND      -705302
#define CL_OMN_TERMDATENOTMATCH        -705304
#define CL_OMN_PRINCEXCHDATENOTMATCH   -705306
#define CL_OMN_DATENOTMATCHFLOWSTART   -705308
#define CL_OMN_DATENOTMATCHFLOWEND     -705310
#define CL_OMN_DATENOTMATCHFIXING      -705312
#define CL_OMN_DATENOTMATCHPAYMENT     -705314
#define CL_OMN_DATENOTMATCHPRINCEXCH   -705316
#define CL_OMN_SWAPNOTMATCH            -705318
#define CL_OMN_NOOPRPNDRNG             -705320
#define CL_OMN_NOOPRPNDFXNG            -705322
#define CL_OMN_DEALNOTHOLDING          -705324
#define CL_OMN_DEALNOTFOUND            -705326
#define CL_OMN_TRADEONHOLD             -705328
#define CL_OMN_INVCLHSERIES            -705330
#define CL_OMN_TRADEIDNOTUNIQUE        -705332
#define CL_OMN_EXPDATENOTVALID         -705334
#define CL_OMN_INVEXPDATE              -705336
#define CL_OMN_FPMLSCHEMA              -705338
#define CL_OMN_FPMLFORMAT              -705340
#define CL_OMN_BUSINESSCENTERPRESENT   -705342
#define CL_OMN_BUSINESSCENTERMISSING   -705344
#define CL_OMN_INVDAYTYPECONVENTION    -705346
#define CL_OMN_TRPALRDTERM             -705348
#define CL_OMN_TRPALRDUNWIND           -705350
#define CL_OMN_TRPALRDINCREASE         -705352
#define CL_OMN_FPML_MESSAGEREJECTED     705353
#define CL_OMN_FPML_INVMSGTYPE         -705354
#define CL_OMN_FPML_UNSMSGTYP          -705356
#define CL_OMN_TRADEIDMANDATORY        -705358
#define CL_OMN_INVEXCHLOOKFORINDEX     -705360
#define CL_OMN_INVEXCHTRADEFORSHARE    -705362
#define CL_OMN_INVFUTPRICEFORSHARE     -705364
#define CL_OMN_INVSETTLETYPEFORINDEX   -705366
#define CL_OMN_INVSETTLEPAYFORPHYSETTL -705368
#define CL_OMN_TRADEDATEAFTERPAYDATE   -705370
#define CL_OMN_TRADEDATEAFTERSETTLE    -705372
#define CL_OMN_TRADEDATEAFTEREXPDATE   -705374
#define CL_OMN_COMDATEAFTEREXPDATE     -705376
#define CL_OMN_BUYSELLEQUAL            -705378
#define CL_OMN_STRIKEPRICENEG          -705380
#define CL_OMN_STRIKEPRICEZERO         -705382
#define CL_OMN_NBROFOPTNEG             -705384
#define CL_OMN_NBROFOPTZERO            -705386
#define CL_OMN_NBROFOPTBADFORMAT       -705388
#define CL_OMN_PAYAMOUNTNEG            -705390
#define CL_OMN_PAYAMOUNTZERO           -705392
#define CL_OMN_MINNBROFOPTNEG          -705394
#define CL_OMN_MINNBROFOPTZERO         -705396
#define CL_OMN_MINNBROFOPTBADFORMAT    -705398
#define CL_OMN_INTMULTEXERNEG          -705400
#define CL_OMN_INTMULTEXERZERO         -705402
#define CL_OMN_INTMULTEXERBADFORMAT    -705404
#define CL_OMN_NEWNBROFOPTNEG          -705406
#define CL_OMN_NEWNBROFOPTZERO         -705408
#define CL_OMN_SETTLEAMOUNTNEG         -705410
#define CL_OMN_SETTLEAMOUNTZERO        -705412
#define CL_OMN_NBROFSHARESNEG          -705414
#define CL_OMN_NBROFSHARESZERO         -705416
#define CL_OMN_FORWARDPRICENEG         -705418
#define CL_OMN_FORWARDPRICEZERO        -705420
#define CL_OMN_PRICEPEROPTNEG          -705422
#define CL_OMN_PRICEPEROPTZERO         -705424
#define CL_OMN_NBROFSHARESBADFORMAT    -705426
#define CL_OMN_INVSTRIKEPRICEDEC       -705428
#define CL_OMN_INVPRICEPEROPTDEC       -705430
#define CL_OMN_INVFORWARDPRICEDEC      -705432
#define CL_OMN_TRPTYPENOTEQUAL         -705434
#define CL_OMN_TRPSUBTYPENOTEQUAL      -705436
#define CL_OMN_INVCOMMODITYRIC         -705438
#define CL_OMN_INVCOMMODITYISIN        -705440
#define CL_OMN_COMMODITYDELISTED       -705442
#define CL_OMN_INVPAYMENTDATE          -705444
#define CL_OMN_MSNGFPLMNT              -705446
#define CL_OMN_2MNYFPLMNTS             -705448
#define CL_OMN_BADFPMLSTRCT            -705450
#define CL_OMN_EMPTYFPMLMNT            -705452
#define CL_OMN_NOSTRVAL                -705454
#define CL_OMN_BADFPMLDATEFMT          -705456
#define CL_OMN_XMLMNTNOTFND            -705458
#define CL_OMN_XMLMNTNOTUNIQU          -705460
#define CL_OMN_XMLMNTEXPTD             -705462
#define CL_OMN_XMLMNTNOTI16            -705464
#define CL_OMN_XMLMNTNOTUI16           -705466
#define CL_OMN_XMLMNTNOTUI32           -705468
#define CL_OMN_XMLMNTNOTUI64           -705470
#define CL_OMN_XMLMNTNOTCHAR           -705472
#define CL_OMN_XMLMNTNOTDATE           -705474
#define CL_OMN_XMLNOATTRIB             -705476
#define CL_OMN_XMLATTRIBNOTFND         -705478
#define CL_OMN_FPMLNOTNUMERIC          -705480
#define CL_OMN_UNEXPFPMLMNT            -705482
#define CL_OMN_INVXMLREF               -705484
#define CL_OMN_FPMLNOTSUPP             -705486
#define CL_OMN_MAXSWAPFLOWS            -705488
#define CL_OMN_INVOPAFTERTERMDATE      -705490
#define CL_OMN_INVRENEGREJECT          -705492
#define CL_OMN_INVRECTIFYRENEG         -705494
#define CL_OMN_INVCANCELRENEG          -705496
#define CL_OMN_MARGINRATIONOTININTRVL  -705498
#define CL_OMN_CHGMARGINRATIONOTALLW   -705500
#define CL_OMN_CASHTRNSFRGRPROWEXIST   -705502
#define CL_OMN_REPOCNCLFULLUNWOUND     -705504
#define CL_OMN_INVCALCUNWINDCONSID     -705506
#define CL_OMN_NOTOWNERAGREEMENT       -705508
#define CL_OMN_NOTOWNERTERMINATION     -705510
#define CL_OMN_NOTOWNERUNWIND          -705512
#define CL_OMN_NOTOWNERRENEG           -705514
#define CL_OMN_RENEGNOTALLWUNWIND      -705516
#define CL_OMN_MEMNOTAUTHONBEHALF      -705518
#define CL_OMN_MEMPARTYEQUALAGR        -705520
#define CL_OMN_NODBLVALU               -705522
#define CL_OMN_NOINTVALU               -705524
#define CL_OMN_NOUINTVALU              -705526
#define CL_OMN_BADSCHEMALOC            -705528
#define CL_OMN_UNKNOWNBIC              -705530
#define CL_OMN_BADFPMLMNTVALUE         -705532
#define CL_OMN_INVOPTYPE               -705534
#define CL_OMN_INVOPTSTYLE             -705536
#define CL_OMN_INVSETTLTYPE            -705538
#define CL_OMN_ISINOTFND               -705540
#define CL_OMN_TMRKTNOTFND             -705542
#define CL_OMN_INSTYPNOTFND            -705544
#define CL_OMN_TMPLSERNOTFND           -705546
#define CL_OMN_PRICEPEROPTTOOBIG       -705548
#define CL_OMN_STRIKEPRICETOOBIG       -705550
#define CL_OMN_FORWARDPRICETOOBIG      -705552
#define CL_OMN_NOPTID                  -705554
#define CL_OMN_MISNGPTID               -705556
#define CL_OMN_2MNYPTID                -705558
#define CL_OMN_CANTCMP                 -705560
#define CL_OMN_DIFFPRD                 -705562
#define CL_OMN_NOFPMLPROD              -705564
#define CL_OMN_NOFPMLPRODCB            -705566
#define CL_OMN_2MNYSGMNTS              -705568
#define CL_OMN_OPNINTSIGRCVD           -705570
#define CL_OMN_INVSETTLETYPEPRODUCT    -705572
#define CL_OMN_USANDEUNOTCOMP          -705574
#define CL_OMN_STANDALONENOTEQUAL      -705576
#define CL_OMN_CHANGEFORMATNOTALLW     -705578
#define CL_OMN_INVMULTEXCHFORSHARE     -705580
#define CL_OMN_ONLY_CLOSING            -705581
#define CL_OMN_INVCOMMODITYCUR         -705582
#define CL_OMN_INVPARTSERIES           -705584
#define CL_OMN_NOCANCELPRICECORR       -705586
#define CL_OMN_TXNONCLASSDENIED        -705588
#define CL_OMN_UNDEFACP                -705590
#define CL_OMN_TRDREPNOTINIT           -705592
#define CL_OMN_TRDREPNOTFND            -705594
#define CL_OMN_INVINSTANCENUMBER       -705596
#define CL_OMN_INVDEALCOLSEQUENCE      -705598
#define CL_OMN_CANOLDNOTALLW           -705600
#define CL_OMN_INVCANTRDID             -705602
#define CL_OMN_DATAINCOMPLETE          -705604
#define CL_OMN_DATAPURGED               705605
#define CL_OMN_INVSERTRDSLP            -705606
#define CL_OMN_CANITNNOTALLW           -705608
#define CL_OMN_DATAINCOMPLETE_NORECOV  -705610
#define CL_OMN_EXTIDBLANK              -705612
#define CL_OMN_EXTIDEXIST              -705614
#define CL_OMN_BADASOF                 -705616
#define CL_OMN_EXTIDNOTFND             -705618
#define CL_OMN_OBLRIGHTNOTFND          -705620
#define CL_OMN_INVCONTROLLER           -705622
#define CL_OMN_INVREGISTRAR            -705624
#define CL_OMN_DUPACCREGCONTROL        -705626
#define CL_OMN_NOTSECLEND              -705628
#define CL_OMN_NOTPOOLTYPE             -705630
#define CL_OMN_NOTBILTYPE              -705632
#define CL_OMN_EFFPASSEDLASTTRDDATE    -705634
#define CL_OMN_USETTLEPASSEDLASTTRD    -705636
#define CL_OMN_INVFROMSETTDATE         -705638
#define CL_OMN_INVTOSETTDATE           -705640
#define CL_OMN_FROMAFTERTOSETTDATE     -705642
#define CL_OMN_WILDCARDNOTALLOWED      -705644
#define CL_OMN_INVPOSADJAMT            -705645
#define CL_OMN_OLDTRDNOTALWAPT         -705646
#define CL_OMN_INVQTYINCR              -705648
#define CL_OMN_ACCALIASNOTUNIQUE       -705650
#define CL_OMN_INVCHARINACCALIAS       -705652
#define CL_OMN_CANCLOLDAPTNOTALLWD     -705654
#define CL_OMN_POSNOTFND               -705656
#define CL_OMN_INVEXTACCRTYPE          -705658
#define CL_OMN_INVEXTACCCONTR          -705660
#define CL_OMN_INVEXTACCVER            -705662
#define CL_OMN_EXTACCCONEXISTS         -705664
#define CL_OMN_NOEXTACCCON             -705666
#define CL_OMN_UPDEXTACCNOTALLW        -705668
#define CL_OMN_INVTRDSESSTATE          -705670
#define CL_OMN_GUPNEWCONTRNOTALLWD      705671
#define CL_OMN_RCTACCNEWCONTRNOTALLWD   705673
#define CL_OMN_RCTNEWCONTRNOTALLWD      705675
#define CL_OMN_INVDAYSRESTRID          -705676
#define CL_OMN_INVCLOSEOUTQTY          -705678
#define CL_OMN_REINSTATENOTFND         -705680
#define CL_OMN_REINSTATENOTALLWD       -705682
#define CL_OMN_INVACCEDIPOS            -705684
#define CL_OMN_NOMODACCTYPORIGIN       -705686
#define CL_OMN_NOMODACCTYP             -705688
#define CL_OMN_INVCONSTCLOFIHELDQTY    -705690
#define CL_OMN_FINHELDADJNOTALLWD      -705692
#define CL_OMN_NEGCLOSEOUT             -705694
#define CL_OMN_NEGFINHELD              -705696
#define CL_OMN_NEGREINSTATEQTY         -705698
#define CL_OMN_CLOSEOUTNOTALLWD        -705700
#define CL_OMN_CLOSEOUTFUTNOTALLWD     -705702
#define CL_OMN_REINSTATEFUTNOTALLWD    -705704
#define CL_OMN_INVREINSTATEQTY         -705706
#define CL_OMN_EXCLREQEXISTS           -705708
#define CL_OMN_REINCLREQEXISTS         -705710
#define CL_OMN_DEALTOOLDEXCL           -705712
#define CL_OMN_INVEXCLREQUEST          -705714
#define CL_OMN_QRYDATEINV              -705716
#define CL_OMN_DEALRECTEXCL            -705718
#define CL_OMN_ONLYALLOWRCTPRICECBO    -705720
#define CL_OMN_FINALHELDTOOBIG         -705722
#define CL_OMN_NOPOSADJ                 705723
#define CL_OMN_INVREQDATE              -705724
#define CL_OMN_INVACCTYPFORSTATECHNG   -705726
#define CL_OMN_RCTCOMBONOTALLWD         705727
#define CL_OMN_NOFASTPURSYM            -705729
#define CL_OMN_NOFASTPURSYMDAYS        -705730
#define CL_OMN_TRPCODENOTFND           -705732
#define CL_OMN_INCONSOCR               -705734
#define CL_OMN_ACTIVEACCDEP            -705735
#define CL_OMN_NONLCLTRNSGUP           -705736
#define CL_OMN_ILLDSTMEMINS            -705738
#define CL_OMN_POSTRANSNOTALL          -705740
#define CL_OMN_DUPLPOSCLOSEOUT         -705742
#define CL_OMN_NOITEMS                 -705744
#define CL_OMN_CBOGUPNOTALL            -705746
#define CL_OMN_GUPSERNOTACT            -705748
#define CL_OMN_WRONGUPSER              -705750
#define CL_OMN_REQNOTPENDING           -705752
#define CL_OMN_INVLESTATE              -705754
#define CL_OMN_CTDPOSNOTSET            -705756
#define CL_OMN_NOSTNDALONGUP           -705758
#define CL_OMN_TXNTOOLATE              -705760
#define CL_OMN_DIFFERENTCLEARINGORG    -705762
#define CL_OMN_INVMEMINACC             -705764
#define CL_OMN_INVSETTLECLASS          -705766
#define CL_OMN_INVEXTACCRSTATE         -705768
#define CL_OMN_INVEXTACCID             -705770
#define CL_OMN_FEEASSOCACCANDTYPE      -705772
#define CL_OMN_INVMARGINCLASS          -705773
#define CL_OMN_IFACIDXNOTPOSSIBLE      -705774
#define CL_OMN_IFACREFNOTPOSSIBLE      -705776
#define CL_OMN_INVRISKCURCONV          -705778
#define CL_OMN_INVRISKMARGINNET        -705780
#define CL_OMN_INVEXTACCINTERM         -705782
#define CL_OMN_NOACCCHANGEINACTIVE     -705784
#define CL_OMN_NOINTEGRITYDEF          -705785
#define CL_OMN_INVAMOUNT               -705786
#define CL_OMN_INVTERMPAYAMOUNT        -705788
#define CL_OMN_INVFLOWAMOUNT           -705790
#define CL_OMN_INVCONSIDERATION        -705792
#define CL_OMN_INVFIRSTSTUB            -705794
#define CL_OMN_INVLASTSTUB             -705796
#define CL_OMN_INVRCFACCMATCHED        -705798
#define CL_OMN_INVCONFUSER             -705800
#define CL_OMN_NOTPENDINGCLMEMACC      -705802
#define CL_OMN_INVTRDANDPOSACCCLH      -705804
#define CL_OMN_INVPOSACCANDCONFMEM     -705806
#define CL_OMN_INVPDNCLHCONF           -705808
#define CL_OMN_INVCOLLHAND             -705810
#define CL_OMN_INVUPFRONT              -705812
#define CL_OMN_EXPNOTINFUTURE          -705814
#define CL_OMN_EXPBEFOREEFF            -705816
#define CL_OMN_FRAINTRATNOTININTRVL    -705818
#define CL_OMN_ACCPROPDELIVERY         -705820
#define CL_OMN_ACCPROPSETTLEMENT       -705822
#define CL_OMN_NOABSTARTSESS           -705824
#define CL_OMN_INIINTRATEMISSING       -705826
#define CL_OMN_INVSETTLINSTR           -705828
#define CL_OMN_INVLEGALACCINSTR        -705830
#define CL_OMN_COLLCHECK                705831
#define CL_OMN_PRENOVREQ               -705833
#define CL_OMN_PRENOVREQFAIL           -705834
#define CL_OMN_INVEXPLIMIT             -705836
#define CL_OMN_NOABSTARTEOD            -705838
#define CL_OMN_OLDGUPTRANSITORY        -705840
#define CL_OMN_INVRCTADJTRD            -705842
#define CL_OMN_ISSUEPENDING            -705844
#define CL_OMN_NOANULPARTXFR           -705846
#define CL_OMN_RCTRDBEFRGUP            -705848
#define CL_OMN_INVMIXCLDATE            -705850
#define CL_OMN_FUTEXPNOTSUPP           -705852
#define CL_OMN_LARGEDATA               -705853
#define CL_OMN_ONLYCANCLALLWD          -705855
#define CL_OMN_RCTRDREQEXIST           -705856
#define CL_OMN_RCTDEALREQEXIST         -705858
#define CL_OMN_INVALLOWTAKEUP          -705860
#define CL_OMN_INVAUTOTAKEUP           -705862
#define CL_OMN_AATINVALIDCONNECTION    -705864
#define CL_OMN_AATCONNECTIONEXIST      -705866
#define CL_OMN_TAKEUPNOLIMIT           -705868
#define CL_OMN_INVGROSSNET             -705870
#define CL_OMN_INVSETPRODFORMAT        -705872
#define CL_OMN_SETPRODEXIST            -705874
#define CL_OMN_CCNOTALLOW              -705876
#define CL_OMN_NETMUSTEXIST            -705878
#define CL_OMN_FACILITY_END             705879
#define CL_OCC_CHGCLOSETOOPENBUY       -708001
#define CL_OCC_CHGCLOSETOOPENSELL      -708003
#define CL_OCC_CHGCLOSETOOPENBOTH      -708005
#define CL_OCC_NONNUMTRADEDATE         -708007
#define CL_OCC_TRADEDATENOTCUR         -708009
#define CL_OCC_NONNUMTRDEXTIME         -708011
#define CL_OCC_NONNUMTRDNUMBER         -708013
#define CL_OCC_NONNUMTRDPRICE          -708015
#define CL_OCC_NONNUMTRDVOL            -708017
#define CL_OCC_INVCHGCODE              -708019
#define CL_OCC_INVBUYMEM               -708021
#define CL_OCC_INVBUYACC               -708023
#define CL_OCC_INVBUYOPCL              -708025
#define CL_OCC_INVSELLMEM              -708027
#define CL_OCC_INVSELLACC              -708029
#define CL_OCC_INVSELLOPCL             -708031
#define CL_OCC_INVCHGSELLOPCL          -708033
#define CL_OCC_INVBUYOCCHGTR           -708035
#define CL_OCC_INVSELLOCCHGTR          -708037
#define CL_OCC_INVCHGBUYOPCL           -708039
#define CL_OCC_INVSERIES               -708041
#define CL_OCC_CLTRDBUYACC             -708043
#define CL_OCC_CLTRDSELLAC             -708045
#define CL_OCC_SELLINSUFPOS            -708047
#define CL_OCC_BUYINSUFPOS             -708049
#define CL_OCC_INVMEMBER               -708051
#define CL_OCC_INVACCOUNT              -708053
#define CL_OCC_EXERCVOLZERO            -708055
#define CL_OCC_INVEXERCVOL             -708057
#define CL_OCC_UNCLTRDCHG              -708059
#define CL_OCC_ALRTRDCHG               -708061
#define CL_OCC_PREMEXBYEX              -708063
#define CL_OCC_DUPLTRDNUMBER           -708065
#define CL_OCC_BUYGUPFORHOUSE          -708067
#define CL_OCC_SELLGUPFORHOUSE         -708069
#define CL_OCC_BUYGUPCHGREQ            -708071
#define CL_OCC_BUYGUPDELREQ            -708073
#define CL_OCC_SELLGUPCHGREQ           -708075
#define CL_OCC_SELLGUPDELREQ           -708077
#define CL_OCC_INVBUYGUPREQTR          -708079
#define CL_OCC_INVSELLGUPREQTR         -708081
#define CL_OCC_INVBUYGUP               -708083
#define CL_OCC_INVSELLGUP              -708085
#define CL_OCC_INVBUYSELLGUP           -708087
#define CL_OCC_INVBUYGUPBUYCHG         -708089
#define CL_OCC_INVBUYGUPSELLCHG        -708091
#define CL_OCC_INVBUYGUPBUYSELLCHG     -708093
#define CL_OCC_INVSELLGUPBUYCHG        -708095
#define CL_OCC_INVSELLGUPSELLCHG       -708097
#define CL_OCC_INVSELLGUPBUYSELLCHG    -708099
#define CL_OCC_INVBUYSELLGUPBUYCHG     -708101
#define CL_OCC_INVBUYSELLGUPSELLCHG    -708103
#define CL_OCC_INVBUYSELLGUPBUYSELLCHG -708105
#define CL_DC_TRDCHANGED               -709001
#define CL_DC_TRDREJECT                -709003
#define CL_MDF_VERSION                  710001

/******************************************************************************
*end*of* cl_messages.h
*******************************************************************************/

#endif /* _CL_MESSAGES_H_ */


#ifndef _PS_MESSAGES_H_
#define _PS_MESSAGES_H_

/******************************************************************************
Module: ps_messages.h

Message definitions generated 2022-12-01 07:07:31 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define PS_NORMAL                       1900001
#define PS_INTERR                      -1900002
#define PS_FUTBUSDATE                  -1900004
#define PS_BUSNOTTOD                   -1900006
#define PS_INVSTARTDATEFMT             -1900008
#define PS_INVENDDATEFMT               -1900010
#define PS_INVOFFICIALFMT              -1900012
#define PS_PERIODNOTZERO               -1900014
#define PS_INVOFFBATLOG                -1900016
#define PS_BATLOGNOTFND                -1900018
#define PS_TERMSTRUCTCURVENOTFND       -1900020
#define PS_INVBUSDATFMT                -1900022
#define PS_INVCOUNTRYIDFMT             -1900024
#define PS_SERIESNOTFND                -1900026
#define PS_CURVENOTFND                 -1900028
#define PS_PRIMARYCURVENOTFND          -1900030
#define PS_SECONDARYCURVENOTFND        -1900032
#define PS_CFMNOTFND                   -1900034
#define PS_SERIESORCURVE               -1900036
#define PS_DAYCOUNTCONVNOTZERO         -1900038
#define PS_DISCMETHODNOTBLANK          -1900040
#define PS_SERIESNOTZERO               -1900042
#define PS_COUNTRYIDBLANK              -1900044
#define PS_CURVEIDBLANK                -1900046
#define PS_ENDNOTLATERSTART            -1900048
#define PS_STARTBEFOREBUS              -1900050
#define PS_PERIODLESSTHAN1             -1900052
#define PS_PERIODNOTMULT30             -1900054
#define PS_WRONGRUNTYPE                -1900056
#define PS_INVFROMDATEFMT              -1900058
#define PS_INVTODATEFMT                -1900060
#define PS_NOTACTIVE                   -1900062
#define PS_SUSPENDED                   -1900064
#define PS_EXPIRED                     -1900066
#define PS_NOPRICEPARAMSCONFIGURATION  -1900068
#define PS_INVALIDTIMESTAMP            -1900070
#define PS_INVALIDOPERATION            -1900072
#define PS_INVALIDDECIMALSVALUE        -1900074
#define PS_NOSNAPSHOT                  -1900076
#define PS_NOPRICECOLLECTION           -1900078
#define PS_INVSNAPSHOTRUNTYPE          -1900080
#define PS_NOTSAMERUNTYPE              -1900082
#define PS_INVCOLLECTIONPRICETYPE      -1900084
#define PS_NOSNAPSHOTCREATE            -1900086
#define PS_NOMARKET                    -1900088
#define PS_INVMARKET                   -1900090
#define PS_NOSNAPSHOTID                -1900092
#define PS_NOCOLLECTIONID              -1900094
#define PS_NOSERIESNAME                -1900096
#define PS_ABSWRNLEVEL                 -1900098
#define PS_PERWRNLEVEL                 -1900100
#define PS_INVALIDSTATECHANGE          -1900102
#define PS_INSNOTOPTION                -1900104
#define PS_INSNOTFUTURE                -1900106
#define PS_INVOPTIONTYPE               -1900108
#define PS_INVBOUGHTORSOLD             -1900110
#define PS_ONEITEMALLOWED              -1900112
#define PS_INVALIDSEQVALUE             -1900114
#define PS_INVALIDVALUATIONDATE        -1900116
#define PS_BATCHNOTCOMPLETED           -1900118
#define PS_OLDSNAPSHOT                 -1900120
#define PS_OLDPRICECOLLECTION          -1900122
#define PS_INVALIDSNAPSHOTPRICETYPE    -1900124
#define PS_INVALIDPRICEATTRIBUTE       -1900126
#define PS_NOWORKINPROGRESS            -1900128
#define PS_INVNAMEDSTRUCTCOMB          -1900130
#define PS_BARRIERNOTSUPPORTED         -1900132
#define PS_INVWORKINPROGRESSVALUE      -1900134
#define PS_BATCHLOCKED                 -1900136
#define PS_FOUREYECONTROL              -1900138
#define PS_INVALIDPRICESOURCETYPE      -1900140
#define PS_OVERRIDEONLYMANUAL          -1900142
#define PS_STRIKEPRICEZERO             -1900144
#define PS_RISKFREERATETOLARGE         -1900146
#define PS_DIVIDENDYIELDTOLARGE        -1900148
#define PS_INVCOLIDEXTREF              -1900150
#define PS_VOLATILITYNEGATIVE          -1900152
#define PS_NOCOLIDEXTREF               -1900154
#define PS_NOPRIVATEPRICELISTID        -1900156
#define PS_INVPRIVATEPRICELISTID       -1900158
#define PS_NOPRIVATEPRICELIST          -1900160
#define PS_RQ65DEPRECATED              -1900162
#define PS_RC65DEPRECATED              -1900164
#define PS_NODATEVIM                   -1900166
#define PS_UNDERLYINGNEGATIVE          -1900168
#define PS_STRIKENEGATIVE              -1900170
#define PS_KNOCKOUTNEGATIVE            -1900172
#define PS_DIVIDENDYIELDNEGATIVE       -1900174
#define PS_INTERESTRATENEGATIVE        -1900176
#define PS_INVPRICEVALIDATIONTYPE      -1900178
#define PS_INVISSUEDATE                -1900180
#define PS_KEEPDAYSNEGATIVE            -1900182
#define PS_INVPRIVATEPRICELISTBASE     -1900184
#define PS_NOTPUBLISHED                -1900186
#define PS_RC316DEPRECATED             -1900188
#define PS_SNAPSHOTCOPYNOTALLOWED      -1900190
#define PS_COLLECTIONCANNOTBEUPDATED   -1900192
#define PS_BADDATE                     -1900194
#define PS_NOINFO                       1900195

/******************************************************************************
*end*of* ps_messages.h
*******************************************************************************/

#endif /* _PS_MESSAGES_H_ */


#ifndef _RI_MESSAGES_H_
#define _RI_MESSAGES_H_

/******************************************************************************
Module: ri_messages.h

Message definitions generated 2022-12-01 07:07:31 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define RI_NORMAL                       900001
#define RI_NYI                         -900002
#define RI_BADMEMBER                   -900004
#define RI_PANIC                       -900006
#define RI_BADSIZE                     -900008
#define RI_BADFORMAT                   -900010
#define RI_TOOMUCHDATA                 -900012
#define RI_ITEXC                       -900014
#define RI_INVPAR                      -900016
#define RI_RANGERR                     -900018
#define RI_NOMEM                       -900020
#define RI_INVPTR                      -900022
#define RI_INVSUB                      -900024
#define RI_ACCVIO                      -900026
#define RI_INTOFLOW                    -900028
#define RI_FLOOFLOW                    -900030
#define RI_FLOUFLOW                     900031
#define RI_INTDIVZERO                  -900032
#define RI_FLODIVZERO                  -900034
#define RI_EOF                         -900035
#define RI_NOTFOUND                    -900037
#define RI_NOROUNDING                  -900039
#define RI_EXTERR                      -900040
#define RI_EXTWARN                     -900041
#define RI_EXTFATAL                    -900042
#define RI_INVTXSTATE                  -900044
#define RI_INVCMD                      -900045
#define RI_UNSUPPCMD                   -900047
#define RI_BUFTOSMALL                  -900048
#define RI_HPARITHTRAP                 -900050
#define RI_REPLAY                       900051
#define RI_INVEXCHANGE                 -900052
#define RI_WAIT_CDB                     900053
#define RI_WAIT_IN_NPSRV                900055
#define RI_WAIT_IN_SRV                  900057
#define RI_WAIT_CL_DELIV                900059
#define RI_WAIT_CL_FIX                  900061
#define RI_WAIT_CL_POS                  900063
#define RI_WAIT_CL_REV_TRADE            900065
#define RI_WAIT_CL_TRADE                900067
#define RI_WAIT_RTR                     900069
#define RI_WAIT_OMDU                    900071
#define RI_ILLEGAL_NODE                -900072
#define RI_INVINT                      -900074
#define RI_NTNOMORE                    -900076
#define RI_NTINVTAB                    -900078
#define RI_NTDUPNAM                    -900080
#define RI_NTNOROOM                    -900082
#define RI_NTNOTFOUND                  -900084
#define RI_NTEXHASH                    -900086
#define RI_NTNOIX                      -900088
#define RI_POSFULL                     -900090
#define RI_POSNOMORE                   -900092
#define RI_POSNOPOINT                  -900094
#define RI_POSUFULL                    -900096
#define RI_TINOVAL                     -900098
#define RI_TIINVTIM                    -900100
#define RI_TIEXPTABFULL                -900102
#define RI_TIVALALEX                   -900103
#define RI_TIIMMTABFULL                -900104
#define RI_HLDINVTYP                   -900106
#define RI_HLDAGREQ                    -900108
#define RI_HLDAGALEX                   -900110
#define RI_HLDAGNOTVALID               -900112
#define RI_HLDNOMEMB                   -900113
#define RI_HLDNOMORE                   -900115
#define RI_HLDPOSALEX                  -900116
#define RI_HLDNOPOS                    -900117
#define RI_HLDNOMAR                    -900119
#define RI_HLDNOMEMMAR                 -900121
#define RI_HLDMARALEX                  -900123
#define RI_HLDNOPRI                    -900125
#define RI_HLDNOMEMPRI                 -900127
#define RI_HLDPRIALEX                  -900129
#define RI_HLDSVALEX                   -900131
#define RI_HLDNOSV                     -900133
#define RI_VALNOVAL                    -900134
#define RI_VALSWNOPER                  -900136
#define RI_REPFMTOFLOW                 -900138
#define RI_MRINVMARK                   -900140
#define RI_MRINVGROUP                  -900142
#define RI_MRINVPARAM                  -900144
#define RI_MRNOPARAM                   -900146
#define RI_MRNOCTRLDEF                 -900148
#define RI_MRNOCORCLA                  -900150
#define RI_MRNOCORSUP                  -900151
#define RI_PRINVMARK                   -900152
#define RI_PRINVGROUP                  -900154
#define RI_PRINVPARAM                  -900156
#define RI_PRNOSYNTHFUT                -900158
#define RI_PRIINVGROUP                 -900160
#define RI_CCTNONODE                   -900162
#define RI_CCTNOMORENOD                -900164
#define RI_CCTNOMOREUNI                -900166
#define RI_CCTTOOMANYPOS               -900168
#define RI_CHFULL                      -900170
#define RI_TDMARFULL                   -900172
#define RI_TDMARALEX                   -900174
#define RI_TDNTDFULL                   -900176
#define RI_TDNTDALEX                   -900178
#define RI_CCTILLEGALMIX               -900180
#define RI_TNMISMATCH                  -900182
#define RI_TNITEXC                     -900184
#define RI_TNILLEGALMIX                -900186
#define RI_TNNODATE                    -900188
#define RI_NOFIX                       -900190
#define RI_QRYUNDEFINED                 900191
#define RI_QRYUNKNOWN                   900193
#define RI_MARG_LIMIT_EXCEEDED         -900195
#define RI_PRICE_LIMIT_EXCEEDED        -900197
#define RI_INVALID_CONFIG_VALUE        -900198
#define RI_HLDFULL                     -900200
#define RI_MRFULL                      -900202
#define RI_MRCORRFULL                  -900204
#define RI_PRFULL                      -900206
#define RI_VALFULL                     -900208
#define RI_HLDNOAG                     -900210
#define RI_CCTFORBIDDENMIX             -900212
#define RI_NTSIZEDIFF                  -900214
#define RI_MRTIERFULL                  -900216
#define RI_MRIMFULL                    -900218
#define RI_MRICFULL                    -900220
#define RI_INVRISKEXP                  -900222
#define RI_INVWROPTMIN                 -900224
#define RI_PRINVSPR                    -900226
#define RI_INVLEG                      -900228
#define RI_PRTICKFULL                  -900230
#define RI_PRSPREADFULL                -900232
#define RI_INVRATIO                    -900234
#define RI_DHNOMOREPOSNOD              -900236
#define RI_DHNOMORERPFNOD              -900238
#define RI_NOFUT                       -900240
#define RI_RCPAR_SYSPALEX              -900242
#define RI_RCPAR_RUNALEX               -900244
#define RI_RCPAR_RUNFULL               -900246
#define RI_RCPAR_SCENALEX              -900248
#define RI_RCPAR_SCENDUPL              -900250
#define RI_RCPAR_NOSCEN                -900252
#define RI_INVDELIV                    -900254
#define RI_VIRTTRADETYPE               -900256
#define RI_NOACC                       -900258
#define RI_IDAYFAIL                    -900260
#define RI_NOCSB                       -900262
#define RI_CSBDUPMEM                   -900264
#define RI_CSBDUPACC                   -900266
#define RI_WAIT_CMS_POS                 900267
#define RI_PRAQRFULL                   -900268
#define RI_PRISNOTQUOTE                -900270
#define RI_TOOMANYLNAM                 -900272
#define RI_CCTDIFFCCY                  -900274
#define RI_BADPARAM                    -900276
#define RI_DB_ERROR                    -900278
#define RI_TOOFEWSURFSRS                900279
#define RI_NOSTRIKELIMIT                900281
#define RI_NRERROR                     -900283
#define RI_VOLASURFMISS                 900285
#define RI_TOOFEWSURFEXP                900287
#define RI_CALIFALLB_OK                 900289
#define RI_CALINOVAL                    900291
#define RI_CALI_NO_ATM                  900293
#define RI_NEG_VOLA                    -900294
#define RI_OMN_NORMAL                   910001
#define RI_OMN_BATNTRN                  910003
#define RI_OMN_NOINFO                   910005
#define RI_OMN_UKNWNQRY                -910006
#define RI_OMN_BADSIZE                 -910008
#define RI_OMN_SNDFAIL                 -910010
#define RI_OMN_BADSEG                  -910012
#define RI_OMN_BADEXCHANGE             -910014
#define RI_OMN_BADMARKET               -910016
#define RI_OMN_NOSUP                   -910018
#define RI_OMN_BADDATE                 -910020
#define RI_OMN_BADSIMVALUE             -910022
#define RI_OMN_BADPRICESIM             -910024
#define RI_OMN_BADVOLVALUE             -910026
#define RI_OMN_BADOLEVEL               -910028
#define RI_OMN_BADSEGMENT              -910030
#define RI_OMN_BADSERIES               -910032
#define RI_OMN_MRPARAM                 -910034
#define RI_OMN_PRPARAM                 -910036
#define RI_OMN_VALINT                  -910038
#define RI_OMN_MRCNTRL                 -910040
#define RI_OMN_GETMTRX                 -910042
#define RI_OMN_MRGMTRX                 -910044
#define RI_OMN_VALFAILED               -910046
#define RI_OMN_SIMDENY                 -910048
#define RI_OMN_SIMNEG                  -910050
#define RI_OMN_SIMNOMTCH               -910052
#define RI_OMN_BADTYPE                 -910054
#define RI_OMN_INVALIDDATES            -910056
#define RI_OMN_INVALIDFLAGS            -910058
#define RI_OMN_INVALIDTRANS            -910060
#define RI_OMN_NOFULLCOLLECT           -910062
#define RI_OMN_NOFUT                   -910064
#define RI_OMN_NOULGPRICE              -910066
#define RI_OMN_TNNODATE                -910068
#define RI_OMN_TNNOKNOWN               -910070
#define RI_OMN_TNNOPRICE               -910072
#define RI_OMN_SWAPNOPRICE             -910074
#define RI_OMN_VALINTPRICE             -910076
#define RI_OMN_NOMVALBOUGHT            -910078
#define RI_OMN_NOMVALSOLD              -910080
#define RI_OMN_NOSPAMRPFPRICE          -910082
#define RI_OMN_VFYERROR                -910084
#define RI_OMN_NOSTMTSTATUS            -910086
#define RI_OMN_NOEXCHANGERATE          -910088
#define RI_OMN_NOEXCHANGERATESRS       -910090
#define RI_OMN_BADPRODUCTAREA          -910092
#define RI_OMN_NOVERIFY                -910094
#define RI_OMN_OPTIONPRICE             -910096
#define RI_OMN_OPTPRICEHIGH            -910098
#define RI_OMN_VOLATILITY              -910100
#define RI_OMN_MSP                     -910102
#define RI_OMN_INTEGRITYFAIL           -910104
#define RI_OMN_NOMEMBER                -910106
#define RI_OMN_NOCLEARINGHOUSE         -910108
#define RI_OMN_NOPSPAMREFPRICE         -910110
#define RI_OMN_NOULGVOLA               -910112
#define RI_OMN_NOCORRMATRIX            -910114
#define RI_OMN_NOLTDPRICE              -910116
#define RI_OMN_NOCASHMBOUGHT           -910118
#define RI_OMN_NOCASHMSOLD             -910120
#define RI_OMN_FWDNOPRICE              -910122
#define RI_OMN_NOUPPERLEVELSRS         -910124
#define RI_OMN_NOSPAMULGPRICE          -910126
#define RI_OMN_ISSUEMARKEDSRS          -910128
#define RI_OMN_RCPARNOSCEN             -910130
#define RI_OMN_RCARWINONLY             -910131
#define RI_OMN_RCARNOSWAPS             -910133
#define RI_OMN_RCPARDIFFSCEN           -910134
#define RI_OMN_RCPARDIFFEXCSCEN        -910136
#define RI_OMN_BADINSDEL               -910138
#define RI_OMN_EXPSERIES               -910140
#define RI_OMN_ZEROQUANTITY            -910142
#define RI_OMN_BADCLOSINGDATE          -910144
#define RI_OMN_BADSTMTDATE             -910146
#define RI_OMN_NONZEROTRADENUMBER      -910148
#define RI_OMN_NOTRADE                 -910150
#define RI_OMN_TRADEDELETED            -910152
#define RI_OMN_RCARDIFFRUNS            -910154
#define RI_OMN_MISCOLPRICE             -910155
#define RI_OMN_CONFIRMEDPRICE          -910156
#define RI_OMN_DBERROR                 -910158
#define RI_OMN_NOOCQ                   -910160
#define RI_OMN_NOVOLA                  -910162
#define RI_OMN_NOASPOROSP              -910164
#define RI_OMN_BADPRICELISTCMD         -910166
#define RI_OMN_NOPRIVATELIST           -910168
#define RI_OMN_BADTRAVALUE             -910170
#define RI_OMN_BADEXPVALUE             -910172
#define RI_OMN_BADFUTVALUE             -910174
#define RI_OMN_BADALARMFLG             -910176
#define RI_OMN_NOFROZENPOS             -910177
#define RI_OMN_PRICESNOTREADY          -910179
#define RI_OMN_SIGNALREADY             -910180
#define RI_OMN_NOMCODEFINED            -910182
#define RI_OMN_NOATYDEFINED            -910183
#define RI_OMN_CALCNBRZERO             -910184
#define RI_OMN_CALLNBRZERO             -910186
#define RI_OMN_NOESPVWV                -910188
#define RI_OMN_NONONESPVWV             -910190
#define RI_OMN_INSIDBLANK              -910192
#define RI_OMN_INVSETTLEMENTPRICETYPE  -910194
#define RI_OMN_BADFIXEDMARGINTYPE      -910196
#define RI_OMN_INVALIDSRS              -910198
#define RI_OMN_NOOPTORFUT              -910200
#define RI_OMN_INVISSUEDATE            -910202
#define RI_OMN_SIMFAILED               -910204
#define RI_MDF_VERSION                  920001

/******************************************************************************
*end*of* ri_messages.h
*******************************************************************************/

#endif /* _RI_MESSAGES_H_ */


#ifndef _RX_MESSAGES_H_
#define _RX_MESSAGES_H_

/******************************************************************************
Module: rx_messages.h

Message definitions generated 2022-12-01 07:07:32 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define RX_PRETRADE_USER_BLOCKED       -850002
#define RX_PRETRADE_BREACH             -850004
#define RX_ACCOUNT_BLOCKED_MANUAL      -850006
#define RX_ACCOUNT_BLOCKED_DISCONNECT  -850008
#define RX_ACCOUNT_BREACH_MAX_SIZE     -850010
#define RX_ACCOUNT_RESTRICT_INSTRUMENT -850012
#define RX_ACCOUNT_BLOCKED_ORDER_RATE  -850014
#define RX_ACCOUNT_BREACH_OPEN_BUY     -850016
#define RX_ACCOUNT_BREACH_OPEN_SELL    -850018
#define RX_ACCOUNT_BREACH_TRADED_BUY   -850020
#define RX_ACCOUNT_BREACH_TRADED_SOLD  -850022
#define RX_ACCOUNT_BREACH_TRADED_NET   -850024
#define RX_ACCOUNT_BREACH_TOTAL_BUY    -850026
#define RX_ACCOUNT_BREACH_TOTAL_SELL   -850028
#define RX_ACCOUNT_BREACH_TOT_NET_BUY  -850030
#define RX_ACCOUNT_BREACH_TOT_NET_SELL -850032
#define RX_PTLG_NOT_FOUND              -850034
#define RX_PARTICIPANT_NOT_FOUND       -850036
#define RX_INSUFFICIENT_YC_PERMISSIONS -850038

/******************************************************************************
*end*of* rx_messages.h
*******************************************************************************/

#endif /* _RX_MESSAGES_H_ */


#ifndef _SU_MESSAGES_H_
#define _SU_MESSAGES_H_

/******************************************************************************
Module: su_messages.h

Message definitions generated 2022-12-01 07:07:29 by amt, version 3.19.7.0 - Built: 2017-12-30.

NOTE: Do not edit this file. It is automatically generated from
      one or more message files.
*******************************************************************************/

#define SU_NORMAL                       1100001
#define SU_TXALRDHDL                    1100003
#define SU_INVTRANSSIZE                -1100004
#define SU_INVTRANSTYPE                -1100006
#define SU_INV_BLOCK                   -1100007
#define SU_INV_QUANTITY                -1100009
#define SU_INV_PREMIUM                 -1100011
#define SU_INV_OPEN_CLOSE_REQ          -1100013
#define SU_INTERNALERROR               -1100015
#define SU_INVUSER                     -1100017
#define SU_INVTX                       -1100019
#define SU_INVSEGMENTNR                -1100021
#define SU_NOTOPEN                     -1100023
#define SU_FILEOPRFAILED               -1100025
#define SU_INSFMEM                     -1100026
#define SU_BUGCHECK                    -1100028
#define SU_ABORT                       -1100030
#define SU_MATCH_VALIDITY              -1100031
#define SU_PARAMETER_FILEOPRFAILED     -1100032
#define SU_NOT_ACTIVE_BIN              -1100033
#define SU_INVPARAMETER                -1100035
#define SU_INVINSTRCLASS               -1100037
#define SU_INVMM                       -1100039
#define SU_INVPMM                      -1100041
#define SU_NON_CUST_TRADE_LIM_ABS_LOW  -1100043
#define SU_NON_CUST_TRADE_LIM_FRAC_LOW -1100045
#define SU_NON_CUST_TRADE_LIM_FRAC_HIG -1100047
#define SU_FARMM_TRADE_LIM_FRAC_HIGH   -1100049
#define SU_FARMM_TRADE_LIM_FRAC_LOW    -1100051
#define SU_TICK_WORSE_VOLUME_LOW       -1100053
#define SU_DERIVED_ORDER_MAX_SIZE_HIGH -1100055
#define SU_INV_EXPIRATION_GROUP        -1100057
#define SU_INV_STRIKE_PRICE_GROUP      -1100059
#define SU_TIMENOTFUTURE               -1100061
#define SU_ITEMEXIST                   -1100063
#define SU_NOITEMEXIST                 -1100065
#define SU_SERIES_STOPPED_CRS          -1100067
#define SU_COMALRACT                   -1100069
#define SU_COMALRSUS                   -1100071
#define SU_INV_TRADING_STATE           -1100073
#define SU_NO_NEXT_STATE               -1100075
#define SU_INV_DESIGNATED_LIST         -1100077
#define SU_INV_CONCERN_VALUE           -1100079
#define SU_INV_CLASS_FAST_MARKET       -1100081
#define SU_INV_FAST_MARKET_LEVEL       -1100083
#define SU_INVUNDERLYING               -1100085
#define SU_EXPO_LIMIT_FILEOPRFAILED    -1100086
#define SU_CURTAIL_SPR_FACT_LOW        -1100087
#define SU_CURTAIL_SPR_FACT_HIG        -1100089
#define SU_SIGNAL_READY_NOT_ALLWD      -1100091
#define SU_INV_SESS_STATE_ORDER        -1100093
#define SU_EXTCIB_ALREADY_TRIGGERED    -1100095
#define SU_UNKNOWN_EXTCIB              -1100097
#define SU_INV_CODE                    -1100099
#define SU_INV_INFOLEVEL               -1100101
#define SU_INV_INFOTYPE                -1100103
#define SU_INV_PRIO                    -1100105
#define SU_MESSAGE_TOO_LONG            -1100107
#define SU_CORE_NOT_EXISTING           -1100109
#define SU_DEST_NOT_EXISTING           -1100111
#define SU_DATENOTVALID                -1100113
#define SU_TIMENOTVALID                -1100115
#define SU_IS_INV_LASTTRADINGSTATE     -1100117
#define SU_IS_INV_ORDERACTION          -1100119
#define SU_IS_INV_STATUS               -1100121
#define SU_IS_INV_STARTPROC            -1100123
#define SU_IS_INV_STATELEVEL           -1100125
#define SU_IS_SESSIONNOTFOUND          -1100127
#define SU_IS_NONEXTSTATE              -1100129
#define SU_IS_SERIESNOTFOUND           -1100131
#define SU_IS_INV_SESS_STATE_ORDER     -1100133
#define SU_IS_FILLEDNEXTSTATE          -1100135
#define SU_IS_BLANKNEXTSTATE           -1100137
#define SU_INV_TIME_ORDER              -1100139
#define SU_ILL_UPD_STATUS_NOTE         -1100141
#define SU_ILL_SPACE_IN_URL            -1100143
#define SU_ILL_CNTRL_IN_URL            -1100145
#define SU_ILL_MARKET                  -1100147
#define SU_ILL_UNDERLYING              -1100149
#define SU_ILL_SERIES                  -1100151
#define SU_IS_COMBO                    -1100153
#define SU_IS_NOINS                    -1100155
#define SU_IS_TOOFEWITEMS              -1100157
#define SU_IS_TOOMANYITEMS             -1100159
#define SU_UU_USERPEND                 -1100161
#define SU_UU_TIMENOTFUT               -1100163
#define SU_IS_INV_NO_OF_WARNINGS       -1100165
#define SU_INV_URGENT                  -1100167
#define SU_TS_INV_SESS_STATE_ORDER     -1100169
#define SU_TS_INV_TSE_NAME             -1100171
#define SU_INV_TRADING_SESSION         -1100173
#define SU_INV_AUCTION_TYPE            -1100175
#define SU_INV_BOOK_TRANSPARANCY       -1100177
#define SU_IS_NOSESS                   -1100179
#define SU_NOT_ISSUER                  -1100181
#define SU_INV_AUCTION_ENDTIME         -1100183
#define SU_INV_ALLOTMENT               -1100185
#define SU_AUCTION_NOTFOUND            -1100187
#define SU_AUCTION_ALR_ACTIVE          -1100189
#define SU_NOT_ENOUGH_TIME_FOR_AUCTION -1100191
#define SU_MDF_VERSION                  1105001

/******************************************************************************
*end*of* su_messages.h
*******************************************************************************/

#endif /* _SU_MESSAGES_H_ */

#endif
