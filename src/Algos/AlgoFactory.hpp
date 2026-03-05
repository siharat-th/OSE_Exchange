//============================================================================
// Name        	: AlgoFactory.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Jan 13, 2024 2:58:52 PM
//============================================================================

#ifndef SRC_ALGOS_ALGOFACTORY_HPP_
#define SRC_ALGOS_ALGOFACTORY_HPP_

#pragma once

#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/error/en.h>
using namespace rapidjson;

#include <stdio.h>
#include <tr1/unordered_map>

#include <KT01/Core/Log.hpp>
#include <ExchangeHandler/session/SessionCallback.hpp>

#include <Algos/AlgoBaseV3.hpp>
#include <Algos/algo_includes.hpp>
#include <AlgoSupport/algo_structs.hpp>
#include <AlgoEngines/engine_structs.hpp>

#include <ExchangeHandler/ExchangeBase2.hpp>
#include <KT01/SecDefs/CmeSecMasterV2.hpp>
#include <exchsupport/cme/settings/CmeSessionSettings.hpp>
using namespace KT01::SECDEF::CME;


#include <AlgoParams/LegParamParser.hpp>
#include <AlgoParams/OrderParser.hpp>
#include <AlgoEngines/engine_structs.hpp>

#include <KT01/Core/Singleton.hpp>

using namespace KTN;

/**
 * @class AlgoFactory
 * @brief Factory class responsible for launching various algorithms.
 *
 * @details This class parses the input JSON, extracts algorithm parameters
 * and legs, and returns an instance of the appropriate algorithm.
 */
class AlgoFactory : public Singleton<AlgoFactory>
{
public:
	/**
   * @brief Launches an algorithm based on the provided JSON configuration.
   * @param json JSON string containing algorithm configuration.
   * @param cpu CPU affinity for the algorithm.
   * @param exch Exchange sender interface for sending orders.
   * @return A pointer to the created AlgoBaseV3 object, or nullptr if the algorithm could not be created.
   */
	static AlgoBaseV3* LaunchAlgo(const std::string& json, int cpu,
			IExchangeSender* exch, vector<KTN::OrderPod>& orders)
	{
		
		AlgoHeader hdr = ParseAlgoHeader(json);

		string archetype = hdr.archetype;
		string guid = hdr.guid;
		//bool enabled = hdr.enabled;

		 //NOTE: If we are running an implied algo, we gen legs in the algo?
		LogMe("PARSING LEGS FOR ALGO " + guid + " ARCH=" + archetype, LogLevel::INFO);
		vector <instrument> legs;
		ParseLegs(archetype, json, legs);

		if ((archetype != ALGO_SQUEEZER) && (archetype != ALGO_ICEBERG)
			 && (archetype != ALGO_AUTOHDGOB))
		{
			LogMe("PARSING LEGS FOR ALGO " + guid + " ARCH=" + archetype, LogLevel::INFO);
			legs = LegParamParser::ParseSpreadLegs( json);
		}
		else
		{
			LogMe("NON-LEG ALGO RECEIVED: " + archetype, LogLevel::WARN);
			instrument blank = {};
			blank.exch = "cme";
			legs.push_back(blank);
		}
		//IMPORTANT-- SINCE WE NOW LAUNCH FROM INSIDE THE HANDLER, WE DONT PASS A SESSION FROM HERE.....
		AlgoInitializer params = {"", guid, cpu, exch, legs, orders}; // Create an AlgoInitializer object with the parsed parameters.

		logLaunching(archetype);

		// if (archetype.compare(ALGO_STACKER) == 0)
		// {
		// 	Stacker* algo = new Stacker(params);
		// 	return algo;
		// }

		// if (archetype.compare(ALGO_MULTISPREADER) == 0)
		// {
		// 	MultiSpreaderV3* algo = new MultiSpreaderV3(params);
		// 	return algo;
		// }

		// if (archetype.compare(ALGO_TWAP) == 0)
		// {
		// 	Twap* algo = new Twap(params);
		// 	return algo;
		// }

		// if (archetype.compare(ALGO_SQUEEZER) == 0)
		// {
		// 	SqueezerOB* algo = new SqueezerOB(params);
		// 	return algo;
		// }

		// if (archetype.compare(ALGO_STACKERTIMER) == 0)
		// {
		// 	StackerTimer* algo = new StackerTimer(params);
		// 	return algo;
		// }

		if (archetype.compare(ALGO_AKLSTACKER) == 0)
		{
			akl::stacker::AklStacker *algo = new akl::stacker::AklStacker(params);
			return algo;
		}

		// if (archetype.compare(ALGO_SKEWER) == 0)
		// {
		// 	Skewer* algo = new Skewer(params);
		// 	return algo;
		// }

		// if (archetype.compare(ALGO_SKEWTRADER) == 0)
		// {
		// 	SkewTrader* algo = new SkewTrader(params);
		// 	return algo;
		// }

		// if (archetype.compare(ALGO_WAVES) == 0)
		// {
		// 	Waves* algo = new Waves(params);
		// 	return algo;
		// }

		// if (archetype.compare(ALGO_AUTOHDGOB) == 0)
		// {
		// 	AutoHedgeOB* algo = new AutoHedgeOB(params);
		// 	return algo;
		// }

		// if (archetype.compare(ALGO_PLWATCHER) == 0)
		// {
		// 	PLWatcher* algo = new PLWatcher(params);
		// 	return algo;
		// }

		LogMe("ALGO " + archetype + " NOT FOUND IN ALGO MASTER SERVICE", LogLevel::ERROR);
		return NULL;
	}


	/**
   * @brief Parses the legs for the specified algorithm.
   * @param archetype The type of algorithm.
   * @param json JSON string containing leg configurations.
   * @param legs Vector where the parsed legs will be stored.
   */
	static void ParseLegs(string archetype, string json, vector<instrument>& legs)
	{
		if ((archetype != ALGO_SQUEEZER) && (archetype != ALGO_ICEBERG)
			 && (archetype != ALGO_WAVES))
		{

			legs = LegParamParser::ParseSpreadLegs( json);

//			if (_sessions.find(legs[0].exch) == _sessions.end()){
//				LogMe("!!! ERROR: SESSION FOR " + legs[0].exch + " NOT FOUND IN ALGO SERVICE", LogLevel::ERROR);
//			}
		}
		else
		{
			LogMe("NON-LEG ALGO RECEIVED: " + archetype, LogLevel::WARN);
			instrument blank = {};
			blank.exch = "cme";
			legs.push_back(blank);
		}
	}

	/**
   * @brief Parses the header section of the JSON string and extracts algorithm metadata.
   * @param json JSON string containing the header information.
   * @return An AlgoHeader structure containing parsed metadata.
   */
	static AlgoHeader ParseAlgoHeader(string json)
	{
		Document d;

		if (d.Parse(json.c_str(), json.length()).HasParseError()) {

			fprintf(stderr, "\nRapidJson Error(offset %u): %d\n",
				(unsigned)d.GetErrorOffset(),
				d.GetParseError());
			int e = (unsigned)d.GetErrorOffset();
			ostringstream oss;
			oss << "JSON OFFSET ERROR AT " << e ;
			LogMe(oss.str(),LogLevel::ERROR);
		}

		std::string templatename = "";
		std::string uniqueid = "";
		//int enabled = 0;

		//bool reset = false;

		AlgoHeader hdr = {};

		for (Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
		{
			hdr.archetype = (*itr)["TemplateName"].GetString();
			hdr.guid= (*itr)["UniqueId"].GetString();
			hdr.enabled = (*itr)["Enabled"].GetInt();

			if ((*itr).HasMember("ResetPosition"))
			{
				hdr.reset_pos = (*itr)["ResetPosition"].GetBool();
			}
			break;
		}
		return hdr;
	}


	/**
  * @brief Parses an order from a JSON string and returns a KTN::Order object.
  * @param json JSON string containing the order.
  * @return A KTN::Order object representing the parsed order.
  */
	static KTN::Order ParseAlgoOrder(const std::string& json)
	{
		Document d;

		if (d.Parse(json.c_str(), json.length()).HasParseError()) {

			fprintf(stderr, "\nRapidJson Error(offset %u): %d\n",
				(unsigned)d.GetErrorOffset(),
				d.GetParseError());
			int e = (unsigned)d.GetErrorOffset();
			ostringstream oss;
			oss << "JSON OFFSET ERROR AT " << e ;
			LogMe(oss.str(),LogLevel::ERROR);

		}

		string uniqueid = d["guid"].GetString();
		const auto& json_order = d["order"];

		LogMe("ORD CMD FOR " + uniqueid, LogLevel::INFO);

		KTN::Order incoming = OrderParser::parseincoming(json_order);
		return incoming;
	}

	/**
  * @brief Parses an engine command from the given JSON string.
  * @param json JSON string containing the engine command.
  * @return An EngineCommand object containing parsed engine command details.
  */
	static EngineCommand ParseEngineCommand(string json)
	{
		//cout << json << endl;

		Document d;

		if (d.Parse(json.c_str(), json.length()).HasParseError()) {

			fprintf(stderr, "\nRapidJson Error(offset %u): %d\n",
				(unsigned)d.GetErrorOffset(),
				d.GetParseError());
			int e = (unsigned)d.GetErrorOffset();
			ostringstream oss;
			oss << "JSON OFFSET ERROR AT " << e ;
			LogMe(oss.str(),LogLevel::ERROR);
		}

		//const auto& element = d;
		string uniqueid = d["guid"].GetString();
		string spreadid = d["id"].GetString();


		/*************************************************************************************
		 * right now, for our custom spread ob with p&l,
		 * we want to have commands that allow the user to manually control
		 * the state of the pl watcher:
		 * 		CANCEL - remove the spread from P&L/Risk Control. The user probably
		 * 				covered the spread themselves or for whatever reason doesn't want to use
		 * 				p&l monitor
		 * 		MARKET  - immediately stop p&l monitoring and close the position, at market.
		 *
		 * 	Note: we can add more commands/features here, and for other AlgoEngines as well...
		 * 	As long as they have the source/uniqueid/spreadid envelope, this should be relatively easy
		 ************************************************************************************/

		 int command = d["Command"].GetInt();
		 int engine = d["Engine"].GetInt();


		 std::unordered_map<KEngineItem::Enum, string> itemsMap;

		 if (d.HasMember("items") && d["items"].IsObject()) {
		         const rapidjson::Value& items = d["items"];
		         for (rapidjson::Value::ConstMemberIterator iter = items.MemberBegin(); iter != items.MemberEnd(); ++iter) {
		             const std::string& key = iter->name.GetString();
		             const std::string& value = iter->value.GetString();

		             KEngineItem::Enum item = KEngineItem::Value(key);

		             itemsMap[item] = value;
		         }
		   }

		 EngineCommand blank = {};
		 KEngineCommand::Enum cmd = KEngineCommand::Value(command);

		 if (KEngineCommand::NONE == cmd)
		 {
			 LogMe("ERROR: INVALID ENGINE COMMAND: " + command, LogLevel::ERROR);
			 return blank;
		 }

		 KEngineType::Enum eng =  KEngineType::Value(engine);
		 if (KEngineType::NONE == eng)
		 {
			 LogMe("ERROR: INVALID ENGINE TYPE: " + engine, LogLevel::ERROR);
			 return blank;
		 }

		 EngineCommand action = {};
		 action.Command = cmd;
		 action.Engine = eng;
		 action.guid = uniqueid;
		 action.spreadid = spreadid;
		 action.items = itemsMap;

		LogMe("GOT ENGINE CMD FOR " + uniqueid + " SPREADID=" + spreadid);

		return action;
	}

	static std::optional<bool> ParseKillswitchCommand(const std::string &json)
	{
		rapidjson::Document document;
		rapidjson::ParseResult result = document.Parse(json.c_str());

		if (!result)
		{
			std::string error = std::string("JSON parse error: ") + rapidjson::GetParseError_En(result.Code()) +
								" at offset " + std::to_string(result.Offset());
			LogMe(error, LogLevel::ERROR);
			return std::nullopt;
		}

		for (rapidjson::GenericValue<rapidjson::UTF8<>>::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
		{
			if (itr->HasMember("State") && (*itr)["State"].IsString())
			{
				const std::string state = (*itr)["State"].GetString();
				if (state == "ON")
				{
					LogMe("GOT KILLSWITCH COMMAND: " + state, LogLevel::INFO);
					return true;
				}
				else if (state == "OFF")
				{
					LogMe("GOT KILLSWITCH COMMAND: " + state, LogLevel::INFO);
					return false;
				}
				else
				{
					LogMe("KILLSWITCH STATE IS INVALID: " + state, LogLevel::ERROR);
					return std::nullopt;
				}
			}
		}

		LogMe("KILLSWITCH COMMAND IS MISSING STATE", LogLevel::ERROR);
		return std::nullopt;
	}

private:
	/**
	 * @brief Logs a message with the specified color.
	 * @param szMsg The message to log.
	 * @param iColor The color code for the log message.
	 */
	static void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO)
	{
		KT01_LOG_INFO("AlgoFactory", szMsg);
	}

	/**
	 * @brief Logs information about the launching of an algorithm.
	 * @param archetype The algorithm type being launched.
	 */
	static void logLaunching(const std::string& archetype) {
	    std::string name;

	    if (archetype.compare(ALGO_MULTISPREADER) == 0) {
	        name = ALGO_MULTISPREADER_NAME;
	    } else if (archetype.compare(ALGO_TWAP) == 0) {
	        name = ALGO_TWAP_NAME;
	    } else if (archetype.compare(ALGO_SQUEEZER) == 0) {
	        name = ALGO_SQUEEZER_NAME;
		} else if (archetype.compare(ALGO_STACKERTIMER) == 0) {
		        name = ALGO_STACKERTIMER_NAME;
		}

	    if (!name.empty()) {
	        LogMe("FACTORY LAUNCHING " + name, LogLevel::INFO);
	    }
	}


};

#endif /* SRC_ALGOS_ALGOFACTORY_HPP_ */
