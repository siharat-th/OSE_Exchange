//============================================================================
// Name        	: CfeSecMaster.cpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 26, 2021 12:40:04 PM
//============================================================================

#include <KT01/SecDefs/CfeSecMaster.hpp>


using namespace KT01::SECDEF::CFE;

CfeSecMaster::CfeSecMaster() :m_secmaster_sp(this), _loaded(false)
{
    KT01_LOG_INFO("CFE_SECMASTER", "CfeSecMaster Constructor");
	Settings settings;
	settings.Load("Settings.txt", "Settings");

	string dir = settings.getString("Cfe.SecDefDir");
    string compfile = settings.getString("Cfe.SecDefCompFile");

	if (dir.length() == 0)
	{
        KT01_LOG_CRITICAL("CFE_SECMASTER", "NO SECDEF DIRECTORY IN SETTINGS!!! FATAL!");
		std::exit(1);
	}

	if (!dir.empty() && dir.back() != '/')
	    dir += '/';
        
	_DIR = dir;
    _COMPFILE = compfile;
	
    if (compfile.empty())
    {
        KT01_LOG_CRITICAL("CFE_SECMASTER", "NO SECDEF FILES IN SETTINGS!!! FATAL!");
        std::exit(1);
    }

    //in this version, we've used our python script to combine the two files
    //and normalize the symnbols
    string fqpath1 = dir + compfile;
    LogMe("GOT SECDEF PATH: " + fqpath1);
	LoadFile(fqpath1);
}

CfeSecMaster::~CfeSecMaster() {
}

void CfeSecMaster::LogMe(std::string szMsg)
{
    KT01_LOG_INFO("CFE_SECMASTER", szMsg);
}

void CfeSecMaster::LoadFile(string filename)
{
    std::ifstream file(filename.c_str());
    if (!file)
    {
        KT01_LOG_ERROR("CFE_SECMASTER","SECMASTER FILE NOT FOUND: " + filename);
        std::exit(1);
    }

    LogMe("CFE SECDEFS LOADING FILE: " + filename);

    // 1) Read the header line and map the fields
    std::string line;
    if (!std::getline(file, line)) {
        KT01_LOG_ERROR("CFE_SECMASTER", filename + " IS EMPTY. FATAL.");
        std::exit(1);
    }

    std::unordered_map<std::string, int> hdrmap;
    vector<string> headerFields = StringSplitter::Split(line, ",");
    for (size_t i = 0; i < headerFields.size(); ++i) {
        hdrmap[headerFields[i]] = static_cast<int>(i);
    }


     // 2) Group all data rows by Symbol
    //    Key = Symbol, Value = vector of raw lines
    unordered_map<string, vector<string>> linesBySymbol;

    while (std::getline(file, line))
    {
        // Skip empty lines or comment lines (starting with '#')
        std::size_t pos = line.find("#");
        if (line.empty() || pos == 0)
            continue;
        
        try
        {
            // Remove quotes
            line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());
            vector<std::string> fields = StringSplitter::Split(line, ",");

            
            // Validate line length
            if (fields.size() != hdrmap.size())
                continue;
              
            string symbol = fields[hdrmap["Symbol"]];
            // store this line in the map
            linesBySymbol[symbol].push_back(line);
        }
        catch (const std::exception& e)
        {
            LogMe("Skipping invalid line: " + line);
            LogMe("Exception: " + std::string(e.what()));
            continue;
        }
    }

    file.close();

    //3 parse and load definitions
    for(auto kv : linesBySymbol)
    {
        vector<string> vec = kv.second;

        CfeSecDef def = ReadCompositeDefinition(vec, hdrmap);
        Add(def);
        // if (prodtype == KOrderProdType::SPREAD)
        //     def = ReadComplexDefinition(vec, hdrmap);
        // else
        //     def = ReadSimpleDefinition(vec, hdrmap);
        
       // Add(def);
        // Store parsed data
       // cout << "DEF LOADED: SECID=" << def.secid << " SYMB=" << def.symbol << " Base63=" << def.b63_secid << endl;
       
    }

    std::ostringstream oss;
    oss << filename << " LOADED " << _secidmap.size() << " SECDEFS FROM FILE";
    LogMe(oss.str());

    _loaded = true;
}

void CfeSecMaster::Add(CfeSecDef def)
{
    _secidmap[def.secid] = def.symbol;
    _secdef_idmap[def.secid] = def;
    _secdef_symbmap[def.symbol] = def;
    _secdef_prod2id[def.product].push_back(def.secid);
}

CfeSecDef CfeSecMaster::ReadCompositeDefinition(vector<string> vec, std::unordered_map<std::string, int>& hdrmap)
{
    CfeSecDef def = {};

    //SecId63,Symbol,Product,ExpireDate,ContractDate,Description,TickSize,ContractSize,TestSymbol,LegSymbol,LegRatio,LegSide,Complex,secid,legsecid

    for (auto & rawLine : vec) 
    {
        auto fields = StringSplitter::Split(rawLine, ",");

        string symbol = fields[hdrmap["Symbol"]];
        string product = fields[hdrmap["Product"]];
        string secid63 = fields[hdrmap["SecId63"]];
        string expire_date = fields[hdrmap["ExpireDate"]];
        string contract_date = fields[hdrmap["ContractDate"]];
        string description = fields[hdrmap["Description"]];
        string tick_size = fields[hdrmap["TickSize"]];
        string contract_size = fields[hdrmap["ContractSize"]];
        string test_symbol = fields[hdrmap["TestSymbol"]];

        string complexFlag = fields[hdrmap["Complex"]];
        bool isComplex = ((complexFlag == "TRUE" || complexFlag == "True"));

        def.symbol    = symbol;
        def.prodtype = (isComplex) ? KOrderProdType::SPREAD : KOrderProdType::FUTURE;
        def.product = product;

        def.b63_secid = secid63;
        def.secid     = Base63::Base63Codec::decode(secid63.c_str());
       
        def.contract_size = checkInt(contract_size);
        def.mintick = decodeTickSize(tick_size);
        def.mult = def.contract_size;
        def.description = description;
        def.test_symbol = (test_symbol == "TRUE");
    }

    return def;
}

