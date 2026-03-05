/*
 * PlWatcher.hpp
 *
 *  Created on: May 24, 2024
 *      Author: sgaer
 */

#ifndef PLWATCHER_HPP_
#define PLWATCHER_HPP_

#pragma once
#include <Algos/AlgoBaseV3.hpp>

#include <AlgoParams/PLWatcherParams.hpp>
#include <AlgoEngines/engine_structs.hpp>
#include <AlgoEngines/PLMonitor.hpp>
#include <AlgoSupport/AlgoPLCallback.hpp>
#include <AlgoParams/Position.hpp>


using namespace KTN::ALGO;
using namespace KTN;

namespace chrono = std::chrono;

namespace KTN {

/**
 * @brief The PLWatcher class is responsible for monitoring the profit and loss of a trading algorithm.
 * 
 * It inherits from AlgoBaseV3 and AlgoPLCallback classes and provides implementations for various methods.
 * The PLWatcher class is used to track market data, trades, and order execution.
 */
class PLWatcher: public AlgoBaseV3, public AlgoPLCallback
{
public:
	/**
  * @brief Constructs a new PLWatcher object.
  * 
  * @param p The AlgoInitializer object used to initialize the PLWatcher.
  */
	PLWatcher(AlgoInitializer p);

	/**
  * @brief Destroys the PLWatcher object.
  */
	virtual ~PLWatcher();

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

	virtual void onPL(Position& pos, ProfitEvent event) ;

	void onEngineJson(EngineCommand& cmd);
	void OrderReceived(KTN::Order& incoming, int index) override;
	void OrderMinUpdate(KTN::OrderPod& ord) override;

private:
	bool GoodToGo();
	bool GotMktData();
	void ReportStatus();
	//void StatusThread();

	void AddLeg(const KTN::Order& ord, instrument& ln);

	//void Sender(string spreadid, KOrderAlgoTrigger::Enum trig);
	//void CheckPositions();

private:
	PLWatcherParams _cp;
	
	PLMonitor* _plm;

	bool _CIRCUITBREAKER;

	std::unordered_map<string, int> _SymbolIndexMap;
	std::unordered_map<string, string> _hdgOrdMap;

};

} /* namespace KTN */

#endif /* SQZER */
