//============================================================================
// Name        	: Waves.hpp
// Author      	: sgaer
// Version     	:
// Copyright   	: Copyright (C) 2024 Katana Financial
// Date			: April 17, 2024 11:33:56 AM
//============================================================================

#ifndef WAVES_HPP
#define WAVES_HPP

#pragma once
#include <Algos/AlgoBaseV3.hpp>

#include <AlgoParams/WavesParams.hpp>
#include <AlgoParams/RiskBase.hpp>

#include <AlgoEngines/PLMonitor.hpp>
#include <AlgoEngines/MatrixGen.hpp>
#include <AlgoEngines/engine_structs.hpp>
#include <AlgoEngines/pricing_structs.hpp>

#include <AlgoEngines/WavgCalcMulti.hpp>
#include <AlgoCalculators/IntervalTimer.hpp>
#include <AlgoCalculators/TimerCallback.hpp>

#include <KT01/Core/ObjectPool.hpp>
using namespace KTN::Core;

using namespace KTN;
using namespace KTN::ALGO;
using namespace KTN::ALGO::PRICING;

namespace chrono = std::chrono;

namespace KTN {

/**
 * @brief Weighted Average algorithm class derived from AlgoBaseV3.
 */
class Waves: public AlgoBaseV3 {
public:
	/**
  * @brief Constructor for Weighted Average algorithm.
  * @param p AlgoInitializer object.
  */
	Waves(AlgoInitializer p);

	/**
  * @brief Destructor for Waves algorithm.
  */
	virtual ~Waves();

	//algo base impl
	virtual void onJson(std::string json);
	virtual void onEngineJson(EngineCommand& cmd);
	virtual void onLaunch(std::string json);
	virtual void onCalculate();

	void onMktData(int index) override;
	void onTrade(int index, double price, double size, int aggressor) override;

	//session callback impl
	void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) override;
	void onHedgeOrderSent(char* clordid, uint64_t reqid, int32_t secid,KOrderSide::Enum side,
					uint32_t qty, int32_t price, uint16_t stratid) override;

	void onOrderSent(char* clordid, uint64_t reqid, int32_t secid, KOrderSide::Enum side,
						uint32_t qty, int32_t price, KOrderAlgoTrigger::Enum trig) override;

	void OrderReceived(KTN::Order& incoming, int idx) override;

	virtual void Enable(bool bEnable);
	void ResetPosition() override;
	void Print() override;
	void DoublePrint();
	void ToggleCommand(int id) override;
	void Restart() override;

private:
	bool GoodToGo();
	bool GotMktData();
	void CancelAllOrders();

	void ReportStatus();
	void StatusThread();

	bool DoubleCalc(int n, int j);
	void TripleCalc(int n, int j);
	void QuadCalc(int n, int j);

	void QuoteGenerator(int index);
	void Prep(int index);


	void CheckSqueeze();
	void CheckSqueeze(int index);
	bool Squeezed(uint64_t reqid);

	bool SendQuote(instrument& leg, KOrderSide::Enum side,
			int price, int qty, vector<SpreadHedgeV3>& hdg);

	bool PrepareQuotingOrder(KTN::OrderPod &ord, instrument& inst,
			KOrderSide::Enum side, int32_t price, int qty);

	SpreadHedgeV3 GetHedgeLeg(int index, KOrderSide::Enum side, int32_t price, int spreadmult);

	bool Working(vector<KTN::OrderPod>& ord, instrument& leg, KOrderAlgoTrigger::Enum trig);

	void SimpleMatrix(string product, string month,int year, int nummonths);

	void BuildMatrix(string product, string month,int year, int nummonths);

	void RegisterLegByRowCol(int n, int j, KOrderProdType::Enum prodtype,
			string symbol);

private:
	WavesParams _cp;

	std::thread _statusthread;
	std::atomic<bool> _STATUS_ACTIVE;

	msgstats _msgcnt[100];

	ObjectPool<OrderPod> _QuoteOrdPool;
	ObjectPool<SpreadHedgeV3> _hdgpool;
	HedgeType::Enum _HDGTYPE;
	vector<SpreadHedgeV3> _hdgVecB;
	vector<SpreadHedgeV3> _hdgVecS;

	vector<uint64_t> _SqueezedOrds;

	//Implied Stuff for Matrix
	const int _INFINITE_COL = 24;
	map<pair<int, int>, std::string> m_GraphMap;
	std::unordered_map<std::string, RC> m_RcMap;

	map<pair<int, int>, std::string> m_TasGraphMap;
	std::unordered_map<std::string, RC> m_TasRcMap;

	int _rows;
	int _cols;
	const int INF = 1e9;

	int _cntQuotes;
	volatile bool _TRADE_TIMER_ACTIVE;

	volatile bool _OK_TO_TRADE;
	int64_t _LAST_TRADE;

	vector<int> _tasindexes;

	bool _PL_STARTED;
	std::unordered_map<string, Position> _posmap;
	std::unordered_map<string, RiskBase> _riskmap;

	long _QUOTEID;
	long _LASTQUOTEID;

	bool _TESTSENT;

	bool _CIRCUITBREAKER;
	bool _QUOTE_TIMER_OK;

	bool _SHOW_QUOTES;

	int _COVERCNT;

	int _SENTCNT;

	string _LASTPL;

	std::unordered_map<string, string> _hdgOrdMap;

	int _GOT_CNT;

	std::chrono::system_clock::time_point _tradeStartTp;

	struct stprpair
	{
		std::pair<int,int>m1;
		std::pair<int, int>m2;
		int cnt;

		void add(std::pair<int,int> p)
		{
			if (cnt == 0){
				m1 = p;
			}
			else{
				m2 = p;
			}

			cnt++;
		}
	};

	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator () (const std::pair<T1, T2>& p) const {
			auto hash1 = std::hash<T1>{}(p.first);
			auto hash2 = std::hash<T2>{}(p.second);
			return hash1 ^ hash2;
		}
	};

	vector<stprpair>_stpairs[24];

	inline string pn(int row, int col)
	{
		ostringstream oss;
		oss << "(" << row <<"," << col << ")";
		return oss.str();
	}

	inline double WavgBidAsk(const DepthBook& md)
	{
		if (md.BidSize == 0 || md.AskSize == 0)
			return 0;

		//double wavg = ((md.BidSize * md.Ask) + (md.AskSize * md.Bid)) / ((double)(md.BidSize + md.AskSize));
		double wavg = ((md.BidSize * md.Ask) + (md.AskSize * md.Bid)) / (static_cast<double>(md.BidSize + md.AskSize));
		return wavg;
	}

};

} /* namespace KTN */

#endif /* WAVES_HPP */
