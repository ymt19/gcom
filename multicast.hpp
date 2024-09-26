#pragma once

#include "ring_buffer.hpp"
#include "tree.hpp"
#include <thread>
#include <queue>
#include <optional>

namespace multicast
{

class Socket
{
public:
    Socket();
    Socket(uint64_t send_buff_size, uint64_t recv_buff_size);
    ~Socket();

    void open(uint16_t port);

    void close();

    void add_node();

    void send();

    void recv();
private:
    void output_packet();

    void input_packet();

    void* background();

    void get_signalfd();

    int register_epoll_events();

    const int SIGSEND = SIGRTMIN;
    const int SIGCLOSE = SIGRTMIN+1;
    const int max_epoll_events = 16;

    struct queue_entry
    {
        uint64_t idx;
        uint64_t seq;
        uint64_t first;
        uint64_t last;
        Endpoint src;
    };

    int sockfd_;
    int signalfd_;
    std::optional<std::thread> background_th_;
    Tree tree_;
    uint64_t generated_seq_;
    RingBuffer send_buff_;
    std::priority_queue<struct queue_entry> send_buff_info_;
    RingBuffer recv_buff_;
    std::priority_queue<struct queue_entry> recv_buff_info_;
};

class Packet
{
public:
    int type;
private:
    struct header
    {
        uint32_t seq;
        uint32_t first;
        uint32_t last;
        uint8_t flag;
    };

    struct header hdr;
};

}
