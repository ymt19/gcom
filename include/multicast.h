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
    pthread_t bg_thread;

    int sd;

    char buff[RECV_BUFF_SIZE];
};
typedef struct receiver_socket_t receiver_socket_t;

struct addr_list_t
{
    sockaddr_in addr;
    struct addr_list_t *next;
};
typedef struct addr_list_t addr_list_t;

struct dest_info_t
{
    ssize_t list_len;
    addr_list_t *addr_list_head;
};
typedef struct dest_info_t dest_info_t;