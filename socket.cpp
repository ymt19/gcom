#include "socket.hpp"

gcom::socket::socket(uint16_t port)
    : flag(ATOMIC_FLAG_INIT), reserved_ssid(0)
{
    struct sockaddr_in addr;

    // init sockfd
    sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        throw std::exception();
    }

    addr.sin_family = AF_INET;
    addr.sin_port = ::htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (::bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        throw std::exception();
    }

    // init epollfd
    epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        throw std::exception();
    }

    register_epoll_event(sockfd);

    // init flag
    flag.test_and_set();

    // start background thread
    bgthread = std::thread([this]{ background(); });
}

gcom::socket::~socket()
{
    flag.test_and_set();

    if (bgthread.joinable())
    {
        bgthread.join();
    }

    ::close(epollfd);
    ::close(sockfd);
}

void gcom::socket::sendto(const void *data, size_t len, endpoint& dest)
{
    if (ss.size() < max_streams)
    {
        reserved_ssid++;
        // create new stream
        {
            auto ret = ss.emplace(reserved_ssid, stream(buffer_size));

            if (!ret.second)
            {
                throw std::exception();
            }

            ss_itr = ret.first;
        }

        // create new timerfd
        {
            auto ret = toms.emplace(reserved_ssid, timeout_manager());

            if (!ret.second)
            {
                throw std::exception();
            }

            register_epoll_event(ret.first->second.get_fd());
        }
    }
    else
    {
        ss_itr++;
        if (ss_itr == ss.end())
        {
            ss_itr = ss.begin();
        }
    }

    ss_itr->second.push_packets((unsigned char *)data, len, payload_size);
}

ssize_t gcom::socket::recvfrom(void *buf, endpoint& from)
{
    uint32_t len = 0;

    if (sr_itr == nullptr)
    {

    }

    // recv stream.get()
    do
    {
        rs_itr++;
        if (rs_itr == rs.end())
        {
            rs_itr = rs.begin();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } while ((len =  rs_itr->second.pop_packets((unsigned char*)buf)) == 0);

    return len;
}

void gcom::socket::add_group(endpoint& ep)
{
    group.insert(ep);
}

ssize_t gcom::socket::output_packet(const struct header* hdr, const void *payload, size_t payload_len, endpoint& dest)
{
    struct sockaddr_in addr;
    unsigned char buf[packet_size];
    uint32_t buf_len = 0, len;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(dest.ipaddr.c_str());
    addr.sin_port = htons(dest.port);

    memcpy(buf, hdr, sizeof(struct header));
    buf_len += sizeof(struct header);
    memcpy(buf + sizeof(struct header), payload, payload_len);
    buf_len += payload_len;

    len = ::sendto(sockfd, buf, buf_len, 0, (struct sockaddr *)&(addr), sizeof(addr));
    if (len != buf_len)
    {
        throw std::exception();
    }

    fprintf(stderr, "output_packet(): len:%ld, payload:%ld\n", len, payload_len);

    return payload_len;
}

size_t gcom::socket::input_packet(struct header* hdr, void *payload)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    unsigned char buf[packet_size];
    ssize_t len, payload_len;

    len = ::recvfrom(sockfd, buf, packet_size, 0, (struct sockaddr *)&addr, &addr_len);
    if (len == -1) // error
    {
        throw std::exception();
    }
    else if (len < sizeof(struct header))
    {
        throw std::exception();
    }

    memcpy(hdr, buf, sizeof(struct header));
    payload_len = len - sizeof(struct header);
    memcpy(payload, buf + sizeof(struct header), payload_len);

    fprintf(stderr, "input_packet(): len:%ld, payload:%ld\n", len, payload_len);

    return payload_len;
}

void gcom::socket::transmit_ready_packets()
{
    // while stream.get_packte()
    // output_packet()
}

void gcom::socket::transmit_ack_packet()
{
    // output_packet()
}

void gcom::socket::transmit_nack_packet()
{
    // output_packet()
}

void gcom::socket::transmit_group_recovery_packet(int streamid, int seq)
{
    // stream.get_packet()
    // output_packet()
}

void gcom::socket::retransmit_packets(int streamid)
{
    // stream.get_packet()
    // output_packet()
}

void gcom::socket::process_arriving_packet()
{
    ssize_t len;
    unsigned char payload[payload_size];
    struct header hdr;
    len = input_packet(&hdr, payload);
    if () // recv data packet
    {
        // 必要ならstream.push_empty()
        // stream.insert()

        if () // this_o + len == tail_o
        {
            transmit_ack_packet();
        }
        else if () // if prev.seq + 1 != seq
        {
            transmit_nack_packet();
        }
    }
    else if () // recv nack packet
    {
        transmit_group_recovery_packet();
    }
    else if () // recv ack packet
    {
        ss.at(hdr.streamid).pop_packets(hdr.idx);
    }
}

void gcom::socket::background()
{
    struct epoll_event events[max_epoll_events];
    int epollfd, nfds;
    unsigned char payload[payload_size];
    ssize_t len;

    while (!flag.test())
    {
        nfds = epoll_wait(epollfd, events, max_epoll_events, epoll_wait_timeout);
        if (nfds > 0)
        {
            for (int i = 0; i < nfds; i++)
            {
                if (events[i].data.fd == sockfd)
                {
                    process_arriving_packet();
                }
                else
                {
                    for (auto itr = toms.begin(); itr != toms.end(); itr++)
                    {
                        if (events[i].data.fd == itr->second.get_fd())
                        {
                            retransmit_packets(itr->first);
                            break;
                        }
                    }
                }
            }
        }

        transmit_ready_packets();
    }
}

void gcom::socket::register_epoll_event(int fd)
{
    struct epoll_event ev;
    
    // Register sockfd_ in epollfd.
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        throw std::exception();
    }
}
