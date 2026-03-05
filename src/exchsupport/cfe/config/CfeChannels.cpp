#include "CfeChannels.hpp"


using namespace KTN::CFE;

CfeChannels::CfeChannels()
{
}

CfeChannels::~CfeChannels()
{
}

void CfeChannels::Load(std::string filename)
{
    std::ifstream file(filename.c_str());
    if (!file)
    {
        KT01_LOG_CRITICAL(__CLASS__,"FILE NOT FOUND: " + filename);
        std::exit(1);
    }

    LogMe("LOADING CHANNEL FILE: " + filename);

    // 1) Read the header line and map the fields
    std::string line;
    if (!std::getline(file, line)) {
        KT01_LOG_CRITICAL(__CLASS__, filename + " IS EMPTY. FATAL.");
        std::exit(1);
    }

    vector<string> headerFields = StringSplitter::Split(line, ",");
    for (size_t i = 0; i < headerFields.size(); ++i) {
        _HDRMAP[headerFields[i]] = static_cast<int>(i);
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

            Channel chan = {};
            chan.protocol = fields[_HDRMAP["protocol"]];
            chan.datacenter = fields[_HDRMAP["datacenter"]];
            chan.unit = checkInt(fields[_HDRMAP["unit"]]);
            chan.port = checkInt(fields[_HDRMAP["port"]]);
            chan.feed = fields[_HDRMAP["feed"]];
            chan.ip = fields[_HDRMAP["realtime_mc"]];
            chan.gap_ip = fields[_HDRMAP["gap_resp_mc"]];

            ostringstream oss;
            oss << chan.protocol << " CHANNEL: " << chan.datacenter << " " << chan.unit << " " << chan.port << " " << chan.feed;
            LogMe(oss.str());

            _channels.push_back(chan);
            
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