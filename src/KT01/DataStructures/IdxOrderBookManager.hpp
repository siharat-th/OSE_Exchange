#ifndef KT01_DATASTRUCTURES_MARKETDEPTH_IDXOBM_HPP
#define KT01_DATASTRUCTURES_MARKETDEPTH_IDXOBM_HPP

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

#include <KT01/Core/Macro.hpp>

#include <akl/PriceConverters.hpp>

#include <akl/Price.hpp>
using namespace akl;

#include <KT01/DataStructures/DepthEnums.hpp>
#include <KT01/DataStructures/DepthBook.hpp>
#include <KT01/DataStructures/DepthBookTypes.hpp>

#include <KT01/Core/Settings.hpp>


using namespace std;

namespace KT01 {
namespace DataStructures {
namespace MarketDepth {

    class IndexedOrderBookManager {
       private:	   
           std::unordered_map<uint32_t, tbb::concurrent_queue<pxl>> pendingUpdates;
		   int _MAXSECS = 16;
		   int _LOADED = 0;
		   bool _DEBUGBOOK = false;

       public:
			DepthBookContainer books;
			std::vector<std::string> productGroups;

	    IndexedOrderBookManager() : _MAXSECS(16),_LOADED(0),_DEBUGBOOK(false)
		{
			Resize(_MAXSECS);

			Settings settings;
			settings.Load("Settings.txt", "Settings");
			_DEBUGBOOK = settings.getBoolean("Cme.DebugMdp");
		}

		void Resize(int size)
		{
			books.resize(size);
			productGroups.resize(size);
			_MAXSECS = size;
			_LOADED = 0;
		}

		int Size()
		{
			return _LOADED;
		}

		int MaxSize()
		{
			return books.size();;
		}

		int IndexOf(uint32_t secid)
		{
			for(auto & b : books)
			{
				//cout << "[IdxObm] IndexOf: secid=" << secid << " b.SecId=" << b.SecId << " b.Index=" << b.Index << endl;
				if(b.SecId == secid)
					return b.Index;
			}
			return -1;
		}

		const DepthBook& operator[](size_t index) {
			if (index > books.size())
			{
				if (_DEBUGBOOK)
					cout << "[IdxObm] ERROR: index=" << index << " booksize=" << books.size() << endl;
				return books.at(0);
			}
			return books.at(index); //[index];
        }

		void Add(int32_t secid, int index, const std::string &productGroup)
		{

			if (_LOADED >= _MAXSECS || index >= _MAXSECS)
			{
				cout << "[IdxObm] ERROR: index=" << index << " booksize=" << books.size() << endl;
				return;
			}

			DepthBook book;
			book.SecId = secid;
			book.RptSeq = 0;
			book.Index = index;
			books[index] = book;

			productGroups[index] = productGroup;

			_LOADED++;
			if (_DEBUGBOOK)
				cout << "[IdxObm] DEBUG: index=" << index << " booksize=" << books.size() << endl;
		}

		void SnapshotReceived(uint32_t secid, std::vector<pxl>& bids, std::vector<pxl>& asks, std::vector<pxl>& iBids, std::vector<pxl>& iAsks, uint32_t seq) 
		{
			int index = IndexOf(secid);

			if (index < 0) return;
			
			DepthBook& book = books[index];
			book.Bids = bids;
			book.Asks = asks;
			book.iBids = iBids;
			book.iAsks = iAsks;
			book.RptSeq = seq;
			book.SnapReceived = true;

			generateLevel1(index);

			pxl p;
			while (pendingUpdates[secid].try_pop(p)) {
				processPxl(p, index);
			}
		}

		void SnapshotReceived(DepthBook& book) 
		{
			int index = IndexOf(book.SecId);

			if (index < 0) return;
			
			// DepthBook& book = books[index];
			// book.Bids = book.Bids;
			// book.Asks = book.Asks;
			// book.iBids = book.iBids;
			// book.iAsks = book.iAsks;
			// book.RptSeq = book.RptSeq;
			book.SnapReceived = true;
			books[index] = book;

			generateLevel1(index);

			pxl p;
			while (pendingUpdates[book.SecId].try_pop(p)) {
				processPxl(p, index);
			}
		}

		// Processes a new update by routing it to the proper side.
		void processPxl(pxl & p, int index) {
			DepthBook& book = books[index];
			if (!book.SnapReceived) {
				pendingUpdates[p.secid].push(p);
				return;
			}
			book.SecId = p.secid;
			book.RptSeq = p.seq;

			switch(p.side)
			{
				case BookSide::Bid:
					processAction(book.Bids, p);
					break;
				case BookSide::Ask:
					processAction(book.Asks, p);
					break;
				case BookSide::ImpBid:
					processAction(book.iBids, p);
					break;
				case BookSide::ImpAsk:
					processAction(book.iAsks, p);
					break;
				default:
					cout << "[IdxObm] ERROR: UNKNOWN SIDE" << endl;
					return;
			}

			// if(p.side == BookSide::Bid)
			// 	processAction(book.Bids, p);
			// else if(p.side == BookSide::Ask)
			// 	processAction(book.Asks, p);
			// else if(p.side == BookSide::ImpBid)
			// 	processAction(book.iBids, p);
			// else if(p.side == BookSide::ImpAsk)
			// 	processAction(book.iAsks, p);

			 generateLevel1(index);

		}

		void processAction(std::vector<pxl>& book, pxl & p) {
			if(p.act == New) {
				// For each entry with a level >= new entry's level, bump its level.
				for(auto &entry : book) {
					if(entry.lvl >= p.lvl)
						entry.lvl++;
				}
				// Add the new entry.
				book.push_back(p);
				// Sort by level ascending.
				// std::sort(book.begin(), book.end(), [](const pxl & a, const pxl & b) {
				// 	return a.lvl < b.lvl;
				// });
				std::sort(book.begin(), book.end(), [](const pxl & a, const pxl & b) {
					int effectiveA = (a.lvl == 0 ? std::numeric_limits<int>::max() : a.lvl);
					int effectiveB = (b.lvl == 0 ? std::numeric_limits<int>::max() : b.lvl);
					return effectiveA < effectiveB;
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
					// std::sort(book.begin(), book.end(), [](const pxl & a, const pxl & b) {
					// 	return a.lvl < b.lvl;
					// });
					std::sort(book.begin(), book.end(), [](const pxl & a, const pxl & b) {
						int effectiveA = (a.lvl == 0 ? std::numeric_limits<int>::max() : a.lvl);
						int effectiveB = (b.lvl == 0 ? std::numeric_limits<int>::max() : b.lvl);
						return effectiveA < effectiveB;
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
						{
							entry.lvl--;
							if (entry.lvl == 0)
							  	entry.lvl = std::numeric_limits<uint16_t>::max();
						}
							
					}
				}
			}
			else {
				std::cout << "[IdxOb ERROR] UNKNOWN action" << std::endl;
			}
		}

		void generateLevel1(int index)
		{
			DepthBook& book = books[index];
			
			if (book.Bids.size() > 0)
			{
				book.Bid = book.Bids[0].px;
				book.BidSize = book.Bids[0].size;
				//cout << "[IdxOb DEBUG] BID UPDATE secid=" << book.SecId << " Bid=" << book.Bid << endl;
			}

			if (book.Asks.size() > 0)
			{
				book.Ask = book.Asks[0].px;
				book.AskSize = book.Asks[0].size;
				//cout << "[IdxOb DEBUG] ASK UPDATE secid=" << book.SecId << " Ask=" << book.Ask << endl;
			}

			if (book.iBids.size() > 0)
			{
				book.IBid = book.iBids[0].px;
				book.IBidSize = book.iBids[0].size;
				//cout << "[IdxOb DEBUG] IBID UPDATE secid=" << book.SecId << " IBid=" << book.IBid << endl;
			}

			if (book.iAsks.size() > 0)
			{
				book.IAsk = book.iAsks[0].px;
				book.IAskSize = book.iAsks[0].size;
				//cout << "[IdxOb DEBUG] IASK UPDATE secid=" << book.SecId << " IAsk=" << book.IAsk << endl;
			}

			if (_DEBUGBOOK)
			{
				string msgtype = (book.Bid == 0 || book.Ask == 0) ? "WARNING" : "DEBUG";
				cout << "[IdxOb " << msgtype  << "] secid=" << book.SecId << " Bid=" << book.Bid << " Ask=" << book.Ask << endl;
			}
			
		}
	   };

} // namespace MktData
} // namespace DataStructures
} // namespace KTN


#endif