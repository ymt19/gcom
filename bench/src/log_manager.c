#include <stdio.h>

#include "log_manager.h"
#include "time_manager.h"

FILE *logfile_fp = NULL;

void lm_init(short server_id)
{
    char filename[16];
    sprintf(filename, "srv%d.csv", server_id);
    remove(filename);

    logfile_fp = fopen(filename, "w");
    // logfile_fp = stderr;
}

void lm_deinit()
{
    fclose(logfile_fp);
}

void lm_append_send_message_log(char *msg, int size)
{
    message_header *hdr = (message_header *)msg;
    fprintf(logfile_fp, "SEND_MESSAGE,%lf,%d,%d,%d,%d,%d\n", get_time(), size, hdr->type, hdr->source_id, hdr->destination_id, hdr->lsn);
}

void lm_append_receive_message_log(char *msg, int size)
{
    message_header *hdr = (message_header *)msg;
    fprintf(logfile_fp, "RECEIVE_MESSAGE,%lf,%d,%d,%d,%d,%d\n", get_time(), size, hdr->type, hdr->source_id, hdr->destination_id, hdr->lsn);
}

void lm_append_commit_tx_log(short client_id, unsigned int lsn)
{
    fprintf(logfile_fp, "COMMIT_TX,%lf,%d,%d\n", get_time(), client_id, lsn);
}

void lm_append_response_tx_log(short client_id, unsigned int lsn)
{
    fprintf(logfile_fp, "RESPONSE_TX,%lf,%d,%d\n", get_time(), client_id, lsn);
}

void lm_append_write_txlog_log(txlog_t *txlog)
{
    fprintf(logfile_fp, "WRITE_TXLOG,%lf,%d,%d,%lf,%lf,%d\n", get_time(), txlog->lsn, txlog->client_id, txlog->append_time, txlog->write_time, txlog->log_size);
}