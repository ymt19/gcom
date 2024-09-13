#pragma once

#include "ring_buffer.hpp"
#include "tree.hpp"
#include <thread>
#include <queue>

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

    int sockfd_;
    int signalfd_;
    std::thread background_th_;
    // Tree tree_;
    uint64_t generated_seq_;
    RingBuffer send_buff_;
    // std::priority_queue<queue_entry> send_buff_info_;
    RingBuffer recv_buff_; // vectorにして複数senderに対応できる
    // std::priority_queue<queue_entry> recv_buff_info_;
};

}
