#pragma once

#include <stdio.h>

struct txlm_config_t
{
    FILE *fp;
    char filename[16];
    size_t latest_lsn;
};
typedef struct txlm_config_t txlm_config_t;

struct txlog_t
{
    size_t lsn;
    size_t client_id;
    double add_time;
    double write_time;
};
typedef struct txlog_t txlog_t;

txlm_config_t *txlm_init(size_t server_id);
void txlm_deinit(txlm_config_t *txlm_config);
size_t txlm_append_log(txlm_config_t *txlm_config, size_t client_id, double add_time, size_t send_log_size);
void txlm_read_log(txlm_config_t *txlm_config, size_t lsn, txlog_t *log);
void txlm_wirte_log(txlm_config_t *txlm_config, txlog_t *log);


int lm_log_to_bytes(char *bytes, txlog_t *log);
void txlm_bytes_to_log(txlog_t *log, char *bytes, size_t len);