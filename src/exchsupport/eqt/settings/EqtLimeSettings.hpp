#ifndef SRC_EXCHSUPPORT_EQT_LIME_LIMESETTINGS_HPP_
#define SRC_EXCHSUPPORT_EQT_LIME_LIMESETTINGS_HPP_

#pragma once

#include <cstdint> // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring> // For string operations (e.g., strncpy)
#include <stdint.h>
#include <string>
#include <KT01/Core/Settings.hpp>
using namespace std;

struct EqtLimeSettings
{
    string Filename;;
    string ExchName; 

    string Org;
    string Host;
    int Port;
    string Account;
    string Username;
    string Password;;

	bool Verbose;
	bool LogMessagesToScreen;

	string EventIdDir;
	string EventIdFile;

	string SessionId; // e.g., "AKL"
	string FirmId;   // e.g., "LIME"
	

    void Load(const string& filename)
    {
        Filename = filename;
        Settings settings = Settings(filename);

        LOAD_TYPED_SETTING_OR_DEFAULT(settings, "Exchange.Name", getString, ExchName, "eqt");

        LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.Org", getString, Org, "AKL");
        LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.Host", getString, Host, "74.120.50.45");
        LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.Port", getInteger, Port, 7001);
        LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.Account", getString, Account, "AKLCAPITAL_TEST");
        LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.Username", getString, Username, "AKLCAPITAL");
        LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.Password", getString, Password, "limeAKLCAPITAL");

		LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.Verbose", getBoolean, Verbose, false);
		LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.LogMessagesToScreen", getBoolean, LogMessagesToScreen, false);

		LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.EventIdDir", getString, EventIdDir, "MsgStorage");
		LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.EventIdFile", getString, EventIdFile, "AKL-LIME-EventId");

		LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.SessionId", getString, SessionId, "AKL");
		LOAD_TYPED_SETTING_OR_DEFAULT(settings, "LIME.FirmId", getString, FirmId, "LIME");
    }
};

#endif /* SRC_EXCHSUPPORT_EQT_LIME_LIMESETTINGS_HPP_ */