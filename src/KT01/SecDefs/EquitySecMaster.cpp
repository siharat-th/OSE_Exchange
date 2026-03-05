//============================================================================
// Name        	: EquitySecMaster.cpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 26, 2021 12:40:04 PM
//============================================================================

#include <KT01/SecDefs/EquitySecMaster.hpp>
#include <KT01/Core/CsvSplitter.hpp>
#include <cmath>

using namespace KT01::SECDEF::EQT;

EquitySecMaster::EquitySecMaster() :m_secmaster_sp(this), _loaded(false)
{

	Settings settings;
    settings.Load("Settings.txt", "Settings");

	string dir = settings.getString("Eqt.SecDefDir");
	string file = settings.getString("Eqt.SecDefFile");

	if (dir.length() == 0)
	{
		LogMe("NO SECDEF DIRECTORY IN SETTINGS!!! FATAL!");
		std::exit(1);
	}

	if (!dir.empty() && dir.back() != '/')
	    dir += '/';

	_DIR = dir;
	_FILE = file;
	string fqpath = dir + file;

	LogMe("GOT SECDEF DIR: " + _DIR);
	LogMe("GOT SECDEF FILE: " + _FILE);

	// Load ExchangeMapping before secdefs (so mapping is available during LoadFile)
	try {
		string exchMapFile = settings.getString("Eqt.ExchangeMappingFile");
		if (!exchMapFile.empty()) {
			LoadExchangeMapping(exchMapFile);
		}
	} catch (const std::exception&) {
		LogMe("Eqt.ExchangeMappingFile not set - no exchange mapping will be applied");
	}

	LoadFile(fqpath, EquitySecDef::PRODTYPE::FUT);
}

EquitySecMaster::~EquitySecMaster() {
}

void EquitySecMaster::LogMe(std::string szMsg)
{
	//LWT_LOG_ME(szMsg,"EQTSECDEF",iColor);
    KT01_LOG_INFO(__CLASS__, szMsg);
}

void EquitySecMaster::LoadFile(string filename, EquitySecDef::PRODTYPE prodtype)
{
    int ilines = 0;

    std::ifstream file(filename.c_str());
    if (!file)
    {
        LogMe("SECMASTER FILE NOT FOUND: " + filename);
        std::exit(1);
    }

    LogMe("LOADING FILE: " + filename);

    int iLines = 0;
    std::string line;

    std::unordered_map<string, int> hdrmap;
    std::unordered_map<int, string> colmap;

    while (std::getline(file, line))
    {
        if (iLines == 0)
        {
            // Parse header row
            vector<string> vec = StringSplitter::Split(line, ",");
            iLines++;

            int icol = 0;
            for (const auto& v : vec)
            {
                hdrmap[v] = icol;
                colmap[icol] = v;
                icol++;
            }

            continue;
        }

        // Skip empty lines or comment lines (starting with '#')
        std::size_t pos = line.find("#");
        if (line.empty() || pos == 0)
        {
            iLines++;
            continue;
        }

        try
        {
            // Remove quotes
            //line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());

            // Do not remove quotes and do not split when comma is within quotes like this: "Heron Therapeutics, Inc."
            vector<std::string> vec = CsvSplitter::Split(line);

            // Validate line length
            if (vec.size() != hdrmap.size())
            {
                LogMe("Skipping line due to column mismatch: " + line);
                continue;
            }

            // Parse fields
            if (vec[hdrmap["min_price_increment"]].compare("C") == 0)
                continue;

            EquitySecDef def = {};
            def.exch = "EQT";
            def.subexch = vec[hdrmap["security_exchange"]];

            //exchange_symbol

            /*def.channel = stoi(vec[hdrmap["ApplID"]]);

            def.msgw = checkInt(vec[hdrmap["MarketSegmentID"]]);
            if (def.msgw == 0)
            	continue;


            def.subexch = vec[hdrmap["SecurityExchange"]];
            def.product = vec[hdrmap["Asset"]];*/
            //vec[hdrmap["security_type"]];
            if (vec[hdrmap["security_type"]] == "EQT")
                def.prodtype = KOrderProdType::EQT;
            def.symbol = vec[hdrmap["symbol"]];
            def.productCode = vec[hdrmap["product_code"]];

             // Extract venue and baseSymbol from symbol (e.g., AAPL@INET)
            size_t atPos = def.symbol.find('@');
            if (atPos != string::npos) {
                def.baseSymbol = def.symbol.substr(0, atPos);      // AAPL
                def.venue = def.symbol.substr(atPos + 1);           // INET
            } else {
                def.baseSymbol = def.symbol;  // Legacy format without @
                def.venue = "";
            }

            // Enrich with ExchangeMapping: add citriusQuoteSource and fixRoute
            if (!def.venue.empty()) {
                auto it = _exchangeMapping.find(def.venue);
                if (it != _exchangeMapping.end()) {
                    def.citriusQuoteSource = it->second.citriusQuoteSource;
                    def.fixRoute = it->second.fixRoute;
                }
            }

			string exchsym = vec[hdrmap["exchange_symbol"]];
            // Check if exchsym starts with "Nasdaq" (case-insensitive)
            if (exchsym.length() >= 6) {
                string prefix = exchsym.substr(0, 6);
                std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);
                if (prefix == "nasdaq") {
                    def.exchangeSymbol = "NASDAQ";
                }
                else {
                    def.exchangeSymbol = exchsym;
                }
            }
            else {
                def.exchangeSymbol = exchsym;
            }
			//LogMe("LOADING SECDEF SYMBOL: " + def.symbol + " EXCHSYM: " + def.exchangeSymbol);


            //def.prodtype = processProdType(def.symbol, def.prodtype);

            def.secid = checkInt(vec[hdrmap["security_id"]]);

            /*if (def.secid == 0)
            	continue;


            def.mmy = checkInt(vec[hdrmap["MaturityMonthYear"]]);*/


            if (def.subexch.length() > 4) {
            	def.subexch.resize(4); // Trims the string to the first 4 characters
			}


            if (hdrmap.find("Leg1MMY") != hdrmap.end())
            {
                def.mmy = stoi(vec[hdrmap["Leg1MMY"]]);
            }

            if (hdrmap.find("Leg2MMY") != hdrmap.end())
            {
                def.mmy2 = stoi(vec[hdrmap["Leg2MMY"]]);
            }

            if (hdrmap.find("min_price_increment") != hdrmap.end())
			{
				def.mintick = checkFloat(vec[hdrmap["min_price_increment"]]);
			}

            if (hdrmap.find("display_factor") != hdrmap.end())
            {
                def.displayFactor = checkFloat(vec[hdrmap["display_factor"]]);

                if (def.displayFactor == 0.0) { // Prevent division by zero
                    def.displayFactor = 1.0;
                }

				def.priceFactor = std::round(1.0 / def.displayFactor);
            }

            



            ////here, we solve for the fact that spreads contain a blank field here. We will substitute 0.
            //if (hdrmap.find("MinPriceIncrementAmount") != hdrmap.end())
            //{
            //    def.mult = checkFloat(vec[hdrmap["MinPriceIncrementAmount"]]);
            //}

//           if (def.symbol == "ESH6-ESU6")
//		   {
//        	   cout << "RAW:" << line << endl;
//        	   cout << def.Print() << endl;
//		   }


            // Store parsed data
            _secidmap[def.secid] = def.symbol;
            _secdef_idmap[def.secid] = def;
            _secdef_symbmap[def.symbol] = def;
            _secdef_prod2id[def.productCode].push_back(def.secid);

            // Store by venue if present
            if (!def.venue.empty()) {
                _secdef_venue2id[def.venue].push_back(def.secid);
            }

            /*if (_prod_segment_map.find(def.product) == _prod_segment_map.end())
                _prod_segment_map[def.product] = def.msgw;*/
        }
        catch (const std::exception& e)
        {
            LogMe("Skipping invalid line: " + line);
            LogMe("Exception: " + std::string(e.what()));
            continue;
        }

        iLines++;
    }

    file.close();

    std::ostringstream oss;
    oss << filename << " LOADED " << iLines << " SECDEFS FROM FILE";
    LogMe(oss.str());

    _loaded = true;
}

void EquitySecMaster::LoadExchangeMapping(string filename)
{
    std::ifstream file(filename.c_str());
    if (!file)
    {
        LogMe("EXCHANGE MAPPING FILE NOT FOUND: " + filename + " (continuing without mapping)");
        return;
    }

    LogMe("LOADING EXCHANGE MAPPING: " + filename);

    int iLines = 0;
    std::string line;
    std::unordered_map<string, int> hdrmap;

    while (std::getline(file, line))
    {
        // Strip trailing \r for Windows line endings
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (iLines == 0)
        {
            // Parse header row (use CsvSplitter for consistency with data rows)
            vector<string> vec = CsvSplitter::Split(line);
            int icol = 0;
            for (const auto& v : vec)
            {
                hdrmap[v] = icol;
                icol++;
            }
            iLines++;
            continue;
        }

        if (line.empty() || line[0] == '#')
        {
            iLines++;
            continue;
        }

        try
        {
            vector<string> vec = CsvSplitter::Split(line);

            if (vec.size() != hdrmap.size())
            {
                LogMe("Skipping exchange mapping line due to column mismatch (expected " + std::to_string(hdrmap.size()) + ", got " + std::to_string(vec.size()) + "): " + line);
                continue;
            }

            ExchangeMappingEntry entry;
            entry.secDefName = vec[hdrmap["SecDefName"]];
            entry.exchange = vec[hdrmap["Exchange"]];
            entry.citriusQuoteSource = vec[hdrmap["CitriusQuoteSource"]];
            entry.fixRoute = vec[hdrmap["FIXRoute"]];
            entry.mic = vec[hdrmap["MIC"]];

            _exchangeMapping[entry.secDefName] = entry;
            _cqs2venue[entry.citriusQuoteSource] = entry.secDefName;

            LogMe("  MAPPED: " + entry.secDefName + " -> CQS=" + entry.citriusQuoteSource + " FIXROUTE=" + entry.fixRoute);
        }
        catch (const std::exception& e)
        {
            LogMe("Skipping invalid exchange mapping line: " + line);
            LogMe("Exception: " + std::string(e.what()));
            continue;
        }

        iLines++;
    }

    file.close();

    std::ostringstream oss;
    oss << filename << " LOADED " << iLines - 1 << " EXCHANGE MAPPINGS";
    LogMe(oss.str());
}

//void CmeSecMaster::LoadOptionsFile(string filename, CmeSecDef::PRODTYPE prodtype)
//{
//	int ilines = 0;
////DBG
//	std::ifstream  file(filename.c_str());
//	if (!file)
//	{
//		LogMe("FILE NOT FOUND: " + filename,LOG_RED);
//		return;
//	}
////DBG
//
//	LogMe("LOADING ***OPTIONS*** FILE: " + filename, LOG_CYAN);
//
//	//int iLoop = 0;
//	int iCnt = 0;
//	int iLines = 0;
//	std::string line;
//
//	tr1::unordered_map<string, int> hdrmap;
//	tr1::unordered_map<int, string> colmap;
//
//	while (std::getline(file, line))
//	{
//		if (iLines == 0)
//		{
//
//			vector<string> vec = StringSplitter::Split(line, ",");
//			iLines++;
//
//			int icol = 0;
//			for (auto v : vec)
//			{
//				hdrmap[v] = icol;
//				colmap[icol] = v;
//				icol++;
//			}
//
//			continue;
//		}
//
//
//		std::size_t pos = line.find("#");
//
//		//cout << line << endl;
//
//
//
//		if (pos != 0)
//		{
//			//cout << line << endl;
//
//			line.erase(std::remove( line.begin(), line.end(), '\"' ),line.end());
//			vector<std::string> vec = StringSplitter::Split(line, ",");
//			iLines++;
//
//			try
//			{
//
//				string secidtest = vec[hdrmap["GBXAlias"]];
//				if (secidtest.length() == 0 || secidtest.compare("") == 0 )
//					continue;
//
//				if (vec[hdrmap["MinPxIncr"]].compare("C") == 0)
//					continue;
//
//				CmeSecDef def = {};
//				def.exch = "CME";
//				def.subexch = vec[hdrmap["UndlyExch"]];
//
//				def.product = vec[hdrmap["SeriesSym"]];
//				def.prodtype = vec[hdrmap["SeriesSecTyp"]];
//				def.pt = prodtype;
//				def.symbol = vec[hdrmap["ITCAlias"]];
//
//
//				def.mmy = stoi(vec[hdrmap["UndlyMMY"]]);
//				def.mintick = stof(vec[hdrmap["SeriesMinPxIncr"]]);
//				def.secid = stoi(vec[hdrmap["GBXAlias"]]);
//
//
//
//
//				if (hdrmap.find("SeriesMult") != hdrmap.end())
//				{
//					def.mult = stof(vec[hdrmap["Mult"]]);
//				}
//
//				if (hdrmap.find("FracPxPrec") != hdrmap.end())
//				{
//					def.fractional = (vec[hdrmap["FracPxPrec"]].compare("Y") == 0) ? true : false;
//				}
//
//				//options specific
//				def.strike = stof(vec[hdrmap["InstrmtStrkPx"]]);
//				def.putcall = stoi(vec[hdrmap["InstrmtPutCall"]]);
//				def.underlying = vec[hdrmap["UndlyID"]];
//				def.expdate = vec[hdrmap["SeriesMatDt"]];
//
//				def.series = vec[hdrmap["SeriesSym"]];
//
//				_secidmap[def.secid] = def.symbol;
//				_secdef_idmap[def.secid] = def;
//				_secdef_symbmap[def.symbol] = def;
////DBG
//			}
//			catch(const std::exception & e)
//			{
//				cout << "EXCEPTION : " << e.what() << endl;
//				cout << vec[hdrmap["ITCAlias"]] <<  " MMY=" << vec[hdrmap["MMY"]] << " MINTICK=" << vec[hdrmap["MinPxIncr"]] << " SECID=" << vec[hdrmap["GBXAlias"]] << endl;
//			}
//
//		}
//	} //end if pos != 0; searching for # as comments
//
//
//	file.close();
////DBG
//	ostringstream oss;
//	oss << filename << " LOADED " << iLines << " SECDEFS FROM FILE";
//	LogMe(oss.str(),LOG_GREEN);
//
//	_loaded = true;
//
//}

