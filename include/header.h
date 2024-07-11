#pragma once

#include <stdint.h>

struct header {
    // rootipaddr
    // previpaddr
    uint32_t seq;
    uint32_t first;
    uint32_t last;
    uint8_t flag;
    uint16_t size;
};

#define DATAGRAM_SIZE_MAX   1472
#define DATA_SIZE_MAX       DATAGRAM_SIZE_MAX - sizeof(struct header)

#define FLAG_NAK            0x01
#define FLAG_ACK            0x02
#define FLAG_MTC    // multicastデータグラム
#define FLAG_RLY    // 転送するデータグラム