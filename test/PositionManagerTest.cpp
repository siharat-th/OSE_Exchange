// #include <Algos/AklStacker/PositionManager.hpp>
// #include "Helpers.hpp"

// #include <Algos/AklStacker/AklStacker.hpp>
// #include <AlgoParams/AlgoInitializer.hpp>

// #include <gtest/gtest.h>

// namespace akl::stacker::test
// {

// TEST(PositionManager, MaxPayupTicksLong)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75), 0), QTY(0));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4499.75), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00), 0), QTY(1));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25), 0), QTY(3));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50), 0), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75), 0), QTY(7));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4499.50), 1), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4499.75), 1), QTY(1));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00), 1), QTY(3));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25), 1), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50), 1), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75), 1), QTY(7));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4499.25), 2), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4499.50), 2), QTY(1));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4499.75), 2), QTY(3));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00), 2), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25), 2), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50), 2), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75), 2), QTY(7));

// 	ASSERT_TRUE(pm.GetInnermostPrice<KTN::ORD::KOrderSide::Enum::BUY>().has_value());
// 	EXPECT_EQ(*pm.GetInnermostPrice<KTN::ORD::KOrderSide::Enum::BUY>(), PX(4500.50));
// 	EXPECT_FALSE(pm.GetInnermostPrice<KTN::ORD::KOrderSide::Enum::SELL>().has_value());

// 	pm.ClearPosition();
// 	EXPECT_EQ(pm.Position(), QTY(0));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75), 0), QTY(0));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4499.75), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75), 0), QTY(0));
// }

// TEST(PositionManager, MaxPayupTicksShort)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-4), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(-4));
// 	pm.Add(QTY(-2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-6));
// 	pm.Add(QTY(-1), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));

// 	EXPECT_FALSE(pm.GetInnermostPrice<KTN::ORD::KOrderSide::Enum::BUY>().has_value());
// 	ASSERT_TRUE(pm.GetInnermostPrice<KTN::ORD::KOrderSide::Enum::SELL>().has_value());
// 	EXPECT_EQ(*pm.GetInnermostPrice<KTN::ORD::KOrderSide::Enum::SELL>(), PX(4500.00));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4499.75), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75), 0), QTY(0));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50), 0), QTY(1));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25), 0), QTY(3));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00), 0), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75), 0), QTY(7));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00), 1), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75), 1), QTY(1));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50), 1), QTY(3));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25), 1), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00), 1), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75), 1), QTY(7));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.25), 2), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00), 2), QTY(1));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75), 2), QTY(3));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50), 2), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25), 2), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00), 2), QTY(7));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75), 2), QTY(7));

// 	pm.ClearPosition();
// 	EXPECT_EQ(pm.Position(), QTY(0));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4499.75), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75), 0), QTY(0));

// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00), 0), QTY(0));
// 	EXPECT_EQ(pm.GetHedgeQuantity<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75), 0), QTY(0));
// }

// TEST(PositionManager, NettingWhenStartingLong)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(7));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	pm.Add(QTY(-5), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(9));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	pm.Add(QTY(-3), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(6));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(3));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	pm.Add(QTY(-5), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	pm.Add(QTY(-2), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// }

// TEST(PositionManager, NettingWhenStartingShort)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(-14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(7));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	pm.Add(QTY(5), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(-9));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	pm.Add(QTY(3), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(-6));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(3));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	pm.Add(QTY(5), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	pm.Add(QTY(2), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// }

// TEST(PositionManager, ClearingSellMaxPayupPositionWhenLongDoesNothing)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4502.00), 0);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00), 0);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, ClearingBuyMaxPayupPositionWhenShortDoesNothing)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4502.00), 0);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00), 0);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// TEST(PositionManager, ClearLongPositionWithMaxPayupTicksZero)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00), 0);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75), 0);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50), 0);
// 	EXPECT_EQ(q, QTY(8));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25), 0);
// 	EXPECT_EQ(q, QTY(4));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00), 0);
// 	EXPECT_EQ(q, QTY(2));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75), 0);
// 	EXPECT_EQ(q, QTY(1));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, ClearShortPositionWithMaxPayupTicksZero)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00), 0);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25), 0);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50), 0);
// 	EXPECT_EQ(q, QTY(8));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75), 0);
// 	EXPECT_EQ(q, QTY(4));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00), 0);
// 	EXPECT_EQ(q, QTY(2));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25), 0);
// 	EXPECT_EQ(q, QTY(1));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// TEST(PositionManager, ClearLongPositionWithMaxPayupTicksZero2)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75), 0);
// 	EXPECT_EQ(q, QTY(15));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, ClearShortPositionWithMaxPayupTicksZero2)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25), 0);
// 	EXPECT_EQ(q, QTY(15));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// TEST(PositionManager, ClearLongPositionWithMaxPayupTicksOne)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00), 1);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75), 1);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50), 1);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25), 1);
// 	EXPECT_EQ(q, QTY(8));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00), 1);
// 	EXPECT_EQ(q, QTY(4));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75), 1);
// 	EXPECT_EQ(q, QTY(2));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.50), 1);
// 	EXPECT_EQ(q, QTY(1));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, ClearShortPositionWithMaxPayupTicksOne)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00), 1);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25), 1);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50), 1);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75), 1);
// 	EXPECT_EQ(q, QTY(8));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00), 1);
// 	EXPECT_EQ(q, QTY(4));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25), 1);
// 	EXPECT_EQ(q, QTY(2));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.50), 1);
// 	EXPECT_EQ(q, QTY(1));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// TEST(PositionManager, ClearLongPositionWithMaxPayupTicksOne2)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.50), 1);
// 	EXPECT_EQ(q, QTY(15));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, ClearShortPositionWithMaxPayupTicksOne2)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.50), 1);
// 	EXPECT_EQ(q, QTY(15));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// TEST(PositionManager, ClearLongPositionWithMaxPayupTicksTwo)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00), 2);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75), 2);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50), 2);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25), 2);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00), 2);
// 	EXPECT_EQ(q, QTY(8));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75), 2);
// 	EXPECT_EQ(q, QTY(4));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.50), 2);
// 	EXPECT_EQ(q, QTY(2));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.25), 2);
// 	EXPECT_EQ(q, QTY(1));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, ClearShortPositionWithMaxPayupTicksTwo)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00), 2);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25), 2);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50), 2);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75), 2);
// 	EXPECT_EQ(q, QTY(0));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00), 2);
// 	EXPECT_EQ(q, QTY(8));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25), 2);
// 	EXPECT_EQ(q, QTY(4));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.50), 2);
// 	EXPECT_EQ(q, QTY(2));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.75), 2);
// 	EXPECT_EQ(q, QTY(1));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// TEST(PositionManager, ClearLongPositionWithMaxPayupTicksTwo2)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.25), 2);
// 	EXPECT_EQ(q, QTY(15));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, ClearShortPositionWithMaxPayupTicksTwo2)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	Quantity q;

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	q = pm.ClearMaxPayupPosition<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.75), 2);
// 	EXPECT_EQ(q, QTY(15));
// 	EXPECT_EQ(pm.Position(), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// TEST(PositionManager, RepriceBidDoesNothing)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, RepriceAskDoesNothing)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// TEST(PositionManager, RepriceBidByOneTick)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(12));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(12));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, RepriceAskByOneTick)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(12));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(12));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// TEST(PositionManager, RepriceBidByTwoTicks)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, RepriceAskByTwoTicks)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(14));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// TEST(PositionManager, RepriceBidByThreeTicks)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(1), PX(4500.00));
// 	EXPECT_EQ(pm.Position(), QTY(1));
// 	pm.Add(QTY(2), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(3));
// 	pm.Add(QTY(4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(7));
// 	pm.Add(QTY(8), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(15));

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.25)));
// 	EXPECT_EQ(pm.Position(), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.25)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4499.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// }

// TEST(PositionManager, RepriceAskByThreeTicks)
// {
// 	AlgoInitializer params;
// 	AklStacker strategy(params);
// 	PositionManager pm(PX(0.25), strategy.GetLogger());
// 	pm.Add(QTY(-1), PX(4501.00));
// 	EXPECT_EQ(pm.Position(), QTY(-1));
// 	pm.Add(QTY(-2), PX(4500.75));
// 	EXPECT_EQ(pm.Position(), QTY(-3));
// 	pm.Add(QTY(-4), PX(4500.50));
// 	EXPECT_EQ(pm.Position(), QTY(-7));
// 	pm.Add(QTY(-8), PX(4500.25));
// 	EXPECT_EQ(pm.Position(), QTY(-15));

// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(1));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(2));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(4));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(8));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_TRUE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.75)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.75)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.75)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));

// 	EXPECT_FALSE(pm.RepricePositions<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.75)));
// 	EXPECT_EQ(pm.Position(), QTY(-15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.75)), QTY(15));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::SELL>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4501.00)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.75)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.50)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.25)), QTY(0));
// 	EXPECT_EQ(pm.PositionAt<KTN::ORD::KOrderSide::Enum::BUY>(PX(4500.00)), QTY(0));
// }

// }

// int main(int argc, char* argv[])
// {
// 	::testing::InitGoogleTest(&argc, argv);
// 	return RUN_ALL_TESTS();
// }