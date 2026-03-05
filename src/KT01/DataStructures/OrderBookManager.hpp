#ifndef KT01_DATASTRUCTURES_MARKETDEPTH_OBM_HPP
#define KT01_DATASTRUCTURES_MARKETDEPTH_OBM_HPP

#pragma once


#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdint>
#include <unordered_map>

#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_vector.h>

#include <akl/PriceConverters.hpp>

#include <akl/Price.hpp>
using namespace akl;

#include <KT01/DataStructures/DepthEnums.hpp>
#include <KT01/DataStructures/DepthBook.hpp>


using namespace std;

namespace KT01 {
namespace DataStructures {
namespace MarketDepth {

    class OrderBookManager {
       private:
           std::unordered_map<uint32_t, DepthBook> books; // Maps SecId to DepthBook
           std::unordered_map<uint32_t, tbb::concurrent_queue<pxl>> pendingUpdates;

       public:
        OrderBookManager() {}

           DepthBook& operator[](uint32_t secid) {
            return books[secid];
        }

		DepthBook get(uint32_t secid )
		{
			return books[secid];
		}

		int count()
		{
			return books.size();
		}

		void snap(uint32_t secid, const std::vector<pxl>& bids, const std::vector<pxl>& asks, const std::vector<pxl>& iBids, const std::vector<pxl>& iAsks, uint32_t seq) {
			DepthBook& book = books[secid];
			book.Bids = bids;
			book.Asks = asks;
			book.iBids = iBids;
			book.iAsks = iAsks;
			book.RptSeq = seq;
			book.SnapReceived = true;

			pxl p;
			while (pendingUpdates[secid].try_pop(p)) {
				processPxl(p);
			}
		}

		// Processes a new update by routing it to the proper side.
		void processPxl(const pxl & p) {
			DepthBook& book = books[p.secid];
			if (!book.SnapReceived) {
				pendingUpdates[p.secid].push(p);
				return;
			}
			book.SecId = p.secid;
			book.RptSeq = p.seq;

			if(p.side == BookSide::Bid)
				processAction(book.Bids, p);
			else if(p.side == BookSide::Ask)
				processAction(book.Asks, p);
			else if(p.side == BookSide::ImpBid)
				processAction(book.iBids, p);
			else if(p.side == BookSide::ImpAsk)
				processAction(book.iAsks, p);

			if (p.lvl == 1)
			{
				if(p.side == BookSide::Bid)
				{
					book.Bid = p.px;
					book.BidSize = p.size;
				}
				else if(p.side == BookSide::Ask)
					{
					book.Ask = p.px;
					book.AskSize = p.size;
				}
				else if(p.side == BookSide::ImpBid)
					{
					book.IBid = p.px;	
					book.IBidSize = p.size;
				}
				else if(p.side == BookSide::ImpAsk)
					{
					book.IAsk = p.px;
					book.IAskSize = p.size;
				}
			}
		}

		void processAction(std::vector<pxl>& book, const pxl & p) {
			if(p.act == New) {
				// For each entry with a level >= new entry's level, bump its level.
				for(auto &entry : book) {
					if(entry.lvl >= p.lvl)
						entry.lvl++;
				}
				// Add the new entry.
				book.push_back(p);
				// Sort by level ascending.
				std::sort(book.begin(), book.end(), [](const pxl & a, const pxl & b) {
					return a.lvl < b.lvl;
				});
				// Keep only up to 10 levels.
				if(book.size() > 10)
					book.resize(10);
			}
			else if(p.act == Change) {
				bool found = false;
				for(auto &entry : book) {
					if(entry.lvl == p.lvl) {
						entry = p;
						found = true;
						break;
					}
				}
				if (!found) {
					// If no matching level exists, simply insert the update.
					book.push_back(p);
					std::sort(book.begin(), book.end(), [](const pxl & a, const pxl & b) {
						return a.lvl < b.lvl;
					});
					if(book.size() > 10)
						book.resize(10);
				}
			}

			else if(p.act == Del) {
				// Remove the entry with the matching level.
				auto it = std::remove_if(book.begin(), book.end(), [p](const pxl & entry) {
					return entry.lvl == p.lvl;
				});
				if(it != book.end()) {
					book.erase(it, book.end());
					// Decrement levels for all entries above the removed level.
					for(auto &entry : book) {
						if(entry.lvl > p.lvl)
							entry.lvl--;
					}
				}
			}
			else {
				std::cout << "UNKNOWN action" << std::endl;
			}
		}
	   };

} // namespace MktData
} // namespace DataStructures
} // namespace KTN


#endif
