#pragma once

#include <stdio.h>
#include <pthread.h>

struct txlm_config_t
{
    char filename[16];
    pthread_mutex_t mutex;
    size_t latest_lsn;
};
typedef struct txlm_config_t txlm_config_t;

struct txlog_t
{
    /* txlog header */
    unsigned int lsn;
    short client_id;
    double add_time;
    unsigned int data_size;
};
typedef struct txlog_t txlog_t;

#define TXLOG_HEADER_OFFSET_LSN         0
#define TXLOG_HEADER_OFFSET_CLIENTID    TXLOG_HEADER_OFFSET_LSN+sizeof(unsigned int)
#define TXLOG_HEADER_OFFSET_ADDTIME     TXLOG_HEADER_OFFSET_LSN+sizeof(short)
#define TXLOG_HEADER_OFFSET_DATASIZE    TXLOG_HEADER_OFFSET_ADDTIME+sizeof(double)
#define TXLOG_HEADER_SIZE               TXLOG_HEADER_OFFSET_DATASIZE+sizeof(unsigned int)

txlm_config_t *txlm_init(size_t server_id);
void txlm_deinit(txlm_config_t *txlm_config);
size_t txlm_append_log(txlm_config_t *txlm_config, unsigned short client_id, double add_time, unsigned int data_size);
void txlm_read_log(unsigned char *bytes, txlm_config_t *txlm_config, size_t lsn);
void txlm_wirte_log(txlm_config_t *txlm_config, size_t lsn, unsigned char *bytes, size_t log_size);
void txlm_get_info_from_header(txlog_t *log, char *bytes, size_t len);