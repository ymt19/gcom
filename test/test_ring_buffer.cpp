#include <gtest/gtest.h>

#include "ring_buffer.hpp"

class TestRingBuffer : public ::testing::Test
{
protected:
    virtual void SetUp()
    {

    }

    multicast::RingBuffer buff;
};

TEST_F(TestRingBuffer, TEST1)
{

}