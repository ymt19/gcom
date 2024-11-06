#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <exception>
#include <iostream>
#include "multicast.hpp"

namespace multicast
{

Socket::Socket() 
    : sockfd_(-1),
    signalfd_(-1),
    generated_seq_(0) {}

Socket::~Socket() {}

void Socket::open(uint16_t port)
{
    std::cerr << "open" << std::endl;
    struct sockaddr_in addr;

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

    try
    {
        signalfd_ = get_signalfd();
    }
    catch(const std::exception& e)
    {
        throw std::exception();
    }

    // thread
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

void Socket::send()
{
    std::cerr << "send" << std::endl;
    // msgを分割して，send_buff_に登録
    // seqをインクリメント
    kill(getpid(), SIGSEND);
}

void Socket::recv()
{
    std::cerr << "recv" << std::endl;
    // recv_buff_から取得
}

void Socket::output_packet()
{
    // udp_sock_.send_to(asio::buffer(&msg, size, dest));
    std::cout << "output packet" << std::endl;
}

void Socket::input_packet()
{
    // udp_sock_.recv()
    std::cout << "input packet" << std::endl;
}

void *Socket::background()
{
    std::cerr << "background" << std::endl;
    struct epoll_event events[max_epoll_events];
    int epollfd, nfds;
    struct signalfd_siginfo fdsi;
    ssize_t len;
    // Packet packet;

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
                input_packet();

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
                }

                if (fdsi.ssi_signo == SIGSEND)
                {
                    std::cerr << "SIGSEND" << std::endl;
                    // timerfd <- タイムアウト
                    output_packet();
                }
                else if (fdsi.ssi_signo == SIGCLOSE)
                {
                    std::cerr << "SIGCLOSE" << std::endl;
                    return nullptr;
                }
                else
                {
                    fprintf(stderr, "read unexpected signal.\n");
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