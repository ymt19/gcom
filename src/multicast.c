#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "multicast.h"

struct header {
    uint32_t seq;
    uint32_t ack;
    uint8_t flag;
};

static void recv_bg(recv_info_t *recv_info);

static void recv_bg(recv_info_t *recv_info)
{

}

sender_socket_t *sender_socket()
{
    sender_socket_t *sock = malloc(sizeof(sender_socket_t));

    send_info->sd = sokcet(AF_INET, SOCK_DGRAM, 0);

    return sock;
}

void sender_close(sender_socket_t *sock)
{
    close(sock->sd);
    free(sock);
}

void sendto_unicast(sender_socket_t *sock, char *buff, int len, addr_t *dest_addr)
{
    
}

void sendto_multicast(sender_socket_t *sock, char *buff, int len, addr_list_t *dest_addr_list)
{

}

receiver_socket_t *recveiver_socket(int port, int buf_size, int buf_blk_size)
{
    receiver_socket_t *sock = mallco(sizeof(receiver_socket_t));

    sock->sd = socket(AF_INET, SOCK_DGRAM, 0);
    sock->addr.sin_family = AF_INET;
    sock->addr.sin_port = htons(port);
    sock->addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock->recv_sd, (struct sockaddr *)&sock->addr, sizeof(sock->addr));

    recv_info->buffer = buffer_init();

    pthread_create(&sock->bg, NULL, (void *)recv_bg, (void*)recv_info);

    return sock;
}

void receiver_close(receiver_socket_t *sock)
{
    close(sock->sd);
    buffer_deinit(sock->recv_buffer);
    pthread_join(sock->bg, NULL);
    free(sock);
}

int recvfrom(receiver_socket_t *sock, char *buff, int len, addr_t *src_addr)
{

}