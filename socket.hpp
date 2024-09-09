#pragma once

#include <boost/asio.hpp>
#include <queue>
#include "ring_buffer.hpp"

namespace multicast
{

namespace ip = boost::asio::ip;

class Socket
{
public:
    Socket(int port);
private:
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
        uint64_t seq;
        uint64_t len;
    };

    void output_packet();
    void input_packet();
    void background(); // 各処理を渡す引数

    boost::asio::io_service _io_service;
    ip::udp::socket _udp_sock;
    RingBuffer _buff;
    std::priority_queue<QueueEntry> _queue;
};

}