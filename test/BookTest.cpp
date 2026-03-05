#include <gtest/gtest.h>

#include <akl/Book.hpp>

#include "Helpers.hpp"

using pxl = KT01::DataStructures::MarketDepth::pxl;

namespace akl::test
{

pxl createNew(KT01::DataStructures::MarketDepth::BookSide side, uint16_t lvl, uint32_t size, double px)
{
	pxl p;
	p.act = KT01::DataStructures::MarketDepth::BookAction::New;
	p.side = side;
	p.lvl = lvl;
	p.size = size;
	p.px = Price::FromUnshifted(px);
	p.seq = 1;
	return p;
}

TEST(BookTest, ProcessingSnapshot)
{
	akl::Book book;
	book.SetSymbol("ESH5");
	EXPECT_FALSE(book.HasBid());
	EXPECT_FALSE(book.HasAsk());
	EXPECT_FALSE(book.HasSide<KTN::ORD::KOrderSide::Enum::BUY>());
	EXPECT_FALSE(book.HasSide<KTN::ORD::KOrderSide::Enum::SELL>());

	KT01::DataStructures::MarketDepth::DepthBook snapshot;
	snapshot.Bids[0] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 1, 100, 450000);
	snapshot.Bids[1] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 2, 101, 449975);
	snapshot.Bids[2] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 3, 102, 449950);
	snapshot.Bids[3] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 4, 103, 449925);
	snapshot.Bids[4] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 5, 104, 449900);
	snapshot.Bids[5] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 6, 105, 449875);
	snapshot.Bids[6] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 7, 106, 449850);
	snapshot.Bids[7] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 8, 107, 449825);
	snapshot.Bids[8] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 9, 108, 449800);
	snapshot.Bids[9] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 10, 109, 449775);

	snapshot.Asks[0] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 1, 110, 450025);
	snapshot.Asks[1] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 2, 111, 450050);
	snapshot.Asks[2] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 3, 112, 450075);
	snapshot.Asks[3] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 4, 113, 450100);
	snapshot.Asks[4] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 5, 114, 450125);
	snapshot.Asks[5] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 6, 115, 450150);
	snapshot.Asks[6] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 7, 116, 450175);
	snapshot.Asks[7] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 8, 117, 450200);
	snapshot.Asks[8] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 9, 118, 450225);
	snapshot.Asks[9] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 10, 119, 450250);

	//snapshot.Bids[0] = {KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Bid, 1, 100, Price::FromUnshifted(4500.00), 1, 1};
	// snapshot.Bids.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Bid, 2, 101, Price::FromUnshifted(4499.75), 1});
	// snapshot.Bids.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Bid, 3, 102, Price::FromUnshifted(4499.50), 1});
	// snapshot.Bids.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Bid, 4, 103, Price::FromUnshifted(4499.25), 1});
	// snapshot.Bids.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Bid, 5, 104, Price::FromUnshifted(4499.00), 1});
	// snapshot.Bids.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Bid, 6, 105, Price::FromUnshifted(4498.75), 1});
	// snapshot.Bids.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Bid, 7, 106, Price::FromUnshifted(4498.50), 1});
	// snapshot.Bids.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Bid, 8, 107, Price::FromUnshifted(4498.25), 1});
	// snapshot.Bids.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Bid, 9, 108, Price::FromUnshifted(4498.00), 1});
	// snapshot.Bids.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Bid, 10, 109, Price::FromUnshifted(4497.75), 1});

	// snapshot.Asks.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Ask, 1, 110, Price::FromUnshifted(4500.25), 1});
	// snapshot.Asks.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Ask, 2, 111, Price::FromUnshifted(4500.50), 1});
	// snapshot.Asks.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Ask, 3, 112, Price::FromUnshifted(4500.75), 1});
	// snapshot.Asks.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Ask, 4, 113, Price::FromUnshifted(4501.00), 1});
	// snapshot.Asks.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Ask, 5, 114, Price::FromUnshifted(4501.25), 1});
	// snapshot.Asks.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Ask, 6, 115, Price::FromUnshifted(4501.50), 1});
	// snapshot.Asks.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Ask, 7, 116, Price::FromUnshifted(4501.75), 1});
	// snapshot.Asks.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Ask, 8, 117, Price::FromUnshifted(4502.00), 1});
	// snapshot.Asks.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Ask, 9, 118, Price::FromUnshifted(4502.25), 1});
	// snapshot.Asks.push_back(pxl{KT01::DataStructures::MarketDepth::BookAction::New, KT01::DataStructures::MarketDepth::BookSide::Ask, 10, 119, Price::FromUnshifted(4502.50), 1});

	
	book.Process(snapshot);

	EXPECT_TRUE(book.HasBid());
	EXPECT_TRUE(book.HasAsk());
	EXPECT_TRUE(book.HasSide<KTN::ORD::KOrderSide::Enum::BUY>());
	EXPECT_TRUE(book.HasSide<KTN::ORD::KOrderSide::Enum::SELL>());

	EXPECT_EQ(book.BidCount(), 10);
	EXPECT_EQ(book.AskCount(), 10);
	EXPECT_EQ(book.LevelCount<KTN::ORD::KOrderSide::Enum::BUY>(), 10);
	EXPECT_EQ(book.LevelCount<KTN::ORD::KOrderSide::Enum::SELL>(), 10);

	EXPECT_EQ(book.GetBidLevel(0), (akl::Level{QTY(100), PX(450000)}));
	EXPECT_EQ(book.GetBidLevel(1), (akl::Level{QTY(101), PX(449975)}));
	EXPECT_EQ(book.GetBidLevel(2), (akl::Level{QTY(102), PX(449950)}));
	EXPECT_EQ(book.GetBidLevel(3), (akl::Level{QTY(103), PX(449925)}));
	EXPECT_EQ(book.GetBidLevel(4), (akl::Level{QTY(104), PX(449900)}));
	EXPECT_EQ(book.GetBidLevel(5), (akl::Level{QTY(105), PX(449875)}));
	EXPECT_EQ(book.GetBidLevel(6), (akl::Level{QTY(106), PX(449850)}));
	EXPECT_EQ(book.GetBidLevel(7), (akl::Level{QTY(107), PX(449825)}));
	EXPECT_EQ(book.GetBidLevel(8), (akl::Level{QTY(108), PX(449800)}));
	EXPECT_EQ(book.GetBidLevel(9), (akl::Level{QTY(109), PX(449775)}));

	EXPECT_EQ(book.GetAskLevel(0), (akl::Level{QTY(110), PX(450025)}));
	EXPECT_EQ(book.GetAskLevel(1), (akl::Level{QTY(111), PX(450050)}));
	EXPECT_EQ(book.GetAskLevel(2), (akl::Level{QTY(112), PX(450075)}));
	EXPECT_EQ(book.GetAskLevel(3), (akl::Level{QTY(113), PX(450100)}));
	EXPECT_EQ(book.GetAskLevel(4), (akl::Level{QTY(114), PX(450125)}));
	EXPECT_EQ(book.GetAskLevel(5), (akl::Level{QTY(115), PX(450150)}));
	EXPECT_EQ(book.GetAskLevel(6), (akl::Level{QTY(116), PX(450175)}));
	EXPECT_EQ(book.GetAskLevel(7), (akl::Level{QTY(117), PX(450200)}));
	EXPECT_EQ(book.GetAskLevel(8), (akl::Level{QTY(118), PX(450225)}));
	EXPECT_EQ(book.GetAskLevel(9), (akl::Level{QTY(119), PX(450250)}));

	EXPECT_EQ(book.BestBid(), PX(450000));
	EXPECT_EQ(book.BestAsk(), PX(450025));
	EXPECT_EQ(book.BestPrice<KTN::ORD::KOrderSide::Enum::BUY>(), PX(450000));
	EXPECT_EQ(book.BestPrice<KTN::ORD::KOrderSide::Enum::SELL>(), PX(450025));
	EXPECT_EQ(book.BestBidQuantity(), QTY(100));
	EXPECT_EQ(book.BestAskQuantity(), QTY(110));
	EXPECT_EQ(book.BestQuantity<KTN::ORD::KOrderSide::Enum::BUY>(), QTY(100));
	EXPECT_EQ(book.BestQuantity<KTN::ORD::KOrderSide::Enum::SELL>(), QTY(110));

}

TEST(BookTest, ProcessingBookUpdate)
{
	akl::Book book;
	book.SetSymbol("ESH5");
	EXPECT_FALSE(book.HasBid());
	EXPECT_FALSE(book.HasAsk());
	EXPECT_FALSE(book.HasSide<KTN::ORD::KOrderSide::Enum::BUY>());
	EXPECT_FALSE(book.HasSide<KTN::ORD::KOrderSide::Enum::SELL>());

	// Set up initial book state with a snapshot
	KT01::DataStructures::MarketDepth::DepthBook snapshot;
	snapshot.Bids[0] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 1, 100, 450000);
	snapshot.Bids[1] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 2, 200, 449975);
	snapshot.Bids[2] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 3, 300, 449950);
	snapshot.Bids[3] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 4, 400, 449925);
	snapshot.Bids[4] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 5, 500, 449900);
	snapshot.Bids[5] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 6, 600, 449875);
	snapshot.Bids[6] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 7, 700, 449850);
	snapshot.Bids[7] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 8, 800, 449825);
	snapshot.Bids[8] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 9, 900, 449800);
	snapshot.Bids[9] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 10, 1000, 449775);

	snapshot.Asks[0] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 1, 1100, 450025);
	snapshot.Asks[1] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 2, 1200, 450050);
	snapshot.Asks[2] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 3, 1300, 450075);
	snapshot.Asks[3] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 4, 1400, 450100);
	snapshot.Asks[4] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 5, 1500, 450125);
	snapshot.Asks[5] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 6, 1600, 450150);
	snapshot.Asks[6] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 7, 1700, 450175);
	snapshot.Asks[7] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 8, 1800, 450200);
	snapshot.Asks[8] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 9, 1900, 450225);
	snapshot.Asks[9] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 10, 2000, 450250);

	book.Process(snapshot);
	book.UpdatePrevious();

	// Verify initial book state
	EXPECT_TRUE(book.HasBid());
	EXPECT_TRUE(book.HasAsk());
	EXPECT_TRUE(book.HasSide<KTN::ORD::KOrderSide::Enum::BUY>());
	EXPECT_TRUE(book.HasSide<KTN::ORD::KOrderSide::Enum::SELL>());
	EXPECT_EQ(book.BestBid(), PX(450000));
	EXPECT_EQ(book.BestAsk(), PX(450025));
	EXPECT_EQ(book.BestBidQuantity(), QTY(100));
	EXPECT_EQ(book.BestAskQuantity(), QTY(1100));

	snapshot.LastAgressor = KTN::ORD::KOrderSide::Enum::BUY;
	snapshot.Last = Price::FromUnshifted(450025);
	snapshot.LastSize = 20;

	EXPECT_FALSE(book.IsLtpAdjusted());
	book.Process(static_cast<KTN::ORD::KOrderSide::Enum>(snapshot.LastAgressor), QTY(snapshot.LastSize), snapshot.Last, 1, 0);
	EXPECT_TRUE(book.IsLtpAdjusted());

	EXPECT_EQ(book.BidCount(), 10);
	EXPECT_EQ(book.AskCount(), 10);
	EXPECT_TRUE(book.HasBid());
	EXPECT_TRUE(book.HasAsk());
	EXPECT_EQ(book.HasSide<KTN::ORD::KOrderSide::Enum::BUY>(), book.HasBid());
	EXPECT_EQ(book.HasSide<KTN::ORD::KOrderSide::Enum::SELL>(), book.HasAsk());
	EXPECT_EQ(book.BestBid(), PX(450000.0));
	EXPECT_EQ(book.BestAsk(), PX(450025.0));
	EXPECT_EQ(book.BestPrice<KTN::ORD::KOrderSide::Enum::BUY>(), book.BestBid());
	EXPECT_EQ(book.BestPrice<KTN::ORD::KOrderSide::Enum::SELL>(), book.BestAsk());
	EXPECT_EQ(book.BestBidQuantity(), QTY(100));
	EXPECT_EQ(book.BestAskQuantity(), QTY(1080));
	EXPECT_EQ(book.BestQuantity<KTN::ORD::KOrderSide::Enum::BUY>(), book.BestBidQuantity());
	EXPECT_EQ(book.BestQuantity<KTN::ORD::KOrderSide::Enum::SELL>(), book.BestAskQuantity());
	EXPECT_TRUE(book.HasPreviousBid());
	EXPECT_TRUE(book.HasPreviousAsk());
	EXPECT_EQ(book.HasPreviousSide<KTN::ORD::KOrderSide::Enum::BUY>(), book.HasPreviousBid());
	EXPECT_EQ(book.HasPreviousSide<KTN::ORD::KOrderSide::Enum::SELL>(), book.HasPreviousAsk());
	EXPECT_EQ(book.PreviousBestBid(), PX(450000.0));
	EXPECT_EQ(book.PreviousBestAsk(), PX(450025.0));
	EXPECT_EQ(book.PreviousBestPrice<KTN::ORD::KOrderSide::Enum::BUY>(), book.PreviousBestBid());
	EXPECT_EQ(book.PreviousBestPrice<KTN::ORD::KOrderSide::Enum::SELL>(), book.PreviousBestAsk());
	EXPECT_EQ(book.PreviousBestBidQuantity(), QTY(100));
	EXPECT_EQ(book.PreviousBestAskQuantity(), QTY(1100));
	EXPECT_EQ(book.PreviousBestQuantity<KTN::ORD::KOrderSide::Enum::BUY>(), book.PreviousBestBidQuantity());
	EXPECT_EQ(book.PreviousBestQuantity<KTN::ORD::KOrderSide::Enum::SELL>(), book.PreviousBestAskQuantity());
	
	// Check all bid levels
	EXPECT_EQ(book.GetBidLevel(0), (akl::Level{QTY(100), PX(450000)}));
	EXPECT_EQ(book.GetBidLevel(1), (akl::Level{QTY(200), PX(449975)}));
	EXPECT_EQ(book.GetBidLevel(2), (akl::Level{QTY(300), PX(449950)}));
	EXPECT_EQ(book.GetBidLevel(3), (akl::Level{QTY(400), PX(449925)}));
	EXPECT_EQ(book.GetBidLevel(4), (akl::Level{QTY(500), PX(449900)}));
	EXPECT_EQ(book.GetBidLevel(5), (akl::Level{QTY(600), PX(449875)}));
	EXPECT_EQ(book.GetBidLevel(6), (akl::Level{QTY(700), PX(449850)}));
	EXPECT_EQ(book.GetBidLevel(7), (akl::Level{QTY(800), PX(449825)}));
	EXPECT_EQ(book.GetBidLevel(8), (akl::Level{QTY(900), PX(449800)}));
	EXPECT_EQ(book.GetBidLevel(9), (akl::Level{QTY(1000), PX(449775)}));

	// Check all ask levels, with first level quantity adjusted by the trade
	EXPECT_EQ(book.GetAskLevel(0), (akl::Level{QTY(1080), PX(450025)}));
	EXPECT_EQ(book.GetAskLevel(1), (akl::Level{QTY(1200), PX(450050)}));
	EXPECT_EQ(book.GetAskLevel(2), (akl::Level{QTY(1300), PX(450075)}));
	EXPECT_EQ(book.GetAskLevel(3), (akl::Level{QTY(1400), PX(450100)}));
	EXPECT_EQ(book.GetAskLevel(4), (akl::Level{QTY(1500), PX(450125)}));
	EXPECT_EQ(book.GetAskLevel(5), (akl::Level{QTY(1600), PX(450150)}));
	EXPECT_EQ(book.GetAskLevel(6), (akl::Level{QTY(1700), PX(450175)}));
	EXPECT_EQ(book.GetAskLevel(7), (akl::Level{QTY(1800), PX(450200)}));
	EXPECT_EQ(book.GetAskLevel(8), (akl::Level{QTY(1900), PX(450225)}));
	EXPECT_EQ(book.GetAskLevel(9), (akl::Level{QTY(2000), PX(450250)}));
}

TEST(BookTest, ProcessingSellLtp)
{
	akl::Book book;
	book.SetSymbol("ESH5");
	EXPECT_FALSE(book.HasBid());
	EXPECT_FALSE(book.HasAsk());

	// Set up initial book state with a snapshot
	KT01::DataStructures::MarketDepth::DepthBook snapshot;
	snapshot.Bids[0] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 1, 100, 450000);
	snapshot.Bids[1] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 2, 200, 449975);
	snapshot.Bids[2] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 3, 300, 449950);
	snapshot.Bids[3] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 4, 400, 449925);
	snapshot.Bids[4] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 5, 500, 449900);
	snapshot.Bids[5] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 6, 600, 449875);
	snapshot.Bids[6] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 7, 700, 449850);
	snapshot.Bids[7] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 8, 800, 449825);
	snapshot.Bids[8] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 9, 900, 449800);
	snapshot.Bids[9] = createNew(KT01::DataStructures::MarketDepth::BookSide::Bid, 10, 1000, 449775);

	snapshot.Asks[0] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 1, 1100, 450025);
	snapshot.Asks[1] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 2, 1200, 450050);
	snapshot.Asks[2] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 3, 1300, 450075);
	snapshot.Asks[3] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 4, 1400, 450100);
	snapshot.Asks[4] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 5, 1500, 450125);
	snapshot.Asks[5] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 6, 1600, 450150);
	snapshot.Asks[6] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 7, 1700, 450175);
	snapshot.Asks[7] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 8, 1800, 450200);
	snapshot.Asks[8] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 9, 1900, 450225);
	snapshot.Asks[9] = createNew(KT01::DataStructures::MarketDepth::BookSide::Ask, 10, 2000, 450250);

	book.Process(snapshot);
	book.UpdatePrevious();
	EXPECT_FALSE(book.IsLtpAdjusted());

	// Process a sell trade (SELL agressor)
	snapshot.LastAgressor = KTN::ORD::KOrderSide::Enum::SELL;
	snapshot.Last = Price::FromUnshifted(450000);
	snapshot.LastSize = 20;

	book.Process(static_cast<KTN::ORD::KOrderSide::Enum>(snapshot.LastAgressor), QTY(snapshot.LastSize), snapshot.Last, 1, 0);
	EXPECT_TRUE(book.IsLtpAdjusted());

	// Verify book state after trade
	EXPECT_EQ(book.BidCount(), 10);
	EXPECT_EQ(book.AskCount(), 10);
	EXPECT_TRUE(book.HasBid());
	EXPECT_TRUE(book.HasAsk());
	EXPECT_EQ(book.BestBid(), PX(450000.0));
	EXPECT_EQ(book.BestAsk(), PX(450025.0));
	EXPECT_EQ(book.BestBidQuantity(), QTY(80));
	EXPECT_EQ(book.BestAskQuantity(), QTY(1100));
	
	// Verify previous book state
	EXPECT_TRUE(book.HasPreviousBid());
	EXPECT_TRUE(book.HasPreviousAsk());
	EXPECT_EQ(book.PreviousBestBid(), PX(450000.0));
	EXPECT_EQ(book.PreviousBestAsk(), PX(450025.0));
	EXPECT_EQ(book.PreviousBestBidQuantity(), QTY(100));
	EXPECT_EQ(book.PreviousBestAskQuantity(), QTY(1100));

	// Check all bid levels, with first level quantity adjusted by the trade
	EXPECT_EQ(book.GetBidLevel(0), (akl::Level{QTY(80), PX(450000)}));
	EXPECT_EQ(book.GetBidLevel(1), (akl::Level{QTY(200), PX(449975)}));
	EXPECT_EQ(book.GetBidLevel(2), (akl::Level{QTY(300), PX(449950)}));
	EXPECT_EQ(book.GetBidLevel(3), (akl::Level{QTY(400), PX(449925)}));
	EXPECT_EQ(book.GetBidLevel(4), (akl::Level{QTY(500), PX(449900)}));
	EXPECT_EQ(book.GetBidLevel(5), (akl::Level{QTY(600), PX(449875)}));
	EXPECT_EQ(book.GetBidLevel(6), (akl::Level{QTY(700), PX(449850)}));
	EXPECT_EQ(book.GetBidLevel(7), (akl::Level{QTY(800), PX(449825)}));
	EXPECT_EQ(book.GetBidLevel(8), (akl::Level{QTY(900), PX(449800)}));
	EXPECT_EQ(book.GetBidLevel(9), (akl::Level{QTY(1000), PX(449775)}));

	// Check all ask levels
	EXPECT_EQ(book.GetAskLevel(0), (akl::Level{QTY(1100), PX(450025)}));
	EXPECT_EQ(book.GetAskLevel(1), (akl::Level{QTY(1200), PX(450050)}));
	EXPECT_EQ(book.GetAskLevel(2), (akl::Level{QTY(1300), PX(450075)}));
	EXPECT_EQ(book.GetAskLevel(3), (akl::Level{QTY(1400), PX(450100)}));
	EXPECT_EQ(book.GetAskLevel(4), (akl::Level{QTY(1500), PX(450125)}));
	EXPECT_EQ(book.GetAskLevel(5), (akl::Level{QTY(1600), PX(450150)}));
	EXPECT_EQ(book.GetAskLevel(6), (akl::Level{QTY(1700), PX(450175)}));
	EXPECT_EQ(book.GetAskLevel(7), (akl::Level{QTY(1800), PX(450200)}));
	EXPECT_EQ(book.GetAskLevel(8), (akl::Level{QTY(1900), PX(450225)}));
	EXPECT_EQ(book.GetAskLevel(9), (akl::Level{QTY(2000), PX(450250)}));
}


}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}