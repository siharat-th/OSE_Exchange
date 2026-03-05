#include <gtest/gtest.h>

#include <Algos/AklStacker/AklStacker.hpp>
#include <Algos/AklStacker/Instrument.hpp>
#include <Algos/AklStacker/messages/StackerRequoteMessage.hpp>
#include "AlgoParams/AlgoInitializer.hpp"
#include "impl/ExchangeSender.hpp"

#include "impl/StrategyTester.hpp"

#include <Algos/AklStacker/messages/StackerClearPositionMessage.hpp>
#include <Algos/AklStacker/messages/StackInfoMessage.hpp>
#include <Algos/AklStacker/messages/ProrataProfileUpdateMessage.hpp>
#include <Algos/AklStacker/messages/ProrataProfileDeleteMessage.hpp>

#include <Algos/AklStacker/PriceConverter.hpp>
#include <Algos/AklStacker/PositionManager.hpp>

#include <akl/ProrataProfileManager.hpp>

namespace akl::test
{

	const char* config =
	"[{"
		"\"topic\": \"ALGOPARAMS,\", "
		"\"source\": \"BATMAN,\", "
		"\"Strategy\": \"765,\", "
		"\"Requote\": 0, "
		"\"ClearPosition\": 0, "
		"\"Adopt\": 0, "
		"\"NewOrders\": true, "
		"\"BidQty\": 5, "
		"\"BidLength\": 3, "
		"\"BidLean\": 200, "
		"\"BidQuickAddLean\": 100, "
		"\"BidQuickAddTime\": 100, "
		"\"BidQuickAddEnabled\": false, "
		"\"BidManualAdj\": 0, "
		"\"BidQty2\": 0, "
		"\"BidLength2\": 0, "
		"\"BidLean2\": 100, "
		"\"BidQuickAddLean2\": 100, "
		"\"BidQuickAddTime2\": 0, "
		"\"BidQuickAddEnabled2\": false, "
		"\"BidManualAdj2\": 1, "
		"\"BidPayup\": 150, "
		"\"BidWinnerPayup\": 100, "
		"\"BidMaxPayupTicks\": 0, "
		"\"BidTimeoutCover\": 10000, "
		"\"AskQty\": 6, "
		"\"AskLength\": 2, "
		"\"AskLean\": 200, "
		"\"AskQuickAddLean\": 100, "
		"\"AskQuickAddTime\": 100, "
		"\"AskQuickAddEnabled\": false, "
		"\"AskManualAdj\": 0, "
		"\"AskQty2\": 0, "
		"\"AskLength2\": 0, "
		"\"AskLean2\": 100, "
		"\"AskQuickAddLean2\": 100, "
		"\"AskQuickAddTime2\": 0, "
		"\"AskQuickAddEnabled2\": false, "
		"\"AskManualAdj2\": 1, "
		"\"AskPayup\": 150, "
		"\"AskWinnerPayup\": 100, "
		"\"AskMaxPayupTicks\": 0, "
		"\"AskTimeoutCover\": 10000, "
		"\"AskProrataEnabled\": false, "
		"\"BidProrataEnabled\": false, "
		"\"AskProrataProfile\": \"ZC_1-2_Normal\", "
		"\"BidProrataProfile\": \"ZC_1-2_Normal\", "
		"\"AskNoAggressing\": true, "
		"\"AskNoAggressing2\": false, "
		"\"BidNoAggressing\": true, "
		"\"BidNoAggressing2\": false, "
		"\"RandomLotSizeMult\": 0, "
		"\"MoveWithCancelNew\": true, "
		"\"RequoteOnMove\": false, "
		"\"IgnoreMarketState\": false, "
		"\"MaxOrderQty\": 5000, "
		"\"MaxPosition\": 20000, "
		"\"RateLimit\": 1000, "
		"\"RateLimitWindow\": 1000, "
		"\"PreventAlgoCancelWhenWorkingOrders\": true, "
		"\"AdoptFromUnknownStrategies\": true, "
		"\"HedgeOnLargeTrade\": true, "
		"\"HedgeOnLargeFill\": true, "
		"\"CancelUnadoptedOrders\": false, "
		"\"AskAddHedgeQty\": 0, "
		"\"BidAddHedgeQty\": 0, "
		"\"AskAltProrataProfileDelay\": 0, "
		"\"BidAltProrataProfileDelay\": 0, "
		"\"ProrataGroup\": \"ZC\", "
		"\"ProrataAllocation\": 0.6, "
		"\"LeanPayupIncrement\": 100, "
		"\"LinkLeanPayup\": true, "
		"\"SecondStackEnabled\": false, "
		"\"ShowProrata\": true, "
		"\"StopQuantity1\": 1, "
		"\"StopQuantity2\": 5, "
		"\"StopQuantity3\": 10, "
		"\"StopQuantity4\": 50, "
		"\"StopQuantity5\": 100, "
		"\"StopTrigger1\": 1, "
		"\"StopTrigger2\": 5, "
		"\"StopTrigger3\": 10, "
		"\"StopTrigger4\": 50, "
		"\"StopTrigger5\": 100, "
		"\"StopLean1\": 1, "
		"\"StopLean2\": 5, "
		"\"StopLean3\": 10, "
		"\"StopLean4\": 50, "
		"\"StopLean5\": 100, "
		"\"MiddleClickQuantity\": 400, "
		"\"MiddleClickTrigger\": 400, "
		"\"StopQuantityMax\": 5000, "
		"\"FlipDelay\": 1000, "
		"\"TriggerQuoteDelay\": 1000, "
		"\"DeleteOnFlipDefault\": true, "
		"\"ShowLeanColumn\": true, "
		"\"ShowTradeColumn\": false, "
		"\"StackRequoteDelay\": 5000, "
		"\"StackRequoteCount\": 0, "
		"\"ProrataRequoteCount\": 0, "
		"\"AskDynamicProfile\": \"\", "
		"\"BidDynamicProfile\": \"\", "
		"\"AskDynamicEnabled\": false, "
		"\"BidDynamicEnabled\": false, "
		"\"DynamicStack\": false, "
		"\"AskMaxPrice\": \"\", "
		"\"AskMinPrice\": \"\", "
		"\"BidMaxPrice\": \"-2'0\", "
		"\"BidMinPrice\": \"\", "
		"\"AskMaxPrice2\": \"\", "
		"\"AskMinPrice2\": \"\", "
		"\"BidMaxPrice2\": \"\", "
		"\"BidMinPrice2\": \"\", "
		"\"AskAltPrice\": \"\", "
		"\"AskAltPrice2\": \"\", "
		"\"BidAltPrice\": \"\", "
		"\"BidAltPrice2\": \"\", "
		"\"AskAltManualAdj\": 1, "
		"\"AskAltManualAdj2\": 1, "
		"\"BidAltManualAdj\": 1, "
		"\"BidAltManualAdj2\": 1, "
		"\"ShowAltManualAdj\": false, "
		"\"Public\":true, "
		"\"User\":\"martin\", "
		"\"Org\":\"AKL\", "
		"\"GroupName\":\"AKL\", "
		"\"Enabled\":1, "
		"\"TestMode\":false, "
		"\"TemplateName\":\"StackerTimer\", "
		"\"FileName\":null, "
		"\"AlgoType\":34, "
		"\"ResetPosition\": false, "
		"\"isOrderAlgo\":false, "
		"\"Tag\":\"STACK\", "
		"\"Legs\": [{"
			"\"LegId\": 0, "
			"\"Exch\": \"cme\", "
			"\"Symbol\": \"ESM5\", "
			"\"ProdType\": \"future\", "
			"\"Inverse\": false, "
			"\"PriceMult\": 0, "
			"\"SpreadMult\": 0, "
			"\"ActiveQuoting\": false"
		"}], "
		"\"Source\": \"BATMAN\", "
		"\"UniqueId\": \"23ROZ5\""
	"}]";

class ProrataTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	ProrataTest()
	{
		
	}

	virtual void SetUp() override
	{
		OrderNums::instance().Refresh();
		AlgoInitializer params;
		params.name = "AklStackerTest";
		params.guid = "1";
		params.ordersender = &e;
		params.legs = LegParamParser::ParseSpreadLegs(config);
		strategy = new StrategyTester<akl::stacker::AklStacker>(params);

		const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef(params.legs[0].symbol);
		priceConverter = new stacker::PriceConverter(def);

		settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1000);
		settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(1000);
		settings.meta.Enabled = 1;
		settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
		settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
		settings.quoteInstrumentSettings.quoteTimeout = 0;
		settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	}

	virtual void TearDown() override
	{
		strategy->Shutdown();
		akl::ProrataProfileManager::instance().Clear();
		delete strategy;
		strategy = nullptr;
	}

	std::string getJson(const stacker::AklStackerParams &settings)
	{
		return settings.toJson();
	}

	impl::ExchangeSender e;
	StrategyTester<akl::stacker::AklStacker> *strategy { nullptr };
	using Side = KTN::ORD::KOrderSide::Enum;

	stacker::AklStackerParams settings;
	const stacker::PriceConverter *priceConverter { nullptr };

	template<typename MESSAGE_TYPE>
	bool ReadNoMessage()
	{
		stacker::StrategyMessagePublisher &publisher = strategy->GetPublisher();
		while (!publisher.IsMessageQueueEmpty())
		{
			stacker::StrategyMessagePublisher::MessageVariant msg = publisher.Pop();
			if (std::holds_alternative<MESSAGE_TYPE>(msg))
			{
				return false;
			}
		}
		return true;
	}

	template<typename MESSAGE_TYPE>
	bool ReadMessage(MESSAGE_TYPE &msg)
	{
		stacker::StrategyMessagePublisher &publisher = strategy->GetPublisher();
		while (!publisher.IsMessageQueueEmpty())
		{
			stacker::StrategyMessagePublisher::MessageVariant m = publisher.Pop();
			if (std::holds_alternative<MESSAGE_TYPE>(m))
			{
				msg = std::get<MESSAGE_TYPE>(m);
				return true;
			}
		}
		return false;
	}

	void InitProrataProfile(
		const char *name,
		const std::array<int, 10> bookQtys,
		const std::array<int, 10> quoteQtys,
		const std::array<int, 10> ltpQtys,
		const std::array<int, 10> windows,
		int count,
		const char *group = "ES")
	{
		stacker::ProrataProfileUpdateMessage msg;
		msg.name = name;
		msg.levelCount = count;
		for (int i = 0; i < bookQtys.size(); ++i)
		{
			msg.bookQuantity[i] = bookQtys[i];
		}
		for (int i = 0; i < quoteQtys.size(); ++i)
		{
			msg.quoteQuantity[i] = quoteQtys[i];
		}
		for (int i = 0; i < ltpQtys.size(); ++i)
		{
			msg.ltpQuantity[i] = ltpQtys[i];
		}
		for (int i = 0; i < windows.size(); ++i)
		{
			msg.window[i] = windows[i];
		}
		msg.msgIndex = 0;
		msg.msgCount = 1;
		msg.group = group;
		akl::ProrataProfileManager::instance().UpdateProfile(msg);
	}

	void InitProrataProfile(
		const char *name,
		const std::array<int, 10> bookQtys,
		const std::array<int, 10> quoteQtys,
		const std::array<int, 10> ltpQtys,
		const std::array<int, 10> windows,
		const std::array<int, 10> addHedgeQtys,
		const std::array<int, 10> addHedgeEnabled,
		int count,
		const char *group = "ES")
	{
		stacker::ProrataProfileUpdateMessage msg;
		msg.name = name;
		msg.levelCount = count;
		for (int i = 0; i < bookQtys.size(); ++i)
		{
			msg.bookQuantity[i] = bookQtys[i];
		}
		for (int i = 0; i < quoteQtys.size(); ++i)
		{
			msg.quoteQuantity[i] = quoteQtys[i];
		}
		for (int i = 0; i < ltpQtys.size(); ++i)
		{
			msg.ltpQuantity[i] = ltpQtys[i];
		}
		for (int i = 0; i < windows.size(); ++i)
		{
			msg.window[i] = windows[i];
		}
		for (int i = 0; i < addHedgeQtys.size(); ++i)
		{
			msg.addHedgeQuantity[i] = addHedgeQtys[i];
		}
		for (int i = 0; i < addHedgeEnabled.size(); ++i)
		{
			msg.addHedgeEnabled[i] = addHedgeEnabled[i];
		}
		msg.msgIndex = 0;
		msg.msgCount = 1;
		msg.group = group;
		akl::ProrataProfileManager::instance().UpdateProfile(msg);
	}

	void UpdateProrataProfile(
		const char *name,
		const std::array<int, 10> bookQtys,
		const std::array<int, 10> quoteQtys,
		const std::array<int, 10> ltpQtys,
		const std::array<int, 10> windows,
		int count,
		const char *group = "ES")
	{
		stacker::ProrataProfileUpdateMessage msg;
		msg.name = name;
		msg.levelCount = count;
		for (int i = 0; i < bookQtys.size(); ++i)
		{
			msg.bookQuantity[i] = bookQtys[i];
		}
		for (int i = 0; i < quoteQtys.size(); ++i)
		{
			msg.quoteQuantity[i] = quoteQtys[i];
		}
		for (int i = 0; i < ltpQtys.size(); ++i)
		{
			msg.ltpQuantity[i] = ltpQtys[i];
		}
		for (int i = 0; i < windows.size(); ++i)
		{
			msg.window[i] = windows[i];
		}
		msg.msgIndex = 0;
		msg.msgCount = 1;
		msg.group = group;
		strategy->InjectMessage(msg);
	}

	void UpdateProrataProfile(
		const char *name,
		const std::array<int, 10> bookQtys,
		const std::array<int, 10> quoteQtys,
		const std::array<int, 10> ltpQtys,
		const std::array<int, 10> windows,
		const std::array<int, 10> addHedgeQtys,
		const std::array<int, 10> addHedgeEnabled,
		int count,
		const char *group = "ES")
	{
		stacker::ProrataProfileUpdateMessage msg;
		msg.name = name;
		msg.levelCount = count;
		for (int i = 0; i < bookQtys.size(); ++i)
		{
			msg.bookQuantity[i] = bookQtys[i];
		}
		for (int i = 0; i < quoteQtys.size(); ++i)
		{
			msg.quoteQuantity[i] = quoteQtys[i];
		}
		for (int i = 0; i < ltpQtys.size(); ++i)
		{
			msg.ltpQuantity[i] = ltpQtys[i];
		}
		for (int i = 0; i < windows.size(); ++i)
		{
			msg.window[i] = windows[i];
		}
		for (int i = 0; i < addHedgeQtys.size(); ++i)
		{
			msg.addHedgeQuantity[i] = addHedgeQtys[i];
		}
		for (int i = 0; i < addHedgeEnabled.size(); ++i)
		{
			msg.addHedgeEnabled[i] = addHedgeEnabled[i];
		}
		msg.msgIndex = 0;
		msg.msgCount = 1;
		msg.group = group;
		strategy->InjectMessage(msg);
	}
};

TEST_F(ProrataTest, QuoteBidAndCancel)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod bid;
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteAskAndCancel)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod ask;
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteBidModifyAndCancel)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod bid;
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	UpdateProrataProfile("P1", {1000}, {200}, {0}, {0}, 1);
	EXPECT_TRUE(e.ReadQtyModify(bid, 200));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteAskModifyAndCancel)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod ask;
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	UpdateProrataProfile("P1", {1000}, {200}, {0}, {0}, 1);
	EXPECT_TRUE(e.ReadQtyModify(ask, 200));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteBidAndCancelOnProfileDelete)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod bid;
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::ProrataProfileDeleteMessage {"P1"});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	UpdateProrataProfile("P1", {1000}, {200}, {0}, {0}, 1);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteAskAndCancelOnProfileDelete)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod ask;
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::ProrataProfileDeleteMessage {"P1"});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	UpdateProrataProfile("P1", {1000}, {200}, {0}, {0}, 1);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteBidOnQuoteCommandAndCancel)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteAskOnQuoteCommandAndCancel)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteBidOnQuoteCommandAndCancel2)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteAskOnQuoteCommandAndCancel2)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteBidThenQuoteAnother)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteAskThenQuoteAnother)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{2000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteTwoBidsInitially)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteTwoAsksInitially)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{2000, 450025}});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, SwitchBidProfile)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	// Switch profiles
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P2";
	strategy->onJson(getJson(settings));
	
	// Should modify the quantity of the existing order
	EXPECT_TRUE(e.ReadQtyModify(bid, 200));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, SwitchAskProfile)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	// Switch profiles
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P2";
	strategy->onJson(getJson(settings));
	
	// Should modify the quantity of the existing order
	EXPECT_TRUE(e.ReadQtyModify(ask, 200));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, SwitchBidProfile2)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 200}, {0}, {0}, 2);
	InitProrataProfile("P2", {1000, 2000}, {200, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	// Switch profiles
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P2";
	strategy->onJson(getJson(settings));
	
	// Should modify the quantities of the existing orders
	EXPECT_TRUE(e.ReadQtyModify(bid[1], 100));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectModifyQtyAck(bid[1], 100);
	EXPECT_TRUE(e.ReadQtyModify(bid[0], 200));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, SwitchAskProfile2)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 200}, {0}, {0}, 2);
	InitProrataProfile("P2", {1000, 2000}, {200, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	// Switch profiles
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P2";
	strategy->onJson(getJson(settings));
	
	// Should modify the quantities of the existing orders
	EXPECT_TRUE(e.ReadQtyModify(ask[1], 100));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectModifyQtyAck(ask[1], 100);
	EXPECT_TRUE(e.ReadQtyModify(ask[0], 200));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, SwitchBidProfileToIncreaseNumberOfOrders)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000, 2000}, {100, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	// Switch profiles to one with more levels
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P2";
	strategy->onJson(getJson(settings));

	// Should add a second order
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, SwitchAskProfileToIncreaseNumberOfOrders)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000, 2000}, {100, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	// Switch profiles to one with more levels
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P2";
	strategy->onJson(getJson(settings));

	// Should add a second order
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, SwitchBidProfileToDecreaseNumberOfOrders)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000, 2000}, {100, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P2";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	// Switch profiles to one with fewer levels
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	strategy->onJson(getJson(settings));

	// Should cancel the extra order
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, SwitchAskProfileToDecreaseNumberOfOrders)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000, 2000}, {100, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P2";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	// Switch profiles to one with fewer levels
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	strategy->onJson(getJson(settings));

	// Should cancel the extra order
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteMultipleBidsAndCancel)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 200}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteMultipleBidsAndCancelSome)
{
	InitProrataProfile("P1", {1000, 2000, 3000}, {100, 200, 300}, {0}, {0}, 3);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{3000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1500, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadCancel(bid[2]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{3000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, CancelBidThenFullFill)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid));
	strategy->InjectFill(bid, 100);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, CancelAskThenFullFill)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask));
	strategy->InjectFill(ask, 100);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, CancelBidAfterLargeTrade)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{500, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 449975}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, CancelAskAfterLargeTrade)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{500, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, CancelBidAfterLargeTrade2)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 499, 450000);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 1, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{500, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, CancelAskAfterLargeTrade2)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 499, 450025);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 1, 450025);
	EXPECT_TRUE(e.ReadCancel(ask));
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{500, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, CancelBidAndHedgeAfterLargeTrade)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 49);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	strategy->InjectCancelAck(bid);
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 49, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{500, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, CancelAskAndHedgeAfterLargeTrade)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 49);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask));
	strategy->InjectCancelAck(ask);
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 49, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{500, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, CancelBidAfterLargeTradeAndHedgeFillsAfterCancel)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectFill(bid, 49);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 49, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid);

	strategy->InjectDepth({{500, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, CancelAskAfterLargeTradeAndHedgeFillsAfterCancel)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectFill(ask, 49);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 49, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid);

	strategy->InjectDepth({{2000, 450000}}, {{500, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeBidTradeAfterProrataCancel)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	KTN::OrderPod hedge1;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge1));
	strategy->InjectAck(hedge1);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 5);
	KTN::OrderPod hedge2;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5, 450000, hedge2));
	strategy->InjectAck(hedge2);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeAskTradeAfterProrataCancel)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	KTN::OrderPod hedge1;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge1));
	strategy->InjectAck(hedge1);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 5);
	KTN::OrderPod hedge2;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5, 450025, hedge2));
	strategy->InjectAck(hedge2);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeFillOnBid)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 50);
	EXPECT_TRUE(e.ReadCancel(bid));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 50, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeFillOnAsk)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 50);
	EXPECT_TRUE(e.ReadCancel(ask));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 50, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeFillOnBid2)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {1}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 49);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 1);
	EXPECT_TRUE(e.ReadCancel(bid));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 50, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 5);
	KTN::OrderPod hedge2;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5, 450000, hedge2));
	strategy->InjectAck(hedge2);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeFillOnAsk2)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 49);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 1);
	EXPECT_TRUE(e.ReadCancel(ask));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 50, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 5);
	KTN::OrderPod hedge2;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5, 450025, hedge2));
	strategy->InjectAck(hedge2);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeFillOnBidWithProrataFifoSplit)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {1}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataAllocation = 0.6;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 29);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 1);
	EXPECT_TRUE(e.ReadCancel(bid));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 30, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeFillOnAskWithProrataFifoSplit)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataAllocation = 0.6;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 29);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 1);
	EXPECT_TRUE(e.ReadCancel(ask));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 30, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

// TODO add hedge tests?

TEST_F(ProrataTest, CancelBidWithStackOrderAfterLargeTrade)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bidStack[2];
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bidStack[0]));
	strategy->InjectAck(bidStack[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bidStack[1]));
	strategy->InjectAck(bidStack[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadCancel(bidStack[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{499, 450000}, {2000, 449975}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bidStack[0]);
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 449975}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bidStack[0]));
	strategy->InjectAck(bidStack[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, DontQuoteAnyMoreBidsAfterOneWasCanceled)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	// Don't expect a submit here because we're not quoting more bids after one was canceled
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, DontQuoteAnyMoreAsksAfterOneWasCanceled)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {0}, {0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	strategy->InjectDepth({{1000, 450000}}, {{2000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());
}

// TODO more add hedge tests?

TEST_F(ProrataTest, BidHedgeOnLargeTradeOff)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = false;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 50);
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 50, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{500, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 449975}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, AskHedgeOnLargeTradeOff)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = false;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 50);
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 50, 450025));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{500, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, BidHedgeOnLargeFillOff)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = false;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 50);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 50, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{500, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 449975}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, AskHedgeOnLargeFillOff)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = false;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 50);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 50, 450025));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{500, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

// TODO even more add hedge tests?

TEST_F(ProrataTest, AdoptProrataOrders)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 10;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 10;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *bidInternal = instr->GetOrder(bid.orderReqId);
	const stacker::Order *askInternal = instr->GetOrder(ask.orderReqId);
	ASSERT_NE(bidInternal, nullptr);
	ASSERT_NE(askInternal, nullptr);
	EXPECT_EQ(bidInternal->type, stacker::Order::PRORATA);
	EXPECT_EQ(askInternal->type, stacker::Order::PRORATA);

	const stacker::PositionManager &pm = instr->GetPositionManager();

	EXPECT_EQ(pm.Position(), Quantity(0));
	strategy->InjectFill(bid, 1);
	EXPECT_EQ(pm.Position(), Quantity(1));
	strategy->InjectFill(ask, 1);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectMessage(stacker::StackerProrataOrderAdoptMessage { strategy->StrategyId(), bid.exchordid, Side::BUY, 0, 1, 0 });
	EXPECT_EQ(bidInternal->type, stacker::Order::STACK);
	EXPECT_EQ(askInternal->type, stacker::Order::PRORATA);

	strategy->InjectFill(bid, 1);
	EXPECT_EQ(pm.Position(), Quantity(1));
	strategy->InjectFill(ask, 1);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectMessage(stacker::StackerProrataOrderAdoptMessage { strategy->StrategyId(), ask.exchordid, Side::SELL, 0, 1, 0 });
	EXPECT_EQ(bidInternal->type, stacker::Order::STACK);
	EXPECT_EQ(askInternal->type, stacker::Order::STACK);

	strategy->InjectFill(bid, 1);
	EXPECT_EQ(pm.Position(), Quantity(1));
	strategy->InjectFill(ask, 1);
	EXPECT_EQ(pm.Position(), Quantity(0));

	// Test that the orders move with the market
	strategy->InjectDepth({{500, 450000}, {2000, 449975}}, {{500, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{499, 450000}, {2000, 449975}}, {{499, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {2000, 449975}}, {{1000, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, AbandonThenAdoptProrataOrders)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 10;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 10;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *bidInternal = instr->GetOrder(bid.orderReqId);
	const stacker::Order *askInternal = instr->GetOrder(ask.orderReqId);
	ASSERT_NE(bidInternal, nullptr);
	ASSERT_NE(askInternal, nullptr);
	EXPECT_EQ(bidInternal->type, stacker::Order::PRORATA);
	EXPECT_EQ(askInternal->type, stacker::Order::PRORATA);

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));
	strategy->InjectFill(bid, 1);
	EXPECT_EQ(pm.Position(), Quantity(1));
	strategy->InjectFill(ask, 1);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectMessage(stacker::StackerProrataOrderAbandonMessage { strategy->StrategyId(), bid.exchordid, Side::BUY, 0, 1 });
	EXPECT_EQ(bidInternal->type, stacker::Order::NONE);
	EXPECT_EQ(askInternal->type, stacker::Order::PRORATA);
	EXPECT_EQ(pm.Position(), Quantity(0));
	strategy->InjectFill(bid, 1);
	EXPECT_EQ(pm.Position(), Quantity(0));
	strategy->InjectFill(ask, 1);
	EXPECT_EQ(pm.Position(), Quantity(-1));

	strategy->InjectMessage(stacker::StackerProrataOrderAbandonMessage { strategy->StrategyId(), ask.exchordid, Side::SELL, 0, 1 });
	EXPECT_EQ(bidInternal->type, stacker::Order::NONE);
	EXPECT_EQ(askInternal->type, stacker::Order::NONE);
	EXPECT_EQ(pm.Position(), Quantity(-1));
	strategy->InjectFill(bid, 1);
	EXPECT_EQ(pm.Position(), Quantity(-1));
	strategy->InjectFill(ask, 1);
	EXPECT_EQ(pm.Position(), Quantity(-1));

	strategy->InjectDepth({{500, 450000}, {2000, 449975}}, {{500, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{499, 450000}, {2000, 449975}}, {{499, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{500, 450000}, {2000, 449975}}, {{500, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerAdoptMessage { strategy->StrategyId(), 0 });
	EXPECT_EQ(bidInternal->type, stacker::Order::STACK);
	EXPECT_EQ(askInternal->type, stacker::Order::STACK);
	EXPECT_EQ(pm.Position(), Quantity(-1));
	strategy->InjectFill(bid, 1);
	EXPECT_EQ(pm.Position(), Quantity(0));
	strategy->InjectFill(ask, 1);
	EXPECT_EQ(pm.Position(), Quantity(-1));

	strategy->InjectDepth({{499, 450000}, {2000, 449975}}, {{499, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {2000, 449975}}, {{1000, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

// TODO tests about trade after book update from TT shouldn't be relevant?

TEST_F(ProrataTest, LargeBidTradeAcrossMultipleLevels)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{999, 450000}, {2000, 449975}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 999, 450000);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 1, 449975);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 498, 449975);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 1, 449975);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeAskTradeAcrossMultipleLevels)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{999, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 999, 450025);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 1, 450050);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 498, 450050);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 1, 450050);
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeBidTradeAcrossMultipleLevels2)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}, {2000, 449975}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 1001, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 449975}}, {{2000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 1, 449975);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 498, 449975);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 1, 449975);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeAskTradeAcrossMultipleLevels2)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 1001, 450025);
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 1, 450050);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 498, 450050);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 1, 450050);
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, MoveProrataOrdersToStackOnStart)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *bidInternal = instr->GetOrder(bid.orderReqId);
	const stacker::Order *askInternal = instr->GetOrder(ask.orderReqId);
	ASSERT_NE(bidInternal, nullptr);
	ASSERT_NE(askInternal, nullptr);

	const stacker::Stack &stack = instr->GetStack(0);
	const stacker::ProrataQuoter &pq = instr->GetProrataQuoter();
	EXPECT_EQ(bidInternal->type, stacker::Order::PRORATA);
	EXPECT_EQ(askInternal->type, stacker::Order::PRORATA);
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_TRUE(pq.HasOrder(bidInternal));
	EXPECT_TRUE(pq.HasOrder(askInternal));

	strategy->Stop();
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(bidInternal->type, stacker::Order::NONE);
	EXPECT_EQ(askInternal->type, stacker::Order::NONE);
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_FALSE(pq.HasOrder(bidInternal));
	EXPECT_FALSE(pq.HasOrder(askInternal));

	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(bidInternal->type, stacker::Order::STACK);
	EXPECT_EQ(askInternal->type, stacker::Order::STACK);
	EXPECT_TRUE(stack.HasOrder(bidInternal));
	EXPECT_TRUE(stack.HasOrder(askInternal));
	EXPECT_FALSE(pq.HasOrder(bidInternal));
	EXPECT_FALSE(pq.HasOrder(askInternal));
}

TEST_F(ProrataTest, CancelProrataOrdersToStackOnStart)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *bidInternal = instr->GetOrder(bid.orderReqId);
	const stacker::Order *askInternal = instr->GetOrder(ask.orderReqId);
	ASSERT_NE(bidInternal, nullptr);
	ASSERT_NE(askInternal, nullptr);
	EXPECT_EQ(bidInternal->type, stacker::Order::PRORATA);
	EXPECT_EQ(askInternal->type, stacker::Order::PRORATA);

	const stacker::Stack &stack = instr->GetStack(0);
	const stacker::ProrataQuoter &pq = instr->GetProrataQuoter();
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_TRUE(pq.HasOrder(bidInternal));
	EXPECT_TRUE(pq.HasOrder(askInternal));

	strategy->Stop();
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(bidInternal->type, stacker::Order::NONE);
	EXPECT_EQ(askInternal->type, stacker::Order::NONE);
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_FALSE(pq.HasOrder(bidInternal));
	EXPECT_FALSE(pq.HasOrder(askInternal));

	strategy->InjectDepth({{499, 450000}, {2000, 449975}}, {{499, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->Start();
	impl::ExchangeSender::transaction t[2];
	ASSERT_TRUE(e.GetTransaction(t[0]));
	ASSERT_TRUE(e.GetTransaction(t[1]));
	EXPECT_EQ(t[0].action, KTN::ORD::KOrderAction::Enum::ACTION_CXL);
	EXPECT_EQ(t[1].action, KTN::ORD::KOrderAction::Enum::ACTION_CXL);
	EXPECT_TRUE(t[0].order.orderReqId == bid.orderReqId || t[0].order.orderReqId == ask.orderReqId);
	if (t[0].order.orderReqId == bid.orderReqId)
	{
		EXPECT_TRUE(t[1].order.orderReqId == ask.orderReqId);
	}
	else
	{
		EXPECT_TRUE(t[1].order.orderReqId == bid.orderReqId);
	}
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(bidInternal->type, stacker::Order::NONE);
	EXPECT_EQ(askInternal->type, stacker::Order::NONE);
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_FALSE(pq.HasOrder(bidInternal));
	EXPECT_FALSE(pq.HasOrder(askInternal));
}

TEST_F(ProrataTest, LeaveProrataOrdersToStackOnStart)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *bidInternal = instr->GetOrder(bid.orderReqId);
	const stacker::Order *askInternal = instr->GetOrder(ask.orderReqId);
	ASSERT_NE(bidInternal, nullptr);
	ASSERT_NE(askInternal, nullptr);
	EXPECT_EQ(bidInternal->type, stacker::Order::PRORATA);
	EXPECT_EQ(askInternal->type, stacker::Order::PRORATA);

	const stacker::Stack &stack = instr->GetStack(0);
	const stacker::ProrataQuoter &pq = instr->GetProrataQuoter();
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_TRUE(pq.HasOrder(bidInternal));
	EXPECT_TRUE(pq.HasOrder(askInternal));

	strategy->Stop();
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(bidInternal->type, stacker::Order::NONE);
	EXPECT_EQ(askInternal->type, stacker::Order::NONE);
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_FALSE(pq.HasOrder(bidInternal));
	EXPECT_FALSE(pq.HasOrder(askInternal));

	EXPECT_EQ(bidInternal->type, stacker::Order::NONE);
	EXPECT_EQ(askInternal->type, stacker::Order::NONE);

	strategy->InjectDepth({{499, 450000}, {2000, 449975}}, {{499, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(bidInternal->type, stacker::Order::NONE);
	EXPECT_EQ(askInternal->type, stacker::Order::NONE);

	EXPECT_EQ(bidInternal->type, stacker::Order::NONE);
	EXPECT_EQ(askInternal->type, stacker::Order::NONE);
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_FALSE(pq.HasOrder(bidInternal));
	EXPECT_FALSE(pq.HasOrder(askInternal));
}

TEST_F(ProrataTest, LargeBidTradeWithLongPosition)
{
	InitProrataProfile("P1", {500}, {0}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}, {2000, 449975}}, {{2000, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerPositionUpdateMessage { strategy->StrategyId(), 10 });
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 1);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectFill(ask[0], 9);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeAskTradeWithShortPosition)
{
	InitProrataProfile("P1", {500}, {0}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}, {2000, 449975}}, {{2000, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerPositionUpdateMessage { strategy->StrategyId(), -10 });
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 1);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectFill(bid[0], 9);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeBidTradeWithLongPosition2)
{
	InitProrataProfile("P1", {500}, {0}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}, {2000, 449975}}, {{2000, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerPositionUpdateMessage { strategy->StrategyId(), 5 });
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 4);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectFill(ask[0], 2);
	KTN::OrderPod hedge1;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1, 450025, hedge1));
	strategy->InjectAck(hedge1);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectFill(ask[0], 4);
	KTN::OrderPod hedge2;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 4, 450025, hedge2));
	strategy->InjectAck(hedge2);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeAskTradeWithShortPosition2)
{
	InitProrataProfile("P1", {500}, {0}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}, {2000, 449975}}, {{2000, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerPositionUpdateMessage { strategy->StrategyId(), -5 });
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 4);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectFill(bid[0], 2);
	KTN::OrderPod hedge1;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000, hedge1));
	strategy->InjectAck(hedge1);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectFill(bid[0], 4);
	KTN::OrderPod hedge2;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 4, 450000, hedge2));
	strategy->InjectAck(hedge2);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeBidTradeWithLongPosition3)
{
	InitProrataProfile("P1", {500}, {0}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}, {2000, 449975}}, {{2000, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerPositionUpdateMessage { strategy->StrategyId(), 5 });
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeAskTradeWithShortPosition3)
{
	InitProrataProfile("P1", {500}, {0}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}, {2000, 449975}}, {{2000, 450025}, {2000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerPositionUpdateMessage { strategy->StrategyId(), -5 });
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

// TODO add hedge tests with position

TEST_F(ProrataTest, RequoteBidAfterFill)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 100);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 100);
	strategy->InjectFill(bid, 100);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449950, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, RequoteAskAfterFill)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 100);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask));
	strategy->InjectAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 100);
	strategy->InjectFill(ask, 100);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, RequoteBidAfterLargeTrade)
{
	InitProrataProfile("P1", {1000}, {100}, {200}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 200, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 200, 449975);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid);

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 449975}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449950, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 200, 449950);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid);

	strategy->InjectDepth({{1000, 449950}}, {{1000, 449975}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449925}}, {{1000, 449950}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449925, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 200, 449925);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid);

	strategy->InjectDepth({{1000, 449900}}, {{1000, 449925}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, RequoteAskAfterLargeTrade)
{
	InitProrataProfile("P1", {1000}, {100}, {200}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 200, 450025);
	EXPECT_TRUE(e.ReadCancel(ask));
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 200, 450050);
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(ask);

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450050}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 200, 450075);
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(ask);

	strategy->InjectDepth({{1000, 450050}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450075}}, {{1000, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 200, 450100);
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(ask);

	strategy->InjectDepth({{1000, 450100}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, RequoteBidAfterLargeFill)
{
	InitProrataProfile("P1", {1000}, {100}, {200}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 21);
	EXPECT_TRUE(e.ReadCancel(bid));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 21, 450000, hedge));
	strategy->InjectAck(hedge);
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 21);
	strategy->InjectFill(bid, 22);
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 22, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid);

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 449975}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449950, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, RequoteAskAfterLargeFill)
{
	InitProrataProfile("P1", {1000}, {100}, {200}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 21);
	EXPECT_TRUE(e.ReadCancel(ask));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 21, 450025, hedge));
	strategy->InjectAck(hedge);
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 21);
	strategy->InjectFill(ask, 22);
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 22, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(ask);

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450050}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, DontRequoteManuallyCanceledBidOrder)
{
	InitProrataProfile("P1", {1000}, {100}, {200}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, DontRequoteManuallyCanceledAskOrder)
{
	InitProrataProfile("P1", {1000}, {100}, {200}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, RequoteAbandonedBidOrder)
{
	InitProrataProfile("P1", {1000}, {100}, {200}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerProrataOrderAbandonMessage { strategy->StrategyId(), bid.exchordid, Side::BUY, 0, 1 });
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, RequoteAbandonedAskOrder)
{
	InitProrataProfile("P1", {1000}, {100}, {200}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerProrataOrderAbandonMessage { strategy->StrategyId(), ask.exchordid, Side::SELL, 0, 1 });
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, RequoteAdoptededBidOrder)
{
	InitProrataProfile("P1", {1000}, {100}, {200}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerProrataOrderAdoptMessage { strategy->StrategyId(), bid.exchordid, Side::BUY, 0, 1, 0 });
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadModify(bid, 449925));
	KTN::OrderPod newBid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, newBid));
	strategy->InjectAck(newBid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, RequoteAdoptededAskOrder)
{
	InitProrataProfile("P1", {1000}, {100}, {200}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerProrataOrderAdoptMessage { strategy->StrategyId(), ask.exchordid, Side::SELL, 0, 1, 0 });
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450100));
	KTN::OrderPod newAsk;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, newAsk));
	strategy->InjectAck(newAsk);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, DontCancelOnLargeTradeAfterClearingProrataProfile)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bidStack[2];
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bidStack[0]));
	strategy->InjectAck(bidStack[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bidStack[1]));
	strategy->InjectAck(bidStack[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	// Clear prorata profiles
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "";
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "";
	strategy->onJson(getJson(settings));

	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadCancel(ask));
	strategy->InjectCancelAck(bid);
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, DontCancelOnLargeTradeAfterClearingProrataProfile2)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	// Clear prorata profile settings
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "";
	strategy->onJson(getJson(settings));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bidStack[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bidStack[0]));
	strategy->InjectAck(bidStack[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bidStack[1]));
	strategy->InjectAck(bidStack[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, DontCancelOnLargeTradeAfterDisablingProrataProfile)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bidStack[2];
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bidStack[0]));
	strategy->InjectAck(bidStack[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bidStack[1]));
	strategy->InjectAck(bidStack[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	// Disable prorata profiles
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = false;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = false;
	strategy->onJson(getJson(settings));

	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadCancel(ask));
	strategy->InjectCancelAck(bid);
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, DontCancelOnLargeTradeAfterDisablingProrataProfile2)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	// Disable prorata profiles
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = false;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = false;
	strategy->onJson(getJson(settings));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bidStack[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bidStack[0]));
	strategy->InjectAck(bidStack[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bidStack[1]));
	strategy->InjectAck(bidStack[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadNothing());
}

// TODO UpdateMessageWithRequoteCountOverMaxOverriden
// TODO InitialValueWithRequoteCountOverMaxOverriden

TEST_F(ProrataTest, BidRequoteAfterLargeFillCountsAgainstAllOrders)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {200, 200}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 21);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 21, 450000, hedge));
	strategy->InjectAck(hedge);
	strategy->InjectCancelAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 449975}}, {{1000, 450000}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 21);
	strategy->InjectFill(bid[0], 22);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 22, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid[0]);
	strategy->InjectCancelAck(bid[1]);

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 449950}}, {{1000, 449975}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449950, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449950, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, AskRequoteAfterLargeFillCountsAgainstAllOrders)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {200, 200}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 21);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 21, 450025, hedge));
	strategy->InjectAck(hedge);
	strategy->InjectCancelAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{2000, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 21);
	strategy->InjectFill(ask[0], 22);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 22, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(ask[0]);
	strategy->InjectCancelAck(ask[1]);

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450050}}, {{2000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, BidRequoteAfterLargeTradeCountsAgainstAllOrders)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {200, 200}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 200, 450000);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	strategy->InjectCancelAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 449975}}, {{1000, 450000}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 200, 449975);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid[0]);
	strategy->InjectCancelAck(bid[1]);

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 449950}}, {{1000, 449975}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449950, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449950, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 200, 449950);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid[0]);
	strategy->InjectCancelAck(bid[1]);

	strategy->InjectDepth({{1000, 449950}}, {{1000, 449975}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449925}}, {{1000, 449950}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449925, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 200, 449925);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(bid[0]);

	strategy->InjectDepth({{2000, 449900}}, {{1000, 449925}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, AskRequoteAfterLargeTradeCountsAgainstAllOrders)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {200, 200}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 200, 450025);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	strategy->InjectCancelAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{2000, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 200, 450050);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(ask[0]);
	strategy->InjectCancelAck(ask[1]);

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450050}}, {{2000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 200, 450075);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(ask[0]);
	strategy->InjectCancelAck(ask[1]);

	strategy->InjectDepth({{1000, 450050}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450075}}, {{1000, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 200, 450100);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectCancelAck(ask[0]);

	strategy->InjectDepth({{1000, 450100}}, {{2000, 450125}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, DontRequoteBeforeQuoteCommend) // TODO
{
	UpdateProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.prorataRequoteCount = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->Stop();
	EXPECT_TRUE(e.ReadNothing());

	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	strategy->InjectCancelAck(ask);

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeTradeOnBidProfileAddHedgeDisabled)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, {50}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeTradeOnAskProfileAddHedgeDisabled)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, {50}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeTradeOnBidProfileAddHedge)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, {50}, {1}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_TRUE(instr);
	const stacker::ProrataPricingModel &pricingModel = instr->GetProrataPricingModel();
	const std::optional<stacker::ProrataProfileUpdateMessage> &profile = pricingModel.GetProfile<Side::BUY>();
	ASSERT_TRUE(profile);
	ASSERT_EQ(profile->levelCount, 1);
	EXPECT_TRUE(profile->addHedgeEnabled[0]);

	std::optional<stacker::ProrataProfileUpdateMessage> profile2 = strategy->GetProrataProfile(profile->name);
	ASSERT_TRUE(profile2);
	ASSERT_EQ(profile2->levelCount, 1);
	EXPECT_TRUE(profile2->addHedgeEnabled[0]);

	const std::optional<stacker::ProrataProfileUpdateMessage> &profile3 = pricingModel.GetProfile<Side::SELL>();
	ASSERT_TRUE(profile3);
	ASSERT_EQ(profile3->levelCount, 1);
	EXPECT_TRUE(profile3->addHedgeEnabled[0]);

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 50, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	ASSERT_TRUE(profile);
	ASSERT_EQ(profile->levelCount, 1);
	EXPECT_FALSE(profile->addHedgeEnabled[0]);

	profile2 = strategy->GetProrataProfile(profile->name);
	ASSERT_TRUE(profile2);
	ASSERT_EQ(profile2->levelCount, 1);
	EXPECT_FALSE(profile2->addHedgeEnabled[0]);

	ASSERT_TRUE(profile3);
	ASSERT_EQ(profile3->levelCount, 1);
	EXPECT_FALSE(profile3->addHedgeEnabled[0]);
}

TEST_F(ProrataTest, LargeTradeOnAskProfileAddHedge)
{
	InitProrataProfile("P1", {1000}, {100}, {500}, {0}, {50}, {1}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_TRUE(instr);
	const stacker::ProrataPricingModel &pricingModel = instr->GetProrataPricingModel();
	const std::optional<stacker::ProrataProfileUpdateMessage> &profile = pricingModel.GetProfile<Side::SELL>();
	ASSERT_TRUE(profile);
	ASSERT_EQ(profile->levelCount, 1);
	EXPECT_TRUE(profile->addHedgeEnabled[0]);

	std::optional<stacker::ProrataProfileUpdateMessage> profile2 = strategy->GetProrataProfile(profile->name);
	ASSERT_TRUE(profile2);
	ASSERT_EQ(profile2->levelCount, 1);
	EXPECT_TRUE(profile2->addHedgeEnabled[0]);

	const std::optional<stacker::ProrataProfileUpdateMessage> &profile3 = pricingModel.GetProfile<Side::BUY>();
	ASSERT_TRUE(profile3);
	ASSERT_EQ(profile3->levelCount, 1);
	EXPECT_TRUE(profile3->addHedgeEnabled[0]);

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 50, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	ASSERT_TRUE(profile);
	ASSERT_EQ(profile->levelCount, 1);
	EXPECT_FALSE(profile->addHedgeEnabled[0]);

	profile2 = strategy->GetProrataProfile(profile->name);
	ASSERT_TRUE(profile2);
	ASSERT_EQ(profile2->levelCount, 1);
	EXPECT_FALSE(profile2->addHedgeEnabled[0]);

	ASSERT_TRUE(profile3);
	ASSERT_EQ(profile3->levelCount, 1);
	EXPECT_FALSE(profile3->addHedgeEnabled[0]);
}

TEST_F(ProrataTest, LargeTradeOnBid_MultipleProrataProfileLevels)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {500, 1000}, {0, 0}, {50, 100}, {1, 1}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 1000, 450000);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

// TODO read profile update and check profile copy
}

TEST_F(ProrataTest, LargeTradeOnAsk_MultipleProrataProfileLevels)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {500, 1000}, {0, 0}, {50, 100}, {1, 1}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 1000, 450025);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	// TODO read profile update and check profile copy
}

TEST_F(ProrataTest, LargeTradeOnBid_MultipleProrataProfileLevels2)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {500, 1000}, {0, 0}, {50, 100}, {1, 1}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1900, 450000}}, {{1900, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 50, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeTradeOnAsk_MultipleProrataProfileLevels2)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {500, 1000}, {0, 0}, {50, 100}, {1, 1}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1900, 450000}}, {{1900, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 50, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeTradeOnBid_MultipleProrataProfileLevels3)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {500, 1000}, {0, 0}, {50, 100}, {1, 1}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid[1]));

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 50, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeTradeOnAsk_MultipleProrataProfileLevels3)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {500, 1000}, {0, 0}, {50, 100}, {1, 1}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask[1]));

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 50, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeTradeOnBid_MultipleProrataProfileLevels4)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {500, 1000}, {0, 0}, {50, 100}, {0, 1}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::SELL, 500, 450000);
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, LargeTradeOnAsk_MultipleProrataProfileLevels4)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 100}, {500, 1000}, {0, 0}, {50, 100}, {0, 1}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{2000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(KTN::ORD::KOrderSide::Enum::BUY, 500, 450025);
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, AutoSwitchToAlternateBidProfile)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;

	stacker::StackerProrataConfigMessage msg;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	// EXPECT_TRUE(ReadMessage(msg));
	// EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->AdvanceTime(stacker::Seconds(5));

	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));
	// EXPECT_TRUE(ReadMessage(msg));
	// EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450025, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &settings = strategy->GetSettings();
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	EXPECT_STRCASEEQ(settings.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "P2");
	std::optional<stacker::ProrataProfileUpdateMessage> profile = ppm.GetProfile<Side::BUY>();
	std::optional<stacker::ProrataProfileUpdateMessage> alternateProfile = ppm.GetAlternateProfile<Side::BUY>();
	ASSERT_TRUE(profile.has_value());
	ASSERT_TRUE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P1");
	EXPECT_STREQ(alternateProfile->name.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadQtyModify(bid, 200));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(settings.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "");
	profile = ppm.GetProfile<Side::BUY>();
	alternateProfile = ppm.GetAlternateProfile<Side::BUY>();
	ASSERT_TRUE(profile.has_value());
	EXPECT_FALSE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());
}
TEST_F(ProrataTest, AutoSwitchToAlternateAskProfile)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	// Update settings to add alternate profile
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450000, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	EXPECT_STREQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P1");
	EXPECT_STREQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "P2");
	std::optional<stacker::ProrataProfileUpdateMessage> profile = ppm.GetProfile<Side::SELL>();
	std::optional<stacker::ProrataProfileUpdateMessage> alternateProfile = ppm.GetAlternateProfile<Side::SELL>();
	ASSERT_TRUE(profile.has_value());
	ASSERT_TRUE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P1");
	EXPECT_STREQ(alternateProfile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadQtyModify(ask, 200));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STREQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P2");
	EXPECT_STREQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "");
	profile = ppm.GetProfile<Side::SELL>();
	alternateProfile = ppm.GetAlternateProfile<Side::SELL>();
	ASSERT_TRUE(profile.has_value());
	EXPECT_FALSE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());
}

TEST_F(ProrataTest, AlternateBidProfileFlipInThenOut)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450025, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	EXPECT_STREQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P1");
	EXPECT_STREQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "P2");
	std::optional<stacker::ProrataProfileUpdateMessage> profile = ppm.GetProfile<Side::BUY>();
	std::optional<stacker::ProrataProfileUpdateMessage> alternateProfile = ppm.GetAlternateProfile<Side::BUY>();
	ASSERT_TRUE(profile.has_value());
	ASSERT_TRUE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P1");
	EXPECT_STREQ(alternateProfile->name.c_str(), "P2");

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STREQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P2");
	EXPECT_STREQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "");
	profile = ppm.GetProfile<Side::BUY>();
	alternateProfile = ppm.GetAlternateProfile<Side::BUY>();
	ASSERT_TRUE(profile.has_value());
	EXPECT_FALSE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->AdvanceTime(stacker::Seconds(5));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450025, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, AlternateAskProfileFlipInThenOut)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	// Set alternate profile
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450000, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "P2");
	std::optional<stacker::ProrataProfileUpdateMessage> profile = ppm.GetProfile<Side::SELL>();
	std::optional<stacker::ProrataProfileUpdateMessage> alternateProfile = ppm.GetAlternateProfile<Side::SELL>();
	ASSERT_TRUE(profile.has_value());
	ASSERT_TRUE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P1");
	EXPECT_STREQ(alternateProfile->name.c_str(), "P2");

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "");
	profile = ppm.GetProfile<Side::SELL>();
	alternateProfile = ppm.GetAlternateProfile<Side::SELL>();
	ASSERT_TRUE(profile.has_value());
	EXPECT_FALSE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->AdvanceTime(stacker::Millis(5000));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, AlternateBidProfileFlipOutThenIn)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	// Set alternate profile
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 449975, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(5000));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "P2");
	std::optional<stacker::ProrataProfileUpdateMessage> profile = ppm.GetProfile<Side::BUY>();
	std::optional<stacker::ProrataProfileUpdateMessage> alternateProfile = ppm.GetAlternateProfile<Side::BUY>();
	ASSERT_TRUE(profile.has_value());
	ASSERT_TRUE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P1");
	EXPECT_STREQ(alternateProfile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadQtyModify(bid, 200));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "");
	profile = ppm.GetProfile<Side::BUY>();
	alternateProfile = ppm.GetAlternateProfile<Side::BUY>();
	ASSERT_TRUE(profile.has_value());
	EXPECT_FALSE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());
}

TEST_F(ProrataTest, AlternateAskProfileFlipOutThenIn)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	// Set alternate profile
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(5000));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "P2");
	std::optional<stacker::ProrataProfileUpdateMessage> profile = ppm.GetProfile<Side::SELL>();
	std::optional<stacker::ProrataProfileUpdateMessage> alternateProfile = ppm.GetAlternateProfile<Side::SELL>();
	ASSERT_TRUE(profile.has_value());
	ASSERT_TRUE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P1");
	EXPECT_STREQ(alternateProfile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadQtyModify(ask, 200));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "");
	profile = ppm.GetProfile<Side::SELL>();
	alternateProfile = ppm.GetAlternateProfile<Side::SELL>();
	ASSERT_TRUE(profile.has_value());
	EXPECT_FALSE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());
}

TEST_F(ProrataTest, AlternateBidProfileFlipInTwice)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450025, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450050}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450050, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "P2");
	std::optional<stacker::ProrataProfileUpdateMessage> profile = ppm.GetProfile<Side::BUY>();
	std::optional<stacker::ProrataProfileUpdateMessage> alternateProfile = ppm.GetAlternateProfile<Side::BUY>();
	ASSERT_TRUE(profile.has_value());
	ASSERT_TRUE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P1");
	EXPECT_STREQ(alternateProfile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadQtyModify(bid, 200));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "");
	profile = ppm.GetProfile<Side::BUY>();
	alternateProfile = ppm.GetAlternateProfile<Side::BUY>();
	ASSERT_TRUE(profile.has_value());
	EXPECT_FALSE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());
}

TEST_F(ProrataTest, AlternateAskProfileFlipInTwice)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450000, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 449975}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 449975, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "P2");
	std::optional<stacker::ProrataProfileUpdateMessage> profile = ppm.GetProfile<Side::SELL>();
	std::optional<stacker::ProrataProfileUpdateMessage> alternateProfile = ppm.GetAlternateProfile<Side::SELL>();
	ASSERT_TRUE(profile.has_value());
	ASSERT_TRUE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P1");
	EXPECT_STREQ(alternateProfile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadQtyModify(ask, 200));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "");
	profile = ppm.GetProfile<Side::SELL>();
	alternateProfile = ppm.GetAlternateProfile<Side::SELL>();
	ASSERT_TRUE(profile.has_value());
	EXPECT_FALSE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());
}

TEST_F(ProrataTest, AutoSwitchToAlternateBidProfileWothMultipleOrders)
{
	InitProrataProfile("P1", {1000, 1500}, {100, 101}, {0, 0}, {0, 0}, 2);
	InitProrataProfile("P2", {1000, 2000}, {100, 101}, {0, 0}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1500, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 101, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1500, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450025, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 101, 450025, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "P2");
	std::optional<stacker::ProrataProfileUpdateMessage> profile = ppm.GetProfile<Side::BUY>();
	std::optional<stacker::ProrataProfileUpdateMessage> alternateProfile = ppm.GetAlternateProfile<Side::BUY>();
	ASSERT_TRUE(profile.has_value());
	ASSERT_TRUE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P1");
	EXPECT_STREQ(alternateProfile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "");
	profile = ppm.GetProfile<Side::BUY>();
	alternateProfile = ppm.GetAlternateProfile<Side::BUY>();
	ASSERT_TRUE(profile.has_value());
	EXPECT_FALSE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());
}

TEST_F(ProrataTest, AutoSwitchToAlternateAskProfileWithMultipleOrders)
{
	InitProrataProfile("P1", {1000, 1500}, {100, 101}, {0, 0}, {0, 0}, 2);
	InitProrataProfile("P2", {1000, 2000}, {100, 101}, {0, 0}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1500, 450025}});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 101, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1000, 449975}}, {{1500, 450000}});
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 101, 450000, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "P2");
	std::optional<stacker::ProrataProfileUpdateMessage> profile = ppm.GetProfile<Side::SELL>();
	std::optional<stacker::ProrataProfileUpdateMessage> alternateProfile = ppm.GetAlternateProfile<Side::SELL>();
	ASSERT_TRUE(profile.has_value());
	ASSERT_TRUE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P1");
	EXPECT_STREQ(alternateProfile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "");
	profile = ppm.GetProfile<Side::SELL>();
	alternateProfile = ppm.GetAlternateProfile<Side::SELL>();
	ASSERT_TRUE(profile.has_value());
	EXPECT_FALSE(alternateProfile.has_value());
	EXPECT_STREQ(profile->name.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());
}

TEST_F(ProrataTest, DeleteAlternateBidProfile)
{
	InitProrataProfile("P1", {1000, 1500}, {100, 101}, {0, 0}, {0, 0}, 2);
	InitProrataProfile("P2", {1000, 2000}, {100, 101}, {0, 0}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1500, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 101, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	ASSERT_TRUE(ppm.GetProfile<Side::BUY>().has_value());
	EXPECT_STREQ(ppm.GetProfile<Side::BUY>()->name.c_str(), "P1");
	ASSERT_TRUE(ppm.GetAlternateProfile<Side::BUY>().has_value());
	EXPECT_STREQ(ppm.GetAlternateProfile<Side::BUY>()->name.c_str(), "P2");

	strategy->InjectMessage(stacker::ProrataProfileDeleteMessage {"P2"});
	EXPECT_TRUE(e.ReadNothing());

	ASSERT_TRUE(ppm.GetProfile<Side::BUY>().has_value());
	EXPECT_STREQ(ppm.GetProfile<Side::BUY>()->name.c_str(), "P1");
	EXPECT_FALSE(ppm.GetAlternateProfile<Side::BUY>().has_value());

	strategy->InjectDepth({{1500, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450025, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 101, 450025, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "");

	ASSERT_TRUE(ppm.GetProfile<Side::BUY>().has_value());
	EXPECT_STREQ(ppm.GetProfile<Side::BUY>()->name.c_str(), "P1");
	EXPECT_FALSE(ppm.GetAlternateProfile<Side::BUY>().has_value());
}

TEST_F(ProrataTest, DeleteAlternateAskProfile)
{
	InitProrataProfile("P1", {1000, 1500}, {100, 101}, {0, 0}, {0, 0}, 2);
	InitProrataProfile("P2", {1000, 2000}, {100, 101}, {0, 0}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1500, 450025}});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 101, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	ASSERT_TRUE(ppm.GetProfile<Side::SELL>().has_value());
	EXPECT_STREQ(ppm.GetProfile<Side::SELL>()->name.c_str(), "P1");
	ASSERT_TRUE(ppm.GetAlternateProfile<Side::SELL>().has_value());
	EXPECT_STREQ(ppm.GetAlternateProfile<Side::SELL>()->name.c_str(), "P2");

	strategy->InjectMessage(stacker::ProrataProfileDeleteMessage {"P2"});
	EXPECT_TRUE(e.ReadNothing());

	ASSERT_TRUE(ppm.GetProfile<Side::SELL>().has_value());
	EXPECT_STREQ(ppm.GetProfile<Side::SELL>()->name.c_str(), "P1");
	EXPECT_FALSE(ppm.GetAlternateProfile<Side::SELL>().has_value());

	strategy->InjectDepth({{1000, 449975}}, {{1500, 450000}});
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 101, 450000, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "");

	ASSERT_TRUE(ppm.GetProfile<Side::SELL>().has_value());
	EXPECT_STREQ(ppm.GetProfile<Side::SELL>()->name.c_str(), "P1");
	EXPECT_FALSE(ppm.GetAlternateProfile<Side::SELL>().has_value());
}

TEST_F(ProrataTest, UpdateAlternateBidProfile)
{
	InitProrataProfile("P1", {1000, 1250, 1500}, {100, 101, 102}, {0, 0, 0}, {0, 0, 0}, 3);
	InitProrataProfile("P2", {1000, 2000, 3000}, {100, 101, 102}, {0, 0, 0}, {0, 0, 0}, 3);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1500, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 101, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 102, 450000, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	ASSERT_TRUE(ppm.GetAlternateProfile<Side::BUY>().has_value());
	stacker::ProrataProfileUpdateMessage upMsg = *ppm.GetAlternateProfile<Side::BUY>();
	upMsg.bookQuantity[1] = 1500;
	strategy->InjectMessage(upMsg);

	strategy->InjectDepth({{1500, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadCancel(bid[2]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450025, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 101, 450025, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 102, 450025, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadCancel(bid[2]));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());
}

TEST_F(ProrataTest, UpdateAlternateAskProfile)
{
	InitProrataProfile("P1", {1000, 1250, 1500}, {100, 101, 102}, {0, 0, 0}, {0, 0, 0}, 3);
	InitProrataProfile("P2", {1000, 2000, 3000}, {100, 101, 102}, {0, 0, 0}, {0, 0, 0}, 3);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1500, 450025}});
	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 101, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 102, 450025, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::ProrataPricingModel &ppm = instr->GetProrataPricingModel();

	ASSERT_TRUE(ppm.GetAlternateProfile<Side::SELL>().has_value());
	stacker::ProrataProfileUpdateMessage upMsg = *ppm.GetAlternateProfile<Side::SELL>();
	upMsg.bookQuantity[1] = 1500;
	strategy->InjectMessage(upMsg);

	strategy->InjectDepth({{1000, 449975}}, {{1500, 450000}});
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadCancel(ask[2]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 101, 450000, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 102, 450000, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadCancel(ask[2]));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "");

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());
}

TEST_F(ProrataTest, AlternateBidProfileFlipInThenOutAfterDelay)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450025, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(5000));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadQtyModify(bid, 200));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.bidSettings.alternateProrataProfile.c_str(), "");

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.bidProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateBidProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->AdvanceTime(stacker::Millis(5000));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450025, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, AlternateAskProfileFlipInThenOutAfterDelay)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);
	InitProrataProfile("P2", {1000}, {200}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.alternateProrataProfileDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.alternateProrataProfileDelay = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.alternateProrataProfile = "P2";
	strategy->onJson(getJson(settings));

	stacker::StackerProrataConfigMessage msg;
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450000, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P1");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "P2");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	const stacker::AklStackerParams &params = strategy->GetSettings();
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P1");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "P2");

	strategy->AdvanceTime(stacker::Millis(5000));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadQtyModify(ask, 200));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.prorataProfile.c_str(), "P2");
	EXPECT_STRCASEEQ(params.quoteInstrumentSettings.askSettings.alternateProrataProfile.c_str(), "");

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(250));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(ReadMessage(msg));
	EXPECT_STREQ(msg.askProrataProfile.c_str(), "P2");
	EXPECT_STREQ(msg.alternateAskProrataProfile.c_str(), "");
	EXPECT_TRUE(ReadNoMessage<stacker::StackerProrataConfigMessage>());

	strategy->AdvanceTime(stacker::Millis(5000));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
}

// TODO more add hedge tests
// TODO MarketDataUpdateAndBidCancelQueued test is no longer relevant?
// TODO dynamic stack tests

TEST_F(ProrataTest, QuoteBidAndCancelWithQuoteTimeout)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.quoteTimeout = 50;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteAskAndCancelWithQuoteTimeout)
{
	InitProrataProfile("P1", {1000}, {100}, {0}, {0}, 1);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.quoteTimeout = 50;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteSecondBidAndCancelWithQuoteTimeout)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 101}, {0, 0}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.quoteTimeout = 50;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 101, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1999, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 101, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(ProrataTest, QuoteSecondAskAndCancelWithQuoteTimeout)
{
	InitProrataProfile("P1", {1000, 2000}, {100, 101}, {0, 0}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.quoteTimeout = 50;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(5));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{2000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 101, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1999, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{2000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 101, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());
}



}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
