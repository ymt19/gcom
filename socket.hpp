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

namespace gcom
{

#define PACKET_SIZE     40 //1472 // max udp payload size
#define PAYLOAD_SIZE    PACKET_SIZE - sizeof(struct header)

class socket
{
public:
    socket() : sock_fd(0), signal_fd(0) {}

    void open(uint16_t port);

    void close();

    void sendto(const void *buf, size_t len, std::set<endpoint>& dest);

    ssize_t recvfrom(void *buf, endpoint& src);

    void add_to_group(endpoint& ep);
private:
    const int SIGSEND = SIGRTMIN;
    const int SIGCLOSE = SIGRTMIN+1;
    const int max_epoll_events = 16;
    const int max_streams = 5;

    struct header
    {
        uint32_t stream;    // stream id
        uint32_t seq;       // seaquence number
        uint32_t head;      // 
        uint32_t tail;
        uint8_t flag;
    };
    const int FLAG_NCK = 0x01;

    ssize_t output_packet(uint32_t seq, uint32_t begin, uint32_t end, uint8_t flag, const void *payload, size_t len, int dest_id);

    size_t input_packet(uint32_t *seq, uint32_t *begin, uint32_t *end, uint8_t *flag, void *payload);

    void output_all();

    void* background();

    int get_signalfd();

    int register_epoll_events();

    int sock_fd;
    int signal_fd;
    std::thread bgthread;
    std::map<int, send_stream> sstreams;
    std::map<int, recv_stream> rstreams;
    std::set<endpoint> group;
};

} // namespace gcom
