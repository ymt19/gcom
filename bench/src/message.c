#include <string.h>
#include <stdio.h>

#include "message.h"

void get_info_from_message_header(char *msg, msg_info_t *msg_info)
{
    memcpy(&msg_info->type, msg + MESSAGE_HEADER_OFFSET_TYPE, sizeof(short));
    memcpy(&msg_info->source_id, msg + MESSAGE_HEADER_OFFSET_SOURCEID, sizeof(short));
    memcpy(&msg_info->destination_id, msg + MESSAGE_HEADER_OFFSET_DESTINATIONID, sizeof(short));
    memcpy(&msg_info->lsn_ack, msg + MESSAGE_HEADER_OFFSET_LSNACK, sizeof(unsigned int));
}

unsigned int create_txlog_message_header(char *msg, short source_id, short destination_id)
{
    msg_e type = TXLOG_MESSAGE;
    unsigned int lsn_ack = -1;

    memcpy(msg + MESSAGE_HEADER_OFFSET_TYPE, &type, sizeof(short));
    memcpy(msg + MESSAGE_HEADER_OFFSET_SOURCEID, &source_id, sizeof(short));
    memcpy(msg + MESSAGE_HEADER_OFFSET_DESTINATIONID, &destination_id, sizeof(short));
    memcpy(msg + MESSAGE_HEADER_OFFSET_LSNACK, &lsn_ack, sizeof(unsigned int));
    return MESSAGE_HEADER_SIZE;
}

unsigned int create_txlogack_message_header(char *msg, short source_id, short destination_id, unsigned int lsn_ack)
{
    msg_e type = TXLOGACK_MESSAGE;

    memcpy(msg + MESSAGE_HEADER_OFFSET_TYPE, &type, sizeof(short));
    memcpy(msg + MESSAGE_HEADER_OFFSET_SOURCEID, &source_id, sizeof(short));
    memcpy(msg + MESSAGE_HEADER_OFFSET_DESTINATIONID, &destination_id, sizeof(short));
    memcpy(msg + MESSAGE_HEADER_OFFSET_LSNACK, &lsn_ack, sizeof(unsigned int));
    return MESSAGE_HEADER_SIZE;
}

unsigned int create_fin_message_header(char *msg, short source_id, short destination_id)
{
    msg_e type = FIN_MESSAGE;
    unsigned int lsn_ack = -1;

    memcpy(msg + MESSAGE_HEADER_OFFSET_TYPE, &type, sizeof(short));
    memcpy(msg + MESSAGE_HEADER_OFFSET_SOURCEID, &source_id, sizeof(short));
    memcpy(msg + MESSAGE_HEADER_OFFSET_DESTINATIONID, &destination_id, sizeof(short));
    memcpy(msg + MESSAGE_HEADER_OFFSET_LSNACK, &lsn_ack, sizeof(unsigned int));
    return MESSAGE_HEADER_SIZE;
}