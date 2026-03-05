//============================================================================
// Name        	: CmeSecMaster.cpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 26, 2021 12:40:04 PM
//============================================================================

#include <KT01/SecDefs/CmeSecMasterV2.hpp>
using namespace KT01::SECDEF::CME;

CmeSecMaster::CmeSecMaster() :/*m_secmaster_sp(this),*/ _loaded(false)
{

	Settings settings;
	settings.Load("Settings.txt", "Settings");

	string dir = settings.getString("Cme.SecDefDir");
	string file = settings.getString("Cme.SecDefFile");

	if (dir.length() == 0)
	{
		LogMe("NO SECDEF DIRECTORY IN SETTINGS!!! FATAL!", LogLevel::ERROR);
		std::exit(1);
	}

	if (!dir.empty() && dir.back() != '/')
	    dir += '/';

	_DIR = dir;
	_FILE = file;
	string fqpath = dir + file;

	LogMe("GOT SECDEF DIR: " + _DIR, LogLevel::INFO);
	LogMe("GOT SECDEF FILE: " + _FILE, LogLevel::INFO);

	LoadFile(fqpath);
}

CmeSecMaster::~CmeSecMaster() {
}

void CmeSecMaster::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO("CMESECDEF", szMsg);
}

void CmeSecMaster::LoadFile(string filename)
{
    //int ilines = 0;

    std::ifstream file(filename.c_str());
    if (!file)
    {
        LogMe("SECMASTER FILE NOT FOUND: " + filename, LogLevel::ERROR);
        std::exit(1);
    }

    LogMe("LOADING FILE: " + filename, LogLevel::INFO);

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
            line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());
            vector<std::string> vec = StringSplitter::Split(line, ",");

            // Validate line length
            if (vec.size() != hdrmap.size())
            {
                LogMe("Skipping line due to column mismatch: " + line, LogLevel::WARN);
                continue;
            }

            // Parse fields
            if (vec[hdrmap["MinPriceIncrement"]].compare("C") == 0)
                continue;

            CmeSecDef def = {};
            def.exch = "CME";
            def.channel = stoi(vec[hdrmap["ApplID"]]);

            def.msgw = checkInt(vec[hdrmap["MarketSegmentID"]]);
			if (def.msgw == 0)
				continue;

            def.subexch = vec[hdrmap["SecurityExchange"]];
            def.product = vec[hdrmap["Asset"]];
            def.secgroup = vec[hdrmap["SecurityGroup"]];

            if (vec[hdrmap["SecurityType"]] == "FUT")
				def.prodtype = KOrderProdType::FUTURE;
			if (vec[hdrmap["SecurityType"]] == "MLEG")
				def.prodtype = KOrderProdType::SPREAD;
			

            def.symbol = vec[hdrmap["Symbol"]];

            def.secid = checkInt(vec[hdrmap["SecurityID"]]);
			if (def.secid == 0) continue;

			def.mmy = checkInt(vec[hdrmap["MaturityMonthYear"]]);
			def.mintick = checkFloat(vec[hdrmap["MinPriceIncrement"]]);

			def.displayFactor = checkFloat(vec[hdrmap["DisplayFactor"]]);
			def.mainFraction = checkInt(vec[hdrmap["MainFraction"]]);
			def.subFraction = checkInt(vec[hdrmap["SubFraction"]]);

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

            if (hdrmap.find("MinPriceIncrementAmount") != hdrmap.end())
            {
            	def.mult = checkFloat(vec[hdrmap["MinPriceIncrementAmount"]]);
            }

            // Store parsed data
            _secidmap[def.secid] = def.symbol;
            _secdef_idmap[def.secid] = def;
            _secdef_symbmap[def.symbol] = def;
            _secdef_prod2id[def.product].push_back(def.secid);

            if (_prod_segment_map.find(def.product) == _prod_segment_map.end())
            	_prod_segment_map[def.product] = def.msgw;

           // cout << "SYMBOL=" << def.symbol << " SEGMENT=" << def.msgw << endl;
        }
        catch (const std::exception& e)
        {
            LogMe("Skipping invalid line: " + line, LogLevel::ERROR);
            LogMe("Exception: " + std::string(e.what()), LogLevel::ERROR);
            continue;
        }

        iLines++;
    }

    file.close();

    std::ostringstream oss;
    oss << filename << " LOADED " << iLines << " SECDEFS FROM FILE";
    LogMe(oss.str(), LogLevel::INFO);

    _loaded = true;
}



//void CmeSecMaster::LoadOptionsFile(string filename, CmeSecDef::PRODTYPE prodtype)
//{
//	int ilines = 0;
////DBG
//	std::ifstream  file(filename.c_str());
//	if (!file)
//	{
//		LogMe("FILE NOT FOUND: " + filename,LogLevel::ERROR);
//		return;
//	}
////DBG
//
//	LogMe("LOADING ***OPTIONS*** FILE: " + filename, LogLevel::INFO);
//
//	//int iLoop = 0;
//	int iCnt = 0;
//	int iLines = 0;
//	std::string line;
//
//	std::unordered_map<string, int> hdrmap;
//	std::unordered_map<int, string> colmap;
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
//	LogMe(oss.str(),LogLevel::INFO);
//
//	_loaded = true;
//
//}
