#include "socket.hpp"

gcom::socket::socket(uint16_t port) : flag(ATOMIC_FLAG_INIT)
{
    std::cerr << "open" << std::endl;
    std::cerr << "PACKET_SIZE " << packet_size << std::endl;
    std::cerr << "PAYLOAD_SIZE " << payload_size << std::endl;
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
    if (sstreamset.size() < max_streams)
    {
        int id;
        auto ret = sstreamset.insert(std::make_pair(id, send_stream(dest, buffer_size)));
        sstreamset_itr = ret.first;
    }

    if (sstreamset_itr == nullptr)
    {
        sstreamset_itr = sstreamset.begin();
    }

    sstreamset_itr->second.push();
}

ssize_t gcom::socket::recvfrom(void *buf, endpoint& from)
{
    uint32_t len = 0;

    // recv stream.get()
    while (len != 0)
    {
        // rstream_set_itrを回す
        //  // 見つけたらrstreamset_itr->get()
    }

    // recv_stream.pop()

    return len;
}

void gcom::socket::add_group(endpoint& ep)
{
    group.insert(ep);
}

ssize_t gcom::socket::output_packet(uint32_t seq, uint32_t begin, uint32_t end, uint8_t flag, const void *payload, size_t len, endpoint& dest)
{
    struct sockaddr_in addr;
    unsigned char buf[packet_size];
    struct header *hdr;
    uint32_t total;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(dest.ipaddr.c_str());
    addr.sin_port = htons(dest.port);

    hdr = (struct header *)buf;
    hdr->seq = seq;
    hdr->head = begin;
    hdr->tail = end;
    hdr->flag = flag;
    std::memcpy(hdr + 1, payload, len);
    total = sizeof(struct header) + len;

    len = ::sendto(sockfd, buf, total, 0, (struct sockaddr *)&(addr), sizeof(addr));
    if (len < 0)
    {
        throw std::exception();
    }

    fprintf(stderr, "output_packet(): len:%ld, seq:%d, begin:%d, end:%d, payload:%s ipaddr:%s, port:%d\n", len, seq, begin, end, (char *)payload, dest.ipaddr, dest.port);

    return len;
}

size_t gcom::socket::input_packet(uint32_t *seq, uint32_t *begin, uint32_t *end, uint8_t *flag, void *payload)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    unsigned char buf[packet_size];
    struct header *hdr;
    ssize_t buf_len, payload_len;

    buf_len = ::recvfrom(sockfd, buf, packet_size, 0, (struct sockaddr *)&addr, &addr_len);
    payload_len = buf_len - sizeof(struct header);
    hdr = (struct header *)buf;
    *seq = hdr->seq;
    *begin = hdr->head;
    *end = hdr->tail;
    *flag = hdr->flag;
    std::memcpy(payload, buf + sizeof(struct header), payload_len);

    fprintf(stderr, "input_packet(): len:%ld, seq:%d, begin:%d, end:%d, payload:%s\n", payload_len, *seq, *begin, *end, (char *)payload);

    return payload_len;
}

void gcom::socket::transmit_ready_packets()
{
    // stream
}

void gcom::socket::retransmit_packets(int streamid)
{
    // 
}

void gcom::socket::process_arriving_packet()
{
    ssize_t len;
    len = input_packet();
    if () // data packet
    {

    }
    else if () // nack packet
    {

    }
    else if () // ack packet
    {

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
                    // data.fd == timer_fd;
                    for (;;)
                    {
                        retransmit_packets();
                        continue;
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
