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

mc_info_t *multicast_init(int recv_port)
{
    mc_info_t *mc_info = malloc(sizeof(mc_info_t));
    
    mc_info->send_sd = sokcet(AF_INET, SOCK_DGRAM, 0);

    mc_info->recv_sd = socket(AF_INET, SOCK_DGRAM, 0);
    mc_info->addr.sin_family = AF_INET;
    mc_info->addr.sin_port = htons(recv_port);
    mc_info->addr.sin_addr.s_addr = INADDR_ANY;
    bind(mc_info->recv_sd, (struct sockaddr *)&mc_info->addr, sizeof(mc_info->addr));

    return mc_info;
}

void multicast_deinit(mc_info_t *mc_info)
{
    close(mc_info->send_sd);
    close(mc_info->recv_sd);
    free(mc_info);
}

void unicast_send()
{

}

void multicast_send()
{

}

void receive()
{

}