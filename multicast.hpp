#pragma once

#include "ring_buffer.hpp"
#include "tree.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <queue>

namespace multicast
{

class Socket
{
public:
    Socket(uint16_t port);
    ~Socket();

    void send(std::string msg, boost::asio::ip::udp::endpoint dest);

    void recv();
private:
    void output_packet();

    void input_packet();

    void background();

    struct header
    {
        uint64_t seq;
        uint64_t first;
        uint64_t last;
        uint64_t len;
        uint8_t flg;
    };

    struct queue_entry
    {
    public:
        uint64_t idx;
        uint64_t seq;
        uint64_t len;
    };

    // uint16_t port_;
    // boost::asio::ip::udp::endpoint endpoint_;
    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket udp_sock_;
    // Tree tree_;
    RingBuffer recv_buff_;
    // std::priority_queue<queue_entry> queue_;
    uint64_t generated_seq_;
    std::thread bgth_;
};

}
