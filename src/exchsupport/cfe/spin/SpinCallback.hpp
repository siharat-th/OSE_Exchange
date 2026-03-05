//============================================================================
// Name        	: SpinCallback.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 2, 2023 5:08:13 PM
//============================================================================

#ifndef SRC_CFE_SPINCALLBACK_HPP_
#define SRC_CFE_SPINCALLBACK_HPP_
#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>

#include <KT01/DataStructures/LimitOrderBook.hpp>
#include <KT01/DataStructures/MDOrder.hpp>
using namespace KT01::OrderBook;
using namespace KT01::OrderBook::MarketDepth;


namespace KTN {
namespace CFE{
namespace Spin {
;
/**
 * @brief The SpinCallback class is an abstract callback class for Spin Server Callback Functions
 */
class SpinCallback  {
public:
	/**
  * @brief Constructs a SpinCallback object.
  */
	SpinCallback(){};

	/**
  * @brief Destroys the SpinCallback object.
  */
	virtual ~SpinCallback() {};

	virtual void onSpinImageAvailable(uint32_t sequence) = 0;

    virtual void onSpinStart(uint32_t startseq, uint32_t endseq, uint32_t count) = 0;
    virtual void onSpinEnd(uint32_t seqnum, uint32_t total_processed) = 0;
    virtual void onBooksAvailable(vector<LimitOrderBook> & books) = 0;

};


} // namespace Spin
} // namespace CFE
} // namespace KTN
//============================================================================


#endif /* SRC_CFE */
