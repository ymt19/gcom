#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "tcp_connection_manager.h"
#include "server.h"

struct sender_worker_thread_info_t
{
    server_config_t *srv_config;
    size_t target_id;
};
typedef struct sender_worker_thread_info_t sender_worker_thread_info_t;

static void sender_worker(sender_worker_thread_info_t *worker_info)
{
    server_config_t *srv_config = worker_info->srv_config;
    size_t target_id = worker_info->target_id;

    fprintf(stdout, "sender thread id:%zu, ipaddr:%s, port:%d\n",
            target_id,
            srv_config->srvs_ipaddr[target_id],
            srv_config->srvs_port[target_id]);
    
    while (1)
    {
        fprintf(stdout, "[connect] sender thread id:%zu, ipaddr:%s, port:%d\n",
                target_id,
                srv_config->srvs_ipaddr[target_id],
                srv_config->srvs_port[target_id]);
        sleep(1);
    }
    
}

void sender_main(server_config_t *srv_config)
{
    // 各secondaryサーバごとにsender_workerを生成
    for (size_t target_id = 1; target_id <= srv_config->num_servers; ++target_id)
    {
        if (srv_config->srv_id == target_id) continue;

        pthread_t worker;
        sender_worker_thread_info_t *worker_info;

        worker_info = malloc(sizeof(sender_worker_thread_info_t));
        worker_info->srv_config = srv_config;
        worker_info->target_id = target_id;

        pthread_create(&worker,
                        NULL,
                        (void *)sender_worker,
                        (void *)worker_info);
        pthread_detach(worker);
    }
}

void reciever(server_config_t *srv_config)
{
    // secondary
}