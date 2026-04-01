//============================================================================
// Name        	: OseSessionSettings.hpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
//============================================================================

#ifndef SRC_EXCHSUPPORT_OSE_OSESESSIONSETTINGS_HPP_
#define SRC_EXCHSUPPORT_OSE_OSESESSIONSETTINGS_HPP_

#pragma once

#include <string>
#include <vector>
#include <sstream>
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

	// Per-session credentials (loaded from individual .conf files)
	struct SessionCreds
	{
		std::string Username;
		std::string LoginId;
		std::string Password;
		std::string NewPassword;
		std::string GatewayHost;
		int GatewayPort = 0;
		std::string BackupHost;
		int BackupPort = 0;
	};

	// Worker sessions (one per worker thread)
	std::vector<SessionCreds> WorkerSessions;

	// BDX session (separate user)
	SessionCreds BdxSession;

	// Legacy single-worker accessors (backward compat — returns WorkerSessions[0])
	const std::string& Username() const { return WorkerSessions[0].Username; }
	const std::string& LoginId() const { return WorkerSessions[0].LoginId; }
	const std::string& Password() const { return WorkerSessions[0].Password; }
	const std::string& GatewayHost() const { return WorkerSessions[0].GatewayHost; }
	int GatewayPort() const { return WorkerSessions[0].GatewayPort; }

	// BDX accessors
	const std::string& BdxLoginId() const { return BdxSession.LoginId; }
	const std::string& BdxPassword() const { return BdxSession.Password; }
	const std::string& BdxGatewayHost() const { return BdxSession.GatewayHost; }
	int BdxGatewayPort() const { return BdxSession.GatewayPort; }

	static SessionCreds LoadSession(const std::string& path)
	{
		SessionCreds c;
		Settings s = Settings(path);
		c.Username = s.getString("OSE.Username");
		c.LoginId = s.getString("OSE.LoginId");
		c.Password = s.getString("OSE.Password");
		c.NewPassword = s.getString("OSE.NewPassword");
		c.GatewayHost = s.getString("OSE.GatewayHost");
		c.GatewayPort = s.getInteger("OSE.GatewayPort");
		c.BackupHost = s.getString("OSE.BackupHost");
		c.BackupPort = s.getInteger("OSE.BackupPort");
		return c;
	}

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

		// Level 3: worker session credentials
		std::string dir = settings.getString("OSE.ConfigDir");

		// Multi-worker: OSE.WorkerSessions = AKL01.conf,AKL02.conf
		std::string workerSessions = settings.getString("OSE.WorkerSessions");
		if (!workerSessions.empty())
		{
			std::istringstream ss(workerSessions);
			std::string token;
			while (std::getline(ss, token, ','))
			{
				// Trim whitespace
				size_t start = token.find_first_not_of(" \t");
				size_t end = token.find_last_not_of(" \t");
				if (start == std::string::npos) continue;
				token = token.substr(start, end - start + 1);

				WorkerSessions.push_back(LoadSession(dir + token));
			}
		}

		// Fallback: single worker from legacy OSE.SessionFile
		if (WorkerSessions.empty())
		{
			std::string sessfile = settings.getString("OSE.SessionFile");
			WorkerSessions.push_back(LoadSession(dir + sessfile));
		}

		// Clamp WorkerCount to available sessions
		if (WorkerCount > (int)WorkerSessions.size())
			WorkerCount = (int)WorkerSessions.size();

		// Level 3b: BDX session (separate user to avoid forced-login kick)
		std::string bdxfile = settings.getString("OSE.BdxSessionFile");
		if (!bdxfile.empty())
		{
			BdxSession = LoadSession(dir + bdxfile);
		}
		else
		{
			// Fallback: same as worker[0] (will cause forced-login conflict!)
			BdxSession = WorkerSessions[0];
		}
	}
};

#endif /* SRC_EXCHSUPPORT_OSE_OSESESSIONSETTINGS_HPP_ */
