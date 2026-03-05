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

#include <unordered_set>

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
		"\"ProdType\": \"spread\", "
		"\"Inverse\": false, "
		"\"PriceMult\": 0, "
		"\"SpreadMult\": 0, "
		"\"ActiveQuoting\": false"
	"}, {"
		"\"LegId\": 1, "
		"\"Exch\": \"cme\", "
		"\"Symbol\": \"ZNH5\", "
		"\"ProdType\": \"future\", "
		"\"Inverse\": false, "
		"\"PriceMult\": 0, "
		"\"SpreadMult\": 0, "
		"\"ActiveQuoting\": false"
	"}], "
	"\"Source\": \"BATMAN\", "
	"\"UniqueId\": \"23ROZ5\""
"}]";

class StressTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	StressTest()
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

TEST_F(StressTest, T)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000000);
	settings.quoteInstrumentSettings.rateLimitSettings.limit = 10000000;
	settings.quoteInstrumentSettings.rateLimitSettings.window = 1;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 1;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(0);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.moveWithCancelNew = true;
	settings.quoteInstrumentSettings.quoteTimeout = 0;


	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod o;
	ASSERT_TRUE(e.ReadSubmit(KTN::ORD::KOrderSide::SELL, 1, 450100, o));
	strategy->InjectAck(o);

	std::unordered_set<uint64_t> reqIds;

	const int iterations = 100000;
	for (int i = 0; i < iterations; ++i)
	{
		std::cout << "Iteration " << i << std::endl;

		strategy->InjectDepth({{1000, 450000}}, {{1000, 450125}});
		ASSERT_TRUE(e.ReadCancel(o));
		strategy->InjectCancelAck(o);
		ASSERT_TRUE(e.ReadSubmit(KTN::ORD::KOrderSide::SELL, 1, 450125, o));
		strategy->InjectAck(o);
		EXPECT_TRUE(e.ReadNothing());
		reqIds.insert(o.orderReqId);


		// if (i == 4999)
		// {
		// 	__asm__("int $3");
		// }

		strategy->InjectDepth({{1000, 450000}}, {{1000, 450100}});
		ASSERT_TRUE(e.ReadCancel(o));
		strategy->InjectCancelAck(o);
		ASSERT_TRUE(e.ReadSubmit(KTN::ORD::KOrderSide::SELL, 1, 450100, o));
		strategy->InjectAck(o);
		EXPECT_TRUE(e.ReadNothing());
		reqIds.insert(o.orderReqId);

		strategy->AdvanceTime(stacker::Millis(100));
		strategy->ExecuteStatusThread();
	}

	ASSERT_EQ(reqIds.size(), iterations * 2);

}

}

int main(int argc, char* argv[])
{
	bool y = OrderNums::instance().isLoaded();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}