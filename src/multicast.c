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

send_info_t *send_info_init()
{
    send_info_t *send_info = malloc(sizeof(send_inof_t));

    send_info->sd = sokcet(AF_INET, SOCK_DGRAM, 0);

    return send_info;
}

void send_info_deinit(send_info_t *send_info)
{
    close(send_info->sd);
    free(send_info);
}

recv_info_t *recv_info_init(int port)
{
    recv_info_t *recv_info = mallco(sizeof(recv_info_t));

    recv_info->sd = socket(AF_INET, SOCK_DGRAM, 0);
    recv_info->addr.sin_family = AF_INET;
    recv_info->addr.sin_port = htons(port);
    recv_info->addr.sin_addr.s_addr = INADDR_ANY;
    bind(recv_info->recv_sd, (struct sockaddr *)&recv_info->addr, sizeof(recv_info->addr));

    recv_info->buffer = buffer_init();

    pthread_create(&recv_info->bg, NULL, (void *)recv_bg, (void*)recv_info);

    return recv_info;
}

void recv_info_init(recv_info_t *recv_info)
{
    close(recv_info->sd);
    buffer_deinit(recv_info->recv_buffer);
    pthread_join(recv_info->bg, NULL);
    free(recv_info);
}

void unicast_send(send_info_t *send_info, char *buff, int len, dest_info_t *dest_info)
{

}

void multicast_send(send_info_t *send_info, char *buff, int len, dest_info_t *dest_info)
{

}

int recv(recv_info_t *recv_info, char *buff, int len)
{

}