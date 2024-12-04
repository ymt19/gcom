#pragma once

#include "ring_buffer.hpp"
#include "endpoint.hpp"
#include <signal.h>
#include <thread>
#include <queue>
#include <optional>
#include <mutex>
#include <map>

#define FLAG_NCK 0x01

struct header
{
    uint32_t seq;
    uint32_t head;
    uint32_t tail;
    uint8_t flag;
};

#define MAX_PACKET_SIZE     40 //1472 // max udp payload size
#define MAX_PAYLOAD_SIZE    MAX_PACKET_SIZE - sizeof(struct header)

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
    const int SIGSEND = SIGRTMIN;
    const int SIGCLOSE = SIGRTMIN+1;
    const int max_epoll_events = 16;

    class queue_entry
    {
    public:
        uint64_t idx;
        uint32_t payload_len;
        uint32_t seq;
        uint32_t head;
        uint32_t tail;
        int src_id;    // send bufの場合-1
        int dest_id;   // recv bufの場合-1

        queue_entry(uint64_t _idx, uint32_t _payload_len, uint64_t _seq, uint32_t _head, uint32_t _tail, int _src_id, int _dest_id)
        {
            idx = _idx;
            payload_len = _payload_len;
            seq = _seq;
            head = _head;
            tail = _tail;
            src_id = _src_id;
            dest_id = _dest_id;
        }

        bool operator< (const queue_entry& a) const { return idx < a.idx; }
        bool operator> (const queue_entry& a) const { return idx > a.idx; }
    };

    ssize_t output_packet(uint32_t seq, uint32_t begin, uint32_t end, uint8_t flag, const void *payload, size_t len, int dest_id);

    size_t input_packet(uint32_t *seq, uint32_t *begin, uint32_t *end, uint8_t *flag, void *payload);

    void output_all();

    void* background();

    int get_signalfd();

    int register_epoll_events();

    int sock_fd;
    int signal_fd;

    std::optional<std::thread> background_thread;

    std::mutex sendbuf_mtx;
    uint64_t generated_seq;
    ring_buffer sendbuf;
    std::queue<queue_entry> sendbuf_info;

    std::mutex recvbuf_mtx;
    ring_buffer recvbuf;
    std::priority_queue<queue_entry, std::vector<queue_entry>, std::greater<queue_entry>> recvbuf_info;


    std::map<int, endpoint> endpoint_list;
};
