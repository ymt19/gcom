#include <stdlib.h>

#include "buffer.h"

buffer_t *buffer_init(int size, int block_size)
{
    buffer_t *buffer = malloc(sizeof(buffer_t));

    buffer->data = malloc(sizeof(char) * size);
    buffer->size = size;
    buffer->block_size = bulock_size;

    return buffer;
}

void buffer_deinit()
{
    free(buffer->data);
    free(buffer);
}

void buffer_set()
{

}

void buffer_get()
{
    
}