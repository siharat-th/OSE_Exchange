//============================================================================
// Name        	: EquitySecMaster.hpp
// Author      	: 
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 1, 2024 12:40:03 PM
//============================================================================

#ifndef SRC_SYMBOLS_EQTSECMASTER_HPP_
#define SRC_SYMBOLS_EQTSECMASTER_HPP_

#include<string>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <tr1/unordered_map>
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


#define SECMASTER_EQT EquitySecMaster::instance().secmaster()

//using namespace KT01::Core;
using namespace std;

namespace KT01 
{
	namespace SECDEF 
	{
		namespace EQT 
		{


			/**
			 * @brief ExchangeMapping entry loaded from CSV.
			 * Maps SecDefName (venue) to CitriusQuoteSource, FIXRoute, etc.
			 */
			struct ExchangeMappingEntry
			{
				string secDefName;          // INET, ARCP, BYXB, ...
				string exchange;            // NASDAQ, NYSE ARCA, ...
				string citriusQuoteSource;  // INET, ARCA, BYX, ...
				string fixRoute;            // LIME-ECN, ARCP (Pillar), ...
				string mic;                 // XNAS, ARCX, ...
			};

			class EquityProdDef
			{
				long secid;
				string product;
				string exch;

				string prodtype;

			};

			/**
			 * @brief Represents the security definition for a financial instrument.
			 */
			class EquitySecDef
			{
			public:

				enum PRODTYPE
				{
					FUT, SPRD, OPT
				};

				long secid;
				string productCode;
				string exch;

				KOrderProdType::Enum prodtype;
				PRODTYPE pt;

				string symbol;
				string baseSymbol;     // Base symbol without venue (e.g., AAPL from AAPL@INET)
				string venue;          // Venue / SecDefName (e.g., INET, ARCP, BYXB, etc.)
				string citriusQuoteSource; // Citrius quote source (e.g., ARCA from ARCP via ExchangeMapping)
				string fixRoute;           // FIX route (e.g., LIME-ECN, ARCP via ExchangeMapping)

				string subexch;

				string exchangeSymbol;

				int msgw;
				int channel;

				bool fractional;

				double mintick;
				double mult;
				int mmy;
				int mmy2;


				//options specific
				double strike;
				int putcall;
				string underlying;
				string series;
				string expdate;

				double displayFactor;
				double priceFactor;

				/**
			  * @brief Generates a string representation of the security definition.
			  * @return The string representation of the security definition.
			  */
				string Print()
				{
					ostringstream oss;
					oss << symbol << " SECID=" << secid << " PRODUCT=" << productCode << " PRODTYPE=" << prodtype << " MINTICK=" << mintick << " MMY=" << mmy;

					oss << " SUBEXCH=" << subexch << " MSGW=" << msgw << " CHANID=" << channel;
					if (!venue.empty()) {
						oss << " VENUE=" << venue << " BASE=" << baseSymbol;
					}
					if (!citriusQuoteSource.empty()) {
						oss << " CQS=" << citriusQuoteSource;
					}
					if (!fixRoute.empty()) {
						oss << " FIXROUTE=" << fixRoute;
					}
					return oss.str();
				}
			};

			/**
			 * @brief Represents the Security Master class for CME
			 * *** VERSION 2 ****
			 * This version put in place after CME retired the .prf.csv files
			 * Instead, we will use the CME secdef.dat file, which has FIX definitions embedded,
			 * We can now run this with the SECDEF market data handler as well.
			 *
			 * Both files have the same field names once we decode them, so this class reads the
			 * file at the locations specified by Cme.SecDefDir and Cme.SecDefFile in the main settings file.
			 */
			class EquitySecMaster : public Singleton<EquitySecMaster> {
			public:
				EquitySecMaster();
				virtual ~EquitySecMaster();

				/**
			  * @brief Retrieves the instance of the CmeSecMaster class.
			  * @return The instance of the CmeSecMaster class.
			  */
				std::shared_ptr<EquitySecMaster> secmaster() { return m_secmaster_sp; }

				/**
			  * @brief Checks if the security master is loaded.
			  * @return True if the security master is loaded, false otherwise.
			  */
				inline bool IsLoaded()
				{
					return _loaded;
				}

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
				inline EquitySecDef getSecDef(int secid)
				{
					if (_secdef_idmap.find(secid) != _secdef_idmap.end())
						return _secdef_idmap[secid];

					EquitySecDef def = {};
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
				inline EquitySecDef getSecDef(string symbol)
				{
					if (_secdef_symbmap.find(symbol) != _secdef_symbmap.end())
					{
						//LogMe("SECMASTER FOUND " + symbol + " PRODUCT=" + _secdef_symbmap[symbol].product, LOG_GREEN);

						return _secdef_symbmap[symbol];
					}

					//string product = symbol.substr(0, symbol.length() - 2);

					//LogMe("SECMASTER DID NOT FIND SYMBOL " + symbol + " RETURN PRODUCT " + product, LOG_RED);

					EquitySecDef def = {};
					def.secid = 0;
					def.symbol = symbol;
					def.prodtype = KOrderProdType::EQT;
					// def.product = KOrderProdType::toString(def.prodtype);
					return def;
				}

				/**
			 * @brief Retrieves the security IDs for the given product code.
			 * @param productCode The product code.
			 * @return A vector of security IDs corresponding to the product code.
			 */
				inline vector<int> getSecIdsForProductCode(string productCode)
				{
					if (_secdef_prod2id.find(productCode) != _secdef_prod2id.end())
					{
						LogMe(productCode + ": SECMASTER FOUND SECID LIST");
						return _secdef_prod2id[productCode];
					}

					return vector<int>();
				}

				/**
				 * @brief Retrieves the security IDs for the given venue.
				 * @param venue The venue / SecDefName (INET, ARCP, etc.).
				 * @return A vector of security IDs corresponding to the venue.
				 */
				inline vector<int> getSecIdsForVenue(string venue)
				{
					if (_secdef_venue2id.find(venue) != _secdef_venue2id.end())
					{
						return _secdef_venue2id[venue];
					}
					return vector<int>();
				}

				/**
				 * @brief Retrieves the security definition for base symbol and venue.
				 * @param baseSymbol The base symbol (e.g., AAPL).
				 * @param venue The venue / SecDefName (e.g., INET).
				 * @return The security definition for AAPL@INET.
				 */
				inline EquitySecDef getSecDefByBaseSymbolAndVenue(string baseSymbol, string venue)
				{
					string fullSymbol = baseSymbol + "@" + venue;
					return getSecDef(fullSymbol);
				}

				/**
				 * @brief Retrieves all unique venues from the secdef.
				 * @return A vector of unique venue strings (INET, ARCP, etc.).
				 */
				inline vector<string> getUniqueVenues()
				{
					vector<string> venues;
					for (const auto& pair : _secdef_venue2id) {
						venues.push_back(pair.first);
					}
					return venues;
				}

				/**
				 * @brief Retrieves base symbols for a venue (for subscribe API).
				 * @param venue The venue / SecDefName (e.g., INET).
				 * @return A vector of base symbols (AAPL, MSFT, etc.).
				 */
				inline vector<string> getBaseSymbolsForVenue(string venue)
				{
					vector<string> symbols;
					if (_secdef_venue2id.find(venue) != _secdef_venue2id.end()) {
						for (int secid : _secdef_venue2id[venue]) {
							if (_secdef_idmap.find(secid) != _secdef_idmap.end()) {
								symbols.push_back(_secdef_idmap[secid].baseSymbol);
							}
						}
					}
					return symbols;
				}

				/**
				 * @brief Reverse-maps a CitriusQuoteSource to the venue/SecDefName.
				 * @param cqs The CitriusQuoteSource (e.g., ARCA, BATS, EDGA).
				 * @return The venue/SecDefName (e.g., ARCP, BZXB, EDGAB). Falls back to cqs if no mapping.
				 */
				inline string getVenueFromCQS(const string& cqs)
				{
					auto it = _cqs2venue.find(cqs);
					if (it != _cqs2venue.end())
						return it->second;
					return cqs;  // fallback: INET=INET, or unmapped sources
				}

				/**
				 * @brief Forward-maps a venue/SecDefName to its CitriusQuoteSource.
				 * @param venue The venue/SecDefName (e.g., ARCP, BYXB, BZXB).
				 * @return The CitriusQuoteSource (e.g., ARCA, BYX, BATS). Falls back to venue if no mapping.
				 */
				inline string getCQSFromVenue(const string& venue)
				{
					auto it = _exchangeMapping.find(venue);
					if (it != _exchangeMapping.end())
						return it->second.citriusQuoteSource;
					return venue;  // fallback: INET=INET, or unmapped venues
				}

				inline std::string processProdType(const std::string& symbol, const std::string& prodtype) {
					// Check if the string contains a '-'
					if (symbol.find('-') != std::string::npos) {
						return "MLEG";
					}
					// Return the original string if no '-' is found
					return prodtype;
				}

			private:
				void LogMe(std::string szMsg);

				void LoadFile(string filename, EquitySecDef::PRODTYPE prodtype);
				void LoadOptionsFile(string filename, EquitySecDef::PRODTYPE prodtype);
				void LoadExchangeMapping(string filename);


				inline float checkFloat(const string& valueStr)
				{
					float value = 0.0f; // Default value for empty fields

					if (!valueStr.empty())
						value = std::stof(valueStr);

					return value;
				}

				inline int checkInt(const string& valueStr)
				{
					int value = 0; // Default value for empty fields

					if (!valueStr.empty())
						value = std::stoi(valueStr);

					return value;
				}



			private:
				std::shared_ptr<EquitySecMaster> m_secmaster_sp;

				bool _loaded;

				string _DIR;
				string _FILE;

				//	string _DIR = "/root/lwt/config3/data/";

				//	const string FUTS_FILE = "cmeg.fut.prf.csv";
				//	const string SPRD_FILE = "cmeg.strat.prf.csv";
				//	const string OPT_FILE = "cmeg.opt.prf.csv";
				//	const string CERT_FILE = "certprf.csv";

				tr1::unordered_map<int, string> _secidmap;
				tr1::unordered_map<int, EquitySecDef> _secdef_idmap;
				tr1::unordered_map<string, EquitySecDef> _secdef_symbmap;
				tr1::unordered_map<string, vector<int> > _secdef_prod2id;
				tr1::unordered_map<string, vector<int> > _secdef_venue2id;      // venue -> secids
				tr1::unordered_map<string, ExchangeMappingEntry> _exchangeMapping; // SecDefName -> mapping entry
				tr1::unordered_map<string, string> _cqs2venue; // CitriusQuoteSource -> venue (reverse mapping)
			};

		}
	}
}

#endif /* SRC_SYMBOLS_EQTSECMASTER_HPP_ */
