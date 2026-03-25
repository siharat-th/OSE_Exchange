//============================================================================
// Name        	: OseSessionSettings.hpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
//============================================================================

#ifndef SRC_EXCHSUPPORT_OSE_OSESESSIONSETTINGS_HPP_
#define SRC_EXCHSUPPORT_OSE_OSESESSIONSETTINGS_HPP_

#pragma once

#include <string>
#include <KT01/Core/Settings.hpp>
#include <ExchangeHandler/session/SessionSettings.hpp>

struct OseSessionSettings : SessionSettings
{
	// Exchange config (Level 2 - oseOmnet.conf)
	std::string ExchName;
	std::string Org;
	std::string Source; // Set by OseGT from constructor param (e.g. "ODIN")
	std::string Account;

	bool ForceLogin;
	bool AutoCancellation;
	int KeepAliveInterval;
	bool MeasureLatency;

	int WorkerCount;
	int WorkerAffinity;
	int BdxAffinity;
	int BdxPollIntervalUs;
	int SendingThreadAffinity;

	bool UseAuditTrail;
	std::string AuditTrailDirectory;
	int AuditCore;

	bool UseStateFile;
	std::string StateFileDirectory;
	bool UseStateLog;

	bool DebugSession;
	bool DebugAppMsgs;
	bool DebugRecvBytes;

	// Session credentials (Level 3 - sessions/AKL01.conf)
	std::string Username;
	std::string LoginId;
	std::string Password;
	std::string NewPassword;
	std::string GatewayHost;
	int GatewayPort;
	std::string BackupHost;
	int BackupPort;

	// BDX session credentials (Level 3 - sessions/AKL02.conf, separate user)
	std::string BdxUsername;
	std::string BdxLoginId;
	std::string BdxPassword;
	std::string BdxGatewayHost;
	int BdxGatewayPort;

	void Load(const std::string& filename)
	{
		Filename = filename;

		Settings settings = Settings(filename);

		// Level 2: exchange config
		ExchName = settings.getString("Exchange.Name");
		Org = settings.getString("OSE.Org");
		Account = settings.getString("OSE.Account");

		ForceLogin = settings.getBoolean("OSE.ForceLogin");
		AutoCancellation = settings.getBoolean("OSE.AutoCancellation");
		KeepAliveInterval = settings.getInteger("OSE.KeepAliveInterval");
		MeasureLatency = settings.getBoolean("OSE.MeasureLatency");

		WorkerCount = settings.getInteger("OSE.WorkerCount");
		WorkerAffinity = settings.getInteger("OSE.WorkerAffinity");
		BdxAffinity = settings.getInteger("OSE.BdxAffinity");
		BdxPollIntervalUs = settings.getInteger("OSE.BdxPollIntervalUs");
		SendingThreadAffinity = settings.getInteger("OSE.SendingThreadAffinity");

		UseAuditTrail = settings.getBoolean("OSE.UseAuditTrail");
		AuditTrailDirectory = settings.getString("OSE.AuditTrailDirectory");
		AuditCore = settings.getInteger("OSE.AuditCore");

		UseStateFile = settings.getBoolean("OSE.UseStateFile");
		StateFileDirectory = settings.getString("OSE.StateFileDirectory");
		UseStateLog = settings.getBoolean("OSE.UseStateLog");

		DebugSession = settings.getBoolean("OSE.DebugSession");
		DebugAppMsgs = settings.getBoolean("OSE.DebugAppMsgs");
		DebugRecvBytes = settings.getBoolean("OSE.DebugRecvBytes");

		// Level 3: session credentials
		std::string dir = settings.getString("OSE.ConfigDir");
		std::string sessfile = settings.getString("OSE.SessionFile");
		Settings sess = Settings((dir + sessfile));

		Username = sess.getString("OSE.Username");
		LoginId = sess.getString("OSE.LoginId");
		Password = sess.getString("OSE.Password");
		NewPassword = sess.getString("OSE.NewPassword");
		GatewayHost = sess.getString("OSE.GatewayHost");
		GatewayPort = sess.getInteger("OSE.GatewayPort");
		BackupHost = sess.getString("OSE.BackupHost");
		BackupPort = sess.getInteger("OSE.BackupPort");

		// Level 3b: BDX session (separate user to avoid forced-login kick)
		std::string bdxfile = settings.getString("OSE.BdxSessionFile");
		if (!bdxfile.empty())
		{
			Settings bdxsess = Settings((dir + bdxfile));
			BdxLoginId = bdxsess.getString("OSE.LoginId");
			BdxPassword = bdxsess.getString("OSE.Password");
			BdxGatewayHost = bdxsess.getString("OSE.GatewayHost");
			BdxGatewayPort = bdxsess.getInteger("OSE.GatewayPort");
		}
		else
		{
			// Fallback: same as worker (will cause forced-login conflict!)
			BdxLoginId = LoginId;
			BdxPassword = Password;
			BdxGatewayHost = GatewayHost;
			BdxGatewayPort = GatewayPort;
		}
	}
};

#endif /* SRC_EXCHSUPPORT_OSE_OSESESSIONSETTINGS_HPP_ */
