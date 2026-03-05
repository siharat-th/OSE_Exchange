#include <gtest/gtest.h>

#include <akl/containers/FixedSizeVector.hpp>

namespace akl::test
{

TEST(FixedSizeVectorTest, Initialization)
{
	FixedSizeVector<int, 5> vec;
	EXPECT_TRUE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 0);
	EXPECT_EQ(vec.Capacity(), 5);

	vec.PushBack(10);
	EXPECT_FALSE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 1);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);

	vec.PushBack(20);
	EXPECT_FALSE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 2);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 20);

	vec.PushBack(30);
	EXPECT_FALSE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 3);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 20);
	EXPECT_EQ(vec[2], 30);

	vec.PushBack(40);
	EXPECT_FALSE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 20);
	EXPECT_EQ(vec[2], 30);
	EXPECT_EQ(vec[3], 40);

	vec.PushBack(50);
	EXPECT_FALSE(vec.Empty());
	EXPECT_TRUE(vec.Full());
	EXPECT_EQ(vec.Size(), 5);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 20);
	EXPECT_EQ(vec[2], 30);
	EXPECT_EQ(vec[3], 40);
	EXPECT_EQ(vec[4], 50);

	vec.PopBack();
	EXPECT_FALSE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 20);
	EXPECT_EQ(vec[2], 30);
	EXPECT_EQ(vec[3], 40);
	
	vec.PushBack(55);
	EXPECT_FALSE(vec.Empty());
	EXPECT_TRUE(vec.Full());
	EXPECT_EQ(vec.Size(), 5);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 20);
	EXPECT_EQ(vec[2], 30);
	EXPECT_EQ(vec[3], 40);
	EXPECT_EQ(vec[4], 55);

	vec.PopBack();
	EXPECT_FALSE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 20);
	EXPECT_EQ(vec[2], 30);
	EXPECT_EQ(vec[3], 40);

	vec.PopBack();
	EXPECT_FALSE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 3);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 20);
	EXPECT_EQ(vec[2], 30);

	vec.PopBack();
	EXPECT_FALSE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 2);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);
	EXPECT_EQ(vec[1], 20);

	vec.PopBack();
	EXPECT_FALSE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 1);
	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec[0], 10);

	vec.PopBack();
	EXPECT_TRUE(vec.Empty());
	EXPECT_FALSE(vec.Full());
	EXPECT_EQ(vec.Size(), 0);
	EXPECT_EQ(vec.Capacity(), 5);
}

TEST(FixedSizeVectorTest, Copy)
{
	FixedSizeVector<int, 5> vec;
	vec.PushBack(111);
	vec.PushBack(222);
	vec.PushBack(333);

	FixedSizeVector<int, 5> vec2 = vec;

	EXPECT_FALSE(vec.Empty());
	EXPECT_FALSE(vec2.Empty());

	EXPECT_FALSE(vec.Full());
	EXPECT_FALSE(vec2.Full());

	EXPECT_EQ(vec.Size(), 3);
	EXPECT_EQ(vec2.Size(), 3);

	EXPECT_EQ(vec.Capacity(), 5);
	EXPECT_EQ(vec2.Capacity(), 5);

	EXPECT_EQ(vec[0], 111);
	EXPECT_EQ(vec2[0], 111);

	EXPECT_EQ(vec[1], 222);
	EXPECT_EQ(vec2[1], 222);

	EXPECT_EQ(vec[2], 333);
	EXPECT_EQ(vec2[2], 333);

	vec[0] = 1111;
	EXPECT_EQ(vec[0], 1111);
	EXPECT_EQ(vec2[0], 111);

	++vec2[1];
	EXPECT_EQ(vec[1], 222);
	EXPECT_EQ(vec2[1], 223);

	vec.PushBack(444);
	EXPECT_EQ(vec.Size(), 4);
	EXPECT_EQ(vec2.Size(), 3);
}

TEST(FixedSizeVectorTest, TimingTest)
{
	// FixedSizeVector<int, 10> vec1;
	// vec1.PushBack(1);
	// vec1.PushBack(2);
	// vec1.PushBack(3);
	// vec1.PushBack(4);
	// vec1.PushBack(5);
	// vec1.PushBack(6);
	// vec1.PushBack(7);
	// vec1.PushBack(8);
	// vec1.PushBack(9);

	// std::vector<int> vec2;
	// vec2.push_back(1);
	// vec2.push_back(2);
	// vec2.push_back(3);
	// vec2.push_back(4);
	// vec2.push_back(5);
	// vec2.push_back(6);
	// vec2.push_back(7);
	// vec2.push_back(8);
	// vec2.push_back(9);

	// constexpr int count = 1;

	// FixedSizeVector<int, 10> vec1Arr[count];
	// std::vector<int> vec2Arr[count];

	// Timestamp t1 = Time::ClockType::now();
	// for (int i = 0; i < count; ++i)
	// {
	// 	vec1Arr[i] = vec1;
	// }
	// Timestamp t2 = Time::ClockType::now();
	// for (int i = 0; i < count; ++i)
	// {
	// 	vec2Arr[i] = vec2;
	// }
	// Timestamp t3 = Time::ClockType::now();

	// printf("%" PRIu64 " %" PRIu64 "\n", (t2 - t1).count(), (t3 - t2).count());

	// FixedSizeVector<Quantity, 10> vec3;
	// printf("sizeof(FixedSizeFixedSizeVector<Quantity, 10>)=%" PRIu64 "\n", sizeof(vec3));
	// FixedSizeVector<Price, 10> vec4;
	// printf("sizeof(FixedSizeFixedSizeVector<Price, 10>)=%" PRIu64 "\n", sizeof(vec4));

	// stacker::DynamicTarget target;
	// target.qtys.PushBack(Quantity(10));
	// target.qtys.PushBack(Quantity(11));
	// target.qtys.PushBack(Quantity(12));
	// target.qtys.PushBack(Quantity(13));
	// target.qtys.PushBack(Quantity(14));
	// target.lastReduce = Time::Now();
	// target.px = Price::FromUnshifted(1234);
	// constexpr int COUNT = 1;
	// stacker::DynamicTarget targets[COUNT];

	// Timestamp t4 = Time::ClockType::now();
	// for (int i = 0; i < COUNT; ++i)
	// {
	// 	targets[i] = target;
	// }
	// Timestamp t5 = Time::ClockType::now();
	// printf("CopyTime=%" PRIu64 "\n", (t5 - t4).count());
}

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
