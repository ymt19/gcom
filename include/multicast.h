#pragma once

#include <netinet/in.h>

struct multicast_info_t
{
    /* 送信用 */
    int send_sd;

    /* 受信用 */
    int recv_sd;
    struct sockaddr_in addr;
};
typedef struct multicast_info_t mc_info_t;

struct destination_info_t
{
    struct sockaddr_in dest_addr[10];
};
typedef struct destination_info_t dest_info_t;