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


#define QTY(qty) (akl::Quantity { qty })
#define PX(px) (akl::Price::FromUnshifted(px))

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

class StackTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	StackTest()
	{
		
	}

	virtual void SetUp() override
	{
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

		strategy->onLaunch(settings.toJson());

		stacker::AklStackerParams &strategySettings = const_cast<stacker::AklStackerParams &>(strategy->GetSettings());
		quoteSettings = &(strategySettings.quoteInstrumentSettings);
		quoteSettings->newOrders = true;
		sideSettings[0] = strategySettings.quoteInstrumentSettings.bidSettings.stackSideSettings;
		sideSettings[1] = strategySettings.quoteInstrumentSettings.askSettings.stackSideSettings;

		stack = const_cast<stacker::Stack *>(&(strategy->GetStack(0)));
		// stack = new stacker::Stack(
		// 	*quoteSettings,
		// 	*sideSettings[0],
		// 	*sideSettings[1], strategy->GetInstrument(), strategy->GetOrderPool(), strategy->GetOrderSender(), strategy->GetBook(), strategy->GetStrategyMessageResponder());
	}

	virtual void TearDown() override
	{

	}

	stacker::Order *getOrder(const KTN::OrderPod &order)
	{
		return const_cast<stacker::Order *>(strategy->GetInstrument()->GetOrder(order.orderReqId));
	}

	impl::ExchangeSender e;
	AlgoInitializer params;
	StrategyTester<akl::stacker::AklStacker> *strategy { nullptr };
	using Side = KTN::ORD::KOrderSide::Enum;

	stacker::AklStackerParams settings;
	const stacker::PriceConverter *priceConverter { nullptr };

	stacker::Stack *stack { nullptr };
	stacker::QuoteInstrumentSettings *quoteSettings { nullptr };
	stacker::QuoteStackSideSettings *sideSettings[2] { nullptr, nullptr };
};

TEST_F(StackTest, NoOrdersWithoutQuote)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 2;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, NoOrdersWithZeroQty)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 2;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, NoOrdersWithZeroDepth)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, NoOrdersWithNewOrdersFalse)
{
	quoteSettings->newOrders = false;
	
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 2;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, OrdersWithInitialQuote)
{

	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 2;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050));

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsOutOneTick)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449950), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[1], 449900));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449925), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 449875));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449900), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 449850));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449875), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[1], 449825));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449850), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 449800));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksOutOneTick)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(450000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 450075));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450050), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[1], 450100));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450075), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 450125));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450100), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 450150));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450125), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[1], 450175));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450150), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 450200));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsInOneTick)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450025), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 450025));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450050), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[1], 450050));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450075), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 450075));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450100), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 450100));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450125), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[1], 450125));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450150), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 450150));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksInOneTick)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(450000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449975), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 449975));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449950), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[1], 449950));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449925), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 449925));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449900), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 449900));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449875), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[1], 449875));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449850), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 449850));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsOutTwoTicks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449950), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	EXPECT_TRUE(e.ReadModify(bid[1], 449900));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449900), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 449875));
	EXPECT_TRUE(e.ReadModify(bid[0], 449850));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449850), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[1], 449825));
	EXPECT_TRUE(e.ReadModify(bid[2], 449800));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksOutTwoTicks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(450000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450050), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 450075));
	EXPECT_TRUE(e.ReadModify(ask[1], 450100));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450100), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 450125));
	EXPECT_TRUE(e.ReadModify(ask[0], 450150));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450150), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[1], 450175));
	EXPECT_TRUE(e.ReadModify(ask[2], 450200));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsInTwoTicks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450050), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 450025));
	EXPECT_TRUE(e.ReadModify(bid[1], 450050));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450100), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 450075));
	EXPECT_TRUE(e.ReadModify(bid[2], 450100));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450150), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[1], 450125));
	EXPECT_TRUE(e.ReadModify(bid[0], 450150));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksInTwoTicks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(450000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449950), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 449975));
	EXPECT_TRUE(e.ReadModify(ask[1], 449950));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449900), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 449925));
	EXPECT_TRUE(e.ReadModify(ask[2], 449900));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449850), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[1], 449875));
	EXPECT_TRUE(e.ReadModify(ask[0], 449850));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsOutThreeTicks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449925), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	EXPECT_TRUE(e.ReadModify(bid[1], 449900));
	EXPECT_TRUE(e.ReadModify(bid[2], 449875));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449850), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 449850));
	EXPECT_TRUE(e.ReadModify(bid[1], 449825));
	EXPECT_TRUE(e.ReadModify(bid[2], 449800));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksOutThreeTicks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(450000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450075), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 450075));
	EXPECT_TRUE(e.ReadModify(ask[1], 450100));
	EXPECT_TRUE(e.ReadModify(ask[2], 450125));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450150), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 450150));
	EXPECT_TRUE(e.ReadModify(ask[1], 450175));
	EXPECT_TRUE(e.ReadModify(ask[2], 450200));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsInThreeTicks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450075), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 450025));
	EXPECT_TRUE(e.ReadModify(bid[1], 450050));
	EXPECT_TRUE(e.ReadModify(bid[0], 450075));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450150), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 450100));
	EXPECT_TRUE(e.ReadModify(bid[1], 450125));
	EXPECT_TRUE(e.ReadModify(bid[0], 450150));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksInThreeTicks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(450000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449925), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 449975));
	EXPECT_TRUE(e.ReadModify(ask[1], 449950));
	EXPECT_TRUE(e.ReadModify(ask[0], 449925));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449850), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 449900));
	EXPECT_TRUE(e.ReadModify(ask[1], 449875));
	EXPECT_TRUE(e.ReadModify(ask[0], 449850));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsOutFourTicks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449900), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 449900));
	EXPECT_TRUE(e.ReadModify(bid[1], 449875));
	EXPECT_TRUE(e.ReadModify(bid[2], 449850));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449800), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 449800));
	EXPECT_TRUE(e.ReadModify(bid[1], 449775));
	EXPECT_TRUE(e.ReadModify(bid[2], 449750));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksOutFourTicks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(450000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450100), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 450100));
	EXPECT_TRUE(e.ReadModify(ask[1], 450125));
	EXPECT_TRUE(e.ReadModify(ask[2], 450150));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450200), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 450200));
	EXPECT_TRUE(e.ReadModify(ask[1], 450225));
	EXPECT_TRUE(e.ReadModify(ask[2], 450250));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsInFourTicks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450100), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 450050));
	EXPECT_TRUE(e.ReadModify(bid[1], 450075));
	EXPECT_TRUE(e.ReadModify(bid[0], 450100));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450200), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 450150));
	EXPECT_TRUE(e.ReadModify(bid[1], 450175));
	EXPECT_TRUE(e.ReadModify(bid[0], 450200));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksInFourTicks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(450100), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450000), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 450050));
	EXPECT_TRUE(e.ReadModify(ask[1], 450025));
	EXPECT_TRUE(e.ReadModify(ask[0], 450000));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(449900), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 449950));
	EXPECT_TRUE(e.ReadModify(ask[1], 449925));
	EXPECT_TRUE(e.ReadModify(ask[0], 449900));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsOutTwentyTicks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449500), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 449500));
	EXPECT_TRUE(e.ReadModify(bid[1], 449475));
	EXPECT_TRUE(e.ReadModify(bid[2], 449450));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449000), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 449000));
	EXPECT_TRUE(e.ReadModify(bid[1], 448975));
	EXPECT_TRUE(e.ReadModify(bid[2], 448950));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksOutTwentyTicks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(450100), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450100, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450125, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450150, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(450600), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 450600));
	EXPECT_TRUE(e.ReadModify(ask[1], 450625));
	EXPECT_TRUE(e.ReadModify(ask[2], 450650));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(451100), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 451100));
	EXPECT_TRUE(e.ReadModify(ask[1], 451125));
	EXPECT_TRUE(e.ReadModify(ask[2], 451150));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsInTwentyTicks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450500), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 450450));
	EXPECT_TRUE(e.ReadModify(bid[1], 450475));
	EXPECT_TRUE(e.ReadModify(bid[0], 450500));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(451000), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 450950));
	EXPECT_TRUE(e.ReadModify(bid[1], 450975));
	EXPECT_TRUE(e.ReadModify(bid[0], 451000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksInTwentyTicks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(452000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 452000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 452025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 452050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(451500), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 451550));
	EXPECT_TRUE(e.ReadModify(ask[1], 451525));
	EXPECT_TRUE(e.ReadModify(ask[0], 451500));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(451000), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 451050));
	EXPECT_TRUE(e.ReadModify(ask[1], 451025));
	EXPECT_TRUE(e.ReadModify(ask[0], 451000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsOut512Ticks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(451200), PX(451225), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 451200, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 451175, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 451150, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(438400), PX(451225), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 438400));
	EXPECT_TRUE(e.ReadModify(bid[1], 438375));
	EXPECT_TRUE(e.ReadModify(bid[2], 438350));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(425600), PX(451225), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 425600));
	EXPECT_TRUE(e.ReadModify(bid[1], 425575));
	EXPECT_TRUE(e.ReadModify(bid[2], 425550));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksOut512Ticks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(400000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 400000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 400025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 400050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(412800), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 412800));
	EXPECT_TRUE(e.ReadModify(ask[1], 412825));
	EXPECT_TRUE(e.ReadModify(ask[2], 412850));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(425600), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 425600));
	EXPECT_TRUE(e.ReadModify(ask[1], 425625));
	EXPECT_TRUE(e.ReadModify(ask[2], 425650));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsIn512Ticks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(400000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 400000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 399975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 399950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(412800), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 412750));
	EXPECT_TRUE(e.ReadModify(bid[1], 412775));
	EXPECT_TRUE(e.ReadModify(bid[0], 412800));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(425600), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 425550));
	EXPECT_TRUE(e.ReadModify(bid[1], 425575));
	EXPECT_TRUE(e.ReadModify(bid[0], 425600));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksIn512Ticks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(451200), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 451200, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 451225, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 451250, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(438400), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 438450));
	EXPECT_TRUE(e.ReadModify(ask[1], 438425));
	EXPECT_TRUE(e.ReadModify(ask[0], 438400));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(425600), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 425650));
	EXPECT_TRUE(e.ReadModify(ask[1], 425625));
	EXPECT_TRUE(e.ReadModify(ask[0], 425600));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsOut513Ticks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(451200), PX(451225), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 451200, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 451175, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 451150, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(438375), PX(451225), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 438400));
	EXPECT_TRUE(e.ReadModify(bid[1], 438375));
	EXPECT_TRUE(e.ReadModify(bid[2], 438350));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(438375), PX(451225), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 438325));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksOut513Ticks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(400000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 400000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 400025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 400050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(412800), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 412800));
	EXPECT_TRUE(e.ReadModify(ask[1], 412825));
	EXPECT_TRUE(e.ReadModify(ask[2], 412850));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(425600), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 425600));
	EXPECT_TRUE(e.ReadModify(ask[1], 425625));
	EXPECT_TRUE(e.ReadModify(ask[2], 425650));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsIn513Ticks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(400000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 400000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 399975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 399950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(412825), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 412750));
	EXPECT_TRUE(e.ReadModify(bid[1], 412775));
	EXPECT_TRUE(e.ReadModify(bid[0], 412800));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(412825), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 412825));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksIn513Ticks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(451200), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 451200, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 451225, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 451250, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(438375), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 438450));
	EXPECT_TRUE(e.ReadModify(ask[1], 438425));
	EXPECT_TRUE(e.ReadModify(ask[0], 438400));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(438375), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 438375));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsOut1024Ticks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(451200), PX(451225), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 451200, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 451175, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 451150, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(425600), PX(451225), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 438400));
	EXPECT_TRUE(e.ReadModify(bid[1], 438375));
	EXPECT_TRUE(e.ReadModify(bid[2], 438350));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(425600), PX(451225), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 425600));
	EXPECT_TRUE(e.ReadModify(bid[1], 425575));
	EXPECT_TRUE(e.ReadModify(bid[2], 425550));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(400000), PX(451225), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 412800));
	EXPECT_TRUE(e.ReadModify(bid[1], 412775));
	EXPECT_TRUE(e.ReadModify(bid[2], 412750));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(400000), PX(451225), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 400000));
	EXPECT_TRUE(e.ReadModify(bid[1], 399975));
	EXPECT_TRUE(e.ReadModify(bid[2], 399950));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksOut1024Ticks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(400000), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 400000, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 400025, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 400050, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(425600), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 412800));
	EXPECT_TRUE(e.ReadModify(ask[1], 412825));
	EXPECT_TRUE(e.ReadModify(ask[2], 412850));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(425600), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 425600));
	EXPECT_TRUE(e.ReadModify(ask[1], 425625));
	EXPECT_TRUE(e.ReadModify(ask[2], 425650));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(451200), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 438400));
	EXPECT_TRUE(e.ReadModify(ask[1], 438425));
	EXPECT_TRUE(e.ReadModify(ask[2], 438450));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(451200), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 451200));
	EXPECT_TRUE(e.ReadModify(ask[1], 451225));
	EXPECT_TRUE(e.ReadModify(ask[2], 451250));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveBidsIn1024Ticks)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(0);
	sideSettings[1]->length = 0;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(400000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 400000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 399975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 399950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(425600), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 412750));
	EXPECT_TRUE(e.ReadModify(bid[1], 412775));
	EXPECT_TRUE(e.ReadModify(bid[0], 412800));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(425600), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 425550));
	EXPECT_TRUE(e.ReadModify(bid[1], 425575));
	EXPECT_TRUE(e.ReadModify(bid[0], 425600));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(451200), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 438350));
	EXPECT_TRUE(e.ReadModify(bid[1], 438375));
	EXPECT_TRUE(e.ReadModify(bid[0], 438400));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(451200), PX(450025), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 451150));
	EXPECT_TRUE(e.ReadModify(bid[1], 451175));
	EXPECT_TRUE(e.ReadModify(bid[0], 451200));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, MoveAsksIn1024Ticks)
{
	sideSettings[0]->qty = QTY(0);
	sideSettings[0]->length = 0;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(449975), PX(451200), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 451200, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 451225, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 451250, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(425600), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 438450));
	EXPECT_TRUE(e.ReadModify(ask[1], 438425));
	EXPECT_TRUE(e.ReadModify(ask[0], 438400));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(425600), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 425650));
	EXPECT_TRUE(e.ReadModify(ask[1], 425625));
	EXPECT_TRUE(e.ReadModify(ask[0], 425600));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(400000), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 412850));
	EXPECT_TRUE(e.ReadModify(ask[1], 412825));
	EXPECT_TRUE(e.ReadModify(ask[0], 412800));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), PX(400000), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 400050));
	EXPECT_TRUE(e.ReadModify(ask[1], 400025));
	EXPECT_TRUE(e.ReadModify(ask[0], 400000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteWithinPriceBands)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450050);
	sideSettings[0]->outsidePrice = PX(449900);
	sideSettings[1]->insidePrice = PX(449975);
	sideSettings[1]->outsidePrice = PX(450125);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[2]));
	strategy->InjectAck(ask[2]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteWithinPriceBands2)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450000);
	sideSettings[0]->outsidePrice = PX(449950);
	sideSettings[1]->insidePrice = PX(450025);
	sideSettings[1]->outsidePrice = PX(450075);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[2]));
	strategy->InjectAck(ask[2]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteExceedingInsideBidPriceBand)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(449975);
	sideSettings[0]->outsidePrice = PX(449950);
	sideSettings[1]->insidePrice = PX(450025);
	sideSettings[1]->outsidePrice = PX(450075);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[1]));
	strategy->InjectAck(bid[1]);

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[2]));
	strategy->InjectAck(ask[2]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteExceedingInsideBidPriceBand2)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(449975);
	sideSettings[1]->insidePrice = PX(450025);
	sideSettings[1]->outsidePrice = PX(450075);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[1]));
	strategy->InjectAck(bid[1]);

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[2]));
	strategy->InjectAck(ask[2]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteExceedingOutsideBidPriceBand)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450000);
	sideSettings[0]->outsidePrice = PX(449975);
	sideSettings[1]->insidePrice = PX(450025);
	sideSettings[1]->outsidePrice = PX(450075);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[2]));
	strategy->InjectAck(ask[2]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteExceedingOutsideBidPriceBand2)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->outsidePrice = PX(449975);
	sideSettings[1]->insidePrice = PX(450025);
	sideSettings[1]->outsidePrice = PX(450075);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[2]));
	strategy->InjectAck(ask[2]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteExceedingInsideAskPriceBand)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450000);
	sideSettings[0]->outsidePrice = PX(449950);
	sideSettings[1]->insidePrice = PX(450050);
	sideSettings[1]->outsidePrice = PX(450075);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[1]));
	strategy->InjectAck(ask[1]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteExceedingInsideAskPriceBand2)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450000);
	sideSettings[1]->insidePrice = PX(450050);
	sideSettings[1]->outsidePrice = PX(450075);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[1]));
	strategy->InjectAck(ask[1]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteExceedingOutsideAskPriceBand)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450000);
	sideSettings[0]->outsidePrice = PX(449950);
	sideSettings[1]->insidePrice = PX(450025);
	sideSettings[1]->outsidePrice = PX(450050);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteExceedingOutsideAskPriceBand2)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450000);
	sideSettings[0]->outsidePrice = PX(449950);
	sideSettings[1]->outsidePrice = PX(450050);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);

	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteWithEqualPriceBands)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(449975);
	sideSettings[0]->outsidePrice = PX(449975);
	sideSettings[1]->insidePrice = PX(450050);
	sideSettings[1]->outsidePrice = PX(450050);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[1];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[0]));
	strategy->InjectAck(bid[0]);

	KTN::OrderPod ask[1];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[0]));
	strategy->InjectAck(ask[0]);

	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, InitialQuoteWithCrossedPriceBands)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(449950);
	sideSettings[0]->outsidePrice = PX(449975);
	sideSettings[1]->insidePrice = PX(450050);
	sideSettings[1]->outsidePrice = PX(450025);
	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidsMovingInWithNewPriceOutsideBands)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450025);
	sideSettings[0]->outsidePrice = PX(449950);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), std::nullopt, true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450025), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[2], 450025));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450050), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450075), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450100), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(bid[2]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidsMovingInWithNewPriceOutsideBandsWithCancelNew)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;
	quoteSettings->moveWithCancelNew = true;

	sideSettings[0]->insidePrice = PX(450025);
	sideSettings[0]->outsidePrice = PX(449950);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), std::nullopt, true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450025), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(bid[2]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450025, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450050), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450075), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450100), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(bid[2]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidsMovingInWithOldPriceOutsideBands)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450125);
	sideSettings[0]->outsidePrice = PX(450000);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), std::nullopt, true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450025), std::nullopt, false);
	stack->SendQueuedSubmits();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450025, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450050), std::nullopt, false);
	stack->SendQueuedSubmits();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450050, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450075), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[0], 450075));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450100), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(bid[1], 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidsMovingInWithOldPriceOutsideBandsWithCancelNew)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;
	quoteSettings->moveWithCancelNew = true;

	sideSettings[0]->insidePrice = PX(450125);
	sideSettings[0]->outsidePrice = PX(450000);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), std::nullopt, true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450025), std::nullopt, false);
	stack->SendQueuedSubmits();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450025, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450050), std::nullopt, false);
	stack->SendQueuedSubmits();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450050, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450075), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450075, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450100), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450100, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, AsksMovingInWithNewPriceOutsideBands)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;

	sideSettings[1]->insidePrice = PX(450000);
	sideSettings[1]->outsidePrice = PX(450075);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(std::nullopt, PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(450000), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[2], 450000));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449975), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449950), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449925), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(ask[2]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, AsksMovingInWithNewPriceOutsideBandsWithCancelNew)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	quoteSettings->moveWithCancelNew = true;

	sideSettings[1]->insidePrice = PX(450000);
	sideSettings[1]->outsidePrice = PX(450075);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(std::nullopt, PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450075, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(450000), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(ask[2]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449975), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449950), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449925), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(ask[2]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, AsksMovingInWithOldPriceOutsideBands)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;

	sideSettings[1]->insidePrice = PX(449900);
	sideSettings[1]->outsidePrice = PX(450025);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(std::nullopt, PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(450000), false);
	stack->SendQueuedSubmits();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449975), false);
	stack->SendQueuedSubmits();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 449975, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449950), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[0], 449950));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449925), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadModify(ask[1], 449925));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, AsksMovingInWithOldPriceOutsideBandsWithCancelNew)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(200);
	sideSettings[1]->length = 3;
	quoteSettings->moveWithCancelNew = true;

	sideSettings[1]->insidePrice = PX(449900);
	sideSettings[1]->outsidePrice = PX(450025);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(std::nullopt, PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(450000), false);
	stack->SendQueuedSubmits();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 450000, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449975), false);
	stack->SendQueuedSubmits();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 449975, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449950), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 449950, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(std::nullopt, PX(449925), false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 200, 449925, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidOutsidePriceMovedIn)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450000);
	sideSettings[0]->outsidePrice = PX(449950);
	sideSettings[1]->insidePrice = PX(450025);
	sideSettings[1]->outsidePrice = PX(450075);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[2]));
	strategy->InjectAck(ask[2]);

	EXPECT_TRUE(e.ReadNothing());

	sideSettings[0]->outsidePrice = PX(449975);
	stack->Update(PX(450000), PX(450025), false);
	stack->SendQueuedSubmits();
	stack->HandlePriceBandUpdate();
	
	EXPECT_TRUE(e.ReadCancel(bid[2]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidInsidePriceMovedOut)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450000);
	sideSettings[0]->outsidePrice = PX(449925);
	sideSettings[1]->insidePrice = PX(450025);
	sideSettings[1]->outsidePrice = PX(450075);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[2]));
	strategy->InjectAck(ask[2]);

	EXPECT_TRUE(e.ReadNothing());

	sideSettings[0]->insidePrice = PX(449975);
	stack->Update(PX(450000), PX(450025), false);
	stack->SendQueuedSubmits();
	stack->HandlePriceBandUpdate();
	
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidInsidePriceMovedOut2)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450000);
	sideSettings[0]->outsidePrice = PX(449925);
	sideSettings[1]->insidePrice = PX(450025);
	sideSettings[1]->outsidePrice = PX(450075);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), PX(450025), true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);

	KTN::OrderPod ask[3];
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450025, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450050, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450075, ask[2]));
	strategy->InjectAck(ask[2]);

	EXPECT_TRUE(e.ReadNothing());

	sideSettings[0]->insidePrice = PX(449975);
	stack->Update(PX(449975), PX(450025), false);
	stack->SendQueuedSubmits();
	stack->HandlePriceBandUpdate();
	
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidsDontRequoteRemovedOrderWhenMovingBackInsidePriceBands)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450025);
	sideSettings[0]->outsidePrice = PX(449950);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), std::nullopt, true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->RemoveOrder(getOrder(bid[0]), false);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450000), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidsDontRequoteRemovedOrderWhenMovingBackInsidePriceBands2)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450025);
	sideSettings[0]->outsidePrice = PX(449950);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), std::nullopt, true);

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450000), std::nullopt, false);
	stack->SendQueuedSubmits();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	stack->RemoveOrder(getOrder(bid[0]), false);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450000), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidsDontRequoteRemovedOrderWhenMovingBackInsidePriceBandsWithCancelNew)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450025);
	sideSettings[0]->outsidePrice = PX(449950);

	quoteSettings->moveWithCancelNew = true;

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), std::nullopt, true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->RemoveOrder(getOrder(bid[0]), false);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450000), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidsDontRequoteFilledOrderWhenMovingBackInsidePriceBands)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450025);
	sideSettings[0]->outsidePrice = PX(449950);

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), std::nullopt, true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->RemoveOrder(getOrder(bid[0]), true);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450000), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StackTest, BidsDontRequoteFilledOrderWhenMovingBackInsidePriceBandsWithCancelNew)
{
	sideSettings[0]->qty = QTY(200);
	sideSettings[0]->length = 3;
	sideSettings[1]->qty = QTY(100);
	sideSettings[1]->length = 3;

	sideSettings[0]->insidePrice = PX(450025);
	sideSettings[0]->outsidePrice = PX(449950);

	quoteSettings->moveWithCancelNew = true;

	stack->SetDefaultStackQuantity(Quantity(sideSettings[0]->qty), Quantity(sideSettings[1]->qty));

	EXPECT_TRUE(e.ReadNothing());
	stack->Update(PX(450000), std::nullopt, true);
	stack->SendQueuedSubmits();

	KTN::OrderPod bid[3];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 200, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	stack->RemoveOrder(getOrder(bid[0]), true);
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(449975), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());

	stack->Update(PX(450000), std::nullopt, false);
	stack->SendQueuedSubmits();
	EXPECT_TRUE(e.ReadNothing());
}

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
