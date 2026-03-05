//============================================================================
// Name        	: CfeSessionSettings.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 20, 2023 4:36:53 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CFE_BOES_CFESESSIONSETTINGS_HPP_
#define SRC_EXCHSUPPORT_CFE_BOES_CFESESSIONSETTINGS_HPP_

#include <cstdint> // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring> // For string operations (e.g., strncpy)
#include <stdint.h>
#include <string>
#include <KT01/Core/Settings.hpp>
#include <ExchangeHandler/session/SessionSettings.hpp>
using namespace std;

/**
 * @brief Represents the settings for a CFE session.
 */
struct CfeSessionSettings : SessionSettings
{
	// Session settings
	string EFID;
	string SessionId;
	string SubId;
	string Password;
	
	std::string ReplayType; /**< The replay type of the firm. */
	int UnitNbr;			/**< The unit number of the firm. */
	int HeartbeatSecs;

	//Connection Info
	string Host;
	int Port;
	string SecondaryHost;
	bool UsePrimary;
	
	string Filename;

	/**
	 * @brief Loads the session settings from the specified file.
	 * @param filename The name of the file containing the session settings.
	 */
	void Load(const string &filename)
	{
		Filename = filename;
		Settings settings = Settings(filename);

		Host = settings.getString("BOE3.Host");
		SecondaryHost = settings.getString("BOE3.SecondaryHost");
		Port = settings.getInteger("BOE3.Port");

		EFID = settings.getString("BOE3.EFID");
		SessionId = settings.getString("BOE3.SessionId");
		SubId = settings.getString("BOE3.SubId");

		Password = settings.getString("BOE3.Password");
		
		ReplayType = settings.getString("BOE3.ReplayType");
		UnitNbr = settings.getInteger("BOE3.UnitNbr");
		HeartbeatSecs = settings.getInteger("BOE3.HeartbeatSecs");

		UsePrimary = settings.getBoolean("BOE3.UsePrimary");
		
	}
};

#endif /* SRC_EXCHSUPPORT_CME_IL3_CMESESSIONSETTINGS_HPP_ */
