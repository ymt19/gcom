#pragma once

#include "server.h"
#include "txlog_manager.h"

void sender_main(server_config_t *srv_config, txlm_config_t *txlm_config);
void reciever_main(server_config_t *srv_config, txlm_config_t *txlm_config);