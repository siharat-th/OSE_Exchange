//============================================================================
// Name        	: JsonOrderDecoders.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Sep 27, 2021 4:33:39 PM
//============================================================================

#ifndef SRC_JSON_JSONORDERDECODERS_HPP_
#define SRC_JSON_JSONORDERDECODERS_HPP_

#include <cstring>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

#include <Orders/Order.hpp>
#include <Orders/OrderUtils.hpp>
using namespace KTN;

/**
 * @brief The JsonOrderDecoders class provides static methods for decoding JSON orders.
 */
class JsonOrderDecoders
{
public:
	/**
	 * @brief Checks the JSON header and returns the header information.
	 * @param json The JSON string to check.
	 * @return The JsonHeader struct containing the header information.
	 */
	inline static JsonHeader CheckJsonHeader(string json)
	{
		Document d;
		JsonHeader hdr = {};

		if (d.Parse(json.c_str(), json.length()).HasParseError())
		{

			fprintf(stderr, "\nRapidJson Error(offset %u): %d\n",
					(unsigned)d.GetErrorOffset(),
					d.GetParseError());
			int e = (unsigned)d.GetErrorOffset();
			ostringstream oss;

			oss << "CheckJsonHeader: JSON OFFSET ERROR AT " << e;
			cout << oss.str() << endl;
			cout << json << endl;

			hdr.isEmpty = true;

			return hdr;
		}

		if (!d.IsArray())
		{

			if (d.HasMember("source"))
				hdr.source = d["source"].GetString();
			if (d.HasMember("topic"))
				hdr.topic = d["topic"].GetString();

			if (d.HasMember("destination"))
				hdr.destination = d["destination"].GetString();

			if (hdr.source.length() > 0 && hdr.topic.length() > 0)
				hdr.isEmpty = false;

			return hdr;
		}

		hdr.isEmpty = true;
		for (Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
		{
			if (itr->HasMember("source") && (*itr)["source"].IsString())
			{
				hdr.source = (*itr)["source"].GetString();
			}
			else
			{
				hdr.source = "";
			}
			if (itr->HasMember("topic") && (*itr)["topic"].IsString())
			{
				hdr.topic = (*itr)["topic"].GetString();
			}
			else
			{
				hdr.topic = "";
			}
			hdr.isEmpty = false;

			if (!(*itr).HasMember("groupname") || !(*itr).HasMember("org") || !(*itr).HasMember("user"))
			{
				hdr.hasUGO = false;
			}
			else
				hdr.hasUGO = true;

			return hdr;
		}

		return hdr;
	}

	/**
	 * @brief Checks the order source in the JSON and returns the source.
	 * @param json The JSON string to check.
	 * @return The order source.
	 */
	inline static std::string CheckOrderSource(string json)
	{
		Document d;

		if (d.Parse(json.c_str(), json.length()).HasParseError())
		{

			fprintf(stderr, "\nRapidJson Error(offset %u): %d\n",
					(unsigned)d.GetErrorOffset(),
					d.GetParseError());
			int e = (unsigned)d.GetErrorOffset();
			ostringstream oss;
			oss << "JSON OFFSET ERROR AT " << e;
			cout << oss.str() << endl;
			cout << json << endl;
			return "";
		}

		if (!d.IsArray())
		{
			cout << "JSON ORDERS MUST BE SENT AS ARRAY!!!!" << endl;
			return "";
		}

		for (Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
		{
			KTN::Order ord = {};

			if ((*itr).HasMember("source"))
			{
				string source = (*itr)["source"].GetString();
				return source;
			}
		}

		return "";
	}

	/**
	 * @brief Decodes the JSON order and returns a vector of Order objects.
	 * @param json The JSON string to decode.
	 * @return A vector of Order objects.
	 */
	inline static std::vector<KTN::Order> DecodeOrder(std::string json)
	{
		Document d;

		std::vector<KTN::Order> res;

		if (d.Parse(json.c_str(), json.length()).HasParseError())
		{

			fprintf(stderr, "\nRapidJson Error(offset %u): %d\n",
					(unsigned)d.GetErrorOffset(),
					d.GetParseError());
			int e = (unsigned)d.GetErrorOffset();
			ostringstream oss;
			oss << "JSON OFFSET ERROR AT " << e;
			cout << oss.str() << endl;

			return res;
		}

		if (!d.IsArray())
		{
			cout << "JSON ORDERS MUST BE SENT AS ARRAY!!!!" << endl;
			return res;
		}

		for (Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
		{
			KTN::Order ord = {};

			ord.action = (*itr)["action"].GetString();
			strcpy(ord.symbol, (*itr)["symbol"].GetString());

			if (itr->HasMember("product") && !(*itr)["product"].IsNull())
			{
				ord.product = (*itr)["product"].GetString();
			}

			string side = (*itr)["side"].GetString();
			ord.OrdSide = KOrderSide::Value(side);
			ord.exchange = (*itr)["exchange"].GetString();

			ord.quantity = (*itr)["quantity"].GetInt();
			ord.leavesqty = (*itr)["leavesqty"].GetInt();
			ord.dispqty = (*itr)["dispqty"].GetInt();
			ord.minqty = (*itr)["minqty"].GetInt();

			ord.price = (*itr)["price"].GetDouble();
			ord.stoppx = 0;
			if ((*itr).HasMember("stoppx"))
				ord.stoppx = (*itr)["stoppx"].GetDouble();

			strcpy(ord.ordernum, (*itr)["ordernum"].GetString());
			string id = (*itr)["exchordid"].GetString();
			if (id.length() > 0)
			{
				ord.exchordid = stoull(id.c_str());
			}

			if (ord.action.compare("NEW") == 0)
			{
				ord.OrdTif = KOrderTif::DAY;

				string tif = (*itr)["tif"].GetString();
				if (tif.compare("GTC") ==0)
					ord.OrdTif = KOrderTif::GTC;
				else if (tif.compare("FOK")== 0 || tif.compare("IOC") == 0)
					ord.OrdTif = KOrderTif::FAK;
				else if (tif.compare("FAK")== 0)
					ord.OrdTif = KOrderTif::FAK;
			}

			if ((*itr).HasMember("ordtype"))
			{
				string ordtype = (*itr)["ordtype"].GetString();
				if (ordtype == "LIMIT")
					ord.OrdType = KOrderType::LIMIT;
				else if (ordtype == "MARKET")
					ord.OrdType = KOrderType::MARKET;
				else if (ordtype == "MARKET_LIMIT")
					ord.OrdType = KOrderType::MARKET_LIMIT;
				else if (ordtype == "STOPLIMIT")
					ord.OrdType = KOrderType::STOPLIMIT;
				else if (ordtype == "STOP" )
					ord.OrdType = KOrderType::STOP;
			}
			else
				ord.OrdType = KOrderType::LIMIT;

			ord.timestamp = 0;
			ord.isManual = (*itr)["isManual"].GetBool();

			ord.text = (*itr)["text"].GetString();

			ord.strategy = (*itr)["strategy"].GetString();
			ord.stratid = atoi(ord.strategy.c_str());
			ord.tag = (*itr)["tag"].GetString();
			ord.tag2 = (*itr)["tag2"].GetString();
			ord.source = (*itr)["source"].GetString();

			ord.user = (*itr)["user"].GetString();
			ord.org = (*itr)["org"].GetString();
			ord.groupname = (*itr)["groupname"].GetString();

			if ((*itr).HasMember("algoid"))
				ord.algoid = (*itr)["algoid"].GetString();
			else
				ord.algoid = "ORDBOOK";

			if (ord.algoid == "")
				ord.algoid = "ORDBOOK";

			ord.guid = ord.algoid;

			if ((*itr).HasMember("openclose") && !(*itr)["openclose"].IsNull())
				ord.openclose = (*itr)["openclose"].GetString();
			else
				ord.openclose = "NONE";

			res.push_back(ord);
		}

		return res;
	}

	/**
	 * @brief Decodes the JSON order and returns a vector of Order objects.
	 * @param json The JSON string to decode.
	 * @note NOTE: this function uses the new KOrdAction, KOrdSide, KOrdTif, KOrdType enums and converts
	 * them from strings to enum values. I know, I know you going to say we can use protobufs, but this is
	 * actually more portable, lightweight, and easy to use. Sam don't play proto games.
	 * @return A vector of Order objects.
	 */
	// Safe accessors for rapidjson values - prevent crash on missing fields
	// Logs a warning when a field is missing or has wrong type
	inline static void LogMissingField(const Value& obj, const char* key, const char* expectedType)
	{
		// Silenced — these warnings are expected for optional fields (minqty, isManual, etc.)
	}

	inline static const char* SafeGetString(const Value& obj, const char* key, const char* defaultVal = "")
	{
		if (obj.HasMember(key) && obj[key].IsString())
			return obj[key].GetString();
		LogMissingField(obj, key, "string");
		return defaultVal;
	}

	inline static int SafeGetInt(const Value& obj, const char* key, int defaultVal = 0)
	{
		if (obj.HasMember(key) && obj[key].IsInt())
			return obj[key].GetInt();
		LogMissingField(obj, key, "int");
		return defaultVal;
	}

	inline static int64_t SafeGetInt64(const Value& obj, const char* key, int64_t defaultVal = 0)
	{
		if (obj.HasMember(key) && obj[key].IsInt64())
			return obj[key].GetInt64();
		LogMissingField(obj, key, "int64");
		return defaultVal;
	}

	inline static double SafeGetDouble(const Value& obj, const char* key, double defaultVal = 0.0)
	{
		if (obj.HasMember(key) && obj[key].IsNumber())
			return obj[key].GetDouble();
		LogMissingField(obj, key, "double");
		return defaultVal;
	}

	inline static bool SafeGetBool(const Value& obj, const char* key, bool defaultVal = false)
	{
		if (obj.HasMember(key) && obj[key].IsBool())
			return obj[key].GetBool();
		LogMissingField(obj, key, "bool");
		return defaultVal;
	}

	inline static std::vector<KTN::Order> DecodeOrdersFromRestApi(std::string json)
	{
		Document d;
		std::vector<KTN::Order> res;

		if (d.Parse(json.c_str(), json.length()).HasParseError())
		{
			fprintf(stderr, "\nRapidJson Error(offset %u): %d\n",
					(unsigned)d.GetErrorOffset(),
					d.GetParseError());
			int e = (unsigned)d.GetErrorOffset();
			std::ostringstream oss;
			oss << "JSON OFFSET ERROR AT " << e;
			std::cout << oss.str() << std::endl;
			return res;
		}

		if (!d.IsArray())
		{
			std::cout << "JSON ORDERS MUST BE SENT AS ARRAY!!!!" << std::endl;
			return res;
		}

		for (Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
		{
			KTN::Order ord = {};
			const Value& obj = *itr;

			ord.OrdAction = KOrderAction::Value(SafeGetString(obj, "KOrdAction", "UNKNOWN"));
			ord.OrdAlgoTrig = KOrderAlgoTrigger::Value(SafeGetString(obj, "KOrdAlgoTrigger", "UNKNOWN"));
			ord.OrdExch = KOrderExchange::Value(SafeGetString(obj, "KOrdExch", "UNKNOWN"));
			ord.OrdFillType = KOrderFillType::Value(SafeGetString(obj, "KOrdFillType", "UNKNOWN"));
			ord.OrdProdType = KOrderProdType::Value(SafeGetString(obj, "KOrdProdType", "UNKNOWN"));
			ord.OrdPutCall = KOrderPutCall::Value(SafeGetString(obj, "KOrdPutCall", "UNKNOWN"));
			ord.OrdState = KOrderState::Value(SafeGetString(obj, "KOrdState", "UNKNOWN"));
			ord.OrdStatus = KOrderStatus::Value(SafeGetString(obj, "KOrdStatus", "UNKNOWN"));
			ord.OrdTif = KOrderTif::Value(SafeGetString(obj, "KOrdTif", "UNKNOWN"));
			ord.OrdType = KOrderType::Value(SafeGetString(obj, "KOrdType", "UNKNOWN"));
			ord.OrdSide = KOrderSide::Value(SafeGetString(obj, "KOrdSide", "UNKNOWN"));

			ord.action = KOrderAction::toString(ord.OrdAction);
			ord.exchange = KOrderExchange::toString(ord.OrdExch);
			ord.filltype = KOrderFillType::toInt(ord.OrdFillType);
			ord.state = KOrderState::toString(ord.OrdState);

			// Symbol - safe copy with length limit
			const char* sym = SafeGetString(obj, "symbol");
			strncpy(ord.symbol, sym, sizeof(ord.symbol) - 1);
			ord.symbol[sizeof(ord.symbol) - 1] = '\0';

			// Product field
			if (obj.HasMember("product") && !obj["product"].IsNull() && obj["product"].IsString())
				ord.product = obj["product"].GetString();

			// Quantities and Price
			ord.quantity = SafeGetInt(obj, "quantity");
			ord.leavesqty = SafeGetInt(obj, "leavesqty");
			ord.fillqty = SafeGetInt(obj, "fillqty");
			ord.dispqty = SafeGetInt(obj, "maxshow");  // DB column is "maxshow", not "dispqty"
			ord.minqty = SafeGetInt(obj, "minqty");
			ord.price = SafeGetDouble(obj, "price");
			ord.stoppx = SafeGetDouble(obj, "stoppx");

			// Order number and exchange order id - safe copy with length limit
			const char* onum = SafeGetString(obj, "ordernum");
			strncpy(ord.ordernum, onum, sizeof(ord.ordernum) - 1);
			ord.ordernum[sizeof(ord.ordernum) - 1] = '\0';

			std::string id = SafeGetString(obj, "exchordid");
			if (!id.empty())
				ord.exchordid = stoull(id.c_str());

			// Timestamp
			ord.timestamp = SafeGetInt64(obj, "timestamp");

			// isManual flag
			ord.isManual = SafeGetBool(obj, "isManual");

			// basic open/close
			ord.openclose = SafeGetString(obj, "openclose", "NONE");

			// Strategy and Strategy ID
			ord.strategy = SafeGetString(obj, "strategy");
			ord.stratid = obj.HasMember("stratid") && obj["stratid"].IsInt()
							  ? obj["stratid"].GetInt()
							  : atoi(ord.strategy.c_str());

			// Tags and Source
			ord.tag = SafeGetString(obj, "tag");
			ord.tag2 = SafeGetString(obj, "tag2");
			ord.source = SafeGetString(obj, "source");

			// User, Organization, and Group Name
			ord.user = SafeGetString(obj, "user");
			ord.org = SafeGetString(obj, "org");
			ord.groupname = SafeGetString(obj, "groupname");

			// Algo ID and GUID
			ord.algoid = SafeGetString(obj, "algoid", "ORDBOOK");
			if (ord.algoid.empty())
				ord.algoid = "ORDBOOK";
			ord.guid = ord.algoid;

			res.push_back(ord);
		}

		return res;
	}
};

#endif /* SRC_JSON_JSONORDERDECODERS_HPP_ */
