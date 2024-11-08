#pragma once

#include "ring_buffer.hpp"
#include "endpoint.hpp"
#include <signal.h>
#include <thread>
#include <queue>
#include <optional>
#include <mutex>

namespace multicast
{

struct header
{
    uint32_t seq;
};

#define MAX_PACKET_SIZE     1472 // max udp payload size
#define MAX_PAYLOAD_SIZE    MAX_PACKET_SIZE - sizeof(struct header)

class packet_info
{
public:
    uint64_t idx_;
    uint64_t seq_;
    uint32_t len_;

    packet_info(uint64_t idx, uint64_t seq, uint32_t len)
    {
        idx_ = idx;
        seq_ = seq;
        len_ = len;
    }

    bool operator< (const packet_info& a) const
    {
        return idx_ < a.idx_;
    }
};

class Socket
{
public:
    Socket();
    ~Socket();

    void open(uint16_t port);

    void close();

    ssize_t sendto(const void *buf, size_t len, int id);

    ssize_t recvfrom(void *buf);

    int add_node(int id, char *ipaddr, uint16_t port);
private:
    ssize_t output_packet(uint32_t seq, const void *payload, size_t len);

    size_t input_packet(uint32_t *seq, void *payload);

    void* background();

    int get_signalfd();

    int register_epoll_events();

    const int SIGSEND = SIGRTMIN;
    const int SIGCLOSE = SIGRTMIN+1;
    const int max_epoll_events = 16;

    int sockfd_;
    int signalfd_;

    std::optional<std::thread> background_th_;

    std::mutex sendbuf_mtx_;
    uint64_t generated_seq_;
    RingBuffer sendbuf_;
    std::queue<packet_info> sendbuf_info_;

    std::mutex recvbuf_mtx_;
    RingBuffer recvbuf_;
    std::priority_queue<packet_info> recvbuf_info_;

    endpoint_list dest_info_;
};

}
