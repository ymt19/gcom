#pragma once

#include <stdio.h>
#include <pthread.h>

struct txlm_config_t
{
    char filename[16];
    size_t latest_lsn;
    pthread_mutex_t mutex_lsn;
};
typedef struct txlm_config_t txlm_config_t;

struct txlog_t
{
    /* txlog header */
    unsigned int lsn;
    short client_id;
    double append_time; // log追加時間
    double write_time; // 各サーバのlog書き込み時間（masterの場合append_timeと同じ）
};
typedef struct txlog_t txlog_t;

#define TXLOG_HEADER_OFFSET_LSN         0
#define TXLOG_HEADER_OFFSET_CLIENTID    TXLOG_HEADER_OFFSET_LSN+sizeof(unsigned int)
#define TXLOG_HEADER_OFFSET_APPENDTIME  TXLOG_HEADER_OFFSET_CLIENTID+sizeof(short)
#define TXLOG_HEADER_OFFSET_WRITETIME   TXLOG_HEADER_OFFSET_APPENDTIME+sizeof(double)
#define TXLOG_HEADER_SIZE               TXLOG_HEADER_OFFSET_WRITETIME+sizeof(double)

txlm_config_t *txlm_init(size_t server_id);
void txlm_deinit(txlm_config_t *txlm_config);
size_t txlm_get_current_lsn(txlm_config_t *txlm_config);
void txlm_append_log(txlm_config_t *txlm_config, txlog_t *txlog, unsigned short client_id);
size_t txlm_read_header(txlm_config_t *txlm_config, unsigned char *header, size_t lsn);
void txlm_wirte_log(txlm_config_t *txlm_config, txlog_t *txlog, int set_append_time_flag);
void txlm_get_info_from_header(txlog_t *log, unsigned char *header);