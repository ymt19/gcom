#pragma once

#include <netinet/in.h>
#include <pthread.h>

#define SEND_BUFFER_SIZE    65535
#define RECV_BUFFER_SIZE    65535

struct endpoint_t
{
    uint16_t port;
    uint32_t addr;
};
typedef struct endpoint_t endpoint_t;

extern int
sender_socket();
extern int
sender_close();
extern ssize_t
send_multicast(const char *buf, ssize_t len);

extern int
receiver_socket(int port);
extern int
receiver_close();
extern ssize_t
receive(const void *buf, ssize_t len, endpoint_t *src);