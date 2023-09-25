#include <stdlib.h>

#include "txlog_manager.h"

lm_config_t *lm_init(size_t server_id)
{
    lm_config_t *lm_config = malloc(sizeof(lm_config_t));
    sprintf(lm_config->filename, "txlog_srv%d", server_id);
    lm_config->latest_lsn = 0;

    lm_config->fp = fopen(lm_config->filename, "w+");
    if (lm_config->fp == NULL) {
        fprintf(stderr, "log_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }

    return lm_config;
}

void lm_deinit(lm_config_t *lm_config)
{
    fclose(lm_config->fp);
    free(lm_config);
}

size_t lm_append_log(lm_config_t *lm_config, size_t client_id, double add_time, size_t send_log_size)
{

}

void lm_read_log(lm_config_t *lm_config, size_t lsn, log_t *log)
{

}

void lm_wirte_log(lm_config_t *lm_config, log_t *log)
{

}

void lm_bytes_to_log(log_t *log, char *bytes, size_t len)
{

}