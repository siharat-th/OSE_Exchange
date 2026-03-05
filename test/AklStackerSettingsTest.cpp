#include <gtest/gtest.h>
#include <chrono>

#include <AlgoParams/AklStackerParams.hpp>
#include "impl/ExchangeSender.hpp"

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
	

using ClockType = std::chrono::steady_clock;
using TimestampType = ClockType::time_point;
using DurationType = ClockType::duration;

class AklStackerParamTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	AklStackerParamTest()
	{
		
	}

	virtual void SetUp() override
	{

	}

	virtual void TearDown() override
	{

	}

	impl::ExchangeSender exchangeSender;
	akl::stacker::AklStackerParams params;
};

TEST_F(AklStackerParamTest, Test1)
{
	// std::cout << config << std::endl;
	// stacker::AklStackerParams params = stacker::AklStackerParams::parse(config);

	// const std::string configCopy = params.toJson();
	// stacker::AklStackerParams paramsCopy = stacker::AklStackerParams::parse(configCopy);

	// EXPECT_EQ(params, paramsCopy);

	//EXPECT_EQ(params.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty, Quantity(11));
	//EXPECT_EQ(params.quoteInstrumentSettings.bidSettings.stackSideSettings[0].length, 5);

	//fmt::print("BidQty: {}\n", params.quoteInstrumentSettings.bidSettings.stackSideSettings[0].qty.value());

	EXPECT_TRUE(true);
}

TEST_F(AklStackerParamTest, Test2)
{
	EXPECT_TRUE(true);
}

TEST_F(AklStackerParamTest, Test3)
{
	EXPECT_TRUE(true);
}

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}