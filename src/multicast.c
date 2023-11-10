#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "multicast.h"

struct header {
    uint32_t seq;
    uint32_t ack;
    uint8_t flg;
    uint16_t sum;

    uint32_t gn1_addr;
    uint16_t gn1_port;
    uint32_t gn2_addr;
    uint16_t gn2_port;
};

static void
output_segment(sender_socket_t *sock)
{
    // sendto()
}

static void
input_segment(receiver_socket_t *sock, int blocking)
{
    // blking==1 blocking
    // recvfrom()
}

static void
receiver_bg(receiver_socket_t *sock)
{
    while ()
    {
        // input_segment(1) blocking
        // if data ->
        // else if nack ->
        // else if ack ->
        // else -> error
    }
}

sender_socket_t *
sender_socket()
{
    sender_socket_t *sock = malloc(sizeof(sender_socket_t));

    send_info->sd = sokcet(AF_INET, SOCK_DGRAM, 0);

    return sock;
}

void
sender_close(sender_socket_t *sock)
{
    close(sock->sd);
    free(sock);
}

ssize_t
sendto_unicast(sender_socket_t *sock, char *buff, ssize_t len, addr_t *dest_addr)
{
    ssize_t sent = 0;
    while ()
    {
        // input_segment(0) nonblocking
        // if nack -> retransmission
        // else if ack -> break
        // else -> error

        // if sent < len
            // sent += output_segment()
    }
    return sent;
}

ssize_t
sendto_multicast(sender_socket_t *sock, char *buff, ssize_t len, addr_list_t *dest_addr_list)
{
    ssize_t sent = 0;
    while ()
    {
        // input_segment(0) nonblocking
        // if nack -> retransmission
        // else if ack
            // ackcnt++
            // if ackcnt == ?? -> break
        // else -> error

        // if sent < len
            // for dest_addr_list
                // output_segment()
            // sent += ??
    }
    return sent;
}

receiver_socket_t *
recveiver_socket(int port, int buf_size, int buf_blk_size)
{
    struct sockaddr_in addr;

    receiver_socket_t *sock = mallco(sizeof(receiver_socket_t));

    sock->sd = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock->recv_sd, (struct sockaddr *)&addr, sizeof(addr));

    pthread_create(&sock->bg, NULL, (void *)receiver_bg, (void*)sock);
    pthread_detatch(sock->bg);

    return sock;
}

void
receiver_close(receiver_socket_t *sock)
{
    close(sock->sd);
    free(sock);
}

int
recvfrom(receiver_socket_t *sock, char *buff, int len, addr_t *src_addr)
{
    while ()
    {
        // if recvbuffにデータがある
            // buffに格納
            // break
    }
    // return ??
}