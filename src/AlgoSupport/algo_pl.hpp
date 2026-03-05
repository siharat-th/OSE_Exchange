//============================================================================
// Name        	: algo_pl.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Feb 10, 2023 4:35:24 PM
//============================================================================


#ifndef SRC_ALGOS_ALGO_PL_HPP_
#define SRC_ALGOS_ALGO_PL_HPP_

#pragma once
#include <string>
#include <map>
#include <vector>
#include <sstream>

#include <KT01/DataStructures/UserStructs.hpp>
#include <Orders/Order.hpp>


/**
 * @brief Structure representing the profit and loss of a side of a trade (Buy or Sell)
 */
struct ProfitLossSide
{
	int iside; /**< The side of the trade */
	double bid; /**< The bid price */
	double ask; /**< The ask price */
	double totqty; /**< The total quantity */
	double totbuys; /**< The total buy quantity */
	double totsells; /**< The total sell quantity */
	double netpos; /**< The net position */
	double sumpx; /**< The sum of prices */
	double avgpx; /**< The average price */

	double buyavgpx; /**< The average buy price */
	double sellavgpx; /**< The average sell price */

	double realized; /**< The realized profit/loss */
	double unrealized; /**< The unrealized profit/loss */

	double tickvalue; /**< The tick value */
	double pl; /**< The profit/loss */
	double plprice; /**< The profit/loss price */
	int fillcount; /**< The number of fills */

	/**
  * @brief Prints the profit and loss side information
  * @return The formatted string representation of the profit and loss side
  */
	string Print()
	{
		ostringstream oss;
		oss <<fixed << std::setprecision(0);
		oss << "qty=" << totqty << " sum=" << sumpx << " avgpx=" << avgpx << " pl=" << pl;
		return oss.str();
	}

	/**
  * @brief Calculates the profit and loss side based on the fills, bid price, ask price, and tick value
  * @param fills The vector of fills
  * @param bidprice The bid price
  * @param askprice The ask price
  * @param tick The tick value
  * @return The calculated profit and loss side
  */
	static ProfitLossSide getPlMolecule(const vector<KTN::Order>& fills, double bidprice, double askprice, double tick)
	{
		ProfitLossSide pls = {};
		pls.tickvalue = tick;
		pls.bid = bidprice;
		pls.ask = askprice;
		pls.fillcount = fills.size();


		double buysum = 0;
		double sellsum = 0;

		double buyqty = 0;
		double sellqty = 0;

		for (auto ord : fills)
		{
			if (ord.OrdSide == KOrderSide::BUY)
			{
				buyqty += ord.lastqty;
				buysum += ord.lastqty * ord.lastpx;
			}
			if (ord.OrdSide == KOrderSide::SELL)
			{
				sellqty += ord.lastqty;
				sellsum += ord.lastqty * ord.lastpx;
			}
		}

		if (buyqty > 0)
			pls.buyavgpx = 	buysum / buyqty;

		if (sellqty > 0)
			pls.sellavgpx = sellsum / sellqty;

		pls.totbuys = buyqty;
		pls.totsells = sellqty;

		pls.netpos = buyqty - sellqty;

		pls.realized = 0;
		pls.unrealized = 0;

		double closed = (pls.netpos > 0) ? pls.totsells : pls.totbuys;
		if (pls.netpos == 0)
			closed = 0;

		pls.realized = (pls.sellavgpx - pls.buyavgpx) * closed;

		if (pls.netpos > 0)
		{
			pls.sumpx = buysum;
			pls.avgpx = pls.buyavgpx;
		}

		if (pls.netpos < 0)
		{
			pls.sumpx = sellsum;
			pls.avgpx = pls.sellavgpx;
		}

		pls.totqty = pls.netpos;

		pls.pl = 0;

		if (pls.totqty != 0)
		{
			int mult = (pls.totqty > 0) ? 1 : -1;
			double avgpx = pls.avgpx;
			double totqty = fabs(pls.totqty);
			//double sum = pls.sumpx;

			//algomd leansym = _mdpair[leg.symbpair];
			double plprice = (mult ==1) ? pls.bid : pls.ask;
			//double plprice = (mult ==1) ? leansym.ask : leansym.bid;

			double plperlot = (plprice - avgpx) * mult;
			double pl = plperlot * totqty;

			//double tickvalue = tickvalue;// leg.tickvalue;// 10;

			pl *= pls.tickvalue;
			pls.pl = pl;

			//cout << "PL=" << pls.pl << endl;
		}

	//	cout << "PRINTER: " << pls.Print() << endl;

//		cout << fixed << "SUM=" << pls.sumpx << " TOTQTY=" << pls.totqty << " SYMB=" << fills[0].symbol << " AVG=" << pls.avgpx <<
//				" BUYQTY=" << pls.totbuys << " SELLQTY=" << pls.totsells << " AVGBUYPX=" << pls.buyavgpx << " AVGSELLPX=" << pls.sellavgpx << " NETPOS=" << pls.netpos << endl;

		return pls;
	}

};

#endif /* SRC_ALGOS_ALGO_PL_HPP_ */
