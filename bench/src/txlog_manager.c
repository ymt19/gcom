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
    fprintf(stdout, "[txlog] lsn:%d, clientid:%d, appendtime:%lf, writetime:%lf, logsize:%d\n", txlog->lsn, txlog->client_id, txlog->append_time, txlog->write_time, txlog->log_size);
}

size_t txlm_get_current_lsn(txlm_config_t *txlm_config)
{
    size_t current_lsn;

    current_lsn = txlm_config->latest_lsn;

    return current_lsn;
}

/**
 * Primaryサーバのクライアントインタフェースが呼び出す
*/
void txlm_append_log(txlm_config_t *txlm_config, txlog_t *txlog, unsigned short client_id)
{
    pthread_mutex_lock(&txlm_config->mutex_lsn);        // lock
    txlog->lsn = txlm_config->latest_lsn+1;
    txlog->client_id = client_id;
    txlog->append_time = get_time();
    txlog->log_size = txlm_config->log_size;

    txlm_wirte_log(txlm_config, txlog);
    txlm_config->latest_lsn++;
    pthread_mutex_unlock(&txlm_config->mutex_lsn);      // unlock
}

/**
 * ヘッダと実データ（0埋め）のバイナリ列を生成
*/
void txlm_read_log(txlm_config_t *txlm_config, txlog_t *txlog, size_t lsn)
{
    FILE *fp;
    int offset = get_log_offset(lsn);

    /* バイナリファイル読み込み */
    fp = fopen(txlm_config->filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "fopen():%s\n", strerror(errno));
    }
    fseek(fp, offset, SEEK_SET);
    fread(txlog, sizeof(txlog_t), 1, fp);
    fclose(fp);

    lm_append_read_txlog_log(offset, txlog);
}

/**
 * 各txlogのヘッダのみファイル出力
*/
void txlm_wirte_log(txlm_config_t *txlm_config, txlog_t *txlog)
{
    FILE *fp;
    int offset = get_log_offset(txlog->lsn);

    txlog->write_time = get_time();

    /* バイナリファイル書き込み */
    fp = fopen(txlm_config->filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "fopen():%s\n", strerror(errno));
    }
    fseek(fp, offset, SEEK_SET);
    fwrite(txlog, sizeof(txlog_t), 1, fp);
    fclose(fp);

    /* ログ書き込み */
    lm_append_write_txlog_log(offset, txlog);
}

static int get_log_offset(unsigned int lsn)
{
    return (lsn - 1) * sizeof(txlog_t);
}