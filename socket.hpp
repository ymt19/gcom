#pragma once

#include "stream.hpp"
#include "endpoint.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <queue>
#include <map>
#include <set>
#include <exception>
#include <iostream>
#include <atomic>

namespace gcom
{

class socket
{
public:
    socket(uint16_t port);
    ~socket();

    void sendto(const void *buf, size_t len, endpoint& dest);

    ssize_t recvfrom(void *buf, endpoint& src);

    void add_group(endpoint& ep);
private:
    static const int max_epoll_events = 16;
    static const int epoll_wait_timeout = 1;   // [ms]
    static const int max_streams = 5;
    static const int buffer_size = 8192; // [bytes]
    static const int packet_size = 1472;
    const int payload_size = packet_size - sizeof(struct header);

    struct header
    {
        uint32_t streamid;       // stream id
        uint32_t this_seqid;     // seaquence number
        uint32_t head_seqid;     // 
        uint32_t tail_seqid;
        uint8_t flag;
    };
    const int FLAG_NCK = 0x01;

    // パケットを送信
    ssize_t output_packet(const struct header* hdr, const void *payload, size_t len, endpoint& dest);

    // パケットを受信
    size_t input_packet(struct header* hdr, void *payload);

    // 送信バッファ内のデータを送信
    void transmit_ready_packets();

    void transmit_ack_packet();

    void transmit_nack_packet();

    // 再送
    void retransmit_packets(int streamid);

    // 受信パケットの処理
    void process_arriving_packet();

    void background();

    void register_epoll_event(int fd);

    int sockfd;
    int epollfd;
    int timerfd;
    std::atomic_flag flag; // lock: terminated, unlock: started
    std::thread bgthread;
    std::map<int, stream> sstreams; // streamid : send_stream
    std::map<int, stream>::iterator sstreams_itr;
    std::map<int, stream> rstreams; // streamid : recv_stream
    std::map<int, stream>::iterator rstreams_itr;
    std::set<endpoint> group;
};

} // namespace gcom
