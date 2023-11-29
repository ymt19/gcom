#include <string.h>
#include <stdio.h>

#include "message.h"

void create_txlog_message_header(char *msg, unsigned int msgsize, short source_id, short destination_id)
{
    message_header hdr;
    hdr.size = msgsize;
    hdr.type = (short)TXLOG_MESSAGE;
    hdr.source_id = source_id;
    hdr.destination_id = destination_id;
    hdr.ack = -1;
    memcpy(msg, &hdr, sizeof(message_header));
}

void create_ack_message_header(char *msg, short source_id, short destination_id, unsigned int ack)
{
    message_header hdr;
    hdr.size = sizeof(message_header);
    hdr.type = (short)ACK_MESSAGE;
    hdr.source_id = source_id;
    hdr.destination_id = destination_id;
    hdr.ack = ack;
    memcpy(msg, &hdr, sizeof(message_header));
}

void create_fin_message_header(char *msg, short source_id, short destination_id)
{
    message_header hdr;
    hdr.size = sizeof(message_header);
    hdr.type = (short)FIN_MESSAGE;
    hdr.source_id = source_id;
    hdr.destination_id = destination_id;
    hdr.ack = -1;
    memcpy(msg, &hdr, sizeof(message_header));
}