#pragma once

#include "ring_buffer.hpp"
#include "endpoint.hpp"
#include <signal.h>
#include <thread>
#include <queue>
#include <optional>
#include <mutex>
#include <map>

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
    // int src_id_;    // send bufの場合-1
    int dest_id_;   // recv bufの場合-1

    packet_info(uint64_t idx, uint64_t seq, uint32_t len, int dest_id)
    {
        idx_ = idx;
        seq_ = seq;
        len_ = len;
        // src_id_ = src_id;
        dest_id_ = dest_id;
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

    /**
     * @brief 
     * 
     * @param buf 送信データ
     * @param len 送信データ長
     * @param dest_id 送信先ID，0の場合登録されているすべてのノードに送信
     */
    void sendto(const void *buf, size_t len, int dest_id);

    ssize_t recvfrom(void *buf);

    void add_endpoint(int id, char *ipaddr, uint16_t port, bool is_same_group);
private:
    ssize_t output_packet(uint32_t seq, const void *payload, size_t len, int dest_id);

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

    std::map<int, endpoint> endpoint_list_; // id : endpoint info, id >= 1
};

}
