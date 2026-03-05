#ifndef SRC_KT01_NET_TCP_TCPHANDLERFACTORY_HPP_
#define SRC_KT01_NET_TCP_TCPHANDLERFACTORY_HPP_

#pragma once

#include <KT01/Core/Settings.hpp>
#include <KT01/Core/Log.hpp>
using namespace KT01::Core;

#include <KT01/Net/Tcp/TcpHandlerInterface.hpp>
#include <KT01/Net/Tcp/EFVITcpV5.hpp>
#include <KT01/Net/Tcp/StandardTcpV1.hpp>

namespace KT01::NET::TCP {

class TcpHandlerFactory {
public:
	static TcpHandlerInterface* Create(string settingsfile, TcpCallback8t* cb)
    {
        Settings mainsett = Settings(settingsfile);
        int version = mainsett.getInteger("TcpVersion");
        if (version == 0)
        {
            KT01_LOG_ERROR("TcpHandlerFactory", "TCP VERSION NOT SET, DEFAULTING TO 1");
            version = 1;
        }
        //let's instantiate the TCP handler based on our config
        switch (version)
        {
        case 1:
            return new StandardTcpV1(cb);
        case 5:
            return new EFVITcpV5(cb);
        default:
            KT01_LOG_CRITICAL("TcpHandlerFactory", "TCP VERSION NOT SET, CRITICAL ERROR!!! EXIT.");
            exit(1);
        }
    }

    static TcpHandlerInterface* Create(TcpCallback8t* cb)
    {
        return new StandardTcpV1(cb);
    }
};

} // namespace KT01::NET::TCP

#endif /* SRC_KT01_NET_TCP_TCPHANDLERFACTORY_HPP_ */
