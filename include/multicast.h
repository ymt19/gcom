#pragma once

#include <netinet/in.h>
#include <pthread.h>

#include "buffer.h"

struct send_info_t
{
    int sd;
};
typedef struct send_info_t send_info_t;

struct receive_info_t
{
    int sd;
    struct sockaddr_in addr;

    buff_t *recv_buff;

    pthread_t bg;
};
typedef struct receive_info_t recv_info_t;

struct destination_info_t
{
    struct sockaddr_in dest_addr[10];
};
typedef struct destination_info_t dest_info_t;