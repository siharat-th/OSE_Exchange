//============================================================================
// Name        	: RiskLimitChecker.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Feb 22, 2022 1:08:37 PM
//============================================================================

#ifndef SRC_RISK_RISKLIMITCHECKER_HPP_
#define SRC_RISK_RISKLIMITCHECKER_HPP_

#include <unordered_map>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

#include <Rest/JsonUrlReader.hpp>
#include <Rest/RestApi.hpp>
#include <KT01/DataStructures/UserStructs.hpp>

#include <KT01/Core/Log.hpp>

/**
 * @class RiskLimitChecker
 * @brief Class for checking risk limits for orders
 */
class RiskLimitChecker {
public:
	/**
  * @brief Default constructor
  */
	RiskLimitChecker();

	/**
  * @brief Destructor
  */
	virtual ~RiskLimitChecker();

	/**
  * @brief Load the risk limits from a source
  */
	void LoadRiskLimits();

	/**
  * @brief Check if an order is within the risk limits
  * @param notional The notional value of the order
  * @param acct The account associated with the order
  * @return True if the order is within the risk limits, false otherwise
  */
	bool isOrderOK(double notional, string acct);

private:
	/**
  * @brief Log a message
  * @param szMsg The message to log
  * @param iColor The color of the log message (optional)
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

private:
	std::unordered_map<string, RiskLimit> _risk_limits;
};

#endif /* SRC_RISK_RISKLIMITCHECKER_HPP_ */
