#ifndef KT01_DATASTRUCTURES_MARKETDEPTH_DEPTHBOOK_HPP
#define KT01_DATASTRUCTURES_MARKETDEPTH_DEPTHBOOK_HPP

#pragma once


#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdint>
#include <unordered_map>

#include <tbb/concurrent_queue.h>

#include <akl/PriceConverters.hpp>


#include <KT01/DataStructures/DepthEnums.hpp>

#include <akl/Price.hpp>
using namespace akl;


using namespace std;

namespace KT01 {
namespace DataStructures {
namespace MarketDepth {

	struct pxl {

		//here, let's initialize the pxl level to max int. 
		//this elimintates the need to check for empty levels
		//and allows us to use the pxl as a default value and not worry about bad sorting for top of book etc
		pxl() : lvl(std::numeric_limits<uint16_t>::max()), seq(0) {}

		BookAction act;
		BookSide side;
		uint16_t lvl;
		uint32_t size;
		akl::Price px;
		uint32_t seq;
		uint32_t secid;  // Ensure updates are book-specific
	};

    class DepthBook {
	public:
		uint32_t SecId;
		uint32_t RptSeq;
		uint8_t SecStatus;
	
		std::vector<pxl> Bids;
		std::vector<pxl> Asks;
		std::vector<pxl> iBids;
		std::vector<pxl> iAsks;
		bool SnapReceived;

		/****************** Legacy to AlgoMD ********/
		uint8_t LastAgressor; /**< The last aggressor. */
		uint16_t Index; /**< The index. */

		akl::Price Bid; /**< The bid price. */
		akl::Price Ask; /**< The ask price. */
		int32_t BidSize; /**< The bid size. */
		int32_t AskSize; /**< The ask size. */

		akl::Price IBid; /**< The implied bid price. */
		akl::Price IAsk; /**< The implied ask price. */
		int32_t IBidSize; /**< The implied bid size. */
		int32_t IAskSize; /**< The implied ask size. */

		akl::Price High; /**< The high price. */
		akl::Price Low; /**< The low price. */

		int32_t Volume; /**< The volume. */
		akl::Price Last; /**< The last price. */
		int32_t LastSize; /**< The last size. */

		uint64_t recvTime { 0 };

		DepthBook() : SecId(0), RptSeq(0), SecStatus(20), SnapReceived(false), LastAgressor(0), Index(0),
              BidSize(0), AskSize(0), IBidSize(0), IAskSize(0), Volume(0), LastSize(0) {
			Bids.resize(10);
			Asks.resize(10);
			iBids.resize(2);
			iAsks.resize(2);
		}


		void clear() {
			Bids.clear();
			Asks.clear();
			iBids.clear();
			iAsks.clear();
			SnapReceived = false;
		}

		// Prints the book side by side for levels 1 to 10.
		void print() const {
			std::cout << "-------------------------------------------------" << "sec=" << SecId << " status=" << static_cast<int16_t>(SecStatus) << endl;
			std::cout << std::setw(6) << "Level" << "  "
					  << std::setw(7) << "BidSz" << "  "
					  << std::setw(8) << "BidPx" << "  |  "
					  << std::setw(8) << "AskPx" << "  "
					  << std::setw(7) << "AskSz" << "  | Seq=" << RptSeq << std::endl;

			for (int level = 1; level <= 10; ++level) {
				std::string bidSz, bidPx, askPx, askSz;
				// Find bid with matching level.
				auto itBid = std::find_if(Bids.begin(), Bids.end(), [level](const pxl &p) {
					return p.lvl == level;
				});
				if (itBid != Bids.end()) {
					//bidPx = std::to_string(PriceConverters::FromMantissa9(itBid->px));
					bidPx = std::to_string(itBid->px);
					bidSz = std::to_string(itBid->size);
				}
				// Find ask with matching level.
				auto itAsk = std::find_if(Asks.begin(), Asks.end(), [level](const pxl &p) {
					return p.lvl == level;
				});
				if (itAsk != Asks.end()) {
					//askPx = std::to_string(PriceConverters::FromMantissa9(itAsk->px));
					askPx = std::to_string(itAsk->px);
					askSz = std::to_string(itAsk->size);
				}
				std::cout << std::setw(6) << level << "  "
						  << std::setw(7) << bidSz << "  "
						  << std::setw(8) << bidPx << "  |  "
						  << std::setw(8) << askPx << "  "
						  << std::setw(7) << askSz << std::endl;
			}

			if (iBids.size() > 0 || iAsks.size() > 0)
				print_imps();
		}

		void print_imps() const {
			std::cout << "-------- Implied Markets -----------" << std::endl;
			std::cout << std::setw(6) << "Level" << "  "
					  << std::setw(7) << "iBidSz" << "  "
					  << std::setw(8) << "iBidPx" << "  |  "
					  << std::setw(8) << "iAskPx" << "  "
					  << std::setw(7) << "iAskSz" << "  | Seq=" << RptSeq << std::endl;

			//imps can't have more than 2 levels.
			for (int level = 1; level <= 2; ++level) {
				std::string bidSz, bidPx, askPx, askSz;
				// Find bid with matching level.
					auto itBid = std::find_if(iBids.begin(), iBids.end(), [level](const pxl &p) {
					return p.lvl == level;
				});
				if (itBid != iBids.end()) {
					//bidPx = std::to_string(PriceConverters::FromMantissa9(itBid->px));
					bidPx = std::to_string(itBid->px);
					bidSz = std::to_string(itBid->size);
				}
				// Find ask with matching level.
				auto itAsk = std::find_if(iAsks.begin(), iAsks.end(), [level](const pxl &p) {
					return p.lvl == level;
				});
				if (itAsk != iAsks.end()) {
					//askPx = std::to_string(PriceConverters::FromMantissa9(itAsk->px));
					askPx = std::to_string(itAsk->px);
					askSz = std::to_string(itAsk->size);
				}
				std::cout << std::setw(6) << level << "  "
						  << std::setw(7) << bidSz << "  "
						  << std::setw(8) << bidPx << "  |  "
						  << std::setw(8) << askPx << "  "
						  << std::setw(7) << askSz << std::endl;
			}
		}
	};

} // namespace MktData
} // namespace DataStructures
} // namespace KTN


#endif
