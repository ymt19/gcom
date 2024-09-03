#pragma once

#include <cstdint>
#include <thread>
#include "ring_buffer.hpp"

namespace multicast
{

struct header
{
    uint64_t seq;
    uint64_t first;
    uint64_t last;
    uint64_t len;
    uint8_t flg;
};

class QueueEntry
{
public:
    uint64_t idx;
};

class SenderSocket
{
private:
    int sockfd_;
    int sigfd_;
    std::thread bg_;
    int genseq_;
    RingBuffer buff_;
public:
    SenderSocket()
    {
        genseq_ = 0;
    }

    ~SenderSocket()
    {

    }

    void send_to()
    {

    }
};

class ReceiverSocket
{
private:
    int sockfd_;
    int sigfd_;
    std::thread bg_;
    RingBuffer buff_;
public:
    ReceiverSocket()
    {

    }

    ~ReceiverSocket()
    {

    }

    void receive_from()
    {

    }
};

}
