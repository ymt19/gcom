#include <string.h>
#include <stdio.h>

#include "message.h"

unsigned int create_txlog_message(char *msg, short source_id, short destination_id, txlog_t *txlog)
{
    message_header *hdr = (message_header *)msg;
    hdr->size = sizeof(message_header) + txlog->log_size;
    hdr->type = (short)TXLOG_MESSAGE;
    hdr->source_id = source_id;
    hdr->destination_id = destination_id;
    hdr->lsn = txlog->lsn;
    memcpy(msg + sizeof(message_header), txlog, sizeof(txlog_t));
    return hdr->size;
}

unsigned int create_ack_message(char *msg, short source_id, short destination_id, unsigned int ack)
{
    message_header *hdr = (message_header *)msg;
    hdr->size = sizeof(message_header);
    hdr->type = (short)ACK_MESSAGE;
    hdr->source_id = source_id;
    hdr->destination_id = destination_id;
    hdr->lsn = ack;
    return hdr->size;
}

unsigned int create_fin_message(char *msg, short source_id, short destination_id)
{
    message_header *hdr = (message_header *)msg;
    hdr->size = sizeof(message_header);
    hdr->type = (short)FIN_MESSAGE;
    hdr->source_id = source_id;
    hdr->destination_id = destination_id;
    hdr->lsn = -1;
    return hdr->size;
}