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

class StackerWithRateLimiterTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	StackerWithRateLimiterTest()
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
};

TEST_F(StackerWithRateLimiterTest, ValidateSettings)
{
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 111;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 2222;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::AklStackerParams &stratSettings = strategy->GetSettings();
	EXPECT_EQ(stratSettings.quoteInstrumentSettings.rateLimitSettings.limit, 111);
	EXPECT_EQ(stratSettings.quoteInstrumentSettings.rateLimitSettings.window, 2222);
}

TEST_F(StackerWithRateLimiterTest, NoOrdersWithRateLimitOfZero)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 0;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackerWithRateLimiterTest, QuoteStrategyWithRateLimitOfOne)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 1;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(30));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(40));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));
}

TEST_F(StackerWithRateLimiterTest, QuoteStrategyWithRateLimitOfThree)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 3;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(40));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));
}

TEST_F(StackerWithRateLimiterTest, AskStackMovesWhileOrdersAreQueued)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	// Move the ask stack
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackerWithRateLimiterTest, AskStackMovesOutThenInWhileOrdersAreQueued)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(100));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	// Move the ask stack out
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(100));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	// Move the ask stack back in
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(799));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackerWithRateLimiterTest, AskStackMovesOutThenInWhileAfterOrdersAreSubmitted)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	// Store order pointers for asks
	KTN::OrderPod ask[5];

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0, ask[2]));
	strategy->InjectAck(ask[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0, ask[3]));
	strategy->InjectAck(ask[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0, ask[4]));
	strategy->InjectAck(ask[4]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	// Update rate limit
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 1;
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	// Move the ask stack out
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450150.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(ask[1], 450175.0));
	EXPECT_TRUE(e.ReadNothing());

	// Move the ask stack back in
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(ask[2], 450075.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(ask[1], 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(ask[0], 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackerWithRateLimiterTest, AskStackMovesWhileOrdersAreQueuedWithCancelNew)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	// Move the ask stack
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450150.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackerWithRateLimiterTest, BidStackMovesOutWhileAskOrdersAreQueued)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.moveWithCancelNew = false;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[5];

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0, bid[3]));
	strategy->InjectAck(bid[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	// Move the bid stack
	strategy->InjectDepth({{1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(bid[0], 449875.0));
	EXPECT_TRUE(e.ReadModify(bid[1], 449850.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(30));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackerWithRateLimiterTest, BidStackMovesOutWhileAskOrdersAreQueuedWithCancelNew)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[5];

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0, bid[3]));
	strategy->InjectAck(bid[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	// Move the bid stack
	strategy->InjectDepth({{1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(30));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449875.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449850.0, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(70));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackerWithRateLimiterTest, BidFillAndHedgeQueued)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[5];

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0, bid[3]));
	strategy->InjectAck(bid[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->InjectFill(bid[0], 10);
	strategy->InjectDepth({{499, 450000}, {1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackerWithRateLimiterTest, BidFillAndHedgeQueuedAndRehedge)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[5];

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0, bid[3]));
	strategy->InjectAck(bid[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->InjectFill(bid[0], 10);
	strategy->InjectDepth({{499, 450000}, {1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{499, 449975}, {1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975.0)); // TODO this order is different from the TT test, because hedgingmodel gets checked prior to rate limiter queue
	
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449850.0, bid[1]));
	strategy->InjectAck(bid[1]);
	
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackerWithRateLimiterTest, BidFillAndHedgeQueuedAndRehedge2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 5;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[5];

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0, bid[3]));
	strategy->InjectAck(bid[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0, bid[4]));
	strategy->InjectAck(bid[4]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->InjectFill(bid[0], 10);
	strategy->InjectDepth({{499, 450000}, {1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{499, 449975}, {1000, 449950}}, {{1000, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975.0)); // TODO see not above
	
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449850.0, bid[1]));
	strategy->InjectAck(bid[1]);
	
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	strategy->AdvanceTime(stacker::Seconds(30));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackerWithRateLimiterTest, BidFillAndHedgeQueuedAndMultipleModDowns)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 11;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[5];

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0, bid[3]));
	strategy->InjectAck(bid[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0, bid[4]));
	strategy->InjectAck(bid[4]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->InjectFill(bid[0], 10);
	strategy->InjectDepth({{499, 450000}, {1000, 449975}}, {{1000, 450025}});
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000.0, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = -4;
	strategy->InjectMessage(posMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	posMsg.position = -1;
	strategy->InjectMessage(posMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	posMsg.position = -2;
	strategy->InjectMessage(posMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadQtyModify(hedge, 3));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	strategy->InjectModifyQtyAck(hedge, 3);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(53));
}

TEST_F(StackerWithRateLimiterTest, BidFillAndHedgeQueuedAndModDownThenCancel)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 11;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[5];

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449925.0, bid[3]));
	strategy->InjectAck(bid[3]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449900.0, bid[4]));
	strategy->InjectAck(bid[4]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450075.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));

	strategy->InjectFill(bid[0], 10);
	strategy->InjectDepth({{499, 450000}, {1000, 449975}}, {{1000, 450025}});
	KTN::OrderPod hedge;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000.0, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = -4;
	strategy->InjectMessage(posMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	posMsg.position = -6;
	strategy->InjectMessage(posMsg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadCancel(hedge));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(60));

	strategy->InjectCancelAck(hedge);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(50));
}

TEST_F(StackerWithRateLimiterTest, HedgingMaxesOutSubmitQueue)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 1;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 5;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 1;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1000;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));

	// Check rate limiter and hedging model initially
	const stacker::RateLimiter &rl = instr->GetStrategyExecution().GetRateLimiter();
	const stacker::HedgingModel &hedgingModel = instr->GetHedgingModel();
	EXPECT_EQ(rl.SubmitQueueSize(), 0);
	
	// Update position to create a large hedging requirement
	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = 514;
	strategy->InjectMessage(posMsg);

	EXPECT_EQ(rl.SubmitQueueSize(), 0);

	strategy->InjectDepth({{499, 450000}, {1000, 449975}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(rl.SubmitQueueSize(), 512);
	EXPECT_TRUE(rl.SubmitQueueFull());

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000.0));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(rl.SubmitQueueSize(), 511);
	EXPECT_FALSE(rl.SubmitQueueFull());

	strategy->InjectDepth({{498, 450000}, {1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(rl.SubmitQueueSize(), 512);
	EXPECT_TRUE(rl.SubmitQueueFull());
}

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}