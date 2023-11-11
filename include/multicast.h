#pragma once

#include <netinet/in.h>
#include <pthread.h>

#define RECV_BUFF_SIZE 65535

struct sender_socket_t
{
    int sd;
};
typedef struct sender_socket_t sender_socket_t;

struct receiver_socket_t
{
    int sd;

    char buff[RECV_BUFF_SIZE];

    pthread_t bg_thread;
};
typedef struct receiver_socket_t receiver_socket_t;

struct endpoint_t
{
    uint16_t port;
    uint32_t addr;
};
typedef struct endpoint_t endpoint_t;

struct endpoint_list_t
{
    endpoint_t endpoint;
    struct endpoint_list_t *next;
};
typedef struct endpoint_list_t endpoint_list_t;

struct dest_info_t
{
    endpoint_list_t *endpoint_list_head;
};
typedef struct dest_info_t dest_info_t;