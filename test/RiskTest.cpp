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

class RiskTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	RiskTest()
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
		//delete stacker;
		//stacker = nullptr;
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
		const int lean = 0)
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

	bool ReadStop(Side side, int qty, int triggerQty, double px, StopAction action, bool deleteOnFlip = false, const int lean = 0)
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
			return msg.side == side
				&& msg.qty == Quantity(qty)
				&& msg.triggerQty == Quantity(triggerQty)
				&& msg.price == Price::FromUnshifted(px)
				&& msg.action == action
				&& msg.deleteOnFlip == (deleteOnFlip ? 1 : 0)
				&& msg.leanQty == Quantity(lean)
				&& msg.hasLean == (lean > 0 ? 1 : 0);
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

TEST_F(RiskTest, NoBidsQuotedDueToExceedingMaxOrderSize)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(100000);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;


	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(100000));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(100000));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1.0, 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(1));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(100000));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(99999));
}

TEST_F(RiskTest, NoAsksQuotedDueToExceedingMaxOrderSize)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(100000);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(100000));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(100000));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1.0, 450000.0));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(99999));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(100000));
}

TEST_F(RiskTest, NothingQuotedDueToExceedingMaxOrderSize)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(1);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(100000);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(100000));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(100000));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(100000));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(100000));
}

TEST_F(RiskTest, NoBidsQuotedDueToExceedingMaxPosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(1);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(1));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1.0, 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(1));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));
}

TEST_F(RiskTest, NoAsksQuotedDueToExceedingMaxPosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(1);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(1));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1.0, 450000.0));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(1));
}

TEST_F(RiskTest, NothingQuotedDueToExceedingInternalMaxPosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(1);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(1));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(1));
}

TEST_F(RiskTest, SomeBidsQuotedDueToExceedingInternalMaxPosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(5);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 4;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(5));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(5));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2.0, 450000.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2.0, 449975.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1.0, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1.0, 450050.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1.0, 450075.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1.0, 450100.0));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(4));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(4));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(1));
}

TEST_F(RiskTest, SomeAsksQuotedDueToExceedingInternalMaxPosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(5);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 4;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(5));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(5));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1.0, 450000.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1.0, 449975.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1.0, 449950.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1.0, 449925.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2.0, 450025.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2.0, 450050.0));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(4));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(4));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(1));
}

TEST_F(RiskTest, SomeOrdersQuotedDueToExceedingInternalMaxPosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(5);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(2);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 4;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(3);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(5));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(5));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2.0, 450000.0));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2.0, 449975.0));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 3.0, 450025.0));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(4));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(3));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(2));
}

TEST_F(RiskTest, BidStackMovingWithCancelNew)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(40);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[4];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449925.0, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectCancelAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[3]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectCancelAck(bid[3]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
}

TEST_F(RiskTest, BidStackBlockedOnMoveWithCancelNew)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(30);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[4];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectDepth({{1000, 449975}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectCancelAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
}

TEST_F(RiskTest, AskStackBlockedOnMoveWithCancelNew)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(30);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	KTN::OrderPod ask[4];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450050.0, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450075.0, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(30));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450050}});
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectCancelAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(RiskTest, BidStackBlockedOnMoveWithCancelNew2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(40);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[4];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449950.0, bid[2]));
	strategy->InjectAck(bid[2]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectDepth({{1000, 449950}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[0]));
	EXPECT_TRUE(e.ReadCancel(bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449925.0, bid[3]));
	strategy->InjectAck(bid[3]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectCancelAck(bid[0]);
	strategy->InjectCancelAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(bid[3]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectCancelAck(bid[3]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
}

TEST_F(RiskTest, AskStackBlockedOnMoveWithCancelNew2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(40);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 3;
	settings.quoteInstrumentSettings.quoteTimeout = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	KTN::OrderPod ask[4];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450050.0, ask[1]));
	strategy->InjectAck(ask[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450075.0, ask[2]));
	strategy->InjectAck(ask[2]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(30));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450075}});
	EXPECT_TRUE(e.ReadCancel(ask[0]));
	EXPECT_TRUE(e.ReadCancel(ask[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450100.0, ask[3]));
	strategy->InjectAck(ask[3]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(40));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectCancelAck(ask[0]);
	strategy->InjectCancelAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	EXPECT_TRUE(e.ReadCancel(ask[3]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(30));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectCancelAck(ask[3]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
}

TEST_F(RiskTest, BidRequoteBlockedDueToLongPosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(40);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(40));

	KTN::OrderPod bid[2];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(40));

	strategy->InjectFill(bid[0], 10.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(50));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(50));

	strategy->InjectFill(bid[0], 10.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(60));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(60));

	strategy->InjectFill(bid[0], 10.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(70));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(70));
}

TEST_F(RiskTest, AskRequoteBlockedDueToShortPosition)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(40);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(40));

	KTN::OrderPod ask[2];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450050.0, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(40));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(20));

	strategy->InjectFill(ask[0], 10.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(20));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(50));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(10));

	strategy->InjectFill(ask[0], 10.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(60));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(10));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(60));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	strategy->InjectFill(ask[0], 10.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(30));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(70));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(30));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(70));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));
}

TEST_F(RiskTest, LongPositionAllowsSellHedgeOrder)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(20);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(20));

	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450050.0, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	strategy->InjectFill(bid[0], 10.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(10));

	KTN::OrderPod hedge;
	strategy->InjectDepth({{499, 450000}, {1000, 449975}}, {{1000, 450025}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450000.0, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(30));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(30));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	strategy->InjectFill(hedge, 9.0);
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(21));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(9));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(9));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	strategy->InjectFill(hedge, 1.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449950.0, bid[0]));
	strategy->InjectAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));
}

TEST_F(RiskTest, ShortPositionAllowsBuyHedgeOrder)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(20);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(500);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(20));

	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450050.0, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	strategy->InjectFill(ask[0], 10.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	KTN::OrderPod hedge;
	strategy->InjectDepth({{1000, 450000}}, {{499, 450025}, {1000, 450050}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450025.0, hedge));
	strategy->InjectAck(hedge);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(30));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));

	strategy->InjectFill(hedge, 9.0);
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(21));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(9));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(9));

	strategy->InjectFill(hedge, 1.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(10));

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450075.0, ask[0]));
	strategy->InjectAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetAvailableQuantity<Side::SELL>(), QTY(0));
}

TEST_F(RiskTest, BidFillAndUnsolicitedCancel)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(20);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid[2];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid[0]));
	strategy->InjectAck(bid[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 449975.0, bid[1]));
	strategy->InjectAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(20));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectFill(bid[0], 5.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(15));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(5));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectCancelAck(bid[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(5));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectFill(bid[1], 1.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(9));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(6));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectCancelAck(bid[1]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(6));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
}

TEST_F(RiskTest, AskFillAndUnsolicitedCancel)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(20);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.payup = Quantity(500);
	settings.quoteInstrumentSettings.askSettings.payup = Quantity(1);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	KTN::OrderPod ask[2];

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask[0]));
	strategy->InjectAck(ask[0]);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450050.0, ask[1]));
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(20));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectFill(ask[0], 5.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(15));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(5));

	strategy->InjectCancelAck(ask[0]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(5));

	strategy->InjectFill(ask[1], 1.0);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(9));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(6));

	strategy->InjectCancelAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(6));
}

TEST_F(RiskTest, BidSubmitRejected)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(100000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(100000);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(10);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	KTN::OrderPod bid;

	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectSubmitReject(bid);
	EXPECT_TRUE(e.ReadNothing());

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
}

// TODO tests for modding quantity with prorata profiles

// TODO Reject risk updates lower than current working qty

// TODO recovered orders

TEST_F(RiskTest, RiskForAbandonedBid)
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
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	
	stacker::StackerPriceAbandonMessage abandonMsg;
	abandonMsg.levels.push_back(stacker::StackerPriceAbandonMessage::level {Price::FromShifted(450000.0), Side::BUY, 0});
	strategy->InjectMessage(abandonMsg);
	
	// Risk should still track the abandoned bid order
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	
	// Fill part of the bid that was abandoned
	strategy->InjectFill(bid, 1.0);
	
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(9));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	
	// Fill more of the bid
	strategy->InjectFill(bid, 3.0);
	
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(6));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(4));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	
	// Cancel ack for the bid
	strategy->InjectCancelAck(bid);
	
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(4));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
}

TEST_F(RiskTest, RiskForAbandonedAsk)
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
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	
	KTN::OrderPod bid;
	KTN::OrderPod ask;
	
	strategy->InjectMessage(stacker::StackerRequoteMessage {});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 10.0, 450000.0, bid));
	strategy->InjectAck(bid);
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 10.0, 450025.0, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
	
	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();
	
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	
	stacker::StackerPriceAbandonMessage abandonMsg;
	abandonMsg.levels.push_back(stacker::StackerPriceAbandonMessage::level {Price::FromShifted(450025.0), Side::SELL, 0});
	strategy->InjectMessage(abandonMsg);
	
	// Risk should still track the abandoned ask order
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(10));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));
	
	// Fill part of the ask that was abandoned
	strategy->InjectFill(ask, 1.0);
	
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(9));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(1));
	
	// Fill more of the ask
	strategy->InjectFill(ask, 3.0);
	
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(6));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(4));
	
	// Cancel ack for the ask
	strategy->InjectCancelAck(ask);
	
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(10));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(4));
}

TEST_F(RiskTest, RecoveredAndWorkingOrdersShouldBeAddedToRisk)
{
	delete strategy;

	KTN::OrderPod recoveredOrders[6];
	recoveredOrders[0].orderReqId = 1;
	recoveredOrders[0].OrdSide = Side::BUY;
	recoveredOrders[0].quantity = 11;
	recoveredOrders[0].price = PX(450000);
	recoveredOrders[1].orderReqId = 2;
	recoveredOrders[1].OrdSide = Side::BUY;
	recoveredOrders[1].quantity = 12;
	recoveredOrders[1].price = PX(449975);
	recoveredOrders[2].orderReqId = 3;
	recoveredOrders[2].OrdSide = Side::BUY;
	recoveredOrders[2].quantity = 13;
	recoveredOrders[2].price = PX(449950);

	recoveredOrders[3].orderReqId = 4;
	recoveredOrders[3].OrdSide = Side::SELL;
	recoveredOrders[3].quantity = 14;
	recoveredOrders[3].price = PX(450025);
	recoveredOrders[4].orderReqId = 5;
	recoveredOrders[4].OrdSide = Side::SELL;
	recoveredOrders[4].quantity = 15;
	recoveredOrders[4].price = PX(450050);
	recoveredOrders[5].orderReqId = 6;
	recoveredOrders[5].OrdSide = Side::SELL;
	recoveredOrders[5].quantity = 16;
	recoveredOrders[5].price = PX(450075);

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

	strategy->InjectDepth({{1000, 450000}}, {{1000, 450025}});
	strategy->onLaunch(getJson(settings));
	strategy->Start();
	EXPECT_TRUE(e.ReadNothing());

	const stacker::Instrument *instr = strategy->GetInstrument();
	ASSERT_NE(instr, nullptr);
	const stacker::InstrumentRisk &risk = instr->GetRisk();

	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(25));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(31));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(0));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectFill(recoveredOrders[1], 1);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(24));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(31));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(0));

	strategy->InjectFill(recoveredOrders[4], 2);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(24));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(29));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(2));

	strategy->InjectCancelAck(recoveredOrders[2]);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(11));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(29));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(2));

	strategy->InjectCancelAck(recoveredOrders[5]);
	EXPECT_EQ(risk.GetWorkingQty<Side::BUY>(), QTY(11));
	EXPECT_EQ(risk.GetWorkingQty<Side::SELL>(), QTY(13));
	EXPECT_EQ(risk.GetFilledQty<Side::BUY>(), QTY(1));
	EXPECT_EQ(risk.GetFilledQty<Side::SELL>(), QTY(2));
}

// TODO tests for moving orders to different strategies

}

int main(int argc, char* argv[])
{
	bool y = OrderNums::instance().isLoaded();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}