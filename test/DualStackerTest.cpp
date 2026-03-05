#include <gtest/gtest.h>

#include <Algos/AklStacker/AklStacker.hpp>
#include <Algos/AklStacker/Instrument.hpp>
#include <Algos/AklStacker/messages/StackerRequoteMessage.hpp>
#include "AlgoParams/AlgoInitializer.hpp"
#include "impl/ExchangeSender.hpp"

#include "impl/StrategyTester.hpp"

#include <Algos/AklStacker/messages/StackerClearPositionMessage.hpp>
#include <Algos/AklStacker/messages/StackInfoMessage.hpp>

#include <Algos/AklStacker/PriceConverter.hpp>

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

class DualStackerTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	DualStackerTest()
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
	}

	virtual void TearDown() override
	{
		strategy->Shutdown();
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
};

TEST_F(DualStackerTest, NoOrdersWithoutQuote)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(8);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(5);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(8);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(5);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, NoOrdersWithNewOrdersOff)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 5;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, NoOrdersWithZeroDepth)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, NoOrdersWithZeroQty)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 5;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, QuoteBids)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, QuoteAsks)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, RemovingOrders)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 2;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[4];
	KTN::OrderPod ask[4];

	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);

	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[3]));
	strategy->InjectAck(ask[3]);

	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack = strategy->GetStack(0);
	const stacker::Stack &stack2 = strategy->GetStack(1);

	const stacker::Order *bidInternal[4] =
	{
		instr->GetOrder(bid[0].orderReqId),
		instr->GetOrder(bid[1].orderReqId),
		instr->GetOrder(bid[2].orderReqId),
		instr->GetOrder(bid[3].orderReqId)
	};
	const stacker::Order *askInternal[4] =
	{
		instr->GetOrder(ask[0].orderReqId),
		instr->GetOrder(ask[1].orderReqId),
		instr->GetOrder(ask[2].orderReqId),
		instr->GetOrder(ask[3].orderReqId)
	};
	ASSERT_NE(bidInternal[0], nullptr);
	ASSERT_NE(bidInternal[1], nullptr);
	ASSERT_NE(bidInternal[2], nullptr);
	ASSERT_NE(bidInternal[3], nullptr);
	ASSERT_NE(askInternal[0], nullptr);
	ASSERT_NE(askInternal[1], nullptr);
	ASSERT_NE(askInternal[2], nullptr);
	ASSERT_NE(askInternal[3], nullptr);

	EXPECT_TRUE(stack.HasOrder(bidInternal[0]));
	EXPECT_TRUE(stack.HasOrder(bidInternal[1]));
	EXPECT_TRUE(stack.HasOrder(askInternal[0]));
	EXPECT_TRUE(stack.HasOrder(askInternal[1]));
	EXPECT_TRUE(stack2.HasOrder(bidInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(bidInternal[3]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[3]));

	strategy->InjectCancelAck(bid[2]);
	EXPECT_TRUE(stack.HasOrder(bidInternal[0]));
	EXPECT_TRUE(stack.HasOrder(bidInternal[1]));
	EXPECT_TRUE(stack.HasOrder(askInternal[0]));
	EXPECT_TRUE(stack.HasOrder(askInternal[1]));
	EXPECT_FALSE(stack2.HasOrder(bidInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(bidInternal[3]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[3]));

	strategy->InjectCancelAck(ask[1]);
	EXPECT_TRUE(stack.HasOrder(bidInternal[0]));
	EXPECT_TRUE(stack.HasOrder(bidInternal[1]));
	EXPECT_TRUE(stack.HasOrder(askInternal[0]));
	EXPECT_FALSE(stack.HasOrder(askInternal[1]));
	EXPECT_FALSE(stack2.HasOrder(bidInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(bidInternal[3]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[3]));

	strategy->InjectCancelAck(ask[3]);
	EXPECT_TRUE(stack.HasOrder(bidInternal[0]));
	EXPECT_TRUE(stack.HasOrder(bidInternal[1]));
	EXPECT_TRUE(stack.HasOrder(askInternal[0]));
	EXPECT_FALSE(stack.HasOrder(askInternal[1]));
	EXPECT_FALSE(stack2.HasOrder(bidInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(bidInternal[3]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[2]));
	EXPECT_FALSE(stack2.HasOrder(askInternal[3]));

	strategy->InjectCancelAck(bid[0]);
	EXPECT_FALSE(stack.HasOrder(bidInternal[0]));
	EXPECT_TRUE(stack.HasOrder(bidInternal[1]));
	EXPECT_TRUE(stack.HasOrder(askInternal[0]));
	EXPECT_FALSE(stack.HasOrder(askInternal[1]));
	EXPECT_FALSE(stack2.HasOrder(bidInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(bidInternal[3]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[2]));
	EXPECT_FALSE(stack2.HasOrder(askInternal[3]));
}

TEST_F(DualStackerTest, StopAndStart)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 2;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[4];
	KTN::OrderPod ask[4];

	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);

	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[3]));
	strategy->InjectAck(ask[3]);

	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack = strategy->GetStack(0);
	const stacker::Stack &stack2 = strategy->GetStack(1);

	const stacker::Order *bidInternal[4] =
	{
		instr->GetOrder(bid[0].orderReqId),
		instr->GetOrder(bid[1].orderReqId),
		instr->GetOrder(bid[2].orderReqId),
		instr->GetOrder(bid[3].orderReqId)
	};
	const stacker::Order *askInternal[4] =
	{
		instr->GetOrder(ask[0].orderReqId),
		instr->GetOrder(ask[1].orderReqId),
		instr->GetOrder(ask[2].orderReqId),
		instr->GetOrder(ask[3].orderReqId)
	};
	
	ASSERT_NE(bidInternal[0], nullptr);
	ASSERT_NE(bidInternal[1], nullptr);
	ASSERT_NE(bidInternal[2], nullptr);
	ASSERT_NE(bidInternal[3], nullptr);
	ASSERT_NE(askInternal[0], nullptr);
	ASSERT_NE(askInternal[1], nullptr);
	ASSERT_NE(askInternal[2], nullptr);
	ASSERT_NE(askInternal[3], nullptr);

	EXPECT_TRUE(stack.HasOrder(bidInternal[0]));
	EXPECT_TRUE(stack.HasOrder(bidInternal[1]));
	EXPECT_TRUE(stack.HasOrder(askInternal[0]));
	EXPECT_TRUE(stack.HasOrder(askInternal[1]));
	EXPECT_TRUE(stack2.HasOrder(bidInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(bidInternal[3]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[3]));

	// Stop the strategy
	// settings.meta.Enabled = 0;
	// strategy->onJson(getJson(settings));
	strategy->Stop();

	EXPECT_FALSE(stack.HasOrder(bidInternal[0]));
	EXPECT_FALSE(stack.HasOrder(bidInternal[1]));
	EXPECT_FALSE(stack.HasOrder(askInternal[0]));
	EXPECT_FALSE(stack.HasOrder(askInternal[1]));
	EXPECT_FALSE(stack2.HasOrder(bidInternal[2]));
	EXPECT_FALSE(stack2.HasOrder(bidInternal[3]));
	EXPECT_FALSE(stack2.HasOrder(askInternal[2]));
	EXPECT_FALSE(stack2.HasOrder(askInternal[3]));
	EXPECT_TRUE(e.ReadNothing());

	// Restart the strategy
	// settings.meta.Enabled = 1;
	// strategy->onJson(getJson(settings));
	strategy->Start();

	EXPECT_TRUE(stack.HasOrder(bidInternal[0]));
	EXPECT_TRUE(stack.HasOrder(bidInternal[1]));
	EXPECT_TRUE(stack.HasOrder(askInternal[0]));
	EXPECT_TRUE(stack.HasOrder(askInternal[1]));
	EXPECT_TRUE(stack2.HasOrder(bidInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(bidInternal[3]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[2]));
	EXPECT_TRUE(stack2.HasOrder(askInternal[3]));

	EXPECT_FALSE(stack2.HasOrder(bidInternal[0]));
	EXPECT_FALSE(stack2.HasOrder(bidInternal[1]));
	EXPECT_FALSE(stack2.HasOrder(askInternal[0]));
	EXPECT_FALSE(stack2.HasOrder(askInternal[1]));
	EXPECT_FALSE(stack.HasOrder(bidInternal[2]));
	EXPECT_FALSE(stack.HasOrder(bidInternal[3]));
	EXPECT_FALSE(stack.HasOrder(askInternal[2]));
	EXPECT_FALSE(stack.HasOrder(askInternal[3]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, ValidatePriceBands)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(8);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(5);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(8);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(5);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 1;

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].outsidePrice = Price::FromUnshifted(451000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].insidePrice = Price::FromUnshifted(450000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].insidePrice = Price::FromUnshifted(450500);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].outsidePrice = Price::FromUnshifted(449500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].outsidePrice = Price::FromUnshifted(450550);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].insidePrice = Price::FromUnshifted(449550);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].insidePrice = Price::FromUnshifted(450250);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].outsidePrice = Price::FromUnshifted(449250);

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::AklStackerParams &set = strategy->GetSettings();
	ASSERT_TRUE(set.quoteInstrumentSettings.askSettings.stackSideSettings[0].outsidePrice.has_value());
	ASSERT_TRUE(set.quoteInstrumentSettings.askSettings.stackSideSettings[0].insidePrice.has_value());
	ASSERT_TRUE(set.quoteInstrumentSettings.bidSettings.stackSideSettings[0].outsidePrice.has_value());
	ASSERT_TRUE(set.quoteInstrumentSettings.bidSettings.stackSideSettings[0].insidePrice.has_value());
	ASSERT_TRUE(set.quoteInstrumentSettings.askSettings.stackSideSettings[1].outsidePrice.has_value());
	ASSERT_TRUE(set.quoteInstrumentSettings.askSettings.stackSideSettings[1].insidePrice.has_value());
	ASSERT_TRUE(set.quoteInstrumentSettings.bidSettings.stackSideSettings[1].outsidePrice.has_value());
	ASSERT_TRUE(set.quoteInstrumentSettings.bidSettings.stackSideSettings[1].insidePrice.has_value());

	EXPECT_EQ(*set.quoteInstrumentSettings.askSettings.stackSideSettings[0].outsidePrice, Price::FromUnshifted(451000));
	EXPECT_EQ(*set.quoteInstrumentSettings.askSettings.stackSideSettings[0].insidePrice, Price::FromUnshifted(450000));
	EXPECT_EQ(*set.quoteInstrumentSettings.bidSettings.stackSideSettings[0].insidePrice, Price::FromUnshifted(450500));
	EXPECT_EQ(*set.quoteInstrumentSettings.bidSettings.stackSideSettings[0].outsidePrice, Price::FromUnshifted(449500));
	EXPECT_EQ(*set.quoteInstrumentSettings.askSettings.stackSideSettings[1].outsidePrice, Price::FromUnshifted(450550));
	EXPECT_EQ(*set.quoteInstrumentSettings.askSettings.stackSideSettings[1].insidePrice, Price::FromUnshifted(449550));
	EXPECT_EQ(*set.quoteInstrumentSettings.bidSettings.stackSideSettings[1].insidePrice, Price::FromUnshifted(450250));
	EXPECT_EQ(*set.quoteInstrumentSettings.bidSettings.stackSideSettings[1].outsidePrice, Price::FromUnshifted(449250));
}

TEST_F(DualStackerTest, AdoptHedgeWhenTurningOffNoAggressing)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(5000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(5000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;

	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {4000, 449975}}, {{1000, 450025}, {4000, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	stacker::StackInfoMessage sim;
	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449975.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450050.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(450000.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450025.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 10);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449975.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450050.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(450000.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450025.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 10);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	strategy->InjectDepth({{1000, 450000}, {4000, 449975}}, {{1000, 450050}, {4000, 450075}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}, {1000, 450000}, {4000, 449975}}, {{1000, 450050}, {4000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449975.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450075.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(450000.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450050.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 10);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack = instr->GetStack(0);
	const stacker::Stack &stack2 = instr->GetStack(1);
	const stacker::MStack &mStack = instr->GetMStack(0);
	const stacker::MStack &mStack2 = instr->GetMStack(1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack2.HasOrder(hedgeOrder));
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	strategy->InjectDepth({{1000, 450025}, {1000, 450000}, {4000, 449975}}, {{999, 450050}, {1000, 450075}, {4000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450125));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack2.HasOrder(hedgeOrder));
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449975.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450100.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(450000.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450075.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	strategy->InjectDepth({{1000, 450025}, {1000, 450000}, {4000, 449975}}, {{49, 450050}, {1000, 450075}, {4000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_TRUE(mStack2.HasOrder(hedgeOrder));
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449975.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450100.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(450025.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 10); // Includes hedge order
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450075.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	// Update bidNoAggressing2 parameter
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = false;
	strategy->onJson(getJson(settings));

	EXPECT_TRUE(e.ReadCancel(bid[2])); // Will cancel since parameters changed
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack.HasOrder(hedgeOrder));
	EXPECT_TRUE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack2.HasOrder(hedgeOrder));

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449975.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450100.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(450025.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450075.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	strategy->InjectDepth({{999, 450025}, {1000, 450000}, {4000, 449975}}, {{49, 450050}, {1000, 450075}, {4000, 450100}});
	EXPECT_TRUE(e.ReadModify(hedge, 449950));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449975.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450100.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(450000.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450075.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);
}

TEST_F(DualStackerTest, AdoptHedgeWithAskNoAggressingOnThenTurnOff)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(5000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(5000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {4000, 449975}}, {{1000, 450025}, {4000, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	stacker::StackInfoMessage sim;
	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449975.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450050.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(450000.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450025.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 10);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449975.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450050.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(450000.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450025.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 10);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	strategy->InjectDepth({{1000, 449975}, {4000, 449950}}, {{1000, 450025}, {4000, 450050}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}, {4000, 449950}}, {{1000, 450000}, {1000, 450025}, {4000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449950.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450050.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449975.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450025.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 10);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack = instr->GetStack(0);
	const stacker::MStack &mStack = instr->GetMStack(0);
	const stacker::Stack &stack2 = instr->GetStack(1);
	const stacker::MStack &mStack2 = instr->GetMStack(1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack2.HasOrder(hedgeOrder));

	strategy->InjectDepth({{999, 449975}, {1000, 449950}, {4000, 449925}}, {{1000, 450000}, {1000, 450025}, {4000, 450050}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449900));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack2.HasOrder(hedgeOrder));
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449925.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450050.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449950.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450025.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 10);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	strategy->InjectDepth({{49, 449975}, {1000, 449950}, {4000, 449925}}, {{1000, 450000}, {1000, 450025}, {4000, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_TRUE(mStack2.HasOrder(hedgeOrder));
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449925.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450050.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449950.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450000.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 10);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 10);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	// Update askNoAggressing2 parameter
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = false;
	strategy->onJson(getJson(settings));

	EXPECT_TRUE(e.ReadCancel(ask[2]));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack.HasOrder(hedgeOrder));
	EXPECT_TRUE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mStack2.HasOrder(hedgeOrder));

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449925.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450050.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449950.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450000.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 10);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);

	strategy->InjectDepth({{49, 449975}, {1000, 449950}, {4000, 449925}}, {{999, 450000}, {1000, 450025}, {4000, 450050}});
	EXPECT_TRUE(e.ReadModify(hedge, 450075));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449925.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 0);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 0);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450050.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 0);
	EXPECT_EQ(sim.askStackQty[1], 0);
	EXPECT_EQ(sim.askStackQty[2], 0);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 0);

	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(449950.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 0);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 0);
	EXPECT_EQ(sim.bidStackQty[4], 0);
	EXPECT_EQ(sim.bidStackQty[5], 0);
	EXPECT_EQ(sim.bidStackQty[6], 0);
	EXPECT_EQ(sim.bidStackQty[7], 0);
	EXPECT_EQ(sim.bidStackQty[8], 0);
	EXPECT_EQ(sim.bidStackQty[9], 0);
	EXPECT_EQ(sim.stackAskPrice, Price::FromUnshifted(450025.0).AsShifted());
	EXPECT_EQ(sim.askStackQty[0], 10);
	EXPECT_EQ(sim.askStackQty[1], 10);
	EXPECT_EQ(sim.askStackQty[2], 10);
	EXPECT_EQ(sim.askStackQty[3], 0);
	EXPECT_EQ(sim.askStackQty[4], 0);
	EXPECT_EQ(sim.askStackQty[5], 0);
	EXPECT_EQ(sim.askStackQty[6], 0);
	EXPECT_EQ(sim.askStackQty[7], 0);
	EXPECT_EQ(sim.askStackQty[8], 0);
	EXPECT_EQ(sim.askStackQty[9], 0);
	EXPECT_EQ(sim.index, 1);
}

TEST_F(DualStackerTest, BidFillAdoptHedgeInStack1WhenBothLeansMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 20);
	strategy->InjectFill(bid[3], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450000}, {1000, 449975}}, {{1000, 450025}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 30, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450000));
	EXPECT_TRUE(e.ReadModify(ask[5], 450000));
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack1 = strategy->GetStack(0);
	const stacker::Stack &stack2 = strategy->GetStack(1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));

	stacker::StackerClearPositionMessage msg;
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeOrder->stackIndex, 0);
	EXPECT_TRUE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
}

TEST_F(DualStackerTest, AskFillAdoptHedgeInStack1WhenBothLeansMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 20);
	strategy->InjectFill(ask[3], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{49, 450025}, {1000, 450050}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 30, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(bid[2], 450025));
	EXPECT_TRUE(e.ReadModify(bid[5], 450025));
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack1 = strategy->GetStack(0);
	const stacker::Stack &stack2 = strategy->GetStack(1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));

	stacker::StackerClearPositionMessage msg;
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeOrder->stackIndex, 0);
	EXPECT_TRUE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
}

TEST_F(DualStackerTest, BidFillAdoptHedgeInStack2WhenStack1LeanNotMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(500);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 20);
	strategy->InjectFill(bid[3], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450000}, {1000, 449975}}, {{1000, 450025}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 30, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{500, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[5], 450000));
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack1 = strategy->GetStack(0);
	const stacker::Stack &stack2 = strategy->GetStack(1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));

	stacker::StackerClearPositionMessage msg;
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeOrder->stackIndex, 1);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_TRUE(stack2.HasOrder(hedgeOrder));
}

TEST_F(DualStackerTest, AskFillAdoptHedgeInStack2WhenStack1LeanNotMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 20);
	strategy->InjectFill(ask[3], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{49, 450025}, {1000, 450050}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 30, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{500, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(bid[5], 450025));
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack1 = strategy->GetStack(0);
	const stacker::Stack &stack2 = strategy->GetStack(1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));

	stacker::StackerClearPositionMessage msg;
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeOrder->stackIndex, 1);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_TRUE(stack2.HasOrder(hedgeOrder));
}

TEST_F(DualStackerTest, BidFillAdoptHedgeInMStack1WhenBothLeansMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = true;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 20);
	strategy->InjectFill(bid[3], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450000}, {1000, 449975}}, {{1000, 450025}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 30, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack1 = strategy->GetStack(0);
	const stacker::Stack &stack2 = strategy->GetStack(1);
	const stacker::MStack &mstack1 = strategy->GetMStack(0);
	const stacker::MStack &mstack2 = strategy->GetMStack(1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack2.HasOrder(hedgeOrder));

	stacker::StackerClearPositionMessage msg;
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeOrder->stackIndex, 0);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_TRUE(mstack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack2.HasOrder(hedgeOrder));
}

TEST_F(DualStackerTest, AskFillAdoptHedgeInMStack1WhenBothLeansMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 20);
	strategy->InjectFill(ask[3], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{49, 450025}, {1000, 450050}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 30, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack1 = strategy->GetStack(0);
	const stacker::Stack &stack2 = strategy->GetStack(1);
	const stacker::MStack &mstack1 = strategy->GetMStack(0);
	const stacker::MStack &mstack2 = strategy->GetMStack(1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack2.HasOrder(hedgeOrder));

	stacker::StackerClearPositionMessage msg;
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeOrder->stackIndex, 0);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_TRUE(mstack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack2.HasOrder(hedgeOrder));
}

TEST_F(DualStackerTest, BidFillAdoptHedgeInMStack2WhenStack1LeanNotMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(500);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = true;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 20);
	strategy->InjectFill(bid[3], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450000}, {1000, 449975}}, {{1000, 450025}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 30, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{500, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack1 = strategy->GetStack(0);
	const stacker::Stack &stack2 = strategy->GetStack(1);
	const stacker::MStack &mstack1 = strategy->GetMStack(0);
	const stacker::MStack &mstack2 = strategy->GetMStack(1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack2.HasOrder(hedgeOrder));

	stacker::StackerClearPositionMessage msg;
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeOrder->stackIndex, 1);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack1.HasOrder(hedgeOrder));
	EXPECT_TRUE(mstack2.HasOrder(hedgeOrder));
}

TEST_F(DualStackerTest, AskFillAdoptHedgeInMStack2WhenStack1LeanNotMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 20);
	strategy->InjectFill(ask[3], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{49, 450025}, {1000, 450050}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 30, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{500, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack1 = strategy->GetStack(0);
	const stacker::Stack &stack2 = strategy->GetStack(1);
	const stacker::MStack &mstack1 = strategy->GetMStack(0);
	const stacker::MStack &mstack2 = strategy->GetMStack(1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack2.HasOrder(hedgeOrder));

	stacker::StackerClearPositionMessage msg;
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeOrder->stackIndex, 1);
	EXPECT_FALSE(stack1.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack2.HasOrder(hedgeOrder));
	EXPECT_FALSE(mstack1.HasOrder(hedgeOrder));
	EXPECT_TRUE(mstack2.HasOrder(hedgeOrder));
}

TEST_F(DualStackerTest, RequoteAfterBidFillWithNoAggressingSubmitsOrderAtBack)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[3], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, RequoteAfterAskFillWithNoAggressingSubmitsOrderAtBack)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[3], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, TickBackWhenInNoAggressingAndBestBidIsAbandoned)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPriceAbandonMessage msg;
	msg.levels.push_back(stacker::StackerPriceAbandonMessage::level {Price::FromUnshifted(450000), KTN::ORD::KOrderSide::Enum::BUY, 1});
	strategy->InjectMessage(msg);

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, TickBackWhenInNoAggressingAndBestAskIsAbandoned)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPriceAbandonMessage msg;
	msg.levels.push_back(stacker::StackerPriceAbandonMessage::level {Price::FromUnshifted(450025), KTN::ORD::KOrderSide::Enum::SELL, 1});
	strategy->InjectMessage(msg);

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, TickBackWhenInNoAggressingAndBestBidIsManuallyCanceled)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[3]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, TickBackWhenInNoAggressingAndBestAskIsManuallyCanceled)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[6];
	KTN::OrderPod ask[6];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[5]));
	strategy->InjectAck(bid[5]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[5]));
	strategy->InjectAck(ask[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[3]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, TickBackWhenInNoAggressingAndBestBidIsAbandonedWithStackLengthOne)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPriceAbandonMessage msg;
	msg.levels.push_back(stacker::StackerPriceAbandonMessage::level {Price::FromUnshifted(450000), KTN::ORD::KOrderSide::Enum::BUY, 1});
	strategy->InjectMessage(msg);

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, TickBackWhenInNoAggressingAndBestAskIsAbandonedWithStackLengthOne)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPriceAbandonMessage msg;
	msg.levels.push_back(stacker::StackerPriceAbandonMessage::level {Price::FromUnshifted(450025), KTN::ORD::KOrderSide::Enum::SELL, 1});
	strategy->InjectMessage(msg);

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, TickBackWhenInNoAggressingAndBestBidIsManuallyCanceledWithStackLengthOne)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, TickBackWhenInNoAggressingAndBestAskIsManuallyCanceledWithStackLengthOne)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(1000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, BidAltManualAdj)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = true;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	settings.quoteInstrumentSettings.showAltManualAdj = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].altManualAdjPrice = std::nullopt;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].altManualAdjPrice = std::nullopt;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].altManualAdjPrice = Price::FromUnshifted(449950);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].altManualAdjPrice = std::nullopt;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].altManualAdj = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].altManualAdj = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].altManualAdj = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].altManualAdj = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[4];
	KTN::OrderPod ask[4];

	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449950, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449950));
	EXPECT_TRUE(e.ReadModify(bid[2], 449925));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[2], 449975));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[2], 449925));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449925));
	EXPECT_TRUE(e.ReadModify(bid[3], 449900));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[3], 449950));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[3], 449900));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449900));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449900}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449875));
	EXPECT_TRUE(e.ReadModify(bid[2], 449875));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449900}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449875}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449850));
	EXPECT_TRUE(e.ReadModify(bid[3], 449850));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449900}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[3], 449900));
	EXPECT_TRUE(e.ReadModify(bid[2], 449925));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[3], 449950));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[2], 449975));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(DualStackerTest, AskAltManualAdj)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].noAggressing = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].noAggressing = true;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(20);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(20);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	settings.quoteInstrumentSettings.showAltManualAdj = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].altManualAdjPrice = std::nullopt;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].altManualAdjPrice = std::nullopt;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].altManualAdjPrice = std::nullopt;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].altManualAdjPrice = Price::FromUnshifted(450075);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].altManualAdj = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].altManualAdj = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].altManualAdj = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].altManualAdj = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[4];
	KTN::OrderPod ask[4];

	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 20, 450075, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450075));
	EXPECT_TRUE(e.ReadModify(ask[2], 450100));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450050));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450100));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450100));
	EXPECT_TRUE(e.ReadModify(ask[3], 450125));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(ask[3], 450075));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[3], 450125));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450125));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450150));
	EXPECT_TRUE(e.ReadModify(ask[2], 450150));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450150}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450175));
	EXPECT_TRUE(e.ReadModify(ask[3], 450175));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[3], 450125));
	EXPECT_TRUE(e.ReadModify(ask[2], 450100));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(ask[3], 450075));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450050));
	EXPECT_TRUE(e.ReadNothing());
}

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
