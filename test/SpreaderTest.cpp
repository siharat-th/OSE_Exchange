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
		"\"Symbol\": \"ZNH5-ZNM5\", "
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

class SpreaderTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	SpreaderTest()
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

TEST_F(SpreaderTest, ValidatePriceConversion)
{
	EXPECT_EQ(tpx(108, 29, 5), 108.921875);
}

TEST_F(SpreaderTest, BidAddTicksAndRemoveTicks1)
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
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());
	
	strategy->InjectDepth({{100000, -tpx(0, 4, 5)}}, {{100000, -tpx(0, 4, 2)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	stacker::StackerSpreaderUpdateMessage msg;
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(tpx(108, 27, 5)).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	
	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 0)}}, {{1000, tpx(108, 27, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest, AskAddTicksAndRemoveTicks1)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 2;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[0] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	
	strategy->InjectDepth({{100000, -tpx(0, 5, 0)}}, {{100000, -tpx(0, 4, 7)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest, BidAddTicksAndRemoveTicks2)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 2;
	settings.spreaderPricingSettings.lockQty[1] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[1] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	
	strategy->InjectDepth({{100000, -tpx(0, 4, 5)}}, {{100000, -tpx(0, 4, 2)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 0)}}, {{1000, tpx(108, 27, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest, AskAddTicksAndRemoveTicks2)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 2;
	settings.spreaderPricingSettings.lockQty[1] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[1] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	
	strategy->InjectDepth({{100000, -tpx(0, 5, 0)}}, {{100000, -tpx(0, 4, 7)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(1), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(1), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest, BidAddTicksAndRemoveTicksProrata)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[2] = 3;
	settings.spreaderPricingSettings.removeTicks[2] = 2;
	settings.spreaderPricingSettings.lockQty[2] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[2] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	
	strategy->InjectDepth({{100000, -tpx(0, 4, 5)}}, {{100000, -tpx(0, 4, 2)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 0)}}, {{1000, tpx(108, 27, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest, AskAddTicksAndRemoveTicksProrata)
{
	settings.quoteInstrumentSettings.newOrders = false;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[2] = 3;
	settings.spreaderPricingSettings.removeTicks[2] = 2;
	settings.spreaderPricingSettings.lockQty[2] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[2] = true;

	strategy->onLaunch(getJson(settings));
	strategy->Start();
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	
	strategy->InjectDepth({{100000, -tpx(0, 5, 0)}}, {{100000, -tpx(0, 4, 7)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	
	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(2), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(2), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest, BidAddTicksAndRemoveTicksQuoteStack1)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 2;
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
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}, {200000, -tpx(0, 5, 0)}}, {{100000, -tpx(0, 4, 5)}, {200000, -tpx(0, 4, 2)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -tpx(0, 5, 0), bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -tpx(0, 5, 2), bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -tpx(0, 4, 2), ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -tpx(0, 4, 0), ask[1]));

	strategy->InjectDepth({{100000, -tpx(0, 4, 5)}, {200000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 2)}, {200000, -tpx(0, 4, 0)}});
	EXPECT_TRUE(e.ReadModify(bid[1], -tpx(0, 4, 7)));
	EXPECT_TRUE(e.ReadModify(ask[0], -tpx(0, 3, 7)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_TRUE(e.ReadModify(bid[0], -tpx(0, 4, 5)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 27, 0)}}, {{1000, tpx(108, 27, 5)}});
	EXPECT_TRUE(e.ReadModify(bid[0], -tpx(0, 5, 0)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_TRUE(e.ReadModify(bid[0], -tpx(0, 4, 5)));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest, AskAddTicksAndRemoveTicksQuoteStack1)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 2;
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
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}, {200000, -tpx(0, 5, 0)}}, {{100000, -tpx(0, 4, 5)}, {200000, -tpx(0, 4, 2)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -tpx(0, 5, 0), bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 1000, -tpx(0, 5, 2), bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -tpx(0, 4, 2), ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 1000, -tpx(0, 4, 0), ask[1]));

	strategy->InjectDepth({{100000, -tpx(0, 5, 0)}, {200000, -tpx(0, 5, 2)}}, {{100000, -tpx(0, 4, 7)}, {200000, -tpx(0, 4, 5)}});
	EXPECT_TRUE(e.ReadModify(bid[0], -tpx(0, 5, 5)));
	EXPECT_TRUE(e.ReadModify(ask[1], -tpx(0, 4, 5)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_TRUE(e.ReadModify(ask[0], -tpx(0, 4, 7)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_TRUE(e.ReadModify(ask[0], -tpx(0, 4, 2)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_TRUE(e.ReadModify(ask[0], -tpx(0, 4, 7)));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest, BidAddTicksAndRemoveTicksQuoteStack2)
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
	settings.spreaderPricingSettings.removeTicks[1] = 2;
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
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}, {200000, -tpx(0, 5, 0)}}, {{100000, -tpx(0, 4, 5)}, {200000, -tpx(0, 4, 2)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2000, -tpx(0, 5, 0), bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2000, -tpx(0, 5, 2), bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2000, -tpx(0, 4, 2), ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2000, -tpx(0, 4, 0), ask[1]));

	strategy->InjectDepth({{100000, -tpx(0, 4, 5)}, {200000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 2)}, {200000, -tpx(0, 4, 0)}});
	EXPECT_TRUE(e.ReadModify(bid[1], -tpx(0, 4, 7)));
	EXPECT_TRUE(e.ReadModify(ask[0], -tpx(0, 3, 7)));
	EXPECT_TRUE(e.ReadNothing());
	
	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_TRUE(e.ReadModify(bid[0], -tpx(0, 4, 5)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 27, 0)}}, {{1000, tpx(108, 27, 5)}});
	EXPECT_TRUE(e.ReadModify(bid[0], -tpx(0, 5, 0)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_TRUE(e.ReadModify(bid[0], -tpx(0, 4, 5)));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest, AskAddTicksAndRemoveTicksQuoteStack2)
{
	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 2;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[0] = false;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 2;
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
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}, {200000, -tpx(0, 5, 0)}},
						  {{100000, -tpx(0, 4, 5)}, {200000, -tpx(0, 4, 2)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid[2];
	KTN::OrderPod ask[2];
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2000, -tpx(0, 5, 0), bid[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 2000, -tpx(0, 5, 2), bid[1]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2000, -tpx(0, 4, 2), ask[0]));
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 2000, -tpx(0, 4, 0), ask[1]));

	strategy->InjectDepth({{100000, -tpx(0, 5, 0)}, {200000, -tpx(0, 5, 2)}},
						  {{100000, -tpx(0, 4, 7)}, {200000, -tpx(0, 4, 5)}});
	EXPECT_TRUE(e.ReadModify(bid[0], -tpx(0, 5, 5)));
	EXPECT_TRUE(e.ReadModify(ask[1], -tpx(0, 4, 5)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_TRUE(e.ReadModify(ask[0], -tpx(0, 4, 7)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_TRUE(e.ReadModify(ask[0], -tpx(0, 4, 2)));
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_TRUE(e.ReadModify(ask[0], -tpx(0, 4, 7)));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest, BidAddTicksAndRemoveTicksQuoteProrata)
{
	InitProrataProfile("P1", {250'000, 500'000}, {5'000, 10'000}, {0, 0}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 2;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[0] = false;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 2;
	settings.spreaderPricingSettings.lockQty[1] = Quantity(200'000);
	settings.spreaderPricingSettings.bidEnabled[1] = false;
	settings.spreaderPricingSettings.addTicks[2] = 3;
	settings.spreaderPricingSettings.removeTicks[2] = 2;
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
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}, {200000, -tpx(0, 5, 0)}},
						  {{100000, -tpx(0, 4, 5)}, {200000, -tpx(0, 4, 2)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod bid;
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100000, -tpx(0, 4, 5)}, {200000, -tpx(0, 4, 7)}},
						  {{100000, -tpx(0, 4, 2)}, {200000, -tpx(0, 4, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5000, -tpx(0, 4, 5), bid));
	strategy->InjectAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 27, 0)}}, {{1000, tpx(108, 27, 5)}});
	EXPECT_TRUE(e.ReadCancel(bid));
	strategy->InjectCancelAck(bid);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1001));

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	ASSERT_TRUE(e.ReadSubmit(Side::BUY, 5000, -tpx(0, 4, 5), bid));
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest, AskAddTicksAndRemoveTicksQuoteProrata)
{
	InitProrataProfile("P1", {250'000, 500'000}, {5'000, 10'000}, {0, 0}, {0, 0}, 2);

	settings.quoteInstrumentSettings.newOrders = true;
	settings.quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(10000);
	settings.quoteInstrumentSettings.riskSettings.maxPosition = Quantity(10000);
	settings.quoteInstrumentSettings.cancelUnadoptedOrders = true;
	settings.spreaderPricingSettings.addTicks[0] = 3;
	settings.spreaderPricingSettings.removeTicks[0] = 2;
	settings.spreaderPricingSettings.lockQty[0] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[0] = false;
	settings.spreaderPricingSettings.addTicks[1] = 3;
	settings.spreaderPricingSettings.removeTicks[1] = 2;
	settings.spreaderPricingSettings.lockQty[1] = Quantity(200'000);
	settings.spreaderPricingSettings.askEnabled[1] = false;
	settings.spreaderPricingSettings.addTicks[2] = 3;
	settings.spreaderPricingSettings.removeTicks[2] = 2;
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
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}, {200000, -tpx(0, 5, 0)}},
						  {{100000, -tpx(0, 4, 5)}, {200000, -tpx(0, 4, 2)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	strategy->AdvanceTime(stacker::Millis(1000));

	strategy->InjectMessage(stacker::StackerRequoteMessage{});
	KTN::OrderPod ask;
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth({{100000, -tpx(0, 5, 0)}, {200000, -tpx(0, 5, 2)}},
						  {{100000, -tpx(0, 4, 7)}, {200000, -tpx(0, 4, 5)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5000, -tpx(0, 4, 7), ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_TRUE(e.ReadCancel(ask));
	strategy->InjectCancelAck(ask);
	EXPECT_TRUE(e.ReadNothing());

	strategy->AdvanceTime(stacker::Millis(1001));

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_TRUE(e.ReadNothing());

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	ASSERT_TRUE(e.ReadSubmit(Side::SELL, 5000, -tpx(0, 4, 7), ask));
	strategy->InjectAck(ask);
	EXPECT_TRUE(e.ReadNothing());
}

TEST_F(SpreaderTest, BidStrikeChanging)
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
	strategy->InjectDepth({{100000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	stacker::StackerSpreaderUpdateMessage msg;
	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100000, -tpx(0, 4, 5)}}, {{100000, -tpx(0, 4, 2)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 5)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(tpx(108, 27, 5)).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 28, 0)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(tpx(108, 28, 0)).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_TRUE(e.ReadNothing());
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 28, 5)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(tpx(108, 28, 5)).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 27, 0)}}, {{1000, tpx(108, 27, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 27, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest, BidStrikeResetWhenMarketQtyGoesBelowResetQty)
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
	strategy->InjectDepth({{100'000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	stacker::StackerSpreaderUpdateMessage msg;
	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100'000, -tpx(0, 4, 5)}}, {{100000, -tpx(0, 4, 2)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 5)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(tpx(108, 27, 5)).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 27, 0)}}, {{1000, tpx(108, 27, 5)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 5)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{20'000, -tpx(0, 4, 5)}}, {{100000, -tpx(0, 4, 2)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 5)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{19'999, -tpx(0, 4, 5)}}, {{100000, -tpx(0, 4, 2)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 0)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{20'000, -tpx(0, 4, 5)}}, {{100000, -tpx(0, 4, 2)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 0)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(tpx(108, 27, 0)).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 25, 5)}}, {{1000, tpx(108, 26, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest, AskStrikeResetWhenMarketQtyGoesBelowResetQty)
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
	strategy->InjectDepth({{100'000, -tpx(0, 4, 7)}}, {{100'000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	stacker::StackerSpreaderUpdateMessage msg;
	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100'000, -tpx(0, 5, 0)}}, {{100'000, -tpx(0, 4, 7)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 28, 0)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(tpx(108, 28, 0)).AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 28, 0)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{100'000, -tpx(0, 5, 0)}}, {{20'000, -tpx(0, 4, 7)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 28, 0)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{100'000, -tpx(0, 5, 0)}}, {{19'999, -tpx(0, 4, 7)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 28, 5)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{100'000, -tpx(0, 5, 0)}}, {{20'000, -tpx(0, 4, 7)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 28, 5)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(tpx(108, 28, 5)).AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 5)}}, {{1000, tpx(108, 30, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::NO);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest, BidStrikeResetWhenMarketQtyGoesBelowResetQtyAfterAddTicksMet)
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
	strategy->InjectDepth({{100'000, -tpx(0, 4, 7)}}, {{100000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	stacker::StackerSpreaderUpdateMessage msg;
	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100'000, -tpx(0, 4, 5)}}, {{100'000, -tpx(0, 4, 2)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 5)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(tpx(108, 27, 5)).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 27, 0)}}, {{1000, tpx(108, 27, 5)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 5)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 5)}}, {{1000, tpx(108, 27, 0)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 5)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 26, 0)}}, {{1000, tpx(108, 26, 5)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 5)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{20'000, -tpx(0, 4, 5)}}, {{100'000, -tpx(0, 4, 2)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 27, 5)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{19'999, -tpx(0, 4, 5)}}, {{100'000, -tpx(0, 4, 2)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 26, 0)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::FromUnshifted(tpx(108, 26, 0)).AsShifted());
	EXPECT_EQ(msg.askStrike, Price::Max().AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{20'000, -tpx(0, 4, 5)}}, {{100'000, -tpx(0, 4, 2)}});
	ASSERT_TRUE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::BUY>(), Price::FromUnshifted(tpx(108, 26, 0)));
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 25, 5)}}, {{1000, tpx(108, 26, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 25, 0)}}, {{1000, tpx(108, 25, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 24, 5)}}, {{1000, tpx(108, 25, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

TEST_F(SpreaderTest, AskStrikeResetWhenMarketQtyGoesBelowResetQtyAfterAddTicksMet)
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
	strategy->InjectDepth({{100'000, -tpx(0, 4, 7)}}, {{100'000, -tpx(0, 4, 5)}});
	strategy->InjectDepth(1, {{1000, tpx(108, 27, 5)}}, {{1000, tpx(108, 28, 0)}});

	stacker::StackerSpreaderUpdateMessage msg;
	const stacker::SpreaderPricingModel &spm = strategy->GetInstrument()->GetSpreaderPricingModel();
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	EXPECT_FALSE(spm.HasStrikePrice<Side::SELL>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100'000, -tpx(0, 5, 0)}}, {{100'000, -tpx(0, 4, 7)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 28, 0)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(tpx(108, 28, 0)).AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 0)}}, {{1000, tpx(108, 28, 5)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 28, 0)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 28, 5)}}, {{1000, tpx(108, 29, 0)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 28, 0)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 0)}}, {{1000, tpx(108, 29, 5)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 28, 0)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{100'000, -tpx(0, 5, 0)}}, {{20'000, -tpx(0, 4, 7)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 28, 0)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth({{100'000, -tpx(0, 5, 0)}}, {{19'999, -tpx(0, 4, 7)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 29, 5)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	ASSERT_TRUE(ReadMessage<stacker::StackerSpreaderUpdateMessage>(msg));
	EXPECT_EQ(msg.bidStrike, Price::Min().AsShifted());
	EXPECT_EQ(msg.askStrike, Price::FromUnshifted(tpx(108, 29, 5)).AsShifted());
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth({{100'000, -tpx(0, 5, 0)}}, {{20'000, -tpx(0, 4, 7)}});
	EXPECT_FALSE(spm.HasStrikePrice<Side::BUY>());
	ASSERT_TRUE(spm.HasStrikePrice<Side::SELL>());
	EXPECT_EQ(spm.StrikePrice<Side::SELL>(), Price::FromUnshifted(tpx(108, 29, 5)));
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->AdvanceTime(stacker::Seconds(1));
	strategy->ExecuteStatusThread();
	EXPECT_TRUE(ReadNoMessage<stacker::StackerSpreaderUpdateMessage>());

	strategy->InjectDepth(1, {{1000, tpx(108, 29, 5)}}, {{1000, tpx(108, 30, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 30, 0)}}, {{1000, tpx(108, 30, 5)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);

	strategy->InjectDepth(1, {{1000, tpx(108, 30, 5)}}, {{1000, tpx(108, 31, 0)}});
	EXPECT_EQ(spm.CanQuote<Side::SELL>(0), stacker::SpreaderPricingModel::YES);
	EXPECT_EQ(spm.CanQuote<Side::BUY>(0), stacker::SpreaderPricingModel::YES_IF_LEAN_MET);
}

}

int main(int argc, char* argv[])
{
	bool y = OrderNums::instance().isLoaded();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}