//============================================================================
// Name        	: RiskLimitChecker.cpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Feb 22, 2022 1:08:37 PM
//============================================================================

#include <Risk/RiskLimitChecker.hpp>

RiskLimitChecker::RiskLimitChecker() {
	// TODO Auto-generated constructor stub

}

RiskLimitChecker::~RiskLimitChecker() {
	// TODO Auto-generated destructor stub
}

void RiskLimitChecker::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void RiskLimitChecker::LoadRiskLimits() {
//	std::string url = JSON_URL_CRYPTO_RISK;
//
//	std::string json = JsonUrlReader::Get(url);
//
//	StringStream s(json.c_str());
//
//	Document d;
//	d.ParseStream(s);
//
//	LogMe("JSON CRYPTO RISK URL STREAM LOADING!", LogLevel::INFO);
//
//
//	const rapidjson::Value& limits = Value(d, d.GetAllocator());
//	assert(limits.IsArray());
//
//	for (Value::ConstValueIterator itr = limits.Begin(); itr != limits.End(); ++itr)
//	{
//
//		RiskLimit lmt = {};
//
//		lmt.key = (*itr)["key"].GetString();
//		lmt.account = (*itr)["account"].GetString();
//		lmt.max_ord_notional = (*itr)["max_ord_notional"].GetDouble();
//
//		_risk_limits[lmt.account] = lmt;
//		LogMe("***LOADED RISK LIMITS FOR ACCOUNT " + lmt.account, LogLevel::INFO);
//	}
}

bool RiskLimitChecker::isOrderOK(double notional, string acct) {

	if (_risk_limits.find(acct) == _risk_limits.end())
	{
		LogMe("ACCOUNT " + acct + " NOT FOUND IN LIMITS MAP.",LogLevel::ERROR);
		return false;
	}

	if (notional > _risk_limits[acct].max_ord_notional)
		return false;

	return true;

}
