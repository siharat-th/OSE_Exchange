#include <gtest/gtest.h>

#include <Algos/AklStacker/AklStacker.hpp>
#include <Algos/AklStacker/Instrument.hpp>
#include "AlgoParams/AlgoInitializer.hpp"
#include "impl/ExchangeSender.hpp"
#include "Helpers.hpp"

#include "impl/StrategyTester.hpp"

#include <Algos/AklStacker/messages/StackerRequoteMessage.hpp>
#include <Algos/AklStacker/messages/StackerPriceAdoptMessage.hpp>
#include <Algos/AklStacker/messages/StackerPriceAbandonMessage.hpp>
#include <Algos/AklStacker/messages/StackerPositionUpdateMessage.hpp>
#include <Algos/AklStacker/messages/StackerClearPositionMessage.hpp>
#include <Algos/AklStacker/messages/StackerAdoptMessage.hpp>

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

AKL_STRONG_TYPE(double, DoubleQty)

class AklStackerTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	AklStackerTest()
	{
		
	}

	virtual void SetUp() override
	{
		OrderNums::instance().Refresh();

		params.name = "AklStackerTest";
		params.guid = "1";
		params.ordersender = &e;
		params.legs = LegParamParser::ParseSpreadLegs(config);

		const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef(params.legs[0].symbol);
		priceConverter = new stacker::PriceConverter(def);

		strategy = new StrategyTester<akl::stacker::AklStacker>(params);

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
	AlgoInitializer params;
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

TEST_F(AklStackerTest, NoOrdersWithoutQuote)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(8);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(5);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, NoOrdersWithNewOrdersOff)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, NoOrdersWithZeroDepth)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, NoOrdersWithZeroQty)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, QuoteBids)
{
	double d = static_cast<double>(Quantity{100});

	/*
	stacker::StrategyMessageResponder::MessageVariant message;
	std::cout << sizeof(message) << std::endl
		<< sizeof(stacker::StackTargetsMessage) << std::endl
		<< sizeof(stacker::StackInfoMessage) << std::endl
		<< sizeof(stacker::StackerRiskInfoMessage) << std::endl;

	stacker::StrategyMessageResponder responder;
	responder.Push(stacker::StackTargetsMessage{});
	responder.Push(stacker::StackInfoMessage{});
	responder.Push(stacker::StackerRiskInfoMessage{});
	while (!responder.IsEmpty())
	{
		responder.Process();
	}*/
	
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
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
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, QuoteAsks)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, NoAsks)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, NoBids)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, ReduceBidLength)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[5];
	KTN::OrderPod ask[5];
	
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925, bid[3]));
	strategy->InjectAck(bid[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900, bid[4]));
	strategy->InjectAck(bid[4]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[3]));
	strategy->InjectAck(ask[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadNothing());
	
	// Setting to same length should do nothing
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());
	
	// Increasing length should do nothing (no additional orders)
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 6;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());
	
	// Setting back to original length should do nothing
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());
	
	// Reducing length should cancel furthest orders
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadCancel(bid[4]));
	EXPECT_TRUE(e.ReadCancel(bid[3]));
	EXPECT_TRUE(e.ReadNothing());
	
	// Setting length to zero should cancel all remaining orders
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadCancel(bid[2]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, ReduceAskLength)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[5];
	KTN::OrderPod ask[5];
	
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925, bid[3]));
	strategy->InjectAck(bid[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900, bid[4]));
	strategy->InjectAck(bid[4]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[3]));
	strategy->InjectAck(ask[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadNothing());
	
	// Setting to same length should do nothing
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());
	
	// Increasing length should do nothing (no additional orders)
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 6;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());
	
	// Setting back to original length should do nothing
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());
	
	// Reducing length should cancel furthest orders
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadCancel(ask[4]));
	EXPECT_TRUE(e.ReadCancel(ask[3]));
	EXPECT_TRUE(e.ReadNothing());
	
	// Setting length to zero should cancel all remaining orders
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadCancel(ask[2]));
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidsAfterLeanUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(999);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1001);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 449925.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(2000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(2001);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(2000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 449975.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 450000.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidsTwoTicksAfterLeanUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(999);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(2001);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 449925.0));
	EXPECT_TRUE(e.ReadModify(o[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 449975.0));
	EXPECT_TRUE(e.ReadModify(o[0], 450000.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveAsksAfterLeanUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(999);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1001);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(2000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(2001);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(2000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveAsksTwoTicksAfterLeanUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(999);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(2001);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 450100.0));
	EXPECT_TRUE(e.ReadModify(o[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 450050.0));
	EXPECT_TRUE(e.ReadModify(o[0], 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidsAfterManualAdjUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(999);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = -1;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = -1000000;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = 0;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = 1;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 449925.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = 2;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = 1;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 449975.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = 0;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 450000.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveAsksAfterManualAdjUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(999);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].manualAdj = -1;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].manualAdj = -1000000;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].manualAdj = 0;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].manualAdj = 1;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].manualAdj = 2;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].manualAdj = 1;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].manualAdj = 0;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidsTwoTicksAfterManualAdjUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(999);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = 0;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = 2;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 449925.0));
	EXPECT_TRUE(e.ReadModify(o[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = 0;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 449975.0));
	EXPECT_TRUE(e.ReadModify(o[0], 450000.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveAsksTwoTicksAfterManualAdjUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(999);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].manualAdj = 2;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[0], 450100.0));
	EXPECT_TRUE(e.ReadModify(o[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].manualAdj = 0;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadModify(o[1], 450050.0));
	EXPECT_TRUE(e.ReadModify(o[0], 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidsAfterDepthUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(o[0], 449925.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {999, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(o[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(o[1], 449975.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(o[0], 450000.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidsTwoTicksAfterDepthUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {1, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(o[0], 449925.0));
	EXPECT_TRUE(e.ReadModify(o[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(o[1], 449975.0));
	EXPECT_TRUE(e.ReadModify(o[0], 450000.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidsTwoTicksAfterDepthUpdateWithQuoteTimeout)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 500;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {1, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(o[0], 449925.0));
	EXPECT_TRUE(e.ReadModify(o[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	// TODO
	// Advance time by 499ms (just under the quote timeout)
	strategy->AdvanceTime(stacker::Millis(499));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the quote timeout
	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadModify(o[1], 449975.0));
	EXPECT_TRUE(e.ReadModify(o[0], 450000.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidsAfterDepthUpdateWithCancelNew)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[6];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(o[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925, o[3]));
	strategy->InjectAck(o[3]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {999, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(o[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900, o[4]));
	strategy->InjectAck(o[4]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(o[4]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[5]));
	strategy->InjectAck(o[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(o[3]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidAfterDepthUpdateWithCancelNewOnAck)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[6];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925, o[3]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadCancel(o[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {999, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900, o[4]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadCancel(o[1]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[5]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectAck(o[4]);
	EXPECT_TRUE(e.ReadCancel(o[4]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectAck(o[3]);
	EXPECT_TRUE(e.ReadCancel(o[3]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidsAfterDepthUpdateWithCancelNewAndQuoteTimeout)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 500;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[7];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(o[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925, o[3]));
	strategy->InjectAck(o[3]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {999, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(o[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900, o[4]));
	strategy->InjectAck(o[4]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{2, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 499ms (just under the quote timeout)
	strategy->AdvanceTime(stacker::Millis(499));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the quote timeout
	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadCancel(o[4]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[5]));
	strategy->InjectAck(o[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {998, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(o[3]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[6]));
	strategy->InjectAck(o[6]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveAsksAfterDepthUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{999, 450025}, {1000, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(o[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {999, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {998, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(o[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {999, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(o[1], 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {999, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(o[0], 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveAsksTwoTicksAfterDepthUpdate)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {1, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(o[0], 450100.0));
	EXPECT_TRUE(e.ReadModify(o[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(o[1], 450050.0));
	EXPECT_TRUE(e.ReadModify(o[0], 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveAsksTwoTicksAfterDepthUpdateWithQuoteTimeout)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 500;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {1, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(o[0], 450100.0));
	EXPECT_TRUE(e.ReadModify(o[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 499ms (just under the quote timeout)
	strategy->AdvanceTime(stacker::Millis(499));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the quote timeout
	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadModify(o[1], 450050.0));
	EXPECT_TRUE(e.ReadModify(o[0], 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveAsksAfterDepthUpdateWithCancelNew)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[7];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{999, 450025}, {1000, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(o[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, o[3]));
	strategy->InjectAck(o[3]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {999, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {998, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(o[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, o[4]));
	strategy->InjectAck(o[4]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {999, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(o[4]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[5]));
	strategy->InjectAck(o[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {999, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(o[3]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[6]));
	strategy->InjectAck(o[6]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveAsksAfterDepthUpdateWithCancelNewAndQuoteTimeout)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 500;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[7];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{999, 450025}, {1000, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(o[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, o[3]));
	strategy->InjectAck(o[3]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {999, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {998, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(o[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, o[4]));
	strategy->InjectAck(o[4]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450025}, {999, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 499ms (just under the quote timeout)
	strategy->AdvanceTime(stacker::Millis(499));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the quote timeout
	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadCancel(o[4]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[5]));
	strategy->InjectAck(o[5]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {999, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(o[3]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[6]));
	strategy->InjectAck(o[6]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveBidsAfterLtp)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(Side::SELL, 1, 450000);
	EXPECT_TRUE(e.ReadModify(o[0], 449925.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(Side::SELL, 998, 450000);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(Side::SELL, 1, 449975);
	EXPECT_TRUE(e.ReadModify(o[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, MoveAsksAfterLtp)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(Side::BUY, 1, 450025);
	EXPECT_TRUE(e.ReadModify(o[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(Side::BUY, 998, 450025);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(Side::BUY, 1, 450050);
	EXPECT_TRUE(e.ReadModify(o[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, DontAllowBidsToCrossMarket)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 450000}, {1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1, 449975}, {1000, 450025}});
	EXPECT_TRUE(e.ReadModify(o[0], 449900.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(o[0], 449975.0));
	EXPECT_TRUE(e.ReadModify(o[2], 450000.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, DontAllowAsksToCrossMarket)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450025}, {1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450050}, {1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(o[0], 450125.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(o[0], 450050.0));
	EXPECT_TRUE(e.ReadModify(o[2], 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, FillsUpdatePosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(0));

	strategy->InjectFill(bid, 1);
	EXPECT_EQ(instrument->GetPosition(), Quantity(1));
	
	strategy->InjectFill(ask, 2);
	EXPECT_EQ(instrument->GetPosition(), Quantity(-1));
	
	strategy->InjectFill(ask, 3);
	EXPECT_EQ(instrument->GetPosition(), Quantity(-4));
	
	strategy->InjectFill(bid, 1);
	EXPECT_EQ(instrument->GetPosition(), Quantity(-3));
	
	strategy->InjectFill(bid, 5);
	EXPECT_EQ(instrument->GetPosition(), Quantity(2));
	
	strategy->InjectFill(bid, 3);
	EXPECT_EQ(instrument->GetPosition(), Quantity(5));
	
	strategy->InjectFill(ask, 2);
	EXPECT_EQ(instrument->GetPosition(), Quantity(3));
	
	strategy->InjectFill(ask, 3);
	EXPECT_EQ(instrument->GetPosition(), Quantity(0));
}

TEST_F(AklStackerTest, BidFullFillAndPayup)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 450000}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 10);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFullFillAndPayup)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{50, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 10);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillAndPayupWhenLevelClears)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 450000}, {100, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillAndPayupWhenLevelClears)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{50, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillAndPayupWhenMarketFlipsDown)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 450000}, {100, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 449975}}, {{100, 450000}, {100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillAndPayupWhenMarketFlipsUp)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{50, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450025}, {100, 450000}}, {{100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillAndPayupWhenMarketFlipsDownTwoTicks)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 2;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 2;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 450000}, {100, 449975}, {100, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 449950}}, {{100, 449975}, {100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillAndPayupWhenMarketFlipsUpTwoTicks)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 2;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 2;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{50, 450025}, {100, 450050}, {100, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450050}, {100, 450000}}, {{100, 450075}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFullFillPayupMissAndPayupAgain)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 450000}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFullFillPayupMissAndPayupAgain)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{50, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{50, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450050}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidMaxPayupTicks)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectDepth({{49, 450000}, {100, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 449975}, {100, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449925.0));
	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 449975}, {100, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskMaxPayupTicks)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}, {100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{50, 450050}, {100, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450100.0));
	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450050}, {100, 450075}});
	EXPECT_TRUE(e.ReadCancel(hedge[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidMaxPayupTicks2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}, {100, 449800}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 449975}, {100, 449800}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449800}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidMaxPayupTicksWithMultiplePositions)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 20, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{48, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449950, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskMaxPayupTicksWithMultiplePositions)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 20, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{50, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450075}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{48, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidMaxPayupTicksWithMultiplePositions2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 20, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 1);

	strategy->InjectDepth({{50, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{48, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 9, 449950, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskMaxPayupTicksWithMultiplePositions2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 20, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 1);

	strategy->InjectDepth({{100, 450000}}, {{50, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450075}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{48, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 9, 450075, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidMaxPayupTicksWithMultiplePositions3)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 20, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 11);

	strategy->InjectDepth({{50, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{48, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	// No order expected due to MaxPayupClearsPosition flag
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskMaxPayupTicksWithMultiplePositions3)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 20, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge, 11);

	strategy->InjectDepth({{100, 450000}}, {{50, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450075}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{48, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	// No order expected due to MaxPayupClearsPosition flag
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidMaxPayupTicksWithMultiplePositions4)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 449950}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 20, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskMaxPayupTicksWithMultiplePositions4)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{100, 450075}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 20, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidMaxPayupTicksWithMultiplePositions5)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 449925}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449950, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskMaxPayupTicksWithMultiplePositions5)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillMaxPayupTicksClearsPosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 100;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(20));

	strategy->InjectDepth({{49, 449925}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instrument->GetPosition(), Quantity(0));
}

TEST_F(AklStackerTest, AskFillMaxPayupTicksClearsPosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 100;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(-20));

	strategy->InjectDepth({{100, 450000}}, {{49, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instrument->GetPosition(), Quantity(0));
}

TEST_F(AklStackerTest, BidFillMaxPayupTicksClearsPosition2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 100;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(20));

	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 449950}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449950, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instrument->GetPosition(), Quantity(10));

	strategy->InjectDepth({{49, 449925}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instrument->GetPosition(), Quantity(0));
}

TEST_F(AklStackerTest, AskFillMaxPayupTicksClearsPosition2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 100;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(-20));

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450075}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instrument->GetPosition(), Quantity(-10));

	strategy->InjectDepth({{100, 450000}}, {{49, 450100}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instrument->GetPosition(), Quantity(0));
}

TEST_F(AklStackerTest, BidFillHedgeMissGoesToStack)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 100;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(10));

	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instrument->GetPosition(), Quantity(10));

	strategy->InjectDepth({{49, 449950}}, {{100, 449975}, {100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instrument->GetPosition(), Quantity(0));

	strategy->InjectDepth({{49, 449950}}, {{99, 449975}, {100, 450025}});
	EXPECT_TRUE(e.ReadModify(hedge, 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillHedgeMissGoesToStack)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 100;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(-10));

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instrument->GetPosition(), Quantity(-10));

	strategy->InjectDepth({{100, 450050}, {100, 450000}}, {{49, 450075}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instrument->GetPosition(), Quantity(0));

	strategy->InjectDepth({{99, 450050}, {100, 450000}}, {{49, 450075}});
	EXPECT_TRUE(e.ReadModify(hedge, 450000.0));
	EXPECT_TRUE(e.ReadNothing());
}

// TODO fix this test so that a requote command automatically places orders
TEST_F(AklStackerTest, AllowRequoteAfterOrderFullyFilled)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	// Simulating another quote cycle should not generate more orders
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());
	
	// Fully fill the ask order
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	// After ask is filled, another quote cycle should submit a new ask order
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	// Quoting again shouldn't do anything
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());
	
	// Now fill the bid order
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	// After bid is filled, another quote cycle should submit a new bid order
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
	
	// Quoting again shouldn't do anything
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, PauseAndResumeStrategyStatus)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::Stack &stack = instrument->GetStack(0);
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(450000.0)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450025.0)));
	
	// Pause strategy
	// settings.meta.Enabled = 0;
	// strategy->onJson(getJson(settings));
	strategy->Stop();

	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(450000.0)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450025.0)));

	// Resume strategy
	//settings.meta.Enabled = 1;
	//strategy->onJson(getJson(settings));
	strategy->Start();

	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(450000.0)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450025.0)));
	EXPECT_TRUE(e.ReadNothing());

	// Test that orders update when market moves
	strategy->InjectDepth({{100, 449975}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(bid, 449975.0));
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, PauseAndResumeStrategyStatusInPreopen)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.quoteInPreopen = false;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::Stack &stack = instrument->GetStack(0);
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(450000.0)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450025.0)));
	
	// Pause strategy
	//settings.meta.Enabled = 0;
	//strategy->onJson(getJson(settings));
	strategy->Stop();

	// Check that orders are not active anymore
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(450000.0)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450025.0)));

	// Update market in preopen
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	strategy->InjectDepth({{100, 449975}}, {{100, 450050}}, KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	EXPECT_TRUE(e.ReadNothing());

	// Resume strategy in preopen
	//settings.meta.Enabled = 1;
	//strategy->onJson(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(450000.0)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450025.0)));

	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	strategy->InjectDepth({{100, 449975}}, {{100, 450050}}, KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);	
	EXPECT_TRUE(e.ReadModify(bid, 449975.0));
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, PauseAndResumeStrategyStatusInPreopenWithIgnoreMarketState)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.quoteInPreopen = true;
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

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

	// Pause strategy
	// settings.meta.Enabled = 0;
	// strategy->onJson(getJson(settings));
	strategy->Stop();

	// Update market in preopen
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	strategy->InjectDepth({{100, 449975}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	// Resume strategy in preopen (should place orders because ignoreMarketState is true)
	// settings.meta.Enabled = 1;
	// strategy->onJson(getJson(settings));
	strategy->Start();
	impl::ExchangeSender::transaction t[2];
	ASSERT_TRUE(e.GetTransaction(t[0]));
	ASSERT_TRUE(e.GetTransaction(t[1]));
	EXPECT_EQ(t[0].action, KTN::ORD::KOrderAction::Enum::ACTION_CXL);
	EXPECT_EQ(t[1].action, KTN::ORD::KOrderAction::Enum::ACTION_CXL);
	EXPECT_TRUE(t[0].order.orderReqId == bid[0].orderReqId || t[0].order.orderReqId == ask[0].orderReqId);
	if (t[0].order.orderReqId == bid[0].orderReqId)
	{
		EXPECT_TRUE(t[1].order.orderReqId == ask[0].orderReqId);
	}
	else
	{
		EXPECT_TRUE(t[1].order.orderReqId == bid[0].orderReqId);
	}
	EXPECT_TRUE(e.ReadNothing());

	// Market opens (shouldn't change anything since we're ignoring market state)
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	EXPECT_TRUE(e.ReadNothing());

	// Update market prices
	strategy->InjectDepth({{100, 449950}}, {{100, 450075}});
	
	// Verify that orders update as expected
	EXPECT_TRUE(e.ReadModify(bid[1], 449925.0));
	EXPECT_TRUE(e.ReadModify(ask[1], 450100.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidTimeoutCover)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450025}, {100, 450000}}, {{100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 999ms (just under the timeout cover)
	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the timeout cover
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskTimeoutCover)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 0;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450075}, {100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 999ms (just under the timeout cover)
	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the timeout cover
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidTimeoutCover2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450025}, {100, 450000}}, {{100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 999ms (just under the timeout cover)
	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the timeout cover
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskTimeoutCover2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 0;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450075}, {100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 999ms (just under the timeout cover)
	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the timeout cover
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidTimeoutCover3)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450025}, {100, 450000}}, {{100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 999ms (just under the timeout cover)
	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{99, 450025}, {100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450000}}, {{99, 450025}, {100, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskTimeoutCover3)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 0;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450075}, {100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 999ms (just under the timeout cover)
	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450075}, {100, 450000}}, {{100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450075}, {100, 450000}}, {{49, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidTimeoutCover4)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450025}, {100, 450000}}, {{100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 999ms (just under the timeout cover)
	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{99, 450025}, {100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450025}, {100, 450000}}, {{100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 999ms more, resetting timeout
	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the timeout cover
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskTimeoutCover4)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 0;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450075}, {100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 999ms (just under the timeout cover)
	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450075}, {100, 450000}}, {{100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450075}, {100, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 999ms more, resetting timeout
	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the timeout cover
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, DontAllowBidStackToQuoteThroughMarket)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	strategy->onLaunch(getJson(settings));
	strategy->Start();

	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	
	// Override the stack prices to test the behavior
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	// Need to cast away const to modify the stack for testing
	stacker::Instrument *instr = const_cast<stacker::Instrument *>(instrument);
	instr->GetStack(0).Update(PX(450125.0), PX(450150.0), true); // TODO change this requote?

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450175, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, DontAllowAskStackToQuoteThroughMarket)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	strategy->onLaunch(getJson(settings));
	strategy->Start();

	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	
	// Override the stack prices to test the behavior
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	// Need to cast away const to modify the stack for testing
	stacker::Instrument *instr = const_cast<stacker::Instrument *>(instrument);
	instr->GetStack(0).Update(PX(449950.0), PX(449975.0), true);

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, DontAllowBidStackToQuoteThroughMarket2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	strategy->onLaunch(getJson(settings));
	strategy->Start();

	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	
	// Override the stack prices to test the behavior
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	// Need to cast away const to modify the stack for testing
	stacker::Instrument *instr = const_cast<stacker::Instrument *>(instrument);
	instr->GetStack(0).Update(PX(450100.0), PX(450125.0), true);

	KTN::OrderPod bid;
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, DontAllowAskStackToQuoteThroughMarket2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	strategy->onLaunch(getJson(settings));
	strategy->Start();

	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450100}});
	
	// Override the stack prices to test the behavior
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	// Need to cast away const to modify the stack for testing
	stacker::Instrument *instr = const_cast<stacker::Instrument *>(instrument);
	instr->GetStack(0).Update(PX(449975.0), PX(450000.0), true);

	KTN::OrderPod bid[2];
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillEqualToMaxOrderQty)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}, {100, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillEqualToMaxOrderQty)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}, {100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillGreaterThanMaxOrderQty)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(bid[1], 1);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectDepth({{49, 450000}, {100, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge[0]));
	strategy->InjectAck(hedge[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillGreaterThanMaxOrderQty)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	strategy->InjectFill(ask[1], 1);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}, {100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge[0]));
	strategy->InjectAck(hedge[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1, 450025, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillGreaterThanMaxOrderQty2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge[0]));
	strategy->InjectAck(hedge[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillGreaterThanMaxOrderQty2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge[0]));
	strategy->InjectAck(hedge[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillGreaterThanMaxOrderQty3)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(bid[1], 10);
	strategy->InjectFill(bid[2], 3);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[3];
	strategy->InjectDepth({{49, 450000}, {100, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge[0]));
	strategy->InjectAck(hedge[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge[1]));
	strategy->InjectAck(hedge[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 3, 450000, hedge[2]));
	strategy->InjectAck(hedge[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillGreaterThanMaxOrderQty3)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	strategy->InjectFill(ask[1], 10);
	strategy->InjectFill(ask[2], 3);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[3];
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}, {100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge[0]));
	strategy->InjectAck(hedge[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge[1]));
	strategy->InjectAck(hedge[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 3, 450025, hedge[2]));
	strategy->InjectAck(hedge[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillDontRehedgeUntilTobChanges)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450000}, {1, 449975}}, {{101, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{48, 450000}, {1, 449975}}, {{101, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillDontRehedgeUntilTobChanges)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{101, 450000}}, {{49, 450025}, {1, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{101, 450000}}, {{48, 450025}, {1, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillDontRehedgeUntilPositionChanges)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 6);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[3];
	strategy->InjectDepth({{49, 450000}, {100, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid, 449975.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 6, 450000, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 1);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 1);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 6, 450000, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 3);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 3, 450000, hedge[2]));
	strategy->InjectAck(hedge[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillDontRehedgeUntilPositionChanges)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 6);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[3];
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 6, 450025, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 1);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 1);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 6, 450025, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 3);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 3, 450025, hedge[2]));
	strategy->InjectAck(hedge[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillMoveQuoteBeforeHedging)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(150);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectFill(bid, 5);
	EXPECT_TRUE(e.ReadModify(bid, 449975.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillMoveQuoteBeforeHedging)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(150);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectFill(ask, 5);
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

// TODO REJECT WAIT TESTS


TEST_F(AklStackerTest, BidFillAfterLevelCleared)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	// Level cleared - price moves down
	strategy->InjectDepth({{100, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid, 449975.0));
	EXPECT_TRUE(e.ReadNothing());

	// Fill at original price
	strategy->InjectFill(bid, 10, 450000);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillAfterLevelCleared)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	// Level cleared - price moves up
	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	// Fill at original price
	strategy->InjectFill(ask, 10, 450025);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillAfterLevelCleared2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}, {100, 449950}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	// Level cleared - price moves down substantially
	strategy->InjectDepth({{100, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid, 449950.0));
	EXPECT_TRUE(e.ReadNothing());

	// Fill at original price
	strategy->InjectFill(bid, 10, 450000);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillAfterLevelCleared2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}, {100, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	// Level cleared - price moves up substantially
	strategy->InjectDepth({{100, 450000}}, {{100, 450075}});
	EXPECT_TRUE(e.ReadModify(ask, 450075.0));
	EXPECT_TRUE(e.ReadNothing());

	// Fill at original price
	strategy->InjectFill(ask, 10, 450025);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillAfterLevelFlipped)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	// Level flipped - market now offers lower than previous bid
	strategy->InjectDepth({{100, 449975}}, {{100, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid, 449975.0));
	EXPECT_TRUE(e.ReadNothing());

	// Fill at original price
	strategy->InjectFill(bid, 10, 450000);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillAfterLevelFlipped)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	// Level flipped - market now bids higher than previous ask
	strategy->InjectDepth({{100, 450025}, {100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	// Fill at original price
	strategy->InjectFill(ask, 10, 450025);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillAfterLevelFlipped2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}, {100, 449950}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	// Level flipped and cleared - dramatic market change
	strategy->InjectDepth({{100, 449950}}, {{100, 449975}, {100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid, 449950.0));
	EXPECT_TRUE(e.ReadNothing());

	// Fill at original price
	strategy->InjectFill(bid, 10, 450000);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillAfterLevelFlipped2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}, {100, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	// Level flipped and cleared - dramatic market change
	strategy->InjectDepth({{100, 450050}, {100, 450000}}, {{100, 450075}});
	EXPECT_TRUE(e.ReadModify(ask, 450075.0));
	EXPECT_TRUE(e.ReadNothing());

	// Fill at original price
	strategy->InjectFill(ask, 10, 450025);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillSendHedgeOnCancelAckOfPreviousHedge)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}, {100, 449950}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}, {100, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid, 449975.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10, 450000.0);
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449950}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillSendHedgeOnCancelAckOfPreviousHedge)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}, {100, 450075}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450050}, {100, 450075}});
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10, 450025.0);
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450075}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillDontImmediatelyRehedgeIfHedgeIsManuallyCanceled)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid, 449975.0));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectFill(bid, 10, 450000.0);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	// Manually cancel the hedge order (simulating user cancellation)
	strategy->InjectCancelAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	// Market moves - should trigger new hedge
	strategy->InjectDepth({{49, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillDontImmediatelyRehedgeIfHedgeIsManuallyCanceled)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectFill(ask, 10, 450025.0);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	// Manually cancel the hedge order (simulating user cancellation)
	strategy->InjectCancelAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	// Market moves - should trigger new hedge
	strategy->InjectDepth({{100, 450000}}, {{49, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillDontRehedgeOnStackCancel)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{99, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 449975}}, {{99, 450025}, {100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillDontRehedgeOnStackCancel)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450000}, {100, 449975}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450000}, {100, 449975}}, {{49, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillAdoptHedgeIntoStackThenRehedge)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 10;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);

	KTN::OrderPod hedge[2];
	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450000}, {100, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);

	const stacker::Order *hedgeInternal[2];
	hedgeInternal[0] = instrument->GetOrder(hedge[0].orderReqId);
	ASSERT_NE(hedgeInternal[0], nullptr);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal[0]));
	EXPECT_FALSE(stack.HasOrder(hedgeInternal[0]));
	EXPECT_EQ(hedgeInternal[0]->type, stacker::Order::Type::HEDGE);
	
	strategy->InjectDepth({{100, 449975}}, {{100, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 449975}}, {{100, 450000}, {100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());

	hedgeInternal[1] = instrument->GetOrder(hedge[1].orderReqId);
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal[0]));
	EXPECT_TRUE(stack.HasOrder(hedgeInternal[0]));
	EXPECT_EQ(hedgeInternal[0]->type, stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeInternal[0]->stackIndex, 0);

	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal[1]));
	EXPECT_FALSE(stack.HasOrder(hedgeInternal[1]));
	EXPECT_EQ(hedgeInternal[1]->type, stacker::Order::Type::HEDGE);
}

TEST_F(AklStackerTest, AskFillAdoptHedgeIntoStackThenRehedge)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 10;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);

	KTN::OrderPod hedge[2];
	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{49, 450025}, {100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);

	const stacker::Order *hedgeInternal[2];
	hedgeInternal[0] = instrument->GetOrder(hedge[0].orderReqId);
	ASSERT_NE(hedgeInternal[0], nullptr);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal[0]));
	EXPECT_FALSE(stack.HasOrder(hedgeInternal[0]));
	EXPECT_EQ(hedgeInternal[0]->type, stacker::Order::Type::HEDGE);
	
	strategy->InjectDepth({{100, 450025}, {100, 449975}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {100, 449975}}, {{49, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());

	hedgeInternal[1] = instrument->GetOrder(hedge[1].orderReqId);
	ASSERT_NE(hedgeInternal[1], nullptr);
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal[0]));
	EXPECT_TRUE(stack.HasOrder(hedgeInternal[0]));
	EXPECT_EQ(hedgeInternal[0]->type, stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeInternal[0]->stackIndex, 0);

	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal[1]));
	EXPECT_FALSE(stack.HasOrder(hedgeInternal[1]));
	EXPECT_EQ(hedgeInternal[1]->type, stacker::Order::Type::HEDGE);
}

TEST_F(AklStackerTest, BidFillDontUseTimeoutCoverWhenWeDontHavePosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(bid, 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450025}, {100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(bid, 450000.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10, 450025.0);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillDontUseTimeoutCoverWhenWeDontHavePosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10, 450025.0);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillDontUseTimeoutCoverWhenWeHaveOppositePosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 1);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(bid, 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450025}, {100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(bid, 450000.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10, 450025.0);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 9, 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillDontUseTimeoutCoverWhenWeHaveOppositePosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 1);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{49, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10, 450025.0);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 9, 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidAbandonAndAdopt)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *bidInternal = instr->GetOrder(bid.orderReqId);
	ASSERT_NE(bidInternal, nullptr);
	EXPECT_EQ(bidInternal->type, stacker::Order::Type::STACK);

	stacker::StackerPriceAbandonMessage abandonMsg;
	abandonMsg.levels.push_back(stacker::StackerPriceAbandonMessage::level{Price::FromUnshifted(449975), KTN::ORD::KOrderSide::BUY, 0});
	strategy->InjectMessage(abandonMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(bidInternal->type, stacker::Order::Type::STACK);

	abandonMsg.levels[0].price = Price::FromUnshifted(450025);
	abandonMsg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(abandonMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(bidInternal->type, stacker::Order::Type::STACK);

	abandonMsg.levels[0].price = Price::FromUnshifted(450000);
	abandonMsg.levels[0].side = KTN::ORD::KOrderSide::SELL;
	strategy->InjectMessage(abandonMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(bidInternal->type, stacker::Order::Type::STACK);

	abandonMsg.levels[0].price = Price::FromUnshifted(450000);
	abandonMsg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(abandonMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(bidInternal->type, stacker::Order::Type::NONE);

	EXPECT_EQ(instr->GetPosition(), QTY(0));

	strategy->InjectFill(bid, 1);
	EXPECT_EQ(instr->GetPosition(), QTY(0));

	stacker::StackerPriceAdoptMessage adoptMsg;
	adoptMsg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(449975), KTN::ORD::KOrderSide::BUY, 0});
	strategy->InjectMessage(adoptMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(bidInternal->type, stacker::Order::Type::NONE);

	adoptMsg.levels[0].price = Price::FromUnshifted(450025);
	adoptMsg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(adoptMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(bidInternal->type, stacker::Order::Type::NONE);

	adoptMsg.levels[0].price = Price::FromUnshifted(450000);
	adoptMsg.levels[0].side = KTN::ORD::KOrderSide::SELL;
	strategy->InjectMessage(adoptMsg);	
	EXPECT_EQ(bidInternal->type, stacker::Order::Type::NONE);
	strategy->InjectFill(bid, 2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instr->GetPosition(), QTY(0));

	adoptMsg.levels[0].price = Price::FromUnshifted(450000);
	adoptMsg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(adoptMsg);
	EXPECT_EQ(bidInternal->type, stacker::Order::Type::STACK);
	strategy->InjectFill(bid, 3);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instr->GetPosition(), QTY(3));
}

TEST_F(AklStackerTest, AskAbandonAndAdopt)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *askInternal = instr->GetOrder(ask.orderReqId);
	ASSERT_NE(askInternal, nullptr);
	EXPECT_EQ(askInternal->type, stacker::Order::Type::STACK);

	stacker::StackerPriceAbandonMessage abandonMsg;
	abandonMsg.levels.push_back(stacker::StackerPriceAbandonMessage::level{Price::FromUnshifted(450050), KTN::ORD::KOrderSide::SELL, 0});
	strategy->InjectMessage(abandonMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(askInternal->type, stacker::Order::Type::STACK);

	abandonMsg.levels[0].price = Price::FromUnshifted(449975);
	abandonMsg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(abandonMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(askInternal->type, stacker::Order::Type::STACK);

	abandonMsg.levels[0].price = Price::FromUnshifted(450025);
	abandonMsg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(abandonMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(askInternal->type, stacker::Order::Type::STACK);

	abandonMsg.levels[0].price = Price::FromUnshifted(450025);
	abandonMsg.levels[0].side = KTN::ORD::KOrderSide::SELL;
	strategy->InjectMessage(abandonMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(askInternal->type, stacker::Order::Type::NONE);

	EXPECT_EQ(instr->GetPosition(), QTY(0));

	strategy->InjectFill(ask, 1);
	EXPECT_EQ(instr->GetPosition(), QTY(0));

	stacker::StackerPriceAdoptMessage adoptMsg;
	adoptMsg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(450050), KTN::ORD::KOrderSide::SELL, 0});
	strategy->InjectMessage(adoptMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(askInternal->type, stacker::Order::Type::NONE);

	adoptMsg.levels[0].price = Price::FromUnshifted(449975);
	adoptMsg.levels[0].side = KTN::ORD::KOrderSide::SELL;
	strategy->InjectMessage(adoptMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(askInternal->type, stacker::Order::Type::NONE);

	adoptMsg.levels[0].price = Price::FromUnshifted(450025);
	adoptMsg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(adoptMsg);	
	EXPECT_EQ(askInternal->type, stacker::Order::Type::NONE);
	strategy->InjectFill(ask, 2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instr->GetPosition(), QTY(0));

	adoptMsg.levels[0].price = Price::FromUnshifted(450025);
	adoptMsg.levels[0].side = KTN::ORD::KOrderSide::SELL;
	strategy->InjectMessage(adoptMsg);
	EXPECT_EQ(askInternal->type, stacker::Order::Type::STACK);
	strategy->InjectFill(ask, 3);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(instr->GetPosition(), QTY(-3));
}

TEST_F(AklStackerTest, LongPositionUpdateDuringPreopen)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{1, 450000}, {100, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPositionUpdateMessage msg;
	msg.position = 10;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(10));
	const stacker::PositionManager &pm = instrument->GetPositionManager();
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(450000.0)), QTY(10));
	
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	strategy->InjectDepth({{9, 450000}, {100, 449975}}, {{100, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, LongPositionUpdateDuringPreopen2)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 449975.0}, {100, 449950.0}}, {{100, 450000}, {100, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPositionUpdateMessage msg;
	msg.position = 10;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(10));
	const stacker::PositionManager &pm = instrument->GetPositionManager();
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(449975.0)), QTY(10));
	
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	strategy->InjectDepth({{9, 449975}}, {{100, 450000}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, LongPositionUpdateDuringPreopen3)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{100, 450000}, {100, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPositionUpdateMessage msg;
	msg.position = 10;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(10));
	const stacker::PositionManager &pm = instrument->GetPositionManager();
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(449975.0)), QTY(10));
	
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	strategy->InjectDepth({{9, 449975}}, {{100, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, ShortPositionUpdateDuringPreopen)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {100, 449975}}, {{100, 450000}, {100, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPositionUpdateMessage msg;
	msg.position = -10;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(-10));
	const stacker::PositionManager &pm = instrument->GetPositionManager();
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(450000.0)), QTY(10));
	
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	strategy->InjectDepth({{100, 449975}}, {{9, 450000}, {100, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, ShortPositionUpdateDuringPreopen2)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{1, 450000}, {100, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPositionUpdateMessage msg;
	msg.position = -10;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(-10));
	const stacker::PositionManager &pm = instrument->GetPositionManager();
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(450025.0)), QTY(10));
	
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	strategy->InjectDepth({{99, 450000}, {100, 449975}}, {{9, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, ShortPositionUpdateDuringPreopen3)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}, {100, 449975}}, {{100, 450000}, {100, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPositionUpdateMessage msg;
	msg.position = -10;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	EXPECT_EQ(instrument->GetPosition(), Quantity(-10));
	const stacker::PositionManager &pm = instrument->GetPositionManager();
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(450025.0)), QTY(10));
	
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	strategy->InjectDepth({{100, 449975}}, {{9, 450025}}, 
		KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillMaintainHedgeOrder)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{100, 450000}});
	EXPECT_TRUE(e.ReadNothing());
	
	stacker::StackerPositionUpdateMessage msg;
	msg.position = -2;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{99, 450000}, {100, 450025}});
	ASSERT_TRUE(e.ReadQtyModify(hedge, 8));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{100, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	
	stacker::StackerClearPositionMessage msg2;
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_TRUE(stack.HasOrder(hedgeOrder));
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	
	strategy->InjectDepth({{100, 449975}}, {{99, 450000}, {100, 450025}});
	ASSERT_TRUE(e.ReadModify(hedge, 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillMaintainHedgeOrder)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	
	stacker::StackerPositionUpdateMessage msg;
	msg.position = 2;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{99, 450025}, {100, 450000}}, {{100, 450050}});
	ASSERT_TRUE(e.ReadQtyModify(hedge, 8));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450025}, {100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	
	stacker::StackerClearPositionMessage msg2;
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_TRUE(stack.HasOrder(hedgeOrder));
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	
	strategy->InjectDepth({{99, 450025}, {100, 450000}}, {{100, 450050}});
	ASSERT_TRUE(e.ReadModify(hedge, 450000.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillAdoptHedgeOrder)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{100, 450000}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerAdoptMessage msg;
	msg.stackIndex = 0;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_TRUE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{100, 449975}}, {{99, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillAdoptHedgeOrder)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerAdoptMessage msg;
	msg.stackIndex = 0;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_TRUE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{99, 450025}, {100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillDontAdoptHedgeOrderDueToLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{99, 450000}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerAdoptMessage msg;
	msg.stackIndex = 0;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{100, 449975}}, { {100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillDontAdoptHedgeOrderDueToLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{99, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerAdoptMessage msg;
	msg.stackIndex = 0;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillCancelHedgeOrderOnAdoptDueToLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{99, 450000}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerAdoptMessage msg;
	msg.stackIndex = 0;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{100, 449975}}, { {100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillCancelHedgeOrderOnAdoptDueToLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{99, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerAdoptMessage msg;
	msg.stackIndex = 0;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillAdoptHedgeOrderByPrice)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{100, 450000}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerPriceAdoptMessage msg;
	msg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(450025.0), KTN::ORD::KOrderSide::SELL, 0});
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	msg.levels[0].price = Price::FromUnshifted(450000.0);
	msg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	msg.levels[0].price = Price::FromUnshifted(450000.0);
	msg.levels[0].side = KTN::ORD::KOrderSide::SELL;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_TRUE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{100, 449975}}, {{99, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillAdoptHedgeOrderByPrice)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerPriceAdoptMessage msg;
	msg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(450025.0), KTN::ORD::KOrderSide::SELL, 0});
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	msg.levels[0].price = Price::FromUnshifted(450000.0);
	msg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	msg.levels[0].price = Price::FromUnshifted(450025.0);
	msg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::STACK);
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_TRUE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{99, 450025}, {100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillDontAdoptHedgeOrderByPriceDueToLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = false;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{99, 450000}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerPriceAdoptMessage msg;
	msg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(450000.0), KTN::ORD::KOrderSide::SELL, 0});
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{100, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillDontAdoptHedgeOrderByPriceDueToLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = false;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{99, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerPriceAdoptMessage msg;
	msg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(450025.0), KTN::ORD::KOrderSide::BUY, 0});
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillCancelHedgeOrderOnAdoptByPriceDueToLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{99, 450000}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerPriceAdoptMessage msg;
	msg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(450000.0), KTN::ORD::KOrderSide::BUY, 0});
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	msg.levels[0].price = Price::FromUnshifted(450025.0);
	msg.levels[0].side = KTN::ORD::KOrderSide::SELL;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	msg.levels[0].price = Price::FromUnshifted(450000.0);
	msg.levels[0].side = KTN::ORD::KOrderSide::SELL;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{100, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskFillCancelHedgeOrderOnAdoptByPriceDueToLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{99, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::Order *hedgeOrder = instrument->GetOrder(hedge.orderReqId);
	
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	stacker::StackerPriceAdoptMessage msg;
	msg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(450025.0), KTN::ORD::KOrderSide::SELL, 0});
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	msg.levels[0].price = Price::FromUnshifted(450000.0);
	msg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	
	msg.levels[0].price = Price::FromUnshifted(450025.0);
	msg.levels[0].side = KTN::ORD::KOrderSide::BUY;
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeOrder));
	EXPECT_FALSE(stack.HasOrder(hedgeOrder));
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidMaxPayupTicksRepricePositionOnAdopt)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10, 450000.0);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectDepth({{49, 450000}, {100, 449975}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 449975}, {100, 449950}}, {{100, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449925.0));
	strategy->InjectFill(bid[1], 10, 449975.0);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::PositionManager &pm = instrument->GetPositionManager();
	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::SELL>(PX(449975.0), 0), QTY(10));
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::BUY>(PX(450000.0)), QTY(10));
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::BUY>(PX(449975.0)), QTY(10));

	stacker::StackerAdoptMessage msg;
	msg.stackIndex = 0;
	strategy->InjectMessage(msg);

	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::SELL>(PX(449975.0), 0), QTY(20));
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::BUY>(PX(450000.0)), QTY(0));
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::BUY>(PX(449975.0)), QTY(20));

	strategy->InjectDepth({{49, 449975}, {100, 449950}}, {{100, 450000}, {100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 20, 449975, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskMaxPayupTicksRepricePositionOnAdopt)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10, 450025.0);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}, {100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {100, 450000}}, {{50, 450050}, {100, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450100.0));
	strategy->InjectFill(ask[1], 10, 450050.0);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::PositionManager &pm = instrument->GetPositionManager();
	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::BUY>(PX(450050.0), 0), QTY(10));
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::SELL>(PX(450025.0)), QTY(10));
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::SELL>(PX(450050.0)), QTY(10));

	stacker::StackerAdoptMessage msg;
	msg.stackIndex = 0;
	strategy->InjectMessage(msg);

	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::BUY>(PX(450050.0), 0), QTY(20));
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::SELL>(PX(450025.0)), QTY(0));
	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::SELL>(PX(450050.0)), QTY(20));

	strategy->InjectDepth({{100, 450025}, {100, 450000}}, {{49, 450050}, {100, 450075}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 20, 450050, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidQuickLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddLean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddTime = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddEnabled = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[2], 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskQuickLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddLean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddTime = 1000;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddEnabled = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{99, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{100, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450075.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidQuickLean2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddLean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddTime = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddEnabled = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[2], 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[2], 449950.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[2], 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskQuickLean2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddLean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddTime = 1000;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddEnabled = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{99, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{100, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450075.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{99, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450150.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{100, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{100, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450075.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidQuickLeanExpires)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddLean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddTime = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddEnabled = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[2], 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadModify(bid[2], 449950.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskQuickLeanExpires)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddLean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddTime = 1000;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddEnabled = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{99, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{100, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450075.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadModify(ask[2], 450150.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidQuickLeanExpires2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddLean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddTime = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddEnabled = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[2], 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadModify(bid[2], 449950.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskQuickLeanExpires2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddLean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddTime = 1000;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddEnabled = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{99, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{100, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450075.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(999));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{999, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadModify(ask[2], 450150.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidQuickLeanAndFill)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddLean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddTime = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].quickAddEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{99, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}, {1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[2], 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectFill(bid[2], 1);
	EXPECT_TRUE(e.ReadModify(bid[2], 449950.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1, 450025.0, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[2], 2);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2, 450025.0, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge[0], 1);
	strategy->InjectFill(hedge[1], 2);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskQuickLeanAndFill)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddLean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddTime = 1000;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].quickAddEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(200);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{99, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{100, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450075.0));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[2];
	strategy->InjectFill(ask[2], 1);
	EXPECT_TRUE(e.ReadModify(ask[2], 450150.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1, 450075.0, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[2], 2);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2, 450075.0, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(hedge[0], 1);
	strategy->InjectFill(hedge[1], 2);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidNoAggressingOn)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450175.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 450150}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450200.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450150}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskNoAggressingOn)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450175.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449950}}, {{1000, 450150}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450200.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450150}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449975.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadModify(bid[0], 450000.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RequoteBid)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackInfoMessage sim;
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

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1999ms (just under the requote delay)
	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
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

	// Advance time by 1 more ms to trigger the requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
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
}

TEST_F(AklStackerTest, RequoteAsk)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1999ms (just under the requote delay)
	strategy->AdvanceTime(stacker::Millis(1999));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RequoteBidAfterTickOut)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1999ms (just under the requote delay)
	strategy->AdvanceTime(stacker::Millis(1999));
	EXPECT_TRUE(e.ReadNothing());

	// Market ticks away from our orders
	strategy->InjectDepth({{999, 450000}, {1000, 449975}}, {{1000, 450025}});

	// Advance time by 1 more ms to trigger the requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RequoteAskAfterTickOut)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1999ms (just under the requote delay)
	strategy->AdvanceTime(stacker::Millis(1999));
	EXPECT_TRUE(e.ReadNothing());

	// Market ticks away from our orders
	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}, {1000, 450050}});

	// Advance time by 1 more ms to trigger the requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RequoteBidAfterTickIn)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1999ms (just under the requote delay)
	strategy->AdvanceTime(stacker::Millis(1999));
	EXPECT_TRUE(e.ReadNothing());

	// Market ticks with new orders inside our stack
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1, 450025}, {1000, 450050}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(bid[2], 450025.0));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectFill(bid[2], 10);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1998ms
	strategy->AdvanceTime(stacker::Millis(1998));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger another requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RequoteAskAfterTickIn)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1999ms (just under the requote delay)
	strategy->AdvanceTime(stacker::Millis(1999));
	EXPECT_TRUE(e.ReadNothing());

	// Market ticks with new orders inside our stack
	strategy->InjectDepth({{1, 450000}, {1000, 449975}}, {{1000, 450025}, {1000, 450050}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450000.0));
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectFill(ask[2], 10);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1998ms
	strategy->AdvanceTime(stacker::Millis(1998));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger another requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RequoteBothSides)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(bid[0], 10);
	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1999ms (just under the requote delay)
	strategy->AdvanceTime(stacker::Millis(1999));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RequoteBidWithNoAggressing)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(bid[0], 10, 450000.0);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1999ms (just under the requote delay)
	strategy->AdvanceTime(stacker::Millis(1999));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RequoteAskWithNoAggressing)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(ask[0], 10, 450025.0);
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1999ms (just under the requote delay)
	strategy->AdvanceTime(stacker::Millis(1999));
	EXPECT_TRUE(e.ReadNothing());

	// Advance time by 1 more ms to trigger the requote
	strategy->AdvanceTime(stacker::Millis(1));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, HedgeBidFillDespiteLevelFlipCausingTimeoutCover)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectDepth({{10, 450000}, {1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	strategy->InjectFill(bid[0], 1, 450000.0);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {1000, 449975}}, {{1000, 450025}});
	strategy->InjectFill(bid[0], 2, 450000.0);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	strategy->InjectFill(bid[0], 3, 450000.0);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 3, 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, HedgeAskFillDespiteLevelFlipCausingTimeoutCover)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectDepth({{1000, 450000}}, {{10, 450025}, {1000, 450050}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	strategy->InjectFill(ask[0], 1, 450025.0);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1, 450025));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1, 450025}, {1000, 450050}});
	strategy->InjectFill(ask[0], 2, 450025.0);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2, 450025));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	strategy->InjectFill(ask[0], 3, 450025.0);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 3, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, UpdateRequoteCountOverMax)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.requoteCount = 1;
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	// Verify initial requote count is set correctly
	EXPECT_EQ(strategy->GetSettings().quoteInstrumentSettings.requoteCount, 1);

	// Update settings with requote count beyond the maximum allowed
	settings.quoteInstrumentSettings.requoteCount = 10;
	ASSERT_GT(settings.quoteInstrumentSettings.requoteCount, stacker::QuoteInstrumentSettings::MAX_REQUOTE_COUNT);
	strategy->onJson(getJson(settings));
	
	// Verify that the requote count is capped at the maximum allowed value
	EXPECT_EQ(strategy->GetSettings().quoteInstrumentSettings.requoteCount, stacker::QuoteInstrumentSettings::MAX_REQUOTE_COUNT);
}

TEST_F(AklStackerTest, InitialValueWithRequoteCountOverMaxOverriden)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	
	// Set requote count over the maximum allowed
	settings.quoteInstrumentSettings.requoteCount = 10;
	// This should exceed QuoteInstrumentSettings::MAX_REQUOTE_COUNT
	ASSERT_GT(settings.quoteInstrumentSettings.requoteCount, stacker::QuoteInstrumentSettings::MAX_REQUOTE_COUNT);
	
	settings.quoteInstrumentSettings.requoteDelay = 2000;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	// Verify that the strategy's requote count is capped at the maximum allowed
	EXPECT_EQ(strategy->GetSettings().quoteInstrumentSettings.requoteCount, 
			  stacker::QuoteInstrumentSettings::MAX_REQUOTE_COUNT);
}

TEST_F(AklStackerTest, MoveStackOffTopOfBookWhenLeanIsNotMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 50;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{999, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449625.0));
	EXPECT_TRUE(e.ReadModify(bid[1], 449600.0));
	EXPECT_TRUE(e.ReadModify(bid[2], 449575.0));
	EXPECT_TRUE(e.ReadModify(ask[0], 450475.0));
	EXPECT_TRUE(e.ReadModify(ask[1], 450500.0));
	EXPECT_TRUE(e.ReadModify(ask[2], 450525.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->AdvanceTime(stacker::Millis(49));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadModify(bid[2], 449950.0));
	EXPECT_TRUE(e.ReadModify(bid[1], 449975.0));
	EXPECT_TRUE(e.ReadModify(bid[0], 450000.0));
	EXPECT_TRUE(e.ReadModify(ask[2], 450150.0));
	EXPECT_TRUE(e.ReadModify(ask[1], 450125.0));
	EXPECT_TRUE(e.ReadModify(ask[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, WaitForNextSnapshotWhenLeanIsNotMetDueToLtpAdjusting)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 50;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(Side::BUY, 1, 450000);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectTrade(Side::SELL, 1, 450100);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}}, {{999, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449625.0));
	EXPECT_TRUE(e.ReadModify(bid[1], 449600.0));
	EXPECT_TRUE(e.ReadModify(bid[2], 449575.0));
	EXPECT_TRUE(e.ReadModify(ask[0], 450475.0));
	EXPECT_TRUE(e.ReadModify(ask[1], 450500.0));
	EXPECT_TRUE(e.ReadModify(ask[2], 450525.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->AdvanceTime(stacker::Millis(49));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadModify(bid[2], 449950.0));
	EXPECT_TRUE(e.ReadModify(bid[1], 449975.0));
	EXPECT_TRUE(e.ReadModify(bid[0], 450000.0));
	EXPECT_TRUE(e.ReadModify(ask[2], 450150.0));
	EXPECT_TRUE(e.ReadModify(ask[1], 450125.0));
	EXPECT_TRUE(e.ReadModify(ask[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, WaitForNextSnapshotWhenLeanIsNotMetDueToLtpAdjusting2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 50;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(Side::BUY, 1, 450000);
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectTrade(Side::SELL, 1, 450100);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}, {1, 449975}}, {{999, 450100}, {1, 450125}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925.0));
	EXPECT_TRUE(e.ReadModify(ask[0], 450175.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	strategy->AdvanceTime(stacker::Millis(49));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	EXPECT_TRUE(e.ReadModify(bid[0], 450000.0));
	EXPECT_TRUE(e.ReadModify(ask[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidRegularPayupOnScratch)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(25);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(1);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 450000}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskRegularPayupOnScratch)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(25);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{50, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{49, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidRegularPayupOnLoser)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(25);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(1);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 449975}}, {{100, 450000}});
	EXPECT_TRUE(e.ReadModify(ask, 450000));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 449975}}, {{100, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 449975}}, {{100, 450000}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskRegularPayupOnLoser)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(25);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(bid, 450025));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}}, {{50, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}}, {{49, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidWinnerPayup)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(25);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(1);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{10, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450050));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->InjectDepth({{49, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{25, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{24, 450025}}, {{100, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskWinnerPayup)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(25);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{10, 450000}, {100, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid, 449975));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{100, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->InjectDepth({{100, 449975}}, {{49, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{25, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{24, 450000}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidWinnerPayup2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(25);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(1);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[2];
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{10, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450050));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(bid[1], 450025));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}}, {{10, 450050}, {100, 450075}});
	EXPECT_TRUE(e.ReadModify(ask, 450075));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450050}}, {{100, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->InjectDepth({{49, 450050}}, {{100, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{25, 450050}}, {{100, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{24, 450050}}, {{100, 450075}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 20, 450050, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450025}}, {{100, 450050}, {100, 450075}});
	EXPECT_TRUE(e.ReadModify(ask, 450050));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{50, 450025}}, {{100, 450050}, {100, 450075}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{49, 450025}}, {{100, 450050}, {100, 450075}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 20, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskWinnerPayup2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(25);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{10, 450000}, {100, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid, 449975));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{100, 450000}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[1], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{10, 449975}, {100, 449950}}, {{100, 450000}});
	EXPECT_TRUE(e.ReadModify(bid, 449950));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449950}}, {{100, 449975}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->InjectDepth({{100, 449950}}, {{49, 449975}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449950}}, {{25, 449975}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449950}}, {{24, 449975}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 20, 449975, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{100, 450000}});
	EXPECT_TRUE(e.ReadModify(bid, 449975));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{50, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{49, 450000}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 20, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidWinnerPayupMoveStackBeforeHedge)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(105);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(1);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 5);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{100, 450000}}, {{10, 450025}, {100, 450050}});
	EXPECT_TRUE(e.ReadModify(ask, 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{200, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(bid, 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->InjectDepth({{105, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{104, 450025}}, {{100, 450050}});
	EXPECT_TRUE(e.ReadModify(bid, 450000.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskWinnerPayupMoveStackBeforeHedge)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(105);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 5);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{10, 450000}, {100, 449975}}, {{100, 450025}});
	EXPECT_TRUE(e.ReadModify(bid, 449975.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{200, 450000}});
	EXPECT_TRUE(e.ReadModify(ask, 450000.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->InjectDepth({{100, 449975}}, {{105, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, 449975}}, {{104, 450000}});
	EXPECT_TRUE(e.ReadModify(ask, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5, 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AdoptHedgeWithBidNoAggressingOnThenTurnOff)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450050.0, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450075.0, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_TRUE(ReadNoMessage<stacker::StackInfoMessage>());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();

	stacker::StackInfoMessage sim;
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

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage(sim));
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

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	KTN::OrderPod hedge;
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450025.0, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage(sim));
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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Stack &stack = instr->GetStack(0);
	const stacker::MStack &mStack = instr->GetMStack(0);
	const stacker::HedgingModel &hm = instr->GetHedgingModel();
	const stacker::Order *hedgeO = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeO, nullptr);
	EXPECT_FALSE(stack.HasOrder(hedgeO));
	EXPECT_FALSE(mStack.HasOrder(hedgeO));
	EXPECT_TRUE(hm.HasOrder(hedgeO));

	strategy->InjectDepth({{1000, 450025}}, {{999, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeO));
	EXPECT_FALSE(mStack.HasOrder(hedgeO));
	EXPECT_TRUE(hm.HasOrder(hedgeO));

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage(sim));
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

	strategy->InjectDepth({{1000, 450025}}, {{49, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeO));
	EXPECT_TRUE(mStack.HasOrder(hedgeO));
	EXPECT_FALSE(hm.HasOrder(hedgeO));

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage(sim));
	EXPECT_EQ(sim.stackBidPrice, Price::FromUnshifted(450025.0).AsShifted());
	EXPECT_EQ(sim.bidStackQty[0], 10);
	EXPECT_EQ(sim.bidStackQty[1], 10);
	EXPECT_EQ(sim.bidStackQty[2], 10);
	EXPECT_EQ(sim.bidStackQty[3], 10);
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

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = false;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadCancel(bid[2]));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(stack.HasOrder(hedgeO));
	EXPECT_FALSE(mStack.HasOrder(hedgeO));
	EXPECT_FALSE(hm.HasOrder(hedgeO));

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage(sim));
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

	strategy->InjectDepth({{999, 450025}, {1000, 450000}}, {{49, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(hedge, 449950.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(10));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage(sim));
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
}

TEST_F(AklStackerTest, AdoptHedgeWithAskNoAggressingOnThenTurnOff)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	
	const stacker::Order *hedgeInternal = instrument->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeInternal, nullptr);
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{49, 449975}, {1000, 449950}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = false;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadCancel(ask[2]));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{49, 449975}, {1000, 449950}}, {{999, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadModify(hedge, 450075.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AdoptHedgeWithBidNoAggressingOnThenCancel)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	
	const stacker::Order *hedgeInternal = instrument->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeInternal, nullptr);
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{1000, 450025}}, {{49, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{999, 450025}, {1000, 450000}}, {{49, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectCancelAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));
}

TEST_F(AklStackerTest, AdoptHedgeWithAskNoAggressingOnThenCancel)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	
	const stacker::Order *hedgeInternal = instrument->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeInternal, nullptr);
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{49, 449975}, {1000, 449950}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{49, 449975}, {1000, 449950}}, {{999, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectCancelAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));
}

TEST_F(AklStackerTest, AdoptHedgeWithBidNoAggressingOnThenAbandonAndAdoptAgain)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::MStack &mstack = instrument->GetMStack(0);
	
	const stacker::Order *hedgeInternal = instrument->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeInternal, nullptr);
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(mstack.HasOrder(hedgeInternal));
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{1000, 450025}}, {{49, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_TRUE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	stacker::StackerPriceAbandonMessage abandonMsg;
	abandonMsg.levels.push_back(stacker::StackerPriceAbandonMessage::level{Price::FromUnshifted(450025.0), KTN::ORD::KOrderSide::BUY, 0});
	strategy->InjectMessage(abandonMsg);

	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	stacker::StackerPriceAdoptMessage adoptMsg;
	adoptMsg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(450025.0), KTN::ORD::KOrderSide::BUY, 0});
	strategy->InjectMessage(adoptMsg);

	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_TRUE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{999, 450025}, {1000, 450000}}, {{49, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_TRUE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectCancelAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));
}

TEST_F(AklStackerTest, AdoptHedgeWithAskNoAggressingOnThenAbandonAndAdoptAgain)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::MStack &mstack = instrument->GetMStack(0);
	
	const stacker::Order *hedgeInternal = instrument->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeInternal, nullptr);
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(mstack.HasOrder(hedgeInternal));
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{49, 449975}, {1000, 449950}}, {{1000, 450000}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_TRUE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	stacker::StackerPriceAbandonMessage abandonMsg;
	abandonMsg.levels.push_back(stacker::StackerPriceAbandonMessage::level{Price::FromUnshifted(450000.0), KTN::ORD::KOrderSide::SELL, 0});
	strategy->InjectMessage(abandonMsg);

	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	stacker::StackerPriceAdoptMessage adoptMsg;
	adoptMsg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(450000.0), KTN::ORD::KOrderSide::SELL, 0});
	strategy->InjectMessage(adoptMsg);

	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_TRUE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{49, 449975}, {1000, 449950}}, {{999, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_TRUE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectCancelAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));
}

TEST_F(AklStackerTest, AdoptHedgeWithBidNoAggressingOnThenStopAndStart)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);

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

	const stacker::Order *bidOrders[3] = {
		instrument->GetOrder(bid[0].orderReqId),
		instrument->GetOrder(bid[1].orderReqId),
		instrument->GetOrder(bid[2].orderReqId)
	};
	const stacker::Order *askOrders[3] = {
		instrument->GetOrder(ask[0].orderReqId),
		instrument->GetOrder(ask[1].orderReqId),
		instrument->GetOrder(ask[2].orderReqId)
	};

	ASSERT_NE(bidOrders[0], nullptr);
	ASSERT_NE(bidOrders[1], nullptr);
	ASSERT_NE(bidOrders[2], nullptr);
	ASSERT_NE(askOrders[0], nullptr);
	ASSERT_NE(askOrders[1], nullptr);
	ASSERT_NE(askOrders[2], nullptr);

	strategy->InjectFill(ask[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Stack &stack = instrument->GetStack(0);
	const stacker::MStack &mstack = instrument->GetMStack(0);
	
	const stacker::Order *hedgeInternal = instrument->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeInternal, nullptr);
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_FALSE(mstack.HasOrder(hedgeInternal));
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal));

	strategy->InjectDepth({{1000, 450025}}, {{49, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(stack.HasOrder(hedgeInternal));
	EXPECT_TRUE(mstack.HasOrder(hedgeInternal));
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));

	EXPECT_TRUE(stack.HasOrder(bidOrders[0]));
	EXPECT_TRUE(stack.HasOrder(bidOrders[1]));
	EXPECT_TRUE(stack.HasOrder(bidOrders[2]));
	EXPECT_FALSE(stack.HasOrder(askOrders[0]));
	EXPECT_TRUE(stack.HasOrder(askOrders[1]));
	EXPECT_TRUE(stack.HasOrder(askOrders[2]));
	EXPECT_EQ(bidOrders[0]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(bidOrders[1]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(bidOrders[2]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(askOrders[0]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(askOrders[1]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(askOrders[2]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeInternal->type, akl::stacker::Order::Type::STACK);
	
	// Stop the strategy
	// settings.meta.Enabled = 0;
	// strategy->onJson(getJson(settings));
	strategy->Stop();

	EXPECT_FALSE(stack.HasOrder(bidOrders[0]));
	EXPECT_FALSE(stack.HasOrder(bidOrders[1]));
	EXPECT_FALSE(stack.HasOrder(bidOrders[2]));
	EXPECT_FALSE(stack.HasOrder(askOrders[0]));
	EXPECT_FALSE(stack.HasOrder(askOrders[1]));
	EXPECT_FALSE(stack.HasOrder(askOrders[2]));
	EXPECT_EQ(bidOrders[0]->type, akl::stacker::Order::Type::NONE);
	EXPECT_EQ(bidOrders[1]->type, akl::stacker::Order::Type::NONE);
	EXPECT_EQ(bidOrders[2]->type, akl::stacker::Order::Type::NONE);
	EXPECT_EQ(askOrders[0]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(askOrders[1]->type, akl::stacker::Order::Type::NONE);
	EXPECT_EQ(askOrders[2]->type, akl::stacker::Order::Type::NONE);
	EXPECT_EQ(hedgeInternal->type, akl::stacker::Order::Type::NONE);
	
	strategy->InjectDepth({{1000, 450025}}, {{49, 450050}, {1000, 450075}});

	// Restart the strategy
	// settings.meta.Enabled = 1;
	// strategy->onJson(getJson(settings));
	strategy->Start();

	EXPECT_TRUE(stack.HasOrder(bidOrders[0]));
	EXPECT_TRUE(stack.HasOrder(bidOrders[1]));
	EXPECT_FALSE(stack.HasOrder(bidOrders[2]));
	EXPECT_FALSE(stack.HasOrder(askOrders[0]));
	EXPECT_TRUE(stack.HasOrder(askOrders[1]));
	EXPECT_TRUE(stack.HasOrder(askOrders[2]));
	EXPECT_EQ(bidOrders[0]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(bidOrders[1]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(bidOrders[2]->type, akl::stacker::Order::Type::NONE);
	EXPECT_EQ(askOrders[0]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(askOrders[1]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(askOrders[2]->type, akl::stacker::Order::Type::STACK);
	EXPECT_EQ(hedgeInternal->type, akl::stacker::Order::Type::STACK);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RequoteAfterBidFillWithNoAggressingSubmitsOrderAtBack)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(bid[0], 10, 450000.0);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RequoteAfterAskFillWithNoAggressingSubmitsOrderAtBack)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectFill(ask[0], 10, 450025.0);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, TickBackWhenInNoAggressingAndBestBidIsAbandoned)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	stacker::StackerPriceAbandonMessage msg;
	msg.levels.push_back(stacker::StackerPriceAbandonMessage::level{Price::FromUnshifted(450000.0), KTN::ORD::KOrderSide::BUY, 0});
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, TickBackWhenInNoAggressingAndBestAskIsAbandoned)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	stacker::StackerPriceAbandonMessage msg;
	msg.levels.push_back(stacker::StackerPriceAbandonMessage::level{Price::FromUnshifted(450025.0), KTN::ORD::KOrderSide::SELL, 0});
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, TickBackWhenInNoAggressingAndBestBidIsManuallyCanceled)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectCancelAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, TickBackWhenInNoAggressingAndBestAskIsManuallyCanceled)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
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

	strategy->InjectCancelAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, TickBackWhenInNoAggressingAndBestBidIsAbandonedAndStackLengthIsOne)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPriceAbandonMessage msg;
	msg.levels.push_back(stacker::StackerPriceAbandonMessage::level{Price::FromUnshifted(450000.0), KTN::ORD::KOrderSide::BUY, 0});
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, TickBackWhenInNoAggressingAndBestAskIsAbandonedAndStackLengthIsOne)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPriceAbandonMessage msg;
	msg.levels.push_back(stacker::StackerPriceAbandonMessage::level{Price::FromUnshifted(450025.0), KTN::ORD::KOrderSide::SELL, 0});
	strategy->InjectMessage(msg);

	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, TickBackWhenInNoAggressingAndBestBidIsManuallyCanceledAndStackLengthIsOne)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, TickBackWhenInNoAggressingAndBestAskIsManuallyCanceledAndStackLengthIsOne)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidAltManualAdj)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].manualAdj = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].altManualAdj = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].altManualAdjPrice = Price::FromUnshifted(449950.0);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];
	
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449900.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449975.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449900.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449875.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449950.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[1], 449875.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 449900}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[2], 449850.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, AskAltManualAdj)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].manualAdj = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].altManualAdj = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].altManualAdjPrice = Price::FromUnshifted(450075.0);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];
	
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450050.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450150.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450075.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadModify(ask[1], 450150.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450125}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450175.0));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillThenClearPostionAfterCloseThenAdopt)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{10, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::Close);
	strategy->InjectDepth({{100, 449975}}, {{10, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Order *hedgeInternal = instrument->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeInternal, nullptr);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal));
	
	stacker::StackerClearPositionMessage clearMsg;
	strategy->InjectMessage(clearMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_FALSE(hedgingModel.HasOrder(hedgeInternal));
	
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	strategy->InjectDepth({{100, 449975}}, {{100, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadModify(ask, 450000.0));
	EXPECT_TRUE(e.ReadNothing());
	
	EXPECT_EQ(hedgeInternal->type, akl::stacker::Order::Type::NONE);
	
	stacker::StackerAdoptMessage adoptMsg;
	adoptMsg.stackIndex = 0;
	strategy->InjectMessage(adoptMsg);
	EXPECT_EQ(hedgeInternal->type, akl::stacker::Order::Type::STACK);
	
	strategy->InjectDepth({{100, 449975}}, {{10, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadModify(ask, 450025.0));
	EXPECT_TRUE(e.ReadModify(hedge, 450025.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, BidFillThenReducePostionAfterCloseThenAdopt)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(100);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);
	
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 450000}}, {{100, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectFill(bid, 10);
	EXPECT_TRUE(e.ReadNothing());
	
	KTN::OrderPod hedge;
	strategy->InjectDepth({{49, 450000}}, {{100, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth({{100, 449975}}, {{10, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::Close);
	strategy->InjectDepth({{100, 449975}}, {{10, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::HedgingModel &hedgingModel = instrument->GetHedgingModel();
	const stacker::Order *hedgeInternal = instrument->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeInternal, nullptr);
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal));
	
	stacker::StackerPositionUpdateMessage updateMsg;
	updateMsg.position = -4;
	strategy->InjectMessage(updateMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(hedgingModel.HasOrder(hedgeInternal));
	
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	strategy->InjectDepth({{100, 449975}}, {{10, 450000}, {100, 450025}});
	EXPECT_TRUE(e.ReadQtyModify(hedge, 6));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, LoadRecoveredOrders)
{
	delete strategy;

	KTN::OrderPod recoveredOrders[6];
	recoveredOrders[0].orderReqId = 1;
	recoveredOrders[0].OrdSide = Side::BUY;
	recoveredOrders[0].quantity = 10;
	recoveredOrders[0].price = PX(450000);
	recoveredOrders[1].orderReqId = 2;
	recoveredOrders[1].OrdSide = Side::BUY;
	recoveredOrders[1].quantity = 10;
	recoveredOrders[1].price = PX(449975);
	recoveredOrders[2].orderReqId = 3;
	recoveredOrders[2].OrdSide = Side::BUY;
	recoveredOrders[2].quantity = 10;
	recoveredOrders[2].price = PX(449950);

	recoveredOrders[3].orderReqId = 4;
	recoveredOrders[3].OrdSide = Side::SELL;
	recoveredOrders[3].quantity = 10;
	recoveredOrders[3].price = PX(450025);
	recoveredOrders[4].orderReqId = 5;
	recoveredOrders[4].OrdSide = Side::SELL;
	recoveredOrders[4].quantity = 10;
	recoveredOrders[4].price = PX(450050);
	recoveredOrders[5].orderReqId = 6;
	recoveredOrders[5].OrdSide = Side::SELL;
	recoveredOrders[5].quantity = 10;
	recoveredOrders[5].price = PX(450075);

	uint64_t eoid = 1; // HACK strategy EOIDs in the test start at 10000 so there should not be collisions.
	for (int i = 0; i < 6; ++i)
	{
		recoveredOrders[i].fillqty = 0;
		recoveredOrders[i].OrdState = KTN::ORD::KOrderState::WORKING;
		recoveredOrders[i].OrdStatus = KTN::ORD::KOrderStatus::NEW;
		recoveredOrders[i].OrdType = KTN::ORD::KOrderType::LIMIT;
		recoveredOrders[i].leavesqty = recoveredOrders[i].quantity;
		const std::string ordernum = OrderNums::instance().NextClOrdId();
		OrderUtils::fastCopy(recoveredOrders[i].ordernum, ordernum.c_str(), sizeof(recoveredOrders[i].ordernum));
		recoveredOrders[i].orderReqId = OrderNums::instance().GenOrderReqId(recoveredOrders[i].ordernum);
		recoveredOrders[i].exchordid = eoid++;
		recoveredOrders[i].execid[0] = 0;
	}

	for (int i = 0; i < 6; ++i)
	{
		params.orders.push_back(recoveredOrders[i]);
	}

	strategy = new StrategyTester<stacker::AklStacker>(params);

	settings.meta.Enabled = 0;
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	strategy->onLaunch(getJson(settings));

	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::Stack &stack = instrument->GetStack(0);
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(450000)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449975)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449950)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450025)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450050)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450075)));

	// settings.meta.Enabled = 1;
	// strategy->onJson(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(450000)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449975)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449950)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450025)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450050)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450075)));

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}, {1000, 449975}}, {{999, 450025}, {1000, 450050}});
	EXPECT_TRUE(e.ReadModify(recoveredOrders[0], 449925.0));
	EXPECT_TRUE(e.ReadModify(recoveredOrders[3], 450100));
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Order *so = instrument->GetOrder(recoveredOrders[1].orderReqId);
	ASSERT_NE(so, nullptr);
	EXPECT_EQ(so->type, akl::stacker::Order::Type::STACK);
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449975)));

	stacker::StackerPriceAbandonMessage abandonMsg;
	abandonMsg.levels.push_back(stacker::StackerPriceAbandonMessage::level{Price::FromUnshifted(449975), KTN::ORD::KOrderSide::BUY, 0});
	strategy->InjectMessage(abandonMsg);
	EXPECT_EQ(so->type, stacker::Order::Type::NONE);
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449975)));

	stacker::StackerPriceAdoptMessage adoptMsg;
	adoptMsg.levels.push_back(stacker::StackerPriceAdoptMessage::level{Price::FromUnshifted(449975), KTN::ORD::KOrderSide::BUY, 0});
	strategy->InjectMessage(adoptMsg);
	EXPECT_EQ(so->type, stacker::Order::Type::STACK);
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449975)));

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, RecoveredOrdersWithStateCompleteShouldntBeAdded)
{
	delete strategy;

	KTN::OrderPod recoveredOrders[6];
	recoveredOrders[0].orderReqId = 1;
	recoveredOrders[0].OrdSide = Side::BUY;
	recoveredOrders[0].quantity = 10;
	recoveredOrders[0].price = PX(450000);
	recoveredOrders[0].exchordid = 1000;
	recoveredOrders[1].orderReqId = 2;
	recoveredOrders[1].OrdSide = Side::BUY;
	recoveredOrders[1].quantity = 10;
	recoveredOrders[1].price = PX(449975);
	recoveredOrders[1].exchordid = 1001;
	recoveredOrders[2].orderReqId = 3;
	recoveredOrders[2].OrdSide = Side::BUY;
	recoveredOrders[2].quantity = 10;
	recoveredOrders[2].price = PX(449950);
	recoveredOrders[2].exchordid = 1002;

	recoveredOrders[3].orderReqId = 4;
	recoveredOrders[3].OrdSide = Side::SELL;
	recoveredOrders[3].quantity = 10;
	recoveredOrders[3].price = PX(450025);
	recoveredOrders[3].exchordid = 1003;
	recoveredOrders[4].orderReqId = 5;
	recoveredOrders[4].OrdSide = Side::SELL;
	recoveredOrders[4].quantity = 10;
	recoveredOrders[4].price = PX(450050);
	recoveredOrders[4].exchordid = 1004;
	recoveredOrders[5].orderReqId = 6;
	recoveredOrders[5].OrdSide = Side::SELL;
	recoveredOrders[5].quantity = 10;
	recoveredOrders[5].price = PX(450075);
	recoveredOrders[5].exchordid = 1005;

	for (int i = 0; i < 6; ++i)
	{
		if (i == 0 || i == 3)
		{
			recoveredOrders[i].fillqty = 0;
			recoveredOrders[i].OrdState = KTN::ORD::KOrderState::COMPLETE;
			recoveredOrders[i].OrdStatus = KTN::ORD::KOrderStatus::CANCELED;
			recoveredOrders[i].OrdType = KTN::ORD::KOrderType::LIMIT;
			recoveredOrders[i].leavesqty = 0;
		}
		else
		{
			recoveredOrders[i].fillqty = 0;
			recoveredOrders[i].OrdState = KTN::ORD::KOrderState::WORKING;
			recoveredOrders[i].OrdStatus = KTN::ORD::KOrderStatus::NEW;
			recoveredOrders[i].OrdType = KTN::ORD::KOrderType::LIMIT;
			recoveredOrders[i].leavesqty = recoveredOrders[i].quantity;
		}
	}

	for (int i = 0; i < 6; ++i)
	{
		params.orders.push_back(recoveredOrders[i]);
	}

	strategy = new StrategyTester<stacker::AklStacker>(params);

	settings.meta.Enabled = 0;
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	strategy->onLaunch(getJson(settings));

	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instrument = strategy->GetInstrument();
	ASSERT_NE(instrument, nullptr);
	const stacker::Stack &stack = instrument->GetStack(0);
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(450000)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449975)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449950)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450025)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450050)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450075)));

	// settings.meta.Enabled = 1;
	// strategy->onJson(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(450000)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449975)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::BUY>().HasOrders(PX(449950)));
	EXPECT_FALSE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450025)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450050)));
	EXPECT_TRUE(stack.GetSide<KTN::ORD::KOrderSide::Enum::SELL>().HasOrders(PX(450075)));

	strategy->InjectDepth({{999, 450000}, {1000, 449975}}, {{999, 450025}, {1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {998, 449975}, {1000, 449950}}, {{1, 450025}, {998, 450050}, {1000, 450075}});
	EXPECT_TRUE(e.ReadModify(recoveredOrders[1], 449900));
	EXPECT_TRUE(e.ReadModify(recoveredOrders[4], 450125));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, CanceledOrderShouldntBeAdoptedOnStart)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *bidInternal = instr->GetOrder(bid.orderReqId);
	ASSERT_NE(bidInternal, nullptr);
	const stacker::Order *askInternal = instr->GetOrder(ask.orderReqId);
	ASSERT_NE(askInternal, nullptr);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Stack &stack = instr->GetStack(0);
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));

	strategy->Stop();
	EXPECT_TRUE(e.ReadNothing());

	strategy->Start();
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, CanceledOrderWhileStrategyIsStoppedShouldntBeAdoptedOnStart)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *bidInternal = instr->GetOrder(bid.orderReqId);
	ASSERT_NE(bidInternal, nullptr);
	const stacker::Order *askInternal = instr->GetOrder(ask.orderReqId);
	ASSERT_NE(askInternal, nullptr);
	EXPECT_TRUE(e.ReadNothing());

	strategy->Stop();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(bid);
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Stack &stack = instr->GetStack(0);
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));

	strategy->Start();
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, FilledOrderShouldntBeAdoptedOnStart)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *bidInternal = instr->GetOrder(bid.orderReqId);
	ASSERT_NE(bidInternal, nullptr);
	const stacker::Order *askInternal = instr->GetOrder(ask.orderReqId);
	ASSERT_NE(askInternal, nullptr);

	strategy->InjectFill(bid, 10);
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Stack &stack = instr->GetStack(0);
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));

	strategy->Stop();
	EXPECT_TRUE(e.ReadNothing());

	strategy->Start();
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, FilledOrderWileStrategyIsStoppedShouldntBeAdoptedOnStart)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid;
	KTN::OrderPod ask;

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *bidInternal = instr->GetOrder(bid.orderReqId);
	ASSERT_NE(bidInternal, nullptr);
	const stacker::Order *askInternal = instr->GetOrder(ask.orderReqId);
	ASSERT_NE(askInternal, nullptr);

	strategy->Stop();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 10);
	strategy->InjectFill(ask, 10);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Stack &stack = instr->GetStack(0);
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));

	strategy->Start();
	EXPECT_FALSE(stack.HasOrder(bidInternal));
	EXPECT_FALSE(stack.HasOrder(askInternal));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, DontModifyBidsUntilTheyreAcked)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadModify(o[0], 449925.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 449975}, {1000, 449950}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadModify(o[1], 449900.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, DontModifyAsksUntilTheyreAcked)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, o[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, o[2]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{999, 450050}, {1000, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadModify(o[0], 450100.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadModify(o[1], 450125.0));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, DontModifyBidsUntilTheyreAckedWithCancelNew)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod o[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, o[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, o[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, o[2]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{999, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadCancel(o[0]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(AklStackerTest, DontReduceHedgeUntilItsAcked)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	strategy->InjectMessage(stacker::StackerRequoteMessage{});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950, bid[2]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050, ask[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075, ask[2]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 10);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{499, 450000}, {1000, 449975}, {1000, 449950}}, {{1000, 450025}});
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, hedge));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask[0], 2);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadQtyModify(hedge, 8));
	EXPECT_TRUE(e.ReadNothing());
}

}

int main(int argc, char* argv[])
{
	bool y = OrderNums::instance().isLoaded();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
