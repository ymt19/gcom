#include <gtest/gtest.h>

#include "../ring_buffer.hpp"

class TestRingBuffer : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        buff = multicast::RingBuffer();
    }

    multicast::RingBuffer buff;
};

TEST_F(TestRingBuffer, PushCase1)
{
    std::string str1(10, 'a');
    std::string str2(20, 'b');
    EXPECT_EQ(10, buff.push(str1));
    EXPECT_EQ(30, buff.push(str2));
}

TEST_F(TestRingBuffer, PushCase2)
{
    std::string str1(5000, 'a');
    EXPECT_THROW(buff.push(str1), std::exception);
}

TEST_F(TestRingBuffer, PushEmptyCase1)
{
    EXPECT_THROW(buff.push_empty(5000), std::exception);
    EXPECT_EQ(10, buff.push_empty(10));
}

TEST_F(TestRingBuffer, PopCase1)
{
    std::string str1(10, 'a');
    std::string str2(20, 'b');
    EXPECT_EQ(10, buff.push(str1));
    EXPECT_EQ(str1, buff.pop(10));
    EXPECT_EQ(30, buff.push(str2));
    EXPECT_EQ(str2, buff.pop(20));
}

TEST_F(TestRingBuffer, SetCase1)
{
    std::string str1(10, 'a');
    ASSERT_THROW(buff.set(0, str1), std::exception);
}

TEST_F(TestRingBuffer, SetGetCase1)
{
    std::string str1(10, 'a');
    std::string str2(20, 'b');
    EXPECT_EQ(30, buff.push_empty(30));
    ASSERT_NO_THROW(buff.set(0, str1));
    EXPECT_EQ(str1, buff.get(0, 10));
    ASSERT_NO_THROW(buff.set(10, str2));
    EXPECT_EQ(str2, buff.get(10, 20));
    EXPECT_EQ(str1 + str2, buff.pop(30));
}