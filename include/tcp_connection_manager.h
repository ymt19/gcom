#pragma once

#include "../include/server.h"

struct sender_config_t
{
    server_config_t *srv_config;
};
typedef struct sender_config_t sender_config_t;

struct reciever_config_t
{
    
};
typedef struct reciever_config_t reciever_config_t;

void *sender(sender_config_t *sender_config);
