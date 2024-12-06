#include <gtest/gtest.h>

#include "../socket.hpp"

class TestSocket : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        sender = new gcom::socket(10000);
    }

    virtual void TearDown()
    {
        delete sender;
    }

    gcom::socket *sender;
};