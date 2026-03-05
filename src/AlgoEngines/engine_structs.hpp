//============================================================================
// Name        	: engine_structs.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: May 6, 2023 2:56:57 PM
//============================================================================

#ifndef SRC_ALGOENGINES_ENGINE_STRUCTS_HPP_
#define SRC_ALGOENGINES_ENGINE_STRUCTS_HPP_

#include <string>
using namespace std;

/**
 * @brief Enumeration for the algo engine commands.
 */
struct KEngineCommand
{

	enum Enum : uint8_t
	{
		NONE, /**< No command */
		CANCEL, /**< Cancel command */
		MARKET, /**< Market command */
		RISK /**< Risk command */
	};

	/**
  * @brief Converts a string command to the corresponding enumeration value.
  * @param cmd The string command.
  * @return The corresponding enumeration value.
  */
	static KEngineCommand::Enum Value(string cmd)
	{
		if (cmd == "CANCEL")
			return Enum::CANCEL;
		if (cmd == "MARKET")
			return Enum::MARKET;
		if (cmd == "RISK")
			return Enum::RISK;

		return Enum::NONE;
	}

	/**
  * @brief Converts an integer command to the corresponding enumeration value.
  * @param cmd The integer command.
  * @return The corresponding enumeration value.
  */
	static KEngineCommand::Enum Value(int cmd)
	{
		if (cmd == (int)Enum::CANCEL)
			return Enum::CANCEL;
		if (cmd == (int)Enum::MARKET)
			return Enum::MARKET;
		if (cmd == (int)Enum::RISK)
			return Enum::RISK;

		return Enum::NONE;
	}

	/**
  * @brief Converts an enumeration value to its string representation.
  * @param status The enumeration value.
  * @return The string representation of the enumeration value.
  */
	static std::string toString(Enum status)
	{
		switch(status) {
			case KEngineCommand::NONE:
				return "NONE";
			case KEngineCommand::CANCEL:
				return "CANCEL";
			case KEngineCommand::MARKET:
				return "MARKET";
			case KEngineCommand::RISK:
				return "RISK";
			default:
				return "INVALID";
		}
	}
};

/**
 * @brief Enumeration for the engine items.
 */
struct KEngineItem
{
	enum Enum : uint8_t
	{
		NONE, /**< No item */
		PROFIT_TAKING, /**< Profit taking item */
		STOP_LOSS, /**< Stop loss item */
		MINUTES_TO_LIVE, /**< Minutes to live item */
		PUSH_TIME /**< Push time item */
	};

	/**
  * @brief Converts a string item to the corresponding enumeration value.
  * @param cmd The string item.
  * @return The corresponding enumeration value.
  */
	static KEngineItem::Enum Value(string cmd)
	{
		if (cmd == "PROFIT_TAKING")
			return Enum::PROFIT_TAKING;
		if (cmd == "STOP_LOSS")
			return Enum::STOP_LOSS;
		if (cmd == "MINUTES_TO_LIVE")
			return Enum::MINUTES_TO_LIVE;
		if (cmd == "PUSH_TIME")
			return Enum::PUSH_TIME;

		return Enum::NONE;
	}

	/**
  * @brief Converts an enumeration value to its string representation.
  * @param item The enumeration value.
  * @return The string representation of the enumeration value.
  */
	static std::string toString(Enum item)
	{
		switch(item) {
			case KEngineItem::NONE:
				return "NONE";
			case KEngineItem::PROFIT_TAKING:
				return "PROFIT_TAKING";
			case KEngineItem::STOP_LOSS:
				return "STOP_LOSS";
			case KEngineItem::MINUTES_TO_LIVE:
				return "MINUTES_TO_LIVE";
			case KEngineItem::PUSH_TIME:
				return "PUSH_TIME";
			default:
				return "INVALID";
		}
	}
};

/**
 * @brief Enumeration for the engine types.
 */
struct KEngineType
{
	enum Enum : uint8_t
	{
		NONE, /**< No engine type */
		PLMON, /**< PLMON engine type */
		SPREADPRICER /**< SPREADPRICER engine type */
	};

	/**
  * @brief Converts a string engine type to the corresponding enumeration value.
  * @param cmd The string engine type.
  * @return The corresponding enumeration value.
  */
	static KEngineType::Enum Value(string cmd)
	{
		if (cmd == "PLMON")
			return Enum::PLMON;
		if (cmd == "SPREADPRICER")
			return Enum::SPREADPRICER;

		return Enum::NONE;
	}

	/**
  * @brief Converts an integer engine type to the corresponding enumeration value.
  * @param cmd The integer engine type.
  * @return The corresponding enumeration value.
  */
	static KEngineType::Enum Value(int cmd)
	{
		if (cmd == (int)Enum::PLMON)
			return Enum::PLMON;
		if (cmd == (int)Enum::SPREADPRICER)
			return Enum::SPREADPRICER;

		return Enum::NONE;
	}

	/**
  * @brief Converts an enumeration value to its string representation.
  * @param engine The enumeration value.
  * @return The string representation of the enumeration value.
  */
	static std::string toString(Enum engine)
	{
		switch(engine) {
			case KEngineType::PLMON:
				return "PLMON";
			case KEngineType::SPREADPRICER:
				return "SPREADPRICER";
			default:
				return "INVALID";
		}
	}
};


struct EngineCommand
{
	string topic;
	string source;
	string guid;
	string spreadid;

	std::unordered_map<KEngineItem::Enum, string> items;

	KEngineType::Enum Engine;
	KEngineCommand::Enum Command;
};


#endif /* SRC_ALGOENGINES_ENGINE_STRUCTS_HPP_ */
