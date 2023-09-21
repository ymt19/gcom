#pragma once

#include <stddef.h>

struct log_manager_info_t
{
    char log_filename[16];
    size_t latest_lsn;
};
typedef struct log_manager_info_t log_manager_info_t;

struct log_t
{
    size_t lsn;
    size_t client_id;
    double add_time;
    double write_time;
};
typedef struct log_t log_t;

size_t lm_append_log(log_manager_info_t *lm_info, size_t client_id, double add_time, size_t send_log_size);
void lm_read_log(log_manager_info_t *lm_info, size_t lsn, log_t *log);
void lm_wirte_log(log_manager_info_t *lm_info, log_t *log);

int lm_log_to_bytes(char *bytes, log_t *log);
void lm_bytes_to_log(log_t *log, char *bytes, size_t len);