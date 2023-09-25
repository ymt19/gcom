#include <errno.h>
#include <stdlib.h>

#include "txlog_manager.h"
#include "server.h"

txlm_config_t *txlm_init(size_t server_id)
{
    txlm_config_t *txlm_config = malloc(sizeof(txlm_config_t));

    sprintf(txlm_config->filename, "txlog_srv%d.bin", server_id);
    remove(txlm_config->filename);

    pthread_mutex_init(&txlm_config->mutex, NULL);

    txlm_config->latest_lsn = 0;

    return txlm_config;
}

void txlm_deinit(txlm_config_t *txlm_config)
{
    pthread_mutex_destroy(&txlm_config->mutex);
    free(txlm_config);
}

size_t txlm_append_log(txlm_config_t *txlm_config, unsigned short client_id, double add_time, unsigned int log_size)
{
    FILE *fp;
    unsigned char bytes[log_size+1];
    unsigned int lsn;
    double add_time;
    unsigned int data_size;
    size_t offset;

    memcpy(bytes + TXLOG_HEADER_OFFSET_LSN, &lsn, sizeof(unsigned int));
    memcpy(bytes + TXLOG_HEADER_OFFSET_CLIENTID, &client_id, sizeof(unsigned int));
    memcpy(bytes + TXLOG_HEADER_OFFSET_ADDTIME, &add_time, sizeof(double));
    memcpy(bytes + TXLOG_HEADER_OFFSET_DATASIZE, &data_size);


    pthread_mutex_lock(&txlm_config->mutex);

    fp = fopen(txlm_config->filename, "wb");
    fseek(fp, offset, SEEK_SET);
    fwrite(bytes, sizeof(unsigned char), log_size, fp);

    pthread_mutex_unlock(&txlm_config->mutex);
}

void txlm_read_log(unsigned char *bytes, txlm_config_t *txlm_config, size_t lsn)
{
    FILE *fp;
    size_t offset;
    size_t log_size;
    
    pthread_mutex_lock(&txlm_config->mutex);

    fp = fopen(txlm_config->filename, "rb");
    fseek(fp, offset, SEEK_SET);
    fread(bytes, sizeof(unsigned char), log_size, fp);

    pthread_mutex_unlock(&txlm_config->mutex);
}

void txlm_wirte_log(txlm_config_t *txlm_config, size_t lsn, unsigned char *bytes, size_t log_size)
{
    FILE *fp;
    size_t offset;
    
    pthread_mutex_lock(&txlm_config->mutex);

    fp = fopen(txlm_config->filename, "wb");
    fseek(fp, offset, SEEK_SET);
    fwrite(bytes, sizeof(unsigned char), log_size, fp);

    pthread_mutex_unlock(&txlm_config->mutex);
}

void txlm_get_info_from_header(txlog_t *log, char *bytes, size_t len)
{

}