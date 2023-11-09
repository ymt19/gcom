#pragma once

#include <netinet/in.h>
#include <pthread.h>

#include "buffer.h"

struct sender_socket_t
{
    int sd;
};
typedef struct sender_socket_t sender_socket_t;

struct receiver_socket_t
{
    int sd;
    struct sockaddr_in addr;

    buff_t *recv_buff;

    pthread_t bg;
};
typedef struct receiver_socket_t receiver_socket_t;

struct addr_t
{
    struct sockaddr_in dest_addr[10];
};
typedef struct addr_t addr_t;

struct addr_list_t
{
    struct addr_t *next;
};
typedef struct addr_list_t addr_list_t;