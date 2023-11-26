#pragma once

#include <netinet/in.h>
#include <pthread.h>

#define RECV_BUFF_SIZE 65535
#define MAX_MULTICAST_SIZE 10

struct sender_socket_t
{
    int sd;
};
typedef struct sender_socket_t sender_socket_t;

struct receiver_socket_t
{
    pthread_t bg_thread;

    int sd;

    char recvbuf[RECV_BUFF_SIZE];
};
typedef struct receiver_socket_t receiver_socket_t;

struct endpoint_t
{
    uint16_t port;
    uint32_t addr;
};
typedef struct endpoint_t endpoint_t;

struct dest_info_t
{
    ssize_t list_size;
    endpoint_t endpoint_list[MAX_MULTICAST_SIZE];
};
typedef struct dest_info_t dest_info_t;

sender_socket_t *
sender_socket();
extern void
sender_close(sender_socket_t *sock);
extern ssize_t
sendto_unicast(sender_socket_t *sock, char *buff, ssize_t len, endpoint_t *dest);
extern ssize_t
sendto_multicast(sender_socket_t *sock, char *buff, ssize_t len, dest_info_t *dest_info);

extern receiver_socket_t *
recveiver_socket(int port);
extern void
receiver_close(receiver_socket_t *sock);
extern ssize_t
recvfrom(receiver_socket_t *sock, char *buff, ssize_t len, endpoint_t *src);