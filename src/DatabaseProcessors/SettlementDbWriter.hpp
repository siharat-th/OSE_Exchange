//============================================================================
// Name        : SettlementDbWriter.hpp
// Description : Writes settlement DailyStats to MySQL
// Copyright   : Copyright (C) 2026 Katana Financial
//============================================================================

#ifndef SRC_DATABASEPROCESSORS_SETTLEMENTDBWRITER_HPP_
#define SRC_DATABASEPROCESSORS_SETTLEMENTDBWRITER_HPP_

#pragma once

#include <vector>
#include <string>
#include <ctime>

#include <KT01/Core/Log.hpp>
#include <DatabaseProcessors/MySqlProc2.hpp>

namespace KTN::OSE {

class SettlementDbWriter
{
public:
	SettlementDbWriter() : _started(false) {}

	/// Convert YYYYMMDD string → days since Unix epoch
	static uint32_t BizDateToDaysSinceEpoch(const char* yyyymmdd)
	{
		struct tm t = {};
		t.tm_year = ((yyyymmdd[0] - '0') * 1000 + (yyyymmdd[1] - '0') * 100 +
		             (yyyymmdd[2] - '0') * 10   + (yyyymmdd[3] - '0')) - 1900;
		t.tm_mon  = ((yyyymmdd[4] - '0') * 10 + (yyyymmdd[5] - '0')) - 1;
		t.tm_mday = (yyyymmdd[6] - '0') * 10 + (yyyymmdd[7] - '0');
		t.tm_hour = 12; // noon to avoid DST edge cases
		time_t epoch = timegm(&t);
		return static_cast<uint32_t>(epoch / 86400);
	}

	/// Write pre-built DailyStat vector to DB
	void WriteDailyStats(const std::vector<DailyStat>& stats, const std::string& table)
	{
		if (stats.empty())
			return;

		if (!_started)
		{
			_dbproc.Start();
			_started = true;
		}

		_dbproc.ProcessDailyStats(stats, table);

		KT01_LOG_INFO("SettlementDbWriter", "Settlement DB write complete (" +
		              std::to_string(stats.size()) + " rows, table=" + table + ")");
	}

private:
	MySqlProc2 _dbproc;
	bool _started;
};

} // namespace KTN::OSE

#endif /* SRC_DATABASEPROCESSORS_SETTLEMENTDBWRITER_HPP_ */
