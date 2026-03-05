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
	"\"SpreaderAddTicks\": 3, "
	"\"SpreaderAddTicks2\": -1, "
	"\"SpreaderAddTicksProrata\": -1, "
	"\"SpreaderRemoveTicks\": -1, "
	"\"SpreaderRemoveTicks2\": -1, "
	"\"SpreaderRemoveTicksProrata\": -1, "
	"\"SpreaderLockQty\": -1, "
	"\"SpreaderLockQty2\": -1, "
	"\"SpreaderLockQtyProrata\": -1, "
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
		"\"Symbol\": \"ZCZ5-ZCH6\", "
		"\"ProdType\": \"spread\", "
		"\"Inverse\": false, "
		"\"PriceMult\": 0, "
		"\"SpreadMult\": 0, "
		"\"ActiveQuoting\": false"
	"}, {"
		"\"LegId\": 1, "
		"\"Exch\": \"cme\", "
		"\"Symbol\": \"ZCZ5\", "
		"\"ProdType\": \"future\", "
		"\"Inverse\": false, "
		"\"PriceMult\": 0, "
		"\"SpreadMult\": 0, "
		"\"ActiveQuoting\": false"
	"}], "
	"\"Source\": \"BATMAN\", "
	"\"UniqueId\": \"23ROZ5\""
"}]";

inline double tpx(int points, int fraction, int subfraction)
{
	double subfractiondouble = subfraction;
	if (subfractiondouble == 2 || subfractiondouble == 7)
	{
		subfractiondouble += 0.5;
	}
	subfractiondouble /= 10.0;
	if (points >= 0)
	{
		return points + ((fraction + subfractiondouble) / 32.0);
	}
	else
	{
		return points - ((fraction + subfractiondouble) / 32.0);
	}
}

class SpreaderTest2 : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	SpreaderTest2()
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

	impl::ExchangeSender e;
	AlgoInitializer params;
	StrategyTester<akl::stacker::AklStacker> *strategy { nullptr };

	stacker::AklStackerParams settings;
	const stacker::PriceConverter *priceConverter { nullptr };
};

TEST_F(SpreaderTest2, BidAddTicksAndRemoveTicks1)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());
	
	strategy->InjectDepth({{100000, -14.00}}, {{100000, -13.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	stacker::StackerSpreaderUpdateMessage msg;
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(434.50).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	
	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.24}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	//EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	// strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	// EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	// strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	// EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	// strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	// EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, AskAddTicksAndRemoveTicks1)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[0] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());
	
	strategy->InjectDepth({{100000, -14.50}}, {{100000, -14.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	stacker::StackerSpreaderUpdateMessage msg;
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(434.75).AsShifted());
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());
	
	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::NO);
	//EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	// strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	// EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::NO);
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	// strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	// EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::NO);
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	// strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	// EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, BidAddTicksAndRemoveTicks2)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 1;
	settings.spreaderPricingSettings.lockQty[1] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[1] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	
	strategy->InjectDepth({{100000, -14.00}}, {{100000, -13.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	stacker::StackerSpreaderUpdateMessage msg;
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(434.50).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	
	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.24}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::NO);
	//EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, AskAddTicksAndRemoveTicks2)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10'000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10'000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 1;
	settings.spreaderPricingSettings.lockQty[1] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[1] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());
	
	strategy->InjectDepth({{100000, -14.50}}, {{100000, -14.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	stacker::StackerSpreaderUpdateMessage msg;
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(434.75).AsShifted());
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());
	
	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::NO);
	//EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, BidAddTicksAndRemoveTicksProrata)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[2] = 3;
	settings.spreaderPricingSettings.removeTicks[2] = 1;
	settings.spreaderPricingSettings.lockQty[2] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[2] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	
	strategy->InjectDepth({{100000, -14.00}}, {{100000, -13.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	stacker::StackerSpreaderUpdateMessage msg;
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(434.50).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	
	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.24}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::NO);
	//EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, AskAddTicksAndRemoveTicksProrata)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[2] = 3;
	settings.spreaderPricingSettings.removeTicks[2] = 1;
	settings.spreaderPricingSettings.lockQty[2] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[2] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());
	
	strategy->InjectDepth({{100000, -14.50}}, {{100000, -14.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	stacker::StackerSpreaderUpdateMessage msg;
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(434.75).AsShifted());
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::NO);
	//EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, BidAddTicksAndRemoveTicksQuoteStack1)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}, {200000, -14.50}}, {{100000, -14.00}, {200000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.50, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.75, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.75, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.5, ask[1]));

	strategy->InjectAck(bid[0]);
	strategy->InjectAck(bid[1]);
	strategy->InjectAck(ask[0]);
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100000, -14.00}, {200000, -14.25}}, {{100000, -13.75}, {200000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[1], -14.25));
	EXPECT_TRUE(e.ReadModify(ask[0], -13.25));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.00));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.50));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.00));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, AskAddTicksAndRemoveTicksQuoteStack1)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[0] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}, {200000, -14.50}}, {{100000, -14.00}, {200000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.50, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.75, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.75, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.5, ask[1]));

	strategy->InjectAck(bid[0]);
	strategy->InjectAck(bid[1]);
	strategy->InjectAck(ask[0]);
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100000, -14.50}, {200000, -14.75}}, {{100000, -14.25}, {200000, -14.00}});
	EXPECT_TRUE(e.ReadModify(bid[0], -15.00));
	EXPECT_TRUE(e.ReadModify(ask[1], -14.00));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	EXPECT_TRUE(e.ReadModify(ask[0], -14.25));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadModify(ask[0], -13.75));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 435.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_TRUE(e.ReadModify(ask[0], -14.25));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, BidAddTicksAndRemoveTicksQuoteStack2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = false;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 1;
	settings.spreaderPricingSettings.lockQty[1] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[1] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200'000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(2000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(2000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(200'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}, {200000, -14.50}}, {{100000, -14.00}, {200000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2000, -14.50, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2000, -14.75, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2000, -13.75, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2000, -13.5, ask[1]));

	strategy->InjectAck(bid[0]);
	strategy->InjectAck(bid[1]);
	strategy->InjectAck(ask[0]);
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100000, -14.00}, {200000, -14.25}}, {{100000, -13.75}, {200000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[1], -14.25));
	EXPECT_TRUE(e.ReadModify(ask[0], -13.25));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.00));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.50));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.00));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, AskAddTicksAndRemoveTicksQuoteStack2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[0] = false;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 1;
	settings.spreaderPricingSettings.lockQty[1] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[1] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200'000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(2000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(2000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(200'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}, {200000, -14.50}},
						  {{100000, -14.00}, {200000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2000, -14.50, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2000, -14.75, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2000, -13.75, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2000, -13.5, ask[1]));

	strategy->InjectAck(bid[0]);
	strategy->InjectAck(bid[1]);
	strategy->InjectAck(ask[0]);
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100000, -14.50}, {200000, -14.75}}, {{100000, -14.25}, {200000, -14.00}});
	EXPECT_TRUE(e.ReadModify(bid[0], -15.00));
	EXPECT_TRUE(e.ReadModify(ask[1], -14.00));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	EXPECT_TRUE(e.ReadModify(ask[0], -14.25));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadModify(ask[0], -13.75));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 435.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_TRUE(e.ReadModify(ask[0], -14.25));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, BidAddTicksAndRemoveTicksQuoteProrata)
{
	InitProrataProfile("P1", {250'000, 500'000}, {5'000, 10'000}, {0, 0}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = false;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 1;
	settings.spreaderPricingSettings.lockQty[1] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[1] = false;
	settings.spreaderPricingSettings.addTicks[2] = 3;
	settings.spreaderPricingSettings.removeTicks[2] = 1;
	settings.spreaderPricingSettings.lockQty[2] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[2] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200'000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(2000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(2000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(200'000);

	settings.quoteInstrumentSettings.bidSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}, {200000, -14.50}},
						  {{100000, -14.00}, {200000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid;
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100000, -14.00}, {200000, -14.25}},
						  {{100000, -13.75}, {200000, -13.5}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5000, -14.00, bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadCancel(bid));
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1001));

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5000, -14.00, bid));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, AskAddTicksAndRemoveTicksQuoteProrata)
{
	InitProrataProfile("P1", {250'000, 500'000}, {5'000, 10'000}, {0, 0}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[0] = false;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 1;
	settings.spreaderPricingSettings.lockQty[1] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[1] = false;
	settings.spreaderPricingSettings.addTicks[2] = 3;
	settings.spreaderPricingSettings.removeTicks[2] = 1;
	settings.spreaderPricingSettings.lockQty[2] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[2] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200'000);

	settings.quoteInstrumentSettings.secondStackEnabled = true;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].length = 0;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].qty = Quantity(2000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[1].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].length = 0;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].qty = Quantity(2000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[1].lean = Quantity(200'000);

	settings.quoteInstrumentSettings.askSettings.prorataEnabled = true;
	settings.quoteInstrumentSettings.askSettings.prorataProfile = "P1";

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}, {200000, -14.50}},
						  {{100000, -14.00}, {200000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod ask;
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100000, -14.50}, {200000, -14.75}},
						  {{100000, -14.25}, {200000, -14.00}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5000, -14.25, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadCancel(ask));
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1001));

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 435.75}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5000, -14.25, ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, BidStrikeChanging)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 2;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	stacker::StackerSpreaderUpdateMessage msg;
	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100000, -14.00}}, {{100000, -13.75}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.50));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(434.50).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.75));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(434.75).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(435.00));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(435.00).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.50}}, {{1000, 435.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.75}}, {{1000, 436.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	//EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, BidStrikeResetWhenMarketQtyGoesBelowResetQty)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;
	settings.spreaderPricingSettings.resetQty = Quantity(20'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100'000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	stacker::StackerSpreaderUpdateMessage msg;
	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100'000, -14.00}}, {{100000, -13.75}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.50));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(434.50).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.50));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{20'000, -14.00}}, {{100000, -13.75}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.50));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{19'999, -14.00}}, {{100000, -13.75}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.25));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{20'000, -14.00}}, {{100000, -13.75}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.25));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(434.25).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.50}}, {{1000, 433.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	//EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, AskStrikeResetWhenMarketQtyGoesBelowResetQty)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[0] = true;
	settings.spreaderPricingSettings.resetQty = Quantity(20'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100'000, -14.25}}, {{100'000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	stacker::StackerSpreaderUpdateMessage msg;
	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100'000, -14.50}}, {{100'000, -14.25}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(434.75));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(434.75).AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(434.75));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{100'000, -14.50}}, {{20'000, -14.25}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(434.75));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{100'000, -14.50}}, {{19'999, -14.25}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(435.00));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{100'000, -14.50}}, {{20'000, -14.25}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(435.00));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(435.00).AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.50}}, {{1000, 435.75}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::NO);
	//EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, BidStrikeResetWhenMarketQtyGoesBelowResetQtyAfterAddTicksMet)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 2;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;
	settings.spreaderPricingSettings.resetQty = Quantity(20'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100'000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	stacker::StackerSpreaderUpdateMessage msg;
	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100'000, -14.00}}, {{100'000, -13.75}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.50));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(434.50).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.50));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.50));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.50));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{20'000, -14.00}}, {{100'000, -13.75}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(434.50));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{19'999, -14.00}}, {{100'000, -13.75}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(433.75));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(433.75).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{20'000, -14.00}}, {{100'000, -13.75}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(433.75));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 433.50}}, {{1000, 433.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.25}}, {{1000, 433.50}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.00}}, {{1000, 433.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, AskStrikeResetWhenMarketQtyGoesBelowResetQtyAfterAddTicksMet)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 2;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[0] = true;
	settings.spreaderPricingSettings.resetQty = Quantity(20'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100'000, -14.25}}, {{100'000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	stacker::StackerSpreaderUpdateMessage msg;
	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100'000, -14.50}}, {{100'000, -14.25}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(434.75));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(434.75).AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(434.75));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(434.75));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(434.75));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{100'000, -14.50}}, {{20'000, -14.25}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(434.75));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{100'000, -14.50}}, {{19'999, -14.25}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(435.50));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(435.50).AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100'000, -14.50}}, {{20'000, -14.25}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(435.50));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, 435.50}}, {{1000, 435.75}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.75}}, {{1000, 436.00}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 436.00}}, {{1000, 436.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, BidRemoveTicksDueToGap)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100000, -14.00}}, {{100000, -13.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	stacker::StackerSpreaderUpdateMessage msg;
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(434.50).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, AskRemoveTicksDueToGap)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[0] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}}, {{100000, -14.00}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100000, -14.50}}, {{100000, -14.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	stacker::StackerSpreaderUpdateMessage msg;
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(434.75).AsShifted());
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.25}}, {{1000, 435.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 435.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 435.50}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::NO);
	// EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest2, BidAddTicksSwitchToLeanAfterLockQtyMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(150'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(150'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100'000, -14.25}, {200'000, -14.50}}, {{100'000, -14.00}, {200'000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.50, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.75, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.75, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.5, ask[1]));

	strategy->InjectAck(bid[0]);
	strategy->InjectAck(bid[1]);
	strategy->InjectAck(ask[0]);
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[1], -14.25));
	EXPECT_TRUE(e.ReadModify(ask[0], -13.25));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.00));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{200'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{150'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{149'999, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.50));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, BidAddTicksSwitchToLeanAfterLockQtyMetThenCancelDespiteAddTicksMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(150'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(150'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100'000, -14.25}, {200'000, -14.50}}, {{100'000, -14.00}, {200'000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.50, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.75, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.75, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.5, ask[1]));

	strategy->InjectAck(bid[0]);
	strategy->InjectAck(bid[1]);
	strategy->InjectAck(ask[0]);
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[1], -14.25));
	EXPECT_TRUE(e.ReadModify(ask[0], -13.25));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.00));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{200'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{150'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{149'999, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.50));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, BidAddTicksLeanOnLockQtyWhenLowerThanLean)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(150'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100'000, -14.25}, {200'000, -14.50}}, {{100'000, -14.00}, {200'000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.50, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.75, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.75, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.5, ask[1]));

	strategy->InjectAck(bid[0]);
	strategy->InjectAck(bid[1]);
	strategy->InjectAck(ask[0]);
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[1], -14.25));
	EXPECT_TRUE(e.ReadModify(ask[0], -13.25));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.00));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{150'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{150'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{149'999, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.50));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, BidAddTicksLeanOnLockQtyWhenLowerThanLeanAndCancelDespiteAddTicksMet)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(150'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(200'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(200'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100'000, -14.25}, {200'000, -14.50}}, {{100'000, -14.00}, {200'000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.50, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.75, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.75, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.5, ask[1]));

	strategy->InjectAck(bid[0]);
	strategy->InjectAck(bid[1]);
	strategy->InjectAck(ask[0]);
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[1], -14.25));
	EXPECT_TRUE(e.ReadModify(ask[0], -13.25));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.00));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{150'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{149'999, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.50));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, BidAddTicksLeanMetButNotLockQty)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(150'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(150'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100'000, -14.25}, {200'000, -14.50}}, {{100'000, -14.00}, {200'000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.50, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.75, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.75, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.5, ask[1]));

	strategy->InjectAck(bid[0]);
	strategy->InjectAck(bid[1]);
	strategy->InjectAck(ask[0]);
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[1], -14.25));
	EXPECT_TRUE(e.ReadModify(ask[0], -13.25));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.00));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{150'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.50));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, BidAddTicksMetThanSpreadQtyDecreases)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;
	settings.spreaderPricingSettings.resetQty = Quantity(1);

	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.bidSettings.stackSideSettings[0].lean = Quantity(150'000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].length = 2;
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].qty = Quantity(1000);
	settings.quoteInstrumentSettings.askSettings.stackSideSettings[0].lean = Quantity(150'000);

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100'000, -14.25}, {200'000, -14.50}}, {{100'000, -14.00}, {200'000, -13.75}});
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.50, bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -14.75, bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.75, ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -13.5, ask[1]));

	strategy->InjectAck(bid[0]);
	strategy->InjectAck(bid[1]);
	strategy->InjectAck(ask[0]);
	strategy->InjectAck(ask[1]);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100'000, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadModify(bid[1], -14.25));
	EXPECT_TRUE(e.ReadModify(ask[0], -13.25));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_TRUE(e.ReadModify(bid[0], -14.00));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100, -14.00}, {200'000, -14.25}}, {{100'000, -13.75}, {200'000, -13.5}});
	EXPECT_TRUE(e.ReadNothing());

	// strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	// EXPECT_TRUE(e.ReadModify(bid[0], -14.50));
	// EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest2, StrategyStartsInPreopenAndSwitchesToOpen)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 1;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -14.25}}, {{100000, -14.00}},
						  KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);
	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}},
						  KT01::DataStructures::MarketDepth::BookSecurityStatus::PreOpen);

	strategy->InjectMarketState(KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);
	strategy->InjectMarketState(1, KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade);

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100000, -14.00}}, {{100000, -13.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	stacker::StackerSpreaderUpdateMessage msg;
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(434.50).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.24}}, {{1000, 434.50}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.50}}, {{1000, 434.75}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 434.75}}, {{1000, 435.00}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, 435.00}}, {{1000, 435.25}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	// strategy->InjectDepth(1, {{1000, 434.25}}, {{1000, 434.50}});
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	// EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	// strategy->InjectDepth(1, {{1000, 434.00}}, {{1000, 434.25}});
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	// EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	// strategy->InjectDepth(1, {{1000, 433.75}}, {{1000, 434.00}});
	// EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	// EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}
}

int main(int argc, char* argv[])
{
	bool y = OrderNums::instance().isLoaded();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
