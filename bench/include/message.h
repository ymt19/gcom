#pragma once

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

void create_txlog_message_header(char *msg, unsigned int msgsize, short source_id, short destination_id, unsigned int lsn);
void create_ack_message_header(char *msg, short source_id, short destination_id, unsigned int ack);
void create_fin_message_header(char *msg, short source_id, short destination_id);