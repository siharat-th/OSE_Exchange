//============================================================================
// Name        	: SpreadPricer.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 19, 2023 2:22:23 PM
//============================================================================

#include <AlgoEngines/SpreadPricer.hpp>

using namespace KTN::ALGO::PRICING;

SpreadPricer::SpreadPricer() : _hdgpool(10000)
{

}

SpreadPricer::~SpreadPricer() {
	//delete[] _legs;
}

void SpreadPricer::LoadOrders(int cbid, algometa meta)
{
	if (_numLegs == 2)
		_HDGTYPE = HedgeType::SIMPLE_RATIO;
	else
		_HDGTYPE = HedgeType::COMPLEX;
}

/**
 * @brief Calculate the quote for a given side as well as all leg prices for hedges,
 * taking into account the current market pricing for each leg and the respective multipliers.
 * See comments inline for more info.

 * @param imp: Hedging Instructions struct to pass to handler/hedger
 * @param index: Inst index in the prices array index of leg to be quoted
 * @param spmult: Spread Multiplier for leg to be quoted.
 * @param isBid: Quick boolean value if this spread is a bid or ask
 * @param edge: Spread edge
 * @param ticks: Discretionary ticks for hedge prices - see note inline
 * @param prices: array of most current prices from MD Handler
 * @param testmode
 */
void SpreadPricer::CalculateSide2(LittleImpSide& imp, int index, int spmult, bool isBid,  double edge, int ticks,
	const DepthBookContainer& prices, bool testmode)
{
	imp.reset();
	double leans = 0;

	testmode = false;

	int pricing_leg_side = (isBid)? 1 : -1;

	for (int i = 0 ; i < _numLegs; i++)
	{
		//algomd this_leg = prices[index];

		int8_t leg_side_ind = pricing_leg_side * _legs[i].spreadmult;

		if (_legs[i].index == index)
		{
			//imp.side = (leg_side_ind > 0) ? "B" : "S";
			imp.OrderSide = (leg_side_ind > 0) ? KOrderSide::BUY : KOrderSide::SELL;
//			if (testmode)
//				cout << "QUOTINGLEG "  << _legs[i].index << " INSTINDEX=" << _legs[index].index
//								<< " MKT: " << this_leg.Bid << "@" << this_leg.Ask << endl;

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


		/*
		 * Next, let's note that "ticks" are the disc ticks-- they are how many
		 * ticks you would have discretion for- so for a bid quote, we'd be looking to sell
		 * the hedge, and so discretion would decrease the price we want by the value
		 * of ticks, and vice versa (increase) for an ask quote.
		 */

		 DepthBook px = prices[index];

		if (leg_side_ind < 0)
		{
			leans += prices[index].Bid;

			SpreadHedgeV3 hdg = *_hdgpool.get();// {};

			//_quoter[ _legs[i].index].NextNew(hdg.ord, KOrderSide::SELL, 0, 0, (px.Bid - ticks));
			hdg.hdgtype = _HDGTYPE;
			hdg.edge = edge;
			hdg.OrdSide = KOrderSide::SELL;
			hdg.price = px.Bid - ticks;
			hdg.instindex = _legs[i].index;
			hdg.secid = _legs[i].secid;
			hdg.mktsegid = _legs[i].mktsegid;
			hdg.spreadmult = _legs[i].spreadmult;
			hdg.ratio = fabs((hdg.spreadmult / (float)spmult));
			imp.hdgPath.push_back(hdg);

//			cout << "LEG=" << i << " HDG SIDE=" << KOrderSide::toString(hdg.OrdSide)
//					<< " @ " << hdg.price << " TICKS=" << ticks << " MULT=" << hdg.spreadmult
//					<< " INSTINDEX=" << (int)hdg.instindex <<  " INDEX=" << (int)index
//					<<  " BID=" << px.Bid << " BID_INDEX=" << prices[i].Bid << endl;
		}
		else
		{
			leans += prices[index].Ask;

			SpreadHedgeV3 hdg = *_hdgpool.get();
			hdg.hdgtype = _HDGTYPE;
			hdg.edge = edge;
//			_quoter[ _legs[i].index].NextNew(hdg.ord, KOrderSide::BUY, 0, 0, (px.Ask + ticks));
			hdg.OrdSide = KOrderSide::BUY;
			hdg.price = px.Ask + ticks;
			hdg.instindex = _legs[i].index;
			hdg.secid = _legs[i].secid;
			hdg.mktsegid = _legs[i].mktsegid;
			hdg.spreadmult = _legs[i].spreadmult;
			hdg.ratio = fabs((hdg.spreadmult / (float)spmult));
			imp.hdgPath.push_back(hdg);

//			cout << "LEG=" << i << " HDG SIDE=" << KOrderSide::toString(hdg.OrdSide)
//				<< " @ " << hdg.price << " TICKS=" << ticks << " MULT=" << hdg.spreadmult
//				<< " INSTINDEX=" << (int)hdg.instindex << endl;
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

	//NOTE: change here- we need to use pricemultiplier which may - or may not - be same sign as spread mult.... Discuss.
	imp.price = leans + (edge * _legs[index].pricemult);
//	imp.edge = edge;
	imp.isBid = isBid;
//	imp.secid = _legs[index].secid;
	//imp.symbol = _legs[index].symbol;
	imp.index = index;

//	if (testmode)
//	{
//		string szside = (isBid) ? "BID" : "ASK";
//		ostringstream ccc;
//		ccc << szside << " SYMB=" << _legs[index].symbol << " PRICE=" << imp.price
//				<< " EDGE=" << edge << " LEANS=" << leans << " PXMULT=" <<  _legs[index].pricemult;
//		LogMe(ccc.str());
//
//		ccc.clear();
//		ccc.str("");
//
//		ccc << "inst=" << _legs[index].symbol << " leans=" << leans << " edge=" << edge << " pricing_leg_side=" << pricing_leg_side << " pricemult="
//				<< _legs[index].pricemult << " mintick=" << _legs[index].mintick ;
//		LogMe(ccc.str(),LogLevel::INFO);
//
//		for(auto& x : imp.hdgPath)
//		{
//			ccc.clear();
//			ccc.str("");
//			ccc << "     HEDGE PRICING INST " << _legs[index].symbol << " BID HEDGE: " << x.inst.symbol << " " <<  KOrderSide::toString(x.OrdSide) << " " << x.qty << " @ " << x.price;
//			LogMe(ccc.str());
//		}
//
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
//		string szside = (isBid) ? "BID" : "ASK";
//		ostringstream yousuck;
//		yousuck << "IMPLIED PRICES: " << szside << "=" << fixed << imp.price << endl;
//		LogMe(yousuck.str(),LogLevel::INFO);
//	}

}

void SpreadPricer::CalculateSide3(LittleImpSide& imp, int index, int spmult, bool isSpreadBid,  double edge, int ticks,
	const DepthBookContainer& prices, bool testmode)
{
	imp.reset();
	double leans = 0;
	bool round_needed = true;

	testmode = false;

	int spread_side = (isSpreadBid)? 1 : -1;

	int quote_leg_side = spread_side * _legs[index].spreadmult;

	//... Leg side = 1 --> DEBIT, so we need to multiply price by -1
	//Why? Don't really know
	int debit_credit_mult = (quote_leg_side > 0) ? -1 : 1;

	for (int i = 0 ; i < _numLegs; i++)
	{
		//algomd this_leg = prices[index];
		int8_t leg_side_ind = spread_side * _legs[i].spreadmult;
		
		if (_legs[i].index == index)
		{
			imp.OrderSide = (leg_side_ind > 0) ? KOrderSide::BUY : KOrderSide::SELL;
//			if (testmode)
//				cout << "QUOTINGLEG "  << _legs[i].index << " INSTINDEX=" << _legs[index].index
//								<< " MKT: " << this_leg.Bid << "@" << this_leg.Ask << endl;

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


		/*
		 * Next, let's note that "ticks" are the disc ticks-- they are how many
		 * ticks you would have discretion for- so for a bid quote, we'd be looking to sell
		 * the hedge, and so discretion would decrease the price we want by the value
		 * of ticks, and vice versa (increase) for an ask quote.
		 */


		//algomd px = prices[i];
		

		if (leg_side_ind < 0)
		{
			leans += prices[i].Bid * _legs[i].pricemult * leg_side_ind;
			
			SpreadHedgeV3 hdg = *_hdgpool.get();// {};
			
			//_quoter[ _legs[i].index].NextNew(hdg.ord, KOrderSide::SELL, 0, 0, (px.Bid - ticks));
			hdg.hdgtype = _HDGTYPE; //
			hdg.edge = edge;//
			hdg.OrdSide = KOrderSide::SELL;
			hdg.price = prices[i].Bid;// - ticks;
			hdg.instindex = _legs[i].index;
			hdg.secid = _legs[i].secid;//
			hdg.mktsegid = _legs[i].mktsegid;
			hdg.spreadmult = _legs[i].spreadmult;
			hdg.ratio = fabs((hdg.spreadmult / (float)spmult));
			hdg.sqz = _legs[i].squeeze;
			imp.hdgPath.push_back(hdg);

			// cout << "[Pricer DEBUG] LEG=" << i << " " << _legs[i].symbol << " HDG SIDE=" << KOrderSide::toString(hdg.OrdSide)
			// 		<< " @ " << hdg.price  << " LEANS=" << leans //<< endl;
			// //<< " TICKS=" << ticks << " MULT=" << hdg.spreadmult
			// 		<< " INSTINDEX=" << (int)hdg.instindex <<  " INDEX=" << (int)index << endl;

		}
		else
		{
			
			leans += prices[i].Ask *  _legs[i].pricemult * leg_side_ind;
			
			SpreadHedgeV3 hdg = *_hdgpool.get();
			hdg.hdgtype = _HDGTYPE;
			hdg.edge = edge;
//			_quoter[ _legs[i].index].NextNew(hdg.ord, KOrderSide::BUY, 0, 0, (px.Ask + ticks));
			hdg.OrdSide = KOrderSide::BUY;
			hdg.price = prices[i].Ask;// + ticks;
			hdg.instindex = _legs[i].index;
			hdg.secid = _legs[i].secid;
			hdg.mktsegid = _legs[i].mktsegid;
			hdg.spreadmult = _legs[i].spreadmult;
			hdg.ratio = fabs((hdg.spreadmult / (float)spmult));
			hdg.sqz = _legs[i].squeeze;
			imp.hdgPath.push_back(hdg);
			
			// cout << "[Pricer DEBUG] LEG=" << i << " " << _legs[i].symbol
			// 	<< " HDG SIDE=" << KOrderSide::toString(hdg.OrdSide)
			// 	<< " @ " << hdg.price << " LEANS=" << leans //<< endl;
			// 	//<< " TICKS=" << ticks << " MULT=" << hdg.spreadmult
			// 	<< " INSTINDEX=" << (int)hdg.instindex << endl;
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

	//NOTE: change here- we need to use pricemultiplier which may - or may not - be same sign as spread mult.... Discuss.
	imp.price = (leans * debit_credit_mult);
	
	//add the edge
	imp.price += (edge * _legs[index].spreadmult * _legs[index].pricemult);

	// cout << "[Pricer DEBUG] ***LEG " << index << " SYMB=" << _legs[index].symbol << " QUOTE SIDE=" << quote_leg_side << " PRICE=" << imp.price  
	// << " MINTICK=" << _legs[index].mintick << endl;
	if (round_needed)
		imp.price = roundToTick(imp.price, _legs[index].mintick, quote_leg_side);
	

   // cout << "[Pricer DEBUG] ***LEG " << index << " " << _legs[index].symbol << " QUOTE SIDE=" << quote_leg_side << " PRICE=" << imp.price  << endl;
//	cout << "-----------------------" << endl;
	imp.isBid = isSpreadBid;
	imp.index = index;

}

/*
 *
 * Basically, here we just have a simple function that generates the price of the leg we want to quote.
 * We are not worrying about hedge price b/c we calculate that on the fly in the algo or handler
 *
 * This is designed really for a simple spread (2 legs/arb/micro etc) but can be applied anywhere
 * if you're ok with calculating the hedge based on the fill.
 *
 * This is important for market making or micro arbs
 *
 */
void SpreadPricer::CalculateSimple(LittleImpSide& imp, int index, int spmult, bool isBid,  double edge, int ticks,
	const DepthBookContainer& prices, bool testmode)
{

	double leans = 0;

	testmode = false;

	int pricing_leg_side = (isBid)? 1 : -1;

	for (int i = 0 ; i < _numLegs; i++)
	{
		int8_t leg_side_ind = pricing_leg_side * _legs[i].spreadmult;

		if (_legs[i].index == index)
		{
			imp.OrderSide = (leg_side_ind > 0) ? KOrderSide::BUY : KOrderSide::SELL;
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


		/*
		 * Next, let's note that "ticks" are the disc ticks-- they are how many
		 * ticks you would have discretion for- so for a bid quote, we'd be looking to sell
		 * the hedge, and so discretion would decrease the price we want by the value
		 * of ticks, and vice versa (increase) for an ask quote.
		 */


		//algomd px = prices[i];

		if (leg_side_ind < 0)
		{
			leans += prices[i].Bid;
		}
		else
		{
			leans += prices[i].Ask;
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

	//NOTE: change here- we need to use pricemultiplier which may - or may not - be same sign as spread mult.... Discuss.
	imp.price = leans + (edge * _legs[index].pricemult);

}



