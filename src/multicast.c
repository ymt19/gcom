#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "multicast.h"

#define UDP_PAYLOAD_SIZE_MAX 1472

#define FLAG_NAK 0x01
#define FLAG_ACK 0x02

struct header {
    uint32_t seq;
    uint32_t ack;
    uint8_t flag;

    uint32_t gn1_addr;
    uint16_t gn1_port;
    uint32_t gn2_addr;
    uint16_t gn2_port;
};

static void
output_segment(int sd, uint32_t seq, uint32_t ack, uint8_t flag, endpoint_t *gn1, endpoint_t *gn2, uint8_t *data, ssize_t len, struct sockaddr_in *destaddr)
{
    uint8_t seg[UDP_PAYLOAD_SIZE_MAX] = {};
    struct header *hdr;
    uint16_t total;

    hdr = (struct header *)seg;
    hdr->seq = seq;
    hdr->ack = ack;
    hdr->flag = flag;
    if (gn1 == NULL) {
        hdr->gn1_addr = NULL;
        hdr->gn1_port = NULL;
    } else {
        hdr->gn1_addr = gn1->addr;
        hdr->gn1_port = gn1->port;
    }
    if (gn2 == NULL) {
        hdr->gn2_addr = NULL;
        hdr->gn2_port = NULL;
    } else {
        hdr->gn2_addr = gn2->addr;
        hdr->gn2_port = gn2->port;
    }
    memcpy(hdr + 1, data, len);
    total = sizeof(*hdr) + len;

    sendto(fd, seg, total, 0, (struct sockaddr *)destaddr, sizeof(*destaddr));
}

static void
input_segment(int sd, struct header *hdr, uint8_t *data, struct sockaddr_in *srcaddr)
{
    uint8_t seg[UDP_PAYLOAD_SIZE_MAX] = {};
    ssize_t len;

    len = recvfrom(sd, seg, UDP_PAYLOAD_SIZE_MAX, 0, (struct sockaddr *)srcaddr, sizeof(*srcaddr));
    memcpy(hdr, seg, sizeof(struct header));
    memcpy(data, seg + sizeof(struct header), len - sizeof(struct header));
}

static void
bg_receiver(receiver_socket_t *sock)
{
    while (1)
    {
        input_segment(sock->sd);
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
sendto_unicast(sender_socket_t *sock, char *buf, ssize_t len, endpoint_t *dest)
{
    ssize_t sent = 0;
    struct sockaddr_in destaddr;

    destaddr.sin_family = AF_INET;
    destaddr.sin_port = dest->port;
    destaddr.sin_addr.s_addr = dest->addr;

    while (sent < len)
    {
        sent += output_segment(sock->sd, );
    }
    while (1) {
        input_segment(sock->sd)
        // if nack -> retransmission
        // else if ack -> break
        // else -> error
    }
    return sent;
}

ssize_t
sendto_multicast(sender_socket_t *sock, const void *buf, ssize_t len, dest_info_t *dest_info)
{
    ssize_t sent = 0;
    int recvack = 0;
    while (sent < len)
    {
        // for dest_addr_list
            // output_segment()
        // sent += ??
    }
    while (recvack > dest_info->liset_size)
    {
        input_segment(sock->sd)
        // if nack -> retransmission
        // else if ack
            // recvack++
        // else -> error
    }
    return sent;
}

receiver_socket_t *
recveiver_socket(int port)
{
    struct sockaddr_in addr;

    receiver_socket_t *sock = mallco(sizeof(receiver_socket_t));

    sock->sd = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock->recv_sd, (struct sockaddr *)&addr, sizeof(addr));

    pthread_create(&sock->bg_thread, NULL, (void *)bg_receiver, (void*)sock);

    return sock;
}

void
receiver_close(receiver_socket_t *sock)
{
    pthread_cancel(sock->bg_thread);
    pthread_join(sock->bg_thread, NULL);
    close(sock->sd);
    free(sock);
}

ssize_t
recvfrom(receiver_socket_t *sock, const void *buff, ssize_t len, addr_t *src_addr)
{
    while (1)
    {
        // if recvbuffにデータがある
            // buffに格納
            // break
    }
    // return ??
}