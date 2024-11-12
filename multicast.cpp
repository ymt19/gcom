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
    sockfd_ = -1;
    signalfd_ = -1;
    generated_seq_ = 0;
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
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ == -1)
    {
        throw std::exception();
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd_, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        throw std::exception();
    }

    // init signalfd_
    try
    {
        signalfd_ = get_signalfd();
    }
    catch(const std::exception& e)
    {
        throw std::exception();
    }

    // start background thread
    background_th_.emplace(&Socket::background, this);
}

void Socket::close()
{
    kill(getpid(), SIGCLOSE);
    background_th_->join();
    ::close(sockfd_);
}

void Socket::sendto(const void *data, size_t len, int dest_id)
{
    uint64_t idx;
    uint32_t begin, end;
    uint32_t payload_len, offset = 0;

    printf("sendto(): data:%s, len:%ld, dest_id:%d\n", (char *)data, len, dest_id);

    sendbuf_mtx_.lock();

    begin = generated_seq_ + 1;
    end = generated_seq_ + (len - 1) / (uint32_t)(MAX_PAYLOAD_SIZE) + 1;
    for (int seq = begin; seq <= end; seq++)
    {
        payload_len = std::min(len - offset, MAX_PAYLOAD_SIZE);
        idx = sendbuf_.push((unsigned char *)data + offset, payload_len);
        sendbuf_info_.push(queue_entry(idx, payload_len, seq, begin, end, -1, dest_id));
        offset += payload_len;
    }
    generated_seq_ = end;

    sendbuf_mtx_.unlock();

    kill(getpid(), SIGSEND);
}

ssize_t Socket::recvfrom(void *data) /* 只今バグり中 */
{
    uint32_t next, begin, end, len = 0;

    while (true)
    {
        // std::cout << 1 << std::endl;
        recvbuf_mtx_.lock();
        if (!recvbuf_info_.empty())
        {
            const queue_entry& en = recvbuf_info_.top();
            std::cout << en.hdr_.begin << " " << en.hdr_.seq << std::endl;
            if (len == 0 && en.hdr_.begin == en.hdr_.seq)
            {
                begin = en.hdr_.begin;
                next = begin+1;
                end = en.hdr_.end;
                len += en.payload_len_;
                recvbuf_info_.pop();
                recvbuf_mtx_.unlock();
                break;
            }
        }
        recvbuf_mtx_.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    while (next <= end)
    {
        recvbuf_mtx_.lock();
        if (!recvbuf_info_.empty())
        {
            const queue_entry& en = recvbuf_info_.top();
            std::cout << en.hdr_.begin << " " << en.hdr_.seq << " " << next << std::endl;
            if (en.hdr_.seq == next)
            {
                next++;
                len += en.payload_len_;
                recvbuf_info_.pop();
            }
        }
        recvbuf_mtx_.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    // recv bufferから取得
    recvbuf_mtx_.lock();
    recvbuf_.pop((unsigned char *)data, len);
    recvbuf_mtx_.unlock();

    return len;
}

void Socket::add_endpoint(int id, char *ipaddr, uint16_t port, bool is_same_group)
{
    endpoint_list_.insert(std::make_pair(id, endpoint(ipaddr, port, is_same_group)));
}

ssize_t Socket::output_packet(uint32_t seq, uint32_t begin, uint32_t end, const void *payload, size_t len, int dest_id)
{
    unsigned char buf[MAX_PACKET_SIZE] = {};
    struct header *hdr;
    uint32_t total;

    hdr = (struct header *)buf;
    hdr->seq = seq;
    hdr->begin = begin;
    hdr->end = end;
    std::memcpy(hdr + 1, payload, len);
    total = sizeof(struct header) + len;

    // 1. loop回したほうがいいのか？
    // 2. std::map endpoint_list_の参照は参照ではなくていいのか？
    endpoint dest = endpoint_list_.at(dest_id);
    len = ::sendto(sockfd_, buf, total, 0, (struct sockaddr *)&(dest.addr_), sizeof(dest.addr_));

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

    buf_len = ::recvfrom(sockfd_, buf, MAX_PACKET_SIZE, 0, (struct sockaddr *)&src, &srclen);
    payload_len = buf_len - sizeof(struct header);
    hdr = (struct header *)buf;
    *seq = hdr->seq;
    *begin = hdr->begin;
    *end = hdr->end;
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
            if (events[i].data.fd == sockfd_)
            {
                struct header hdr;
                std::memset(payload, '\0', MAX_PAYLOAD_SIZE);
                len = input_packet(&hdr.seq, &hdr.begin, &hdr.end, payload);

                // if (packet.type == NCK)
                // {
                // }
                // else if (packet.type == DAT)
                // {
                // }

                recvbuf_mtx_.lock();
                idx = recvbuf_.push(payload, len);
                recvbuf_info_.push(queue_entry(idx, len, hdr.seq, hdr.begin, hdr.end, -1, -1)); // src_id_を特定する
                recvbuf_mtx_.unlock();
            }
            else if (events[i].data.fd == signalfd_)
            {
                len = read(signalfd_, &fdsi, sizeof(struct signalfd_siginfo));
                if (len != sizeof(struct signalfd_siginfo))
                {
                    // error
                    continue;
                }

                if (fdsi.ssi_signo == SIGSEND)
                {
                    std::cerr << "SIGSEND" << std::endl;

                    sendbuf_mtx_.lock();
                    while (!sendbuf_info_.empty())
                    {
                        std::memset(payload, '\0', MAX_PAYLOAD_SIZE);
                        queue_entry& en = sendbuf_info_.front();
                        sendbuf_.get(en.idx_, payload, en.payload_len_);
                        if (en.dest_id_ == 0) // 全ノードに送信
                        {
                            for (auto dest = endpoint_list_.begin(); dest != endpoint_list_.end(); ++dest)
                            {
                                len = output_packet(en.hdr_.seq, en.hdr_.begin, en.hdr_.end, payload, en.payload_len_, dest->first);
                            }
                        }
                        else // 指定ノードに送信
                        {
                            len = output_packet(en.hdr_.seq, en.hdr_.begin, en.hdr_.end, payload, en.payload_len_, en.dest_id_);
                        }
                        sendbuf_info_.pop();
                    }
                    sendbuf_mtx_.unlock();
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
    ev.data.fd = sockfd_;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd_, &ev) == -1)
    {
        exit(EXIT_FAILURE);
    }

    // Register signalfd_ in epollfd.
    ev.events = EPOLLIN;
    ev.data.fd = signalfd_;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, signalfd_, &ev) == -1)
    {
        exit(EXIT_FAILURE);
    }

    return epollfd;
}

} // namespace multicast