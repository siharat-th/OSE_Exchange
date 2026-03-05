//============================================================================
// Name        : OseSecMaster.hpp
// Author      : Siharat Thammaya
// Copyright   : Copyright (C) 2026 AKL
// Description : Security Master for OSE (loads ose-secdef.csv from Python capture tool)
//============================================================================

#ifndef SRC_SYMBOLS_OSESECMASTER_HPP_
#define SRC_SYMBOLS_OSESECMASTER_HPP_

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>
#include <KT01/Core/Settings.hpp>
#include <KT01/Core/Singleton.hpp>

#include <Orders/OrderEnumsV2.hpp>
using namespace KTN::ORD;

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace std;

#define SECMASTER_OSE OseSecMaster::instance().secmaster()

namespace KT01
{
	namespace SECDEF
	{
		namespace OSE
		{

			struct OseLegDef
			{
				uint32_t orderbook_id;
				string symbol;
				string product;
				KOrderProdType::Enum prodtype;
				KOrderSide::Enum side;
				int ratio;
			};

			class OseSecDef
			{
			public:
				uint32_t orderbook_id;
				string symbol;
				string short_symbol;
				string product;
				string exch;
				KOrderProdType::Enum prodtype;
				string description;

				double mintick;
				double mult;
				uint16_t price_decimals;
				uint32_t expiration_date;

				vector<OseLegDef> legs;

				bool test_symbol;

				static std::string Print(const OseSecDef &def)
				{
					using namespace rapidjson;
					Document d;
					d.SetObject();
					Document::AllocatorType &alloc = d.GetAllocator();

					d.AddMember("orderbook_id", def.orderbook_id, alloc);
					d.AddMember("symbol", Value(def.symbol.c_str(), alloc), alloc);
					d.AddMember("short_symbol", Value(def.short_symbol.c_str(), alloc), alloc);
					d.AddMember("product", Value(def.product.c_str(), alloc), alloc);
					d.AddMember("exch", Value(def.exch.c_str(), alloc), alloc);
					d.AddMember("prodtype", Value(KOrderProdType::toString(def.prodtype).c_str(), alloc), alloc);
					d.AddMember("mintick", def.mintick, alloc);
					d.AddMember("price_decimals", def.price_decimals, alloc);
					d.AddMember("expiration_date", def.expiration_date, alloc);
					d.AddMember("test_symbol", def.test_symbol, alloc);

					if (def.prodtype == KOrderProdType::SPREAD && !def.legs.empty())
					{
						Value legsArr(kArrayType);
						legsArr.Reserve(static_cast<SizeType>(def.legs.size()), alloc);
						for (auto &leg : def.legs)
						{
							Value legObj(kObjectType);
							legObj.AddMember("orderbook_id", leg.orderbook_id, alloc);
							legObj.AddMember("symbol", Value(leg.symbol.c_str(), alloc), alloc);
							legObj.AddMember("product", Value(leg.product.c_str(), alloc), alloc);
							legObj.AddMember("side", Value(KOrderSide::toString(leg.side).c_str(), alloc), alloc);
							legObj.AddMember("ratio", leg.ratio, alloc);
							legsArr.PushBack(legObj, alloc);
						}
						d.AddMember("legs", legsArr, alloc);
					}

					StringBuffer buffer;
					Writer<StringBuffer> writer(buffer);
					d.Accept(writer);
					return std::string(buffer.GetString());
				}
			};

			class OseSecMaster : public Singleton<OseSecMaster>
			{
			public:
				OseSecMaster();
				virtual ~OseSecMaster();

				std::shared_ptr<OseSecMaster> secmaster()
				{
					static std::shared_ptr<OseSecMaster> shared_instance = std::shared_ptr<OseSecMaster>(this, [](OseSecMaster *){});
					return shared_instance;
				}

				inline bool IsLoaded() { return _loaded; }

				inline bool Contains(uint32_t orderbook_id)
				{
					return _secdef_idmap.find(orderbook_id) != _secdef_idmap.end();
				}

				void Add(OseSecDef def);

				inline string getSymbol(uint32_t orderbook_id)
				{
					if (_secidmap.find(orderbook_id) != _secidmap.end())
						return _secidmap[orderbook_id];

					ostringstream oss;
					oss << orderbook_id;
					return oss.str();
				}

				inline OseSecDef getSecDef(uint32_t orderbook_id)
				{
					if (_secdef_idmap.find(orderbook_id) != _secdef_idmap.end())
						return _secdef_idmap[orderbook_id];

					OseSecDef def = {};
					def.orderbook_id = orderbook_id;
					ostringstream oss;
					oss << orderbook_id;
					def.symbol = oss.str();
					return def;
				}

				inline OseSecDef getSecDef(string symbol)
				{
					if (_secdef_symbmap.find(symbol) != _secdef_symbmap.end())
						return _secdef_symbmap[symbol];

					OseSecDef def = {};
					def.orderbook_id = 0;
					def.symbol = symbol;
					def.prodtype = KOrderProdType::FUTURE;
					return def;
				}

				inline vector<uint32_t> getSecIdsForProduct(string product)
				{
					if (_secdef_prod2id.find(product) != _secdef_prod2id.end())
						return _secdef_prod2id[product];
					return vector<uint32_t>();
				}

				inline int getSegmentIdForProduct(string product)
				{
					if (_prod_segment_map.find(product) != _prod_segment_map.end())
						return _prod_segment_map[product];
					return 255;
				}

				// Series key (composite from series_t fields) → orderbook_id mapping
				// Populated by OmnetWorker during DQ2, read by OmnetBdxThread during BO5
				inline void RegisterSeriesKey(uint64_t seriesKey, uint32_t orderbook_id)
				{
					_seriesKeyToObid[seriesKey] = orderbook_id;
				}

				inline uint32_t GetOrderbookIdBySeriesKey(uint64_t seriesKey) const
				{
					auto it = _seriesKeyToObid.find(seriesKey);
					if (it != _seriesKeyToObid.end())
						return it->second;
					return 0;
				}

			private:
				void LogMe(std::string szMsg);
				void LoadFile(string filename);

				inline double checkDouble(const string &valueStr)
				{
					if (!valueStr.empty())
						return std::stod(valueStr);
					return 0.0;
				}

				inline int checkInt(const string &valueStr)
				{
					if (!valueStr.empty())
						return std::stoi(valueStr);
					return 0;
				}

				inline uint32_t checkUint(const string &valueStr)
				{
					if (!valueStr.empty())
						return static_cast<uint32_t>(std::stoul(valueStr));
					return 0;
				}

			private:
				bool _loaded;
				string _DIR;
				string _SECDEFFILE;

				std::unordered_map<uint32_t, string> _secidmap;
				std::unordered_map<string, int> _prod_segment_map;
				std::unordered_map<uint32_t, OseSecDef> _secdef_idmap;
				std::unordered_map<string, OseSecDef> _secdef_symbmap;
				std::unordered_map<string, vector<uint32_t>> _secdef_prod2id;
				std::unordered_map<uint64_t, uint32_t> _seriesKeyToObid;
			};

		} // namespace OSE
	} // namespace SECDEF
} // namespace KT01

#endif /* SRC_SYMBOLS_OSESECMASTER_HPP_ */
