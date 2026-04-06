//============================================================================
// Name        : MySqlProc2.hpp
// Description : Slim MySQL processor for OSE_Exchange (DailyStats only)
// Copyright   : Copyright (C) 2017-2024 Katana Financial
//============================================================================

#ifndef SRC_DATABASEPROCESSORS_MYSQLPROC2_HPP_
#define SRC_DATABASEPROCESSORS_MYSQLPROC2_HPP_

#include <string>
#include <vector>

#include <DatabaseProcessors/SettlementTypes.hpp>

// Forward-declare MYSQL to avoid pulling mysql.h into every translation unit
// (mysql.h defines 'NET' which conflicts with namespace KTN::NET)
struct MYSQL;

class MySqlProc2
{
public:
	MySqlProc2();
	~MySqlProc2();

	void Start();
	void Stop();

	void ProcessDailyStats(const std::vector<DailyStat>& stats, std::string sztable);

private:
	void LogMe(std::string szMsg);
	void ConnectMySql();
	void DisconnectMySql();
	void ExecuteSQL(std::string szStmt);

	MYSQL* conn;
	int DAILY_STAT_COUNTER;
};

#endif /* SRC_DATABASEPROCESSORS_MYSQLPROC2_HPP_ */
