//============================================================================
// Name        : DailyStatSqlGen.hpp
// Copyright   : Copyright (C) 2017-2024 Katana Financial
//============================================================================

#ifndef SRC_DATABASEPROCESSORS_DAILYSTATSQLGEN_HPP_
#define SRC_DATABASEPROCESSORS_DAILYSTATSQLGEN_HPP_

#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>

using namespace std;

#include <DatabaseProcessors/SettlementTypes.hpp>
#include <DatabaseProcessors/SqlUtils.hpp>

class DailyStatSqlGen
{
public:
	static vector<std::string> GenStatements(const vector<DailyStat>& vec, string table)
	{
		vector<string> stmts;
		int x = 0;
		int BATCH_SIZE = 300;
		std::vector<string> values;

		string m_insertbase = "INSERT INTO " + table + " (";
		m_insertbase += "symbol,secid,price,size,update_type,update_desc,tradedate,rpt_seq,ssboe,update_time";
		m_insertbase += ")VALUES";

		for (auto q : vec)
		{
			std::string vals = GenValues(q);
			values.push_back(vals);

			x++;

			if (x % BATCH_SIZE == 0)
			{
				stmts.push_back(GenQuery(values, m_insertbase));
				values.clear();
				x = 0;
				usleep(100);
			}
		}

		if (values.size() > 0)
		{
			stmts.push_back(GenQuery(values, m_insertbase));
			usleep(100);
		}

		return stmts;
	}

private:
	static std::string GenValues(const DailyStat& item)
	{
		ostringstream oss;

		std::string update_time = SqlUtils::GetTimeMillis();
		time_t seconds_past_epoch = time(0);
		long ssboe = seconds_past_epoch;

		oss << "('";
		oss << item.Symbol << "'," << item.SecurityID << "," << item.Price << ","
			<< item.Size << ",'" << item.UpdateType << "','" << item.EntryType << "'," << item.TradeDate << ","
			<< item.RptSeq << "," << ssboe << ",'" << update_time << "'"
			<< ")";

		return oss.str();
	}

	static std::string GenQuery(vector<string> v, string queryPreamble)
	{
		std::string szQuery = SqlUtils::vector_join(v, ",");
		std::string szStmt = queryPreamble + szQuery + ";";

		return szStmt;
	}
};

#endif /* SRC_DATABASEPROCESSORS_DAILYSTATSQLGEN_HPP_ */
