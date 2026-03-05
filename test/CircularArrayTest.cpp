#include <gtest/gtest.h>

#include <akl/containers/CircularArray.hpp>

namespace akl::test
{

TEST(CircularArrayTest, Empty)
{
	CircularArray<int, 5> ca;
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
	EXPECT_EQ(ca.Capacity(), 5);
	EXPECT_FALSE(ca.PopFront());
	EXPECT_FALSE(ca.PopBack());
}

TEST(CircularArrayTest, PushFrontPopFront)
{
	CircularArray<int, 5> ca(0);

	EXPECT_TRUE(ca.PushFront(111));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushFront(222));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushFront(333));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 333);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca.Front(), 333);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushFront(444));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 444);
	EXPECT_EQ(ca[1], 333);
	EXPECT_EQ(ca[2], 222);
	EXPECT_EQ(ca[3], 111);
	EXPECT_EQ(ca.Front(), 444);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushFront(555));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 222);
	EXPECT_EQ(ca[4], 111);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_FALSE(ca.PushFront(666));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 222);
	EXPECT_EQ(ca[4], 111);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 444);
	EXPECT_EQ(ca[1], 333);
	EXPECT_EQ(ca[2], 222);
	EXPECT_EQ(ca[3], 111);
	EXPECT_EQ(ca.Front(), 444);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 333);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca.Front(), 333);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);

	EXPECT_FALSE(ca.PopFront());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
}

TEST(CircularArrayTest, PushBackPopBack)
{
	CircularArray<int, 5> ca(0);

	EXPECT_TRUE(ca.PushBack(111));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushBack(222));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 222);

	EXPECT_TRUE(ca.PushBack(333));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.PushBack(444));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 444);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 444);

	EXPECT_TRUE(ca.PushBack(555));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 444);
	EXPECT_EQ(ca[4], 555);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_FALSE(ca.PushBack(666));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 444);
	EXPECT_EQ(ca[4], 555);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 444);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 444);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 222);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);

	EXPECT_FALSE(ca.PopBack());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
}

TEST(CircularArrayTest, PushFrontPopBack)
{
	CircularArray<int, 5> ca(0);

	EXPECT_TRUE(ca.PushFront(111));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushFront(222));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushFront(333));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 333);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca.Front(), 333);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushFront(444));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 444);
	EXPECT_EQ(ca[1], 333);
	EXPECT_EQ(ca[2], 222);
	EXPECT_EQ(ca[3], 111);
	EXPECT_EQ(ca.Front(), 444);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushFront(555));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 222);
	EXPECT_EQ(ca[4], 111);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_FALSE(ca.PushFront(666));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 222);
	EXPECT_EQ(ca[4], 111);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 222);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 222);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 444);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);

	EXPECT_FALSE(ca.PopBack());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
}

TEST(CircularArrayTest, PushBackPopFront)
{
	CircularArray<int, 5> ca(0);

	EXPECT_TRUE(ca.PushBack(111));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushBack(222));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 222);

	EXPECT_TRUE(ca.PushBack(333));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.PushBack(444));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 444);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 444);

	EXPECT_TRUE(ca.PushBack(555));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 444);
	EXPECT_EQ(ca[4], 555);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_FALSE(ca.PushBack(666));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 444);
	EXPECT_EQ(ca[4], 555);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 333);
	EXPECT_EQ(ca[2], 444);
	EXPECT_EQ(ca[3], 555);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 333);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca[2], 555);
	EXPECT_EQ(ca.Front(), 333);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 444);
	EXPECT_EQ(ca[1], 555);
	EXPECT_EQ(ca.Front(), 444);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);

	EXPECT_FALSE(ca.PopFront());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
}

TEST(CircularArrayTest, PushPopBothDirections)
{
	CircularArray<int, 5> ca(0);

	EXPECT_TRUE(ca.PushBack(111));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushFront(222));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushBack(333));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.PushFront(444));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 444);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca[3], 333);
	EXPECT_EQ(ca.Front(), 444);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.PushBack(555));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 444);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca[3], 333);
	EXPECT_EQ(ca[4], 555);
	EXPECT_EQ(ca.Front(), 444);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_FALSE(ca.PushFront(666));
	EXPECT_FALSE(ca.PushBack(777));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 444);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca[3], 333);
	EXPECT_EQ(ca[4], 555);
	EXPECT_EQ(ca.Front(), 444);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 555);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 333);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
	
	EXPECT_FALSE(ca.PopBack());
	EXPECT_FALSE(ca.PopFront());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
}

TEST(CircularArrayTest, PushPopBothDirections2)
{
	CircularArray<int, 5> ca(0);

	EXPECT_TRUE(ca.PushFront(111));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PushBack(222));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 222);

	EXPECT_TRUE(ca.PushFront(333));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 333);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca[2], 222);
	EXPECT_EQ(ca.Front(), 333);
	EXPECT_EQ(ca.Back(), 222);

	EXPECT_TRUE(ca.PushBack(444));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 333);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca[2], 222);
	EXPECT_EQ(ca[3], 444);
	EXPECT_EQ(ca.Front(), 333);
	EXPECT_EQ(ca.Back(), 444);

	EXPECT_TRUE(ca.PushFront(555));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca[1], 333);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca[3], 222);
	EXPECT_EQ(ca[4], 444);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 444);

	EXPECT_FALSE(ca.PushFront(666));
	EXPECT_FALSE(ca.PushBack(777));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca[1], 333);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca[3], 222);
	EXPECT_EQ(ca[4], 444);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 444);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca[1], 333);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca[3], 222);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 222);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 333);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca[2], 222);
	EXPECT_EQ(ca.Front(), 333);
	EXPECT_EQ(ca.Back(), 222);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 333);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca.Front(), 333);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PopFront());
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.PopBack());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
	
	EXPECT_FALSE(ca.PopBack());
	EXPECT_FALSE(ca.PopFront());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
}

TEST(CircularArrayTest, Insert)
{
	CircularArray<int, 5> ca(0);

	EXPECT_FALSE(ca.InsertAt(1, 111));
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);

	EXPECT_TRUE(ca.InsertAt(0, 111));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca.Front(), 111);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.InsertAt(0, 222));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 111);

	EXPECT_TRUE(ca.InsertAt(2, 333));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 111);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.InsertAt(1, 444));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca[3], 333);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.InsertAt(3, 555));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca[2], 111);
	EXPECT_EQ(ca[3], 555);
	EXPECT_EQ(ca[4], 333);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.RemoveAt(2));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 4);
	EXPECT_EQ(ca[0], 222);
	EXPECT_EQ(ca[1], 444);
	EXPECT_EQ(ca[2], 555);
	EXPECT_EQ(ca[3], 333);
	EXPECT_EQ(ca.Front(), 222);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.RemoveAt(0));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 444);
	EXPECT_EQ(ca[1], 555);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca.Front(), 444);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_FALSE(ca.RemoveAt(3));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 3);
	EXPECT_EQ(ca[0], 444);
	EXPECT_EQ(ca[1], 555);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca.Front(), 444);
	EXPECT_EQ(ca.Back(), 333);

	EXPECT_TRUE(ca.RemoveAt(2));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 2);
	EXPECT_EQ(ca[0], 444);
	EXPECT_EQ(ca[1], 555);
	EXPECT_EQ(ca.Front(), 444);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.RemoveAt(0));
	EXPECT_FALSE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 1);
	EXPECT_EQ(ca[0], 555);
	EXPECT_EQ(ca.Front(), 555);
	EXPECT_EQ(ca.Back(), 555);

	EXPECT_TRUE(ca.RemoveAt(0));
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);

	EXPECT_FALSE(ca.RemoveAt(0));
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
}

TEST(CircularArrayTest, Clear)
{
	CircularArray<int, 5> ca(0);

	EXPECT_TRUE(ca.PushBack(111));
	EXPECT_TRUE(ca.PushBack(222));
	EXPECT_TRUE(ca.PushBack(333));
	EXPECT_TRUE(ca.PushBack(444));
	EXPECT_TRUE(ca.PushBack(555));
	EXPECT_FALSE(ca.Empty());
	EXPECT_TRUE(ca.Full());
	EXPECT_EQ(ca.Size(), 5);
	EXPECT_EQ(ca[0], 111);
	EXPECT_EQ(ca[1], 222);
	EXPECT_EQ(ca[2], 333);
	EXPECT_EQ(ca[3], 444);
	EXPECT_EQ(ca[4], 555);

	ca.Clear();
	EXPECT_FALSE(ca.PopFront());
	EXPECT_FALSE(ca.PopBack());
	EXPECT_TRUE(ca.Empty());
	EXPECT_FALSE(ca.Full());
	EXPECT_EQ(ca.Size(), 0);
}

}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}