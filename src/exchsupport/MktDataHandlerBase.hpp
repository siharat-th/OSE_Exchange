#pragma once

#include <string>

#include <KT01/DataStructures/IdxOrderBookManager.hpp>
#include <AlgoSupport/AlgoMktDataCallback.hpp>

class MktDataHandlerBase
{
public:
	MktDataHandlerBase() { }

	virtual ~MktDataHandlerBase() { }

	virtual void Subscribe(KTN::AlgoMktDataCallback *subscriber) = 0;
	virtual void Start() = 0;
	virtual void AddSymbol(const std::string &symbol, int index, uint32_t secId) = 0;
	
	inline void SetHandleTrades(bool handleTrades)
	{
        _handleTrades = handleTrades;
    }

	virtual void PrintStats() { }

	KT01::DataStructures::MarketDepth::IndexedOrderBookManager Obm;

protected:
	bool _handleTrades { false};
};