#include <error.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include "multicast.h"
// #include "buffer.h"

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#ifdef DEBUG
#define DEBUG_PRINT(...) \
    fprintf(stderr, "%s(%d) %s:", __FILE__, __LINE__, __func__), \
    fprintf(stderr, "Debug: %s\n", __VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#define EPOLL_MAX_EVENTS 16

#define SIGSEND     SIGRTMIN
#define SIGCLOSE    SIGRTMIN+1

// /**
//  * データグラムを出力
//  */
// static void
// output_datagram(int fd, uint32_t seq, uint32_t ack, uint8_t flag, uint8_t *payload, ssize_t payload_size, struct sockaddr_in *destaddr)
// {
//     uint8_t seg[UDP_PAYLOAD_SIZE_MAX] = {};
//     uint16_t seg_size;
//     struct header *hdr;

//     hdr = (struct header *)seg;
//     hdr->seq = seq;
//     hdr->ack = ack;
//     hdr->flag = flag;
//     // memcpy(seg + sizeof(struct header), payload, payload_size);
//     seg_size = sizeof(*hdr) + payload_size;

//     sendto(fd, seg, seg_size, 0, (struct sockaddr *)destaddr, sizeof(*destaddr));
// }

// /**
//  * データグラムを入力
//  */
// static void
// input_datagram(int fd, struct header *hdr, uint8_t *payload, struct sockaddr_in *srcaddr)
// {
//     // uint8_t seg[UDP_PAYLOAD_SIZE_MAX] = {};
//     // ssize_t len;

//     // len = recvfrom(fd, seg, UDP_PAYLOAD_SIZE_MAX, 0, (struct sockaddr *)srcaddr, sizeof(srcaddr));
//     // memcpy(hdr, seg, sizeof(struct header));
//     // memcpy(payload, seg + sizeof(struct header), len - sizeof(struct header));
// }

static int
signalfd_create()
{
    sigset_t mask;
    int sigfd;
    sigemptyset(&mask);
    sigaddset(&mask, SIGSEND);
    sigaddset(&mask, SIGCLOSE);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sigfd = signalfd(-1, &mask, SFD_CLOEXEC);
    if (sigfd == -1)
        handle_error("signalfd");
    return sigfd;
}

/**
 * @brief sender_socketのバックグラウンドスレッドとして呼び出される
 */
static void *
bg_sender_socket(sender_socket_t *sock)
{

    // 変数：データグラム
    // int nextseq;
    // struct header hdr;
    // uint8_t data[DATA_SIZE_MAX];
    // epoll
    struct epoll_event ev, events[EPOLL_MAX_EVENTS];
    int epollfd, nfds;
    // signalfd
    struct signalfd_siginfo fdsi;
    ssize_t size;


    // epollfdの生成
    epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // sockfdをepollに登録
    ev.events = EPOLLIN;
    ev.data.fd = sock->sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock->sockfd, &ev) == -1)
    {
        perror("epoll_ctl: sock->udpsock");
        exit(EXIT_FAILURE);
    }

    // signalfdをepollに登録
    ev.events = EPOLLIN;
    ev.data.fd = sock->sigfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock->sigfd, &ev) == -1)
    {
        perror("epoll_ctl: sfd");
        exit(EXIT_FAILURE);
    }

    // 各イベントの処理
    for (;;)
    {
        nfds = epoll_wait(epollfd, events, EPOLL_MAX_EVENTS, -1);
        if (nfds == -1)
            handle_error("epoll_wait");

        for (int i = 0; i < nfds; ++i)
        {
            // データを受信した場合
            if (events[i].data.fd == sock->sockfd)
            {
                // input_segment();
            }
            // シグナルを受信した場合
            else if (events[i].data.fd == sock->sigfd)
            {
                size = read(sock->sigfd, &fdsi, sizeof(struct signalfd_siginfo));
                if (size != sizeof(struct signalfd_siginfo))
                    handle_error("read");

                if (fdsi.ssi_signo == SIGSEND)
                {
                    DEBUG_PRINT("SIGSEND");
                    // buffer_get(&sock->buff, nextseq, &hdr, &data);
                    // fprintf(stderr, "seq:%d, ")
                }
                else if (fdsi.ssi_signo == SIGCLOSE)
                {
                    DEBUG_PRINT("SIGCLOSE");
                    return NULL;
                }
                else
                {
                    fprintf(stderr, "read unexpected signal.\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

/**
 * 
 */
sender_socket_t *
sender_socket()
{
    int en;
    sender_socket_t *sock;
    
    sock = malloc(sizeof(sender_socket_t));
    if (sock == NULL)
        handle_error("malloc");

    sock->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock->sockfd == -1)
        handle_error("socket");
    
    sock->sigfd = signalfd_create();

    sock->genseq = 0;
    en = pthread_mutex_init(&sock->mutex_genseq, NULL);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_destroy");

    // buffer_create(&sock->buff);

    en = pthread_create(&sock->bg_threadid, NULL, (void *)bg_sender_socket, (void *)sock);
    if (en != 0)
        handle_error_en(en, "pthread_create");

    return sock;
}

/**
 * 
 */
void
sender_close(sender_socket_t *sock)
{
    int en;

    // バックグラウンドスレッドにシグナル送信
    if (kill(getpid(), SIGCLOSE) != 0)
        handle_error("kill");

    en = pthread_join(sock->bg_threadid, NULL);
    if (en != 0)
        handle_error_en(en, "pthread_join");

    if (close(sock->sockfd) != 0)
        handle_error("close");

    if (close(sock->sigfd) != 0)
        handle_error("close");

    // buffer_free(&sock->buff);

    free(sock);
}

/**
 * @brief マルチキャストによる送信
 */
ssize_t
send_multicast(sender_socket_t *sock, const char *buff, ssize_t len)
{
    int en;
    ssize_t sent = 0;
    int first, last;
    struct header hdr;

    // seqの割り当て
    en = pthread_mutex_lock(&sock->mutex_genseq);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_lock");
    first = sock->genseq + 1;
    last = first;
    sock->genseq = last;
    en = pthread_mutex_unlock(&sock->mutex_genseq);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_unlock");

    // バッファに追加
    for (int seq = first; seq <= last; seq++)
    {
        hdr.seq = seq;
        hdr.first = first;
        hdr.last = last;
        // hdr.flag
        hdr.size = len;
        // buffer_push(&sock->buff, &hdr, buff);
        #ifdef DEBUG
            fprintf(stderr, "buffer add seq:%d, first:%d, last:%d, size:%d\n", hdr.seq, hdr.first, hdr.last, hdr.size);
        #endif
    }

    if (kill(getpid(), SIGSEND) != 0)
        handle_error("kill");

    return sent;
}

/**
 * 
 */
static void *
bg_receiver_socket(receiver_socket_t *sock)
{
    // epoll
    struct epoll_event ev, events[EPOLL_MAX_EVENTS];
    int epollfd, nfds;
    // signalfd
    struct signalfd_siginfo fdsi;
    ssize_t size;

    // epollfdの生成
    epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // sockfdをepollに登録
    ev.events = EPOLLIN;
    ev.data.fd = sock->sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock->sockfd, &ev) == -1)
    {
        perror("epoll_ctl: sock->udpsock");
        exit(EXIT_FAILURE);
    }

    // signalfdをepollに登録
    ev.events = EPOLLIN;
    ev.data.fd = sock->sigfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock->sigfd, &ev) == -1)
    {
        perror("epoll_ctl: sfd");
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        nfds = epoll_wait(epollfd, events, EPOLL_MAX_EVENTS, -1);
        if (nfds == -1)
            handle_error("epoll_wait");

        for (int i = 0; i < nfds; ++i)
        {
            // データを受信した場合
            if (events[i].data.fd == sock->sockfd)
            {
                // input_segment();
            }
            // シグナルを受信した場合
            else if (events[i].data.fd == sock->sigfd)
            {
                size = read(sock->sigfd, &fdsi, sizeof(struct signalfd_siginfo));
                if (size != sizeof(struct signalfd_siginfo))
                    handle_error("read");

                if (fdsi.ssi_signo == SIGCLOSE)
                {
                    DEBUG_PRINT("SIGCLOSE");
                    return NULL;
                }
                else
                {
                    fprintf(stderr, "read unexpected signal.\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

/**
 * 
 */
receiver_socket_t *
receiver_socket(int port)
{
    int en;
    struct sockaddr_in addr;
    receiver_socket_t *sock;
    
    sock = malloc(sizeof(receiver_socket_t));
    if (sock == NULL)
        handle_error("malloc");

    // alm.confの読み込み

    // sockfdの設定
    sock->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock->sockfd == -1)
        handle_error("socket");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock->sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
        handle_error("bind");

    // sigfdの設定
    sock->sigfd = signalfd_create();

    en = pthread_create(&sock->bg_threadid, NULL, (void *)bg_receiver_socket, (void*)sock);
    if (en != 0)
        handle_error_en(en, "pthread_create");

    return sock;
}

/**
 * 
 */
void
receiver_close(receiver_socket_t *sock)
{
    int en;

    // バックグラウンドスレッドにシグナル送信
    if (kill(getpid(), SIGCLOSE) != 0)
        handle_error("kill");

    en = pthread_join(sock->bg_threadid, NULL);
    if (en != 0)
        handle_error_en(en, "pthread_join");

    if (close(sock->sockfd) != 0)
        handle_error("close");

    if (close(sock->sigfd) != 0)
        handle_error("close");

    free(sock);
}

/**
 * 
 */
ssize_t
receive(receiver_socket_t *sock, const void *buff, ssize_t len, endpoint_t *src_addr)
{
    DEBUG_PRINT("receive");
    // for (;;)
    // {
        
    // }
    
    return 0;
}