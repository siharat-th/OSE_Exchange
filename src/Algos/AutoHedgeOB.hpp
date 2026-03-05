/*
 * AutoHedgeOB.hpp
 *
 *  Created on: May 24, 2024
 *      Author: sgaer
 */

#ifndef AUTOHDGOB2_HPP_
#define AUTOHDGOB2_HPP_

#pragma once
#include <Algos/AlgoBaseV3.hpp>

#include <AlgoParams/AutoHedgeOB2Params.hpp>
#include <AlgoParams/RiskBase.hpp>
#include <AlgoEngines/engine_structs.hpp>
#include <KT01/Core/ObjectPool.hpp>
using namespace KTN::Core;

using namespace KTN::ALGO;
using namespace KTN;

namespace chrono = std::chrono;

namespace KTN {

/**
 * AutoHedgeOB class is an implementation of the AlgoBaseV3 class.
 * It provides functionality for automatic hedging based on parameters.
 * This is an Order Book based algorithm, so it's activated from the Market Depth Window
 */
class AutoHedgeOB: public AlgoBaseV3 {
public:
	/**
  * Constructor for the AutoHedgeOB class.
  * @param p The AlgoInitializer object used to initialize the algorithm.
  */
	AutoHedgeOB(AlgoInitializer p);

	/**
  * Destructor for the AutoHedgeOB class.
  */
	virtual ~AutoHedgeOB();

	//algo base impl
	virtual void onJson(std::string json);
	virtual void onLaunch(std::string json);
	virtual void onCalculate();

	void onMktData(int index) override;
	void onTrade(int index, double price, double size, int aggressor) override;

	void minOrderExecution(KTN::OrderPod& ord, bool hedgeSent) override;

	virtual void Enable(bool bEnable);
	void ResetPosition() override;
	void Print() override;
	void ToggleCommand(int id) override;
	void Restart() override;

private:
	bool GoodToGo(int index);
	bool GotMktData(int index);
	void ReportStatus();

	void AddLeg(const KTN::Order& ord, instrument& ln);
	int AddLeg(string symbol, instrument& ln);

	void OrderReceived(KTN::Order& incoming, int index) override;
	void OrderMinUpdate(KTN::OrderPod& ord) override;

	void MapHdg(const KTN::OrderPod & ord, int action, const AutoHedgeOB2Params& sq);

	//bool SqzTriggered(const KTN::Order & ord, const algomd& md, const SqzOrdParam& sq);
	bool SendHedge(uint64_t reqid, int fillqty, AutoHedgeOB2Params& p);

private:
	AutoHedgeOB2Params _cp;
	ObjectPool<SpreadHedgeV3> _hdgpool;

	bool _CIRCUITBREAKER;

	std::unordered_map<string, int> _SymbolIndexMap;
	std::unordered_map<string, string> _hdgOrdMap;
	std::unordered_map<uint64_t, AutoHedgeOB2Params> _hdgMap;
	std::unordered_map<string, uint64_t> _reqMap;

};

} /* namespace KTN */

#endif /* SQZER */
