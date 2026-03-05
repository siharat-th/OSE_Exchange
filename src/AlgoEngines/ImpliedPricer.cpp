//============================================================================
// Name        	: ImpliedPricer.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 19, 2023 2:22:23 PM
//============================================================================

#include <AlgoEngines/ImpliedPricer.hpp>

using namespace KTN::ALGO::PRICING;

ImpliedPricer::ImpliedPricer() {


}

ImpliedPricer::~ImpliedPricer() {
	//delete[] _legs;
}

void ImpliedPricer::floydShortestPaths(EdgeStruct (&adjMatrix)[MAX_SIZE][MAX_SIZE], bool isBid) {
    // Initialize distance matrix with bid prices
    float distance[MAX_SIZE][MAX_SIZE];
    for (int i = 0; i < MAX_SIZE; ++i) {
        for (int j = 0; j < MAX_SIZE; ++j) {
            if (i == j || !adjMatrix[i][j].exists) {
                distance[i][j] = INF;
            } else {
            	if (isBid)
            		distance[i][j] = adjMatrix[i][j].prices.bid;
            	else
            		distance[i][j] = adjMatrix[i][j].prices.ask;
            }
        }
    }

    // Floyd's algorithm
    for (int k = 0; k < MAX_SIZE; ++k) {
        for (int i = 0; i < MAX_SIZE; ++i) {
            for (int j = 0; j < MAX_SIZE; ++j) {
                if (distance[i][k] + distance[k][j] < distance[i][j]) {
                    distance[i][j] = distance[i][k] + distance[k][j];
                }
            }
        }
    }

    // Output shortest paths
    string bidask = (isBid) ? "bid" : "ask";
    cout << "Shortest paths (in terms of " << bidask << " prices):" << endl;
    for (int i = 0; i < MAX_SIZE; ++i) {
        for (int j = 0; j < MAX_SIZE; ++j) {
            if (distance[i][j] == INF) {
                cout << "INF\t";
            } else {
                cout << "(" << i << "," << j << ")=" << distance[i][j] << "\t";
            }
        }
        cout << endl;
    }
}

// Function to calculate shortest paths using Row-by-row Floyd-Warshall algorithm
void ImpliedPricer::floydShortestPaths2(EdgeStruct (&adjMatrix)[MAX_SIZE][MAX_SIZE], bool isBid) {
    // Initialize distance matrix with bid prices
    float distance[MAX_SIZE][MAX_SIZE];
    for (int i = 0; i < MAX_SIZE; ++i) {
        for (int j = 0; j < MAX_SIZE; ++j) {
            if (i == j || !adjMatrix[i][j].exists) {
                distance[i][j] = INF;
            } else {
            	if (isBid)
					distance[i][j] = adjMatrix[i][j].prices.bid;
				else
					distance[i][j] = adjMatrix[i][j].prices.ask;
            }
        }
    }

    // Row-by-row Floyd-Warshall algorithm
    for (int k = 0; k < MAX_SIZE; ++k) {
        for (int i = k + 1; i < MAX_SIZE; ++i) {
            for (int j = k + 1; j < MAX_SIZE; ++j) {
                if (distance[i][k] + distance[k][j] < distance[i][j]) {
                    distance[i][j] = distance[i][k] + distance[k][j];
                }
            }
        }
    }

    // Output shortest paths
    cout << "Shortest paths (in terms of bid prices):" << endl;
    for (int i = 0; i < MAX_SIZE; ++i) {
        for (int j = i; j < MAX_SIZE; ++j) {
            if (distance[i][j] == INF) {
                cout << "INF\t";
            } else {
                cout << distance[i][j] << "\t";
            }
        }
        cout << endl;
    }
}

LittleImpSide ImpliedPricer::CalculateSide(int index, int spmult, bool isBid,  double edge, DepthBook* prices, bool testmode)
{
	LittleImpSide imp = {};
	instrument inst = _legs[index];

	double leans = 0;

	testmode = false;

	int pricing_leg_side = (isBid)? 1 : -1;


	for (int i = 0 ; i < _numLegs; i++)
	{
		DepthBook this_leg = prices[_legs[i].index];

		int leg_side_ind = pricing_leg_side * _legs[i].spreadmult;

		if (_legs[i].index == inst.index)
		{
			//imp.side = (leg_side_ind > 0) ? "B" : "S";
			imp.OrderSide = KOrderSide::Value(leg_side_ind);
			if (testmode)
				cout << "QUOTINGLEG "  << _legs[i].index << " INSTINDEX=" << inst.index
								<< " MKT: " << this_leg.Bid << "@" << this_leg.Ask << endl;

			//NOW WE SKIP BC WE SOLVE FOR ALL OTHER LEGS THEN ADD EDGE TO QUOTE THIS ONE!!!
			continue;
		}

		/*so, pricing/quoting leg=0 and if i=1 and this_leg has spreadmult = -1,
		 * I am selling this leg if we are buying the spread (isBid=true).
		 *
		 * so we add the bid to the spread price, and add the this_leg @ bid to the hedge path.
		 *
		 *
		 * Similarly, let's say i =0 and pricing_quoting leg is now leg 1.  Leg 0 has spreadmult of 1,
		 * isBid = true as we are still buying the spread, so we add the sides according to the lean:
		 *
		 * If I want to buy -35's by bidding for leg 0:
		 * 		I lean on bid of legs with SM < 0 and I lean on bids of legs with SM > 0.
		 * If I want to buy -35's by offering leg 1:
		 * 		I lean on ask of legs with SM > 0 and I lean on bid of legs with SM < 0.
		 *
		 * This is the "normal state" for the "buyness" of the first leg, indicating that we are buying the spread.
		 */

		SpreadHedgeV3 hdg = {};

		if (leg_side_ind < 0)
		{
			leans += this_leg.Bid;

			//hdg.side = "S";
//			hdg.OrdSide = KOrderSide::SELL;
//			hdg.price = this_leg.bid;
//			hdg.instindex = i;
//			hdg.tickprecision = 0;// _legs[i].tickprecision;
			imp.hdgPath.push_back(hdg);

		}
		else
		{
			leans += this_leg.Ask;

			//hdg.side = "B";
//			hdg.OrdSide = KOrderSide::BUY;
//			hdg.price = this_leg.ask;
//			hdg.instindex = i;
//			hdg.tickprecision = 0;// _legs[i].tickprecision;
			imp.hdgPath.push_back(hdg);
		}
	}
	//finished summing up all legs and mapping the hedge route

	//now we have the sum of the leans. we can now derive the price we wish to bid/offer
	//since we have the sum of the diff between all other legs and ours, we just add the edge * spreadmult_of_the_quoting_leg:

	/* so , in this example, if the sum of all legs, which in this case is a simple 2-leg strategy,
	 * is 108, which is the bid of the 2nd spread, we can then add the edge, say (-35 * SM) (1), to 108, which
	 * gives us a bid for Leg0 of 73.  Similarly, given the offer for Leg0 is 80, and let's say we are quoting
	 * Leg1 on the bid of -35, the offer for Leg 1 (SM = -1) will be the sum of the offers, 80, + (edge * SM (in this leg, it's -1)), giving us
	 * 80 + (-35 * -1) = 115.
	 *
	 */

	imp.price = leans + (edge * inst.spreadmult);
	imp.isBid = isBid;
	imp.index = index;

	//if (testmode)
	{
		string szside = (isBid) ? "BID" : "ASK";
		ostringstream ccc;
		ccc << szside << " SYMB=" << inst.symbol << " PRICE=" << imp.price;
		LogMe(ccc.str());

		ccc.clear();
		ccc.str("");

		ccc << "inst=" << inst.symbol << " leans=" << leans << " edge=" << edge << " pricing_leg_side=" << pricing_leg_side << " pricemult="
				<< inst.pricemult << " mintick=" << inst.mintick ;
		LogMe(ccc.str(),LogLevel::INFO);

//		for(auto& x : imp.hdgPath)
//		{
//			ccc.clear();
//			ccc.str("");
//			ccc << "     HEDGE PRICING INST " << inst.symbol << " BID HEDGE: " << x.secid << " " <<  KOrderSide::toString(x.OrdSide) << " " << x.qty << " @ " << x.price;
//			LogMe(ccc.str());
//		}

	}

	//factor in our quoting offsets
	//wait wait wait.. This is correct for a "straight" leg (i.e. a buy, or instside = 1)
//	double ImpliedBid = (leanbids * (-1 * instside)) + bidedge;
//	double ImpliedAsk = (leanasks * (-1 * instside)) + askedge;


	//cout << "2 LEAN IMPBID=" << ImpliedBid << endl;

	if (testmode)
	{
		string szside = (isBid) ? "BID" : "ASK";
		ostringstream yousuck;
		yousuck << "IMPLIED PRICES: " << szside << "=" << fixed << imp.price << endl;
		LogMe(yousuck.str(),LogLevel::INFO);
	}

	return imp;
}
//DEPRE-FUCKING-CATED
//LittleImp ImpliedPricer::Calculate(int index, double spbid, double spask,  const algomd* prices2, int numPrices, bool testmode=false)
//{
//	LittleImp imp = {};
//	instrument inst = _legs[index];
//
//	double leanbids = 0;
//	double leanasks = 0;
//
//	int pricing_leg_side = inst.iside;
//	int pricing_leg_mult = inst.spreadmult;
//	int price_chooser = inst.iside * inst.spreadmult;
//
//
//	double bidedge = spbid * inst.mintick;
//	double askedge = spask * inst.mintick;
//
//	testmode = false;
//
//	for (int i = 0 ; i < _numLegs; i++)
//	{
//		algomd legsym = prices2[_legs[i].index];
//
//		if (_legs[i].index == inst.index)
//		{
//			if (testmode)
//				cout << "QUOTINGLEG "  << _legs[i].index << " INSTINDEX=" << inst.index
//								<< " MKT: " << legsym.bid << "@" << legsym.ask << endl;
//			continue;
//		}
//
//		//THE ***LEG*** SPREADMULT NOT THE INST YOU ARE SOLVING FOR
//
////		ostringstream fkk;
////		fkk << "LEG=" << inst.symbol << " SPRDMULT=" << inst.spreadmult << " inst=" << inst.symbol
////				<< " instside=" << inst_native_side;
////		LogMe(fkk.str());
//
//		//so if this leg's spreadmult > 0 -> I will need to buy its ask...
//
//
//		int leg_side_ind = pricing_leg_side * _legs[i].spreadmult;
//
//		SpreadHedge hb ={};
//		SpreadHedge ha ={};
//
//		double legpxbidsprd = (_legs[i].spreadmult > 0) ? legsym.ask : legsym.bid;
//		double legpxasksprd = (_legs[i].spreadmult > 0) ? legsym.bid : legsym.ask;
//
//		if (leg_side_ind > 0)
//		{
//			leanbids += legsym.ask * fabs((double)inst.pricemult / (double)_legs[i].pricemult) * (double)_legs[i].spreadmult;
//			leanasks += legsym.bid * fabs((double)inst.pricemult / (double)_legs[i].pricemult)* (double)_legs[i].spreadmult;
//		}
//		else
//		{
//			leanbids += legsym.bid * fabs((double)inst.pricemult / (double)_legs[i].pricemult)* (double)_legs[i].spreadmult;
//			leanasks += legsym.ask * fabs((double)inst.pricemult / (double)_legs[i].pricemult)* (double)_legs[i].spreadmult;
//		}
//
////		if (_legs[i].tickprecision > 0)
////		{
////			legpxbidsprd = priceRoundConvert(legpxbidsprd, _legs[i].tickprecision, _legs[i].mintick);
////			legpxasksprd = priceRoundConvert(legpxasksprd, _legs[i].tickprecision, _legs[i].mintick);
////		}
//
//
//		if (testmode)
//			cout << "LEG " << legsym.symbol << ": " << legsym.bid << "@" << legsym.ask
//			<< " LEANBIDS=" << leanbids << " LEANASKS=" << leanasks << " LEGINDEX=" << _legs[i].index
//			<< " BIDEDGE=" << bidedge << " ASKEDGE=" << askedge << endl;
//
//
//		hb.side = (leg_side_ind > 0) ? "B" : "S";
//		hb.OrdSide = KOrderSide::Value(hb.side);
//		hb.price = (leg_side_ind > 0) ? legsym.ask : legsym.bid;
//		hb.inst = _legs[i];
//		hb.tickprecision = 0;// _legs[i].tickprecision;
//		imp.hdgBidPath.push_back(hb);
//
//
//		ha.side = (leg_side_ind > 0) ? "S" : "B";
//		ha.OrdSide = KOrderSide::Value(ha.side);
//		ha.price = (leg_side_ind > 0) ? legsym.bid : legsym.ask;
//		ha.inst = _legs[i];
//		ha.tickprecision = 0;//_legs[i].tickprecision;
//		imp.hdgAskPath.push_back(ha);
//
//	}
//
////	if (testmode)
////			cout << "****** END ITER ************" << endl;
//
//	// now we have the sum of the legs except for the leg we are interested in quoting...
////	double ib = (-1 * leanbids) - bidedge;
////	double ia = (-1 * leanasks) - askedge;
//
//
//
//	//next trick.. we have to flip bidedge/askedge if spreadmult is < 0
//	//why because you're mult is flipped so to 'ask' on the whole spreaqd, you need
//	//to buy this leg, so ...in other words, if this is the 2nd leg, and we are buying the spread
//	//(i.e. first leg spreadmult = 1, so buy the spread = buy leg 1,
//	//then for leg 2 with spreadmult < 0, to 'buy' the spread, we need to sell this leg...
//	//as in we need to **sell** CLX/Z and then buy CLV/X, so we need to sell
//	//X/Z 30 over; similiarly we then have to flip the sign on the bids and asks...
//	//now i'm cross eyed
//
////	if (pricing_leg_mult < 0)
////	{
////		double tempbid = bidedge;
////		double tempask = askedge;
////
////		bidedge = tempask ;//* -1;
////		askedge = tempbid ;//* -1;
////
////		tempbid = leanbids;
////		tempask = leanasks;
////
////		leanbids = tempask;
////		leanasks = tempbid;
////
////		vector<SpreadHedge> htbids = {};
////		htbids = imp.hdgBidPath;
////
////		vector<SpreadHedge> htasks = {};
////		htasks = imp.hdgAskPath;
////
////		imp.hdgBidPath = htasks;
////		imp.hdgAskPath = htbids;
////
////		for(auto& x : imp.hdgBidPath)
////		{
////			x.OrdSide = KOrderSide::Not(x.OrdSide);
////			x.side = (x.OrdSide == KOrderSide::BUY) ? "B" : "S";
////		}
////		for(auto& x : imp.hdgAskPath)
////		{
////			x.OrdSide = KOrderSide::Not(x.OrdSide);
////			x.side = (x.OrdSide == KOrderSide::BUY) ? "B" : "S";
////		}
////
////
////	}
//
//
//	//****you must divide by inst_mult to put the leans + edge into the same terms as the inst you are quoting...
//	// to make the solved price -- (m1 * sm1) = k - (m2 * sm2)
//	//								m1 = (k-(m2*sm2)) / sm1
//
//	double ib = bidedge - leanbids;
//	double ia = askedge - leanasks;
//
//	ib *= (double)1/(double)pricing_leg_mult;
//	ia *= (double)1/(double)pricing_leg_mult;
//
//	imp.bid = ib;
//	imp.ask = ia;
//
//	/*double refprice = 0;
//
//	for(auto x : imp.hdgBidPath)
//		x.primarypx = refprice;
//	for (auto x : imp.hdgAskPath)
//		x.primarypx = refprice;*/
//
//
//	//if (testmode)
//	{
//		ostringstream ccc;
//		ccc << "SYMB=" << inst.symbol << " IMP: BidEdge=" << fixed << bidedge << " AskEdge=" << fixed << askedge << " IB=" << ib << " IA=" << ia;
//		LogMe(ccc.str());
//
//		ccc.clear();
//		ccc.str("");
//
//		ccc << "inst=" << inst.symbol << " leanbids=" << leanbids << " asks=" << leanasks << " bidedge=" << bidedge
//				<< " askedge=" << askedge << " pricing_leg_mult=" << pricing_leg_mult << " pricemult="
//				<< inst.pricemult << " mintick=" << inst.mintick ;
//		LogMe(ccc.str(),LogLevel::INFO);
//
//		for(auto& x : imp.hdgBidPath)
//		{
//			ccc.clear();
//			ccc.str("");
//			ccc << "PRICING INST " << inst.symbol << " BID HEDGE: " << x.inst.symbol << " " <<  KOrderSide::toString(x.OrdSide) << " " << x.qty << " @ " << x.price;
//			LogMe(ccc.str());
//		}
//
//		for(auto& x : imp.hdgAskPath)
//		{
//			ccc.clear();
//			ccc.str("");
//			ccc << "PRICING INST " << inst.symbol << " ASK HEDGE: "  << x.inst.symbol << " " << KOrderSide::toString(x.OrdSide) << " " << x.qty << " @ " << x.price;
//			LogMe(ccc.str());
//		}
//	}
//
//	//factor in our quoting offsets
//	//wait wait wait.. This is correct for a "straight" leg (i.e. a buy, or instside = 1)
////	double ImpliedBid = (leanbids * (-1 * instside)) + bidedge;
////	double ImpliedAsk = (leanasks * (-1 * instside)) + askedge;
//
//
//	//cout << "2 LEAN IMPBID=" << ImpliedBid << endl;
//
//	if (testmode)
//	{
//		ostringstream yousuck;
//		yousuck << "IMPLIED PRICES: BID=" << fixed << ib << " ASK=" << fixed << ia << endl;
//		LogMe(yousuck.str(),LogLevel::INFO);
//	}
//
//	return imp;
//}


