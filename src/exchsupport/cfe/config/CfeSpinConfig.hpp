#ifndef CHANNELS_CFE_SPINCONFIG_HPP
#define CHANNELS_CFE_SPINCONFIG_HPP

#include<string>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>
#include <KT01/Core/Settings.hpp>

namespace KTN::CFE
{

struct SpinSettings
{
    std::string type;
    std::string datacenter;
    std::string unit;
    string ip;
    string secondary_ip;
    int port;
    string username;
    string password;
    string subid;
    string set_name;
};

class CfeSpinConfig
{
public:
CfeSpinConfig();
    ~CfeSpinConfig();

    void Load(std::string filename);

    void Info()
    {
        for (auto& config : _configs)
        {
            ostringstream oss;
            oss << "SpinConfig: " << config.type << ", Datacenter: " << config.datacenter
                << ", Unit: " << config.unit << ", IP: " << config.ip
                << ", Secondary IP: " << config.secondary_ip << ", Port: " << config.port
                << ", Username: " << config.username << ", Password: " << config.password
                << ", SubId: " << config.subid;
            LogMe(oss.str());
        }
    }

    SpinSettings GetChannel(string datacenter, string unit)
    {
        cout << "[DEBUG] GETTING SPIN CONFIG FOR " << datacenter << " " << unit << endl;
        for (auto& config : _configs)
        {
            if (config.datacenter == datacenter && config.unit == unit)
                return config;
        }

        throw std::runtime_error("Channel not found");
    }

    vector<SpinSettings> GetSpins()
    {
        return _configs;
    }

private:
    void LogMe(std::string szMsg)
    {
        KT01_LOG_INFO(__CLASS__, szMsg);
    }

    inline int checkInt(const string& valueStr)
    {
        int value = 0; // Default value for empty fields

        if (!valueStr.empty())
            value = std::stoi(valueStr);

        return value;
    }

private:
    std::unordered_map<std::string, int> _HDRMAP;
    std::vector<SpinSettings> _configs;
   
};

} // end namespace 

/*-----------------------------------------------*/
#endif /* CHANNELS_CFE_CHANNELS_HPP */