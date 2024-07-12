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
#include "utils.h"

#define DATAGRAM_SIZE_MAX   1472
#define DATA_SIZE_MAX       DATAGRAM_SIZE_MAX - sizeof(struct header)

#define FLAG_NAK            0x01
#define FLAG_ACK            0x02
#define FLAG_MTC    // multicastデータグラム
#define FLAG_RLY    // 転送するデータグラム

#define EPOLL_MAX_EVENTS 16

#define SIGSEND     SIGRTMIN /* 送信バッファ追加通知: メイン->バックグラウンド */
#define SIGCLOSE    SIGRTMIN+1 /* バックグラウンド終了通知: メイン->バックグラウンド */

struct header {
    // rootipaddr
    // previpaddr
    uint32_t seq;
    uint32_t first;
    uint32_t last;
    uint8_t flag;
};

struct sender_socket
{
    int sockfd;
    int sigfd;
    pthread_t bg_threadid;
    int genseq; /* generated seqence number */
    pthread_mutex_t mutex_genseq;
    uint8_t sendbuf[SEND_BUFFER_SIZE];
    pthread_mutex_t mutex_sendbuf;
};

struct receiver_socket
{
    int sockfd;
    int sigfd;
    pthread_t bg_threadid;
    uint8_t recvbuf[RECV_BUFFER_SIZE];
    pthread_mutex_t mutex_recvbuf;
};

static struct sender_socket *ss = NULL;
static struct receiver_socket *rs = NULL;

// /**
//  * セグメントを出力
//  */
// static void
// output_segment(int fd, uint32_t seq, uint32_t ack, uint8_t flag, uint8_t *payload, ssize_t payload_size, struct sockaddr_in *destaddr)
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
//  * セグメントを入力
//  */
// static void
// input_segment(int fd, struct header *hdr, uint8_t *payload, struct sockaddr_in *srcaddr)
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
 * @brief ソケット監視バックグラウンドスレッド
 */
static void *
bg_sender_socket()
{
    /* segment */
    // int seq;
    // int first;
    // int last;
    // int flag;
    // char data[DATA_SIZE_MAX];
    /* epoll */
    struct epoll_event ev, events[EPOLL_MAX_EVENTS];
    int epollfd, nfds;
    /* signalfd */
    struct signalfd_siginfo fdsi;
    ssize_t size;

    /* epollの生成 */
    epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    /* sockfdをepollに登録 */
    ev.events = EPOLLIN;
    ev.data.fd = ss->sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ss->sockfd, &ev) == -1)
    {
        perror("epoll_ctl: sock->udpsock");
        exit(EXIT_FAILURE);
    }

    /* signalfdをepollに登録 */
    ev.events = EPOLLIN;
    ev.data.fd = ss->sigfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, ss->sigfd, &ev) == -1)
    {
        perror("epoll_ctl: sfd");
        exit(EXIT_FAILURE);
    }

    /* 各イベントの処理 */
    for (;;)
    {
        nfds = epoll_wait(epollfd, events, EPOLL_MAX_EVENTS, -1);
        if (nfds == -1)
            handle_error("epoll_wait");

        for (int i = 0; i < nfds; ++i)
        {
            /* データを受信した場合 */
            if (events[i].data.fd == ss->sockfd)
            {
                // input_segment();
            }
            /* シグナルを受信した場合 */
            else if (events[i].data.fd == ss->sigfd)
            {
                size = read(ss->sigfd, &fdsi, sizeof(struct signalfd_siginfo));
                if (size != sizeof(struct signalfd_siginfo))
                    handle_error("read");

                if (fdsi.ssi_signo == SIGSEND)
                {
                    DEBUG_PRINT("SIGSEND");
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
int
sender_socket()
{
    int en;
    
    ss = malloc(sizeof(struct sender_socket));
    if (ss == NULL)
        handle_error("malloc");

    ss->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ss->sockfd == -1)
        handle_error("socket");

    ss->sigfd = signalfd_create();

    ss->genseq = 0;
    en = pthread_mutex_init(&ss->mutex_genseq, NULL);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_destroy");

    en = pthread_create(&ss->bg_threadid, NULL, (void *)bg_sender_socket, NULL);
    if (en != 0)
        handle_error_en(en, "pthread_create");

    return 0;
}

/**
 * 
 */
int
sender_close()
{
    int en;

    if (ss == NULL)
        return -1;

    // バックグラウンドスレッドにシグナル送信
    if (kill(getpid(), SIGCLOSE) != 0)
        handle_error("kill");

    en = pthread_join(ss->bg_threadid, NULL);
    if (en != 0)
        handle_error_en(en, "pthread_join");

    if (close(ss->sockfd) != 0)
        handle_error("close");

    if (close(ss->sigfd) != 0)
        handle_error("close");

    free(ss);
    ss = NULL;
    return 0;
}

/**
 * @brief マルチキャストによる送信
 */
ssize_t
send_multicast(const char *buf, ssize_t len)
{
    int en;
    ssize_t sent = 0;
    int first, last;

    if (ss == NULL)
        return -1;

    /* asign sequence number */
    en = pthread_mutex_lock(&ss->mutex_genseq);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_lock");
    first = ss->genseq + 1;
    last = first;
    ss->genseq = last;
    en = pthread_mutex_unlock(&ss->mutex_genseq);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_unlock");

    /* write buffer */

    if (kill(getpid(), SIGSEND) != 0)
        handle_error("kill");

    return sent;
}

/**
 * @brief ソケット監視バックグラウンドスレッド
 */
static void *
bg_receiver_socket()
{
    // datagram
    // struct header hdr;
    // char data[DATA_SIZE_MAX];
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
    ev.data.fd = rs->sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, rs->sockfd, &ev) == -1)
    {
        perror("epoll_ctl: sock->udpsock");
        exit(EXIT_FAILURE);
    }

    // signalfdをepollに登録
    ev.events = EPOLLIN;
    ev.data.fd = rs->sigfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, rs->sigfd, &ev) == -1)
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
            if (events[i].data.fd == rs->sockfd)
            {
                // input_segment();
            }
            // シグナルを受信した場合
            else if (events[i].data.fd == rs->sigfd)
            {
                size = read(rs->sigfd, &fdsi, sizeof(struct signalfd_siginfo));
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
int
receiver_socket(int port)
{
    int en;
    struct sockaddr_in addr;
    
    rs = malloc(sizeof(struct receiver_socket));
    if (rs == NULL)
        handle_error("malloc");

    // alm.confの読み込み

    // sockfdの設定
    rs->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (rs->sockfd == -1)
        handle_error("socket");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(rs->sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
        handle_error("bind");

    // sigfdの設定
    rs->sigfd = signalfd_create();

    // ソケット監視バックグラウンドスレッドの起動
    en = pthread_create(&rs->bg_threadid, NULL, (void *)bg_receiver_socket, NULL);
    if (en != 0)
        handle_error_en(en, "pthread_create");

    return 0;
}

/**
 * 
 */
int
receiver_close()
{
    int en;

    if (rs == NULL)
        return -1;

    // バックグラウンドスレッドにシグナル送信
    if (kill(getpid(), SIGCLOSE) != 0)
        handle_error("kill");

    en = pthread_join(rs->bg_threadid, NULL);
    if (en != 0)
        handle_error_en(en, "pthread_join");

    if (close(rs->sockfd) != 0)
        handle_error("close");

    if (close(rs->sigfd) != 0)
        handle_error("close");

    free(rs);
    rs = NULL;
    return 0;
}

/**
 * 
 */
ssize_t
receive(const void *buf, ssize_t len, endpoint_t *src_addr)
{
    if (rs == NULL)
        return -1;

    DEBUG_PRINT("receive");

    return 0;
}