#pragma once

#include <stdio.h>
#include <pthread.h>

#define TXLOG_MIN_LSN   1

struct txlm_config_t
{
    char filename[16];
    size_t latest_lsn;  // 生成済み最大LSN
    pthread_mutex_t mutex_lsn;
    unsigned int log_size; // ヘッダと実データの合計
};
typedef struct txlm_config_t txlm_config_t;

struct txlog_t
{
    unsigned int lsn;
    short client_id;
    double append_time; // txlm_append_log呼び出し時間
    double write_time; // 各サーバのlog書き込み時間
    unsigned int log_size; // 実データサイズ（ヘッダを除いた部分）
};
typedef struct txlog_t txlog_t;

txlm_config_t *txlm_init(size_t server_id, unsigned int log_size);
void txlm_deinit(txlm_config_t *txlm_config);
void print_txlog_info(txlog_t *txlog);
size_t txlm_get_current_lsn(txlm_config_t *txlm_config);
void txlm_append_log(txlm_config_t *txlm_config, txlog_t *txlog, unsigned short client_id);
void txlm_read_log(txlm_config_t *txlm_config, txlog_t *txlog, size_t lsn);
void txlm_write_log(txlm_config_t *txlm_config, txlog_t *txlog);
void txlm_get_info_from_header(txlog_t *log, char *header);