#include "../include/background.h"

void *client(client_thread_info_t *info)
{
    size_t client_id = info->client_id;
    server_config_t *srv_config = info->srv_config;
    double bench_finish_time = srv_config->system_start_time + srv_config->duration;

    int cnt = 1;
    while (bench_finish_time > get_time())
    {
        fprintf(stdout, "clinent id: %zu, count: %d\n", client_id, cnt++);
        sleep(1);
    }
}