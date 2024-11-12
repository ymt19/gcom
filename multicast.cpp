#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include <iostream>
#include "multicast.hpp"

namespace multicast
{

Socket::Socket() 
{
    sock_fd = -1;
    signal_fd = -1;
    generated_seq = 0;
}

Socket::~Socket() {
    // close();
}

void Socket::open(uint16_t port)
{
    std::cerr << "open" << std::endl;
    std::cout << "MAX_PACKET_SIZE " << MAX_PACKET_SIZE << std::endl;
    std::cout << "MAX_PAYLOAD_SIZE " << MAX_PAYLOAD_SIZE << std::endl;
    struct sockaddr_in addr;

    // init sockfd_
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1)
    {
        throw std::exception();
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        throw std::exception();
    }

    // init signalfd_
    try
    {
        signal_fd = get_signalfd();
    }
    catch(const std::exception& e)
    {
        throw std::exception();
    }

    // start background thread
    background_thread.emplace(&Socket::background, this);
}

void Socket::close()
{
    kill(getpid(), SIGCLOSE);
    background_thread->join();
    ::close(sock_fd);
}

void Socket::sendto(const void *data, size_t len, int dest_id)
{
    uint64_t idx;
    uint32_t begin, end;
    uint32_t payload_len, offset = 0;

    printf("sendto(): data:%s, len:%ld, dest_id:%d\n", (char *)data, len, dest_id);

    sendbuf_mtx.lock();

    begin = generated_seq + 1;
    end = generated_seq + (len - 1) / (uint32_t)(MAX_PAYLOAD_SIZE) + 1;
    for (int seq = begin; seq <= end; seq++)
    {
        payload_len = std::min(len - offset, MAX_PAYLOAD_SIZE);
        idx = sendbuf.push((unsigned char *)data + offset, payload_len);
        sendbuf_info.push(queue_entry(idx, payload_len, seq, begin, end, -1, dest_id));
        offset += payload_len;
    }
    generated_seq = end;

    sendbuf_mtx.unlock();

    kill(getpid(), SIGSEND);
}

ssize_t Socket::recvfrom(void *data)
{
    uint32_t next, head, tail, len = 0;

    while (true)
    {
        recvbuf_mtx.lock();
        if (!recvbuf_info.empty())
        {
            const queue_entry& en = recvbuf_info.top();
            std::cout << en.head << " " << en.seq << std::endl;
            if (len == 0 && en.head == en.seq)
            {
                head = en.head;
                next = head+1;
                tail = en.tail;
                len += en.payload_len;
                recvbuf_info.pop();
                recvbuf_mtx.unlock();
                break;
            }
        }
        recvbuf_mtx.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    while (next <= tail)
    {
        recvbuf_mtx.lock();
        if (!recvbuf_info.empty())
        {
            const queue_entry& en = recvbuf_info.top();
            std::cout << en.head << " " << en.seq << " " << next << std::endl;
            if (en.seq == next)
            {
                next++;
                len += en.payload_len;
                recvbuf_info.pop();
            }
        }
        recvbuf_mtx.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    // recv bufferから取得
    recvbuf_mtx.lock();
    recvbuf.pop((unsigned char *)data, len);
    recvbuf_mtx.unlock();

    return len;
}

void Socket::add_endpoint(int id, char *ipaddr, uint16_t port, bool is_same_group)
{
    endpoint_list.insert(std::make_pair(id, endpoint(ipaddr, port, is_same_group)));
}

ssize_t Socket::output_packet(uint32_t seq, uint32_t begin, uint32_t end, const void *payload, size_t len, int dest_id)
{
    unsigned char buf[MAX_PACKET_SIZE] = {};
    struct header *hdr;
    uint32_t total;

    hdr = (struct header *)buf;
    hdr->seq = seq;
    hdr->head = begin;
    hdr->tail = end;
    std::memcpy(hdr + 1, payload, len);
    total = sizeof(struct header) + len;

    // - std::map endpoint_list_の参照は参照ではなくていいのか？
    endpoint dest = endpoint_list.at(dest_id);
    len = ::sendto(sock_fd, buf, total, 0, (struct sockaddr *)&(dest.addr_), sizeof(dest.addr_));

    printf("output_packet(): len:%ld, seq:%d, begin:%d, end:%d, payload:%s dest:%d\n", len, seq, begin, end, (char *)payload, dest_id);

    return len; // -1の場合sendto()error
}

size_t Socket::input_packet(uint32_t *seq, uint32_t *begin, uint32_t *end, void *payload)
{
    unsigned char buf[MAX_PACKET_SIZE] = {};
    ssize_t buf_len, payload_len;
    struct header *hdr;
    struct sockaddr_in src;
    socklen_t srclen = sizeof(src);

    buf_len = ::recvfrom(sock_fd, buf, MAX_PACKET_SIZE, 0, (struct sockaddr *)&src, &srclen);
    payload_len = buf_len - sizeof(struct header);
    hdr = (struct header *)buf;
    *seq = hdr->seq;
    *begin = hdr->head;
    *end = hdr->tail;
    std::memcpy(payload, buf + sizeof(struct header), payload_len);

    printf("input_packet(): len:%ld, seq:%d, begin:%d, end:%d, payload:%s\n", payload_len, *seq, *begin, *end, (char *)payload);

    return payload_len;
}

void *Socket::background()
{
    struct epoll_event events[max_epoll_events];
    int epollfd, nfds;
    struct signalfd_siginfo fdsi;
    unsigned char payload[MAX_PAYLOAD_SIZE];
    ssize_t len;
    uint64_t idx;

    epollfd = register_epoll_events();

    for (;;)
    {
        nfds = epoll_wait(epollfd, events, max_epoll_events, -1);
        if (nfds == -1)
        {
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; ++i)
        {
/**************************** multicast algorithm ********************************/
            if (events[i].data.fd == sock_fd)
            {
                struct header hdr;
                std::memset(payload, '\0', MAX_PAYLOAD_SIZE);
                len = input_packet(&hdr.seq, &hdr.head, &hdr.tail, payload);

                // if (packet.type == NCK)
                // {
                // }
                // else if (packet.type == DAT)
                // {
                // }

                recvbuf_mtx.lock();
                idx = recvbuf.push(payload, len);
                recvbuf_info.push(queue_entry(idx, len, hdr.seq, hdr.head, hdr.tail, -1, -1)); // src_id_を特定する
                recvbuf_mtx.unlock();
            }
            else if (events[i].data.fd == signal_fd)
            {
                len = read(signal_fd, &fdsi, sizeof(struct signalfd_siginfo));
                if (len != sizeof(struct signalfd_siginfo))
                {
                    // error
                    continue;
                }

                if (fdsi.ssi_signo == SIGSEND)
                {
                    std::cerr << "SIGSEND" << std::endl;

                    sendbuf_mtx.lock();
                    while (!sendbuf_info.empty())
                    {
                        std::memset(payload, '\0', MAX_PAYLOAD_SIZE);
                        queue_entry& en = sendbuf_info.front();
                        sendbuf.get(en.idx, payload, en.payload_len);
                        if (en.dest_id == 0) // 全ノードに送信
                        {
                            for (auto dest = endpoint_list.begin(); dest != endpoint_list.end(); ++dest)
                            {
                                len = output_packet(en.seq, en.head, en.tail, payload, en.payload_len, dest->first);
                            }
                        }
                        else // 指定ノードに送信
                        {
                            len = output_packet(en.seq, en.head, en.tail, payload, en.payload_len, en.dest_id);
                        }
                        sendbuf_info.pop();
                    }
                    sendbuf_mtx.unlock();
                    continue;
                }
                else if (fdsi.ssi_signo == SIGCLOSE)
                {
                    std::cerr << "SIGCLOSE" << std::endl;
                    return nullptr;
                }
                else
                {
                    fprintf(stderr, "read unexpected signal.\n");
                    continue;
                }
            }
/*********************************************************************************/
        }
    }
}

int Socket::get_signalfd()
{
    sigset_t mask;
    int fd;
    sigemptyset(&mask);
    sigaddset(&mask, SIGSEND);
    sigaddset(&mask, SIGCLOSE);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    fd = signalfd(-1, &mask, SFD_CLOEXEC);
    if (fd == -1)
    {
        throw std::exception();
    }
    return fd;
}

int Socket::register_epoll_events()
{
    int epollfd;
    struct epoll_event ev;

    epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        exit(EXIT_FAILURE);
    }
    
    // Register sockfd_ in epollfd.
    ev.events = EPOLLIN;
    ev.data.fd = sock_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_fd, &ev) == -1)
    {
        exit(EXIT_FAILURE);
    }

    // Register signalfd_ in epollfd.
    ev.events = EPOLLIN;
    ev.data.fd = signal_fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, signal_fd, &ev) == -1)
    {
        exit(EXIT_FAILURE);
    }

    return epollfd;
}

} // namespace multicast