#pragma once

#include <stdint.h>

/**
 * @brief ring buffer
 */
struct buffer
{
    uint8_t *data;
    size_t size;
    uint64_t write_idx;
    uint64_t read_idx;
};

extern void
buffer_init(struct buffer *buf, size_t size);
extern void
buffer_free(struct buffer *buf);
extern int
buffer_push(struct buffer *buf, uint8_t *push, size_t len);
extern int
buffer_push_empty(struct buffer *buf, size_t len);
extern int
buffer_pop(struct buffer *buf, size_t len);
extern int
buffer_set(struct buffer *buf, uint64_t idx, uint8_t *set, size_t len);
extern int
buffer_get(struct buffer *buf, uint64_t idx, uint8_t *get, size_t len);