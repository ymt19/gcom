#pragma once

#include "txlog_manager.h"

enum message_enum
{
    TXLOG_MESSAGE,
    ACK_MESSAGE,
    FIN_MESSAGE,
};

struct message_header
{
    unsigned int size;
    short type;
    short source_id;
    short destination_id;
    unsigned int lsn;   // TXLOG_MESSAGE/ACK_MESSAGE
};
typedef struct message_header message_header;

unsigned int create_txlog_message(char *msg, short source_id, short destination_id, txlog_t *txlog);
unsigned int create_ack_message(char *msg, short source_id, short destination_id, unsigned int ack);
unsigned int create_fin_message(char *msg, short source_id, short destination_id);