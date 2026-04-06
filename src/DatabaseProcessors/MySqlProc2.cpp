//============================================================================
// Name        : MySqlProc2.cpp
// Description : Slim MySQL processor for OSE_Exchange (DailyStats only)
// Copyright   : Copyright (C) 2017-2024 Katana Financial
//============================================================================

// mysql.h MUST be included before any project headers that bring in KTN::NET
// (mysql.h defines 'typedef struct NET NET' which conflicts with namespace KTN::NET)
#include <mysql/mysql.h>

#include <DatabaseProcessors/MySqlProc2.hpp>
#include <DatabaseProcessors/SqlGen/DailyStatSqlGen.hpp>
#include <Settings/DatabaseSettings.hpp>
#include <KT01/Core/Log.hpp>

#include <sstream>
#include <unistd.h>

MySqlProc2::MySqlProc2() : conn(nullptr), DAILY_STAT_COUNTER(0)
{
}

MySqlProc2::~MySqlProc2()
{
}

void MySqlProc2::LogMe(std::string szMsg)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void MySqlProc2::Start()
{
	DAILY_STAT_COUNTER = 0;
	ConnectMySql();
}

void MySqlProc2::Stop()
{
	DisconnectMySql();
}

void MySqlProc2::ConnectMySql()
{
	const std::string DATAFILE = "DbSettings.txt";
	DatabaseSettings settings;
	settings.Init(DATAFILE);

	const char* server = settings.Host.c_str();
	const char* user = settings.User.c_str();
	const char* password = settings.Password.c_str();
	const char* database = settings.Db.c_str();

	conn = mysql_init(NULL);

	bool reconnect = true;
	if (mysql_options(conn, MYSQL_OPT_RECONNECT, &reconnect)) {
		LogMe("mysql_options() failed");
		mysql_close(conn);
		conn = nullptr;
		return;
	}

	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
		char buff[255];
		sprintf(buff, "%s", mysql_error(conn));
		LogMe(buff);
		return;
	}

	char buff[255];
	sprintf(buff, "DB Connected to %s", server);
	LogMe(buff);
}

void MySqlProc2::DisconnectMySql()
{
	if (conn) {
		mysql_close(conn);
		conn = nullptr;
		LogMe("DATABASE CONNECTION CLOSED");
	}
}

void MySqlProc2::ExecuteSQL(std::string szStmt)
{
	if (!conn) return;
	if (mysql_query(conn, szStmt.c_str())) {
		char buff[2222];
		sprintf(buff, "%s", mysql_error(conn));
		LogMe(buff);
		LogMe(szStmt);
	}
	usleep(500);
}

void MySqlProc2::ProcessDailyStats(const std::vector<DailyStat>& stats, std::string sztable)
{
	std::vector<std::string> stmts = DailyStatSqlGen::GenStatements(stats, sztable);

	for (auto& stmt : stmts)
	{
		DAILY_STAT_COUNTER++;
		ExecuteSQL(stmt);
	}

	if (DAILY_STAT_COUNTER % 1000 == 0 || DAILY_STAT_COUNTER < 100)
	{
		std::ostringstream oss;
		oss << "DB UPDATED " << DAILY_STAT_COUNTER << " TOTAL DAILY STATS.";
		LogMe(oss.str());
	}
}
