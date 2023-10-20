#include <stdio.h>

#include "log_manager.h"
#include "time_manager.h"

FILE *logfile_fp = NULL;

void lm_init(short server_id)
{
    char filename[16];
    sprintf(filename, "srv%d.log", server_id);
    remove(filename);

    logfile_fp = fopen(filename, "w");
    // logfile_fp = stderr;
}

void lm_deinit()
{
    fclose(logfile_fp);
}

void lm_append_send_message_log(char *msg, int msg_len)
{
    msg_info_t msg_info;
    get_info_from_message_header(msg, &msg_info);
    fprintf(logfile_fp, "SEND_MESSAGE,%lf,%d,%d,%d,%d,%d\n", get_time(), msg_len, msg_info.type, msg_info.source_id, msg_info.destination_id, msg_info.lsn_ack);
}

void lm_append_receive_message_log(char *msg, int msg_len)
{
    msg_info_t msg_info;
    get_info_from_message_header(msg, &msg_info);
    fprintf(logfile_fp, "RECEIVE_MESSAGE,%lf,%d,%d,%d,%d,%d\n", get_time(), msg_len, msg_info.type, msg_info.source_id, msg_info.destination_id, msg_info.lsn_ack);
}

void lm_append_commit_tx_log(short client_id, short lsn)
{
    fprintf(logfile_fp, "COMMIT_TX,%lf,%d,%d\n", get_time(), client_id, lsn);
}

void lm_append_response_tx_log(short client_id, short lsn)
{
    fprintf(logfile_fp, "RESPONSE_TX,%lf,%d,%d\n", get_time(), client_id, lsn);
}

void lm_append_write_txlog_log(txlog_t *txlog)
{
    fprintf(logfile_fp, "WRITE_TXLOG,%lf,%d,%d,%lf,%lf,%d\n", get_time(), txlog->lsn, txlog->client_id, txlog->append_time, txlog->write_time, txlog->data_size);
}