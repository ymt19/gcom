#pragma once

#include <netinet/in.h>
#include <pthread.h>

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
extern int
send_multicast(void *buf, size_t len);

extern int
receiver_socket(int port);
extern int
receiver_close();
extern int
receive(void *buf, size_t len, endpoint_t *src);