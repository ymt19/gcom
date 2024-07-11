#pragma once

#include <netinet/in.h>
#include <pthread.h>
#include "buffer.h"

struct sender_socket_t
{
    int sockfd;                         // ソケット参照ファイルディスクリプタ
    int sigfd;                          // シグナル受信用ファイルディスクリプタ
    pthread_t bg_threadid;              // 制御メッセージ受信処理スレッド
    int genseq;                         // 生成済みSEQ
    pthread_mutex_t mutex_genseq;       // seqに対するmutex
    buffer_t sendbuf;                   // 送信バッファ
};
typedef struct sender_socket_t sender_socket_t;

struct receiver_socket_t
{
    int sockfd;                         // ソケット参照ファイルディスクリプタ
    int sigfd;                          // シグナル受信用ファイルディスクリプタ
    pthread_t bg_threadid;              // 制御メッセージ受信処理スレッド
    // buffer_t buff;                      // 受信バッファ
};
typedef struct receiver_socket_t receiver_socket_t;

struct endpoint_t
{
    uint16_t port;
    uint32_t addr;
};
typedef struct endpoint_t endpoint_t;

sender_socket_t *sender_socket();
void sender_close(sender_socket_t *sock);
ssize_t send_multicast(sender_socket_t *sock, const char *buff, ssize_t len);

receiver_socket_t *receiver_socket(int port);
void receiver_close(receiver_socket_t *sock);
ssize_t receive(receiver_socket_t *sock, const void *buff, ssize_t len, endpoint_t *src);