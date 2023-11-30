#include "txlog_manager.h"
#include "log_manager.h"

int main() {
    txlog_t w_txlog;
    txlog_t r_txlog;

    txlm_config_t *config = txlm_init(1, 1024);
    lm_init(1);

    for (int i = 1; i <= 99; i++)
    {
        w_txlog.lsn = i;
        w_txlog.client_id = 100+i;
        w_txlog.append_time = 1000+i;
        w_txlog.write_time = 10000+i;
        w_txlog.log_size = 100000+i;
        txlm_write_log(config, &w_txlog);
        txlm_read_log(config, &r_txlog, i);

        printf("w_txlog: ");
        print_txlog_info(&w_txlog);
        printf("r_txlog: ");
        print_txlog_info(&r_txlog);
        printf("\n");
    }
}