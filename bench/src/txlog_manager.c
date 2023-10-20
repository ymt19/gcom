#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "txlog_manager.h"
#include "time_manager.h"
#include "log_manager.h"

static int get_log_offset(unsigned int lsn);

txlm_config_t *txlm_init(size_t server_id, unsigned int log_size)
{
    if (log_size < TXLOG_HEADER_SIZE) {
        return NULL;
    }

    txlm_config_t *txlm_config = malloc(sizeof(txlm_config_t));

    sprintf(txlm_config->filename, "txlog_srv%ld.bin", server_id);
    remove(txlm_config->filename);

    txlm_config->latest_lsn = TXLOG_MIN_LSN-1;
    pthread_mutex_init(&txlm_config->mutex_lsn, NULL);

    txlm_config->log_size = log_size;
    printf("logsize:%d\n", log_size);

    return txlm_config;
}

void txlm_deinit(txlm_config_t *txlm_config)
{
    pthread_mutex_destroy(&txlm_config->mutex_lsn);
    free(txlm_config);
}

void print_txlog_info(txlog_t *txlog)
{
    fprintf(stdout, "[txlog] lsn:%d, clientid:%d, appendtime:%lf, writetime:%lf, datasize:%d\n", txlog->lsn, txlog->client_id, txlog->append_time, txlog->write_time, txlog->data_size);
}

size_t txlm_get_current_lsn(txlm_config_t *txlm_config)
{
    size_t current_lsn;

    current_lsn = txlm_config->latest_lsn;

    return current_lsn;
}

void txlm_append_log(txlm_config_t *txlm_config, txlog_t *txlog, unsigned short client_id)
{
    pthread_mutex_lock(&txlm_config->mutex_lsn);        // lock
    txlog->lsn = txlm_config->latest_lsn+1;
    txlog->client_id = client_id;
    txlog->append_time = -1;
    txlog->write_time = -1;
    txlog->data_size = txlm_config->log_size - (TXLOG_HEADER_SIZE);

    txlm_wirte_log(txlm_config, txlog, 1);
    txlm_config->latest_lsn++;
    pthread_mutex_unlock(&txlm_config->mutex_lsn);      // unlock
}

/**
 * ヘッダと実データ（0埋め）のバイナリ列を生成
*/
size_t txlm_read_log(txlm_config_t *txlm_config, char *log_data, size_t lsn)
{
    FILE *fp;
    int offset = get_log_offset(lsn);

    fp = fopen(txlm_config->filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "fopen():%s\n", strerror(errno));
    }
    fseek(fp, offset, SEEK_SET);
    fread(log_data, sizeof(char), TXLOG_HEADER_SIZE, fp);
    fclose(fp);

    return txlm_config->log_size;
}

/**
 * 各txlogのヘッダのみファイル出力
*/
void txlm_wirte_log(txlm_config_t *txlm_config, txlog_t *txlog, int set_append_time_flag)
{
    FILE *fp;
    char header[TXLOG_HEADER_SIZE];
    int offset = get_log_offset(txlog->lsn);

    txlog->write_time = get_time();
    if (set_append_time_flag) {
        txlog->append_time = txlog->write_time;
    }

    memcpy(header + TXLOG_HEADER_OFFSET_LSN, &txlog->lsn, sizeof(unsigned int));
    memcpy(header + TXLOG_HEADER_OFFSET_CLIENTID, &txlog->client_id, sizeof(unsigned int));
    memcpy(header + TXLOG_HEADER_OFFSET_APPENDTIME, &txlog->append_time, sizeof(double));
    memcpy(header + TXLOG_HEADER_OFFSET_WRITETIME, &txlog->write_time, sizeof(double));
    memcpy(header + TXLOG_HEADER_OFFSET_DATASIZE, &txlog->data_size, sizeof(unsigned int));

    fp = fopen(txlm_config->filename, "wb");
    fseek(fp, offset, SEEK_SET);
    fwrite(header, sizeof(char), TXLOG_HEADER_SIZE, fp);
    fclose(fp);

    lm_append_write_txlog_log(txlog);
}

void txlm_get_info_from_header(txlog_t *log, char *header)
{
    memcpy(&log->lsn, header + TXLOG_HEADER_OFFSET_LSN, sizeof(unsigned int));
    memcpy(&log->client_id, header + TXLOG_HEADER_OFFSET_CLIENTID, sizeof(short));
    memcpy(&log->append_time, header + TXLOG_HEADER_OFFSET_APPENDTIME, sizeof(double));
    memcpy(&log->write_time, header + TXLOG_HEADER_OFFSET_WRITETIME, sizeof(double));
    memcpy(&log->data_size, header + TXLOG_HEADER_OFFSET_DATASIZE, sizeof(unsigned int));
}

static int get_log_offset(unsigned int lsn)
{
    return (lsn - 1) * TXLOG_HEADER_SIZE;
}