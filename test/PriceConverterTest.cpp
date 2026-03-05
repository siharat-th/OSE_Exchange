#include <cstdint>
#include <gtest/gtest.h>

#include <KT01/SecDefs/CmeSecMasterV2.hpp>
#include <Algos/AklStacker/PriceConverter.hpp>

namespace akl::test
{

class PriceConverterTest : public ::testing::Test
{
public:
	static void SetUpTestSuite()
	{

	}

	static void TearDownTestSuite()
	{

	}

	PriceConverterTest()
	{
		
	}

	virtual void SetUp() override
	{

	}

	virtual void TearDown() override
	{
		
	}
};

TEST_F(PriceConverterTest, DigitParse)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ESM5");
	const stacker::PriceConverter pc(def);
	EXPECT_EQ(pc.ParseDigits("0", 1), 0);
	EXPECT_EQ(pc.ParseDigits("1", 1), 1);
	EXPECT_EQ(pc.ParseDigits("2", 1), 2);
	EXPECT_EQ(pc.ParseDigits("3", 1), 3);
	EXPECT_EQ(pc.ParseDigits("4", 1), 4);
	EXPECT_EQ(pc.ParseDigits("5", 1), 5);
	EXPECT_EQ(pc.ParseDigits("6", 1), 6);
	EXPECT_EQ(pc.ParseDigits("7", 1), 7);
	EXPECT_EQ(pc.ParseDigits("8", 1), 8);
	EXPECT_EQ(pc.ParseDigits("9", 1), 9);
	
	EXPECT_EQ(pc.ParseDigits("", 1), 0);
	EXPECT_EQ(pc.ParseDigits("01", 1), 0);
	EXPECT_EQ(pc.ParseDigits("10", 1), 1);
	EXPECT_EQ(pc.ParseDigits("10", 2), 10);
	EXPECT_EQ(pc.ParseDigits("10", 3), 10);
}

TEST_F(PriceConverterTest, ES)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ESM5");
	const stacker::PriceConverter pc(def);
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(450000.00)).c_str(), "4500.00");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(450025.00)).c_str(), "4500.25");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(450050.00)).c_str(), "4500.50");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(450075.00)).c_str(), "4500.75");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(450100.00)).c_str(), "4501.00");

	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(0.00)).c_str(), "0.00");

	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(-1225.00)).c_str(), "-12.25");
}

TEST_F(PriceConverterTest, ES2)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ESM5");
	const stacker::PriceConverter pc(def);

	Price pxOut;
	EXPECT_TRUE(pc.TryParse("4500.00", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(450000.00));
	EXPECT_TRUE(pc.TryParse("4500.25", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(450025.00));
	EXPECT_TRUE(pc.TryParse("4500.26", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(450025.00));
	EXPECT_TRUE(pc.TryParse("4500.255555555555555555", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(450025.00));

	EXPECT_FALSE(pc.TryParse("abc", pxOut));
	EXPECT_TRUE(pc.TryParse("123456789123456789123456", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(1234567800));
}

TEST_F(PriceConverterTest, ES3)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ESM5");
	const stacker::PriceConverter pc(def);

	const Price tickIncrement = Price::FromUnshifted(25);
	const Price startingPx = Price::FromUnshifted(-500000);
	const Price endingPx = Price::FromUnshifted(500000);

	for (Price px = startingPx; px <= endingPx; px += tickIncrement)
	{
		const std::string pxStr = pc.DisplayPrice(px);
		Price pxOut;
		EXPECT_TRUE(pc.TryParse(pxStr.c_str(), pxOut));
		EXPECT_EQ(px, pxOut);
	}
}

// TEST_F(PriceConverterTest, VX)
// {
// 	ttsdk::Instrument::ResponseCode responseCode;
// 	ttsdk::InstrumentPtr ttInstr = tt.GetInstrument(ttsdk::MarketId::CFE, "VX", ttsdk::ProductType::Future, "VX Dec23", responseCode);
// 	ASSERT_TRUE(ttInstr);

// 	const PriceConverter pc(ttInstr);
// 	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(0.05)).c_str(), "0.05");
// 	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(15.90)).c_str(), "15.90");
// 	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(15.95)).c_str(), "15.95");
// 	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(16.00)).c_str(), "16.00");
// 	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(16.05)).c_str(), "16.05");

// 	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(0.00)).c_str(), "0.00");

// 	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(-0.35)).c_str(), "-0.35");
// }

TEST_F(PriceConverterTest, ZC)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ZCH5");
	const stacker::PriceConverter pc(def);

	Price pxOut;
	EXPECT_TRUE(pc.TryParse("500", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(500.00));
	EXPECT_TRUE(pc.TryParse("500\'0", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(500.00));
	EXPECT_TRUE(pc.TryParse("500\'2", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(500.25));
	EXPECT_TRUE(pc.TryParse("500\'4", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(500.50));
	EXPECT_TRUE(pc.TryParse("500\'6", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(500.75));
	EXPECT_TRUE(pc.TryParse("501\'0", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(501.00));
	EXPECT_TRUE(pc.TryParse("500\'6666666", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(500.75));

	EXPECT_FALSE(pc.TryParse("abc", pxOut));

	EXPECT_TRUE(pc.TryParse("100000000\'0", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(99999999.00));
	EXPECT_TRUE(pc.TryParse("-100000000\'0", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-99999999.00));
	EXPECT_TRUE(pc.TryParse("500000000000", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(99999999.00));
}

TEST_F(PriceConverterTest, ZC2)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ZCH5");
	const stacker::PriceConverter pc(def);
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(500)).c_str(), "500\'0");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(500.25)).c_str(), "500\'2");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(500.50)).c_str(), "500\'4");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(500.75)).c_str(), "500\'6");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(501.00)).c_str(), "501\'0");

	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(0.00)).c_str(), "0\'0");

	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(-0.25)).c_str(), "-0\'2");
}

TEST_F(PriceConverterTest, ZC3)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ZCH5");
	const stacker::PriceConverter pc(def);

	const Price tickIncrement = Price::FromUnshifted(0.25);
	const Price startingPx = Price::FromUnshifted(-500);
	const Price endingPx = Price::FromUnshifted(500);

	for (Price px = startingPx; px <= endingPx; px += tickIncrement)
	{
		const std::string pxStr = pc.DisplayPrice(px);
		Price pxOut;
		EXPECT_TRUE(pc.TryParse(pxStr.c_str(), pxOut));
		EXPECT_EQ(px, pxOut);
		if (px != pxOut)
		{
			printf("%s %ld %ld", pxStr.c_str(), px.AsShifted(), pxOut.AsShifted());
		}
	}
}

TEST_F(PriceConverterTest, ZN)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ZNH5-ZNM5");
	const stacker::PriceConverter pc(def);
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(3)).c_str(), "3\'000");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(3.00781250)).c_str(), "3\'002");

	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(-3)).c_str(), "-3\'000");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(-3.00781250)).c_str(), "-3\'002");

	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(0)).c_str(), "0\'000");
}

TEST_F(PriceConverterTest, ZN2)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ZNH5-ZNM5");
	const stacker::PriceConverter pc(def);

	Price pxOut;
	EXPECT_TRUE(pc.TryParse("3", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3));
	EXPECT_TRUE(pc.TryParse("3\'0", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3));
	EXPECT_TRUE(pc.TryParse("3\'00", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3));
	EXPECT_TRUE(pc.TryParse("3\'000", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3));
	EXPECT_TRUE(pc.TryParse("3\'002", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.00781250));
	EXPECT_TRUE(pc.TryParse("3\'0022", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.00781250));
}

TEST_F(PriceConverterTest, ZN3)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ZNH5-ZNM5");
	const stacker::PriceConverter pc(def);

	const Price tickIncrement = Price::FromUnshifted(0.00781250);
	const Price startingPx = Price::FromUnshifted(-300);
	const Price endingPx = Price::FromUnshifted(300);

	for (Price px = startingPx; px <= endingPx; px += tickIncrement)
	{
		const std::string pxStr = pc.DisplayPrice(px);
		Price pxOut;
		EXPECT_TRUE(pc.TryParse(pxStr.c_str(), pxOut));
		EXPECT_EQ(px, pxOut);
		if (px != pxOut)
		{
			printf("%s %ld %ld", pxStr.c_str(), px.AsShifted(), pxOut.AsShifted());
		}
	}
}

TEST_F(PriceConverterTest, ZT)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ZTH5-ZTM5");
	const stacker::PriceConverter pc(def);
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(3)).c_str(), "3\'000");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(3.00781250)).c_str(), "3\'002");

	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(-3)).c_str(), "-3\'000");
	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(-3.00781250)).c_str(), "-3\'002");

	EXPECT_STRCASEEQ(pc.DisplayPrice(Price::FromUnshifted(0)).c_str(), "0\'000");
}

TEST_F(PriceConverterTest, ZT2)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ZTH5-ZTM5");
	const stacker::PriceConverter pc(def);

	Price pxOut;
	EXPECT_TRUE(pc.TryParse("0", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(0));
	EXPECT_TRUE(pc.TryParse("0\'0", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(0));
	EXPECT_TRUE(pc.TryParse("0\'00", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(0));
	EXPECT_TRUE(pc.TryParse("0\'000", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(0));

	EXPECT_TRUE(pc.TryParse("3\'001", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.00390625));
	EXPECT_TRUE(pc.TryParse("3\'002", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.00781250));
	EXPECT_TRUE(pc.TryParse("3\'003", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.01171875));
	EXPECT_TRUE(pc.TryParse("3\'005", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.01562500));
	EXPECT_TRUE(pc.TryParse("3\'006", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.01953125));
	EXPECT_TRUE(pc.TryParse("3\'007", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.02343750));
	EXPECT_TRUE(pc.TryParse("3\'008", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.02734375));
	EXPECT_TRUE(pc.TryParse("3\'010", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.03125000));
	EXPECT_TRUE(pc.TryParse("3\'011", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.03515625));
	EXPECT_TRUE(pc.TryParse("3\'012", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.03906250));
	EXPECT_TRUE(pc.TryParse("3\'013", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(3.04296875));

	EXPECT_TRUE(pc.TryParse("-3\'001", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.00390625));
	EXPECT_TRUE(pc.TryParse("-3\'002", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.00781250));
	EXPECT_TRUE(pc.TryParse("-3\'003", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.01171875));
	EXPECT_TRUE(pc.TryParse("-3\'005", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.01562500));
	EXPECT_TRUE(pc.TryParse("-3\'006", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.01953125));
	EXPECT_TRUE(pc.TryParse("-3\'007", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.02343750));
	EXPECT_TRUE(pc.TryParse("-3\'008", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.02734375));
	EXPECT_TRUE(pc.TryParse("-3\'010", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.03125000));
	EXPECT_TRUE(pc.TryParse("-3\'011", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.03515625));
	EXPECT_TRUE(pc.TryParse("-3\'012", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.03906250));
	EXPECT_TRUE(pc.TryParse("-3\'013", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.04296875));

	EXPECT_TRUE(pc.TryParse("-3\'01333333333333333333", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.04296875));
	EXPECT_TRUE(pc.TryParse("-3\'01\'333333333333333333", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.04296875));
	EXPECT_TRUE(pc.TryParse("-3\'0\'\'\'1\'333333333333333333", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-3.04296875));
	EXPECT_TRUE(pc.TryParse("-3000000000000000000000000000000\'0\'\'\'1\'333333333333333333", pxOut));
	EXPECT_EQ(pxOut, Price::FromUnshifted(-99999999));

	EXPECT_FALSE(pc.TryParse("", pxOut));
	EXPECT_FALSE(pc.TryParse("\'\'\'\'", pxOut));
	EXPECT_FALSE(pc.TryParse("/asfdkhjbhfvk\\", pxOut));
}

TEST_F(PriceConverterTest, ZT3)
{
	const KT01::SECDEF::CME::CmeSecDef def = KT01::SECDEF::CME::CmeSecMaster::instance().getSecDef("ZTH5-ZTM5");
	const stacker::PriceConverter pc(def);

	const Price tickIncrement = Price::FromUnshifted(0.00390625);
	const Price startingPx = Price::FromUnshifted(-300);
	const Price endingPx = Price::FromUnshifted(300);

	for (Price px = startingPx; px <= endingPx; px += tickIncrement)
	{
		const std::string pxStr = pc.DisplayPrice(px);
		Price pxOut;
		EXPECT_TRUE(pc.TryParse(pxStr.c_str(), pxOut));
		EXPECT_EQ(px, pxOut);
		if (px != pxOut)
		{
			printf("%s %ld %ld", pxStr.c_str(), px.AsShifted(), pxOut.AsShifted());
		}
	}
}

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}