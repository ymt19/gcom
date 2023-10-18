#include "txlog_manager.h"

int main() {
    /**** tx log mamanger起動 ****/
    txlm_config_t *txlm_config = txlm_init(1);
    /****************************/

    txlog_t txlog;
    char header[TXLOG_HEADER_SIZE];

    txlm_append_log(txlm_config, &txlog, 1);
    print_txlog_info(&txlog);
    txlm_read_header(txlm_config, header, txlog.lsn);
    txlm_get_info_from_header(&txlog, header);
    print_txlog_info(&txlog);
    
    txlm_append_log(txlm_config, &txlog, 3);
    print_txlog_info(&txlog);
    txlm_read_header(txlm_config, header, txlog.lsn);
    txlm_get_info_from_header(&txlog, header);
    print_txlog_info(&txlog);

    txlm_deinit(txlm_config);
}