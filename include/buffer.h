#pragma once

struct buffer_t
{
    char *data;
    int size;
    int block_size;
    int next_block_num;
};
typedef buffer_t buff_t;