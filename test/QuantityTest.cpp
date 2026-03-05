#include <gtest/gtest.h>

#include <akl/Quantity.hpp>

namespace akl::test
{

TEST(Quantity, EqualValues)
{
	Quantity q1 { 2 };
	const Quantity q2 { 2 };

	EXPECT_EQ(Quantity{2}, Quantity{2});
	EXPECT_EQ(Quantity(2), Quantity(2));
	EXPECT_NE(Quantity{2}, Quantity{1});
	EXPECT_EQ(-Quantity{2}, Quantity{-2});
	EXPECT_EQ(-Quantity{-2}, Quantity{2});
	EXPECT_EQ(-(-Quantity{2}), Quantity{2});
	EXPECT_EQ(q1, q2);

	EXPECT_TRUE(Quantity{2} == Quantity{2});
	EXPECT_FALSE(Quantity{2} != Quantity{2});
	EXPECT_FALSE(Quantity{2} < Quantity{2});
	EXPECT_TRUE(Quantity{2} <= Quantity{2});
	EXPECT_FALSE(Quantity{2} > Quantity{2});
	EXPECT_TRUE(Quantity{2} >= Quantity{2});
	EXPECT_TRUE(Quantity{-2} == -Quantity{2});
	EXPECT_TRUE(-(-Quantity{2}) == Quantity{2});

	EXPECT_TRUE(q1 == q1);
	EXPECT_FALSE(q1 != q1);
	EXPECT_FALSE(q1 < q1);
	EXPECT_TRUE(q1 <= q1);
	EXPECT_FALSE(q1 > q1);
	EXPECT_TRUE(q1 >= q1);

	EXPECT_TRUE(q2 == q2);
	EXPECT_FALSE(q2 != q2);
	EXPECT_FALSE(q2 < q2);
	EXPECT_TRUE(q2 <= q2);
	EXPECT_FALSE(q2 > q2);
	EXPECT_TRUE(q2 >= q2);

	EXPECT_TRUE(q1 == q2);
	EXPECT_FALSE(q1 != q2);
	EXPECT_FALSE(q1 < q2);
	EXPECT_TRUE(q1 <= q2);
	EXPECT_FALSE(q1 > q2);
	EXPECT_TRUE(q1 >= q2);

	EXPECT_TRUE(q2 == q1);
	EXPECT_FALSE(q2 != q1);
	EXPECT_FALSE(q2 < q1);
	EXPECT_TRUE(q2 <= q1);
	EXPECT_FALSE(q2 > q1);
	EXPECT_TRUE(q2 >= q1);
}

TEST(Quantity, Greater)
{
	Quantity q1 { 200 };
	const Quantity q2 { 2 };

	EXPECT_TRUE(q1 == q1);
	EXPECT_FALSE(q1 != q1);
	EXPECT_FALSE(q1 < q1);
	EXPECT_TRUE(q1 <= q1);
	EXPECT_FALSE(q1 > q1);
	EXPECT_TRUE(q1 >= q1);

	EXPECT_TRUE(q2 == q2);
	EXPECT_FALSE(q2 != q2);
	EXPECT_FALSE(q2 < q2);
	EXPECT_TRUE(q2 <= q2);
	EXPECT_FALSE(q2 > q2);
	EXPECT_TRUE(q2 >= q2);

	EXPECT_FALSE(q1 == q2);
	EXPECT_TRUE(q1 != q2);
	EXPECT_FALSE(q1 < q2);
	EXPECT_FALSE(q1 <= q2);
	EXPECT_TRUE(q1 > q2);
	EXPECT_TRUE(q1 >= q2);

	EXPECT_FALSE(q2 == q1);
	EXPECT_TRUE(q2 != q1);
	EXPECT_TRUE(q2 < q1);
	EXPECT_TRUE(q2 <= q1);
	EXPECT_FALSE(q2 > q1);
	EXPECT_FALSE(q2 >= q1);
}

TEST(Quantity, Addition)
{
	EXPECT_EQ(Quantity{1} + Quantity{2}, Quantity{3});
	EXPECT_EQ(Quantity{2} + Quantity{1}, Quantity{3});
	EXPECT_EQ(Quantity{1} + Quantity{2} + Quantity{1}, Quantity{4});
	EXPECT_NE(Quantity{1} + Quantity{2}, Quantity{4});

	EXPECT_TRUE(Quantity{1} + Quantity{2} == Quantity{3});
	EXPECT_TRUE(Quantity{2} + Quantity{1} == Quantity{3});
	EXPECT_FALSE(Quantity{2} + Quantity{1} == Quantity{4});

	Quantity q;
	EXPECT_EQ(q, Quantity{});
	EXPECT_EQ(q, Quantity{0});

	q += Quantity(10);
	EXPECT_EQ(q, Quantity{10});

	q += Quantity(111);
	EXPECT_EQ(q, Quantity{121});
}

TEST(Quantity, Subtraction)
{
	EXPECT_EQ(Quantity{10} - Quantity{2}, Quantity{8});
	EXPECT_EQ(-Quantity{2} + Quantity{10}, Quantity{8});
	EXPECT_EQ(Quantity{10} - Quantity{2} - Quantity{1}, Quantity{7});
	EXPECT_NE(Quantity{10} - Quantity{2}, Quantity{7});

	EXPECT_TRUE(Quantity{3} - Quantity{2} == Quantity{1});
	EXPECT_TRUE(-Quantity{2} + Quantity{3} == Quantity{1});
	EXPECT_FALSE(Quantity{3} - Quantity{2} == Quantity{4});

	Quantity q;
	EXPECT_EQ(q, Quantity{});
	EXPECT_EQ(q, Quantity{0});

	q -= Quantity(10);
	EXPECT_EQ(q, Quantity{-10});

	q -= Quantity(111);
	EXPECT_EQ(q, Quantity{-121});
}

TEST(Quantity, Increment)
{
	EXPECT_EQ(Quantity(10), ++Quantity(9));
	EXPECT_EQ(Quantity(10), Quantity(10)++);

	Quantity q1(0);
	EXPECT_EQ(q1, Quantity(0));
	Quantity q2 = ++q1;
	EXPECT_EQ(q1, Quantity(1));
	EXPECT_EQ(q2, Quantity(1));
	Quantity q3 = q2++;
	EXPECT_EQ(q1, Quantity(1));
	EXPECT_EQ(q2, Quantity(2));
	EXPECT_EQ(q3, Quantity(1));
}

TEST(Quantity, Decrement)
{
	EXPECT_EQ(Quantity(10), --Quantity(11));
	EXPECT_EQ(Quantity(10), Quantity(10)--);

	Quantity q1(7);
	EXPECT_EQ(q1, Quantity(7));
	Quantity q2 = --q1;
	EXPECT_EQ(q1, Quantity(6));
	EXPECT_EQ(q2, Quantity(6));
	Quantity q3 = q2--;
	EXPECT_EQ(q1, Quantity(6));
	EXPECT_EQ(q2, Quantity(5));
	EXPECT_EQ(q3, Quantity(6));
}

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}