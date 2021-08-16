package tpc.spark.fletcher.app.utils

import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.types._
import org.apache.spark.sql.types.{StructType, StructField, StringType, IntegerType};
import org.apache.spark.sql.DataFrame

class Dataset(val spark: SparkSession, val dataset_name: String) {
  import spark.implicits._
  private val file_prefix = sys.env("DATASET_DIR") + "/" // + "/dataset/"
  //private val file_prefix ="/home/yyunon/Desktop/repositories/thesis_journals/resources/spark-tpc-ds-performance-test/src/data/"
  //
  //The following is just a quick fix for tpch query.
  private val lineitem_schema = StructType(
    Seq(
      StructField("l_orderkey", IntegerType, false),
      StructField("l_partkey", IntegerType, false),
      StructField("l_suppkey", IntegerType, false),
      StructField("l_linenumber", IntegerType, false),
      StructField("l_quantity", DoubleType, false),
      StructField("l_extendedprice", DoubleType, false),
      StructField("l_discount", DoubleType, false),
      StructField("l_tax", DoubleType, false),
      StructField("l_returnflag", StringType, false),
      StructField("l_linestatus", StringType, false),
      StructField("l_shipdate", DateType, false),
      StructField("l_commitdate", DateType, false),
      StructField("l_receiptdate", DateType, false),
      StructField("l_shipinstruct", StringType, false),
      StructField("l_shipmode", StringType, false),
      StructField("l_comment", StringType, false)
    ))
  private val part_schema = StructType(
    Seq(
      StructField("p_partkey", IntegerType, true),
      StructField("p_name", StringType, true),
      StructField("p_mfgr", StringType, true),
      StructField("p_brand", StringType, true),
      StructField("p_type", StringType, true),
      StructField("p_size", IntegerType, true),
      StructField("p_container", StringType, true),
      StructField("p_retailprice", DoubleType, true),
      StructField("p_comment", StringType, true)
    ))
//
  private val call_center_schema = StructType(
    Seq(
      StructField("cc_call_center_sk", IntegerType, true),
      StructField("cc_call_center_id", StringType, true),
      StructField("cc_rec_start_date", StringType, true),
      StructField("cc_rec_end_date", StringType, true),
      StructField("cc_closed_date_sk", IntegerType, true),
      StructField("cc_open_date_sk", IntegerType, true),
      StructField("cc_name", StringType, true),
      StructField("cc_class", StringType, true),
      StructField("cc_employees", IntegerType, true),
      StructField("cc_sq_ft", IntegerType, true),
      StructField("cc_hours", StringType, true),
      StructField("cc_manager", StringType, true),
      StructField("cc_mkt_id", IntegerType, true),
      StructField("cc_mkt_class", StringType, true),
      StructField("cc_mkt_desc", StringType, true),
      StructField("cc_market_manager", StringType, true),
      StructField("cc_division", IntegerType, true),
      StructField("cc_division_name", StringType, true),
      StructField("cc_company", IntegerType, true),
      StructField("cc_company_name", StringType, true),
      StructField("cc_street_number", StringType, true),
      StructField("cc_street_name", StringType, true),
      StructField("cc_street_type", StringType, true),
      StructField("cc_suite_number", StringType, true),
      StructField("cc_city", StringType, true),
      StructField("cc_county", StringType, true),
      StructField("cc_state", StringType, true),
      StructField("cc_zip", StringType, true),
      StructField("cc_country", StringType, true),
      StructField("cc_gmt_offset", DoubleType, true),
      StructField("cc_tax_percentage", DoubleType, true)
    ))

  private val catalog_page_schema = StructType(
    Seq(
      StructField("cp_catalog_page_sk", IntegerType, true),
      StructField("cp_catalog_page_id", StringType, true),
      StructField("cp_start_date_sk", IntegerType, true),
      StructField("cp_end_date_sk", IntegerType, true),
      StructField("cp_department", StringType, true),
      StructField("cp_catalog_number", IntegerType, true),
      StructField("cp_catalog_page_number", IntegerType, true),
      StructField("cp_description", StringType, true),
      StructField("cp_type", StringType, true)
    ))

  private val catalog_returns_schema = StructType(
    Seq(
      StructField("cr_returned_date_sk", IntegerType, true),
      StructField("cr_returned_time_sk", IntegerType, true),
      StructField("cr_item_sk", IntegerType, true),
      StructField("cr_refunded_customer_sk", IntegerType, true),
      StructField("cr_refunded_cdemo_sk", IntegerType, true),
      StructField("cr_refunded_hdemo_sk", IntegerType, true),
      StructField("cr_refunded_addr_sk", IntegerType, true),
      StructField("cr_returning_customer_sk", IntegerType, true),
      StructField("cr_returning_cdemo_sk", IntegerType, true),
      StructField("cr_returning_hdemo_sk", IntegerType, true),
      StructField("cr_returning_addr_sk", IntegerType, true),
      StructField("cr_call_center_sk", IntegerType, true),
      StructField("cr_catalog_page_sk", IntegerType, true),
      StructField("cr_ship_mode_sk", IntegerType, true),
      StructField("cr_warehouse_sk", IntegerType, true),
      StructField("cr_reason_sk", IntegerType, true),
      StructField("cr_order_number", IntegerType, true),
      StructField("cr_return_quantity", IntegerType, true),
      StructField("cr_return_amount", DoubleType, true),
      StructField("cr_return_tax", DoubleType, true),
      StructField("cr_return_amt_inc_tax", DoubleType, true),
      StructField("cr_fee", DoubleType, true),
      StructField("cr_return_ship_cost", DoubleType, true),
      StructField("cr_refunded_cash", DoubleType, true),
      StructField("cr_reversed_charge", DoubleType, true),
      StructField("cr_store_credit", DoubleType, true),
      StructField("cr_net_loss", DoubleType, true)
    ))
  private val catalog_sales_schema = StructType(
    Seq(
      StructField("cs_sold_date_sk", IntegerType, true),
      StructField("cs_sold_time_sk", IntegerType, true),
      StructField("cs_ship_date_sk", IntegerType, true),
      StructField("cs_bill_customer_sk", IntegerType, true),
      StructField("cs_bill_cdemo_sk", IntegerType, true),
      StructField("cs_bill_hdemo_sk", IntegerType, true),
      StructField("cs_bill_addr_sk", IntegerType, true),
      StructField("cs_ship_customer_sk", IntegerType, true),
      StructField("cs_ship_cdemo_sk", IntegerType, true),
      StructField("cs_ship_hdemo_sk", IntegerType, true),
      StructField("cs_ship_addr_sk", IntegerType, true),
      StructField("cs_call_center_sk", IntegerType, true),
      StructField("cs_catalog_page_sk", IntegerType, true),
      StructField("cs_ship_mode_sk", IntegerType, true),
      StructField("cs_warehouse_sk", IntegerType, true),
      StructField("cs_item_sk", IntegerType, true),
      StructField("cs_promo_sk", IntegerType, true),
      StructField("cs_order_number", IntegerType, true),
      StructField("cs_quantity", IntegerType, true),
      StructField("cs_wholesale_cost", DoubleType, true),
      StructField("cs_list_price", DoubleType, true),
      StructField("cs_sales_price", DoubleType, true),
      StructField("cs_ext_discount_amt", DoubleType, true),
      StructField("cs_ext_sales_price", DoubleType, true),
      StructField("cs_ext_wholesale_cost", DoubleType, true),
      StructField("cs_ext_list_price", DoubleType, true),
      StructField("cs_ext_tax", DoubleType, true),
      StructField("cs_coupon_amt", DoubleType, true),
      StructField("cs_ext_ship_cost", DoubleType, true),
      StructField("cs_net_paid", DoubleType, true),
      StructField("cs_net_paid_inc_tax", DoubleType, true),
      StructField("cs_net_paid_inc_ship", DoubleType, true),
      StructField("cs_net_paid_inc_ship_tax", DoubleType, true),
      StructField("cs_net_profit", DoubleType, true)
    ))
  private val customer_schema = StructType(
    Seq(
      StructField("c_customer_sk", IntegerType, true),
      StructField("c_customer_id", StringType, true),
      StructField("c_current_cdemo_sk", IntegerType, true),
      StructField("c_current_hdemo_sk", IntegerType, true),
      StructField("c_current_addr_sk", IntegerType, true),
      StructField("c_first_shipto_date_sk", IntegerType, true),
      StructField("c_first_sales_date_sk", IntegerType, true),
      StructField("c_salutation", StringType, true),
      StructField("c_first_name", StringType, true),
      StructField("c_last_name", StringType, true),
      StructField("c_preferred_cust_flag", StringType, true),
      StructField("c_birth_day", IntegerType, true),
      StructField("c_birth_month", IntegerType, true),
      StructField("c_birth_year", IntegerType, true),
      StructField("c_birth_country", StringType, true),
      StructField("c_login", StringType, true),
      StructField("c_email_address", StringType, true),
      StructField("c_last_review_date", StringType, true)
    ))

  private val customer_address_schema = StructType(
    Seq(
      StructField("ca_address_sk", IntegerType, true),
      StructField("ca_address_id", StringType, true),
      StructField("ca_street_number", StringType, true),
      StructField("ca_street_name", StringType, true),
      StructField("ca_street_type", StringType, true),
      StructField("ca_suite_number", StringType, true),
      StructField("ca_city", StringType, true),
      StructField("ca_county", StringType, true),
      StructField("ca_state", StringType, true),
      StructField("ca_zip", StringType, true),
      StructField("ca_country", StringType, true),
      StructField("ca_gmt_offset", DoubleType, true),
      StructField("ca_location_type", StringType, true)
    ))
  private val customer_demographics_schema = StructType(
    Seq(
      StructField("cd_demo_sk", IntegerType, true),
      StructField("cd_gender", StringType, true),
      StructField("cd_marital_status", StringType, true),
      StructField("cd_education_status", StringType, true),
      StructField("cd_purchase_estimate", IntegerType, true),
      StructField("cd_credit_rating", StringType, true),
      StructField("cd_dep_count", IntegerType, true),
      StructField("cd_dep_employed_count", IntegerType, true),
      StructField("cd_dep_college_count", IntegerType, true)
    ))
  private val household_demographics_schema = StructType(
    Seq(
      StructField("hd_demo_sk", IntegerType, true),
      StructField("hd_income_band_sk", IntegerType, true),
      StructField("hd_buy_potential", StringType, true),
      StructField("hd_dep_count", IntegerType, true),
      StructField("hd_vehicle_count", IntegerType, true)
    ))
  private val income_band_schema = StructType(
    Seq(
      StructField("ib_income_band_sk", IntegerType, true),
      StructField("ib_lower_bound", IntegerType, true),
      StructField("ib_upper_bound", IntegerType, true)
    ))
  private val inventory_schema = StructType(
    Seq(
      StructField("inv_date_sk", IntegerType, true),
      StructField("inv_item_sk", IntegerType, true),
      StructField("inv_warehouse_sk", IntegerType, true),
      StructField("inv_quantity_on_hand", LongType, true)
    ))
  private val promotion_schema = StructType(
    Seq(
      StructField("p_promo_sk", IntegerType, true),
      StructField("p_promo_id", StringType, true),
      StructField("p_start_date_sk", IntegerType, true),
      StructField("p_end_date_sk", IntegerType, true),
      StructField("p_item_sk", IntegerType, true),
      StructField("p_cost", DoubleType, true),
      StructField("p_response_target", IntegerType, true),
      StructField("p_promo_name", StringType, true),
      StructField("p_channel_dmail", StringType, true),
      StructField("p_channel_email", StringType, true),
      StructField("p_channel_catalog", StringType, true),
      StructField("p_channel_tv", StringType, true),
      StructField("p_channel_radio", StringType, true),
      StructField("p_channel_press", StringType, true),
      StructField("p_channel_event", StringType, true),
      StructField("p_channel_demo", StringType, true),
      StructField("p_channel_details", StringType, true),
      StructField("p_purpose", StringType, true),
      StructField("p_discount_active", StringType, true)
    ))
  private val reason_schema = StructType(
    Seq(
      StructField("r_reason_sk", IntegerType, true),
      StructField("r_reason_id", StringType, true),
      StructField("r_reason_desc", StringType, true)
    ))
  private val ship_mode_schema = StructType(
    Seq(
      StructField("sm_ship_mode_sk", IntegerType, true),
      StructField("sm_ship_mode_id", StringType, true),
      StructField("sm_type", StringType, true),
      StructField("sm_code", StringType, true),
      StructField("sm_carrier", StringType, true),
      StructField("sm_contract", StringType, true)
    ))
  private val store_schema = StructType(
    Seq(
      StructField("s_store_sk", IntegerType, true),
      StructField("s_store_id", StringType, true),
      StructField("s_rec_start_date", StringType, true),
      StructField("s_rec_end_date", StringType, true),
      StructField("s_closed_date_sk", IntegerType, true),
      StructField("s_store_name", StringType, true),
      StructField("s_number_employees", IntegerType, true),
      StructField("s_floor_space", IntegerType, true),
      StructField("s_hours", StringType, true),
      StructField("s_manager", StringType, true),
      StructField("s_market_id", IntegerType, true),
      StructField("s_geography_class", StringType, true),
      StructField("s_market_desc", StringType, true),
      StructField("s_market_manager", StringType, true),
      StructField("s_division_id", IntegerType, true),
      StructField("s_division_name", StringType, true),
      StructField("s_company_id", IntegerType, true),
      StructField("s_company_name", StringType, true),
      StructField("s_street_number", StringType, true),
      StructField("s_street_name", StringType, true),
      StructField("s_street_type", StringType, true),
      StructField("s_suite_number", StringType, true),
      StructField("s_city", StringType, true),
      StructField("s_county", StringType, true),
      StructField("s_state", StringType, true),
      StructField("s_zip", StringType, true),
      StructField("s_country", StringType, true),
      StructField("s_gmt_offset", DoubleType, true),
      StructField("s_tax_precentage", DoubleType, true)
    ))
  private val store_returns_schema = StructType(
    Seq(
      StructField("sr_returned_date_sk", IntegerType, true),
      StructField("sr_return_time_sk", IntegerType, true),
      StructField("sr_item_sk", IntegerType, true),
      StructField("sr_customer_sk", IntegerType, true),
      StructField("sr_cdemo_sk", IntegerType, true),
      StructField("sr_hdemo_sk", IntegerType, true),
      StructField("sr_addr_sk", IntegerType, true),
      StructField("sr_store_sk", IntegerType, true),
      StructField("sr_reason_sk", IntegerType, true),
      StructField("sr_ticket_number", IntegerType, true),
      StructField("sr_return_quantity", IntegerType, true),
      StructField("sr_return_amt", DoubleType, true),
      StructField("sr_return_tax", DoubleType, true),
      StructField("sr_return_amt_inc_tax", DoubleType, true),
      StructField("sr_fee", DoubleType, true),
      StructField("sr_return_ship_cost", DoubleType, true),
      StructField("sr_refunded_cash", DoubleType, true),
      StructField("sr_reversed_charge", DoubleType, true),
      StructField("sr_store_credit", DoubleType, true),
      StructField("sr_net_loss", DoubleType, true)
    ))
  private val time_dim_schema = StructType(
    Seq(
      StructField("t_time_sk", IntegerType, true),
      StructField("t_time_id", StringType, true),
      StructField("t_time", IntegerType, true),
      StructField("t_hour", IntegerType, true),
      StructField("t_minute", IntegerType, true),
      StructField("t_second", IntegerType, true),
      StructField("t_am_pm", StringType, true),
      StructField("t_shift", StringType, true),
      StructField("t_sub_shift", StringType, true),
      StructField("t_meal_time", StringType, true)
    ))
  private val warehouse_schema = StructType(
    Seq(
      StructField("w_warehouse_sk", IntegerType, true),
      StructField("w_warehouse_id", StringType, true),
      StructField("w_warehouse_name", StringType, true),
      StructField("w_warehouse_sq_ft", IntegerType, true),
      StructField("w_street_number", StringType, true),
      StructField("w_street_name", StringType, true),
      StructField("w_street_type", StringType, true),
      StructField("w_suite_number", StringType, true),
      StructField("w_city", StringType, true),
      StructField("w_county", StringType, true),
      StructField("w_state", StringType, true),
      StructField("w_zip", StringType, true),
      StructField("w_country", StringType, true),
      StructField("w_gmt_offset", DoubleType, true)
    ))
  private val web_page_schema = StructType(
    Seq(
      StructField("wp_web_page_sk", IntegerType, true),
      StructField("wp_web_page_id", StringType, true),
      StructField("wp_rec_start_date", StringType, true),
      StructField("wp_rec_end_date", StringType, true),
      StructField("wp_creation_date_sk", IntegerType, true),
      StructField("wp_access_date_sk", IntegerType, true),
      StructField("wp_autogen_flag", StringType, true),
      StructField("wp_customer_sk", IntegerType, true),
      StructField("wp_url", StringType, true),
      StructField("wp_type", StringType, true),
      StructField("wp_char_count", IntegerType, true),
      StructField("wp_link_count", IntegerType, true),
      StructField("wp_image_count", IntegerType, true),
      StructField("wp_max_ad_count", IntegerType, true)
    ))
  private val web_returns_schema = StructType(
    Seq(
      StructField("wr_returned_date_sk", IntegerType, true),
      StructField("wr_returned_time_sk", IntegerType, true),
      StructField("wr_item_sk", IntegerType, true),
      StructField("wr_refunded_customer_sk", IntegerType, true),
      StructField("wr_refunded_cdemo_sk", IntegerType, true),
      StructField("wr_refunded_hdemo_sk", IntegerType, true),
      StructField("wr_refunded_addr_sk", IntegerType, true),
      StructField("wr_returning_customer_sk", IntegerType, true),
      StructField("wr_returning_cdemo_sk", IntegerType, true),
      StructField("wr_returning_hdemo_sk", IntegerType, true),
      StructField("wr_returning_addr_sk", IntegerType, true),
      StructField("wr_web_page_sk", IntegerType, true),
      StructField("wr_reason_sk", IntegerType, true),
      StructField("wr_order_number", IntegerType, true),
      StructField("wr_return_quantity", IntegerType, true),
      StructField("wr_return_amt", DoubleType, true),
      StructField("wr_return_tax", DoubleType, true),
      StructField("wr_return_amt_inc_tax", DoubleType, true),
      StructField("wr_fee", DoubleType, true),
      StructField("wr_return_ship_cost", DoubleType, true),
      StructField("wr_refunded_cash", DoubleType, true),
      StructField("wr_reversed_charge", DoubleType, true),
      StructField("wr_account_credit", DoubleType, true),
      StructField("wr_net_loss", DoubleType, true)
    ))
  private val web_sales_schema = StructType(
    Seq(
      StructField("ws_sold_date_sk", IntegerType, true),
      StructField("ws_sold_time_sk", IntegerType, true),
      StructField("ws_ship_date_sk", IntegerType, true),
      StructField("ws_item_sk", IntegerType, true),
      StructField("ws_bill_customer_sk", IntegerType, true),
      StructField("ws_bill_cdemo_sk", IntegerType, true),
      StructField("ws_bill_hdemo_sk", IntegerType, true),
      StructField("ws_bill_addr_sk", IntegerType, true),
      StructField("ws_ship_customer_sk", IntegerType, true),
      StructField("ws_ship_cdemo_sk", IntegerType, true),
      StructField("ws_ship_hdemo_sk", IntegerType, true),
      StructField("ws_ship_addr_sk", IntegerType, true),
      StructField("ws_web_page_sk", IntegerType, true),
      StructField("ws_web_site_sk", IntegerType, true),
      StructField("ws_ship_mode_sk", IntegerType, true),
      StructField("ws_warehouse_sk", IntegerType, true),
      StructField("ws_promo_sk", IntegerType, true),
      StructField("ws_order_number", IntegerType, true),
      StructField("ws_quantity", IntegerType, true),
      StructField("ws_wholesale_cost", DoubleType, true),
      StructField("ws_list_price", DoubleType, true),
      StructField("ws_sales_price", DoubleType, true),
      StructField("ws_ext_discount_amt", DoubleType, true),
      StructField("ws_ext_sales_price", DoubleType, true),
      StructField("ws_ext_wholesale_cost", DoubleType, true),
      StructField("ws_ext_list_price", DoubleType, true),
      StructField("ws_ext_tax", DoubleType, true),
      StructField("ws_coupon_amt", DoubleType, true),
      StructField("ws_ext_ship_cost", DoubleType, true),
      StructField("ws_net_paid", DoubleType, true),
      StructField("ws_net_paid_inc_tax", DoubleType, true),
      StructField("ws_net_paid_inc_ship", DoubleType, true),
      StructField("ws_net_paid_inc_ship_tax", DoubleType, true),
      StructField("ws_net_profit", DoubleType, true)
    ))
  private val web_site_schema = StructType(
    Seq(
      StructField("web_site_sk", IntegerType, true),
      StructField("web_site_id", StringType, true),
      StructField("web_rec_start_date", StringType, true),
      StructField("web_rec_end_date", StringType, true),
      StructField("web_name", StringType, true),
      StructField("web_open_date_sk", IntegerType, true),
      StructField("web_close_date_sk", IntegerType, true),
      StructField("web_class", StringType, true),
      StructField("web_manager", StringType, true),
      StructField("web_mkt_id", IntegerType, true),
      StructField("web_mkt_class", StringType, true),
      StructField("web_mkt_desc", StringType, true),
      StructField("web_market_manager", StringType, true),
      StructField("web_company_id", IntegerType, true),
      StructField("web_company_name", StringType, true),
      StructField("web_street_number", StringType, true),
      StructField("web_street_name", StringType, true),
      StructField("web_street_type", StringType, true),
      StructField("web_suite_number", StringType, true),
      StructField("web_city", StringType, true),
      StructField("web_county", StringType, true),
      StructField("web_state", StringType, true),
      StructField("web_zip", StringType, true),
      StructField("web_country", StringType, true),
      StructField("web_gmt_offset", DoubleType, true),
      StructField("web_tax_percentage", DoubleType, true)
    ))

  private val date_dim_schema = StructType(
    Seq(
      StructField("d_date_sk", IntegerType, true),
      StructField("d_date_id", StringType, true),
      StructField("d_date", StringType, true),
      StructField("d_month_seq", IntegerType, true),
      StructField("d_week_seq", IntegerType, true),
      StructField("d_quarter_seq", IntegerType, true),
      StructField("d_year", IntegerType, true),
      StructField("d_dow", IntegerType, true),
      StructField("d_moy", IntegerType, true),
      StructField("d_dom", IntegerType, true),
      StructField("d_qoy", IntegerType, true),
      StructField("d_fy_year", IntegerType, true),
      StructField("d_fy_quarter_seq", IntegerType, true),
      StructField("d_fy_week_seq", IntegerType, true),
      StructField("d_day_name", StringType, true),
      StructField("d_quarter_name", StringType, true),
      StructField("d_holiday", StringType, true),
      StructField("d_weekend", StringType, true),
      StructField("d_following_holiday", StringType, true),
      StructField("d_first_dom", IntegerType, true),
      StructField("d_last_dom", IntegerType, true),
      StructField("d_same_day_ly", IntegerType, true),
      StructField("d_same_day_lq", IntegerType, true),
      StructField("d_current_day", StringType, true),
      StructField("d_current_week", StringType, true),
      StructField("d_current_month", StringType, true),
      StructField("d_current_quarter", StringType, true),
      StructField("d_current_year", StringType, true)
    ))
  private val store_sales_schema = StructType(
    Seq(
      StructField("ss_sold_date_sk", IntegerType, true),
      StructField("ss_sold_time_sk", IntegerType, true),
      StructField("ss_item_sk", IntegerType, true),
      StructField("ss_customer_sk", IntegerType, true),
      StructField("ss_cdemo_sk", IntegerType, true),
      StructField("ss_hdemo_sk", IntegerType, true),
      StructField("ss_addr_sk", IntegerType, true),
      StructField("ss_store_sk", IntegerType, true),
      StructField("ss_promo_sk", IntegerType, true),
      StructField("ss_ticket_number", IntegerType, true),
      StructField("ss_quantity", IntegerType, true),
      StructField("ss_wholesale_cost", DoubleType, true),
      StructField("ss_list_price", DoubleType, true),
      StructField("ss_sales_price", DoubleType, true),
      StructField("ss_ext_discount_amt", DoubleType, true),
      StructField("ss_ext_sales_price", DoubleType, true),
      StructField("ss_ext_wholesale_cost", DoubleType, true),
      StructField("ss_ext_list_price", DoubleType, true),
      StructField("ss_ext_tax", DoubleType, true),
      StructField("ss_coupon_amt", DoubleType, true),
      StructField("ss_net_paid", DoubleType, true),
      StructField("ss_net_paid_inc_tax", DoubleType, true),
      StructField("ss_net_profit", DoubleType, true)
    ))
  private val item_schema = StructType(
    Seq(
      StructField("i_item_sk", IntegerType, true),
      StructField("i_item_id", StringType, true),
      StructField("i_rec_start_date", StringType, true),
      StructField("i_rec_end_date", StringType, true),
      StructField("i_item_desc", StringType, true),
      StructField("i_current_price", DoubleType, true),
      StructField("i_wholesale_cost", DoubleType, true),
      StructField("i_brand_id", IntegerType, true),
      StructField("i_brand", StringType, true),
      StructField("i_class_id", IntegerType, true),
      StructField("i_class", StringType, true),
      StructField("i_category_id", IntegerType, true),
      StructField("i_category", StringType, true),
      StructField("i_manufact_id", IntegerType, true),
      StructField("i_manufact", StringType, true),
      StructField("i_size", StringType, true),
      StructField("i_formulation", StringType, true),
      StructField("i_color", StringType, true),
      StructField("i_units", StringType, true),
      StructField("i_container", StringType, true),
      StructField("i_manager_id", IntegerType, true),
      StructField("i_product_name", StringType, true)
    ))

  var schema: StructType = _

  def load_dataset(partitions: Int, num_rows: Int) = {
    dataset_name match {
      case "call_center" =>
        schema = call_center_schema
      case "catalog_page" =>
        schema = catalog_page_schema
      case "catalog_returns" =>
        schema = catalog_returns_schema
      case "catalog_sales" =>
        schema = catalog_sales_schema
      case "customer" =>
        schema = customer_schema
      case "customer_address" =>
        schema = customer_address_schema
      case "customer_demographics" =>
        schema = customer_demographics_schema
      case "date_dim" =>
        schema = date_dim_schema
      case "household_demographics" =>
        schema = household_demographics_schema
      case "income_band" =>
        schema = income_band_schema
      case "inventory" =>
        schema = inventory_schema
      case "item" =>
        schema = item_schema
      case "promotion" =>
        schema = promotion_schema
      case "reason" =>
        schema = reason_schema
      case "ship_mode" =>
        schema = ship_mode_schema
      case "store" =>
        schema = store_schema
      case "store_returns" =>
        schema = store_returns_schema
      case "store_sales" =>
        schema = store_sales_schema
      case "time_dim" =>
        schema = time_dim_schema
      case "warehouse" =>
        schema = warehouse_schema
      case "web_page" =>
        schema = web_page_schema
      case "web_returns" =>
        schema = web_returns_schema
      case "web_sales" =>
        schema = web_sales_schema
      case "web_site" =>
        schema = web_site_schema
      //TODO: Seperate TPCH queries later
      case "lineitem" =>
        schema = lineitem_schema
      case "part" =>
        schema = part_schema
    }

    val dataset = spark.read
      .format("csv")
      .option("sep", "|")
      .schema(schema)
      .load(file_prefix + dataset_name + ".tbl")
      .limit(num_rows)
      .repartition(partitions)
    //dataset.prIntegerTypeSchema()
    //dataset.show()

    dataset
  }

}
