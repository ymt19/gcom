#include <errno.h>
#include <stdlib.h>

#include "txlog_manager.h"
#include "time_manager.h"

txlm_config_t *txlm_init(size_t server_id)
{
    txlm_config_t *txlm_config = malloc(sizeof(txlm_config_t));

    sprintf(txlm_config->filename, "txlog_srv%d.bin", server_id);
    remove(txlm_config->filename);

    txlm_config->latest_lsn = 0;
    pthread_mutex_init(&txlm_config->mutex_lsn, NULL);

    return txlm_config;
}

void txlm_deinit(txlm_config_t *txlm_config)
{
    pthread_mutex_destroy(&txlm_config->mutex_lsn);
    free(txlm_config);
}

size_t txlm_get_current_lsn(txlm_config_t *txlm_config)
{
    size_t current_lsn;

    current_lsn = txlm_config->latest_lsn;

    return current_lsn;
}

void txlm_append_log(txlm_config_t *txlm_config, txlog_t *txlog, unsigned short client_id)
{
    unsigned int lsn;

    pthread_mutex_lock(&txlm_config->mutex_lsn);        // lock
    lsn = ++txlm_config->latest_lsn;
    pthread_mutex_unlock(&txlm_config->mutex_lsn);      // unlock

    txlog->lsn = lsn;
    txlog->client_id = client_id;
    txlog->append_time = -1;
    txlog->write_time = -1;

    txlm_wirte_log(txlm_config, txlog, 1);
}

size_t txlm_read_header(txlm_config_t *txlm_config, unsigned char *header, size_t lsn)
{
    FILE *fp;
    size_t offset = lsn * TXLOG_HEADER_SIZE;

    fp = fopen(txlm_config->filename, "rb");
    fseek(fp, offset, SEEK_SET);
    fread(header, sizeof(unsigned char), TXLOG_HEADER_SIZE, fp);
    fclose(fp);

    return TXLOG_HEADER_SIZE;
}

void txlm_wirte_log(txlm_config_t *txlm_config, txlog_t *txlog, int set_append_time_flag)
{
    FILE *fp;
    unsigned char header[TXLOG_HEADER_SIZE];
    size_t offset = txlog->lsn * TXLOG_HEADER_SIZE;

    txlog->write_time = get_time();
    if (set_append_time_flag) {
        txlog->append_time = txlog->write_time;
    }

    memcpy(header + TXLOG_HEADER_OFFSET_LSN, &txlog->lsn, sizeof(unsigned int));
    memcpy(header + TXLOG_HEADER_OFFSET_CLIENTID, &txlog->client_id, sizeof(unsigned int));
    memcpy(header + TXLOG_HEADER_OFFSET_APPENDTIME, &txlog->append_time, sizeof(double));
    memcpy(header + TXLOG_HEADER_OFFSET_WRITETIME, &txlog->write_time, sizeof(double));

    fp = fopen(txlm_config->filename, "wb");
    fseek(fp, offset, SEEK_SET);
    fwrite(header, sizeof(unsigned char), TXLOG_HEADER_SIZE, fp);
    fclose(fp);
}

void txlm_get_info_from_header(txlog_t *log, unsigned char *header)
{
    memcpy(&log->lsn, header + TXLOG_HEADER_OFFSET_LSN, sizeof(unsigned int));
    memcpy(&log->client_id, header + TXLOG_HEADER_OFFSET_CLIENTID, sizeof(short));
    memcpy(&log->append_time, header + TXLOG_HEADER_OFFSET_APPENDTIME, sizeof(double));
    memcpy(&log->write_time, header + TXLOG_HEADER_OFFSET_WRITETIME, sizeof(double));
}