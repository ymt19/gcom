#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// secondaryサーバ数の最大値
#define MAX_NUM_SECONDARY_SERVERS  10

struct tcp_server_config
{
    // primaryとsecandary
    size_t srv_id;
    char ipaddr[32];
    unsigned short port;
    size_t duration;

    // primaryのみ
    size_t iops;
    size_t num_threads;
    size_t send_log_size;
    size_t num_secondary_servers;
    size_t num_servers;
    char srvs_ipaddr[MAX_NUM_SECONDARY_SERVERS+2][32]; // indexとIDが一致
    unsigned short srvs_port[MAX_NUM_SECONDARY_SERVERS+2]; // indexとIDが一致

    double system_start_time;
};
typedef struct tcp_server_config tcp_server_config;

