#pragma once

#include "stream_send.hpp"
#include "stream_recv.hpp"
#include "timeout_manager.hpp"
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
        uint32_t streamid;        // stream id
        uint32_t idx;           // index
        uint32_t tail_idx;      // tail index
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

    void transmit_group_recovery_packet(int streamid, int seq);

    // 再送
    void retransmit_packets(int streamid);

    // 受信パケットの処理
    void process_arriving_packet();

    void background();

    void register_epoll_event(int fd);

    int sockfd;
    int epollfd;
    std::map<int, timeout_manager> toms; // {streamid, timeout_manager}
    std::atomic_flag flag; // lock: terminated, unlock: started
    std::thread bgthread;
    int reserved_ssid;
    std::map<int, stream_send> ss; // {streamid, stream}
    std::map<int, stream_send>::iterator ss_itr;
    std::map<int, stream_recv> sr; // {streamid : stream}
    std::map<int, stream_recv>::iterator sr_itr;
    std::set<endpoint> group;
};

} // namespace gcom
