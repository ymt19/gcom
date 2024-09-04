#pragma once

#include <boost/asio.hpp>

namespace multicast
{

namespace ip = boost::asio::ip;

class Socket
{
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

    boost::asio::io_service ios;
    ip::udp::socket sock;
    // set<endpoint, pair<buff, queue<QueueEntry>>>
    void output_packet();
    void input_packet();
    void background(); // 各処理を渡す引数
public:
    Socket();
};

}