#pragma once

enum message_enum
{
    TXLOG_MESSAGE,
    TXLOGACK_MESSAGE,
    FIN_MESSAGE,
    ERROR,
};
typedef enum message_enum msg_e;

struct message_info_t
{
    msg_e type;
    short source_id;
    short destination_id;
    unsigned int lsn_ack;
};
typedef struct message_info_t msg_info_t;

/* ヘッダオフセット */
#define MESSAGE_HEADER_OFFSET_TYPE              0
#define MESSAGE_HEADER_OFFSET_SOURCEID          MESSAGE_HEADER_OFFSET_TYPE + sizeof(short)
#define MESSAGE_HEADER_OFFSET_DESTINATIONID     MESSAGE_HEADER_OFFSET_SOURCEID + sizeof(short)
#define MESSAGE_HEADER_OFFSET_LSNACK            MESSAGE_HEADER_OFFSET_DESTINATIONID + sizeof(short)
#define MESSAGE_HEADER_SIZE                     MESSAGE_HEADER_OFFSET_LSNACK + sizeof(unsigned int)


void get_info_from_message_header(char *msg, msg_info_t *msg_info);
unsigned int create_txlog_message_header(char *msg, short source_id, short destination_id);
unsigned int create_txlogack_message_header(char *msg, short source_id, short destination_id, unsigned int lsn_ack);
unsigned int create_fin_message_header(char *msg, short source_id, short destination_id);