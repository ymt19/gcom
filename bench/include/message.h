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
    unsigned int ack;   // 受信したlsnを格納
};
typedef struct message_header message_header;

void create_txlog_message_header(char *msg, unsigned int msgsize, short source_id, short destination_id);
void create_ack_message_header(char *msg, short source_id, short destination_id, unsigned int ack);
void create_fin_message_header(char *msg, short source_id, short destination_id);