//============================================================================
// Name        	: CmeMsgw.cpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 26, 2021 12:40:04 PM
//============================================================================

#include "CmeMsgw.hpp"

using namespace KT01::MSGW;

CmeMsgw::CmeMsgw() : _loaded(false) {


}

CmeMsgw::~CmeMsgw() {
}

void CmeMsgw::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

/**
 * FULLY QUALIFIED Directory must have trailing "/"
 *
 * NOTE: for algos, etc, doesn't matter; we just need the productgwfile....
 */
void CmeMsgw::Load()
{
	Settings settings;
	settings.Load("Settings.txt", "Settings");

	string dir = settings.getString("Cme.MsgwDir");
	string filename = settings.getString("Cme.MsgwFile");

	if (dir.length() == 0)
	{
		LogMe("NO SECDEF DIRECTORY IN SETTINGS!!! FATAL!", LogLevel::ERROR);
		std::exit(1);
	}

	if (!dir.empty() && dir.back() != '/')
		dir += '/';

	string fqpath = dir + filename;

	LogMe("GOT CME MSGW DIR: " + dir, LogLevel::INFO);
	LogMe("GOT CME MSGW FILE: " + filename, LogLevel::INFO);

	//int ilines = 0;

	std::ifstream  file(fqpath.c_str());
	if (!file)
	{
		LogMe("CME GW FILE NOT FOUND: " + fqpath,LogLevel::ERROR);
		std::exit(1);
	}


	//int iCnt = 0;
	int iLines = 0;
	std::string line;

	std::unordered_map<string, int> hdrmap;
	std::unordered_map<int, string> colmap;

	while (std::getline(file, line))
	{
		if (iLines == 0)
		{
			vector<string> vec = StringSplitter::Split(line, ",");
			iLines++;

			int icol = 0;
			for (auto v : vec)
			{
				//cout << "HDRMAP " << v << " = " << icol << endl;
				hdrmap[v] = icol;
				colmap[icol] = v;
				icol++;
			}

			continue;
		}


		std::size_t pos = line.find("#");

		if (pos != 0)
		{
			//cout << line << endl;

			line.erase(std::remove( line.begin(), line.end(), '\"' ),line.end());
			line.erase(std::remove( line.begin(), line.end(), '\n' ),line.end());
			line.erase(std::remove( line.begin(), line.end(), '\r' ),line.end());

			line.erase(std::remove( line.begin(), line.end(), ';' ),line.end());

			vector<std::string> vec = StringSplitter::Split(line, ",");
			iLines++;

			try
			{

				Msgw gw = {};
				gw.description = vec[MsgwFields::Description];
				gw.segid = stoi(vec[MsgwFields::SegID]);
				gw.primip = vec[MsgwFields::PrimaryIP];
				gw.secip = vec[MsgwFields::SecondaryIP];

#ifdef SIMULATOR_TEST
				LogMe("********* SIMULATOR TEST ***************",LogLevel::WARN);
				gw.primip = "127.0.0.1";
				gw.secip = gw.primip;
				LogMe("LOADED MSGW: " + gw.Print());
#endif

				_gwmap[gw.segid] = gw;

				//LogMe("LOADED MSGW: " + gw.Print());

			}
			catch(const std::exception & e)
			{
				cout << "EXCEPTION : " << e.what() << endl;
				cout << vec[hdrmap["ITCAlias"]] <<  " MMY=" << vec[hdrmap["MMY"]] << " MINTICK=" << vec[hdrmap["MinPxIncr"]] << " SECID=" << vec[hdrmap["GBXAlias"]] << endl;
				std::exit(1);
			}

		}
	} //end if pos != 0; searching for # as comments


	file.close();
//DBG
//	ostringstream oss;
//	oss << filename << " LOADED " << iLines << " MSGW DEFS FROM FILE";
//	LogMe(oss.str(),LogLevel::INFO);


	LoadProductGatewayFile();

	_loaded = true;

}

void CmeMsgw::LoadProductGatewayFile()
{
	Settings settings;
	settings.Load("Settings.txt", "Settings");

	string dir = settings.getString("Cme.MsgwDir");
	string filename = settings.getString("Cme.MsgwProductFile");

	if (dir.length() == 0)
	{
		LogMe("NO SECDEF DIRECTORY IN SETTINGS!!! FATAL!", LogLevel::ERROR);
		std::exit(1);
	}

	if (!dir.empty() && dir.back() != '/')
		dir += '/';

	string fqpath = dir + filename;

	LogMe("GOT CME MSGW PRODUCTS DIR: " + dir, LogLevel::INFO);
	LogMe("GOT CME MSGW PRODUCTS FILE: " + fqpath, LogLevel::INFO);

	//int ilines = 0;

	std::ifstream  file(fqpath.c_str());
	if (!file)
	{
		LogMe("PRODGW FILE NOT FOUND: " + fqpath,LogLevel::ERROR);
		std::exit(1);
	}

	//int iCnt = 0;
	int iLines = 0;
	std::string line;

	std::unordered_map<string, int> hdrmap;
	std::unordered_map<int, string> colmap;

	while (std::getline(file, line))
	{
		if (iLines == 0)
		{
			vector<string> vec = StringSplitter::Split(line, ",");
			iLines++;

			int icol = 0;
			for (auto v : vec)
			{
				hdrmap[v] = icol;
				colmap[icol] = v;
				icol++;
			}

			continue;
		}

		std::size_t pos = line.find("#");

		//cout << line << endl;

		if (pos != 0)
		{
			//cout << line << endl;

			line.erase(std::remove( line.begin(), line.end(), '\"' ),line.end());
			line.erase(std::remove( line.begin(), line.end(), '\r' ),line.end());
			line.erase(std::remove( line.begin(), line.end(), '\n' ),line.end());
			vector<std::string> vec = StringSplitter::Split(line, ",");
			iLines++;

			try
			{
				string product = vec[0];
				int segid = stoi(vec[1]);
				string prodtype = vec[2];

				if (prodtype.compare("FUT") == 0)
				{
					_prodgw[product] = segid;
				}


				if (prodtype.compare("OOF") == 0)
				{
					_opts_prodgw[product] = segid;
				}

			}
			catch(const std::exception & e)
			{
				cout << "EXCEPTION : " << e.what() << endl;
				cout << vec[hdrmap["ITCAlias"]] <<  " MMY=" << vec[hdrmap["MMY"]] << " MINTICK="
						<< vec[hdrmap["MinPxIncr"]] << " SECID=" << vec[hdrmap["GBXAlias"]] << endl;
				std::exit(1);
			}

		}
	} //end if pos != 0; searching for # as comments


	file.close();
//DBG
//	ostringstream oss;
//	oss << filename << " LOADED " << iLines << " PRODCT GATEWAY DEFS FROM FILE " + filename << " FUTS=" << _prodgw.size() << " OPTS=" << _opts_prodgw.size();
//	LogMe(oss.str(),LogLevel::INFO);

	_loaded = true;

}


void CmeMsgw::Print()
{

	LogMe("*** MSGW DEFINITIONS ****", LogLevel::INFO);
	for (auto sd : _gwmap)
	{
		Msgw def = sd.second;

		LogMe(def.Print(), LogLevel::INFO);

	}

	LogMe("*** FUTURES PRODUCT GATEWAYS ****",LogLevel::INFO);
	for (auto x : _prodgw)
	{
		ostringstream oss;
		oss << x.first << "=" << x.second;
		LogMe(oss.str(), LogLevel::INFO);

	}

	LogMe("*** OPTIONS PRODUCT GATEWAYS ****",LogLevel::INFO);
	for (auto x : _opts_prodgw)
	{
		ostringstream oss;
		oss << x.first << "=" << x.second;
		LogMe(oss.str(), LogLevel::INFO);

	}

}
