//============================================================================
// Name        	: CfeSessionSettings.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 20, 2023 4:36:53 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CFE_BOE3_BOE3SETTINGS_HPP_
#define SRC_EXCHSUPPORT_CFE_BOE3_BOE3SETTINGS_HPP_

#include <KT01/Core/Log.hpp>
#include <KT01/Core/Settings.hpp>
#include <cstdint> // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring> // For string operations (e.g., strncpy)
#include <exchsupport/cfe/settings/CfeSessionSettings.hpp>
#include <stdint.h>
#include <string>

/**
 * @brief Represents the settings for a CFE session.
 */
struct CfeBoe3Settings
{
	// common settings
	std::string Org;	  /**< The organization of the firm. */
	std::string ExchName; /**< The name of the exchange. */
	std::string Source;	  /**< The source of the firm. */

	// Order Entry Settings
	std::string Account;  /**< The account of the firm. */
	std::string FirmId;	  /**< The ID of the firm. */
	std::string Capacity; /**< The capacity of the firm.  C=Customer; F=Firm*/
	std::string CTI;	  /**< The CTI code of the firm. valid values: 1-4 */

	std::string PreventMatchInstruction; /**< The prevent match instruction. */
	std::string FrequentTraderId;		 /**< The frequent trader ID. */

	std::string Tag50_Manual; /**< The manual tag 50. */
	std::string Tag50_Auto;	  /**< The automated tag 50. */
	struct Tag50Pair
	{
		const std::string user;
		const std::string tag50;
	};
	std::vector<Tag50Pair> Tag50List;

	std::string SessionDir;	 /**< The directory for the session. */
	std::string SessionFile; /**< The file for the session. */
	bool UseFeed_A;			 /**< Flag to indicate whether to use Feed A. */
	int Affinity;			 /**< The thread affinity. */
	std::string Filename;	 /**< The name of the settings file. */

	CfeSessionSettings SessionSettings; /**< The session settings. */

	// general settings
	long PollerMax;
	bool MeasureLatency; /**< Flag to indicate whether to measure latency. */

	bool LogMessagesToScreen; /**< Flag to indicate whether to log messages to the screen. */
	bool UseAudit;			  /**< Flag to indicate whether to use audit trail. */
	int AuditCore;			  /**< The core for the audit trail. */
	std::string AuditDir;	  /**< The directory for the audit trail. */

	std::string StateFileDir; /**< The directory for the state file. */

	bool DebugRecvBytes;
	bool DebugSession;
	bool DebugAppMsgs;
	bool DebusSessMsgs;
	bool DebugGapDetection;

	/**
	 * @brief Loads the session settings from the specified file.
	 * @param filename The name of the file containing the session settings.
	 */
	void Load(const std::string &filename)
	{
		Filename = filename;
		Settings settings = Settings(filename);

		ExchName = settings.getString("Exchange.Name");
		Org = settings.getString("BOE3.Org");

		Account = settings.getString("BOE3.Account");
		// FirmId = settings.getString("BOE3.FirmId");
		Capacity = settings.getString("BOE3.Capacity");
		CTI = settings.getString("BOE3.CTI");
		PreventMatchInstruction = settings.getString("BOE3.PreventMatchInstruction");
		FrequentTraderId = settings.getString("BOE3.FrequentTraderId");

		Tag50_Manual = settings.getString("BOE3.Tag50_Manual");
		Tag50_Auto = settings.getString("BOE3.Tag50_Auto");

		const std::string tag50ListStr = settings.getString("BOE3.Tag50_Manual_List");
		const std::vector<std::string> tag50Pairs = StringSplitter::Split(tag50ListStr, ",");
		for (const std::string &pair : tag50Pairs)
		{
			const std::vector<std::string> parts = StringSplitter::Split(pair, "|");
			if (parts.size() == 2)
			{
				Tag50List.push_back({parts[0], parts[1]});
			}
			else
			{
				LOGERROR("CfeBoe3Settings::Load: Invalid Tag50 pair format in BOE3.Tag50_Manual_List: {}", pair);
			}
		}
		UseFeed_A = settings.getBoolean("BOE3.UseFeedA");
		Affinity = settings.getInteger("BOE3.SendingThreadAffinity");

		SessionDir = settings.getString("BOE3.SessionDir");
		SessionFile = settings.getString("BOE3.SessionFile");

		SessionSettings.Load((SessionDir + SessionFile));

		// common load
		LogMessagesToScreen = settings.getBoolean("BOE3.LogMessagesToScreen");
		UseAudit = settings.getBoolean("BOE3.UseAuditTrail");
		AuditCore = settings.getInteger("BOE3.AuditCore");
		AuditDir = settings.getString("BOE3.AuditTrailDirectory");
		StateFileDir = settings.getString("BOE3.StateFileDirectory");

		PollerMax = settings.getLong("BOE3.PollerMax");
		MeasureLatency = settings.getBoolean("BOE3.MeasureLatency");

		DebugRecvBytes = settings.getBoolean("BOE3.DebugRecvBytes");
		DebugSession = settings.getBoolean("BOE3.DebugSession");
		DebugAppMsgs = settings.getBoolean("BOE3.DebugAppMsgs");
		DebusSessMsgs = settings.getBoolean("BOE3.DebugSessMsgs");
		DebugGapDetection = settings.getBoolean("BOE3.DebugGapDetection");
	}
};

#endif /* SRC_EXCHSUPPORT_CFE_BOE3_BOE3SETTINGS_HPP_ */