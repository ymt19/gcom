#pragma once

#include "server.h"
#include "time_manager.h"

#include <unistd.h>

struct client_thread_info_t
{
    size_t client_id;
    server_config_t *srv_config;
};
typedef struct client_thread_info_t client_thread_info_t;


void client(client_thread_info_t *info);