//============================================================================
// Name        	: MatrixGen.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Aug 30, 2023 7:46:11 PM
//============================================================================

#ifndef SRC_ALGOENGINES_MATRIXGEN_HPP_
#define SRC_ALGOENGINES_MATRIXGEN_HPP_
#pragma once

#include <memory>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <AlgoSupport/algo_structs.hpp>
#include <KT01/Core/Log.hpp>

struct RC
{
	int iRow;
	int iCol;
};

struct SimpleEdge
{
	SimpleEdge(void):
		bidrow(0),askrow(0),
		bidedge(-9999),askedge(-9999),
		bidprice(0),askprice(0),
		bidspprice(0), askspprice(0),
		bidspside(0),askspside(0)
	{}

	//hedging row for bid:
	int bidrow;
	//hedging row for ask:
	int askrow;

	int bidedge;
	int askedge;

	int bidprice;
	int askprice;

	int bidspprice;
	int askspprice;

	int bidcalcspread;
	int askcalcspread;

	int bidspside;
	int askspside;


	string Print(int side)
	{
		ostringstream oss;

		if (side == 1)
		{
			oss << "BID HedgeRow:" << bidrow << " Edge:" << bidedge << " px:" << bidprice << " sp:" << bidspside << " @ " << bidspprice << " calc:" << bidcalcspread;
			return oss.str();
		}

		if (side == -1)
		{
			oss << "ASK HedgeRow:" << askrow << " Edge:" << askedge << " px:" << askprice << " sp:" << askspside << " @ " << askspprice << " calc:" << askcalcspread;
			return oss.str();
		}

		return ("SIDE IS INCORRECT!");
	}

//	int bidspcol;
//	int askspcol;

};

/**
 * @brief The MatrixGen class is responsible for generating spread and outright matrices from the input data.
 */
class MatrixGen {
public:
	MatrixGen();
	virtual ~MatrixGen();

	/**
  * @brief Generates the spread matrix.
  * @param Product The product name.
  * @param StartMonth The starting month.
  * @param StartYear The starting year.
  * @param NumMonths The number of months.
  */
	void GenSpreadMatrix(string Product, string StartMonth, int StartYear, int NumMonths);

	/**
  * @brief Generates the outright matrix.
  * @param product The product name.
  * @param StartMont The starting month.
  * @param StartYear The starting year.
  * @param NumMonths The number of months.
  */
	void GenOutrightMatrix(string product, string StartMont, int StartYear, int NumMonths);

	/**
  * @brief Gets the graph map.
  * @return The graph map.
  */
	const map<pair<int, int>, std::string>& GetGraphMap() const;

	/**
  * @brief Gets the row-column map.
  * @return The row-column map.
  */
	const std::unordered_map<std::string, RC>& GetRcMap() const;

private:
	/**
  * @brief Logs a message.
  * @param szMsg The message to log.
  * @param iColor The color of the log message.
  */
	void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

private:
	int _INFINITE_COL;
	map<pair<int, int>, std::string> m_GraphMap;
	std::unordered_map<std::string, RC> m_RcMap;
};

#endif /* SRC_ALGOENGINES_MATRIXGEN_HPP_ */
