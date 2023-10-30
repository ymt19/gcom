#pragma once

struct buffer_t
{
    char *buffer;
    int size;
    int block_size;
};
typedef buffer_t buff_t;