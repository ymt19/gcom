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
    std::cerr << "close start" << std::endl;

    kill(getpid(), SIGCLOSE);
    background_th_->join();
    ::close(sockfd_);

    std::cerr << "close end" << std::endl;
}

void Socket::sendto(const void *data, size_t len, int dest_id)
{
    std::cerr << "send" << std::endl;
    // msgを分割して，send_buff_に登録
    // seqをインクリメント
    uint64_t idx;

    sendbuf_mtx_.lock();
    idx = sendbuf_.push((unsigned char *)data, len);
    generated_seq_++;
    sendbuf_info_.push(packet_info(idx, generated_seq_, len, dest_id));
    sendbuf_mtx_.unlock();

    kill(getpid(), SIGSEND);
}

ssize_t Socket::recvfrom(void *data)
{
    std::cerr << "recv" << std::endl;

    uint32_t len = 0;
    for (;;)
    {
        if (!recvbuf_info_.empty())
        {
            recvbuf_mtx_.lock();
            const packet_info& packet = recvbuf_info_.top();
            recvbuf_info_.pop();
            len += packet.len_;
            recvbuf_mtx_.unlock();
            break;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    recvbuf_mtx_.lock();
    recvbuf_.pop((unsigned char *)data, len);
    recvbuf_mtx_.unlock();
    return len;
}

void Socket::add_endpoint(int id, char *ipaddr, uint16_t port, bool is_same_group)
{
    endpoint_list_.insert(std::make_pair(id, endpoint(ipaddr, port, is_same_group)));
}

ssize_t Socket::output_packet(uint32_t seq, const void *payload, size_t len, int dest_id)
{
    std::cout << "output packet" << std::endl;

    unsigned char buf[MAX_PACKET_SIZE] = {};
    struct header *hdr;
    uint32_t total;

    hdr = (struct header *)buf;
    hdr->seq = seq;
    std::memcpy(hdr + 1, payload, len);
    total = sizeof(struct header) + len;

    // 1. loop回したほうがいいのか？
    // 2. std::map endpoint_list_の参照は参照ではなくていいのか？
    endpoint dest = endpoint_list_.at(dest_id);
    len = ::sendto(sockfd_, buf, total, 0, (struct sockaddr *)&(dest.addr_), sizeof(dest.addr_));

    return len; // -1の場合sendto()error
}

size_t Socket::input_packet(uint32_t *seq, void *payload)
{
    std::cout << "input packet" << std::endl;

    unsigned char buf[MAX_PACKET_SIZE] = {};
    ssize_t len, payload_len;
    struct header *hdr;
    struct sockaddr_in src;
    socklen_t srclen = sizeof(src);

    len = ::recvfrom(sockfd_, buf, MAX_PACKET_SIZE, 0, (struct sockaddr *)&src, &srclen);
    payload_len = len - sizeof(struct header);
    hdr = (struct header *)buf;
    *seq = hdr->seq;
    std::memcpy(payload, buf + sizeof(struct header), payload_len);

    return payload_len;
}

void *Socket::background()
{
    std::cerr << "background" << std::endl;
    struct epoll_event events[max_epoll_events];
    int epollfd, nfds;
    struct signalfd_siginfo fdsi;
    unsigned char payload[MAX_PAYLOAD_SIZE];
    ssize_t len;
    uint64_t idx;
    uint32_t seq;

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
                len = input_packet(&seq, payload);
                std::cout << len << " " << seq << " " << payload << std::endl;

                recvbuf_mtx_.lock();
                idx = recvbuf_.push(payload, len);
                recvbuf_info_.push(packet_info(idx, seq, len, -1));
                recvbuf_mtx_.unlock();

                // if (packet.type == ACK)
                // {
                // }
                // else if (packet.type == NACK)
                // {
                // }
                // else if (packet.type == DATA)
                // {
                // }
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
                        packet_info& pack = sendbuf_info_.front();
                        sendbuf_.get(pack.idx_, payload, pack.len_);
                        len = output_packet(pack.seq_, payload, pack.len_, pack.dest_id_);
                        std::cout << pack.len_ << " " << len << " " << pack.seq_ << " " << payload << std::endl;
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