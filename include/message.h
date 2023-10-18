#pragma once

enum message_enum
{
    TXLOGDATA_MESSAGE,
    ACK_MESSAGE,
    ERROR,
};
typedef enum message_enum message_e;

#define MESSAGE_HEADER_OFFSET_SIZE      0
#define MESSAGE_HEADER_OFFSET_TYPE      
