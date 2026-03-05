#ifndef CHANNELS_CFE_CHANNELS_HPP
#define CHANNELS_CFE_CHANNELS_HPP

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

struct Channel
{
    std::string protocol;
    std::string datacenter;
    int unit;
    int port;
    string feed;
    string ip;
    string gap_ip;
};



class CfeChannels{
public:
    CfeChannels();
    ~CfeChannels();

    void Load(std::string filename);

    void Info()
    {
        for (auto& channel : _channels)
        {
            ostringstream oss;
            oss << "Channel: " << channel.protocol << " " << channel.datacenter << ", Unit: " << channel.unit
                      << ", Port: " << channel.port << ", Feed: " << channel.feed
                      << ", IP: " << channel.ip << ", Gap IP: " << channel.gap_ip << std::endl;

            LogMe(oss.str());
        }
    }

    Channel GetChannel(string protocol, string datacenter, int unit, string feed)
    {
        cout << "[DEBUG] GETTING " << protocol << " CHANNEL FOR " << datacenter << " " << unit << " " << feed << endl;
        for (auto& channel : _channels)
        {
            if (channel.protocol == protocol && channel.datacenter == datacenter 
                && channel.unit == unit && channel.feed == feed)
                return channel;
        }

        throw std::runtime_error("Channel not found");
    }

    vector<Channel> GetChannels()
    {
        return _channels;
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
    std::vector<Channel> _channels;
   
};

} // end namespace 

/*-----------------------------------------------*/
#endif /* CHANNELS_CFE_CHANNELS_HPP */