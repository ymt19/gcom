#include <unistd.h>

#include "background.h"
#include "time_manager.h"
#include "txlog_manager.h"

struct client_thread_info_t
{
    size_t client_id;
    server_config_t *srv_config;
    txlm_config_t *txlm_config;
};
typedef struct client_thread_info_t client_thread_info_t;

static void client_worker(client_thread_info_t *info);
static void commit(txlm_config_t *txlm_config, size_t client_id);


static void client_worker(client_thread_info_t *info)
{
    size_t client_id = info->client_id;
    server_config_t *srv_config = info->srv_config;
    txlm_config_t *txlm_config = info->txlm_config;
    double bench_finish_time = srv_config->system_start_time + srv_config->duration;

    while (bench_finish_time > get_time())
    {
        commit(txlm_config, client_id);
        sleep(1);
    }
}

/**
 * log (commit)
*/
static void commit(txlm_config_t *txlm_config, size_t client_id)
{
    txlog_t txlog;
    txlm_append_log(txlm_config, &txlog, client_id);
    print_txlog_info(&txlog);
}

void background(server_config_t *srv_config, txlm_config_t *txlm_config)
{
    pthread_t workers[MAX_NUM_CLIENT_THREADS];
    client_thread_info_t *worker_info_set;

    worker_info_set = malloc(sizeof(client_thread_info_t) * (srv_config->num_threads+1));

    // 起動時間の設定
    srv_config->system_start_time = get_time();

    // client thread起動
    for (int client_id = 0; client_id < srv_config->num_threads; ++client_id)
    {
        worker_info_set[client_id].client_id = client_id+1;
        worker_info_set[client_id].srv_config = srv_config;
        worker_info_set[client_id].txlm_config = txlm_config;

        pthread_create(&workers[client_id], NULL, (void *)client_worker, (void *)&worker_info_set[client_id]);
    }

    for (int client_id = 0; client_id < srv_config->num_threads; ++client_id)
    {
        pthread_join(workers[client_id], NULL);
    }
    free(worker_info_set);
}