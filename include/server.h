#pragma once

#include <stdio.h>
#include <stdlib.h>

struct srv_config
{
    size_t srv_id;
    char ip_addr[16];
    unsigned short port;
    size_t duration;
    size_t iops;
    size_t num_threads;
    size_t log_size;

    double system_start_time;
};
typedef struct srv_config srv_config;

