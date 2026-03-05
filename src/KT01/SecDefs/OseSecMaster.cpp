//============================================================================
// Name        : OseSecMaster.cpp
// Author      : Siharat Thammaya
// Copyright   : Copyright (C) 2026 AKL
// Description : Security Master for OSE — loads ose-secdef.csv
//============================================================================

#include <KT01/SecDefs/OseSecMaster.hpp>

using namespace KT01::SECDEF::OSE;

OseSecMaster::OseSecMaster() : _loaded(false)
{
	KT01_LOG_INFO("OSE_SECMASTER", "OseSecMaster Constructor");

	Settings settings;
	settings.Load("Settings.txt", "Settings");

	string dir = settings.getString("Ose.SecDefDir");
	string secdeffile = settings.getString("Ose.SecDefFile");

	if (dir.length() == 0)
	{
		KT01_LOG_CRITICAL("OSE_SECMASTER", "NO Ose.SecDefDir IN SETTINGS!!! FATAL!");
		std::exit(1);
	}

	if (!dir.empty() && dir.back() != '/')
		dir += '/';

	_DIR = dir;
	_SECDEFFILE = secdeffile;

	if (secdeffile.empty())
	{
		KT01_LOG_CRITICAL("OSE_SECMASTER", "NO Ose.SecDefFile IN SETTINGS!!! FATAL!");
		std::exit(1);
	}

	string fqpath = dir + secdeffile;
	LogMe("GOT SECDEF PATH: " + fqpath);
	LoadFile(fqpath);
}

OseSecMaster::~OseSecMaster() {
}

void OseSecMaster::LogMe(std::string szMsg)
{
	KT01_LOG_INFO("OSE_SECMASTER", szMsg);
}

void OseSecMaster::LoadFile(string filename)
{
	std::ifstream file(filename.c_str());
	if (!file)
	{
		KT01_LOG_ERROR("OSE_SECMASTER", "SECMASTER FILE NOT FOUND: " + filename);
		std::exit(1);
	}

	LogMe("OSE SECDEFS LOADING FILE: " + filename);

	// 1) Read the header line and map the fields
	std::string line;
	if (!std::getline(file, line))
	{
		KT01_LOG_ERROR("OSE_SECMASTER", filename + " IS EMPTY. FATAL.");
		std::exit(1);
	}

	std::unordered_map<std::string, int> hdrmap;
	vector<string> headerFields = StringSplitter::Split(line, ",");
	for (size_t i = 0; i < headerFields.size(); ++i)
	{
		hdrmap[headerFields[i]] = static_cast<int>(i);
	}

	// 2) Parse each row — OSE secdef CSV from Python capture tool
	// Expected columns: orderbook_id, short_symbol, symbol, financial_product,
	//                   price_decimals, tick_size_display, expiration_date, num_legs,
	//                   leg1_orderbook_id, leg1_side, leg1_ratio, ...
	int loaded = 0;

	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
			continue;

		try
		{
			line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());
			vector<std::string> fields = StringSplitter::Split(line, ",");

			if (fields.size() < 6)
				continue;

			OseSecDef def = {};
			def.orderbook_id = checkUint(fields[hdrmap["orderbook_id"]]);
			def.exch = "OSE";
			def.test_symbol = false;

			// Primary symbol = short_symbol (e.g., "JBLM26")
			// Long symbol kept for product extraction and DQ2 matching (e.g., "FUT_JBL_260615")
			string long_symbol = fields[hdrmap["symbol"]];
			if (hdrmap.find("short_symbol") != hdrmap.end() && !fields[hdrmap["short_symbol"]].empty())
				def.symbol = fields[hdrmap["short_symbol"]];
			else
				def.symbol = long_symbol;
			def.short_symbol = def.symbol;
			def.description = long_symbol; // Store long_symbol for DQ2 ins_id_s matching

			// Financial product
			string fp = fields[hdrmap["financial_product"]];
			if (fp == "Future")
				def.prodtype = KOrderProdType::FUTURE;
			else if (fp == "Combination" || fp == "Spread")
				def.prodtype = KOrderProdType::SPREAD;
			else if (fp == "Option")
				def.prodtype = KOrderProdType::OPTION;
			else
				def.prodtype = KOrderProdType::FUTURE;

			// Extract product from long symbol (e.g., "FUT_JBL_260615" -> "JBL")
			// For spreads: "FSPR_JBL_260313/JBL_260615" -> "JBL"
			{
				vector<string> parts = StringSplitter::Split(long_symbol, "_");
				if (parts.size() >= 2 && (parts[0] == "FUT" || parts[0] == "FSPR"))
					def.product = parts[1];
				else if (parts.size() >= 2 && (parts[0] == "CAL" || parts[0] == "PUT"))
					def.product = parts[1];
				else
					def.product = def.symbol;
			}

			def.price_decimals = static_cast<uint16_t>(checkInt(fields[hdrmap["price_decimals"]]));

			if (hdrmap.find("tick_size_display") != hdrmap.end())
				def.mintick = checkDouble(fields[hdrmap["tick_size_display"]]);

			if (hdrmap.find("expiration_date") != hdrmap.end())
				def.expiration_date = checkUint(fields[hdrmap["expiration_date"]]);

			def.mult = 1.0;

			// Parse legs for spreads
			if (hdrmap.find("num_legs") != hdrmap.end())
			{
				int num_legs = checkInt(fields[hdrmap["num_legs"]]);
				for (int i = 1; i <= num_legs; ++i)
				{
					string prefix = "leg" + std::to_string(i) + "_";
					OseLegDef leg = {};

					if (hdrmap.find(prefix + "orderbook_id") != hdrmap.end())
						leg.orderbook_id = checkUint(fields[hdrmap[prefix + "orderbook_id"]]);

					if (hdrmap.find(prefix + "side") != hdrmap.end())
					{
						string sideStr = fields[hdrmap[prefix + "side"]];
						leg.side = (sideStr == "Buy" || sideStr == "B")
							? KOrderSide::BUY : KOrderSide::SELL;
					}

					if (hdrmap.find(prefix + "ratio") != hdrmap.end())
						leg.ratio = checkInt(fields[hdrmap[prefix + "ratio"]]);

					leg.prodtype = KOrderProdType::FUTURE;
					def.legs.push_back(leg);
				}
			}

			Add(def);
			loaded++;
		}
		catch (const std::exception &e)
		{
			LogMe("Skipping invalid line: " + line);
			LogMe("Exception: " + std::string(e.what()));
			continue;
		}
	}

	file.close();

	std::ostringstream oss;
	oss << filename << " LOADED " << loaded << " SECDEFS FROM FILE";
	LogMe(oss.str());

	_loaded = true;
}

void OseSecMaster::Add(OseSecDef def)
{
	_secidmap[def.orderbook_id] = def.symbol;
	_secdef_idmap[def.orderbook_id] = def;
	_secdef_symbmap[def.symbol] = def;
	// Also add long_symbol (description) as alternate key for DQ2 ins_id_s matching
	if (!def.description.empty() && def.description != def.symbol)
		_secdef_symbmap[def.description] = def;
	_secdef_prod2id[def.product].push_back(def.orderbook_id);
}
