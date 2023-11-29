#pragma once

#include <stdio.h>

#include "message.h"
#include "txlog_manager.h"

void lm_init(short server_id);
void lm_deinit();
void lm_append_send_message_log(char *msg);
void lm_append_receive_message_log(char *msg);
void lm_append_commit_tx_log(short client_id, unsigned int lsn);
void lm_append_response_tx_log(short client_id, unsigned int lsn);
void lm_append_write_txlog_log(txlog_t *txlog);