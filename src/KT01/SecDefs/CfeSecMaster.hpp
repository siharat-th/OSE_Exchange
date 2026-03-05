//============================================================================
// Name        	: CfeSecMasterV2.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 1, 2024 12:40:03 PM
//============================================================================

#ifndef SRC_SYMBOLS_CFESECMASTERV2_HPP_
#define SRC_SYMBOLS_CFESECMASTERV2_HPP_

#include <string>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>
#include <KT01/Core/Settings.hpp>
#include <KT01/Core/Singleton.hpp>

// for prod types
#include <Orders/OrderEnumsV2.hpp>
using namespace KTN::ORD;

#include <KT01/Helpers/Base63.hpp>
using namespace KT01::Base63;

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace std;

#define SECMASTER_CFE KT01::SECDEF::CFE::CfeSecMaster::instance().secmaster()

namespace KT01
{
	namespace SECDEF
	{
		namespace CFE
		{

			class CfeLegDef
			{
			public:
				string b63_secid;
				uint64_t secid;
				string symbol;
				string product;
				KOrderProdType::Enum prodtype;
				KOrderSide::Enum side;
				int ratio;
				int mult;
				double mintick;
			};

			/**
			 * @brief Represents the security definition for a financial instrument.
			 */
			class CfeSecDef
			{
			public:
				string b63_secid;
				uint64_t secid;
				string symbol;
				string product;
				string exch;
				KOrderProdType::Enum prodtype;
				string description;

				double mintick;
				double mult;
				int mmy;
				int mmy2;

				vector<CfeLegDef> legs;

				int contract_size;
				bool test_symbol;

				/**
				 * @brief Generates a json string representation of the security definition.
				 * @return The string representation of the security definition.
				 */
				static std::string Print(const CfeSecDef &def)
				{
					using namespace rapidjson;

					// Create a RapidJSON Document (an in-memory JSON object)
					Document d;
					d.SetObject();
					Document::AllocatorType &alloc = d.GetAllocator();
					// Add top-level fields to the JSON
					d.AddMember("b63_secid", Value(def.b63_secid.c_str(), alloc), alloc);
					d.AddMember("secid", def.secid, alloc);
					d.AddMember("symbol", Value(def.symbol.c_str(), alloc), alloc);
					d.AddMember("product", Value(def.product.c_str(), alloc), alloc);
					d.AddMember("exch", Value(def.exch.c_str(), alloc), alloc);
					d.AddMember("prodtype", Value(KOrderProdType::toString(def.prodtype).c_str(), alloc), alloc);
					d.AddMember("description", Value(def.description.c_str(), alloc), alloc);
					d.AddMember("mintick", def.mintick, alloc);
					d.AddMember("mult", def.mult, alloc);
					d.AddMember("test_symbol", def.test_symbol, alloc);

					if (def.prodtype == KOrderProdType::SPREAD)
					{
						// Build the "legs" array
						Value legsArr(kArrayType);
						legsArr.Reserve(static_cast<SizeType>(def.legs.size()), alloc);

						for (auto &leg : def.legs)
						{
							Value legObj(kObjectType);
							legObj.AddMember("b63_secid", Value(leg.b63_secid.c_str(), alloc), alloc);
							legObj.AddMember("secid", leg.secid, alloc);
							legObj.AddMember("symbol", Value(leg.symbol.c_str(), alloc), alloc);
							legObj.AddMember("product", Value(leg.product.c_str(), alloc), alloc);
							legObj.AddMember("prodtype", Value(KOrderProdType::toString(leg.prodtype).c_str(), alloc), alloc);
							legObj.AddMember("side", Value(KOrderSide::toString(leg.side).c_str(), alloc), alloc);
							legObj.AddMember("ratio", leg.ratio, alloc);
							legObj.AddMember("mult", leg.mult, alloc);
							legObj.AddMember("mintick", leg.mintick, alloc);

							legsArr.PushBack(legObj, alloc);
						}

						d.AddMember("legs", legsArr, alloc);
					}

					// Convert JSON Document to string
					StringBuffer buffer;
					Writer<StringBuffer> writer(buffer);
					d.Accept(writer);

					return std::string(buffer.GetString());
				}
			};

			

			/**
			 * @brief Represents the Security Master class for Cfe
			 * *** VERSION 2 ****
			 * This version put in place after Cfe retired the .prf.csv files
			 * Instead, we will use the Cfe secdef.dat file, which has FIX definitions embedded,
			 * We can now run this with the SECDEF market data handler as well.
			 *
			 * Both files have the same field names once we decode them, so this class reads the
			 * file at the locations specified by Cfe.SecDefDir and Cfe.SecDefFile in the main settings file.
			 */
			class CfeSecMaster : public Singleton<CfeSecMaster>
			{
			public:
				CfeSecMaster();
				virtual ~CfeSecMaster();

				/**
				 * @brief Retrieves the instance of the CfeSecMaster class.
				 * @return The instance of the CfeSecMaster class.
				 */
				std::shared_ptr<CfeSecMaster> secmaster() { return m_secmaster_sp; }

				/**
				 * @brief Checks if the security master is loaded.
				 * @return True if the security master is loaded, false otherwise.
				 */
				inline bool IsLoaded()
				{
					return _loaded;
				}


				/**
				 * @brief Adds a security definition to the security master.
				 * @param def The security definition to be added.
				 */
				void Add(CfeSecDef def);

				/**
				 * @brief Retrieves the symbol for the given security ID.
				 * @param secid The security ID.
				 * @return The symbol corresponding to the security ID.
				 */
				inline string getSymbol(int secid)
				{
					if (_secidmap.find(secid) != _secidmap.end())
						return _secidmap[secid];

					ostringstream oss;
					oss << secid;
					return oss.str();
				}

				/**
				 * @brief Retrieves the security definition for the given security ID.
				 * @param secid The security ID.
				 * @return The security definition corresponding to the security ID.
				 */
				inline CfeSecDef getSecDef(int secid)
				{
					if (_secdef_idmap.find(secid) != _secdef_idmap.end())
						return _secdef_idmap[secid];

					CfeSecDef def = {};
					def.secid = secid;
					ostringstream oss;
					oss << secid;
					def.symbol = oss.str();
					return def;
				}

				/**
				 * @brief Retrieves the security definition for the given symbol.
				 * @param symbol The symbol.
				 * @return The security definition corresponding to the symbol.
				 */
				inline CfeSecDef getSecDef(string symbol)
				{
					if (_secdef_symbmap.find(symbol) != _secdef_symbmap.end())
					{
						// LogMe("SECMASTER FOUND " + symbol + " PRODUCT=" + _secdef_symbmap[symbol].product);

						return _secdef_symbmap[symbol];
					}

					string product = symbol.substr(0, symbol.length() - 2);

					// LogMe("SECMASTER DID NOT FIND SYMBOL " + symbol + " RETURN PRODUCT " + product);

					CfeSecDef def = {};
					def.secid = 0;
					def.symbol = symbol;
					def.prodtype = KOrderProdType::FUTURE;
					def.product = product;
					return def;
				}

				/**
				 * @brief Retrieves the security IDs for the given product.
				 * @param product The product.
				 * @return A vector of security IDs corresponding to the product.
				 */
				inline vector<int> getSecIdsForProduct(string product)
				{
					if (_secdef_prod2id.find(product) != _secdef_prod2id.end())
					{
						LogMe(product + ": SECMASTER FOUND SECID LIST");
						return _secdef_prod2id[product];
					}

					return vector<int>();
				}

				/**
				 * @brief Retrieves the security IDs for the given product.
				 * @param product The product.
				 * @return A vector of security IDs corresponding to the product.
				 */
				inline int getSegmentIdForProduct(string product)
				{
					if (_prod_segment_map.find(product) != _prod_segment_map.end())
					{
						LogMe(product + ": SECMASTER FOUND PRODUCT IN SEGMENT LIST");

						return _prod_segment_map[product];
					}

					return 255;
				}

			private:
				void LogMe(std::string szMsg);

				void LoadFile(string filename);//, KOrderProdType::Enum prodtype);
                
				CfeSecDef ReadCompositeDefinition(vector<string> vec, std::unordered_map<std::string, int>& hdrmap);

				inline float checkFloat(const string &valueStr)
				{
					float value = 0.0f; // Default value for empty fields

					if (!valueStr.empty())
						value = std::stof(valueStr);

					return value;
				}

				inline int checkInt(const string &valueStr)
				{
					int value = 0; // Default value for empty fields

					if (!valueStr.empty())
						value = std::stoi(valueStr);

					return value;
				}

				inline int decodeTickSize(string tickStr)
				{
					if (tickStr.empty())
						return 0;

					if (tickStr == "penny")
						return 1;
					if (tickStr == "cmplxpenny")
						return 1;
					if (tickStr == "nickel")
						return 5;
					if (tickStr == "taspenny")
						return 1;
					if (tickStr == "halfdollar")
						return 50;

					return 0;
				}

				inline std::string normalSpreadSymbol(const std::string &input)
				{
					// Find the delimiter " - " separating the two symbols.
					size_t delimPos = input.find(" - ");
					if (delimPos == std::string::npos)
					{
						return "";
					}

					// Extract the left and right parts.
					std::string leftSymbol = input.substr(0, delimPos);
					std::string rightSymbol = input.substr(delimPos + 3); // skip " - "

					// Process left symbol: get the part before the first colon.
					size_t colonPos = leftSymbol.find(':');
					if (colonPos != std::string::npos)
					{
						leftSymbol = leftSymbol.substr(0, colonPos);
					}
					// Remove any slash from the left symbol.
					leftSymbol.erase(std::remove(leftSymbol.begin(), leftSymbol.end(), '/'), leftSymbol.end());

					// Process right symbol similarly.
					colonPos = rightSymbol.find(':');
					if (colonPos != std::string::npos)
					{
						rightSymbol = rightSymbol.substr(0, colonPos);
					}
					rightSymbol.erase(std::remove(rightSymbol.begin(), rightSymbol.end(), '/'), rightSymbol.end());

					// Combine the processed symbols with a hyphen.
					return leftSymbol + "-" + rightSymbol;
				}

			private:
				std::shared_ptr<CfeSecMaster> m_secmaster_sp;

				bool _loaded;

				string _DIR;
				string _COMPFILE;

				std::unordered_map<int, string> _secidmap;
				std::unordered_map<string, int> _prod_segment_map;
				std::unordered_map<int, CfeSecDef> _secdef_idmap;
				std::unordered_map<string, CfeSecDef> _secdef_symbmap;
				std::unordered_map<string, vector<int>> _secdef_prod2id;
			};

		} // namespace CFE
	} // namespace SECDEF
} // namespace KT01

#endif /* SRC_SYMBOLS_CfeSECMASTER_HPP_ */
