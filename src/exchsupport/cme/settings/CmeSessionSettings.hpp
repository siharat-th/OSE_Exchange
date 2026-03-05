//============================================================================
// Name        	: CmeSessionSettings.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 20, 2023 4:36:53 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_CMESESSIONSETTINGS_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_CMESESSIONSETTINGS_HPP_

#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <stdint.h>
#include <string>
#include <KT01/Core/Settings.hpp>
#include <ExchangeHandler/session/SessionSettings.hpp>
using namespace std;

/**
 * @brief Represents the settings for a CME session.
 */
struct CmeSessionSettings : SessionSettings
{
	std::string TraderId;
	string SessionId;
	string FirmId;
	string AccessKey;
	string SecretKey;
	uint64_t SelfMatchId;
	int Port;
	int BackupPort;


	int PartyListId;
	int KeepAliveSecs;
	bool FaultTolerance;

	std::string Source;
	std::string ExchName;
	std::string Org;
	std::string Account;
	int CustOrdCapacity;

	std::string Location;

	std::string Tag50_Manual;
	std::string Tag50_Auto;

	std::string SystemName;
	std::string SystemVersionId;
	std::string SystemVendor;

	bool UseFeed_A;
	int Affinity;

	string Filename;

	/**
  * @brief Loads the session settings from the specified file.
  * @param filename The name of the file containing the session settings.
  */
	void Load(const string& filename)
	{
		Filename = filename;

		Settings settings = Settings(filename);

		Org = settings.getString("IL3.Org");
		ExchName = settings.getString("Exchange.Name");
		Account = settings.getString("IL3.Account");
		CustOrdCapacity = settings.getInteger("IL3.CustOrdCapacity");

		Tag50_Manual = settings.getString("IL3.Manual");
		Tag50_Auto = settings.getString("IL3.Automated");
		Location = "US";//settings.getString("IL3.Location");
		KeepAliveSecs = settings.getInteger("IL3.KeepAliveInterval");

		PartyListId = settings.getInteger("IL3.PartyListId");
		SystemName = settings.getString("IL3.SystemName");
		SystemVersionId = settings.getString("IL3.SystemVersionId");
		SystemVendor = settings.getString("IL3.SystemVendor");

		UseFeed_A = settings.getBoolean("IL3.UseFeedA");
		Affinity = settings.getInteger("IL3.SendingThreadAffinity");

		FaultTolerance = settings.getBoolean("IL3.FaultTolerance");

		string dir = settings.getString("IL3.ConfigDir");
		string sessfile = settings.getString("IL3.SessionFile");
		Settings sess = Settings((dir + sessfile));

		TraderId = sess.getString("IL3.TraderId");
		SecretKey = sess.getString("IL3.SecretKey");
		AccessKey = sess.getString("IL3.AccessKey");
		FirmId = sess.getString("IL3.FirmId");
		SessionId = sess.getString("IL3.SessionId");
		SelfMatchId = sess.getLong("IL3.SelfMatchId");
		Port = sess.getInteger("IL3.Port");
		BackupPort = sess.getInteger("IL3.BackupPort");

	}
};

#endif /* SRC_EXCHSUPPORT_CME_IL3_CMESESSIONSETTINGS_HPP_ */
