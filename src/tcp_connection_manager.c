#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "tcp_connection_manager.h"
#include "server.h"

struct sender_worker_thread_info_t
{
    sender_config_t *sender_config;
    size_t target_id;
    char target_ipaddr[32];
    unsigned short target_port;
};
typedef struct sender_worker_thread_info_t sender_worker_thread_info_t;

void *sender_worker(sender_worker_thread_info_t *thread_info)
{
    sender_config_t *sender_config = thread_info->sender_config;
    server_config_t *srv_config = sender_config->srv_config;
    fprintf(stdout, "sender thread id:%zu, ipaddr:%s, port:%d\n", thread_info->target_id, thread_info->target_ipaddr, thread_info->target_port);
    
    while (!(srv_config->finish_flag))
    {
        /* code */
    }
    
}

void *sender(sender_config_t *sender_config)
{
    server_config_t *srv_config = sender_config->srv_config;
    pthread_t worker_thread[MAX_NUM_SECONDARY_SERVERS+1];
    sender_worker_thread_info_t *worker_thread_info_set;

    worker_thread_info_set = malloc(sizeof(sender_worker_thread_info_t) * srv_config->num_servers);

    // 各standbyサーバにsender_workerを生成
    for (size_t target_id = 1; target_id <= srv_config->num_servers; ++target_id)
    {
        if (srv_config->srv_id == target_id) continue;

        worker_thread_info_set[target_id].target_id = target_id;
        strncpy(worker_thread_info_set[target_id].target_ipaddr, 
                srv_config->srvs_ipaddr[target_id],
                strlen(srv_config->srvs_ipaddr[target_id]));
        worker_thread_info_set[target_id].target_ipaddr[strlen(srv_config->srvs_ipaddr[target_id])] = '\0';
        worker_thread_info_set[target_id].target_port = srv_config->srvs_port[target_id];

        pthread_create(&worker_thread[target_id], NULL, (void *)sender_worker, (void *)&worker_thread_info_set[target_id]);
    }
    
    for (int target_id = 1; target_id <= srv_config->num_servers; ++target_id)
    {
        pthread_join(worker_thread[target_id], NULL);
    }
    free(worker_thread_info_set);
}

void reciever(server_config_t *srv_config)
{
    // secondary
}