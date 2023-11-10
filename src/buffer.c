#include <stdlib.h>

#include "buffer.h"

buffer_t *alloc_buffer(int size)
{
    buffer_t *buffer = malloc(sizeof(buffer_t));

    buffer->data = malloc(sizeof(char) * size);
    buffer->size = size;
    buffer->block_size = bulock_size;

    return buffer;
}

void dealloc_buffer()
{
    free(buffer->data);
    free(buffer);
}

void set_buffer()
{

}

void get_buffer()
{
    
}