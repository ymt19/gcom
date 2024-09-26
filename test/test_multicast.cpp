#include <gtest/gtest.h>

#include "../multicast.hpp"

class TestSocket : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        socket = new multicast::Socket();
    }

    virtual void TearDown()
    {
        delete socket;
    }

    multicast::Socket *socket;
}