#include "txlog_manager.h"
#include "log_manager.h"

#include <pthread.h>

struct info_t
{
    txlm_config_t *config;
};

void *reader(struct info_t *info);

int main() {
    txlog_t w_txlog[100];

    txlm_config_t *config = txlm_init(1, 1024);
    lm_init(1);

    pthread_t worker;
    struct info_t info;
    info.config = config;
    pthread_create(&worker, NULL, (void *)reader, (void *)&info);

    for (int i = 1; i <= 30; i++)
    {
        txlm_append_log(config, &w_txlog[i], i+100);
    }

    pthread_join(worker, NULL);

    for (int i = 1; i <= 30; i++)
    {
        printf("w_txlog: ");
        print_txlog_info(&w_txlog[i]);
    }
}

void *reader(struct info_t *info)
{
    txlm_config_t *config = info->config;
    txlog_t r_txlog[100];

    for (int i = 1; i <= 30; i++) {
        while (1){
            if (txlm_get_current_lsn(config) >= i) {
                txlm_read_log(config, &r_txlog[i], i);
                printf("r_txlog: ");
                print_txlog_info(&r_txlog[i]);
            }
        }
    }
}