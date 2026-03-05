#include "CfeSpinConfig.hpp"


using namespace KTN::CFE;

CfeSpinConfig::CfeSpinConfig()
{
}

CfeSpinConfig::~CfeSpinConfig()
{
}

void CfeSpinConfig::Load(std::string filename)
{
    std::ifstream file(filename.c_str());
    if (!file)
    {
        KT01_LOG_CRITICAL(__CLASS__,"FILE NOT FOUND: " + filename);
        std::exit(1);
    }

    LogMe("LOADING SPING CONFIG FILE: " + filename);

    // 1) Read the header line and map the fields
    std::string line;
    if (!std::getline(file, line)) {
        KT01_LOG_CRITICAL(__CLASS__, filename + " IS EMPTY. FATAL.");
        std::exit(1);
    }

    vector<string> headerFields = StringSplitter::Split(line, ",");
    for (size_t i = 0; i < headerFields.size(); ++i) {
        _HDRMAP[headerFields[i]] = static_cast<int>(i);
        //NOTE: these fields are in the same order and naming scheme as the CFE spin server sessions file you get from from the exchange
       // cout << "HEADER FIELD: " << headerFields[i] << " = " << _HDRMAP[headerFields[i]] << endl;
    }
    
  
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

            SpinSettings config = {};
            config.datacenter = fields[_HDRMAP["Primary/Redundant"]];
            config.unit = fields[_HDRMAP["Unit"]];
            config.ip = fields[_HDRMAP["IP"]];
            config.secondary_ip = fields[_HDRMAP["DR IP"]];
            config.port = checkInt(fields[_HDRMAP["Port"]]);
            config.username = fields[_HDRMAP["Username"]];
            config.password = fields[_HDRMAP["Password"]];
            config.subid = fields[_HDRMAP["Session Sub Id"]];
            config.type = fields[_HDRMAP["Type"]];

            ostringstream oss;
            oss << "SPIN CONFIG: " << config.datacenter << " UNIT=" << config.unit << " IP=" << config.ip 
            << ":" << config.port << " U=" << config.username << " P=" << config.password << " SUBID=" << config.subid;
            LogMe(oss.str());

            _configs.push_back(config);
            
        }
        catch (const std::exception& e)
        {
            LogMe("Skipping invalid line: " + line);
            LogMe("Exception: " + std::string(e.what()));
            continue;
        }
    }
    
    file.close();
  
}