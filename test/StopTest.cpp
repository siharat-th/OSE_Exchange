#include <gtest/gtest.h>

#include <Algos/AklStacker/AklStacker.hpp>
#include <Algos/AklStacker/Instrument.hpp>
#include "AlgoParams/AlgoInitializer.hpp"
#include "impl/ExchangeSender.hpp"
#include "Helpers.hpp"

#include "impl/StrategyTester.hpp"

#include <Algos/AklStacker/StrategyMessagePublisher.hpp>
#include <Algos/AklStacker/messages/StackerRequoteMessage.hpp>
#include <Algos/AklStacker/messages/StackerPriceAdoptMessage.hpp>
#include <Algos/AklStacker/messages/StackerPriceAbandonMessage.hpp>
#include <Algos/AklStacker/messages/StackerPositionUpdateMessage.hpp>
#include <Algos/AklStacker/messages/StackerClearPositionMessage.hpp>
#include <Algos/AklStacker/messages/StackerAdoptMessage.hpp>
#include <Algos/AklStacker/messages/StopUpdateMessage.hpp>

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

class StopTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	StopTest()
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

	using StopAction = stacker::StopUpdateMessage::Action;
	using Side = KTN::ORD::KOrderSide::Enum;

	stacker::StopUpdateMessage createStop(
		const KTN::ORD::KOrderSide::Enum side,
		const int qty,
		const int triggerQty,
		const double px,
		const StopAction action,
		bool deleteOnFlip = false,
		const int lean = 0,
		const bool sendOnTrade = false)
	{
		const std::optional<int8_t> aklSide = stacker::KtnSideToAklSide(side);
		EXPECT_NE(aklSide, std::nullopt);
		if (aklSide)
		{
			stacker::StopUpdateMessage msg;
			msg.side = side;
			msg.qty = Quantity(qty);
			msg.triggerQty = Quantity(triggerQty);
			msg.price = Price::FromUnshifted(px);
			msg.action = action;
			msg.deleteOnFlip = deleteOnFlip ? 1 : 0;
			msg.leanQty = Quantity(lean);
			msg.hasLean = lean > 0 ? 1 : 0;
			msg.sendOnTrade = sendOnTrade ? 1 : 0;
			return msg;
		}
		else
		{
			return stacker::StopUpdateMessage{};
		}
	}

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

	bool ReadNoStop()
	{
		return ReadNoMessage<stacker::StopUpdateMessage>();
	}

	bool ReadStop(
		Side side,
		int qty,
		int triggerQty,
		double px,
		StopAction action,
		bool deleteOnFlip = false,
		const int lean = 0,
		bool sendOnTrade = false)
	{
		stacker::StopUpdateMessage msg;
		if (ReadMessage(msg))
		{
			EXPECT_EQ(msg.side, side);
			EXPECT_EQ(msg.qty, Quantity(qty));
			EXPECT_EQ(msg.triggerQty, Quantity(triggerQty));
			EXPECT_EQ(msg.price, Price::FromUnshifted(px));
			EXPECT_EQ(msg.action, action);
			EXPECT_EQ(msg.deleteOnFlip, deleteOnFlip ? 1 : 0);
			EXPECT_EQ(msg.leanQty, Quantity(lean));
			EXPECT_EQ(msg.hasLean, lean > 0 ? 1 : 0);
			EXPECT_EQ(msg.sendOnTrade, sendOnTrade ? 1 : 0);
			return msg.side == side
				&& msg.qty == Quantity(qty)
				&& msg.triggerQty == Quantity(triggerQty)
				&& msg.price == Price::FromUnshifted(px)
				&& msg.action == action
				&& msg.deleteOnFlip == (deleteOnFlip ? 1 : 0)
				&& msg.leanQty == Quantity(lean)
				&& msg.hasLean == (lean > 0 ? 1 : 0)
				&& msg.sendOnTrade == (sendOnTrade ? 1 : 0);
		}
		return false;
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

	impl::ExchangeSender e;
	AlgoInitializer params;
	StrategyTester<akl::stacker::AklStacker> *strategy { nullptr };

	stacker::AklStackerParams settings;
	const stacker::PriceConverter *priceConverter { nullptr };
};

TEST_F(StopTest, SendSellStop)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, SendBuyStop)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, SendSellStopAfterIncreasingQuantity)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	msg = createStop(Side::SELL, 15, 900, 450000.0, StopAction::UPDATE);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 15, 900, 450000.0, StopAction::UPDATE));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 15, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 15, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, SendBuyStopAfterIncreasingQuantity)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));

	msg = createStop(Side::BUY, 15, 900, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 15, 900, 450025.0, StopAction::UPDATE));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 15, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 15, 900, 450025.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, SendSellStopOnLtp)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectTrade(Side::SELL, 1, 450000);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, SendBuyStopOnLtp)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectTrade(Side::BUY, 1, 450100);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, SendSellStopOnFullLtp)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	KTN::OrderPod o;
	strategy->InjectTrade(Side::SELL, 1000, 450000);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
}

TEST_F(StopTest, SendBuyStopOnFullLtp)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}, {1000, 450050}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));

	KTN::OrderPod o;
	strategy->InjectTrade(Side::BUY, 1000, 450025);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, SendSellStopOnGap)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
}

TEST_F(StopTest, SendBuyStopOnGap)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}, {1000, 450050}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, SendSellStopOnFlip)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 449975}}, {{1, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
}

TEST_F(StopTest, SendBuyStopOnFlip)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}, {1000, 450050}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));

	KTN::OrderPod o;
	strategy->InjectDepth({{1, 450025}, {1000, 450000}, {1000, 449975}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, SendMultipleSellStopsOnSweep)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}, {1000, 449925}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();

	stacker::StopUpdateMessage msg1 = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 449950, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449950.0, StopAction::DELETE));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::DELETE));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
}

TEST_F(StopTest, SendMultipleBuyStopsOnSweep)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}, {1000, 450100}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();

	stacker::StopUpdateMessage msg1 = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::DELETE));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450050.0, StopAction::DELETE));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, SendMultipleSellStopsOnSweep2)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}, {1000, 449925}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg1 = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o[3];
	strategy->InjectDepth({{899, 449950}, {1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 449950, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendMultipleBuyStopsOnSweep2)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}, {1000, 450100}});

	stacker::StopUpdateMessage msg1 = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{899, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendMultipleSellStopsOnSweep3)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}, {1000, 449925}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg1 = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW);
	stacker::StopUpdateMessage msg4 = createStop(Side::SELL, 10, 900, 449925.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW));
	strategy->InjectMessage(msg4);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449925.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 449950, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendMultipleBuyStopsOnSweep3)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}, {1000, 450100}});

	stacker::StopUpdateMessage msg1 = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW);
	stacker::StopUpdateMessage msg4 = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW));
	strategy->InjectMessage(msg4);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendMultipleSellStopsOnSweepWithMaxOrderQty)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(7);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}, {1000, 449925}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg1 = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o[6];
	strategy->InjectDepth({{899, 449950}, {1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 7, 449950, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 3, 449950, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 7, 449975, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 3, 449975, o[3]));
	strategy->InjectAck(o[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 7, 450000, o[4]));
	strategy->InjectAck(o[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 3, 450000, o[5]));
	strategy->InjectAck(o[5]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendMultipleBuyStopsOnSweepWithMaxOrderQty)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(7);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}, {1000, 450100}});

	stacker::StopUpdateMessage msg1 = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o[6];
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{899, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 7, 450075, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 3, 450075, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 7, 450050, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 3, 450050, o[3]));
	strategy->InjectAck(o[3]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 7, 450025, o[4]));
	strategy->InjectAck(o[4]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 3, 450025, o[5]));
	strategy->InjectAck(o[5]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendMultipleSellStopsOnSweepWithLongPosition)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 5;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 5;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}, {1000, 449925}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg1 = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = 5;
	strategy->InjectMessage(posMsg);

	KTN::OrderPod hedge;
	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 449950, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 5, 449950, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendMultipleBuyStopsOnSweepWithShortPosition)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 5;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 5;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}, {1000, 450100}});

	stacker::StopUpdateMessage msg1 = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = -5;
	strategy->InjectMessage(posMsg);

	KTN::OrderPod hedge;
	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 5, 450075, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendMultipleSellStopsOnSweepWithShortPosition)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 5;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 5;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}, {1000, 449950}, {1000, 449925}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg1 = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = -5;
	strategy->InjectMessage(posMsg);

	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 449925}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 449950, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 449975, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendMultipleBuyStopsOnSweepWithLongPosition)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.maxPayupTicks = 5;
	settings.quoteInstrumentSettings.askSettings.maxPayupTicks = 5;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450025}, {1000, 450050}, {1000, 450075}, {1000, 450100}});

	stacker::StopUpdateMessage msg1 = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	stacker::StopUpdateMessage msg2 = createStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW);
	stacker::StopUpdateMessage msg3 = createStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW);
	
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW));
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = 5;
	strategy->InjectMessage(posMsg);

	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 450000}, {1000, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, o[0]));
	strategy->InjectAck(o[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450050, o[1]));
	strategy->InjectAck(o[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendSellStopImmediately)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	KTN::OrderPod o;
	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 1001, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendBuyStopImmediately)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	KTN::OrderPod o;
	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 1001, 450100.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SendSellStopAndHedge)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = 7;
	strategy->InjectMessage(posMsg);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(7));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod hedge;
	KTN::OrderPod stop;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 7, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectFill(hedge, 7);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(stop, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, SendBuyStopAndHedge)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = -7;
	strategy->InjectMessage(posMsg);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(-7));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod hedge;
	KTN::OrderPod stop;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 7, 450100, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectFill(hedge, 7);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(stop, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, DontSendSellStopWithHedgeWhenStopTriggerIsLower)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(900);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = 7;
	strategy->InjectMessage(posMsg);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(7));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 100, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 100, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 7, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	strategy->InjectDepth({{100, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod stop;
	strategy->InjectDepth({{99, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 100, 450000.0, StopAction::DELETE));

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectFill(hedge, 7);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(stop, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, DontSendBuyStopOnHedgeWhenStopTriggerIsLower)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(900);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = -7;
	strategy->InjectMessage(posMsg);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(-7));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 100, 450100.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 100, 450100.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod hedge;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 7, 450100, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	strategy->InjectDepth({{1000, 450000}}, {{100, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod stop;
	strategy->InjectDepth({{1000, 450000}}, {{99, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 100, 450100.0, StopAction::DELETE));

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectFill(hedge, 7);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(stop, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, SendSellStopAndWithShortPosition)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = -7;
	strategy->InjectMessage(posMsg);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(-7));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod stop;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectFill(stop, 10);
	EXPECT_EQ(pm.Position(), Quantity(-7));
}

TEST_F(StopTest, SendBuyStopWithLongPosition)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = 7;
	strategy->InjectMessage(posMsg);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(7));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod stop;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE));

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectFill(stop, 10);
	EXPECT_EQ(pm.Position(), Quantity(7));
}

TEST_F(StopTest, MoveBidStackBasedOnSellStop)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->InjectMessage(stacker::StackerRequoteMessage {});

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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *stopOrder = instr->GetOrder(o.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}, {200, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(bid[0], 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, MoveAskStackBasedOnBuyStop)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->InjectMessage(stacker::StackerRequoteMessage {});

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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450175));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}, {200, 450125}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(ask[0], 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, CancelBidProrataOnSellStop)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {500}, {100}, {0}, {0}, 1);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *stopOrder = instr->GetOrder(o.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, CancelAskProrataOnSellStop)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {500}, {100}, {0}, {0}, 1);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, SendSellStopOnLargeTrade)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 0, true);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 0, true));

	strategy->InjectTrade(Side::SELL, 499, 450000);

	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o;
	strategy->InjectTrade(Side::SELL, 1, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, false, 0, true));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, SendBuyStopOnLargeTrade)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 0, true);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 0, true));

	strategy->InjectTrade(Side::BUY, 499, 450100);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o;
	strategy->InjectTrade(Side::BUY, 1, 450100);
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, false, 0, true));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, SendSellStopOnLargeFill)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 0, true);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 0, true));

	strategy->InjectFill(bid, 24);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	KTN::OrderPod stop;
	strategy->InjectFill(bid, 1);
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 25, 450000, hedge));
	strategy->InjectAck(hedge);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, false, 0, true));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);
}

TEST_F(StopTest, SendBuyStopOnLargeFill)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 0, true);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 0, true));

	strategy->InjectFill(ask, 24);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	KTN::OrderPod stop;
	strategy->InjectFill(ask, 1);
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 25, 450100, hedge));
	strategy->InjectAck(hedge);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, false, 0, true));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);
}

TEST_F(StopTest, DontSendSellStopOnLargeTrade)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 0, false);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 0, false));

	strategy->InjectTrade(Side::SELL, 499, 450000);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(Side::SELL, 1, 450000);
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);

	strategy->InjectTrade(Side::SELL, 600, 450000);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o;
	strategy->InjectTrade(Side::SELL, 1, 450000);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, false, 0, false));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, DontSendBuyStopOnLargeTrade)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeTrade = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 0, false);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 0, false));

	strategy->InjectTrade(Side::BUY, 499, 450100);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectTrade(Side::BUY, 1, 450100);
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	strategy->InjectTrade(Side::BUY, 600, 450100);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod o;
	strategy->InjectTrade(Side::BUY, 1, 450100);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, false, 0, false));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, DontSendSellStopOnLargeFill)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 0, false);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 0, false));

	strategy->InjectFill(bid, 24);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectFill(bid, 1);
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 25, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);

	KTN::OrderPod stop;
	strategy->InjectTrade(Side::SELL, 1101, 450000);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, false, 0, false));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);
}

TEST_F(StopTest, DontSendBuyStopOnLargeFill)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.hedgeOnLargeFill = true;
	settings.quoteInstrumentSettings.prorataAllocation = 1.0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {1000}, {100}, {500}, {1000}, 1);
	strategy->InjectDepth({{2000, 450000}}, {{2000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 0, false);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 0, false));

	strategy->InjectFill(ask, 24);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	strategy->InjectFill(ask, 1);
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 25, 450100, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	KTN::OrderPod stop;
	strategy->InjectTrade(Side::BUY, 1101, 450100);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, false, 0, false));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);
}

TEST_F(StopTest, CancelStopsForClientOnTimeout)
{
	settings.quoteInstrumentSettings.newOrders = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	msg.clientId = "111111";
	stacker::StopUpdateMessage msg2 = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	msg2.clientId = "222222";
	strategy->InjectMessage(msg);
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));
	EXPECT_TRUE(ReadNoStop());

	strategy->AdvanceTime(stacker::Seconds(4));
	// strategy->InjectMessage(stacker::HeartbeatMessage { msg.clientId });
	// strategy->InjectMessage(stacker::HeartbeatMessage { msg2.clientId });
	// strategy->ExecuteStatusThread();
	// EXPECT_TRUE(e.ReadNothing());
	// EXPECT_TRUE(ReadNoStop());
	// EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	// EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);

	strategy->AdvanceTime(stacker::Seconds(4));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);

	// strategy->InjectMessage(stacker::HeartbeatMessage { msg2.clientId });
	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	strategy->onClientDisconnected(msg.clientId);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	strategy->AdvanceTime(stacker::Seconds(3));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	strategy->onClientDisconnected(msg2.clientId);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
}

TEST_F(StopTest, CancelStopsOnClose)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msgSell = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	stacker::StopUpdateMessage msgBuy = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);

	strategy->InjectMessage(msgSell);
	strategy->InjectMessage(msgBuy);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::Close);
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_EQ(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, CancelStopsOnHalt)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msgSell = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	stacker::StopUpdateMessage msgBuy = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);

	strategy->InjectMessage(msgSell);
	strategy->InjectMessage(msgBuy);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));
	EXPECT_TRUE(ReadNoStop());

	// Simulate market halt
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::TradingHalt);
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_EQ(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, SendSellStopImmediatelyWhenLevelFlipsInFlight)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
}

TEST_F(StopTest, SendBuyStopImmediatelyWhenLevelFlipsInFlight)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, SendSellStopImmediatelyWhenLevelFlipsInFlightWithMaxOrderQty)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 449975}}, {{1000, 450000}});
	stacker::StopUpdateMessage msg = createStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5, 450000, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::DELETE));
}

TEST_F(StopTest, SendBuyStopImmediatelyWhenLevelFlipsInFlightWithMaxOrderQty)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o[3];
	strategy->InjectDepth({{1000, 450025}}, {{1000, 450050}});
	stacker::StopUpdateMessage msg = createStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o[0]));
	strategy->InjectAck(o[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o[1]));
	strategy->InjectAck(o[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5, 450025, o[2]));
	strategy->InjectAck(o[2]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, SendSellStopImmediatelyWhenLevelGapsInFlight)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
}

TEST_F(StopTest, SendBuyStopImmediatelyWhenLevelGapsInFlight)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, SendSellStopOnOpen)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{101, 450000}, {1000, 450025}});
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{899, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
}

TEST_F(StopTest, SendBuyStopOnOpen)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{101, 450025}, {1000, 450000}}, {{1000, 450025}});
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, KillStopsDuringPreopenWhichWouldMiss)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msgBuy = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	stacker::StopUpdateMessage msgSell = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msgBuy);
	strategy->InjectMessage(msgSell);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
}

// TEST_F(StopTest, SendSellStopWhileStrategyStopped)
// {
// 	req->AddParam<bool>("NewOrders", false);
// 	req->AddParam<double>("StackBidQty", 0.0);
// 	req->AddParam<double>("StackBidLean", 200.0);
// 	req->AddParam<int>("StackBidLength",3);
// 	req->AddParam<double>("StackAskQty", 0.0);
// 	req->AddParam<double>("StackAskLean", 200.0);
// 	req->AddParam<int>("StackAskLength", 3);
// 	req->AddParam<bool>("MoveWithCancelNew", true);

// 	ttsdk::SDKAlgoRequestPtr reqPtr(req);
// 	tt.strategyManager.OnStartRequest(algoOrder, req);
// 	Subscribe();
// 	tt.PublishDepth("ESU3", {{{1000.0, 450000.0}}}, {{{1000.0, 450025.0}}});

// 	const stacker::StopHandler &stopHandler = GetStopHandler();
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_TRUE(ReadNoStop());

// 	messaging::StrategyStatusMessage msg;
// 	msg.id[0] = '1';
// 	msg.id[1] = '\0';
// 	msg.status = 0;
// 	tt.strategyManager.Handle(msg);

// 	SendStop(1, 10, 900, 450000.0, StopAction::NEW);
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 1);
// 	EXPECT_NE(stopHandler.GetStop<Side::ASK>(Price::FromUnshifted(450000.0)), nullptr);
// 	EXPECT_TRUE(ReadStop(1, 10, 900, 450000.0, StopAction::NEW));

// 	ttsdk::OrderPtr o;
// 	tt.PublishDepth("ESU3", {{{899.0, 450000.0}}}, {{{1000.0, 450025.0}}});
// 	ASSERT_TRUE(e.ReadSubmit(ttsdk::OrderSide::Sell, 10.0, 450000.0, o));
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_TRUE(ReadStop(1, 10, 900, 450000.0, StopAction::DELETE));

// 	tt.PublishDepth("ESU3", {{{1000.0, 449975.0}}}, {{{1000.0, 450000.0}}});
// 	EXPECT_TRUE(e.ReadNothing());

// 	OrderType *stopOT = GetOrderType(o);
// 	ASSERT_NE(stopOT, nullptr);
// 	EXPECT_EQ(stopOT->type, OrderType::Type::STOP);

// 	msg.status = 1;
// 	tt.strategyManager.Handle(msg);

// 	EXPECT_EQ(stopOT->type, OrderType::Type::STACK);

// 	messaging::StackerAdoptMessage adoptMsg;
// 	adoptMsg.id[0] = '1';
// 	adoptMsg.id[1] = '\0';
// 	tt.strategyManager.Handle(adoptMsg);

// 	EXPECT_EQ(stopOT->type, OrderType::Type::STACK);
// 	EXPECT_TRUE(e.ReadNothing());

// 	tt.PublishDepth("ESU3", {{{1000.0, 449975.0}}}, {{{199.0, 450000.0}, {1000.0, 450025.0}}});
// 	EXPECT_TRUE(e.ReadCancel(o));
// 	EXPECT_TRUE(e.ReadNothing());
// }

// TEST_F(StopTest, SendBuyStopWhileStrategyStopped)
// {
// 	req->AddParam<bool>("NewOrders", false);
// 	req->AddParam<double>("StackBidQty", 0.0);
// 	req->AddParam<double>("StackBidLean", 200.0);
// 	req->AddParam<int>("StackBidLength",3);
// 	req->AddParam<double>("StackAskQty", 0.0);
// 	req->AddParam<double>("StackAskLean", 200.0);
// 	req->AddParam<int>("StackAskLength", 3);
// 	req->AddParam<bool>("MoveWithCancelNew", true);

// 	ttsdk::SDKAlgoRequestPtr reqPtr(req);
// 	tt.strategyManager.OnStartRequest(algoOrder, req);
// 	Subscribe();
// 	tt.PublishDepth("ESU3", {{{1000.0, 450000.0}}}, {{{1000.0, 450025.0}}});

// 	const stacker::StopHandler &stopHandler = GetStopHandler();
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_TRUE(ReadNoStop());

// 	messaging::StrategyStatusMessage msg;
// 	msg.id[0] = '1';
// 	msg.id[1] = '\0';
// 	msg.status = 0;
// 	tt.strategyManager.Handle(msg);

// 	SendStop(0, 10, 900, 450025.0, StopAction::NEW);
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 1);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_NE(stopHandler.GetStop<Side::BID>(Price::FromUnshifted(450025.0)), nullptr);
// 	EXPECT_TRUE(ReadStop(0, 10, 900, 450025.0, StopAction::NEW));

// 	ttsdk::OrderPtr o;
// 	tt.PublishDepth("ESU3", {{{1000.0, 450000.0}}}, {{{899.0, 450025.0}}});
// 	ASSERT_TRUE(e.ReadSubmit(ttsdk::OrderSide::Buy, 10.0, 450025.0, o));
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_TRUE(ReadStop(0, 10, 900, 450025.0, StopAction::DELETE));

// 	tt.PublishDepth("ESU3", {{{1000.0, 450025.0}}}, {{{1000.0, 450050.0}}});
// 	EXPECT_TRUE(e.ReadNothing());

// 	OrderType *stopOT = GetOrderType(o);
// 	ASSERT_NE(stopOT, nullptr);
// 	EXPECT_EQ(stopOT->type, OrderType::Type::STOP);

// 	msg.status = 1;
// 	tt.strategyManager.Handle(msg);

// 	EXPECT_EQ(stopOT->type, OrderType::Type::STACK);

// 	messaging::StackerAdoptMessage adoptMsg;
// 	adoptMsg.id[0] = '1';
// 	adoptMsg.id[1] = '\0';
// 	tt.strategyManager.Handle(adoptMsg);

// 	EXPECT_EQ(stopOT->type, OrderType::Type::STACK);
// 	EXPECT_TRUE(e.ReadNothing());

// 	tt.PublishDepth("ESU3", {{{199.0, 450025.0}, {1000.0, 450000.0}}}, {{{1000.0, 450050.0}}});
// 	EXPECT_TRUE(e.ReadCancel(o));
// 	EXPECT_TRUE(e.ReadNothing());
// }

TEST_F(StopTest, RejectNewStopAtSamePriceAsWorkingStop)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg1 = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg2 = createStop(Side::SELL, 20, 2000, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_EQ(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0))->quantity, Quantity(10));
	EXPECT_EQ(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0))->triggerQuantity, Quantity(900));
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 2000, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, RejectUpdateToStopWhichTriggered)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg1 = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg1);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	stacker::StopUpdateMessage msg2 = createStop(Side::SELL, 15, 900, 450000.0, StopAction::UPDATE);
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 15, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, Hedging_20240118_142139)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(50);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(355);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(350);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(50);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(505);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{400, 450000}, {1400, 449975}}, {{480, 450025}, {900, 450050}});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 50, 450000, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 50, 449975, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 50, 449950, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 50, 450050, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 50, 450075, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 50, 450100, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 25, 100, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{348, 450000}, {1400, 449975}}, {{480, 450025}, {900, 450050}});
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 50, 449925, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod stop;
	strategy->InjectDepth({{97, 450000}, {1400, 449975}}, {{480, 450025}, {900, 450050}});
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 25, 450000, stop));
	strategy->InjectAck(stop);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 1);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 14);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 14, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 1);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 2);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 2, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 16);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 16, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 1);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 6);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 6, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 1);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 1);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid[0], 7);
	EXPECT_TRUE(e.ReadSubmit(Side::SELL, 7, 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, DeleteBidStopsOnBidMoveUp)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectMessage(createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, true));
	strategy->InjectMessage(createStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW, false));
	strategy->InjectMessage(createStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW, true));
	strategy->InjectMessage(createStop(Side::SELL, 10, 900, 449925.0, StopAction::NEW, false));
	strategy->InjectMessage(createStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW, true));
	strategy->InjectMessage(createStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW, false));
	strategy->InjectMessage(createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, true));
	strategy->InjectMessage(createStop(Side::BUY, 10, 900, 450125.0, StopAction::NEW, false));

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 4);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 4);

	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, true));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW, false));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW, true));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449925.0, StopAction::NEW, false));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW, true));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW, false));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, true));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450125.0, StopAction::NEW, false));
	EXPECT_TRUE(ReadNoStop());

	// Simulate a price move that would trigger deleteOnFlip stops
	strategy->InjectDepth({{1, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 4);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 2);

	// Check that the stops with deleteOnFlip=true were deleted
	EXPECT_TRUE(ReadStop(KTN::ORD::KOrderSide::Enum::SELL, 10, 900, 449950.0, StopAction::DELETE, true));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, true));
	EXPECT_TRUE(ReadNoStop());

	// Update a stop to have deleteOnFlip=true
	strategy->InjectMessage(createStop(Side::SELL, 10, 900, 449975.0, StopAction::UPDATE, true));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::UPDATE, true));
	EXPECT_TRUE(ReadNoStop());

	// Reset the book
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 4);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 2);
	EXPECT_TRUE(ReadNoStop());

	// Trigger the updated deleteOnFlip stop
	strategy->InjectDepth({{1, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 4);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::DELETE, true));
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, DeleteAskStopsOnAskMoveDown)
{
	settings.quoteInstrumentSettings.newOrders = false;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectMessage(createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, true));
	strategy->InjectMessage(createStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW, false));
	strategy->InjectMessage(createStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW, true));
	strategy->InjectMessage(createStop(Side::SELL, 10, 900, 449925.0, StopAction::NEW, false));
	strategy->InjectMessage(createStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW, true));
	strategy->InjectMessage(createStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW, false));
	strategy->InjectMessage(createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, true));
	strategy->InjectMessage(createStop(Side::BUY, 10, 900, 450125.0, StopAction::NEW, false));

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 4);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 4);

	// Check messages were properly processed
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, true));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449975.0, StopAction::NEW, false));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449950.0, StopAction::NEW, true));
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 449925.0, StopAction::NEW, false));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450050.0, StopAction::NEW, true));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW, false));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, true));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450125.0, StopAction::NEW, false));
	EXPECT_TRUE(ReadNoStop());

	// Simulate a price move that would trigger deleteOnFlip stops
	strategy->InjectDepth({{1000, 450000}}, {{1, 450025}, {1000, 450050}});
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 2);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 4);

	// Check that the stops with deleteOnFlip=true were deleted
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, true));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450050.0, StopAction::DELETE, true));
	EXPECT_TRUE(ReadNoStop());

	// Update a stop to have deleteOnFlip=true
	strategy->InjectMessage(createStop(Side::BUY, 10, 900, 450075.0, StopAction::UPDATE, true));
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::UPDATE, true));
	EXPECT_TRUE(ReadNoStop());

	// Reset the book
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 2);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 4);
	EXPECT_TRUE(ReadNoStop());

	// Trigger the updated deleteOnFlip stop
	strategy->InjectDepth({{1000, 450000}}, {{1, 450025}, {1000, 450050}});
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 4);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::DELETE, true));
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, MoveBidStackBasedOnSellStopLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->InjectMessage(stacker::StackerRequoteMessage {});

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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{950, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{949, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, false, 950));

	const stacker::Order *stopOrder = instr->GetOrder(o.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}, {200, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectDepth({{200, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(bid[0], 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, MoveBidStackBasedOnSellStopLean2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->InjectMessage(stacker::StackerRequoteMessage {});

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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{950, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{949, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);

	stacker::StopUpdateMessage updateMsg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::UPDATE, false, 0);
	strategy->InjectMessage(updateMsg);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::UPDATE, false, 0));
	EXPECT_TRUE(e.ReadModify(bid[0], 450000));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, false, 0));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}, {200, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectDepth({{200, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(bid[0], 450000));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, MoveBidStackBasedOnBuyStopLean3)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100, 450025}, {1000, 450000}}, {{100, 450075}, {1000, 450100}});

	strategy->InjectMessage(stacker::StackerRequoteMessage {});

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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 1000);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 1000));

	strategy->InjectDepth({{100, 450025}, {999, 450000}}, {{100, 450075}, {1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectTrade(Side::SELL, 100, 450025);
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
}

TEST_F(StopTest, MoveAskStackBasedOnBuyStopLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->InjectMessage(stacker::StackerRequoteMessage {});

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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{1000, 450000}}, {{950, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{949, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450175));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, false, 950));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}, {200, 450125}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(ask[0], 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, MoveAskStackBasedOnBuyStopLean2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->InjectMessage(stacker::StackerRequoteMessage {});

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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{1000, 450000}}, {{950, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{949, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450175));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	stacker::StopUpdateMessage updateMsg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::UPDATE, false, 0);
	strategy->InjectMessage(updateMsg);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::UPDATE, false, 0));
	EXPECT_TRUE(e.ReadModify(ask[0], 450100));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450175));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, false, 0));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}, {200, 450125}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadModify(ask[0], 450100));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, MoveAskStackBasedOnBuyStopLean3)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{100, 450075}, {1000, 450100}});

	strategy->InjectMessage(stacker::StackerRequoteMessage {});

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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 1000);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 1000));

	strategy->InjectDepth({{1000, 450000}}, {{100, 450075}, {999, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectTrade(Side::BUY, 100, 450075);
	EXPECT_TRUE(e.ReadModify(ask[0], 450175));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
}

TEST_F(StopTest, CancelBidProrataOnSellStopLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {500}, {100}, {0}, {0}, 1);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{950, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{949, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadCancel(bid));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, false, 950));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, CancelBidProrataOnSellStopLean2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {500}, {100}, {0}, {0}, 1);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{950, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{949, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadCancel(bid));
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage updateMsg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::UPDATE, false, 0);
	strategy->InjectMessage(updateMsg);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::UPDATE, false, 0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, false, 0));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, CancelAskProrataOnSellStopLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {500}, {100}, {0}, {0}, 1);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{1000, 450000}}, {{950, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{949, 450100}});
	EXPECT_TRUE(e.ReadCancel(ask));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, false, 950));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, CancelAskProrataOnSellStopLean2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;
	settings.quoteInstrumentSettings.prorataQuoteTimeout = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";
	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	UpdateProrataProfile("P1", {500}, {100}, {0}, {0}, 1);
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 100, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{1000, 450000}}, {{950, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{949, 450100}});
	EXPECT_TRUE(e.ReadCancel(ask));
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage updateMsg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::UPDATE, false, 0);
	strategy->InjectMessage(updateMsg);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::UPDATE, false, 0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 100, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, false, 0));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(0));

	strategy->InjectFill(o, 10);
	EXPECT_EQ(pm.Position(), Quantity(0));
}

TEST_F(StopTest, SellStopShouldIgnoreTimeoutCover)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(900);
	settings.quoteInstrumentSettings.bidSettings.winnerPayup = Quantity(900);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = 5;
	strategy->InjectMessage(posMsg);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450100}});

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450025.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450025}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450025}}, {{1000, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450025.0, StopAction::DELETE));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, BuyStopShouldIgnoreTimeoutCover)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(900);
	settings.quoteInstrumentSettings.askSettings.winnerPayup = Quantity(900);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = -5;
	strategy->InjectMessage(posMsg);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}});

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450075.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450075}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450075}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::DELETE));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5, 450075));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SellStopShouldIgnoreTimeoutCover2)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.bidSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(900);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = 5;
	strategy->InjectMessage(posMsg);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450025}}, {{1000, 450100}});

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450025.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450025}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450025}}, {{1000, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450025.0, StopAction::DELETE));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerClearPositionMessage clearMsg;
	strategy->InjectMessage(clearMsg);

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, BuyStopShouldIgnoreTimeoutCover2)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.askSettings.timeoutCover = 1000;
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(900);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StackerPositionUpdateMessage posMsg;
	posMsg.position = -5;
	strategy->InjectMessage(posMsg);

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}});

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450075.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450075}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450075}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450075, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450075.0, StopAction::DELETE));

	strategy->AdvanceTime(stacker::Millis(999));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());

	stacker::StackerClearPositionMessage clearMsg;
	strategy->InjectMessage(clearMsg);

	strategy->AdvanceTime(stacker::Millis(1));
	strategy->Recalculate();
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, AdoptBidStop)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450100));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{200, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(bid[2], 450025));
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Stack &stack = instr->GetStack(0);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);
	
	stacker::StackerAdoptMessage adoptMsg;
	adoptMsg.stackIndex = 0;
	strategy->InjectMessage(adoptMsg);
	
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STACK);
	EXPECT_TRUE(stack.HasOrder(oInternal));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{199, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadModify(bid[2], 449950));
	EXPECT_TRUE(e.ReadModify(o, 449950));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, AdoptAskStop)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 449975}}, {{200, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450000));
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Stack &stack = instr->GetStack(0);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);
	
	stacker::StackerAdoptMessage adoptMsg;
	adoptMsg.stackIndex = 0;
	strategy->InjectMessage(adoptMsg);
	
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STACK);
	EXPECT_TRUE(stack.HasOrder(oInternal));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{199, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[2], 450075));
	EXPECT_TRUE(e.ReadModify(o, 450075));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, AdoptBidStopWithNoAggressingOn)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450100));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{200, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Stack &stack = instr->GetStack(0);
	const stacker::MStack &mstack = instr->GetMStack(0);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_FALSE(mstack.HasOrder(oInternal));
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	stacker::StackerAdoptMessage adoptMsg;
	adoptMsg.stackIndex = 0;
	strategy->InjectMessage(adoptMsg);

	EXPECT_EQ(oInternal->type, stacker::Order::Type::STACK);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_TRUE(mstack.HasOrder(oInternal));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{199, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(o));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, AdoptAskStopWithNoAggressingOn)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 449975}}, {{200, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Stack &stack = instr->GetStack(0);
	const stacker::MStack &mstack = instr->GetMStack(0);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_FALSE(mstack.HasOrder(oInternal));
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	stacker::StackerAdoptMessage adoptMsg;
	adoptMsg.stackIndex = 0;
	strategy->InjectMessage(adoptMsg);

	EXPECT_EQ(oInternal->type, stacker::Order::Type::STACK);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_TRUE(mstack.HasOrder(oInternal));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{199, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(o));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, AdoptBidStopByPriceWithNoAggressingOn)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadModify(ask[0], 450100));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450025.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{200, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Stack &stack = instr->GetStack(0);
	const stacker::MStack &mstack = instr->GetMStack(0);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_FALSE(mstack.HasOrder(oInternal));
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	stacker::StackerPriceAdoptMessage priceMsg;
	priceMsg.levels.push_back(stacker::StackerPriceAdoptMessage::level {Price::FromUnshifted(450000.0), Side::BUY, 0});
	strategy->InjectMessage(priceMsg);

	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_FALSE(mstack.HasOrder(oInternal));
	
	priceMsg.levels[0].price = Price::FromUnshifted(450025.0);
	priceMsg.levels[0].side = Side::SELL;
	strategy->InjectMessage(priceMsg);

	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_FALSE(mstack.HasOrder(oInternal));

	priceMsg.levels[0].price = Price::FromUnshifted(450025.0);
	priceMsg.levels[0].side = Side::BUY;
	strategy->InjectMessage(priceMsg);

	EXPECT_EQ(oInternal->type, stacker::Order::Type::STACK);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_TRUE(mstack.HasOrder(oInternal));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{199, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(o));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, AdoptAskStopByPriceWithNoAggressingOn)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].noAggressing = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.stopSettings.triggerQuoteDelay = 2000;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	KTN::OrderPod bid[3];
	KTN::OrderPod ask[3];
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
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

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod o;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadModify(bid[0], 449925));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *oInternal = instr->GetOrder(o.orderReqId);
	ASSERT_NE(oInternal, nullptr);
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	strategy->InjectDepth({{1000, 449975}}, {{200, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Stack &stack = instr->GetStack(0);
	const stacker::MStack &mstack = instr->GetMStack(0);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_FALSE(mstack.HasOrder(oInternal));
	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);

	stacker::StackerPriceAdoptMessage priceMsg;
	priceMsg.levels.push_back(stacker::StackerPriceAdoptMessage::level {Price::FromUnshifted(450025.0), Side::SELL, 0});
	strategy->InjectMessage(priceMsg);

	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_FALSE(mstack.HasOrder(oInternal));

	priceMsg.levels[0].price = Price::FromUnshifted(450000.0);
	priceMsg.levels[0].side = Side::BUY;
	strategy->InjectMessage(priceMsg);

	EXPECT_EQ(oInternal->type, stacker::Order::Type::STOP);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_FALSE(mstack.HasOrder(oInternal));

	priceMsg.levels[0].price = Price::FromUnshifted(450000.0);
	priceMsg.levels[0].side = Side::SELL;
	strategy->InjectMessage(priceMsg);

	EXPECT_EQ(oInternal->type, stacker::Order::Type::STACK);
	EXPECT_FALSE(stack.HasOrder(oInternal));
	EXPECT_TRUE(mstack.HasOrder(oInternal));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 449975}}, {{199, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(o));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, RejectSellStopForExceedingMaxOrderQty)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 21, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 21, 900, 450000.0, StopAction::DELETE));

	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StopUpdateMessage msg2 = createStop(Side::SELL, 20, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 900, 450000.0, StopAction::NEW));

	stacker::StopUpdateMessage msg3 = createStop(Side::SELL, 21, 900, 450000.0, StopAction::UPDATE);
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 21, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	stacker::StopUpdateMessage msg4 = createStop(Side::SELL, 20, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg4);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 900, 450000.0, StopAction::NEW));

	KTN::OrderPod orders[20];
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	for (int i = 0; i < 20; i++) {
		ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000, orders[i]));
		strategy->InjectAck(orders[i]);
	}
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
}

TEST_F(StopTest, RejectBuyStopForExceedingMaxOrderQty)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 21, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 21, 900, 450025.0, StopAction::DELETE));

	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg2 = createStop(Side::BUY, 20, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 20, 900, 450025.0, StopAction::NEW));

	stacker::StopUpdateMessage msg3 = createStop(Side::BUY, 21, 900, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 21, 900, 450025.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg4 = createStop(Side::BUY, 20, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg4);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 20, 900, 450025.0, StopAction::NEW));

	KTN::OrderPod orders[20];
	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	for (int i = 0; i < 20; i++) {
		ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1, 450025, orders[i]));
		strategy->InjectAck(orders[i]);
	}
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
}

TEST_F(StopTest, RejectSellStopWhenMaxOrderQtyInternalIsReduced)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(2);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectMessage(createStop(Side::SELL, 21, 900, 450000.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_TRUE(ReadStop(Side::SELL, 21, 900, 450000.0, StopAction::NEW));

	strategy->InjectMessage(createStop(Side::SELL, 20, 900, 449975.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 2);
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 900, 449975.0, StopAction::NEW));

	strategy->InjectMessage(createStop(Side::SELL, 21, 900, 449950.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 3);
	EXPECT_TRUE(ReadStop(Side::SELL, 21, 900, 449950.0, StopAction::NEW));

	strategy->InjectMessage(createStop(Side::SELL, 20, 900, 449925.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 4);
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 900, 449925.0, StopAction::NEW));

	// Update maxOrderQtyInternal to 1
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);
	strategy->onJson(getJson(settings));
	
	EXPECT_TRUE(ReadStop(Side::SELL, 21, 900, 449950.0, StopAction::DELETE));
	EXPECT_TRUE(ReadStop(Side::SELL, 21, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 2);

	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, RejectBuyStopWhenMaxOrderQtyInternalIsReduced)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(2);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectMessage(createStop(Side::BUY, 21, 900, 450025.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 21, 900, 450025.0, StopAction::NEW));

	strategy->InjectMessage(createStop(Side::BUY, 20, 900, 450050.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 2);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 20, 900, 450050.0, StopAction::NEW));

	strategy->InjectMessage(createStop(Side::BUY, 21, 900, 450075.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 3);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 21, 900, 450075.0, StopAction::NEW));

	strategy->InjectMessage(createStop(Side::BUY, 20, 900, 450100.0, StopAction::NEW));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 4);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 20, 900, 450100.0, StopAction::NEW));

	// Update maxOrderQtyInternal to 1
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);
	strategy->onJson(getJson(settings));
	
	EXPECT_TRUE(ReadStop(Side::BUY, 21, 900, 450075.0, StopAction::DELETE));
	EXPECT_TRUE(ReadStop(Side::BUY, 21, 900, 450025.0, StopAction::DELETE));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 2);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

// TODO add tests about manual order adoption and stops

TEST_F(StopTest, BuyStopHedgeInFlightFills)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(250);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(250);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->AdvanceTime(stacker::Seconds(1));

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 1);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(-1));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod hedge[5];
	KTN::OrderPod stop;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, stop));
	strategy->InjectAck(stop);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1, 450100, hedge[0]));
	strategy->InjectAck(hedge[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge[0].orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectFill(ask, 2);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2, 450100, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 3);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 3, 450100, hedge[2]));
	strategy->InjectAck(hedge[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}, {1000, 450125}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, SellStopHedgeInFlightFills)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(250);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(250);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->AdvanceTime(stacker::Seconds(1));

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 1);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(1));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	KTN::OrderPod hedge[5];
	KTN::OrderPod stop;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, stop));
	strategy->InjectAck(stop);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000, hedge[0]));
	strategy->InjectAck(hedge[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge[0].orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectFill(bid, 2);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2, 450000, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 3);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 3, 450000, hedge[2]));
	strategy->InjectAck(hedge[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {1000, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, BuyStopHedgeInFlightFillsWhenOrdersCanceledDueToLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(250);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(250);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->AdvanceTime(stacker::Seconds(1));

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 1);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(-1));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{1000, 450000}}, {{950, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{949, 450100}});
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[5];
	KTN::OrderPod stop;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, stop));
	strategy->InjectAck(stop);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1, 450100, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, false, 950));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge[0].orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectFill(ask, 2);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2, 450100, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 3);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 3, 450100, hedge[2]));
	strategy->InjectAck(hedge[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1, 450100}, {1000, 450125}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SellStopHedgeInFlightFillsWhenOrdersCanceledDueToLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(250);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(250);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->AdvanceTime(stacker::Seconds(1));

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 1);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(1));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{950, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{949, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge[5];
	KTN::OrderPod stop;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, stop));
	strategy->InjectAck(stop);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1, 450000, hedge[0]));
	strategy->InjectAck(hedge[0]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, false, 950));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge[0].orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);

	strategy->InjectFill(bid, 2);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2, 450000, hedge[1]));
	strategy->InjectAck(hedge[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 3);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 3, 450000, hedge[2]));
	strategy->InjectAck(hedge[2]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1, 450000}, {1000, 449975}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, BuyStopDontHedgeFillsUntilStopTriggers)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(250);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(250);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->AdvanceTime(stacker::Seconds(1));

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 1);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(-1));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_NE(stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450100.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{1000, 450000}}, {{950, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{949, 450100}});
	EXPECT_TRUE(e.ReadCancel(ask));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450125));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(ask, 3);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{900, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	KTN::OrderPod stop;
	strategy->InjectDepth({{1000, 450000}}, {{899, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450100, stop));
	strategy->InjectAck(stop);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 4, 450100, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::BUY, 10, 900, 450100.0, StopAction::DELETE, false, 950));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);
}

TEST_F(StopTest, AskStopDontHedgeFillsUntilStopTriggers)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(250);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(250);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->AdvanceTime(stacker::Seconds(1));

	KTN::OrderPod bid;
	KTN::OrderPod ask;
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 450000, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450100, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 1);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::PositionManager &pm = instr->GetPositionManager();
	EXPECT_EQ(pm.Position(), Quantity(1));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_NE(stopHandler.GetStop<Side::SELL>(Price::FromUnshifted(450000.0)), nullptr);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::NEW, false, 950));

	strategy->InjectDepth({{950, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{949, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadCancel(bid));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10, 449975));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectFill(bid, 3);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{900, 450000}}, {{1000, 450100}});
	EXPECT_TRUE(e.ReadNothing());

	KTN::OrderPod hedge;
	KTN::OrderPod stop;
	strategy->InjectDepth({{899, 450000}}, {{1000, 450100}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10, 450000, stop));
	strategy->InjectAck(stop);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 4, 450000, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadStop(Side::SELL, 10, 900, 450000.0, StopAction::DELETE, false, 950));

	const stacker::Order *hedgeOrder = instr->GetOrder(hedge.orderReqId);
	ASSERT_NE(hedgeOrder, nullptr);
	EXPECT_EQ(hedgeOrder->type, stacker::Order::Type::HEDGE);

	const stacker::Order *stopOrder = instr->GetOrder(stop.orderReqId);
	ASSERT_NE(stopOrder, nullptr);
	EXPECT_EQ(stopOrder->type, stacker::Order::Type::STOP);
}

TEST_F(StopTest, RejectBuyStopForExceedingAvailableRisk)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(50);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(100);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	strategy->AdvanceTime(stacker::Seconds(1));

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 101, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 101, 900, 450025.0, StopAction::DELETE));

	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg2 = createStop(Side::BUY, 100, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(100));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 100, 900, 450025.0, StopAction::NEW));

	stacker::StopUpdateMessage msg3 = createStop(Side::BUY, 101, 900, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 101, 900, 450025.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg4 = createStop(Side::BUY, 100, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg4);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(100));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 100, 900, 450025.0, StopAction::NEW));

	KTN::OrderPod orders[2];
	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 50, 450025, orders[0]));
	strategy->InjectAck(orders[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 50, 450025, orders[1]));
	strategy->InjectAck(orders[1]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(100));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
}

TEST_F(StopTest, RejectSellStopForExceedingAvailableRisk)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(50);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(100);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 101, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::SELL, 101, 900, 450000.0, StopAction::DELETE));

	strategy->InjectDepth({{899, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg2 = createStop(Side::SELL, 100, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(100));
	EXPECT_TRUE(ReadStop(Side::SELL, 100, 900, 450000.0, StopAction::NEW));

	stacker::StopUpdateMessage msg3 = createStop(Side::SELL, 101, 900, 450000.0, StopAction::UPDATE);
	strategy->InjectMessage(msg3);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::SELL, 101, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectDepth({{899, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	stacker::StopUpdateMessage msg4 = createStop(Side::SELL, 100, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg4);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(100));
	EXPECT_TRUE(ReadStop(Side::SELL, 100, 900, 450000.0, StopAction::NEW));

	KTN::OrderPod orders[2];
	strategy->InjectDepth({{899, 450000}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 50, 450000, orders[0]));
	strategy->InjectAck(orders[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 50, 450000, orders[1]));
	strategy->InjectAck(orders[1]);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(100));
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
}

TEST_F(StopTest, BuyStopRiskQueueAndDelete)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW));

	stacker::StopUpdateMessage msgDel = createStop(Side::BUY, 25, 900, 450025.0, StopAction::DELETE);
	strategy->InjectMessage(msgDel);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, SellStopRiskQueueAndDelete)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW));

	stacker::StopUpdateMessage msgDel = createStop(Side::SELL, 25, 900, 450000.0, StopAction::DELETE);
	strategy->InjectMessage(msgDel);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::DELETE));
}

TEST_F(StopTest, BuyStopRiskQueueIncreaseAndDelete)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW));

	stacker::StopUpdateMessage msgUpdate1 = createStop(Side::BUY, 30, 900, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate1);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 30, 900, 450025.0, StopAction::UPDATE));

	stacker::StopUpdateMessage msgUpdate2 = createStop(Side::BUY, 50, 900, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(50));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 50, 900, 450025.0, StopAction::UPDATE));

	stacker::StopUpdateMessage msgDel = createStop(Side::BUY, 50, 900, 450025.0, StopAction::DELETE);
	strategy->InjectMessage(msgDel);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 50, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, SellStopRiskQueueIncreaseAndDelete)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW));

	stacker::StopUpdateMessage msgUpdate1 = createStop(Side::SELL, 30, 900, 450000.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate1);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(30));
	EXPECT_TRUE(ReadStop(Side::SELL, 30, 900, 450000.0, StopAction::UPDATE));

	stacker::StopUpdateMessage msgUpdate2 = createStop(Side::SELL, 50, 900, 450000.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate2);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(50));
	EXPECT_TRUE(ReadStop(Side::SELL, 50, 900, 450000.0, StopAction::UPDATE));

	stacker::StopUpdateMessage msgDel = createStop(Side::SELL, 50, 900, 450000.0, StopAction::DELETE);
	strategy->InjectMessage(msgDel);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::SELL, 50, 900, 450000.0, StopAction::DELETE));
}

TEST_F(StopTest, BuyStopRiskQueueSubmitAndReject)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW));

	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	KTN::OrderPod o;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 25, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectSubmitReject(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
}

TEST_F(StopTest, SellStopRiskQueueSubmitAndReject)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW));

	strategy->InjectDepth({{899, 450000}}, {{1000, 450025}});
	KTN::OrderPod o;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 25, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());

	strategy->InjectSubmitReject(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
}

// TEST_F(StopTest, BuyStopRiskQueueSubmitAndRejectWithStrategyPaused)
// {
// 	req->AddParam<bool>("NewOrders", false);
// 	req->AddParam<double>("MaxOrderQtyInternal", 10000);
// 	req->AddParam<double>("MaxPositionInternal", 10000);

// 	ttsdk::SDKAlgoRequestPtr reqPtr(req);
// 	tt.strategyManager.OnStartRequest(algoOrder, req);
// 	Subscribe();
// 	tt.PublishDepth("ESU3", {{{1000.0, 450000.0}}}, {{{1000.0, 450025.0}}});

// 	messaging::StrategyStatusMessage msg;
// 	msg.id[0] = '1';
// 	msg.id[1] = '\0';
// 	msg.status = static_cast<char>(akl::stacker::Strategy::Status::PAUSED);
// 	tt.strategyManager.Handle(msg);

// 	const akl::InstrumentRisk &risk = GetRisk();
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));

// 	const stacker::StopHandler &stopHandler = GetStopHandler();
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_TRUE(ReadNoStop());

// 	SendStop(0, 25, 900, 450025.0, StopAction::NEW);
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 1);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(25));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));
// 	EXPECT_TRUE(ReadStop(0, 25, 900, 450025.0, StopAction::NEW));

// 	tt.PublishDepth("ESU3", {{{1000.0, 450000.0}}}, {{{899.0, 450025.0}}});
// 	ttsdk::OrderPtr o;
// 	ASSERT_TRUE(e.ReadSubmit(ttsdk::OrderSide::Buy, 25.0, 450025.0, o));
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(25));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));
// 	EXPECT_TRUE(ReadStop(0, 25, 900, 450025.0, StopAction::DELETE));
// 	EXPECT_TRUE(ReadNoStop());

// 	impl::Order *io = asOrder(o);
// 	ASSERT_NE(io, nullptr);
// 	io->Reject();
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));
// }

// TEST_F(StopTest, SellStopRiskQueueSubmitAndRejectWithStrategyPaused)
// {
// 	req->AddParam<bool>("NewOrders", false);
// 	req->AddParam<double>("MaxOrderQtyInternal", 10000);
// 	req->AddParam<double>("MaxPositionInternal", 10000);

// 	ttsdk::SDKAlgoRequestPtr reqPtr(req);
// 	tt.strategyManager.OnStartRequest(algoOrder, req);
// 	Subscribe();
// 	tt.PublishDepth("ESU3", {{{1000.0, 450000.0}}}, {{{1000.0, 450025.0}}});

// 	messaging::StrategyStatusMessage msg;
// 	msg.id[0] = '1';
// 	msg.id[1] = '\0';
// 	msg.status = static_cast<char>(akl::stacker::Strategy::Status::PAUSED);
// 	tt.strategyManager.Handle(msg);

// 	const akl::InstrumentRisk &risk = GetRisk();
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));

// 	const stacker::StopHandler &stopHandler = GetStopHandler();
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_TRUE(ReadNoStop());

// 	SendStop(1, 25, 900, 450000.0, StopAction::NEW);
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 1);
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(25));
// 	EXPECT_TRUE(ReadStop(1, 25, 900, 450000.0, StopAction::NEW));

// 	tt.PublishDepth("ESU3", {{{899.0, 450000.0}}}, {{{1000.0, 450025.0}}});
// 	ttsdk::OrderPtr o;
// 	ASSERT_TRUE(e.ReadSubmit(ttsdk::OrderSide::Sell, 25.0, 450000.0, o));
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(25));
// 	EXPECT_TRUE(ReadStop(1, 25, 900, 450000.0, StopAction::DELETE));
// 	EXPECT_TRUE(ReadNoStop());

// 	impl::Order *io = asOrder(o);
// 	ASSERT_NE(io, nullptr);
// 	io->Reject();
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));
// }

TEST_F(StopTest, BuyStopQueueIncreaseAndRejectDueToMaxNumOrders)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 20, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 20, 900, 450025.0, StopAction::NEW));

	stacker::StopUpdateMessage msgUpdate = createStop(Side::BUY, 21, 900, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 21, 900, 450025.0, StopAction::DELETE));

	strategy->InjectDepth({{1000, 450000}}, {{899, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, SellStopQueueIncreaseAndRejectDueToMaxNumOrders)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 20, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(20));
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 900, 450000.0, StopAction::NEW));

	stacker::StopUpdateMessage msgUpdate = createStop(Side::SELL, 21, 900, 450000.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::SELL, 21, 900, 450000.0, StopAction::DELETE));

	strategy->InjectDepth({{899, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, BuyStopWithThresholdOverQuantity)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 20, 1001, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	KTN::OrderPod o;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 20, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 20, 1001, 450025.0, StopAction::NEW));
	EXPECT_TRUE(ReadStop(Side::BUY, 20, 1001, 450025.0, StopAction::DELETE));

	strategy->InjectSubmitReject(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
}

TEST_F(StopTest, SellStopWithThresholdOverQuantity)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 20, 1001, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	KTN::OrderPod o;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 20, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(20));
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 1001, 450000.0, StopAction::NEW));
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 1001, 450000.0, StopAction::DELETE));

	strategy->InjectSubmitReject(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
}

TEST_F(StopTest, BuyStopAndIncreaseThresholdOverQuantity)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 20, 1000, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 20, 1000, 450025.0, StopAction::NEW));

	stacker::StopUpdateMessage msgUpdate = createStop(Side::BUY, 20, 1001, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate);
	KTN::OrderPod o;
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 20, 450025, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 20, 1001, 450025.0, StopAction::UPDATE));
	EXPECT_TRUE(ReadStop(Side::BUY, 20, 1001, 450025.0, StopAction::DELETE));

	strategy->InjectSubmitReject(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
}

TEST_F(StopTest, SellStopAndIncreaseThresholdOverQuantity)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 20, 1000, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(20));
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 1000, 450000.0, StopAction::NEW));

	stacker::StopUpdateMessage msgUpdate = createStop(Side::SELL, 20, 1001, 450000.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate);
	KTN::OrderPod o;
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 20, 450000, o));
	strategy->InjectAck(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(20));
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 1001, 450000.0, StopAction::UPDATE));
	EXPECT_TRUE(ReadStop(Side::SELL, 20, 1001, 450000.0, StopAction::DELETE));

	strategy->InjectSubmitReject(o);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
}

TEST_F(StopTest, BuyStopRiskQueueAndDeleteOnFlip)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW, true);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW, true));

	strategy->InjectDepth({{1000, 449975}}, {{1, 450000}, {1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::DELETE, true));
}

TEST_F(StopTest, SellStopRiskQueueAndDeleteOnFlip)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW, true);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW, true));

	strategy->InjectDepth({{1, 450025}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::DELETE, true));
}

TEST_F(StopTest, BuyStopCanceledOnMarketStateChange)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW));

	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::Close);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::DELETE));
}

TEST_F(StopTest, SellStopCanceledOnMarketStateChange)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW));

	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::Close);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::DELETE));
}

// TEST_F(StopTest, BuyStopCanceledOnClientDisconnect)
// {
// 	req->AddParam<bool>("NewOrders", false);
// 	req->AddParam<double>("MaxOrderQtyInternal", 10000);
// 	req->AddParam<double>("MaxPositionInternal", 10000);

// 	ttsdk::SDKAlgoRequestPtr reqPtr(req);
// 	tt.strategyManager.OnStartRequest(algoOrder, req);
// 	Subscribe();
// 	Subscribe(2);
// 	tt.PublishDepth("ESU3", {{{1000.0, 450000.0}}}, {{{1000.0, 450025.0}}});

// 	const akl::InstrumentRisk &risk = GetRisk();
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));

// 	const stacker::StopHandler &stopHandler = GetStopHandler();
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_TRUE(ReadNoStop());

// 	SendStop(0, 25, 900, 450025.0, StopAction::NEW);
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 1);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(25));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));
// 	EXPECT_TRUE(ReadStop(0, 25, 900, 450025.0, StopAction::NEW));
// 	EXPECT_TRUE(ReadStop(0, 25, 900, 450025.0, StopAction::NEW));

// 	tt.strategyManager.OnDisconnect(1);
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));
// 	EXPECT_TRUE(ReadStop(0, 25, 900, 450025.0, StopAction::DELETE));
// 	EXPECT_TRUE(ReadNoStop());
// }

// TEST_F(StopTest, SellStopCanceledOnClientDisconnect)
// {
// 	req->AddParam<bool>("NewOrders", false);
// 	req->AddParam<double>("MaxOrderQtyInternal", 10000);
// 	req->AddParam<double>("MaxPositionInternal", 10000);

// 	ttsdk::SDKAlgoRequestPtr reqPtr(req);
// 	tt.strategyManager.OnStartRequest(algoOrder, req);
// 	Subscribe();
// 	Subscribe(2);
// 	tt.PublishDepth("ESU3", {{{1000.0, 450000.0}}}, {{{1000.0, 450025.0}}});

// 	const akl::InstrumentRisk &risk = GetRisk();
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));

// 	const stacker::StopHandler &stopHandler = GetStopHandler();
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_TRUE(ReadNoStop());

// 	SendStop(1, 25, 900, 450000.0, StopAction::NEW);
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 1);
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(25));
// 	EXPECT_TRUE(ReadStop(1, 25, 900, 450000.0, StopAction::NEW));
// 	EXPECT_TRUE(ReadStop(1, 25, 900, 450000.0, StopAction::NEW));

// 	tt.strategyManager.OnDisconnect(1);
// 	EXPECT_TRUE(e.ReadNothing());
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::BID>(), 0);
// 	EXPECT_EQ(stopHandler.GetStopCount<Side::ASK>(), 0);
// 	EXPECT_EQ(risk.GetWorkingQty<Side::BID>(), QTY(0));
// 	EXPECT_EQ(risk.GetWorkingQty<Side::ASK>(), QTY(0));
// 	EXPECT_TRUE(ReadStop(1, 25, 900, 450000.0, StopAction::DELETE));
// 	EXPECT_TRUE(ReadNoStop());
// }

TEST_F(StopTest, BuyStopCanceledOnFlipDuringPreopen)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW));

	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	strategy->InjectDepth({{1, 450000}, {1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, SellStopCanceledOnFlipDuringPreopen)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW));

	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));

	strategy->InjectDepth({{1000, 449975}}, {{1, 450000}, {1000, 450050}});
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, BuyStopCanceledDueToReducingMaxOrderQtyThroughTT)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW));

	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, SellStopCanceledDueToReducingMaxOrderQtyThroughTT)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW));

	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);
	strategy->onJson(getJson(settings));
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::DELETE));
	EXPECT_TRUE(ReadNoStop());
}

TEST_F(StopTest, PreallocatingOneOrder)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 6, 1000, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::StopInfo *stopInfo = stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0));
	ASSERT_NE(stopInfo, nullptr);

	ASSERT_EQ(stopInfo->orders.Size(), 1);
	EXPECT_EQ(stopInfo->orders[0]->targetQuantity, Quantity(6));
	EXPECT_EQ(stopInfo->orders[0]->lastRiskQuantity, Quantity(6));
	EXPECT_TRUE(stopInfo->orders[0]->riskPreallocated);
}

TEST_F(StopTest, PreallocatingOneOrderOfMaxSize)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 1000, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::StopInfo *stopInfo = stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0));
	ASSERT_NE(stopInfo, nullptr);

	ASSERT_EQ(stopInfo->orders.Size(), 1);
	EXPECT_EQ(stopInfo->orders[0]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[0]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[0]->riskPreallocated);
}

TEST_F(StopTest, PreallocatingTwoOrders)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 17, 1000, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::StopInfo *stopInfo = stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0));
	ASSERT_NE(stopInfo, nullptr);

	ASSERT_EQ(stopInfo->orders.Size(), 2);
	EXPECT_EQ(stopInfo->orders[0]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[0]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[0]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[1]->targetQuantity, Quantity(7));
	EXPECT_EQ(stopInfo->orders[1]->lastRiskQuantity, Quantity(7));
	EXPECT_TRUE(stopInfo->orders[1]->riskPreallocated);
}

TEST_F(StopTest, PreallocatingTwoOrdersOfMaxSize)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 20, 1000, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::StopInfo *stopInfo = stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0));
	ASSERT_NE(stopInfo, nullptr);

	ASSERT_EQ(stopInfo->orders.Size(), 2);
	EXPECT_EQ(stopInfo->orders[0]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[0]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[0]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[1]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[1]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[1]->riskPreallocated);
}

TEST_F(StopTest, PreallocatingOrders)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 1000, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::StopInfo *stopInfo = stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0));
	ASSERT_NE(stopInfo, nullptr);

	ASSERT_EQ(stopInfo->orders.Size(), 1);
	EXPECT_EQ(stopInfo->orders[0]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[0]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[0]->riskPreallocated);

	stacker::StopUpdateMessage msgUpdate1 = createStop(Side::BUY, 11, 1000, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate1);
	ASSERT_EQ(stopInfo->orders.Size(), 2);
	EXPECT_EQ(stopInfo->orders[0]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[0]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[0]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[1]->targetQuantity, Quantity(1));
	EXPECT_EQ(stopInfo->orders[1]->lastRiskQuantity, Quantity(1));
	EXPECT_TRUE(stopInfo->orders[1]->riskPreallocated);

	stacker::StopUpdateMessage msgUpdate2 = createStop(Side::BUY, 14, 1000, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate2);
	ASSERT_EQ(stopInfo->orders.Size(), 2);
	EXPECT_EQ(stopInfo->orders[0]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[0]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[0]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[1]->targetQuantity, Quantity(4));
	EXPECT_EQ(stopInfo->orders[1]->lastRiskQuantity, Quantity(4));
	EXPECT_TRUE(stopInfo->orders[1]->riskPreallocated);

	stacker::StopUpdateMessage msgUpdate3 = createStop(Side::BUY, 20, 1000, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate3);
	ASSERT_EQ(stopInfo->orders.Size(), 2);
	EXPECT_EQ(stopInfo->orders[0]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[0]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[0]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[1]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[1]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[1]->riskPreallocated);

	stacker::StopUpdateMessage msgUpdate4 = createStop(Side::BUY, 41, 1000, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate4);
	ASSERT_EQ(stopInfo->orders.Size(), 5);
	EXPECT_EQ(stopInfo->orders[0]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[0]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[0]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[1]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[1]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[1]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[2]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[2]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[2]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[3]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[3]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[3]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[4]->targetQuantity, Quantity(1));
	EXPECT_EQ(stopInfo->orders[4]->lastRiskQuantity, Quantity(1));
	EXPECT_TRUE(stopInfo->orders[4]->riskPreallocated);

	stacker::StopUpdateMessage msgUpdate5 = createStop(Side::BUY, 52, 1000, 450025.0, StopAction::UPDATE);
	strategy->InjectMessage(msgUpdate5);
	ASSERT_EQ(stopInfo->orders.Size(), 6);
	EXPECT_EQ(stopInfo->orders[0]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[0]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[0]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[1]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[1]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[1]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[2]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[2]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[2]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[3]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[3]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[3]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[4]->targetQuantity, Quantity(10));
	EXPECT_EQ(stopInfo->orders[4]->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(stopInfo->orders[4]->riskPreallocated);
	EXPECT_EQ(stopInfo->orders[5]->targetQuantity, Quantity(2));
	EXPECT_EQ(stopInfo->orders[5]->lastRiskQuantity, Quantity(2));
	EXPECT_TRUE(stopInfo->orders[5]->riskPreallocated);
}

TEST_F(StopTest, PreallocatingAndReusingOrders)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 10, 1000, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());

	const stacker::StopInfo *stopInfo = stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0));
	ASSERT_NE(stopInfo, nullptr);

	ASSERT_EQ(stopInfo->orders.Size(), 1);
	stacker::Order *o = stopInfo->orders[0];
	EXPECT_EQ(o->targetQuantity, Quantity(10));
	EXPECT_EQ(o->lastRiskQuantity, Quantity(10));
	EXPECT_TRUE(o->riskPreallocated);

	stacker::StopUpdateMessage msgDelete = createStop(Side::BUY, 10, 1000, 450025.0, StopAction::DELETE);
	strategy->InjectMessage(msgDelete);
	EXPECT_TRUE(e.ReadNothing());

	stopInfo = stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0));
	EXPECT_EQ(stopInfo, nullptr);
	EXPECT_EQ(o->targetQuantity, Quantity(0));
	EXPECT_EQ(o->lastRiskQuantity, Quantity(0));
	EXPECT_FALSE(o->riskPreallocated);

	stacker::StopUpdateMessage msgNew = createStop(Side::BUY, 66, 1000, 450025.0, StopAction::NEW);
	strategy->InjectMessage(msgNew);
	EXPECT_TRUE(e.ReadNothing());

	stopInfo = stopHandler.GetStop<Side::BUY>(Price::FromUnshifted(450025.0));
	ASSERT_NE(stopInfo, nullptr);
	ASSERT_EQ(stopInfo->orders.Size(), 1);
	stacker::Order *o2 = stopInfo->orders[0];
	EXPECT_EQ(o2->targetQuantity, Quantity(66));
	EXPECT_EQ(o2->lastRiskQuantity, Quantity(66));
	EXPECT_TRUE(o2->riskPreallocated);
	EXPECT_EQ(o, o2);

	strategy->InjectDepth({{1000, 450000}}, {{999, 450025}});
	EXPECT_TRUE(e.ReadSubmit(Side::BUY, 66, 450025));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(StopTest, AdoptWithPreallocatedBuyStop)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW, true);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
	EXPECT_TRUE(ReadStop(Side::BUY, 25, 900, 450025.0, StopAction::NEW, true));

	stacker::StackerAdoptMessage adoptMsg;
	adoptMsg.stackIndex = 0;
	strategy->InjectMessage(adoptMsg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	adoptMsg.stackIndex = 1;
	strategy->InjectMessage(adoptMsg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	stacker::StackerPriceAdoptMessage priceAdoptMsg;
	priceAdoptMsg.levels.push_back(stacker::StackerPriceAdoptMessage::level {Price::FromUnshifted(450025.0), Side::BUY, 0});
	strategy->InjectMessage(priceAdoptMsg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	priceAdoptMsg.levels[0].index = 1;
	strategy->InjectMessage(priceAdoptMsg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 1);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));
}

TEST_F(StopTest, AdoptWithPreallocatedSellStop)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(0));

	const stacker::StopHandler &stopHandler = instr->GetStopHandler();
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 0);
	EXPECT_TRUE(ReadNoStop());

	stacker::StopUpdateMessage msg = createStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW, true);
	strategy->InjectMessage(msg);
	EXPECT_TRUE(e.ReadNothing());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));
	EXPECT_TRUE(ReadStop(Side::SELL, 25, 900, 450000.0, StopAction::NEW, true));

	stacker::StackerAdoptMessage adoptMsg;
	adoptMsg.stackIndex = 0;
	strategy->InjectMessage(adoptMsg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));

	adoptMsg.stackIndex = 1;
	strategy->InjectMessage(adoptMsg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));

	stacker::StackerPriceAdoptMessage priceAdoptMsg;
	priceAdoptMsg.levels.push_back(stacker::StackerPriceAdoptMessage::level {Price::FromUnshifted(450000.0), Side::SELL, 0});
	strategy->InjectMessage(priceAdoptMsg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));

	priceAdoptMsg.levels[0].index = 1;
	strategy->InjectMessage(priceAdoptMsg);

	EXPECT_TRUE(e.ReadNothing());
	EXPECT_TRUE(ReadNoStop());
	EXPECT_EQ(stopHandler.GetStopCount<Side::BUY>(), 0);
	EXPECT_EQ(stopHandler.GetStopCount<Side::SELL>(), 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), Quantity(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), Quantity(25));
}

}

int main(int argc, char* argv[])
{
	bool y = OrderNums::instance().isLoaded();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
