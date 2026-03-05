//============================================================================
// Name        	: CmeSecMasterV2.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 1, 2024 12:40:03 PM
//============================================================================

#ifndef SRC_SYMBOLS_CMESECMASTERV2_HPP_
#define SRC_SYMBOLS_CMESECMASTERV2_HPP_

#include <string>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <KT01/Core/Log.hpp>
#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>
#include <KT01/Core/Settings.hpp>
#include <KT01/Core/Settings.hpp>
#include <KT01/Core/Singleton.hpp>

// for prod types
#include <Orders/OrderEnumsV2.hpp>
using namespace KTN::ORD;

#define SECDEF_OPTIONS 0

#define SECMASTER CmeSecMaster::instance().secmaster()

using namespace std;

namespace KT01{
namespace SECDEF{
namespace CME{

/**
 * @brief Represents the security definition for a financial instrument.
 */
class CmeSecDef
{
public:

	long secid;
	string product;
	string exch;
	string secgroup;

	//string prodtype;
	KOrderProdType::Enum prodtype;

				string symbol;

				string subexch;

				int msgw;
				int channel;

				bool fractional;

				double mintick;
				double mult;
				int mmy;
				int mmy2;

	double displayFactor;
	int mainFraction;
	int subFraction;


	//options specific
	double strike;
	int putcall;
	string underlying;
	string series;
	string expdate;

				KOrderPutCall::Enum OrdPutCall;

				/**
				 * @brief Generates a string representation of the security definition.
				 * @return The string representation of the security definition.
				 */
				string Print()
				{
					ostringstream oss;
					oss << symbol << " SECID=" << secid << " PRODUCT=" << product << " PRODTYPE=" << prodtype << " MINTICK=" << mintick << " MMY=" << mmy;

					oss << " SUBEXCH=" << subexch << " MSGW=" << msgw << " CHANID=" << channel;
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
			class CmeSecMaster : public Singleton<CmeSecMaster>
			{
			public:
				CmeSecMaster();
				virtual ~CmeSecMaster();

				/**
				 * @brief Retrieves the instance of the CmeSecMaster class.
				 * @return The instance of the CmeSecMaster class.
				 */
				std::shared_ptr<CmeSecMaster> secmaster()
				{
					static std::shared_ptr<CmeSecMaster> shared_instance = std::shared_ptr<CmeSecMaster>(this, [](CmeSecMaster *){ });
					return shared_instance;
				}

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
				inline CmeSecDef getSecDef(int secid)
				{
					if (_secdef_idmap.find(secid) != _secdef_idmap.end())
						return _secdef_idmap[secid];

					CmeSecDef def = {};
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
	inline CmeSecDef getSecDef(string symbol)
	{
		if (_secdef_symbmap.find(symbol) != _secdef_symbmap.end())
		{
			//LogMe("SECMASTER FOUND " + symbol + " PRODUCT=" + _secdef_symbmap[symbol].product, LogLevel::INFO);

						return _secdef_symbmap[symbol];
					}

					string product = symbol.substr(0, symbol.length() - 2);

		//LogMe("SECMASTER DID NOT FIND SYMBOL " + symbol + " RETURN PRODUCT " + product, LogLevel::ERROR);

					CmeSecDef def = {};
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
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

	void LoadFile(string filename);
	//void LoadOptionsFile(string filename, CmeSecDef::PRODTYPE prodtype);

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

			private:
				//std::shared_ptr<CmeSecMaster> m_secmaster_sp;

				bool _loaded;

				string _DIR;
				string _FILE;

				std::unordered_map<int, string> _secidmap;
				std::unordered_map<string, int> _prod_segment_map;
				std::unordered_map<int, CmeSecDef> _secdef_idmap;
				std::unordered_map<string, CmeSecDef> _secdef_symbmap;
				std::unordered_map<string, vector<int>> _secdef_prod2id;
			};

} /* namespace CME */
} /* namespace SECDEF */
} /* namespace KT01 */

#endif /* SRC_SYMBOLS_CMESECMASTER_HPP_ */