//============================================================================
// Name        	: LimitOrderBook.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Apr 8, 2024 4:19:34 PM
//============================================================================

#ifndef SRC_ORDERBOOK_LIMITORDERBOOK_HPP_
#define SRC_ORDERBOOK_LIMITORDERBOOK_HPP_
#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdint>
#include <map>
#include <unordered_map>

#include <KT01/DataStructures/MDOrder.hpp>
#include <KT01/DataStructures/DepthBook.hpp>
//#include <DataHandlers/data_includes.hpp>
#include <KT01/Core/Log.hpp>
#include <akl/BranchPrediction.hpp>

namespace KT01
{
    namespace OrderBook
    {
        namespace MarketDepth
        {

            /**
             * @brief Represents a limit order book.
             */
            class LimitOrderBook
            {
            private:
                uint32_t secid = 0;
                string base63_secid;
                string symbol;
				bool dirty { false };

                // very simply put, I can use a map with the orderid as the key
                std::unordered_map<uint64_t, MdOrder> bids;
                std::unordered_map<uint64_t, MdOrder> asks;
                //std::vector<vap> VAPs;

                using BidMbpMapType = std::map<akl::Price, int, std::greater<>>;
                using AskMbpMapType = std::map<akl::Price, int>;

                BidMbpMapType mbpBids;
                AskMbpMapType mbpAsks;

                KT01::DataStructures::MarketDepth::BookSecurityStatus secStatus = KT01::DataStructures::MarketDepth::BookSecurityStatus::UnknownOrInvalid;

            public:
                /**
                 * @brief Constructs a new LimitOrderBook object.
                 */
                LimitOrderBook() {}

				bool IsDirty() const { return dirty; }
				void SetDirty() { dirty = true; }
				void ClearDirty() { dirty = false; }

                void init(uint32_t secid, const std::string &base63_secid, const std::string &symbol)
                {
                    this->secid = secid;
                    this->base63_secid = base63_secid;
                    this->symbol = symbol;
                }

                uint32_t SecId() const { return secid; }
                std::string Base63SecId() const { return base63_secid; }
                std::string Symbol() const { return symbol; }
                KT01::DataStructures::MarketDepth::BookSecurityStatus SecurityStatus() const { return secStatus; }

                std::unordered_map<uint64_t, MdOrder> &Bids() { return bids; }
                std::unordered_map<uint64_t, MdOrder> &Asks() { return asks; }
                std::unordered_map<uint64_t, MdOrder> &GetBook(bool is_bid)
                {
                    return (is_bid) ? bids : asks;
                }

				void update_trading_status(const char tradingStatus)
				{
					switch (tradingStatus)
					{
						case 'S':
							secStatus = KT01::DataStructures::MarketDepth::BookSecurityStatus::Close;
							break;
						case 'Q':
							secStatus = KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen;
							break;
						case 'T':
							secStatus = KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade;
							break;
						case 'H':
							secStatus = KT01::DataStructures::MarketDepth::BookSecurityStatus::TradingHalt;
							break;
						default:
							LOGWARN("[LimitOrderBook] Unknown trading status code for secid {}: {}", secid, tradingStatus);
							secStatus = KT01::DataStructures::MarketDepth::BookSecurityStatus::UnknownOrInvalid;
							break;
					}
				}

                /**
                 * @brief Adds an order to the limit order book.
                 *
                 * @param order The order to be added.
                 * @param is_bid A flag indicating whether the order is a bid or an offer.
                 */
                void add_order(const MdOrder &order, bool is_bid)
                {
                    secid = order.secid;
                    base63_secid = order.base63_secid;
					dirty = true;

                    if (is_bid)
                    {
                        bids[order.id] = order;
                        mbpBids[akl::Price::FromUnshifted(order.price)] += order.quantity;
                    }
                    else
                    {
                        asks[order.id] = order;
                        mbpAsks[akl::Price::FromUnshifted(order.price)] += order.quantity;
                    }
                }

                /**
                 * @brief Updates an existing order in the limit order book.
                 *
                 * @param order The order to be updated.
                 * @return true if the order was successfully updated, false otherwise.
                 * @note This is where we preserve/change priority based on seqnum. Regardles of price,
                 *      the order with the lowest seqnum is the highest priority. Therefore, since this order
                 *      is being modified, it gets a new seqnum.  Note that reduce size takes care of preserving
                 *      priority, so any timee an order is modified, it gets a new seqnum, and therefore, a new
                 *      priority.
                 */
                bool update_order(const MdOrder& order)
                {
                    auto* book = find_book(order.id);
                    if (book)
                    {
                        MdOrder &bookOrder = (*book)[order.id];
                        dirty = true;

                        if (bookOrder.price == order.price)
                        {
                            const int qtyDelta = static_cast<int>(order.quantity) - static_cast<int>(bookOrder.quantity);
                            if (qtyDelta > 0)
                            {
                                if (book == &bids)
                                {
                                    mbpBids[akl::Price::FromUnshifted(order.price)] += qtyDelta;
                                }
                                else
                                {
                                    mbpAsks[akl::Price::FromUnshifted(order.price)] += qtyDelta;
                                }
                            }
                            else if (qtyDelta < 0)
                            {
                                if (book == &bids)
                                {
                                    reduce_qty_at_price(mbpBids, akl::Price::FromUnshifted(bookOrder.price), -qtyDelta);
                                }
                                else
                                {
                                    reduce_qty_at_price(mbpAsks, akl::Price::FromUnshifted(bookOrder.price), -qtyDelta);
                                }
                            }
                        }
                        else
                        {
                            if (book == &bids)
                            {
                                reduce_qty_at_price(mbpBids, akl::Price::FromUnshifted(bookOrder.price), bookOrder.quantity);
                                mbpBids[akl::Price::FromUnshifted(order.price)] += order.quantity;
                            }
                            else
                            {
                                reduce_qty_at_price(mbpAsks, akl::Price::FromUnshifted(bookOrder.price), bookOrder.quantity);
                                mbpAsks[akl::Price::FromUnshifted(order.price)] += order.quantity;
                            }
                        }

                        bookOrder.price = order.price;
                        bookOrder.quantity = order.quantity;
                        bookOrder.ns = order.ns;
                        bookOrder.seqnum = order.seqnum;

                        return true;
                    }
                    return false;  // order.id not found in bids or asks
                }

                bool reduce_order(uint64_t orderid, uint16_t cxl_qty)
                {
                    auto* book = find_book(orderid);
                    if (book)
                    {
						dirty = true;
                        MdOrder &bookOrder = (*book)[orderid];
                        if (bookOrder.quantity < cxl_qty)
                            return false;  // Cannot reduce more than available

                        bookOrder.quantity -= cxl_qty;

                        if (book == &bids)
                        {
                            reduce_qty_at_price(mbpBids, akl::Price::FromUnshifted(bookOrder.price), cxl_qty);
                        }
                        else
                        {
                            reduce_qty_at_price(mbpAsks, akl::Price::FromUnshifted(bookOrder.price), cxl_qty);
                        }

                        return true;
                    }
                    return false;  // order.id not found in bids or asks
                }


                bool delete_order(uint64_t order_id)
                {
                    auto* book = find_book(order_id);
                    if (book)
                    {
						dirty = true;
                        std::unordered_map<uint64_t, MdOrder>::const_iterator it = book->find(order_id);
                        if (it != book->end())
                        {
                            if (book == &bids)
                            {
                                reduce_qty_at_price(mbpBids, akl::Price::FromUnshifted(it->second.price), it->second.quantity);
                            }
                            else
                            {
                                reduce_qty_at_price(mbpAsks, akl::Price::FromUnshifted(it->second.price), it->second.quantity);
                            }
                            book->erase(it);
                        }
                        return true;
                    }
                    return false;
                }

                bool execute_order(uint64_t orderid, uint16_t execqty)
                {
                    auto* book = find_book(orderid);
                    if (book)
                    {
						dirty = true;
                        std::unordered_map<uint64_t, MdOrder>::iterator it = book->find(orderid);
                        if (it != book->end())
                        {
                            if (book == &bids)
                            {
                                reduce_qty_at_price(mbpBids, akl::Price::FromUnshifted(it->second.price), execqty);
                            }
                            else
                            {
                                reduce_qty_at_price(mbpAsks, akl::Price::FromUnshifted(it->second.price), execqty);
                            }

                            if (it->second.quantity > execqty)
                            {
                                it->second.quantity -= execqty;
                            }
                            else
                            {
                                book->erase(it);
                            }
                        }
                       
                        return true;
                    }
                    return false;  // order.id not found in bids or asks
                }

                template<typename PRICE_MAP_TYPE>
                inline void reduce_qty_at_price(PRICE_MAP_TYPE &map, const akl::Price price, const int qty)
                {
                    typename PRICE_MAP_TYPE::iterator it = map.find(price);
                    if (it != map.end())
                    {
                        it->second -= qty;
                        if (it->second <= 0)
                        {
                            map.erase(it);
                        }
                    }
                }

                std::unordered_map<uint64_t, MdOrder>* find_book(uint64_t order_id)
                {
                    auto bid_it = bids.find(order_id);
                    if (bid_it != bids.end())
                        return &bids;

                    auto ask_it = asks.find(order_id);
                    if (ask_it != asks.end())
                        return &asks;

                    return nullptr;  // Not found in either
                }

                std::unordered_map<uint64_t, MdOrder>* find_book(uint64_t order_id, char& side)
                {
                    auto bid_it = bids.find(order_id);
                    if (bid_it != bids.end()) {
						side = 'B';
                        return &bids;
                    }

                    auto ask_it = asks.find(order_id);
                    if (ask_it != asks.end()) {
						side = 'S';
                        return &asks;
                    }

                    return nullptr;  // Not found in either
                }


                int count(bool is_bid) const
                {
                    auto &book = (is_bid) ? bids : asks;
                    return book.size();
                }

                void clear()
                {
                    bids.clear();
                    asks.clear();
                }

                void print_orders() const
                {
                    std::cout << "Bids:" << std::endl;
                    for (const auto &order : bids)
                    {
                        std::cout << "ID: " << order.first << ", Price: " << order.second.price << ", Quantity: " << order.second.quantity << std::endl;
                    }

                    std::cout << "Asks:" << std::endl;
                    for (const auto &order : asks)
                    {
                        std::cout << "ID: " << order.first << ", Price: " << order.second.price << ", Quantity: " << order.second.quantity << std::endl;
                    }
                }

                void sort_bids(std::vector<MdOrder> & sorted_bids) const
                {
                    for (const auto &it : bids)
                        sorted_bids.push_back(it.second);

                    std::sort(sorted_bids.begin(), sorted_bids.end(), [](const MdOrder &a, const MdOrder &b) {
                        if (a.price == b.price)
                            return a.seqnum < b.seqnum;  // tie-breaker: earlier order
                        return a.price > b.price;
                    });
                }

                void sort_asks(std::vector<MdOrder> & sorted_asks) const
                {
                    for (const auto &it : asks)
                        sorted_asks.push_back(it.second);

                    std::sort(sorted_asks.begin(), sorted_asks.end(), [](const MdOrder &a, const MdOrder &b) {
                        if (a.price == b.price)
                            return a.seqnum < b.seqnum;  // tie-breaker: earlier order
                        return a.price < b.price;
                    });
                }

                void to_depthbook(KT01::DataStructures::MarketDepth::DepthBook& db) const
                {
                    db.clear();
                    db.SecId = secid;
                    db.SecStatus = secStatus;

					BidMbpMapType::const_iterator bidIt = mbpBids.cbegin();
					AskMbpMapType::const_iterator askIt = mbpAsks.cbegin();
					int bidMatchedQty = 0;
					int askMatchedQty = 0;

					if (unlikely(secStatus == KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen))
					{
						while (bidIt != mbpBids.cend() && askIt != mbpAsks.cend() && bidIt->first >=askIt->first)
						{
							const int matchQty = std::min(bidIt->second - bidMatchedQty, askIt->second - askMatchedQty);
							bidMatchedQty += matchQty;
							askMatchedQty += matchQty;
							if (bidMatchedQty >= bidIt->second)
							{
								++bidIt;
								bidMatchedQty = 0;
							}
							if (askMatchedQty >= askIt->second)
							{
								++askIt;
								askMatchedQty = 0;
							}
						}
					}

					uint16_t level = 1;
					if (bidIt != mbpBids.cend())
					{
						KT01::DataStructures::MarketDepth::pxl entry;
						entry.lvl = level;
						entry.side = KT01::DataStructures::MarketDepth::BookSide::Bid;
						entry.px = bidIt->first;
						entry.size = std::max(bidIt->second - bidMatchedQty, 0);
						entry.seq = 0;  // seq can be filled later
						entry.secid = secid;
						entry.act = KT01::DataStructures::MarketDepth::BookAction::New;
						db.Bids.push_back(entry);
						++level;
						++bidIt;

						for (; bidIt != mbpBids.cend() && level <= 10; ++level, bidIt++)
						{
							KT01::DataStructures::MarketDepth::pxl entry;
							entry.lvl = level;
							entry.side = KT01::DataStructures::MarketDepth::BookSide::Bid;
							entry.px = bidIt->first;
							entry.size = bidIt->second;
							entry.seq = 0;  // seq can be filled later
							entry.secid = secid;
							entry.act = KT01::DataStructures::MarketDepth::BookAction::New;
							db.Bids.push_back(entry);
						}
					}

					level = 1;
					if (askIt != mbpAsks.cend())
					{
						KT01::DataStructures::MarketDepth::pxl entry;
						entry.lvl = level;
						entry.side = KT01::DataStructures::MarketDepth::BookSide::Ask;
						entry.px = askIt->first;
						entry.size = std::max(askIt->second - askMatchedQty, 0);
						entry.seq = 0;  // seq can be filled later
						entry.secid = secid;
						entry.act = KT01::DataStructures::MarketDepth::BookAction::New;
						db.Asks.push_back(entry);
						++level;
						++askIt;

						for (; askIt != mbpAsks.cend() && level <= 10; ++level, askIt++)
						{
							KT01::DataStructures::MarketDepth::pxl entry;
							entry.lvl = level;
							entry.side = KT01::DataStructures::MarketDepth::BookSide::Ask;
							entry.px = askIt->first;
							entry.size = askIt->second;
							entry.seq = 0;  // seq can be filled later
							entry.secid = secid;
							entry.act = KT01::DataStructures::MarketDepth::BookAction::New;
							db.Asks.push_back(entry);
						}
					}
                }

                void print_side(const std::string &side,  std::vector<MdOrder> sorted) const
                {
                    std::string border = "-----------------------------------------------------------------------------\n";
                    std::cout << border;

                    for (const auto &order : sorted)
                    {
                        std::cout << std::left 
                                << std::setw(6)  << side
                                << std::setw(30) << order.id
                                << std::setw(12) << order.price
                                << std::setw(10) << order.quantity
                                << std::setw(18) << order.ns
                                << std::setw(10) << order.seqnum 
                                << "\n";
                    }

                    std::cout << border;

                }

                void print_order_book() const
                {
                    std::cout << "\n==============================  ORDER BOOK ======================================\n";
                    std::cout << std::left 
                            << std::setw(6)  << "Side"
                            << std::setw(30) << "Order ID"
                            << std::setw(12) << "Price"
                            << std::setw(10) << "Qty"
                            << std::setw(18) << "Timestamp(ns)"
                            << std::setw(10) << "SeqNum" 
                            << "\n";
                    std::cout << "=================================================================================\n";

                   
                    std::vector<MdOrder> sorted_asks;
                    sort_asks(sorted_asks);

                    print_side("ASK", sorted_asks);

                    std::vector<MdOrder> sorted_bids;
                    sort_bids(sorted_bids);
                    print_side("BID", sorted_bids);
                }


            };



        } // namespace MarketDepth
    } // namespace MD
} // namespace KTN

#endif /* SRC_ORDERBOOK_LIMITORDERBOOK_HPP_ */